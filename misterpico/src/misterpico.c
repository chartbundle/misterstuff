/*
 * Copyright (c) 2021 Valentin Milea <valentin.milea@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <pico/i2c_slave.h>
#include <hardware/pwm.h>
#include <hardware/clocks.h>
#include <hardware/adc.h>
#include <hardware/timer.h>
#include <pico/stdlib.h>
#include <lwip/ip.h>
#include <lwip/udp.h>
#include <pico_1wire.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "lwipopts.h"

#define STDOUT 1
#define RLE_ESCAPE 0xd2
#define NUMPWMOUT 4
#define NUMPWMIN 4
#define I2C_DATA_CMD_FIRST_BYTE 0x00000800
#define SERIAL_TIMEOUTUS 100000
#define PWMIN_DIV 64
#define PWMIN_PERIODUS 2389 // 2.5 mS
#define PWMIN_ROTATE 3      // /2^x

#define PORT_CMD 35310
#define PORT_OLED 35311
#define PORT_PWM 35312
#define PORT_ADC 35313
#define PORT_ONEWIRE 35314

extern struct udp_pcb *l_udp_pcb;
// #define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define SHAREDBUFLEN 2048
uint8_t sharedbuf[SHAREDBUFLEN];
#define MAX_OWDEVICES 8
alarm_pool_t *core1alarms;
pico_1wire_t *owctx;
uint64_t ow_devices[MAX_OWDEVICES];
uint8_t ow_devicecount=0;
#define ONEWIRELOOPMS 23


#define BPREP sharedbuf[0]='\0';

#define BPRINTF(args...) snprintf((char *)(sharedbuf+strlen((char *)sharedbuf)),SHAREDBUFLEN-strlen((char *)sharedbuf),args)
#define BFLUSH(x) if(strlen((char *)sharedbuf)>0 ) do_send(x,strlen((char *)sharedbuf))



void do_send(uint16_t port,uint16_t len) {
        struct pbuf *p;
        err_t wr_err;
        p = pbuf_alloc(PBUF_TRANSPORT,len,PBUF_RAM);
        
        memcpy(p->payload,sharedbuf,len);
        wr_err  = udp_sendto(l_udp_pcb,p,IP_ADDR_BROADCAST,port);
        (void) (wr_err);
        // if (wr_err) printf("wr_err: %d\n",wr_err);
        pbuf_free(p);
}

void do_send_str(uint16_t port, char *msg) {
    uint16_t len;
    len=MIN(strlen(msg),SHAREDBUFLEN-1);
    memcpy(sharedbuf,msg,len);
    sharedbuf[len]='\0';
    do_send(port,len);
}

static bool text_debug;
static const uint I2C_SLAVE_ADDRESS = 0x3c;
// static const uint I2C_BAUDRATE = 100000; // 100 kHz

// For this example, we run both the master and slave from the same board.
// You'll need to wire pin GP4 to GP6 (SDA), and pin GP5 to GP7 (SCL).
static const uint I2C_SLAVE_SDA_PIN = PICO_DEFAULT_I2C_SDA_PIN; // 4
static const uint I2C_SLAVE_SCL_PIN = PICO_DEFAULT_I2C_SCL_PIN; // 5
                                                                // static const uint I2C_MASTER_SDA_PIN = 6;
                                                                // static const uint I2C_MASTER_SCL_PIN = 7;

static struct
{
    volatile uint8_t mem[2][2048];
    volatile uint16_t mem_address;
    volatile uint16_t len[2];
    uint8_t command;
    bool skip;
    bool mem_address_written;
    volatile uint8_t bank;
    volatile uint16_t max;
} context;

const uint8_t pwminpins[] = {17, 19, 21, 9};
const uint8_t pwminslice[] = {0, 1, 2, 4};
const uint8_t pwmoutpins[] = {6, 10, 12, 14};
const uint8_t pwmoutslice[] = {3, 5, 6, 7};
const uint8_t pwmoutdivint[] = {255, 255, 255, 255};
const uint8_t pwmoutdivfrac[] = {0, 0, 0, 0};
const uint8_t pwmoutshift[] = {0, 0, 0, 0};

#define GPIOOUT_NUM 11
const uint8_t gpiooutpins[] = {0, 1, 2, 3, 7, 8, 11, 13, 16, 18, 20};
//#define GPIOOUT_NUM 9
//const uint8_t gpiooutpins[] = {2, 3, 7, 8, 11, 13, 16, 18, 20};

#define ADCIN_NUM 5
const uint8_t adcinpins[] = {26, 27, 28, 29, 255};

uint16_t adctot[ADCIN_NUM];
uint8_t adccount;

static struct
{
    bool measuring;
    volatile uint8_t dutycycle[NUMPWMIN];
    uint16_t runtotal[NUMPWMIN];
    uint32_t startus;
    uint32_t maxmax;
    uint32_t maxela;
    uint32_t maxt;
    uint32_t maxus;
} pwmin;

// static struct {
//     volatile uint8_t enable;
//     volatile uint8_t pulls;
//     volatile uint8_t output;
//     volatile bool update;
// } outputs;

static volatile uint8_t read_bank;
static uint8_t scratchpad[2048];
static uint8_t rlepad[2048];
// Our handler is called from the I2C ISR, so it must complete quickly. Blocking calls /
// printing to stdio may interfere with interrupt handling.
static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event)
{
    uint32_t value;
    i2c_hw_t *hw = i2c_get_hw(i2c);

    if (event == I2C_SLAVE_RECEIVE || event == I2C_SLAVE_FINISH)
    {
        while (hw->status & I2C_IC_STATUS_RFNE_BITS)
        {
            context.mem_address_written = true;
            value = hw->data_cmd;
            if (value & I2C_DATA_CMD_FIRST_BYTE && context.mem_address != 0)
            {
                if (!context.skip)
                {
                    if (context.mem_address > context.max)
                        context.max = context.mem_address;
                    context.len[context.bank] = context.mem_address;
                    context.mem_address = 0;
                    context.bank = (context.bank + 1) % 2;
                    context.len[context.bank] = 0;
                };
                context.skip = 0;
            }
            if (context.mem_address == 0 && ((value & 0xff) != 0x40))
                context.skip = 1;
            if (!context.skip)
                context.mem[context.bank][context.mem_address++] = value & 0xff;
        }
    }
    // case I2C_SLAVE_REQUEST: // master is requesting data
    //     // load from memory
    //     i2c_write_byte(i2c, context.mem[context.mem_address]);
    //     context.mem_address++;
    //     break;
    if (event == I2C_SLAVE_FINISH && context.mem_address_written)
    {
        if (!context.skip)
        {
            if (context.mem_address > context.max)
                context.max = context.mem_address;
            context.len[context.bank] = context.mem_address;
            context.mem_address = 0;
            context.bank = (context.bank + 1) % 2;
            context.len[context.bank] = 0;
        };
        context.mem_address_written = false;
        context.skip = 0;
    }
}



static void setup_slave()
{
    gpio_init(I2C_SLAVE_SDA_PIN);
    gpio_set_function(I2C_SLAVE_SDA_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SLAVE_SDA_PIN);

    gpio_init(I2C_SLAVE_SCL_PIN);
    gpio_set_function(I2C_SLAVE_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SLAVE_SCL_PIN);

    i2c_init(i2c0, 1000000);
    // configure I2C0 for slave mode
    i2c_slave_init(i2c0, I2C_SLAVE_ADDRESS, &i2c_slave_handler);
}

static void main_i2c_loop()
{
    size_t length;
    size_t rlelen;
    uint16_t i;
    uint8_t j;
    uint8_t a = 0;
    uint8_t n = 0;
    if (read_bank == context.bank)
        return;
    length = context.len[read_bank];
    memcpy(scratchpad, (void *)context.mem[read_bank], length);
    if (text_debug) {
        BPREP;
        BPRINTF("%d %d %d %d :: ", length, read_bank, context.mem_address, context.max);
    }
    read_bank = context.bank;
    // Simple RLE Coding
    rlelen = 0;
    // skip the command byte
    for (i = 1; i <= length; i++) // Deliberate
    {
        if (i == length || (i != 1 && scratchpad[i] != a) || n == 255 )
        {
            if (n < 3 && a != RLE_ESCAPE)
            {
                for (j = 0; j < n; j++)
                {
                    rlepad[rlelen++] = a;
                }
                n = 0;
            }
            else
            {
                rlepad[rlelen++] = RLE_ESCAPE;
                rlepad[rlelen++] = a;
                rlepad[rlelen++] = n;
                n = 0;
            };
        }
        n++;
        if (i<length) a = scratchpad[i];
    }

    if (text_debug) {
        BPRINTF("%d: %X %X\n", rlelen, rlepad[0], rlepad[1]);
        BFLUSH(PORT_CMD);
    }
    sharedbuf[0]=0x0e;
    sharedbuf[1]=0x41;
    sharedbuf[2] = rlelen & 255;
    sharedbuf[3] = rlelen >> 8;
    memcpy(sharedbuf+4,rlepad, rlelen);
    do_send(PORT_OLED,rlelen+4);
}

void setup_pwm()
{
    uint8_t i;
    // NUMPWMOUT Outputs
    for (i = 0; i < NUMPWMOUT; i++)
    {

        pwm_set_clkdiv_int_frac(pwmoutslice[i],
                                pwmoutdivint[i], pwmoutdivfrac[i]);
        pwm_set_wrap(pwmoutslice[i], 255);
        pwm_set_chan_level(pwmoutslice[i], PWM_CHAN_A, 0);
        gpio_set_function(pwmoutpins[i], GPIO_FUNC_PWM);
    };

    // NUMPWMIN Inputs
    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv_mode(&cfg, PWM_DIV_B_HIGH);
    pwm_config_set_clkdiv_int(&cfg, PWMIN_DIV);
    for (i = 0; i < NUMPWMIN; i++)
    {
        pwm_init(pwminslice[i], &cfg, false);
        gpio_pull_up(pwminpins[i]);
        gpio_set_function(pwminpins[i], GPIO_FUNC_PWM);
    };
};

void setup_gpio()
{
    uint8_t i;
    for (i = 0; i < GPIOOUT_NUM; i++)
    {
        gpio_init(gpiooutpins[i]);
        gpio_pull_up(gpiooutpins[i]);
    }
};

void gpioout(uint8_t data)
{
    uint8_t pin;
    pin = gpiooutpins[data & 15]; // low 4 bits
    if (data & 16)
    {                             // direction bit 5(1-8)
        gpio_put(pin, data & 64); // value bit 7
        gpio_set_dir(pin, true);
    }
    else
    {
        // pull up/down enable bit 6, set pull up if data set, else pull down
        gpio_set_pulls(pin, (data & 32) && (data & 64), (data & 32) && !(data & 64));
        gpio_set_dir(pin, false);
    };
};

void pwmout(uint8_t num, uint8_t val)
{
    pwm_set_chan_level(pwmoutslice[num], PWM_CHAN_A, val << pwmoutshift[num]);
}

bool main_pwm_loop(__unused struct repeating_timer *repeatingtimer)
{
    uint8_t i;
    uint32_t t;
    uint32_t elapsed;
    //    uint32_t clock_divided = clock_get_hz(clk_sys)/1000000;
    uint32_t maxcount;
    uint16_t curcount;
    uint8_t scratch;
    t = time_us_32();
    if (pwmin.measuring)
    {
        if (pwmin.startus <= t)
            elapsed = t - pwmin.startus;
        else
            elapsed = t + 0xffffffff - pwmin.startus;

        if (elapsed >= PWMIN_PERIODUS)
        {
            // Done measuring
            for (i = 0; i < NUMPWMIN; i++)
                pwm_set_enabled(pwminslice[i], false);

            // Compute the duty

            maxcount = (elapsed * (clock_get_hz(clk_sys) / 1000000)) / PWMIN_DIV;
            if (maxcount > pwmin.maxmax)
            {
                pwmin.maxmax = maxcount;
                pwmin.maxela = elapsed;
                pwmin.maxt = t;
                pwmin.maxus = pwmin.startus;
            }
            for (i = 0; i < NUMPWMIN; i++)
            {
                curcount = pwm_get_counter(pwminslice[i]);
                if (curcount >= maxcount)
                    scratch = 255;
                else if (curcount == 0)
                    scratch = 0;
                else
                    scratch = 255 * curcount / maxcount;

                if (scratch == 255 && pwmin.dutycycle[i] == 0)
                    pwmin.runtotal[i] = 255 << PWMIN_ROTATE;
                else if (scratch == 0 && pwmin.dutycycle[i] == 255)
                    pwmin.runtotal[i] = 0;
                else
                    pwmin.runtotal[i] += scratch - pwmin.dutycycle[i];

                pwmin.dutycycle[i] = pwmin.runtotal[i] >> PWMIN_ROTATE;
            };
            pwmin.measuring = false;
        }
    }
    if (!pwmin.measuring)
    {
        for (i = 0; i < NUMPWMIN; i++)
            pwm_set_counter(pwminslice[i], 0);
        for (i = 0; i < NUMPWMIN; i++)
            pwm_set_enabled(pwminslice[i], true);
        pwmin.startus = time_us_32();
        pwmin.measuring = true;
    }
    return(true);
};

void setup_adc()
{
    adc_init();
    adc_set_temp_sensor_enabled(true);
    uint8_t i;
    for (i = 0; i < ADCIN_NUM; i++)
    {
        if (adcinpins[i] < 255)
        {
            adc_gpio_init(adcinpins[i]);
        }
    }
};

void main_output_loop()
{
    uint8_t i;

    if (text_debug) {
        BPREP;
        BPRINTF("LED: %d %d %d %d\n", pwmin.dutycycle[0], pwmin.dutycycle[1], pwmin.dutycycle[2], pwmin.dutycycle[3]);
        BFLUSH(PORT_CMD);
    };
    sharedbuf[0]=0x0e;
    sharedbuf[1]=0x50;
    sharedbuf[2]=NUMPWMIN;
    for (i = 0; i < NUMPWMIN; i++)
        sharedbuf[i+3] = pwmin.dutycycle[i];
    
    do_send(PORT_PWM,NUMPWMIN+3);
};

bool onewire_running=false;

void onewire_output_loop() {
    static uint16_t ourms=0;
    static uint8_t nextdevice=0;
    bool do_increment=true;
    uint8_t res;
    // Centidegrees: degrees C*100
    uint16_t temp;
    if (! onewire_running) {
        onewire_running=true;
// Two states
// Kick Off conversion
// 750mS later
// Retrieve all data
// Expect to be called more or less every ONEWIRELOOP mS
    if (ourms == 0 ) {
       res=pico_1wire_convert_temperature(owctx,0,false);
    }
    if (ourms >= 750) {
        if (nextdevice>=ow_devicecount) {
            nextdevice=0;
            ourms=0;
            do_increment=false;
        } else {
            res=pico_1wire_get_temperature(owctx,ow_devices[nextdevice],&temp);
            if (res) {
                BPREP;
                BPRINTF("OWFAIL: dev %d addr %llX\n",nextdevice,ow_devices[nextdevice]);
                BFLUSH(PORT_CMD);
            } else {
            if (text_debug) {
                BPREP;
                BPRINTF("OW: dev %d %d C100\n", nextdevice,temp);
                BFLUSH(PORT_CMD);
            };
            sharedbuf[0]=0x0e;
            sharedbuf[1]=0x52;
            sharedbuf[2]=nextdevice;
            sharedbuf[3]=temp & 255;
            sharedbuf[4]=temp >> 8;
            do_send(PORT_ONEWIRE,5);
            };
            nextdevice++;
        }



    }


    if (do_increment) ourms += ONEWIRELOOPMS;
    onewire_running=false;
        
    } else {
        ourms += ONEWIRELOOPMS;
    }

}

void adc_output_loop()
{
    uint8_t i;
//    const float conversion_factor = 3.3f / (1 << 12);

    adccount++;
    for (i = 0; i < ADCIN_NUM; i++)
    {
        adc_select_input(i);
        adctot[i] += adc_read();
    }
    if (adccount >= 16)
    {
        adccount = 0;
        if (text_debug) {
        BPREP;
        BPRINTF("ADC: ");
        }
        sharedbuf[0]=0x0e;
        sharedbuf[1]=0x56;
        sharedbuf[2]=ADCIN_NUM;
        for (i = 0; i < ADCIN_NUM; i++)
        {
            if (text_debug) {
                BPRINTF("%d 0x%03x ;", i, adctot[i] >> 4);
            };
            sharedbuf[(i*2)+3] = adctot[i] >>12;
            sharedbuf[(i*2)+4] = (adctot[i] >>4) & 0xff;
            adctot[i] = 0;
        }
        if (text_debug) {
            BPRINTF("\n");
            BFLUSH(PORT_CMD);
        }
        do_send(PORT_ADC,(ADCIN_NUM*2)+3);

    }
};

void udp_recv_cb(void *arg , struct udp_pcb* upcb, struct pbuf* p, const ip_addr_t* addr, uint16_t port) {
    (void)(arg);
    (void)(upcb);
    (void)(addr);
    (void)(port);
    uint16_t i;
    uint8_t c;
    uint8_t state = 0 ;
    uint8_t temp = 0;
    uint8_t *ptr;

    ptr = (uint8_t *)p->payload;
    for (i = 0 ; i < p->len ; i++ ) {

        c=ptr[i];
        {
            switch (state)
            {
            case 0:
                if (c == 0x02)
                {
                    state = 1;
                };
                if (c == 'd')
                    text_debug = true;
                if (c == 'D')
                    text_debug = false;
                break;
            case 1:
                if (c == 0x31)
                    state = 0x80; // GPIO Output
                if (c == 0x32)
                    state = 0x90; // PWM Output
                break;
            case 0x80:
                gpioout(c & 0xff);
                state = 0;
                break;
            case 0x90:
                temp = c & 0xff;
                state = 0x91;
                break;
            case 0x91:
                pwmout(temp, c & 0xff);
                state = 0;
                break;
            default:
                state = 0;
            }
        };
    };
    pbuf_free(p);
};

void loop_1() {
    static uint64_t timeus,timealarm;
    static uint8_t mainoutcount,adcoutcount,onewireoutcount;
    static uint16_t timey;
    if (timealarm == 0 ) timealarm = time_us_64() + 1000;
    while (1)
    {
        main_i2c_loop();
        timeus = time_us_64();
        if (timeus > timealarm)
        {
            timealarm = timealarm + 1000;
            mainoutcount ++;
            if (mainoutcount >= 8)
            { // 8 ms
                main_output_loop();
                mainoutcount = 0;
            }
            adcoutcount ++;
            if (adcoutcount >= 9)
            { // run adc every 9mS, it runs 16 loops per ADC for 144mS
                adc_output_loop();
                adcoutcount = 0;
            }
            onewireoutcount ++;
            if (onewireoutcount >= ONEWIRELOOPMS) {}
                onewireoutcount = 0;
                onewire_output_loop(timeus);
            };

            timey++;
            if (timey >= 2000) {
                printf("PICO says hi %lld\n",timeus / 1000000);
                timey=0;
            }

        }
    };

void loop_1_delay(uint16_t delay) {
    uint64_t timeus,timeus2;
    timeus = time_us_64();
    loop_1();
    timeus2 = time_us_64();
    if (timeus2 < (timeus+delay)) 
        sleep_us(delay-(timeus2 -timeus));
}


int main_1()
{
    struct repeating_timer pwm_timer;
    core1alarms = alarm_pool_create_with_unused_hardware_alarm(4);
    context.max = 0;
    text_debug=1;
    do_send_str(PORT_CMD,"\nDoes a bunch of stuff.\n");

    setup_slave();
    setup_pwm();
    setup_gpio();
    setup_adc();
    
    alarm_pool_add_repeating_timer_us(core1alarms,PWMIN_PERIODUS,&main_pwm_loop,NULL,&pwm_timer);
          



    udp_recv(l_udp_pcb,udp_recv_cb,NULL);

while(1) loop_1();
};


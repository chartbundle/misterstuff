#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "pico/multicore.h"
#include "hardware/watchdog.h"
#include "hardware/structs/watchdog.h"
#include "pico/unique_id.h"
#include "pico/util/queue.h"

#include "tusb_lwip_glue.h"
#include "lwipopts.h"

queue_t send_q;
queue_t recv_q;
const char stringy[] = "Hello World2";
#define PAYLOAD 2048
uint8_t recv_payload[PAYLOAD];
uint8_t recv_process[PAYLOAD];

// struct data_out{
//   uint16_t port;
//   struct pbuf *pbufptr;
// };

struct udp_pcb *l_udp_pcb;

// Main for the other core
// void main_1(void) {
//     err_t wr_err;
//     struct data_out d;
//     printf("Launched\n");
//     while(1) {
//         d.port = 35310;
//         d.pbufptr = pbuf_alloc(PBUF_TRANSPORT,sizeof(stringy),PBUF_RAM);
//         printf(";");
//         memcpy(d.pbufptr->payload,stringy,sizeof(stringy));
//         wr_err  = udp_sendto(l_udp_pcb,d.pbufptr,IP_ADDR_BROADCAST,d.port);
//         if (wr_err) printf("%d\n",wr_err);
//         pbuf_free(d.pbufptr);
//         //queue_add_blocking(&send_q,&d);
//         // Since the send happens after the queue is cleared
//         // don't free here, let the other core clear once sent
//     };
// }
extern void main_1(void);

int main()
{
//    uint8_t n;
//    struct pbuf *bob;
    err_t wr_err;
    // struct data_out d;
//    // TX 12 RX 13
//    stdio_uart_init_full(uart0,115200,0,1);
//    stdio_init_all();
//    stdio_usb_init();
    set_sys_clock_khz(200000,true);
    printf("Hello world\n");
    // Initialize tinyusb, lwip, dhcpd and httpd
    pbuf_init();
    init_lwip();
    wait_for_netif_is_up();
    dhcpd_init();
    l_udp_pcb = udp_new();
    wr_err = udp_bind(l_udp_pcb, IP_ADDR_ANY, 35310);
    printf("udp bind %d\n",wr_err);
    ip_set_option(l_udp_pcb, SOF_BROADCAST);

    sleep_ms(10);

    multicore_launch_core1(main_1);
//    n=0;
    while (true)
    {
        tud_task();
        service_traffic();
        sleep_us(1);
        // if (n++ == 0 ) printf("|");
        // if (queue_try_remove(&send_q , &d) ) {
        // printf("d");
        // wr_err  = udp_sendto(l_udp_pcb,d.pbufptr,IP_ADDR_BROADCAST,d.port);
        // printf("%d\n",wr_err);
        // pbuf_free(d.pbufptr);
        // };
    }

    return(0);
}


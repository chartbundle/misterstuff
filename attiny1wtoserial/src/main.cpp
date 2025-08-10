#include <Arduino.h>
#include <OneWire.h>
// #include <SoftwareSerial.h>
#define MAXDEV 8
uint8_t owlist[MAXDEV][8];
uint8_t owresult[MAXDEV][2];
uint8_t owcount = 0;
uint8_t owready = 0;
uint8_t owdata[16];
OneWire DS(2);
bool rv;
uint8_t i, j;
uint8_t tempcfg;
struct soft_ring_buffer rxBuff;
TinySoftwareSerial mySerial(&rxBuff, -1, 1);

void setup()
{
  // put your setup code here, to run once:
  // Setup serial and output devices
  mySerial.begin(9600);
  // Find 1-wire devices
  DS.reset_search();

  for (i = 0; i < MAXDEV; i++)
  {
    rv = DS.search(owlist[i]);
    if (rv)
    {
      owcount = i + 1;
    }
    else
    {
      break;
    }
  }

  // Output 1-wire device list
  for (i = 0; i < owcount; i++)
  {
    Serial.print("ROM =");
    for (i = 0; i < 8; i++)
    {
      mySerial.write(' ');
      mySerial.print(owlist[i][j], HEX);
    }
    mySerial.println();
  }
}

void loop()
{
  uint8_t i, j;
  uint8_t owcrc;
  int8_t tempraw;
  int16_t tempx100;
  // put your main code here, to run repeatedly:
  // Start conversion on all devices
  DS.skip();
  DS.write(0xBE);
  // retrieve new conversion results
  delay(750);
  for (i = 0; i < owcount; i++)
  {
    DS.reset();
    DS.select(owlist[i]);
    DS.write(0xBE);
    for (j = 0; j < 9; j++)
    {
      owdata[j] = DS.read();
    }
    // Check CRC
    owcrc = DS.crc8(owdata, 8);

    // CountperC should always read 0x10, FF indicates a missing device
    if (owcrc == owdata[8] && owdata[7] != 0xFF)
    {

      tempraw = (owdata[1] << 8) | owdata[0];
      if (owlist[i][0] == 0x10)
      {
        tempraw = tempraw << 3; // 9 bit resolution default
        if (owdata[7] == 0x10)
        {
          // "count remain" gives full 12 bit resolution
          tempraw = (tempraw & 0xFFF0) + 12 - owdata[6];
        }
      }
      else
      {
        tempcfg = (owdata[4] & 0x60);
        // at lower res, the low bits are undefined, so let's zero them
        if (tempcfg == 0x00)
          tempraw = tempraw & ~7; // 9 bit resolution, 93.75 ms
        else if (tempcfg == 0x20)
          tempraw = tempraw & ~3; // 10 bit res, 187.5 ms
        else if (tempcfg == 0x40)
          tempraw = tempraw & ~1; // 11 bit res, 375 ms
                                  //// default is 12 bit resolution, 750 ms conversion time
      }
      tempx100 = 100 * tempraw / 16;
      mySerial.println(tempx100);
    }
    else
    {
      mySerial.println("FAIL");
    }
  }
}

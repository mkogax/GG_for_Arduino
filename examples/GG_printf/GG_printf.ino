#include <GG_for_Arduino.h>

/*
  GG for Arduino ** Printf() demo
  Arduino standard serial (115200bps / 8bit length / non-parity)
  No command line interface


  Example

  ** GG start
  [setup] millis()=       0
  [loop] millis()=       0
  c=  1=0x01=0b00000001
  delay(1000)
  [loop] millis()=    1002
  c=  2=0x02=0b00000010
  delay(1000)

*/

void setup() {
  gg_start("** GG start\n");     // Start processing GG for Arduino
  gg_printf("[setup] millis()=%8ld\n", millis());
}

void loop() {
  static unsigned char c = 0;
  gg_printf("[loop] millis()=%8ld\n", millis());
  c++;
  gg_printf("c=%3d=0x%02X=0b%08b\n", c, c, c);
  gg_printf("delay(1000)\n");
  delay(1000);
}

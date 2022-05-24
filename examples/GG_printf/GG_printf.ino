#include <GG_for_Arduino.h>

/*
  GG for Arduino ** Printf() demo
  Arduino standard serial (115200bps / 8bit length / non-parity)
  No command line interface
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

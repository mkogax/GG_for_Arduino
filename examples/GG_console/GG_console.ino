#include <GG_for_Arduino.h>

/*
  GG for Arduino ** Simple console demo
  Arduino standard serial (115200bps / 8bit length / non-parity)
  Enter the help command to display the command list.
*/

void setup() {
  gg_init();    // initialization
}

void loop() {
  gg_con_MonCheck();  // console processing (returns immediately if nothing is done)
}

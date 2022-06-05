#include <GG_for_Arduino.h>

/*
  GG for Arduino ** Simple console demo
  Arduino standard serial (115200bps / 8bit length / non-parity)
  Enter the help command to display the command list.


  Example

  ** GG **
  >help
          help [cmd..]                  : command help
  -- memory command --
            md [addr [alen]]            : mem dump
            ms addr data..              : mem set
            mf addr alen data           : mem fill
  >md 0x0000 20
  00000000  00 00 D8 21 04 00 00 00  00 00 00 00 06 03 F2 02  ...!.... ........
  00000010  D8 21 00 00  .!..
  >

*/

void setup() {
  gg_start("** GG **\n>");    // Start processing GG for Arduino
  gg_con_RegistMemCMD();      // register memory command
}

void loop() {
  gg_con_Check();  // console processing (returns immediately if nothing is done)
}

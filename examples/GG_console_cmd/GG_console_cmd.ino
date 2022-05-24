#include <GG_for_Arduino.h>

/*
  GG for Arduino ** Example of adding a console command
  Arduino standard serial (115200bps / 8bit length / non-parity)
  Enter the help command to display the command list.

  C_test1() is the execution part of the command to be added (test1).
  Argc and argv are command line arguments.
  You can write your commands just like the command line main() routines found in C training manuals and the like.
  argv[] is 32 arrays. argv[0] contains the command string itself (eg "test1").


  Example

  ** GG **
  >help
  -- GG standard command --
          help [cmd..]                  : command help
            md [addr [alen]]            : mem dump
            ms addr data..              : mem set
            mf addr alen data           : mem fill
            tp [#1 [#2]]                : TP(test pin) set
  -- Example of registering a console command --
         test1 [...]                    : Displays the arguments when the command is executed
  >test1 1 22 333 0xabcd 100+20+0x1000
  argc=6
  argv[ 0] test1            (0x00000000,0)
  argv[ 1] 1                (0x00000001,1)
  argv[ 2] 22               (0x00000016,22)
  argv[ 3] 333              (0x0000014D,333)
  argv[ 4] 0xabcd           (0x0000ABCD,43981)
  argv[ 5] 100+20+0x1000    (0x00001078,4216)
  >

*/

void setup() {
  gg_start("** GG **\n>");    // Start processing GG for Arduino

  // Register with console command (you can add more and more in this way)
  GG_CON_CMDMRK("-- Example of original command registration --");     // Delimited line (displayed in the command list)
  GG_CON_CMDADD(C_test1, "test1", "[...]", "Display command line arguments when executing this command" );  // [1]command(function),[2]command(name),[3]help(parameters),[4]help(description)
}

void loop() {
  gg_con_Check();  // console processing (returns immediately if nothing is done)
}

int C_test1(int argc, char **argv)
{
  int i;
  gg_printf("argc=%d\n", argc);
  for (i=0; i<argc; i++) {
    long x = 0;
    if (isDigit(argv[i][0])) x = gg_asc2int(argv[i]);
    gg_printf("argv[%2d] %-16s (0x%08lX,%ld)\n", i, argv[i], x, x);
  }
  return 0;
}

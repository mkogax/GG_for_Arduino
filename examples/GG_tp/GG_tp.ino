#include <GG_for_Arduino.h>

/*
  GG for Arduino ** Example of using TP function
  Arduino standard serial (115200bps / 8bit length / non-parity)
  Enter the help command to display the command list.

  The status of the test point (TP) is output in real time to the output pin of Arduino.
  There are two test point outputs (TP1, TP2).
  You can know the processing time, delay time, execution order, etc. by connecting to the oscilloscope and observing the state change.

  Test points (TP) are managed by status number.
  You can specify which status number to output to the output pin with the "tp" command.
  >tp 10 20  (TP1 output:status number 10, TP2 output:status number 20）

 */

void setup() {
  gg_tp1_no = 2;  // Arduino pin number for TP1
  gg_tp2_no = 3;  // Arduino pin number for TP2
  gg_start("** GG **\n>");    // Start processing GG for Arduino
}

void loop() {

  // Turn number 10 twice on and off at the beginning of loop()
  GG_TP_ON(10);   // number 10 ON
  GG_TP_OFF(10);  // number 10 OFF
  GG_TP_ON(10);   // number 10 ON
  GG_TP_OFF(10);  // number 10 OFF

  // number 20 : Observe the processing time of gg_con_Check()
  GG_TP_ON(20);   // number 20 ON
  gg_con_Check();  // console processing (returns immediately if nothing is done)
  GG_TP_OFF(20);  // number 20 OFF

  // number 30 : Observe the processing time of delayMicroseconds(100)
  GG_TP_ON(30);   // number 30 ON
  delayMicroseconds(100);
  GG_TP_OFF(30);  // number 30 OFF
}

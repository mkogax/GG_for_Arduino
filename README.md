# GG for Arduino

GG for Arduino is a serial console library. It also contains various functions for implementing the console.  

## Console

You can implement a command line interface on your Arduino and add your own commands.  

### Communication specifications

* Arduino standard serial(Serial)
* 115200BPS / 8bit data length / non-parity
* Echo back is done by GG
* Execute command with carriage return (\r) or line feed (\n)

### Pre-registered commands

* `help` help
* `md` mem dump
* `ms` mem set
* `mf` mem fill
* `tp` TP(test point) select

### How to add your own command

You can add your own commands. The command execution part is a function that passes command line arguments with argc and argv. Please refer to the example of the main function using command line arguments in the standard reference book of C language.  
After creating the function of the original command, register it as shown in the example program below.  

* [GG_console_cmd.ino](examples/GG_console_cmd/GG_console_cmd.ino).

### How to use without command line interface

You can use character output like printf () without command line interface.  
See example below.

* [GG_printf.ino](examples/GG_printf/GG_printf.ino).

### Timing analysis

By using the TP function, you can analyze the timing of your program's operation.  
You can measure the processing time of the routine. It is also convenient for observing the status of interrupt processing and task processing in the RTOS.  

First, embed the status number control (GG_TP_ON / GG_TP_OFF) in the program point you want to analyze. Then use the tp command to select the status number to be output to TP1 or TP2. You can also switch at any time.  

Millions of logical state numbers can be placed, but there are only two physical outputs, TP1 and TP2. This is limited because the number of physical signals is limited and the CPU load increases when many signal pin switchings are checked.

In the sample below, TP1 is Arduino pin 2 and TP2 is Arduino pin 3. The oscilloscope has TP1 (pin 2) connected to yellow CH1 and TP2 (pin 3) connected to purple CH2. The tp command (tp 10 30) sets TP1 to status number 10 and TP2 to status number 30.

* [GG_tp.ino](examples/GG_tp/GG_tp.ino).  

![tp_10_30](tp_10_30.jpg "Y:TP1(status number 10),M:TP2(status number 30)")

## Functions

All functions do not use the heap.

|Functions / Variables|specification|
|--|---|
|***GG_for_Arduino.h***||
|int gg_start(const char \*title)|Start processing GG for Arduino|
|int gg_tp1_no|Arduino output pin number for TP1. Set before gg_start() if necessary|
|int gg_tp2_no|Arduino output pin number for TP2. Set before gg_start() if necessary|
|long gg_con_baud|baud rate. Set before gg_start() if necessary|
|***gg_base.h***||
|GG_TP_ON(number)|Turn on the TP status number|
|GG_TP_OFF(number)|Turn off the TP status number|
|***gg_uty.h***||
|GG_TP_ON(number)|Turn on the TP status number|
|GG_TP_OFF(number)|Turn off the TP status number|
|int gg_GetC(void)|1 character input (-1 = no input)|
|int gg_PutC(int c)|1 character output|
|int gg_PutS(const char \*s)|String output|
|int32_t gg_asc2int(char \*p)|Convert from string to integer. Supports '0x'|
|int16_t gg_mbuf_init(GGT_MBUF \*mbuf, void\* buf, int max)|Mini buffer tool. Initialization|
|int16_t gg_mbuf_put(GGT_MBUF \*mbuf, int16_t d)|Mini buffer tool. 1 byte write|
|int16_t gg_mbuf_get(GGT_MBUF \*mbuf)|Mini buffer tool. 1 byte read|
|int16_t gg_mbuf_peek(GGT_MBUF \*mbuf, int16_t ix)|Mini buffer tool. Peeping|
|gg_time_t|A type of total time expressed in seconds that is valid only in GG. The starting point (0) is January 1, 2000|
|GGT_BCDTIME|BCD date and time structure|
|GGT_TIME|Date and time structure|
|int gg_tim_bcd2dt(GGT_BCDTIME \*b, GGT_TIME \*t)|Convert from GGT_BCDTIME to GGT_TIME|
|int gg_tim_dt2bcd(GGT_TIME \*t, GGT_BCDTIME \*b)|Convert from GGT_TIME to GGT_BCDTIME|
|gg_time_t gg_tim_dt2tt(GGT_TIME \*t)|Convert from GGT_TIME to gg_time_t|
|gg_time_t gg_tim_YYMMDD2tt(int YY, int MM, int DD)|Convert from date to gg_time_t|
|gg_time_t gg_tim_hhmmss2tt(int hh, int mm, int ss)|Convert from hour, minute and second to gg_time_t|
|int gg_tim_tt2dt(gg_time_t x, GGT_TIME \*t)|Convert from gg_time_t to GGT_TIME|
|int gg_tim_checkdt(GGT_TIME \*t)|Date and time validity check|
|int gg_tim_repairdt(GGT_TIME \*t)|Date and time correction|
|int gg_tim_tt2week(gg_time_t x)|Determine what day of the week from gg_time_t(0=sun,..)|
|***gg_printf.h***||
|int gg_vxprintf(int (\*putc)(int), const char \*fmt, va_list vlst)|vprintf() with putc()|
|int gg_xprintf(int (\*putc)(int), const char \*fmt, ...)|printf() with putc()|
|int gg_printf(const char \*fmt, ...)|printf() with standard output.% e is not implemented|
|int gg_vsprintf(char \*buf, const char \*fmt, va_list vlst)|vsprintf() to buf[]|
|int gg_sprintf(char \*buf, const char \*fmt, ...)|sprintf() to buf[]|
|gg_printDATE(gg_time_t)|"YY/MM/DD"|
|gg_printTIME(gg_time_t)|"hh:mm:ss"|
|gg_printDT(gg_time_t)|"YY/MM/DD hh:mm:ss"|
|gg_printDTs(gg_time_t)|"YY/MM/DD hh:mm:ss "|
|***gg_console.h***||
|GG_CON_CMDADD(func, cmd, prm, help)|Registering console commands|
|GG_CON_CMDMRK(title)|Display delimiter of list in help|
|GG_CON|Work area for console (internal use)|
|int gg_con_MonInit(void)|Console initialization|
|char \*gg_con_prompt|Prompt string. ">" by default|
|int gg_con_MonRePrompt(void)|Redisplay prompt|
|int gg_con_MonPrompt(void)|Prompt display (used internally)|
|int gg_con_Check(void)|Console check with gg_GetC()|
|int gg_con_MonCheck(int c)|Console check with 1 character|
|int gg_con_MonExec(char \*buf)|Command line processing (Note: buf is destroyed by arg analysis)|
|int gg_con_MonBuf2Arg(char \*p, char \**argv, int argmax)|Extract arg from buf[] (return = argc)|
|int gg_con_MonExecArg(int argc, char \**argv)|Search for argv [0] as a command and execute it if found|
|int gg_con_CmdInit(void)|Initialization of command definition list|
|int gg_con_CmdAdd(GGT_CMD \*def)|Add to command definition list|
|int gg_con_CmdN(void)|Number of command registrations|
|int gg_con_CmdHelp(char \*cmd)|Help for the specified command|
|int gg_con_CmdHelpAll(void)|Display command list|
|int gg_con_RegistCMD(void)|Registration of basic commands to be pre-registered|

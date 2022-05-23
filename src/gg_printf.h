/*
	gg_printf		GGツール 書式付きprintツール

	2022.01.10	M.Kogan	初版開発開始
*/

#ifndef 	__GG_PRINTF_H__
#define 	__GG_PRINTF_H__


//------------------
//	書式付きprintツール
//------------------
//	stdio.h関連のうち「書式付きprint」関連をメインに実装
#include "stdarg.h"			// va_arg他
int gg_vxprintf(int (*putc)(int), const char *fmt, va_list vlst);	// putc()指定でvprintf()
int gg_xprintf(int (*putc)(int), const char *fmt, ...);				// putc()指定でprintf()
int gg_printf(const char *fmt, ...);								// 標準出力でprintf()
int gg_vsprintf(char *buf, const char *fmt, va_list vlst);			// buf[]指定でvprintf()
int gg_sprintf(char *buf, const char *fmt, ...);					// buf[]指定でprintf()

//	日時表示
int gg_xprintDATE(int (*putc)(int), gg_time_t x);		// "YY/MM/DD"
int gg_xprintTIME(int (*putc)(int), gg_time_t x);		// "hh:mm:ss"
int gg_xprintDT(int (*putc)(int), gg_time_t x);			// "YY/MM/DD hh:mm:ss"
int gg_xprintDTs(int (*putc)(int), gg_time_t x);		// "YY/MM/DD hh:mm:ss " 最後に空白
#define gg_printDATE(x)	gg_xprintDATE(NULL, x)			// "YY/MM/DD"
#define gg_printTIME(x)	gg_xprintTIME(NULL, x)			// "hh:mm:ss"
#define gg_printDT(x)	gg_xprintDT(NULL, x)			// "YY/MM/DD hh:mm:ss"
#define gg_printDTs(x)	gg_xprintDTs(NULL, x)			// "YY/MM/DD hh:mm:ss " 最後に空白

#endif	//	__GG_PRINTF_H__

/*
	ggbase		GGツール基本部メイン

	2022.01.05	M.Kogan	初版開発開始
	2022.06.05	M.Kogan	TP分離,gg_init()廃止

*/

#ifndef 	__GGBASE_H__
#define 	__GGBASE_H__


#include <Arduino.h>


#ifdef __cplusplus
extern "C" {
#endif


//------------------
//	HW定義(HW環境依存部)
//------------------
//	割禁定義
//	整数型宣言
#include "GG_HW_def.h"			// GGツール HW定義(HW環境依存部)


//------------------
//	GGツール
//------------------
#include "gg_uty.h"      	// UTY
#include "gg_printf.h"      // PRINTF
#include "gg_console.h"     // CONSOLE


#ifdef __cplusplus
}
#endif

#endif	//	__GGBASE_H__

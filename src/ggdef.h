/*
	ggdef.h	GGツール 個別定義部
	
	2022.01.08	M.Kogan	初版開発開始

	***********************************
	***        Arduino専用          ***
	***********************************

*/

#ifndef 	__GGDEF_H__
#define 	__GGDEF_H__

//------------------
//	割込み禁止制御
//------------------
extern int gg_DI_count;						// 割込み禁止ネスト数(0=割込み許可中)(GG_for_Arduino.cpp)
#define GG_DI_ON_real()		noInterrupts()	// 割込み禁止(リアル)
#define GG_DI_OFF_real()	interrupts()	// 割込み許可(リアル)
#define GG_DI_ON()			{ GG_DI_ON_real(); gg_DI_count=1; GG_TP_ON(1)}						// 割込み禁止(count=1)
#define GG_DI_OFF()			{ GG_DI_ON_real(); gg_DI_count=0; GG_TP_OFF(1); GG_DI_OFF_real(); }	// 割込み許可(count=0)
#define GG_DI_BEGIN()		{ GG_DI_ON_real(); gg_DI_count++; GG_TP_ON(1);						// 割禁開始 TP#1=割禁状態
#define GG_DI_END()		  	  GG_DI_ON_real(); if (--gg_DI_count<=0) { gg_DI_count=0; GG_DI_OFF_real(); GG_TP_OFF(1); }	}	// 割禁解除 TP#1=割禁状態



//------------------
//	整数型宣言
//------------------
//	以下の整数型の宣言が必要
//	int8_t,int16_t,int32_t,int64_t,uint8_t,uint16_t,uint32_t,uint64_t

#if 0		//	stdint.hが無い場合は整数型を宣言する(#if 1とする)
typedef signed char			int8_t;
typedef signed short		int16_t;
typedef signed long			int32_t;
typedef signed long long	int64_t;
typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned long		uint32_t;
typedef unsigned long long	uint64_t;
#else
#include "stdint.h"
#endif

#define NULL (0)


//------------------
//	テストポイント(TP機能)設定
//------------------
//	以下のテストポイント(TP機能)設定が必要
//	GG_TP_NOCODE		テストポイント(TP機能)コードの非生成(なければコード生成する)
//	GG_TP1_ON/OFF		TP1(出力ピン1)制御
//	GG_TP2_ON/OFF		TP2(出力ピン2)制御

extern void gg_tp1_set(int flag);		// TP1(出力ピン1)出力(GG_for_Arduino.cpp)
extern void gg_tp2_set(int flag);		// TP2(出力ピン2)出力(GG_for_Arduino.cpp)
#define GG_TP1_ON()		(gg_tp1_set(1))	// TP1(出力ピン1)をON
#define GG_TP1_OFF()	(gg_tp1_set(0))	// TP1(出力ピン1)をOFF
#define GG_TP2_ON()		(gg_tp2_set(1))	// TP2(出力ピン2)をON
#define GG_TP2_OFF()	(gg_tp2_set(0))	// TP2(出力ピン2)をOFF

#endif	//	__GG_USERDEF_H__

/*
	GG_HW_def.h	GGツール HW定義(HW環境依存部)
	
	2022.01.08	M.Kogan	初版開発開始
	2022.06.05	M.Kogan	ファイル名変更,TP分離

	***********************************
	***        Arduino専用          ***
	***********************************

*/

#ifndef 	__GG_HW_DEF_H__
#define 	__GG_HW_DEF_H__

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

#ifndef NULL
#define NULL (0)
#endif

#endif	//	__GG_HW_DEF_H__

/*
	ggbase		GGツール基本部メイン

	2022.01.05	M.Kogan	初版開発開始

	gg_di	割込み禁止制御
	gg_tp	テストポイント(TP機能)

*/

#ifndef 	__GGBASE_H__
#define 	__GGBASE_H__

#ifdef __cplusplus
extern "C" {
#endif

//------------------
//	個別定義
//------------------
//	整数型宣言
//	テストポイント(TP機能)設定
//	環境選択
#include "ggdef.h"			// GGツール 個別定義部

//------------------
//	テストポイント(TP機能)
//------------------
//	GG_TP_ON(x)
//		GG_TP1==xならTP1(出力ピン1)をON
//		GG_TP2==xならTP2(出力ピン2)をON
//	GG_TP_OFF(x)
//		GG_TP1==xならTP1(出力ピン1)をOFF
//		GG_TP2==xならTP2(出力ピン2)をOFF
//
extern int gg_tp1_sel;		// TP1(出力ピン1)に出力する状態番号変数(ggbase.c)
extern int gg_tp2_sel;		// TP2(出力ピン2)に出力する状態番号変数(ggbase.c)
#ifndef GG_TP1
#define GG_TP1			(gg_tp1_sel)	// TP1(出力ピン1)に出力する状態番号
#endif
#ifndef GG_TP2
#define GG_TP2			(gg_tp2_sel)	// TP2(出力ピン2)に出力する状態番号
#endif
#ifndef GG_TP_NOCODE		// テストポイント(TP機能)を使用する場合
	#define GG_TP_ON(x)		{if (GG_TP1==(x)) GG_TP1_ON();  if (GG_TP2==(x)) GG_TP2_ON();  }
	#define GG_TP_OFF(x)	{if (GG_TP1==(x)) GG_TP1_OFF(); if (GG_TP2==(x)) GG_TP2_OFF(); }
#else						// テストポイント(TP機能)を使用しない場合
	#define GG_TP_ON(x)		// 無効
	#define GG_TP_OFF(x)	// 無効
#endif



//------------------
//	GGツール関数
//------------------
//	起動直後1回だけコール
int ggbase_init(void);		// GGツール初期化


#include "gg_uty.h"      	// UTY
#include "gg_printf.h"      // PRINTF
#include "gg_console.h"     // CONSOLE

#ifdef __cplusplus
}
#endif

#endif	//	__GGBASE_H__

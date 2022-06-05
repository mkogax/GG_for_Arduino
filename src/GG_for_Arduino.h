/*
	GG_for_Arduino.h		Arduino用GGツール(ヘッダファイル)
	
	2022.04.30	M.Kogan	初版開発開始

*/

#ifndef 	__GG_FOR_ARDUINO_H__
#define 	__GG_FOR_ARDUINO_H__

#include "ggbase.h"

//	gg_start()時の変更可能パラメータ(事前にデフォルト値が入っている)(Arduino専用)
extern long gg_con_baud;			// コンソール用シリアル(Serial)のボーレート(負数ならgg_start()で初期化しない)

int gg_start(const char *title);	// Arduino用GGの処理開始

//------------------
//	テストポイント(TP機能)
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

//	GG_TP_ON(x)
//		GG_TP1==xならTP1(出力ピン1)をON
//		GG_TP2==xならTP2(出力ピン2)をON
//	GG_TP_OFF(x)
//		GG_TP1==xならTP1(出力ピン1)をOFF
//		GG_TP2==xならTP2(出力ピン2)をOFF
//
extern int gg_tp1_sel;		// TP1(出力ピン1)に出力する状態番号変数
extern int gg_tp2_sel;		// TP2(出力ピン2)に出力する状態番号変数
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

// TP機能出力ピン番号
//	旧v1.0.x対応のため公開している(本来は公開不要,GG_tp.inoサンプル参照)
//	gg_start()前にセットすれば自動的にgg_tp_start()が呼ばれる
extern int gg_tp1_no;		// TP1出力ピン番号(-1=なし)
extern int gg_tp2_no;		// TP2出力ピン番号(-1=なし)

int gg_tp_start(int tp1_no, int tp2_no);		// TP機能スタート(出力ピン初期化とコマンド登録)


#endif	//	__GG_FOR_ARDUINO_H__

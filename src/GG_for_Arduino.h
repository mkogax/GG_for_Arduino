/*
	GG_for_Arduino.h		Arduino用GGツール(ヘッダファイル)
	
	2022.04.30	M.Kogan	初版開発開始

*/

#ifndef 	__GG_FOR_ARDUINO_H__
#define 	__GG_FOR_ARDUINO_H__

#include "ggbase.h"

//	gg_start()時の変更可能パラメータ(事前にデフォルト値が入っている)(Arduino専用)
extern int gg_tp1_no;				// TP1出力ピン番号(-1=なし)
extern int gg_tp2_no;				// TP2出力ピン番号(-1=なし)
extern long gg_con_baud;			// コンソール用シリアル(Serial)のボーレート(負数ならgg_start()で初期化しない)

int gg_start(const char *title);	// Arduino用GGの処理開始


#endif	//	__GG_FOR_ARDUINO_H__

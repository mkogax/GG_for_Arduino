/*
	GG_for_Arduino.cpp	Arduino用GGツール(個別実行コード)

	2022.04.30	M.Kogan	初版開発開始

*/

#include "GG_for_Arduino.h"
#include "Arduino.h"

int gg_DI_count;					// 割込み禁止ネスト数(0=割込み許可中)
int gg_tp1_no = -1;					// TP1出力ピン番号(-1=なし)
int gg_tp2_no = -1;					// TP2出力ピン番号(-1=なし)
long gg_con_baud = 115200;			// コンソール用シリアル(Serial)のボーレート(-1=gg_init()で初期化しない)

static int std_putc_sub(int c)
{
    Serial.write(c);
    return 0;
}
static int std_putc(int c)
{
    if (c=='\n') std_putc_sub('\r'); 
    return std_putc_sub(c);
}
static int std_getc()
{
  return Serial.read();
}

int gg_init(void)					// Arduino用GGツール初期化
{
	// TP出力の初期化(セットされてれば出力に初期化)
	if (gg_tp1_no>=0) pinMode(gg_tp1_no, OUTPUT);
	if (gg_tp2_no>=0) pinMode(gg_tp2_no, OUTPUT);

	// コンソール用シリアルのボーレート設定(負数であればセットしない)
	if (gg_con_baud>0) {
		Serial.begin(gg_con_baud);		// ArduinoのSerialをセットアップ
		gg_std_putc = std_putc;			// ArduinoのSerialへの出力ルーチン
		gg_std_getc = std_getc;			// ArduinoのSerialからの入力ルーチン
	}

	// GGツールベース初期化
	ggbase_init();    	// GGツール初期化
	gg_con_MonInit();	// GGモニタ初期化	
	gg_con_RegistCMD();	// 標準コマンド登録
	gg_con_MonStart();	// GGモニタ開始(タイトルとプロンプト表示)

	return 0;
}

//
//	テストポイント(TP機能)
//
void gg_tp1_set(int flag)		// TP1(出力ピン1)出力(GG_for_Arduino.cpp)
{
	if (gg_tp1_no>=0) {
		digitalWrite(gg_tp1_no, flag);
	}
}
void gg_tp2_set(int flag)		// TP2(出力ピン2)出力(GG_for_Arduino.cpp)
{
	if (gg_tp2_no>=0) {
		digitalWrite(gg_tp2_no, flag);
	}
}

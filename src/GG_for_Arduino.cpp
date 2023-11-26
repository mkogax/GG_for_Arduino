/*
	GG_for_Arduino.cpp	Arduino用GGツール(個別実行コード)

	2022.04.30	M.Kogan	初版開発開始

*/

#include "GG.h"
#include "Arduino.h"

int gg_DI_count;					// 割込み禁止ネスト数(0=割込み許可中)
long gg_con_baud = 115200;			// コンソール用シリアル(Serial)のボーレート(-1=gg_start()で初期化しない)

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

int gg_start(const char *title)		// Arduino用GGの処理開始
{

	// コンソール用シリアルのボーレート設定(負数であればセットしない)
	if (gg_con_baud>0) {
		Serial.begin(gg_con_baud);		// ArduinoのSerialをセットアップ
		while(!Serial)	;				// Serialの準備ができるまで待つ(USBシリアル対応)	2023.11.26 M.Kogan
		gg_std_putc = std_putc;			// ArduinoのSerialへの出力ルーチン
		gg_std_getc = std_getc;			// ArduinoのSerialからの入力ルーチン
	}

	// GGツールベース初期化
	gg_con_MonInit();		// GGモニタ初期化	
	gg_con_RegistHelpCMD();	// helpコマンド登録(最初はこれだけ)

	// tp1/tp2番号指定あればTP機能スタート(旧v1.0.x対応)	2022.06.05 M.Kogan
	if ((gg_tp1_no>=0)||(gg_tp2_no>=0)) {
		gg_tp_start(gg_tp1_no, gg_tp2_no);		// TP機能スタート(出力ピン初期化とコマンド登録)
	}

	// デバッグモニタ開始(タイトル表示) 旧:gg_con_MonStart()
	if (title) gg_PutS(title);

	return 0;
}

//
//	テストポイント(TP機能)
//
int gg_tp1_sel = -1;			// TP1(出力ピン1)状態番号変数
int gg_tp2_sel = -1;			// TP2(出力ピン2)状態番号変数

int gg_tp1_no = -1;				// TP1出力ピン番号(-1=なし)	Arduinoピン番号
int gg_tp2_no = -1;				// TP2出力ピン番号(-1=なし)	Arduinoピン番号

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

static int C_tp(int argc, char**argv)					// TP設定
{
	if (argc<=4) {
		if (argc>1) GG_TP1	 = gg_asc2int(argv[1]);
		if (argc>2) GG_TP2	 = gg_asc2int(argv[2]);
		gg_printf("<TP status number(The specified status number appears in TP)>\n");
		gg_printf("TP1 = %3d (-1=do nothing)\n", GG_TP1);
		gg_printf("TP2 = %3d (-1=do nothing)\n", GG_TP2);
	} else {
		gg_con_CmdHelp(argv[0]);
	}
	return 0;
}

int gg_tp_start(int tp1_no, int tp2_no)		// TP機能スタート(出力ピン初期化とコマンド登録)
{
	// TP出力の初期化(セットされてれば出力に初期化)
	gg_tp1_no = tp1_no;
	gg_tp2_no = tp2_no;
	if (gg_tp1_no>=0) pinMode(gg_tp1_no, OUTPUT);
	if (gg_tp2_no>=0) pinMode(gg_tp2_no, OUTPUT);

	// コマンド登録
	GG_CON_CMDMRK("-- TP command --");		// 区切り行（helpでコマンド一覧のときに表示）
	GG_CON_CMDADD(C_tp,		"tp",		"[#1 [#2]]",			"TP(test point) select"	);		//
	return 0;
}

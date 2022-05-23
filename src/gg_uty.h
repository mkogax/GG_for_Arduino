/*
	gg_uty		GGツール 一般ユーティリティ

	2022.01.25	M.Kogan	初版開発開始
*/

#ifndef 	__GG_UTY_H__
#define 	__GG_UTY_H__


//--------------------------------------------------------------------------
//
// 一般ユーティリティ(gg_uty.c)		2022.01.25 M.Kogan
//
//--------------------------------------------------------------------------

//------------------
//	標準入出力
//------------------
extern int (*gg_std_putc)(int);		// 標準出力関数へのポインタ
extern int (*gg_std_getc)(void);	// 標準入力関数へのポインタ(-1=入力なし)
int gg_GetC(void);				// 1文字入力(-1=入力なし)
int gg_PutC(int c);				// 1文字出力
int gg_PutS(const char *s);		// 文字列出力
int gg_PreDetection(int c);		// ESC/Break事前検出(GGの標準入力バッファに入れるときチェック)

//------------------
//	文字入力ツール
//------------------
extern int gg_sbufmax;				// 出力文字列バッファ最大値(デフォルト=256)
int32_t gg_asc2int(char *p);		// 文字列から数値
int32_t gg_asc2int_noerr(char *p);	// 文字列から数値へ(残ってもエラーなし)
float gg_asc2float(char *p);		// 文字列から数値(float)へ

//------------------
//	文字出力ツール
//------------------
int gg_strlen(const char *buf);						// 文字列(ASCIZ)の長さを返す
int gg_isdigit(int c);								// 文字が10進数字であるとき0を返す
char* gg_strcpy(char* a, const char* b);			// 文字列コピー
int gg_strcmp(const char* a, const char* b);		// 文字列比較
int gg_strcmpx(const char* a, const char* b);		// 文字列比較(大文字小文字無視)
int gg_wccmp(const char* wc, const char* b);		// ワイルドカード文字列比(wcは*,?を含む,0=一致)
int gg_wccmpx(const char* wc, const char* b);		// ワイルドカード文字列比(wcは*,?を含む,0=一致)(大文字小文字無視)
void *gg_memcpy(void *buf1, const void *buf2, unsigned int n);	// メモリコピー
void *gg_memset(void *buf, int ch, unsigned int n);				// メモリセット
int gg_toupper(int c);								// 大文字化
int gg_tolower(int c);								// 小文字化
char *gg_strcat(char* a, const char* b, int max);	// 文字列追加

//------------------
//	ミニバッファ
//------------------
//	バイト単位のFIFOバッファ
typedef struct GGtagMBUF {
	int16_t	max;	// 最大格納数(バイト)
	int16_t	n;		// 現在格納数(バイト)
	int16_t	wix;	// 次回書込みindex
	int16_t	rix;	// 次回読出しindex
	uint8_t	*buf;	// バッファ(バイト)へのポインタ
} GGT_MBUF;
int16_t	gg_mbuf_init(GGT_MBUF *mbuf, void* buf, int max);	// ミニバッファ 初期化
int16_t	gg_mbuf_put(GGT_MBUF *mbuf, int16_t d);				// ミニバッファ 書込み
int16_t	gg_mbuf_get(GGT_MBUF *mbuf);						// ミニバッファ 読出し(-1=なし)
int16_t	gg_mbuf_peek(GGT_MBUF *mbuf, int16_t ix);			// ミニバッファ のぞき見(-1=なし)

//------------------
//	日時変換(2000年～2099年)
//	本来はgg_time_tが負数になる2067年から禁止していたが、負数も認めることで2099年まで対応する		(2099年対応)	2022.02.11 M.Kogan
//------------------
//	time_t(long)<-->YYYY,MM,DD,hh,mm,ss(int)<-->YY,MM,DD,hh,mm,ss(BCD)
#define gg_time_t	int32_t		// 日時の総秒カウンタ(2000/01/01 00:00:00=0L)
typedef struct GGtagBCDTIME {	// BCD日時の保持構造体
	uint8_t	bYY;	// 年(BCD) 0x00=2000年
	uint8_t	bMM;	// 月(BCD) 0x01～0x12
	uint8_t	bDD;	// 日(BCD) 0x01～0x31
	uint8_t	bhh;	// 時(BCD) 0x00～0x23
	uint8_t	bmm;	// 分(BCD) 0x00～0x59
	uint8_t	bss;	// 秒(BCD) 0x00～0x59
} GGT_BCDTIME;
typedef struct GGtagTIME {		// 日時の保持構造体
	int8_t	tYY;	// 年 	0～99(2000年～2099年)		(2099年対応)	2022.02.11 M.Kogan
	int8_t	tMM;	// 月	1～12
	int8_t	tDD;	// 日	1～31
	int8_t	thh;	// 時	0～23
	int8_t	tmm;	// 分	0～59
	int8_t	tss;	// 秒	0～59
} GGT_TIME;
int gg_tim_bcd2dt(GGT_BCDTIME *b, GGT_TIME *t);			// BCD日時-->日時 変換
int gg_tim_dt2bcd(GGT_TIME *t, GGT_BCDTIME *b);			// 日時-->BCD日時 変換
gg_time_t gg_tim_dt2tt(GGT_TIME *t);					// 日時-->総秒(time_t)
gg_time_t gg_tim_YYMMDD2tt(int YY, int MM, int DD);		// 年月日-->総秒(time_t)
gg_time_t gg_tim_hhmmss2tt(int hh, int mm, int ss);		// 時分秒-->総秒(time_t)
int gg_tim_tt2dt(gg_time_t x, GGT_TIME *t);				// 総秒(time_t)-->日時
int gg_tim_checkdt(GGT_TIME *t);						// 日時のチェック(0=OK)
int gg_tim_repairdt(GGT_TIME *t);						// 日時の補正(それなりに修正してしまう)
int gg_tim_tt2week(gg_time_t x);						// 総秒(time_t)-->曜日(0=日,1=月..6=土)	2000年1月1日(土)から導出


#endif	//	__GG_UTY_H__
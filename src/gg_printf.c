/*
	gg_printf		GGツール 書式付きprintツール

	2022.01.10	M.Kogan	初版開発開始
*/

#include "ggbase.h"

//
//	書式付変換(汎用)
//
// 書式の文字フラグ
#define XPF_Sharp	(1<<0)		// #
#define XPF_Zero	(1<<1)		// 0
#define XPF_Minus	(1<<2)		// -
#define XPF_Space	(1<<3)		// SP(1個の半角スペース)
#define XPF_Plus	(1<<4)		// +
#define XPF_h		(1<<5)		// h 
#define XPF_l		(1<<6)		// l
// ワークバッファのサイズ
#define XP_BUFMAX	(32)		// ワークバッファのサイズ(255以内であること)
// 書式変換用ワーク定義
typedef struct tagXPW {			// 書式変換用ワーク
	int (*xput)(int);			// 出力関数(xputmax==0)/出力バッファ(xputmax>0)
	char *xbuf;					// 上記でダメな時の出力バッファ			2022.04.03 M.Kogan
	int16_t	xputmax;			// 出力バッファサイズ(0のときputは出力関数)(32767以内)
	int16_t xputn;				// 出力書込みインデックス(32767以内)
	uint8_t	flags;				// 書式フラグ(XPF_xxx)(8bit)
	int8_t	width;				// フィールド幅(127以内)
	int8_t	pwidth;				// 精度幅(小数点以下の桁数)(127以内)
	char	work[XP_BUFMAX];	// 書式用ワークバッファ
	char	*buf;				// 書式バッファ
	int8_t	bufn;				// 書式バッファ格納数
	char	kigou;				// 先頭記号(-/+/' '/\0)
	int8_t	gyaku;				// 書式バッファ逆並びフラグ
} T_XPW;

static void XPutC(T_XPW *xpw, int c)		// 1文字出力(出力関数あるいはバッファ書込み)
{
	if (!xpw->xputmax) {					// max==0のときは
		xpw->xput(c);						// 出力関数で出力
	} else {								// max!=0のときはバッファ書込みの最大サイズを示す
	//	char *buf=(char *)xpw->xput;		// バッファポインタと見る
		char *buf=(char *)xpw->xbuf;		// 上記でダメだったとき用		2022.04.03 M.Kogan
		if (xpw->xputn<(xpw->xputmax-1)) {	// 最後に\0を付ける為nはmax-1までに制限
			buf[xpw->xputn++] = c;
		}
	}
}
static void XCnvInt(T_XPW *xpw, long d)		// 十進変換(buf[0]から逆並びで入る)
{
	xpw->buf = xpw->work;		// 書式用ワークバッファを使う
	xpw->bufn = 0;				// 書式バッファ格納数
	xpw->kigou = 0;				// 先頭記号(-/+/' '/\0)
	xpw->gyaku = 1;				// 書式バッファ逆並びフラグ=ON
	if (d<0) {
		xpw->kigou = '-';
		d = -d;
	} else {
		if (xpw->flags&XPF_Plus) {				// 強制+表示
			xpw->kigou = '+';
		} else 	if (xpw->flags&XPF_Space) {		// 強制空白表示
			xpw->kigou = ' ';
		}
	}
	if (!d) {
		if (!xpw->pwidth) return;	// ゼロで精度が0桁なら何も書かない
		xpw->buf[xpw->bufn++] = '0';
	} else {
		while(d>0) {				// "4321"
			if (xpw->bufn>=XP_BUFMAX) break;
			xpw->buf[xpw->bufn++] = '0'+(char)(d%10);
			d /= 10;
		}
	}
	while(xpw->pwidth>xpw->bufn) {	// 精度が指定されているときは、その桁までゼロを埋める
		if (xpw->bufn>=XP_BUFMAX) break;
		xpw->buf[xpw->bufn++] = '0';
	}
}
static void XCnvXOB(T_XPW *xpw, unsigned long d, char code)	// uint/HEX/OCT/BIN変換(buf[0]から逆並びで入る)
{
	int x;
	xpw->buf = xpw->work;		// 書式用ワークバッファを使う
	xpw->bufn = 0;				// 書式バッファ格納数
	xpw->kigou = 0;				// 先頭記号(-/+/' '/\0)
	xpw->gyaku = 1;				// 書式バッファ逆並びフラグ=ON
	if (!d) {
		if (!xpw->pwidth) return;	// ゼロで精度が0桁なら何も書かない
		xpw->buf[xpw->bufn++] = '0';
	} else {
		while(d>0) {				// "4321"
			if (xpw->bufn>=XP_BUFMAX) break;
			switch(code) {
			case 'u':				// 符号なし整数
				xpw->buf[xpw->bufn++] = '0'+(char)(d%10);
				d /= 10;
				break;
			case 'x':				// hex
				x = d%16;
				xpw->buf[xpw->bufn++] = x+((x<=9)?'0':('a'-10));
				d /= 16;
				break;
			case 'X':				// HEX
				x = d%16;
				xpw->buf[xpw->bufn++] = x+((x<=9)?'0':('A'-10));
				d /= 16;
				break;
			case 'o':				// OCT
				xpw->buf[xpw->bufn++] = '0'+(char)(d%8);
				d /= 8;
				break;
			case 'b':				// BIN
				xpw->buf[xpw->bufn++] = '0'+(char)(d%2);
				d /= 2;
				break;
			}
		}
	}
	while(xpw->pwidth>xpw->bufn) {	// 精度が指定されているときは、その桁までゼロを埋める
		if (xpw->bufn>=XP_BUFMAX) break;
		xpw->buf[xpw->bufn++] = '0';
	}
}
static void XCnvF(T_XPW *xpw, double d)	// 浮動小数点変換(buf[0]から逆並びで入る)
{
	int i;
	int w;			// ワーク
	long di;		// 整数部
	xpw->buf = xpw->work;		// 書式用ワークバッファを使う
	xpw->bufn = 0;				// 書式バッファ格納数
	xpw->kigou = 0;				// 先頭記号(-/+/' '/\0)
	xpw->gyaku = 1;				// 書式バッファ逆並びフラグ=ON
	if (d<0.0) {
		xpw->kigou = '-';
		d = -d;
	} else {
		if (xpw->flags&XPF_Plus) {				// 強制+表示
			xpw->kigou = '+';
		} else 	if (xpw->flags&XPF_Space) {		// 強制空白表示
			xpw->kigou = ' ';
		}
	}
	if (xpw->pwidth<0) {		// 精度無指定なら
		if 		 (d<10.0) 		xpw->pwidth=5;	// "+x.xxxxx"
		else if (d<100.0) 		xpw->pwidth=4;	// "+xx.xxxx"
		else if (d<1000.0) 		xpw->pwidth=3;	// "+xxx.xxx"
		else if (d<10000.0) 	xpw->pwidth=2;	// "+xxxx.xx"
		else					xpw->pwidth=1;	// "+xxxxx.x"
	}
	di = (long)d;				// 整数部(int)
	if (xpw->pwidth>0) {		// 精度が1桁異常の場合だけ小数点以下を記述
		d  -= (double)di;							// 小数部(double)
		xpw->bufn += xpw->pwidth;					// 小数部は点に近いところから
		for (i=1; i<=xpw->pwidth; i++) {			// 小数部桁全部を埋める(逆並び)
			d *= 10.0;
			w = (int)d;
			d -= (double)w;
			if ((xpw->bufn-i)<XP_BUFMAX) {
				xpw->buf[xpw->bufn-i] = '0'+w;
			}
		}
		if (xpw->bufn>XP_BUFMAX) xpw->bufn=XP_BUFMAX;
		if (xpw->bufn<XP_BUFMAX) xpw->buf[xpw->bufn++] = '.';	// 小数点 '.'
	}
	if (!di) {
		xpw->buf[xpw->bufn++] = '0';
	} else {
		while(di>0) {				// "4321"
			if (xpw->bufn>=XP_BUFMAX) break;
			xpw->buf[xpw->bufn++] = '0'+(char)(di%10);
			di /= 10;
		}
	}
	while(xpw->pwidth>xpw->bufn) {	// 精度が指定されているときは、その桁までゼロを埋める
		if (xpw->bufn>=XP_BUFMAX) break;
		xpw->buf[xpw->bufn++] = '0';
	}
}
static void XCnvC(T_XPW *xpw, char c)		// １文字変換
{
	xpw->work[0] = c;			// バッファに文字セット
	xpw->buf = xpw->work;		// 書式用ワークバッファを使う
	xpw->bufn = 1;				// 書式バッファ格納数=1
	xpw->kigou = 0;				// 先頭記号(-/+/' '/\0)=なし
	xpw->gyaku = 0;				// 書式バッファ逆並びフラグ=OFF
}
static void XCnvStr(T_XPW *xpw, char * const p)	// 文字列変換
{
	xpw->buf = p;				// 文字列をそのまま使う
	xpw->bufn = gg_strlen(p);	// 書式バッファ格納数=文字数
	xpw->kigou = 0;				// 先頭記号(-/+/' '/\0)=なし
	xpw->gyaku = 0;				// 書式バッファ逆並びフラグ=OFF
}
static void XPrintDFS(T_XPW *xpw)			// 整列表示
{
	int i;
	if (xpw->flags&XPF_Minus) {					// 左揃え				"+12.34    "
		if (xpw->kigou) {						// 記号(-/+/SP/なし)
			XPutC(xpw, xpw->kigou);
			xpw->width--;
		}
		for (i=0; i<xpw->bufn; i++) {			// 文字部
			if (xpw->gyaku) {					// 逆並び
				XPutC(xpw, xpw->buf[xpw->bufn-i-1]);
			} else {							// 正並び
				XPutC(xpw, xpw->buf[i]);
			}
			xpw->width--;
		}
		while(xpw->width>0) {					// フィールド幅に足りない分を追加
			XPutC(xpw, ' ');
			xpw->width--;
		}
	} else {									// 右揃え
		if (xpw->flags&XPF_Zero) {				// '0'で埋める右揃え	"+000012.34"
			if (xpw->kigou) {					// 記号
				XPutC(xpw, xpw->kigou);
				xpw->width--;
			}
			while(xpw->width>xpw->bufn) {		// フィールド幅に足りない分'0'挿入
				XPutC(xpw, '0');				// '0' で埋める
				xpw->width--;
			}
		} else {								// ' 'で埋める右揃え	"    +12.34"
			if (xpw->kigou) {					// 記号分を先に引く
				xpw->width--;
			}
			while(xpw->width>xpw->bufn) {		// フィールド幅に足りない分' '挿入
				XPutC(xpw, ' ');				// ' ' で埋める
				xpw->width--;
			}
			if (xpw->kigou) {					// 記号(-/+/SP/なし)
				XPutC(xpw, xpw->kigou);			// 記号
			}
		}
		while(xpw->width>xpw->bufn) {			// フィールド幅に足りない分を挿入
			if (xpw->flags&XPF_Zero) {	
				XPutC(xpw, '0');				// '0' で埋める
			} else {
				XPutC(xpw, ' ');				// ' ' で埋める
			}
			xpw->width--;
		}
		for (i=0; i<xpw->bufn; i++) {			// 文字部
			if (xpw->gyaku) {					// 逆並び
				XPutC(xpw, xpw->buf[xpw->bufn-i-1]);
			} else {							// 正並び
				XPutC(xpw, xpw->buf[i]);
			}
		}
	}
}
static int VXprintf(T_XPW *xpw, const char *fmt, va_list vlst)	// 文字列に変換
{
	int c;
	while(*fmt) {
		c = *fmt++;				// fmt++
		if (c!='%') {			// 非書式付処理のときは
			XPutC(xpw, c);		// そのまま出力
			continue;
		}
		if (*fmt=='%') {		// "%%"なら
			XPutC(xpw, '%');	// %を出力
			fmt++;				// fmt++
			continue;
		}
		xpw->flags = 0;			// 書式フラグ(XPF_xxx)
		xpw->width = -1;		// フィールド幅
		xpw->pwidth = -1;		// 精度幅(小数点以下の桁数)
		while(*fmt) {			// フラグ類処理
			c = *fmt++;			// fmt++
			switch(c) {			// <フラグ類>
			case '#':	xpw->flags |= XPF_Sharp;	continue;	// #
			case '0':	xpw->flags |= XPF_Zero; 	continue;	// 0
			case '-':	xpw->flags |= XPF_Minus;	continue;	// -
			case ' ':	xpw->flags |= XPF_Space;	continue;	// SP(1個の半角スペース)
			case '+':	xpw->flags |= XPF_Plus; 	continue;	// +
			case 'h':	xpw->flags |= XPF_h;		continue;	// h 
			case 'l':	xpw->flags |= XPF_l;		continue;	// l
			}
			if (gg_isdigit(c)) {	// <フィールド幅>
				xpw->width = c-'0';
				while(gg_isdigit(*fmt)) {
					xpw->width = xpw->width*10+(*fmt++ - '0');
				}
				continue;		// *fmtは十進文字でない
			}
			if (c=='.') {		// <精度(小数点幅)>
				xpw->pwidth = 0;
				while(gg_isdigit(*fmt)) {
					xpw->pwidth = xpw->pwidth*10+(*fmt++ - '0');
				}
				continue;		// *fmtは十進文字でない
			}
			switch(c) {			// <変換指定子>
			case 'd':			// 十進変換
			case 'i':
				if (xpw->flags&XPF_l) {			// 'l'
					XCnvInt(xpw, va_arg(vlst, long));					// 十進変換(long)
				} else if (xpw->flags&XPF_h) {	// 'h'
					XCnvInt(xpw, va_arg(vlst, short));					// 十進変換(short)
				} else {						// 通常
					XCnvInt(xpw, va_arg(vlst, int));					// 十進変換(int)
				}
				XPrintDFS(xpw);					// 整列表示
				break;
			case 'u':			// uint/HEX/OCT/BIN変換
			case 'x':
			case 'X':
			case 'o':
			case 'b':
				if (xpw->flags&XPF_l) {			// 'l'
					XCnvXOB(xpw, va_arg(vlst, unsigned long), c);		// uint/HEX/OCT/BIN変換(unsigned long)
				} else if (xpw->flags&XPF_h) {	// 'h'
					XCnvXOB(xpw, va_arg(vlst, unsigned short), c);		// uint/HEX/OCT/BIN変換(unsigned short)
				} else {						// 通常
					XCnvXOB(xpw, va_arg(vlst, unsigned int), c);		// uint/HEX/OCT/BIN変換(unsigned int)
				}
				XPrintDFS(xpw);					// 整列表示
				break;
			case 'f':			// 浮動小数点変換
				if (xpw->flags&XPF_l) {			// 'l'
					XCnvF(xpw, va_arg(vlst, double));					// 浮動小数点変換(double)
				} else if (xpw->flags&XPF_h) {	// 'h'
					XCnvF(xpw, va_arg(vlst, float));					// 浮動小数点変換(float)
				} else {						// 通常
					XCnvF(xpw, va_arg(vlst, double));					// 浮動小数点変換(double)
				}
				XPrintDFS(xpw);					// 整列表示
				break;
		//	case 'e':			// E形式は非対応
		//		break;
			case 'c':			// １文字変換
				XCnvC(xpw, va_arg(vlst, char));		// １文字変換
				XPrintDFS(xpw);							// 整列表示
				break;
			case 's':			// 文字列変換
				XCnvStr(xpw, va_arg(vlst,char *));	// 文字列変換
				XPrintDFS(xpw);							// 整列表示
				break;
			default:
				XPutC(xpw, c);	// 該当しない文字の場合はそのまま出力して抜ける
				break;
			}
			break;				// 書式変換後は抜ける
		}
	}
	return 0;
}

//------------------
//	書式付きprintツール
//------------------
//	stdio.h関連のうち「書式付きprint」関連をメインに実装

int gg_vxprintf(int (*putc)(int), const char *fmt, va_list vlst)		// putc()指定でvprintf()
{
	int sts = 0;
	T_XPW	XPW;
	if (!putc) putc=gg_std_putc;	// 出力関数(putc)=NULLなら標準出力をセット
	XPW.xput = putc;				// 出力バッファ:出力関数をセット
	XPW.xputmax = 0;				// 出力バッファサイズ:0=出力関数使用
	XPW.xputn = 0;					// 書込みインデックス
	sts = VXprintf(&XPW, fmt, vlst);
	return sts;
}

int gg_xprintf(int (*putc)(int), const char *fmt, ...)				// putc()指定でprintf()
{
	int sts = 0;
	va_list vlst;
	va_start(vlst, fmt);
	sts = gg_vxprintf(putc, fmt, vlst);		// putc()へ出力
	va_end(vlst);
	return sts;
}

int gg_printf(const char *fmt, ...)									// 標準出力でprintf()
{
	int sts = 0;
	va_list vlst;
	va_start(vlst, fmt);
	sts = gg_vxprintf(NULL, fmt, vlst);		// NULL=標準出力へ出力
	va_end(vlst);
	return sts;
}

int gg_vsprintf(char *buf, const char *fmt, va_list vlst)			// buf[]指定でvprintf()
{
	int sts = 0;
	T_XPW	XPW;
//	(char *)XPW.xput = buf;		// 出力バッファ:出力先アドレスをセット
	XPW.xbuf = buf;		// 上記でダメだったとき用		2022.04.03 M.Kogan
	XPW.xputmax = gg_sbufmax;		// 出力バッファサイズ:バッファサイズ(バイト)
	XPW.xputn = 0;					// 書込みインデックス
	sts = VXprintf(&XPW, fmt, vlst);
	buf[XPW.xputn] = 0;
	return sts;
}

int gg_sprintf(char *buf, const char *fmt, ...)					// buf[]指定でprintf()
{
	int sts = 0;
	va_list vlst;
	va_start(vlst, fmt);
	sts = gg_vsprintf(buf, fmt, vlst);		// buf[]へ出力
	va_end(vlst);
	return sts;
}

//
//	日時表示
//
int gg_xprintDATE(int (*putc)(int), gg_time_t x)		// "YY/MM/DD"
{
	GGT_TIME t;
	gg_tim_tt2dt(x, &t);
	return gg_xprintf(putc, "%02d/%02d/%02d", t.tYY, t.tMM, t.tDD);
}
int gg_xprintTIME(int (*putc)(int), gg_time_t x)		// "hh:mm:ss"
{
	GGT_TIME t;
	gg_tim_tt2dt(x, &t);
	return gg_xprintf(putc, "%02d:%02d:%02d", t.thh, t.tmm, t.tss);
}
int gg_xprintDT(int (*putc)(int), gg_time_t x)			// "YY/MM/DD hh:mm:ss"
{
	gg_xprintDATE(putc, x);
	gg_xprintf(putc, " ");
	gg_xprintTIME(putc, x);
	return 0;
}
int gg_xprintDTs(int (*putc)(int), gg_time_t x)			// "YY/MM/DD hh:mm:ss " 最後に空白
{
	gg_xprintDATE(putc, x);
	gg_xprintf(putc, " ");
	gg_xprintTIME(putc, x);
	gg_xprintf(putc, " ");
	return 0;
}

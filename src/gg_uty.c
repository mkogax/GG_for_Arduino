/*
	gg_uty		GGツール 一般ユーティリティ

	2022.01.25	M.Kogan	初版開発開始
*/

#include "ggbase.h"



//--------------------------------------------------------------------------
//
// 一般ユーティリティ(gg_uty.c)		2022.01.25 M.Kogan
//
//--------------------------------------------------------------------------

static int null_putc(int c)					// 標準出力関数(デフォルト,何もしない)
{
	return 0;
}
static int null_getc(void)					// 標準入力関数(デフォルト,何も返さない)
{
	return -1;
}
int (*gg_std_putc)(int) = null_putc;		// 標準出力関数へのポインタ
int (*gg_std_getc)(void) = null_getc;      	// 標準入力関数へのポインタ(-1=入力なし)

int gg_GetC(void)				// 1文字入力(-1=入力なし)
{
	return gg_std_getc();		// 1文字入力(-1=入力なし)
}

int gg_PutC(int c)				// 1文字出力
{
	return gg_std_putc(c);		// 1文字出力
}

int gg_PreDetection(int c)		// ESC/Break事前検出(GGの標準入力バッファに入れるときチェック)
{
	switch(c) {
	case 0x1b:					// ESC検出
	case 0x03:					// Break検出
		GG_CON.PreDetection = c;
		break;
	}
	return GG_CON.PreDetection;	// 現在の事前検出状態を返す（とりあえず）
}

int gg_PutS(const char *s)		// 文字列出力
{
	int sts = 0;
	while(*s) {
		sts = gg_PutC(*s++);
		if (sts) break;
	}
	return sts;
}

//------------------
//	文字入力ツール
//------------------
int gg_sbufmax = 256;						// 出力文字列バッファ最大値(デフォルト=256)

static char *getint(char *p, int32_t *d)	// DEC取得
{
	int32_t c;
	*d = 0;
	while(*p) {
		c = *p;
		if ((c>='0')&&(c<='9')) {
			*d = *d * 10 + c - '0';
			p++;
		} else {
			break;
		}
	}
	return p;
}
static char *gethex(char *p, int32_t *d)	// HEX取得
{
	int32_t c;
	*d = 0;
	while(*p) {
		c = gg_toupper(*p);
		if ((c>='0')&&(c<='9')) {
			*d = (*d<<4) + c - '0';
			p++;
		} else if ((c>='A')&&(c<='F')) {
			*d = (*d<<4) + c - 'A' + 10;
			p++;
		} else {
			break;
		}
	}
	return p;
}
static char *getbit(char *p, int32_t *d)	// BIT取得
{
	int32_t c;
	*d = 0;
	while(*p) {
		c = *p;
		if ((c>='0')&&(c<='1')) {
			*d = (*d<<1) + c - '0';
			p++;
		} else {
			break;
		}
	}
	return p;
}
static char *getoct(char *p, int32_t *d)	// OCT取得
{
	int32_t c;
	*d = 0;
	while(*p) {
		c = *p;
		if ((c>='0')&&(c<='7')) {
			*d = (*d<<3) + c - '0';
			p++;
		} else {
			break;
		}
	}
	return p;
}
static char *getsect(char *p, int32_t *d);	// 1文節取得
static char *getelm(char *p, int32_t *d)	// 1エレメント取得
{
	char c;
	*d = 0;
	if (!*p) return p;
	c = *p;
	if (c==')') return p;
	if (c=='(') {
		p = getsect(++p, d);
		if (*p==')') {
			p++;
		} else {
			gg_printf(") is not found.\n");
		}
		return p;
	}
	if (c=='-') {				// -xxxx
		p = getelm(++p, d);
		*d = -*d;
		return p;
	}
	if (c=='~') {				// ~xxxx
		p = getelm(++p, d);
		*d = ~*d;
		return p;
	}
	if (c=='$') {				// $FFFF	Hex
		p = gethex(++p, d);
		return p;
	}
	if (c=='0') {
		switch(p[1]) {
		case 'x':				// 0xFFFF	Hex
		case 'X':
			p = gethex(&p[2], d);
			return p;
		case 'b':				// 0b1111	Bit
		case 'B':
			p = getbit(&p[2], d);
			return p;
		case 'o':				// 0o7777	Oct
		case 'O':
			p = getoct(&p[2], d);
			return p;
		default:				// 0777		Oct
			p = getoct(&p[1], d);
			return p;
		}
	}
	if ((c>='0')&&(c<='9')) {	// 9999		Dec
		p = getint(p, d);
		return p;
	}
#if 0
	GGT_VAR *vp = KMonVar_SearchX(p);	// 次に演算子があるかもしれないので部分一致で
	if (vp) {							// メモリ変数
		*d = (long)vp->addr;
		p += Kstrlen(vp->name);
		return p;
	}
#endif
	return p;
}
static char *getsect(char *p, int32_t *w)		// 1文節取得
{
	int32_t x, d;
	p = getelm(p, &x);
	while(*p) {
		switch(*p++) {
		case '+':						// A+B
			p = getelm(p, &d);
			x += d;
			break;
		case '-':						// A-B
			p = getelm(p, &d);
			x -= d;
			break;
		case '*':						// A*B
			p = getelm(p, &d);
			x *= d;
			break;
		case '/':						// A/B
			p = getelm(p, &d);
			if (d) x /= d;
			break;
		case '&':						// A&B
			p = getelm(p, &d);
			x &= d;
			break;
		case '|':						// A|B
			p = getelm(p, &d);
			x |= d;
			break;
		case '^':						// A^B
			p = getelm(p, &d);
			x ^= d;
			break;
		case '>':						// A>>B
			if (*p=='>') {
				p = getelm(&p[1], &d);
				x >>= d;
			} else {					// エラー
				p--;
				goto owari;
			}
			break;
		case '<':						// A<<B
			if (*p=='<') {
				p = getelm(&p[1], &d);
				x <<= d;
			} else {					// エラー
				p--;
				goto owari;
			}
			break;
		default:						// エラー
			p--;
			goto owari;
		}
	}
owari:
	*w = x;
	return p;
}
int32_t gg_asc2int(char *p)			// 文字列から数値
{
	int32_t x;
	char *pp;
	pp = getsect(p, &x);
	if (*pp) {
		gg_printf("Syntax error --> %s\n", p);
	}
	return x;
}

int32_t gg_asc2int_noerr(char *p)	// 文字列から数値へ(残ってもエラーなし)
{
	int32_t x;
	getsect(p, &x);
	return x;
}

float gg_asc2float(char *p)			// 文字列から数値(float)へ
{
	return atof(p);
}


//------------------
//	文字出力ツール
//------------------

int gg_strlen(const char *buf)	// 文字列(ASCIZ)の長さを返す
{
	int n = 0;
	if (!buf) return 0;			// バッファがNULLでも0を返す
	while(*buf++) n++;
	return n;
}
int gg_isdigit(int c)			// 文字が10進数字であるとき0を返す
{
	if (c<'0') return 0;
	if (c>'9') return 0;
	return 1;
}

char* gg_strcpy(char* a, const char* b)				// 文字列コピー
{
	char *p = a;
	while(*b) *a++ = *b++;
	*a++ = 0;
	return p;
}

int gg_strcmp(const char* a, const char* b)			// 文字列比較
{
	char x, y;
	while((*a)||(*b)) {
		x = *a++;
		y = *b++;
		if (x==y) continue;
		if (!x) return -1;
		if (!y) return 1;
		if (x<y) return -1;
		if (x>y) return 1;
	}
	return 0;
}

int gg_strcmpx(const char* a, const char* b)		// 文字列比較(大文字小文字無視)
{
	char x, y;
	while((*a)||(*b)) {
		x = gg_toupper(*a++);
		y = gg_toupper(*b++);
		if (x==y) continue;
		if (!x) return -2;			// -2=前文字列の終わりまで一致
		if (!y) return 2;			//  2=後文字列の終わりまで一致
		if (x<y) return -1;			// -1=前文字列が小さい
		if (x>y) return 1;			//  1=前文字列が大きい
	}
	return 0;						//  0=一致
}

int gg_wccmp(const char* wc, const char* b)			// ワイルドカード文字列比(wcは*,?を含む,0=一致)
{
	char c;
	while(*wc) {
		c = *wc++;
		if (c=='*') {							// '*'なら
			while(*b) {
				if (!gg_wccmp(wc, b)) return 0;	// bについて短くしながら最後まで一致するかどうか判定
				b++;
			}
			continue;							// wcが終わっているかの判定をメインループに任せる
		} else if (c=='?') {					// '?'なら
			if (!*b) return -1;					// bが無いならNG
			++b;								// bがまだあるならbを一つ減らして判定
			continue;
		}
		if (c!=*b) return -1;					// 違ってればNG
		if (*b) b++;							// bがあるなら次の文字
	}
	if (*b) return -1;							// まだbがあるならNG
	return 0;
}

int gg_wccmpx(const char* wc, const char* b)		// ワイルドカード文字列比(wcは*,?を含む,0=一致)(大文字小文字無視)
{
	char c;
	while(*wc) {
		c = gg_toupper(*wc++);
		if (c=='*') {							// '*'なら
			while(*b) {
				if (!gg_wccmpx(wc, b)) return 0;// bについて短くしながら最後まで一致するかどうか判定
				b++;
			}
			continue;							// wcが終わっているかの判定をメインループに任せる
		} else if (c=='?') {					// '?'なら
			if (!*b) return -1;					// bが無いならNG
			++b;								// bがまだあるならbを一つ減らして判定
			continue;
		}
		if (c!=gg_toupper(*b)) return -1;		// 違ってればNG
		if (*b) b++;							// bがあるなら次の文字
	}
	if (*b) return -1;							// まだbがあるならNG
	return 0;
}

void *gg_memcpy(void *buf1, const void *buf2, unsigned int n)	// メモリコピー
{
	char *p = (char *)buf1;
	const char *q = (const char *)buf2;
	while(n-- > 0) *p++ = *q++;
	return buf1;
}

void *gg_memset(void *buf, int ch, unsigned int n)				// メモリセット
{
	char *p = (char *)buf;
	while(n-- > 0) *p++ = ch;
	return buf;
}

int gg_toupper(int c)								// 大文字化
{
	if ((c>='a')&&(c<='z')) return c-'a'+'A';
	return c;
}

int gg_tolower(int c)								// 小文字化
{
	if ((c>='A')&&(c<='Z')) return c-'A'+'a';
	return c;
}

char *gg_strcat(char* a, const char* b, int max)	// 文字列追加
{
	int len;
	len = gg_strlen(a);
	max--;
	if (len>=max) return a;
	while(*b) {
		if (len>=max) break;
		a[len++] = *b++;
	}
	a[len] = 0;
	return a;
}


//------------------
//	ミニバッファ
//------------------
//	バイト単位のFIFOバッファ

int16_t	gg_mbuf_init(GGT_MBUF *mbuf, void* buf, int max)	// ミニバッファ 初期化
{
	GG_DI_BEGIN();		// 割禁開始
	mbuf->max 	= max;	// 最大格納数(バイト)
	mbuf->n 	= 0;	// 現在格納数(バイト)
	mbuf->wix 	= 0;	// 次回書込みindex
	mbuf->rix 	= 0;	// 次回読出しindex
	mbuf->buf	= buf;	// バッファ(バイト)へのポインタ
	GG_DI_END();		// 割禁終了
	return 0;
}
int16_t	gg_mbuf_put(GGT_MBUF *mbuf, int16_t d)				// ミニバッファ 追加
{
	int sts = 0;
	GG_DI_BEGIN();		// 割禁開始
	if (!mbuf) {
		sts = -2;		// mbufエラー(-2)
	} else if (mbuf->n >= mbuf->max) {
		sts = -1;		// 容量オーバー(-1)
	} else {
		mbuf->n++;
		mbuf->buf[mbuf->wix++] = d;	// 追加
		if (mbuf->wix >= mbuf->max) mbuf->wix = 0;	// indexはループ使用
		sts = 0;		// 追加成功(0)
	}
	GG_DI_END();		// 割禁終了
	return sts;
}
int16_t	gg_mbuf_get(GGT_MBUF *mbuf)						// ミニバッファ 取り出し(-1=なし)
{
	int sts = 0;
	GG_DI_BEGIN();		// 割禁開始
	if (!mbuf) {
		sts = -2;		// mbufエラー(-2)
	} else if (mbuf->n <= 0) {
		sts = -1;		// データなし(-1)
	} else {
		mbuf->n--;
		sts = mbuf->buf[mbuf->rix++];	// 取り出し(符号なしなので0x00..0xff)
		if (mbuf->rix >= mbuf->max) mbuf->rix = 0;	// indexはループ使用
	}
	GG_DI_END();		// 割禁終了
	return sts;
}
int16_t	gg_mbuf_peek(GGT_MBUF *mbuf, int16_t ix)			// ミニバッファ のぞき見(-1=なし)
{
	int sts = 0;
	GG_DI_BEGIN();		// 割禁開始
	if (!mbuf) {
		sts = -2;		// mbufエラー(-2)
	} else if ((ix<0)||(mbuf->n <= ix)) {	// ixが格納内範囲内(0..n-1)かチェック
		sts = -1;		// データなし(-1)
	} else {
		mbuf->n--;
		sts = mbuf->buf[mbuf->rix++];	// 取り出し(符号なしなので0x00..0xff)
		if (mbuf->rix >= mbuf->max) mbuf->rix = 0;	// indexはループ使用
	}
	GG_DI_END();		// 割禁終了
	return sts;
}


//------------------
//	日時変換(2000年～2099年)
//	本来はgg_time_tが負数になる2067年から禁止していたが、負数も認めることで2099年まで対応する		(2099年対応)	2022.02.11 M.Kogan
//------------------
//	time_t(long)<-->YYYY,MM,DD,hh,mm,ss(int)<-->YY,MM,DD,hh,mm,ss(BCD)
#define SECDAY		(86400L)		// 3600*24		1日の秒数
#define DAY4YEAR	(1461L)			// 366+365*3	4年間の日数
#define DAY1M		(31)									// 1月までの日数
#define DAY2M		(31+28)									// 2月までの日数
#define DAY3M		(31+28+31)								// 3月までの日数
#define DAY4M		(31+28+31+30)							// 4月までの日数
#define DAY5M		(31+28+31+30+31)						// 5月までの日数
#define DAY6M		(31+28+31+30+31+30)						// 6月までの日数
#define DAY7M		(31+28+31+30+31+30+31)					// 7月までの日数
#define DAY8M		(31+28+31+30+31+30+31+31)				// 8月までの日数
#define DAY9M		(31+28+31+30+31+30+31+31+30)			// 9月までの日数
#define DAY10M		(31+28+31+30+31+30+31+31+30+31)			// 10月までの日数
#define DAY11M		(31+28+31+30+31+30+31+31+30+31+30)		// 11月までの日数
#define DAY12M		(31+28+31+30+31+30+31+31+30+31+30+31)	// 12月までの日数
#define SEC60YEAR	(15L*DAY4YEAR*SECDAY)					// 60年の秒数			(2099年対応)	2022.02.11 M.Kogan
#define SEC100YEAR	(25LL*DAY4YEAR*SECDAY)					// 100年の秒数(負数)	(2099年対応)	2022.02.11 M.Kogan
static int8_t bcd2bin(uint8_t h)			// BCD-->bin
{
	return (h>>4)*10+(h&0xf);
}
static uint8_t bin2bcd(int8_t t)			// bin-->BCD
{
	return ((t/10)<<4)+(t%10);
}
int gg_tim_bcd2dt(GGT_BCDTIME *b, GGT_TIME *t)			// BCD日時-->日時 変換
{
	t->tYY = bcd2bin(b->bYY);
	t->tMM = bcd2bin(b->bMM);
	t->tDD = bcd2bin(b->bDD);
	t->thh = bcd2bin(b->bhh);
	t->tmm = bcd2bin(b->bmm);
	t->tss = bcd2bin(b->bss);
	return 0;
}
int gg_tim_dt2bcd(GGT_TIME *t, GGT_BCDTIME *b)			// 日時-->BCD日時 変換
{
	b->bYY = bin2bcd(t->tYY);
	b->bMM = bin2bcd(t->tMM);
	b->bDD = bin2bcd(t->tDD);
	b->bhh = bin2bcd(t->thh);
	b->bmm = bin2bcd(t->tmm);
	b->bss = bin2bcd(t->tss);
	return 0;
}
gg_time_t gg_tim_dt2tt(GGT_TIME *t)						// 日時-->総秒(time_t)
{
	long td;
	int y4;
	long td60 = 0L;			// 2060年以上なら60年の秒数が入る	(2099年対応)	2022.02.11 M.Kogan
	int8_t YY = t->tYY;		// YYを60年引くときの対策(元データを壊さないように)	2022.02.16 M.Kogan

//	if (gg_tim_checkdt(t)) return 0L;	// チェックエラーなら2000年1月1日		2022.02.11 M.Kogan
	gg_tim_repairdt(t);					// 日時の補正(それなりに修正してしまう)	2022.02.12 M.Kogan

	if (YY>=60) {			// 2060年以上なら60年引いて計算		(2099年対応)	2022.02.11 M.Kogan
		YY -= 60;
		td60 = SEC60YEAR;	// 60年の秒数						(2099年対応)	2022.02.11 M.Kogan
	}

	y4 = YY%4;
	td = (YY/4)*DAY4YEAR;					// 4年区切りの日数
	if (y4) td += 365L*y4 + 1;				// 4年区切り以降(1,2,3年目)の日数+通過した先頭年のうるう日
	switch(t->tMM) {						// 月ごとの開始日数
											// 1月=0日
	case 2:	td += DAY1M;	break;			// 2月=31日
	case 3:	td += DAY2M;	break;			// 3月=(31+28)日
	case 4:	td += DAY3M;	break;
	case 5:	td += DAY4M;	break;
	case 6:	td += DAY5M;	break;
	case 7:	td += DAY6M;	break;
	case 8:	td += DAY7M;	break;
	case 9:	td += DAY8M;	break;
	case 10:td += DAY9M;	break;
	case 11:td += DAY10M;	break;
	case 12:td += DAY11M;	break;
	}
	if ((!y4)&&(t->tMM>2)) td += 1;			// うるう年の3月以降は+1
	td += t->tDD;							// 日を加算
	td--;									// 1月1日をゼロとする
	td = td*SECDAY;							// 日数を秒に
	td+= t->thh*3600L+t->tmm*60L+t->tss;	// 時分秒を秒に(long演算指定)して加算

	td += td60;				// 2067年以降は負数だが逆変換は可能	(2099年対応)	2022.02.11 M.Kogan

	return td;
}
gg_time_t gg_tim_YYMMDD2tt(int YY, int MM, int DD)		// 年月日-->総秒(time_t)
{
	GGT_TIME T;
	T.tYY = YY;
	T.tMM = MM;
	T.tDD = DD;
	T.thh = 0;
	T.tmm = 0;
	T.tss = 0;
	return gg_tim_dt2tt(&T);
}
gg_time_t gg_tim_hhmmss2tt(int hh, int mm, int ss)		// 時分秒-->総秒(time_t)
{
	GGT_TIME T;
	T.tYY = 0;
	T.tMM = 1;
	T.tDD = 1;
	T.thh = hh;
	T.tmm = mm;
	T.tss = ss;
	return gg_tim_dt2tt(&T);
}
int gg_tim_tt2dt(gg_time_t x, GGT_TIME *t)				// 総秒(time_t)-->日時
{
	long td;
	int y4;
	int sts = 0;
	int y60 = 0;			// 2060年以上なら60年の年数が入る	(2099年対応)	2022.02.11 M.Kogan

	if (x<0L) {								// 負数でも2099年まで使う
		int64_t xll = (uint32_t)x;			// 日時を符号なし32bitとみなして64bit化				2022.02.12 M.Kogan
		if (xll>=SEC100YEAR) {				// 2100年より未来な時限定(負数でも2099年まで有効)		(2099年対応)	2022.02.11 M.Kogan
			xll -= SEC100YEAR;				// 64bit時刻で100年戻して(200年以上戻すことはない)
			x = (int32_t)xll;				// 32bit時刻にセット
			sts = -1;
		}
	}

	if ((x<0)||(x>=SEC60YEAR)) {			// 2060年以降は60年引いて演算		(2099年対応)	2022.02.11 M.Kogan
		x -= SEC60YEAR;
		y60 = 60;							// 60年の年数						(2099年対応)	2022.02.11 M.Kogan
	}

	td = x/SECDAY;							// 総日数
	t->tYY = (int8_t)((td/DAY4YEAR)*4);		// 4年区切りでの年

	td %= DAY4YEAR;
	if (td<366) {							// 1月1日(0)から数えてうるう年の12月31日(365)以内なら
		y4 = 0;
	} else {
		td--;								// うるう年分を1日引いて
		y4 = td/365;						// y4=1..3
		td = td%365;						// td=0..364
	}
	t->tYY += y4;							// 年確定(4年区切り+0..3)
	t->tYY += y60;							// 2060年以降は60年加算				(2099年対応)	2022.02.11 M.Kogan
	if ((!y4)&&(td==(31+29-1))) {			// うるう年の2月29日だけ例外
		t->tMM = 2;
		t->tDD = 29;
	} else {								// y4=1..3,td=0..364 か y4=0,td=0..365
		if ((!y4)&&(td>(31+29-1))) td--;								// うるう年の3月〜12月は1日減らす(2月29日がなかったことにする)
		if 		(td<DAY1M)	{ t->tMM=1; t->tDD=(int8_t)(td       +1); }	// 月ごとの開始日数で逆算
		else if (td<DAY2M)  { t->tMM=2; t->tDD=(int8_t)(td-DAY1M +1); }
		else if (td<DAY3M)  { t->tMM=3; t->tDD=(int8_t)(td-DAY2M +1); }
		else if (td<DAY4M)  { t->tMM=4; t->tDD=(int8_t)(td-DAY3M +1); }
		else if (td<DAY5M)  { t->tMM=5; t->tDD=(int8_t)(td-DAY4M +1); }
		else if (td<DAY6M)  { t->tMM=6; t->tDD=(int8_t)(td-DAY5M +1); }
		else if (td<DAY7M)  { t->tMM=7; t->tDD=(int8_t)(td-DAY6M +1); }
		else if (td<DAY8M)  { t->tMM=8; t->tDD=(int8_t)(td-DAY7M +1); }
		else if (td<DAY9M)  { t->tMM=9; t->tDD=(int8_t)(td-DAY8M +1); }
		else if (td<DAY10M) { t->tMM=10;t->tDD=(int8_t)(td-DAY9M +1); }
		else if (td<DAY11M) { t->tMM=11;t->tDD=(int8_t)(td-DAY10M+1); }
		else				{ t->tMM=12;t->tDD=(int8_t)(td-DAY11M+1); }
	}
	x = x%SECDAY;							// 時分秒
	t->tss = (int8_t)(x%60L);	x /= 60L;
	t->tmm = (int8_t)(x%60L);	x /= 60L;
	t->thh = (int8_t)x;

	return sts;
}
int gg_tim_checkdt(GGT_TIME *t)				// 日時のチェック(0=OK)
{
//	if ((t->tYY<0)||(t->tYY>67)) return -1;
	if ((t->tYY<0)||(t->tYY>99)) return -1;	//	2099年まで有効				(2099年対応)		2022.02.11 M.Kogan
	switch(t->tMM) {
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		if (t->tDD>31) return -1;
		break;
	case 2:
		if (t->tYY%4) {	// 非うるう年(4で割り切れない年)
			if (t->tDD>28) return -1;
		} else {		// うるう年(4で割り切れる年)
			if (t->tDD>29) return -1;
		}
		break;
	case 4:
	case 6:
	case 9:
	case 11:
		if (t->tDD>30) return -1;
		break;
	default:
		return -1;
	}
	if (t->tDD<1) return -1;
	if ((t->thh<0)||(t->thh>23)) return -1;
	if ((t->tmm<0)||(t->tmm>59)) return -1;
	if ((t->tss<0)||(t->tss>59)) return -1;
	return 0;
}

int gg_tim_repairdt(GGT_TIME *t)						// 日時の補正(それなりに修正してしまう)
{

	// 個別範囲制限
	if (t->tYY<0)  t->tYY = 0;
	if (t->tYY>99) t->tYY = 99;		//	2099年まで有効				(2099年対応)		2022.02.11 M.Kogan
	if (t->tMM<1)  t->tMM = 1;
	if (t->tMM>12) t->tMM = 12;
	if (t->tDD<1)  t->tDD = 1;
	if (t->tDD>31) t->tDD = 31;
	if (t->thh<0)  t->thh = 0;
	if (t->thh>23) t->thh = 23;
	if (t->tmm<0)  t->tmm = 0;
	if (t->tmm>59) t->tmm = 59;
	if (t->tss<0)  t->tss = 0;
	if (t->tss>59) t->tss = 59;

	// 日数制限
	switch(t->tMM) {
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		if (t->tDD>31) t->tDD = 31;
		break;
	case 2:
		if (t->tYY%4) {	// 非うるう年(4で割り切れない年)
			if (t->tDD>28) t->tDD = 28;
		} else {		// うるう年(4で割り切れる年)
			if (t->tDD>29) t->tDD = 29;
		}
		break;
	case 4:
	case 6:
	case 9:
	case 11:
		if (t->tDD>30) t->tDD = 30;
		break;
	}

	return 0;
}

int gg_tim_tt2week(gg_time_t x)							// 総秒(time_t)-->曜日(0=日,1=月..6=土)	2000年1月1日(土)から導出
{
	int w;
	int64_t xll = (uint32_t)x;			// 日時を符号なし32bitとみなして64bit化				2022.02.12 M.Kogan
	if (xll>=SEC100YEAR) {				// 2100年より未来な時限定(負数でも2099年まで有効)		(2099年対応)	2022.02.11 M.Kogan
		xll -= SEC100YEAR;				// 64bit時刻で100年戻して(200年以上戻すことはない)
		x = (int32_t)xll;				// 32bit時刻にセット
	}
	x = (int32_t)(xll/(int64_t)SECDAY);	// 日数に変換してから32bitに戻す			2022.02.16 M.Kogan
	w = x%7L;
	return (w+6)%7;						// x=0(2000年1月1日)は土(6)なので6を足してから余りを取りなおす
}

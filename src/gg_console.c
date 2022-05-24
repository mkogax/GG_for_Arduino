/*
	gg_console		GGツール コンソールツール

	2022.01.25	M.Kogan	初版開発開始
*/

#include "ggbase.h"

GGT_CON GG_CON;				// GG_CONワークの実体

//--------------------------------------------------------------------------
//
// デバッグモニタ(console.c)		2022.01.25 M.Kogan
//
//--------------------------------------------------------------------------
int gg_con_MonInit(void)			// デバッグモニタ初期化
{
	gg_memset(&GG_CON, 0, sizeof(GG_CON));	// KMONワークのゼロクリア
#if 0
	gg_con_VarInit();						// 変数の初期化
#endif
	gg_con_CmdInit();						// コマンドの初期化
	return 0;
}

char *gg_con_prompt = ">";			// プロンプト文字列		2022.05.24 M.Kogan
int gg_con_MonRePrompt(void)		// プロンプト再表示
{
	int i;
	if (gg_con_prompt) gg_PutS(gg_con_prompt);	// プロンプト
	for (i=0; i<GG_CON.BufP; i++) {
		gg_PutC(GG_CON.Buf[i]);
	}
	return 0;
}

int gg_con_MonPrompt(void)			// プロンプト表示(内部で使用)
{
	GG_CON.BufP = 0;						// バッファクリア
	return gg_con_MonRePrompt();
}

int gg_con_Check(void)				// デバッグモニタチェック(0=実行無か実行成功,-999=EXIT要請)(gg_GetC()から1文字受付)
{
	return gg_con_MonCheck(gg_GetC());		// gg_GetC()から1文字受付
}

int gg_con_MonCheck(int c)			// デバッグモニタチェック(0=実行無か実行成功,-999=EXIT要請)(1文字受付)
{
	int sts = 0;
	if (c<=0) return 0;				// 文字が無いなら実行無しでそのまま戻る
	if (c=='\t') c=' ';				// タブは空白化
	switch(c) {
	case '\r':						// 改行ならば実行
	case '\n':						// LFでも実行
		gg_PutC('\n');				// エコーバック
		GG_CON.Buf[GG_CON.BufP] = 0;		// EOS記入
		sts = gg_con_MonExec(GG_CON.Buf);	// 実行
		gg_con_MonPrompt();					// 次のプロンプト表示とバッファクリア(EXITでもやる)
		return sts;					// 実行有りで戻る
	case '\b':						// BackSpaceならば1文字消去
		if (GG_CON.BufP>0) {
			gg_PutC('\b');			// 画面から1文字削除
			gg_PutC(' ');
			gg_PutC('\b');
			GG_CON.BufP--;			// バッファから1文字削除
		} else {					// バッファに消す文字がないときは警告音
			gg_PutC('\a');			// BELL
		}
		break;
	case 0x1b:						// ESCならば再表示
		gg_PutS("**ESC\n");
		gg_con_MonRePrompt();		// プロンプトとバッファ再表示
		break;
	case 0x3:						// ^Cならばキャンセル
		gg_PutS("**BREAK\n");
		gg_con_MonPrompt();			// バッファクリアしてプロンプト表示
		break;
	case 18:						// ^Rならば再表示(RePrompt)
		gg_PutS("**REPROMPT\n");
		gg_con_MonRePrompt();		// プロンプトとバッファ再表示
		break;
	default:						// それ以外はバッファ更新
		if ((GG_CON.BufP>=GG_CON_BUFMAX)||(c<' ')) {	// あふれるか制御文字なら
			gg_PutC('\a');								// BELL
		} else {										// それ以外は文字追加
			gg_PutC(c);									// エコーバック
			GG_CON.Buf[GG_CON.BufP++] = c;
		}
		break;
	}
	return 0;			// キー入力無しによる実行無し
}

int gg_con_MonExec(char *buf)		// コマンド処理(bufはarg解析で破壊される)
{
	int argc = gg_con_MonBuf2Arg(buf, GG_CON.Argv, GG_CON_ARGMAX);	// buf-->arg解析(return=argc)
	return gg_con_MonExecArg(argc, GG_CON.Argv);					// コマンド処理(arg)
}

int gg_con_MonBuf2ArgNC(char *p, char **argv, int argmax)	// buf-->arg解析(return=argc)(;コメント機能なし)
{
	int  argc = 0;						// argc(argv登録数)

	// arg解析
	while(argc<argmax) {				// argmaxまでargvを1つずつ抽出
		while(*p==' ') p++;				// 頭のスペースを省く
		if (!*p) break;					// おしまい
		argv[argc++] = p;				// 先頭をargv登録
		while ((*p!=' ')&&(*p)) p++;	// 文字列の続く間スキップ(空白あるいは終端まで)
		if (!*p) break;					// おしまい(終端)なら抜ける
		*p++ = 0;						// 終端でない(空白)ならargv文字列の終わり(\0)に書き換える(ASCIZ)
	}

	return argc;						// argcを返す
}

int gg_con_MonBuf2Arg(char *p, char **argv, int argmax)		// buf-->arg解析(return=argc)
{
	char *pp = p;	// p保存

	// ;以降(コメント)を排除（""などは考慮していない）
	while(*p) {
		if (*p==';') {
			*p = 0;
			break;
		}
		p++;
	}

	return gg_con_MonBuf2ArgNC(pp, argv, argmax);		// buf-->arg解析(return=argc)(;コメント機能なし)
}	

int gg_con_MonExecArg(int argc, char **argv)				// コマンド処理(arg)
{
	int i;

	if (!argc) return 0;					// コマンドなし(改行のみ)は成功とみなす

	// コマンド解析
	GGT_CMD *cp = gg_con_CmdSearch(argv[0]);	// コマンド検索
	if (cp) {									// 見つかれば
		return cp->func(argc, argv);			// コマンド関数実行
	}

	// 見つからないときのエラー表示
	for (i=0; i<argc; i++) {
		gg_printf("[%s]", argv[i]);
	}
	gg_PutS("\nIllegal command. (see help)\n");
	return -1;
}


//--------------------------------------------------------------------------
//
// コマンド定義リスト(gg_console.c)			2022.01.25 M.Kogan
//
//--------------------------------------------------------------------------

int gg_con_CmdInit(void)				// コマンド定義リストの初期化
{
	gg_memset(&GG_CON.cmd, 0, sizeof(GG_CON.cmd));	// ゼロクリア
	return 0;
}

int gg_con_CmdAdd(GGT_CMD *def)		// コマンド定義リストの追加
{
	// 空のとき
	if (!GG_CON.cmd.link) {		// 空の時は
		GG_CON.cmd.link = def;	// 先頭に登録
		def->next = NULL;		// 次=なし
		return 0;
	}

	// リストの最後に追加
	GGT_CMD *p = GG_CON.cmd.link;
	while(1) {
		if (p==def) return -1;	// 登録済みの同じアドレス(同じ定義)なら無効
		if (!p->next) {			// 最後なら追加
			p->next = def;		// 最後の次に追加
			def->next = NULL;	// 次=なし
			break;
		}
		p = p->next;			// (上でチェック済みなのでpはNULLにならない)
	}
	return 0;					// 登録OK(0)
}

int gg_con_CmdN(void)					// コマンド登録数
{
	int n = 0;
	GGT_CMD *p = GG_CON.cmd.link;
	while(p) {
		n++;
		p = p->next;
	}
	return n;
}

static void help1line(GGT_CMD *cp)		// 1行ヘルプ表示
{
	const char *help = cp->help;		// ヘルプ文字列
	if (!cp->func) {					// コマンド関数が登録されていていないものは区切り(mark off)
		gg_printf("%s\n", help);
	} else {							// コマンド関数が登録されていているもの(コマンド説明)
		if (*help=='!') help++;			// !をスキップ				2007.09.12 M.Kogan
		gg_printf("%12s %-24s : %s\n", cp->name, cp->arg, help);
	}
}
int	gg_con_CmdHelp(char *cmd)			// コマンドヘルプ
{
	GGT_CMD *cp = GG_CON.cmd.link;
	int flag = 0;
	while(cp) {
		if (cp->func) {					// コマンド関数が登録されていていないものは区切り(mark off)なのでスキップ
			if (!gg_wccmp(cmd, cp->name)) {
				help1line(cp);
				flag++;
			}
		}
		cp = cp->next;
	}
	if (!flag) {
		gg_printf("<%s> is not found.\n", cmd);
	}
	return flag;
}
int	gg_con_CmdHelpAll(void)				// コマンド一覧
{
	int n = 0;
	GGT_CMD *cp = GG_CON.cmd.link;
	GG_CON.PreDetection = 0;					// ESC/Break検出フラグをクリア（gg_PreDetection()でセット）
	while(cp) {
		if (*cp->help!='!') {					// ヘルプ文字列先頭に!のある行は一覧表示をスキップ
			help1line(cp);
			if (GG_CON.PreDetection) return -1;	// ESC(0x1b)かBreak(0x03)で中止
		}
		cp = cp->next;
	}
	return n;
}

GGT_CMD* gg_con_CmdSearch(char *name)		// コマンド名のサーチ(完全一致,登録情報のポインタを返す。NULL=エラー)
{
	GGT_CMD* p = GG_CON.cmd.link;
	while(p) {
		if (p->func) {						// コマンド関数が登録されていていないものは区切り(mark off)なのでスキップ
			int sts = gg_strcmpx(p->name, name);	// 文字列比較(大文字小文字無視)
			if (sts== 0) return p;					//  0=一致ならOK(一致した登録情報のポインタ)
		}
		p = p->next;						// 次
	}
	return NULL;							// 見つからない
}

GGT_CMD* gg_con_CmdSearchX(char *name)	// コマンド名のサーチ(nameは途中でも良い,登録情報のポインタを返す。NULL=エラー)
{
	GGT_CMD* p = GG_CON.cmd.link;
	while(p) {
		if (p->func) {						// コマンド関数が登録されていていないものは区切り(mark off)なのでスキップ
			int sts = gg_strcmpx(p->name, name);	// 文字列比較(大文字小文字無視)
			if (sts== 0) return p;				//  0=一致ならOK(一致した登録情報のポインタ)
			if (sts==-2) return p;				// -2=前文字列まで一致ならOK(一致した登録情報のポインタ)
		}
		p = p->next;						// 次
	}
	return NULL;							// 見つからない
}



//--------------------------------------------------------------------------
//
// コマンド登録(gg_console.c)			2022.01.27 M.Kogan
//
//--------------------------------------------------------------------------

static C_help(int argc, char **argv)
{
	int i;
	if (argc>1) {
		for (i=1; i<argc; i++) {				// コマンドラインに列挙した全コマンドについて
			gg_con_CmdHelp(argv[i]);			// コマンドヘルプ
		}
	} else {									// パラメータなしは一覧表示
		gg_con_CmdHelpAll();					// コマンド一覧
	}
	return 0;
}

static int C_quit(int argc, char**argv)			// 終了(EXITコード発行)
{
	if (argc==1) return GG_CON_EXIT;			// EXITコード(-999)
	gg_con_CmdHelp(argv[0]);
	return 0;
}

//
//	メモリダンプ系
//
static uint32_t maddr = 0;
static uint32_t malen = 256;
static int mdchar(void *buf, long len)
{
	long j;
	uint8_t *w = (uint8_t *)buf;
	uint8_t x;
	gg_PutC(' ');
	gg_PutC(' ');		// 2個空白	2022.02.02 M.Kogan
	for (j=0; j<16; j++) {
		if (j>=len) break;
		if (j==8) gg_PutC(' ');;
		x = w[j];
		if (x< ' ') x = '.';
		if (x> '~') x = '.';	// '~'含む	2022.02.02 M.Kogan
		gg_PutC(x);
		if (GG_CON.PreDetection) return -1;
	}
	gg_PutC('\n');
	return 0;
}

static int C_mdb(int argc, char**argv)						// メモリダンプ(byte)
{
	uint8_t buf[16];
	long i, j;
	GG_CON.PreDetection = 0;
	if (argc>1) maddr = gg_asc2int(argv[1]);
	if (argc>2) malen = gg_asc2int(argv[2]);
	for (i=0; i<malen; i+=16) {
		gg_printf("%08lX ", maddr+i);
		for (j=0; j<16; j++) {
			if (j>=(malen-i)) break;
			if (j==8) gg_PutC(' ');
			gg_printf(" %02X", buf[j]=*(uint8_t *)(maddr+i+j));
			if (GG_CON.PreDetection) goto owari;
		}
		if (mdchar(buf, malen-i)) goto owari;
	}
owari:
	maddr += malen;
	return 0;
}
static int C_mdw(int argc, char**argv)						// メモリダンプ(word)
{
	uint16_t buf[8];
	long i, j;
	if (argc>1) maddr = gg_asc2int(argv[1]);
	if (argc>2) malen = gg_asc2int(argv[2]);
	GG_CON.PreDetection = 0;
	for (i=0; i<malen; i+=16) {
		gg_printf("%08lX ", maddr+i);
		for (j=0; j<16; j+=2) {
			if (j>=(malen-i)) break;
			if (j==8) gg_PutC(' ');
			gg_printf(" %04X", buf[j/2]=*(uint16_t *)(maddr+i+j));
			if (GG_CON.PreDetection) goto owari;
		}
		if (mdchar(buf, malen-i)) goto owari;
	}
owari:
	maddr += malen;
	return 0;
}
static int C_mdd(int argc, char**argv)						// メモリダンプ(dword)
{
	uint32_t buf[8];
	long i, j;
	if (argc>1) maddr = gg_asc2int(argv[1]);
	if (argc>2) malen = gg_asc2int(argv[2]);
	GG_CON.PreDetection = 0;
	for (i=0; i<malen; i+=16) {
		gg_printf("%08lX ", maddr+i);
		for (j=0; j<16; j+=4) {
			if (j>=(malen-i)) break;
			if (j==8) gg_PutC(' ');
			gg_printf(" %08lX", buf[j/4]=*(uint32_t *)(maddr+i+j));
			if (GG_CON.PreDetection) goto owari;
		}
		if (mdchar(buf, malen-i)) goto owari;
	}
owari:
	maddr += malen;
	return 0;
}

static int C_msb(int argc, char**argv)						// メモリセット(byte)
{
	uint32_t addr;
	uint32_t data;
	if (argc>2) {
		addr = gg_asc2int(argv[1]);
		while(argc>2) {
			data = gg_asc2int(argv[2]);
			*(uint8_t *)(addr) = (uint8_t)data;
			addr += 1;
			argc--;
			argv++;
		}
	} else {
		gg_con_CmdHelp(argv[0]);
	}
	return 0;
}
static int C_msw(int argc, char**argv)						// メモリセット(word)
{
	uint32_t addr;
	uint32_t data;
	if (argc>2) {
		addr = gg_asc2int(argv[1]);
		while(argc>2) {
			data = gg_asc2int(argv[2]);
			*(uint16_t *)(addr) = (uint16_t)data;
			addr += 2;
			argc--;
			argv++;
		}
	} else {
		gg_con_CmdHelp(argv[0]);
	}
	return 0;
}
static int C_msd(int argc, char**argv)						// メモリセット(dword)
{
	uint32_t addr;
	uint32_t data;
	if (argc>2) {
		addr = gg_asc2int(argv[1]);
		while(argc>2) {
			data = gg_asc2int(argv[2]);
			*(uint32_t *)(addr) = (uint32_t)data;
			addr += 4;
			argc--;
			argv++;
		}
	} else {
		gg_con_CmdHelp(argv[0]);
	}
	return 0;
}

static int C_mfb(int argc, char**argv)						// メモリフィル(byte)
{
	uint32_t addr;
	long  	 alen;
	uint32_t data;
	long i;
	if (argc>3) {
		addr = gg_asc2int(argv[1]);
		alen = gg_asc2int(argv[2]);
		data = gg_asc2int(argv[3]);
		for (i=0; i<alen; i+=1) {
			*(uint8_t *)(addr+i) = (uint8_t)data;
		}
	} else {
		gg_con_CmdHelp(argv[0]);
	}
	return 0;
}
static int C_mfw(int argc, char**argv)						// メモリフィル(word)
{
	uint32_t addr;
	long  	 alen;
	uint32_t data;
	long i;
	if (argc>3) {
		addr = gg_asc2int(argv[1]);
		alen = gg_asc2int(argv[2]);
		data = gg_asc2int(argv[3]);
		for (i=0; i<alen; i+=2) {
			*(uint16_t *)(addr+i) = (uint16_t)data;
		}
	} else {
		gg_con_CmdHelp(argv[0]);
	}
	return 0;
}
static int C_mfd(int argc, char**argv)						// メモリフィル(dword)
{
	uint32_t addr;
	long  	 alen;
	uint32_t data;
	long i;
	if (argc>3) {
		addr = gg_asc2int(argv[1]);
		alen = gg_asc2int(argv[2]);
		data = gg_asc2int(argv[3]);
		for (i=0; i<alen; i+=4) {
			*(uint32_t *)(addr+i) = (uint32_t)data;
		}
	} else {
		gg_con_CmdHelp(argv[0]);
	}
	return 0;
}

#ifndef GG_TP_NOCODE	// TP対応
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
#endif

int gg_con_RegistCMD(void)		// 基本コマンドの登録
{
	GG_CON_CMDMRK("-- GG standard command --");			// 区切り行（helpでコマンド一覧のときに表示）
	GG_CON_CMDADD(C_help, 	"help",		"[cmd..]",				"command help"		);
	GG_CON_CMDADD(C_quit, 	"quit", 	"",						"!quit"				);		// ヘルプ文字列先頭に!のある行は一覧表示をスキップ
	GG_CON_CMDADD(C_mdb,	"md",		"[addr [alen]]",		"mem dump"			);		// メモリ系共通処理
	GG_CON_CMDADD(C_msb,	"ms",		"addr data..",			"mem set"			);		//
	GG_CON_CMDADD(C_mfb,	"mf",		"addr alen data",		"mem fill"			);		//
	GG_CON_CMDADD(C_mdb,	"mdb",		"[addr [alen]]",		"!mem dump(byte)"	);		//
	GG_CON_CMDADD(C_mdw,	"mdw",		"[addr [alen]]",		"!mem dump(word)"	);		//
	GG_CON_CMDADD(C_mdd,	"mdd",		"[addr [alen]]",		"!mem dump(dword)"	);		//
	GG_CON_CMDADD(C_msb,	"msb",		"addr data..",			"!mem set(byte)"	);		//
	GG_CON_CMDADD(C_msw,	"msw",		"addr data..",			"!mem set(word)"	);		//
	GG_CON_CMDADD(C_msd,	"msd",		"addr data..",			"!mem set(dword)"	);		//
	GG_CON_CMDADD(C_mfb,	"mfb",		"addr alen data",		"!mem fill(byte)"	);		//
	GG_CON_CMDADD(C_mfw,	"mfw",		"addr alen data",		"!mem fill(word)"	);		//
	GG_CON_CMDADD(C_mfd,	"mfd",		"addr alen data",		"!mem fill(dword)"	);		//
#ifndef GG_TP_NOCODE	// TP対応
	GG_CON_CMDADD(C_tp,		"tp",		"[#1 [#2]]",			"TP(test point) select"	);		//
#endif
	return 0;
}

/*
	gg_console		GGツール コンソールツール

	2022.01.25	M.Kogan	初版開発開始
*/

#ifndef 	__GG_CONSOLE_H__
#define 	__GG_CONSOLE_H__


//------------------
//	コンソールツール
//------------------
//	コマンド登録管理
//	コマンド実行ユーティリティ

#ifndef GG_CON_BUFMAX
#define GG_CON_BUFMAX	(256)	// コマンド文字列最大長
#endif
#ifndef GG_CON_ARGMAX
#define GG_CON_ARGMAX	(32)	// argc最大数
#endif

// コマンドリストエントリ
typedef struct GGtagCMD {		// コマンドリンクエレメント
	struct GGtagCMD*	next;	// 次リンク(NULL=終了)
	int (*func)(int argc, char**argv);	// コマンド関数へのポインタ
	const char *name;			// コマンド名
	const char *arg;			// パラメータヘルプ
	const char *help;			// 説明
} GGT_CMD;
// コマンド登録
//	** CC-RXでプロジェクトを空アプリから始めると初期値あり変数のROMからRAMへの転送が非対応になるので注意								2022.01.27 M.Kogan
//	** プロジェクトをアプリから始めるか、空アプリから始めたならリンクオプションでROMからRAMへマップするセクションに以下3行をセットする	2022.01.27 M.Kogan
//		D=R
//		D_1=R_1
//		D_2=R_2
#define GG_CON_CMDADD(func, cmd, prm, help) {\
	static GGT_CMD gcmd = { NULL, func, cmd, prm, help };\
	gg_con_CmdAdd(&gcmd);\
}
// コマンドの区切り(mark off)登録
#define GG_CON_CMDMRK(title) {\
	static GGT_CMD gcmd = { NULL, NULL, NULL, NULL, title };\
	gg_con_CmdAdd(&gcmd);\
}

// コンソールワークエリア(GG_CONワーク)
typedef struct GGtagCON {

	// コマンドライン系
	int16_t	BufP;					// コマンド文字列バッファポインタ
	char	Buf[GG_CON_BUFMAX+1];	// コマンド文字列バッファ
	char	*Argv[GG_CON_ARGMAX];	// argv(RAM上)
	char	PreDetection;			// 事前検出文字(ESC=0x1b,Break=0x03) gg_PreDetection()でセット

	// コマンドリンクリスト(cmd)
	struct {
		GGT_CMD *link;				// cmdコマンドリンク
	} cmd;

} GGT_CON;
extern GGT_CON GG_CON;				// GG_CONワーク(gg_console.c)

//--------------------------------------------------------------------------
//
// デバッグモニタ(console.c)		2022.01.25 M.Kogan
//
//--------------------------------------------------------------------------
//
//	デバッグモニタは1行コマンドを受付て実行する
//
#define GG_CON_EXIT	(-999)			// EXITコード
int gg_con_MonInit(void);			// デバッグモニタ初期化
extern char *gg_con_prompt;			// プロンプト文字列
int gg_con_MonRePrompt(void);		// プロンプト再表示
int gg_con_MonPrompt(void);			// プロンプト表示(内部で使用)
int gg_con_Check(void);				// デバッグモニタチェック(0=実行無か実行成功,-999=EXIT要請)(gg_GetC()から1文字受付)
int gg_con_MonCheck(int c);			// デバッグモニタチェック(0=実行無か実行成功,-999=EXIT要請)(1文字受付)

int gg_con_MonExec(char *buf);								// コマンド処理(bufはarg解析で破壊される)
int gg_con_MonBuf2ArgNC(char *p, char **argv, int argmax);	// buf-->arg解析(return=argc)(;コメント機能なし)
int gg_con_MonBuf2Arg(char *p, char **argv, int argmax);	// buf-->arg解析(return=argc)
int gg_con_MonExecArg(int argc, char **argv);				// コマンド処理(arg)



//--------------------------------------------------------------------------
//
// コマンド定義リスト(gg_console.c)			2022.01.25 M.Kogan
//
//--------------------------------------------------------------------------
//
//	 コマンドの追加方法
//
//		(1)コマンド群にコマンドサブルーチン(static int C_xxx(int argc, char **argv))を記述する
//
//			C言語の練習でよく使われる main(argc, argv) とおなじ要領でコンソールからコールされます。
//			例えば "abc" というコマンド文字列で C_abc() を定義しておくと、コンソールで
//			>abc 123 456
//			と入力して改行すると
//			C_abc(argc, argv)が呼び出され、そのとき引数には
//				argc=3
//				argv[0]="abc"
//				argv[1]="123"
//				argv[2]="456"
//			とセットされています。
//
//		(2)起動時にコマンド登録(GG_CON_CMDADD)しておく
//
//			1パラ目=上で追加したコマンドサブルーチン
//			2パラ目=コマンド名の文字列
//			3パラ目=コマンド引数の説明(ヘルプで表示)
//			4パラ目=コマンドの説明(ヘルプで表示)(!が先頭だと通常helpでは出ない。検索help(cn*等)で出る)
//	
//			＊必要であれば区切り(mark off)も登録できる(KMON_CMDMRK)
//
//
int gg_con_CmdInit(void);				// コマンド定義リストの初期化
int gg_con_CmdAdd(GGT_CMD *def);		// コマンド定義リストの追加
int gg_con_CmdN(void);					// コマンド登録数
int	gg_con_CmdHelp(char *cmd);			// コマンドヘルプ
int	gg_con_CmdHelpAll(void);			// コマンド一覧
GGT_CMD* gg_con_CmdSearch(char *name);	// コマンド名のサーチ(完全一致,登録情報のポインタを返す。NULL=エラー)
GGT_CMD* gg_con_CmdSearchX(char *name);// コマンド名のサーチ(nameは途中でも良い,登録情報のポインタを返す。NULL=エラー)


//--------------------------------------------------------------------------
//
// 基本コマンドと登録(gg_console.c)		2022.01.27 M.Kogan
//
//--------------------------------------------------------------------------

int gg_con_RegistHelpCMD(void);		// helpコマンド登録
int gg_con_RegistMemCMD(void);		// メモリ系コマンド登録



#endif	//	__GG_CONSOLE_H__

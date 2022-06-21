# GG for Arduino

GG for Arduino はシリアルコンソールのライブラリです。コンソールを組み込むための補助的なツール（例えば printf()のような）を含みます。

## コンソール

Arduinoでコマンドラインインターフェース（コンソールの基本機能）が実現できます。もちろん自分で作ったコマンドの追加もできます。

https://user-images.githubusercontent.com/11693904/174482312-17b5dc9a-c0d9-4d6a-a9e9-b6fb8989c29e.mp4

### 通信仕様

* Arduinoの標準シリアル(Serial)
* 115200BPS / 8bitデータ長 / ノンパリティ
* エコーバックはGGが出す（端末でキーを打てば打ったキーが表示されます）
* CR(\r)またはLF(\n)でコマンド実行

### 登録済みコマンド

* `help` コマンド一覧の表示

### gg_con_RegistMemCMD()で追加登録されるコマンド

* `md` メモリダンプ
* `ms` メモリセット
* `mf` メモリフィル（ブロックセット）

下記の例をご覧ください。

* [GG_console.ino](examples/GG_console/GG_console.ino).

### gg_tp_start()で追加登録されるコマンド

* `tp` TP(テストポイント)の状態番号の表示と変更

### コマンドの追加方法

自分で作ったコマンドを追加できます。コマンド実装部（サブルーチン）にはコマンドラインのパラメータが argc と argv で渡されます。C言語の参考書や解説に「コマンドライン引数」を使ったmain()プログラムの例があると思います。これと同じですので参考にしてください。  
コマンド実装部（サブルーチン）ができたら登録します。  
下記の例をご覧ください。

* [GG_console_cmd.ino](examples/GG_console_cmd/GG_console_cmd.ino).

### コマンドラインインターフェースなしでコンソールを使う方法

コマンドラインインターフェース（コマンド受付機能）は使わないで、printf() のようなコンソールの出力機能だけ使うこともできます。  
下記の例をご覧ください。

* [GG_printf.ino](examples/GG_printf/GG_printf.ino).

### タイミング分析

TP（テストポイント）機能を使ってプログラムのタイミング分析ができます。  
サブルーチンの処理時間を測定することができます。また割込み処理やRTOSのタスク処理の状況確認にも使えます。  

まず、分析するプログラムポイントに状態番号コントロール（GG_TP_ON / GG_TP_OFF）を埋め込みます。 次に、tpコマンドを使用して、TP1またはTP2に出力する状態番号を選択します。 状態番号はいつでもtpコマンドで切り替えることができます。  

状態番号はいくつでも配置できますが、物理的な出力先はTP1とTP2の2つだけです。 Arduinoのピン数が制限されていることと、出力チェックは出力先が増えるだけCPU負荷が増加することから、出力先が2つに限定されています。  

以下の例では、TP1はArduinoの2番ピン、TP2はArduinoの3番ピンです。TP1（2番ピン）はオシロスコープの黄色のCH1に接続され、TP2（3番ピン）は紫色のCH2に接続されています。

* [GG_tp.ino](examples/GG_tp/GG_tp.ino).  

https://user-images.githubusercontent.com/11693904/174753954-4fe72062-f222-4848-b2a1-9c6575c5a101.mp4

## 利用できる関数/変数/定義

ヒープ(malloc)は使いません。  

|関数 / 変数|仕様|
|--|---|
|***GG.h***||
|int gg_start(const char \*title)|GG for Arduino の開始|
|long gg_con_baud|ボーレート. 115200から変更するならgg_start()の前にセット|
|int gg_tp_start(int tp1_no, int tp2_no)|TP機能スタート(出力ピン初期化とコマンド登録)|
|GG_TP_ON(number)|TP状態番号をON|
|GG_TP_OFF(number)|TP状態番号をOFF|
|***gg_uty.h***||
|int gg_GetC(void)|1文字入力 (-1 = 入力なし)|
|int gg_PutC(int c)|1文字出力|
|int gg_PutS(const char \*s)|文字列出力|
|int32_t gg_asc2int(char \*p)|文字列を整数に変換. '0x'で16進数と認識|
|int16_t gg_mbuf_init(GGT_MBUF \*mbuf, void\* buf, int max)|ミニバッファツール. 初期化|
|int16_t gg_mbuf_put(GGT_MBUF \*mbuf, int16_t d)|ミニバッファツール. 1バイト書込み|
|int16_t gg_mbuf_get(GGT_MBUF \*mbuf)|ミニバッファツール. 1バイト読出し|
|int16_t gg_mbuf_peek(GGT_MBUF \*mbuf, int16_t ix)|ミニバッファツール. のぞき見|
|gg_time_t|GGでのみ有効な秒単位での総時間。 開始点(0)は2000年1月1日|
|GGT_BCDTIME|日時の構造体(BCD)|
|GGT_TIME|日時の構造体(整数)|
|int gg_tim_bcd2dt(GGT_BCDTIME \*b, GGT_TIME \*t)|GGT_BCDTIMEからGGT_TIMEに変換|
|int gg_tim_dt2bcd(GGT_TIME \*t, GGT_BCDTIME \*b)|GGT_TIMEからGGT_BCDTIMEに変換|
|gg_time_t gg_tim_dt2tt(GGT_TIME \*t)|GGT_TIMEからgg_time_tに変換|
|gg_time_t gg_tim_YYMMDD2tt(int YY, int MM, int DD)|年月日からgg_time_tに変換|
|gg_time_t gg_tim_hhmmss2tt(int hh, int mm, int ss)|時分秒からgg_time_tに変換|
|int gg_tim_tt2dt(gg_time_t x, GGT_TIME \*t)|gg_time_tからGGT_TIMEに変換|
|int gg_tim_checkdt(GGT_TIME \*t)|日時のチェック|
|int gg_tim_repairdt(GGT_TIME \*t)|日時が異常ならそれなりの値に修正|
|int gg_tim_tt2week(gg_time_t x)|gg_time_tから曜日(0=日,1=月..6=土)に変換|
|***gg_printf.h***||
|int gg_vxprintf(int (\*putc)(int), const char \*fmt, va_list vlst)|putc()で出力するvprintf()|
|int gg_xprintf(int (\*putc)(int), const char \*fmt, ...)|putc()で出力するprintf()|
|int gg_printf(const char \*fmt, ...)|標準出力のprintf().%eは非実装|
|int gg_vsprintf(char \*buf, const char \*fmt, va_list vlst)|buf(メモリ)に出力するvsprintf()|
|int gg_sprintf(char \*buf, const char \*fmt, ...)|buf(メモリ)に出力するsprintf()|
|gg_printDATE(gg_time_t)|"YY/MM/DD"|
|gg_printTIME(gg_time_t)|"hh:mm:ss"|
|gg_printDT(gg_time_t)|"YY/MM/DD hh:mm:ss"|
|gg_printDTs(gg_time_t)|"YY/MM/DD hh:mm:ss "|
|***gg_console.h***||
|GG_CON_CMDADD(func, cmd, prm, help)|コンソールコマンド登録|
|GG_CON_CMDMRK(title)|コマンド一覧での区切り行登録|
|GG_CON|コンソール処理のワークエリア(内部で使用)|
|int gg_con_MonInit(void)|コンソールの初期化|
|char \*gg_con_prompt|プロンプト文字列. 初期値=">"|
|int gg_con_MonRePrompt(void)|プロンプトの再表示|
|int gg_con_MonPrompt(void)|プロンプトの表示 (内部で使用)|
|int gg_con_Check(void)|gg_GetC()から1文字取得したらコンソールチェック処理|
|int gg_con_MonCheck(int c)|コンソールチェック処理(c=-1のときは何もしない)|
|int gg_con_MonExec(char \*buf)|コマンドライン1行の処理(注:argv分割のためbufの中は破壊される)|
|int gg_con_MonBuf2Arg(char \*p, char \**argv, int argmax)|buf[]からargv解析(return = argc)(非リエントラント)|
|int gg_con_MonExecArg(int argc, char \**argv)|argv[0]をコマンド名として登録コマンドを検索し、見つかればコマンド実装部を実行|
|int gg_con_CmdInit(void)|コマンド登録リストの初期化|
|int gg_con_CmdAdd(GGT_CMD \*def)|コマンド登録リストに追加|
|int gg_con_CmdN(void)|コマンド登録数|
|int gg_con_CmdHelp(char \*cmd)|cmdのヘルプ表示|
|int gg_con_CmdHelpAll(void)|コマンド一覧の表示|
|int gg_con_RegistMemCMD(void)|メモリ系コマンド登録|

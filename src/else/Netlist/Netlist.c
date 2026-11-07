//------------------------------------------------------------------------
//Netlist
//File name : Netlist.c
//Date : 2010/06/07
//Designer : R.Inoue
//Ver : 3.0（配布Ver）
//------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../../header/Define.h"


//------------------------------------------------------------------------
// 定義
//------------------------------------------------------------------------
#define MAXN 1028      // ネットリストファイル名サイズ
#define STR_MAX 1024   //get_token用
#define SPRINT_MAX 256 //sprintf用
#define HASH_SIZE 5003 //ハッシュ表サイズ
#define HASH_MODE
#undef DEBUG_MA
#undef DEBUG_NL

//モジュール配列メンバー
typedef struct _Module_Array_MEM_Format_ {
	char *m_name_ins; //インスタンス名
	int m_type;	      //タイプ
	int m_n_in;	      //入力数
	char *m_out_name; //出力名
	char **m_in_name; //入力名配列
	// exe. <通常>  A:[0], B:[1],・・・, J:[9]
	//      <DFF >  D:[0]
	//      <RDFF>  D:[0], CD(リセット):[1]
	//      <DFFS>  D:[0], TI(スキャンイン):[1], TE(スキャンイネーブル):[2]
	//      <RDFFS> D:[0], TI(スキャンイン):[1], TE(スキャンイネーブル):[2], CD(リセット):[3]
	int out_nl_id;    //出力信号のネットリストID
	int *in_nl_id;    //入力信号のネットリストID配列
} Module_Array_MEM;

//信号名ハッシュメンバー
typedef struct _Signal_Name_Hash_Format_ {
	int nl_id;        //ネットリストID
	struct _Signal_Name_Hash_Format_ *next;
} Signal_Name_Hash;

//構文解析配列
int *parser_array[2]; //for_in:[0], for_out[1]

//------------------------------------------------------------------------
// 静的変数
//------------------------------------------------------------------------
//端子名配列（10入力まで対応 04/03/16）
char pin_name_array[10] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J' };

//モージュール配列
static Module_Array_MEM **module_array = NULL;

//信号名ハッシュテーブル
static Signal_Name_Hash **hash_t = NULL;

//モージュール数
static int n_module = 0;

//仮の信号数
static int nl_num = 0;

//------------------------------------------------------------------------
// プロタイプ宣言
//------------------------------------------------------------------------
static void option(int, char **, char *);
static void file_open(FILE **, char *);
static void init_global(void);
static void read_net(FILE *);
static void read_file(FILE *);
static void read_1st(FILE *);
static void calc_pi_po(FILE *, int);
static void alloc_module_array(void);
static void read_2nd(FILE *);
static void read_n_in(int, char *, int);
static void alloc_in_name(int);
static void read_module_type(FILE *, char *, int);
static void read_ins(FILE *, int);
static void read_pin(FILE *, int);
static void alloc_nl(void);
static void parser(FILE *);
static void alloc_hash_table(void);
static void store_signal(void);
static void store_pi_po(FILE *);
static void read_pi_po(FILE *, int *, int);
static void hash_search_pi_po(char *, int *, int);
static void read_assign(FILE *, int *);
static void analyze_asyntax_error(void);
static int hash_key(char *);
static void free_hash(void);
static void connection_signal(void);
static void connection_no_fout(int, int);
static void connection_yes_fout(int, int);
static void connection_po_fout(int, int, int);
static void alloc_nl_in_out(void);
static void connection_n_signal(void);
static void set_pi_ins(void);
static void set_ff_array(void);
static int get_token(FILE *, char *, int, int *);
static void error(int, char *);
static void free_module_array(void);
static void debug_ma(void);
static void debug_nl(void);

//------------------------------------------------------------------------
// 外部関数
//------------------------------------------------------------------------
void read_nl(char *v_name)
{
	FILE *r_fp; //ネットリストファイルポインタ

	//ファイルオープン
	file_open(&r_fp,v_name);

	//外部変数初期化
	init_global();

	//ネットリスト読み込み
	read_net(r_fp);

	//ネットリスト配列デバッグ
#ifdef DEBUG_NL
	debug_nl();
#endif

	//確認メッセージ
//	fprintf(stderr, "Completion of Read_Netlist !!\n");

	return ;
}

//------------------------------------------------------------------------
// 内部関数
//------------------------------------------------------------------------
//------------------------------------------------------------------------
// コマンドラインの処理
//------------------------------------------------------------------------
static void option(int argc, char *argv[], char *v_name)
{
	//使い方説明
	if (argc != 2) {
		printf("******** The usage of Netlist.exe *******\n");
		printf("Netlist.exe [Verilog-HDL Netlist file]\n");
		printf("*****************************************\n");
		exit(-1);
	}

	//ファイル名取得
	strcpy(v_name, argv[1]);
}

//------------------------------------------------------------------------
// ファイルオープン
//------------------------------------------------------------------------
static void file_open(FILE **r_fp, char *v_name)
{
	if ((*r_fp = fopen(v_name, "r")) == (FILE *)NULL) {
		fprintf(stderr, "ファイルオープン失敗(%s)\n", v_name);
		exit(-1);
	}
}

//------------------------------------------------------------------------
// 外部変数初期化
//------------------------------------------------------------------------
static void init_global(void)
{
	n_net = 0;    //信号数
	n_pi = 0;     //外部入力数
	n_po = 0;     //外部出力線
	n_dff = 0;    //DFF数
	n_rdff = 0;   //RDFF数
	n_dffs = 0;   //DFFS数
	n_rdffs = 0;  //RDFFS数
	n_assign = 0; //assign数
}

//------------------------------------------------------------------------
// ネットリスト読み込み制御
//------------------------------------------------------------------------
static void read_net(FILE *rfp)
{
	//ネットリストファイル読み込み
	read_file(rfp);

	//ネットリスト配列,外部変数,構文解析配列 領域確保,初期化
	alloc_nl();

	//構文解析
	parser(rfp);

	//ハッシュ表解放
	free_hash();

	//信号接続
	connection_signal();

	//モジュール配列デバッグ
#ifdef DEBUG_MA
	debug_ma();
#endif

	//モジュール配列,構文解析配列 解放
	free_module_array();
}

//------------------------------------------------------------------------
// ネットリストファイル情報読み込み
//------------------------------------------------------------------------
static void read_file(FILE *rfp)
{
	//モジュール数, PI数, PO数算出 & module名取得
	read_1st(rfp);

	//モジュール配列領域確保, 初期化
	alloc_module_array();

	//各モジュール読み込み
	read_2nd(rfp);
}

//------------------------------------------------------------------------
// モジュール数, PI数, PO数算出 & module名取得
//------------------------------------------------------------------------
static void read_1st(FILE *rfp)
{
	char str[STR_MAX];
	int c;

	while (get_token(rfp, str, 1, 0) != EOF) {
		//module名取得
		if (!strcmp(str, "module")) {
			get_token(rfp, str, 1, 0);
			module_name = (char *)malloc(sizeof(char)* (strlen(str) + 1));
			strcpy(module_name, str);
			while ((c = getc(rfp)) != ';'); continue;
		}
		else if (!strcmp(str, "wire")) { while ((c = getc(rfp)) != ';'); continue; }
		//assign数算出
		else if (!strcmp(str, "assign")) {
			n_assign++;
			while ((c = getc(rfp)) != ';'); continue;
		}
		else if (!strcmp(str, "endmodule")) { continue; }
		//PI数算出
		else if (!strcmp(str, "input")) calc_pi_po(rfp, 0);
		//PO数算出
		else if (!strcmp(str, "output")) calc_pi_po(rfp, 1);
		//モジュール数算出
		else {
			n_module++;
			while ((c = getc(rfp)) != ';'); continue;
		}
	}
	rewind(rfp);
}

//------------------------------------------------------------------------
// PI数, PO数算出
//------------------------------------------------------------------------
static void calc_pi_po(FILE *rfp, int mode)
{
	char str[STR_MAX]; //get_token用
	int bit_1, bit_2;  //ビット幅がある場合のビット数計算用
	int re_no;         //返り値
	int num;
	int c;

	//初期化
	re_no = num = 0;

	c = fgetc(rfp);
	while ((c == ' ') || (c == '\t')) c = fgetc(rfp);
	//bit幅あり
	if (c == '[') {
		get_token(rfp, str, 1, 0); bit_1 = atoi(str);
		get_token(rfp, str, 1, 0); bit_2 = atoi(str);
		re_no = get_token(rfp, str, 0, 0);
		while ((re_no != 1) && (re_no != 2)) {
			num++;
			re_no = get_token(rfp, str, 0, 0);
		}
		if (re_no == 1) num++;
		if (mode == 0) n_pi += (num * (abs(bit_1 - bit_2) + 1));
		else if (mode == 1) n_po += (num * (abs(bit_1 - bit_2) + 1));
	}
	//bit幅なし
	else {
		ungetc(c, rfp);
		re_no = get_token(rfp, str, 0, 0);
		while ((re_no != 1) && (re_no != 2)) {
			//クロック読み飛ばし
			if (!strcmp(str, "CLK") || !strcmp(str, "clk")) {
				re_no = get_token(rfp, str, 0, 0);
				continue;
			}
			if ((re_no == 0) && (mode == 0)) n_pi++;
			else if ((re_no == 0) && (mode == 1)) n_po++;
			re_no = get_token(rfp, str, 0, 0);
		}
		//クロック読み飛ばし（クロックでなっかた場合実行）
		if (strcmp(str, "CLK") && strcmp(str, "clk")) {
			if ((re_no == 1) && (mode == 0)) n_pi++;
			else if ((re_no == 1) && (mode == 1)) n_po++;
		}
	}
}

//------------------------------------------------------------------------
// モジュール配列領域確保, 初期化
//------------------------------------------------------------------------
static void alloc_module_array(void)
{
	int i;

	module_array = (Module_Array_MEM **)malloc(sizeof(Module_Array_MEM *)* n_module);
	for (i = 0; i<n_module; i++)
		module_array[i] = (Module_Array_MEM *)malloc(sizeof(Module_Array_MEM));
}

//------------------------------------------------------------------------
// 各モジュールの読み込み
//------------------------------------------------------------------------
static void read_2nd(FILE *rfp)
{
	int in_num; //入力数読み込み用
	int m_no;   //現在読んでいるモジュール番号
	char str[STR_MAX];
	int c;

	//初期化
	in_num = m_no = 0;

	while (get_token(rfp, str, 2, &in_num) != EOF) {
		if (!strcmp(str, "module")) { while ((c = getc(rfp)) != ';'); continue; }
		else if (!strcmp(str, "input")) { while ((c = getc(rfp)) != ';'); continue; }
		else if (!strcmp(str, "output")) { while ((c = getc(rfp)) != ';'); continue; }
		else if (!strcmp(str, "wire")) { while ((c = getc(rfp)) != ';'); continue; }
		else if (!strcmp(str, "assign")) { while ((c = getc(rfp)) != ';'); continue; }
		else if (!strcmp(str, "endmodule")) { continue; }
		else {
			//入力数の読み込み
			read_n_in(in_num, str, m_no);
			//入力信号名,入力信号ネットリストID配列の領域確保
			alloc_in_name(m_no);
			//モジュールタイプ
			read_module_type(rfp, str, m_no);
			//インスタンス名の読み込み
			read_ins(rfp, m_no);
			//端子, 信号名の読み込み
			read_pin(rfp, m_no);

			in_num = 0;
			m_no++;
			while ((c = getc(rfp)) != ';'); continue;
		}
	}
	rewind(rfp);
}

//------------------------------------------------------------------------
// 入力数の読み込み
//------------------------------------------------------------------------
static void read_n_in(int in_num, char *str, int m_no)
{
	if (!strcmp(str, "BUF")) module_array[m_no]->m_n_in = 1;
	else if (!strcmp(str, "INV")) module_array[m_no]->m_n_in = 1;
	else if (!strcmp(str, "DFF")) module_array[m_no]->m_n_in = 1;
	else if (!strcmp(str, "RDFF")) module_array[m_no]->m_n_in = 2;
	else if (!strcmp(str, "DFFS")) module_array[m_no]->m_n_in = 3;
	else if (!strcmp(str, "RDFFS")) module_array[m_no]->m_n_in = 4;
	else module_array[m_no]->m_n_in = in_num;
}

//------------------------------------------------------------------------
// 入力数信号名,入力ハッシュポインタ配列の領域確保
//------------------------------------------------------------------------
static void alloc_in_name(int m_no)
{
	module_array[m_no]->m_in_name = (char **)malloc(sizeof(char *)* module_array[m_no]->m_n_in);
	module_array[m_no]->in_nl_id = (int *)malloc(sizeof(int)* module_array[m_no]->m_n_in);
}

//------------------------------------------------------------------------
// モジュールタイプの読み込み
//------------------------------------------------------------------------
static void read_module_type(FILE *rfp, char *str, int m_no)
{
	switch (str[0]) {
	case 'A': module_array[m_no]->m_type = AND; break;
	case 'B': module_array[m_no]->m_type = BUF; break;
	case 'D': if (strlen(str) == 3) { module_array[m_no]->m_type = DFF; n_dff++; }
			  else { module_array[m_no]->m_type = DFFS; n_dffs++; }
			  break;
	case 'E': if (strlen(str) == 4) module_array[m_no]->m_type = EXOR;
			  else module_array[m_no]->m_type = EXNOR;
			  //error : 2入力以外のEXOR, EXNORがあった場合
			  if (module_array[m_no]->m_n_in != 2) {
				  get_token(rfp, str, 0, 0);
				  error(1, str);
				  exit(-1);
			  }
			  break;
	case 'I': module_array[m_no]->m_type = INV;	break;
	case 'N': if (strlen(str) == 3) module_array[m_no]->m_type = NOR;
			  else module_array[m_no]->m_type = NAND; break;
	case 'O': module_array[m_no]->m_type = OR; break;
	case 'R': if (strlen(str) == 4) { module_array[m_no]->m_type = RDFF; n_rdff++; }
			  else { module_array[m_no]->m_type = RDFFS; n_rdffs++; }
			  break;
	}
}

//------------------------------------------------------------------------
// インスタンス名の読み込み
//------------------------------------------------------------------------
static void read_ins(FILE *rfp, int m_no)
{
	char str[STR_MAX]; //get_token用

	get_token(rfp, str, 0, 0);
	module_array[m_no]->m_name_ins = (char *)malloc(sizeof(char)* (strlen(str) + 1));
	strcpy(module_array[m_no]->m_name_ins, str);
}

//------------------------------------------------------------------------
// 端子,信号名の読み込み
//------------------------------------------------------------------------
static void read_pin(FILE *rfp, int m_no)
{
	char str[STR_MAX]; //get_token用
	int i, j;

	for (i = 0; i<module_array[m_no]->m_n_in + 1; i++) { //出力線分+1
		while (get_token(rfp, str, 0, 0) != 0);
		//出力信号
		if (str[0] == 'Z' || str[0] == 'Q') {
			while (get_token(rfp, str, 0, 0) != 0);
			module_array[m_no]->m_out_name = (char *)malloc(sizeof(char)* (strlen(str) + 1));
			strcpy(module_array[m_no]->m_out_name, str);
		}
		//各DFFのD（入力）
		else if (((str[0] == 'D') && (module_array[m_no]->m_type == DFF)) ||
			((str[0] == 'D') && (module_array[m_no]->m_type == RDFF)) ||
			((str[0] == 'D') && (module_array[m_no]->m_type == DFFS)) ||
			((str[0] == 'D') && (module_array[m_no]->m_type == RDFFS))) {
			while (get_token(rfp, str, 0, 0) != 0);
			module_array[m_no]->m_in_name[0] = (char *)malloc(sizeof(char)* (strlen(str) + 1));
			strcpy(module_array[m_no]->m_in_name[0], str);
		}
		//各DFFのCP（クロック）
		else if (!strcmp(str, "CP")) {
			while (get_token(rfp, str, 0, 0) != 0);
			i--;
		}
		//RDFF,RDFFSのCD（リセット）
		else if (!strcmp(str, "CD")) {
			while (get_token(rfp, str, 0, 0) != 0);
			if (module_array[m_no]->m_type == RDFF) {
				module_array[m_no]->m_in_name[1] = (char *)malloc(sizeof(char)* (strlen(str) + 1));
				strcpy(module_array[m_no]->m_in_name[1], str);
			}
			else if (module_array[m_no]->m_type == RDFFS) {
				module_array[m_no]->m_in_name[3] = (char *)malloc(sizeof(char)* (strlen(str) + 1));
				strcpy(module_array[m_no]->m_in_name[3], str);
			}
		}
		//DFFS,RDFFSのTI（スキャンイン）
		else if (!strcmp(str, "TI")) {
			while (get_token(rfp, str, 0, 0) != 0);
			module_array[m_no]->m_in_name[1] = (char *)malloc(sizeof(char)* (strlen(str) + 1));
			strcpy(module_array[m_no]->m_in_name[1], str);
		}
		//DFFS,RDFFSのTE（スキャンイネーブル）
		else if (!strcmp(str, "TE")) {
			while (get_token(rfp, str, 0, 0) != 0);
			module_array[m_no]->m_in_name[2] = (char *)malloc(sizeof(char)* (strlen(str) + 1));
			strcpy(module_array[m_no]->m_in_name[2], str);
		}
		//その他の入力信号
		else {
			j = 0;
			while (pin_name_array[j] != str[0]) {
				j++;
				if (j > 10) error(2, module_array[m_no]->m_name_ins);
			}
			while (get_token(rfp, str, 0, 0) != 0);
			module_array[m_no]->m_in_name[j] = (char *)malloc(sizeof(char)* (strlen(str) + 1));
			strcpy(module_array[m_no]->m_in_name[j], str);
		}
	}
}

//------------------------------------------------------------------------
// ネットリスト配列,外部変数,構文解析配列 領域確保,初期化
//------------------------------------------------------------------------
static void alloc_nl(void)
{
	int i;

	//仮の信号線数
	nl_num = n_pi + n_po + n_module;
	for (i = 0; i<n_module; i++) nl_num += module_array[i]->m_n_in;

	//領域確保
	nl = (NLIST *)malloc(sizeof(NLIST)* nl_num);
	pi = (NLIST **)malloc(sizeof(NLIST *)* n_pi);
	po = (NLIST **)malloc(sizeof(NLIST *)* n_po);
	dff = (NLIST **)malloc(sizeof(NLIST *)* n_dff);
	rdff = (NLIST **)malloc(sizeof(NLIST *)* n_rdff);
	dffs = (NLIST **)malloc(sizeof(NLIST *)* n_dffs);
	rdffs = (NLIST **)malloc(sizeof(NLIST *)* n_rdffs);
	assign = (NLIST **)malloc(sizeof(NLIST *)* n_assign);
	parser_array[0] = (int *)malloc(sizeof(int)* (nl_num));
	parser_array[1] = (int *)malloc(sizeof(int)* (nl_num));

	//初期化
	for (i = 0; i<nl_num; i++) {
		nl[i].type = -1;
		nl[i].n_in = 0;
		nl[i].in = (NLIST **)NULL;
		nl[i].n_out = 0;
		nl[i].out = (NLIST **)NULL;
		parser_array[0][i] = 0;
		parser_array[1][i] = 0;

		nl[i].open_pass = YES;
		nl[i].fault_pass = NO;
		nl[i].val_1t = X;
		nl[i].val_2t = X;
		nl[i].suf_1t = 0;
		nl[i].suf_2t = 0;
		nl[i].suf_fc = 0;

		nl[i].tf_flag = OFF;
	}
}

//------------------------------------------------------------------------
// 構文解析
//------------------------------------------------------------------------
static void parser(FILE *rfp)
{
	//ハッシュ表の領域確保
	alloc_hash_table();

	//信号情報の格納
	store_signal();

	//PI,PO読み込み
	store_pi_po(rfp);

	//構文エラー解析
	analyze_asyntax_error();
}

//------------------------------------------------------------------------
// ハッシュ表の領域確保
//------------------------------------------------------------------------
static void alloc_hash_table(void)
{
	int i;

	hash_t = (Signal_Name_Hash **)malloc(sizeof(Signal_Name_Hash *)* HASH_SIZE);
	for (i = 0; i<HASH_SIZE; i++) hash_t[i] = NULL;
}

//------------------------------------------------------------------------
// 信号情報の格納
//------------------------------------------------------------------------
static void store_signal(void)
{
	Signal_Name_Hash *ptr;
	int hash_value; //ハッシュ値
	int i, j;

	for (i = 0; i<n_module; i++) {
		//入力
		for (j = 0; j<module_array[i]->m_n_in; j++) {
			hash_value = hash_key(module_array[i]->m_in_name[j]);
			//探索
			ptr = hash_t[hash_value];
			while (ptr != NULL) {
				if (!strcmp(nl[ptr->nl_id].name, module_array[i]->m_in_name[j])) {
					//構文解析配列インクリメント
					parser_array[0][ptr->nl_id] ++;
					//入力信号ネットリストID配列挿入
					module_array[i]->in_nl_id[j] = ptr->nl_id;
					break;
				}
				ptr = ptr->next;
			}
			//新規作成
			if (ptr == NULL) {
				//ハッシュ表挿入
				ptr = (Signal_Name_Hash *)malloc(sizeof(Signal_Name_Hash));
				ptr->next = hash_t[hash_value];
				hash_t[hash_value] = ptr;
				ptr->nl_id = n_net;
				//ネットリスト挿入
				nl[n_net].name = (char *)malloc(sizeof(char)* (strlen(module_array[i]->m_in_name[j]) + 1));
				strcpy(nl[n_net].name, module_array[i]->m_in_name[j]);
				nl[n_net].n = n_net; //ネットリストID
				//構文解析配列代入
				parser_array[0][n_net] = 1;
				//入力信号ネットリストID配列挿入
				module_array[i]->in_nl_id[j] = n_net;

				n_net++;
			}
		}
		//出力
		hash_value = hash_key(module_array[i]->m_out_name);
		//探索
		ptr = hash_t[hash_value];
		while (ptr != NULL) {
			if (!strcmp(nl[ptr->nl_id].name, module_array[i]->m_out_name)) {
				//構文解析配列インクリメント
				parser_array[1][ptr->nl_id] ++;
				//出力信号ネットリストID挿入
				module_array[i]->out_nl_id = ptr->nl_id;
				break;
			}
			ptr = ptr->next;
		}
		//新規作成
		if (ptr == NULL) {
			//ハッシュ表挿入
			ptr = (Signal_Name_Hash *)malloc(sizeof(Signal_Name_Hash));
			ptr->next = hash_t[hash_value];
			hash_t[hash_value] = ptr;
			ptr->nl_id = n_net;
			//ネットリスト挿入
			nl[n_net].name = (char *)malloc(sizeof(char)* (strlen(module_array[i]->m_out_name) + 1));
			strcpy(nl[n_net].name, module_array[i]->m_out_name);
			nl[n_net].n = n_net; //ネットリストID
			//構文解析配列代入
			parser_array[1][n_net] = 1;
			//出力信号ネットリストID挿入
			module_array[i]->out_nl_id = n_net;

			n_net++;
		}
	}
}

//------------------------------------------------------------------------
// PI,PO読み込み制御
//------------------------------------------------------------------------
static void store_pi_po(FILE *rfp)
{
	char str[STR_MAX]; //get_token用
	int pi_no;
	int po_no;
	int assign_no;
	int c;

	//初期化
	pi_no = po_no = assign_no = 0;

	while (get_token(rfp, str, 0, 0) != EOF) {
		if (!strcmp(str, "module")) { while ((c = getc(rfp)) != ';'); continue; }
		else if (!strcmp(str, "input")) read_pi_po(rfp, &pi_no, 0);
		else if (!strcmp(str, "output")) read_pi_po(rfp, &po_no, 1);
		else if (!strcmp(str, "assign")) { while ((c = getc(rfp)) != ';'); continue; }//read_assign(rfp, &assign_no);
		else if (!strcmp(str, "wire")) { while ((c = getc(rfp)) != ';'); continue; }
		else if (!strcmp(str, "endmodule")) { continue; }
		else { while ((c = getc(rfp)) != ';'); continue; }
	}
}

//------------------------------------------------------------------------
// PI,PO読み込み
//------------------------------------------------------------------------
static void read_pi_po(FILE *rfp, int *no, int mode)
{
	char str[STR_MAX];    //get_token用
	char tmp[SPRINT_MAX]; //sprint用
	int bit_1, bit_2;     //ビット幅がある場合のビット数計算用
	int n_bit;            //ビット数
	int start_bit;        //最初のビット数
	int re_no;            //返り値
	int i, c;

	//初期化
	re_no = 0;

	c = fgetc(rfp);
	while ((c == ' ') || (c == '\t')) c = fgetc(rfp);
	//bit幅あり
	if (c == '[') {
		get_token(rfp, str, 1, 0); bit_1 = atoi(str);
		get_token(rfp, str, 1, 0); bit_2 = atoi(str);
		n_bit = abs(bit_1 - bit_2) + 1;
		re_no = get_token(rfp, str, 0, 0);
		//最初のビット数
		if (bit_1 <= bit_2) start_bit = bit_1;
		else start_bit = bit_2;
		//複数ポート宣言
		while ((re_no != 1) && (re_no != 2)) {
			for (i = 0; i<n_bit; i++) {
				sprintf(tmp, "%s[%d]", str, i + start_bit);
				hash_search_pi_po(tmp, no, mode);
			}
			re_no = get_token(rfp, str, 0, 0);
		}
		//単独ポート宣言
		if (re_no == 1) {
			for (i = 0; i<n_bit; i++) {
				sprintf(tmp, "%s[%d]", str, i + start_bit);
				hash_search_pi_po(tmp, no, mode);
			}
		}
	}
	//bit幅なし
	else {
		ungetc(c, rfp);
		re_no = get_token(rfp, str, 0, 0);
		while ((re_no != 1) && (re_no != 2)) {
			//クロック読み飛ばし
			if (!strcmp(str, "CLK") || !strcmp(str, "clk")) {
				re_no = get_token(rfp, str, 0, 0);
				continue;
			}
			if (re_no == 0) hash_search_pi_po(str, no, mode);
			re_no = get_token(rfp, str, 0, 0);
		}
		//クロック読み飛ばし（クロックでなっかた場合実行）
		if (strcmp(str, "CLK") && strcmp(str, "clk")) {
			if (re_no == 1) hash_search_pi_po(str, no, mode);
		}
	}
}

//------------------------------------------------------------------------
// PI,PO ハッシュ探索
//------------------------------------------------------------------------
static void hash_search_pi_po(char *word, int *no, int mode)
{
	int hash_value;   //ハッシュ値
	Signal_Name_Hash *ptr;

	hash_value = hash_key(word);
	ptr = hash_t[hash_value];
	while (ptr != NULL) {
		if (!strcmp(nl[ptr->nl_id].name, word)) {
			//PI
			if (mode == 0) {
				//TYPE
				nl[ptr->nl_id].type = IN;
				//PI配列
				pi[(*no)++] = &nl[ptr->nl_id];
			}
			//PO
			else if (mode == 1) {
				//PO配列
				po[(*no)++] = &nl[ptr->nl_id];
			}
			break;
		}
		ptr = ptr->next;
	}
	//error : 信号がスルーしている場合
	if (ptr == NULL) error(3, word);
}

//------------------------------------------------------------------------
// assign読み込み（2006.03.24 未対応）
//------------------------------------------------------------------------
static void read_assign(FILE *rfp, int *assign_no)
{
	char str[STR_MAX]; //get_token用
	int c;

	get_token(rfp, str, 0, 0);
	c = fgetc(rfp);
	while (c != 'b') c = fgetc(rfp);
	while ((c != '0') && (c != '1')) c = fgetc(rfp);
	if ((c == '0') || (c == '1')) {
		nl[n_net].n = n_net;
		nl[n_net].name = (char *)malloc(sizeof(char)* (strlen(str) + 1));
		strcpy(nl[n_net].name, str);
		nl[n_net].n_in = 0;
		nl[n_net].n_out = 0;
		nl[n_net].in = NULL;
		nl[n_net].name_ins = NULL;
		nl[n_net].name_port = NULL;
		nl[n_net].out = NULL;
		if (c == '0') nl[n_net].type = GND;
		else if (c == '1') nl[n_net].type = ACC;
		while (c != ';') c = fgetc(rfp);

		n_net++;
	}
	else error(6, str);
}

//------------------------------------------------------------------------
// 構文エラー解析
//------------------------------------------------------------------------
static void analyze_asyntax_error(void)
{
	int i;

	for (i = 0; i<n_net; i++) {
		//PIバッティング
		if ((parser_array[1][i] >= 1) && (nl[i].type == IN))	error(4, nl[i].name);
		//内部信号バッティング
		else if (parser_array[1][i] >= 2) error(5, nl[i].name);;
	}
}

//------------------------------------------------------------------------
// ハッシュ値演算
//------------------------------------------------------------------------
static int hash_key(char *word)
{
	unsigned int key[2] = { 0, 0 };
	int i, slen, no;

	slen = strlen(word);
	for (i = 0; i<slen; i++) {
#ifdef HASH_MODE
		if (i % 2 == 0) {
			key[0] = 65599 * key[1] + word[i];
			no = 0;
		}
		else {
			key[1] = 65599 * key[0] + word[i];
			no = 1;
		}
#endif
#ifndef HASH_MODE
		key[0] += word[i];
		no = 0;
#endif
	}

	return (key[no] % HASH_SIZE);
}

//------------------------------------------------------------------------
// ハッシュ表解放
//------------------------------------------------------------------------
static void free_hash(void)
{
	Signal_Name_Hash *ptr;
	Signal_Name_Hash *temp;
	int i;

	for (i = 0; i<HASH_SIZE; i++) {
		ptr = hash_t[i];
		while (ptr != NULL) {
			temp = ptr;
			ptr = ptr->next;
			free(temp);
		}
		hash_t[i] = NULL;
	}
	free(hash_t);
}

//------------------------------------------------------------------------
// 信号接続制御
//------------------------------------------------------------------------
static void connection_signal(void)
{
	//各信号 n_in,n_out 読み込み,領域確保
	alloc_nl_in_out();

	//各信号接続
	connection_n_signal();

	//外部入力インスタンス名,端子名設定
	set_pi_ins();

	//FF配列挿入
	set_ff_array();
}

//------------------------------------------------------------------------
// 各信号 n_in,n_out 読み込み,領域確保
//------------------------------------------------------------------------
static void alloc_nl_in_out(void)
{
	int i, j;

	//各信号 n_in の読み込み
	for (i = 0; i<n_module; i++)
		nl[module_array[i]->out_nl_id].n_in = module_array[i]->m_n_in;

	//各信号 n_out の読み込み
	for (i = 0; i<n_net; i++) nl[i].n_out = parser_array[0][i];
	for (i = 0; i<n_po; i++) //POがFOUTしている場合
	if (parser_array[0][po[i]->n] >= 1) po[i]->n_out++;

	//各信号 in,out 領域確保,初期化
	for (i = 0; i<n_net; i++) {
		if (nl[i].n_in != 0) {
			nl[i].in = (NLIST **)malloc(sizeof(NLIST *)* nl[i].n_in);
			for (j = 0; j<nl[i].n_in; j++) nl[i].in[j] = NULL;
		}
		if (nl[i].n_out != 0) {
			nl[i].out = (NLIST **)malloc(sizeof(NLIST *)* nl[i].n_out);
			for (j = 0; j<nl[i].n_out; j++) nl[i].out[j] = NULL;
		}
	}
}

//------------------------------------------------------------------------
// 信号接続
//------------------------------------------------------------------------
static void connection_n_signal(void)
{
	int po_fout_flag; //POがFOUT（YES:1 NO:0）
	int i, j, k;

	//初期化
	po_fout_flag = 0;

	for (i = 0; i<n_module; i++) {
		for (j = 0; j<module_array[i]->m_n_in; j++) {
			//IN側がFOUTしていない場合
			if (nl[module_array[i]->in_nl_id[j]].n_out == 1) {
				connection_no_fout(i, j);
			}
			//IN側がFOUTしている場合
			else if (nl[module_array[i]->in_nl_id[j]].n_out >= 2) {
				connection_yes_fout(i, j);
				//POがFOUTしているかチェック
				for (k = 0; k<n_po; k++) {
					if (nl[module_array[i]->in_nl_id[j]].n == po[k]->n) {
						//PO配列を書き換える
						po[k] = &nl[n_net];
						po_fout_flag = 1; break;
					}
				}
				//POがFOUTしている場合
				if (po_fout_flag == 1) {
					connection_po_fout(i, j, k);
				}
			}
			po_fout_flag = 0;
		}
	}
}

//------------------------------------------------------------------------
// 信号接続 IN側がFOUTしていない場合
//------------------------------------------------------------------------
static void connection_no_fout(int m_no, int in_no)
{
	int no;

	//初期化
	no = 0;

	//IN側の出力信号
	nl[module_array[m_no]->in_nl_id[in_no]].out[0] = &nl[module_array[m_no]->out_nl_id];
	//OUT側の入力信号
	while (nl[module_array[m_no]->out_nl_id].in[no] != NULL) no++;
	nl[module_array[m_no]->out_nl_id].in[no] = &nl[module_array[m_no]->in_nl_id[in_no]];
	//OUT側のTYPE
	nl[module_array[m_no]->out_nl_id].type = module_array[m_no]->m_type;
	//OUT側のインスタンス名
	nl[module_array[m_no]->out_nl_id].name_ins
		= (char *)malloc(sizeof(char)* (strlen(module_array[m_no]->m_name_ins) + 1));
	strcpy(nl[module_array[m_no]->out_nl_id].name_ins, module_array[m_no]->m_name_ins);
	//OUT側の端子名
	nl[module_array[m_no]->out_nl_id].name_port = (char *)malloc(sizeof(char)* (1 + 1));
	if (nl[module_array[m_no]->out_nl_id].type == DFF || nl[module_array[m_no]->out_nl_id].type == RDFF ||
		nl[module_array[m_no]->out_nl_id].type == DFFS || nl[module_array[m_no]->out_nl_id].type == RDFFS) {
		strcpy(nl[module_array[m_no]->out_nl_id].name_port, "Q");
	}
	else
		strcpy(nl[module_array[m_no]->out_nl_id].name_port, "Z");
}

//------------------------------------------------------------------------
// 信号接続 IN側がFOUTしている場合
//------------------------------------------------------------------------
static void connection_yes_fout(int m_no, int in_no)
{
	char tmp[SPRINT_MAX]; //sprint用
	int no;

	//初期化
	no = 0;

	//FOUT作成
	sprintf(tmp, "%s_%s_%c", nl[module_array[m_no]->in_nl_id[in_no]].name
		, nl[module_array[m_no]->out_nl_id].name
		, pin_name_array[in_no]);
	nl[n_net].name = (char *)malloc(sizeof(char)* (strlen(tmp) + 1));
	strcpy(nl[n_net].name, tmp);
	nl[n_net].n = n_net;
	nl[n_net].type = FOUT;
	nl[n_net].n_in = 1;
	nl[n_net].n_out = 1;
	nl[n_net].in = (NLIST **)malloc(sizeof(NLIST *)); nl[n_net].in[0] = NULL;
	nl[n_net].out = (NLIST **)malloc(sizeof(NLIST *)); nl[n_net].out[0] = NULL;
	//元のIN側の出力信号
	while (nl[module_array[m_no]->in_nl_id[in_no]].out[no] != NULL) no++;
	nl[module_array[m_no]->in_nl_id[in_no]].out[no] = &nl[n_net];
	no = 0;
	//OUT側の入力信号
	while (nl[module_array[m_no]->out_nl_id].in[no] != NULL) no++;
	nl[module_array[m_no]->out_nl_id].in[no] = &nl[n_net];
	no = 0;
	//OUT側のTYPE
	nl[module_array[m_no]->out_nl_id].type = module_array[m_no]->m_type;
	//FOUTの入力信号
	nl[n_net].in[0] = &nl[module_array[m_no]->in_nl_id[in_no]];
	//FOUTの出力信号
	nl[n_net].out[0] = &nl[module_array[m_no]->out_nl_id];
	//FOUTのインスタンス名
	nl[n_net].name_ins
		= (char *)malloc(sizeof(char)* (strlen(module_array[m_no]->m_name_ins) + 1));
	strcpy(nl[n_net].name_ins, module_array[m_no]->m_name_ins);
	//FOUTの端子名
	if (nl[module_array[m_no]->out_nl_id].type == DFF || nl[module_array[m_no]->out_nl_id].type == RDFF ||
		nl[module_array[m_no]->out_nl_id].type == DFFS || nl[module_array[m_no]->out_nl_id].type == RDFFS) {
		if (in_no == 0) {
			nl[n_net].name_port = (char *)malloc(sizeof(char)* (1 + 1));
			nl[n_net].name_port[0] = 'D'; nl[n_net].name_port[1] = '\0';
		}
		else if (in_no == 1) {
			if (nl[module_array[m_no]->out_nl_id].type == RDFF) {
				nl[n_net].name_port = (char *)malloc(sizeof(char)* (2 + 1));
				strcpy(nl[n_net].name_port, "CD");
			}
			else if (nl[module_array[m_no]->out_nl_id].type == DFFS || nl[module_array[m_no]->out_nl_id].type == RDFFS) {
				nl[n_net].name_port = (char *)malloc(sizeof(char)* (2 + 1));
				strcpy(nl[n_net].name_port, "TI");
			}
		}
		else if (in_no == 2) {
			nl[n_net].name_port = (char *)malloc(sizeof(char)* (2 + 1));
			strcpy(nl[n_net].name_port, "TE");
		}
		else if (in_no == 3) {
			nl[n_net].name_port = (char *)malloc(sizeof(char)* (2 + 1));
			strcpy(nl[n_net].name_port, "CD");
		}
	}
	else {
		nl[n_net].name_port = (char *)malloc(sizeof(char)* (1 + 1));
		nl[n_net].name_port[0] = pin_name_array[in_no];	nl[n_net].name_port[1] = '\0';
	}
	//OUT側のインスタンス名
	nl[module_array[m_no]->out_nl_id].name_ins
		= (char *)malloc(sizeof(char)* (strlen(module_array[m_no]->m_name_ins) + 1));
	strcpy(nl[module_array[m_no]->out_nl_id].name_ins, module_array[m_no]->m_name_ins);
	//OUT側の端子名
	nl[module_array[m_no]->out_nl_id].name_port = (char *)malloc(sizeof(char)* (1 + 1));
	if (nl[module_array[m_no]->out_nl_id].type == DFF || nl[module_array[m_no]->out_nl_id].type == RDFF ||
		nl[module_array[m_no]->out_nl_id].type == DFFS || nl[module_array[m_no]->out_nl_id].type == RDFFS) {
		strcpy(nl[module_array[m_no]->out_nl_id].name_port, "Q");
	}
	else
		strcpy(nl[module_array[m_no]->out_nl_id].name_port, "Z");

	n_net++;
}

//------------------------------------------------------------------------
// 信号接続 POがFOUTしている場合 2010.09.07変更
//------------------------------------------------------------------------
static void connection_po_fout(int m_no, int in_no, int po_no)
{
	char tmp[SPRINT_MAX]; //sprint用
	int no;

	//初期化
	no = 0;

	//FOUT作成
	nl[n_net].name = (char *)malloc(sizeof(char)* (strlen(nl[module_array[m_no]->in_nl_id[in_no]].name) + 1));
	strcpy(nl[n_net].name, nl[module_array[m_no]->in_nl_id[in_no]].name);
	nl[n_net].n = n_net;
	nl[n_net].type = FOUT;
	nl[n_net].n_in = 1;
	nl[n_net].in = (NLIST **)malloc(sizeof(NLIST *));
	nl[n_net].in[0] = NULL;
	nl[n_net].n_out = 0;
	//元のIN側の出力信号
	while (nl[module_array[m_no]->in_nl_id[in_no]].out[no] != NULL) no++;
	nl[module_array[m_no]->in_nl_id[in_no]].out[no] = &nl[n_net];
	free(nl[module_array[m_no]->in_nl_id[in_no]].name);
	sprintf(tmp, "%s_stem", nl[n_net].name);
	nl[module_array[m_no]->in_nl_id[in_no]].name = (char *)malloc(sizeof(char)* (strlen(tmp) + 1));
	strcpy(nl[module_array[m_no]->in_nl_id[in_no]].name, tmp);
	//FOUTの入力信号
	nl[n_net].in[0] = &nl[module_array[m_no]->in_nl_id[in_no]];
	//FOUTのインスタンス名,端子名
	nl[n_net].name_ins
		= (char *)malloc(sizeof(char)* (strlen(nl[n_net].name) + 1));
	strcpy(nl[n_net].name_ins, nl[n_net].name);
	nl[n_net].name_port = NULL;

	n_net++;
}

//------------------------------------------------------------------------
// 外部入力インスタンス名,端子名設定
//------------------------------------------------------------------------
static void set_pi_ins(void)
{
	int i;

	for (i = 0; i<n_pi; i++) {
		pi[i]->name_ins = (char *)malloc(sizeof(char)* (strlen(pi[i]->name) + 1));
		strcpy(pi[i]->name_ins, pi[i]->name);
		pi[i]->name_port = NULL;
	}
}

//------------------------------------------------------------------------
// FF配列挿入
//------------------------------------------------------------------------
static void set_ff_array(void)
{
	int dff_no, dffs_no, rdff_no, rdffs_no;
	int i;

	//初期化
	dff_no = dffs_no = rdff_no = rdffs_no = 0;

	for (i = 0; i<n_net; i++) {
		switch (nl[i].type) {
		case DFF: dff[dff_no++] = &nl[i]; break;
		case DFFS: dffs[dffs_no++] = &nl[i]; break;
		case RDFF: rdff[rdff_no++] = &nl[i]; break;
		case RDFFS:	rdffs[rdffs_no++] = &nl[i]; break;
		}
	}
}

//------------------------------------------------------------------------
// 1単語読む+α
//------------------------------------------------------------------------
static int get_token(FILE *rfp, char *s, int mode, int *in_num)
{
	int aha_flag;     //isalpha判別用
	char c_in_num[3]; //入力数読み込み用
	int i, c;

	//初期化
	i = aha_flag = 0;

	//一文字読む
	c = fgetc(rfp);
	while ((c == ' ') || (c == '\t') || (c == '\n')) c = fgetc(rfp);
	if (c == EOF) return EOF;              //EOFの場合

	//区切り文字まで読み続け単語を作成
	if (mode == 0) {
		while ((c != '\t') && (c != '\n') && (c != ' ') && (c != ';') &&
			(c != '(') && (c != ')') && (c != ',') && (c != ':') && (c != '=') && (c != '.')) {
			s[i] = c;
			c = fgetc(rfp);
			i++;
		}
	}
	else if (mode == 1) {
		while ((c != '\t') && (c != '\n') && (c != ' ') && (c != ';') && (c != '[') && (c != ']') &&
			(c != '(') && (c != ')') && (c != ',') && (c != ':') && (c != '=') && (c != '.')) {
			s[i] = c;
			c = fgetc(rfp);
			i++;
		}
	}
	else if (mode == 2) {
		while ((c != '\t') && (c != '\n') && (c != ' ') && (c != ';') && (aha_flag = isalpha(c)) &&
			(c != '(') && (c != ')') && (c != ',') && (c != ':') && (c != '=') && (c != '.')) {
			s[i] = c;
			c = fgetc(rfp);
			i++;
		}
		if (aha_flag == 0) {
			c_in_num[0] = c;
			c = fgetc(rfp);
			if ((c != ' ') && (c != '\t')) { c_in_num[1] = c; c_in_num[2] = '\0'; }
			else c_in_num[1] = '\0';
			*in_num = atoi(c_in_num);
		}
	}
	s[i] = '\0';

	//どの区切り文字まで読んだかを返す
	if ((c == ';') && (i != 0)) return 1;      //最後の文字が「;」の場合
	else if ((c == ';') && (i == 0)) return 2; //最後の文字が「;」でかつ単独の場合
	else if (s[0] == '\0') return -2;      //文字列が作れていない場合
	else return 0;                        //通常
}

//------------------------------------------------------------------------
// エラー処理
//------------------------------------------------------------------------
static void error(int code, char *s)
{
	fprintf(stderr, "ERROR : ");
	switch (code) {
	case 1:
		fprintf(stderr, "2入力以外のEXOR,EXNORが存在します. instans_name : %s\n", s);
		break;
	case 2:
		fprintf(stderr, "11入力以上は対応していません. 注文により即対応？ instans_name : %s\n", s);
		break;
	case 3:
		fprintf(stderr, "信号 \"%s\" がスルーしています. バッファを挿入してください.\n", s);
		fprintf(stderr, "        もしくは, 信号 \"%s\" の記述中に改行が入っている可能性があります.\n", s);
		break;
	case 4:
		fprintf(stderr, "外部入力信号 \"%s\" がバッティングしています. いかれた回路を読み込ませないで下さい（怒）\n", s);
		break;
	case 5:
		fprintf(stderr, "信号 \"%s\" がバッティングします. いかれた回路を読み込ませないで下さい（怒）\n", s);
		break;
	case 6:
		fprintf(stderr, "assign文はACC,GNDのみ対応しています. 信号 \"%s\" を外部入力宣言から取る等で対応してください.\n", s);
		break;
	}

	exit(1);
}

//------------------------------------------------------------------------
// モジュール配列,構文解析配列 解放
//------------------------------------------------------------------------
static void free_module_array(void)
{
	int i, j;

	//モジュール配列
	for (i = 0; i<n_module; i++) {
		free(module_array[i]->m_out_name);
		free(module_array[i]->m_name_ins);
		free(module_array[i]->in_nl_id);
		for (j = 0; j<module_array[i]->m_n_in; j++)
			free(module_array[i]->m_in_name[j]);
		free(module_array[i]);
	}
	free(module_array);

	//構文解析配列
	free(parser_array[0]);
	free(parser_array[1]);
}

#ifdef DEBUG_MA
//------------------------------------------------------------------------
// モジュール配列デバッグ
//------------------------------------------------------------------------
static void debug_ma(void)
{
	int i, j;

	printf("n_module : %d\n", n_module);
	for (i = 0; i<n_module; i++) {
		printf("module_no:%d ", i);
		printf("instans_name:%s ", module_array[i]->m_name_ins);
		printf("type:%d ", module_array[i]->m_type);
		printf("out_name:%s ", module_array[i]->m_out_name);
		printf("n_in:%d\n", module_array[i]->m_n_in);
		printf("in_name : ");
		for (j = 0; j<module_array[i]->m_n_in; j++) {
			printf("[%d]:%s ", j, module_array[i]->m_in_name[j]);
		}
		printf("\n");
	}
	printf("\n");
}
#endif

#ifdef DEBUG_NL
//------------------------------------------------------------------------
// ネットリスト配列デバッグ
//------------------------------------------------------------------------
static void debug_nl(void)
{
	int i, j;

	printf("n_net : %d\n", n_net);
	printf("n_pi : %d ", n_pi);
	for (i = 0; i<n_pi; i++) {
		printf("pi[%d]:%s ", i, pi[i]->name);
	}
	printf("\n");
	printf("n_po : %d ", n_po);
	for (i = 0; i<n_po; i++) {
		printf("po[%d]:%s ", i, po[i]->name);
	}
	printf("\n");
	printf("n_dff : %d ", n_dff);
	for (i = 0; i<n_dff; i++) {
		printf("dff[%d]:%s ", i, dff[i]->name);
	}
	printf("\n");
	printf("n_rdff : %d ", n_rdff);
	for (i = 0; i<n_rdff; i++) {
		printf("rdff[%d]:%s ", i, rdff[i]->name);
	}
	printf("\n");
	printf("n_dffs : %d ", n_dffs);
	for (i = 0; i<n_dffs; i++) {
		printf("dffs[%d]:%s ", i, dffs[i]->name);
	}
	printf("\n");
	printf("n_rdffs : %d ", n_rdffs);
	for (i = 0; i<n_rdffs; i++) {
		printf("rdffs[%d]:%s ", i, rdffs[i]->name);
	}
	printf("\n\n");

	for (i = 0; i<n_net; i++) {
		printf("nl[%d] name : %s\n", nl[i].n, nl[i].name);
		printf("type : %d ins_name : %s port_name : %s\n", nl[i].type, nl[i].name_ins, nl[i].name_port);
		printf("n_in : %d ", nl[i].n_in);
		for (j = 0; j<nl[i].n_in; j++) {
			printf("in[%d]:%s ", j, nl[i].in[j]->name);
		}
		printf("\n");
		printf("n_out : %d ", nl[i].n_out);
		for (j = 0; j<nl[i].n_out; j++) {
			printf("out[%d]:%s ", j, nl[i].out[j]->name);
		}
		printf("\n");
	}
	printf("\n");
}
#endif

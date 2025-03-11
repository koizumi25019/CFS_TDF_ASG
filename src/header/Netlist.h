//------------------------------------------------------------------------
//Netlist 定義, 外部変数, プロトタイプ宣言
//File name : Nelist.h
//Date : 2006/03/26
//Designer : R.Inoue
//Ver : 3.00（配布Ver）
//------------------------------------------------------------------------
#pragma once
#include<stdbool.h>
//------------------------------------------------------------------------
// 定義
//------------------------------------------------------------------------
//ゲートタイプ
#define  IN		0
#define  BUF    10
#define  INV	11
#define  AND	12
#define  NAND	13
#define  OR		14
#define  NOR	15
#define  EXOR	16 //2入力のみ可
#define  EXNOR	17 //2入力のみ可
#define  FOUT	18
#define  DFF	19 //D-フリップフロップ
#define  RDFF	21 //D-フリップフロップ（リセット有）
#define  DFFS	23 //D-スキャンフリップフロップ
#define  RDFFS	24 //D-スキャンフリップフロップ（リセット有）
#define  GND	25 //グランド
#define  ACC	26 //電源

//ネットリスト構造体定義
typedef struct _Netlist_Format_ {
	char *name;						//名前
	int n;							//ID（ユニーク）
	int type;						//タイプ
	int n_in;						//入力数
	struct _Netlist_Format_ **in;	//入力ポインタ配列
	int n_out;						//出力数
	struct _Netlist_Format_ **out;	//出力ポインタ配列
	char *name_port;				//端子名 pin名
	char *name_ins;					//インスタンス名
	int level;						//レベル付け

	int open_pass;					//オープンパス
	int fault_pass;					//故障到達可能パス

	int flag;					    //フラグ
	int ppo_flag;				    //擬似外部出力フラグ
	int tf_flag;					//目標故障フラグ

	int val_1t;						//1時刻目の論理値
	int val_2t;						//2時刻目の論理値

	unsigned int suf_1t;			//1時刻目の添え字
	unsigned int suf_2t;			//2時刻目の添え字
	unsigned int suf_fc;			//故障回路の添え字

	//koizumi追加(from ASG)
	unsigned int	varsgc;			    /**< variables for good-circuit  */
	unsigned int	varsfc;				/**< variables for faulty-circuit  */
	char* consgc;						/**< constraints for good-circuit  */
	char** consfc;

} NLIST;

//------------------------------------------------------------------------
// 外部変数
//------------------------------------------------------------------------
//ネットリスト
NLIST *nl;

//モジュール名
char *module_name;

//信号数
int n_net;

//外部入力数, 外部入力
NLIST **pi;
int n_pi;

//疑似外部出力数, 疑似外部出力
NLIST **ppi;
int n_ppi;

//外部出力数, 外部出力
NLIST **po;
int n_po;

//疑似外部出力数, 疑似外部出力
NLIST **ppo;
int n_ppo;

//DFF, DFF数
NLIST **dff;
int n_dff;

//RDFF, RDFF数
NLIST **rdff;
int n_rdff;

//DFFS, DFFS数
NLIST **dffs;
int n_dffs;

//RDFFS, RDFFS数
NLIST **rdffs;
int n_rdffs;

//assign
NLIST **assign;
int n_assign;

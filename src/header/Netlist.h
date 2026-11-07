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

	unsigned int suf_1t;			//1時刻目のブール変数
	unsigned int suf_2t;			//2時刻目のブール変数
	unsigned int suf_fc;			//故障回路のブール変数
	//追加
	unsigned int prop;              //故障伝搬変数のブール変数

	//追加
	unsigned int	varsgc;			    /**< variables for good-circuit  */
	unsigned int	varsfc;				/**< variables for faulty-circuit  */
	char* consgc;						/**< constraints for good-circuit  */
	char** consfc;

} NLIST;

//------------------------------------------------------------------------
// 外部変数(実体は globals.c)
extern NLIST *nl;					//ネットリスト(信号線の配列)
extern char *module_name;			//モジュール名
extern int n_net;					//信号線数
extern NLIST **pi;					//外部入力
extern int n_pi;
extern NLIST **ppi;					//疑似外部入力(FFの出力)
extern int n_ppi;
extern NLIST **po;					//外部出力
extern int n_po;
extern NLIST **ppo;					//疑似外部出力(FFの入力)
extern int n_ppo;
extern NLIST **dff;					//DFF
extern int n_dff;
extern NLIST **rdff;				//RDFF
extern int n_rdff;
extern NLIST **dffs;				//DFFS
extern int n_dffs;
extern NLIST **rdffs;				//RDFFS
extern int n_rdffs;
extern NLIST **assign;				//assign
extern int n_assign;

//------------------------------------------------------------------------
//  共通の定義: 状態コード・故障タイプ・変数番号・グローバル変数の宣言・主な関数の宣言
//  グローバル変数の実体は globals.c にある(ここでは extern で宣言するだけ)
//------------------------------------------------------------------------
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Netlist.h"
#include "Stack.h"
#include "Fault.h"
#include "../opt/opt.h"
#include "../lib/lib.h"
#include "../asg/read.h"

//------------------------------------------------------------------------
// 状態コード
//------------------------------------------------------------------------
#define		NO				0
#define		YES				1
#define		ERROR			-1
#define		SUCCESS			0

//------------------------------------------------------------------------
// 故障のタイプ
//------------------------------------------------------------------------
#define		STF				0		//STF(1->0)
#define		STR				1		//STR(0->1)

//------------------------------------------------------------------------
// ブール変数の番号(clasp に渡す問題の変数)
//   1 .. LFSR_BITS                 : LFSR(シード)
//   VAR_FIRST_CIRCUIT ..           : 外部入力・FF・各信号線の1時刻目の値(suf_1t)、続けて2時刻目の値(suf_2t)。それぞれ n_net 個
//   VAR_FIRST_FAULT_CIRCUIT ..     : 故障回路・故障検出回路・緩和変数・WSAの変数(目標故障ごとに割り当てる)
//------------------------------------------------------------------------
#define		VAR_FIRST_CIRCUIT		(LFSR_BITS + 1)
#define		VAR_FIRST_FAULT_CIRCUIT	(VAR_FIRST_CIRCUIT + 2 * n_net)

//------------------------------------------------------------------------
// グローバル変数
//------------------------------------------------------------------------
/* 回路 */
extern NLIST** sort_nl;				//レベル順に並べた信号線
extern STACK Level;					//レベライズ用のスタック
extern STACK Open;					//オープンパス用のスタック
extern QUE Que;						//キュー(故障範囲の探索用)
extern unsigned int bool_id;		//clasp 用ブール変数IDの割り当てカウンタ
extern unsigned int fc_ppo;			//故障回路に含まれるPPO数

/* 出力フォルダー(末尾に区切りを含む) */
extern char* folder_out;

/* 故障リスト(遷移故障)。ufl は検出するたびに組み直される(検出した故障は取り除かれる) */
extern int all_num;					//全故障数
extern int ufl_num;					//未検出故障数
extern UNDET_FAULT_LIST* ufl;		//未検出故障リスト
extern int dfl_num;					//検出故障数
extern UNDET_FAULT_LIST* dfl;		//検出故障リスト
extern int unseed_num;				//シード生成不能故障数
extern UNDET_FAULT_LIST* unseed;	//シード生成不能故障リスト(ソルバが解を求められなかった一次故障)
extern int abortfl_num;				//打ち切り故障数
extern UNDET_FAULT_LIST* abortfl;	//打ち切り故障リスト(MTTGが求まらず、STTGで検出できた故障)
extern int hpfl_num;				//高消費電力故障数
extern UNDET_FAULT_LIST* hpfl;		//高消費電力故障リスト(緩和したWSA閾値でないと検出できなかった故障)
extern int tfl_num;					//目標故障数
extern UNDET_FAULT_LIST* tfl;		//目標故障リスト(先頭が一次故障)

/* 実行結果の集計 */
extern int seed_num;				//生成したシード数
extern int seed_relaxed_num;		//そのうち、緩和したWSA閾値で生成したシード数
extern int wsa_used_max;			//使ったWSA閾値の最大値[%]

//------------------------------------------------------------------------
// 関数
//------------------------------------------------------------------------
//Level_stack.c
void level_push(NLIST* p);
NLIST* level_pop(void);
//levelize.c
int levelize(void);
int judge_in_level(NLIST **in_pin, int n);
//Qsort.c
void Qsort(void);
//set_literal_id.c
void set_literal_id(void);
//init_exturn.c
int init_exturn(void);
//make_ppo_ppi.c
void make_ppo_ppi(void);
//Netlist.c
void read_nl(char *name);
//validate_gates.c
int validate_circuit_gates(void);
//pin_make.c
int pin_make(void);
//Que.c
void enqueue(NLIST *data);
NLIST* dequeue(void);
int queue_emp(void);
//reset_flag.c
void reset_flag(void);
//check_open_pass.c
int check_open_pass(void);
//open_stack.c
void open_push(NLIST* p);
NLIST* open_pop(void);
//transition_delay_fault_set.c
int transition_delay_fault_set(void);
//fault_select.c
int fault_select_targets(void);
int fault_select_single(const UNDET_FAULT_LIST* fault);
//mttg_main.c
int mttg_main(void);

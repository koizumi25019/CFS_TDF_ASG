//------------------------------------------------------------------------
//  グローバル変数の実体
//  各ヘッダには extern 宣言だけを書き、実体はこのファイルに1回だけ定義する
//  (ヘッダに実体を書くと、GCC 10 以降などでは「多重定義」のリンクエラーになる)
//------------------------------------------------------------------------
#include "Define.h"
#include "../asg/opb/opb.h"
#include "../asg/opb/clasp/clasp.h"

/* ---- Netlist.h: 回路 ---- */
NLIST* nl;
char* module_name;
int n_net;
NLIST** pi;
int n_pi;
NLIST** ppi;
int n_ppi;
NLIST** po;
int n_po;
NLIST** ppo;
int n_ppo;
NLIST** dff;
int n_dff;
NLIST** rdff;
int n_rdff;
NLIST** dffs;
int n_dffs;
NLIST** rdffs;
int n_rdffs;
NLIST** assign;
int n_assign;

/* ---- Define.h: 作業用 ---- */
NLIST** sort_nl;
STACK Level;
STACK Open;
QUE Que;
unsigned int bool_id;
unsigned int fc_ppo;
char* folder_out;

/* ---- Define.h: 故障リストと集計 ---- */
int all_num;
int ufl_num;
UNDET_FAULT_LIST* ufl;
int dfl_num;
UNDET_FAULT_LIST* dfl;
int untest_num;
UNDET_FAULT_LIST* untestfl;
int unseed_num;
UNDET_FAULT_LIST* unseed;
int abortfl_num;
UNDET_FAULT_LIST* abortfl;
int hpfl_num;
UNDET_FAULT_LIST* hpfl;
int tfl_num;
UNDET_FAULT_LIST* tfl;
int seed_num;
int seed_relaxed_num;
int wsa_used_max;

/* ---- その他のヘッダ ---- */
BIT_INT* detflag;		//lib.h
READDATA readdata;		//read.h
clock_t time_graph;		//read.h
OPB opb;				//opb.h
CLASPLOG clasp;			//clasp.h

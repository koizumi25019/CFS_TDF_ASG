#include"Command_Info.h"
#include"Stack.h"
#include<time.h>
//追加
#include "../asg/target.h"
//------------------------------------------------------------------------
// 実行ステータス
//------------------------------------------------------------------------
#define		NO				0
#define		YES				1
#define     ERROR           -1
#define     FAILURE			-1
#define		SUCCESS			0	
#define		SAT			    1
#define		UNSAT			0


//------------------------------------------------------------------------
// フラグ
//------------------------------------------------------------------------
#define		ON				1
#define		OFF				0
#define		FLAG_DOWN		0
#define		FLAG_UP		    1


//------------------------------------------------------------------------
//　ファイル情報
//------------------------------------------------------------------------

/* 入力ファイル */
#define		CIRCUIT			0

/* オプション */
#define		RAND			1
#define		TIME			2
#define		LOW_POWER		4
#define		WSATH			5
#define		TARGET_LIMIT	6
#define		FAULT	        7
#define     FOLDER          8
#define     MODE_1          9
#define     PARALLEL       10

//------------------------------------------------------------------------
//all0 all1
//------------------------------------------------------------------------
#define		all1		4294967295	//論理値1のときの値
#define		all0		0			//論理値0のときの値

//------------------------------------------------------------------------
//故障のタイプ
//------------------------------------------------------------------------
#define		STF		    0		   //STF(1->0)
#define		STR			1          //STR(0->1)

//------------------------------------------------------------------------
//ソートテーブル
//------------------------------------------------------------------------
NLIST** sort_nl;
STACK Level;			//レベライズ用のスタック

//------------------------------------------------------------------------
//出力ファイル
//------------------------------------------------------------------------
FILE* fout_tp;			   //テストパターン集合ファイル
FILE* fout_seed;           //seedファイル

FILE* fout_fault;		   //故障集合ファイル
FILE* fout_pin_input;	   //入力ピンファイル
FILE* fout_pin;			   //入力ピンファイル(故障シミュレーション)
FILE* fout_log;			   //ログファイル
FILE* fout_undet;		   //最終未検出故障ファイル(空ファイル)
FILE* fout_det;			   //最終検出故障ファイル(＝初期未検出故障集合ファイル)

FILE* fout_untest_tp;		//テスト不能故障削除時のテストパターン集合ファイル
FILE* fout_untest_fault;	//テスト不能故障集合ファイル
FILE* fout_high_fault;		//高消費電力故障集合ファイル
FILE* fout_init_undet;		//初期未検出集合ファイル
FILE* fout_untest_pbo;		//テスト不能故障削除用のSTTGのPBOファイル
FILE* fout_literal;			//テスト不能削除前のリテラル情報

FILE* fout_mttg_pbo;		//MTTGのPBOファイル
FILE* fout_mini_pbo;		//目標故障選択時のminiPBO
FILE* fout_mini_pbo_copy;	//目標故障選択時の1つ前のファイル保存用
FILE* fout_fsim_result;	    //故障辞書ファイル
FILE* fout_mttg;	        //1TP当たりで検出された故障数ファイル
FILE* fout_log;				//ログファイル
FILE* fout_tfl_log;         //目標故障集合ログファイル

//------------------------------------------------------------------------
//入力ファイル
//------------------------------------------------------------------------
FILE* fin_untest_pbo_result;	//テスト不能故障削除用のSTTGのPBO結果ファイル
FILE* fin_mini_pbo_result;	    //目標故障選択用のPBO結果ファイル

//------------------------------------------------------------------------
//フォルダー指定								
//------------------------------------------------------------------------
char* folder_out;		//出力ファイル用のフォルダー先

//------------------------------------------------------------------------
//遷移故障用
//------------------------------------------------------------------------
int all_num;					//全故障数
UNDET_FAULT_LIST *afl;			//全故障リスト
int dfl_num;					//検出故障数
UNDET_FAULT_LIST *dfl;			//検出故障リスト
int ufl_num;					//未検出故障数
UNDET_FAULT_LIST *ufl;			//未検出故障リスト
int red_num;					//テスト不能故障数
UNDET_FAULT_LIST *red;			//テスト不能故障リスト
int hpfl_num;					//高消費電力故障数
UNDET_FAULT_LIST *hpfl;			//高消費電力故障リスト
int node_num;                   //総故障信号線数 
int tfl_num;					//目標故障数
UNDET_FAULT_LIST* tfl;			//目標故障リスト
UNDET_FAULT_LIST** tfl_node;	//目標故障の信号線集合



//------------------------------------------------------------------------
//その他							
//------------------------------------------------------------------------
#define		X	-1				//ドントケア
unsigned int suffix;			//添え字
STACK Open;						//オープンパス用のスタック
unsigned int fc_ppo;			//PPO数初期化
QUE Que;						//キュー
int tp_num;						//テストベクトル数

//------------------------------------------------------------------------
//プロトタイプ宣言
//------------------------------------------------------------------------
//Level_stack.c
void level_push(NLIST* p);
NLIST* level_pop(void);
//levelize.c
int levelize(void);
int judge_in_level(NLIST **in_pin, int n);
//Qsort.c
void Qsort(void);
int compare_int(void *a, void *b);
//set_literal_id.c
void set_literal_id(void);
//cmdline.c
int cmdline(int argc, char *argv[]);
//init_exturn.c
int init_exturn(void);
//make_ppo.c
void make_ppo_ppi(void);
//Netlist.c
void read_nl(char *name);
//and.c
int and(NLIST *net, int flag);
//fout.c
int fout(NLIST *net, int flag);
//inv.c
int inv(NLIST *net, int flag);
//nand.c
int nand(NLIST *net, int flag);
//nor.c
int nor(NLIST *net, int flag);
//or.c
int or (NLIST *net, int flag);
//logic_sim.c
int logic_sim(void);
//logic_sim_1time.c
int logic_sim_1time(void);
//logic_sim_2time.c
int logic_sim_2time(void);
//ppo_ppi_cat.c
int ppo_ppi_cat(void);
//test_v_read.c
int test_v_read(void);
//fault_judge.c
int fault_judge(void);
//file_output.c
int file_output(clock_t time, int mode);
//make_test_set.c
void make_test_set(FILE *fp_ts);
//pin_make.c
int pin_make(void);
//Que.c
void enqueue(NLIST *data, int mode);
NLIST* dequeue(void);
int queue_emp(void);
//reset_flag.c
void reset_flag(void);
//standard_output.c
void standard_output(void);
//check_open_pass.c
int check_open_pass(void);
//open_stack.c
void open_push(NLIST* p);
NLIST* open_pop(void);
//transition_delay_fault_set.c
int transition_delay_fault_set(void);
//fault_select.c
int fault_select(int unsat_flag,SORTED* sorted,int loop);
//mini_calc_pbo_size.c
PBO mini_calc_pbo_size(void);
int mini_calc_hard_constraint(void);
//mini_pbo_generate.c
void mini_pbo_generate_min(FILE* fp);
//untest_calc_pbo_size.c
PBO untest_calc_pbo_size(UNDET_FAULT_LIST* node);
int untest_calc_hard_constraint(UNDET_FAULT_LIST* node);
int untest_calc_soft_constraint(void);
//untest_check_wsa.c
int untest_check_wsa(UNDET_FAULT_LIST* node, UNDET_FAULT_LIST* pre_node);
//untest_pbo_generation.c
void untest_pbo_generate_min(FILE* fp, UNDET_FAULT_LIST* node);
void pbo_generate_normal_c(FILE *fp);
void pbo_generate_fault_c(FILE *fp, UNDET_FAULT_LIST* node);
void pbo_generate_fdetect_c(FILE *fp, UNDET_FAULT_LIST* node);
void pbo_generate_power(FILE *fp);
//unsat_dicision.c
int unsat_dicision(UNDET_FAULT_LIST* node, UNDET_FAULT_LIST* pre_node,int mode);
//untest_delete_main.c
int untest_delete_main(void);
//fault_select.c
int fault_select(int unsat_flag,SORTED* sorted,int loop);
//mttg_calc_pbo_size.c
PBO mttg_calc_pbo_size(void);
PBO mttg_calc_hard_constraint(void);
int mttg_calc_soft_constraint(void);
//mttg_check_wsa.c
int mttg_check_wsa(void);
//mttg_main.c
int mttg_main(void);
//mttg_pbo_generate.c
void mttg_pbo_generate_min(FILE* fp);
void pbo_generate_fdetect_c_mttg(FILE *fp, UNDET_FAULT_LIST* node);
void pbo_generate_fault_ex(FILE *fp, UNDET_FAULT_LIST* node, int mode);
void pbo_generate_power_mttg(FILE *fp);
//specific_fault_range.c
int specific_fault_range(char* name);
//undet_fault_delete.c
int undet_fault_delete(UNDET_FAULT_LIST* node);
void ufl_free(UNDET_FAULT_LIST* node, UNDET_FAULT_LIST* pre_node,int mode);
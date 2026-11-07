#pragma once
#include"Netlist.h"
#include"../lib/lib.h"

//------------------------------------------------------------------------
//未検出故障集合
//------------------------------------------------------------------------
typedef struct Undet_fault_list{
	int type;						   //故障タイプ(STF:1->0 or STR:0->1)
	char name[256];					   //故障信号線名
	NLIST* netlist;	                   //ネットリスト
	int node_id;                       //信号線ID
	int tfl_det_suffix;			       //目標故障時の故障検出回路のブール変数番号
	int tfl_suf_fc;                    //suf_fcは目標故障ごとに変わるので格納
	int tfl_fc_ppo;					   //故障回路に含まれるPPO数算出
	struct Undet_fault_list* next;	   //後続アドレス
	//追加 fromASG_read.h_FNODE構造体
	char* string;                      //string
	char* nece;						   //必須割り当て情報ファイルを1行ずつ格納
	BIT_INT_XP* necenet;			   //bit int necessary
	BIT_INT* edge;				       //compatible edge
	int n_edge;                        //繋がっている辺の数
	//追加
	//int resolving_count;               //再シード生成回数
}UNDET_FAULT_LIST;

//------------------------------------------------------------------------
//PBOのサイズ
//------------------------------------------------------------------------
typedef struct Size_Pbo{
	int val;		  //変数の数
	int h_constraint; //制約数
	int s_constraint; //ソフト制約数
	int lit;		  //リテラル数
}PBO;

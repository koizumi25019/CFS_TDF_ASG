//------------------------------------------------------------------------
//  PBOの制約式の生成(mttg_main.c から使う)
//------------------------------------------------------------------------
#pragma once
#include <stdio.h>
#include "../header/Fault.h"

/** 故障箇所 site から到達できる範囲(故障回路)を求め、各信号線に故障回路の変数番号 suf_fc を割り当てる。
    idx: 目標故障の番号(0 が一次故障)。-fp のときは一次故障の各信号線に故障伝搬変数 prop も割り当てる。 */
int build_fault_cone(NLIST* site, int idx);

/** 正常回路(1時刻目・2時刻目)の制約式 */
void pbo_generate_good_circuit(FILE* fp);

/** 故障回路(build_fault_cone の範囲)の制約式 */
void pbo_generate_faulty_circuit(FILE* fp, const UNDET_FAULT_LIST* node);

/** 故障検出回路(正常値と故障値の XOR を OR でまとめる)の制約式。node に検出変数の番号を記録する */
void pbo_generate_fault_detection(FILE* fp, UNDET_FAULT_LIST* node);

/** 故障伝搬変数の制約式(一次故障のみ。-fp) */
void pbo_generate_fault_propagation(FILE* fp, const UNDET_FAULT_LIST* node);

/** 故障励起の制約式。primary=1: 必ず励起する(ハード), 0: 緩和変数つき(ソフト) */
void pbo_generate_fault_excitation(FILE* fp, const UNDET_FAULT_LIST* node, int primary);

/** 低消費電力の制約式(信号線の遷移を数え、WSA が閾値[%]以下であること) */
void pbo_generate_power(int wsa_threshold, FILE* fp);

/** 低消費電力の制約で遷移を数える信号線の数(FFから到達できる信号線) */
int pbo_count_transition_nets(void);

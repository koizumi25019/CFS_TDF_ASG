//------------------------------------------------------------------------
//  検出できなかった故障の原因の判定と、検出できない故障の事前除外(-untest_mode)
//
//  原因は、制約を1つずつ外した単一目標の問題を解いて調べる:
//    1. LFSR+フェーズシフタを外す(消費電力の制約は残す)  解がある -> シード生成不能(LFSR/PS のモデルが原因)
//    2. 消費電力の制約も外す                             解がある -> WSA閾値のために検出できない
//                                                         解なし   -> 冗長(回路上、検出するテストが無い)
//  どこかで打ち切りになったら、判定できない。
//------------------------------------------------------------------------
#pragma once
#include "../header/Fault.h"

typedef enum {
	DIAG_ERROR = -1,		//ソルバの実行に失敗した
	DIAG_REDUNDANT = 0,		//冗長
	DIAG_UNSEEDABLE,		//シード生成不能(LFSR+フェーズシフタ)
	DIAG_WSA,				//WSA閾値のために検出できない
	DIAG_UNKNOWN			//判定できない(打ち切り)
} DiagVerdict;

/** 一次故障 fault が、全ての制約を入れた問題で解なしだったときの原因を調べる。
    wsa_threshold: そのとき使った閾値[%](負: 消費電力の制約なし) */
DiagVerdict diagnose_unsat(const UNDET_FAULT_LIST* fault, int wsa_threshold);

/** 故障 fault を verdict に応じたリスト(untestfl / unseed / wsafl / unknownfl)に移し、ufl から外す。SUCCESS / ERROR */
int diagnose_commit(const UNDET_FAULT_LIST* fault, DiagVerdict verdict);

/** 判定の名前(ログ用) */
const char* diagnose_verdict_text(DiagVerdict verdict);

/** -untest_mode: 最初に全ての故障を単独で調べ、冗長な故障とWSA閾値のために検出できない故障を ufl から除く。SUCCESS / ERROR */
int untest_mode_run(void);

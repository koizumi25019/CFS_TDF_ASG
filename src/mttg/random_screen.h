//------------------------------------------------------------------------
//  ランダムパターンによる、検出できる故障(冗長でない故障)の先行判定
//  ランダムなテストパターンを XID2.exe で故障シミュレーションし、検出できた故障を「テストが存在する」と確定する。
//  検出した故障を検出済みにはしない(パターンは保存しない)。冗長判定(単一目標)の対象から外すためだけに使う。
//------------------------------------------------------------------------
#pragma once
#include "../header/Fault.h"

/** ランダムパターン opt.random_patterns 個を故障シミュレーションする。SUCCESS / ERROR */
int random_screen(void);

/** random_screen でテストが存在すると確定した故障か */
int random_screen_known_testable(const UNDET_FAULT_LIST* fault);

/** 記録を解放する */
void random_screen_release(void);

//------------------------------------------------------------------------
//  PBOの問題ファイル(mttg_pbo.txt)の作成
//------------------------------------------------------------------------
#pragma once
#include <stdbool.h>

/** どんな問題を作るか */
typedef struct {
	bool with_lfsr;			/**< LFSR+フェーズシフタの制約を入れる(false: テストパターンだけを求める) */
	int  wsa_threshold;		/**< WSA閾値[%]。負なら消費電力の制約を入れない */
} ProblemSpec;

/** 目標故障集合 tfl(先頭が一次故障)について、spec の問題ファイルを出力フォルダーの mttg_pbo.txt に作る。
    SUCCESS / ERROR */
int problem_write(const ProblemSpec* spec);

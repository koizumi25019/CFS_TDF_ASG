//------------------------------------------------------------------------
//  clasp の出力(解)の読み取り
//------------------------------------------------------------------------
#pragma once
#include <stdbool.h>
#include <stdio.h>

#define N_SOL             2
#define SOL_SEED          0				//解のうち、LFSRのシード
#define SOL_TP            1				//解のうち、テストパターン(外部入力と疑似外部入力)

#define CLASP_RETRY_COUNT 3			//-regen: 打ち切り時間 60/180/300秒 で最大3回

#define CLASP_OKAY		  true
#define CLASP_ERROR		  false

/** clasp の結果 */
#define CLASP_STATUS_ERROR	-1			//出力を読めない・解が不完全
#define CLASP_STATUS_UNSAT	-2			//解なし(制約を満たす割当てが存在しない)
#define CLASP_STATUS_UNKNOWN -8			//打ち切り時間までに解が見つからなかった
#define CLASP_STATUS_SAT	7			//解が見つかった(打ち切りで最適性が未確認のものを含む)

typedef struct CLASP_LOG_
{
	int					  status;			  /**< CLASP_STATUS_* */
	char** sol;				  /**< solution: sol[SOL_SEED], sol[SOL_TP] (0/1 の文字列) */
}
CLASPLOG;

extern CLASPLOG			  clasp;			  /**< 直近の clasp の結果 */

/** clasp.exe を1回実行する(attempt_index: 0,1,2 = 打ち切り時間の延長の段階。-regen のとき 60/180/300秒) */
int clasp_call(int attempt_index);

/** clasp を実行して結果を clasp に読む(-regen のときは打ち切り時間を延ばして再試行) */
bool CLASP(void);

/** ログから状態 clasp.status を読む */
void CLASPgetStatus(FILE* fileptr);

/** ログから最後の解を読み、clasp.sol に格納する */
void CLASPgetSol(FILE* fileptr);

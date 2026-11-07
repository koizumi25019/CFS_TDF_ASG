//------------------------------------------------------------------------
//  故障シミュレーション結果(fsim_log.txt)から、検出できた故障を未検出故障から除く
//------------------------------------------------------------------------
#pragma once

/** 一次故障を検出できたシードの種類(検出した故障の分類に使う) */
typedef enum {
	DETECT_NORMAL = 0,			//複数目標(MTTG)で、指定のWSA閾値のまま
	DETECT_AFTER_STTG,			//MTTGで求まらず、単一目標(STTG)で検出(閾値は指定のまま) -> 打ち切り故障
	DETECT_RELAXED_WSA			//緩めたWSA閾値で検出 -> 高消費電力故障
} DetectClass;

typedef struct {
	int detected_count;			//今回のシードで検出した故障数(未検出だったもの)
	int detected_target_count;	//そのうち目標故障だった数
} JudgeResult;

/** fsim_log.txt を読み、検出した故障を ufl から dfl へ移す。
    detect_class が DETECT_NORMAL 以外のとき、一次故障(tfl の先頭)を abortfl / hpfl にも記録する。
    目標故障リスト tfl は解放しない(呼び出し側が行う)。戻り値: SUCCESS / ERROR */
int fault_judge(DetectClass detect_class, JudgeResult* result);

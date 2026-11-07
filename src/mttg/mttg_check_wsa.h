//------------------------------------------------------------------------
//  生成したテストパターンのキャプチャ時消費電力(WSA)の確認
//------------------------------------------------------------------------
#pragma once

typedef enum {
	WSA_WITHIN_THRESHOLD = 0,	//閾値以下(低消費電力のテストパターン)
	WSA_EXCEEDS_THRESHOLD,		//閾値を超える
	WSA_TOOL_ERROR				//WSA.exe を実行できない・結果を読めない
} WsaResult;

/** tp.txt のテストパターンを WSA.exe で調べる。threshold: WSA閾値[%]。
    原因が分かるようにメッセージを出す(閾値超過・実行失敗) */
WsaResult mttg_check_wsa(int threshold);

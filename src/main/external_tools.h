//------------------------------------------------------------------------
//  外部ツール(clasp.exe / XID2.exe / WSA.exe)の呼び出し
//------------------------------------------------------------------------
#pragma once

#define TOOL_CLASP		"clasp.exe"		//擬似ブール最適化ソルバ
#define TOOL_FSIM		"XID2.exe"		//遷移故障の故障シミュレータ
#define TOOL_WSA		"WSA.exe"		//キャプチャ時消費電力(WSA)の計算

/** コマンドを実行して終了コードを返す。起動できなければ -1 */
int run_external(const char* command);

/** 必要なツールが見つかるか、実行の最初に確認する。無ければ名前を挙げて ERROR */
int check_external_tools(void);

//------------------------------------------------------------------------
//  オプション(コマンドライン引数と設定ファイル .set)
//------------------------------------------------------------------------
#pragma once
#include <stdio.h>
#include <stdbool.h>

#define	OPT_OKAY				  true		  /**< return code   = okay */
#define	OPT_ERROR				  false		  /**< return code   = error */

//------------------------------------------------------------------------
//  既定値・制限値
//------------------------------------------------------------------------
#define DEFAULT_WSA_THRESHOLD	  20		  /**< WSA閾値の既定値[%]。論文 5章の設定値 */
#define DEFAULT_SOLVER_TIME		  10		  /**< ソルバの打ち切り時間の既定値[s] */
#define LFSR_BITS				  100		  /**< LFSRのビット数(-lfsr ON のとき)。回路モデルの変数番号がこの値を前提にしている */

//------------------------------------------------------------------------
//  オプションの値
//------------------------------------------------------------------------
typedef struct Option
{
	/* 入力ファイル */
	char* circuit;			  /**< -net          ネットリスト(.v) */
	char* fault_file;		  /**< -fault        RPR故障集合 */
	char* xor_tap_file;		  /**< -xor_tap      フェーズシフタのXORタップ */
	char* scan_chain_file;	  /**< -scan_chain   スキャンチェーン */

	/* 出力先 */
	char* folder;			  /**< -folder       出力フォルダー(存在しないこと) */

	/* 生成するもの */
	bool  lfsr;				  /**< -lfsr         ON: LFSRのシードを求める(LFSR+フェーズシフタのモデルを使う)。OFF: テストパターンだけを求める(ATPG) */
	bool  untest_mode;		  /**< -untest_mode  ON: 最初に、全ての故障を単独で調べて、検出できない故障を除く */

	/* 低消費電力 */
	bool  low_power;		  /**< -power        ON: キャプチャ時消費電力(WSA)を閾値以下にする */
	int   wsa_threshold;	  /**< -wsath        WSA閾値[%] */

	/* ソルバ(clasp) */
	int   solver_time;		  /**< -time         打ち切り時間[s] */
	int   parallel_threads;	  /**< -parallel     並列スレッド数(0: 並列なし) */
	bool  first_model_only;	  /**< -mode_1       最初の解で探索を打ち切る */

	/* 目標故障選択 */
	int   target_limit;		  /**< -target_limit 目標故障数の上限(0: 制限なし) */

	/* 再生成(打ち切り・解なしになった一次故障への対処) */
	bool  regen;			  /**< -regen        論文のフロー: 打ち切り時間を延ばし(60/180/300s)，WSA閾値を緩める(+5%/+10%) */
	bool  sttg;				  /**< -sttg         MTTGで求まらない一次故障を，単一目標(STTG)で再生成する */

	/* 実験的な機能 */
	bool  fault_propagation;  /**< -fp           一次故障に故障伝搬変数の制約を加える */
	bool  cps;				  /**< -cps          故障シミュレーションで1件も検出できなかったら，次は単一目標にする */

	int   lfsr_bits;		  /**< -b            LFSRのビット数(LFSR_BITS のみ対応) */
}
OPTION;

//------------------------------------------------------------------------
//  グローバル変数
//------------------------------------------------------------------------
extern OPTION opt;

//------------------------------------------------------------------------
//  関数
//------------------------------------------------------------------------
/** コマンドライン引数を解析する。失敗時はヘルプを表示して OPT_ERROR */
bool OPT(int argc, char** argv);

/** 引数を解析する(-set でその位置に設定ファイルを読み込む) */
bool OPTset(int argc, char** argv);

/** 設定ファイルを読む */
bool OPTread(char* filename);

/** 必須オプションの有無・値の範囲・入力ファイルを検査する */
bool OPTcheck(void);

/** ON/OFF を文字列にする(ログ出力用) */
const char* OPTonoff(bool value);

/** ヘルプを表示する */
void OPTdispHelp(void);

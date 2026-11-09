//------------------------------------------------------------------------
//  実行結果の出力(画面・log.txt・故障一覧ファイル)
//------------------------------------------------------------------------
#pragma once
#include <stdio.h>
#include <time.h>
#include "../../header/Fault.h"

//------------------------------------------------------------------------
//  出力ファイル名(出力フォルダー内)
//------------------------------------------------------------------------
#define FILE_LOG			"log.txt"					//実行情報
#define FILE_SEED			"seed.txt"					//生成したシード(1行1シード)
#define FILE_TEST_PATTERN	"tp.txt"					//直近のテストパターン
#define FILE_TARGET_LOG		"tfl_log.txt"				//目標故障集合の履歴
#define FILE_SEED_LOG		"mttg.txt"					//シードごとの検出故障数・WSA閾値
#define FILE_DETECTED		"det.txt"					//検出できた故障
#define FILE_UNDETECTED		"undet.txt"					//最後まで検出できなかった故障(正常終了ならば空)
#define FILE_UNSEED			"unseed_fault.txt"			//シード生成不能故障(ソルバが解を求められなかった一次故障)
#define FILE_ABORT			"abort_fault.txt"			//MTTGが求まらず，STTGで検出できた故障
#define FILE_HIGH_POWER		"high_power_fault.txt"		//緩和したWSA閾値でないと検出できなかった故障

//------------------------------------------------------------------------
//  終了状態
//------------------------------------------------------------------------
#define RUN_COMPLETED		"COMPLETED"					//全故障を処理して終了
#define RUN_ABORTED			"ABORTED"					//エラーで途中終了(途中までの結果は出力する)

//------------------------------------------------------------------------
//  関数
//------------------------------------------------------------------------
/** 出力フォルダー内のファイルのパスを作る。バッファに収まらなければ ERROR */
int output_path(char* buffer, size_t size, const char* file_name);

/** 故障タイプの名前("STR" / "STF") */
const char* fault_type_name(int type);

/** 実行経過の時間計測を始める(プログラム開始時に1回呼ぶ) */
void report_start_clock(void);

/** 実行情報(設定と，ここまでの集計)を out に書く */
void report_summary(FILE* out, const char* status, const char* reason);

/** 目標故障集合 tfl を tfl_log.txt に追記する */
int report_append_targets(void);

/** シードごとの記録(mttg.txt)に1行追記する */
int report_append_seed_log(int tfl_count, int detected_target_count, int detected_count, int wsa_threshold, const char* phase);

/** 終了時の出力: log.txt と故障一覧ファイル。エラー終了(status = RUN_ABORTED)でも出力する */
int report_write_final(const char* status, const char* reason);

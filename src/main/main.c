//------------------------------------------------------------------------
//  TDF_ASG: 遷移故障のシード生成(メイン)
//------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "../header/Define.h"
#include "../asg/init.h"
#include "../opt/opt.h"
#include "../else/OUTPUT/report.h"
#include "external_tools.h"

//===================================================
//関数名:main
//機能:TDF_ASGのmain
//戻り値：int 正常終了:SUCCESS(=0) , 異常終了:ERROR(=-1)
//引数：コマンドライン引数の数, コマンドライン引数
//===================================================
int main(int argc, char* argv[]) {
	int result;

	//オプション設定
	if (OPT(argc, argv) != true) return ERROR;
	if (check_external_tools() != SUCCESS) return ERROR;
	report_start_clock();

	//ネットリストの読み込みと初期化
	read_nl(opt.circuit);
	if (validate_circuit_gates() != SUCCESS) return ERROR;
	make_ppo_ppi();
	if (init_exturn() != SUCCESS) {
		printf("[ERROR] Initialization failed (init_exturn).\n");
		return ERROR;
	}
	if (InitGlobalVars() != true) return ERROR;

	//消費電力を考慮する場合は、オープンパス(WSAの対象外)を調べる
	if (opt.low_power && check_open_pass() != SUCCESS) {
		printf("[ERROR] Open-path check failed.\n");
		return ERROR;
	}

	//遷移故障のシード生成。エラーで終了しても、途中までの結果を出力する
	result = mttg_main();
	if (report_write_final(result == SUCCESS ? RUN_COMPLETED : RUN_ABORTED,
			result == SUCCESS ? NULL : "see the messages above") != SUCCESS) {
		printf("[ERROR] Writing the result files failed.\n");
		result = ERROR;
	}
	if (result != SUCCESS) return ERROR;

	printf("\n----------[FINISH] TDF_ASG----------\n");
	return SUCCESS;
}

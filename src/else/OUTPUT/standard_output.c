//------------------------------------------------------------------------
//インクルード
//------------------------------------------------------------------------
#include <stdio.h>
#include <time.h>
#include<stdlib.h>
#include<string.h>

//------------------------------------------------------------------------
//自作インクルード
//------------------------------------------------------------------------
#include"../../header/Define.h"

//===================================================
//関数名:standard_output
//機能:標準出力
//戻り値：なし
//引数：なし
//===================================================
void standard_output(void) {

	printf("//---------------------------------------------------\n");
	printf("//	    Transition Fault ASG   Information\n");
	printf("//---------------------------------------------------\n");
	printf("// Target Circuit              :%s\n", fi[CIRCUIT].name);
	printf("// Fault Type                  :TDF\n");
	if (fi[TARGET_LIMIT].value == 1) {
		printf("// Seed Generation             :STTG\n");
	}
	else {
		printf("// Seed Generation             :MTTG\n");
	}
	printf("// Number of Total Faults      :%d\n", (int)all_num);
	printf("// Number of Unseed Faults     :%d\n", (int)unseed_num);
	printf("// Number of High Power Faults :%d\n", (int)hpfl_num);
	printf("// Number of Detected Faults   :%d\n", (int)dfl_num);
	printf("// Number of Undetected Faults :%d\n", (int)ufl_num);
	printf("// Number of Seeds             :%d\n", seed_num);
	printf("// Rate of Detected Faults     :%.2lf%%\n", (((double)dfl_num) / ((double)all_num)) * 100.0);
	printf("// Elapsed time                :%.2lfsec\n", (double)clock() / CLOCKS_PER_SEC);
	printf("// Time to abort solver        :%dsec\n", fi[TIME].value);
	printf("// Time to graph               :%.2lfsec\n", (double)time_graph / CLOCKS_PER_SEC);
	printf("// Target faults limit         :%d\n", fi[TARGET_LIMIT].value);
	printf("// Rand Option                 :%s\n", fi[RAND].name);
	if (fi[PARALLEL].flag == 0) {
	printf("// Parallel Mode Option        :OFF\n");
	}
	if (fi[PARALLEL].flag == 1) {
	printf("// Parallel Mode Option        :ON\n");
	printf("// CPU Thread                  :%d\n", fi[PARALLEL].value);
	}
	if (fi[MODE_1].flag == 1) {
	printf("// Mode 1 Option               :%s\n", fi[MODE_1].name);
	}
	printf("// Low Power Mode              :%s\n", fi[LOW_POWER].name);
	printf("// WSA                         :%d[％]\n", fi[WSATH].value);
	printf("//---------------------------------------------------\n");
}
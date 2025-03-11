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
#include"../header/Define.h"


//===================================================
//関数名:unsat_dicision
//機能:SATかUNSATか判定
//戻り値：int　SAT(=1),UNSAT(=0)
//引数：mode(0:テスト不能故障削除時,1:目標故障選択時,2:高消費電力故障反転時)
//===================================================
int unsat_dicision(UNDET_FAULT_LIST* node, UNDET_FAULT_LIST* pre_node,int mode) {
    int i;
	char txt_cmd[300];
	char str[50000];
	char *d_str;

	//領域確保
	if ((d_str = (char*)calloc(n_pi * 2, sizeof(char))) == NULL) {
		printf("【ERROR】:No calloc d_str(unsat_dicision.c)\n");
		return ERROR;
	}

	//clasp結果ファイルオープン
	if (mode == 0||mode==2) {
		sprintf(txt_cmd, "clasp_log_untest.txt");
		if ((fin_untest_pbo_result = fopen(txt_cmd, "r")) == NULL) {
			printf("【ERROR】:File open clasp_log_untest.txt(unsat_dicision.c)\n");
			exit(1);
		}
		for (i = 0; i < 4; i++) {
			fgets(str, 50000, fin_untest_pbo_result);
		}
		fclose(fin_untest_pbo_result);
	}
	else if (mode == 1) {
		if ((fin_mini_pbo_result = fopen("clasp_log_mini.txt", "r")) == NULL) {
			printf("【ERROR】:File open clasp_log_mini.txt(unsat_dicision.c)\n");
			exit(1);
		}
		for (i = 0; i < 4; i++) {
			fgets(str, 50000, fin_mini_pbo_result);
		}
		fclose(fin_mini_pbo_result);
	}

	d_str = strtok(str, "\n");
	if (strcmp(d_str, "s UNSATISFIABLE") != 0) {
		return SAT;
	}

	//テスト不能故障削除時のUNSAT
	if (mode == 0 || mode==2) {
		//ゲートグループから削除
		//undet_fault_delete(node);

		if (mode == 0) {//テスト不能故障削除
			//未検出故障リストから削除
			ufl_free(node, pre_node, 0);
		}
		else if (mode == 2) {//高消費電力
			//未検出故障リストから削除
			ufl_free(node, pre_node, 1);
		}

		fclose(fin_untest_pbo_result);
	}

	if (mode == 1) {
		fclose(fin_mini_pbo_result);
	}

	return UNSAT;
}
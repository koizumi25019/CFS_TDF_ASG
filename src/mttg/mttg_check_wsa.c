//------------------------------------------------------------------------
//インクルード
//------------------------------------------------------------------------
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

//------------------------------------------------------------------------
//自作インクルード
//------------------------------------------------------------------------
#include"../header/Define.h"

//===================================================
//関数名:mttg_check_wsa
//機能:WSA計算(高消費電力故障か否か)
//     念のためテスト生成されたものが低消費か再確認
//戻り値：int 正常終了:SUCCESS(=0) , 異常終了:ERROR(=-1)
//引数：なし
//===================================================
int mttg_check_wsa(void) {
	char cmd[500];
	char str[20000];
	char txt_cmd[100];
	char txt_cmd2[100];
	FILE* fout_high;
	FILE* fout_low;
	int flag = ERROR;

	//WSA解析
	sprintf(cmd, "WSA.exe -sc %s -tptx %stp.txt -mode 1 -rate %d -high %shigh_mttg.txt -low %slow_mttg.txt -rep %s%s_wsa_rep.txt", fi[CIRCUIT].name,folder_out, fi[WSATH].value,folder_out,folder_out,folder_out, module_name);
	system(cmd);

	//結果ファイルオープン(高消費電力テストパターン,低消費電力テストパターン)
	sprintf(txt_cmd, "%shigh_mttg.txt", folder_out);
	if ((fout_high = (fopen(txt_cmd, "r"))) == NULL) {
		printf("【ERROR】:File open high_mttg.txt(mttg_check_wsa.c)\n");
		return ERROR;
	}

	sprintf(txt_cmd, "%slow_mttg.txt", folder_out);
	if ((fout_low = (fopen(txt_cmd, "r"))) == NULL) {
		printf("【ERROR】:File open low_mttg.txt(mttg_check_wsa.c)\n");
		return ERROR;
	}

	//高消費電力故障の場合→テスト生成おかしい
	if ((fgets(str, n_net + 1, fout_low)) != NULL) {
		flag = SAT;
	}
	else if ((fgets(str, n_net + 1, fout_high)) != NULL) {
		flag = UNSAT;
	}

	fclose(fout_high);
	fclose(fout_low);

	return flag;

}
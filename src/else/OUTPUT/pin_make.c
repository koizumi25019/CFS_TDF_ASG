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
//関数名:pin_make
//機能:PINファイル作成
//戻り値：int 正常終了:SUCCESS(=0) , 異常終了:ERROR(=-1)
//引数：なし
//===================================================
int pin_make(void) {
	int i;
	char txt_cmd[300];

	//入力ピンファイルオープン
	sprintf(txt_cmd, "%spin_input.txt", folder_out);
	if ((fout_pin_input = (fopen(txt_cmd, "w"))) == NULL) {
		printf("【ERROR】:File open pin_input.txt(pin_make.c)\n");
		return ERROR;
	}
	//入力ピンファイルオープン(故障シミュレーション)
	sprintf(txt_cmd, "%spin.txt", folder_out);
	if ((fout_pin = (fopen(txt_cmd, "w"))) == NULL) {
		printf("【ERROR】:File open pin.txt(pin_make.c)\n");
		return ERROR;
	}

	//外部入力
	for (i = 0; i < n_pi; i++) {
		fprintf(fout_pin_input, "%s\n", pi[i]->name);
		fprintf(fout_pin, "%s\n", pi[i]->name);
	}
	//疑似外部入力
	for (i = 0; i < n_dff; i++) {
		fprintf(fout_pin_input, "%s\n", dff[i]->name); 
		fprintf(fout_pin, "%s\n", dff[i]->name);
	}

	fclose(fout_pin_input);
	fclose(fout_pin);

	return SUCCESS;
}
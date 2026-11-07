//------------------------------------------------------------------------
//インクルード
//------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//------------------------------------------------------------------------
//自作インクルード
//------------------------------------------------------------------------
#include "../../header/Define.h"
#include "report.h"

//===================================================
//関数名:write_pin_file
//機能:外部入力と疑似外部入力(FF)の名前を1行ずつ書いたファイルを作る
//戻り値：int 正常終了:SUCCESS , 異常終了:ERROR
//引数：ファイル名
//===================================================
static int write_pin_file(const char* file_name) {
	char path[1024];
	FILE* file = NULL;

	if (output_path(path, sizeof(path), file_name) != SUCCESS) return ERROR;
	if (fopen_s(&file, path, "w") != 0 || file == NULL) {
		printf("[ERROR] Cannot create %s\n", path);
		return ERROR;
	}
	for (int i = 0; i < n_pi; i++) {
		fprintf(file, "%s\n", pi[i]->name);
	}
	for (int i = 0; i < n_dff; i++) {
		fprintf(file, "%s\n", dff[i]->name);
	}
	if (fclose(file) != 0) {
		printf("[ERROR] Cannot write %s\n", path);
		return ERROR;
	}
	return SUCCESS;
}

//===================================================
//関数名:pin_make
//機能:PINファイル作成(pin_input.txt: 入力ピン, pin.txt: 故障シミュレーション用)
//戻り値：int 正常終了:SUCCESS(=0) , 異常終了:ERROR(=-1)
//引数：なし
//===================================================
int pin_make(void) {
	if (write_pin_file("pin_input.txt") != SUCCESS) return ERROR;
	if (write_pin_file("pin.txt") != SUCCESS) return ERROR;
	return SUCCESS;
}

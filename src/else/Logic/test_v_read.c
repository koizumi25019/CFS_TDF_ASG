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

//=========================================================================
//  関数名 : test_v_read
//  機  能 : テストベクトル挿入
//　戻り値：int 正常終了:SUCCESS 異常終了:ERROR
//　引数：void
//=========================================================================
int test_v_read(void) {
	int i,j;
	FILE *fp;
	char *str;
	char *p_str;
	char txt_cmd[200];

	//fopen
	if (sprintf_s(txt_cmd, sizeof(txt_cmd), "%stp.txt", folder_out) < 0) {
		printf("[ERROR] Test-pattern path is too long.\n");
		return ERROR;
	}
	fp = fopen(txt_cmd, "r");
	if (fp == NULL) {
		printf("[ERROR] Cannot open test-pattern file: %s\n", txt_cmd);
		return ERROR;
	}

	//テストパターン領域確保
	str = (char*)malloc(sizeof(char)*((n_pi + n_ppi) + 1));
	if (str == NULL) {
		fclose(fp);
		printf("[ERROR] Cannot allocate test-pattern buffer.\n");
		return ERROR;
	}

	//テストパターン読込
	if (fgets(str, (n_pi + n_ppi + 1), fp) == NULL) {
		fclose(fp);
		free(str);
		printf("[ERROR] Cannot read test-pattern file: %s\n", txt_cmd);
		return ERROR;
	}

	//改行コード切り出し
	p_str = strtok(str, "\n");
	if (p_str == NULL || strlen(p_str) < (size_t)(n_pi + n_ppi)) {
		fclose(fp);
		free(str);
		printf("[ERROR] Test pattern is shorter than expected.\n");
		return ERROR;
	}

	//格納
	for (i = 0; i < n_pi ; i++) {
		pi[i]->val_1t = p_str[i] - 48;
	}
	for (j = 0; j < n_ppi; j++) {
		ppi[j]->val_1t = p_str[i] - 48;
		i++;
	}

	//fclose
	fclose(fp);

	//free
	free(str);

	return SUCCESS;
}
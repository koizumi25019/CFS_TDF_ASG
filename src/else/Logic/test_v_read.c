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
	char txt_cmd[50];

	//fopen
	sprintf(txt_cmd, "% stp.txt", folder_out);
	fp = fopen(txt_cmd, "r");

	//テストパターン領域確保
	str = (char*)malloc(sizeof(char)*((n_pi + n_ppi) + 1));

	//テストパターン読込
	fgets(str, (n_pi + n_ppi + 1), fp);

	//改行コード切り出し
	p_str = strtok(str, "\n");

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
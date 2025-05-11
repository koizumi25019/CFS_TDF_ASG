//------------------------------------------------------------------------
//インクルード
//------------------------------------------------------------------------
#include <stdio.h>
#include <time.h>
#include<stdlib.h>
#include<string.h>
#include<direct.h>
#include<sys/stat.h>

//------------------------------------------------------------------------
//自作インクルード
//------------------------------------------------------------------------
#include"../../header/Define.h"

//int folder_exists(const char* name) {
//	struct stat st;
//	return (stat(name, &st) == 0 && (st.st_mode & S_IFDIR));
//}

//===================================================
//関数名:init_exturn
//機能:初期化関連
//戻り値：int 正常終了:SUCCESS(=0) , 異常終了:ERROR(=-1)
//引数：なし
//===================================================
int init_exturn(void) {
	char txt_cmd[300];
	//char* file_n;
	int str_file;

	//遷移故障関連
	hpfl = NULL;
	ufl_num = 0;		            //未検出故障数
	dfl_num;				        //検出故障数
	unseed_num = 0;		            //シード生成不能故障数
	hpfl_num = 0;			        //高消費電力故障数
	seed_num = 0;					    //テストベクトル数

	//キュー
	Que.max = n_net * 1000;	        //最大値
	Que.front = 0;				    //先頭インデックス
	Que.rear = 0;				    //末尾インデックス
	Que.num = 0;				    //要素数
	if ((Que.que = (NLIST**)calloc(n_net * 1000, sizeof(NLIST*))) == NULL) {
		printf("【ERROR】:No calloc Que(init_exturn.c)");
		exit(EXIT_FAILURE);
	}

	//レベライズ
	Level.max_num = n_net * 100;	//最大値
	Level.ptr = 0;					//インデックス
	if ((Level.stk = (NLIST**)calloc(n_net, sizeof(NLIST*))) == NULL) {
		printf("【ERROR】:No calloc Level stack(init_exturn.c)\n");
		return ERROR;
	}

	//オープンパス
	Open.max_num = n_net * 100;		//最大値
	Open.ptr = 0;					//インデックス
	if ((Open.stk = (NLIST**)calloc(n_net, sizeof(NLIST*))) == NULL) {
		printf("【ERROR】:No calloc Open stack(init_exturn.c)\n");
		return ERROR;
	}

	//フォルダーの作成
	if ((folder_out = (char*)calloc(100, sizeof(char))) == NULL) {
		printf("【ERROR】:No calloc Folder name output(init_exturn.c)\n");
		return ERROR;
	}
	strcpy(folder_out,fi[FOLDER].name);
	str_file = strlen(fi[FOLDER].name) - 1;
	fi[FOLDER].name[str_file] = '\0';

	//char new_name[100];
	//int cnt = 0;
	//strcpy(new_name, folder_out);
	//while (folder_exists(new_name)) {
	//	cnt++;
	//	snprintf(new_name, sizeof(new_name), "%s_%d", fi[FOLDER].name, cnt);
	//}

	//// folder_outをnew_nameに更新
	//strcpy(folder_out, new_name);

	if (_mkdir(folder_out) != 0) {
		printf("【ERROR】:Please Delete Same name Folder(init_exturn.c)\n");
		return ERROR;
	}

	//ファイルを初期化
	sprintf(txt_cmd, "%smttg.txt", folder_out);;
	if ((fout_mttg = fopen(txt_cmd, "w")) == NULL) {
		printf("【ERROR】:File open mttg.txt(init_exturn.c)\n");
		return ERROR;
	}
	fclose(fout_mttg);

	sprintf(txt_cmd, "%stfl_log.txt", folder_out);
	if ((fout_tfl_log = fopen(txt_cmd, "w")) == NULL) {
		printf("【ERROR】:File open tfl_log.txt(init_exturn.c)\n");
		return ERROR;
	}
	fclose(fout_tfl_log);

	return SUCCESS;
}
//------------------------------------------------------------------------
//インクルード
//------------------------------------------------------------------------
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <sys/stat.h>

//------------------------------------------------------------------------
//自作インクルード
//------------------------------------------------------------------------
#include "../../header/Define.h"
#include "../OUTPUT/report.h"

//===================================================
//関数名:create_empty_file
//機能:出力フォルダー内に空のファイルを作る(追記用ファイルの初期化)
//戻り値：int 正常終了:SUCCESS , 異常終了:ERROR
//引数：ファイル名
//===================================================
static int create_empty_file(const char* file_name) {
	char path[1024];
	FILE* file = NULL;

	if (output_path(path, sizeof(path), file_name) != SUCCESS) return ERROR;
	if (fopen_s(&file, path, "w") != 0 || file == NULL) {
		printf("[ERROR] Cannot create %s\n", path);
		return ERROR;
	}
	fclose(file);
	return SUCCESS;
}

//===================================================
//関数名:init_output_folder
//機能:出力フォルダー名(末尾に区切りを補う)を決め，フォルダーを作る
//戻り値：int 正常終了:SUCCESS , 異常終了:ERROR
//引数：なし
//===================================================
static int init_output_folder(void) {
	size_t length = strlen(opt.folder);

	if (length == 0) {
		printf("[ERROR] The output folder name (-folder) is empty.\n");
		return ERROR;
	}
	if ((folder_out = (char*)calloc(length + 2, sizeof(char))) == NULL) {
		printf("[ERROR] Out of memory (output folder name).\n");
		return ERROR;
	}
	strcpy(folder_out, opt.folder);
	if (folder_out[length - 1] != '/' && folder_out[length - 1] != '\\') {
		strcat(folder_out, "/");
	}

	if (_mkdir(folder_out) != 0) {
		if (errno == EEXIST) {
			printf("[ERROR] The output folder already exists: %s\n        Delete it or give another name with -folder.\n", folder_out);
		}
		else {
			printf("[ERROR] Cannot create the output folder %s (%s)\n", folder_out, strerror(errno));
		}
		return ERROR;
	}
	return SUCCESS;
}

//===================================================
//関数名:init_exturn
//機能:初期化関連(集計の初期化，作業用スタック・キュー，出力フォルダー)
//戻り値：int 正常終了:SUCCESS(=0) , 異常終了:ERROR(=-1)
//引数：なし
//===================================================
int init_exturn(void) {

	//集計
	hpfl = NULL;
	abortfl = NULL;
	abortfl_num = 0;
	ufl_num = 0;
	dfl_num = 0;
	unseed_num = 0;
	hpfl_num = 0;
	seed_num = 0;
	seed_relaxed_num = 0;
	wsa_used_max = 0;

	//キュー(故障範囲の探索用。同じ信号線は2度入れないので，信号線数以下で足りる)
	Que.max = n_net + 1;
	Que.front = 0;
	Que.rear = 0;
	Que.num = 0;
	if ((Que.que = (NLIST**)calloc(Que.max, sizeof(NLIST*))) == NULL) {
		printf("[ERROR] Out of memory (queue).\n");
		return ERROR;
	}

	//レベライズ用スタック
	Level.max_num = n_net;
	Level.ptr = 0;
	if ((Level.stk = (NLIST**)calloc(n_net, sizeof(NLIST*))) == NULL) {
		printf("[ERROR] Out of memory (level stack).\n");
		return ERROR;
	}

	//オープンパス用スタック
	Open.max_num = n_net;
	Open.ptr = 0;
	if ((Open.stk = (NLIST**)calloc(n_net, sizeof(NLIST*))) == NULL) {
		printf("[ERROR] Out of memory (open-path stack).\n");
		return ERROR;
	}

	//出力フォルダーと，追記するファイルの初期化
	if (init_output_folder() != SUCCESS) return ERROR;
	if (create_empty_file(FILE_SEED_LOG) != SUCCESS) return ERROR;
	if (create_empty_file(FILE_TARGET_LOG) != SUCCESS) return ERROR;

	return SUCCESS;
}

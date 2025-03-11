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

// 関数プロトタイプ宣言
NLIST* search_netlist(char* name);

//===================================================
// 関数名:transition_fault_set
// 機能:遷移故障の未検出故障集合を構造体に格納
// 戻り値：int 正常終了:SUCCESS(=0) , 異常終了:ERROR(=-1)
// 引数：なし
//===================================================

int transition_delay_fault_set(void) {

	int id = 0;
	char line[100];//ファイルの1行分を取得
	char fault_type[10];
	char net_name[50];
	UNDET_FAULT_LIST* head = NULL;
	UNDET_FAULT_LIST* node;
	NLIST* net_info; 

	//未検出故障集合ファイルオープン
	FILE* fp = fopen(fi[FAULT].name, "r");
	if (fp == NULL) {
		printf("【ERROR】: File open failed (%s)\n", fi[FAULT].name);
		return ERROR;
	}

	// ファイルから各行を読み取って解析
	while (fgets(line, sizeof(line), fp) != NULL) {//ファイルの1行分を取得
		if (sscanf(line, "%s %s", net_name, fault_type) != 2) {
			printf("【WARNING】: Invalid format in line: %s", line);
			continue;//現在の反復(itertion)をスキップし、次の反復に移る
		}

		// ネット名を基にソート済みネットリストから該当する情報を探す
		net_info = search_netlist(net_name); 
		if (net_info == NULL) {
			printf("【ERROR】: Net not found (%s)\n", net_name);
			continue;
		}

		// 新しい未検出故障ノードを作成
		node = (UNDET_FAULT_LIST*)malloc(sizeof(UNDET_FAULT_LIST));
		if (node == NULL) {
			printf("【ERROR】: Memory allocation failed\n");
			fclose(fp);
			return ERROR;
		}

		// 故障情報の格納 (NLIST の情報を UNDET_FAULT_LIST に反映)
		node->netlist = net_info;  // NLIST のポインタをそのまま使用可能
		strcpy(node->name, net_name);

		// 故障タイプの設定
		if (_stricmp(fault_type, "STR") == 0) {
			node->type = STR;
		}
		else if (_stricmp(fault_type, "STF") == 0) {
			node->type = STF;
		}
		else {
			printf("【WARNING】: Unknown fault type (%s)\n", fault_type);
			free(node);
			continue;
		}

		//信号線故障ID入力
		node->node_id = id;
		id++;

		// リストの先頭に追加
		node->next = head;
		head = node;
		ufl_num++;
	}

	fclose(fp);

	// ufl にリストを格納
	ufl = head;
	afl = head; 
	node_num = id;

	printf("【INFO】: Transition fault set created successfully.\n");
	return SUCCESS;
}



//===================================================
// 関数名:search_netlist
// 機能:ネット名からソートネットリストを探索し、該当するノードの情報を返す
// 戻り値：UNDET_FAULT_LIST* 該当するノード, 見つからない場合はNULL
// 引数：char* name - ネット名
//===================================================
NLIST* search_netlist(char* name) {
	for (int i = 0; i < n_net; i++) {
		if (strcmp(sort_nl[i]->name, name) == 0) {
			return sort_nl[i];
		}
	}
	return NULL;
}

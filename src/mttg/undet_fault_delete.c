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
// 関数名: undet_fault_delete
// 機能: 未検出故障リストから指定された遷移故障を削除
// 戻り値：int 正常終了:SUCCESS(=0), 異常終了:ERROR(=-1)
// 引数：UNDET_FAULT_LIST* node - 削除したい故障のポインタ
//===================================================
int undet_fault_delete(UNDET_FAULT_LIST* node) {
	UNDET_FAULT_LIST* current, * prev;

	// 未検出故障リストが空、または削除対象のノードがNULLの場合
	if (ufl == NULL || node == NULL) {
		return ERROR;
	}

	current = ufl;
	prev = NULL;

	// 未検出故障リストを走査
	while (current != NULL) {
		// 故障タイプとネット名が一致するかを確認
		if (current->type == node->type && strcmp(current->name, node->name) == 0) {
			// ノードを削除
			if (prev == NULL) {
				// 削除するノードがヘッドノードの場合
				ufl = current->next;
			}
			else {
				prev->next = current->next;
			}
			// メモリを解放
			free(current);
			ufl_num--; // 未検出故障数をデクリメント
			return SUCCESS;
		}
		prev = current;
		current = current->next;
	}

	// ノードが見つからなかった場合
	return ERROR;
}



//========================================================
//関数名:ufl_free
//機能：テスト不能故障削除
//戻り値：void
//引数：削除したい故障，,テスト不能(mode=0)か高消費電力故障(mode=1)なのか
//========================================================
void ufl_free(UNDET_FAULT_LIST* node, UNDET_FAULT_LIST* pre_node,int mode) {
	UNDET_FAULT_LIST*p_node;

	p_node = (UNDET_FAULT_LIST*)malloc(sizeof(UNDET_FAULT_LIST));

	//情報格納
	strcpy(p_node->name, node->name);
	p_node->type=node->type;

	p_node->netlist = node->netlist;

	if (mode == 0) {//テスト不能
		//p_nodeをredまたはhpflの先頭に追加している
		p_node->next = unseed;
		unseed = p_node;
	}
	else if (mode == 1) {//高消費電力
		p_node->next = hpfl;
		hpfl = p_node;
	}

	//削除(free)
	if (pre_node == NULL) {

		ufl = node->next;

		ufl_num--;

		if (mode == 0) {
			unseed_num++;
		}
		else if (mode == 1) {
			hpfl_num++;
		}

		free(node);

	}
	else {
		pre_node->next = node->next;

		ufl_num--;

		if (mode == 0) {
			unseed_num++;
		}
		else if (mode == 1) {
			hpfl_num++;
		}

		free(node);

	}
	//if (mode == 0) {
	//	red = p_node;
	//}
	//else if (mode == 1) {
	//	hpfl = p_node;
	//}

	return;
}
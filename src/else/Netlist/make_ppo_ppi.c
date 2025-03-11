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
//関数名:make_ppo_ppi
//機能:疑似外部入出力配列の作成
//戻り値：なし
//引数：なし
//===================================================
void make_ppo_ppi(void) {

	int i;
	int count_ppo = 0;
	int count_ppi = 0;

	//領域確保
	ppo = (NLIST**)malloc(sizeof(NLIST*)*n_dff);
	ppi = (NLIST**)malloc(sizeof(NLIST*)*n_dff);


	//ppi生成
	for (i = 0; i < n_net; i++) {
		if (nl[i].type == DFF) {
			ppi[count_ppi++] = &nl[i];
		}
	}

	//ppo生成
	for (i = 0; i < count_ppi; i++) {
		ppo[count_ppo++] = ppi[i]->in[0];
		ppo[i]->ppo_flag = 1;
	}

	//総数格納
	n_ppi = count_ppi;
	n_ppo = count_ppo;

	//疑似外部出力数設定
	n_ppo = n_dff;

	return SUCCESS;

}
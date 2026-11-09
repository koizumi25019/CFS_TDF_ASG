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
//関数名:levelize
//機能:レベライズ
//戻り値：int 正常終了:SUCCESS(=0) , 異常終了:ERROR(=-1)
//引数：なし
//===================================================
int levelize(void) {

	int i;
	int max_in_lev = 0;	//最大入力ゲートレベル
	NLIST* nlist;		//NLIST


	//(疑似)外部入力をスタックに入れる
	//外部入力
	for (i = 0; i < n_pi; i++) { 
		level_push(pi[i]);
	}
	//外部入力
	for (i = 0; i < n_dff; i++) {
		level_push(dff[i]);
	}

	while (Level.ptr != 0) {

		nlist = level_pop();

		switch (nlist->type) {

		case IN:
			nlist->level = 1;		//レベル設定
			nlist->flag = 1;		//フラグ設定
			for (i = 0; i < nlist->n_out; i++) {
				level_push(nlist->out[i]);//出力プッシュ
			}
			break;

		case DFF:
			nlist->level = 1;		//レベル設定
			nlist->flag = 1;		//フラグ設定
			for (i = 0; i < nlist->n_out; i++) {
				level_push(nlist->out[i]);//出力プッシュ
			}
			break;

		default:

			//入力線のレベル判定
			if (judge_in_level(nlist->in, nlist->n_in) == SUCCESS) {

				//最大の入力レベル算出
				for (i = 0; i < nlist->n_in; i++) {
					if (max_in_lev < nlist->in[i]->level) {
						max_in_lev = nlist->in[i]->level;
					}
				}

				nlist->level = max_in_lev + 1;	//レベル設定
				nlist->flag = 1;				//フラグ設定
				max_in_lev = 0;					//変数初期化

				if (nlist->ppo_flag != 1) {
					for (i = 0; i < nlist->n_out; i++) {
						level_push(nlist->out[i]);	//出力プッシュ
					}
				}

			}
			break;

		}

	}


	//---------------------------------------------
	//Qソート
	//---------------------------------------------
	Qsort();

	//---------------------------------------------
	//添え字（ID）の設定
	//---------------------------------------------
	set_literal_id();

	//---------------------------------------------
	//フラグリセット
	//---------------------------------------------
	reset_flag();

	return SUCCESS;

}

//=========================================================================
//  関数名 : judge_in_level
//  機  能 : 入力線のレベライズ完了の判定
//  戻り値 : 1(完) / 0(未)
//  引  数 : 入力ポインタ配列，入力数
//=========================================================================
int judge_in_level(NLIST **in_pin, int n) {

	int i = 0;

	//全入力線のレベル付けは完了しているか判定
	while (i < n) {
		if (in_pin[i]->flag != 1) {
			return ERROR;	//失敗
		}
		i++;
	}

	return SUCCESS;	//成功

}
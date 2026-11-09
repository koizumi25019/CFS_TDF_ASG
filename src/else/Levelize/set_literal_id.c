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
//  関数名 : set_literaid
//  機  能 : リテラルID設定
//  戻り値 : なし
//  引  数 : なし
//=========================================================================
void set_literal_id(void) {

	int i;
	int first = VAR_FIRST_CIRCUIT;

	//添え字設定
	for (i = 0; i < n_pi; i++) {
		pi[i]->suf_1t = i + first;
		pi[i]->suf_2t = i + first + n_net;
		//printf("1時刻目：%d  ", pi[i]->suf_1t);
		//printf("2時刻目：%d  ", pi[i]->suf_2t);
		//printf("名前：%s\n", pi[i]->name);
	}
	for (i = 0; i < n_dff; i++) {
		dff[i]->suf_1t = i + n_pi + first;
		dff[i]->suf_2t = i + n_pi + first + n_net;
		//printf("1時刻目：%d  ", dff[i]->suf_1t);
		//printf("2時刻目：%d  ", dff[i]->suf_2t);
		//printf("名前：%s\n", dff[i]->name);
	}
	for (i = n_pi + n_dff; i < n_net; i++) {
		sort_nl[i]->suf_1t = i + first;
		sort_nl[i]->suf_2t = i + first + n_net;
		//printf("1時刻目：%d  ", sort_nl[i]->suf_1t);
		//printf("2時刻目：%d  ", sort_nl[i]->suf_2t);
		//printf("名前：%s\n", sort_nl[i]->name);
	}

}
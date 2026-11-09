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
//  関数名 : Qsort
//  機  能 : クイックソート
//  戻り値 : なし
//  引  数 : なし
//=========================================================================
static int compare_level(const void* a, const void* b)
{
	return (*(NLIST* const*)a)->level - (*(NLIST* const*)b)->level;
}

void Qsort(void) {

	int i;

	if ((sort_nl = (NLIST **)calloc(n_net, sizeof(NLIST*))) == NULL) {
		printf("【ERROR】:No calloc sort_nl(Qsort.c)\n");
		exit(ERROR);
	}

	//ソート用ポインタ配列にNLISTコピー
	for (i = 0; i < n_net; i++) { 
		sort_nl[i] = &nl[i];
	}

	//クイックソート
	qsort(sort_nl, n_net, sizeof(NLIST*), compare_level);

}

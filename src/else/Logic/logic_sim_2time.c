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
//  関数名 : logic_sim_2time
//  機  能 : 2時刻目論理シミュレーション
//　戻り値：int 正常終了:SUCCESS 異常終了:ERROR
//　引数：void
//=========================================================================
int logic_sim_2time(void) {
	int i;

	for (i = n_pi + n_ppi; i < n_net; i++) {
		/*if (strcmp("G115", sort_nl[i]->name) == 0) {
			printf("G115\n");
		}
		if (strcmp(nl[63].out[0]->name, sort_nl[i]->name) == 0) {
			printf("G115\n");
		}*/
		switch (sort_nl[i]->type) {
		case AND:
			sort_nl[i]->val_2t = and (sort_nl[i], 2);
			break;

		case NAND:
			sort_nl[i]->val_2t = nand(sort_nl[i], 2);
			break;

		case OR:
			sort_nl[i]->val_2t = or(sort_nl[i], 2);
			break;

		case NOR:
			sort_nl[i]->val_2t = nor(sort_nl[i], 2);
			break;

		case INV:
			sort_nl[i]->val_2t = inv(sort_nl[i], 2);
			break;

		case FOUT:
			sort_nl[i]->val_2t = fout(sort_nl[i], 2);
			break;

		case DFF:
		case DFFS:
			break;


		default:
			puts("未対応");
			return ERROR;
		}
	}

	return SUCCESS;
}
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
//  関数名 : pbo_generate_normal_c
//  機  能 : 正常回路記述
//  戻り値 : なし
//  引  数 : ファイルポインタ
//=========================================================================
void pbo_generate_normal_c(FILE *fp) {

	int i, j;

	//-----------------------------------------------------------------------------
	//■PBOファイル記述
	//-----------------------------------------------------------------------------
	for (i = 0; i < n_net; i++) {
		switch (sort_nl[i]->type) {
			//●AND-------------------------------------------------------------------------------------------------------------------------------
		case AND:
			//1時刻目
			for (j = 0; j < sort_nl[i]->n_in; j++) { fprintf(fp, "1 x%d +1 ~x%d >=1;\n", sort_nl[i]->in[j]->suf_1t, sort_nl[i]->suf_1t); }
			for (j = 0; j < sort_nl[i]->n_in; j++) { fprintf(fp, "1 ~x%d +", sort_nl[i]->in[j]->suf_1t); }
			fprintf(fp, "1 x%d >=1;\n", sort_nl[i]->suf_1t);
			//2時刻目
			for (j = 0; j < sort_nl[i]->n_in; j++) { fprintf(fp, "1 x%d +1 ~x%d >=1;\n", sort_nl[i]->in[j]->suf_2t, sort_nl[i]->suf_2t); }
			for (j = 0; j < sort_nl[i]->n_in; j++) { fprintf(fp, "1 ~x%d +", sort_nl[i]->in[j]->suf_2t); }
			fprintf(fp, "1 x%d >=1;\n", sort_nl[i]->suf_2t);
			break;
			//●NAND-------------------------------------------------------------------------------------------------------------------------------
		case NAND:
			//1時刻目
			for (j = 0; j < sort_nl[i]->n_in; j++) { fprintf(fp, "1 x%d +1 x%d >=1;\n", sort_nl[i]->in[j]->suf_1t, sort_nl[i]->suf_1t); }
			for (j = 0; j < sort_nl[i]->n_in; j++) { fprintf(fp, "1 ~x%d +", sort_nl[i]->in[j]->suf_1t); }
			fprintf(fp, "1 ~x%d >=1;\n", sort_nl[i]->suf_1t);
			//2時刻目
			for (j = 0; j < sort_nl[i]->n_in; j++) { fprintf(fp, "1 x%d +1 x%d >=1;\n", sort_nl[i]->in[j]->suf_2t, sort_nl[i]->suf_2t); }
			for (j = 0; j < sort_nl[i]->n_in; j++) { fprintf(fp, "1 ~x%d +", sort_nl[i]->in[j]->suf_2t); }
			fprintf(fp, "1 ~x%d >=1;\n", sort_nl[i]->suf_2t);
			break;
			//●OR-------------------------------------------------------------------------------------------------------------------------------
		case OR:
			//1時刻目
			for (j = 0; j < sort_nl[i]->n_in; j++) { fprintf(fp, "1 ~x%d +1 x%d >=1;\n", sort_nl[i]->in[j]->suf_1t, sort_nl[i]->suf_1t); }
			for (j = 0; j < sort_nl[i]->n_in; j++) { fprintf(fp, "1 x%d +", sort_nl[i]->in[j]->suf_1t); }
			fprintf(fp, "1 ~x%d >=1;\n", sort_nl[i]->suf_1t);
			//2時刻目
			for (j = 0; j < sort_nl[i]->n_in; j++) { fprintf(fp, "1 ~x%d +1 x%d >=1;\n", sort_nl[i]->in[j]->suf_2t, sort_nl[i]->suf_2t); }
			for (j = 0; j < sort_nl[i]->n_in; j++) { fprintf(fp, "1 x%d +", sort_nl[i]->in[j]->suf_2t); }
			fprintf(fp, "1 ~x%d >=1;\n", sort_nl[i]->suf_2t);
			break;
			//●NOR-------------------------------------------------------------------------------------------------------------------------------
		case NOR:
			//1時刻目
			for (j = 0; j < sort_nl[i]->n_in; j++) { fprintf(fp, "1 ~x%d +1 ~x%d >=1;\n", sort_nl[i]->in[j]->suf_1t, sort_nl[i]->suf_1t); }
			for (j = 0; j < sort_nl[i]->n_in; j++) { fprintf(fp, "1 x%d +", sort_nl[i]->in[j]->suf_1t); }
			fprintf(fp, "1 x%d >=1;\n", sort_nl[i]->suf_1t);
			//2時刻目
			for (j = 0; j < sort_nl[i]->n_in; j++) { fprintf(fp, "1 ~x%d +1 ~x%d >=1;\n", sort_nl[i]->in[j]->suf_2t, sort_nl[i]->suf_2t); }
			for (j = 0; j < sort_nl[i]->n_in; j++) { fprintf(fp, "1 x%d +", sort_nl[i]->in[j]->suf_2t); }
			fprintf(fp, "1 x%d >=1;\n", sort_nl[i]->suf_2t);
			break;
			//●INV-------------------------------------------------------------------------------------------------------------------------------
		case INV:
			//1時刻目
			fprintf(fp, "1 ~x%d +1 ~x%d >=1;\n1 x%d +1 x%d >=1;\n", sort_nl[i]->in[0]->suf_1t, sort_nl[i]->suf_1t, sort_nl[i]->in[0]->suf_1t, sort_nl[i]->suf_1t);
			//2時刻目
			fprintf(fp, "1 ~x%d +1 ~x%d >=1;\n1 x%d +1 x%d >=1;\n", sort_nl[i]->in[0]->suf_2t, sort_nl[i]->suf_2t, sort_nl[i]->in[0]->suf_2t, sort_nl[i]->suf_2t);
			break;
			//●FOUT-------------------------------------------------------------------------------------------------------------------------------
		case FOUT:
			//1時刻目
			fprintf(fp, "1 ~x%d +1 x%d >=1;\n1 x%d +1 ~x%d >=1;\n", sort_nl[i]->in[0]->suf_1t, sort_nl[i]->suf_1t, sort_nl[i]->in[0]->suf_1t, sort_nl[i]->suf_1t);
			//2時刻目
			fprintf(fp, "1 ~x%d +1 x%d >=1;\n1 x%d +1 ~x%d >=1;\n", sort_nl[i]->in[0]->suf_2t, sort_nl[i]->suf_2t, sort_nl[i]->in[0]->suf_2t, sort_nl[i]->suf_2t);
			break;
			//●BUF-------------------------------------------------------------------------------------------------------------------------------
		case BUF:
			//1時刻目
			fprintf(fp, "1 ~x%d +1 x%d >=1;\n1 x%d +1 ~x%d >=1;\n", sort_nl[i]->in[0]->suf_1t, sort_nl[i]->suf_1t, sort_nl[i]->in[0]->suf_1t, sort_nl[i]->suf_1t);
			//2時刻目
			fprintf(fp, "1 ~x%d +1 x%d >=1;\n1 x%d +1 ~x%d >=1;\n", sort_nl[i]->in[0]->suf_2t, sort_nl[i]->suf_2t, sort_nl[i]->in[0]->suf_2t, sort_nl[i]->suf_2t);
			break;
			//●EXOR-------------------------------------------------------------------------------------------------------------------------------
		case EXOR:
			//1時刻目
			fprintf(fp, "1 ~x%d +1 ~x%d +1 ~x%d >=1;\n", sort_nl[i]->in[0]->suf_1t, sort_nl[i]->in[1]->suf_1t, sort_nl[i]->suf_1t);
			fprintf(fp, "1 x%d +1 x%d +1 ~x%d >=1;\n", sort_nl[i]->in[0]->suf_1t, sort_nl[i]->in[1]->suf_1t, sort_nl[i]->suf_1t);
			fprintf(fp, "1 x%d +1 ~x%d +1 x%d >=1;\n", sort_nl[i]->in[0]->suf_1t, sort_nl[i]->in[1]->suf_1t, sort_nl[i]->suf_1t);
			fprintf(fp, "1 ~x%d +1 x%d +1 x%d >=1;\n", sort_nl[i]->in[0]->suf_1t, sort_nl[i]->in[1]->suf_1t, sort_nl[i]->suf_1t);
			//2時刻目
			fprintf(fp, "1 ~x%d +1 ~x%d +1 ~x%d >=1;\n", sort_nl[i]->in[0]->suf_2t, sort_nl[i]->in[1]->suf_2t, sort_nl[i]->suf_2t);
			fprintf(fp, "1 x%d +1 x%d +1 ~x%d >=1;\n", sort_nl[i]->in[0]->suf_2t, sort_nl[i]->in[1]->suf_2t, sort_nl[i]->suf_2t);
			fprintf(fp, "1 x%d +1 ~x%d +1 x%d >=1;\n", sort_nl[i]->in[0]->suf_2t, sort_nl[i]->in[1]->suf_2t, sort_nl[i]->suf_2t);
			fprintf(fp, "1 ~x%d +1 x%d +1 x%d >=1;\n", sort_nl[i]->in[0]->suf_2t, sort_nl[i]->in[1]->suf_2t, sort_nl[i]->suf_2t);
			break;
			//●DFF-------------------------------------------------------------------------------------------------------------------------------
		case DFF:
			fprintf(fp, "1 ~x%d +1 x%d >=1;\n1 x%d +1 ~x%d >=1;\n", sort_nl[i]->in[0]->suf_1t, sort_nl[i]->suf_2t, sort_nl[i]->in[0]->suf_1t, sort_nl[i]->suf_2t);
			break;
			//●IN-------------------------------------------------------------------------------------------------------------------------------
		case IN:
			fprintf(fp, "1 ~x%d +1 x%d >=1;\n1 x%d +1 ~x%d >=1;\n", sort_nl[i]->suf_1t, sort_nl[i]->suf_2t, sort_nl[i]->suf_1t, sort_nl[i]->suf_2t);
			break;
		}
	}

}

//=========================================================================
//  関数名 : pbo_generate_fault_c
//  機  能 : 故障回路記述
//  戻り値 : なし
//  引  数 : ファイルポインタ,対象故障
//=========================================================================
void pbo_generate_fault_c(FILE* fp, UNDET_FAULT_LIST* node, int f) {

	int i, j;

	char* cons_prop = NULL;	//故障伝搬変数のブール制約用
	cons_prop = (char*)allocMemory(50000, sizeof(char));
	cons_prop = allocMemory(50000, sizeof(char));


	//fprintf(fp, "\n");
	//-----------------------------------------------------------------------------
	//■PBOファイル記述
	//-----------------------------------------------------------------------------
	for (i = 0; i < n_net; i++) {
		if (sort_nl[i]->fault_pass == YES && strcmp(sort_nl[i]->name, node->name) != 0) {
			switch (sort_nl[i]->type) {
				//●AND-------------------------------------------------------------------------------------------------------------------------------
			case AND:
				for (j = 0; j < sort_nl[i]->n_in; j++) { fprintf(fp, "1 x%d +1 ~x%d >=1;\n", sort_nl[i]->in[j]->suf_fc, sort_nl[i]->suf_fc); }
				for (j = 0; j < sort_nl[i]->n_in; j++) { fprintf(fp, "1 ~x%d +", sort_nl[i]->in[j]->suf_fc); }
				fprintf(fp, "1 x%d >=1;\n", sort_nl[i]->suf_fc);
				break;
				//●NAND-------------------------------------------------------------------------------------------------------------------------------
			case NAND:
				for (j = 0; j < sort_nl[i]->n_in; j++) { fprintf(fp, "1 x%d +1 x%d >=1;\n", sort_nl[i]->in[j]->suf_fc, sort_nl[i]->suf_fc); }
				for (j = 0; j < sort_nl[i]->n_in; j++) { fprintf(fp, "1 ~x%d +", sort_nl[i]->in[j]->suf_fc); }
				fprintf(fp, "1 ~x%d >=1;\n", sort_nl[i]->suf_fc);
				break;
				//●OR-------------------------------------------------------------------------------------------------------------------------------
			case OR:
				for (j = 0; j < sort_nl[i]->n_in; j++) { fprintf(fp, "1 ~x%d +1 x%d >=1;\n", sort_nl[i]->in[j]->suf_fc, sort_nl[i]->suf_fc); }
				for (j = 0; j < sort_nl[i]->n_in; j++) { fprintf(fp, "1 x%d +", sort_nl[i]->in[j]->suf_fc); }
				fprintf(fp, "1 ~x%d >=1;\n", sort_nl[i]->suf_fc);
				break;
				//●NOR-------------------------------------------------------------------------------------------------------------------------------
			case NOR:
				for (j = 0; j < sort_nl[i]->n_in; j++) { fprintf(fp, "1 ~x%d +1 ~x%d >=1;\n", sort_nl[i]->in[j]->suf_fc, sort_nl[i]->suf_fc); }
				for (j = 0; j < sort_nl[i]->n_in; j++) { fprintf(fp, "1 x%d +", sort_nl[i]->in[j]->suf_fc); }
				fprintf(fp, "1 x%d >=1;\n", sort_nl[i]->suf_fc);
				break;
				//●INV-------------------------------------------------------------------------------------------------------------------------------
			case INV:
				fprintf(fp, "1 ~x%d +1 ~x%d >=1;\n1 x%d +1 x%d >=1;\n", sort_nl[i]->in[0]->suf_fc, sort_nl[i]->suf_fc, sort_nl[i]->in[0]->suf_fc, sort_nl[i]->suf_fc);
				break;
				//●FOUT-------------------------------------------------------------------------------------------------------------------------------
			case FOUT:
				fprintf(fp, "1 ~x%d +1 x%d >=1;\n1 x%d +1 ~x%d >=1;\n", sort_nl[i]->in[0]->suf_fc, sort_nl[i]->suf_fc, sort_nl[i]->in[0]->suf_fc, sort_nl[i]->suf_fc);
				break;
				//●BUF-------------------------------------------------------------------------------------------------------------------------------
			case BUF:
				fprintf(fp, "1 ~x%d +1 x%d >=1;\n1 x%d +1 ~x%d >=1;\n", sort_nl[i]->in[0]->suf_fc, sort_nl[i]->suf_fc, sort_nl[i]->in[0]->suf_fc, sort_nl[i]->suf_fc);
				break;
				//●EXOR-------------------------------------------------------------------------------------------------------------------------------
			case EXOR:
				fprintf(fp, "1 ~x%d +1 ~x%d +1 ~x%d >=1;\n", sort_nl[i]->in[0]->suf_fc, sort_nl[i]->in[1]->suf_fc, sort_nl[i]->suf_fc);
				fprintf(fp, "1 x%d +1 x%d +1 ~x%d >=1;\n", sort_nl[i]->in[0]->suf_fc, sort_nl[i]->in[1]->suf_fc, sort_nl[i]->suf_fc);
				fprintf(fp, "1 x%d +1 ~x%d +1 x%d >=1;\n", sort_nl[i]->in[0]->suf_fc, sort_nl[i]->in[1]->suf_fc, sort_nl[i]->suf_fc);
				fprintf(fp, "1 ~x%d +1 x%d +1 x%d >=1;\n", sort_nl[i]->in[0]->suf_fc, sort_nl[i]->in[1]->suf_fc, sort_nl[i]->suf_fc);
				break;
			}
			if (f == 0) {//fは何個目の目標故障かを示している　1次故障に対して故障伝搬変数を導入 //CONS_SIZEはとりあえず50000にしてる
				//故障伝搬変数に関する制約作成
				if (0 != sort_nl[i]->n_out) {

					//~ACT(X)
					sprintf_s(cons_prop, 50000, "%s1 ~x%d",
						cons_prop,
						suffix);

					for (int m = 0; m < sort_nl[i]->n_out; m++)
					{
						//+ ACT(Y)
						sprintf_s(cons_prop, 50000, "%s +1 x%d",
							cons_prop,
							suffix);
						suffix++;
					}
					sprintf_s(cons_prop, 50000, "%s >=1;\n",
						cons_prop);

					//OPBcalcSize(&opb.total, 0, 1, 0, 0);
				}
				//~ACT(X) + X' + X >=1
				sprintf_s(cons_prop, 50000, "%s1 ~x%d +1 x%d +1 x%d >=1;\n",
					cons_prop,
					suffix,
					sort_nl[i]->suf_2t,
					sort_nl[i]->suf_fc);

				//~ACT(X) + ~X' + ~X >=1
				sprintf_s(cons_prop, 50000, "%s1 ~x%d +1 ~x%d +1 ~x%d >=1;\n",
					cons_prop,
					suffix,
					sort_nl[i]->suf_2t,
					sort_nl[i]->suf_fc);

				//OPBcalcSize(&opb.total, 0, 1, 0, 0);
				suffix++;
			}
		}
	}
	fprintf(fp, "%s", cons_prop);
	free(cons_prop);
}


//=========================================================================
//  関数名 : pbo_generate_fdetect_c
//  機  能 : 故障検出回路記述
//  戻り値 : なし
//  引  数 : ファイルポインタ，対象故障
//=========================================================================
void pbo_generate_fdetect_c(FILE *fp, UNDET_FAULT_LIST* node) {

	int i, j/*,k*/;
	unsigned int *fdetct_in;

	if ((fdetct_in = (unsigned int*)calloc(fc_ppo, sizeof(unsigned int))) == NULL) {
		printf("【ERROR】:No calloc Fault Det Circuit(sttg_pbo_generate.c)\n"); //故障検出回路の入力配列領域確保失敗
		exit(1);
	}

	j = 0;
	for (i = 0; i < n_net; i++) {
		if (nl[i].fault_pass == YES && nl[i].ppo_flag ==1)  {
			//●EXOR
			fprintf(fp, "1 ~x%d +1 ~x%d +1 ~x%d >=1;\n", nl[i].suf_2t, nl[i].suf_fc, suffix);
			fprintf(fp, "1 x%d +1 x%d +1 ~x%d >=1;\n", nl[i].suf_2t, nl[i].suf_fc, suffix);
			fprintf(fp, "1 x%d +1 ~x%d +1 x%d >=1;\n", nl[i].suf_2t, nl[i].suf_fc, suffix);
			fprintf(fp, "1 ~x%d +1 x%d +1 x%d >=1;\n", nl[i].suf_2t, nl[i].suf_fc, suffix);
			fdetct_in[j] = suffix;
			suffix++;
			j++;
		}
	}

	if (fc_ppo != 1) {
		for (i = 0; (unsigned int)i < fc_ppo; i++) { fprintf(fp, "1 ~x%d +1 x%d >=1;\n", fdetct_in[i], suffix); }
		for (i = 0; (unsigned int)i < fc_ppo; i++) { fprintf(fp, "1 x%d +", fdetct_in[i]); }
		fprintf(fp, "1 ~x%d >=1;\n", suffix);
		fprintf(fp, "1 x%d >=1;\n", suffix);
		suffix++;
	}
	else if (fc_ppo == 1) {
		fprintf(fp, "1 x%d >=1;\n", suffix - 1);
	}

	//ここから故障励起関連だと思う

	for (i = 0; i < n_net; i++) {
		if (strcmp(nl[i].name, node->name) == 0) {

			switch (node->type) {
			case STF:
				fprintf(fp, "1 x%d >=1;\n", nl[i].suf_1t);
				fprintf(fp, "1 ~x%d >=1;\n", nl[i].suf_2t);
				fprintf(fp, "1 x%d >=1;\n", nl[i].suf_fc);
				break;
			case STR:
				fprintf(fp, "1 ~x%d >=1;\n", nl[i].suf_1t);
				fprintf(fp, "1 x%d >=1;\n", nl[i].suf_2t);
				fprintf(fp, "1 ~x%d >=1;\n", nl[i].suf_fc);
				break;
			}
			break;
		}
	}

}

//=========================================================================
//  関数名 : pbo_generate_power
//  機  能 : 消費電力制約
//  戻り値 : なし
//  引  数 : ファイルポインタ
//=========================================================================
void pbo_generate_power(FILE *fp) {
	int i;
	char* cons1 = (char*)NULL;

	if ((cons1 = (char*)calloc(500000, sizeof(char))) == NULL) {
		printf("【ERROR】:No calloc cons1(mttg_pbo_generate.c)\n");
		exit(1);
	}

	for (i = 0; i < n_net; i++) {
		if (nl[i].open_pass == NO) {

			fprintf(fp, "1 x%d +1 x%d x%d +1 ~x%d ~x%d >=1;\n", suffix, nl[i].suf_1t, nl[i].suf_2t, nl[i].suf_1t, nl[i].suf_2t);
			 
			//WSA制約
			sprintf(cons1, "%s-1 x%d ", cons1, suffix);
			suffix++;
		}
	}

	//WSA制約(WSA閾値)
	sprintf(cons1, "%s>=-%d;\n", cons1, n_net *  fi[WSATH].value / 100);
	suffix++;

	fprintf(fp, "%s", cons1);
}
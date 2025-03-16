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
//koizumi追加
#include "../asg/opb/opb.h"

//=========================================================================
//  関数名 : mttg_pbo_generate_min
//  機  能 : MTTG用PBOファイル記述【最適化関数・サイズ(variable・constraint・その他)】
//  戻り値 : なし
//  引  数 : ファイルポインタ
//=========================================================================
void mttg_pbo_generate_min(FILE* fp) {

	PBO size_pbo;
	int i;


	//---------------------------------------------
	//PBOサイズ計算
	//---------------------------------------------
	size_pbo = mttg_calc_pbo_size();

	//-----------------------------------------------------------------------------
	//PBOファイル記述
	//-----------------------------------------------------------------------------
	if (strcmp(fi[LOW_POWER].name, "OFF") == 0) {
		fprintf(fp, "* #variable= %d ", size_pbo.val + 1);
		fprintf(fp, "#constraint= %d\n", size_pbo.h_constraint);
		fprintf(fp, "#product= %d sizeproduct= %d\n", 2 * size_pbo.s_constraint, 4 * size_pbo.s_constraint);
		fprintf(fp, "*\n");
		fprintf(fp, "*  this is a dummy instance\n");
		fprintf(fp, "*\n");
	}
	else if (strcmp(fi[LOW_POWER].name, "ON") == 0) {
		fprintf(fp, "* #variable= %d ", size_pbo.val);
		fprintf(fp, "#constraint= %d ", size_pbo.h_constraint + opb.total.cons);//koizumi追加
		fprintf(fp, "#product= %d sizeproduct= %d\n", 2 * size_pbo.s_constraint, 4 * size_pbo.s_constraint);
		fprintf(fp, "*\n");
		fprintf(fp, "*  this is a dummy instance\n");
		fprintf(fp, "*\n");
	}

	fprintf(fp, "min: 1 x%d", size_pbo.val - size_pbo.s_constraint);
	for (i = 2; i <= tfl_num; i++) {
		fprintf(fp, " +1 x%d", size_pbo.val - size_pbo.s_constraint + i - 1);
	}
	fprintf(fp, ";\n");

}

//=========================================================================
//  関数名 : pbo_generate_fdetect_c_mttg
//  機  能 : 故障検出回路記述(MTTG限定)
//  戻り値 : なし
//  引  数 : ファイルポインタ，対象故障
//=========================================================================
void pbo_generate_fdetect_c_mttg(FILE *fp, UNDET_FAULT_LIST* node) {

	int i, j, k;
	unsigned int *fdetct_in;

	if ((fdetct_in = (unsigned int*)calloc(fc_ppo, sizeof(unsigned int))) == NULL) {
		printf("【ERROR】:No calloc Fault Det Circuit(sttg_pbo_generate.c)\n"); 
		exit(1);
	}

	node->tfl_fc_ppo = fc_ppo;
	node->tfl_suf_fc = node->netlist->suf_fc;

	j = 0;
	for (i = 0; i < n_net; i++) {
		if (nl[i].fault_pass == YES && nl[i].ppo_flag == 1 ) {
			//●EXOR
			fprintf(fp, "1 ~x%d +1 ~x%d +1 ~x%d >=1;\n", nl[i].suf_2t, nl[i].suf_fc, suffix);
			printf("1 ~x%d +1 ~x%d +1 ~x%d >=1;\n", nl[i].suf_2t, nl[i].suf_fc, suffix);
			fprintf(fp, "1 x%d +1 x%d +1 ~x%d >=1;\n", nl[i].suf_2t, nl[i].suf_fc, suffix);
			printf("1 x%d +1 x%d +1 ~x%d >=1;\n", nl[i].suf_2t, nl[i].suf_fc, suffix);
			fprintf(fp, "1 x%d +1 ~x%d +1 x%d >=1;\n", nl[i].suf_2t, nl[i].suf_fc, suffix);
			printf("1 x%d +1 ~x%d +1 x%d >=1;\n", nl[i].suf_2t, nl[i].suf_fc, suffix);
			fprintf(fp, "1 ~x%d +1 x%d +1 x%d >=1;\n", nl[i].suf_2t, nl[i].suf_fc, suffix);
			printf("1 ~x%d +1 x%d +1 x%d >=1;\n", nl[i].suf_2t, nl[i].suf_fc, suffix);
			if (fc_ppo == 1) {
				node->tfl_det_suffix = suffix;
			}
			fdetct_in[j] = suffix;
			suffix++;
			j++;
		}
	}

	if (fc_ppo != 1) {//fc_ppo:故障回路の外部出力の数が複数の場合制約式を追加。追加するのはORゲート
		for (i = 0; (unsigned int)i < fc_ppo; i++) { 
			fprintf(fp, "1 ~x%d +1 x%d >=1;\n", fdetct_in[i], suffix); 
			printf("1 ~x%d +1 x%d >=1;\n", fdetct_in[i], suffix);
		}
		for (i = 0; (unsigned int)i < fc_ppo; i++) { 
			fprintf(fp, "1 x%d +", fdetct_in[i]);
			printf("1 x%d +", fdetct_in[i]);
		}
		fprintf(fp, "1 ~x%d >=1;\n", suffix);
		printf("1 ~x%d >=1;\n", suffix);
		node->tfl_det_suffix = suffix;
		suffix++;
	}
}

//=========================================================================
//  関数名 : pbo_generate_fault_ex
//  機  能 : 故障励起記述(MTTG限定)
//  戻り値 : なし
//  引  数 : ファイルポインタ，対象故障,ハード(mode=0),ソフト(mode=1)
//=========================================================================
void pbo_generate_fault_ex(FILE *fp, UNDET_FAULT_LIST* node,int mode) {

	int i;


	//目標故障1個目(ハード)
	if(mode == 0) {
		fprintf(fp, "1 x%d >=1;\n", node->tfl_det_suffix);
		//故障検出回路における最終出力先を一意に判断するための変数
		//故障が検出される回路上での位置を示す

		switch (node->type) {
		case STF:
			fprintf(fp, "1 x%d >=1;\n", node->netlist->suf_1t);
			fprintf(fp, "1 ~x%d >=1;\n", node->netlist->suf_2t);
			fprintf(fp, "1 x%d >=1;\n", node->tfl_suf_fc);
			break;
		case STR:
			fprintf(fp, "1 ~x%d >=1;\n", node->netlist->suf_1t);
			fprintf(fp, "1 x%d >=1;\n", node->netlist->suf_2t);
			fprintf(fp, "1 ~x%d >=1;\n", node->tfl_suf_fc);
			break;
		}
	}

	//目標故障1個目以外(ソフト)
	else if (mode == 1) {
		fprintf(fp, "1 x%d +1 x%d >=1;\n", suffix, node->tfl_det_suffix);

		switch (node->type) {
		case STF:
			fprintf(fp, "1 x%d +1 x%d >=1;\n", suffix, node->netlist->suf_1t);
			fprintf(fp, "1 x%d +1 ~x%d >=1;\n", suffix, node->netlist->suf_2t);
			fprintf(fp, "1 x%d +1 x%d >=1;\n", suffix,node->tfl_suf_fc);
			break;
		case STR:
			fprintf(fp, "1 x%d +1 ~x%d >=1;\n", suffix,node->netlist->suf_1t);
			fprintf(fp, "1 x%d +1 x%d >=1;\n", suffix,node->netlist->suf_2t);
			fprintf(fp, "1 x%d +1 ~x%d >=1;\n", suffix, node->tfl_suf_fc);
			break;
		}
		suffix++;
	}

}


//=========================================================================
//  関数名 : pbo_generate_power_mttg
//  機  能 : 消費電力制約(MTTG)(ソフト)
//  戻り値 : なし
//  引  数 : ファイルポインタ
//=========================================================================
void pbo_generate_power_mttg(FILE *fp) {
	int i;
	char* cons1 = (char*)NULL;

	if ((cons1 = (char*)calloc(500000, sizeof(char))) == NULL) {
		printf("【ERROR】:No calloc cons1(mttg_pbo_generate.c)\n");
		exit(1);
	}

	//信号線遷移制約
	for (i = 0; i < n_net; i++) {
		if (nl[i].open_pass == NO) {

			fprintf(fp, "1 x%d +1 x%d x%d +1 ~x%d ~x%d >=1;\n", suffix, nl[i].suf_1t, nl[i].suf_2t, nl[i].suf_1t, nl[i].suf_2t);
			
			//WSA制約
			sprintf(cons1,"%s-1 x%d ", cons1, suffix);
			suffix++;
		}
	}

	//WSA制約(WSA閾値)
	sprintf(cons1,  "%s>=-%d;\n",cons1,n_net *  fi[WSATH].value / 100);
	suffix++;

	fprintf(fp, "%s", cons1);

}
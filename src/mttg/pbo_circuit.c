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
//  関数名 : pbo_generate_good_circuit
//  機  能 : 正常回路(2時刻分)の制約式を書く
//  戻り値 : なし
//  引  数 : ファイルポインタ
//=========================================================================
void pbo_generate_good_circuit(FILE *fp) {

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
//  関数名 : pbo_generate_faulty_circuit
//  機  能 : 故障回路(故障箇所のTFO)の制約式を書く
//  戻り値 : なし
//  引  数 : ファイルポインタ,対象故障
//=========================================================================
void pbo_generate_faulty_circuit(FILE* fp, const UNDET_FAULT_LIST* node) {

	int i, j;

	//-----------------------------------------------------------------------------
	//■PBOファイル記述
	//-----------------------------------------------------------------------------
	for (i = 0; i < n_net; i++) {
		if (sort_nl[i]->fault_pass == YES && sort_nl[i] != node->netlist) {
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
		}
	}
}
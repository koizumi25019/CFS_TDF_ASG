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

//=========================================================================
//  関数名 : pbo_generate_good_circuit
//  機  能 : 正常回路(2時刻分)の制約式を書く
//  戻り値 : なし
//  引  数 : ファイルポインタ，必要な範囲(need1: 1時刻目, need2: 2時刻目。信号線ごとに 1 なら書く。NULL なら全て)
//=========================================================================
static unsigned int frame_var(const NLIST* net, int frame) {
	return frame == 1 ? net->suf_1t : net->suf_2t;
}

/* ゲート gate の frame 時刻目の制約式 */
static void write_gate(FILE* fp, const NLIST* gate, int frame) {
	unsigned int out = frame_var(gate, frame);
	int j;

	switch (gate->type) {
	case AND:
		for (j = 0; j < gate->n_in; j++) { fprintf(fp, "1 x%d +1 ~x%d >=1;\n", frame_var(gate->in[j], frame), out); }
		for (j = 0; j < gate->n_in; j++) { fprintf(fp, "1 ~x%d +", frame_var(gate->in[j], frame)); }
		fprintf(fp, "1 x%d >=1;\n", out);
		break;
	case NAND:
		for (j = 0; j < gate->n_in; j++) { fprintf(fp, "1 x%d +1 x%d >=1;\n", frame_var(gate->in[j], frame), out); }
		for (j = 0; j < gate->n_in; j++) { fprintf(fp, "1 ~x%d +", frame_var(gate->in[j], frame)); }
		fprintf(fp, "1 ~x%d >=1;\n", out);
		break;
	case OR:
		for (j = 0; j < gate->n_in; j++) { fprintf(fp, "1 ~x%d +1 x%d >=1;\n", frame_var(gate->in[j], frame), out); }
		for (j = 0; j < gate->n_in; j++) { fprintf(fp, "1 x%d +", frame_var(gate->in[j], frame)); }
		fprintf(fp, "1 ~x%d >=1;\n", out);
		break;
	case NOR:
		for (j = 0; j < gate->n_in; j++) { fprintf(fp, "1 ~x%d +1 ~x%d >=1;\n", frame_var(gate->in[j], frame), out); }
		for (j = 0; j < gate->n_in; j++) { fprintf(fp, "1 x%d +", frame_var(gate->in[j], frame)); }
		fprintf(fp, "1 x%d >=1;\n", out);
		break;
	case INV:
		fprintf(fp, "1 ~x%d +1 ~x%d >=1;\n1 x%d +1 x%d >=1;\n", frame_var(gate->in[0], frame), out, frame_var(gate->in[0], frame), out);
		break;
	case FOUT:
	case BUF:
		fprintf(fp, "1 ~x%d +1 x%d >=1;\n1 x%d +1 ~x%d >=1;\n", frame_var(gate->in[0], frame), out, frame_var(gate->in[0], frame), out);
		break;
	case EXOR:
		fprintf(fp, "1 ~x%d +1 ~x%d +1 ~x%d >=1;\n", frame_var(gate->in[0], frame), frame_var(gate->in[1], frame), out);
		fprintf(fp, "1 x%d +1 x%d +1 ~x%d >=1;\n", frame_var(gate->in[0], frame), frame_var(gate->in[1], frame), out);
		fprintf(fp, "1 x%d +1 ~x%d +1 x%d >=1;\n", frame_var(gate->in[0], frame), frame_var(gate->in[1], frame), out);
		fprintf(fp, "1 ~x%d +1 x%d +1 x%d >=1;\n", frame_var(gate->in[0], frame), frame_var(gate->in[1], frame), out);
		break;
	default:
		break;
	}
}

void pbo_generate_good_circuit(FILE *fp, const unsigned char* need1, const unsigned char* need2) {

	int i;

	for (i = 0; i < n_net; i++) {
		const NLIST* gate = sort_nl[i];
		int index = (int)(gate - nl);
		int write1 = need1 == NULL || need1[index];
		int write2 = need2 == NULL || need2[index];

		switch (gate->type) {
		case DFF:		//2時刻目のFF出力 = 1時刻目のFF入力
			if (write2) fprintf(fp, "1 ~x%d +1 x%d >=1;\n1 x%d +1 ~x%d >=1;\n", gate->in[0]->suf_1t, gate->suf_2t, gate->in[0]->suf_1t, gate->suf_2t);
			break;
		case IN:		//外部入力は2時刻で同じ値
			if (write1 || write2) fprintf(fp, "1 ~x%d +1 x%d >=1;\n1 x%d +1 ~x%d >=1;\n", gate->suf_1t, gate->suf_2t, gate->suf_1t, gate->suf_2t);
			break;
		default:
			if (write1) write_gate(fp, gate, 1);
			if (write2) write_gate(fp, gate, 2);
			break;
		}
	}
}

//=========================================================================
//  必要な範囲の計算(正常回路のうち、制約式を書く必要がある信号線)
//  目標故障ごとに pbo_scope_add_target で、その故障の制約式が使う正常値の信号線を加え、
//  最後に pbo_scope_close で、それらの値を決める入力側の信号線へ広げる。
//  使われない信号線の制約式は、他の変数の値に影響しないので省いても解は変わらない。
//=========================================================================
void pbo_scope_add_target(const UNDET_FAULT_LIST* target, int primary, unsigned char* need1, unsigned char* need2) {
	int site = (int)(target->netlist - nl);

	need1[site] = need2[site] = 1;		//励起: 故障箇所の1時刻目・2時刻目の正常値
	for (int i = 0; i < n_net; i++) {
		const NLIST* gate = &nl[i];
		if (gate->fault_pass != YES) continue;
		if (gate->ppo_flag == 1) need2[i] = 1;				//検出: PPOの正常値と故障値の比較
		if (primary && opt.fault_propagation) need2[i] = 1;	//-fp: 故障伝搬変数が正常値を使う
		if (i == site) continue;
		for (int j = 0; j < gate->n_in; j++) {
			if (gate->in[j]->fault_pass != YES) need2[gate->in[j] - nl] = 1;	//故障回路の外から入る正常値
		}
	}
}

void pbo_scope_close(unsigned char* need1, unsigned char* need2) {
	/* 2時刻目: 出力側(レベルの高いほう)から入力側へ */
	for (int k = n_net - 1; k >= 0; k--) {
		const NLIST* gate = sort_nl[k];
		int index = (int)(gate - nl);
		if (!need2[index]) continue;
		if (gate->type == DFF) { need1[gate->in[0] - nl] = 1; continue; }	//2時刻目のFF出力 = 1時刻目のFF入力
		if (gate->type == IN) { need1[index] = 1; continue; }				//外部入力は2時刻で同じ値
		for (int j = 0; j < gate->n_in; j++) need2[gate->in[j] - nl] = 1;
	}
	/* 1時刻目 */
	for (int k = n_net - 1; k >= 0; k--) {
		const NLIST* gate = sort_nl[k];
		int index = (int)(gate - nl);
		if (!need1[index] || gate->type == DFF || gate->type == IN) continue;
		for (int j = 0; j < gate->n_in; j++) need1[gate->in[j] - nl] = 1;
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
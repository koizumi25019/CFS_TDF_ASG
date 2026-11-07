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
//追加
#include "../asg/opb/opb.h"

//=========================================================================
//  関数名 : pbo_generate_fault_detection
//  機  能 : 故障検出回路記述(MTTG限定)
//  戻り値 : なし
//  引  数 : ファイルポインタ，対象故障
//=========================================================================
void pbo_generate_fault_detection(FILE *fp, UNDET_FAULT_LIST* node) {

	int i, j/*, k*/;
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
			fprintf(fp, "1 ~x%d +1 ~x%d +1 ~x%d >=1;\n", nl[i].suf_2t, nl[i].suf_fc, bool_id);
			fprintf(fp, "1 x%d +1 x%d +1 ~x%d >=1;\n", nl[i].suf_2t, nl[i].suf_fc, bool_id);
			fprintf(fp, "1 x%d +1 ~x%d +1 x%d >=1;\n", nl[i].suf_2t, nl[i].suf_fc, bool_id);
			fprintf(fp, "1 ~x%d +1 x%d +1 x%d >=1;\n", nl[i].suf_2t, nl[i].suf_fc, bool_id);
			if (fc_ppo == 1) {
				node->tfl_det_suffix = bool_id;
			}
			fdetct_in[j] = bool_id;
			bool_id++;
			j++;
		}
	}

	if (fc_ppo != 1) {//fc_ppo:故障回路の外部出力の数が複数の場合制約式を追加。追加するのはORゲート
		for (i = 0; (unsigned int)i < fc_ppo; i++) { 
			fprintf(fp, "1 ~x%d +1 x%d >=1;\n", fdetct_in[i], bool_id);
		}
		for (i = 0; (unsigned int)i < fc_ppo; i++) { 
			fprintf(fp, "1 x%d +", fdetct_in[i]);
		}
		fprintf(fp, "1 ~x%d >=1;\n", bool_id);
		node->tfl_det_suffix = bool_id;
		bool_id++;
	}
	free(fdetct_in);
}

//=========================================================================
//  関数名 : pbo_generate_fault_propagation
//  機  能 : 故障伝搬変数の制約(一次故障のみ, -fp ON)
//           故障回路の各信号線Xに故障伝搬変数 X_D を導入する。
//             X_D = 1 : 故障の影響がXを通って観測点(PPO)まで伝搬している
//           (1) X_D -> (X_G != X_F)     : (~X_D + X_G + X_F)(~X_D + ~X_G + ~X_F)
//                 X_G:正常回路(2時刻目)の値, X_F:故障回路の値
//           (2) X_D -> (Y_D + Z_D + ...) : Xの出力先のうち観測点へ到達できるもののどれか
//                 観測点(PPO)は伝搬の終点なので(2)を作らない
//           さらに故障箇所では X_D = 1 とする(単位節)。(1)(2)だけでは全ての X_D を
//           0 にして満たせてしまい、故障の影響が消滅した時点でソルバが矛盾を検出できない。
//           観測点へ到達できない信号線は故障の影響を伝えられないので X_D = 0 とする。
//  戻り値 : なし
//  引  数 : ファイルポインタ，一次故障(specific_fault_range 実行直後であること)
//=========================================================================
void pbo_generate_fault_propagation(FILE* fp, const UNDET_FAULT_LIST* node) {

	int i, m, changed;
	int site = -1;
	unsigned char* reach;

	if ((reach = (unsigned char*)calloc(n_net, sizeof(unsigned char))) == NULL) {
		printf("【ERROR】:No calloc propagation reach(mttg_pbo_generate.c)\n");
		exit(1);
	}

	//故障回路(故障箇所のTFO)内で，観測点(PPO)へ到達できる信号線を求める
	do {
		changed = 0;
		for (i = 0; i < n_net; i++) {
			if (nl[i].fault_pass != YES || reach[i]) continue;
			if (nl[i].ppo_flag == 1) {
				reach[i] = 1;
				changed = 1;
				continue;
			}
			for (m = 0; m < nl[i].n_out; m++) {
				NLIST* out = nl[i].out[m];
				if (out->fault_pass == YES && reach[out - nl]) {
					reach[i] = 1;
					changed = 1;
					break;
				}
			}
		}
	} while (changed);

	for (i = 0; i < n_net; i++) {
		if (nl[i].fault_pass != YES || nl[i].prop == 0) continue;
		if (&nl[i] == node->netlist) site = i;

		//観測点へ到達できない信号線は故障の影響を伝えられない
		if (!reach[i]) {
			fprintf(fp, "1 ~x%u >=1;\n", nl[i].prop);
			continue;
		}

		//(2) 出力先のどれかへ伝搬(観測点は終点なので作らない)
		if (nl[i].ppo_flag != 1) {
			fprintf(fp, "1 ~x%u", nl[i].prop);
			for (m = 0; m < nl[i].n_out; m++) {
				NLIST* out = nl[i].out[m];
				if (out->fault_pass == YES && reach[out - nl] && out->prop != 0) {
					fprintf(fp, " +1 x%u", out->prop);
				}
			}
			fprintf(fp, " >=1;\n");
		}

		//(1) 伝搬しているなら正常値と故障値は異なる
		fprintf(fp, "1 ~x%u +1 x%u +1 x%u >=1;\n", nl[i].prop, nl[i].suf_2t, nl[i].suf_fc);
		fprintf(fp, "1 ~x%u +1 ~x%u +1 ~x%u >=1;\n", nl[i].prop, nl[i].suf_2t, nl[i].suf_fc);
	}

	//故障箇所の故障伝搬変数は1(縮退故障の制約により正常値と故障値は必ず異なる)
	if (site >= 0) {
		fprintf(fp, "1 x%u >=1;\n", nl[site].prop);
	}

	free(reach);
}

//=========================================================================
//  関数名 : pbo_generate_fault_excitation
//  機  能 : 故障励起記述(MTTG限定)
//  戻り値 : なし
//  引  数 : ファイルポインタ，対象故障，一次故障か(1: 必ず励起する(ハード), 0: 緩和変数つき(ソフト))
//=========================================================================
void pbo_generate_fault_excitation(FILE *fp, const UNDET_FAULT_LIST* node, int primary) {

	//int i;


	//目標故障1個目(ハード)
	if (primary) {
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
	else {
		fprintf(fp, "1 x%d +1 x%d >=1;\n", bool_id, node->tfl_det_suffix);

		switch (node->type) {
		case STF:
			fprintf(fp, "1 x%d +1 x%d >=1;\n", bool_id, node->netlist->suf_1t);
			fprintf(fp, "1 x%d +1 ~x%d >=1;\n", bool_id, node->netlist->suf_2t);
			fprintf(fp, "1 x%d +1 x%d >=1;\n", bool_id,node->tfl_suf_fc);
			break;
		case STR:
			fprintf(fp, "1 x%d +1 ~x%d >=1;\n", bool_id,node->netlist->suf_1t);
			fprintf(fp, "1 x%d +1 x%d >=1;\n", bool_id,node->netlist->suf_2t);
			fprintf(fp, "1 x%d +1 ~x%d >=1;\n", bool_id, node->tfl_suf_fc);
			break;
		}
		bool_id++;
	}

}


//=========================================================================
//  関数名 : pbo_generate_power
//  機  能 : 消費電力制約(MTTG)(ソフト)
//  戻り値 : なし
//  引  数 : WSA閾値[%], ファイルポインタ
//=========================================================================
void pbo_generate_power(int wsath, FILE* fp) {
	int i;
	unsigned int wsa_bool_id = bool_id;

	//信号線遷移制約
	for (i = 0; i < n_net; i++) {
		if (nl[i].open_pass == NO) {

			fprintf(fp, "1 x%d +1 x%d x%d +1 ~x%d ~x%d >=1;\n", bool_id, nl[i].suf_1t, nl[i].suf_2t, nl[i].suf_1t, nl[i].suf_2t);

			bool_id++;
		}
	}

	// WSA threshold constraint
	for (i = 0; i < n_net; i++) {
		if (nl[i].open_pass == NO) fprintf(fp, "-1 x%d ", wsa_bool_id++);
	}
	fprintf(fp, ">=-%d;\n", n_net * wsath / 100);
	bool_id++;

}

//=========================================================================
//  関数名 : pbo_count_transition_nets
//  機  能 : 低消費電力の制約で遷移を数える信号線(FFから到達できる信号線)の数
//  戻り値 : 信号線数
//  引  数 : なし
//=========================================================================
int pbo_count_transition_nets(void) {
	int count = 0;

	for (int i = 0; i < n_net; i++) {
		if (nl[i].open_pass == NO) count++;
	}
	return count;
}

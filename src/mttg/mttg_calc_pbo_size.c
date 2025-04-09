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
//  関数名 : mttg_calc_pbo_size
//  機  能 : PBOのサイズを計算
//  戻り値 : なし
//  引  数 : なし
//=========================================================================
PBO mttg_calc_pbo_size(void) {

	PBO pbo;
	pbo.val = 0;
	pbo.h_constraint = 0;
	pbo.s_constraint = 0;
	suffix = n_net*2+101;//追加



	//---------------------------------------------
	//制約数計算
	//---------------------------------------------
	//ハード制約(正常回路・故障回路・故障検出回路）数計算
	pbo = mttg_calc_hard_constraint();
	//ソフト制約(tflの残りの故障検出励起）数計算
	pbo.s_constraint = mttg_calc_soft_constraint();

	if (strcmp(fi[LOW_POWER].name, "OFF") == 0) {
		pbo.h_constraint += pbo.s_constraint;
		//---------------------------------------------
		//変数計算(tfl_num!=1)
		//---------------------------------------------
		if (tfl_num != 1) {
			pbo.val += pbo.s_constraint;
		}
	}
	//ソフト制約(消費電力制約)数計算(消費電力考慮あり)
	if (strcmp(fi[LOW_POWER].name , "ON")==0) {
		pbo.s_constraint += untest_calc_soft_constraint();
		pbo.h_constraint += pbo.s_constraint;
		//---------------------------------------------
		//変数計算(tfl_num!=1)
		//---------------------------------------------
		if (tfl_num != 1) {
			pbo.val += pbo.s_constraint;
		}
	}

	//---------------------------------------------
	//変数計算(tfl_num==1)
	//---------------------------------------------
	if (tfl_num == 1) {
		if (fc_ppo == 1) {
			pbo.val = suffix + fc_ppo + pbo.s_constraint - 1;
		}
		else if (fc_ppo != 1) {
			pbo.val = suffix + fc_ppo + 1 + pbo.s_constraint;
		}
	}

	return pbo;

}


//=========================================================================
//  関数名 : mttg_calc_hard_constraint
//  機  能 : 変数・制約数計算(ハード)
//  戻り値 : PBOサイズ
//  引  数 : なし
//=========================================================================
PBO mttg_calc_hard_constraint(void) {
	int i,j=0;
	int fault_det;
	PBO p;
	UNDET_FAULT_LIST* head;
	UNDET_FAULT_LIST* tmp;

	p.val = 0;
	p.h_constraint = 0;

	if (tfl == NULL) {
		printf("目標故障集合設定エラー\n");
		exit(1);
	}
	head = tfl;

	//-------------------------------------------------------------------------------------
	//正常回路について
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------
	//	正常回路の制約数を計算
	//  正常回路の制約は論理ゲートの基本動作を表現するので、モデルによらず同じ
	//-------------------------------------------------------------------
	for (i = 0; i < n_net; i++) {
		switch (nl[i].type) {
		case AND:
			p.h_constraint += nl[i].n_in + 1;
			break;
		case NAND:
			p.h_constraint += nl[i].n_in + 1;
			break;
		case OR:
			p.h_constraint += nl[i].n_in + 1;
			break;
		case NOR:
			p.h_constraint += nl[i].n_in + 1;
			break;
		case INV:
			p.h_constraint += 2;
			break;
		case FOUT:
			p.h_constraint += 2;
			break;
		case BUF:
			p.h_constraint += 2;
			break;
		case EXOR:
			p.h_constraint += 4;
			break;
		}
	}
	//2時刻目の正常回路
	p.h_constraint = p.h_constraint * 2;
	//1時刻目と2時刻目のFFを接続するDFF
	p.h_constraint += 2 * n_dff;
	//1時刻目と2時刻目のPIを接続するDFF
	p.h_constraint += n_pi * 2;

	//-------------------------------------------------------------------------------------
	//故障回路について
	//-------------------------------------------------------------------------------------
	while (head != NULL) {
		tmp = head->next;	//tmpに次の目標故障ノードを保存
		specific_fault_range(head->name);	//故障範囲の特定
		//-------------------------------------------------------------------
		//	故障回路の制約数を計算
		//-------------------------------------------------------------------
		for (i = 0; i < n_net; i++) {
			if (nl[i].fault_pass == YES && strcmp(nl[i].name, head->name) != 0) {
				switch (nl[i].type) {
				case AND:
					p.h_constraint += nl[i].n_in + 1;
					break;
				case NAND:
					p.h_constraint += nl[i].n_in + 1;
					break;
				case OR:
					p.h_constraint += nl[i].n_in + 1;
					break;
				case NOR:
					p.h_constraint += nl[i].n_in + 1;
					break;
				case INV:
					p.h_constraint += 2;
					break;
				case FOUT:
					p.h_constraint += 2;
					break;
				case BUF:
					p.h_constraint += 2;
					break;
				case EXOR:
					p.h_constraint += 4;
					break;
				}
			}
		}
		//-------------------------------------------------------------------------------------
		// 故障検出回路について
		//-------------------------------------------------------------------------------------
		//-------------------------------------------------------------------
		//	故障検出回路の制約数を計算
		//-------------------------------------------------------------------
		//PPOが単一の場合【EXOR１つ分】
		if (fc_ppo == 1) { p.h_constraint += 4; }
		//PPOが複数の場合【EXOR複数(PPO数分)＆ORゲート】
		else if (fc_ppo != 1) { p.h_constraint += fc_ppo * 4 + fc_ppo + 1; }
		//-------------------------------------------------------------------
		//	変数の数を計算
		//-------------------------------------------------------------------
		//tflが複数ある場合
		if (tfl_num != 1) {
			if (fc_ppo == 1)p.val += suffix - p.val; 	            //PPOが単一の場合
			else if (fc_ppo != 1)p.val += suffix + fc_ppo - p.val;	//PPOが複数の場合*/
		}
		//---------------------------------------------
		//故障励起について
		//---------------------------------------------
		//1個目だけハード残りソフト
		if (j == 0) {
			fault_det = /*head->input_num * 2 + */3 + 1;
			p.h_constraint += fault_det;
		}
		j++;

		//添え字設定(tfl_num!=1)
		if (tfl_num != 1) {
			suffix = p.val + 1;
		}

		//次の故障へ
		head = tmp;
	}

	//戻り値：変数の数・制約数
	return p;
}

//=========================================================================
//  関数名 : mttg_calc_soft_constraint
//  機  能 : 制約数計算(ソフト　1つ目以外の目標故障励起)
//  戻り値 : なし
//  引  数 : なし
//=========================================================================
int mttg_calc_soft_constraint(void) {
	UNDET_FAULT_LIST* head;
	int n = 0;
	int fault_det;

	head = tfl->next;

	while (head != NULL) {
		fault_det = 3 + 1;
		n += fault_det;
		head = head->next;
	}

	return n;
}
//------------------------------------------------------------------------
//インクルード
//------------------------------------------------------------------------
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<string.h>

//------------------------------------------------------------------------
//自作インクルード
//------------------------------------------------------------------------
#include"../header/Define.h"
#include"../asg/opb/clasp/clasp.h"
#include"../asg/createSGmodel.h"
#include"../asg/opb/opb.h"


//===================================================
//関数名:mttg_main
//機能:遷移故障のMTTGのメイン
//戻り値：int 正常終了:SUCCESS(=0) , 異常終了:ERROR(=-1)
//引数：なし
//===================================================
int mttg_main(void) {
	int i = 0;
	char txt_cmd[200];
	char cmd[500];
	int unsat_flag = OFF;
	UNDET_FAULT_LIST* head, * next;
	int det_fault_clasp_num;
	int loop = 0;
	//シードファイルオープン
	sprintf(txt_cmd, "%sseed.txt", folder_out);
	if ((fout_seed = (fopen(txt_cmd, "w"))) == NULL) {
		printf("【ERROR】:File open seed.txt(mttg_main.c)\n");
		return ERROR;
	}

	//ファイル読み込み
	if (ReadFile() != true) return ERROR;

	//PINファイル生成
	if (pin_make() != SUCCESS) {
		printf("【ERROR】:pin make(mttg_main.c)\n");
		return ERROR;
	}

	//レベライズ
	if (levelize() != SUCCESS) {
		printf("【ERROR】:levelize(mttg_main.c)\n");
		return ERROR;
	}

	//未検出故障リストの作成
	if (fi[FAULT].flag == ON) {
		if (transition_delay_fault_set() != SUCCESS) {
			printf("【ERROR】:Transition_Delay_Fault_Set(mttg_main.c)\n");
			return ERROR;
		}
	}

	//全故障数の格納
	all_num = ufl_num;

	//標準出力
	standard_output();

	/** create the constraint for good-circuit */
	AssigneVarsGC();

	//LFSR+PS制約式取得
	CreateLFSRmodel();

	//テスト生成
	while (ufl_num != 0) {

		//PBOファイル
		sprintf(txt_cmd, "%smttg_pbo.txt", folder_out);
		if ((fout_mttg_pbo = fopen(txt_cmd, "w")) == NULL) {
			printf("【ERROR】:File open mttg_pbo.txt(mttg_main.c)\n");
			exit(1);
		}

		//目標故障選択
		if (fault_select(unsat_flag,loop) != SUCCESS) {
			printf("【ERROR】:Fault Select(mttg_main.c)\n");
			return ERROR;
		}
		file_output(0, 3);

		//PBOサイズ記述
		mttg_pbo_generate_min(fout_mttg_pbo);

		//PS制約式記述
		makeProbFileConsPS_clasp(fout_mttg_pbo);

		//正常回路の記述
		pbo_generate_normal_c(fout_mttg_pbo);

		suffix = n_net * 2 + 101;

		head = tfl;
		i = 0;
		while (head != NULL) {
			//故障範囲の特定
			specific_fault_range(head->name);
			//故障回路の記述
			pbo_generate_fault_c(fout_mttg_pbo, head);
			//故障検出回路の記述
			pbo_generate_fdetect_c_mttg(fout_mttg_pbo, head);
			//次の故障
			head = head->next;
			i++;
		}

		//故障励起
		head = tfl;
		i = 0;
		while (head != NULL) {
			if (i == 0) {
				pbo_generate_fault_ex(fout_mttg_pbo, head,0);
			}
			else {
				pbo_generate_fault_ex(fout_mttg_pbo, head, 1);
			}
			head = head->next;
			i++;
		}

		//低消費電力制約
		if (strcmp(fi[LOW_POWER].name, "ON") == 0) {
			pbo_generate_power_mttg(fout_mttg_pbo);
		}

		fclose(fout_mttg_pbo);

		//========================================================================
		//	clasp ⇒ テストベクトル生成
		//========================================================================
		//Clasp
		sprintf(cmd, "clasp.exe %smttg_pbo.txt --time-limit %d > %sclasp_log_mttg.txt", folder_out, fi[TIME].value, folder_out);
		system(cmd);

		//clasp情報取得
		if (CLASP() != CLASP_OKAY) {
			printf("\n	SYSTEM ERROR: solver error. ");
			return ERROR;
		}

		//故障シミュレーション用テストパターンファイルオープン
		sprintf(txt_cmd, "%stp.txt", folder_out);
		FILE* fout_tp = fopen(txt_cmd, "w");
		if (fout_tp == NULL) {
			printf("【ERROR】:File open tp.txt(mttg_main.c)\n");
			return ERROR;
		}
		fprintf(fout_tp, "%s\n", clasp.sol[SOL_TP]);
		fclose(fout_tp);

		//低消費テストパターンか確認(消費電力指向時のみ)
		if ((mttg_check_wsa() != SAT) && (strcmp(fi[LOW_POWER].name, "ON") == 0)) {
			printf("【ERROR】:MTTG TEST NG PATTERN(mttg_main.c)\n");
			return ERROR;
		}

		//論理シミュレーション
		if (logic_sim() != SUCCESS) {
			printf("【ERROR】:Logic Simulation(mttg_main.c)\n");
			return ERROR;
		}

		//シード集合へ出力
		fprintf(fout_seed, "%s\n", clasp.sol[SOL_SEED]);

		//========================================================================
		//	故障シミュレーション
		//========================================================================
		sprintf(cmd, "XID2.exe -c %s -tx %stp.txt -pin %spin.txt -fm TDF -fsim YES -dic %sfsim_log.txt -det %sxid_det.txt -undet %sxid_undet.txt", fi[CIRCUIT].name, folder_out, folder_out, folder_out, folder_out, folder_out);
		system(cmd);

		//コンソールアプリケーションの初期化
		//system("cls");

		//検出故障特定&検出故障数を未検出故障から削除
		if ((det_fault_clasp_num = fault_judge()) == ERROR) {
			printf("【ERROR】:Fault Judge(mttg_main.c)\n");
			return ERROR;
		}

		//例外終了条件
		if (det_fault_clasp_num == 0) {
			unsat_flag = ON;
		}
		else unsat_flag = OFF;

	}

	num_seed = loop;
	fclose(fout_seed);

	return SUCCESS;
}
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
	UNDET_FAULT_LIST* head/*, * next*/;
	int det_fault_clasp_num;
	int loop = 0;
	SORTED sorted;
	int resolving_count = 0;
	int change_wsath_flag = OFF;
	int sttg_flag = OFF;

	//シードファイルオープン
	sprintf(txt_cmd, "%sseed.txt", folder_out);
	if ((fout_seed = (fopen(txt_cmd, "w"))) == NULL) {
		printf("【ERROR】:File open seed.txt(mttg_main.c)\n");
		return ERROR;
	}

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

	//ファイル読み込み
	if (ReadFile() != true) return ERROR;

	//標準出力
	standard_output();

	//ASGの正常回路におけるブール変数番号設定
	AssigneVarsGC();

	//LFSR+PS制約式取得
	CreateLFSRmodel();

	//テスト生成
	while (ufl_num != 0) {

		//PBOファイル
		sprintf(txt_cmd, "%smttg_pbo.txt", folder_out);
		if ((fout_mttg_pbo = fopen(txt_cmd, "w")) == NULL) {
			printf("【ERROR】:File open mttg_pbo.txt(mttg_main.c)\n");
			printf("Trying to open: '%s'\n", txt_cmd);
			perror(txt_cmd);
			exit(1);
		}

		//目標故障選択
		if (fault_select(sttg_flag,&sorted,loop++) != SUCCESS) {
			printf("【ERROR】:Fault Select(mttg_main.c)\n");
			return ERROR;
		}

		file_output(0, 3);

		//PBOサイズ記述

		mttg_pbo_generate_min(fout_mttg_pbo);

		//フェーズシフタ制約式記述
		makeProbFileConsPS_clasp(fout_mttg_pbo);

		//正常回路の記述
		pbo_generate_normal_c(fout_mttg_pbo);

		suffix = n_net * 2 + 101;

		head = tfl;
		i = 0;
		while (head != NULL) {
			//故障範囲の特定
			specific_fault_range(head->name,i);
			//故障回路の記述
			pbo_generate_fault_c(fout_mttg_pbo, head,i);//変更
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
			pbo_generate_power_mttg(change_wsath_flag,fout_mttg_pbo);
		}

		fclose(fout_mttg_pbo);

		//========================================================================
		//	clasp 　 シード生成
		//========================================================================
		if (CLASP() != CLASP_OKAY) {
			printf("\n	SYSTEM ERROR: solver error. ");
			return ERROR;
		}

		//シード生成不能故障でないの場合
		if (clasp.status != CLASP_STATUS_UNKNOWN && clasp.status != CLASP_UNSAT) {

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
		}
		//打ち切り故障と判定
		else {
			if (clasp.status == CLASP_UNSAT) {//まだunknown->打ち切り故障の可能性があるので，STTGに切り替えるためにまだ未検出故障から削除しない
				//シード生成不能故障を未検出故障リストから削除
				UNDET_FAULT_LIST* unseed_fault;
				UNDET_FAULT_LIST* pre_node = NULL;
				unseed_fault = ufl;
				while (1) {
					if (tfl->type == unseed_fault->type && strcmp(tfl->name, unseed_fault->name) == 0) {//信号線と故障タイプが一致するか確認
						bitintSetNbit_Zero(detflag, unseed_fault->node_id);
						ufl_free(unseed_fault, pre_node, 0);
						break;
					}
					pre_node = unseed_fault;
					unseed_fault = unseed_fault->next;
				}
				//目標故障解放
				UNDET_FAULT_LIST* node;
				UNDET_FAULT_LIST* d_head;
				node = tfl;
				while (node != NULL) {
					d_head = node->next;
					free(node);
					node = d_head;
				}
			}
			//WSA閾値を変更したSTTGでもシードが求まらない場合は異常終了
			else if (sttg_flag == ON && change_wsath_flag == ON) {
				printf("WSAを変更したSTTGでもシードが求まりませんでした。異常終了します。\n");
				exit(1);
			}
			//STTGを行ったにもかかわらずシードが求まらない場合，WSA閾値を変更して再シード生成
			else if(sttg_flag==ON){
				change_wsath_flag == ON;
			}
			//CLASP UNKNOWN STTGに切り替えて再シード生成
			else{
            sttg_flag = ON;
			}
		}
	}

	num_seed = loop;
	fclose(fout_seed);
	return SUCCESS;
}
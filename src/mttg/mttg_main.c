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

// [STTG/WSATH INTEGRATION] Release copied target faults before selecting a retry target.
static void release_target_faults(void) {
	UNDET_FAULT_LIST* node = tfl;
	while (node != NULL) {
		UNDET_FAULT_LIST* next = node->next;
		free(node);
		node = next;
	}
	tfl = NULL;
	tfl_num = 0;
}

static int discard_target_as_unseed(void) {
	UNDET_FAULT_LIST* target;
	UNDET_FAULT_LIST* current;
	UNDET_FAULT_LIST* previous;
	if (tfl == NULL) return ERROR;
	target = tfl;
	current = ufl;
	previous = NULL;
	while (current != NULL) {
		if (current->type == target->type && strcmp(current->name, target->name) == 0) {
			bitintSetNbit_Zero(detflag, current->node_id);
			ufl_free(current, previous, 0);
			release_target_faults();
			return SUCCESS;
		}
		previous = current;
		current = current->next;
	}
	release_target_faults();
	return ERROR;
}


//===================================================
//関数名:mttg_main
//機能:遷移故障のMTTGのメイン
//戻り値：int 正常終了:SUCCESS(=0) , 異常終了:ERROR(=-1)
//引数：なし
//===================================================
/* Count the emitted constraints instead of maintaining a second circuit model. */
static int write_problem_file(FILE* body, const char* path, unsigned int relaxation_start)
{
    FILE* output = NULL;
    unsigned int maximum_variable = 0;
    unsigned int constraint_count = 0;
    int character;
    rewind(body);
    while ((character = fgetc(body)) != EOF) {
        if (character == ';') {
            constraint_count++;
        }
        if (character == 'x') {
            unsigned int variable;
            if (fscanf_s(body, "%u", &variable) == 1 && variable > maximum_variable) {
                maximum_variable = variable;
            }
        }
    }
    if (ferror(body) || fopen_s(&output, path, "w") != 0) {
        return ERROR;
    }
    {
        int transition_count = strcmp(fi[LOW_POWER].name, "ON") == 0
            ? untest_calc_soft_constraint() : 0;
        fprintf(output, "* #variable= %u #constraint= %u #product= %d sizeproduct= %d\n",
            maximum_variable, constraint_count, 2 * transition_count, 4 * transition_count);
    }
    fprintf(output, "min:");
    for (int target = 1; target < tfl_num; target++) {
        fprintf(output, " +1 x%u", relaxation_start + target - 1);
    }
    fprintf(output, ";\n");
    rewind(body);
    while ((character = fgetc(body)) != EOF) {
        fputc(character, output);
    }
    {
        int failed = ferror(body) || ferror(output);
        if (fclose(output) != 0) {
            failed = 1;
        }
        return failed ? ERROR : SUCCESS;
    }
}

int mttg_main(void) {
	unsigned int relaxation_start;
	int i = 0;
	char output_path[1024];
	char simulation_command[4096];
	UNDET_FAULT_LIST* target_fault/*, * next*/;
	int detected_fault_count;
	int generation_attempt_count = 0;
	SORTED sorted = {0};
	int change_wsath_25 = OFF; // [STTG/WSATH INTEGRATION]
	int change_wsath_30 = OFF; // [STTG/WSATH INTEGRATION]
	int sttg_flag = OFF;
	int retry_single_fault = OFF; // [CPS INTEGRATION]

	//シードファイルオープン
	sprintf_s(output_path, sizeof(output_path), "%sseed.txt", folder_out);
	if (fopen_s(&fout_seed, output_path, "w") != 0) {
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
	if (CreateLFSRmodel() != true) {
		return ERROR;
	}

	//テスト生成
	while (ufl_num != 0) {

		//PBOファイル
		sprintf_s(output_path, sizeof(output_path), "%smttg_pbo.txt", folder_out);
		fout_mttg_pbo = tmpfile();
		if (fout_mttg_pbo == NULL) {
			return ERROR;
		}

		//目標故障選択
		int single_fault_flag = (sttg_flag == ON || retry_single_fault == ON) ? ON : OFF;
		if (fault_select(single_fault_flag,&sorted,generation_attempt_count++) != SUCCESS) {
			printf("【ERROR】:Fault Select(mttg_main.c)\n");
			return ERROR;
		}

		file_output(0, 3);

		//PBOサイズ記述



		//フェーズシフタ制約式記述
		makeProbFileConsPS_clasp(fout_mttg_pbo);

		//正常回路の記述
		pbo_generate_normal_c(fout_mttg_pbo);

		bool_id = n_net * 2 + 101;

		target_fault = tfl;
		i = 0;
		while (target_fault != NULL) {
			//故障範囲の特定
			specific_fault_range(target_fault->name,i);
			//故障回路の記述
			pbo_generate_fault_c(fout_mttg_pbo, target_fault,i);//変更
			//故障検出回路の記述
			pbo_generate_fdetect_c_mttg(fout_mttg_pbo, target_fault);
			//次の故障
			target_fault = target_fault->next;
			i++;
		}

		relaxation_start = bool_id;
		//故障励起
		target_fault = tfl;
		i = 0;
		while (target_fault != NULL) {
			if (i == 0) {
				pbo_generate_fault_ex(fout_mttg_pbo, target_fault,0);
			}
			else {
				pbo_generate_fault_ex(fout_mttg_pbo, target_fault, 1);
			}
			target_fault = target_fault->next;
			i++;
		}

		//低消費電力制約
		if (strcmp(fi[LOW_POWER].name, "ON") == 0) {
			pbo_generate_power_mttg(change_wsath_25, change_wsath_30, fout_mttg_pbo);
		}

		if (write_problem_file(fout_mttg_pbo, output_path, relaxation_start) != SUCCESS) {
			fclose(fout_mttg_pbo);
			return ERROR;
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
			sprintf_s(output_path, sizeof(output_path), "%stp.txt", folder_out);
			FILE* fout_tp = NULL;
			fopen_s(&fout_tp, output_path, "w");
			if (fout_tp == NULL) {
				printf("【ERROR】:File open tp.txt(mttg_main.c)\n");
				return ERROR;
			}
			fprintf(fout_tp, "%s\n", clasp.sol[SOL_TP]);
			fclose(fout_tp);

			//低消費テストパターンか確認(消費電力指向時のみ)
			if (strcmp(fi[LOW_POWER].name, "ON") == 0 &&
				mttg_check_wsa(change_wsath_25, change_wsath_30) != SAT) {
				printf("【ERROR】:MTTG TEST NG PATTERN(mttg_main.c)\n");
				return ERROR;
			}

			//論理シミュレーション
			if (logic_sim() != SUCCESS) {
				printf("【ERROR】:Logic Simulation(mttg_main.c)\n");
				return ERROR;
			}

			//シード集合へ出力
			//========================================================================
			//	故障シミュレーション
			//========================================================================
			sprintf_s(simulation_command, sizeof(simulation_command), "XID2.exe -c \"%s\" -tx \"%stp.txt\" -pin \"%spin.txt\" -fm TDF -fsim YES -dic \"%sfsim_log.txt\" -det \"%sxid_det.txt\" -undet \"%sxid_undet.txt\"", fi[CIRCUIT].name, folder_out, folder_out, folder_out, folder_out, folder_out);
			if (system(simulation_command) != 0) {
				printf("[ERROR] XID2 fault simulation failed.\n");
				return ERROR;
			}

			//コンソールアプリケーションの初期化
			//system("cls");

			//検出故障特定&検出故障数を未検出故障から削除
			if ((detected_fault_count = fault_judge(sttg_flag, change_wsath_25)) == ERROR) {
				printf("【ERROR】:Fault Judge(mttg_main.c)\n");
				return ERROR;
			}
			if (detected_fault_count > 0) {
				fprintf(fout_seed, "%s\n", clasp.sol[SOL_SEED]);
			}
			if (detected_fault_count == 0 && (single_fault_flag == ON || strcmp(fi[CPS_MODE].name, "ON") != 0)) {
				printf("[ERROR] Solver/simulator mismatch: no remaining fault was detected.\n");
				return ERROR;
			}
			// [CPS INTEGRATION] Retry the next iteration with one fault after zero detection.
			tfl = NULL; // fault_judge releases the copied target list
			sttg_flag = OFF;
			change_wsath_25 = OFF;
			change_wsath_30 = OFF;
			if (strcmp(fi[CPS_MODE].name, "ON") == 0) {
				retry_single_fault = (detected_fault_count == 0) ? ON : OFF;
				if (retry_single_fault == ON) {
					printf("[CPS] No fault was detected; single-fault mode is scheduled.\n");
				}
			}
		}
		//打ち切り故障と判定
		else {
			// [STTG/WSATH INTEGRATION] Bounded state machine for solver UNKNOWN.
			if (clasp.status == CLASP_UNSAT || strcmp(fi[STTG_WSATH_MODE].name, "ON") != 0) {
				if (discard_target_as_unseed() != SUCCESS) {
					printf("[ERROR] Failed to discard the current target fault.\n");
					return ERROR;
				}
				sttg_flag = OFF;
				change_wsath_25 = OFF;
				change_wsath_30 = OFF;
				retry_single_fault = OFF;
				printf("[MTTG] Target fault moved to the unseed list.\n");
				continue;
			}

			if (sttg_flag == ON && (strcmp(fi[LOW_POWER].name, "ON") != 0 || change_wsath_30 == ON)) {
				if (discard_target_as_unseed() != SUCCESS) {
					printf("[ERROR] Failed to discard the final STTG target fault.\n");
					return ERROR;
				}
				sttg_flag = OFF;
				change_wsath_25 = OFF;
				change_wsath_30 = OFF;
				printf("[STTG/WSATH] Retry stages exhausted; target moved to unseed.\n");
				continue;
			}

			release_target_faults();
			if (sttg_flag == OFF) {
				sttg_flag = ON;
				printf("[STTG/WSATH] Retry with STTG.\n");
			}
			else if (change_wsath_25 == OFF) {
				change_wsath_25 = ON;
				printf("[STTG/WSATH] Retry STTG with WSA threshold 25.\n");
			}
			else {
				change_wsath_30 = ON;
				printf("[STTG/WSATH] Retry STTG with WSA threshold 30.\n");
			}
			continue;
		}
	}

	num_seed = seed_num;
	fclose(fout_seed);
	return SUCCESS;
}

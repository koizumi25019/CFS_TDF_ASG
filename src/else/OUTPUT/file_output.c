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
#include"../../header/Define.h"

//===================================================
//関数名:file_output
//機能:ファイル生成
//戻り値：int 正常終了:0 , 異常終了:1
//引数：生成テストパターン数格納変数,時間,テスト不能故障時(=0),全故障ファイル(=1),ログファイル(=2),目標故障集合ログファイル(=3)
//===================================================
int file_output(clock_t time,int mode) {
	UNDET_FAULT_LIST* node;
	UNDET_FAULT_LIST *head;
	int j = 0;
	char txt_cmd[500];

	if (mode == 0) {

		//信号線リテラル情報ファイル
		sprintf(txt_cmd, "%ssignal_literal.txt", folder_out);
		if ((fout_literal = (fopen(txt_cmd, "w"))) == NULL) {
			printf("【ERROR】:File open signal_literal.txt(file_output.c)\n");
			return ERROR;
		}

		//テスト不能故障ファイル
		sprintf(txt_cmd, "%suntest_fault.txt", folder_out);
		if ((fout_untest_fault = (fopen(txt_cmd, "w"))) == NULL) {
			printf("【ERROR】:File open untest_fault.txt(file_output.c)\n");
			return ERROR;
		}

		//高消費電力故障ファイル
		if ((strcmp(fi[LOW_POWER].name, "ON")) == 0) {
			sprintf(txt_cmd, "%suntest_high_fault.txt", folder_out);
			if ((fout_high_fault = (fopen(txt_cmd, "w"))) == NULL) {
				printf("【ERROR】:File open untest_high_fault.txt(file_output.c)\n");
				return ERROR;
			}
		}

		//初期未検出故障ファイル
		sprintf(txt_cmd, "%sinit_undet_fault.txt", folder_out);
		if ((fout_init_undet = (fopen(txt_cmd, "w"))) == NULL) {
			printf("【ERROR】:File open init_undet_fault.txt(file_output.c)\n");
			return ERROR;
		}

		fprintf(fout_literal,"-------------------------------------------\n");
		fprintf(fout_literal,"All Fault NUM         :%d\n", all_num);
		fprintf(fout_literal,"UNDET Fault NUM       :%d\n",ufl_num);
		fprintf(fout_literal,"Untest Fault NUM      :%d\n",red_num);
		fprintf(fout_literal,"High Power Fault NUM  :%d\n", hpfl_num);
		fprintf(fout_literal,"-------------------------------------------\n\n");
		for (int i = 0; i < n_net; i++) {
			fprintf(fout_literal,"Name            :%s\n", nl[i].name);
			fprintf(fout_literal,"  ID            :%d\n", nl[i].n);
		}

		node = red;
		while (node != NULL) {
		j++;
			//fprintf(fout_untest_fault, "---------------------------------\n");
			//fprintf(fout_untest_fault, "Number:%d\n", j);
			fprintf(fout_untest_fault, "%s\n", node->name);
			if (node->type == STR) {
				fprintf(fout_untest_fault, "STR\n");
			}
			else {
				fprintf(fout_untest_fault, "STF\n");
			}
			//fprintf(fout_untest_fault, "---------------------------------\n\n");
			node = node->next;
		}

		j = 0;
		node = hpfl;
		while (node != NULL) {
			j++;
			//fprintf(fout_high_fault, "---------------------------------\n");
			//fprintf(fout_high_fault, "Number:%d\n", j);
			fprintf(fout_high_fault, "%s\n", node->name);
			if (node->type == STR) {
				fprintf(fout_high_fault, "STR\n");
			}
			else {
				fprintf(fout_high_fault, "STF\n");
			}
			//fprintf(fout_high_fault, "---------------------------------\n\n");
			node = node->next;
		}

		j = 0;
		node = ufl;
		while (node != NULL) {
			j++;
			//fprintf(fout_init_undet, "---------------------------------\n");
			//fprintf(fout_init_undet, "Number:%d\n", j);
			fprintf(fout_init_undet, "%s\n", node->name);
			if (node->type == STR) {
				fprintf(fout_init_undet, "STR\n");
			}
			else {
				fprintf(fout_init_undet, "STF\n");
			}
			//fprintf(fout_init_undet, "---------------------------------\n\n");
			node = node->next;
		}

		fclose(fout_literal);
		fclose(fout_untest_fault);
		fclose(fout_init_undet);

		if ((strcmp(fi[LOW_POWER].name, "ON")) == 0) {
			fclose(fout_high_fault);
		}

	}

	//全故障ファイル記述
	else if (mode == 1) {

		sprintf(txt_cmd, "%sall_fault.txt", folder_out);
		if ((fout_fault = (fopen(txt_cmd, "w"))) == NULL) {
			printf("【ERROR】:File open all_fault.txt(file_output.c)\n");
			return ERROR;
		}

		j = 0;
		node = afl;
		while (node != NULL) {
			j++;
			//fprintf(fout_fault, "---------------------------------\n");
			//fprintf(fout_fault, "Number:%d\n", j);
			fprintf(fout_fault, "%s\n", node->name);
			if (node->type == STR) {
				fprintf(fout_fault, "STR\n");
			}
			else {
				fprintf(fout_fault, "STF\n");
			}
			//fprintf(fout_fault, "---------------------------------\n\n");
			node = node->next;
		}

		fclose(fout_fault);
	}

	//LOGファイル記述
	else if (mode == 2) {

		//LOGファイル
		sprintf(txt_cmd, "%slog.txt", folder_out);
		if ((fout_log = fopen(txt_cmd, "w")) == NULL) {
			printf("【ERROR】:File open log.txt(file_output.c)\n");
			return ERROR;
		}

		fprintf(fout_log, "//---------------------------------------------------\n");
		fprintf(fout_log, "//	    Transition Fault ASG   Information\n");
		fprintf(fout_log, "//---------------------------------------------------\n");
		fprintf(fout_log, "// Target Circuit              :%s\n", fi[CIRCUIT].name);
		fprintf(fout_log, "// Fault Type                  :NODE\n");
		if (fi[TARGET_LIMIT].value == 1) {
			fprintf(fout_log, "// Test Generation             :STTG\n");
		}
		else {
			fprintf(fout_log, "// Test Generation             :MTTG\n");
		}
		fprintf(fout_log, "// Number of Total Faults      :%d\n", (int)all_num);
		fprintf(fout_log, "// Number of Untestable Faults :%d\n", (int)red_num);
		fprintf(fout_log, "// Number of High Power Faults :%d\n", (int)hpfl_num);
		fprintf(fout_log, "// Number of Detected Faults   :%d\n", (int)dfl_num);
		fprintf(fout_log, "// Number of Undetected Faults :%d\n", (int)ufl_num);
		fprintf(fout_log, "// Number of Seeds             :%d\n", tp_num);
		fprintf(fout_log, "// Rate of Detected Faults     :%.2lf%%\n", (((double)dfl_num) / ((double)all_num))*100.0);
		fprintf(fout_log, "// Elapsed time                :%.2lfsec\n", (double)clock() / CLOCKS_PER_SEC);
		fprintf(fout_log, "// Time to abort solver        :%dsec\n", fi[TIME].value);
		fprintf(fout_log, "// Target faults limit         :%d\n", fi[TARGET_LIMIT].value);
		fprintf(fout_log, "// Rand Optiont                :%s\n", fi[RAND].name);
		fprintf(fout_log, "// Low Power Mode              :%s\n", fi[LOW_POWER].name);
		fprintf(fout_log, "// WSA                         :%d[％]\n", fi[WSATH].value);
		fprintf(fout_log, "//---------------------------------------------------\n");

		fclose(fout_log);

		//最終未検出故障ファイル(空になるはず)
		sprintf(txt_cmd, "%sundet.txt", folder_out);
		if ((fout_undet = fopen(txt_cmd, "w")) == NULL) {
			printf("【ERROR】:File open undet.txt(file_output.c)\n");
			return ERROR;
		}

		j = 0;
		node = ufl;
		while (node != NULL) {
			j++;
			//fprintf(fout_undet, "---------------------------------\n");
			//fprintf(fout_undet, "Number:%d\n", j);
			fprintf(fout_undet, "%s\n", node->name);
			if (node->type == STR) {
				fprintf(fout_undet, "STR\n");
			}
			else {
				fprintf(fout_undet, "STF\n");
			}
			//fprintf(fout_undet, "---------------------------------\n\n");
			node = node->next;
		}

		fclose(fout_undet);

		//検出故障ファイルオープン
		sprintf(txt_cmd, "%sdet.txt", folder_out);
		if ((fout_det = fopen(txt_cmd, "w")) == NULL) {
			printf("【ERROR】:File open det.txt(file_output.c)\n");
			return ERROR;
		}

		j = 0;
		node = dfl;
		while (node != NULL) {
			j++;
			//fprintf(fout_det, "---------------------------------\n");
			//fprintf(fout_det, "Number:%d\n", j);
			fprintf(fout_det, "%s\n", node->name);
			if (node->type == STR) {
				fprintf(fout_det, "STR\n");
			}
			else {
				fprintf(fout_det, "STF\n");
			}
			//fprintf(fout_det, "---------------------------------\n\n");
			node = node->next;
		}

		fclose(fout_det);
	}

	//目標故障選択ファイル
	else if (mode == 3) {
		sprintf(txt_cmd, "%stfl_log.txt", folder_out);
		if ((fout_tfl_log = fopen(txt_cmd, "a+")) == NULL) {
			printf("【ERROR】:File open tfl_log.txt(init_exturn.c)\n");
			return ERROR;
		}

		head = tfl;
		while (head != NULL) {
			fprintf(fout_tfl_log, "%s\n", head->name);
			if (head->type == STR) {
				fprintf(fout_tfl_log, " STR\n");
			}
			else if (head->type == STF) {
				fprintf(fout_tfl_log, " STF\n");
			}
			head = head->next;
		}
		fprintf(fout_tfl_log, "========================================================================\n\n\n");
		
		fclose(fout_tfl_log);
	}

	return SUCCESS;
}

//------------------------------------------------------------------------
//インクルード
//------------------------------------------------------------------------
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

//------------------------------------------------------------------------
//自作インクルード
//------------------------------------------------------------------------
#include"../../header/Define.h"

//===================================================
//関数名:fault_judge
//機能:検出故障特定
//戻り値：検出された故障数
//引数：なし
//===================================================
int fault_judge(void) {
	UNDET_FAULT_LIST *d_head;
	UNDET_FAULT_LIST *u_head;
	UNDET_FAULT_LIST *df_node;
	UNDET_FAULT_LIST *pre_node;
	UNDET_FAULT_LIST *node;
	char str[256];
	char txt_cmd[300];
	char* p_str_type;
	int  p_type;
	char* p_str_net;
	int i = 1;
	int j, k;
	int val_1[1000];
	int val_2[1000];
	int count = 0;
	int det_fault_num_clasp = 0;//1テストパターンでの検出故障数

	//故障辞書ファイルopen
	sprintf(txt_cmd, "%sfsim_log.txt", folder_out);
	if ((fout_fsim_result = fopen(txt_cmd, "r")) == NULL) {
		printf("【ERROR】:File open fsim_log.txt(fault_judge.c)\n");
		return ERROR;
	}

	d_head = dfl;

	//----------------------------------------
	//tp0
	//STR G9
	//STF G6
	//STF G8
	//STF G16
	//STF G8_G16_B
	//----------------------------------------

	while (fgets(str, 256, fout_fsim_result) != NULL) {
		if (ufl_num == 0)break;
		if (i != 1 && strcmp(str,"\n")!=0) { 
			p_str_type= strtok(str, " \t\n"); //STR or STF
			if (strcmp(p_str_type, "STR") == 0) {
				p_type = STR;
			}
			else if (strcmp(p_str_type, "STF") == 0) {
				p_type = STF;
			}
			p_str_net = strtok(NULL, " \t\n"); //G9
			for (j = 0; j < n_net; j++) {
				if (strcmp(p_str_net, nl[j].name) == 0) {
					for (k = 0; k < nl[j].n_in; k++) {
						val_1[k] = nl[j].in[k]->val_1t;
						val_2[k] = nl[j].in[k]->val_2t;
					}
					break;
				}
			}

			u_head = ufl;

			//ノードの1番最初が検出した場合の処理
			if (p_type == u_head->type) {
				if (strcmp(u_head->name, p_str_net) == 0) {
						//検出故障ノード
						df_node = (UNDET_FAULT_LIST*)malloc(sizeof(UNDET_FAULT_LIST));

						//情報コピー
						strcpy(df_node->name, u_head->name);
						df_node->type=u_head->type;
						df_node->netlist = u_head->netlist;

						//目標故障設定
						if (u_head->netlist->tf_flag == FLAG_UP) {
							count++;//目標故障が検出された場合countする
						}

						//線形リスト生成
						df_node->next =d_head;
						d_head = df_node;

						//検出、未検出故障数更新
						dfl_num++;//検出故障数更新
						det_fault_num_clasp++;//1テストパターンでの検出故障数

						//追加
						bitintSetNbit_Zero(detflag, u_head->node_id);//node_idは故障に一意に割り当てられているもの

						//検出故障を未検出故障リストから削除
						ufl_free(u_head, NULL, -1);
						/*pre_node = u_head->next;
						free(u_head);
						ufl = pre_node;*/

						continue;
				}
			}

			pre_node = ufl;
			u_head = ufl->next;
			//それ以外の処理
			while (u_head != NULL) {
				if (p_type == u_head->type) {
					if (strcmp(u_head->name, p_str_net) == 0) {
							//検出故障ノード
							df_node = (UNDET_FAULT_LIST*)malloc(sizeof(UNDET_FAULT_LIST));

							//情報コピー
							strcpy(df_node->name, u_head->name);
							df_node->type = u_head->type;
							df_node->netlist = u_head->netlist;

							//目標故障
							if (u_head->netlist->tf_flag == FLAG_UP) {
								count++;
							}

							//線形リスト生成
							df_node->next = d_head;
							d_head = df_node;

							//検出、未検出故障数更新
							dfl_num++;
							det_fault_num_clasp++;

							//追加
							bitintSetNbit_Zero(detflag, u_head->node_id);//node_idは故障に一意に割り当てられているもの

							//検出故障を未検出故障リストから削除
							ufl_free(u_head, pre_node, -1);
							/*pre_node = u_head->next->next;
							free(u_head->next);
							u_head->next = pre_node;*/

							break;
					}
				}
				pre_node = pre_node->next;
				u_head = u_head->next;
			}
		}
		i++;
	}

	//dfl 検出故障リストの更新
	dfl = d_head;

	//目標故障削除
	node = tfl;
	while (node != NULL) {
		d_head = node->next;
		free(node);
		node = d_head;
	}

	// 故障検出したらテストパターン数をインクリメント
	if (det_fault_num_clasp > 0) {
		seed_num++;
	}

	//標準出力
	standard_output();

	//ファイル操作
	fclose(fout_fsim_result);

	sprintf(txt_cmd, "%smttg.txt", folder_out);
	fout_mttg = fopen(txt_cmd, "a+");
	fprintf(fout_mttg, "%d: tfl_num:%d\ttfl_det_fault:%d\tdet_fault:%d\n", seed_num, tfl_num, count, det_fault_num_clasp);
	fclose(fout_mttg);
	
	return det_fault_num_clasp;
}
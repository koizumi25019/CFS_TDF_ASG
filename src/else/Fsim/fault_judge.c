//------------------------------------------------------------------------
//  故障シミュレーション結果の読み取り
//------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fault_judge.h"
#include "../../header/Define.h"
#include "../fault_list.h"
#include "../OUTPUT/report.h"

//---------------------------------------------------------------------
//  "STR" / "STF" を故障タイプにする
//---------------------------------------------------------------------
static int parse_fault_type(const char* token, int* type) {
	if (token == NULL) return ERROR;
	if (strcmp(token, "STR") == 0) { *type = STR; return SUCCESS; }
	if (strcmp(token, "STF") == 0) { *type = STF; return SUCCESS; }
	return ERROR;
}

//---------------------------------------------------------------------
//  未検出故障リストから (故障タイプ, 信号線名) の故障を探す
//---------------------------------------------------------------------
static UNDET_FAULT_LIST* find_undetected(int type, const char* name) {
	for (UNDET_FAULT_LIST* fault = ufl; fault != NULL; fault = fault->next) {
		if (fault->type == type && strcmp(fault->name, name) == 0) return fault;
	}
	return NULL;
}

//---------------------------------------------------------------------
//  fault_judge
//---------------------------------------------------------------------
int fault_judge(DetectClass detect_class, JudgeResult* result) {
	char path[1024];
	char line[512];
	int line_number = 0;
	FILE* log = NULL;

	result->detected_count = 0;
	result->detected_target_count = 0;

	if (output_path(path, sizeof(path), "fsim_log.txt") != SUCCESS) return ERROR;
	if (fopen_s(&log, path, "r") != 0 || log == NULL) {
		printf("[ERROR] Cannot open the fault-simulation result %s (XID2.exe did not write it).\n", path);
		return ERROR;
	}

	while (ufl_num > 0 && fgets(line, sizeof(line), log) != NULL) {
		char* context = NULL;
		char* type_token;
		char* fault_name;
		int type;
		UNDET_FAULT_LIST* fault;
		UNDET_FAULT_LIST* node;

		line_number++;
		if (line_number == 1) continue;		//1行目はテストパターン名

		type_token = strtok_s(line, " \t\r\n", &context);
		fault_name = strtok_s(NULL, " \t\r\n", &context);
		if (fault_name == NULL || parse_fault_type(type_token, &type) != SUCCESS) continue;

		fault = find_undetected(type, fault_name);
		if (fault == NULL) continue;		//目標外で、すでに検出済みか対象外の故障

		for (node = tfl; node != NULL; node = node->next) {
			if (fault_same(node, fault)) {
				result->detected_target_count++;
				break;
			}
		}

		if (fault_list_push_copy(&dfl, &dfl_num, fault) != SUCCESS) { fclose(log); return ERROR; }
		result->detected_count++;

		//一次故障(tfl の先頭)の分類
		if (detect_class != DETECT_NORMAL && tfl != NULL && fault_same(tfl, fault)) {
			int ok = detect_class == DETECT_RELAXED_WSA
				? fault_list_push_copy(&hpfl, &hpfl_num, fault)
				: fault_list_push_copy(&abortfl, &abortfl_num, fault);
			if (ok != SUCCESS) { fclose(log); return ERROR; }
		}

		if (fault_remove_undetected(fault) != SUCCESS) {
			printf("[ERROR] Internal error: the detected fault %s is not in the undetected list.\n", fault_name);
			fclose(log);
			return ERROR;
		}
	}

	fclose(log);
	return SUCCESS;
}

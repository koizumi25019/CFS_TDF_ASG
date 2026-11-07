//------------------------------------------------------------------------
//  目標故障の選択
//------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../../header/Define.h"
#include "../../asg/read.h"
#include "../../asg/compat.h"
#include "../../else/fault_list.h"

static bool compat_graph_ready = false;	//両立可能故障グラフを作成済みか

//---------------------------------------------------------------------
//  単一目標: 指定した故障だけを目標にする
//---------------------------------------------------------------------
int fault_select_single(const UNDET_FAULT_LIST* fault) {
	UNDET_FAULT_LIST* selected;

	if (fault == NULL) {
		printf("[ERROR] No undetected fault is available for single-target test generation.\n");
		return ERROR;
	}
	selected = fault_copy(fault);
	if (selected == NULL) {
		printf("[ERROR] Out of memory (target fault).\n");
		return ERROR;
	}
	tfl = selected;
	tfl_num = 1;
	printf("[INFO] Single-fault test generation: %s\n", selected->name);
	return SUCCESS;
}

//---------------------------------------------------------------------
//  複数目標(必須割当て -nece が無いとき): 未検出故障からランダムに選ぶ
//---------------------------------------------------------------------
static void seed_random_once(void) {
	static int seeded = 0;

	if (!seeded) {
		srand((unsigned int)time(NULL));
		seeded = 1;
	}
}

static int select_random_faults(int limit) {
	UNDET_FAULT_LIST** candidates;
	UNDET_FAULT_LIST* current = ufl;
	UNDET_FAULT_LIST* tail = NULL;
	int selected_count;

	candidates = (UNDET_FAULT_LIST**)malloc(sizeof(*candidates) * ufl_num);
	if (candidates == NULL) {
		printf("[ERROR] Out of memory (candidates).\n");
		return ERROR;
	}
	for (int i = 0; i < ufl_num; i++) {
		if (current == NULL) {
			printf("[ERROR] The undetected fault count does not match the list.\n");
			free(candidates);
			return ERROR;
		}
		candidates[i] = current;
		current = current->next;
	}

	seed_random_once();
	for (int i = ufl_num - 1; i > 0; i--) {	//Fisher-Yates シャッフル
		int random_index = rand() % (i + 1);
		UNDET_FAULT_LIST* temporary = candidates[i];
		candidates[i] = candidates[random_index];
		candidates[random_index] = temporary;
	}

	selected_count = (limit <= 0 || limit > ufl_num) ? ufl_num : limit;
	tfl = NULL;
	tfl_num = 0;
	for (int i = 0; i < selected_count; i++) {
		UNDET_FAULT_LIST* copy = fault_copy(candidates[i]);
		if (copy == NULL) {
			printf("[ERROR] Out of memory (target faults).\n");
			fault_list_free(tfl);
			tfl = NULL;
			tfl_num = 0;
			free(candidates);
			return ERROR;
		}
		if (tail == NULL) tfl = copy; else tail->next = copy;
		tail = copy;
		tfl_num++;
	}
	free(candidates);
	return SUCCESS;
}

//---------------------------------------------------------------------
//  複数目標
//---------------------------------------------------------------------
int fault_select_targets(void) {
	int result;

	if (ufl == NULL || ufl_num <= 0) {
		printf("[ERROR] No undetected fault is available.\n");
		return ERROR;
	}

	if (opt.necessary_file != NULL) {
		//両立可能故障集合から選ぶ(故障数に関係なく常にこの方法)
		if (!compat_graph_ready) {
			if (compat_prepare() != TARGET_OKAY) return ERROR;
			compat_graph_ready = true;
		}
		result = compat_select() == TARGET_OKAY ? SUCCESS : ERROR;
	}
	else {
		result = select_random_faults(opt.target_limit);
	}

	if (result == SUCCESS) {
		printf("[INFO] Selected %d target faults.\n", tfl_num);
	}
	return result;
}

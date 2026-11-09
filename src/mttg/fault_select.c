//------------------------------------------------------------------------
//  目標故障の選択
//------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../header/Define.h"
#include "../asg/compat.h"
#include "../else/fault_list.h"

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
//  複数目標: 互いに両立可能な故障の集合(必須割当てから作る)
//---------------------------------------------------------------------
int fault_select_targets(void) {
	if (ufl == NULL || ufl_num <= 0) {
		printf("[ERROR] No undetected fault is available.\n");
		return ERROR;
	}
	if (opt.target_limit == 1) {
		return fault_select_single(ufl);
	}
	if (!compat_graph_ready) {
		if (compat_prepare() != TARGET_OKAY) return ERROR;
		compat_graph_ready = true;
	}
	if (compat_select() != TARGET_OKAY) return ERROR;
	printf("[INFO] Selected %d target faults.\n", tfl_num);
	return SUCCESS;
}

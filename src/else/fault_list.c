//------------------------------------------------------------------------
//  故障リスト(UNDET_FAULT_LIST)の操作
//------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>

#include "fault_list.h"
#include "../header/Define.h"

int fault_same(const UNDET_FAULT_LIST* a, const UNDET_FAULT_LIST* b) {
	return a->node_id == b->node_id;
}

UNDET_FAULT_LIST* fault_copy(const UNDET_FAULT_LIST* source) {
	UNDET_FAULT_LIST* copy;

	if (source == NULL) return NULL;
	copy = (UNDET_FAULT_LIST*)malloc(sizeof(*copy));
	if (copy == NULL) return NULL;
	*copy = *source;
	copy->next = NULL;
	return copy;
}

int fault_list_push_copy(UNDET_FAULT_LIST** list, int* count, const UNDET_FAULT_LIST* source) {
	UNDET_FAULT_LIST* copy = fault_copy(source);

	if (copy == NULL) {
		printf("[ERROR] Out of memory (fault list).\n");
		return ERROR;
	}
	copy->next = *list;
	*list = copy;
	(*count)++;
	return SUCCESS;
}

void fault_list_free(UNDET_FAULT_LIST* head) {
	while (head != NULL) {
		UNDET_FAULT_LIST* next = head->next;
		free(head);
		head = next;
	}
}

int fault_remove_undetected(const UNDET_FAULT_LIST* target) {
	UNDET_FAULT_LIST** link = &ufl;

	while (*link != NULL && !fault_same(*link, target)) {
		link = &(*link)->next;
	}
	if (*link == NULL) {
		return ERROR;
	}
	UNDET_FAULT_LIST* removed = *link;
	*link = removed->next;
	bitintSetNbit_Zero(detflag, removed->node_id);
	free(removed);
	ufl_num--;
	return SUCCESS;
}

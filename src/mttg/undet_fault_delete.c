#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../header/Define.h"

static int same_fault(const UNDET_FAULT_LIST* left, const UNDET_FAULT_LIST* right)
{
	return left->type == right->type && strcmp(left->name, right->name) == 0;
}

static UNDET_FAULT_LIST* clone_fault(const UNDET_FAULT_LIST* source)
{
	UNDET_FAULT_LIST* copy = (UNDET_FAULT_LIST*)malloc(sizeof(*copy));

	if (copy == NULL) {
		printf("[ERROR] Failed to allocate a fault-list node.\n");
		exit(EXIT_FAILURE);
	}

	*copy = *source;
	copy->next = NULL;
	return copy;
}

static UNDET_FAULT_LIST** find_fault_link(const UNDET_FAULT_LIST* target)
{
	UNDET_FAULT_LIST** link = &ufl;

	while (*link != NULL) {
		if (*link == target || same_fault(*link, target)) {
			return link;
		}
		link = &(*link)->next;
	}

	return NULL;
}

int undet_fault_delete(UNDET_FAULT_LIST* target)
{
	UNDET_FAULT_LIST** link;
	UNDET_FAULT_LIST* removed;

	if (target == NULL) {
		return ERROR;
	}

	link = find_fault_link(target);
	if (link == NULL) {
		return ERROR;
	}

	removed = *link;
	*link = removed->next;
	if (detflag != NULL) {
		bitintSetNbit_Zero(detflag, removed->node_id);
	}
	free(removed);
	ufl_num--;
	return SUCCESS;
}

void ufl_free(UNDET_FAULT_LIST* target, UNDET_FAULT_LIST* previous, int mode)
{
	UNDET_FAULT_LIST** link;
	UNDET_FAULT_LIST* classified;
	UNDET_FAULT_LIST* removed;

	(void)previous;

	if (target == NULL) {
		return;
	}

	link = find_fault_link(target);
	if (link == NULL) {
		return;
	}
	removed = *link;

	if (mode == 0 || mode == 1) {
		classified = clone_fault(removed);
		if (mode == 0) {
			classified->next = unseed;
			unseed = classified;
			unseed_num++;
		}
		else {
			classified->next = hpfl;
			hpfl = classified;
			hpfl_num++;
		}
	}

	*link = removed->next;
	if (detflag != NULL) {
		bitintSetNbit_Zero(detflag, removed->node_id);
	}
	free(removed);
	ufl_num--;
}

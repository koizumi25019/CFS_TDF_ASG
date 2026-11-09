//------------------------------------------------------------------------
//  検出できない故障の事前除外
//------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "screen.h"
#include "essential.h"
#include "../else/fault_list.h"

int screen_untestable_faults(void)
{
	EssentialOptions options = essential_default_options();
	UNDET_FAULT_LIST** link = &ufl;
	int conflict_count = 0, unobservable_count = 0;

	while (*link != NULL) {
		UNDET_FAULT_LIST* fault = *link;
		EssentialResult result;

		if (!essential_compute(fault->netlist, fault->type, &options, &result)) {
			printf("[ERROR] Computing the necessary assignments failed for %s.\n", fault->name);
			return ERROR;
		}
		if (result.status == ESSENTIAL_OK) {
			link = &fault->next;
			continue;
		}
		if (result.status == ESSENTIAL_CONFLICT) conflict_count++; else unobservable_count++;
		if (fault_list_push_copy(&untestfl, &untest_num, fault) != SUCCESS) return ERROR;
		*link = fault->next;
		bitintSetNbit_Zero(detflag, fault->node_id);
		free(fault);
		ufl_num--;
	}
	printf("[INFO] Untestable faults found by the necessary assignments: %d (conflicting assignments %d, effect cannot reach a pseudo primary output %d).\n",
		untest_num, conflict_count, unobservable_count);
	return SUCCESS;
}

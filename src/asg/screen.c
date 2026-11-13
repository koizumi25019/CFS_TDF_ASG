//------------------------------------------------------------------------
//  検出できない故障の事前除外
//------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "screen.h"
#include "essential.h"
#include "../else/fault_list.h"

/* 故障番号(node_id)ごとの、必須割当ての記録 */
static unsigned int** cache_packed;
static int* cache_count;
static int cache_size;

bool screen_cached_assignments(const UNDET_FAULT_LIST* fault, int* count, const unsigned int** packed)
{
	if (cache_packed == NULL || fault->node_id < 0 || fault->node_id >= cache_size || cache_packed[fault->node_id] == NULL) return false;
	*count = cache_count[fault->node_id];
	*packed = cache_packed[fault->node_id];
	return true;
}

void screen_release_cache(void)
{
	for (int i = 0; i < cache_size; i++) free(cache_packed[i]);
	free(cache_packed);
	free(cache_count);
	cache_packed = NULL;
	cache_count = NULL;
	cache_size = 0;
}

int screen_untestable_faults(void)
{
	EssentialOptions options = essential_default_options();
	UNDET_FAULT_LIST** link = &ufl;
	int conflict_count = 0, unobservable_count = 0;

	screen_release_cache();
	cache_size = all_num;
	cache_packed = (unsigned int**)calloc(cache_size > 0 ? cache_size : 1, sizeof(*cache_packed));
	cache_count = (int*)calloc(cache_size > 0 ? cache_size : 1, sizeof(*cache_count));
	if (cache_packed == NULL || cache_count == NULL) {
		printf("[ERROR] Out of memory (necessary assignments).\n");
		return ERROR;
	}

	while (*link != NULL) {
		UNDET_FAULT_LIST* fault = *link;
		EssentialResult result;

		if (!essential_compute(fault->netlist, fault->type, &options, &result)) {
			printf("[ERROR] Computing the necessary assignments failed for %s.\n", fault->name);
			return ERROR;
		}
		if (result.status == ESSENTIAL_OK) {
			unsigned int* packed = (unsigned int*)malloc(sizeof(unsigned int) * (result.count > 0 ? result.count : 1));
			if (packed == NULL) {
				printf("[ERROR] Out of memory (necessary assignments).\n");
				return ERROR;
			}
			for (int k = 0; k < result.count; k++) {
				packed[k] = (((unsigned int)result.frame[k] * (unsigned int)n_net + (unsigned int)result.net[k]) << 1) | result.value[k];
			}
			cache_packed[fault->node_id] = packed;
			cache_count[fault->node_id] = result.count;
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

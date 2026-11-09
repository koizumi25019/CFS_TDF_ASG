//-------------------------------------------------------------------------------------------------------------
//	両立可能故障集合による目標故障の選択
//	  1. compat_prepare(): 故障ごとの必須割当て(ビット表現)と，両立可能な故障どうしを結ぶ辺(両立可能故障グラフ)を作る。最初に1回。
//	  2. compat_select()  : 未検出故障から，互いに両立可能な故障の集合を選んで tfl にする。シードごとに呼ぶ。
//	「両立可能」とは，2つの故障の必須割当てに衝突(同じ信号線に 0 と 1)が無いこと。
//-------------------------------------------------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "compat.h"
#include "read.h"
#include "../header/Define.h"
#include "../else/fault_list.h"

//-------------------------------------------------------------------------------------------------------------
//	信号線名から信号線番号を探す(ソート済み配列を二分探索)
//-------------------------------------------------------------------------------------------------------------
static int compare_net_name(const void* left, const void* right)
{
	return strcmp((*(NLIST* const*)left)->name, (*(NLIST* const*)right)->name);
}

//-------------------------------------------------------------------------------------------------------------
//	故障の必須割当て("G11=1 G17=0 ...")をビット表現にする
//-------------------------------------------------------------------------------------------------------------
static bool compat_set_necessary(UNDET_FAULT_LIST* fault, NLIST** sorted_nets, char* buffer)
{
	char* context = NULL;
	char* name;

	if (fault->node_id < 0 || fault->node_id >= readdata.necenet.num || readdata.necenet.assignments[fault->node_id] == NULL) {
		printf("[ERROR] No necessary assignments for the fault %s %s.\n", fault->name, fault_type_name(fault->type));
		return TARGET_ERROR;
	}
	if (strcpy_s(buffer, MAXSIZE_BUFFER, readdata.necenet.assignments[fault->node_id]) != 0) {
		printf("[ERROR] Too long necessary assignments for the fault %s.\n", fault->name);
		return TARGET_ERROR;
	}

	name = strtok_s(buffer, "=, \t\r\n", &context);
	while (name != NULL) {
		char* value = strtok_s(NULL, "=, \t\r\n", &context);
		NLIST key = { 0 };
		NLIST* key_pointer = &key;
		NLIST** found;

		if (value == NULL || (strcmp(value, "0") != 0 && strcmp(value, "1") != 0)) {
			printf("[ERROR] Malformed necessary assignments for the fault %s (signal %s).\n", fault->name, name);
			return TARGET_ERROR;
		}
		key.name = name;
		found = (NLIST**)bsearch(&key_pointer, sorted_nets, n_net, sizeof(NLIST*), compare_net_name);
		if (found == NULL) {
			printf("[ERROR] The necessary assignments of the fault %s name an unknown signal: %s\n", fault->name, name);
			return TARGET_ERROR;
		}
		if (*value == '0') {
			Set_NINT_Zero_XP(fault->necenet, (unsigned int)((*found) - nl));
		}
		else {
			Set_NINT_One_XP(fault->necenet, (unsigned int)((*found) - nl));
		}
		name = strtok_s(NULL, "=, \t\r\n", &context);
	}
	return TARGET_OKAY;
}

//-------------------------------------------------------------------------------------------------------------
//	compat_prepare
//-------------------------------------------------------------------------------------------------------------
bool compat_prepare(void)
{
	UNDET_FAULT_LIST** nodes;
	NLIST** sorted_nets;
	char* buffer;
	clock_t start = clock();
	int last_percent = -1;
	int word_count = n_net / MAXSIZE_BITINT + 1;

	if (readdata.necenet.assignments == NULL) {
		printf("[ERROR] compat_prepare: no necessary assignments (-nece) were read.\n");
		return TARGET_ERROR;
	}

	/* 故障ごとの領域: 必須割当て(0/1/未定) と 辺(両立可能な故障のビット列) */
	for (UNDET_FAULT_LIST* fault = ufl; fault != NULL; fault = fault->next)
	{
		fault->necenet = (BIT_INT_XP*)allocMemory(1, sizeof(BIT_INT_XP));
		fault->necenet->int_num = word_count;
		fault->necenet->x_buf = (unsigned int*)allocMemory(word_count, sizeof(unsigned int));
		fault->necenet->p_buf = (unsigned int*)allocMemory(word_count, sizeof(unsigned int));
		All_Bit_X_XP(fault->necenet);

		fault->edge = (BIT_INT*)allocMemory(1, sizeof(BIT_INT));
		fault->edge->int_num = ufl_num / MAXSIZE_BITINT + 1;
		fault->edge->flag = (unsigned int*)allocMemory(fault->edge->int_num, sizeof(unsigned int));
		bitintSetAll_Zero(fault->edge);
		for (int j = 0; j < ufl_num; j++)
			bitintSetNbit_One(fault->edge, j);	/* 最初は全ての故障と両立可能として，衝突のあるものを外す */
		fault->n_edge = ufl_num;
	}

	/* 必須割当てをビットにする */
	sorted_nets = (NLIST**)allocMemory(n_net, sizeof(NLIST*));
	for (int i = 0; i < n_net; i++) sorted_nets[i] = &nl[i];
	qsort(sorted_nets, n_net, sizeof(NLIST*), compare_net_name);
	buffer = (char*)allocMemory(MAXSIZE_BUFFER, sizeof(char));
	for (UNDET_FAULT_LIST* fault = ufl; fault != NULL; fault = fault->next)
	{
		if (compat_set_necessary(fault, sorted_nets, buffer) != TARGET_OKAY) {
			free(buffer);
			free(sorted_nets);
			return TARGET_ERROR;
		}
	}
	free(buffer);
	free(sorted_nets);

	/* 辺: 全ての故障の組について、必須割当てが衝突するものを外す */
	nodes = (UNDET_FAULT_LIST**)allocMemory(ufl_num, sizeof(*nodes));
	{
		UNDET_FAULT_LIST* fault = ufl;
		for (int i = 0; i < ufl_num; i++, fault = fault->next) nodes[i] = fault;
	}
	for (int i = 0; i < ufl_num; i++)
	{
		UNDET_FAULT_LIST* a = nodes[i];
		int percent;

		for (int j = i + 1; j < ufl_num; j++)
		{
			UNDET_FAULT_LIST* b = nodes[j];
			for (int k = 0; k < word_count; k++)
			{
				/* x_buf=1 は「値が1または未定」、p_buf=1 は「値が0または未定」。両方の積に0のビットがあれば衝突 */
				unsigned x = a->necenet->x_buf[k] & b->necenet->x_buf[k];
				unsigned p = a->necenet->p_buf[k] & b->necenet->p_buf[k];
				if ((x | p) != 0xFFFFFFFFu)
				{
					bitintSetNbit_Zero(a->edge, b->node_id);
					a->n_edge--;
					bitintSetNbit_Zero(b->edge, a->node_id);
					b->n_edge--;
					break;
				}
			}
		}

		percent = (int)(100.0 * (i + 1) / ufl_num);
		if (percent != last_percent) {	/* 進捗は1%刻みで表示する */
			printf("\rGenerating graph progress  >> %d%%", percent);
			last_percent = percent;
		}
	}
	free(nodes);

	time_graph = clock() - start;
	printf("\n	Making Graph CPU time：	%.3f[s]\n", (float)time_graph / CLOCKS_PER_SEC);
	return TARGET_OKAY;
}

//-------------------------------------------------------------------------------------------------------------
//	compat_select
//-------------------------------------------------------------------------------------------------------------
/* 辺の数(両立可能な故障の数)が多い順。同じなら故障番号の小さい順 */
static int compare_fault_degree(const void* left, const void* right)
{
	const UNDET_FAULT_LIST* a = *(const UNDET_FAULT_LIST* const*)left;
	const UNDET_FAULT_LIST* b = *(const UNDET_FAULT_LIST* const*)right;

	if (a->n_edge != b->n_edge) {
		return a->n_edge > b->n_edge ? -1 : 1;
	}
	return (a->node_id > b->node_id) - (a->node_id < b->node_id);
}

bool compat_select(void)
{
	UNDET_FAULT_LIST** candidates;
	UNDET_FAULT_LIST* tail = NULL;
	BIT_INT remaining;
	int candidate_count = 0;
	int limit = opt.target_limit;

	if (ufl == NULL || ufl_num <= 0) {
		return TARGET_ERROR;
	}
	if (limit <= 0 || limit > ufl_num) {
		limit = ufl_num;
	}

	/* 未検出故障を，辺の多い順に並べる */
	candidates = (UNDET_FAULT_LIST**)allocMemory(ufl_num, sizeof(*candidates));
	for (UNDET_FAULT_LIST* fault = ufl; fault != NULL; fault = fault->next) {
		if (candidate_count >= ufl_num) {
			free(candidates);
			return TARGET_ERROR;
		}
		candidates[candidate_count++] = fault;
	}
	if (candidate_count != ufl_num) {
		free(candidates);
		return TARGET_ERROR;
	}
	qsort(candidates, candidate_count, sizeof(*candidates), compare_fault_degree);

	/* 先頭から順に選び、選んだ故障と両立しない故障を候補(remaining)から外していく。
	   選んだ集合の全ての故障が互いに両立可能になる。先頭の故障が一次故障。 */
	remaining.int_num = detflag->int_num;
	remaining.flag = (unsigned int*)allocMemory(remaining.int_num, sizeof(*remaining.flag));
	memcpy(remaining.flag, detflag->flag, remaining.int_num * sizeof(*remaining.flag));
	tfl = NULL;
	tfl_num = 0;
	for (int index = 0; index < candidate_count && tfl_num < limit; index++) {
		UNDET_FAULT_LIST* candidate = candidates[index];
		UNDET_FAULT_LIST* copy;

		if (!bitintGetNbit(&remaining, candidate->node_id)) {
			continue;
		}
		copy = fault_copy(candidate);
		if (copy == NULL) {
			printf("[ERROR] Out of memory (target faults).\n");
			fault_list_free(tfl);
			tfl = NULL;
			tfl_num = 0;
			free(remaining.flag);
			free(candidates);
			return TARGET_ERROR;
		}
		if (tail == NULL) {
			tfl = copy;
		}
		else {
			tail->next = copy;
		}
		tail = copy;
		tfl_num++;
		for (unsigned int word = 0; word < remaining.int_num; word++) {
			remaining.flag[word] &= candidate->edge->flag[word];
		}
		bitintSetNbit_Zero(&remaining, candidate->node_id);
	}
	free(remaining.flag);
	free(candidates);
	return tfl_num > 0 ? TARGET_OKAY : TARGET_ERROR;
}

//-------------------------------------------------------------------------------------------------------------
//	両立可能故障集合による目標故障の選択
//	  1. compat_prepare(): 故障ごとの必須割当て(essential.h)から，両立可能な故障どうしを結ぶ辺(両立可能故障グラフ)を作る。最初に1回。
//	  2. compat_select()  : 未検出故障から，互いに両立可能な故障の集合を選んで tfl にする。シードごとに呼ぶ。
//	「両立可能」とは，2つの故障の必須割当てに衝突(同じ信号線・同じ時刻に 0 と 1)が無いこと。
//	必須割当ては回路と故障だけで決まるので，LFSR の有無に関係なく使える。
//-------------------------------------------------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "compat.h"
#include "essential.h"
#include "read.h"
#include "../header/Define.h"
#include "../else/fault_list.h"

//-------------------------------------------------------------------------------------------------------------
//	compat_prepare
//	  衝突は「同じ (時刻, 信号線) に 0 を要求する故障 と 1 を要求する故障」の組。
//	  故障の組を総当たりにせず，(時刻, 信号線) ごとに 0 側の故障と 1 側の故障を並べて，その組だけを調べる。
//-------------------------------------------------------------------------------------------------------------
bool compat_prepare(void)
{
	EssentialOptions options = essential_default_options();
	UNDET_FAULT_LIST** nodes;
	unsigned int key_count = 2u * (unsigned int)n_net;
	int* fault_first;			/* 故障 i の必須割当てが assigned[fault_first[i] ..] にある */
	unsigned int* assigned_key;	/* (時刻 * n_net + 信号線) */
	unsigned char* assigned_value;
	int* offset[2];				/* 値(0/1)ごとの、キーごとの故障リストの先頭(CSR) */
	int* member[2];
	int* fill;
	int total = 0, capacity = 0;
	clock_t start = clock();
	int last_percent = -1;

	nodes = (UNDET_FAULT_LIST**)allocMemory(ufl_num, sizeof(*nodes));
	{
		UNDET_FAULT_LIST* fault = ufl;
		for (int i = 0; i < ufl_num; i++, fault = fault->next) nodes[i] = fault;
	}

	/* 故障ごとの辺: 最初は(有効な)全ての故障と両立可能として，衝突のあるものを外す */
	for (int i = 0; i < ufl_num; i++)
	{
		UNDET_FAULT_LIST* fault = nodes[i];
		fault->edge = (BIT_INT*)allocMemory(1, sizeof(BIT_INT));
		fault->edge->int_num = detflag->int_num;
		fault->edge->flag = (unsigned int*)allocMemory(fault->edge->int_num, sizeof(unsigned int));
		memcpy(fault->edge->flag, detflag->flag, fault->edge->int_num * sizeof(unsigned int));
		bitintSetNbit_Zero(fault->edge, fault->node_id);
		fault->n_edge = ufl_num - 1;
	}

	/* 必須割当てを求めて、並べておく */
	fault_first = (int*)allocMemory(ufl_num + 1, sizeof(int));
	assigned_key = NULL;
	assigned_value = NULL;
	for (int i = 0; i < ufl_num; i++)
	{
		EssentialResult result;

		if (!essential_compute(nodes[i]->netlist, nodes[i]->type, &options, &result)) {
			printf("[ERROR] Computing the necessary assignments failed for %s.\n", nodes[i]->name);
			return TARGET_ERROR;
		}
		fault_first[i] = total;
		if (total + result.count > capacity) {
			capacity = (total + result.count) * 2;
			assigned_key = (unsigned int*)realloc(assigned_key, sizeof(unsigned int) * capacity);
			assigned_value = (unsigned char*)realloc(assigned_value, capacity);
			if (assigned_key == NULL || assigned_value == NULL) {
				printf("[ERROR] Out of memory (necessary assignments).\n");
				return TARGET_ERROR;
			}
		}
		for (int k = 0; k < result.count; k++) {
			assigned_key[total] = (unsigned int)result.frame[k] * (unsigned int)n_net + (unsigned int)result.net[k];
			assigned_value[total] = result.value[k];
			total++;
		}
	}
	fault_first[ufl_num] = total;

	/* (時刻, 信号線, 値) ごとの故障リストを作る */
	for (int v = 0; v < 2; v++) {
		offset[v] = (int*)allocMemory(key_count + 1, sizeof(int));
		member[v] = (int*)allocMemory(total + 1, sizeof(int));
	}
	for (int k = 0; k < total; k++) offset[assigned_value[k]][assigned_key[k] + 1]++;
	for (int v = 0; v < 2; v++)
		for (unsigned int key = 0; key < key_count; key++) offset[v][key + 1] += offset[v][key];
	fill = (int*)allocMemory(key_count, sizeof(int));
	for (int v = 0; v < 2; v++) {
		memset(fill, 0, sizeof(int) * key_count);
		for (int i = 0; i < ufl_num; i++)
			for (int k = fault_first[i]; k < fault_first[i + 1]; k++)
				if (assigned_value[k] == v) {
					unsigned int key = assigned_key[k];
					member[v][offset[v][key] + fill[key]++] = i;
				}
	}

	/* 衝突する組の辺を外す */
	for (unsigned int key = 0; key < key_count; key++)
	{
		int percent;

		for (int a = offset[0][key]; a < offset[0][key + 1]; a++)
		{
			UNDET_FAULT_LIST* zero_fault = nodes[member[0][a]];
			for (int b = offset[1][key]; b < offset[1][key + 1]; b++)
			{
				UNDET_FAULT_LIST* one_fault = nodes[member[1][b]];
				if (bitintGetNbit(zero_fault->edge, one_fault->node_id))
				{
					bitintSetNbit_Zero(zero_fault->edge, one_fault->node_id);
					zero_fault->n_edge--;
					bitintSetNbit_Zero(one_fault->edge, zero_fault->node_id);
					one_fault->n_edge--;
				}
			}
		}
		percent = (int)(100.0 * (key + 1) / key_count);
		if (percent != last_percent) {	/* 進捗は1%刻みで表示する */
			printf("\rGenerating graph progress  >> %d%%", percent);
			last_percent = percent;
		}
	}

	free(fill);
	for (int v = 0; v < 2; v++) { free(offset[v]); free(member[v]); }
	free(assigned_key);
	free(assigned_value);
	free(fault_first);
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

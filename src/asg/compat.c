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
#include "screen.h"
#include "read.h"
#include "../header/Define.h"
#include "../else/fault_list.h"

//-------------------------------------------------------------------------------------------------------------
//	辺(両立可能な故障の組)の数え直し
//-------------------------------------------------------------------------------------------------------------
static int popcount_and(const unsigned int* a, const unsigned int* b, unsigned int words)
{
	int count = 0;
	for (unsigned int w = 0; w < words; w++) {
		unsigned int x = a[w] & b[w];
		x = x - ((x >> 1) & 0x55555555u);
		x = (x & 0x33333333u) + ((x >> 2) & 0x33333333u);
		x = (x + (x >> 4)) & 0x0F0F0F0Fu;
		count += (int)((x * 0x01010101u) >> 24);
	}
	return count;
}

//-------------------------------------------------------------------------------------------------------------
//	compat_prepare
//	  衝突は「同じ (時刻, 信号線) に 0 を要求する故障 と 1 を要求する故障」の組。
//	  故障の組を総当たりにせず，(時刻, 信号線) ごとに 0 側の故障と 1 側の故障を並べて，その組だけを調べる。
//	  0 側・1 側がどちらも多いときは、組を1つずつ調べず、片側の故障をビット列にして、もう片側の辺から一度に外す。
//-------------------------------------------------------------------------------------------------------------
bool compat_prepare(void)
{
	EssentialOptions options = essential_default_options();
	UNDET_FAULT_LIST** nodes;
	unsigned int key_count = 2u * (unsigned int)n_net;
	unsigned int words = detflag->int_num;
	int* fault_first;			/* 故障 i の必須割当てが packed[fault_first[i] ..] にある */
	unsigned int* packed;		/* ((時刻 * n_net + 信号線) << 1) | 値 */
	int* offset[2];				/* 値(0/1)ごとの、キーごとの故障リストの先頭(CSR) */
	int* member[2];
	int* fill;
	unsigned int* zero_bits;
	unsigned int* one_bits;
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
		fault->edge->int_num = words;
		fault->edge->flag = (unsigned int*)allocMemory(words, sizeof(unsigned int));
		memcpy(fault->edge->flag, detflag->flag, words * sizeof(unsigned int));
		bitintSetNbit_Zero(fault->edge, fault->node_id);
	}

	/* 必須割当てを集める(screen_untestable_faults の記録があればそれを使い、無ければ求める) */
	fault_first = (int*)allocMemory(ufl_num + 1, sizeof(int));
	packed = NULL;
	for (int i = 0; i < ufl_num; i++)
	{
		int count = 0;
		const unsigned int* source = NULL;
		EssentialResult result;
		unsigned int* converted = NULL;

		if (!screen_cached_assignments(nodes[i], &count, &source)) {
			if (!essential_compute(nodes[i]->netlist, nodes[i]->type, &options, &result)) {
				printf("[ERROR] Computing the necessary assignments failed for %s.\n", nodes[i]->name);
				return TARGET_ERROR;
			}
			count = result.count;
			converted = (unsigned int*)allocMemory(count > 0 ? count : 1, sizeof(unsigned int));
			for (int k = 0; k < count; k++) {
				converted[k] = (((unsigned int)result.frame[k] * (unsigned int)n_net + (unsigned int)result.net[k]) << 1) | result.value[k];
			}
			source = converted;
		}
		fault_first[i] = total;
		if (total + count > capacity) {
			capacity = (total + count) * 2;
			packed = (unsigned int*)realloc(packed, sizeof(unsigned int) * capacity);
			if (packed == NULL) {
				printf("[ERROR] Out of memory (necessary assignments).\n");
				return TARGET_ERROR;
			}
		}
		memcpy(packed + total, source, sizeof(unsigned int) * count);
		total += count;
		free(converted);
	}
	fault_first[ufl_num] = total;

	/* (時刻, 信号線, 値) ごとの故障リストを作る */
	for (int v = 0; v < 2; v++) {
		offset[v] = (int*)allocMemory(key_count + 1, sizeof(int));
		member[v] = (int*)allocMemory(total + 1, sizeof(int));
	}
	for (int k = 0; k < total; k++) offset[packed[k] & 1u][(packed[k] >> 1) + 1]++;
	for (int v = 0; v < 2; v++)
		for (unsigned int key = 0; key < key_count; key++) offset[v][key + 1] += offset[v][key];
	fill = (int*)allocMemory(key_count, sizeof(int));
	for (int v = 0; v < 2; v++) {
		memset(fill, 0, sizeof(int) * key_count);
		for (int i = 0; i < ufl_num; i++)
			for (int k = fault_first[i]; k < fault_first[i + 1]; k++)
				if ((int)(packed[k] & 1u) == v) {
					unsigned int key = packed[k] >> 1;
					member[v][offset[v][key] + fill[key]++] = i;
				}
	}

	/* 衝突する組の辺を外す */
	zero_bits = (unsigned int*)allocMemory(words, sizeof(unsigned int));
	one_bits = (unsigned int*)allocMemory(words, sizeof(unsigned int));
	for (unsigned int key = 0; key < key_count; key++)
	{
		int percent;
		int zero_count = offset[0][key + 1] - offset[0][key];
		int one_count = offset[1][key + 1] - offset[1][key];

		if (zero_count > 0 && one_count > 0)
		{
			if ((double)zero_count * one_count > (double)(zero_count + one_count) * words / 4.0)
			{
				/* 多いとき: 片側をビット列にして、もう片側の辺から一度に外す */
				memset(zero_bits, 0, sizeof(unsigned int) * words);
				memset(one_bits, 0, sizeof(unsigned int) * words);
				for (int a = offset[0][key]; a < offset[0][key + 1]; a++) {
					int id = nodes[member[0][a]]->node_id;
					zero_bits[id / MAXSIZE_BITINT] |= 1u << (id % MAXSIZE_BITINT);
				}
				for (int b = offset[1][key]; b < offset[1][key + 1]; b++) {
					int id = nodes[member[1][b]]->node_id;
					one_bits[id / MAXSIZE_BITINT] |= 1u << (id % MAXSIZE_BITINT);
				}
				for (int a = offset[0][key]; a < offset[0][key + 1]; a++) {
					unsigned int* edge = nodes[member[0][a]]->edge->flag;
					for (unsigned int w = 0; w < words; w++) edge[w] &= ~one_bits[w];
				}
				for (int b = offset[1][key]; b < offset[1][key + 1]; b++) {
					unsigned int* edge = nodes[member[1][b]]->edge->flag;
					for (unsigned int w = 0; w < words; w++) edge[w] &= ~zero_bits[w];
				}
			}
			else
			{
				for (int a = offset[0][key]; a < offset[0][key + 1]; a++)
				{
					UNDET_FAULT_LIST* zero_fault = nodes[member[0][a]];
					for (int b = offset[1][key]; b < offset[1][key + 1]; b++)
					{
						UNDET_FAULT_LIST* one_fault = nodes[member[1][b]];
						bitintSetNbit_Zero(zero_fault->edge, one_fault->node_id);
						bitintSetNbit_Zero(one_fault->edge, zero_fault->node_id);
					}
				}
			}
		}
		percent = (int)(100.0 * (key + 1) / key_count);
		if (percent != last_percent) {	/* 進捗は1%刻みで表示する */
			printf("\rGenerating graph progress  >> %d%%", percent);
			last_percent = percent;
		}
	}

	/* 辺の数 */
	for (int i = 0; i < ufl_num; i++) nodes[i]->n_edge = popcount_and(nodes[i]->edge->flag, nodes[i]->edge->flag, words);

	free(zero_bits);
	free(one_bits);
	free(fill);
	for (int v = 0; v < 2; v++) { free(offset[v]); free(member[v]); }
	free(packed);
	free(fault_first);
	free(nodes);
	screen_release_cache();

	time_graph = clock() - start;
	printf("\n	Making Graph CPU time：	%.3f[s]\n", (float)time_graph / CLOCKS_PER_SEC);
	return TARGET_OKAY;
}

//-------------------------------------------------------------------------------------------------------------
//	compat_select
//	  1. 一次故障: 残っている故障のうち、両立可能な故障(辺)が最も少ないもの(他の故障と組みにくく、後に回すと
//	     単独になりやすい故障)。同じなら故障番号の小さいもの。
//	  2. 二次故障: これまでに選んだ故障の全てと両立可能な故障(候補)のうち、候補の中で両立可能な故障が最も多いものを
//	     順に選ぶ。選んだ故障と両立しない故障は候補から外れるので、選んだ集合の全ての故障が互いに両立可能になる。
//	  辺の数は、残っている(未検出の)故障だけで数える。
//-------------------------------------------------------------------------------------------------------------
bool compat_select(void)
{
	UNDET_FAULT_LIST** by_id;
	UNDET_FAULT_LIST* tail = NULL;
	UNDET_FAULT_LIST* next = NULL;
	BIT_INT remaining;
	unsigned int words;
	int limit = opt.target_limit;
	int lowest_degree = -1;

	if (ufl == NULL || ufl_num <= 0) {
		return TARGET_ERROR;
	}
	if (limit <= 0 || limit > ufl_num) {
		limit = ufl_num;
	}
	words = detflag->int_num;
	by_id = (UNDET_FAULT_LIST**)allocMemory(all_num, sizeof(*by_id));
	remaining.int_num = words;
	remaining.flag = (unsigned int*)allocMemory(words, sizeof(*remaining.flag));
	memcpy(remaining.flag, detflag->flag, words * sizeof(*remaining.flag));

	for (UNDET_FAULT_LIST* fault = ufl; fault != NULL; fault = fault->next) {
		int degree = popcount_and(fault->edge->flag, detflag->flag, words);
		by_id[fault->node_id] = fault;
		if (lowest_degree < 0 || degree < lowest_degree || (degree == lowest_degree && fault->node_id < next->node_id)) {
			lowest_degree = degree;
			next = fault;
		}
	}

	tfl = NULL;
	tfl_num = 0;
	while (next != NULL && tfl_num < limit) {
		UNDET_FAULT_LIST* selected = next;
		UNDET_FAULT_LIST* copy = fault_copy(selected);
		int best_degree = -1;

		if (copy == NULL) {
			printf("[ERROR] Out of memory (target faults).\n");
			fault_list_free(tfl);
			tfl = NULL;
			tfl_num = 0;
			free(remaining.flag);
			free(by_id);
			return TARGET_ERROR;
		}
		if (tail == NULL) tfl = copy; else tail->next = copy;
		tail = copy;
		tfl_num++;

		/* 選んだ故障と両立しない故障を候補から外す */
		for (unsigned int w = 0; w < words; w++) remaining.flag[w] &= selected->edge->flag[w];
		bitintSetNbit_Zero(&remaining, selected->node_id);

		/* 次は、候補の中で、候補どうしの辺が最も多い故障 */
		next = NULL;
		for (unsigned int w = 0; w < words; w++) {
			unsigned int bits = remaining.flag[w];
			while (bits != 0) {
				unsigned int low = bits & (~bits + 1u);
				int bit = 0;
				UNDET_FAULT_LIST* candidate;
				int degree;
				while (((low >> bit) & 1u) == 0) bit++;
				candidate = by_id[w * MAXSIZE_BITINT + bit];
				degree = popcount_and(candidate->edge->flag, remaining.flag, words);
				if (degree > best_degree) {
					best_degree = degree;
					next = candidate;
				}
				bits &= bits - 1u;
			}
		}
	}
	free(remaining.flag);
	free(by_id);
	return tfl_num > 0 ? TARGET_OKAY : TARGET_ERROR;
}

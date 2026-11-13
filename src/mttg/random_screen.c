//------------------------------------------------------------------------
//  ランダムパターンによる、検出できる故障(冗長でない故障)の先行判定
//------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "random_screen.h"
#include "../header/Define.h"
#include "../main/external_tools.h"
#include "../else/OUTPUT/report.h"

static unsigned char* known;		//故障番号(node_id)ごとに、テストが存在すると確定したか
static int known_size;

static uint64_t rng_state = 0xD1B54A32D192ED03ULL;
static uint64_t next_random(void) {
	rng_state ^= rng_state >> 12; rng_state ^= rng_state << 25; rng_state ^= rng_state >> 27;
	return rng_state * 0x2545F4914F6CDD1DULL;
}

static int compare_fault_key(const void* left, const void* right) {
	const UNDET_FAULT_LIST* a = *(const UNDET_FAULT_LIST* const*)left;
	const UNDET_FAULT_LIST* b = *(const UNDET_FAULT_LIST* const*)right;
	int by_name = strcmp(a->name, b->name);
	return by_name != 0 ? by_name : (a->type > b->type) - (a->type < b->type);
}

int random_screen_known_testable(const UNDET_FAULT_LIST* fault) {
	return known != NULL && fault->node_id >= 0 && fault->node_id < known_size && known[fault->node_id];
}

void random_screen_release(void) {
	free(known);
	known = NULL;
	known_size = 0;
}

int random_screen(void) {
	char tp[1024], pin[1024], dic[1024], det[1024], undet[1024];
	char command[8192];
	char line[1024];
	FILE* file = NULL;
	UNDET_FAULT_LIST** sorted;
	int exit_code, count = 0, marked = 0;

	random_screen_release();
	if (opt.random_patterns <= 0 || ufl_num <= 0) return SUCCESS;
	if (output_path(tp, sizeof(tp), "random_tp.txt") != SUCCESS || output_path(pin, sizeof(pin), "pin.txt") != SUCCESS ||
		output_path(dic, sizeof(dic), "random_fsim_log.txt") != SUCCESS || output_path(det, sizeof(det), "random_xid_det.txt") != SUCCESS ||
		output_path(undet, sizeof(undet), "random_xid_undet.txt") != SUCCESS) {
		return ERROR;
	}

	/* ランダムなテストパターン(外部入力, FF の順) */
	if (fopen_s(&file, tp, "w") != 0 || file == NULL) {
		printf("[ERROR] Cannot create %s\n", tp);
		return ERROR;
	}
	for (int p = 0; p < opt.random_patterns; p++) {
		for (int i = 0; i < n_pi + n_dff; i++) fputc((next_random() >> 40) & 1 ? '1' : '0', file);
		fputc('\n', file);
	}
	fclose(file);

	printf("[RANDOM] Fault simulation of %d random patterns (%s).\n", opt.random_patterns, TOOL_FSIM);
	snprintf(command, sizeof(command), TOOL_FSIM " -c \"%s\" -tx \"%s\" -pin \"%s\" -fm TDF -fsim YES -dic \"%s\" -det \"%s\" -undet \"%s\"",
		opt.circuit, tp, pin, dic, det, undet);
	exit_code = run_external(command);
	if (exit_code != 0) {
		printf("[ERROR] %s failed (exit code %d).\n        Command: %s\n", TOOL_FSIM, exit_code, command);
		return ERROR;
	}

	sorted = (UNDET_FAULT_LIST**)malloc(sizeof(*sorted) * ufl_num);
	known = (unsigned char*)calloc(all_num > 0 ? all_num : 1, 1);
	if (sorted == NULL || known == NULL) {
		printf("[ERROR] Out of memory (random screen).\n");
		free(sorted);
		return ERROR;
	}
	known_size = all_num;
	for (UNDET_FAULT_LIST* fault = ufl; fault != NULL && count < ufl_num; fault = fault->next) sorted[count++] = fault;
	qsort(sorted, count, sizeof(*sorted), compare_fault_key);

	if (fopen_s(&file, dic, "r") != 0 || file == NULL) {
		printf("[ERROR] Cannot open the fault-simulation result %s.\n", dic);
		free(sorted);
		return ERROR;
	}
	while (fgets(line, sizeof(line), file) != NULL) {		/* パターンごとの見出し(tp0 ...)の行は、故障名が無いので読み飛ばす */
		char* context = NULL;
		char* type_token = strtok_s(line, " \t\r\n", &context);
		char* name = strtok_s(NULL, " \t\r\n", &context);
		UNDET_FAULT_LIST key, * key_pointer = &key, ** found;

		if (type_token == NULL || name == NULL) continue;
		if (strcmp(type_token, "STR") == 0) key.type = STR;
		else if (strcmp(type_token, "STF") == 0) key.type = STF;
		else continue;
		strcpy_s(key.name, sizeof(key.name), name);
		found = (UNDET_FAULT_LIST**)bsearch(&key_pointer, sorted, count, sizeof(*sorted), compare_fault_key);
		if (found != NULL && !known[(*found)->node_id]) {
			known[(*found)->node_id] = 1;
			marked++;
		}
	}
	fclose(file);
	free(sorted);
	printf("[RANDOM] %d of %d undetected faults are detectable (no redundancy check needed).\n", marked, ufl_num);
	return SUCCESS;
}

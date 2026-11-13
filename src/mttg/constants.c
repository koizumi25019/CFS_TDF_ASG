//------------------------------------------------------------------------
//  定数の信号線の検出
//------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "constants.h"
#include "pbo_generate.h"
#include "../header/Define.h"
#include "../asg/essential.h"
#include "../asg/opb/clasp/clasp.h"
#include "../else/OUTPUT/report.h"

#define SIM_WORDS 128				/* 64パターン × 128 = 8192 回 */

//---------------------------------------------------------------------
//  乱数で動かす(64パターンを1語で並列に)
//---------------------------------------------------------------------
static uint64_t rng_state = 0x9E3779B97F4A7C15ULL;

static uint64_t next_random(void) {
	rng_state ^= rng_state >> 12; rng_state ^= rng_state << 25; rng_state ^= rng_state >> 27;
	return rng_state * 0x2545F4914F6CDD1DULL;
}

static uint64_t evaluate(const NLIST* gate, const uint64_t* value) {
	uint64_t r;
	switch (gate->type) {
	case INV:	return ~value[gate->in[0] - nl];
	case BUF:
	case FOUT:	return value[gate->in[0] - nl];
	case EXOR:	return value[gate->in[0] - nl] ^ value[gate->in[1] - nl];
	case AND:
	case NAND:
		r = ~(uint64_t)0;
		for (int i = 0; i < gate->n_in; i++) r &= value[gate->in[i] - nl];
		return gate->type == AND ? r : ~r;
	case OR:
	case NOR:
		r = 0;
		for (int i = 0; i < gate->n_in; i++) r |= value[gate->in[i] - nl];
		return gate->type == OR ? r : ~r;
	}
	return 0;
}

//---------------------------------------------------------------------
//  net がいつも value であることを clasp で証明する(逆の値になる問題が解なしなら証明できた)
//---------------------------------------------------------------------
static int prove(int net, int value, int* proved) {
	unsigned char* need1 = (unsigned char*)calloc(n_net, 1);
	unsigned char* need2 = (unsigned char*)calloc(n_net, 1);
	char path[1024];
	FILE* body = tmpfile();
	FILE* out = NULL;
	unsigned int constraints = 0;
	int c;

	*proved = 0;
	if (need1 == NULL || need2 == NULL || body == NULL || output_path(path, sizeof(path), "mttg_pbo.txt") != SUCCESS) {
		free(need1); free(need2);
		if (body) fclose(body);
		return ERROR;
	}
	need1[net] = 1;
	for (int k = n_net - 1; k >= 0; k--) {		/* net の値を決める入力側の信号線 */
		const NLIST* gate = sort_nl[k];
		int index = (int)(gate - nl);
		if (!need1[index] || gate->type == IN || gate->type == DFF) continue;
		for (int j = 0; j < gate->n_in; j++) need1[gate->in[j] - nl] = 1;
	}
	pbo_generate_good_circuit(body, need1, need2);
	fprintf(body, value ? "1 ~x%u >=1;\n" : "1 x%u >=1;\n", nl[net].suf_1t);
	rewind(body);
	while ((c = fgetc(body)) != EOF) if (c == ';') constraints++;
	rewind(body);
	if (fopen_s(&out, path, "w") != 0 || out == NULL) {
		printf("[ERROR] Cannot create %s\n", path);
		free(need1); free(need2); fclose(body);
		return ERROR;
	}
	fprintf(out, "* #variable= %u #constraint= %u #product= 0 sizeproduct= 0\nmin:;\n", (unsigned)(VAR_FIRST_CIRCUIT + 2 * n_net - 1), constraints);
	while ((c = fgetc(body)) != EOF) fputc(c, out);
	fclose(out);
	fclose(body);
	free(need1);
	free(need2);

	if (CLASP_solve(false) != CLASP_OKAY) return ERROR;
	*proved = (clasp.status == CLASP_STATUS_UNSAT);
	return SUCCESS;
}

//---------------------------------------------------------------------
//  constants_detect
//---------------------------------------------------------------------
int constants_detect(void) {
	uint64_t* value = (uint64_t*)malloc(sizeof(uint64_t) * n_net);
	uint64_t* all_one = (uint64_t*)malloc(sizeof(uint64_t) * n_net);
	uint64_t* all_zero = (uint64_t*)malloc(sizeof(uint64_t) * n_net);
	int candidate_count = 0, proved_count = 0;

	if (value == NULL || all_one == NULL || all_zero == NULL) {
		printf("[ERROR] Out of memory (constant detection).\n");
		free(value); free(all_one); free(all_zero);
		return ERROR;
	}
	for (int i = 0; i < n_net; i++) { all_one[i] = all_zero[i] = ~(uint64_t)0; }
	for (int w = 0; w < SIM_WORDS; w++) {
		for (int k = 0; k < n_net; k++) {
			const NLIST* gate = sort_nl[k];
			int index = (int)(gate - nl);
			value[index] = (gate->type == IN || gate->type == DFF) ? next_random() : evaluate(gate, value);
			all_one[index] &= value[index];
			all_zero[index] &= ~value[index];
		}
	}

	for (int k = 0; k < n_net; k++) {
		const NLIST* gate = sort_nl[k];
		int index = (int)(gate - nl), v, proved;

		if (gate->type == IN || gate->type == DFF) continue;
		if (all_one[index] == ~(uint64_t)0) v = 1;
		else if (all_zero[index] == ~(uint64_t)0) v = 0;
		else continue;
		candidate_count++;
		if (prove(index, v, &proved) != SUCCESS) {
			printf("[ERROR] The solver run failed while checking the constant candidate %s.\n", gate->name);
			free(value); free(all_one); free(all_zero);
			return ERROR;
		}
		if (proved) {
			if (!essential_set_constant(gate, v)) {
				free(value); free(all_one); free(all_zero);
				return ERROR;
			}
			proved_count++;
		}
	}
	printf("[INFO] Constant signals: %d candidates from %d random patterns, %d proved.\n", candidate_count, SIM_WORDS * 64, proved_count);
	free(value); free(all_one); free(all_zero);
	return SUCCESS;
}

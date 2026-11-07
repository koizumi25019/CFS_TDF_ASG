#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../header/Define.h"

static int same_fault(int type, const char* name, const UNDET_FAULT_LIST* fault)
{
	return fault->type == type && strcmp(fault->name, name) == 0;
}

static UNDET_FAULT_LIST* clone_fault(const UNDET_FAULT_LIST* source)
{
	UNDET_FAULT_LIST* copy = (UNDET_FAULT_LIST*)malloc(sizeof(*copy));

	if (copy == NULL) {
		printf("[ERROR] Failed to allocate a detected-fault node.\n");
		exit(EXIT_FAILURE);
	}

	*copy = *source;
	copy->next = NULL;
	return copy;
}

static void prepend_classified_fault(
	UNDET_FAULT_LIST** list,
	int* count,
	const UNDET_FAULT_LIST* source)
{
	UNDET_FAULT_LIST* copy = clone_fault(source);

	copy->next = *list;
	*list = copy;
	(*count)++;
}

static void classify_sttg_fault(
	const UNDET_FAULT_LIST* fault,
	int sttg_enabled,
	int changed_to_wsath_25)
{
	if (strcmp(fi[STTG_WSATH_MODE].name, "ON") != 0 || sttg_enabled != ON) {
		return;
	}
	if (tfl == NULL || !same_fault(tfl->type, tfl->name, fault)) {
		return;
	}

	if (changed_to_wsath_25 == ON) {
		prepend_classified_fault(&hpfl, &hpfl_num, fault);
	}
	else {
		prepend_classified_fault(&atfl, &atfl_num, fault);
	}
}

static int parse_fault_type(const char* token, int* type)
{
	if (token == NULL || type == NULL) {
		return ERROR;
	}
	if (strcmp(token, "STR") == 0) {
		*type = STR;
		return SUCCESS;
	}
	if (strcmp(token, "STF") == 0) {
		*type = STF;
		return SUCCESS;
	}
	return ERROR;
}

static void release_target_faults(void)
{
	UNDET_FAULT_LIST* current = tfl;

	while (current != NULL) {
		UNDET_FAULT_LIST* next = current->next;
		free(current);
		current = next;
	}
	tfl = NULL;
}

int fault_judge(int sttg_enabled, int changed_to_wsath_25)
{
	char line[256];
	char result_path[300];
	int line_number = 0;
	int detected_target_count = 0;
	int detected_count = 0;

	sprintf_s(result_path, sizeof(result_path), "%sfsim_log.txt", folder_out);
	if (fopen_s(&fout_fsim_result, result_path, "r") != 0) {
		printf("[ERROR] Cannot open fsim_log.txt (fault_judge.c).\n");
		return ERROR;
	}

	while (fgets(line, sizeof(line), fout_fsim_result) != NULL) {
		char* context = NULL;
		char* type_token;
		char* fault_name;
		int fault_type;
		UNDET_FAULT_LIST* current;
		UNDET_FAULT_LIST* previous;
		UNDET_FAULT_LIST* detected_copy;

		line_number++;
		if (ufl_num == 0) {
			break;
		}
		if (line_number == 1 || line[0] == '\n' || line[0] == '\r') {
			continue;
		}

		type_token = strtok_s(line, " \t\r\n", &context);
		fault_name = strtok_s(NULL, " \t\r\n", &context);
		if (fault_name == NULL || parse_fault_type(type_token, &fault_type) != SUCCESS) {
			continue;
		}

		previous = NULL;
		current = ufl;
		while (current != NULL && !same_fault(fault_type, fault_name, current)) {
			previous = current;
			current = current->next;
		}
		if (current == NULL) {
			continue;
		}

		for (UNDET_FAULT_LIST* target = tfl; target != NULL; target = target->next) {
			if (same_fault(current->type, current->name, target)) {
				detected_target_count++;
				break;
			}
		}

		detected_copy = clone_fault(current);
		detected_copy->next = dfl;
		dfl = detected_copy;
		dfl_num++;
		detected_count++;

		bitintSetNbit_Zero(detflag, current->node_id);
		classify_sttg_fault(current, sttg_enabled, changed_to_wsath_25);
		ufl_free(current, previous, -1);
	}

	release_target_faults();
	if (detected_count > 0) {
		seed_num++;
	}

	standard_output();
	fclose(fout_fsim_result);
	fout_fsim_result = NULL;

	sprintf_s(result_path, sizeof(result_path), "%smttg.txt", folder_out);
	if (fopen_s(&fout_mttg, result_path, "a+") != 0) {
		printf("[ERROR] Cannot open mttg.txt (fault_judge.c).\n");
		return ERROR;
	}
	fprintf(
		fout_mttg,
		"%d: tfl_num:%d\ttfl_det_fault:%d\tdet_fault:%d\n",
		seed_num,
		tfl_num,
		detected_target_count,
		detected_count);
	fclose(fout_mttg);
	fout_mttg = NULL;

	return detected_count;
}

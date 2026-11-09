//------------------------------------------------------------------------
//  検出できなかった故障の原因の判定と、検出できない故障の事前除外(-untest_mode)
//------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>

#include "diagnose.h"
#include "problem.h"
#include "../header/Define.h"
#include "../asg/opb/clasp/clasp.h"
#include "../else/fault_list.h"
#include "../else/OUTPUT/report.h"

//---------------------------------------------------------------------
//  fault だけを目標にした問題を解き、ソルバの状態(CLASP_STATUS_*)を返す。失敗したら CLASP_STATUS_ERROR
//---------------------------------------------------------------------
static int solve_alone(const UNDET_FAULT_LIST* fault, bool with_lfsr, int wsa_threshold) {
	UNDET_FAULT_LIST* saved_list = tfl;
	int saved_count = tfl_num;
	ProblemSpec spec;
	int status = CLASP_STATUS_ERROR;

	tfl = fault_copy(fault);
	tfl_num = 1;
	if (tfl == NULL) {
		printf("[ERROR] Out of memory (target fault).\n");
		tfl = saved_list;
		tfl_num = saved_count;
		return CLASP_STATUS_ERROR;
	}
	spec.with_lfsr = with_lfsr;
	spec.wsa_threshold = wsa_threshold;
	if (problem_write(&spec) == SUCCESS && CLASP_solve(false) == CLASP_OKAY) {
		status = clasp.status;
	}
	fault_list_free(tfl);
	tfl = saved_list;
	tfl_num = saved_count;
	return status;
}

//---------------------------------------------------------------------
//  原因の判定
//---------------------------------------------------------------------
DiagVerdict diagnose_unsat(const UNDET_FAULT_LIST* fault, int wsa_threshold) {
	int status;

	if (opt.lfsr) {
		status = solve_alone(fault, false, wsa_threshold);
		if (status == CLASP_STATUS_ERROR) return DIAG_ERROR;
		if (status == CLASP_STATUS_SAT) return DIAG_UNSEEDABLE;
		if (status == CLASP_STATUS_UNKNOWN) return DIAG_UNKNOWN;
	}
	if (wsa_threshold >= 0) {
		status = solve_alone(fault, false, -1);
		if (status == CLASP_STATUS_ERROR) return DIAG_ERROR;
		if (status == CLASP_STATUS_SAT) return DIAG_WSA;
		if (status == CLASP_STATUS_UNKNOWN) return DIAG_UNKNOWN;
	}
	return DIAG_REDUNDANT;
}

const char* diagnose_verdict_text(DiagVerdict verdict) {
	switch (verdict) {
	case DIAG_REDUNDANT:	return "redundant: no test exists even without the LFSR / power constraints";
	case DIAG_UNSEEDABLE:	return "unseedable: testable without the LFSR / phase shifter, but no seed exists for them";
	case DIAG_WSA:			return "not testable within the WSA threshold (testable without the power constraint)";
	case DIAG_UNKNOWN:		return "unknown: the solver timed out while checking the cause";
	default:				return "error";
	}
}

int diagnose_commit(const UNDET_FAULT_LIST* fault, DiagVerdict verdict) {
	UNDET_FAULT_LIST** list;
	int* count;

	switch (verdict) {
	case DIAG_REDUNDANT:	list = &untestfl;	count = &untest_num;	break;
	case DIAG_UNSEEDABLE:	list = &unseed;		count = &unseed_num;	break;
	case DIAG_WSA:			list = &wsafl;		count = &wsafl_num;		break;
	case DIAG_UNKNOWN:		list = &unknownfl;	count = &unknownfl_num;	break;
	default:				return ERROR;
	}
	if (fault_list_push_copy(list, count, fault) != SUCCESS) return ERROR;
	if (fault_remove_undetected(fault) != SUCCESS) {
		printf("[ERROR] Internal error: the fault %s is not in the undetected list.\n", fault->name);
		return ERROR;
	}
	return SUCCESS;
}

//---------------------------------------------------------------------
//  -untest_mode
//---------------------------------------------------------------------
int untest_mode_run(void) {
	UNDET_FAULT_LIST** faults;
	int checked_count = ufl_num;
	int redundant_before = untest_num;
	int wsa_before = wsafl_num;
	int last_percent = -1;

	faults = (UNDET_FAULT_LIST**)malloc(sizeof(*faults) * (checked_count > 0 ? checked_count : 1));
	if (faults == NULL) {
		printf("[ERROR] Out of memory (untest mode).\n");
		return ERROR;
	}
	{
		UNDET_FAULT_LIST* fault = ufl;
		for (int i = 0; i < checked_count; i++, fault = fault->next) {
			faults[i] = fault_copy(fault);		//ufl のノードは取り除くと解放されるので、複製を調べる
			if (faults[i] == NULL) {
				printf("[ERROR] Out of memory (untest mode).\n");
				for (int j = 0; j < i; j++) free(faults[j]);
				free(faults);
				return ERROR;
			}
		}
	}

	printf("[UNTEST] Checking %d faults one by one (the LFSR constraint is not used).\n", checked_count);
	for (int i = 0; i < checked_count; i++) {
		UNDET_FAULT_LIST* fault = faults[i];
		int status = solve_alone(fault, false, -1);
		int percent = (int)(100.0 * (i + 1) / checked_count);

		if (status == CLASP_STATUS_ERROR) {
			printf("[ERROR] The solver run failed while checking %s.\n", fault->name);
			for (int j = 0; j < checked_count; j++) free(faults[j]);
			free(faults);
			return ERROR;
		}
		if (status == CLASP_STATUS_UNSAT) {
			if (diagnose_commit(fault, DIAG_REDUNDANT) != SUCCESS) return ERROR;
		}
		else if (status == CLASP_STATUS_SAT && opt.low_power) {
			status = solve_alone(fault, false, opt.wsa_threshold);
			if (status == CLASP_STATUS_ERROR) {
				printf("[ERROR] The solver run failed while checking %s.\n", fault->name);
				for (int j = 0; j < checked_count; j++) free(faults[j]);
				free(faults);
				return ERROR;
			}
			if (status == CLASP_STATUS_UNSAT && diagnose_commit(fault, DIAG_WSA) != SUCCESS) return ERROR;
		}
		/* 打ち切りの故障は、ここでは判定せず、通常の流れで扱う */
		if (percent != last_percent) {
			printf("\r[UNTEST] progress >> %d%%", percent);
			last_percent = percent;
		}
	}
	printf("\n[UNTEST] Removed %d faults (%d redundant, %d beyond the WSA threshold); %d faults remain.\n",
		untest_num - redundant_before + wsafl_num - wsa_before, untest_num - redundant_before, wsafl_num - wsa_before, ufl_num);
	for (int j = 0; j < checked_count; j++) free(faults[j]);
	free(faults);
	return SUCCESS;
}

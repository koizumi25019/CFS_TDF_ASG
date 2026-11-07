//------------------------------------------------------------------------
//  実行結果の出力(画面・log.txt・故障一覧ファイル)
//------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "report.h"
#include "../../header/Define.h"

static clock_t run_start_time;

void report_start_clock(void) {
	run_start_time = clock();
}

int output_path(char* buffer, size_t size, const char* file_name) {
	int written = snprintf(buffer, size, "%s%s", folder_out, file_name);
	if (written < 0 || (size_t)written >= size) {
		printf("[ERROR] The output path is too long: %s%s\n", folder_out, file_name);
		return ERROR;
	}
	return SUCCESS;
}

const char* fault_type_name(int type) {
	return type == STR ? "STR" : "STF";
}

//---------------------------------------------------------------------
//  故障一覧ファイル("名前" と "STR/STF" を1故障あたり2行)
//---------------------------------------------------------------------
static int write_fault_list(const char* file_name, const UNDET_FAULT_LIST* list) {
	char path[1024];
	FILE* file = NULL;

	if (output_path(path, sizeof(path), file_name) != SUCCESS) return ERROR;
	if (fopen_s(&file, path, "w") != 0 || file == NULL) {
		printf("[ERROR] Cannot create %s\n", path);
		return ERROR;
	}
	for (; list != NULL; list = list->next) {
		fprintf(file, "%s\n%s\n", list->name, fault_type_name(list->type));
	}
	if (fclose(file) != 0) {
		printf("[ERROR] Cannot write %s\n", path);
		return ERROR;
	}
	return SUCCESS;
}

//---------------------------------------------------------------------
//  実行情報
//---------------------------------------------------------------------
void report_summary(FILE* out, const char* status, const char* reason) {
	double elapsed = (double)(clock() - run_start_time) / CLOCKS_PER_SEC;
	double rate = all_num > 0 ? 100.0 * dfl_num / all_num : 0.0;

	fprintf(out, "//---------------------------------------------------\n");
	fprintf(out, "//	    Transition Fault ASG   Information\n");
	fprintf(out, "//---------------------------------------------------\n");
	fprintf(out, "// Target Circuit              :%s\n", opt.circuit);
	fprintf(out, "// Fault Type                  :TDF\n");
	fprintf(out, "// Seed Generation             :%s\n", opt.target_limit == 1 ? "STTG" : "MTTG");
	fprintf(out, "// Status                      :%s\n", status);
	if (reason != NULL && reason[0] != '\0') {
		fprintf(out, "// Reason                      :%s\n", reason);
	}
	fprintf(out, "//---- result\n");
	fprintf(out, "// Number of Total Faults      :%d\n", all_num);
	fprintf(out, "// Number of Detected Faults   :%d\n", dfl_num);
	fprintf(out, "// Number of Undetected Faults :%d\n", ufl_num);
	fprintf(out, "// Number of Unseed Faults     :%d   (-> %s)\n", unseed_num, FILE_UNSEED);
	fprintf(out, "// Number of Abort Faults      :%d   (-> %s)\n", abortfl_num, FILE_ABORT);
	fprintf(out, "// Number of High Power Faults :%d   (-> %s)\n", hpfl_num, FILE_HIGH_POWER);
	fprintf(out, "// Number of Seeds             :%d\n", seed_num);
	fprintf(out, "// Seeds with relaxed WSA      :%d\n", seed_relaxed_num);
	fprintf(out, "// Rate of Detected Faults     :%.2lf%%\n", rate);
	fprintf(out, "// Elapsed time                :%.2lfsec\n", elapsed);
	fprintf(out, "// Time to graph               :%.2lfsec\n", (double)time_graph / CLOCKS_PER_SEC);
	fprintf(out, "//---- setting\n");
	fprintf(out, "// Low Power Mode              :%s\n", OPTonoff(opt.low_power));
	if (opt.low_power) {
		fprintf(out, "// WSA threshold (specified)   :%d%%\n", opt.wsa_threshold);
		fprintf(out, "// WSA threshold (highest used):%d%%\n", wsa_used_max > 0 ? wsa_used_max : opt.wsa_threshold);
	}
	fprintf(out, "// Solver time limit           :%dsec%s\n", opt.solver_time, opt.regen ? " (-regen: 60/180/300sec)" : "");
	fprintf(out, "// Target faults limit         :%d\n", opt.target_limit);
	fprintf(out, "// Regeneration (-regen)       :%s\n", OPTonoff(opt.regen));
	fprintf(out, "// STTG retry (-sttg)          :%s\n", OPTonoff(opt.sttg));
	fprintf(out, "// FP mode (-fp)               :%s\n", OPTonoff(opt.fault_propagation));
	fprintf(out, "// CPS mode (-cps)             :%s\n", OPTonoff(opt.cps));
	if (opt.parallel_threads > 0) {
		fprintf(out, "// Parallel mode (-parallel)   :ON (%d threads)\n", opt.parallel_threads);
	}
	else {
		fprintf(out, "// Parallel mode (-parallel)   :OFF\n");
	}
	fprintf(out, "// First model only (-mode_1)  :%s\n", OPTonoff(opt.first_model_only));
	fprintf(out, "//---------------------------------------------------\n");
}

//---------------------------------------------------------------------
//  目標故障集合の履歴(tfl_log.txt)とシードごとの記録(mttg.txt)
//---------------------------------------------------------------------
int report_append_targets(void) {
	char path[1024];
	FILE* file = NULL;

	if (output_path(path, sizeof(path), FILE_TARGET_LOG) != SUCCESS) return ERROR;
	if (fopen_s(&file, path, "a") != 0 || file == NULL) {
		printf("[ERROR] Cannot append to %s\n", path);
		return ERROR;
	}
	for (const UNDET_FAULT_LIST* fault = tfl; fault != NULL; fault = fault->next) {
		fprintf(file, "%s\n %s\n", fault->name, fault_type_name(fault->type));
	}
	fclose(file);
	return SUCCESS;
}

int report_append_seed_log(int tfl_count, int detected_target_count, int detected_count, int wsa_threshold, const char* phase) {
	char path[1024];
	FILE* file = NULL;

	if (output_path(path, sizeof(path), FILE_SEED_LOG) != SUCCESS) return ERROR;
	if (fopen_s(&file, path, "a") != 0 || file == NULL) {
		printf("[ERROR] Cannot append to %s\n", path);
		return ERROR;
	}
	fprintf(file, "%d: tfl_num:%d\ttfl_det_fault:%d\tdet_fault:%d\twsa:%d\tphase:%s\n",
		seed_num, tfl_count, detected_target_count, detected_count, wsa_threshold, phase);
	fclose(file);
	return SUCCESS;
}

//---------------------------------------------------------------------
//  終了時の出力
//---------------------------------------------------------------------
int report_write_final(const char* status, const char* reason) {
	char path[1024];
	FILE* file = NULL;
	int result = SUCCESS;

	if (output_path(path, sizeof(path), FILE_LOG) != SUCCESS) return ERROR;
	if (fopen_s(&file, path, "w") != 0 || file == NULL) {
		printf("[ERROR] Cannot create %s\n", path);
		return ERROR;
	}
	report_summary(file, status, reason);
	if (fclose(file) != 0) {
		printf("[ERROR] Cannot write %s\n", path);
		result = ERROR;
	}

	if (write_fault_list(FILE_DETECTED, dfl) != SUCCESS) result = ERROR;
	if (write_fault_list(FILE_UNDETECTED, ufl) != SUCCESS) result = ERROR;
	if (write_fault_list(FILE_UNSEED, unseed) != SUCCESS) result = ERROR;
	if (write_fault_list(FILE_ABORT, abortfl) != SUCCESS) result = ERROR;
	if (write_fault_list(FILE_HIGH_POWER, hpfl) != SUCCESS) result = ERROR;
	return result;
}

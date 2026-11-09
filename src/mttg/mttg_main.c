//------------------------------------------------------------------------
//  MTTG(複数目標故障シード生成)の全体の流れ
//
//    mttg_main()
//      mttg_prepare()           入力の読み込み・回路モデルの準備
//      generate_seed()          未検出故障が無くなるまで繰り返す。1回でシードを1つ作る
//        select_targets()         目標故障の選択(先頭が一次故障)
//        problem_write()          PBOの問題ファイルを作る(problem.c)
//        CLASP()                  ソルバで解く
//        apply_solution()         解けたら: テストパターン→WSA確認→故障シミュレーション→検出故障の整理
//        regen_next_attempt()     解けなかったら: 再生成の方針(regen.h)に従って次の試行へ
//        give_up_on_primary()     次が無ければ一次故障を断念し、原因を調べて分類する(diagnose.c)
//------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../header/Define.h"
#include "../asg/opb/clasp/clasp.h"
#include "../asg/createSGmodel.h"
#include "../else/fault_list.h"
#include "../else/Fsim/fault_judge.h"
#include "../else/OUTPUT/report.h"
#include "../main/external_tools.h"
#include "pbo_generate.h"
#include "mttg_check_wsa.h"
#include "regen.h"
#include "problem.h"
#include "diagnose.h"
#include "../asg/essential.h"
#include "../asg/screen.h"

static FILE* seed_file;				//seed.txt(1行1シード。-lfsr ON のみ)
static FILE* tp_set_file;			//tp_set.txt(1行1テストパターン。seed.txt の同じ行のシードから生成される)

typedef struct {
	int cps_single_next;			//-cps: 前のシードが1件も検出できなかったので、次は単一目標にする
} MttgState;

//===================================================
//関数名:mttg_prepare
//機能:入力の読み込みと回路モデルの準備
//戻り値：int 正常終了:SUCCESS , 異常終了:ERROR
//===================================================
static int mttg_prepare(void) {
	char path[1024];

	if (opt.lfsr) {
		if (output_path(path, sizeof(path), FILE_SEED) != SUCCESS) return ERROR;
		if (fopen_s(&seed_file, path, "w") != 0 || seed_file == NULL) {
			printf("[ERROR] Cannot create %s\n", path);
			return ERROR;
		}
	}
	if (output_path(path, sizeof(path), FILE_TEST_PATTERN_SET) != SUCCESS) return ERROR;
	if (fopen_s(&tp_set_file, path, "w") != 0 || tp_set_file == NULL) {
		printf("[ERROR] Cannot create %s\n", path);
		return ERROR;
	}
	if (pin_make() != SUCCESS) return ERROR;
	if (levelize() != SUCCESS) {
		printf("[ERROR] Levelization of the netlist failed.\n");
		return ERROR;
	}
	if (transition_delay_fault_set() != SUCCESS) return ERROR;
	all_num = ufl_num;
	fault_flags_init();
	if (opt.lfsr && ReadFile() != true) return ERROR;		//LFSR+フェーズシフタの入力ファイル
	if (!essential_open()) return ERROR;
	if (screen_untestable_faults() != SUCCESS) return ERROR;		//必須割当てだけで検出不能と分かる故障を外す

	report_summary(stdout, "RUNNING", NULL);
	AssigneVarsGC();					//正常回路のブール変数番号
	if (opt.lfsr && CreateLFSRmodel() != true) {	//LFSR+フェーズシフタの制約式
		printf("[ERROR] Creating the LFSR / phase-shifter model failed.\n");
		return ERROR;
	}
	if (opt.untest_mode && untest_mode_run() != SUCCESS) return ERROR;	//冗長な故障などを最初に除く
	return SUCCESS;
}

//===================================================
//関数名:write_test_pattern
//機能:ソルバの解のテストパターンを tp.txt に書く
//===================================================
static int write_test_pattern(void) {
	char path[1024];
	FILE* file = NULL;

	if (output_path(path, sizeof(path), FILE_TEST_PATTERN) != SUCCESS) return ERROR;
	if (fopen_s(&file, path, "w") != 0 || file == NULL) {
		printf("[ERROR] Cannot create %s\n", path);
		return ERROR;
	}
	fprintf(file, "%s\n", clasp.sol[SOL_TP]);
	fclose(file);
	return SUCCESS;
}

//===================================================
//関数名:run_fault_simulation
//機能:XID2.exe でテストパターンの故障シミュレーションを行う(結果は fsim_log.txt)
//===================================================
static int run_fault_simulation(void) {
	char tp[1024], pin[1024], dic[1024], det[1024], undet[1024];
	char command[8192];
	int exit_code;

	if (output_path(tp, sizeof(tp), FILE_TEST_PATTERN) != SUCCESS || output_path(pin, sizeof(pin), "pin.txt") != SUCCESS ||
		output_path(dic, sizeof(dic), "fsim_log.txt") != SUCCESS || output_path(det, sizeof(det), "xid_det.txt") != SUCCESS ||
		output_path(undet, sizeof(undet), "xid_undet.txt") != SUCCESS) {
		return ERROR;
	}
	snprintf(command, sizeof(command), TOOL_FSIM " -c \"%s\" -tx \"%s\" -pin \"%s\" -fm TDF -fsim YES -dic \"%s\" -det \"%s\" -undet \"%s\"",
		opt.circuit, tp, pin, dic, det, undet);
	exit_code = run_external(command);
	if (exit_code != 0) {
		printf("[ERROR] %s failed (exit code %d).\n        Command: %s\n", TOOL_FSIM, exit_code, command);
		return ERROR;
	}
	return SUCCESS;
}

//===================================================
//関数名:release_targets
//機能:目標故障リストを解放する
//===================================================
static void release_targets(void) {
	fault_list_free(tfl);
	tfl = NULL;
	tfl_num = 0;
}

//===================================================
//関数名:apply_solution
//機能:ソルバが見つけたシードを使う。テストパターン→WSA確認→故障シミュレーション→検出故障の整理
//引数：解けた試行，STTGへ切り替えて解けたか，状態
//戻り値：int 正常終了:SUCCESS , 異常終了:ERROR
//===================================================
static int apply_solution(const MttgAttempt* attempt, int switched_to_single, MttgState* state) {
	int wsa = regen_wsa_threshold(attempt);
	DetectClass detect_class = attempt->wsa_stage > 0 ? DETECT_RELAXED_WSA
		: (switched_to_single ? DETECT_AFTER_STTG : DETECT_NORMAL);
	JudgeResult result;
	int single = (tfl_num == 1);
	char primary_name[256];
	int primary_type = tfl->type;

	strcpy_s(primary_name, sizeof(primary_name), tfl->name);

	if (write_test_pattern() != SUCCESS) return ERROR;
	if (opt.low_power && mttg_check_wsa(wsa) != WSA_WITHIN_THRESHOLD) {
		return ERROR;
	}
	if (run_fault_simulation() != SUCCESS) return ERROR;
	if (fault_judge(detect_class, &result) != SUCCESS) return ERROR;

	if (result.detected_count > 0) {
		seed_num++;
		if (attempt->wsa_stage > 0) seed_relaxed_num++;
		if (opt.low_power && wsa > wsa_used_max) wsa_used_max = wsa;
		if (opt.lfsr) {
			fprintf(seed_file, "%s\n", clasp.sol[SOL_SEED]);
			fflush(seed_file);			//途中でエラー終了しても、ここまでのシードは残す
		}
		fprintf(tp_set_file, "%s\n", clasp.sol[SOL_TP]);
		fflush(tp_set_file);
		if (report_append_seed_log(tfl_num, result.detected_target_count, result.detected_count, wsa, regen_phase_name(attempt->phase)) != SUCCESS) return ERROR;
		printf("[SEED %d] detected %d faults (targets %d/%d, %s), %d undetected faults remain", seed_num,
			result.detected_count, result.detected_target_count, tfl_num, regen_phase_name(attempt->phase), ufl_num);
		if (opt.low_power) printf(", WSA threshold %d%%", wsa);
		printf("\n");
		if (attempt->wsa_stage > 0) {
			printf("[WSA] This seed needed a relaxed WSA threshold (%d%%, specified: %d%%).\n", wsa, opt.wsa_threshold);
		}
		state->cps_single_next = 0;
	}
	else if (single || !opt.cps) {
		printf("[ERROR] The seed for %s %s detected none of the remaining faults in fault simulation.\n"
			"        The PBO model and %s disagree. See %s%s and %s%s.\n",
			primary_name, fault_type_name(primary_type), TOOL_FSIM, folder_out, FILE_TEST_PATTERN, folder_out, "fsim_log.txt");
		return ERROR;
	}
	else {
		printf("[CPS] No fault was detected; the next target will be a single fault.\n");
		state->cps_single_next = 1;
	}

	release_targets();
	return SUCCESS;
}

//===================================================
//関数名:give_up_on_primary
//機能:再生成してもソルバが解けなかった一次故障を断念する。解なしのときは原因を調べて分類する
//引数：解なしと確定していたか(0: 打ち切り)，最後に使ったWSA閾値
//===================================================
static int give_up_on_primary(int solver_unsat, int wsa) {
	UNDET_FAULT_LIST* primary = fault_copy(tfl);
	DiagVerdict verdict = DIAG_UNKNOWN;

	if (primary == NULL) {
		printf("[ERROR] Internal error: there is no primary fault to give up.\n");
		return ERROR;
	}
	if (solver_unsat) {
		printf("[DIAG] %s %s: no solution. Checking the cause ...\n", primary->name, fault_type_name(primary->type));
		verdict = diagnose_unsat(primary, opt.low_power ? wsa : -1);
		if (verdict == DIAG_ERROR) {
			printf("[ERROR] The solver run failed while checking the cause.\n");
			free(primary);
			return ERROR;
		}
	}
	printf("[MTTG] The primary fault %s %s was given up: %s.\n", primary->name, fault_type_name(primary->type),
		solver_unsat ? diagnose_verdict_text(verdict) : "the solver found neither a solution nor unsatisfiability in time");
	if (diagnose_commit(primary, verdict) != SUCCESS) {
		free(primary);
		return ERROR;
	}
	free(primary);
	release_targets();
	return SUCCESS;
}

//===================================================
//関数名:select_targets
//機能:次のシードの目標故障(tfl)を選ぶ
//===================================================
static int select_targets(MttgState* state) {
	if (state->cps_single_next) {			//-cps: 前のシードが何も検出できなかったので、単一目標にする
		state->cps_single_next = 0;
		return fault_select_single(ufl);
	}
	return fault_select_targets();
}

//===================================================
//関数名:switch_to_primary_only
//機能:目標故障集合を、その一次故障だけにする(STTGへ切り替え)
//===================================================
static int switch_to_primary_only(void) {
	UNDET_FAULT_LIST* primary = fault_copy(tfl);

	if (primary == NULL) {
		printf("[ERROR] Out of memory (primary fault).\n");
		return ERROR;
	}
	release_targets();
	if (fault_select_single(primary) != SUCCESS) {
		free(primary);
		return ERROR;
	}
	free(primary);
	return SUCCESS;
}

//===================================================
//関数名:generate_seed
//機能:シードを1つ作る(うまくいかなければ regen.h の方針で再生成し、最後は一次故障を断念する)
//===================================================
static int generate_seed(MttgState* state) {
	MttgAttempt attempt = regen_first_attempt();
	int switched_to_single = 0;

	if (select_targets(state) != SUCCESS) return ERROR;
	if (report_append_targets() != SUCCESS) return ERROR;
	if (tfl_num == 1) attempt.phase = PHASE_SINGLE;		//最初から単一目標(-target_limit 1 または -cps)

	for (;;) {
		MttgAttempt next;
		int solver_unsat;

		{
			ProblemSpec spec;
			spec.with_lfsr = opt.lfsr;
			spec.wsa_threshold = opt.low_power ? regen_wsa_threshold(&attempt) : -1;
			if (problem_write(&spec) != SUCCESS) return ERROR;
		}
		if (CLASP() != CLASP_OKAY) {
			printf("[ERROR] The solver run failed.\n");
			return ERROR;
		}
		if (clasp.status == CLASP_STATUS_SAT) {
			return apply_solution(&attempt, switched_to_single, state);
		}

		solver_unsat = (clasp.status == CLASP_STATUS_UNSAT);
		if (!regen_next_attempt(&attempt, solver_unsat, &next)) {
			return give_up_on_primary(solver_unsat, regen_wsa_threshold(&attempt));
		}
		if (next.phase != attempt.phase) {
			if (switch_to_primary_only() != SUCCESS) return ERROR;
			switched_to_single = 1;
			if (report_append_targets() != SUCCESS) return ERROR;
			printf("[REGEN] Retry the primary fault alone (STTG) at WSA threshold %d%%.\n", regen_wsa_threshold(&next));
		}
		else {
			printf("[REGEN] Retry %s at WSA threshold %d%%.\n", regen_phase_name(next.phase), regen_wsa_threshold(&next));
		}
		attempt = next;
	}
}

//===================================================
//関数名:mttg_main
//機能:遷移故障のMTTGのメイン
//戻り値：int 正常終了:SUCCESS(=0) , 異常終了:ERROR(=-1)
//===================================================
int mttg_main(void) {
	MttgState state = { 0 };
	int result = mttg_prepare();

	while (result == SUCCESS && ufl_num != 0) {
		result = generate_seed(&state);
	}

	release_targets();
	essential_close();
	if (seed_file != NULL) {
		fclose(seed_file);
		seed_file = NULL;
	}
	if (tp_set_file != NULL) {
		fclose(tp_set_file);
		tp_set_file = NULL;
	}
	return result;
}

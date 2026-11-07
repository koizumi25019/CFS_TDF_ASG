//------------------------------------------------------------------------
//  clasp の実行
//------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../header/Define.h"
#include "../main/external_tools.h"
#include "../else/OUTPUT/report.h"

/* clasp の終了コード(ビットの和)。1:時間切れなどで中断 10:解が見つかった 20:探索空間を調べ尽くした */
enum {
    CLASP_EXIT_NO_MODEL = 0,
    CLASP_EXIT_INTERRUPTED = 1,
    CLASP_EXIT_SAT = 10,
    CLASP_EXIT_SAT_INTERRUPTED = 11,
    CLASP_EXIT_EXHAUSTED = 20,
    CLASP_EXIT_OPTIMUM = 30
};

static int is_clasp_result_code(int exit_code)
{
    return exit_code == CLASP_EXIT_NO_MODEL || exit_code == CLASP_EXIT_INTERRUPTED ||
        exit_code == CLASP_EXIT_SAT || exit_code == CLASP_EXIT_SAT_INTERRUPTED ||
        exit_code == CLASP_EXIT_EXHAUSTED || exit_code == CLASP_EXIT_OPTIMUM;
}

int clasp_call(int attempt_index)
{
    /* -regen のとき、打ち切り時間を延ばしながら再試行する */
    static const int retry_timeouts[CLASP_RETRY_COUNT] = { 60, 180, 300 };
    int timeout_seconds = opt.solver_time;
    int exit_code;
    char command[4096];
    char pbo_path[1024], log_path[1024];
    char parallel_option[64] = "";
    const char* model_option = opt.first_model_only ? " --models=1 --opt-mode=ignore" : "";

    if (attempt_index < 0 || attempt_index >= CLASP_RETRY_COUNT) {
        return ERROR;
    }
    if (opt.regen) {
        timeout_seconds = retry_timeouts[attempt_index];
    }
    if (opt.parallel_threads > 0) {
        snprintf(parallel_option, sizeof(parallel_option), " --parallel-mode=%d", opt.parallel_threads);
    }
    if (output_path(pbo_path, sizeof(pbo_path), "mttg_pbo.txt") != SUCCESS ||
        output_path(log_path, sizeof(log_path), "clasp_log_mttg.txt") != SUCCESS) {
        return ERROR;
    }
    snprintf(command, sizeof(command), TOOL_CLASP " \"%s\" --quiet=1 --time-limit=%d%s%s > \"%s\"",
        pbo_path, timeout_seconds, parallel_option, model_option, log_path);

    printf("[CLASP] Attempt %d, timeout %d seconds.\n", attempt_index + 1, timeout_seconds);
    exit_code = run_external(command);
    if (!is_clasp_result_code(exit_code)) {
        printf("[ERROR] %s exited with code %d.\n        Command: %s\n", TOOL_CLASP, exit_code, command);
        return ERROR;
    }
    return SUCCESS;
}

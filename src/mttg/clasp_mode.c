#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../header/Define.h"
#ifndef _WIN32
#include <sys/wait.h>
#endif

/* Windows の system() はコマンドの終了コードを返す。POSIX は wait ステータスを返す。 */
static int command_exit_code(int status)
{
#ifdef _WIN32
    return status;
#else
    return (status != -1 && WIFEXITED(status)) ? WEXITSTATUS(status) : -1;
#endif
}

/* clasp exit codes: bit flags 1 (interrupted, e.g. time limit), 10 (model found),
   20 (search space exhausted). 1/11 are normal results of --time-limit. */
static int is_clasp_result_code(int exit_code)
{
    return exit_code == 0 || exit_code == 1 || exit_code == 10 ||
        exit_code == 11 || exit_code == 20 || exit_code == 30;
}

int clasp_retry_enabled(void)
{
    return strcmp(fi[RESOLVING_MODE].name, "ON") == 0 ||
        strcmp(fi[REGEN_MODE].name, "ON") == 0;
}

int clasp_call(int retry_index)
{
    const int retry_timeouts[CLASP_RETRY_COUNT] = {60, 180, 300};
    int timeout_seconds = fi[TIME].value;
    int exit_code;
    char command[4096];
    char parallel_option[64] = "";
    const char* model_option = fi[MODE_1].flag == ON && strcmp(fi[MODE_1].name, "ON") == 0
        ? " --models=1 --opt-mode=ignore" : "";

    if (retry_index < 0 || retry_index >= CLASP_RETRY_COUNT) {
        return ERROR;
    }
    if (clasp_retry_enabled()) {
        timeout_seconds = retry_timeouts[retry_index];
    }
    if (fi[PARALLEL].flag == ON) {
        sprintf_s(parallel_option, sizeof(parallel_option), " --parallel-mode=%d", fi[PARALLEL].value);
    }
    if (sprintf_s(command, sizeof(command),
        "clasp.exe \"%smttg_pbo.txt\" --quiet=1 --time-limit=%d%s%s > \"%sclasp_log_mttg.txt\"",
        folder_out, timeout_seconds, parallel_option, model_option, folder_out) < 0) {
        return ERROR;
    }
    printf("[CLASP] Attempt %d, timeout %d seconds.\n", retry_index + 1, timeout_seconds);
    exit_code = command_exit_code(system(command));
    if (!is_clasp_result_code(exit_code)) {
        printf("[ERROR] clasp exited with code %d.\n", exit_code);
        return ERROR;
    }
    return SUCCESS;
}

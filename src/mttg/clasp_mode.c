#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../header/Define.h"

int clasp_call(int retry_index)
{
    const int retry_timeouts[] = {60, 180, 300};
    int timeout_seconds = fi[TIME].value;
    int exit_code;
    char command[4096];
    char parallel_option[64] = "";
    const char* model_option = fi[MODE_1].flag == ON && strcmp(fi[MODE_1].name, "ON") == 0
        ? " --models=1 --opt-mode=ignore" : "";

    if (retry_index < 0 || retry_index >= 3) {
        return ERROR;
    }
    if (strcmp(fi[RESOLVING_MODE].name, "ON") == 0) {
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
    exit_code = system(command);
    /* clasp reports SAT/UNSAT using nonzero exit codes. */
    if (exit_code != 0 && exit_code != 10 && exit_code != 20 && exit_code != 30) {
        printf("[ERROR] clasp exited with code %d.\n", exit_code);
        return ERROR;
    }
    return SUCCESS;
}

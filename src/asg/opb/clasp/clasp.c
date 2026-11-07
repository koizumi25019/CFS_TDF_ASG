//-------------------------------------------------------------------------------------------------------------
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "clasp.h"
#include "../../read.h"
#include "../../../header/Define.h"
#include "../../../else/OUTPUT/report.h"

void CLASPgetStatus(FILE* log)
{
    char line[4096];
    clasp.status = CLASP_STATUS_ERROR;
    rewind(log);
    while (fgets(line, sizeof(line), log) != NULL) {
        if (strncmp(line, "s UNSATISFIABLE", 15) == 0) {
            clasp.status = CLASP_STATUS_UNSAT;
        } else if (strncmp(line, "s SATISFIABLE", 13) == 0 ||
                   strncmp(line, "s OPTIMUM FOUND", 15) == 0) {
            clasp.status = CLASP_STATUS_SAT;
        } else if (strncmp(line, "s UNKNOWN", 9) == 0) {
            clasp.status = CLASP_STATUS_UNKNOWN;
        }
    }
}

/* Read the last model by variable ID, not by token position. */
void CLASPgetSol(FILE* log)
{
    char* line = (char*)malloc(MAXSIZE_BUFFER);		/* 1行の最大長。スタックに置くには大きい */
    int required_count = readdata.lfsrnet.num + n_pi + n_dff;
    unsigned char* seen = (unsigned char*)calloc(required_count + 1, 1);
    int assigned_count = 0;
    int in_model = 0;
    int invalid_model = 0;
    if (seen == NULL || line == NULL) {
        free(seen);
        free(line);
        clasp.status = CLASP_STATUS_ERROR;
        return;
    }
    rewind(log);
    while (fgets(line, MAXSIZE_BUFFER, log) != NULL) {
        char* context = NULL;
        char* token;
        if (line[0] != 'v' || line[1] != ' ') {
            in_model = 0;
            continue;
        }
        if (!in_model) {
            memset(seen, 0, required_count + 1);
            memset(clasp.sol[SOL_SEED], '0', readdata.lfsrnet.num);
            memset(clasp.sol[SOL_TP], '0', n_pi + n_dff);
            assigned_count = 0;
            invalid_model = 0;
            in_model = 1;
        }
        token = strtok_s(line + 2, " \t\r\n", &context);
        while (token != NULL) {
            char value = '1';
            char* end;
            long variable_id;
            if (*token == '-') {
                value = '0';
                token++;
            }
            if (*token == 'x') {
                token++;
            }
            errno = 0;
            variable_id = strtol(token, &end, 10);
            if (*token == '\0' || *end != '\0' || errno == ERANGE ||
                variable_id < 0 || variable_id > INT_MAX) {
                invalid_model = 1;
                break;
            }
            if (variable_id > 0 && variable_id <= required_count) {
                int index = (int)variable_id;
                char* destination = index <= readdata.lfsrnet.num
                    ? &clasp.sol[SOL_SEED][index - 1]
                    : &clasp.sol[SOL_TP][index - readdata.lfsrnet.num - 1];
                if (seen[index] && *destination != value) {
                    invalid_model = 1;
                }
                if (!seen[index]) {
                    seen[index] = 1;
                    assigned_count++;
                }
                *destination = value;
            }
            token = strtok_s(NULL, " \t\r\n", &context);
        }
    }
    if (invalid_model || assigned_count != required_count) {
        clasp.status = CLASP_STATUS_ERROR;
    }
    free(seen);
    free(line);
}

bool CLASP(void)
{
    int attempt_count = opt.regen ? CLASP_RETRY_COUNT : 1;
    char log_path[1024];
    if (output_path(log_path, sizeof(log_path), "clasp_log_mttg.txt") != SUCCESS) {
        return CLASP_ERROR;
    }
    for (int attempt = 0; attempt < attempt_count; attempt++) {
        FILE* log = NULL;
        if (clasp_call(attempt) != SUCCESS) {
            return CLASP_ERROR;
        }
        if (fopen_s(&log, log_path, "r") != 0 || log == NULL) {
            printf("[ERROR] Cannot read the clasp log %s.\n", log_path);
            return CLASP_ERROR;
        }
        CLASPgetStatus(log);
        if (clasp.status == CLASP_STATUS_SAT) {
            CLASPgetSol(log);
        }
        fclose(log);
        if (clasp.status == CLASP_STATUS_ERROR) {
            printf("[ERROR] The clasp output %s could not be interpreted (no result line, or an incomplete model).\n", log_path);
            return CLASP_ERROR;
        }
        if (clasp.status != CLASP_STATUS_UNKNOWN) {
            break;		//解が見つかった、または解なしと確定した
        }
    }
    return CLASP_OKAY;
}

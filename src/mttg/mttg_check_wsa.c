//------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../header/Define.h"

int mttg_check_wsa(int threshold)
{
    char command[4096];
    char path[1024];
    FILE* result = NULL;
    int accepted;
    sprintf_s(command, sizeof(command),
        "WSA.exe -sc \"%s\" -tptx \"%stp.txt\" -mode 1 -rate %d -high \"%shigh_mttg.txt\" -low \"%slow_mttg.txt\" -rep \"%s%s_wsa_rep.txt\"",
        fi[CIRCUIT].name, folder_out, threshold, folder_out, folder_out, folder_out, module_name);
    if (system(command) != 0) {
        return ERROR;
    }
    sprintf_s(path, sizeof(path), "%slow_mttg.txt", folder_out);
    if (fopen_s(&result, path, "r") != 0) {
        return ERROR;
    }
    /* Only presence is needed; do not read n_net bytes into a fixed-size buffer. */
    accepted = fgetc(result) != EOF;
    fclose(result);
    return accepted ? SAT : UNSAT;
}

//------------------------------------------------------------------------
//  生成したテストパターンのキャプチャ時消費電力(WSA)の確認
//------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mttg_check_wsa.h"
#include "../header/Define.h"
#include "../main/external_tools.h"
#include "../else/OUTPUT/report.h"

WsaResult mttg_check_wsa(int threshold)
{
    char command[4096];
    char tp_path[1024], high_path[1024], low_path[1024], report_path[1024], report_name[512];
    FILE* low = NULL;
    int exit_code, within;

    snprintf(report_name, sizeof(report_name), "%s_wsa_rep.txt", module_name);
    if (output_path(tp_path, sizeof(tp_path), FILE_TEST_PATTERN) != SUCCESS ||
        output_path(high_path, sizeof(high_path), "high_mttg.txt") != SUCCESS ||
        output_path(low_path, sizeof(low_path), "low_mttg.txt") != SUCCESS ||
        output_path(report_path, sizeof(report_path), report_name) != SUCCESS) {
        return WSA_TOOL_ERROR;
    }

    snprintf(command, sizeof(command),
        TOOL_WSA " -sc \"%s\" -tptx \"%s\" -mode 1 -rate %d -high \"%s\" -low \"%s\" -rep \"%s\"",
        opt.circuit, tp_path, threshold, high_path, low_path, report_path);
    exit_code = run_external(command);
    if (exit_code != 0) {
        printf("[ERROR] %s failed (exit code %d).\n        Command: %s\n", TOOL_WSA, exit_code, command);
        return WSA_TOOL_ERROR;
    }

    if (fopen_s(&low, low_path, "r") != 0 || low == NULL) {
        printf("[ERROR] %s did not write %s.\n", TOOL_WSA, low_path);
        return WSA_TOOL_ERROR;
    }
    /* 閾値以下のテストパターンが書き出されていれば OK (中身は読まず、有無だけを見る) */
    within = fgetc(low) != EOF;
    fclose(low);

    if (!within) {
        printf("[ERROR] %s judged the solver's test pattern to exceed the WSA threshold %d%%.\n"
               "        The WSA constraint of the PBO model and %s disagree. See %s and %s.\n",
               TOOL_WSA, threshold, TOOL_WSA, report_path, high_path);
        return WSA_EXCEEDS_THRESHOLD;
    }
    return WSA_WITHIN_THRESHOLD;
}

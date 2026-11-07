//------------------------------------------------------------------------
//  故障ファイル(-fault)の読み込み: 遷移故障の未検出故障リストを作る
//  1行に「信号線名 STR|STF」(sa0 / sa1 の表記も可)
//------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../header/Define.h"
#include "fault_list.h"

static int compare_net_name(const void* left, const void* right)
{
    return strcmp((*(NLIST* const*)left)->name, (*(NLIST* const*)right)->name);
}

//===================================================
//関数名:parse_fault_type
//機能:故障タイプの表記(STR/sa0, STF/sa1)を STR / STF にする
//戻り値：int SUCCESS / ERROR
//===================================================
static int parse_fault_type(const char* text, int* type)
{
    if (strcmp(text, "STR") == 0 || strcmp(text, "sa0") == 0) { *type = STR; return SUCCESS; }
    if (strcmp(text, "STF") == 0 || strcmp(text, "sa1") == 0) { *type = STF; return SUCCESS; }
    return ERROR;
}

//===================================================
//関数名:transition_delay_fault_set
//機能:故障ファイルを読み、未検出故障リスト ufl を作る
//戻り値：int 正常終了:SUCCESS , 異常終了:ERROR(原因をメッセージに出す)
//===================================================
int transition_delay_fault_set(void)
{
    FILE* input = NULL;
    char line[1024];
    int line_number = 0;
    int fault_count = 0;
    UNDET_FAULT_LIST* head = NULL;
    NLIST** sorted_nets;
    unsigned char* seen;              /* 信号線ごと・故障タイプごとの登録済みフラグ(重複の検出) */
    const char* reason = NULL;

    if (fopen_s(&input, opt.fault_file, "r") != 0 || input == NULL) {
        printf("[ERROR] Cannot open the fault file: %s\n", opt.fault_file);
        return ERROR;
    }

    sorted_nets = (NLIST**)malloc(sizeof(NLIST*) * n_net);
    seen = (unsigned char*)calloc((size_t)n_net * 2, 1);
    if (sorted_nets == NULL || seen == NULL) {
        printf("[ERROR] Out of memory (fault file).\n");
        free(sorted_nets);
        free(seen);
        fclose(input);
        return ERROR;
    }
    for (int i = 0; i < n_net; i++) sorted_nets[i] = &nl[i];
    qsort(sorted_nets, n_net, sizeof(NLIST*), compare_net_name);

    while (fgets(line, sizeof(line), input) != NULL) {
        char name[256];
        char type_name[16];
        char extra[2];
        int type;
        NLIST key = { 0 };
        NLIST* key_pointer = &key;
        NLIST** found;
        UNDET_FAULT_LIST* fault;

        line_number++;
        if (strspn(line, " \t\r\n") == strlen(line)) {
            continue;
        }
        if (sscanf_s(line, "%255s %15s %1s", name, (unsigned)sizeof(name),
            type_name, (unsigned)sizeof(type_name), extra, (unsigned)sizeof(extra)) != 2) {
            reason = "expected \"<signal> <STR|STF>\"";
            break;
        }
        if (parse_fault_type(type_name, &type) != SUCCESS) {
            reason = "unknown fault type (STR or STF expected)";
            break;
        }
        key.name = name;
        found = (NLIST**)bsearch(&key_pointer, sorted_nets, n_net, sizeof(NLIST*), compare_net_name);
        if (found == NULL) {
            reason = "the signal is not in the netlist";
            break;
        }
        if (seen[(size_t)((*found) - nl) * 2 + type]) {
            reason = "the same fault is listed twice";
            break;
        }
        seen[(size_t)((*found) - nl) * 2 + type] = 1;

        fault = (UNDET_FAULT_LIST*)calloc(1, sizeof(*fault));
        if (fault == NULL) {
            reason = "out of memory";
            break;
        }
        strcpy_s(fault->name, sizeof(fault->name), name);
        fault->type = type;
        fault->netlist = *found;
        fault->node_id = fault_count++;
        fault->next = head;
        head = fault;
    }
    fclose(input);
    free(sorted_nets);
    free(seen);

    if (reason != NULL) {
        printf("[ERROR] %s line %d: %s.\n", opt.fault_file, line_number, reason);
        fault_list_free(head);
        return ERROR;
    }
    if (fault_count == 0) {
        printf("[ERROR] %s has no fault.\n", opt.fault_file);
        return ERROR;
    }
    ufl = head;
    ufl_num = fault_count;
    return SUCCESS;
}

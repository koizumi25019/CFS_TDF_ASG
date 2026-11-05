//------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../header/Define.h"

static NLIST* find_net(const char* name)
{
    for (int index = 0; index < n_net; index++) {
        if (strcmp(sort_nl[index]->name, name) == 0) {
            return sort_nl[index];
        }
    }
    return NULL;
}

int transition_delay_fault_set(void)
{
    FILE* input = NULL;
    char line[1024];
    int line_number = 0;
    UNDET_FAULT_LIST* head = NULL;
    int fault_count = 0;
    if (fopen_s(&input, fi[FAULT].name, "r") != 0) {
        return ERROR;
    }
    while (fgets(line, sizeof(line), input) != NULL) {
        char name[256];
        char type_name[16];
        char extra[2];
        char key[280];
        int type;
        NLIST* net;
        UNDET_FAULT_LIST* fault;
        line_number++;
        if (strspn(line, " \t\r\n") == strlen(line)) {
            continue;
        }
        if (sscanf_s(line, "%255s %15s %1s", name, (unsigned)sizeof(name),
            type_name, (unsigned)sizeof(type_name), extra, (unsigned)sizeof(extra)) != 2) {
            goto invalid_input;
        }
        if (strcmp(type_name, "STR") == 0 || strcmp(type_name, "sa0") == 0) {
            type = STR;
        } else if (strcmp(type_name, "STF") == 0 || strcmp(type_name, "sa1") == 0) {
            type = STF;
        } else {
            goto invalid_input;
        }
        net = find_net(name);
        if (net == NULL) {
            goto invalid_input;
        }
        for (fault = head; fault != NULL; fault = fault->next) {
            if (fault->type == type && fault->netlist == net) {
                goto invalid_input;
            }
        }
        fault = (UNDET_FAULT_LIST*)allocMemory(1, sizeof(*fault));
        strcpy_s(fault->name, sizeof(fault->name), name);
        fault->type = type;
        fault->netlist = net;
        fault->node_id = fault_count++;
        sprintf_s(key, sizeof(key), "%s\t%s\n", name, type == STR ? "sa0" : "sa1");
        fault->string = _strdup(key);
        fault->next = head;
        head = fault;
    }
    fclose(input);
    ufl = head;
    afl = head;
    ufl_num = fault_count;
    node_num = fault_count;
    return fault_count > 0 ? SUCCESS : ERROR;

invalid_input:
    printf("[ERROR] Invalid or duplicate fault at line %d.\n", line_number);
    fclose(input);
    while (head != NULL) {
        UNDET_FAULT_LIST* next = head->next;
        free(head->string);
        free(head);
        head = next;
    }
    return ERROR;
}

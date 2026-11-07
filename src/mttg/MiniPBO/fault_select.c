//------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../../header/Define.h"
#include "../../asg/read.h"

static UNDET_FAULT_LIST* copy_fault(const UNDET_FAULT_LIST* source)
{
    UNDET_FAULT_LIST* copy;

    if (source == NULL) return NULL;

    copy = (UNDET_FAULT_LIST*)malloc(sizeof(*copy));
    if (copy == NULL) return NULL;

    *copy = *source;
    copy->next = NULL;
    return copy;
}

static void free_fault_copies(UNDET_FAULT_LIST* head)
{
    while (head != NULL) {
        UNDET_FAULT_LIST* next = head->next;
        free(head);
        head = next;
    }
}

/* Make `source` the only target fault (STTG). */
static int select_single_fault(const UNDET_FAULT_LIST* source)
{
    UNDET_FAULT_LIST* selected;

    if (source == NULL) {
        printf("[WARNING] No undetected faults available.\n");
        return ERROR;
    }

    selected = copy_fault(source);
    if (selected == NULL) {
        printf("[ERROR] Memory allocation failed.\n");
        return ERROR;
    }

    selected->netlist->tf_flag = FLAG_UP;
    tfl = selected;
    tfl_num = 1;

    printf("[INFO] Single-fault test generation: %s\n", selected->name);
    return SUCCESS;
}

static void seed_random_once(void)
{
    static int seeded = OFF;

    if (seeded == OFF) {
        srand((unsigned int)time(NULL));
        seeded = ON;
    }
}

static int select_random_faults(int fault_limit)
{
    int i;
    int selected_count;
    UNDET_FAULT_LIST* current = ufl;
    UNDET_FAULT_LIST* head = NULL;
    UNDET_FAULT_LIST** candidates;

    candidates = (UNDET_FAULT_LIST**)malloc(sizeof(*candidates) * ufl_num);
    if (candidates == NULL) {
        printf("[ERROR] Memory allocation failed.\n");
        return ERROR;
    }

    for (i = 0; i < ufl_num; i++) {
        if (current == NULL) {
            printf("[ERROR] Undetected fault count does not match the list.\n");
            free(candidates);
            return ERROR;
        }
        current->netlist->tf_flag = FLAG_DOWN;
        candidates[i] = current;
        current = current->next;
    }

    seed_random_once();
    for (i = ufl_num - 1; i > 0; i--) {
        int random_index = rand() % (i + 1);
        UNDET_FAULT_LIST* temporary = candidates[i];
        candidates[i] = candidates[random_index];
        candidates[random_index] = temporary;
    }

    selected_count = (fault_limit <= 0 || fault_limit > ufl_num)
        ? ufl_num
        : fault_limit;

    for (i = 0; i < selected_count; i++) {
        UNDET_FAULT_LIST* copy = copy_fault(candidates[i]);
        if (copy == NULL) {
            int reset_index;
            printf("[ERROR] Memory allocation failed.\n");
            for (reset_index = 0; reset_index < i; reset_index++) {
                candidates[reset_index]->netlist->tf_flag = FLAG_DOWN;
            }
            free_fault_copies(head);
            free(candidates);
            return ERROR;
        }

        candidates[i]->netlist->tf_flag = FLAG_UP;
        copy->next = head;
        head = copy;
    }

    free(candidates);
    tfl = head;
    tfl_num = selected_count;
    return SUCCESS;
}

// Retry a solver-aborted primary fault alone (-sttg_wsath).
int fault_select_single(const UNDET_FAULT_LIST* primary)
{
    return select_single_fault(primary);
}

// Select target faults for multi-target or single-target test generation.
// With sttg_flag == ON the first undetected fault becomes the only target (-cps retry).
int fault_select(int sttg_flag, SORTED* sorted, int loop)
{
    int result;
    int fault_limit = fi[TARGET_LIMIT].value;

    if (sttg_flag == ON) {
        return select_single_fault(ufl);
    }

    if (ufl == NULL || ufl_num <= 0) {
        printf("[WARNING] No undetected faults available.\n");
        return ERROR;
    }

    READER_NECESSARY_ON
    {
        if (loop == 0) {
            COMPinti();
            CreateCompatibleInfo();
            if (CreateNecessaryNet() != true) return ERROR;
            if (CreateCompatibleEdge() != true) return ERROR;
        }
        if (CompatibleFaultSet(sorted, loop) != true) return ERROR;
        result = SUCCESS;
    }
    else
    {
        result = select_random_faults(fault_limit);
    }

    if (result == SUCCESS) {
        printf("[INFO] Selected %d target faults.\n", tfl_num);
    }
    return result;
}

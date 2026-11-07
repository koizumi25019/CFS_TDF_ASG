#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <crtdbg.h>
#include "../src/header/Define.h"
#include "../src/asg/opb/clasp/clasp.h"
void test_input_readers(void);
void test_phase_constraints(void);
void test_gate_simulation(void);
static void report_invalid_parameter(const wchar_t* expression, const wchar_t* function,
    const wchar_t* file, unsigned int line, uintptr_t reserved)
{
    (void)reserved;
    fwprintf(stderr, L"Invalid parameter: %ls, function=%ls, file=%ls:%u\n",
        expression, function, file, line);
    _exit(2);
}

static void check_model(const char* text, int status, const char* seed, const char* pattern)
{
    FILE* log = tmpfile();
    assert(log != NULL);
    fputs(text, log);
    CLASPgetObjVal(log);
    if (clasp.status == CLASP_STATUS_OPTIMAL) {
        CLASPgetSol(log);
    }
    assert(clasp.status == status);
    if (seed != NULL) {
        assert(strcmp(clasp.sol[SOL_SEED], seed) == 0);
        assert(strcmp(clasp.sol[SOL_TP], pattern) == 0);
    }
    fclose(log);
}

static void free_targets(void)
{
    while (tfl != NULL) {
        UNDET_FAULT_LIST* next = tfl->next;
        free(tfl);
        tfl = next;
    }
}

int main(void)
{
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
    _set_error_mode(_OUT_TO_STDERR);
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    _set_invalid_parameter_handler(report_invalid_parameter);
    char seed[3] = "00", pattern[3] = "00";
    char* solutions[] = {seed, pattern};
    const int count = 320;
    UNDET_FAULT_LIST* faults = calloc(count, sizeof(*faults));
    NLIST* nets = calloc(count, sizeof(*nets));
    BIT_INT active;
    SORTED sorted = {0};
    clasp.sol = solutions;
    readdata.lfsrnet.num = n_pi = 2;
    n_dff = 0;
    check_model("c Answer: 1\nv x1 -x2 x3 -x4\no 1\nc Answer: 2\nv -x4 x2\nv -x1 x3\ns OPTIMUM FOUND\n", CLASP_STATUS_OPTIMAL, "01", "10");
    check_model("v 4 -2 1 -3 0\ns SATISFIABLE\n", CLASP_STATUS_OPTIMAL, "10", "01");
    check_model("v x1 x2 x3\ns SATISFIABLE\n", CLASP_STATUS_ERROR, NULL, NULL);
    check_model("v x1 -x1 x2 x3 x4\ns SATISFIABLE\n", CLASP_STATUS_ERROR, NULL, NULL);
    check_model("s UNSATISFIABLE\n", CLASP_UNSAT, NULL, NULL);
    check_model("s UNKNOWN\n", CLASP_STATUS_UNKNOWN, NULL, NULL);
    check_model("c crashed\n", CLASP_STATUS_ERROR, NULL, NULL);
    assert(faults && nets);
    ufl_num = all_num = count;
    ufl = faults;
    active.int_num = count / 32 + 1;
    active.flag = calloc(active.int_num, sizeof(unsigned));
    detflag = &active;
    for (int i = 0; i < count; i++) {
        UNDET_FAULT_LIST* f = &faults[i];
        f->node_id = count - i - 1;
        f->netlist = &nets[i];
        f->next = i + 1 < count ? &faults[i + 1] : NULL;
        f->necenet = calloc(1, sizeof(*f->necenet));
        f->necenet->int_num = 1;
        f->necenet->x_buf = calloc(1, sizeof(unsigned));
        f->necenet->p_buf = calloc(1, sizeof(unsigned));
        All_Bit_X_XP(f->necenet);
        if (i % 2) {
            Set_NINT_Zero_XP(f->necenet, 0);
        } else {
            Set_NINT_One_XP(f->necenet, 0);
        }
        f->edge = calloc(1, sizeof(*f->edge));
        f->edge->int_num = active.int_num;
        f->edge->flag = calloc(active.int_num, sizeof(unsigned));
        for (int id = 0; id < count; id++) {
            bitintSetNbit_One(f->edge, id);
        }
        f->n_edge = count;
        bitintSetNbit_One(&active, f->node_id);
    }
    assert(CreateCompatibleEdge());
    for (int i = 0; i < count; i++) {
        for (int j = 0; j < count; j++) {
            assert(bitintGetNbit(faults[i].edge, faults[j].node_id) == (i % 2 == j % 2));
        }
    }
    fi[CPS_MODE].name = fi[FP_MODE].name = "OFF";
    fi[TARGET_LIMIT].value = 17;
    assert(CompatibleFaultSet(&sorted, 0) && tfl_num == 17);
    for (UNDET_FAULT_LIST* a = tfl; a != NULL; a = a->next) {
        for (UNDET_FAULT_LIST* b = tfl; b != NULL; b = b->next) {
            assert(bitintGetNbit(a->edge, b->node_id));
        }
    }
    free_targets();
    for (int i = 0; i < count; i++) {
        for (int id = 0; id < count; id++) {
            bitintSetNbit_One(faults[i].edge, id);
        }
    }
    fi[TARGET_LIMIT].value = 0;
    assert(CompatibleFaultSet(&sorted, 1) && tfl_num == count);
    free_targets();
    for (int i = 0; i < count; i++) {
        free(faults[i].edge->flag);
        free(faults[i].edge);
        free(faults[i].necenet->x_buf);
        free(faults[i].necenet->p_buf);
        free(faults[i].necenet);
    }
    free(active.flag);
    free(faults);
    free(nets);
    puts("PASS: 7 solver cases; 102400 adjacency checks; target limit; complete graph.");
    test_input_readers();
    test_phase_constraints();
    test_gate_simulation();
    return 0;
}

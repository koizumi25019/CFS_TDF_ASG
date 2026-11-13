#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <crtdbg.h>
#include "../src/header/Define.h"
#include "../src/asg/opb/clasp/clasp.h"
#include "../src/asg/essential.h"
#include "../src/asg/screen.h"
#include "../src/asg/compat.h"
#include "../src/mttg/pbo_generate.h"
#include "../src/else/fault_list.h"
void test_input_readers(void);
void test_phase_constraints(void);
void test_essential_gates(void);
static void report_invalid_parameter(const wchar_t* expression, const wchar_t* function,
    const wchar_t* file, unsigned int line, uintptr_t reserved)
{
    (void)reserved;
    fwprintf(stderr, L"Invalid parameter: %ls, function=%ls, file=%ls:%u\n",
        expression, function, file, line);
    _exit(2);
}

//------------------------------------------------------------------------
//  clasp の出力の読み取り
//------------------------------------------------------------------------
static void check_model(const char* text, int status, const char* seed, const char* pattern)
{
    FILE* log = tmpfile();
    assert(log != NULL);
    fputs(text, log);
    CLASPgetStatus(log);
    if (clasp.status == CLASP_STATUS_SAT) {
        CLASPgetSol(log);
    }
    assert(clasp.status == status);
    if (seed != NULL) {
        assert(strcmp(clasp.sol[SOL_SEED], seed) == 0);
        assert(strcmp(clasp.sol[SOL_TP], pattern) == 0);
    }
    fclose(log);
}

static void test_solver_output(void)
{
    char seed[8] = "00", pattern[8] = "00";
    char* solutions[] = {seed, pattern};
    clasp.sol = solutions;
    readdata.lfsrnet.num = n_pi = 2;
    n_dff = 0;
    check_model("c Answer: 1\nv x1 -x2 x3 -x4\no 1\nc Answer: 2\nv -x4 x2\nv -x1 x3\ns OPTIMUM FOUND\n", CLASP_STATUS_SAT, "01", "10");
    check_model("v 4 -2 1 -3 0\ns SATISFIABLE\n", CLASP_STATUS_SAT, "10", "01");
    check_model("v x1 x2 x3\ns SATISFIABLE\n", CLASP_STATUS_ERROR, NULL, NULL);
    check_model("v x1 -x1 x2 x3 x4\ns SATISFIABLE\n", CLASP_STATUS_ERROR, NULL, NULL);
    check_model("s UNSATISFIABLE\n", CLASP_STATUS_UNSAT, NULL, NULL);
    check_model("s UNKNOWN\n", CLASP_STATUS_UNKNOWN, NULL, NULL);
    check_model("c crashed\n", CLASP_STATUS_ERROR, NULL, NULL);
    /* -lfsr OFF: no seed variables, the model holds the test pattern only */
    readdata.lfsrnet.num = 0;
    n_pi = 4;
    seed[0] = '\0';
    pattern[4] = '\0';
    check_model("v 1 -2 3 -4\ns SATISFIABLE\n", CLASP_STATUS_SAT, "", "1010");
    clasp.sol = NULL;
    n_pi = 0;
    readdata.lfsrnet.num = 0;
    puts("PASS: 8 solver output cases (status, model by variable id, incomplete / contradictory models, no-LFSR model).");
}

//------------------------------------------------------------------------
//  s27 を全数探索して、必須割当てと両立可能故障グラフを照合する
//------------------------------------------------------------------------
#define S27_PATTERNS 128						/* 外部入力4 + FF3 = 7ビット */

static int evaluate_gate(const NLIST* gate, const signed char* values)
{
    int result;
    #define INPUT_VALUE(k) values[gate->in[k] - nl]
    switch (gate->type) {
    case INV: return !INPUT_VALUE(0);
    case BUF: case FOUT: return INPUT_VALUE(0);
    case EXOR: return INPUT_VALUE(0) ^ INPUT_VALUE(1);
    case AND: case NAND: result = 1; for (int i = 0; i < gate->n_in; i++) result &= INPUT_VALUE(i); return gate->type == AND ? result : !result;
    case OR: case NOR: result = 0; for (int i = 0; i < gate->n_in; i++) result |= INPUT_VALUE(i); return gate->type == OR ? result : !result;
    }
    return 0;
    #undef INPUT_VALUE
}

/* 2時刻目の回路を、fault_net が stuck に固定されたものとして計算する(fault_net < 0 なら良回路) */
static void simulate_frame2(const signed char* frame2_sources, signed char* out, int fault_net, int stuck)
{
    memcpy(out, frame2_sources, n_net);
    if (fault_net >= 0) out[fault_net] = (signed char)stuck;
    for (int k = 0; k < n_net; k++) {
        int index = (int)(sort_nl[k] - nl);
        if (nl[index].type == IN || nl[index].type == DFF) continue;
        if (index == fault_net) { out[index] = (signed char)stuck; continue; }
        out[index] = (signed char)evaluate_gate(&nl[index], out);
    }
}

static void simulate_pattern(int pattern, signed char* frame1, signed char* frame2)
{
    signed char sources2[4096] = {0};
    for (int i = 0; i < n_pi; i++) frame1[pi[i] - nl] = (pattern >> i) & 1;
    for (int i = 0; i < n_ppi; i++) frame1[ppi[i] - nl] = (pattern >> (n_pi + i)) & 1;
    for (int k = 0; k < n_net; k++) {
        int index = (int)(sort_nl[k] - nl);
        if (nl[index].type == IN || nl[index].type == DFF) continue;
        frame1[index] = (signed char)evaluate_gate(&nl[index], frame1);
    }
    for (int i = 0; i < n_pi; i++) sources2[pi[i] - nl] = frame1[pi[i] - nl];
    for (int i = 0; i < n_ppi; i++) sources2[ppi[i] - nl] = frame1[ppi[i]->in[0] - nl];
    simulate_frame2(sources2, frame2, -1, 0);
}

/* パターンが遷移故障(site, type)を検出するか。観測は PPO のみ(LoC) */
static int detects(int pattern, int site, int type, const signed char* frame1, const signed char* frame2)
{
    signed char faulty[4096];
    signed char sources2[4096] = {0};
    int good_value = type == STR;
    (void)pattern;
    if (frame1[site] != !good_value || frame2[site] != good_value) return 0;
    for (int i = 0; i < n_pi; i++) sources2[pi[i] - nl] = frame1[pi[i] - nl];
    for (int i = 0; i < n_ppi; i++) sources2[ppi[i] - nl] = frame1[ppi[i]->in[0] - nl];
    simulate_frame2(sources2, faulty, site, !good_value);
    for (int i = 0; i < n_net; i++) {
        if (nl[i].ppo_flag == 1 && faulty[i] != frame2[i]) return 1;
    }
    return 0;
}

static void write_all_faults(const char* path)
{
    FILE* file = NULL;
    assert(fopen_s(&file, path, "w") == 0);
    for (int i = 0; i < n_net; i++) fprintf(file, "%s STR\n%s STF\n", nl[i].name, nl[i].name);
    fclose(file);
}

static int conflicts(const EssentialResult* a, const EssentialResult* b)
{
    for (int i = 0; i < a->count; i++)
        for (int j = 0; j < b->count; j++)
            if (a->net[i] == b->net[j] && a->frame[i] == b->frame[j] && a->value[i] != b->value[j]) return 1;
    return 0;
}

static void test_s27_essential_and_compat(void)
{
    const char* fault_path = "audit_build/s27_all_faults.txt";
    EssentialOptions options = essential_default_options();
    signed char frame1[S27_PATTERNS][4096], frame2[S27_PATTERNS][4096];
    int detectable_faults = 0, proven_untestable = 0, assignment_checks = 0;
    int fault_count = 0;

    opt.circuit = "input/circuit/s27.v";
    opt.fault_file = (char*)fault_path;
    opt.target_limit = 0;
    read_nl(opt.circuit);
    assert(validate_circuit_gates() == SUCCESS);
    make_ppo_ppi();
    Level.max_num = n_net;
    Level.ptr = 0;
    Level.stk = (NLIST**)calloc(n_net, sizeof(NLIST*));
    assert(levelize() == SUCCESS);
    write_all_faults(fault_path);
    assert(transition_delay_fault_set() == SUCCESS);
    all_num = ufl_num;
    fault_flags_init();
    assert(essential_open());
    assert(n_pi + n_ppi == 7);

    for (int pattern = 0; pattern < S27_PATTERNS; pattern++) simulate_pattern(pattern, frame1[pattern], frame2[pattern]);

    for (UNDET_FAULT_LIST* fault = ufl; fault != NULL; fault = fault->next) {
        EssentialResult result;
        int site = (int)(fault->netlist - nl);
        int detecting = 0;

        fault_count++;
        assert(essential_compute(fault->netlist, fault->type, &options, &result));
        for (int pattern = 0; pattern < S27_PATTERNS; pattern++) {
            if (!detects(pattern, site, fault->type, frame1[pattern], frame2[pattern])) continue;
            detecting++;
            /* 検出するテストでは、必須割当ては全て成り立つ */
            assert(result.status == ESSENTIAL_OK);
            for (int k = 0; k < result.count; k++) {
                const signed char* values = result.frame[k] ? frame2[pattern] : frame1[pattern];
                assert(values[result.net[k]] == result.value[k]);
                assignment_checks++;
            }
        }
        if (detecting > 0) detectable_faults++;
        if (result.status != ESSENTIAL_OK) {
            assert(detecting == 0);			/* 冗長と判定した故障には、検出するテストが無い */
            proven_untestable++;
        }
    }
    assert(fault_count == 2 * n_net);
    assert(detectable_faults == 16);
    assert(proven_untestable >= 28);		/* 全数探索で検出できない32故障のうち、必須割当てだけで分かるもの */
    assert(assignment_checks > 500);

    /* 両立可能故障グラフ: 辺 = 必須割当てが衝突しない故障の組 */
    assert(screen_untestable_faults() == SUCCESS);
    assert(untest_num == proven_untestable && ufl_num == 2 * n_net - proven_untestable);
    assert(compat_prepare());
    {
        UNDET_FAULT_LIST* remaining[64];
        int remaining_count = 0;
        for (UNDET_FAULT_LIST* fault = ufl; fault != NULL; fault = fault->next) remaining[remaining_count++] = fault;
        assert(remaining_count == ufl_num);
        for (int i = 0; i < remaining_count; i++) {
            for (int j = 0; j < remaining_count; j++) {
                EssentialResult a, b;
                int expected;
                if (i == j) continue;
                assert(essential_compute(remaining[i]->netlist, remaining[i]->type, &options, &a));
                {   /* a の配列は次の呼び出しで書き換わるので複製する */
                    int net[512]; unsigned char frame[512], value[512];
                    EssentialResult copy = a;
                    memcpy(net, a.net, sizeof(int) * a.count); memcpy(frame, a.frame, a.count); memcpy(value, a.value, a.count);
                    assert(essential_compute(remaining[j]->netlist, remaining[j]->type, &options, &b));
                    copy.net = net; copy.frame = frame; copy.value = value;
                    expected = !conflicts(&copy, &b);
                }
                assert(bitintGetNbit(remaining[i]->edge, remaining[j]->node_id) == expected);
            }
        }
        /* 選んだ故障は互いに両立可能で、選ばれなかった故障は、選んだ故障のどれかと衝突する(極大) */
        assert(compat_select() && tfl_num >= 1);
        for (UNDET_FAULT_LIST* a = tfl; a != NULL; a = a->next)
            for (UNDET_FAULT_LIST* b = tfl; b != NULL; b = b->next)
                if (a != b) assert(bitintGetNbit(a->edge, b->node_id));
        for (int i = 0; i < remaining_count; i++) {
            int selected = 0, compatible_with_all = 1;
            for (UNDET_FAULT_LIST* t = tfl; t != NULL; t = t->next) {
                if (t->node_id == remaining[i]->node_id) selected = 1;
                else if (!bitintGetNbit(remaining[i]->edge, t->node_id)) compatible_with_all = 0;
            }
            assert(selected || !compatible_with_all);
        }
        fault_list_free(tfl);
        tfl = NULL;
        tfl_num = 0;
        opt.target_limit = 2;
        assert(compat_select() && tfl_num == 2);
        fault_list_free(tfl);
        tfl = NULL;
        tfl_num = 0;
    }
    /* 範囲を絞った正常回路: 故障箇所と観測点を含み、入力側へ閉じている。回路の一部だけになる故障がある */
    {
        unsigned char need1[4096], need2[4096];
        int smaller = 0;
        Que.max = n_net + 1;
        Que.front = Que.rear = Que.num = 0;
        Que.que = (NLIST**)calloc(Que.max, sizeof(NLIST*));
        for (UNDET_FAULT_LIST* fault = ufl; fault != NULL; fault = fault->next) {
            int site = (int)(fault->netlist - nl), needed = 0;
            memset(need1, 0, sizeof(need1));
            memset(need2, 0, sizeof(need2));
            bool_id = VAR_FIRST_FAULT_CIRCUIT;
            build_fault_cone(fault->netlist, 0);
            pbo_scope_add_target(fault, 1, need1, need2);
            pbo_scope_close(need1, need2);
            assert(need1[site] && need2[site]);
            for (int i = 0; i < n_net; i++) {
                if (nl[i].fault_pass == YES && nl[i].ppo_flag == 1) assert(need2[i]);
                if (nl[i].type == IN && need2[i]) assert(need1[i]);
                if (nl[i].type == DFF && need2[i]) assert(need1[nl[i].in[0] - nl]);
                if (nl[i].type != IN && nl[i].type != DFF) {
                    for (int j = 0; j < nl[i].n_in; j++) {
                        if (need2[i]) assert(need2[nl[i].in[j] - nl]);
                        if (need1[i]) assert(need1[nl[i].in[j] - nl]);
                    }
                }
                needed += need1[i] + need2[i];
            }
            if (needed < 2 * n_net) smaller++;
        }
        assert(smaller > 0);
        free(Que.que);
    }
    essential_close();
    puts("PASS: s27 exhaustive check: necessary assignments hold in every detecting test, redundant faults have none, compatibility graph and target sets.");
}

int main(void)
{
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
    _set_error_mode(_OUT_TO_STDERR);
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    _set_invalid_parameter_handler(report_invalid_parameter);
    test_solver_output();
    test_input_readers();
    test_phase_constraints();
    test_essential_gates();
    test_s27_essential_and_compat();
    return 0;
}

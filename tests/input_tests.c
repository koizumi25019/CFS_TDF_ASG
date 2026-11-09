#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/header/Define.h"
#include "../src/asg/opb/opb.h"
#include "../src/asg/essential.h"

static void write_fixture(const char* path, const char* text)
{
    FILE* file = NULL;
    assert(fopen_s(&file, path, "w") == 0);
    fputs(text, file);
    fclose(file);
}

static void release_channels(void)
{
    for (int index = 0; index < readdata.psnet.num; index++) {
        PS_NLIST* channel = &readdata.psnet.nlist[index];
        free(channel->name);
        free(channel->in);
        free(channel->out_schain);
        free(channel->cons);
    }
    free(readdata.psnet.nlist);
    readdata.psnet.nlist = NULL;
    readdata.psnet.num = 0;
}

void test_input_readers(void)
{
    const char* bad_taps[] = {
        "0:_0\n", "0:_101\n", "0:_-1\n", "0:_1_1\n", "0:_1junk\n",
        "0:\n", "9:_1\n", "-1:_1\n", "0:_1\n0:_2\n", "0 _1\n"
    };
    const char* bad_scans[] = {
        "missing\n", "G0,G0\n", "G0\n", "G0,G1,G0\n", "G0,internal\n"
    };
    NLIST nets[4] = {0};
    nl = nets;
    n_net = 4;
    n_pi = n_ppi = 1;
    nets[0].name = "G0"; nets[0].type = IN; nets[0].n_out = 1;
    nets[1].name = "G1"; nets[1].type = DFF; nets[1].n_out = 1;
    nets[2].name = "output"; nets[2].type = INV;
    nets[3].name = "internal"; nets[3].type = INV; nets[3].n_out = 1;
    readdata.lfsrnet.num = 100;
    opt.xor_tap_file = "audit_build/test_taps.txt";
    opt.scan_chain_file = "audit_build/test_scan.txt";
    for (int index = 0; index < sizeof(bad_taps) / sizeof(*bad_taps); index++) {
        printf("[TEST] Invalid tap fixture %d.\n", index);
        fflush(stdout);
        write_fixture(opt.xor_tap_file, bad_taps[index]);
        assert(ReadXORtap() == READ_ERROR);
        release_channels();
    }
    for (int index = 0; index < sizeof(bad_scans) / sizeof(*bad_scans); index++) {
        write_fixture(opt.xor_tap_file, "0:_1_2\n1:_3\n");
        assert(ReadXORtap());
        write_fixture(opt.scan_chain_file, bad_scans[index]);
        assert(ReadScanChain() == READ_ERROR);
        release_channels();
    }
    write_fixture(opt.xor_tap_file, "\n0:_1_2\n\t\n1:_3\n");
    assert(ReadXORtap());
    assert(readdata.psnet.nlist[0].nin == 2);
    write_fixture(opt.scan_chain_file, "G0, output\n\nG1\n");
    assert(ReadScanChain());
    assert(readdata.psnet.nlist[0].nout_slength == 2);
    assert(readdata.psnet.nlist[1].nout_slength == 1);
    release_channels();
    nl = NULL;
    puts("PASS: 10 invalid tap files, 5 invalid scan files, blank lines and ragged scan rows.");
}

static int constraints_satisfied(const char* constraints, unsigned int assignment)
{
    int sum = 0;
    const char* cursor = constraints;
    while (*cursor != '\0') {
        if (*cursor == 'x') {
            unsigned int variable_id = (unsigned int)strtoul(cursor + 1, NULL, 10);
            int value = (assignment >> (variable_id - 1)) & 1u;
            sum += cursor[-1] == '~' ? !value : value;
        } else if (*cursor == ';') {
            if (sum < 1) {
                return 0;
            }
            sum = 0;
        }
        cursor++;
    }
    return 1;
}

void test_phase_constraints(void)
{
    BIT_INT registers[3];
    unsigned int words[3][6] = {{1}, {2}, {4}};
    int taps[] = {1, 2, 3};
    NLIST destination = {0};
    NLIST* scan[] = {&destination};
    PS_NLIST channel = {0};
    char* text = calloc(MAXSIZE_CONS, 1);
    assert(text != NULL);
    destination.type = IN;
    destination.varsgc = 4;
    channel.in = taps;
    channel.out_schain = scan;
    channel.nout_slength = 1;
    channel.cons = text;
    readdata.psnet.num = 1;
    readdata.psnet.nlist = &channel;
    readdata.lfsrnet.num = 3;
    for (int index = 0; index < 3; index++) {
        registers[index].int_num = 6;
        registers[index].flag = words[index];
    }
    for (int width = 1; width <= 3; width++) {
        channel.nin = width;
        text[0] = '\0';
        assert(CreateConsPhaseShifter(registers, 0));
        for (unsigned int assignment = 0; assignment < 16; assignment++) {
            int parity = 0;
            for (int input = 0; input < width; input++) {
                parity ^= (assignment >> input) & 1u;
            }
            assert(constraints_satisfied(text, assignment) ==
                (parity == ((assignment >> 3) & 1u)));
        }
    }
    /* Two identical expressions cancel: the output must be zero. */
    words[1][0] = words[0][0];
    channel.nin = 2;
    text[0] = '\0';
    assert(CreateConsPhaseShifter(registers, 0));
    for (unsigned int assignment = 0; assignment < 16; assignment++) {
        assert(constraints_satisfied(text, assignment) == ((assignment & 8u) == 0));
    }
    channel.nout_slength = 0;
    text[0] = '\0';
    assert(CreateConsPhaseShifter(registers, 1));
    assert(text[0] == '\0');
    free(text);
    readdata.psnet.nlist = NULL;
    readdata.psnet.num = 0;
    puts("PASS: phase-shifter parity truth tables (64 assignments), zero parity and short channel.");
}

/* 1つのゲート Z = f(A, B) について、Z が良値のとき必ず成り立つ値を全探索で求め、必須割当て(強い含意)と一致するか調べる */
void test_essential_gates(void)
{
    NLIST gates[3] = {0};
    NLIST* inputs[] = {&gates[0], &gates[1]};
    NLIST* outputs_a[] = {&gates[2]};
    NLIST* outputs_b[] = {&gates[2]};
    const int types[] = {AND, NAND, OR, NOR, INV, BUF, FOUT, EXOR};
    int checked = 0;
    nl = gates;
    n_net = 3;
    n_po = 0;
    gates[0].type = gates[1].type = IN;
    gates[0].name = "A"; gates[1].name = "B"; gates[2].name = "Z";
    gates[0].level = gates[1].level = 1;
    gates[2].level = 2;
    gates[2].ppo_flag = 1;
    gates[0].out = outputs_a; gates[0].n_out = 1;
    gates[2].in = inputs;
    assert(essential_open());
    for (int kind = 0; kind < sizeof(types) / sizeof(*types); kind++) {
        int type = types[kind];
        int single_input = type == INV || type == BUF || type == FOUT;
        gates[2].type = type;
        gates[2].n_in = single_input ? 1 : 2;
        gates[1].out = single_input ? NULL : outputs_b;
        gates[1].n_out = single_input ? 0 : 1;
        assert(validate_circuit_gates() == SUCCESS);
        for (int fault_type = STF; fault_type <= STR; fault_type++) {
            EssentialOptions options = essential_default_options();
            EssentialResult result;
            int good_value = fault_type == STR;
            int forced[3] = {-1, -1, -1};		/* 全ての成立する割当てで同じ値になる信号線 */
            int consistent = 0;
            options.frames = 1;
            for (int assignment = 0; assignment < 4; assignment++) {
                int a = assignment & 1, b = (assignment >> 1) & 1, z;
                switch (type) {
                case AND: z = a & b; break;
                case NAND: z = !(a & b); break;
                case OR: z = a | b; break;
                case NOR: z = !(a | b); break;
                case INV: z = !a; break;
                case EXOR: z = a ^ b; break;
                default: z = a; break;
                }
                if (z != good_value || (single_input && b)) continue;
                for (int net = 0; net < 3; net++) {
                    int value = net == 0 ? a : (net == 1 ? b : z);
                    if (!consistent) forced[net] = value; else if (forced[net] != value) forced[net] = -2;
                }
                consistent = 1;
            }
            assert(essential_compute(&gates[2], fault_type, &options, &result));
            assert(result.status == ESSENTIAL_OK);
            for (int net = 0; net < 3; net++) {
                int found = -1;
                for (int k = 0; k < result.count; k++) if (result.net[k] == net) found = result.value[k];
                if (single_input && net == 1) continue;
                assert(found == (forced[net] == -2 ? -1 : forced[net]));
                checked++;
            }
        }
    }
    essential_close();
    gates[2].type = EXNOR;
    assert(validate_circuit_gates() == ERROR);
    gates[2].type = EXOR;
    gates[2].n_in = 1;
    assert(validate_circuit_gates() == ERROR);
    nl = NULL;
    puts("PASS: implications of 8 gate types match the exhaustive truth tables (both fault types), unsupported type and bad arity.");
}

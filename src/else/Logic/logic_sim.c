//------------------------------------------------------------------------
#include <stdio.h>
#include "../../header/Define.h"

/* Only gate types encoded by the ATPG constraints are accepted. */
int validate_circuit_gates(void)
{
    for (int index = 0; index < n_net; index++) {
        NLIST* gate = &nl[index];
        int valid_inputs = 0;
        switch (gate->type) {
        case IN:
            valid_inputs = gate->n_in == 0;
            break;
        case DFF:
        case BUF:
        case FOUT:
        case INV:
            valid_inputs = gate->n_in == 1;
            break;
        case EXOR:
            valid_inputs = gate->n_in == 2;
            break;
        case AND:
        case NAND:
        case OR:
        case NOR:
            valid_inputs = gate->n_in > 0;
            break;
        default:
            printf("[ERROR] Unsupported gate type %d at %s.\n", gate->type, gate->name);
            return ERROR;
        }
        if (!valid_inputs) {
            printf("[ERROR] Invalid input count %d at %s.\n", gate->n_in, gate->name);
            return ERROR;
        }
    }
    return SUCCESS;
}

int logic_sim_frame(int frame)
{
    if (frame != 1 && frame != 2) {
        return ERROR;
    }
    for (int index = n_pi + n_ppi; index < n_net; index++) {
        NLIST* gate = sort_nl[index];
        int value;
        switch (gate->type) {
        case AND:
            value = and(gate, frame);
            break;
        case NAND:
            value = nand(gate, frame);
            break;
        case OR:
            value = or(gate, frame);
            break;
        case NOR:
            value = nor(gate, frame);
            break;
        case INV:
            value = inv(gate, frame);
            break;
        case BUF:
        case FOUT:
            value = fout(gate, frame);
            break;
        case EXOR:
            value = frame == 1
                ? gate->in[0]->val_1t ^ gate->in[1]->val_1t
                : gate->in[0]->val_2t ^ gate->in[1]->val_2t;
            break;
        case DFF:
            continue;
        default:
            printf("[ERROR] Unsupported gate %s in logic simulation.\n", gate->name);
            return ERROR;
        }
        if (frame == 1) {
            gate->val_1t = value;
        } else {
            gate->val_2t = value;
        }
    }
    return SUCCESS;
}

int logic_sim(void)
{
    if (test_v_read() != SUCCESS || logic_sim_1time() != SUCCESS ||
        ppo_ppi_cat() != SUCCESS || logic_sim_2time() != SUCCESS) {
        printf("[ERROR] Two-frame logic simulation failed.\n");
        return ERROR;
    }
    return SUCCESS;
}

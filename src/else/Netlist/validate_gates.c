//------------------------------------------------------------------------
//インクルード
//------------------------------------------------------------------------
#include <stdio.h>
#include "../../header/Define.h"

//=========================================================================
//  関数名 : validate_circuit_gates
//  機  能 : ネットリストのゲート種別と入力数が、制約生成で扱えるものか検査する
//  戻り値 : SUCCESS / ERROR(未対応のゲート種別，または入力数が不正)
//  引  数 : なし
//=========================================================================
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

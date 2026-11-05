//-------------------------------------------------------------------------------------------------------------
//	include
//-------------------------------------------------------------------------------------------------------------
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "opb.h"
#include "./clasp/clasp.h"
#include "../read.h"
#include "../createSGmodel.h"
#include "../../lib/lib.h"
//#include "debug.h"


//*************************************************************************************************************
//	@name		：　CreateLFSRmodel
//	@function	：	create the lfsr model
//	@return		：	(bool) okay, error 
//*************************************************************************************************************
bool CreateLFSRmodel(
	void
)
{
	BIT_INT* poly = (BIT_INT*)NULL;
	BIT_INT* lfsr = (BIT_INT*)NULL;
	int				xor_flag = 0;

	/** set the polynomial */
	LFSRsetPolynomial(&poly);

	/** set the lfsr */
	LFSRinti(&lfsr);

	/** alloc the memory for phase-shifter constraint */
	for (int i = 0; i < readdata.psnet.num; i++)
		//readdata.psnet.nlist[i].cons:i番目のフェーズシフタの制約式を文字列として格納するポインタ
		readdata.psnet.nlist[i].cons = (char*)allocMemory(MAXSIZE_BUFFER, sizeof(char));

	for (int i = 0; i < readdata.psnet.maxslength; i++)
		//readdata.psnet.maxslength:フェーズシフタの最大スキャン長(最大のシフト回数
	{
		/** simulate the LFSR */
		//iの数がつまりLFSRを回す回数となる
		if (i != 0)
			SimulateLFSR(lfsr, poly, 1);

		/** create the pahase-shifer constraint */
		//ここのiの数がmcycleとなるつまり現在のLFSRを回した回数
		if (CreateConsPhaseShifter(lfsr, i) != LFSR_MODEL_OKAY) return LFSR_MODEL_ERROR;
	}

	/** free the memory */
	free(poly->flag);
	free(poly);
	for (int i = 0; i < readdata.lfsrnet.num; i++) { 
		free(lfsr[i].flag); 
	}
	free(lfsr);
#ifdef NDEBUG
	assert(poly == NULL);
	assert(lfsr == NULL);
#endif

	/** alloc the memory for solution for seed */
	//readdata.lfsrnet.num + 1:シードのビット数に加えて、文字列の終端を表す\0を格納するための余分な1バイトのため+1
	//2次元配列にすることで異なる種類のclaspの応答を管理
	//clasp.sol[0]にはシードに関するclasp応答を、clasp.sol[1]にはテストパターンに対するclasp応答を格納するような感じ
	clasp.sol[SOL_SEED] = (char*)allocMemory(readdata.lfsrnet.num + 1, sizeof(char));
	clasp.sol[SOL_SEED][readdata.lfsrnet.num] = '\0';

#ifdef __DEBUG_OPB_CONS_PS__
	_CALL_DEBUG_OPB_CONS_PS_
#endif // __DEBUG_OPB_CONS_PS__


		printf("	Create the lfsr model completed ... \n");

	return LFSR_MODEL_OKAY;
}

//*************************************************************************************************************
//	@name		：　LFSRsetPolynomial
//	@function	：	set the polynomial
//	@return		：	(void)
//*************************************************************************************************************
void LFSRsetPolynomial(
	BIT_INT** poly				  /**< polynomial */
)
{
	(*poly) = (BIT_INT*)allocMemory(1, sizeof(BIT_INT));

	(*poly)->int_num = N_LFSR;

	(*poly)->flag = (unsigned int*)allocMemory(6, sizeof(unsigned int));

	SET_POLYNOMIAL(readdata.lfsrnet.num, (*poly));

	return;
}

//*************************************************************************************************************
//	@name		：　LFSRinit
//	@function	：	initial the LFSR
//	@return		：	(void)
//*************************************************************************************************************
void LFSRinti(
	BIT_INT** lfsr				  /**< lfsr */
)
{
	*lfsr = (BIT_INT*)allocMemory(readdata.lfsrnet.num, sizeof(BIT_INT));

	for (int i = 0; i < readdata.lfsrnet.num; i++)
	{
		(*lfsr)[i].int_num = (unsigned int)N_LFSR;

		(*lfsr)[i].flag = (unsigned int*)allocMemory((*lfsr)[i].int_num, sizeof(unsigned int));

		/** set the all-bits to zero */
		bitintSetAll_Zero(&(*lfsr)[i]);

		/** set the i-bits to one */
		bitintSetNbit_One(&(*lfsr)[i], (unsigned)(i));
	}

	return;
}

//*************************************************************************************************************
//	@name		：　LFSRsimulate
//	@function	：	simulate the LFSR
//	@return		：	(void) 
//*************************************************************************************************************
void SimulateLFSR(
	BIT_INT* lfsr,				  /**< lfsr */
	BIT_INT* poly,			      /**< polynomial */
	int                   mcycle			  /**< number of cycles */
)
{
	BIT_INT* xor;
	xor = (BIT_INT*)allocMemory(1, sizeof(BIT_INT));
	xor ->int_num = N_LFSR;
	xor ->flag = (unsigned int*)allocMemory(xor ->int_num, sizeof(unsigned int));

	/** set the all-bits to zero */
	bitintSetAll_Zero(xor);

	/** simulate the LFSR */
	for (int cycle = 0; cycle < mcycle; cycle++)
	{
		/** calculate the LSB */
		for (int i = 0; i < readdata.lfsrnet.num; i++)
		{
			if (bitintGetNbit(poly, i) == true)
			{
				for (int j = 0; j < N_LFSR; j++)
					xor ->flag[j] ^= lfsr[i].flag[j];
			}
		}

		/** shift */
		for (int i = readdata.lfsrnet.num - 1; i >= 1; i--)
		{
			for (unsigned int j = 0; j < lfsr[i].int_num; j++)
				lfsr[i].flag[j] = lfsr[i - 1].flag[j];
		}

		/** set the LSB to xor-result */
		for (unsigned int j = 0; j < lfsr[0].int_num; j++)
			lfsr[0].flag[j] = xor ->flag[j];

	}

	free(xor ->flag);
	free(xor);

	return;
}

//*************************************************************************************************************
//	@name		：　CreateConsLFSR_XOR
//	@function	：	create the lfsr constraint for xor
//	@return		：	(bool) okay, error 
//*************************************************************************************************************
static bool append_phase_literal(char* text, size_t* length, const char* format, int variable_id)
{
    int written = _snprintf_s(text + *length, MAXSIZE_CONS - *length,
        _TRUNCATE, format, variable_id);
    if (written < 0) {
        printf("[ERROR] Phase-shifter constraints exceed the text buffer.\n");
        return false;
    }
    *length += (size_t)written;
    return true;
}

bool CreateConsPhaseShifter(BIT_INT* lfsr, int cycle)
{
    for (int channel_index = 0; channel_index < readdata.psnet.num; channel_index++) {
        PS_NLIST* channel = &readdata.psnet.nlist[channel_index];
        NLIST* destination;
        int* inputs = NULL;
        int input_count;
        unsigned int assignment_count;
        size_t length;
        if (cycle >= channel->nout_slength || channel->out_schain[cycle] == NULL) {
            continue;
        }
        destination = channel->out_schain[cycle];
        if (destination->type != IN && destination->type != DFF) {
            continue; /* Terminal-output padding is not a scan input. */
        }
        input_count = PSgetInputBit(channel, lfsr, &inputs);
        if (input_count < 0 || input_count >= 15 || destination->varsgc == 0) {
            free(inputs);
            printf("[ERROR] Unsupported phase-shifter parity width or unassigned scan input.\n");
            return LFSR_MODEL_ERROR;
        }
        assignment_count = 1u << input_count;
        length = strlen(channel->cons);
        /* Each clause excludes an input assignment with the wrong parity output. */
        for (unsigned int assignment = 0; assignment < assignment_count; assignment++) {
            int parity = 0;
            for (int input = 0; input < input_count; input++) {
                int bit = (assignment >> input) & 1u;
                parity ^= bit;
                if (!append_phase_literal(channel->cons, &length,
                    bit ? "+1 ~x%d " : "+1 x%d ", inputs[input])) {
                    free(inputs);
                    return LFSR_MODEL_ERROR;
                }
            }
            if (!append_phase_literal(channel->cons, &length,
                parity ? "+1 x%d >=1;\n" : "+1 ~x%d >=1;\n", destination->varsgc)) {
                free(inputs);
                return LFSR_MODEL_ERROR;
            }
        }
        OPBcalcSize(&opb.total, 0, (int)assignment_count, 0, 0);
        free(inputs);
    }
    return LFSR_MODEL_OKAY;
}

//*************************************************************************************************************
//	@name		：　PSgetInputBitint
//	@function	：	get the phase-shifter input-bitint
//	@return		：	(int) number of phase-phase-shifter input-bitint
//*************************************************************************************************************
int PSgetInputBit(
	PS_NLIST* psnet,			  /**< phase-shifter net */
	BIT_INT* lfsr,				  /**< lfsr */
	int** in				  /**< in */
)
{
	BIT_INT* xor = (BIT_INT*)NULL;
	int			nin = 0;
	int			index = 0;

	/**  */
	xor = (BIT_INT*)allocMemory(1, sizeof(BIT_INT));
	xor ->int_num = N_LFSR;
	xor ->flag = (unsigned int*)allocMemory(xor ->int_num, sizeof(unsigned int));
	bitintSetAll_Zero(xor);

	for (int i = 0; i < psnet->nin; i++)
	{
		for (int j = 0; j < N_LFSR; j++)
		{
			xor ->flag[j] ^= lfsr[psnet->in[i] - 1].flag[j];
			//printf("xor->flag[%d][%d]=%d\n",i, j, xor ->flag[j]);
		}
	}
	//printf("---------------------------------------------------\n");

	for (int j = 0; j < readdata.lfsrnet.num; j++)
	{
		if (bitintGetNbit(xor, (unsigned int)j) == true)
		{
			nin++;
		}
	}

	*in = (int*)allocMemory(nin, sizeof(int));
	for (int i = 0; i < readdata.lfsrnet.num; i++)
	{
		if (bitintGetNbit(xor, (unsigned int)i) == true)
		{
			(*in)[index++] = i + 1;
			//printf("in[%d]=%d\n", index - 1, i + 1);
			//printf("\n");
		}
	}

	free(xor ->flag);
	free(xor);

	return nin;
}










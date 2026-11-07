/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*																											 */
/*	program		:	urashima																				 */
/*	file		:	./src/lib/lib.c																	         */
/*	deginer		:	R.miura																			  		 */
/*	date		:	2022.10.01																  				 */
/*																											 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


//-------------------------------------------------------------------------------------------------------------
//	include
//-------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "lib.h"
#include "../asg/read.h"

static unsigned int MASK_INT[] = {
			0x00000001,						  /**  1 */
			0x00000002,						  /**  2 */
			0x00000004,						  /**  3 */
			0x00000008,						  /**  4 */
			0x00000010,						  /**  5 */
			0x00000020,						  /**  6 */
			0x00000040,						  /**  7 */
			0x00000080,						  /**  8 */
			0x00000100,						  /**  9 */
			0x00000200,						  /** 10 */
			0x00000400,						  /** 11 */
			0x00000800,						  /** 12 */
			0x00001000,						  /** 13 */
			0x00002000,						  /** 14 */
			0x00004000,						  /** 15 */
			0x00008000,						  /** 16 */
			0x00010000,						  /** 17 */
			0x00020000,						  /** 18 */
			0x00040000,						  /** 19 */
			0x00080000,						  /** 20 */
			0x00100000,						  /** 21 */
			0x00200000,						  /** 22 */
			0x00400000,						  /** 23 */
			0x00800000,						  /** 24 */
			0x01000000,						  /** 25 */
			0x02000000,						  /** 26 */
			0x04000000,						  /** 27 */
			0x08000000,						  /** 28 */
			0x10000000,						  /** 29 */
			0x20000000,						  /** 30 */
			0x40000000,						  /** 31 */
			0x80000000						  /** 32 */
};



//*************************************************************************************************************
//	@name		：　fileOpen
//	@function	：	open the file
//	@return		：	(void)
//*************************************************************************************************************
void fileOpen(
	FILE** fileptr,			  /**< pointer to file */
	const char* filename,			  /**< filename */
	const char* mode				  /**< mode */
)
{
	if (fopen_s(fileptr, filename, mode) != 0)
	{
		printf("\n	FILE ERROR: file open error. ");
		printf("%c%s%c cannot open.\n", '"', filename, '"');
		exit(EXIT_FAILURE);
	}

	return;
}

//*************************************************************************************************************
//	@name		：　allocMemory
//	@function	：	allocate the memory
//	@return		：	(void*) pointer to allocated the memory
//*************************************************************************************************************
void* allocMemory(
	size_t			      count,			  /**< number of counts of alloc memory */
	size_t			      size				  /**< typesizeof */
)
{
	void* ptr = (void*)NULL;

#ifdef NDEBUG
	assert(count > 0);
#endif

	if ((ptr = calloc(count, size)) == (void*)NULL)
	{
		printf("\n	MEMORY ERROR: memory allocation could not be secured.\n");

		exit(EXIT_FAILURE);
	}

	return	ptr;
}

//*************************************************************************************************************
//	@name		：　bitintSetAll_Zero
//	@function	：	set the all-bits to zero
//	@return		：	(void)
//*************************************************************************************************************
void bitintSetAll_Zero(
	BIT_INT* bitint			  /** bit int */
)
{
	for (unsigned int i = 0; i < bitint->int_num; i++)
	{
		/** set the all-bits to zero */
		bitint->flag[i] = 0;
	}

	return;
}

//*************************************************************************************************************
//	@name		：　bitintSetNbit_One
//	@function	：	set the n-bits to one
//	@return		：	(void)
//*************************************************************************************************************
void bitintSetNbit_One(
	BIT_INT* bitint,			  /** bit int */
	unsigned int	      nbit				  /** n-bit */
)
{
#ifdef NDEBUG
	assert(0 <= nbit);
#endif

	bitint->flag[nbit / (8 * sizeof(unsigned int))] |= MASK_INT[nbit % (8 * sizeof(unsigned int))];

	return;
}

//*************************************************************************************************************
//	@name		：　bitintSetNbit_Zero
//	@function	：	set the n-bits to zero
//	@return		：	(void)
//*************************************************************************************************************
void bitintSetNbit_Zero(
	BIT_INT* bitint,			  /** bit int */
	unsigned int	      nbit				  /** n-bit */
)
{
#ifdef NDEBUG
	assert(0 <= nbit);
#endif

	bitint->flag[nbit / (8 * sizeof(unsigned int))] &= ~MASK_INT[nbit % (8 * sizeof(unsigned int))];

	return;
}

//*************************************************************************************************************
//	@name		：　bitintGetNbit
//	@function	：	get the n-bit
//	@return		：	(void)
//*************************************************************************************************************
unsigned int bitintGetNbit(
	BIT_INT* bitint,			  /** bit int */
	unsigned int	      nbit				  /** n-bit */
)
{
#ifdef NDEBUG
	assert(0 <= nbit);
#endif

	if ((nbit % (8 * sizeof(unsigned int))) == 0)
	{
		if (nbit == false)
		{
			if ((bitint->flag[0] & MASK_INT[0]) == false)	return false;
			else if ((bitint->flag[0] & MASK_INT[0]) != false)  return true;
			else
			{
				printf("\n	SYSTEM ERROR: get the n-bit error.\n");

				exit(EXIT_FAILURE);
			}
		}
		else
		{
			if ((bitint->flag[nbit / (8 * sizeof(unsigned int))] & MASK_INT[0]) == false)	return false;
			else if ((bitint->flag[nbit / (8 * sizeof(unsigned int))] & MASK_INT[0]) != false)  return true;
			else
			{
				printf("\n	SYSTEM ERROR: get the n-bit error.\n");

				exit(EXIT_FAILURE);
			}
		}
	}
	else
	{
		if ((bitint->flag[(nbit / (8 * sizeof(unsigned int)))] & MASK_INT[nbit % (8 * sizeof(unsigned int))]) == false) return false;
		else if ((bitint->flag[(nbit / (8 * sizeof(unsigned int)))] & MASK_INT[nbit % (8 * sizeof(unsigned int))]) != false) return true;
		else
		{
			printf("\n	SYSTEM ERROR: get the n-bit error.\n");

			exit(EXIT_FAILURE);
		}
	}

}


//------------------------------------------------------------------------
//  関数名 : All_Bit_X_XP
//  機  能 : 全ビットをX(ドントケア)にセット( Xバッファ=0 ,Pバッファ=0 )
//  戻り値 : なし
//  引  数 : ポインタ配列(テストパターン格納先)
//------------------------------------------------------------------------
void All_Bit_X_XP(BIT_INT_XP* all_x)
{
	unsigned int i;

	for (i = 0; i < all_x->int_num; i++) {
		all_x->x_buf[i] = 0xFFFFFFFF;		// x_buf = 1
		all_x->p_buf[i] = 0xFFFFFFFF;		// p_buf = 1
	}

}


//------------------------------------------------------------------------
//  関数名 : Set_NINT_One_XP
//  機  能 : 指定ビット目に1をセット
//  戻り値 : なし
//  引  数 : ポインタ配列, 指定ビット数
//------------------------------------------------------------------------
void Set_NINT_One_XP(BIT_INT_XP* one_set, unsigned int pi_num) {

	one_set->x_buf[pi_num / (8 * sizeof(unsigned int))] |= MASK_INT[pi_num % (8 * sizeof(unsigned int))];	// flag = 1
	one_set->p_buf[pi_num / (8 * sizeof(unsigned int))] &= ~MASK_INT[pi_num % (8 * sizeof(unsigned int))];	// flag = 1
}


//------------------------------------------------------------------------
//  関数名 : Set_NINT_Zero_XP
//  機  能 : 指定ビット目に0をセット
//  戻り値 : なし
//  引  数 : ポインタ配列, 指定ビット数
//------------------------------------------------------------------------
void Set_NINT_Zero_XP(BIT_INT_XP* zero_set, unsigned int pi_num) {

	zero_set->x_buf[(pi_num / (8 * sizeof(unsigned int)))] &= ~MASK_INT[pi_num % (8 * sizeof(unsigned int))];	// flag = 0
	zero_set->p_buf[(pi_num / (8 * sizeof(unsigned int)))] |= MASK_INT[pi_num % (8 * sizeof(unsigned int))];	// flag = 0
}




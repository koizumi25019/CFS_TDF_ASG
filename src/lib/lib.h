/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*																											 */
/*	program		:	urashima																				 */
/*	file		:	./src/lib/lib.h																	         */
/*	deginer		:	R.miura																			  		 */
/*	date		:	2022.10.01																  				 */
/*																											 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#pragma once 
//-------------------------------------------------------------------------------------------------------------
//	include
//-------------------------------------------------------------------------------------------------------------
#include <stdio.h>

#include "../header/Netlist.h"


//-------------------------------------------------------------------------------------------------------------
//	define
//-------------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------------
//	structre
//-------------------------------------------------------------------------------------------------------------

/**  */
typedef struct BitUnsignedInt
{
	unsigned int		  int_num;			  /**< number of ints */
	unsigned int* flag;				  /**< flag */
}
BIT_INT;

typedef struct BitUnsignedIntXP {

	unsigned int int_num;					 /**< number of ints */
	unsigned int* x_buf;					 /**< x-flag */
	unsigned int* p_buf;			   		 /**< p-flag */

}
BIT_INT_XP;

/** 32-bit mask */

//-------------------------------------------------------------------------------------------------------------
//	global variable
//-------------------------------------------------------------------------------------------------------------
extern BIT_INT*				   detflag;		/**< 有効(未検出)な故障のビット列。ビット番号 = 故障番号(fault_id) */

//-------------------------------------------------------------------------------------------------------------
//	prototype declaration
//-------------------------------------------------------------------------------------------------------------
/** open the file */
void fileOpen(
	FILE** fileptr,			  /**< pointer to file */
	const char* filename,			  /**< filename */
	const char* mode				  /**< mode */
);

/** allocate the memory */
void* allocMemory(
	size_t			      count,			  /**< number of counts of alloc memory */
	size_t			      size				  /**< typesizeof */
);

/** calculate the hash value */
//int calcHash(
//	char* buffer			  /** buffer */
//);
//
///** push the netlist in stack */
//void stackPUSH(
//	NLIST* netptr			  /** pointer to netlist */
//);
//
///** pop the netlist from stack */
//NLIST* stackPOP(
//	void
//);
//
///** enqueue the netlist to queue */
//void queENQ(
//	NLIST* netptr,			  /** pointer to netlist */
//	int				      mode				  /** mode */
//);
//
///** dequeue the netlist from queue */
//NLIST* queDEQ(
//	void
//);

/** set the all-bit to one */

/** set the all-bit to zero */
void bitintSetAll_Zero(
	BIT_INT* bitint			  /** bit int */
);

/** set the n-bits to one */
void bitintSetNbit_One(
	BIT_INT* bitint,			  /** bit int */
	unsigned int	      nbit				  /** n-bit */
);

/** set the n-bits to zero */
void bitintSetNbit_Zero(
	BIT_INT* bitint,			  /** bit int */
	unsigned int	      nbit				  /** n-bit */
);

/** get the n-bit */
unsigned int bitintGetNbit(
	BIT_INT* bitint,			  /** bit int */
	unsigned int	      nbit				  /** n-bit */
);

/** determines if single-byte characters are used */



//全ビットをXにセット
void All_Bit_X_XP(BIT_INT_XP*);

//指定ビット目を0にセット
void Set_NINT_Zero_XP(BIT_INT_XP*, unsigned int);

//指定ビット目を1にセット
void Set_NINT_One_XP(BIT_INT_XP*, unsigned int);













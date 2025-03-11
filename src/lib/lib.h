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
#define	MAXSIZE_STACK     100				  /**< maximum size of stack */
#define	MAXSIZE_QUEUE     100				  /**< maximum size of queue */
#define	QUE_MODE_ONE      1 				  /**< mode= "only one netlist" */
#define	QUE_MODE_ALLIN    2 				  /**< mode= "all input netlist" */
#define	QUE_MODE_ALLOUT   3 				  /**< mode= "all output netlist" */
#define POSSIBLE		  1					  /**< flag type = possible  */
#define UNPOSSIBLE		  0					  /**< flag type = unpossible  */


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

//-------------------------------------------------------------------------------------------------------------
//	global variable
//-------------------------------------------------------------------------------------------------------------
BIT_INT*				   detflag;

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
void bitintSetAll_One(
	BIT_INT* bitint			  /** bit int */
);

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
bool isSingleByte(
	int					  c					  /** characters */
);

//全ビットを0にセット
void All_INT_Zero_XP(BIT_INT_XP*);

//全ビットを1にセット
void All_INT_One_XP(BIT_INT_XP*);

//全ビットをXにセット
void All_Bit_X_XP(BIT_INT_XP*);

//指定ビット目を0にセット
void Set_NINT_Zero_XP(BIT_INT_XP*, unsigned int);

//指定ビット目を1にセット
void Set_NINT_One_XP(BIT_INT_XP*, unsigned int);

//指定ビット目をXにセット
void Set_NBit_X_XP(BIT_INT_XP*, unsigned int);

//指定ビット目の値をゲット(0, 1)
unsigned int  Get_NBit_INT_XP(BIT_INT_XP*, unsigned int);

//Xバッファのみの指定ビットの値を得る
unsigned int  Get_NBit_Xbuf(BIT_INT_XP*, unsigned int);

//指定ビットの値を得る(xbuf, pbuf)
unsigned int  Get_NBit_XP(BIT_INT_XP* get_n, unsigned int pi_num);









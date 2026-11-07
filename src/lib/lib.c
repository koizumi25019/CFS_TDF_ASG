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
//	@name		：　calcHash
//	@function	：	calculate the hash value
//	@return		：	(int) hash value
//*************************************************************************************************************
int calcHash(
	char* buffer			  /** buffer */
)
{
	int hashval = 0;

	for (size_t i = 0; i < strlen(buffer) - 1; i++)
	{
		hashval += buffer[i];
	}

#ifdef NDEBUG
	assert(0 <= hashval < MAXSIZE_HASH);
#endif

	return hashval % MAXSIZE_HASH;
}

//*************************************************************************************************************
//	@name		：　stackPUSH
//	@function	：	push the netlist in stack
//	@return		：	(viod)
//*************************************************************************************************************
//void stackPUSH(
//	NLIST* netptr			  /** pointer to netlist */
//)
//{
//	stack.stk[stack.ptr++] = netptr;
//
//#ifdef NDEBUG
//	assert(stack.ptr <= (stack.maxnum));
//#endif
//
//	if (stack.ptr >= stack.maxnum)
//	{
//		printf("\n	SYSTEM ERROR: stack overflow\n");
//
//		exit(EXIT_FAILURE);
//	}
//
//	return;
//}

//*************************************************************************************************************
//	@name		：　stackPOP
//	@function	：	pop the netlist from stack
//	@return		：	(NLIST*) pointer to netlist
//*************************************************************************************************************
//NLIST* stackPOP(
//	void
//)
//{
//#ifdef NDEBUG
//	assert(stack.ptr >= 0);
//#endif
//
//	if (stack.ptr < 0)
//	{
//		printf("\n	SYSTEM ERROR: stack underflow\n");
//
//		exit(EXIT_FAILURE);
//	}
//
//	return	stack.stk[--stack.ptr];
//}

//*************************************************************************************************************
//	@name		：　queENQ
//	@function	：	enqueue the netlist to queue
//	@return		：	(void)
//*************************************************************************************************************
//void queENQ(
//	NLIST* netptr,			  /** pointer to netlist */
//	int				      mode				  /** mode */
//)
//{
//	switch (mode)
//	{
//	case QUE_MODE_ONE:
//		que.que[que.rear++] = netptr;
//		que.num++;
//
//		if (que.maxnum == que.rear) que.rear = 0;
//
//#ifdef NDEBUG
//		assert(que.num < que.maxnum);
//#endif
//		if (que.maxnum < que.num)
//		{
//			printf("\n	SYSTEM ERROR: queue over flow.\n");
//
//			exit(EXIT_FAILURE);
//		}
//
//		break;
//
//	case QUE_MODE_ALLOUT:
//		for (int i = 0; i < netptr->n_out; i++)
//		{
//			que.que[que.rear++] = netptr->out[i];
//			que.num++;
//
//			if (que.maxnum == que.rear) que.rear = 0;
//
//#ifdef NDEBUG
//			assert(que.num < que.maxnum);
//#endif
//			if (que.maxnum < que.num)
//			{
//				printf("\n	SYSTEM ERROR: queue over flow.\n");
//
//				exit(EXIT_FAILURE);
//			}
//		}
//
//		break;
//
//	case QUE_MODE_ALLIN:
//		for (int i = 0; i < netptr->n_in; i++)
//		{
//			que.que[que.rear++] = netptr->in[i];
//			que.num++;
//
//			if (que.maxnum == que.rear) que.rear = 0;
//
//#ifdef NDEBUG
//			assert(que.num < que.maxnum);
//#endif
//			if (que.maxnum < que.num)
//			{
//				printf("\n	SYSTEM ERROR: queue over flow.\n");
//
//				exit(EXIT_FAILURE);
//			}
//		}
//
//		break;
//
//	default:
//		printf("\n	SYSTEM ERROR: stack mode error.\n");
//
//		exit(EXIT_FAILURE);
//
//		break;
//	}
//
//	return;
//}
//
////*************************************************************************************************************
////	@name		：　queDEQ
////	@function	：	dequeue the netlist from queue
////	@return		：	(NLIST*) pointer to nelist
////*************************************************************************************************************
//NLIST* queDEQ(
//	void
//)
//{
//	NLIST* netptr = (NLIST*)NULL;
//
//	netptr = que.que[que.front++];
//	que.num--;
//
//	if (que.maxnum == que.front)
//	{
//		que.front = 0;
//	}
//
//#ifdef NDEBUG
//	assert(que.num >= 0);
//#endif
//
//	if (que.num < 0)
//	{
//		printf("\n	SYSTEM ERROR: queue under flow.\n");
//
//		exit(EXIT_FAILURE);
//	}
//
//	return	netptr;
//}
//
////=========================================================================
////  関数名 : queue_emp
////  機  能 : スタックが空か判定
////  戻り値 : 0（無），1（有）
////  引  数 : なし
////=========================================================================
//int queue_emp() {
//
//	if (que.front == que.rear) {
//		return 0;
//	}
//
//	return 1;
//
//}

//*************************************************************************************************************
//	@name		：　bitintSetAll_One
//	@function	：	set the all-bits to one
//	@return		：	(void)
//*************************************************************************************************************
void bitintSetAll_One(
	BIT_INT* bitint			  /** bit int */
)
{
	for (unsigned int i = 0; i < bitint->int_num; i++)
	{
		/** set the all-bits to one */
		bitint->flag[i] = 0xFFFFFFFF;
	}

	return;
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

//*************************************************************************************************************
//	@name		：　isSingleByte
//	@function	：	determines if single-byte characters are used
//	@return		：	(bool) true, false
//*************************************************************************************************************
bool isSingleByte(
	int					  c					  /** characters */
)
{
	if (c >= -1 && c <= 255)
	{
		return true;
	}

	return false;
}

//------------------------------------------------------------------------
//  関数名 : All_INT_One_XP
//  機  能 : 全ビットを1にセット( flag=1 )
//  戻り値 : なし
//  引  数 : ポインタ配列(フラグ格納先)
//------------------------------------------------------------------------
void All_INT_One_XP(BIT_INT_XP* all_one)
{
	unsigned int i;

	for (i = 0; i < all_one->int_num; i++)
	{
		all_one->x_buf[i] = 0xFFFFFFFF;	// x_buf = 1
		all_one->p_buf[i] = 0;			// p_buf = 0
	}

}


//------------------------------------------------------------------------
//  関数名 : All_INT_Zero_XP
//  機  能 : 全ビットを0にセット( flag=0 )
//  戻り値 : なし
//  引  数 : ポインタ配列(フラグ格納先)
//------------------------------------------------------------------------
void All_INT_Zero_XP(BIT_INT_XP* all_zero)
{
	unsigned int i;

	for (i = 0; i < all_zero->int_num; i++) {
		all_zero->x_buf[i] = 0;				// x_buf = 0
		all_zero->p_buf[i] = 0xFFFFFFFF;		// p_buf = 1
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


//------------------------------------------------------------------------
//  関数名 : Set_NBit_X_XP
//  機  能 : 指定ビット目にXをセット( Xバッファ=1 ,Pバッファ=1 )
//  戻り値 : なし
//  引  数 : ポインタ配列(テストパターン格納先), 指定ビット数
//------------------------------------------------------------------------
void Set_NBit_X_XP(BIT_INT_XP* x_set, unsigned int pi_num)
{

	x_set->x_buf[(pi_num / (8 * sizeof(unsigned int)))] |= MASK_INT[pi_num % (8 * sizeof(unsigned int))];	// x_buf = 1
	x_set->p_buf[(pi_num / (8 * sizeof(unsigned int)))] |= MASK_INT[pi_num % (8 * sizeof(unsigned int))];	// p_buf = 1

}


//------------------------------------------------------------------------
//  関数名 : Get_NBit_INT_XP
//  機  能 : 指定ビットの値を得る( 0 or 1 or X ?)
//  戻り値 : 格納値( 0 or 1 or X )
//  引  数 : ポインタ配列, 指定ビット数
//------------------------------------------------------------------------
unsigned int  Get_NBit_INT_XP(BIT_INT_XP* get_n, unsigned int pi_num) {

	// 指定ビットの探索
	//=======================================================================
	if ((pi_num % (8 * sizeof(unsigned int))) == 0) {
		//=======================================================================
		if (pi_num == 0) {
			// 指定ビットの中身を確認(0, 1)
			if ((get_n->x_buf[0] & MASK_INT[0]) == 0 && (get_n->p_buf[0] & MASK_INT[0]) != 0)	return 0; // (x_buf=0 , p_buf=1)

			else if ((get_n->x_buf[0] & MASK_INT[0]) != 0 && (get_n->p_buf[0] & MASK_INT[0]) == 0) return 1; // (x_buf=1 , p_buf=0)

			else if ((get_n->x_buf[0] & MASK_INT[0]) != 0 && (get_n->p_buf[0] & MASK_INT[0]) != 0)return 3; // (x_buf=1 , p_buf=1)

			else {
				printf("格納 ERROR\n");
				exit(-1);
			}
		}
		//=======================================================================
		else {
			// 指定ビットの中身を確認(0, 1)
			if ((get_n->x_buf[pi_num / (8 * sizeof(unsigned int))] & MASK_INT[0]) == 0 && (get_n->p_buf[pi_num / (8 * sizeof(unsigned int))] & MASK_INT[0]) != 0) return 0; // (x_buf=0 , p_buf=1)

			else if ((get_n->x_buf[pi_num / (8 * sizeof(unsigned int))] & MASK_INT[0]) != 0 && (get_n->p_buf[pi_num / (8 * sizeof(unsigned int))] & MASK_INT[0]) == 0) return 1; // (x_buf=1 , p_buf=0)

			else if ((get_n->x_buf[pi_num / (8 * sizeof(unsigned int))] & MASK_INT[0]) != 0 && (get_n->p_buf[pi_num / (8 * sizeof(unsigned int))] & MASK_INT[0]) != 0) return 3; // (x_buf=1 , p_buf=1)

			else {
				printf("格納 ERROR\n");
				exit(-1);
			}
		}
		//=======================================================================
	}
	//=======================================================================
	else {
		// 指定ビットの中身を確認(0, 1)
		if ((get_n->x_buf[(pi_num / (8 * sizeof(unsigned int)))] & MASK_INT[pi_num % (8 * sizeof(unsigned long long int))]) == 0 && (get_n->p_buf[(pi_num / (8 * sizeof(unsigned int)))] & MASK_INT[pi_num % (8 * sizeof(unsigned int))]) != 0) return 0; // (x_buf=0 , p_buf=1)

		else if ((get_n->x_buf[(pi_num / (8 * sizeof(unsigned int)))] & MASK_INT[pi_num % (8 * sizeof(unsigned long long int))]) != 0 && (get_n->p_buf[(pi_num / (8 * sizeof(unsigned int)))] & MASK_INT[pi_num % (8 * sizeof(unsigned int))]) == 0) return 1; // (x_buf=1 , p_buf=0)

		else if ((get_n->x_buf[(pi_num / (8 * sizeof(unsigned int)))] & MASK_INT[pi_num % (8 * sizeof(unsigned long long int))]) != 0 && (get_n->p_buf[(pi_num / (8 * sizeof(unsigned int)))] & MASK_INT[pi_num % (8 * sizeof(unsigned int))]) != 0) return 3; // (x_buf=1 , p_buf=1)

		else {
			printf("格納 ERROR\n");
			exit(-1);
		}
	}
	//=======================================================================

}


//------------------------------------------------------------------------
//  関数名 : Get_NBit_Xbuf
//  機  能 : Xバッファのみの指定ビットの値を得る( 0 or 1 ?)
//  戻り値 : 格納値( 0 or 1 )
//  引  数 : ポインタ配列(テストパターン格納先), 指定ビット数
//------------------------------------------------------------------------
unsigned int  Get_NBit_Xbuf(BIT_INT_XP* get_n, unsigned int pi_num)
{

	// 指定ビットの探索
	if ((pi_num % (8 * sizeof(unsigned int))) == 0) {
		//====================================================================
		if (pi_num == 0) {
			// 指定ビットの中身を確認(0, 1)
			if ((get_n->x_buf[0] & MASK_INT[0]) == 0) return 0; // (x_buf=0)

			else if ((get_n->x_buf[0] & MASK_INT[0]) != 0) return 1; // (x_buf=1)

			else {
				printf("TP格納 ERROR\n");
				exit(-1);
			}
		}
		//====================================================================
		else {
			// 指定ビットの中身を確認(0, 1)
			if ((get_n->x_buf[pi_num / (8 * sizeof(unsigned int))] & MASK_INT[0]) == 0) return 0; // (x_buf=0)

			else if ((get_n->x_buf[pi_num / (8 * sizeof(unsigned int))] & MASK_INT[0]) != 0) return 1; // (x_buf=1)

			else {
				printf("TP格納 ERROR\n");
				exit(-1);
			}
		}
		//====================================================================
	}
	//====================================================================
	else {
		// 指定ビットの中身を確認(0, 1)
		if ((get_n->x_buf[(pi_num / (8 * sizeof(unsigned int)))] & MASK_INT[pi_num % (8 * sizeof(unsigned int))]) == 0) return 0; // (x_buf=0)

		else if ((get_n->x_buf[(pi_num / (8 * sizeof(unsigned int)))] & MASK_INT[pi_num % (8 * sizeof(unsigned int))]) != 0) return 1; // (x_buf=1)

		else {
			printf("TP格納 ERROR\n");
			exit(-1);
		}
	}
}
//------------------------------------------------------------------------
//  関数名 : Get_NBit_XP
//  機  能 : 指定ビットの値を得る(xbuf,pbuf)
//  戻り値 : 格納値( 0 or 1 or 2 or 3 )
//  引  数 : ポインタ配列, 指定ビット数
//------------------------------------------------------------------------
unsigned int  Get_NBit_XP(BIT_INT_XP* get_n, unsigned int pi_num) {

	// 指定ビットの探索
	//=======================================================================
	if ((pi_num % (8 * sizeof(unsigned int))) == 0) {
		//=======================================================================
		if (pi_num == 0) {
			// 指定ビットの中身を確認(0, 1)
			if ((get_n->x_buf[0] & MASK_INT[0]) == 0 && (get_n->p_buf[0] & MASK_INT[0]) == 0)	return 0; // (x_buf=0 , p_buf=0)

			else if ((get_n->x_buf[0] & MASK_INT[0]) == 0 && (get_n->p_buf[0] & MASK_INT[0]) != 0)	return 1; // (x_buf=0 , p_buf=1)

			else if ((get_n->x_buf[0] & MASK_INT[0]) != 0 && (get_n->p_buf[0] & MASK_INT[0]) == 0) return 2; // (x_buf=1 , p_buf=0)

			else if ((get_n->x_buf[0] & MASK_INT[0]) != 0 && (get_n->p_buf[0] & MASK_INT[0]) != 0)return 3; // (x_buf=1 , p_buf=1)

			else {
				printf("格納 ERROR\n");
				exit(-1);
			}
		}
		//=======================================================================
		else {
			// 指定ビットの中身を確認(0, 1)
			if ((get_n->x_buf[pi_num / (8 * sizeof(unsigned int))] & MASK_INT[0]) == 0 && (get_n->p_buf[pi_num / (8 * sizeof(unsigned int))] & MASK_INT[0]) == 0) return 0; // (x_buf=0 , p_buf=0)

			else if ((get_n->x_buf[pi_num / (8 * sizeof(unsigned int))] & MASK_INT[0]) == 0 && (get_n->p_buf[pi_num / (8 * sizeof(unsigned int))] & MASK_INT[0]) != 0) return 1; // (x_buf=0 , p_buf=1)

			else if ((get_n->x_buf[pi_num / (8 * sizeof(unsigned int))] & MASK_INT[0]) != 0 && (get_n->p_buf[pi_num / (8 * sizeof(unsigned int))] & MASK_INT[0]) == 0) return 2; // (x_buf=1 , p_buf=0)

			else if ((get_n->x_buf[pi_num / (8 * sizeof(unsigned int))] & MASK_INT[0]) != 0 && (get_n->p_buf[pi_num / (8 * sizeof(unsigned int))] & MASK_INT[0]) != 0) return 3; // (x_buf=1 , p_buf=1)

			else {
				printf("格納 ERROR\n");
				exit(-1);
			}
		}
		//=======================================================================
	}
	//=======================================================================
	else {
		// 指定ビットの中身を確認(0, 1)
		if ((get_n->x_buf[(pi_num / (8 * sizeof(unsigned int)))] & MASK_INT[pi_num % (8 * sizeof(unsigned int))]) == 0 && (get_n->p_buf[(pi_num / (8 * sizeof(unsigned int)))] & MASK_INT[pi_num % (8 * sizeof(unsigned int))]) == 0) return 0; // (x_buf=0 , p_buf=0)

		else if ((get_n->x_buf[(pi_num / (8 * sizeof(unsigned int)))] & MASK_INT[pi_num % (8 * sizeof(unsigned int))]) == 0 && (get_n->p_buf[(pi_num / (8 * sizeof(unsigned int)))] & MASK_INT[pi_num % (8 * sizeof(unsigned int))]) != 0) return 1; // (x_buf=0 , p_buf=1)

		else if ((get_n->x_buf[(pi_num / (8 * sizeof(unsigned int)))] & MASK_INT[pi_num % (8 * sizeof(unsigned int))]) != 0 && (get_n->p_buf[(pi_num / (8 * sizeof(unsigned int)))] & MASK_INT[pi_num % (8 * sizeof(unsigned int))]) == 0) return 2; // (x_buf=1 , p_buf=0)

		else if ((get_n->x_buf[(pi_num / (8 * sizeof(unsigned int)))] & MASK_INT[pi_num % (8 * sizeof(unsigned int))]) != 0 && (get_n->p_buf[(pi_num / (8 * sizeof(unsigned int)))] & MASK_INT[pi_num % (8 * sizeof(unsigned int))]) != 0) return 3; // (x_buf=1 , p_buf=1)

		else {
			printf("格納 ERROR\n");
			exit(-1);
		}
	}

}




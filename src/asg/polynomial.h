/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*																											 */
/*	program		:	ASG																						 */
/*	file		:	./src/asg/polynomial.h																     */
/*	deginer		:	R.miura																				  	 */
/*	date		:	2022.10.01																  				 */
/*																											 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#pragma once
//-------------------------------------------------------------------------------------------------------------
//	include
//-------------------------------------------------------------------------------------------------------------
#include "../lib/lib.h"
#include "read.h"


//-------------------------------------------------------------------------------------------------------------
//	define
//-------------------------------------------------------------------------------------------------------------
#define  N_LFSR           6

/** polynomial [3 - 167] */
#define SET_POLYNOMIAL(num, return_polynomial)	do															  \
{																											  \
	switch (num) 																							  \
	{																										  \
		case 3: 																							  \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 6u; 																  \
			break;                                                                                            \
		case 4: 																							  \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0xcu; 																  \
			break;                                                                                            \
		case 5: 																							  \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u;	 															  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0x14u; 															  \
			break;                                                                                            \
		case 6: 																							  \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0x30u; 															  \
			break;                                                                                            \
		case 7: 																							  \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0x60u; 															  \
			break;                                                                                            \
		case 8: 																							  \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0xb8u; 															  \
			break;                                                                                            \
		case 9: 																							  \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0x110u; 															  \
			break;                                                                                            \
		case 10: 																							  \
			return_polynomial->flag[5] = 0x0u;	 															  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0x240u; 															  \
			break;                                                                                            \
		case 11: 																							  \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0x500u; 															  \
			break;                                                                                            \
		case 12: 																							  \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0x829u; 															  \
			break;                                                                                            \
		case 13: 																							  \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0x100du; 															  \
			break;                                                                                            \
		case 14: 																							  \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0x2015u; 															  \
			break;                                                                                            \
		case 15: 																							  \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0x6000u; 															  \
			break;                                                                                            \
		case 16: 																							  \
			return_polynomial->flag[5] = 0x0u;																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0xd008u; 															  \
			break;                                                                                            \
		case 17: 																							  \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0x12000u; 															  \
			break;                                                                                            \
		case 18: 																							  \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0x20400u; 															  \
			break;															                                  \
		case 19: 																							  \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0x40023u; 															  \
			break;                                                                                            \
		case 20: 																							  \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u;	 															  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0x90000u; 															  \
			break;                                                                                            \
		case 21: 																							  \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0x140000u; 														  \
			break;                                                                                            \
		case 22: 																							  \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0x300000u; 														  \
			break;                                                                                            \
		case 23: 																							  \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0x420000u; 														  \
			break;																						      \
		case 24: 																							  \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0xe10000u; 														  \
			break;                                                                                            \
		case 25: 																							  \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0x1200000u; 														  \
			break;                                                                                            \
		case 26: 																							  \
			return_polynomial->flag[5] = 0x0u;																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0x2000023u; 														  \
			break;                                                                                            \
		case 27: 																							  \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0x4000013u; 														  \
			break;                                                                                            \
		case 28: 																							  \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0x9000000u; 														  \
			break;                                                                                            \
		case 29: 																							  \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0x14000000u; 														  \
			break;                                                                                            \
		case 30: 																							  \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0x20000029u; 														  \
			break;                                                                                            \
		case 31:                                                                                              \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0x48000000u; 														  \
			break;                                                                                            \
		case 32:                                                                                              \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x0u; 																  \
			return_polynomial->flag[0] = 0x80200003u; 														  \
			break;                                                                                            \
		case 33:                                                                                              \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x1u; 																  \
			return_polynomial->flag[0] = 0x80000u; 															  \
			break;                                                                                            \
		case 34:                                                                                              \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x2u; 																  \
			return_polynomial->flag[0] = 0x4000003u; 														  \
			break;                                                                                            \
		case 35:                                                                                              \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x5u; 																  \
			return_polynomial->flag[0] = 0x0u; 																  \
			break;                                                                                            \
		case 36:                                                                                              \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x8u; 																  \
			return_polynomial->flag[0] = 0x1000000u; 														  \
			break;                                                                                            \
		case 37:                                                                                              \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x10u; 															  \
			return_polynomial->flag[0] = 0x1fu; 															  \
			break;                                                                                            \
		case 38:                                                                                              \
			return_polynomial->flag[5] = 0x0u; 																  \
			return_polynomial->flag[4] = 0x0u; 																  \
			return_polynomial->flag[3] = 0x0u; 																  \
			return_polynomial->flag[2] = 0x0u; 																  \
			return_polynomial->flag[1] = 0x20u; 															  \
			return_polynomial->flag[0] = 0x31u; 															  \
			break;                                                                                            \
		case 39:                                                                                              \
			return_polynomial->flag[5] = 0x0u;												                  \
			return_polynomial->flag[4] = 0x0u;													              \
			return_polynomial->flag[3] = 0x0u;														          \
			return_polynomial->flag[2] = 0x0u;															      \
			return_polynomial->flag[1] = 0x3000u; 															  \
			return_polynomial->flag[0] = 0x3000000u; 														  \
			break;                                                                                            \
		case 40:                                                                                              \
			return_polynomial->flag[5] = 0x0u;									                              \
			return_polynomial->flag[4] = 0x0u;					                                              \
			return_polynomial->flag[3] = 0x0u;						                                          \
			return_polynomial->flag[2] = 0x0u;							                                      \
			return_polynomial->flag[1] = 0xa0u; 															  \
			return_polynomial->flag[0] = 0x140000u; 														  \
			break;                                                                                            \
		case 41:                                                                                              \
			return_polynomial->flag[5] = 0x0u;					                                              \
			return_polynomial->flag[4] = 0x0u;						                                          \
			return_polynomial->flag[3] = 0x0u;							                                      \
			return_polynomial->flag[2] = 0x0u;								                                  \
			return_polynomial->flag[1] = 0x120u; 															  \
			return_polynomial->flag[0] = 0x0u;										                          \
			break;                                                                                            \
		case 42:                                                                                              \
			return_polynomial->flag[5] = 0x0u;											                      \
			return_polynomial->flag[4] = 0x0u;												                  \
			return_polynomial->flag[3] = 0x0u;													              \
			return_polynomial->flag[2] = 0x0u;														          \
			return_polynomial->flag[1] = 0x300u; 															  \
			return_polynomial->flag[0] = 0xc0000u;															  \
			break;                                                                                            \
		case 43:                                                                                              \
			return_polynomial->flag[5] = 0x0u;						                                          \
			return_polynomial->flag[4] = 0x0u;							                                      \
			return_polynomial->flag[3] = 0x0u;								                                  \
			return_polynomial->flag[2] = 0x0u;									                              \
			return_polynomial->flag[1] = 0x630u; 															  \
			return_polynomial->flag[0] = 0x0u;											                      \
			break;                                                                                            \
		case 44:                                                                                              \
			return_polynomial->flag[5] = 0x0u;												                  \
			return_polynomial->flag[4] = 0x0u;													              \
			return_polynomial->flag[3] = 0x0u;														          \
			return_polynomial->flag[2] = 0x0u;															      \
			return_polynomial->flag[1] = 0xc00u; 															  \
			return_polynomial->flag[0] = 0x30000u; 															  \
			break;                                                                                            \
		case 45:                                                                                              \
			return_polynomial->flag[5] = 0x0u;											                      \
			return_polynomial->flag[4] = 0x0u;												                  \
			return_polynomial->flag[3] = 0x0u;													              \
			return_polynomial->flag[2] = 0x0u;														          \
			return_polynomial->flag[1] = 0x1b00u; 															  \
			return_polynomial->flag[0] = 0x0u;																  \
			break;                                                                                            \
		case 46:                                                                                              \
			return_polynomial->flag[5] = 0x0u;									                              \
			return_polynomial->flag[4] = 0x0u;										                          \
			return_polynomial->flag[3] = 0x0u;											                      \
			return_polynomial->flag[2] = 0x0u;												                  \
			return_polynomial->flag[1] = 0x180030u; 														  \
			return_polynomial->flag[0] = 0x0u;														          \
			break;                                                                                            \
		case 47:                                                                                              \
			return_polynomial->flag[5] = 0x0u;								                                  \
			return_polynomial->flag[4] = 0x0u;									                              \
			return_polynomial->flag[3] = 0x0u;										                          \
			return_polynomial->flag[2] = 0x0u;											                      \
			return_polynomial->flag[1] = 0x4000u; 															  \
			return_polynomial->flag[0] = 0x0u;													              \
			break;																				              \
		case 48:                                                                                              \
			return_polynomial->flag[5] = 0x0u;						                                          \
			return_polynomial->flag[4] = 0x0u;							                                      \
			return_polynomial->flag[3] = 0x0u;								                                  \
			return_polynomial->flag[2] = 0x0u;									                              \
			return_polynomial->flag[1] = 0xc000u; 															  \
			return_polynomial->flag[0] = 0x180000u; 														  \
			break;                                                                                            \
		case 49:                                                                                              \
			return_polynomial->flag[5] = 0x0u;												                  \
			return_polynomial->flag[4] = 0x0u;													              \
			return_polynomial->flag[3] = 0x0u;														          \
			return_polynomial->flag[2] = 0x0u;															      \
			return_polynomial->flag[1] = 0x10080u; 															  \
			return_polynomial->flag[0] = 0x0u;																  \
			break;                                                                                            \
		case 50:                                                                                              \
			return_polynomial->flag[5] = 0x0u;	                                                              \
			return_polynomial->flag[4] = 0x0u;		                                                          \
			return_polynomial->flag[3] = 0x0u;			                                                      \
			return_polynomial->flag[2] = 0x0u;				                                                  \
			return_polynomial->flag[1] = 0x30000u; 															  \
			return_polynomial->flag[0] = 0xc00000u; 														  \
			break;                                                                                            \
		case 51:                                                                                              \
			return_polynomial->flag[5] = 0x0u;							                                      \
			return_polynomial->flag[4] = 0x0u;								                                  \
			return_polynomial->flag[3] = 0x0u;									                              \
			return_polynomial->flag[2] = 0x0u;										                          \
			return_polynomial->flag[1] = 0x6000cu; 															  \
			return_polynomial->flag[0] = 0x0u;												                  \
			break;                                                                                            \
		case 52:                                                                                              \
			return_polynomial->flag[5] = 0x0u;				                                                  \
			return_polynomial->flag[4] = 0x0u;					                                              \
			return_polynomial->flag[3] = 0x0u;						                                          \
			return_polynomial->flag[2] = 0x0u;							                                      \
			return_polynomial->flag[1] = 0x90000u; 															  \
			return_polynomial->flag[0] = 0x0u;									                              \
			break;                                                                                            \
		case 53:                                                                                              \
			return_polynomial->flag[5] = 0x0u;										                          \
			return_polynomial->flag[4] = 0x0u;											                      \
			return_polynomial->flag[3] = 0x0u;												                  \
			return_polynomial->flag[2] = 0x0u;													              \
			return_polynomial->flag[1] = 0x180030u; 													      \
			return_polynomial->flag[0] = 0x0u;															      \
			break;                                                                                            \
		case 54:                                                                                              \
			return_polynomial->flag[5] = 0x0u;				                                                  \
			return_polynomial->flag[4] = 0x0u;					                                              \
			return_polynomial->flag[3] = 0x0u;						                                          \
			return_polynomial->flag[2] = 0x0u;							                                      \
			return_polynomial->flag[1] = 0x300000u; 													      \
			return_polynomial->flag[0] = 0x30000u; 															  \
			break;                                                                                            \
		case 55:                                                                                              \
			return_polynomial->flag[5] = 0x0u;										                          \
			return_polynomial->flag[4] = 0x0u;											                      \
			return_polynomial->flag[3] = 0x0u;												                  \
			return_polynomial->flag[2] = 0x0u;													              \
			return_polynomial->flag[1] = 0x400000u; 													      \
			return_polynomial->flag[0] = 0x40000000u; 														  \
			break;                                                                                            \
		case 56:                                                                                              \
			return_polynomial->flag[5] = 0x0u;									                              \
			return_polynomial->flag[4] = 0x0u;										                          \
			return_polynomial->flag[3] = 0x0u;											                      \
			return_polynomial->flag[2] = 0x0u;												                  \
			return_polynomial->flag[1] = 0xc00006u; 													      \
			return_polynomial->flag[0] = 0x0u;														          \
			break;                                                                                            \
		case 57:                                                                                              \
			return_polynomial->flag[5] = 0x0u;									                              \
			return_polynomial->flag[4] = 0x0u;						                                          \
			return_polynomial->flag[3] = 0x0u;								                                  \
			return_polynomial->flag[2] = 0x0u;							                                      \
			return_polynomial->flag[1] = 0x1020000u; 														  \
			return_polynomial->flag[0] = 0x0u;											                      \
			break;                                                                                            \
		case 58:                                                                                              \
			return_polynomial->flag[5] = 0x0u;																  \
			return_polynomial->flag[4] = 0x0u;					                                              \
			return_polynomial->flag[3] = 0x0u;						                                          \
			return_polynomial->flag[2] = 0x0u;							                                      \
			return_polynomial->flag[1] = 0x2000040u; 														  \
			return_polynomial->flag[0] = 0x0u;									                              \
			break;                                                                                            \
		case 59:                                                                                              \
			return_polynomial->flag[5] = 0x0u;							                                      \
			return_polynomial->flag[4] = 0x0u;								                                  \
			return_polynomial->flag[3] = 0x0u;									                              \
			return_polynomial->flag[2] = 0x0u;										                          \
			return_polynomial->flag[1] = 0x6000030u; 														  \
			return_polynomial->flag[0] = 0x0u;												                  \
			break;                                                                                            \
		case 60:                                                                                              \
			return_polynomial->flag[5] = 0x0u;									                              \
			return_polynomial->flag[4] = 0x0u;										                          \
			return_polynomial->flag[3] = 0x0u;											                      \
			return_polynomial->flag[2] = 0x0u;												                  \
			return_polynomial->flag[1] = 0xc000000u; 														  \
			return_polynomial->flag[0] = 0x0u;														          \
			break;                                                                                            \
		case 61:                                                                                              \
			return_polynomial->flag[5] = 0x0u;										                          \
			return_polynomial->flag[4] = 0x0u;											                      \
			return_polynomial->flag[3] = 0x0u;												                  \
			return_polynomial->flag[2] = 0x0u;													              \
			return_polynomial->flag[1] = 0x18003000u; 														  \
			return_polynomial->flag[0] = 0x0u;															      \
			break;                                                                                            \
		case 62:                                                                                              \
			return_polynomial->flag[5] = 0x0u;										                          \
			return_polynomial->flag[4] = 0x0u;											                      \
			return_polynomial->flag[3] = 0x0u;												                  \
			return_polynomial->flag[2] = 0x0u;													              \
			return_polynomial->flag[1] = 0x30000000u; 														  \
			return_polynomial->flag[0] = 0x30u; 														      \
			break;                                                                                            \
		case 63:                                                                                              \
			return_polynomial->flag[5] = 0x0u;							                                      \
			return_polynomial->flag[4] = 0x0u;								                                  \
			return_polynomial->flag[3] = 0x0u;									                              \
			return_polynomial->flag[2] = 0x0u;										                          \
			return_polynomial->flag[1] = 0x60000000u; 														  \
			return_polynomial->flag[0] = 0x0u;												                  \
			break;																			                  \
		case 64:                                                                                              \
			return_polynomial->flag[5] = 0x0u;																  \
			return_polynomial->flag[4] = 0x0u;									                              \
			return_polynomial->flag[3] = 0x0u;										                          \
			return_polynomial->flag[2] = 0x0u;											                      \
			return_polynomial->flag[1] = 0xd8000000u; 														  \
			return_polynomial->flag[0] = 0x0u;													              \
			break;                                                                                            \
		case 65:                                                                                              \
			return_polynomial->flag[5] = 0x0u;														          \
			return_polynomial->flag[4] = 0x0u;									                              \
			return_polynomial->flag[3] = 0x0u;										                          \
			return_polynomial->flag[2] = 0x0u;											                      \
			return_polynomial->flag[1] = 0x4001u; 															  \
			return_polynomial->flag[0] = 0x0u;													              \
			break;                                                                                            \
		case 66:                                                                                              \
			return_polynomial->flag[5] = 0x0u;											                      \
			return_polynomial->flag[4] = 0x0u;												                  \
			return_polynomial->flag[3] = 0x0u;													              \
			return_polynomial->flag[2] = 0x2u;														          \
			return_polynomial->flag[1] = 0x1800001u; 														  \
			return_polynomial->flag[0] = 0x0u;																  \
			break;                                                                                            \
		case 67:                                                                                              \
			return_polynomial->flag[5] = 0x0u;								                                  \
			return_polynomial->flag[4] = 0x0u;									                              \
			return_polynomial->flag[3] = 0x0u;										                          \
			return_polynomial->flag[2] = 0x6u;											                      \
			return_polynomial->flag[1] = 0x3000000u; 														  \
			return_polynomial->flag[0] = 0x0u;													              \
			break;                                                                                            \
		case 68:                                                                                              \
			return_polynomial->flag[5] = 0x0u;						                                          \
			return_polynomial->flag[4] = 0x0u;							                                      \
			return_polynomial->flag[3] = 0x0u;								                                  \
			return_polynomial->flag[2] = 0x8u;									                              \
			return_polynomial->flag[1] = 0x4000000u; 														  \
			return_polynomial->flag[0] = 0x0u;											                      \
			break;                                                                                            \
		case 69:                                                                                              \
			return_polynomial->flag[5] = 0x0u;				                                                  \
			return_polynomial->flag[4] = 0x0u;					                                              \
			return_polynomial->flag[3] = 0x0u;						                                          \
			return_polynomial->flag[2] = 0x14u; 														      \
			return_polynomial->flag[1] = 0x280u; 															  \
			return_polynomial->flag[0] = 0x0u;									                              \
			break;                                                                                            \
		case 70:                                                                                              \
			return_polynomial->flag[5] = 0x0u;							                                      \
			return_polynomial->flag[4] = 0x0u;								                                  \
			return_polynomial->flag[3] = 0x0u;									                              \
			return_polynomial->flag[2] = 0x30u; 														      \
			return_polynomial->flag[1] = 0x600000u; 													      \
			return_polynomial->flag[0] = 0x0u;												                  \
			break;                                                                                            \
		case 71:                                                                                              \
			return_polynomial->flag[5] = 0x0u;										                          \
			return_polynomial->flag[4] = 0x0u;											                      \
			return_polynomial->flag[3] = 0x0u;												                  \
			return_polynomial->flag[2] = 0x40u; 														      \
			return_polynomial->flag[1] = 0x1u;														          \
			return_polynomial->flag[0] = 0x0u;															      \
			break;                                                                                            \
		case 72:                                                                                              \
			return_polynomial->flag[5] = 0x0u;										                          \
			return_polynomial->flag[4] = 0x0u;											                      \
			return_polynomial->flag[3] = 0x0u;												                  \
			return_polynomial->flag[2] = 0x82u; 														      \
			return_polynomial->flag[1] = 0x0u;														          \
			return_polynomial->flag[0] = 0x1040000u; 														  \
			break;                                                                                            \
		case 73:                                                                                              \
			return_polynomial->flag[5] = 0x0u;						                                          \
			return_polynomial->flag[4] = 0x0u;							                                      \
			return_polynomial->flag[3] = 0x0u;								                                  \
			return_polynomial->flag[2] = 0x100u; 															  \
			return_polynomial->flag[1] = 0x8000u; 															  \
			return_polynomial->flag[0] = 0x0u;											                      \
			break;                                                                                            \
		case 74:                                                                                              \
			return_polynomial->flag[5] = 0x0u;										                          \
			return_polynomial->flag[4] = 0x0u;											                      \
			return_polynomial->flag[3] = 0x0u;												                  \
			return_polynomial->flag[2] = 0x300u; 															  \
			return_polynomial->flag[1] = 0x6000000u; 														  \
			return_polynomial->flag[0] = 0x0u;															      \
			break;                                                                                            \
		case 75:                                                                                              \
			return_polynomial->flag[5] = 0x0u;												                  \
			return_polynomial->flag[4] = 0x0u;													              \
			return_polynomial->flag[3] = 0x0u;														          \
			return_polynomial->flag[2] = 0x600u; 															  \
			return_polynomial->flag[1] = 0x80000001u; 														  \
			return_polynomial->flag[0] = 0x0u;																  \
			break;                                                                                            \
		case 76:                                                                                              \
			return_polynomial->flag[5] = 0x0u;															      \
			return_polynomial->flag[4] = 0x0u;																  \
			return_polynomial->flag[3] = 0x0u;									                              \
			return_polynomial->flag[2] = 0xc00u; 															  \
			return_polynomial->flag[1] = 0x180u; 															  \
			return_polynomial->flag[0] = 0x0u;												                  \
			break;                                                                                            \
		case 77:                                                                                              \
			return_polynomial->flag[5] = 0x0u;								                                  \
			return_polynomial->flag[4] = 0x0u;									                              \
			return_polynomial->flag[3] = 0x0u;										                          \
			return_polynomial->flag[2] = 0x1800u; 															  \
			return_polynomial->flag[1] = 0x6000u; 															  \
			return_polynomial->flag[0] = 0x0u;													              \
			break;                                                                                            \
		case 78:                                                                                              \
			return_polynomial->flag[5] = 0x0u;							                                      \
			return_polynomial->flag[4] = 0x0u;								                                  \
			return_polynomial->flag[3] = 0x0u;									                              \
			return_polynomial->flag[2] = 0x3000u; 															  \
			return_polynomial->flag[1] = 0x6000000u; 														  \
			return_polynomial->flag[0] = 0x0u;												                  \
			break;                                                                                            \
		case 79:                                                                                              \
			return_polynomial->flag[5] = 0x0u;				                                                  \
			return_polynomial->flag[4] = 0x0u;					                                              \
			return_polynomial->flag[3] = 0x0u;						                                          \
			return_polynomial->flag[2] = 0x4020u; 															  \
			return_polynomial->flag[1] = 0x0u;								                                  \
			return_polynomial->flag[0] = 0x0u;									                              \
			break;                                                                                            \
		case 80:                                                                                              \
			return_polynomial->flag[5] = 0x0u;						                                          \
			return_polynomial->flag[4] = 0x0u;							                                      \
			return_polynomial->flag[3] = 0x0u;								                                  \
			return_polynomial->flag[2] = 0xc000u; 															  \
			return_polynomial->flag[1] = 0x600u; 															  \
			return_polynomial->flag[0] = 0x0u;											                      \
			break;                                                                                            \
		case 81:                                                                                              \
			return_polynomial->flag[5] = 0x0u;				                                                  \
			return_polynomial->flag[4] = 0x0u;					                                              \
			return_polynomial->flag[3] = 0x0u;						                                          \
			return_polynomial->flag[2] = 0x11000u; 															  \
			return_polynomial->flag[1] = 0x0u;								                                  \
			return_polynomial->flag[0] = 0x0u;									                              \
			break;                                                                                            \
		case 82:                                                                                              \
			return_polynomial->flag[5] = 0x0u;					                                              \
			return_polynomial->flag[4] = 0x0u;						                                          \
			return_polynomial->flag[3] = 0x0u;							                                      \
			return_polynomial->flag[2] = 0x24000u; 															  \
			return_polynomial->flag[1] = 0x4800u; 															  \
			return_polynomial->flag[0] = 0x0u;										                          \
			break;                                                                                            \
		case 83:                                                                                              \
			return_polynomial->flag[5] = 0x0u;						                                          \
			return_polynomial->flag[4] = 0x0u;							                                      \
			return_polynomial->flag[3] = 0x0u;								                                  \
			return_polynomial->flag[2] = 0x60000u; 															  \
			return_polynomial->flag[1] = 0x30u; 														      \
			return_polynomial->flag[0] = 0x0u;											                      \
			break;                                                                                            \
		case 84:                                                                                              \
			return_polynomial->flag[5] = 0x0u;							                                      \
			return_polynomial->flag[4] = 0x0u;								                                  \
			return_polynomial->flag[3] = 0x0u;									                              \
			return_polynomial->flag[2] = 0x80040u; 															  \
			return_polynomial->flag[1] = 0x0u;											                      \
			return_polynomial->flag[0] = 0x0u;												                  \
			break;                                                                                            \
		case 85:                                                                                              \
			return_polynomial->flag[5] = 0x0u;									                              \
			return_polynomial->flag[4] = 0x0u;										                          \
			return_polynomial->flag[3] = 0x0u;											                      \
			return_polynomial->flag[2] = 0x180000u; 													      \
			return_polynomial->flag[1] = 0x3000000u; 														  \
			return_polynomial->flag[0] = 0x0u;														          \
			break;                                                                                            \
		case 86:                                                                                              \
			return_polynomial->flag[5] = 0x0u;											                      \
			return_polynomial->flag[4] = 0x0u;												                  \
			return_polynomial->flag[3] = 0x0u;													              \
			return_polynomial->flag[2] = 0x300300u; 													      \
			return_polynomial->flag[1] = 0x0u;															      \
			return_polynomial->flag[0] = 0x0u;																  \
			break;                                                                                            \
		case 87:                                                                                              \
			return_polynomial->flag[5] = 0x0u;									                              \
			return_polynomial->flag[4] = 0x0u;										                          \
			return_polynomial->flag[3] = 0x0u;											                      \
			return_polynomial->flag[2] = 0x400200u; 													      \
			return_polynomial->flag[1] = 0x0u;													              \
			return_polynomial->flag[0] = 0x0u;														          \
			break;                                                                                            \
		case 88:                                                                                              \
			return_polynomial->flag[5] = 0x0u;												                  \
			return_polynomial->flag[4] = 0x0u;													              \
			return_polynomial->flag[3] = 0x0u;														          \
			return_polynomial->flag[2] = 0xc00000u; 													      \
			return_polynomial->flag[1] = 0x0u;																  \
			return_polynomial->flag[0] = 0x18000u;															  \
			break;                                                                                            \
		case 89:                                                                                              \
			return_polynomial->flag[5] = 0x0u;													              \
			return_polynomial->flag[4] = 0x0u;														          \
			return_polynomial->flag[3] = 0x0u;															      \
			return_polynomial->flag[2] = 0x1000000u; 														  \
			return_polynomial->flag[1] = 0x40000u; 															  \
			return_polynomial->flag[0] = 0x0u;								                                  \
			break;                                                                                            \
		case 90:                                                                                              \
			return_polynomial->flag[5] = 0x0u;								                                  \
			return_polynomial->flag[4] = 0x0u;									                              \
			return_polynomial->flag[3] = 0x0u;										                          \
			return_polynomial->flag[2] = 0x30000c0u; 														  \
			return_polynomial->flag[1] = 0x0u;												                  \
			return_polynomial->flag[0] = 0x0u;													              \
			break;                                                                                            \
		case 91:                                                                                              \
			return_polynomial->flag[5] = 0x0u;							                                      \
			return_polynomial->flag[4] = 0x0u;								                                  \
			return_polynomial->flag[3] = 0x0u;									                              \
			return_polynomial->flag[2] = 0x6000000u; 														  \
			return_polynomial->flag[1] = 0x0u;											                      \
			return_polynomial->flag[0] = 0xc0u; 														      \
			break;                                                                                            \
		case 92:                                                                                              \
			return_polynomial->flag[5] = 0x0u;									                              \
			return_polynomial->flag[4] = 0x0u;										                          \
			return_polynomial->flag[3] = 0x0u;											                      \
			return_polynomial->flag[2] = 0xc00c000u; 														  \
			return_polynomial->flag[1] = 0x0u;													              \
			return_polynomial->flag[0] = 0x0u;														          \
			break;                                                                                            \
		case 93:                                                                                              \
			return_polynomial->flag[5] = 0x0u;								                                  \
			return_polynomial->flag[4] = 0x0u;									                              \
			return_polynomial->flag[3] = 0x0u;										                          \
			return_polynomial->flag[2] = 0x14000000u; 														  \
			return_polynomial->flag[1] = 0x0u;												                  \
			return_polynomial->flag[0] = 0x0u;													              \
			break;                                                                                            \
		case 94:                                                                                              \
			return_polynomial->flag[5] = 0x0u;				                                                  \
			return_polynomial->flag[4] = 0x0u;					                                              \
			return_polynomial->flag[3] = 0x0u;						                                          \
			return_polynomial->flag[2] = 0x20000100u; 														  \
			return_polynomial->flag[1] = 0x0u;								                                  \
			return_polynomial->flag[0] = 0x0u;									                              \
			break;                                                                                            \
		case 95:                                                                                              \
			return_polynomial->flag[5] = 0x0u;								                                  \
			return_polynomial->flag[4] = 0x0u;									                              \
			return_polynomial->flag[3] = 0x0u;										                          \
			return_polynomial->flag[2] = 0x40080000u; 														  \
			return_polynomial->flag[1] = 0x0u;												                  \
			return_polynomial->flag[0] = 0x0u;													              \
			break;                                                                                            \
		case 96:                                                                                              \
			return_polynomial->flag[5] = 0x0u;							                                      \
			return_polynomial->flag[4] = 0x0u;								                                  \
			return_polynomial->flag[3] = 0x0u;									                              \
			return_polynomial->flag[2] = 0xa0000000u; 														  \
			return_polynomial->flag[1] = 0x14000u; 															  \
			return_polynomial->flag[0] = 0x0u;												                  \
			break;                                                                                            \
		case 97:                                                                                              \
			return_polynomial->flag[5] = 0x0u;									                              \
			return_polynomial->flag[4] = 0x0u;										                          \
			return_polynomial->flag[3] = 0x0u;											                      \
			return_polynomial->flag[2] = 0x4000001u; 														  \
			return_polynomial->flag[1] = 0x0u;													              \
			return_polynomial->flag[0] = 0x0u;														          \
			break;                                                                                            \
		case 98:                                                                                              \
			return_polynomial->flag[5] = 0x0u;							                                      \
			return_polynomial->flag[4] = 0x0u;								                                  \
			return_polynomial->flag[3] = 0x0u;									                              \
			return_polynomial->flag[2] = 0x400002u; 													      \
			return_polynomial->flag[1] = 0x0u;											                      \
			return_polynomial->flag[0] = 0x0u;												                  \
			break;                                                                                            \
		case 99:                                                                                              \
			return_polynomial->flag[5] = 0x0u;						                                          \
			return_polynomial->flag[4] = 0x0u;							                                      \
			return_polynomial->flag[3] = 0x4u;								                                  \
			return_polynomial->flag[2] = 0x1u;									                              \
			return_polynomial->flag[1] = 0x280000u;														      \
			return_polynomial->flag[0] = 0x0u;											                      \
			break;                                                                                            \
		case 100:												                                              \
			return_polynomial->flag[5] = 0x0u;						                                          \
			return_polynomial->flag[4] = 0x0u;							                                      \
			return_polynomial->flag[3] = 0x8u;								                                  \
			return_polynomial->flag[2] = 0x0u;									                              \
			return_polynomial->flag[1] = 0x40000000u; 														  \
			return_polynomial->flag[0] = 0x0u;											                      \
			break;                                                                                            \
		case 101:                                                                                             \
			return_polynomial->flag[5] = 0x0u;						                                          \
			return_polynomial->flag[4] = 0x0u;							                                      \
			return_polynomial->flag[3] = 0x18u;								                                  \
			return_polynomial->flag[2] = 0x60000000u; 														  \
			return_polynomial->flag[1] = 0x0u;										                          \
			return_polynomial->flag[0] = 0x0u;											                      \
			break;                                                                                            \
		case 102:                                                                                             \
			return_polynomial->flag[5] = 0x0u;						                                          \
			return_polynomial->flag[4] = 0x0u;							                                      \
			return_polynomial->flag[3] = 0x30u;								                                  \
			return_polynomial->flag[2] = 0x0u;									                              \
			return_polynomial->flag[1] = 0xcu;										                          \
			return_polynomial->flag[0] = 0x0u;											                      \
			break;                                                                                            \
		case 103:                                                                                             \
			return_polynomial->flag[5] = 0x0u;					                                              \
			return_polynomial->flag[4] = 0x0u;						                                          \
			return_polynomial->flag[3] = 0x40u;							                                      \
			return_polynomial->flag[2] = 0x20000000u;													      \
			return_polynomial->flag[1] = 0x0u;									                              \
			return_polynomial->flag[0] = 0x0u;										                          \
			break;                                                                                            \
		case 104:                                                                                             \
			return_polynomial->flag[5] = 0x0u;						                                          \
			return_polynomial->flag[4] = 0x0u;							                                      \
			return_polynomial->flag[3] = 0xc0u;								                                  \
			return_polynomial->flag[2] = 0x30000000u;													      \
			return_polynomial->flag[1] = 0x0u;										                          \
			return_polynomial->flag[0] = 0x0u;											                      \
			break;                                                                                            \
		case 105:                                                                                             \
			return_polynomial->flag[5] = 0x0u;							                                      \
			return_polynomial->flag[4] = 0x0u;								                                  \
			return_polynomial->flag[3] = 0x100u;									                          \
			return_polynomial->flag[2] = 0x1000000u;												          \
			return_polynomial->flag[1] = 0x0u;											                      \
			return_polynomial->flag[0] = 0x0u;												                  \
			break;                                                                                            \
		case 106:                                                                                             \
			return_polynomial->flag[5] = 0x0u;								                                  \
			return_polynomial->flag[4] = 0x0u;									                              \
			return_polynomial->flag[3] = 0x200u;										                      \
			return_polynomial->flag[2] = 0x4000000u;												          \
			return_polynomial->flag[1] = 0x0u;												                  \
			return_polynomial->flag[0] = 0x0u;													              \
			break;                                                                                            \
		case 107:                                                                                             \
			return_polynomial->flag[5] = 0x0u;				                                                  \
			return_polynomial->flag[4] = 0x0u;					                                              \
			return_polynomial->flag[3] = 0x500u;						                                      \
			return_polynomial->flag[2] = 0x0u;							                                      \
			return_polynomial->flag[1] = 0xa00u;								                              \
			return_polynomial->flag[0] = 0x0u;									                              \
			break;                                                                                            \
		case 108:                                                                                             \
			return_polynomial->flag[5] = 0x0u;						                                          \
			return_polynomial->flag[4] = 0x0u;							                                      \
			return_polynomial->flag[3] = 0x800u;								                              \
			return_polynomial->flag[2] = 0x1000u;								                              \
			return_polynomial->flag[1] = 0x0u;										                          \
			return_polynomial->flag[0] = 0x0u;											                      \
			break;                                                                                            \
		case 109:                                                                                             \
			return_polynomial->flag[5] = 0x0u;							                                      \
			return_polynomial->flag[4] = 0x0u;								                                  \
			return_polynomial->flag[3] = 0x1860u;								                              \
			return_polynomial->flag[2] = 0x0u;										                          \
			return_polynomial->flag[1] = 0x0u;											                      \
			return_polynomial->flag[0] = 0x0u;												                  \
			break;                                                                                            \
		case 110:													                                          \
			return_polynomial->flag[5] = 0x0u;								                                  \
			return_polynomial->flag[4] = 0x0u;																  \
			return_polynomial->flag[3] = 0x3000u;												              \
			return_polynomial->flag[2] = 0x3u;														          \
			return_polynomial->flag[1] = 0x0u;															      \
			return_polynomial->flag[0] = 0x0u;																  \
			break;                                                                                            \
		case 111:                                                                                             \
			return_polynomial->flag[5] = 0x0u;					                                              \
			return_polynomial->flag[4] = 0x0u;						                                          \
			return_polynomial->flag[3] = 0x4010u;						                                      \
			return_polynomial->flag[2] = 0x0u;								                                  \
			return_polynomial->flag[1] = 0x0u;									                              \
			return_polynomial->flag[0] = 0x0u;										                          \
			break;                                                                                            \
		case 112:                                                                                             \
			return_polynomial->flag[5] = 0x0u;				                                                  \
			return_polynomial->flag[4] = 0x0u;					                                              \
			return_polynomial->flag[3] = 0xa000u;					                                          \
			return_polynomial->flag[2] = 0x14u;							                                      \
			return_polynomial->flag[1] = 0x0u;								                                  \
			return_polynomial->flag[0] = 0x0u;									                              \
			break;                                                                                            \
		case 113:                                                                                             \
			return_polynomial->flag[5] = 0x0u;								                                  \
			return_polynomial->flag[4] = 0x0u;									                              \
			return_polynomial->flag[3] = 0x10080u;									                          \
			return_polynomial->flag[2] = 0x0u;											                      \
			return_polynomial->flag[1] = 0x0u;												                  \
			return_polynomial->flag[0] = 0x0u;													              \
			break;                                                                                            \
		case 114:                                                                                             \
			return_polynomial->flag[5] = 0x0u;					                                              \
			return_polynomial->flag[4] = 0x0u;						                                          \
			return_polynomial->flag[3] = 0x30000u;						                                      \
			return_polynomial->flag[2] = 0x0u;								                                  \
			return_polynomial->flag[1] = 0x1u;									                              \
			return_polynomial->flag[0] = 0x80000000u;													      \
			break;                                                                                            \
		case 115:                                                                                             \
			return_polynomial->flag[5] = 0x0u;								                                  \
			return_polynomial->flag[4] = 0x0u;									                              \
			return_polynomial->flag[3] = 0x60018u;									                          \
			return_polynomial->flag[2] = 0x0u;											                      \
			return_polynomial->flag[1] = 0x0u;												                  \
			return_polynomial->flag[0] = 0x0u;													              \
			break;                                                                                            \
		case 116:                                                                                             \
			return_polynomial->flag[5] = 0x0u;				                                                  \
			return_polynomial->flag[4] = 0x0u;					                                              \
			return_polynomial->flag[3] = 0xc0000u;					                                          \
			return_polynomial->flag[2] = 0x0u;							                                      \
			return_polynomial->flag[1] = 0x3000u;							                                  \
			return_polynomial->flag[0] = 0x0u;									                              \
			break;                                                                                            \
		case 117:                                                                                             \
			return_polynomial->flag[5] = 0x0u;						                                          \
			return_polynomial->flag[4] = 0x0u;							                                      \
			return_polynomial->flag[3] = 0x140004u;							                                  \
			return_polynomial->flag[2] = 0x1u;									                              \
			return_polynomial->flag[1] = 0x0u;										                          \
			return_polynomial->flag[0] = 0x0u;											                      \
			break;                                                                                            \
		case 118:                                                                                             \
			return_polynomial->flag[5] = 0x0u;					                                              \
			return_polynomial->flag[4] = 0x0u;						                                          \
			return_polynomial->flag[3] = 0x200000u;						                                      \
			return_polynomial->flag[2] = 0x100000u;							                                  \
			return_polynomial->flag[1] = 0x0u;									                              \
			return_polynomial->flag[0] = 0x0u;										                          \
			break;                                                                                            \
		case 119:                                                                                             \
			return_polynomial->flag[5] = 0x0u;						                                          \
			return_polynomial->flag[4] = 0x0u;							                                      \
			return_polynomial->flag[3] = 0x404000u;							                                  \
			return_polynomial->flag[2] = 0x0u;									                              \
			return_polynomial->flag[1] = 0x0u;										                          \
			return_polynomial->flag[0] = 0x0u;											                      \
			break;                                                                                            \
		case 120:                                                                                             \
			return_polynomial->flag[5] = 0x0u;					                                              \
			return_polynomial->flag[4] = 0x0u;						                                          \
			return_polynomial->flag[3] = 0x810000u;						                                      \
			return_polynomial->flag[2] = 0x0u;								                                  \
			return_polynomial->flag[1] = 0x0u;									                              \
			return_polynomial->flag[0] = 0x102u;										                      \
			break;                                                                                            \
		case 121:                                                                                             \
			return_polynomial->flag[5] = 0x0u;						                                          \
			return_polynomial->flag[4] = 0x0u;							                                      \
			return_polynomial->flag[3] = 0x1000040u;												          \
			return_polynomial->flag[2] = 0x0u;									                              \
			return_polynomial->flag[1] = 0x0u;										                          \
			return_polynomial->flag[0] = 0x0u;											                      \
			break;                                                                                            \
		case 122:                                                                                             \
			return_polynomial->flag[5] = 0x0u;								                                  \
			return_polynomial->flag[4] = 0x0u;									                              \
			return_polynomial->flag[3] = 0x3000000u;												          \
			return_polynomial->flag[2] = 0x0u;											                      \
			return_polynomial->flag[1] = 0x60000000u;													      \
			return_polynomial->flag[0] = 0x0u;													              \
			break;                                                                                            \
		case 123:                                                                                             \
			return_polynomial->flag[5] = 0x0u;							                                      \
			return_polynomial->flag[4] = 0x0u;								                                  \
			return_polynomial->flag[3] = 0x5000000u;												          \
			return_polynomial->flag[2] = 0x0u;										                          \
			return_polynomial->flag[1] = 0x0u;											                      \
			return_polynomial->flag[0] = 0x0u;												                  \
			break;                                                                                            \
		case 124:                                                                                             \
			return_polynomial->flag[5] = 0x0u;													              \
			return_polynomial->flag[4] = 0x0u;														          \
			return_polynomial->flag[3] = 0x8000000u;													      \
			return_polynomial->flag[2] = 0x400000u;															  \
			return_polynomial->flag[1] = 0x0u;									                              \
			return_polynomial->flag[0] = 0x0u;							                                      \
			break;															                                  \
		case 125:                                                                                             \
			return_polynomial->flag[5] = 0x0u;									                              \
			return_polynomial->flag[4] = 0x0u;										                          \
			return_polynomial->flag[3] = 0x18000000u;													      \
			return_polynomial->flag[2] = 0x0u;												                  \
			return_polynomial->flag[1] = 0x0u;													              \
			return_polynomial->flag[0] = 0x30000u;													          \
			break;                                                                                            \
		case 126:                                                                                             \
			return_polynomial->flag[5] = 0x0u;										                          \
			return_polynomial->flag[4] = 0x0u;											                      \
			return_polynomial->flag[3] = 0x30000000u;													      \
			return_polynomial->flag[2] = 0x3000000u;												          \
			return_polynomial->flag[1] = 0x0u;														          \
			return_polynomial->flag[0] = 0x0u;															      \
			break;                                                                                            \
		case 127:                                                                                             \
			return_polynomial->flag[5] = 0x0u;							                                      \
			return_polynomial->flag[4] = 0x0u;								                                  \
			return_polynomial->flag[3] = 0x60000000u;													      \
			return_polynomial->flag[2] = 0x0u;										                          \
			return_polynomial->flag[1] = 0x0u;											                      \
			return_polynomial->flag[0] = 0x0u;												                  \
			break;                                                                                            \
		case 128:                                                                                             \
			return_polynomial->flag[5] = 0x0u;						                                          \
			return_polynomial->flag[4] = 0x0u;							                                      \
			return_polynomial->flag[3] = 0xa0000014u;													      \
			return_polynomial->flag[2] = 0x0u;									                              \
			return_polynomial->flag[1] = 0x0u;										                          \
			return_polynomial->flag[0] = 0x0u;											                      \
			break;                                                                                            \
		case 129:                                                                                             \
			return_polynomial->flag[5] = 0x0u;					                                              \
			return_polynomial->flag[4] = 0x0u;						                                          \
			return_polynomial->flag[3] = 0x8000001u;												          \
			return_polynomial->flag[2] = 0x0u;								                                  \
			return_polynomial->flag[1] = 0x0u;									                              \
			return_polynomial->flag[0] = 0x0u;										                          \
			break;                                                                                            \
		case 130:                                                                                             \
			return_polynomial->flag[5] = 0x0u;				                                                  \
			return_polynomial->flag[4] = 0x0u;					                                              \
			return_polynomial->flag[3] = 0x40000002u;													      \
			return_polynomial->flag[2] = 0x0u;							                                      \
			return_polynomial->flag[1] = 0x0u;								                                  \
			return_polynomial->flag[0] = 0x0u;									                              \
			break;                                                                                            \
		case 131:                                                                                             \
			return_polynomial->flag[5] = 0x0u;										                          \
			return_polynomial->flag[4] = 0x0u;											                      \
			return_polynomial->flag[3] = 0x6u;												                  \
			return_polynomial->flag[2] = 0xc0000u;												              \
			return_polynomial->flag[1] = 0x0u;														          \
			return_polynomial->flag[0] = 0x0u;															      \
			break;                                                                                            \
		case 132:                                                                                             \
			return_polynomial->flag[5] = 0x0u;					                                              \
			return_polynomial->flag[4] = 0x8u;						                                          \
			return_polynomial->flag[3] = 0x40u;							                                      \
			return_polynomial->flag[2] = 0x0u;								                                  \
			return_polynomial->flag[1] = 0x0u;									                              \
			return_polynomial->flag[0] = 0x0u;										                          \
			break;                                                                                            \
		case 133:                                                                                             \
			return_polynomial->flag[5] = 0x0u;				                                                  \
			return_polynomial->flag[4] = 0x18u;					                                              \
			return_polynomial->flag[3] = 0x0u;						                                          \
			return_polynomial->flag[2] = 0x30000u;						                                      \
			return_polynomial->flag[1] = 0x0u;								                                  \
			return_polynomial->flag[0] = 0x0u;									                              \
			break;                                                                                            \
		case 134:                                                                                             \
			return_polynomial->flag[5] = 0x0u;										                          \
			return_polynomial->flag[4] = 0x20u;											                      \
			return_polynomial->flag[3] = 0x0u;												                  \
			return_polynomial->flag[2] = 0x1000u;												              \
			return_polynomial->flag[1] = 0x0u;														          \
			return_polynomial->flag[0] = 0x0u;															      \
			break;                                                                                            \
		case 135:                                                                                             \
			return_polynomial->flag[5] = 0x0u;					                                              \
			return_polynomial->flag[4] = 0x40u;						                                          \
			return_polynomial->flag[3] = 0x8000000u;												          \
			return_polynomial->flag[2] = 0x0u;								                                  \
			return_polynomial->flag[1] = 0x0u;									                              \
			return_polynomial->flag[0] = 0x0u;										                          \
			break;                                                                                            \
		case 136:                                                                                             \
			return_polynomial->flag[5] = 0x0u;												                  \
			return_polynomial->flag[4] = 0xc0u;											                      \
			return_polynomial->flag[3] = 0x0u;													              \
			return_polynomial->flag[2] = 0x0u;														          \
			return_polynomial->flag[1] = 0x0u;															      \
			return_polynomial->flag[0] = 0x600u;															  \
			break;                                                                                            \
		case 137:                                                                                             \
			return_polynomial->flag[5] = 0x0u;				                                                  \
			return_polynomial->flag[4] = 0x100u;					                                          \
			return_polynomial->flag[3] = 0x80000u;					                                          \
			return_polynomial->flag[2] = 0x0u;							                                      \
			return_polynomial->flag[1] = 0x0u;								                                  \
			return_polynomial->flag[0] = 0x0u;									                              \
			break;                                                                                            \
		case 138:                                                                                             \
			return_polynomial->flag[5] = 0x0u;										                          \
			return_polynomial->flag[4] = 0x300u;											                  \
			return_polynomial->flag[4] = 0x300u;												              \
			return_polynomial->flag[3] = 0x6u;													              \
			return_polynomial->flag[2] = 0x0u;														          \
			return_polynomial->flag[1] = 0x0u;															      \
			return_polynomial->flag[0] = 0x0u;																  \
			break;                                                                                            \
		case 139:                                                                                             \
			return_polynomial->flag[5] = 0x0u;					                                              \
			return_polynomial->flag[4] = 0x4a0u;						                                      \
			return_polynomial->flag[3] = 0x4u;							                                      \
			return_polynomial->flag[2] = 0x0u;								                                  \
			return_polynomial->flag[1] = 0x0u;									                              \
			return_polynomial->flag[0] = 0x0u;										                          \
			break;                                                                                            \
		case 140:                                                                                             \
			return_polynomial->flag[5] = 0x0u;											                      \
			return_polynomial->flag[4] = 0x800u;										                      \
			return_polynomial->flag[3] = 0x4000u;												              \
			return_polynomial->flag[2] = 0x0u;														          \
			return_polynomial->flag[1] = 0x0u;															      \
			return_polynomial->flag[0] = 0x0u;																  \
			break;                                                                                            \
		case 141:                                                                                             \
			return_polynomial->flag[5] = 0x0u;								                                  \
			return_polynomial->flag[4] = 0x1800u;								                              \
			return_polynomial->flag[3] = 0x3000u;									                          \
			return_polynomial->flag[2] = 0x0u;											                      \
			return_polynomial->flag[1] = 0x0u;												                  \
			return_polynomial->flag[0] = 0x0u;													              \
			break;                                                                                            \
		case 142:                                                                                             \
			return_polynomial->flag[5] = 0x0u;							                                      \
			return_polynomial->flag[4] = 0x2000u;							                                  \
			return_polynomial->flag[3] = 0x1000000u;													      \
			return_polynomial->flag[2] = 0x0u;										                          \
			return_polynomial->flag[1] = 0x0u;											                      \
			return_polynomial->flag[0] = 0x0u;												                  \
			break;                                                                                            \
		case 143:                                                                                             \
			return_polynomial->flag[5] = 0x0u;						                                          \
			return_polynomial->flag[4] = 0x6000u;						                                      \
			return_polynomial->flag[3] = 0x6000000u;							                              \
			return_polynomial->flag[2] = 0x0u;									                              \
			return_polynomial->flag[1] = 0x0u;										                          \
			return_polynomial->flag[0] = 0x0u;											                      \
			break;                                                                                            \
		case 144:                                                                                             \
			return_polynomial->flag[5] = 0x0u;						                                          \
			return_polynomial->flag[4] = 0xc000u;						                                      \
			return_polynomial->flag[3] = 0x0u;								                                  \
			return_polynomial->flag[2] = 0x600u;									                          \
			return_polynomial->flag[1] = 0x0u;										                          \
			return_polynomial->flag[0] = 0x0u;											                      \
			break;                                                                                            \
		case 145:                                                                                             \
			return_polynomial->flag[5] = 0x0u;						                                          \
			return_polynomial->flag[4] = 0x10000u;						                                      \
			return_polynomial->flag[3] = 0x0u;								                                  \
			return_polynomial->flag[2] = 0x10000000u;													      \
			return_polynomial->flag[1] = 0x0u;										                          \
			return_polynomial->flag[0] = 0x0u;											                      \
			break;                                                                                            \
		case 146:                                                                                             \
			return_polynomial->flag[5] = 0x0u;							                                      \
			return_polynomial->flag[4] = 0x30000u;							                                  \
			return_polynomial->flag[3] = 0x0u;									                              \
			return_polynomial->flag[2] = 0x600000u;									                          \
			return_polynomial->flag[1] = 0x0u;											                      \
			return_polynomial->flag[0] = 0x0u;												                  \
			break;                                                                                            \
		case 147:                                                                                             \
			return_polynomial->flag[5] = 0x0u;								                                  \
			return_polynomial->flag[4] = 0x60000u;								                              \
			return_polynomial->flag[3] = 0x3000u;									                          \
			return_polynomial->flag[2] = 0x0u;											                      \
			return_polynomial->flag[1] = 0x0u;												                  \
			return_polynomial->flag[0] = 0x0u;													              \
			break;                                                                                            \
		case 148:                                                                                             \
			return_polynomial->flag[5] = 0x0u;								                                  \
			return_polynomial->flag[4] = 0x80000u;						                                      \
			return_polynomial->flag[3] = 0x1000000u;												          \
			return_polynomial->flag[2] = 0x0u;										                          \
			return_polynomial->flag[1] = 0x0u;											                      \
			return_polynomial->flag[0] = 0x0u;												                  \
			break;                                                                                            \
		case 149:                                                                                             \
			return_polynomial->flag[5] = 0x0u;							                                      \
			return_polynomial->flag[4] = 0x180000u;							                                  \
			return_polynomial->flag[3] = 0x0u;									                              \
			return_polynomial->flag[2] = 0x0u;										                          \
			return_polynomial->flag[1] = 0xc0u;											                      \
			return_polynomial->flag[0] = 0x0u;												                  \
			break;                                                                                            \
		case 150:                                                                                             \
			return_polynomial->flag[5] = 0x0u;					                                              \
			return_polynomial->flag[4] = 0x200000u;					                                          \
			return_polynomial->flag[3] = 0x0u;							                                      \
			return_polynomial->flag[2] = 0x1u;								                                  \
			return_polynomial->flag[1] = 0x0u;									                              \
			return_polynomial->flag[0] = 0x0u;										                          \
			break;                                                                                            \
		case 151:                                                                                             \
			return_polynomial->flag[5] = 0x0u;								                                  \
			return_polynomial->flag[4] = 0x480000u;								                              \
			return_polynomial->flag[3] = 0x0u;										                          \
			return_polynomial->flag[2] = 0x0u;											                      \
			return_polynomial->flag[1] = 0x0u;												                  \
			return_polynomial->flag[0] = 0x0u;													              \
			break;                                                                                            \
		case 152:                                                                                             \
			return_polynomial->flag[5] = 0x0u;						                                          \
			return_polynomial->flag[4] = 0xc00000u;						                                      \
			return_polynomial->flag[3] = 0x0u;								                                  \
			return_polynomial->flag[2] = 0x600000u;								                              \
			return_polynomial->flag[1] = 0x0u;										                          \
			return_polynomial->flag[0] = 0x0u;											                      \
			break;                                                                                            \
		case 153:                                                                                             \
			return_polynomial->flag[5] = 0x0u;					                                              \
			return_polynomial->flag[4] = 0x1800000u;												          \
			return_polynomial->flag[3] = 0x0u;							                                      \
			return_polynomial->flag[2] = 0x0u;								                                  \
			return_polynomial->flag[1] = 0x0u;									                              \
			return_polynomial->flag[0] = 0x0u;										                          \
			break;                                                                                            \
		case 154:                                                                                             \
			return_polynomial->flag[5] = 0x0u;											                      \
			return_polynomial->flag[4] = 0x2800000u;												          \
			return_polynomial->flag[3] = 0x0u;													              \
			return_polynomial->flag[2] = 0x0u;														          \
			return_polynomial->flag[1] = 0x0u;															      \
			return_polynomial->flag[0] = 0x5000000u;														  \
			break;                                                                                            \
		case 155:                                                                                             \
			return_polynomial->flag[5] = 0x0u;						                                          \
			return_polynomial->flag[4] = 0x6000000u;												          \
			return_polynomial->flag[3] = 0xc000000u;												          \
			return_polynomial->flag[2] = 0x0u;									                              \
			return_polynomial->flag[1] = 0x0u;										                          \
			return_polynomial->flag[0] = 0x0u;											                      \
			break;                                                                                            \
		case 156:                                                                                             \
			return_polynomial->flag[5] = 0x0u;												                  \
			return_polynomial->flag[4] = 0xc000000u;												          \
			return_polynomial->flag[3] = 0x0u;														          \
			return_polynomial->flag[2] = 0x0u;															      \
			return_polynomial->flag[1] = 0x180u;															  \
			return_polynomial->flag[0] = 0x0u;																  \
			break;                                                                                            \
		case 157:                                                                                             \
			return_polynomial->flag[5] = 0x0u;					                                              \
			return_polynomial->flag[4] = 0x18000000u;													      \
			return_polynomial->flag[3] = 0x6u;							                                      \
			return_polynomial->flag[2] = 0x0u;								                                  \
			return_polynomial->flag[1] = 0x0u;									                              \
			return_polynomial->flag[0] = 0x0u;										                          \
			break;                                                                                            \
		case 158:                                                                                             \
			return_polynomial->flag[5] = 0x0u;											                      \
			return_polynomial->flag[4] = 0x30000008u;										                  \
			return_polynomial->flag[3] = 0x4u;													              \
			return_polynomial->flag[2] = 0x0u;														          \
			return_polynomial->flag[1] = 0x0u;															      \
			return_polynomial->flag[0] = 0x0u;																  \
			break;                                                                                            \
		case 159:                                                                                             \
			return_polynomial->flag[5] = 0x0u;							                                      \
			return_polynomial->flag[4] = 0x40000000u;						                                  \
			return_polynomial->flag[3] = 0x80000000u;							                              \
			return_polynomial->flag[2] = 0x0u;										                          \
			return_polynomial->flag[1] = 0x0u;											                      \
			return_polynomial->flag[0] = 0x0u;												                  \
			break;                                                                                            \
		case 160:                                                                                             \
			return_polynomial->flag[5] = 0x0u;							                                      \
			return_polynomial->flag[4] = 0xc0003000u;						                                  \
			return_polynomial->flag[3] = 0x0u;									                              \
			return_polynomial->flag[2] = 0x0u;										                          \
			return_polynomial->flag[1] = 0x0u;											                      \
			return_polynomial->flag[0] = 0x0u;												                  \
			break;                                                                                            \
		case 161:                                                                                             \
			return_polynomial->flag[5] = 0x0u;													              \
			return_polynomial->flag[4] = 0x4001u; 															  \
			return_polynomial->flag[3] = 0x0u;															      \
			return_polynomial->flag[2] = 0x0u;																  \
			return_polynomial->flag[1] = 0x0u;											                      \
			return_polynomial->flag[0] = 0x0u;												                  \
			break;                                                                                            \
		case 162:                                                                                             \
			return_polynomial->flag[5] = 0x0u;							                                      \
			return_polynomial->flag[4] = 0x3u;								                                  \
			return_polynomial->flag[3] = 0x0u;									                              \
			return_polynomial->flag[2] = 0x600u; 															  \
			return_polynomial->flag[1] = 0x0u;											                      \
			return_polynomial->flag[0] = 0x0u;												                  \
			break;                                                                                            \
		case 163:                                                                                             \
			return_polynomial->flag[5] = 0x0u;													              \
			return_polynomial->flag[4] = 0x6u;														          \
			return_polynomial->flag[3] = 0xc0u; 														      \
			return_polynomial->flag[2] = 0x0u;																  \
			return_polynomial->flag[1] = 0x0u;                                                                \
			return_polynomial->flag[0] = 0x0u;												                  \
			break;                                                                                            \
		case 164:                                                                                             \
			return_polynomial->flag[5] = 0x0u;						                                          \
			return_polynomial->flag[4] = 0x60000cu;						                                      \
			return_polynomial->flag[3] = 0x0u;								                                  \
			return_polynomial->flag[2] = 0x0u;									                              \
			return_polynomial->flag[1] = 0x0u;										                          \
			return_polynomial->flag[0] = 0x0u;											                      \
			break;                                                                                            \
		case 165:                                                                                             \
			return_polynomial->flag[5] = 0x10u; 														      \
			return_polynomial->flag[4] = 0x68u; 														      \
			return_polynomial->flag[3] = 0x0u;														          \
			return_polynomial->flag[2] = 0x0u;															      \
			return_polynomial->flag[1] = 0x0u;																  \
			return_polynomial->flag[0] = 0x0u;								                                  \
			break;                                                                                            \
		case 166:                                                                                             \
			return_polynomial->flag[5] = 0x30u; 														      \
			return_polynomial->flag[4] = 0x0u;										                          \
			return_polynomial->flag[3] = 0xc0000000u;													      \
			return_polynomial->flag[2] = 0x0u;												                  \
			return_polynomial->flag[1] = 0x0u;													              \
			return_polynomial->flag[0] = 0x0u;														          \
			break;                                                                                            \
		case 167:                                                                                             \
			return_polynomial->flag[5] = 0x40u; 														      \
			return_polynomial->flag[4] = 0x1u;									                              \
			return_polynomial->flag[3] = 0x0u;										                          \
			return_polynomial->flag[2] = 0x0u;											                      \
			return_polynomial->flag[1] = 0x0u;												                  \
			return_polynomial->flag[0] = 0x0u;													              \
			break;                                                                                            \
		default:                                                                                              \
			return_polynomial->flag[5] = 0xa0u; 														      \
			return_polynomial->flag[4] = 0x1400000u;													      \
			return_polynomial->flag[3] = 0x0u;																  \
			return_polynomial->flag[2] = 0x0u;													              \
			return_polynomial->flag[1] = 0x0u;														          \
			return_polynomial->flag[0] = 0x0u;															      \
			break;                                                                                            \
	}																										  \
}																											  \
while (false);


//-------------------------------------------------------------------------------------------------------------
//	structre
//-------------------------------------------------------------------------------------------------------------
typedef struct Polynomail
{
	unsigned int          flag[N_LFSR];
}
POLYNOMAIL;


//-------------------------------------------------------------------------------------------------------------
//	global variable
//-------------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------------
//	prototype declaration
//-------------------------------------------------------------------------------------------------------------
/** set the polynomial */
void LFSRsetPolynomial(
	BIT_INT** poly				  /**< polynomial */
);

/** initial the LFSR */
void LFSRinti(
	BIT_INT** lfsr				  /**< lfsr */
);

/** simulate the LFSR */
void SimulateLFSR(
	BIT_INT* lfsr,				  /**< lfsr */
	BIT_INT* poly,			      /**< polynomial */
	int                   mcycle			  /**< number of cycles */
);

/** get the phase-shifter input-bitint */
int PSgetInputBit(
	PS_NLIST* psnet,			  /**< phase-shifter net */
	BIT_INT* lfsr,				  /**< lfsr */
	int** in				  /**< in */
);










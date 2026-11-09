/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*																											 */
/*	program		:	ASG																						 */
/*	file		:	./src/asg/init/init.c																     */
/*	deginer		:	R.miura																			  		 */
/*	date		:	2022.10.01																  				 */
/*																											 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


//-------------------------------------------------------------------------------------------------------------
//	include
//-------------------------------------------------------------------------------------------------------------
#include <stdio.h>

#include "init.h"
#include "read.h"
#include "./opb/clasp/clasp.h"
#include "../lib/lib.h"
#include "./opb/opb.h"

//*************************************************************************************************************
//	@name		：　InitGlobalVars
//	@function	：	initialize the global variable
//	@return		：	(bool) okay, error 
//*************************************************************************************************************
bool InitGlobalVars(
	void
)
{
	InitGlobalVarsOPB();

	//InitGlobalVarsREADDATA();

	InitGlobalVarsCLASP();

#ifdef __DEBUG_INIT__
	_CALL_DEBUG_INIT_
#endif


		printf("	Initialize the global variables completed ... \n");
	return INIT_OKAY;
}


//*************************************************************************************************************
//	@name		：　InitGlobalVarsOPB
//	@function	：	initialize the opb
//	@return		：	(void) 
//*************************************************************************************************************
void InitGlobalVarsOPB(
	void
)
{
	
	opb.constant.cons = 0;
	opb.constant.pros = 0;
	opb.constant.spros = 0;
	opb.constant.vars = 0;
	opb.total.cons = 0;
	opb.total.pros = 0;
	opb.total.spros = 0;
	opb.total.vars = 0;
	return;
}

//*************************************************************************************************************
//	@name		：　InitGlobalVarsREADDATA
//	@function	：	initialize the reading data
//	@return		：	(void) 
//*************************************************************************************************************
//void InitGlobalVarsREADDATA(
//	void
//)
//{
//	
//	for (int i = 0; i < MAXSIZE_HASH; i++)
//	{
//		readdata.fault.list[i] = (FNODE*)NULL;
//	}
//	readdata.fault.numdete = 0;
//	readdata.fault.numinit = 0;
//	readdata.fault.numrema = 0;
//	readdata.fault.numred = 0;
//	num_seed = 0;
//	return;
//}

//*************************************************************************************************************
//	@name		：　InitGlobalVarsCLASP
//	@function	：	initialize the scip
//	@return		：	(void) 
//*************************************************************************************************************
#if 1
void InitGlobalVarsCLASP(
	void
)
{
	
	clasp.sol = (char**)NULL;
	clasp.sol = (char**)allocMemory(N_SOL, sizeof(char*));

	/** for test pattern */
	clasp.sol[SOL_TP] = (char*)allocMemory(n_dff + n_pi + 1, sizeof(char));
	clasp.sol[SOL_TP][n_dff + n_pi] = '\0';

	clasp.status = CLASP_STATUS_ERROR;

	return;
}
#endif










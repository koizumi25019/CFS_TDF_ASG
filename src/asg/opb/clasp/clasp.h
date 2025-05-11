#pragma once
//-------------------------------------------------------------------------------------------------------------
//	include
//-------------------------------------------------------------------------------------------------------------
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

//-------------------------------------------------------------------------------------------------------------
//	define
//-------------------------------------------------------------------------------------------------------------
#define N_SOL             2
#define SOL_SEED          0
#define SOL_TP            1

#define CLASP_OKAY		  true
#define CLASP_ERROR		  false
#define CLASP_STATUS_ERROR -1
#define CLASP_OBJVAL_ERROR -1
#define CLASP_UNSAT -2
#define	CLASP_STATUS_UNKNOWN		-8
#define CLASP_STATUS_USERINTERRUPT	2
#define CLASP_STATUS_NODELIMIT		3
#define CLASP_STATUS_TIMELIMIT		4
#define CLASP_STATUS_MEMLIMIT		5
#define CLASP_STATUS_GAPLIMIT		6
#define CLASP_STATUS_OPTIMAL		7	
#define CLASP_STATUS_INFEASIBLE		8
#define CLASP_STATUS_UNBOUNDED		9
#define CLASP_STATUS_TOTALNODELIMIT	10	
#define CLASP_STATUS_STALLNODELIMIT	11
#define CLASP_STATUS_SOLLIMIT		12
#define CLASP_STATUS_BESTSOLLIMIT	13
#define CLASP_STATUS_RESTARTLIMIT	14
#define CLASP_STATUS_INFORUNBD		15


//-------------------------------------------------------------------------------------------------------------
//	structre
//-------------------------------------------------------------------------------------------------------------
/** problem size */
typedef struct CLASP_LOG_
{
	int					  status;			  /**< status */
	int					  objval;			  /**< objective value */
	char** sol;				  /**< solution */
}
CLASPLOG;


//-------------------------------------------------------------------------------------------------------------
//	global variable
//-------------------------------------------------------------------------------------------------------------
CLASPLOG					   clasp;			   /**< clasp log */


//-------------------------------------------------------------------------------------------------------------
//	prototype declaration
//-------------------------------------------------------------------------------------------------------------
/** clasp */
bool CLASP(
	void
);
/** get the objective value */
void CLASPgetObjVal(
	FILE* fileptr			  /**< pointer to file */
);

/** get the solution */
void CLASPgetSol(
	FILE* fileptr			  /**< pointer to file */
);
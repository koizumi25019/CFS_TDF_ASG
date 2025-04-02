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

char cmd[100];

/** call clasp (limit times =120[s]) */
#define	_CALL_CLASP_LIMIT_TIME_120_ do     \
{                                         \
    printf("	resolving time limit 1000...\n");              \
    sprintf(cmd, "clasp.exe %smttg_pbo.txt --time-limit 120 > %sclasp_log_mttg.txt", folder_out,folder_out);          \
    system(cmd);          \
}       \
while (false);

/** call clasp (limit times =1000[s]) */
#define	_CALL_CLASP_LIMIT_TIME_1000_	do\
{																											  \
	printf("	resolving time limit 1000...\n");																		  \																							  \
    sprintf(cmd, "clasp.exe %smttg_pbo.txt --time-limit 1000 > %sclasp_log_mttg.txt", folder_out,folder_out);\
    system(cmd);																																			  \
}																											  \
while (false);

/** call clasp (limit times =3000[s]) */
#define	_CALL_CLASP_LIMIT_TIME_3000_	do																  		  \
{																											  \
	printf("	resolving time limit 3000...\n");														  \																									  \
    sprintf(cmd, "clasp.exe %smttg_pbo.txt --time-limit 3000 > %sclasp_log_mttg.txt", folder_out,folder_out);\
    system(cmd);																				  \																								  \
}																											  \
while (false);

/** call clasp (limit times =5000[s]) */
#define	_CALL_CLASP_LIMIT_TIME_5000_	do																  		  \
{																											  \
	printf("	resolving time limit 5000...\n");														  \
    sprintf(cmd, "clasp.exe %smttg_pbo.txt --time-limit 5000 > %sclasp_log_mttg.txt", folder_out,folder_out);\
    system(cmd);																																		  \
}																											  \
while (false);

/** call clasp (limit times =8000[s]) */
#define	_CALL_CLASP_LIMIT_TIME_8000_	do																  		  \
{																											  \
	printf("	resolving time limit 8000...\n");														  \
    sprintf(cmd, "clasp.exe %smttg_pbo.txt --time-limit 8000 > %sclasp_log_mttg.txt", folder_out,folder_out);\
    system(cmd);																																		  \
}																											  \
while (false);

/** call clasp (limit times =10000[s]) */
#define	_CALL_CLASP_LIMIT_TIME_10000_	do																  		  \
{																											  \
	printf("	resolving time limit 10000...\n");														  \
    sprintf(cmd, "clasp.exe %smttg_pbo.txt --time-limit 10000 > %sclasp_log_mttg.txt", folder_out,folder_out);\
    system(cmd);																																			  \
}																											  \
while (false);

/** call clasp (limit times =15000[s]) */
#define	_CALL_CLASP_LIMIT_TIME_15000_	do																  		  \
{																											  \
	printf("	resolving time limit 15000...\n");														  \
    sprintf(cmd, "clasp.exe %smttg_pbo.txt --time-limit 15000 > %sclasp_log_mttg.txt", folder_out,folder_out);\
    system(cmd);																																			  \
}																											  \
while (false);

/** call clasp (limit times =20000[s]) */
#define	_CALL_CLASP_LIMIT_TIME_20000_	do		 \
{                                                \
    printf("	resolving time limit 200000...\n");     \
    sprintf(cmd, "clasp.exe %smttg_pbo.txt --time-limit 20000 > %sclasp_log_mttg.txt", folder_out,folder_out);\
    system(cmd);																																			  \
}																											  \
while (false);

/** call clasp (solution =1) */
#define	_CALL_CLASP_SOL_1_	do																  				  \
{																											  \
	printf("	resolving solution 1...\n");														  \
    sprintf(cmd, "clasp.exe %smttg_pbo.txt --models=1 > %sclasp_log_mttg.txt", folder_out, folder_out); \
	system(cmd);					 \
																								  \
}																											  \
while (false);

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
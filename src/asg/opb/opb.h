//-------------------------------------------------------------------------------------------------------------
//	include
//-------------------------------------------------------------------------------------------------------------
#pragma once
#include <stdio.h>

#include "../../header/Netlist.h"
#include "../createSGmodel.h"


//-------------------------------------------------------------------------------------------------------------
//	define
//-------------------------------------------------------------------------------------------------------------
#define	MAXSIZE_CONS      500000			  /**< maximum size of constraint */


/**  */


//-------------------------------------------------------------------------------------------------------------
//	structre
//-------------------------------------------------------------------------------------------------------------
/** problem size */
typedef struct OPBproblemSize
{
	int					  vars;			      /**< number of variables */
	int					  cons;				  /**< number of constraints */
	int					  pros;				  /**< number of products */
	int					  spros;			  /**< number of sizeproducts */
}
OPBSIZE;

/** opb */
typedef struct OPBproblem
{
	OPBSIZE				  constant;			  /**< constant size */
	OPBSIZE				  total;			  /**< total size */
}
OPB;

//-------------------------------------------------------------------------------------------------------------
//	global variable
//-------------------------------------------------------------------------------------------------------------
extern OPB					opb;				/**< 問題のサイズ(変数・制約の数) */


//-------------------------------------------------------------------------------------------------------------
//	prototype declaration
//-------------------------------------------------------------------------------------------------------------
/** create the pahase-shifter constraint */
bool CreateConsPhaseShifter(
	BIT_INT* lfsr,				  /**< lfsr */
	int      		      mcycle			  /**< number of cycles */
);

/** create the good-circuit constraint */
bool CreateConsGC(
	void
);

/** assigne the variable for good-circuit */
void AssigneVarsGC(
	void
);

/** create the good-circuit constraint -AND */
void CreateConsGC_AND(
	NLIST* netptr			  /**< pointer to netlist */
);

/** create the good-circuit constraint -NAND */
void CreateConsGC_NAND(
	NLIST* netptr			  /**< pointer to netlist */
);

/** create the good-circuit constraint -OR */
void CreateConsGC_OR(
	NLIST* netptr			  /**< pointer to netlist */
);

/** create the good-circuit constraint -NOR */
void CreateConsGC_NOR(
	NLIST* netptr			  /**< pointer to netlist */
);

/** create the good-circuit constraint -BUF */
void CreateConsGC_BUF(
	NLIST* netptr			  /**< pointer to netlist */
);

/** create the good-circuit constraint -INV */
void CreateConsGC_INV(
	NLIST* netptr			  /**< pointer to netlist */
);

/** create the good-circuit constraint -XOR */
void CreateConsGC_XOR(
	NLIST* netptr			  /**< pointer to netlist */
);

/** create the good-circuit constraint -XNOR */
void CreateConsGC_XNOR(
	NLIST* netptr			  /**< pointer to netlist */
);

/** calculate the problem size */
void OPBcalcSize(
	OPBSIZE* sizeptr,			  /**< pointer to OPB size */
	int					  vars,				  /**< number of variables */
	int					  cons,				  /**< number of constraints */
	int					  pros,				  /**< number of products */
	int					  spros				  /**< number of sizseproducts */
);

/** make file -write the phase-shifter cosntraint */
void makeProbFileConsPS_clasp(
	FILE* fileptr			  /**< pointer to file */
);










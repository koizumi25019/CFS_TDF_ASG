/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*																											 */
/*	program		:	urashima																				 */
/*	file		:	./src/opt/opt.h																	         */
/*	deginer		:	R.miura																			  		 */
/*	date		:	2022.10.01																  				 */
/*																											 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#pragma once
//-------------------------------------------------------------------------------------------------------------
//	include
//-------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdbool.h>

//-------------------------------------------------------------------------------------------------------------
//	define
//-------------------------------------------------------------------------------------------------------------
#define	OPT_OKAY				  true		  /**< return code   = okay */
#define	OPT_ERROR				  false		  /**< return code   = error */

#define	MAXSIZE_FILENAME		  100		  /**< maximum size of filename */

#define FILE_NOSET			      (char*)NULL /**< initial filename */
#define MODE_NOSET			      -1          /**< initial mode */

#define DROP_YES				  true
#define DROP_NO				      false
#define DROP_ERROR			      -1

#define	PrintHelpMessage		  /*colorNo4*/ printf	  /**< help message */


//-------------------------------------------------------------------------------------------------------------
//	structre
//-------------------------------------------------------------------------------------------------------------
/** input file structre */
typedef struct Input_File
{
	char* net;				  /**< netlist file */
	char* fault;			  /**< fault list file */
	char* xortap;			  /**< xor-tap file */
	char* scanchain;		  /**< scan-chain file */
	char* necessary;		  /**< necessary file */
	int lfsr;				  /**< num lfsr */
	int loop;				  /**< num of selection compatible group  */
}
INPUT;

/** output file structre */
typedef struct Output_File
{
	char* pin;			      /**< pin file */
	char* seed;		          /**< seed file */
	char* test;		          /**< test pattern file */
	char* graphlog;			  /**< graph log file */
	char* log;			      /**< log file */
	char* untestable;         /**< untestable fault file */
}
OUTPUT;

/** file structre */
typedef struct File
{
	INPUT				  input;			  /**< input files */
	OUTPUT				  output;			  /**< output files */
}
FILES;

/** mode structre */
typedef struct Mode
{
	int					  drop;		          /**< mode   = method */
}
MODES;

/** option structre */
typedef struct Option
{
	FILES			      file;			      /**< files */
	MODES				  mode;	   			  /**< modes */
}
OPTION;


//-------------------------------------------------------------------------------------------------------------
//	global variable
//-------------------------------------------------------------------------------------------------------------
OPTION					   opt;				  /**< option */


//-------------------------------------------------------------------------------------------------------------
//	prototype declaration
//-------------------------------------------------------------------------------------------------------------
/** set the options */
bool OPT(
	int					  argc,			      /**< number of command-arguments */
	char** argv			      /**< arguments */
);

/** initialize the options */
void OPTinit(
	void
);

/** initialize the files */
void OPTinitFile(
	void
);

/** initialize the modes */
void OPTinitMode(
	void
);

/** initialize the parameters */
//void OPTinitParam(
//	void
//);

/** set the options */
bool OPTset(
	int				      argc,				  /**< number of command-arguments */
	char** argv				  /**< command-arguments */
);

/** read the setting file */
bool OPTread(
	char* filename			  /**< filename */
);

/** check for essential options */
bool OPTcheck(
	void
);

/** check for modes */
bool OPTcheckMode(
	void
);

/** check for files */
bool OPTcheckFile(
	void
);

/** display the help */
void OPTdispHelp(
	void
);










#pragma once
//-------------------------------------------------------------------------------------------------------------
//	include
//-------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <time.h>

#include "target.h"
//#include "read.h"
#include "../opt/opt.h"
#include "../header/Netlist.h"
#include "../lib/lib.h"


//-------------------------------------------------------------------------------------------------------------
//	define
//-------------------------------------------------------------------------------------------------------------
#define READ_OKAY		  true		          /**< return code   = ture */
#define READ_ERROR		  false				  /**< return code   = false */

#define MAXSIZE_BUFFER	  500000	          /**< maximum size of buffer */
#define MAXSIZE_BITINT	  32		          /**< maximum size of bitint */

#define FOUND			  true		          /**< return code   = true */
#define NOT_FOUND		  false				  /**< return code   = false */

/** fault reader -on */
#define READER_FAULT_ON			if (opt.file.input.fault != FILE_NOSET)

/** fault reader -on */
#define READER_NECESSARY_ON		if (opt.file.input.necessary != FILE_NOSET)

#define READER_GRAPH_ON			if (opt.file.input.graph != FILE_NOSET)
#define READER_GRAPH_OFF		if (opt.file.input.graph == FILE_NOSET)

/** compare string and "End-Of-File" */
#define COMP_EOF(string)		string != (char*)NULL

/** compare string and "New-Line" */
#define COMP_NEWLINE(string)	strcmp(string, "\n")!=0	

#define	MAXSIZE_HASH	  100				  /**< size of fault list */
#define TFR				  2			          /**< transition fault model   = rise */
#define	TFF				  4			          /**< transition fault model   = fall */
#define SF0				  8			          /**< stack-at fault model   = 0 */
#define	SF1				  16			      /**< stack-at fault model   = 1 */

#define	FP				  1					  /**< flag type   = fault point */
#define	TFO				  2			          /**< flag type   = transitive fout */
#define	TPO				  4					  /**< flag type   = transitive priamary-output */
#define	TFO_TPO			  (TFO|TPO)			  /**< flag type   = TFO & TPO */
#define	FP_TPO			  (FP|TPO)			  /**< flag type   = FP & TPO */

#define	DETECTED		  7					  /**< fault detected */
#define	UNDETECTED		  17				  /**< fault undetected */
#define	REDEUNDANT		  27				  /**< fault redeundant */																						  \

/** xor-tap reader -on */
#define READER_XOR_TAP_ON		if (opt.file.input.xortap != FILE_NOSET)

/** scan-chain reader -on */
#define READER_SCAN_CHAIN_ON	if (opt.file.input.scanchain != FILE_NOSET)


//-------------------------------------------------------------------------------------------------------------
//	structre
//-------------------------------------------------------------------------------------------------------------

/** phase-shifter net structres */
typedef struct PhaseShifterNetList
{
	char* name;								  /**< name */
	int					  id;			      /**< id */
	int					  vars;			      /**< variable */
	char* cons;								  /**< constriant */
	int					  nin;			      /**< number of inputs */
	int* in;								  /**< input lists */
	int					  nout_slength;	      /**< scan-length */
	NLIST** out_schain;						  /**< out (scan-chain) */
}
PS_NLIST;

/** phase shifter structres */
typedef struct PhaseShitfter
{
	int					  num;			      /**< number of phase-shifters */
	int					  maxslength;		  /**< maximum maximum scan-length */
	struct PhaseShifterNetList* nlist;        /**< phase-shifter list */
}
PS;

/* add data since 2023/10/09 */
/** linear feedback shift register */
typedef struct LinearFeedbackShiftRegister
{
	int					  num;			      /**< number of LFSR */
	//int					  maxslength;		  /**< maximum bits for LFSR */
	struct LFSRNetList* nlist;				  /**< LFSR list */
}
LFSR;

/** necessary structre */
typedef struct Necessary
{
	int					  num;			      /**< number of nece */
	struct NecessaryNetlist* necelist;					  /**< list */
}
NECESSARY;

/** read data structres */
typedef struct ReadData
{
	struct PhaseShitfter  psnet;			  /**< phase-shifter net  */
	struct LinearFeedbackShiftRegister lfsrnet;/** lfsr net */
	struct Necessary	necenet;			  /**< necessary net */
}
READDATA;

/** lfsr net structres */
typedef struct LFSRNetList
{
	char* name;								  /**< name */
	int					  id;			      /**< id */
	int					  vars;			      /**< variable */
}
LFSR_NLIST;

/** necessary net structre */
typedef struct NecessaryNetlist
{
	char* string;			  /**< string */
	int					  id;			      /**< id */
	char* nece;				  /**< Info necessary */
	char* name;				  /**< Name */
	int					  type;				  /**< fault type */
}
NECE_NLIST;

/** clique node structre */
typedef struct CliqueNode
{
	int					  depth;			  /**< depth */
	int					  select;			  
	int					  num_r;			  /**< relaxation varaibales :ŠÉ˜a•Ï””H*/
	BIT_INT* edge;			                  //•Ó‚ªŒq‚ª‚Á‚Ä‚¢‚é‚©‚ðŽ¦‚·(edge==1‚È‚çŒq‚ª‚Á‚Ä‚¢‚é)
	struct CliqueNode* nextptr;				  /**< pointer to next node */
}
CNODE;

//-------------------------------------------------------------------------------------------------------------
//	global variable
//-------------------------------------------------------------------------------------------------------------
READDATA			      readdata;			  /**< reading data */
int						num_clique;			  /**< num of clique */
int						num_seed;			  /**< num of seed */
bool					num_clique_flag;
clock_t					time_graph;

//-------------------------------------------------------------------------------------------------------------
//	prototype declaration
//-------------------------------------------------------------------------------------------------------------
/** read the file */
bool ReadFile(
	void
);

/** read the xor-tap */
bool ReadXORtap(
	void
);

/** get the number of phase-shifters */
int getNumPhaseShifter(
	FILE* fileptr,		      /**< pointer to file */
	char* buffer		      /**< buffer */
);

/** get the index of pahase-shifter */
int getPhaseShifterIndex(
	char* buffer		      /**< buffer */
);

/** create the xor-tap list */
bool createXORtapList(
	int 				  index,		      /**< index */
	char* buffer		      /**< buffer */
);

/** read the scan-chain */
bool ReadScanChain(
	void
);

/** get the maximum scan-length */
int getMaxScanLength(
	FILE* fileptr,		      /**< pointer to file */
	char* buffer		      /**< buffer */
);

/** create the lfsr list */
bool CreateLFSRList(
	void
);

/** reading the necessary */
bool ReadNecessary(
	void
);

/** search for necessary node */
bool searchNnode(
	char* buffer,			  /**< buffer (key) */
	int no					  /**< No. */
);

CNODE* CreateCliqueNode(
	int depth,				/* depth */
	int select,				/* select no. */
	int num					/* remain->num */
);
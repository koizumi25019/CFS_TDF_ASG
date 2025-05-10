/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*																											 */
/*	program		:	ASG																						 */
/*	file		:	./src/asg/target.h																	     */
/*	deginer		:	T.sone																			  		 */
/*	date		:	2022.10.01		(2023.10.10)											  				 */
/*																											 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#pragma once
//-------------------------------------------------------------------------------------------------------------
//	include
//-------------------------------------------------------------------------------------------------------------
#include "read.h"
#include "../header/Fault.h"


//-------------------------------------------------------------------------------------------------------------
//	define
//-------------------------------------------------------------------------------------------------------------
#define TARGET_OKAY	     true				  /**< return code = okay */
#define TARGET_ERROR	 false				  /**< return code = error */
#define F1				16
#define F2				256
#define F3				4096
#define F4				65536
#define F5				1048576
#define F6				16777216
#define F7				268435456
#define F8				4294967296

//-------------------------------------------------------------------------------------------------------------
//	structre
//-------------------------------------------------------------------------------------------------------------

/* sort structure*/
typedef struct Sorted {
	unsigned int num;						/**< number of sort strcuture */
	struct Edge* sort;					/**< sort list */
}SORTED;//頂点リストをソートしたもの

/* edge structure*/
typedef struct Edge {
	unsigned int id;						/**< id *///頂点に一意に割り当てられるid？？
	unsigned int n_edge;					/*繋がっている辺の数*/
	//追加
	BIT_INT* edge;				    /**< compatible edge */
}EDGE;//頂点
//-------------------------------------------------------------------------------------------------------------
//	global variable
//-------------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------------
//	prototype declaration
//-------------------------------------------------------------------------------------------------------------
/** set the target-fault list */
//bool SetTarget(
//	TARGET* remain,			  /**< remain-fault list */
//	TARGET* target,			  /**< target-fault list */
//	SORTED* sorted,			  /**< sorted list */
//	int loop				  /**< number of loop */
//);

/** create necessary net */
bool CreateNecessaryNet(
	void
);

/* create compatible edge*/
bool CreateCompatibleEdge(
	void
);

/* set the direct target */
//bool DirectInputTarget(
//	TARGET* target			  /**< target-fault list */
//);

/* sort edge Descend*/
void SortEdgeDescend(
	SORTED* sorted 			  /**< sorted list */
);

/* sort edge Ascend*/
//void SortEdgeAscend(
//	TARGET* remain,			  /**< target-fault list */
//	SORTED* sorted 			  /**< sorted list */
//);


//int FuncSortAscend(
//	const void* n1, const void* n2
//);

int FuncSortDescend(
	const void* n1, const void* n2
);

/** create the compatible infomation */
void CreateCompatibleInfo(
	void
);

/** initial compatible infomation */
void COMPinti(
	void
);

/** set the CompatibleFaultSet*/
bool CompatibleFaultSet(
	SORTED* sorted, 			  /**< sorted list */
	int loop				  /**< loop variable */
);

/* counter bits */
int CountBits(
	unsigned int n			  /**< target-number */			
);

/* output graph log file */
//void OutGraphLogfile(
//);

/** reading the graph */
//bool ReadGraph(
	/*TARGET* remain*/
//);

//void shuffle(int* array, int n);

//bool RandomTarget(
	//TARGET* remain,			  /**< remain-fault list */
	//TARGET* target			  /**< target-fault list */
//);
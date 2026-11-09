//------------------------------------------------------------------------
//  入力ファイル(XORタップ・スキャンチェーン・必須割当て)の読み込みと、読み込んだデータ
//------------------------------------------------------------------------
#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "../opt/opt.h"
#include "../header/Netlist.h"
#include "../header/Fault.h"
#include "../lib/lib.h"

#define READ_OKAY		  true		          /**< return code   = ok */
#define READ_ERROR		  false				  /**< return code   = error */
#define MAXSIZE_BUFFER	  500000	          /**< maximum size of buffer (1行の最大長) */
#define MAXSIZE_BITINT	  32		          /**< ビット列の1語のビット数 */

//------------------------------------------------------------------------
//  構造体
//------------------------------------------------------------------------
/** フェーズシフタの1チャンネル */
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

/** フェーズシフタ */
typedef struct PhaseShitfter
{
	int					  num;			      /**< number of phase-shifters */
	int					  maxslength;		  /**< maximum maximum scan-length */
	struct PhaseShifterNetList* nlist;        /**< phase-shifter list */
}
PS;

/** LFSRの1ビット */
typedef struct LFSRNetList
{
	char* name;								  /**< name */
	int					  id;			      /**< id */
	int					  vars;			      /**< variable */
}
LFSR_NLIST;

/** LFSR */
typedef struct LinearFeedbackShiftRegister
{
	int					  num;			      /**< number of LFSR */
	struct LFSRNetList* nlist;				  /**< LFSR list */
}
LFSR;

/** 読み込んだデータ */
typedef struct ReadData
{
	struct PhaseShitfter  psnet;			  /**< phase-shifter net  */
	struct LinearFeedbackShiftRegister lfsrnet;/** lfsr net */
}
READDATA;

//------------------------------------------------------------------------
//  グローバル変数
//------------------------------------------------------------------------
extern READDATA			readdata;			  /**< 読み込んだデータ */
extern clock_t			time_graph;			  /**< 両立可能故障グラフの作成時間 */

//------------------------------------------------------------------------
//  関数
//------------------------------------------------------------------------
/** 全ての入力ファイルを読む */
bool ReadFile(void);

/** read the xor-tap */
bool ReadXORtap(void);

/** get the number of phase-shifters */
int getNumPhaseShifter(FILE* fileptr, char* buffer);

/** get the index of pahase-shifter */
int getPhaseShifterIndex(char* buffer);

/** create the xor-tap list */
bool createXORtapList(int index, char* buffer);

/** read the scan-chain */
bool ReadScanChain(void);

/** create the lfsr list */
bool CreateLFSRList(void);


//-------------------------------------------------------------------------------------------------------------
//	include
//-------------------------------------------------------------------------------------------------------------
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "./opb.h"
#include "../createSGmodel.h"
#include "../init.h"
#include "../../header/Netlist.h"
#include "../../lib/lib.h"
//#include "../../debug/debug.h"

//*************************************************************************************************************
//	@name		：　AssigneVarsGC
//	@function	：	assigne the variable for good-circuit
//	@return		：	(void)
//*************************************************************************************************************
void AssigneVarsGC(
	void
)
{
	/** for lfsr *///LFSRのための変数を確保
	//opb.constant.vars:これまでに割り当てられた変数の総数を保持するカウンター。つまりsuffixに相当
	//readdata.lfsrnet.num:LFSRの信号線数
	opb.constant.vars += readdata.lfsrnet.num;

	/** assigne the variable for primary inputs */
	//外部入力に変数インデックスを割り当て
	for (int i = 0; i < n_pi; i++) {
		//pi[i]->varsgc:i番目の外部入力信号線に割り当てられた変数インデックス
		pi[i]->varsgc = ++opb.constant.vars;
		printf("x%d→%s varsgc\n", pi[i]->varsgc, pi[i]->name);
	}
	//疑似外部入力に変数インデックス割当て
	for (int i = 0; i < n_ppi; i++) {
		//pi[i]->varsgc:i番目の疑似外部入力信号線に割り当てられた変数インデックス
		ppi[i]->varsgc = ++opb.constant.vars;
		printf("x%d→%s varsgc\n", ppi[i]->varsgc, ppi[i]->name);
	}
	/** assigne the variable for others */
	//その他の信号線に変数インデックスを割り当て
	for (int i = 0; i < n_net; i++)
	{
		if (nl[i].varsgc == UNASSIGN)
			nl[i].varsgc = ++opb.constant.vars;
			printf("x%d→%s varsgc\n", nl[i].varsgc, nl[i].name);
	}

	return;
}








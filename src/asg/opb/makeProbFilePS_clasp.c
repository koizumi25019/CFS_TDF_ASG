//-------------------------------------------------------------------------------------------------------------
//	include
//-------------------------------------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "../read.h"

//*************************************************************************************************************
//	@name		：　makeProbFilePS_clasp
//	@function	：	make file -write the phase-shifter cosntraint
//	@return		：	(void)
//*************************************************************************************************************
void makeProbFileConsPS_clasp(
	FILE* fileptr			  /**< pointer to file */
)
{

	for (int i = 0; i < readdata.psnet.num; i++)
	{

#ifdef NDEBUG
		assert(strcmp(readdata.psnet.nlist[i].cons, "") != 0);
#endif

		fprintf(fileptr, "%s", readdata.psnet.nlist[i].cons);
	}
	for (int i = 0;i < readdata.lfsrnet.num;i++)
	{
		if (i == 0) { fprintf(fileptr, "1 x%d ", readdata.lfsrnet.nlist[i].vars); }
		else{ fprintf(fileptr, "+1 x%d ", readdata.lfsrnet.nlist[i].vars); }
	}
	fprintf(fileptr, ">=1; \n");
	return;
}
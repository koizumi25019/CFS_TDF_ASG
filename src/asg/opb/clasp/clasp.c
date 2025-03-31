//-------------------------------------------------------------------------------------------------------------
//	include
//-------------------------------------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <direct.h>

#include "clasp.h"
#include "../../read.h"
#include "../../../lib/lib.h"
//#include "debug.h"
#include "../../../header/Define.h"

//*************************************************************************************************************
//	@name		：　CLASP
//	@function	：	call the "clasp.exe"
//	@return		：	(bool) okay, error
//*************************************************************************************************************
bool CLASP(
	void
)
{
	FILE* fileptr = (FILE*)NULL;
	int resolving_flag = 0;
	char txt_cmd[200];

	while (true)
	{
		//clasp logファイルオープン
		sprintf(txt_cmd, "%sclasp_log_mttg.txt", folder_out);
		if ((fileptr = (fopen(txt_cmd, "r"))) == NULL) {
			printf("【ERROR】:File open clasp_log_mttg.txt(clasp.c)\n");
			return ERROR;
		}

		/** get the objective value */
		CLASPgetObjVal(fileptr);

		if (clasp.status == CLASP_UNSAT)
		{
			/** remove redeundant fault */
			/** close the "clasp log file" in read-mode */
			break;
		}
		else if (clasp.status == CLASP_STATUS_UNKNOWN)
		{
			/** unknown error */
			/** close the "clasp log file" in read-mode */
			fclose(fileptr);

			if (resolving_flag == 0 && 5000 > fi[TIME].value)
			{
				/** resolving */
				_CALL_CLASP_LIMIT_TIME_5000_;
				resolving_flag = 1;
			}
			else if (resolving_flag == 1 && 10000 > fi[TIME].value)
			{
				/** resolving */
				_CALL_CLASP_LIMIT_TIME_10000_;
				resolving_flag = 2;
			}
			else if (resolving_flag == 2)
			{
				/** resolving */
				_CALL_CLASP_SOL_1_;
				resolving_flag = 3;
			}
			else
			{
				return CLASP_ERROR;
			}
		}
		else
		{
			/** get the CLASP solution */
			CLASPgetSol(fileptr);
			break;
		}
	}
	/** close the "clasp log file" in read-mode */
	fclose(fileptr);

	//assert(remove(CLASP_LOG_FILE) == 0);

	return	CLASP_OKAY;
}

//*************************************************************************************************************
//	@name		：　CLASPgetObjVal
//	@function	：	get the object value
//	@return		：	(void)
//*************************************************************************************************************
void CLASPgetObjVal(
	FILE* fileptr			  /**< pointer to file */
)
{
	char* buffer = (char*)NULL;
	char* tmptok = (char*)NULL;
	char* context = (char*)NULL;

	buffer = (char*)allocMemory(MAXSIZE_BUFFER, sizeof(char));

	clasp.status = CLASP_OBJVAL_ERROR;

	while (COMP_EOF(fgets(buffer, MAXSIZE_BUFFER, fileptr)))
	{
		if (strstr(buffer, "Answer:") != NULL)
		{
			printf("Answer\n");
			for (int i = 0; i < strlen(buffer); i++)
			{
				if (isdigit(buffer[i]) != 0)
				{
					clasp.status = atoi(strtok_s(&buffer[i], "\n", &context));
					break;
				}
			}
			break;
		}
		if (strstr(buffer, "UNSATISFIABLE") != NULL)
		{
			printf("UNSAT\n");
			clasp.status = CLASP_UNSAT;
			break;
		}
		if (strstr(buffer, "UNKNOWN") != NULL)
		{
			printf("UNKNOWN\n");
			clasp.status = CLASP_STATUS_UNKNOWN;
			break;
		}
	}

	free(buffer);

	return;
}

//*************************************************************************************************************
//	@name		：　CLASPgetSol
//	@function	：	get the solution
//	@return		：	(void)
//*************************************************************************************************************
void CLASPgetSol(
	FILE* fileptr			  /**< pointer to file */
)
{
	char* buffer = (char*)NULL;
	char* tmptok = (char*)NULL;
	char* context = (char*)NULL;
	int	  count = 0;

	buffer = (char*)allocMemory(MAXSIZE_BUFFER, sizeof(char));

	/** initialize the solution to all-zeros */
	memset(clasp.sol[SOL_SEED], '0', readdata.lfsrnet.num);
	memset(clasp.sol[SOL_TP], '0', n_dff + n_pi);

	/** get the solution */
	while (COMP_EOF(fgets(buffer, MAXSIZE_BUFFER, fileptr)))
	{
		if (COMP_NEWLINE(buffer))
		{
			/** get the variable number */
			if (buffer[0] == 'v') {
				tmptok = strtok_s(buffer, " \n",&context);//vを削除する
				while ((tmptok = strtok_s(NULL, " \n",&context)) != NULL) {
					if (count >= 0 && count < readdata.lfsrnet.num) {
						if (tmptok[0] == '-') {
						clasp.sol[SOL_SEED][count] = '0';
						}
						else {
						clasp.sol[SOL_SEED][count] = '1';
						}
						count++;
					}
					else if (count >= readdata.lfsrnet.num && count < readdata.lfsrnet.num + n_dff + n_pi) {
						if (tmptok[0] == '-') {
							clasp.sol[SOL_TP][count-readdata.lfsrnet.num] = '0';
						}
						else {
							clasp.sol[SOL_TP][count-readdata.lfsrnet.num] = '1';
						}
						count++;
					}
					if (count == readdata.lfsrnet.num + n_dff + n_pi) {
						return;
					}
				}
			}

		}
		else
		{
			break;
		}
	}

	free(buffer);

	return;
}







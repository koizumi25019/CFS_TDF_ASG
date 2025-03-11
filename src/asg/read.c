/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*																											 */
/*	program		:	ASG																						 */
/*	file		:	./src/asg/read.c																		 */
/*	deginer		:	R.miura			covered T.sone													  		 */
/*	date		:	2022.10.01		(2023.10.10)											  				 */
/*																											 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


//-------------------------------------------------------------------------------------------------------------
//	include
//-------------------------------------------------------------------------------------------------------------
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "read.h"
#include "../lib/lib.h"
#include "../header/Netlist.h"
//#include "debug.h"


//*************************************************************************************************************
//	@name		：　ReadFile
//	@function	：	read the file
//	@return		：	(bool) okay, error
//*************************************************************************************************************
bool ReadFile(
	void
)
{
	/** read the xor-tap */
	if (CreateLFSRList() != READ_OKAY) return READ_ERROR;

	/** read the xor-tap */
	if (ReadXORtap() != READ_OKAY) return READ_ERROR;

	/** read the scan-chain */
	if (ReadScanChain() != READ_OKAY) return READ_ERROR;

	printf("	Reading the files completed ... \n");

	return READ_OKAY;
}

//*************************************************************************************************************
//	@name		：　ReadXORtap
//	@function	：	read the xor-tap
//	@return		：	(bool) okay, error
//*************************************************************************************************************
bool ReadXORtap(
	void
)
{
	READER_XOR_TAP_ON
	{
		FILE * fileptr = (FILE*)NULL;
		char* buffer = (char*)NULL;
		char* context = (char*)NULL;
		int     index = 0;

		/** open the "xor-tap file" in read-mode */
		fileOpen(&fileptr, opt.file.input.xortap, "r");

		buffer = (char*)allocMemory(MAXSIZE_BUFFER, sizeof(char));

		/** get the number of bits */
		readdata.psnet.num = getNumPhaseShifter(fileptr, buffer);

		/** alloc the memory for phase-shifter netlist */
		readdata.psnet.nlist = (PS_NLIST*)allocMemory(readdata.psnet.num, sizeof(PS_NLIST));

		/** read the xor-tap list */
		for (int i = 0; COMP_EOF(fgets(buffer, MAXSIZE_BUFFER, fileptr)); i++)
		{
			if (COMP_NEWLINE(buffer))
			{
				/** get the index of phase-shifter */
				index = getPhaseShifterIndex(strtok_s(buffer, ":", &context));

				/** create the xor-tap list */
				if (createXORtapList(index, strtok_s(NULL, "\n\0", &context)) != READ_OKAY) return READ_ERROR;
			}
		}

		free(buffer);

		/** close the "xor-tap file" in read-mode */
		fclose(fileptr);


#ifdef __DEBUG_READ_XOR_TAP__
		_CALL_DEBUG_READ_XOR_TAP_
#endif  // __DEBUG_READ_XOR_TAP__
	}

	return READ_OKAY;
}

//*************************************************************************************************************
//	@name		：　getNumPhaseShifter
//	@function	：	get the number of phase-shifters
//	@return		：	(int) number of phase-shifters
//*************************************************************************************************************
int getNumPhaseShifter(
	FILE* fileptr,		      /**< pointer to file */
	char* buffer		      /**< buffer */
)
{
	int     numps = 0;

	while (COMP_EOF(fgets(buffer, MAXSIZE_BUFFER, fileptr)))
	{
		if (COMP_NEWLINE(buffer))
		{
			numps++;
		}
	}
	rewind(fileptr);

	return numps;
}

//*************************************************************************************************************
//	@name		：  getPhaseShifterIndex
//	@function	：	get the index of pahase-shifter
//	@return		：	(int) index of pahase-shifter
//*************************************************************************************************************
int getPhaseShifterIndex(
	char* buffer		      /**< buffer */
)
{
	int		index = -1;

	for (int i = 0; i < strlen(buffer); i++)
	{
		if (isSingleByte(buffer[i]) == true)
		{
			if (isdigit(buffer[i]) != false)
			{
				index = atoi(&buffer[i]);
				break;
			}
		}
	}

#ifdef NDEBUG
	assert(index != -1);
#endif

	return index;
}

//*************************************************************************************************************
//	@name		：  createXORtapList
//	@function	：	create the xor-tap list
//	@return		：	(bool) okay, error
//*************************************************************************************************************
bool createXORtapList(
	int 				  index,		      /**< index */
	char* buffer		      /**< buffer */
)
{
	char* context = (char*)NULL;
	int		k = 0;

	/** initialize */
	readdata.psnet.nlist[index].id = index + 1;								/** id */
	readdata.psnet.nlist[index].vars = index + 1;								/** variable */
	readdata.psnet.nlist[index].nin = 0;										/** number of inputs */
	readdata.psnet.nlist[index].nout_slength = 0;										/** number of outputs */
	readdata.psnet.nlist[index].name = (char*)allocMemory(30, sizeof(char));		/** name */
	sprintf_s(readdata.psnet.nlist[index].name, 30, "ps_out_%d", readdata.psnet.nlist[index].id);

	/** get the number of xor-taps */
	for (int i = 0; i < strlen(buffer); i++)
	{
		if (buffer[i] == '_')
		{
			readdata.psnet.nlist[index].nin++;
		}
	}
	readdata.psnet.nlist[index].in = (int*)allocMemory(readdata.psnet.nlist[index].nin, sizeof(int));

	/** store the xor-tap number */
	readdata.psnet.nlist[index].in[k++] = atoi(strtok_s(buffer, "_\n\0", &context));
	while ((buffer = strtok_s(NULL, "_\n\0", &context)) != NULL)
	{
		readdata.psnet.nlist[index].in[k++] = atoi(buffer);
	}

	return READ_OKAY;
}

//*************************************************************************************************************
//	@name		：　ReadScanChain
//	@function	：	read the scan-chain
//	@return		：	(bool) okay, error
//*************************************************************************************************************
bool ReadScanChain(
	void
)
{
	READER_SCAN_CHAIN_ON
	{
		FILE * fileptr = (FILE*)NULL;
		char* buffer = (char*)NULL;
		char* context = (char*)NULL;
		char* token = (char*)NULL;
		int     index = 0;
		int     j = 0;

		/** open the "scan-chain file" in read-mode */
		fileOpen(&fileptr, opt.file.input.scanchain, "r");

		buffer = (char*)allocMemory(MAXSIZE_BUFFER, sizeof(char));

		/** get the maximum scan-length */
		readdata.psnet.maxslength = getMaxScanLength(fileptr, buffer);

		/** alloc the memory for output of phase-shifter */
		for (int i = 0; i < readdata.psnet.num; i++)
		{
			if (readdata.psnet.nlist[i].nout_slength >= 1)
				readdata.psnet.nlist[i].out_schain = (NLIST**)allocMemory(readdata.psnet.nlist[i].nout_slength, sizeof(NLIST*));
		}

		//readdata.psnet.nlist[i].out_schain
		//ここにスキャンチェーンとPI,PPIを繋げる

		/** read the scan-chain */
		for (int i = 0; i < readdata.psnet.maxslength; i++)
		{
			index = 0;
			j = 0;

			fgets(buffer, MAXSIZE_BUFFER, fileptr);

			if (COMP_NEWLINE(buffer))
			{
				while (j < strlen(buffer))
				{
					if (buffer[j] != ' ' && buffer[j] != ',' && buffer[j] != '\n' && buffer[j] != '\0')
					{
						token = strtok_s(&buffer[j], ", \n\0", &context);

						/** store the pointer to netlist */
						for (int k = 0; k < n_net; k++)
						{
							if (!strcmp(token, nl[k].name))//名前が一致したら繋げる
							{
								readdata.psnet.nlist[index++].out_schain[i] = &nl[k];
								break;
							}
						}

						/** error */
						if (readdata.psnet.nlist[index - 1].out_schain[i] == NULL)
						{
							printf("\n	FILE ERROR: scan-chain file reading failed. ");
							printf("%s was passed through. \n", token);
							return READ_ERROR;
						}

						if ((token = strtok_s(NULL, "\n\0", &context)) == NULL) break;
						strcpy_s(buffer, MAXSIZE_BUFFER, token);
						j = 0;
					}
					else
					{
						j++;
					}
				}
			}
			else
			{
				i--;
			}
		}

		free(buffer);

		/** close the "scan-chain file" in read-mode */
		fclose(fileptr);


#ifdef __DEBUG_READ_SCAN_CHAIN__
		_CALL_DEBUG_READ_SCAN_CHAIN_
#endif  // __DEBUG_READ_SCAN_CHAIN__
	}

	return READ_OKAY;
}

//*************************************************************************************************************
//	@name		：　getMaxScanLength
//	@function	：	get the maximum scan-length
//	@return		：	(int) maximum scan-length
//*************************************************************************************************************
int getMaxScanLength(
	FILE* fileptr,		      /**< pointer to file */
	char* buffer		      /**< buffer */
)
{
	int     index = 0;
	int     maxscanlength = 0;

	/** get the maximum scan-length */
	while (COMP_EOF(fgets(buffer, MAXSIZE_BUFFER, fileptr)))
	{
		if (COMP_NEWLINE(buffer)) maxscanlength++;
	}
	rewind(fileptr);

	/** get the scan-length of each scan-chain */
	while (COMP_EOF(fgets(buffer, MAXSIZE_BUFFER, fileptr)))
	{
		if (COMP_NEWLINE(buffer))
		{
			for (int i = 0; i < strlen(buffer); i++)
			{
				if (buffer[i] == ',') readdata.psnet.nlist[index++].nout_slength++;
			}
			readdata.psnet.nlist[index].nout_slength++;
		}
		index = 0;
	}

	rewind(fileptr);

	return maxscanlength;
}


/* add create source code since 2023/10/09 */
//*************************************************************************************************************
//	@name		：  CreateLFSRList
//	@function	：	Create the LFSR list
//	@return		：	(bool) okay, error
//*************************************************************************************************************
bool CreateLFSRList(
	void
)
{
	/* variable declaration */
	char* context = (char*)NULL;
	int		k = 0;
	int		i = 0;

	/* variable initializion */
	readdata.lfsrnet.num = opt.file.input.lfsr;
	readdata.lfsrnet.nlist = (LFSR_NLIST*)allocMemory(readdata.lfsrnet.num, sizeof(LFSR_NLIST));
	for (i = 0;i < readdata.lfsrnet.num;i++) 
	{
		readdata.lfsrnet.nlist[i].id = i + 1;
		readdata.lfsrnet.nlist[i].vars = i + 1;
		readdata.lfsrnet.nlist[i].name = (char*)allocMemory(200, sizeof(char));		/** name */
		sprintf_s(readdata.lfsrnet.nlist[i].name, 200, "lsfr_net_%d", readdata.lfsrnet.nlist[i].id);
	}
	/** error */
	if (readdata.lfsrnet.nlist[i-1].vars!=readdata.lfsrnet.num)
	{
		printf("\n	FILE ERROR: LFSR reading failed. ");
		printf("the number of lfsr vars failed. \n");

		return READ_ERROR;
	}
	
	return READ_OKAY;
}
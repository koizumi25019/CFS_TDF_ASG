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
//追加
#include"../header/Fault.h"
#include"../header/Define.h"
#include"target.h"
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
	//追加
    /** create the detection infomation list */
	detflag = (BIT_INT*)allocMemory(1, sizeof(BIT_INT));
	detflag->int_num = all_num / MAXSIZE_BITINT + 1;
	detflag->flag = (unsigned int*)allocMemory(detflag->int_num, sizeof(unsigned int));
	bitintSetAll_One(detflag);

	/** read the xor-tap */
	if (CreateLFSRList() != READ_OKAY) return READ_ERROR;

	/** read the xor-tap */
	if (ReadXORtap() != READ_OKAY) return READ_ERROR;

	/** read the scan-chain */
	if (ReadScanChain() != READ_OKAY) return READ_ERROR;

	/** read the necessary */
	if (ReadNecessary() != READ_OKAY) return READ_ERROR;

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
//*************************************************************************************************************
//	@name		：  CreateCompatibleInfo
//	@function	：	Create Compatible Infomation
//	@return		：	(bool) okay, error
//*************************************************************************************************************
void CreateCompatibleInfo(
	void
)
{
	/* variable declaration */
	int j = 0;
	int type = 0;
	UNDET_FAULT_LIST* current = ufl;

	/** add necessary to structure  */

	for (int i = 0; i < ufl_num; i++)
	{
		for (j = 0; j < readdata.necenet.num; j++) {
			//未検出故障リストの読み込んだ必須割当て情報の信号線名と故障タイプが一致すれば未検出故障リストの必須割当情報をコピーする
			if (!strcmp(current->string, readdata.necenet.necelist[j].string))
			{
				sprintf_s(current->nece, MAXSIZE_BUFFER, "%s", readdata.necenet.necelist[j].nece);
				break;
			}
		}
		current = current->next;
	}
	return;
}

//*************************************************************************************************************
//	@name		：　COMPinit
//	@function	：	initialize the compatible sets infomation
//	@return		：	(void)
//*************************************************************************************************************
void COMPinti(
	void
)
{
	/* variable initializion */

	UNDET_FAULT_LIST* head = ufl;//追加//先頭を保存

	for (int i = 0; i < ufl_num; i++)
	{
		ufl->necenet = (BIT_INT_XP*)allocMemory(1, sizeof(BIT_INT_XP));
		ufl->necenet->int_num = n_net / MAXSIZE_BITINT + 1;
		ufl->necenet->x_buf = (unsigned int*)allocMemory(
			ufl->necenet->int_num, sizeof(unsigned int));
		ufl->necenet->p_buf = (unsigned int*)allocMemory(
			ufl->necenet->int_num, sizeof(unsigned int));
		All_Bit_X_XP(ufl->necenet);
		ufl->nece = (char*)allocMemory(MAXSIZE_BUFFER, sizeof(char));//必須割り当て情報ファイルを1行ずつ格納
		ufl->edge = (BIT_INT*)allocMemory(1, sizeof(BIT_INT));
		ufl->edge->int_num = ufl_num / MAXSIZE_BITINT + 1;
		ufl->edge->flag = (unsigned int*)allocMemory(ufl->edge->int_num, sizeof(unsigned int));
		bitintSetAll_Zero(ufl->edge);//オール0：すべて未検出
		for (int j = 0; j < ufl_num; j++)
		{
			bitintSetNbit_One(ufl->edge, j);//指定位置(j番目を1にセット)これは自分の位置を1にしておくためだと思う
		}
		ufl->n_edge = ufl_num;//?最初はn_edgeは未検出故障数？
		ufl = ufl->next;//後続アドレスへのポインタ
	}
	//追加
	//リストの先頭に戻す
	ufl = head;
	return;
}

//*************************************************************************************************************
//	@name		：  ReadNeessary
//	@function	：	Reading File Necessary
//	@return		：	(bool) okay, error
//*************************************************************************************************************
bool ReadNecessary(
	void
)
{
	READER_NECESSARY_ON
	{
	/* variable declaration */
	FILE* fileptr = (FILE*)NULL;
	char* buffer = (char*)NULL;
	char* context = (char*)NULL;
	char* string = (char*)NULL;
	char* token = (char*)NULL;
	char* token2 = (char*)NULL;
	int cnt = 0;
	int f_length = 0;
	int hash = 0;
	int type = 0;
	int i = 0;

	/* partitioning allocation */
	buffer = (char*)allocMemory(MAXSIZE_BUFFER, sizeof(char));
	string = (char*)allocMemory(MAXSIZE_BUFFER, sizeof(char));

	readdata.necenet.necelist = (NECE_NLIST*)allocMemory(/*readdata.fault.numinit*/all_num, sizeof(NECE_NLIST));

	//必須割り当て情報ファイルオープン
	fileOpen(&fileptr, opt.file.input.necessary, "r");

	/** create the fault list */
	while (COMP_EOF(fgets(buffer, MAXSIZE_BUFFER, fileptr)))
	{
		if (COMP_NEWLINE(buffer))
		{
			token = strtok_s(buffer, ", \n\0", &context);//token:信号線名
			sprintf_s(string, MAXSIZE_BUFFER, "%s",token);//string:信号線名
			token2 = strtok_s(NULL, ", \n\0", &context);//token2:故障タイプ(ex:SF1,SF0)
			if (!strcmp(token2, "SF0"))
			{
				//type = SF0;おそらくいらない
				sprintf_s(string, MAXSIZE_BUFFER, "%s\tsa0\n", string);
				//string:G16\tsa0\n
			}
			else if (!strcmp(token2, "SF1"))
			{
				//type = SF1;おそらくいらない
				sprintf_s(string, MAXSIZE_BUFFER, "%s\tsa1\n", string);
				//string:G16\tsa1\n
			}
			/** calcurate the hash */
			//線形リストから探索に改変したのでハッシュ値計算いらない
			//hash = calcHash(string);

			/** create Necessary node */
			//未検出故障リスト(線形)から一致する文字列を探索
			if (searchNnode(string/*readdata.fault.list[hash]*/ , cnt) != NOT_FOUND)
			{
				//必須割り当て情報取得(ex:G16=0 G15=1......など)
				token2 = strtok_s(NULL, "\n\0", &context);
				readdata.necenet.necelist[cnt].nece = (char*)allocMemory(MAXSIZE_BUFFER, sizeof(char));
				//readdata.necenet.necelist[cnt].neceにはG16=0などが格納
				sprintf_s(readdata.necenet.necelist[cnt].nece, MAXSIZE_BUFFER, "%s", token2);
				cnt++;
			}
		}
	}
	free(buffer);
	free(string);
	printf("一致した文字列の数:%d\n", cnt);
	if (cnt != ufl_num)
	{
		printf("\n	SYSTEM ERROR: Reading necessary file error. ");
		printf("	Not enough necessary information. ");
		printf("\n	num of necessary info = %d", cnt);
		printf("		num of target fault   = %d\n", ufl_num);

		return READ_ERROR;
	}
	else if(cnt==0){
		printf("ERROR:故障リストと一致する必須割当情報が存在しません\n");

	    return READ_ERROR;
	}
	else {
		readdata.necenet.num = cnt;
	}
	fclose(fileptr);
	}

	return READ_OKAY;
}

//*************************************************************************************************************
//	@name		：　searchNnode
//	@function	：	search for necessary node
//	@return		：	(bool) found, not found
//*************************************************************************************************************
bool searchNnode(
	char* buffer,			  /**< buffer (key) */
	int no
)
{
	UNDET_FAULT_LIST* current = ufl;
	while (current != NULL)
	{
		//buffer:G16\tsa1\n
		if (!strcmp(current->string, buffer))
		{
			//searchNnodeのcurrent->string(未検出故障のstring)とbuffer(G16_sa1)
			readdata.necenet.necelist[no].string = (char*)allocMemory(MAXSIZE_BUFFER, sizeof(char));
			sprintf_s(readdata.necenet.necelist[no].string, 200, "%s", current->string);
			readdata.necenet.necelist[no].id = no;
			readdata.necenet.necelist[no].type = current->type;
			readdata.necenet.necelist[no].name = (char*)allocMemory(200, sizeof(char));
			sprintf_s(readdata.necenet.necelist[no].name, 200, "%s", current->name);
			return	FOUND;
		}
		current = current->next;
	}

	return NOT_FOUND;
}




//追加 ASGのtarget.cに記載されていた関数を持ってきた

//*************************************************************************************************************
//	@name		：　CreateNecessaryNet
//	@function	：	making graph for compatible
//	@return		：	(bool) okay, error
//*************************************************************************************************************
bool CreateNecessaryNet(
	void
)
{
	char* buffer = (char*)NULL;
	char* context = (char*)NULL;
	char* token = (char*)NULL;
	char* token2 = (char*)NULL;


	buffer = (char*)allocMemory(MAXSIZE_BUFFER, sizeof(char));

	//追加
	//リストの先頭を保持するhead
	UNDET_FAULT_LIST* head = ufl;
	for (int i = 0; i < ufl_num; i++)
	{
		//buffer:G16=0 G15=1...ある信号線に対する必須割り当て情報
		sprintf_s(buffer, MAXSIZE_BUFFER, "%s", ufl->nece);
		//token:G16などの信号線名
		token = strtok_s(buffer, "=, ,\n", &context);
		while (token != NULL) {
			//token2:必須割り当ての信号線の値(ex:G16=0の0)
			token2 = strtok_s(NULL, "=, ,\n", &context);
			for (int j = 0; j < n_net; j++)
			{
				if (!strcmp(token, nl[j].name))
				{
					if (!strcmp(token2, "0"))
					{
						Set_NINT_Zero_XP(ufl->necenet, (unsigned int)j);
					}
					else if (!strcmp(token2, "1"))
					{
						Set_NINT_One_XP(ufl->necenet, (unsigned int)j);
					}
					else
					{
						return TARGET_ERROR;
					}
					break;
				}
			}
			token = strtok_s(NULL, "=, ,\n", &context);
		}
		ufl = ufl->next;
		printf("\r	Reading necessary infomation progress  >> %.2f%%", ((float)(i + 1) / (float)ufl_num) * 100);
	}
	//追加
	//リストを先頭に戻す
	ufl = head;
#ifdef __DEBUG_SET_TARGET_NECESSARY__
	_CALL_DEBUG_SET_TARGET_NECESSARY_(remain);
#endif // __DEBUG_SET_TARGET_NECESSARY__

	return TARGET_OKAY;
}

//*************************************************************************************************************
//	@name		：　CreateCompatibleEdge
//	@function	：	making graph for compatible fault sets
//	@return		：	(bool) okay, error
//*************************************************************************************************************
bool CreateCompatibleEdge(
	void
)
{
	int comp_flag = 0;
	int value = 0;
	unsigned int buffer;
	clock_t half_start, half_end;

	/* initialize comp variable */
	BIT_INT_XP* comp;

	half_start = clock();
	comp = (BIT_INT_XP*)allocMemory(1, sizeof(BIT_INT_XP));
	comp->int_num = n_net / MAXSIZE_BITINT + 1;//信号線数/32+1
	comp->x_buf = (unsigned int*)allocMemory(comp->int_num, sizeof(unsigned int));
	comp->p_buf = (unsigned int*)allocMemory(comp->int_num, sizeof(unsigned int));
	All_Bit_X_XP(comp);
	printf("\n");

	//追加
	UNDET_FAULT_LIST* ufl_i;
	ufl_i = ufl;
	UNDET_FAULT_LIST* ufl_j;
	ufl_j = ufl;

	/* create compatible infomation */
	for (int i = 0; i < ufl_num; i++)
	{
		for (int j = 0; j < ufl_num; j++)
		{
			comp_flag = 0;
			if (i != j)
			{
				for (int k = 0; k < comp->int_num; k++)
				{
					if (comp_flag == 1)break;
					/* all X */
					All_Bit_X_XP(comp);
					/* search comp */
					comp->x_buf[k] &= ufl_i->necenet->x_buf[k];
					comp->x_buf[k] &= ufl_j->necenet->x_buf[k];
					comp->p_buf[k] &= ufl_i->necenet->p_buf[k];
					comp->p_buf[k] &= ufl_j->necenet->p_buf[k];
					/* search crash */
					buffer = comp->x_buf[k];
					buffer |= comp->p_buf[k];
					if (buffer != 0xFFFFFFFF)//0がある場合論理値が衝突。つまり圧縮ができない、つまり両立不可能
					{
						bitintSetNbit_Zero(ufl_i->edge, j);
						comp_flag = 1;
						ufl_i->n_edge -= 1;
					}
				}
			}
			//追加
			ufl_j = ufl_j->next;
		}
		//追加
		ufl_i = ufl_i->next;
		ufl_j = ufl;//ufl_jのポインタが更新されたままになってしまうのでポインタをuflでリセット

		printf("\r	            Generating graph progress  >> %.2f%%", ((float)(i + 1) / (float)ufl_num) * 100);
	}
#ifdef __DEBUG_SET_TARGET_COMP__
	_CALL_DEBUG_SET_TARGET_COMP_(remain);
#endif // __DEBUG_SET_TARGET_COMP__
	half_end = clock();
	time_graph = half_end - half_start;
	printf("\n	Making Graph CPU time：	%.3f[s]\n", (float)time_graph / CLOCKS_PER_SEC);

	return TARGET_OKAY;
}

//*************************************************************************************************************
//	@name		：　CompatibleFaultSet
//	@function	：	set the Compatible Fault list
//	@return		：	(bool) okay, error
//*************************************************************************************************************
bool CompatibleFaultSet(
	SORTED* sorted,			  /**< sorted list */
	int loop				  /**< loop variable */
)
{
	CNODE** clique = (CNODE**)NULL;
	CNODE* cnodeptr = (CNODE*)NULL;
	CNODE* head = (CNODE*)NULL;
	CNODE* p = (CNODE*)NULL;
	int depth = 0;				/* depth */
	int select;				/* select no. */
	int j = 0;
	int flag = 0, select_flag = 0, end_flag = 0, d_flag = 0;
	int count = 0;

	int* select_id;
	select_id = (int*)calloc(ufl_num + 1, sizeof(int));//select_idはソートされた集合の数と一致するのでuflでいい
	if (select_id == NULL) {
		printf("【ERROR】: Memory allocation failed.\n");
		return ERROR;
	}

	for (int i = 0; i < ufl_num; i++) select_id[i] = -1;

	//両立可能故障における目標故障選択

	//未検出故障数が100以下の場合の処理
	//if (ufl_num < 100)
	//{
	//	if (num_clique_flag == false)
	//	{
	//		num_clique = loop + 1;
	//		num_clique_flag = true;
	//	}
	//	tfl = ufl;
	//	tfl_num = ufl_num;
	//	return TARGET_OKAY;
	//}
	//else
	//{
		//シード生成の最初は故障が繋がる辺の数を基準に降順ソードする
		//繋がった辺の数が多いやつからグラフを作成していくため
		//辺の数が多いやつから選択することで効率的にグラフを作成
		if (loop == 0) {
			SortEdgeDescend(sorted);
		}
	//}

	//両立可能故障集合から最大クリークを抽出

	//両立可能故障グラフの頂点の構造体を作成、初期化
	//全故障数分クリーク作成
	head = CreateCliqueNode(depth++, -1, all_num);//線形リスト　depth==0

	//detflagはすべて1で初期化されているので、
	//それと同じアルゴリズムを用いるedge_flagにもオール1を代入している
	for (int k = 0; k <head->edge->int_num; k++)//cnodeptr->edge->int_num = ufl_num / 32 + 1;
	{
		head->edge->flag[k] = detflag->flag[k];//detflag[]で32故障分。これをk分繰り返すと全故障参照できる
	}

	//隣接する頂点の数を数える
	for (int k = 0; k < head->edge->int_num; k++)
	{
		count += CountBits(head->edge->flag[k]);//1の数(隣接する頂点が1)をカウント
	}
	head->num_r = count;//隣接する頂点の数

	//depth:深さ　深さ優先探索の深さ　深くないやつ程多くの辺を持つ故障
	//depth-1の数が両立可能故障集合数と一致
	//ここからクリーク抽出
	//head->num_r==countでcountが0になったらループ抜ける。0になると繋がる辺がなくなったということ
	while ((depth < ufl_num) && (head->num_r != 0))//head->num_r：隣接する頂点数
	{
		j = 0;
		select_flag = 0;
		while (j < all_num) {
			if (select_flag == 1)break;
			select = sorted->sort[j].id;//ex;sorted->sort[0].id 一番つながっている辺の数が多い節の故障id．これはufl(未検出故障集合)に対してloop回数1回目に定義
			//辺が繋がっているか(edge==1)かを取得して,それが何ビット目かをselectに取得
			if (bitintGetNbit(head->edge, select) == 1) {//select番目のbitが1か0かを返す関数　つまりselect番目の故障が隣接する頂点数が一番多いので、その故障の番号を選択している
				select_id[depth] = select;//select_id[depth];深さがdepthの時の故障の番号を記憶
				select_flag = 1;
				bitintSetNbit_Zero(head->edge, select);//故障を選択したのでその故障番号を0に設定

				printf("\n選択した故障を0に設定(select:故障ID)：%d\n", select);
				/* debug bitint */
				//unsigned int decimalNumber;
				//unsigned int binaryNumber[32]; // 32ビットの2進数を仮定
				//int k = 0;
				//int l = 0;
				//for (int m = 0;m < head->edge->int_num;m++)
				//{
				//	k = 0;
				//	decimalNumber = head->edge->flag[m];
				//	while (decimalNumber > 0)
				//	{
				//		binaryNumber[k] = decimalNumber % 2;
				//		decimalNumber /= 2;
				//		k++;
				//	}
				//	printf("depth:[%d]\thead  ->  edge[%d]     \t:  ", depth, m);
				//	for (l = k - 1;l >= 0;l--)
				//	{
				//		printf("%d", binaryNumber[l]);
				//	}
				//}

				/* create next depth node */
				if ((cnodeptr = CreateCliqueNode(depth, select, all_num)) != NULL)//全故障数分クリークを作成 depthは1からスタート
				{
					cnodeptr->nextptr = head;//一つ後ろのクリークノードのポインタ
					head = cnodeptr;

					depth++;

					//edge->int_num:32bit_flag確保数
					UNDET_FAULT_LIST* current = afl;
					for (int i = 0; i < select; i++) {
						current = current->next;
					}
					for (int k = 0; k < cnodeptr->edge->int_num; k++)
					{
						//flag[k]は各要素が32bitのデータを持つ
						//flag[k] は「k 番目の 32 bit」を指しており、ビットフラグをまとめて 32 bit 単位で管理している
						//たとえば、故障(ノード)数が 300 個くらいあるとすると、300 ビットをひとつの変数には収められません。
						//300 / 32 ＝ 約 9.3 …
						//よって 10 個前後の 32 bit 変数に分割して「ビット AND」をしないといけません。
						head->edge->flag[k]= (head->nextptr->edge->flag[k])	& (current->edge->flag[k]);//変更 //後ろのcnodeのflag
						//後ろのedge_flagと今のedge_flagの積をとると、次の深さのedge
					}
					count = 0;
					for (int k = 0; k < head->edge->int_num; k++)
					{
						count += CountBits(head->edge->flag[k]);//1の数をカウント
					}
					head->num_r = count;

					/* debug bitint */
					//unsigned int decimalNumber_c;
					//unsigned int binaryNumber_c[32]; // 32ビットの2進数を仮定
					//int p = 0;
					//int q = 0;
					//for (int m = 0; m < current->edge->int_num; m++)
					//{
					//	p = 0;
					//	decimalNumber_c = current->edge->flag[m];
					//	while (decimalNumber_c > 0)
					//	{
					//		binaryNumber_c[p] = decimalNumber_c % 2;
					//		decimalNumber_c /= 2;
					//		p++;
					//	}
					//	printf("\ndepth:[%d]\tcurrent  ->  edge[%d]    :  ", depth, m);
					//	for (q = p - 1; q >= 0; q--)
					//	{
					//		printf("%d", binaryNumber_c[q]);
					//	}
					//}

				}
				else return TARGET_ERROR;

				/* debug bitint */
				//unsigned int decimalNumber_;
				//unsigned int binaryNumber_[32]; // 32ビットの2進数を仮定
				//int p = 0;
				//int q = 0;
				//for (int m = 0;m < head->edge->int_num;m++)
				//{
				//	p = 0;
				//	decimalNumber_ = head->edge->flag[m];
				//	while (decimalNumber_ > 0)
				//	{
				//		binaryNumber_[p] = decimalNumber_ % 2;
				//		decimalNumber_ /= 2;
				//		p++;
				//	}
				//	printf("\ndepth:[%d]\thead  ->  edge[%d]     \t:  ", depth, m);
				//	for (q = p - 1;q >= 0;q--)
				//	{
				//		printf("%d", binaryNumber_[q]);
				//	}
				//}
			}
			j++;
		}
	}
	printf("\n");
	//ここまでクリーク抽出

	//変更
	//目標故障集合作成
	// tfl_num を depth - 1 に設定
	tfl_num = depth - 1;
	UNDET_FAULT_LIST* current = afl;
	tfl = NULL;
	UNDET_FAULT_LIST* tfl_head = NULL;
	UNDET_FAULT_LIST* new_node = NULL;

	for (int i = 0; i < tfl_num; i++)
	{
		// select_id[i+1] の回数だけ current を先に進める
		for (int j = 0; j < select_id[i + 1]; j++) {
			if (current == NULL) {
				printf("両立可能故障_目標故障設定ミス\n");
				exit(1);
			}
			current = current->next;
		}

		// 新たな tfl 用ノードを確保して情報をコピー（deep copy）
		new_node = (UNDET_FAULT_LIST*)malloc(sizeof(UNDET_FAULT_LIST));
		if (new_node == NULL) {
			printf("【ERROR】: Memory allocation failed.\n");
			exit(1);
		}
		// 各フィールドのコピー
		new_node->type = current->type;
		strcpy(new_node->name, current->name);
		new_node->netlist = current->netlist;      // ポインタ自体はコピーされるが、構造体全体の複製が不要ならこのままでOK
		new_node->node_id = current->node_id;
		// 例として、故障選択済みフラグを設定
		if (new_node->netlist != NULL) {
			new_node->netlist->tf_flag = FLAG_UP;
		}
		new_node->next = NULL;  // 新規確保時は next は NULL にしておく

		// tfl リストへの連結
		if (tfl == NULL) {
			tfl = new_node;
			tfl_head = new_node;
		}
		else {
			tfl->next = new_node;
			tfl = new_node;
		}

		// current を AFL の先頭にリセットして、次のループで再度 select_id に従って走査する
		current = afl;
	}

	// tfl_head が新たな tfl リストの先頭ノードとなる
	tfl = tfl_head;
	if (tfl == NULL) {
		printf("両立可能故障集合設定エラー\n");
		exit(1);
	}

	/* 以降、不要になった構造体の解放（以前確保したメモリのクリーンアップなど） */
	while (head)
	{
		p = head;
		head = head->nextptr;
		free(p);
	}
#ifdef __DEBUG_SET_TARGET__
	_CALL_DEBUG_SET_TARGET_(target);
#endif // __DEBUG_SET_TARGET__

	return TARGET_OKAY;
}

//*************************************************************************************************************
//	@name		：　SortEdgeDescend
//	@function	：	sort edge Descend
//	@return		：	null
//*************************************************************************************************************
void SortEdgeDescend(
	SORTED* sorted 			  /**< sorted list */
)
{
	UNDET_FAULT_LIST* current = ufl;
	sorted->num = ufl_num;
	sorted->sort = (EDGE*)allocMemory(sorted->num, sizeof(EDGE));


	for (int i = 0; i < sorted->num; i++)
	{
		sorted->sort[i].id = i;
		sorted->sort[i].n_edge = current->n_edge;//繋がっている辺の数　CreateCompatibleEdgeで辺がつながっている数をカウントしている
		current = current->next;
	}
	qsort(sorted->sort, ufl_num, sizeof(EDGE), FuncSortDescend);

	for (int i = 0; i < sorted->num; i++)
	{
		printf("sort[%d].id:%d---n_edge:%d\n",i, sorted->sort[i].id, sorted->sort[i].n_edge);
	}

	return;
}
//*************************************************************************************************************
//	@name		：　sort_ascend
//	@function	：	sort_ascend
//	@return		：	
//*************************************************************************************************************
//int FuncSortAscend(
//	const void* n1, const void* n2
//)
//{
//	if (((EDGE*)n1)->n_edge > ((EDGE*)n2)->n_edge) {
//		return 1;
//	}
//	else if (((EDGE*)n1)->n_edge < ((EDGE*)n2)->n_edge) {
//		return -1;
//	}
//	else {
//		return 0;
//	}
//}
//*************************************************************************************************************
//	@name		：　CreateCliqueNode
//	@function	：	create the clique node
//	@return		：	(CNODE*) pointer to clique node
//*************************************************************************************************************
CNODE* CreateCliqueNode(
	int depth,				/* depth */
	int select,				/* select no. */
	int num					//初期故障集合数
)
{
	/* variable declaration */
	char* context = (char*)NULL;
	CNODE* cnodeptr = (CNODE*)NULL;

	/* partitioning allocation */
	cnodeptr = (CNODE*)allocMemory(1, sizeof(CNODE));

	/** set the depth */
	cnodeptr->depth = depth;

	/** set the select no. */
	cnodeptr->select = select;

	/** set the num comp */
	cnodeptr->num_r = 0;

	/** set the edge */
	cnodeptr->edge = (BIT_INT*)allocMemory(1, sizeof(BIT_INT));
	cnodeptr->edge->int_num = num / 32 + 1;
	cnodeptr->edge->flag = (unsigned int*)allocMemory(cnodeptr->edge->int_num, sizeof(unsigned int));


	/** set the pointer to next node */
	cnodeptr->nextptr = (CNODE*)NULL;

	return cnodeptr;
}
//*************************************************************************************************************
//	@name		：　CountBits
//	@function	：	bits counter
//	@return		：	num bits (int)
//*************************************************************************************************************
int CountBits(
	unsigned int n
)
{
	//動作例
	//たとえば、n = 13 (1101 in binary) の場合:
	//初期状態: n = 1101。
	//ステップ 1 : 各 2 ビットでカウント → n = 101。
	//ステップ 2 : 各 4 ビットでカウント → n = 11。
	//ステップ 3 : 各 8 ビットでカウント → n = 3。
	//結果 : 「1」の個数は 3。
	n = (n & 0x55555555) + (n >> 1 & 0x55555555);//0x55555555 == 0101010.....（偶数ビットが 1）。
	n = (n & 0x33333333) + (n >> 2 & 0x33333333);//0x33333333 == 00110011... （4ビットごとに下位2ビットが 1）。
	n = (n & 0x0f0f0f0f) + (n >> 4 & 0x0f0f0f0f);
	n = (n & 0x00ff00ff) + (n >> 8 & 0x00ff00ff);
	n = (n & 0x0000ffff) + (n >> 16 & 0x0000ffff);
	return n;
}
//*************************************************************************************************************
//	@name		：　sort_descend
//	@function	：	sort_descend
//	@return		：	
//*************************************************************************************************************
int FuncSortDescend(
	const void* n1, const void* n2
)
{
	if (((EDGE*)n1)->n_edge > ((EDGE*)n2)->n_edge) {
		return -1;
	}
	else if (((EDGE*)n1)->n_edge < ((EDGE*)n2)->n_edge) {
		return 1;
	}
	else {
		return 0;
	}
}

//*************************************************************************************************************
//	@name		：　OutGraphLogfile
//	@function	：	output the log
//	@return		：	null
//*************************************************************************************************************
//void OutGraphLogfile(
//	void
//)
//{
//	/* variable declaration */
//	int min = 0;
//	int max = 0;
//	int sum = 0;
//
//	/* calulate */
//	min = remain->list[0]->n_edge;
//	for (int i = 0; i < remain->num; i++)
//	{
//		if (min > remain->list[i]->n_edge)
//		{
//			min = remain->list[i]->n_edge;
//		}
//		if (max < remain->list[i]->n_edge)
//		{
//			max = remain->list[i]->n_edge;
//		}
//		sum += remain->list[i]->n_edge;
//	}
//
//	/** output graph log file */
//	FILE* fileptr = (FILE*)NULL;
//	fileOpen(&fileptr, opt.file.output.graphlog, "w");
//	fprintf(fileptr, "//-------------------------------------------------------\n");
//	fprintf(fileptr, "//          Compatible Fault Graph infomation\n");
//	fprintf(fileptr, "//-------------------------------------------------------\n");
//	fprintf(fileptr, "//  Number of vertex                : %d\n", all_num);
//	fprintf(fileptr, "//  Number of edges                 : %d\n", sum / 2);
//	fprintf(fileptr, "//  Number of minimum degree        : %d\n", min);
//	fprintf(fileptr, "//  Number of maximum degree        : %d\n", max);
//	fprintf(fileptr, "//  Number of average degree        : %d\n", sum / ufl_num);
//	fprintf(fileptr, "//  Percentage of branch density    : %.2f%%\n", ((float)sum / (float)(ufl_num * (ufl_num - 1)) * 100));
//	fprintf(fileptr, "//-------------------------------------------------------\n\n");
//
//	/** output number of adjacent vertices in graph */
//	for (int i = 0; i < ufl_num; i++)
//	{
//		if (remain->list[i]->type == SF1)
//		{
//			fprintf(fileptr, "%30s\tsa1    %3d\n", remain->list[i]->name, remain->list[i]->n_edge);
//		}
//		else
//		{
//			fprintf(fileptr, "%30s\tsa0    %3d\n", remain->list[i]->name, remain->list[i]->n_edge);
//		}
//	}
//
//	fclose(fileptr);
//	return;
//}

//*************************************************************************************************************
//	@name		：  ReadGraph
//	@function	：	Reading File Graph
//	@return		：	(bool) okay, error
//*************************************************************************************************************
//bool ReadGraph(
//	//TARGET* remain
//)
//{
//
//	/* variable declaration */
//	FILE* fileptr = (FILE*)NULL;
//	char* buffer = (char*)NULL;
//	char* context = (char*)NULL;
//	char* string = (char*)NULL;
//	char* string2 = (char*)NULL;
//	char* token = (char*)NULL;
//	char* token2 = (char*)NULL;
//	int type = 0;
//	int type2 = 0;
//	int cnt = 0;
//	int num_length = 0;
//	int i = 0, j = 0;
//
//	/* partitioning allocation */
//	buffer = (char*)allocMemory(MAXSIZE_BUFFER, sizeof(char));
//	string = (char*)allocMemory(MAXSIZE_BUFFER, sizeof(char));
//	string2 = (char*)allocMemory(MAXSIZE_BUFFER, sizeof(char));
//
//	/** open the "fault file" in read-mode */
//	fileOpen(&fileptr, opt.file.input.graph, "r");
//
//	for (i = 0; i < ufl_num; i++)
//	{
//		bitintSetAll_Zero(->edge);
//		remain->list[i]->n_edge = 0;
//	}
//
//	//ファイルの行数カウント
//	while (COMP_EOF(fgets(buffer, MAXSIZE_BUFFER, fileptr)))
//	{
//		if (COMP_NEWLINE(buffer))
//		{
//			num_length++;
//		}
//	}rewind(fileptr);
//
//
//	/** create the graph */
//	while (COMP_EOF(fgets(buffer, MAXSIZE_BUFFER, fileptr)))
//	{
//		if (COMP_NEWLINE(buffer))
//		{
//			token = strtok_s(buffer, " ", &context);//tokenに故障信号線名取得
//			sprintf_s(string, MAXSIZE_BUFFER, "%s", token);//stringに1つ目の故障信号線名を格納
//			type = atoi(strtok_s(NULL, ",\n", &context));//8か16かを取得→つまり0縮退か1縮退かを取得している
//			if (type == SF0)
//			{
//				sprintf_s(string, MAXSIZE_BUFFER, "%s\tsa0\n", string);
//			}
//			else if (type == SF1)
//			{
//				sprintf_s(string, MAXSIZE_BUFFER, "%s\tsa1\n", string);
//			}
//
//			token2 = strtok_s(NULL, " ", &context);
//			sprintf_s(string2, MAXSIZE_BUFFER, "%s", token2);//string2に2つ目の故障信号線名を取得
//			type2 = atoi(strtok_s(NULL, ",\n", &context));
//			if (type2 == SF0)
//			{
//				sprintf_s(string2, MAXSIZE_BUFFER, "%s\tsa0\n", string2);
//			}
//			else if (type2 == SF1)
//			{
//				sprintf_s(string2, MAXSIZE_BUFFER, "%s\tsa1\n", string2);
//			}
//
//			//remain：未検出故障集合
//			for (i = 0; i < remain->num; i++)
//			{
//				//取得した1つ目の故障信号線名と未検出故障リストの信号線名を比較
//				if (!strcmp(string, remain->list[i]->string))
//				{
//					for (j = i; j < remain->num; j++)
//					{
//						//取得した2つ目の故障信号線名と未検出故障リストの信号線名を比較
//						if (!strcmp(string2, remain->list[j]->string))
//						{
//							//(remain->list[i]->edge, j)はそれぞれの故障がグラフにおいて辺で結ばれていることを示すために1をセットしている
//							//詳しくは両立可能故障集合_グラフ生成アルゴリズムというファイルを参照
//							bitintSetNbit_One(remain->list[i]->edge, j);
//							remain->list[i]->n_edge++;//その故障が繋がっている辺の数をインクリメント
//							//さっきと逆のことを行う。一方の故障からもう一方の故障へ辺が繋がっていることを先ほど示したので、こちらの故障からも先ほどの故障へ辺が引かれていることを示すための処理
//							bitintSetNbit_One(remain->list[j]->edge, i);
//							remain->list[j]->n_edge++;
//							break;
//						}
//					}
//					if (j == remain->num)	return READ_ERROR;
//					break;
//				}
//			}
//			if (i == remain->num)	return READ_ERROR;
//		}
//		cnt++;
//		//進行状況確認したければ
//		printf("\r	Reading graph infomation progress  >> %.2f%%", ((float)(cnt) / (float)num_length) * 100);
//	}
//
//	fclose(fileptr);
//
//
//	return READ_OKAY;
//
//}

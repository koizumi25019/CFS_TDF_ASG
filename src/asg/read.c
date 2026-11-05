//-------------------------------------------------------------------------------------------------------------
//	include
//-------------------------------------------------------------------------------------------------------------
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>

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
	bitintSetAll_Zero(detflag);
	for (int fault_id = 0; fault_id < all_num; fault_id++) {
		bitintSetNbit_One(detflag, fault_id);
	}

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
static int blank_line(const char* line)
{
    return strspn(line, " \t\r\n") == strlen(line);
}

int getNumPhaseShifter(FILE* input, char* line)
{
    int count = 0;
    while (fgets(line, MAXSIZE_BUFFER, input) != NULL) {
        if (strchr(line, '\n') == NULL && !feof(input)) {
            count = -1;
            break;
        }
        if (!blank_line(line)) {
            count++;
        }
    }
    rewind(input);
    return count;
}

int getPhaseShifterIndex(char* label)
{
    char* end;
    long index;
    if (label == NULL) {
        return -1;
    }
    while (*label != '\0' && !isdigit((unsigned char)*label)) {
        if (*label == '-') {
            return -1;
        }
        label++;
    }
    if (*label == '\0') {
        return -1;
    }
    errno = 0;
    index = strtol(label, &end, 10);
    if (errno == ERANGE || index < 0 || index >= readdata.psnet.num) {
        return -1;
    }
    return (int)index;
}

bool createXORtapList(int channel_index, char* tap_text)
{
    PS_NLIST* channel;
    int* taps;
    int tap_count = 0;
    char* context = NULL;
    char* token;
    if (channel_index < 0 || channel_index >= readdata.psnet.num || tap_text == NULL) {
        return READ_ERROR;
    }
    channel = &readdata.psnet.nlist[channel_index];
    if (channel->name != NULL) {
        return READ_ERROR;
    }
    taps = (int*)allocMemory(readdata.lfsrnet.num, sizeof(*taps));
    token = strtok_s(tap_text, "_ \t\r\n", &context);
    while (token != NULL) {
        char* end;
        long tap;
        errno = 0;
        tap = strtol(token, &end, 10);
        if (*end != '\0' || errno == ERANGE || tap < 1 ||
            tap > readdata.lfsrnet.num || tap_count >= readdata.lfsrnet.num) {
            free(taps);
            return READ_ERROR;
        }
        for (int index = 0; index < tap_count; index++) {
            if (taps[index] == tap) {
                free(taps);
                return READ_ERROR;
            }
        }
        taps[tap_count++] = (int)tap;
        token = strtok_s(NULL, "_ \t\r\n", &context);
    }
    if (tap_count == 0) {
        free(taps);
        return READ_ERROR;
    }
    channel->id = channel_index + 1;
    channel->vars = channel_index + 1;
    channel->nin = tap_count;
    channel->in = taps;
    channel->name = (char*)allocMemory(30, 1);
    sprintf_s(channel->name, 30, "ps_out_%d", channel->id);
    return READ_OKAY;
}

bool ReadXORtap(void)
{
    FILE* input = NULL;
    char* line;
    int valid = 1;
    if (opt.file.input.xortap == FILE_NOSET ||
        fopen_s(&input, opt.file.input.xortap, "r") != 0) {
        return READ_ERROR;
    }
    line = (char*)allocMemory(MAXSIZE_BUFFER, 1);
    readdata.psnet.num = getNumPhaseShifter(input, line);
    if (readdata.psnet.num <= 0) {
        free(line);
        fclose(input);
        return READ_ERROR;
    }
    readdata.psnet.nlist = (PS_NLIST*)allocMemory(readdata.psnet.num, sizeof(PS_NLIST));
    while (fgets(line, MAXSIZE_BUFFER, input) != NULL) {
        char* separator;
        int channel_index;
        if (blank_line(line)) {
            continue;
        }
        separator = strchr(line, ':');
        if (separator == NULL) {
            valid = 0;
            break;
        }
        *separator = '\0';
        channel_index = getPhaseShifterIndex(line);
        if (!createXORtapList(channel_index, separator + 1)) {
            valid = 0;
            break;
        }
    }
    free(line);
    fclose(input);
    if (!valid) {
        printf("[ERROR] Invalid channel index, duplicate channel or invalid XOR tap.\n");
    }
    return valid ? READ_OKAY : READ_ERROR;
}

bool ReadScanChain(void)
{
    FILE* input = NULL;
    char* line;
    unsigned char* mapped;
    int row = 0;
    int mapped_input_count = 0;
    int valid = 1;
    if (opt.file.input.scanchain == FILE_NOSET ||
        fopen_s(&input, opt.file.input.scanchain, "r") != 0) {
        return READ_ERROR;
    }
    line = (char*)allocMemory(MAXSIZE_BUFFER, 1);
    mapped = (unsigned char*)allocMemory(n_net, 1);
    /* First count rows; every channel can then safely store at most that many entries. */
    readdata.psnet.maxslength = getNumPhaseShifter(input, line);
    if (readdata.psnet.maxslength <= 0) {
        valid = 0;
    }
    if (valid) {
        for (int channel = 0; channel < readdata.psnet.num; channel++) {
            readdata.psnet.nlist[channel].nout_slength = 0;
            readdata.psnet.nlist[channel].out_schain = (NLIST**)allocMemory(
                readdata.psnet.maxslength, sizeof(NLIST*));
        }
    }
    while (valid && fgets(line, MAXSIZE_BUFFER, input) != NULL) {
        char* context = NULL;
        char* name;
        int column = 0;
        if (blank_line(line)) {
            continue;
        }
        name = strtok_s(line, ", \t\r\n", &context);
        while (name != NULL) {
            int net_index;
            NLIST* net;
            PS_NLIST* channel;
            if (column >= readdata.psnet.num) {
                valid = 0;
                break;
            }
            for (net_index = 0; net_index < n_net; net_index++) {
                if (strcmp(name, nl[net_index].name) == 0) {
                    break;
                }
            }
            if (net_index == n_net) {
                valid = 0;
                break;
            }
            net = &nl[net_index];
            /* Existing scan files pad the last row with unused terminal outputs. */
            if (net->type == IN || net->type == DFF) {
                if (mapped[net_index]) {
                    valid = 0;
                    break;
                }
                mapped[net_index] = 1;
                mapped_input_count++;
            } else if (net->n_out != 0) {
                valid = 0;
                break;
            }
            channel = &readdata.psnet.nlist[column++];
            channel->out_schain[row] = net;
            channel->nout_slength = row + 1;
            name = strtok_s(NULL, ", \t\r\n", &context);
        }
        row++;
    }
    if (mapped_input_count != n_pi + n_ppi) {
        valid = 0;
    }
    free(mapped);
    free(line);
    fclose(input);
    if (!valid) {
        printf("[ERROR] Invalid scan-chain signal, duplicate input, missing input or excess column.\n");
    }
    return valid ? READ_OKAY : READ_ERROR;
}

/* Retained for source compatibility; scan lengths are now checked by ReadScanChain. */
int getMaxScanLength(FILE* input, char* line)
{
    return getNumPhaseShifter(input, line);
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
		ufl->n_edge = ufl_num;
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
bool ReadNecessary(void)
{
    FILE* input = NULL;
    char* line;
    unsigned char* found;
    int matched_count = 0;
    int valid = 1;
    if (opt.file.input.necessary == FILE_NOSET) {
        return READ_OKAY;
    }
    if (fopen_s(&input, opt.file.input.necessary, "r") != 0) {
        return READ_ERROR;
    }
    line = (char*)allocMemory(MAXSIZE_BUFFER, 1);
    found = (unsigned char*)allocMemory(all_num, 1);
    readdata.necenet.necelist = (NECE_NLIST*)allocMemory(all_num, sizeof(NECE_NLIST));
    while (fgets(line, MAXSIZE_BUFFER, input) != NULL) {
        char* context = NULL;
        char* name = strtok_s(line, ", \t\r\n", &context);
        char* type_name;
        char* assignments;
        int type;
        UNDET_FAULT_LIST* fault;
        if (name == NULL) {
            continue;
        }
        type_name = strtok_s(NULL, ", \t\r\n", &context);
        assignments = strtok_s(NULL, "\r\n", &context);
        if (type_name == NULL || assignments == NULL) {
            valid = 0;
            break;
        }
        if (strcmp(type_name, "SF0") == 0) {
            type = STR;
        } else if (strcmp(type_name, "SF1") == 0) {
            type = STF;
        } else {
            valid = 0;
            break;
        }
        for (fault = ufl; fault != NULL; fault = fault->next) {
            if (fault->type == type && strcmp(fault->name, name) == 0) {
                NECE_NLIST* entry;
                if (found[fault->node_id]) {
                    valid = 0;
                    break;
                }
                found[fault->node_id] = 1;
                entry = &readdata.necenet.necelist[matched_count++];
                entry->string = _strdup(fault->string);
                entry->name = _strdup(fault->name);
                entry->type = type;
                entry->id = fault->node_id;
                entry->nece = _strdup(assignments);
                break;
            }
        }
        if (!valid) {
            break;
        }
    }
    readdata.necenet.num = matched_count;
    free(found);
    free(line);
    fclose(input);
    if (!valid || matched_count != all_num) {
        printf("[ERROR] Missing, duplicate or malformed necessary assignments.\n");
        return READ_ERROR;
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
bool CreateNecessaryNet(void)
{
    char* buffer = (char*)allocMemory(MAXSIZE_BUFFER, 1);
    UNDET_FAULT_LIST* fault;
    int valid = 1;
    for (fault = ufl; fault != NULL && valid; fault = fault->next) {
        char* context = NULL;
        char* name;
        strcpy_s(buffer, MAXSIZE_BUFFER, fault->nece);
        name = strtok_s(buffer, "=, \t\r\n", &context);
        while (name != NULL) {
            char* value = strtok_s(NULL, "=, \t\r\n", &context);
            int net_index;
            if (value == NULL || (strcmp(value, "0") != 0 && strcmp(value, "1") != 0)) {
                valid = 0;
                break;
            }
            for (net_index = 0; net_index < n_net; net_index++) {
                if (strcmp(name, nl[net_index].name) == 0) {
                    break;
                }
            }
            if (net_index == n_net) {
                valid = 0;
                break;
            }
            if (*value == '0') {
                Set_NINT_Zero_XP(fault->necenet, (unsigned int)net_index);
            } else {
                Set_NINT_One_XP(fault->necenet, (unsigned int)net_index);
            }
            name = strtok_s(NULL, "=, \t\r\n", &context);
        }
    }
    free(buffer);
    return valid ? TARGET_OKAY : TARGET_ERROR;
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
	clock_t half_start, half_end;
	half_start = clock();
	int int_num = ufl->necenet->int_num;


	// ノードを配列にコピー
	UNDET_FAULT_LIST** nodes = malloc(sizeof(*nodes) * ufl_num);
	UNDET_FAULT_LIST* p = ufl;
	for (int i = 0; i < ufl_num; i++, p = p->next) nodes[i] = p;


	/* create compatible infomation */
	for (int i = 0; i < ufl_num; i++)
	{
		UNDET_FAULT_LIST* ufl_i = nodes[i];
		for (int j = i+1; j < ufl_num; j++)
		{
			UNDET_FAULT_LIST* ufl_j = nodes[j];
			for (int k = 0; k < int_num; k++)
			{
				unsigned x = ufl_i->necenet->x_buf[k] & ufl_j->necenet->x_buf[k];
				unsigned p = ufl_i->necenet->p_buf[k] & ufl_j ->necenet->p_buf[k];
				/* search crash */
				if ((x | p) != 0xFFFFFFFF)//0がある場合論理値が衝突。つまり圧縮ができない、つまり両立不可能
				{
					bitintSetNbit_Zero(ufl_i->edge, ufl_j->node_id);
					ufl_i->n_edge--;
					bitintSetNbit_Zero(ufl_j->edge, ufl_i->node_id);
					ufl_j->n_edge--;
					break;
				}
			}
		}
		printf("\rGenerating graph progress  >> %.2f%%", ((float)(i + 1) / (float)ufl_num) * 100);
	}
#ifdef __DEBUG_SET_TARGET_COMP__
	_CALL_DEBUG_SET_TARGET_COMP_(remain);
#endif // __DEBUG_SET_TARGET_COMP__
	half_end = clock();
	time_graph = half_end - half_start;
	printf("\n	Making Graph CPU time：	%.3f[s]\n", (float)time_graph / CLOCKS_PER_SEC);
	free(nodes);


	return TARGET_OKAY;
}

//*************************************************************************************************************
//	@name		：　CompatibleFaultSet
//	@function	：	set the Compatible Fault list
//	@return		：	(bool) okay, error
//*************************************************************************************************************
static int compare_fault_degree(const void* left, const void* right)
{
    const UNDET_FAULT_LIST* a = *(const UNDET_FAULT_LIST* const*)left;
    const UNDET_FAULT_LIST* b = *(const UNDET_FAULT_LIST* const*)right;
    if (a->n_edge != b->n_edge) {
        return a->n_edge > b->n_edge ? -1 : 1;
    }
    return (a->node_id > b->node_id) - (a->node_id < b->node_id);
}

bool CompatibleFaultSet(SORTED* sorted, int loop)
{
    UNDET_FAULT_LIST** candidates;
    UNDET_FAULT_LIST* current;
    UNDET_FAULT_LIST* tail = NULL;
    BIT_INT remaining;
    int candidate_count = 0;
    int target_limit = fi[TARGET_LIMIT].value;
    int all_fault_cutoff = 300;
    int select_all;

    (void)sorted;
    (void)loop;
    if (ufl == NULL || ufl_num <= 0) {
        return TARGET_ERROR;
    }
    if (strcmp(fi[CPS_MODE].name, "ON") == 0 &&
        strcmp(fi[FP_MODE].name, "ON") != 0) {
        all_fault_cutoff = 10;
    }
    /* Preserve the small-list fallback; this path does not promise a clique. */
    select_all = ufl_num < all_fault_cutoff;
    if (target_limit <= 0 || target_limit > ufl_num) {
        target_limit = ufl_num;
    }
    candidates = (UNDET_FAULT_LIST**)allocMemory(ufl_num, sizeof(*candidates));
    for (current = ufl; current != NULL; current = current->next) {
        if (candidate_count >= ufl_num) {
            free(candidates);
            return TARGET_ERROR;
        }
        candidates[candidate_count++] = current;
        current->netlist->tf_flag = FLAG_DOWN;
    }
    if (candidate_count != ufl_num) {
        free(candidates);
        return TARGET_ERROR;
    }
    if (!select_all) {
        qsort(candidates, candidate_count, sizeof(*candidates), compare_fault_degree);
    }
    remaining.int_num = detflag->int_num;
    remaining.flag = (unsigned int*)allocMemory(remaining.int_num, sizeof(*remaining.flag));
    memcpy(remaining.flag, detflag->flag, remaining.int_num * sizeof(*remaining.flag));
    tfl = NULL;
    tfl_num = 0;
    for (int index = 0; index < candidate_count && tfl_num < target_limit; index++) {
        UNDET_FAULT_LIST* candidate = candidates[index];
        UNDET_FAULT_LIST* copy;
        if (!select_all && !bitintGetNbit(&remaining, candidate->node_id)) {
            continue;
        }
        copy = (UNDET_FAULT_LIST*)allocMemory(1, sizeof(*copy));
        *copy = *candidate; /* Metadata is shared; only the copied list node is owned. */
        copy->next = NULL;
        if (tail == NULL) {
            tfl = copy;
        } else {
            tail->next = copy;
        }
        tail = copy;
        tfl_num++;
        candidate->netlist->tf_flag = FLAG_UP;
        if (!select_all) {
            /* Intersect neighbors: each choice must agree with ALL earlier choices. */
            for (unsigned int word = 0; word < remaining.int_num; word++) {
                remaining.flag[word] &= candidate->edge->flag[word];
            }
            bitintSetNbit_Zero(&remaining, candidate->node_id);
        }
    }
    free(remaining.flag);
    free(candidates);
    return tfl_num > 0 ? TARGET_OKAY : TARGET_ERROR;
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
	UNDET_FAULT_LIST* current = afl;
	sorted->num = all_num;
	sorted->sort = (EDGE*)allocMemory(sorted->num, sizeof(EDGE));


	for (int i = 0; i < sorted->num; i++)
	{
		sorted->sort[i].id = i;
		sorted->sort[i].n_edge = current->n_edge;//繋がっている辺の数　CreateCompatibleEdgeで辺がつながっている数をカウントしている
		current = current->next;
	}
	qsort(sorted->sort, all_num, sizeof(EDGE), FuncSortDescend);

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
void OutGraphLogfile(void)
{
    FILE* output = NULL;
    UNDET_FAULT_LIST* fault;
    long long degree_sum = 0;
    int count = 0;
    int minimum = 0;
    int maximum = 0;
    if (opt.file.output.graphlog == FILE_NOSET) {
        return;
    }
    for (fault = ufl; fault != NULL; fault = fault->next) {
        int degree = 0;
        for (UNDET_FAULT_LIST* neighbor = ufl; neighbor != NULL; neighbor = neighbor->next) {
            if (fault != neighbor && bitintGetNbit(fault->edge, neighbor->node_id)) {
                degree++;
            }
        }
        if (count == 0 || degree < minimum) {
            minimum = degree;
        }
        if (degree > maximum) {
            maximum = degree;
        }
        degree_sum += degree;
        count++;
    }
    fileOpen(&output, opt.file.output.graphlog, "w");
    fprintf(output, "vertices=%d edges=%lld min_degree=%d max_degree=%d\n",
        count, degree_sum / 2, minimum, maximum);
    fprintf(output, "average_degree=%.2f density=%.2f%%\n",
        count ? (double)degree_sum / count : 0.0,
        count > 1 ? 100.0 * degree_sum / count / (count - 1) : 0.0);
    fclose(output);
}

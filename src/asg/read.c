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
    if (opt.xor_tap_file == NULL ||
        fopen_s(&input, opt.xor_tap_file, "r") != 0) {
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
    if (opt.scan_chain_file == NULL ||
        fopen_s(&input, opt.scan_chain_file, "r") != 0) {
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
	readdata.lfsrnet.num = opt.lfsr_bits;
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
//	@name		：  ReadNeessary
//	@function	：	Reading File Necessary
//	@return		：	(bool) okay, error
//*************************************************************************************************************
/* 故障を (信号線名, 故障タイプ) で二分探索するための並び */
static int compare_fault_key(const void* left, const void* right)
{
    const UNDET_FAULT_LIST* a = *(const UNDET_FAULT_LIST* const*)left;
    const UNDET_FAULT_LIST* b = *(const UNDET_FAULT_LIST* const*)right;
    int by_name = strcmp(a->name, b->name);
    return by_name != 0 ? by_name : (a->type > b->type) - (a->type < b->type);
}

bool ReadNecessary(void)
{
    FILE* input = NULL;
    char* line;
    UNDET_FAULT_LIST** sorted;
    int matched_count = 0;
    int line_number = 0;
    int valid = 1;
    if (opt.necessary_file == NULL) {
        return READ_OKAY;
    }
    if (fopen_s(&input, opt.necessary_file, "r") != 0 || input == NULL) {
        printf("[ERROR] Cannot open the necessary-assignment file: %s\n", opt.necessary_file);
        return READ_ERROR;
    }
    line = (char*)allocMemory(MAXSIZE_BUFFER, 1);
    readdata.necenet.assignments = (char**)allocMemory(all_num, sizeof(char*));
    sorted = (UNDET_FAULT_LIST**)allocMemory(all_num, sizeof(*sorted));
    {
        int i = 0;
        for (UNDET_FAULT_LIST* fault = ufl; fault != NULL && i < all_num; fault = fault->next) sorted[i++] = fault;
    }
    qsort(sorted, all_num, sizeof(*sorted), compare_fault_key);
    while (fgets(line, MAXSIZE_BUFFER, input) != NULL) {
        char* context = NULL;
        char* name = strtok_s(line, ", \t\r\n", &context);
        char* type_name;
        char* assignments;
        UNDET_FAULT_LIST key;
        UNDET_FAULT_LIST* key_pointer = &key;
        UNDET_FAULT_LIST** found;
        line_number++;
        if (name == NULL) {
            continue;
        }
        type_name = strtok_s(NULL, ", \t\r\n", &context);
        assignments = strtok_s(NULL, "\r\n", &context);
        if (type_name == NULL || assignments == NULL) {
            printf("[ERROR] %s line %d: expected \"<signal> <SF0|SF1> <assignments>\".\n", opt.necessary_file, line_number);
            valid = 0;
            break;
        }
        memset(&key, 0, sizeof(key));
        strcpy_s(key.name, sizeof(key.name), name);
        if (strcmp(type_name, "SF0") == 0) {
            key.type = STR;
        } else if (strcmp(type_name, "SF1") == 0) {
            key.type = STF;
        } else {
            printf("[ERROR] %s line %d: unknown fault type %s (SF0 or SF1 expected).\n", opt.necessary_file, line_number, type_name);
            valid = 0;
            break;
        }
        found = (UNDET_FAULT_LIST**)bsearch(&key_pointer, sorted, all_num, sizeof(*sorted), compare_fault_key);
        if (found == NULL) {
            continue;   /* 対象の故障集合にない故障の必須割当ては使わない */
        }
        if (readdata.necenet.assignments[(*found)->node_id] != NULL) {
            printf("[ERROR] %s line %d: duplicate entry for %s %s.\n", opt.necessary_file, line_number, name, type_name);
            valid = 0;
            break;
        }
        readdata.necenet.assignments[(*found)->node_id] = _strdup(assignments);
        matched_count++;
    }
    readdata.necenet.num = all_num;
    free(sorted);
    free(line);
    fclose(input);
    if (!valid) {
        return READ_ERROR;
    }
    if (matched_count != all_num) {
        printf("[ERROR] %s has necessary assignments for only %d of the %d target faults.\n", opt.necessary_file, matched_count, all_num);
        return READ_ERROR;
    }
    return READ_OKAY;
}

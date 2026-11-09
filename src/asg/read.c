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
//#include "debug.h"


//*************************************************************************************************************
//	@name		：　ReadFile
//	@function	：	read the input files of the LFSR / phase-shifter model (-lfsr ON only)
//	@return		：	(bool) okay, error
//*************************************************************************************************************
bool ReadFile(
	void
)
{
	/** create the lfsr list */
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

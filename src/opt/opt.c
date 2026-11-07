//-------------------------------------------------------------------------------------------------------------
//	include
//-------------------------------------------------------------------------------------------------------------
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

#include "opt.h"
#include "../asg/read.h"
#include "../lib/lib.h"
//追加
#include "../header/Define.h"
//#include "debug.h"


static int integrated_mode_index(const char* option)
{
    if (strcmp(option, "-cps") == 0) return CPS_MODE;
    if (strcmp(option, "-fp") == 0) return FP_MODE;
    if (strcmp(option, "-resolving") == 0) return RESOLVING_MODE;
    if (strcmp(option, "-sttg_wsath") == 0) return STTG_WSATH_MODE;
    if (strcmp(option, "-regen") == 0) return REGEN_MODE;
    return -1;
}

/* Integrated ON/OFF modes that were not given default to OFF. */
static void set_integrated_mode_defaults(void)
{
    const int modes[] = { CPS_MODE, FP_MODE, RESOLVING_MODE, STTG_WSATH_MODE, REGEN_MODE };
    for (size_t i = 0; i < sizeof(modes) / sizeof(modes[0]); i++) {
        if (fi[modes[i]].flag == OFF) {
            fi[modes[i]].name = "OFF";
        }
    }
}

//*************************************************************************************************************
//	@name		：　OPT
//	@function	：	analyze the option
//	@return		：	(bool) okay, error
//*************************************************************************************************************
bool OPT(
	int					  argc,			      /**< number of command-arguments */
	char** argv			      /**< command-arguments */
)
{
	/** initialize the options */
	OPTinit();

	/** set the options */
	if (OPTset(argc, argv) != OPT_OKAY)
	{
		/** display the help */
		OPTdispHelp();

		return OPT_ERROR;
	}

	/** check for essential options */
	if (OPTcheck() != OPT_OKAY)
	{
		/** display the help */
		OPTdispHelp();

		return OPT_ERROR;
	}


#ifdef __DEBUG_OPT__
	_CALL_DEBUG_OPT_(argc);
#endif


	printf("	Setup options completed ... \n");

	return OPT_OKAY;
}

//*************************************************************************************************************
//	@name		：　OPTinit
//	@function	：	initialize the options
//	@return		：	(void)
//*************************************************************************************************************
void OPTinit(
	void
)
{
	/** initialize the filename */
	OPTinitFile();

	/** initialize the mode */
	OPTinitMode();

	return;
}

//*************************************************************************************************************
//	@name		：　OPTinitFile
//	@function	：	initialize the filename
//	@return		：	(void)
//*************************************************************************************************************
void OPTinitFile(
	void
)
{
	opt.file.input.fault = FILE_NOSET;
	opt.file.input.net = FILE_NOSET;
	opt.file.input.scanchain = FILE_NOSET;
	opt.file.input.necessary = FILE_NOSET;
	opt.file.input.xortap = FILE_NOSET;
	opt.file.input.lfsr = 100;
	opt.file.input.loop = 0;

	opt.file.output.graphlog = FILE_NOSET;
	opt.file.output.log = FILE_NOSET;
	opt.file.output.pin = FILE_NOSET;
	opt.file.output.seed = FILE_NOSET;
	opt.file.output.test = FILE_NOSET;
	opt.file.output.untestable = FILE_NOSET;

	return;
}

//*************************************************************************************************************
//	@name		：　OPTinitMode
//	@function	：	initialize the mode
//	@return		：	(void)
//*************************************************************************************************************
void OPTinitMode(
	void
)
{
	opt.mode.drop = DROP_YES;

	return;
}

//*************************************************************************************************************
//	@name		：　OPTparseInt
//	@function	：	convert the option value to an integer (the whole value must be a number)
//	@return		：	(bool) okay, error
//*************************************************************************************************************
static bool OPTparseInt(const char* option, const char* value, int* result)
{
	char* end = NULL;
	long number;

	errno = 0;
	number = strtol(value, &end, 10);
	if (*value == '\0' || *end != '\0' || errno == ERANGE || number < INT_MIN || number > INT_MAX)
	{
		printf("\n	COMMAND ERROR: option setup is failed. ");
		printf("%c%s%c requires an integer, but %c%s%c was given.\n\n", '"', option, '"', '"', value, '"');
		return OPT_ERROR;
	}
	*result = (int)number;
	return OPT_OKAY;
}

//*************************************************************************************************************
//	@name		：　OPTapply
//	@function	：	apply one "option value" pair.
//					The command line and the setting file (.set) share this function,
//					so both accept the same options.
//	@return		：	(bool) okay, error
//*************************************************************************************************************
static bool OPTapply(const char* name, const char* value)
{
	int mode_index;

	if (value == NULL)
	{
		printf("[ERROR] Missing value for %s.\n", name);
		return OPT_ERROR;
	}

	/* 回路ファイル名 */
	if (!strcmp(name, "-net")) {
		fi[CIRCUIT].name = _strdup(value);
		fi[CIRCUIT].flag = ON;
	}
	/* 低消費電力モード */
	else if (!strcmp(name, "-power")) {
		fi[LOW_POWER].name = _strdup(value);
		fi[LOW_POWER].flag = ON;
	}
	/* 出力フォルダー */
	else if (!strcmp(name, "-folder")) {
		fi[FOLDER].name = _strdup(value);
		fi[FOLDER].flag = ON;
	}
	/* ランダム多重故障選択(default:OFF) */
	else if (!strcmp(name, "-rand")) {
		fi[RAND].name = _strdup(value);
		fi[RAND].flag = ON;
	}
	/* Solver打ち切り時間(default:10) */
	else if (!strcmp(name, "-time")) {
		if (OPTparseInt(name, value, &fi[TIME].value) != OPT_OKAY) return OPT_ERROR;
		fi[TIME].flag = ON;
	}
	/* WSA閾値＝消費電力閾値(default:DEFAULT_WSA_THRESHOLD) */
	else if (!strcmp(name, "-wsath")) {
		if (OPTparseInt(name, value, &fi[WSATH].value) != OPT_OKAY) return OPT_ERROR;
		fi[WSATH].flag = ON;
	}
	/* 目標故障の上限(0の場合は制限なし) */
	else if (!strcmp(name, "-target_limit")) {
		if (OPTparseInt(name, value, &fi[TARGET_LIMIT].value) != OPT_OKAY) return OPT_ERROR;
		fi[TARGET_LIMIT].flag = ON;
	}
	/* 故障ファイル */
	else if (!strcmp(name, "-fault")) {
		fi[FAULT].name = _strdup(value);
		fi[FAULT].flag = ON;
	}
	/* CLASP --mode 1 指定 */
	else if (!strcmp(name, "-mode_1")) {
		fi[MODE_1].name = _strdup(value);
		fi[MODE_1].flag = ON;
	}
	/* CLASP PARALLELモード 指定 */
	else if (!strcmp(name, "-parallel")) {
		if (OPTparseInt(name, value, &fi[PARALLEL].value) != OPT_OKAY) return OPT_ERROR;
		fi[PARALLEL].flag = ON;
	}
	/* 実験モード(-cps, -fp, -resolving, -sttg_wsath, -regen): ON/OFF (default:OFF) */
	else if ((mode_index = integrated_mode_index(name)) >= 0) {
		if (strcmp(value, "ON") != 0 && strcmp(value, "OFF") != 0) {
			printf("%s requires ON or OFF.\n", name);
			return OPT_ERROR;
		}
		fi[mode_index].name = _strdup(value);
		fi[mode_index].flag = ON;
	}
	/* scan-chain */
	else if (!strcmp(name, "-scan_chain"))
		opt.file.input.scanchain = _strdup(value);
	/* xor-tap */
	else if (!strcmp(name, "-xor_tap"))
		opt.file.input.xortap = _strdup(value);
	/* 必須割り当て情報ファイル */
	else if (!strcmp(name, "-nece"))
		opt.file.input.necessary = _strdup(value);
	/* lfsr bits */
	else if (!strcmp(name, "-b")) {
		if (OPTparseInt(name, value, &opt.file.input.lfsr) != OPT_OKAY) return OPT_ERROR;
	}
	/* log */
	else if (!strcmp(name, "-l"))
		opt.file.output.log = _strdup(value);
	/* seed */
	else if (!strcmp(name, "-seed"))
		opt.file.output.seed = _strdup(value);
	/* test pattern */
	else if (!strcmp(name, "-t"))
		opt.file.output.test = _strdup(value);
	/* untestable fault */
	else if (!strcmp(name, "-u"))
		opt.file.output.untestable = _strdup(value);
	/* fault drop */
	else if (!strcmp(name, "-d")) {
		if (!strcmp(value, "N"))
			opt.mode.drop = DROP_NO;
		else if (!strcmp(value, "Y"))
			opt.mode.drop = DROP_YES;
		else {
			printf("\n	COMMAND ERROR: option setup is failed. ");
			printf("%c-d=%s%c is not expected.\n\n", '"', value, '"');
			return OPT_ERROR;
		}
	}
	else {
		printf("\n	COMMAND ERROR: option setup is failed. ");
		printf("%c%s%c is not expected.\n\n", '"', name, '"');
		return OPT_ERROR;
	}

	return OPT_OKAY;
}

//*************************************************************************************************************
//	@name		：　OPTfinalize
//	@function	：	check the options that must be given, and set the default values.
//					Called after both the command line and the setting file were read.
//	@return		：	(bool) okay, error
//*************************************************************************************************************
static bool OPTfinalize(void)
{
	/* 必要コマンドの入力確認 */
	if (fi[CIRCUIT].flag == OFF) { printf("回路ファイル名を入力してください\n");		return OPT_ERROR; }
	if (fi[LOW_POWER].flag == OFF) { printf("低消費電力指向か否かを入力してください\n");		return OPT_ERROR; }
	if (fi[FOLDER].flag == OFF) { printf("出力フォルダー名を入力してください\n");		return OPT_ERROR; }

	/* defaultの設定 */
	if (fi[RAND].flag == OFF) {
		fi[RAND].name = "OFF";
	}
	if (fi[TIME].flag == OFF) {
		fi[TIME].value = 10;
	}
	if (fi[MODE_1].flag == OFF) {
		fi[MODE_1].name = "OFF";
	}
	if (fi[WSATH].flag == OFF) {
		fi[WSATH].value = DEFAULT_WSA_THRESHOLD;
	}
	set_integrated_mode_defaults();

	return OPT_OKAY;
}

//*************************************************************************************************************
//	@name		：　OPTset
//	@function	：	set the options.
//					"-set <file>" reads the setting file at that position;
//					options written after it override the file (later wins).
//	@return		：	(bool) okay, error
//*************************************************************************************************************
bool OPTset(
	int				      argc,				  /**< number of command-arguments */
	char** argv				  /**< command-arguments */
)
{
	for (int i = 1; i < argc; i++)
	{
		/** help */
		if (strcmp(argv[i], "-help") == 0)
			return OPT_ERROR;

		if (i + 1 >= argc) {
			printf("[ERROR] Missing value for %s.\n", argv[i]);
			return OPT_ERROR;
		}

		/** read the option */
		if (strcmp(argv[i], "-set") == 0) {
			if (OPTread(argv[i + 1]) != OPT_OKAY) return OPT_ERROR;
		}
		else if (OPTapply(argv[i], argv[i + 1]) != OPT_OKAY) {
			return OPT_ERROR;
		}
		i++; /* skip the value */
	}

	return OPTfinalize();
}

//*************************************************************************************************************
//	@name		：　OPTread
//	@function	：	read the option-setting file.
//					A line that starts with '-' is "option value"; other lines are comments.
//					Both LF and CRLF line ends are accepted.
//	@return		：	(bool) okay, error
//*************************************************************************************************************
bool OPTread(
	char* filename			  /**< filename */
)
{
	FILE* fileptr = (FILE*)NULL;
	char* buffer = (char*)NULL;
	bool result = OPT_OKAY;

	/** open the "setting file" in read-mode */
	fileOpen(&fileptr, filename, "r");

	buffer = allocMemory(MAXSIZE_BUFFER, sizeof(char));

	/** read the option */
	while (COMP_EOF(fgets(buffer, MAXSIZE_BUFFER, fileptr)))
	{
		char* context = (char*)NULL;
		char* name;
		char* value;

		if (buffer[0] != '-')
			continue;

		name = strtok_s(buffer, " \t\r\n", &context);
		/* the value is the first word after the option name */
		value = strtok_s(NULL, " \t\r\n", &context);

		if (strcmp(name, "-help") == 0 || OPTapply(name, value) != OPT_OKAY) {
			result = OPT_ERROR;
			break;
		}
	}

	free(buffer);
	fclose(fileptr);

	return result;
}

//*************************************************************************************************************
//	@name		：　OPTcheck
//	@function	：	check for essential options
//	@return		：	(bool) okay, error
//*************************************************************************************************************
bool OPTcheck(
	void
)
{
	if (OPTcheckMode() != OPT_OKAY)	return OPT_ERROR;

	if (OPTcheckFile() != OPT_OKAY)	return OPT_ERROR;

	return OPT_OKAY;
}

//*************************************************************************************************************
//	@name		：　OPTcheckMode
//	@function	：	check for essential mode
//	@return		：	(bool) okay, error
//*************************************************************************************************************
bool OPTcheckMode(
	void
)
{
	if (opt.file.input.lfsr != 100) {
		printf("[ERROR] This circuit encoding currently requires a 100-bit LFSR.\n");
		return OPT_ERROR;
	}
	if (fi[TIME].value <= 0 || fi[WSATH].value < 0 || fi[WSATH].value > 100 ||
		fi[TARGET_LIMIT].value < 0 || fi[FAULT].flag != ON ||
		(strcmp(fi[LOW_POWER].name, "ON") != 0 && strcmp(fi[LOW_POWER].name, "OFF") != 0)) {
		printf("[ERROR] Invalid time, threshold, target limit, power mode or fault file.\n");
		return OPT_ERROR;
	}
	if (strcmp(fi[REGEN_MODE].name, "ON") == 0 &&
		(strcmp(fi[RESOLVING_MODE].name, "ON") == 0 || strcmp(fi[STTG_WSATH_MODE].name, "ON") == 0)) {
		printf("[ERROR] -regen cannot be combined with -resolving or -sttg_wsath.\n");
		return OPT_ERROR;
	}
#ifdef NDEBUG
	assert(opt.mode.drop == DROP_NO || opt.mode.drop == DROP_YES);
#endif

	if (opt.mode.drop != DROP_NO && opt.mode.drop != DROP_YES)
	{
		printf("\n	COMMAND ERROR: option setup is failed. ");
		printf("%c-d%c is not expected.\n\n", '"', '"');
		return OPT_ERROR;
	}

	return	OPT_OKAY;
}

//*************************************************************************************************************
//	@name		：　OPTcheckFile
//	@function	：	check for essential file
//	@return		：	(bool) okay, error
//*************************************************************************************************************
bool OPTcheckFile(
	void
)
{
	if (opt.file.input.scanchain == FILE_NOSET)
	{
		printf("\n	COMMAND ERROR: option setup is failed. ");
		printf("no scan-chain file.\n\n");
		return OPT_ERROR;
	}

	if (opt.file.input.xortap == FILE_NOSET)
	{
		printf("\n	COMMAND ERROR: option setup is failed. ");
		printf("no xor-tap file.\n\n");
		return OPT_ERROR;
	}

	if (opt.file.output.seed == FILE_NOSET)
	{
		printf("\n	COMMAND ERROR: option setup is failed. ");
		printf("no seed set file.\n\n");
		return OPT_ERROR;
	}


	return	OPT_OKAY;
}

//*************************************************************************************************************
//	@name		：　OPTdispHelp
//	@function	：	display the help
//	@return		：	(void)
//*************************************************************************************************************
void OPTdispHelp(
	void
)
{
	PrintHelpMessage("\n	> help ***********************************************************************************\n\n");

	PrintHelpMessage("		command = urashima -n <.v> -f <.txt> -p <.txt> -t <.txt>\n");

	PrintHelpMessage("\n		>> file option \n");

	PrintHelpMessage("		   -n(essential)   :   netlist <.v> \n");
	PrintHelpMessage("		   -f(essential)   :   fault list <.txt> \n");
	PrintHelpMessage("		   -x(essential)   :   xor-tap list <.txt> \n");
	PrintHelpMessage("		   -c(essential)   :   scan-chain <.txt> \n");

	PrintHelpMessage("		   -l              :   log file <.txt>\n");
	PrintHelpMessage("		   -p(essential)   :   pin file <.txt>\n");
	PrintHelpMessage("		   -s(essential)   :   seed set file <.txt> \n");
	PrintHelpMessage("		   -t              :   test set file <.txt> \n");
	PrintHelpMessage("		   -u              :   untestable fault list <.txt> \n");

	PrintHelpMessage("		   -set            :   setting options by file <.set>. Options after it override the file. \n");

	PrintHelpMessage("\n		>> mode option \n");
	PrintHelpMessage("		   -d              :   fault drop [Y(YES)/N(NO)]. %cY%c is default. \n", '"', '"');
	PrintHelpMessage("\t\t   -cps            :   experimental CPS mode [ON/OFF]. OFF is default. \n");

	PrintHelpMessage("\t\t   -fp             :   fault-propagation constraints [ON/OFF]. OFF is default. \n");
	PrintHelpMessage("\t\t   -resolving      :   retry clasp UNKNOWN with 60/180/300 sec [ON/OFF]. OFF is default. \n");
	PrintHelpMessage("\t\t   -sttg_wsath     :   retry the aborted primary fault alone (STTG), then with WSA threshold +5%%/+10%% [ON/OFF]. OFF is default. \n");
	PrintHelpMessage("\t\t   -regen          :   paper regeneration flow: clasp 60/180/300 sec, then WSA threshold +5%%/+10%% [ON/OFF]. OFF is default. \n");
	PrintHelpMessage("\n		>> help \n");
	PrintHelpMessage("		   -help           :   print the help. \n");

	PrintHelpMessage("\n");

		return;
}










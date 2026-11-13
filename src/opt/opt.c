//-------------------------------------------------------------------------------------------------------------
//	オプションの解析
//	  コマンドラインと設定ファイル(.set)は同じ OPTapply() で処理するので、同じオプションが使える。
//-------------------------------------------------------------------------------------------------------------
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

#include "opt.h"
#include "../asg/read.h"
#include "../lib/lib.h"

OPTION opt;

/* -power は必須。指定の有無を区別するため、-power だけ別に記録する */
static bool power_given;

//-------------------------------------------------------------------------------------------------------------
//	値の変換
//-------------------------------------------------------------------------------------------------------------
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

static bool OPTparseOnOff(const char* option, const char* value, bool* result)
{
	if (strcmp(value, "ON") == 0)  { *result = true;  return OPT_OKAY; }
	if (strcmp(value, "OFF") == 0) { *result = false; return OPT_OKAY; }
	printf("\n	COMMAND ERROR: option setup is failed. ");
	printf("%c%s%c requires ON or OFF, but %c%s%c was given.\n\n", '"', option, '"', '"', value, '"');
	return OPT_ERROR;
}

const char* OPTonoff(bool value)
{
	return value ? "ON" : "OFF";
}

//-------------------------------------------------------------------------------------------------------------
//	"オプション名 値" を1組処理する
//-------------------------------------------------------------------------------------------------------------
static bool OPTapply(const char* name, const char* value)
{
	if (value == NULL)
	{
		printf("[ERROR] Missing value for %s.\n", name);
		return OPT_ERROR;
	}
	if (strcmp(name, "-power") == 0) power_given = true;

	/* 入力ファイル・出力先 */
	if (!strcmp(name, "-net"))              opt.circuit = _strdup(value);
	else if (!strcmp(name, "-fault"))       opt.fault_file = _strdup(value);
	else if (!strcmp(name, "-xor_tap"))     opt.xor_tap_file = _strdup(value);
	else if (!strcmp(name, "-scan_chain"))  opt.scan_chain_file = _strdup(value);
	else if (!strcmp(name, "-folder"))      opt.folder = _strdup(value);
	/* 生成するもの */
	else if (!strcmp(name, "-lfsr"))        return OPTparseOnOff(name, value, &opt.lfsr);
	else if (!strcmp(name, "-untest_mode")) return OPTparseOnOff(name, value, &opt.untest_mode);
	/* 低消費電力 */
	else if (!strcmp(name, "-power"))       return OPTparseOnOff(name, value, &opt.low_power);
	else if (!strcmp(name, "-wsath"))       return OPTparseInt(name, value, &opt.wsa_threshold);
	/* ソルバ */
	else if (!strcmp(name, "-time"))        return OPTparseInt(name, value, &opt.solver_time);
	else if (!strcmp(name, "-parallel"))    return OPTparseInt(name, value, &opt.parallel_threads);
	else if (!strcmp(name, "-mode_1"))      return OPTparseOnOff(name, value, &opt.first_model_only);
	/* 目標故障選択 */
	else if (!strcmp(name, "-target_limit")) return OPTparseInt(name, value, &opt.target_limit);
	/* 再生成 */
	else if (!strcmp(name, "-regen"))       return OPTparseOnOff(name, value, &opt.regen);
	else if (!strcmp(name, "-sttg"))        return OPTparseOnOff(name, value, &opt.sttg);
	/* 高速化 */
	else if (!strcmp(name, "-const"))       return OPTparseOnOff(name, value, &opt.constants);
	else if (!strcmp(name, "-learn"))       return OPTparseOnOff(name, value, &opt.learn);
	else if (!strcmp(name, "-scope"))       return OPTparseOnOff(name, value, &opt.scope);
	/* 実験的な機能 */
	else if (!strcmp(name, "-fp"))          return OPTparseOnOff(name, value, &opt.fault_propagation);
	else if (!strcmp(name, "-cps"))         return OPTparseOnOff(name, value, &opt.cps);
	else if (!strcmp(name, "-b"))           return OPTparseInt(name, value, &opt.lfsr_bits);
	/* 指定しても使われないオプション(既存のスクリプトを動かすため、警告だけにする) */
	else if (!strcmp(name, "-seed"))
		printf("[WARNING] %s is ignored. Seeds are always written to <folder>/seed.txt.\n", name);
	else if (!strcmp(name, "-rand"))
		printf("[WARNING] %s is ignored. Targets are selected from the compatible fault sets.\n", name);
	else if (!strcmp(name, "-nece"))
		printf("[WARNING] %s is ignored. Necessary assignments are computed from the circuit.\n", name);
	/* 廃止したオプション */
	else if (!strcmp(name, "-resolving"))
	{
		printf("[ERROR] %s was removed. Use -regen (clasp time limit 60/180/300 s, then relax the WSA threshold).\n", name);
		return OPT_ERROR;
	}
	else if (!strcmp(name, "-sttg_wsath"))
	{
		printf("[ERROR] %s was removed. Use -sttg (retry the primary fault alone). Relaxing the WSA threshold is done only by -regen.\n", name);
		return OPT_ERROR;
	}
	else
	{
		printf("\n	COMMAND ERROR: option setup is failed. ");
		printf("%c%s%c is not expected.\n\n", '"', name, '"');
		return OPT_ERROR;
	}

	return OPT_OKAY;
}

//-------------------------------------------------------------------------------------------------------------
//	初期値・既定値
//	  数値は「未指定」を -1 で区別して、指定されなかったものを既定値にする。
//-------------------------------------------------------------------------------------------------------------
static void OPTinit(void)
{
	memset(&opt, 0, sizeof(opt));
	opt.lfsr = true;
	opt.scope = true;
	opt.constants = false;
	opt.solver_time = -1;
	opt.wsa_threshold = -1;
	opt.target_limit = -1;
	opt.parallel_threads = -1;
	opt.lfsr_bits = -1;
	power_given = false;
}

static void OPTsetDefaults(void)
{
	if (opt.solver_time < 0)      opt.solver_time = DEFAULT_SOLVER_TIME;
	if (opt.wsa_threshold < 0)    opt.wsa_threshold = DEFAULT_WSA_THRESHOLD;
	if (opt.target_limit < 0)     opt.target_limit = 0;
	if (opt.parallel_threads < 0) opt.parallel_threads = 0;
	if (opt.lfsr_bits < 0)        opt.lfsr_bits = LFSR_BITS;
}

//-------------------------------------------------------------------------------------------------------------
//	設定ファイル
//	  '-' で始まる行が "オプション名 値"。それ以外の行はコメント。LF・CRLF のどちらも読める。
//-------------------------------------------------------------------------------------------------------------
bool OPTread(char* filename)
{
	FILE* fileptr = (FILE*)NULL;
	char* buffer = (char*)NULL;
	bool result = OPT_OKAY;

	fileOpen(&fileptr, filename, "r");
	buffer = allocMemory(MAXSIZE_BUFFER, sizeof(char));

	while (fgets(buffer, MAXSIZE_BUFFER, fileptr) != NULL)
	{
		char* context = (char*)NULL;
		char* name;
		char* value;

		if (buffer[0] != '-')
			continue;

		name = strtok_s(buffer, " \t\r\n", &context);
		/* 値は、オプション名の次の1語 */
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

//-------------------------------------------------------------------------------------------------------------
//	コマンドライン
//	  "-set <file>" はその位置で設定ファイルを読み込む。後ろに書いた指定が優先される。
//-------------------------------------------------------------------------------------------------------------
bool OPTset(int argc, char** argv)
{
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-help") == 0)
			return OPT_ERROR;

		if (i + 1 >= argc) {
			printf("[ERROR] Missing value for %s.\n", argv[i]);
			return OPT_ERROR;
		}

		if (strcmp(argv[i], "-set") == 0) {
			if (OPTread(argv[i + 1]) != OPT_OKAY) return OPT_ERROR;
		}
		else if (OPTapply(argv[i], argv[i + 1]) != OPT_OKAY) {
			return OPT_ERROR;
		}
		i++; /* 値を読み飛ばす */
	}
	return OPT_OKAY;
}

//-------------------------------------------------------------------------------------------------------------
//	検査: 必須オプション、値の範囲、入力ファイル
//-------------------------------------------------------------------------------------------------------------
static bool OPTcheckInputFile(const char* option, const char* path)
{
	FILE* file = NULL;
	if (fopen_s(&file, path, "r") != 0 || file == NULL)
	{
		printf("[ERROR] Cannot open the input file of %s: %s\n", option, path);
		return OPT_ERROR;
	}
	fclose(file);
	return OPT_OKAY;
}

bool OPTcheck(void)
{
	/* 必須オプション */
	if (opt.circuit == NULL)         { printf("回路ファイル名を入力してください (-net)\n");                  return OPT_ERROR; }
	if (opt.fault_file == NULL)      { printf("故障ファイル名を入力してください (-fault)\n");                return OPT_ERROR; }
	if (opt.lfsr && opt.scan_chain_file == NULL) { printf("スキャンチェーンファイル名を入力してください (-scan_chain。-lfsr OFF なら不要)\n"); return OPT_ERROR; }
	if (opt.lfsr && opt.xor_tap_file == NULL)    { printf("XORタップファイル名を入力してください (-xor_tap。-lfsr OFF なら不要)\n");          return OPT_ERROR; }
	if (opt.folder == NULL)          { printf("出力フォルダー名を入力してください (-folder)\n");              return OPT_ERROR; }
	if (!power_given)                { printf("低消費電力指向か否かを入力してください (-power ON/OFF)\n");    return OPT_ERROR; }

	OPTsetDefaults();

	/* 値の範囲 */
	if (opt.lfsr_bits != LFSR_BITS) {
		printf("[ERROR] -b %d: this circuit encoding currently requires a %d-bit LFSR.\n", opt.lfsr_bits, LFSR_BITS);
		return OPT_ERROR;
	}
	if (opt.solver_time <= 0) {
		printf("[ERROR] -time must be a positive number of seconds (given: %d).\n", opt.solver_time);
		return OPT_ERROR;
	}
	if (opt.wsa_threshold < 0 || opt.wsa_threshold > 100) {
		printf("[ERROR] -wsath must be between 0 and 100 (given: %d).\n", opt.wsa_threshold);
		return OPT_ERROR;
	}
	if (opt.target_limit < 0 || opt.parallel_threads < 0) {
		printf("[ERROR] -target_limit and -parallel must not be negative.\n");
		return OPT_ERROR;
	}
	if (opt.regen && !opt.low_power) {
		printf("[WARNING] -regen without -power ON only extends the clasp time limit (there is no WSA threshold to relax).\n");
	}

	/* 入力ファイル(実行の最後で見つかるより、最初に分かるほうがよい) */
	if (OPTcheckInputFile("-net", opt.circuit) != OPT_OKAY) return OPT_ERROR;
	if (OPTcheckInputFile("-fault", opt.fault_file) != OPT_OKAY) return OPT_ERROR;
	if (opt.xor_tap_file != NULL && OPTcheckInputFile("-xor_tap", opt.xor_tap_file) != OPT_OKAY) return OPT_ERROR;
	if (opt.scan_chain_file != NULL && OPTcheckInputFile("-scan_chain", opt.scan_chain_file) != OPT_OKAY) return OPT_ERROR;
	if (!opt.lfsr && (opt.xor_tap_file != NULL || opt.scan_chain_file != NULL)) {
		printf("[WARNING] -xor_tap and -scan_chain are not used with -lfsr OFF.\n");
	}

	return OPT_OKAY;
}

//-------------------------------------------------------------------------------------------------------------
//	ヘルプ
//-------------------------------------------------------------------------------------------------------------
void OPTdispHelp(void)
{
	printf("\n	> help ***********************************************************************************\n\n");
	printf("		command = TDF_ASG.exe -net <.v> -fault <.txt> -xor_tap <.txt> -scan_chain <.txt> -folder <dir> -power <ON|OFF>\n");
	printf("		          TDF_ASG.exe -set <.set>   (options in the file; options written after -set override it)\n");

	printf("\n		>> files \n");
	printf("		   -net(essential)         :   netlist <.v> \n");
	printf("		   -fault(essential)       :   fault list <.txt> \n");
	printf("		   -xor_tap(essential)     :   xor-tap list <.txt> (not needed with -lfsr OFF) \n");
	printf("		   -scan_chain(essential)  :   scan-chain <.txt> (not needed with -lfsr OFF) \n");
	printf("		   -folder(essential)      :   output folder (must not exist) \n");
	printf("		   -set                    :   setting file <.set> \n");

	printf("\n		>> what to generate \n");
	printf("		   -lfsr                   :   [ON/OFF] ON (default): seeds of the LFSR + phase shifter. OFF: test patterns only (ATPG) \n");
	printf("		   -untest_mode            :   [ON/OFF] first check every fault alone and remove the untestable ones. OFF is default. \n");

	printf("\n		>> test generation \n");
	printf("		   -power(essential)       :   [ON/OFF] keep the capture power (WSA) below the threshold \n");
	printf("		   -wsath                  :   WSA threshold [%%]. %d is default. \n", DEFAULT_WSA_THRESHOLD);
	printf("		   -target_limit           :   upper limit of target faults per seed (0: no limit) \n");
	printf("		   -time                   :   clasp time limit [s]. %d is default. \n", DEFAULT_SOLVER_TIME);
	printf("		   -parallel               :   clasp threads \n");
	printf("		   -mode_1                 :   [ON/OFF] stop at the first model. OFF is default. \n");

	printf("\n		>> regeneration for a primary fault that clasp could not solve (all OFF by default) \n");
	printf("		   -regen                  :   [ON/OFF] clasp time limit 60/180/300 s, then relax the WSA threshold by +5%%/+10%% \n");
	printf("		   -sttg                   :   [ON/OFF] retry the primary fault alone (STTG). The WSA threshold is not changed. \n");

	printf("\n		>> speed \n");
	printf("		   -scope                  :   [ON/OFF] without the power constraint, write only the part of the good circuit the target faults use. ON is default. \n");

	printf("		   -const                  :   [ON/OFF] find signals that are always the same value and use them in the necessary assignments. OFF is default (no measured effect). \n");

	printf("		   -learn                  :   [ON/OFF] use static learning (indirect implications) in the necessary assignments. OFF is default. \n");

	printf("\n		>> experimental (all OFF by default) \n");
	printf("		   -fp                     :   [ON/OFF] fault-propagation variables for the primary fault \n");
	printf("		   -cps                    :   [ON/OFF] if a seed detects no fault, use a single target next \n");

	printf("\n		>> help \n");
	printf("		   -help                   :   print the help. \n\n");
}

//-------------------------------------------------------------------------------------------------------------
//	全体
//-------------------------------------------------------------------------------------------------------------
bool OPT(int argc, char** argv)
{
	OPTinit();

	if (OPTset(argc, argv) != OPT_OKAY || OPTcheck() != OPT_OKAY)
	{
		OPTdispHelp();
		return OPT_ERROR;
	}

	printf("	Setup options completed ... \n");
	return OPT_OKAY;
}

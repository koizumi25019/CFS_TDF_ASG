//-------------------------------------------------------------------------------------------------------------
//	include
//-------------------------------------------------------------------------------------------------------------
#include <assert.h>
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

static bool set_integrated_mode(int argc, char** argv, int* index, int mode_index)
{
    const char* option = argv[*index];

    (*index)++;
    if (*index >= argc ||
        (strcmp(argv[*index], "ON") != 0 && strcmp(argv[*index], "OFF") != 0)) {
        printf("%s requires ON or OFF.\n", option);
        return OPT_ERROR;
    }

    fi[mode_index].name = argv[*index];
    fi[mode_index].flag = ON;
    return OPT_OKAY;
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
//	@name		：　OPTset
//	@function	：	set the options
//	@return		：	(bool) okay, error
//*************************************************************************************************************
bool OPTset(
	int				      argc,				  /**< number of command-arguments */
	char** argv				  /**< command-arguments */
)
{
	for (int i = 1; i < argc; i++)
	{
        if (strcmp(argv[i], "-help") != 0 && i + 1 >= argc) {
            printf("[ERROR] Missing value for %s.\n", argv[i]);
            return OPT_ERROR;
        }
        int integrated_mode = integrated_mode_index(argv[i]);
		/** help */
		if (strcmp(argv[i], "-help") == 0)
			return OPT_ERROR;

		//追加
		//------------------------------------------------------------------------
		// 入力ファイル
		//------------------------------------------------------------------------

		/** read the option */
		else if (strcmp(argv[i], "-set") == 0)
			return OPTread(argv[++i]);


		/* 回路ファイル名の登録 */
		else if (!strcmp(argv[i], "-net")) {
			i++;
			fi[CIRCUIT].name = argv[i];
			fi[CIRCUIT].flag = ON;
		}

		//------------------------------------------------------------------------
		// オプション
		//------------------------------------------------------------------------

		/* 低消費電力モード取得 */
		else if (!strcmp(argv[i], "-power")) {
			i++;
			fi[LOW_POWER].name = argv[i];
			fi[LOW_POWER].flag = ON;
		}

		/* フォルダーの指定 */
		else if (!strcmp(argv[i], "-folder")) {
			i++;
			fi[FOLDER].name = argv[i];
			fi[FOLDER].flag = ON;
		}

		/* ランダム多重故障選択(default:OFF)取得 */
		else if (!strcmp(argv[i], "-rand")) {
			i++;
			fi[RAND].name = argv[i];
			fi[RAND].flag = ON;
		}

		/* Solver打ち切り時間(default:10)取得 */
		else if (!strcmp(argv[i], "-time")) {
			i++;
			fi[TIME].value = atoi(argv[i]);
			fi[TIME].flag = ON;
		}

		/* WSA閾値＝消費電力閾値(default:20)取得 */
		else if (!strcmp(argv[i], "-wsath")) {
			i++;
			fi[WSATH].value = atoi(argv[i]);
			fi[WSATH].flag = ON;
		}

		/* 目標故障の上限取得(0の場合は制限なし) */
		else if (!strcmp(argv[i], "-target_limit")) {
			i++;
			fi[TARGET_LIMIT].value = atoi(argv[i]);
			fi[TARGET_LIMIT].flag = ON;
		}

		/*故障ファイル取得*/
		else if (!strcmp(argv[i], "-fault")) {
			i++;
			fi[FAULT].name = argv[i];
			fi[FAULT].flag = ON;
		}

		/* CLASP --mode 1 指定 */
		else if (!strcmp(argv[i], "-mode_1")) {
			i++;
			fi[MODE_1].name = argv[i];
			fi[MODE_1].flag = ON;
		}

		/* CLASP PARALLELモード 指定 */
		else if (!strcmp(argv[i], "-parallel")) {
			i++;
			fi[PARALLEL].value = atoi(argv[i]);
			fi[PARALLEL].flag = ON;
		}

        /* Integrated experimental modes are OFF by default. */
        else if (integrated_mode >= 0) {
            if (set_integrated_mode(argc, argv, &i, integrated_mode) != OPT_OKAY) {
                return OPT_ERROR;
            }
        }

		/** scan-chain */
		else if (strcmp(argv[i], "-scan_chain") == 0)
			opt.file.input.scanchain = _strdup(argv[++i]);

		/** xor-tap */
		else if (strcmp(argv[i], "-xor_tap") == 0)
			opt.file.input.xortap = _strdup(argv[++i]);

		/** necessary */
		else if (strcmp(argv[i], "-nece") == 0)
			opt.file.input.necessary = _strdup(argv[++i]);

		/** lfsr bits */
		else if (strcmp(argv[i], "-b") == 0)
			opt.file.input.lfsr = atoi(argv[++i]);

		/** log */
		else if (strcmp(argv[i], "-l") == 0)
			opt.file.output.log = _strdup(argv[++i]);

		/** seed */
		else if (strcmp(argv[i], "-seed") == 0)
			opt.file.output.seed = _strdup(argv[++i]);

		/** test pattern */
		else if (strcmp(argv[i], "-t") == 0)
			opt.file.output.test = _strdup(argv[++i]);

		/** untestable fault */
		else if (strcmp(argv[i], "-u") == 0)
			opt.file.output.untestable = _strdup(argv[++i]);

		/** fault drop */
		else if (strcmp(argv[i], "-d") == 0)
		{
			if (strcmp(argv[++i], "N") == 0)
				opt.mode.drop = DROP_NO;

			else if (strcmp(argv[i], "Y") == 0)
				opt.mode.drop = DROP_YES;

			else
			{
				printf("\n	COMMAND ERROR: option setup is failed. ");
				printf("%c-d=%s%c is not expected.\n\n", '"', argv[i], '"');
				return OPT_ERROR;
			}
		}
	}

	//------------------------------------------------------------------------
	// 必要コマンドの入力確認
	//------------------------------------------------------------------------
	if (fi[CIRCUIT].flag == OFF) { printf("回路ファイル名を入力してください\n");		return OPT_ERROR; }
	if (fi[LOW_POWER].flag == OFF) { printf("低消費電力指向か否かを入力してください\n");		return OPT_ERROR; }
	if (fi[FOLDER].flag == OFF) { printf("出力フォルダー名を入力してください\n");		return OPT_ERROR; }
	//------------------------------------------------------------------------
	// defaultの設定
	//------------------------------------------------------------------------
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
//	@name		：　OPTread
//	@function	：	read the option-setting file
//	@return		：	(bool) okay, error
//*************************************************************************************************************
bool OPTread(
	char* filename			  /**< filename */
)
{
	FILE* fileptr = (FILE*)NULL;
	char* buffer = (char*)NULL;
	char* context = (char*)NULL;
	char* token1 = (char*)NULL;
	char* token2 = (char*)NULL;

	/** open the "setting file" in read-mode */
	fileOpen(&fileptr, filename, "r");

	buffer = allocMemory(MAXSIZE_BUFFER, sizeof(char));

	/** read the option */
	while (COMP_EOF(fgets(buffer, MAXSIZE_BUFFER, fileptr)))
	{
		if (buffer[0] == '-')
		{
			token1 = strtok_s(buffer, " \n\0", &context);

			/** help */
			if (strcmp(token1, "-help") == 0) return OPT_ERROR;

			/** scan-chain */
			else if (strcmp(token1, "-scan_chain") == 0)
			{
				token2 = strtok_s(NULL, "\n\0", &context);

				for (int i = 0; i < strlen(token2); i++)
				{
					if (token2[i] != ' ' && token2[i] != '\t')
					{
						opt.file.input.scanchain = _strdup(strtok_s(&token2[i], " \n\0", &context));
						break;
					}
				}
			}

			/** xor-tap */
			else if (strcmp(token1, "-xor_tap") == 0)
			{
				token2 = strtok_s(NULL, "\n\0", &context);

				for (int i = 0; i < strlen(token2); i++)
				{
					if (token2[i] != ' ' && token2[i] != '\t')
					{
						opt.file.input.xortap = _strdup(strtok_s(&token2[i], " \n\0", &context));
						break;
					}
				}
			}

			/** log */
			else if (strcmp(token1, "-l") == 0)
			{
				token2 = strtok_s(NULL, "\n\0", &context);

				for (int i = 0; i < strlen(token2); i++)
				{
					if (token2[i] != ' ' && token2[i] != '\t')
					{
						opt.file.output.log = _strdup(strtok_s(&token2[i], " \n\0", &context));
						break;
					}
				}
			}

			/** seed */
			else if (strcmp(token1, "-seed") == 0)
			{
				token2 = strtok_s(NULL, "\n\0", &context);

				for (int i = 0; i < strlen(token2); i++)
				{
					if (token2[i] != ' ' && token2[i] != '\t')
					{
						opt.file.output.seed = _strdup(strtok_s(&token2[i], " \n\0", &context));
						break;
					}
				}
			}

			/** test pattern */
			else if (strcmp(token1, "-t") == 0)
			{
				token2 = strtok_s(NULL, "\n\0", &context);

				for (int i = 0; i < strlen(token2); i++)
				{
					if (token2[i] != ' ' && token2[i] != '\t')
					{
						opt.file.output.test = _strdup(strtok_s(&token2[i], " \n\0", &context));
						break;
					}
				}
			}

			/** untestable fault */
			else if (strcmp(token1, "-u") == 0)
			{
				token2 = strtok_s(NULL, "\n\0", &context);

				for (int i = 0; i < strlen(token2); i++)
				{
					if (token2[i] != ' ' && token2[i] != '\t')
					{
						opt.file.output.untestable = _strdup(strtok_s(&token2[i], " \n\0", &context));
						break;
					}
				}
			}

			/** fault drop */
			else if (!strcmp(token1, "-d"))
			{
				token2 = strtok_s(NULL, " \n\0", &context);

				for (int i = 0; i < strlen(buffer); i++)
				{
					if (token2[i] != ' ' && token2[i] != '\t')
					{
						token2 = strtok_s(&token2[i], " \n\0", &context);

						if (!strcmp(token2, "N"))
							opt.mode.drop = DROP_NO;

						else if (!strcmp(token2, "Y"))
							opt.mode.drop = DROP_YES;

						else
						{
							printf("\n	COMMAND ERROR: option setup is failed. ");
							printf("%c-d=%s%c is not expected.\n\n", '"', token2, '"');
							return OPT_ERROR;
						}
						break;
					}
				}
			}
			/* num lfsr */
			else if (!strcmp(token1, "-b"))
			{
				token2 = strtok_s(NULL, "\n\0", &context);

				for (int i = 0; i < strlen(token2); i++)
				{
					if (token2[i] != ' ' && token2[i] != '\t')
					{
						opt.file.input.lfsr = atoi(strtok_s(&token2[i], " \n\0", &context));
						break;
					}
				}
			}
			/** 必須割り当て情報ファイル */
			else if (strcmp(token1, "-nece") == 0)
			{
				token2 = strtok_s(NULL, "\n\0", &context);

				for (int i = 0; i < strlen(token2); i++)
				{
					if (token2[i] != ' ' && token2[i] != '\t')
					{
						opt.file.input.necessary = _strdup(strtok_s(&token2[i], " \n\0", &context));
						break;
					}
				}
			}
			//ネットリスト読み込み
			else if (!strcmp(token1, "-net"))
			{
				token2 = strtok_s(NULL, "\n\0", &context);

				for (int i = 0; i < strlen(token2); i++)
				{
					if (token2[i] != ' ' && token2[i] != '\t')
					{
						fi[CIRCUIT].name = _strdup(strtok_s(&token2[i], " \n\0", &context));
						fi[CIRCUIT].flag = ON;
						break;
					}

				}
			}
			//低消費電力モード指定
			else if (!strcmp(token1, "-power"))
			{
				token2 = strtok_s(NULL, "\n\0", &context);

				for (int i = 0; i < strlen(token2); i++)
				{
					if (token2[i] != ' ' && token2[i] != '\t')
					{
						fi[LOW_POWER].name = _strdup(strtok_s(&token2[i], " \n\0", &context));
						fi[LOW_POWER].flag = ON;
						break;
					}

				}
			}
			//フォルダー指定
			else if (!strcmp(token1, "-folder"))
			{
				token2 = strtok_s(NULL, "\n\0", &context);

				for (int i = 0; i < strlen(token2); i++)
				{
					if (token2[i] != ' ' && token2[i] != '\t')
					{
						fi[FOLDER].name = _strdup(strtok_s(&token2[i], " \n\0", &context));
						fi[FOLDER].flag = ON;
						break;
					}

				}
			}
			//ランダム選択モード指定
			else if (!strcmp(token1, "-rand"))
			{
				token2 = strtok_s(NULL, "\n\0", &context);

				for (int i = 0; i < strlen(token2); i++)
				{
					if (token2[i] != ' ' && token2[i] != '\t')
					{
						fi[RAND].name = _strdup(strtok_s(&token2[i], " \n\0", &context));
						fi[RAND].flag = ON;
						break;
					}

				}
			}
			//Solver打ち切り時間(default:10)取得 
			else if (!strcmp(token1, "-time"))
			{
				token2 = strtok_s(NULL, "\n\0", &context);

				for (int i = 0; i < strlen(token2); i++)
				{
					if (token2[i] != ' ' && token2[i] != '\t')
					{
						fi[TIME].value = atoi(strtok_s(&token2[i], " \n\0", &context));
						fi[TIME].flag = ON;
						break;
					}

				}
			}
			//WSA閾値＝消費電力閾値(default:20)取得
			else if (!strcmp(token1, "-wsath"))
			{
				token2 = strtok_s(NULL, "\n\0", &context);

				for (int i = 0; i < strlen(token2); i++)
				{
					if (token2[i] != ' ' && token2[i] != '\t')
					{
						fi[WSATH].value = atoi(strtok_s(&token2[i], " \n\0", &context));
						fi[WSATH].flag = ON;
						break;
					}

				}
			}
			//目標故障の上限取得(0の場合は制限なし)得
			else if (!strcmp(token1, "-target_limit"))
			{
				token2 = strtok_s(NULL, "\n\0", &context);

				for (int i = 0; i < strlen(token2); i++)
				{
					if (token2[i] != ' ' && token2[i] != '\t')
					{
						fi[TARGET_LIMIT].value = atoi(strtok_s(&token2[i], " \n\0", &context));
						fi[TARGET_LIMIT].flag = ON;
						break;
					}

				}
			}
			//故障ファイル取得
			else if (!strcmp(token1, "-fault"))
			{
				token2 = strtok_s(NULL, "\n\0", &context);

				for (int i = 0; i < strlen(token2); i++)
				{
					if (token2[i] != ' ' && token2[i] != '\t')
					{
						fi[FAULT].name = _strdup(strtok_s(&token2[i], " \n\0", &context));
						fi[FAULT].flag = ON;
						break;
					}

				}
			}
			//故障ファイル取得
			else if (!strcmp(token1, "-mode_1"))
			{
				token2 = strtok_s(NULL, "\n\0", &context);

				for (int i = 0; i < strlen(token2); i++)
				{
					if (token2[i] != ' ' && token2[i] != '\t')
					{
						fi[MODE_1].name = _strdup(strtok_s(&token2[i], " \n\0", &context));
						fi[MODE_1].flag = ON;
						break;
					}

				}
			}
			//CLASP PARALLELモード 指定
			else if (!strcmp(token1, "-parallel"))
			{
				token2 = strtok_s(NULL, "\n\0", &context);

				for (int i = 0; i < strlen(token2); i++)
				{
					if (token2[i] != ' ' && token2[i] != '\t')
					{
						fi[PARALLEL].value = atoi(strtok_s(&token2[i], " \n\0", &context));
						fi[PARALLEL].flag = ON;
						break;
					}
				}
			}
			// [CPS INTEGRATION] experimental CPS mode (default: OFF)
			else if (!strcmp(token1, "-cps"))
			{
				token2 = strtok_s(NULL, "\n\0", &context);

				for (int i = 0; i < strlen(token2); i++)
				{
					if (token2[i] != ' ' && token2[i] != '\t')
					{
						char* cps_value = strtok_s(&token2[i], " \n\0", &context);
						if (strcmp(cps_value, "ON") != 0 && strcmp(cps_value, "OFF") != 0)
						{
							printf("\n\tCOMMAND ERROR: option setup is failed. ");
							printf("-cps requires ON or OFF.\n\n");
							return OPT_ERROR;
						}

						fi[CPS_MODE].name = _strdup(cps_value);
						fi[CPS_MODE].flag = ON;
						break;
					}
				}
			}
			// 論文の再シード生成フロー(step8-10) (default: OFF)
			else if (!strcmp(token1, "-regen"))
			{
				char* regen_value = strtok_s(NULL, " \t\r\n", &context);
				if (regen_value == NULL ||
					(strcmp(regen_value, "ON") != 0 && strcmp(regen_value, "OFF") != 0))
				{
					printf("\n\tCOMMAND ERROR: option setup is failed. ");
					printf("-regen requires ON or OFF.\n\n");
					return OPT_ERROR;
				}
				fi[REGEN_MODE].name = _strdup(regen_value);
				fi[REGEN_MODE].flag = ON;
			}
			else
			{
				printf("\n	COMMAND ERROR: option setup is failed. ");
				printf("%c%s%c is not expected.\n\n", '"', token1, '"');
				return OPT_ERROR;
			}
		}
	}

	//------------------------------------------------------------------------
    // defaultの設定
    //------------------------------------------------------------------------
	if (fi[RAND].flag == OFF) {
		fi[RAND].name = "OFF";
	}
	if (fi[TIME].flag == OFF) {
		fi[TIME].value = 10;
	}
	if (fi[WSATH].flag == OFF) {
		fi[WSATH].value = DEFAULT_WSA_THRESHOLD;
	}
	set_integrated_mode_defaults();

	free(buffer);

	fclose(fileptr);

	return OPT_OKAY;
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

	PrintHelpMessage("		   -z              :   setting options by file <.set> \n");

	PrintHelpMessage("\n		>> mode option \n");
	PrintHelpMessage("		   -d              :   fault drop [Y(YES)/N(NO)]. %cY%c is default. \n", '"', '"');
	PrintHelpMessage("\t\t   -cps            :   experimental CPS mode [ON/OFF]. OFF is default. \n");

	PrintHelpMessage("\t\t   -fp             :   fault-propagation constraints [ON/OFF]. OFF is default. \n");
	PrintHelpMessage("\t\t   -resolving      :   retry clasp UNKNOWN with 60/180/300 sec [ON/OFF]. OFF is default. \n");
	PrintHelpMessage("\t\t   -sttg_wsath     :   STTG and WSA 25/30 fallback [ON/OFF]. OFF is default. \n");
	PrintHelpMessage("\t\t   -regen          :   paper regeneration flow: clasp 60/180/300 sec, then WSA threshold +5%%/+10%% [ON/OFF]. OFF is default. \n");
	PrintHelpMessage("\n		>> help \n");
	PrintHelpMessage("		   -help           :   print the help. \n");

	PrintHelpMessage("\n");

		return;
}










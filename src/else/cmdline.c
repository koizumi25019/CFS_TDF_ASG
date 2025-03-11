//------------------------------------------------------------------------
//インクルード
//------------------------------------------------------------------------
#include<stdio.h>　			
#include<stdlib.h>
#include<string.h>

//------------------------------------------------------------------------
//自作インクルード
//------------------------------------------------------------------------
#include"../header/Define.h"

//===================================================
//関数名:cmdline
//機能:コマンドライン解析
//戻り値：int 正常終了:SUCCESS(=0) , 異常終了:ERROR(=-1)
//引数：コマンドライン引数の数, コマンドライン引数
//===================================================
int cmdline(int argc, char *argv[])
{
	int i;

	//------------------------------------------------------------------------
	// 実行コマンド解析
	//------------------------------------------------------------------------
	for (i = 1; i < argc; i++)
	{

		//------------------------------------------------------------------------
		// 入力ファイル
		//------------------------------------------------------------------------

		/* 回路ファイル名の登録 */
		if (!strcmp(argv[i], "-net")) {
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

        //-s,-x,-cに関する処理はOPTにあります
		else if (!strcmp(argv[i], "-seed")) {
			i++;
		}
		else if (!strcmp(argv[i], "-xor_tap")) {
			i++;
		}
		else if (!strcmp(argv[i], "-scan_chain")) {
			i++;
		}

		/* 未定義のオプションが指定された場合 */
		else
		{
			printf("Option【%s】is not defined! 未定義のオプションが定義されました！\n", argv[i]);
			return ERROR;
		}

	}


	//------------------------------------------------------------------------
	// 必要コマンドの入力確認
	//------------------------------------------------------------------------
	if (fi[CIRCUIT].flag == OFF) { printf("回路ファイル名を入力してください\n");		return ERROR; }
	if (fi[LOW_POWER].flag == OFF) { printf("低消費電力指向か否かを入力してください\n");		return ERROR; }
	if (fi[FOLDER].flag == OFF) { printf("出力フォルダー名を入力してください\n");		return ERROR; }
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
		fi[WSATH].value = 20;
	}

	return SUCCESS;

}
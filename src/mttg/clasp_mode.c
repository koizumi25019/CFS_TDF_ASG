#include <stdio.h>

#include"../asg/opb/clasp/clasp.h"
#include"../header/Define.h"

void clasp_call(int resolving_flag) {

	char cmd[500];

	if (fi[PARALLEL].flag == ON) {
		//解が1つ見つかった時点でその解を採用するモード
		if (fi[MODE_1].flag == ON) {
			if (fi[TIME].flag == ON) {
				printf("mode 1(-mode_1)と打ち切り時間指定(-time)を同時に指定しないでください\n");
				exit(1);
			}
			printf("CLASPモード指定：-parallel=ON -mode_1=ON \n");
			sprintf(cmd, "clasp.exe %smttg_pbo.txt > %sclasp_log_mttg.txt --parallel-mode %d --mode 1", folder_out, folder_out, fi[PARALLEL].value);
			system(cmd);
		}
		//打ち切り時間指定
		else if (fi[TIME].flag == ON) {
			if (resolving_flag == 0) {
				printf("CLASPモード指定：-parallel=ON -time %d\n", 10);
				sprintf(cmd, "clasp.exe %smttg_pbo.txt --quiet=1 --time-limit %d --parallel-mode %d > %sclasp_log_mttg.txt", folder_out, 60, fi[PARALLEL].value, folder_out);
				system(cmd);
			}
			else if (resolving_flag == 1) {
				printf("CLASPモード指定：-parallel=ON -time %d\n", 60);
				sprintf(cmd, "clasp.exe %smttg_pbo.txt --time-limit %d --parallel-mode %d > %sclasp_log_mttg.txt", folder_out, 300, fi[PARALLEL].value, folder_out);
				system(cmd);
			}
			else if (resolving_flag == 2) {
				printf("CLASPモード指定：-parallel=ON -time %d\n", 300);
				sprintf(cmd, "clasp.exe %smttg_pbo.txt --time-limit %d --parallel-mode %d > %sclasp_log_mttg.txt", folder_out, 600, fi[PARALLEL].value, folder_out);
				system(cmd);
			}
			else if (resolving_flag == 3) {
				printf("CLASPモード指定：-parallel=ON -time %d\n", 600);
				sprintf(cmd, "clasp.exe %smttg_pbo.txt --time-limit %d --parallel-mode %d > %sclasp_log_mttg.txt", folder_out, 600, fi[PARALLEL].value, folder_out);
				system(cmd);
			}
			else {
				printf("想定していない再シード生成です\n");
				exit(1);
			}
		}
		//paralellモードのみ指定
		else {
			printf("CLASPモード指定：-parallel=ON\n");
			sprintf(cmd, "clasp.exe %smttg_pbo.txt  --parallel-mode %d > %sclasp_log_mttg.txt", folder_out, fi[PARALLEL].value, folder_out);
			system(cmd);
		}
	}
	//mode1のみ指定
	else if (fi[MODE_1].flag == ON) {
		printf("CLASPモード指定：-mode 1=ON -time %d\n", fi[TIME].value);
		sprintf(cmd, "clasp.exe %smttg_pbo.txt --time-limit %d --mode 1 > %sclasp_log_mttg.txt", folder_out, fi[TIME].value, folder_out);
		system(cmd);
	}
	//parallelモード指定なし
	else {
		printf("claspモード指定なし　打ち切り時間：%d\n", fi[TIME].value);
		sprintf(cmd, "clasp.exe %smttg_pbo.txt --time-limit %d > %sclasp_log_mttg.txt", folder_out, fi[TIME].value, folder_out);
		system(cmd);
	}
}
//------------------------------------------------------------------------
//  外部ツールの呼び出し
//------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <sys/wait.h>
#endif

#include "external_tools.h"
#include "../header/Define.h"

int run_external(const char* command) {
	int status = system(command);
#ifdef _WIN32
	return status;		//Windows の system() はコマンドの終了コードをそのまま返す
#else
	return (status != -1 && WIFEXITED(status)) ? WEXITSTATUS(status) : -1;
#endif
}

//===================================================
//関数名:tool_exists
//機能:ツールが カレントフォルダー または PATH にあるか
//===================================================
static int tool_exists(const char* tool) {
	char command[256];

#ifdef _WIN32
	snprintf(command, sizeof(command), "where %s >nul 2>&1", tool);
#else
	snprintf(command, sizeof(command), "command -v %s >/dev/null 2>&1", tool);
#endif
	return run_external(command) == 0;
}

int check_external_tools(void) {
	const char* tools[] = { TOOL_CLASP, TOOL_FSIM, TOOL_WSA };
	int missing = 0;

	for (size_t i = 0; i < sizeof(tools) / sizeof(tools[0]); i++) {
		/* WSA.exe は低消費電力指向のときだけ使う */
		if (strcmp(tools[i], TOOL_WSA) == 0 && !opt.low_power) continue;
		if (!tool_exists(tools[i])) {
			printf("[ERROR] %s was not found. Put it in the current folder or in a folder in PATH.\n", tools[i]);
			missing++;
		}
	}
	return missing == 0 ? SUCCESS : ERROR;
}

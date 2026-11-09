//------------------------------------------------------------------------
//  PBOの問題ファイル(mttg_pbo.txt)の作成
//------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../header/Define.h"
#include "../asg/createSGmodel.h"
#include "../asg/opb/opb.h"
#include "../else/OUTPUT/report.h"
#include "pbo_generate.h"
#include "problem.h"

//===================================================
//関数名:write_problem_file
//機能:PBOの問題ファイルを書く(ヘッダ・目的関数・制約)。
//     変数の数・制約の数は、実際に出力した制約から数える(別に予測しない)
//引数：制約の本体(一時ファイル)，問題ファイルのパス，緩和変数の最初の番号
//===================================================
static int write_problem_file(FILE* body, const char* path, unsigned int relaxation_start, int with_power) {
	FILE* output = NULL;
	unsigned int maximum_variable = 0;
	unsigned int constraint_count = 0;
	int character;
	int transition_count = with_power ? pbo_count_transition_nets() : 0;

	rewind(body);
	while ((character = fgetc(body)) != EOF) {
		if (character == ';') {
			constraint_count++;
		}
		if (character == 'x') {
			unsigned int variable;
			if (fscanf_s(body, "%u", &variable) == 1 && variable > maximum_variable) {
				maximum_variable = variable;
			}
		}
	}
	if (ferror(body)) {
		printf("[ERROR] Cannot read the constraint buffer.\n");
		return ERROR;
	}
	if (fopen_s(&output, path, "w") != 0 || output == NULL) {
		printf("[ERROR] Cannot create %s\n", path);
		return ERROR;
	}

	fprintf(output, "* #variable= %u #constraint= %u #product= %d sizeproduct= %d\n",
		maximum_variable, constraint_count, 2 * transition_count, 4 * transition_count);
	fprintf(output, "min:");
	for (int target = 1; target < tfl_num; target++) {		//二次故障の緩和変数の個数だけ最小化する
		fprintf(output, " +1 x%u", relaxation_start + target - 1);
	}
	fprintf(output, ";\n");
	rewind(body);
	while ((character = fgetc(body)) != EOF) {
		fputc(character, output);
	}
	{
		int failed = ferror(body) || ferror(output);
		if (fclose(output) != 0) {
			failed = 1;
		}
		if (failed) {
			printf("[ERROR] Cannot write %s\n", path);
		}
		return failed ? ERROR : SUCCESS;
	}
}

//===================================================
//関数名:problem_write
//機能:目標故障集合 tfl と、問題の種類 spec から、PBOの問題ファイル mttg_pbo.txt を作る
//戻り値：int 正常終了:SUCCESS , 異常終了:ERROR
//===================================================
int problem_write(const ProblemSpec* spec) {
	char path[1024];
	FILE* body;
	unsigned int relaxation_start;
	int index;
	int result;

	if (output_path(path, sizeof(path), "mttg_pbo.txt") != SUCCESS) return ERROR;
	if ((body = tmpfile()) == NULL) {
		printf("[ERROR] Cannot create a temporary file for the constraints.\n");
		return ERROR;
	}

	if (spec->with_lfsr) makeProbFileConsPS_clasp(body);	//LFSR + フェーズシフタ
	pbo_generate_good_circuit(body);				//正常回路(2時刻分)

	bool_id = VAR_FIRST_FAULT_CIRCUIT;				//故障回路・故障検出回路の変数はここから
	index = 0;
	for (UNDET_FAULT_LIST* target = tfl; target != NULL; target = target->next, index++) {
		build_fault_cone(target->netlist, index);	//故障箇所から到達できる範囲を求める
		pbo_generate_faulty_circuit(body, target);
		pbo_generate_fault_detection(body, target);
		if (index == 0 && opt.fault_propagation) {	//一次故障だけ
			pbo_generate_fault_propagation(body, target);
		}
	}

	relaxation_start = bool_id;						//以降の変数は、二次故障の緩和変数
	index = 0;
	for (UNDET_FAULT_LIST* target = tfl; target != NULL; target = target->next, index++) {
		pbo_generate_fault_excitation(body, target, index == 0);	//一次故障は必ず励起、二次故障は緩和変数つき
	}

	if (spec->wsa_threshold >= 0) {
		pbo_generate_power(spec->wsa_threshold, body);
	}

	result = write_problem_file(body, path, relaxation_start, spec->wsa_threshold >= 0);
	fclose(body);
	return result;
}


//------------------------------------------------------------------------
//  必須割当て(Essential Assignment)
//
//  求め方(SAF_RPR_GEN の essential_assignment.c と同じ考え方。ただし範囲を限定せず、回路全体の割当てを返す)
//    1. 故障の励起条件を割り当てる。STR は1時刻目 0 / 2時刻目 1、STF は1時刻目 1 / 2時刻目 0 (正常値)
//    2. 一意活性化: 故障の影響が観測点へ届く経路が1本に絞られるゲートでは、他の入力に非制御値を割り当てる
//    3. 含意: 前方含意(入力から出力)と後方含意(出力から入力)を、新しい割当てが出なくなるまで繰り返す。
//       2時刻展開のときは「外部入力は2時刻で同じ値」「2時刻目のFF出力 = 1時刻目のFF入力」も含意に使う
//  割当ての間に矛盾が出たら、その故障を検出するテストは存在しない(冗長)。
//
//  時刻の番号: 0 = 1時刻目, 1 = 2時刻目。縮退故障として扱う(frames = 1)ときは 1 だけを使う
//------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "essential.h"
#include "../header/Define.h"

#define VALUE_UNKNOWN	(-1)

//------------------------------------------------------------------------
//  作業領域
//------------------------------------------------------------------------
static bool opened = false;
static int capacity_nets = 0;

static signed char* value_of[2];		//[時刻][信号線] -1:未定 0 1
static unsigned char* is_po;			//外部出力か

/* 割当ての記録(結果として返す)と、含意を計算する待ち行列 */
static int* log_net;
static unsigned char* log_frame;
static unsigned char* log_value;
static int log_count;
static int* work_net;
static unsigned char* work_frame;
static int work_count;

/* 一意活性化のための作業(世代番号で初期化を省く) */
static int* tfo_mark;
static int* useful_mark;
static int* queued_mark;
static int* done_mark;
static int generation;
static int* tfo_list;
static int* dfs_stack;
static int* heap;
static int heap_count;

/* 定数として登録された信号線 */
static int* constant_net;
static unsigned char* constant_value;
static int constant_count;

/* 今の計算の指定と状態 */
static EssentialOptions current;
static bool conflict;

//------------------------------------------------------------------------
//  既定の指定
//------------------------------------------------------------------------
EssentialOptions essential_default_options(void)
{
	EssentialOptions options;
	options.frames = 2;
	options.strong = true;
	options.observe_po = false;
	return options;
}

const char* essential_status_name(EssentialStatus status)
{
	switch (status) {
	case ESSENTIAL_OK:			return "ok";
	case ESSENTIAL_CONFLICT:	return "conflict";
	case ESSENTIAL_UNOBSERVABLE:return "unobservable";
	}
	return "unknown";
}

//------------------------------------------------------------------------
//  領域の確保と解放
//------------------------------------------------------------------------
void essential_close(void)
{
	free(value_of[0]); free(value_of[1]); value_of[0] = value_of[1] = NULL;
	free(is_po); is_po = NULL;
	free(log_net); free(log_frame); free(log_value); log_net = NULL; log_frame = log_value = NULL;
	free(work_net); free(work_frame); work_net = NULL; work_frame = NULL;
	free(tfo_mark); free(useful_mark); free(queued_mark); free(done_mark);
	tfo_mark = useful_mark = queued_mark = done_mark = NULL;
	free(tfo_list); free(dfs_stack); free(heap); tfo_list = dfs_stack = heap = NULL;
	free(constant_net); free(constant_value); constant_net = NULL; constant_value = NULL; constant_count = 0;
	opened = false;
}

bool essential_open(void)
{
	size_t n;

	if (opened) essential_close();
	n = (size_t)n_net;
	capacity_nets = n_net;
	value_of[0] = (signed char*)malloc(n);
	value_of[1] = (signed char*)malloc(n);
	is_po = (unsigned char*)calloc(n, 1);
	log_net = (int*)malloc(sizeof(int) * n * 2);
	log_frame = (unsigned char*)malloc(n * 2);
	log_value = (unsigned char*)malloc(n * 2);
	work_net = (int*)malloc(sizeof(int) * n * 2);
	work_frame = (unsigned char*)malloc(n * 2);
	tfo_mark = (int*)calloc(n, sizeof(int));
	useful_mark = (int*)calloc(n, sizeof(int));
	queued_mark = (int*)calloc(n, sizeof(int));
	done_mark = (int*)calloc(n, sizeof(int));
	tfo_list = (int*)malloc(sizeof(int) * n);
	dfs_stack = (int*)malloc(sizeof(int) * n);
	heap = (int*)malloc(sizeof(int) * n);
	if (!value_of[0] || !value_of[1] || !is_po || !log_net || !log_frame || !log_value || !work_net || !work_frame ||
		!tfo_mark || !useful_mark || !queued_mark || !done_mark || !tfo_list || !dfs_stack || !heap) {
		printf("[ERROR] Out of memory (essential assignments).\n");
		essential_close();
		return false;
	}
	memset(value_of[0], VALUE_UNKNOWN, n);
	memset(value_of[1], VALUE_UNKNOWN, n);
	for (int i = 0; i < n_po; i++) is_po[po[i] - nl] = 1;
	log_count = work_count = generation = 0;
	opened = true;
	return true;
}

bool essential_set_constant(const NLIST* net, int value)
{
	int* nets;
	unsigned char* values;

	if (!opened || net == NULL || (value != 0 && value != 1)) return false;
	nets = (int*)realloc(constant_net, sizeof(int) * (constant_count + 1));
	if (nets == NULL) return false;
	constant_net = nets;
	values = (unsigned char*)realloc(constant_value, (size_t)constant_count + 1);
	if (values == NULL) return false;
	constant_value = values;
	constant_net[constant_count] = (int)(net - nl);
	constant_value[constant_count] = (unsigned char)value;
	constant_count++;
	return true;
}

//------------------------------------------------------------------------
//  割当て
//------------------------------------------------------------------------
static void set_value(int frame, int net, int value)
{
	signed char old = value_of[frame][net];

	if (old != VALUE_UNKNOWN) {
		if (old != value) conflict = true;
		return;
	}
	value_of[frame][net] = (signed char)value;
	log_net[log_count] = net;
	log_frame[log_count] = (unsigned char)frame;
	log_value[log_count] = (unsigned char)value;
	log_count++;
	work_net[work_count] = net;
	work_frame[work_count] = (unsigned char)frame;
	work_count++;
}

static int index_of(const NLIST* net)
{
	return (int)(net - nl);
}

//------------------------------------------------------------------------
//  1つのゲートについて、入力と出力の値から分かることを全て割り当てる
//------------------------------------------------------------------------
static void imply_gate(int frame, int gate_index)
{
	const NLIST* gate = &nl[gate_index];
	signed char* values = value_of[frame];
	int output = values[gate_index];

	switch (gate->type) {
	case INV:
	case BUF:
	case FOUT: {
		int input_index = index_of(gate->in[0]);
		int invert = gate->type == INV;
		if (values[input_index] != VALUE_UNKNOWN) set_value(frame, gate_index, values[input_index] ^ invert);
		if (output != VALUE_UNKNOWN) set_value(frame, input_index, output ^ invert);
		return;
	}
	case EXOR: {
		int a = index_of(gate->in[0]), b = index_of(gate->in[1]);
		if (values[a] != VALUE_UNKNOWN && values[b] != VALUE_UNKNOWN) set_value(frame, gate_index, values[a] ^ values[b]);
		else if (output != VALUE_UNKNOWN && values[a] != VALUE_UNKNOWN) set_value(frame, b, output ^ values[a]);
		else if (output != VALUE_UNKNOWN && values[b] != VALUE_UNKNOWN) set_value(frame, a, output ^ values[b]);
		return;
	}
	case AND: case NAND: case OR: case NOR: {
		int controlling = (gate->type == OR || gate->type == NOR);		//入力がこの値なら出力が決まる
		int invert = (gate->type == NAND || gate->type == NOR);
		int controlled_count = 0, unknown_count = 0, last_unknown = -1;

		for (int i = 0; i < gate->n_in; i++) {
			int input_index = index_of(gate->in[i]);
			if (values[input_index] == VALUE_UNKNOWN) { unknown_count++; last_unknown = input_index; }
			else if (values[input_index] == controlling) controlled_count++;
		}
		/* 前方含意 */
		if (controlled_count > 0) set_value(frame, gate_index, controlling ^ invert);
		else if (unknown_count == 0) set_value(frame, gate_index, (!controlling) ^ invert);
		/* 後方含意 */
		output = values[gate_index];
		if (output != VALUE_UNKNOWN) {
			if (output == ((!controlling) ^ invert)) {
				for (int i = 0; i < gate->n_in; i++) set_value(frame, index_of(gate->in[i]), !controlling);
			}
			else if (current.strong && controlled_count == 0 && unknown_count == 1) {
				set_value(frame, last_unknown, controlling);
			}
		}
		return;
	}
	default:
		return;		//IN, DFF は時刻間の接続で扱う
	}
}

//------------------------------------------------------------------------
//  新しい割当てが出なくなるまで含意を続ける
//------------------------------------------------------------------------
static void propagate(void)
{
	bool two_frames = current.frames == 2;

	while (work_count > 0 && !conflict) {
		int net, frame;
		const NLIST* gate;

		work_count--;
		net = work_net[work_count];
		frame = work_frame[work_count];
		gate = &nl[net];

		imply_gate(frame, net);
		for (int i = 0; i < gate->n_out; i++) {
			const NLIST* successor = gate->out[i];
			if (successor->type == DFF) {
				if (two_frames && frame == 0) set_value(1, index_of(successor), value_of[0][net]);	//2時刻目のFF出力 = 1時刻目のFF入力
			}
			else {
				imply_gate(frame, index_of(successor));
			}
		}
		if (two_frames) {
			if (gate->type == IN) set_value(1 - frame, net, value_of[frame][net]);		//外部入力は2時刻で同じ値
			if (gate->type == DFF && frame == 1) set_value(0, index_of(gate->in[0]), value_of[1][net]);
		}
	}
	work_count = 0;
}

//------------------------------------------------------------------------
//  一意活性化
//------------------------------------------------------------------------
static int compare_level(const void* left, const void* right)
{
	int a = nl[*(const int*)left].level, b = nl[*(const int*)right].level;
	return (a > b) - (a < b);
}

static bool is_observation_point(int net)
{
	return nl[net].ppo_flag == 1 || (current.observe_po && is_po[net]);
}

static void heap_push(int net)
{
	int position = heap_count++;
	heap[position] = net;
	while (position > 0) {
		int parent = (position - 1) / 2;
		if (nl[heap[parent]].level <= nl[heap[position]].level) break;
		int temporary = heap[parent]; heap[parent] = heap[position]; heap[position] = temporary;
		position = parent;
	}
}

static int heap_pop(void)
{
	int top = heap[0], position = 0;
	heap[0] = heap[--heap_count];
	for (;;) {
		int left = 2 * position + 1, right = left + 1, smallest = position;
		if (left < heap_count && nl[heap[left]].level < nl[heap[smallest]].level) smallest = left;
		if (right < heap_count && nl[heap[right]].level < nl[heap[smallest]].level) smallest = right;
		if (smallest == position) break;
		int temporary = heap[smallest]; heap[smallest] = heap[position]; heap[position] = temporary;
		position = smallest;
	}
	return top;
}

static void heap_enqueue_outputs(int net)
{
	const NLIST* gate = &nl[net];
	for (int i = 0; i < gate->n_out; i++) {
		int successor = index_of(gate->out[i]);
		if (gate->out[i]->type == DFF || useful_mark[successor] != generation || queued_mark[successor] == generation) continue;
		queued_mark[successor] = generation;
		heap_push(successor);
	}
}

/* ゲート gate_index が唯一の経路上にあるとき、故障の影響が通らない入力(経路の入力 path_input 以外で、
   故障の影響を受けない信号線)に非制御値を割り当てる */
static void assign_side_inputs(int gate_index, int path_input)
{
	const NLIST* gate = &nl[gate_index];
	int non_controlling;

	switch (gate->type) {
	case AND: case NAND:	non_controlling = 1; break;
	case OR: case NOR:		non_controlling = 0; break;
	default:				return;
	}
	for (int i = 0; i < gate->n_in; i++) {
		int input_index = index_of(gate->in[i]);
		if (input_index == path_input || tfo_mark[input_index] == generation) continue;
		set_value(1, input_index, non_controlling);
	}
}

/* 故障の影響の経路を調べる。経路が無ければ ESSENTIAL_UNOBSERVABLE */
static EssentialStatus sensitize_unique_path(int site)
{
	int tfo_count = 0, stack_top = 0;
	bool observation_seen = false;

	generation++;
	/* 故障箇所から2時刻目の組合せ回路を出力方向にたどる(FFの入力で止まる) */
	dfs_stack[stack_top++] = site;
	tfo_mark[site] = generation;
	while (stack_top > 0) {
		int net = dfs_stack[--stack_top];
		tfo_list[tfo_count++] = net;
		for (int i = 0; i < nl[net].n_out; i++) {
			int successor = index_of(nl[net].out[i]);
			if (nl[net].out[i]->type == DFF || tfo_mark[successor] == generation) continue;
			tfo_mark[successor] = generation;
			dfs_stack[stack_top++] = successor;
		}
	}
	qsort(tfo_list, tfo_count, sizeof(int), compare_level);

	/* 観測点に届く信号線だけを残す(出力側から) */
	for (int k = tfo_count - 1; k >= 0; k--) {
		int net = tfo_list[k];
		bool useful = is_observation_point(net);
		for (int i = 0; i < nl[net].n_out && !useful; i++) {
			int successor = index_of(nl[net].out[i]);
			if (nl[net].out[i]->type != DFF && useful_mark[successor] == generation) useful = true;
		}
		if (useful) useful_mark[net] = generation;
	}
	if (useful_mark[site] != generation) return ESSENTIAL_UNOBSERVABLE;

	/* レベルの低い順に調べ、未処理の信号線が1つだけになったら、それが全ての経路の通る点 */
	heap_count = 0;
	queued_mark[site] = generation;
	heap_push(site);
	while (heap_count > 0) {
		int net = heap_pop();

		done_mark[net] = generation;
		if (net != site && heap_count == 0 && !observation_seen) {
			for (int i = 0; i < nl[net].n_in; i++) {
				int input_index = index_of(nl[net].in[i]);
				if (done_mark[input_index] == generation) {
					assign_side_inputs(net, input_index);
					break;
				}
			}
		}
		if (is_observation_point(net)) observation_seen = true;
		heap_enqueue_outputs(net);
	}
	return ESSENTIAL_OK;
}

//------------------------------------------------------------------------
//  essential_compute
//------------------------------------------------------------------------
bool essential_compute(const NLIST* site, int type, const EssentialOptions* options, EssentialResult* result)
{
	int site_index, good_value;
	EssentialStatus status;

	if (!opened || site == NULL || result == NULL || (type != STR && type != STF)) return false;
	current = options != NULL ? *options : essential_default_options();
	if (current.frames != 1 && current.frames != 2) return false;

	/* 前の結果を消す */
	for (int k = 0; k < log_count; k++) value_of[log_frame[k]][log_net[k]] = VALUE_UNKNOWN;
	log_count = work_count = 0;
	conflict = false;

	for (int k = 0; k < constant_count; k++) {
		if (current.frames == 2) set_value(0, constant_net[k], constant_value[k]);
		set_value(1, constant_net[k], constant_value[k]);
	}
	site_index = index_of(site);
	good_value = (type == STR) ? 1 : 0;		//2時刻目の正常値。1時刻目はその反対
	if (current.frames == 2) set_value(0, site_index, !good_value);
	set_value(1, site_index, good_value);
	status = sensitize_unique_path(site_index);
	propagate();
	if (status == ESSENTIAL_OK && conflict) status = ESSENTIAL_CONFLICT;

	result->status = status;
	result->count = log_count;
	result->net = log_net;
	result->frame = log_frame;
	result->value = log_value;
	return true;
}

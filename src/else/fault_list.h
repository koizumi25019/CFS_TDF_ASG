//------------------------------------------------------------------------
//  故障リスト(UNDET_FAULT_LIST)の操作
//------------------------------------------------------------------------
#pragma once
#include "../header/Fault.h"

/** 同じ故障か(故障の識別は node_id。複製しても node_id は変わらない) */
int fault_same(const UNDET_FAULT_LIST* a, const UNDET_FAULT_LIST* b);

/** 故障ノードを1つ複製する(next は NULL)。確保できなければ NULL。
    故障の付随情報(必須割当て・辺など)へのポインタは元のノードと共有する */
UNDET_FAULT_LIST* fault_copy(const UNDET_FAULT_LIST* source);

/** source の複製を list の先頭に追加し，*count を増やす */
int fault_list_push_copy(UNDET_FAULT_LIST** list, int* count, const UNDET_FAULT_LIST* source);

/** リストのノードを全て解放する(複製したノード用) */
void fault_list_free(UNDET_FAULT_LIST* head);

/** 未検出故障リスト ufl から target と同じ故障を取り除く(有効故障ビットも落とす)。
    見つからなければ ERROR */
int fault_remove_undetected(const UNDET_FAULT_LIST* target);

/** 有効な(未検出の)故障を表すビット列 detflag を、全ての故障(0 .. all_num-1)を有効にして作る */
void fault_flags_init(void);

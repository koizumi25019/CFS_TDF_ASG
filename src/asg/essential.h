//------------------------------------------------------------------------
//  必須割当て(Essential Assignment)
//  故障を検出するために必ず成り立たなければならない信号線の値を、回路と故障だけから求める。
//  LFSR の有無やソルバに依存しないので、LFSR ON/OFF のどちらでも使える。
//------------------------------------------------------------------------
#pragma once
#include <stdbool.h>
#include "../header/Fault.h"

/** 求め方の指定 */
typedef struct {
	int  frames;		/**< 2: 2時刻展開(遷移故障。1時刻目の初期値と時刻間の接続も使う)
							 1: 2時刻目だけ(縮退故障として扱う。SAF の必須割当てファイルとの照合用) */
	bool strong;		/**< true: 「出力が制御された値で、入力の残りが全て非制御値なら、最後の入力は制御値」も使う
							 false: 出力が非制御値のときの後方含意と前方含意だけ(SAF_RPR_GEN と同じ) */
	bool observe_po;	/**< true: 外部出力も観測点にする(縮退故障との照合用)。遷移故障(LoC)は PPO だけ */
} EssentialOptions;

/** 求めた結果の状態 */
typedef enum {
	ESSENTIAL_OK = 0,			/**< 必須割当てが求まった */
	ESSENTIAL_CONFLICT,			/**< 必須割当ての間に矛盾がある(検出するテストが存在しない) */
	ESSENTIAL_UNOBSERVABLE		/**< 故障の影響が観測点に届かない(検出するテストが存在しない) */
} EssentialStatus;

/** 求めた必須割当て。配列は essential_compute を次に呼ぶまで有効 */
typedef struct {
	EssentialStatus status;
	int count;					/**< 割当ての数 */
	const int* net;				/**< [count] 信号線番号(nl の添字) */
	const unsigned char* frame;	/**< [count] 0: 1時刻目, 1: 2時刻目 */
	const unsigned char* value;	/**< [count] 0 または 1 */
} EssentialResult;

/** 既定の指定(2時刻展開・強い含意・PPO のみ観測) */
EssentialOptions essential_default_options(void);

/** 作業領域を確保する(回路を読み込んだ後に1回)。失敗したら false */
bool essential_open(void);

/** 作業領域を解放する */
void essential_close(void);

/** 信号線 net が常に value であることが分かっているとき(定数)に登録する。以後の必須割当てに、時刻の両方で加える */
bool essential_set_constant(const NLIST* net, int value);

/** 静的学習: 直接の含意では分からない間接含意を、回路ごとに1回見つけて記録する。以後の essential_compute が使う。
    見つかった間接含意の数と、矛盾から分かった定数の数を返す(定数は essential_set_constant に登録する)。失敗したら false */
bool essential_learn(int* learned_count, int* constant_count_found);

/** 信号線 site の遷移故障 type(STR/STF) の必須割当てを求める。
    status が ESSENTIAL_OK でないときも、そこまでに求めた割当てを返す */
bool essential_compute(const NLIST* site, int type, const EssentialOptions* options, EssentialResult* result);

/** 状態の名前(ログ用) */
const char* essential_status_name(EssentialStatus status);

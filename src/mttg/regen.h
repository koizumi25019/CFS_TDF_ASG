//------------------------------------------------------------------------
//  再生成の方針: ソルバが一次故障を含む問題を解けなかったときに、次に何を試すか
//
//  試行 = (対象の取り方, WSA閾値の段階)
//    PHASE_MULTI  : 目標故障集合(複数目標, MTTG)
//    PHASE_SINGLE : 一次故障だけ(単一目標, STTG)               ... -sttg
//    WSA閾値の段階 0,1,2 = -wsath, +5%, +10%                   ... -regen だけが緩める
//
//  順序:  MULTI(0) -> [-regen] MULTI(1) -> MULTI(2) -> [-sttg] SINGLE(0) -> [-regen] SINGLE(1) -> SINGLE(2) -> 断念
//  ソルバの打ち切り時間の延長(60/180/300秒)は -regen のとき、各試行の中で clasp_mode.c が行う。
//------------------------------------------------------------------------
#pragma once

#define REGEN_WSA_STEP		5		//WSA閾値の緩和幅[%]
#define REGEN_WSA_MAX_STAGE	2		//緩和の最大段階(既定 20% -> 25% -> 30%)

typedef enum { PHASE_MULTI = 0, PHASE_SINGLE = 1 } MttgPhase;

typedef struct {
	MttgPhase phase;				//対象の取り方
	int wsa_stage;					//WSA閾値の段階
} MttgAttempt;

/** 最初の試行(複数目標、指定のWSA閾値) */
MttgAttempt regen_first_attempt(void);

/** 試行 attempt の WSA 閾値[%] */
int regen_wsa_threshold(const MttgAttempt* attempt);

/** 試行 current が失敗したとき(solver_unsat: 1=解なしと確定, 0=打ち切り)の次の試行。
    次があれば 1 を返して *next に入れる。無ければ 0(その一次故障は断念) */
int regen_next_attempt(const MttgAttempt* current, int solver_unsat, MttgAttempt* next);

/** 試行の名前("MTTG" / "STTG")。ログ用 */
const char* regen_phase_name(MttgPhase phase);

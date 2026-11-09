//------------------------------------------------------------------------
//  再生成の方針(詳細は regen.h)
//------------------------------------------------------------------------
#include "regen.h"
#include "../opt/opt.h"

/* WSA閾値を緩められる最大の段階。緩めるのは -regen のときだけ(低消費電力指向のときだけ意味がある) */
static int max_relax_stage(void) {
	int stage = 0;

	if (!opt.regen || !opt.low_power) return 0;
	while (stage < REGEN_WSA_MAX_STAGE && opt.wsa_threshold + REGEN_WSA_STEP * (stage + 1) <= 100) {
		stage++;
	}
	return stage;
}

MttgAttempt regen_first_attempt(void) {
	MttgAttempt first = { PHASE_MULTI, 0 };
	return first;
}

int regen_wsa_threshold(const MttgAttempt* attempt) {
	return opt.wsa_threshold + REGEN_WSA_STEP * attempt->wsa_stage;
}

int regen_next_attempt(const MttgAttempt* current, int solver_unsat, MttgAttempt* next) {
	/* 閾値を緩められるなら、同じ対象のまま緩める(解なしも、WSA制約が原因かもしれないので緩める) */
	if (current->wsa_stage < max_relax_stage()) {
		next->phase = current->phase;
		next->wsa_stage = current->wsa_stage + 1;
		return 1;
	}
	/* 複数目標で打ち切られた一次故障は、単独で解き直す。解なしと確定したなら、単独でも解なしなので行わない */
	if (current->phase == PHASE_MULTI && opt.sttg && !solver_unsat) {
		next->phase = PHASE_SINGLE;
		next->wsa_stage = 0;
		return 1;
	}
	return 0;
}

const char* regen_phase_name(MttgPhase phase) {
	return phase == PHASE_SINGLE ? "STTG" : "MTTG";
}

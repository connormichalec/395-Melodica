/*
 * ADSR.c
 *
 *  Created on: Apr 9, 2025
 *      Author: Connor Michalec
 */

#include "ADSR.h"


// Max num of adsrs avail
#define NUM_ADSRS 50

ADSR adsrs[NUM_ADSRS];

void init_adsrs() {

}

int get_num_adsrs() {
	return NUM_ADSRS;
}

void adsrs_tick() {
	// Increment all adsrs

	// Check state and handle based on current state
	for(int i = 0; i<NUM_ADSRS; i++) {
		// Tick increment for each
		adsrs[i].cur_step++;
	}

	// Set cur_val based on cur_step:

	// If ADSR passes attack threshold move it to next state automatically (sustain)
}

float get_adsr_val(ADSR * adsr) {
	return adsr->cur_val;
}

ADSR * get_adsr(int adsr_idx) {
	return &adsrs[adsr_idx];
}

int create_ADSR(float attack, float sustain, float rel) {
	/*
	adsrs[num_adsrs].attack = attack;
	adsrs[num_adsrs].sustain = sustain;
	adsrs[num_adsrs].rel = rel;
	adsrs[num_adsrs].cur_val = 0.0f;
	adsrs[num_adsrs].cur_step = 0;
	adsrs[num_adsrs].cur_state = NONE;

	num_adsrs++;

	// return adsr idx*/
	return adsrs[NUM_ADSRS].adsr_idx;
}

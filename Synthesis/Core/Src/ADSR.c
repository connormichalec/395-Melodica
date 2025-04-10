/*
 * ADSR.c
 *
 *  Created on: Apr 9, 2025
 *      Author: Connor Michalec
 */

#include "ADSR.h"


ADSR adsrs[50];

int num_adsrs;			// Number of oscillators currently enabled

void init_adsrs() {
	num_adsrs = 0;
}

int get_num_adsrs() {
	return num_adsrs;
}

void adsrs_tick() {
	// Increment all adsrs

	// Check state and handle based on current state
	for(int i = 0; i<num_adsrs; i++) {
		// Tick increment for each
		adsrs[i].cur_step++;
	}

	// Set cur_val based on cur_step:
}

float get_adsr_val(ADSR * adsr) {
	return adsr->cur_val;
}

ADSR * get_adsr(int adsr_idx) {
	return &adsrs[adsr_idx];
}

int create_ADSR(float attack, float sustain, float rel) {
	adsrs[num_adsrs].attack = attack;
	adsrs[num_adsrs].sustain = sustain;
	adsrs[num_adsrs].rel = rel;
	adsrs[num_adsrs].cur_val = 0.0f;
	adsrs[num_adsrs].cur_step = 0;
	adsrs[num_adsrs].cur_state = NONE;

	num_adsrs++;

	// return adsr idx
	return adsrs[num_adsrs].adsr_idx;
}

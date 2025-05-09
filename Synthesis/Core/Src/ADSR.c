/*
 * ADSR.c
 *
 *  Created on: Apr 9, 2025
 *      Author: Connor Michalec
 */

#include "ADSR.h"
#include <stdlib.h>




// TODO:
// Make ADSRs a linked list (like filters are) so multiple can be added to the voice, this can be utilized so that more than one adsrs can be used for things like filters

void init_adsrs() {
	// nothing needed

}

// Normalize timing factors to be more understandable
float factor_scale = 0.00001f;

// All functions return from 0-1

float attack_func(ADSR * adsr) {

	float val;

	// Assume instant
	if(adsr->attack_factor<=0.02f) {
		val = adsr->attack_level;
		ADSR_next(adsr);
		adsr->last_sus_val = val;
		return(val);
	}

	val = adsr->cur_step * (1/adsr->attack_factor) * factor_scale;

	if(val>=adsr->attack_level)
		ADSR_next(adsr);

	adsr->last_sus_val = val;

	return(val);
}

float decay_func(ADSR * adsr) {
	// linear decay from attack level to sustain level after decay time
	// rise/run!!

	float val;

	// Assume instant
	if(adsr->decay_factor<=0.02f) {
		val = adsr->sustain_level;
		adsr->last_sus_val = val;
		ADSR_next(adsr);
		return val;
	}

	val = adsr->attack_level + adsr->cur_step * -(1/adsr->decay_factor * factor_scale);
	adsr->last_sus_val = val;

	if(val<=adsr->sustain_level)
		ADSR_next(adsr);

	return(val);
}

float sustain_func(ADSR * adsr) {
	// sustain at sustain level indefinitely until release. IF SUSTAIN LEVEL IS SET TO 0, go to done state however:

	if(adsr->sustain_level<=0)
		ADSR_set_state(adsr, DONE);

	adsr->last_sus_val = adsr->sustain_level;

	return(adsr->sustain_level);
}

float rel_func(ADSR * adsr) {


	float val;

	// Assume instant:
	if(adsr->release_factor<=0.02f) {
		val = 0.0f;
		ADSR_next(adsr);
		return val;
	}

	val = adsr->last_sus_val + adsr->cur_step * -((1/adsr->release_factor) * factor_scale);

	if(val<=0.1f)
		ADSR_next(adsr);

	return(val);
}

void ADSR_set_state(ADSR * adsr, ADSRstate state) {
	adsr->cur_state = state;
	adsr->cur_step = 0;
}

void ADSR_next(ADSR * adsr) {
	switch(adsr->cur_state) {
		case NONE:
			ADSR_set_state(adsr, ATTACK);
			break;
		case ATTACK:
			ADSR_set_state(adsr, DECAY);
			break;
		case DECAY:
			ADSR_set_state(adsr, SUSTAIN);
			break;
		case SUSTAIN:
			ADSR_set_state(adsr, RELEASE);
			break;
		case RELEASE:
			ADSR_set_state(adsr, DONE);
			break;
		default:
			break;
	}
}

void ADSR_tick(ADSR * adsr) {
	adsr->cur_step++;

	switch(adsr->cur_state) {
		case NONE:
			break;

		case ATTACK:
			adsr->cur_val = attack_func(adsr);
			break;

		case DECAY:
			adsr->cur_val = decay_func(adsr);
			break;

		case SUSTAIN:
			adsr->cur_val = sustain_func(adsr);
			break;

		case RELEASE:
			adsr->cur_val = rel_func(adsr);
			break;

		default:
			break;
	}
}

float get_adsr_val(ADSR * adsr) {
	return adsr->cur_val;
}

ADSR * create_ADSR(float attack_factor, float attack_level, float decay_factor, float sustain_level, float release_factor) {
	ADSR * adsr = malloc(sizeof(ADSR));

	adsr->attack_factor = attack_factor;
	adsr->attack_level = attack_level;
	adsr->decay_factor = decay_factor;
	adsr->sustain_level = sustain_level;
	adsr->release_factor = release_factor;
	adsr->cur_val = 0.0f;
	adsr->cur_step = 0;
	adsr->cur_state = NONE;

	// return ADSR
	return adsr;
}

void delete_ADSR(ADSR * adsr) {
	if(adsr==NULL)
		return;

	adsr->attack_factor = 0.0f;
	adsr->attack_level = 0.0f;
	adsr->decay_factor = 0.0f;
	adsr->sustain_level = 0.0f;
	adsr->release_factor = 0.0f;
	adsr->cur_val = 0.0f;
	adsr->cur_step = 0;
	adsr->cur_state = DONE;

	free(adsr);
}

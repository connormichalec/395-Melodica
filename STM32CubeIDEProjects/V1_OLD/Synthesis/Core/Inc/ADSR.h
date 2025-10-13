/*
 * ADSR.h
 *
 *  Created on: Apr 9, 2025
 *      Author: Connor Michalec
 */

#ifndef INC_ADSR_H_
#define INC_ADSR_H_

typedef enum {
	NONE,
	ATTACK,
	DECAY,
	SUSTAIN,
	RELEASE,
	DONE
} ADSRstate;


typedef struct {
	float attack_factor;
	float attack_level;
	float decay_factor;
	float sustain_level;
	float release_factor;
	float cur_val;
	float last_sus_val; 			// Contains the last value either held by attack or sustain to be used for release calculation
	long cur_step;
	ADSRstate cur_state;
} ADSR;

void init_adsrs();

void ADSR_tick(ADSR * adsr);

/**
 * Go to next adsr state (for example if note is pressed calling this will change it from NONE to ATTACK)
 */
void ADSR_next(ADSR * adsr);

/**
 * Go to specific ADSR state
 */
void ADSR_set_state(ADSR * adsr, ADSRstate state);

float get_adsr_val(ADSR * adsr);

ADSR * create_ADSR(float attack_factor, float attack_level, float decay_factor, float sustain_level, float release_factor);

void delete_ADSR(ADSR * adsr);


#endif /* INC_ADSR_H_ */

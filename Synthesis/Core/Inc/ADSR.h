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
	SUSTAIN,
	RELEASE
} ADSRstate;


typedef struct {
	float attack;
	float sustain;
	float rel;
	float cur_val;
	int cur_step;
	int adsr_idx;
	ADSRstate cur_state;
} ADSR;

void init_adsrs();

void adsrs_tick();

int get_num_adsrs();

float get_adsr_val(ADSR * adsr);

ADSR * get_adsr(int adsr_idx);

int create_ADSR(float attack, float sustain, float rel);


#endif /* INC_ADSR_H_ */

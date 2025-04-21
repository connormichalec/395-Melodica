/*
 * voice.h
 *
 *  Created on: Apr 16, 2025
 *      Author: conno
 */

#ifndef INC_VOICE_H_
#define INC_VOICE_H_

#include "ADSR.h"
#include "oscillator.h"

#define VOICE_NUM_OSC 5
#define NUM_VOICES 10

typedef struct {
	Oscillator * osc[VOICE_NUM_OSC];
	int enabled;
} Voice;

/**
 * Initialize voices system
 */
void init_voices();

/**
 * Get Total number of voices
 */
int get_num_voices();

/**
 * Get number of voices currently enabled
 */
int num_enabled_voices();

/**
 *
 */
int enable_voice(oscillatorTypes type, float frequency, float detune);

/**
 *
 */
void disable_voice(Voice * voice);

Voice * get_voice(int voice_idx);

#endif /* INC_VOICE_H_ */

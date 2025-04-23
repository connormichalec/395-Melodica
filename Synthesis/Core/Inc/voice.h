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

#define VOICE_NUM_OSC 2
#define NUM_VOICES 6

// TODO: Implement function pointres in struct for more streamlined interfacing (liek a class)
typedef struct {
	Oscillator * osc[VOICE_NUM_OSC];
	int num_osc;
	ADSR * adsr;
	int enabled;
	int NOTE;			// Note that this voice is tracked to, set to -1 when not used.
} Voice;

/**
 * Initialize voices system
 */
void init_voices();


/**
 * Tick ALL voices (convinience)
 */
void tick_voices();

/**
 * Get the ADSR value, should control amplitude of final sample
 */
float get_voice_ADSR_val(Voice * voice);

/**
 * Used to manage voice functions
 */
void tick_voice(Voice * voice);

/**
 * Get Total number of voices
 */
int get_num_voices();

/**
 * Get number of voices currently enabled
 */
int num_enabled_voices();

void construct_voice(oscillatorTypes type, Voice * v, float frequency, float detune);


/**
 * Enable voice using a MIDI note no
 */
int enable_voice(oscillatorTypes type, int note, float detune);

/**
 *
 */
void disable_voice(Voice * voice);

Voice * get_voice_from_idx(int voice_idx);

Voice * get_voice_from_note(int note);

#endif /* INC_VOICE_H_ */

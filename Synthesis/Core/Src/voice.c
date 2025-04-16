/*
 * voice.c
 *
 *  Created on: Apr 16, 2025
 *      Author: Connor Michalec
 */

#include "voice.h"
#include <stddef.h>

// Number of voices
Voice voices[NUM_VOICES];
int num_voices_enabled;

int get_num_voices() {
	return NUM_VOICES;
}

int num_enabled_voices() {
	return num_voices_enabled;
}

int getFirstDisabledVoiceIdx() {
	for(int x = 0; x<NUM_VOICES; x++) {
		if(voices[x].enabled==0) {
			return x;
		}
	}
	return -1;
}

void init_voices() {
	init_oscillators();
	init_adsrs();

	for(int i = 0; i<NUM_VOICES; i++) {
		for(int x = 0; x<VOICE_NUM_OSC; x++) {
			voices[i].osc[x] = NULL;
		}
		voices[i].enabled = 0;
	}
}

int enable_voice(oscillatorTypes type, float frequency, float detune) {
	// assign adsr to voice

	int idx = getFirstDisabledVoiceIdx();
	// No voices avail
	if(idx == -1) return -1;

	Voice * v = get_voice(idx);

	v->enabled = 1;

	for(int i =0; i<VOICE_NUM_OSC; i++) {
		// Create multiple oscillators detuned by detune amt

		// Alternate whether detune adds or subtracts to frequency based on if i is odd or even (creates even spread):
		float freq = i%2==0 ? (frequency + (float)i * detune) : (frequency - (float)i * detune);
		v->osc[i] = get_oscillator(enable_oscillator(type, freq));
	}

	num_voices_enabled++;
	return idx;
}

void disable_voice(Voice * voice) {
	// Check if voice is actually enabled:
	if(!voice->enabled)
		return;

	// Disable all oscillators associated w/
	for(int i =0; i<VOICE_NUM_OSC; i++) {
		disable_oscillator(voice->osc[i]);
		voice->osc[i] = NULL;
	}

	voice->enabled = 0;

	num_voices_enabled--;

}

Voice * get_voice(int voice_idx) {
	return &voices[voice_idx];
}



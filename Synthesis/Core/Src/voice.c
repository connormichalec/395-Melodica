/*
 * voice.c
 *
 * An encapsulation of a "sound" can have multiple oscillators, ADSR, etc...
 *
 *  Created on: Apr 16, 2025
 *      Author: Connor Michalec
 */

#include "voice.h"
#include <stddef.h>
#include "midi.h"

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
		voices[i].adsr = NULL;
		voices[i].num_osc = 0;
		voices[i].NOTE = -1;
	}
}

void tick_voices() {
	// Tick all voices
	for(int i = 0; i < NUM_VOICES; i++) {
		if(voices[i].enabled) {
			tick_voice(&voices[i]);
		}
	}
}

void tick_voice(Voice * voice) {
	if(!voice->enabled)
		return;

	//Tick ADSRs:
	ADSR_tick(voice->adsr);

	//If ADSR is set to DONE state, disable this voice as it is done playing until re-enabled.
	if(voice->adsr->cur_state == DONE)
		disable_voice(voice);

}

float get_voice_ADSR_val(Voice * voice) {
	return(voice->adsr->cur_val);
}

float detune_scale = 7.0f;
void construct_voice(oscillatorTypes type, Voice * v, float frequency, float detune) {
	v->enabled = 1;


	for(int i =0; i<VOICE_NUM_OSC; i++) {
		// Create multiple oscillators detuned by detune amt

		// Alternate whether detune adds or subtracts to frequency based on if i is odd or even (creates even spread):
		// detune/VOICE_NUM_OSC so that there is the same amount of detune regardless of the amount of oscillators.
		float freq = i%2==0 ? (frequency + (float)i * (detune*detune_scale)/VOICE_NUM_OSC) : (frequency - (float)i * (detune*detune_scale)/VOICE_NUM_OSC);

		v->osc[i] = get_oscillator(enable_oscillator(type, freq));
		v->num_osc++;
	}

	// Create adsr for this voice:
	v->adsr = create_ADSR(0.2f, 1.0f, 0.0f, 1.0f, 0.4f);

	// Go to attack phase of adsr
	ADSR_next(v->adsr);

}

int enable_voice(oscillatorTypes type, int note, float detune) {
	int idx = getFirstDisabledVoiceIdx();

	// No voices avail
	if(idx == -1) return -1;

	// Make sure there are enough oscillators available for this Voice to be enabled:
	if(VOICE_NUM_OSC > get_num_oscillators()-num_enabled_oscillators()) {
		return -1;
	}

	Voice * v = get_voice_from_idx(idx);

	// Check if this note is already active:
	if(get_voice_from_note(note)!=NULL) {
		// Already a voice with this note, disable that voice:
		disable_voice(get_voice_from_note(note));
	}


	// Create new voice with this note:
	v->NOTE = note;

	construct_voice(type, v, ToFrequency(note), detune);

	num_voices_enabled++;

	return idx;
}

void disable_voice(Voice * voice) {
	// Check if voice is actually enabled:
	if(voice==NULL || !voice->enabled)
		return;

	// Disable all oscillators associated w/
	for(int i =0; i<voice->num_osc; i++) {
		disable_oscillator(voice->osc[i]);
		voice->osc[i] = NULL;
	}

	voice->num_osc = 0;

	delete_ADSR(voice->adsr);

	voice->adsr = NULL;
	voice->enabled = 0;
	voice->NOTE = -1;

	num_voices_enabled--;

}

Voice * get_voice_from_idx(int voice_idx) {
	if(voice_idx==-1)
		return NULL;

	return &voices[voice_idx];
}

Voice * get_voice_from_note(int note) {
	if(note==-1)
		return NULL;

	for(int i = 0; i<NUM_VOICES; i++) {
		if(voices[i].NOTE==note) {
			return(&voices[i]);
		}
	}
	return NULL;
}



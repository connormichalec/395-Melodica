/*
 * signal.c
 *
 *  Created on: Apr 4, 2025
 *      Author: Connor Michalec
 */

#include "signal.h"
#include "oscillator.h"
#include "midi.h"
#include "ADSR.h"

#define MIDI_NUM_NOTES 128

int sample_rate;				// Sample rate of DAC

// This array keeps track of of what oscillator index a key is tacked to. When a keypress happens that key will be set to an osc idx.
int key_oscillators[MIDI_NUM_NOTES];

void initialize_signal(int sample_rate_) {
	sample_rate = sample_rate_;
	init_oscillators();
	init_adsrs();
	for(int i = 0; i<MIDI_NUM_NOTES; i++) {
		// -1 means no oscillator assigned
		key_oscillators[i] = -1;
	}
}


void keyboard_update(uint8_t note, uint8_t state) {
	// for now state is just turn on or off
	if(state) {
		// Key turned on, assign an oscillator to that key.
		int idx = enable_oscillator(SAW, ToFrequency(note));
		key_oscillators[note] = idx;
	}
	else {
		// Key turned off, disable oscillator assigned to that key. (TODO: Dont disable oscillator, just update adsr)
		int idx = key_oscillators[note];
		disable_oscillator(get_oscillator(idx));
	}
}

float signal_next_sample() {

	// Go through oscillators and retrieve a sample, note I am using a phase accumulator to keep track of waveform.

	float val = 0.0f;

	for(int i = 0; i<get_num_oscillators(); i++) {
		Oscillator* o = get_oscillator(i);									// Current oscillator we are dealing with

		if(o->enabled) {
			float phase_increment = (o->frequency/(float)sample_rate);			// How much to increment the phase each step.

			o->phase += phase_increment;
			if(o->phase >= 1.0f)
				o->phase = 0.0f;															// reset phase to 0 if phase gets to 1.

			val = val + o->oscillatorFunction(o->phase);									// Get sample value for that part of the phase
		}
	}

	// Normalize val based on num of oscillators active:
	val = val / (float)num_enabled_oscillators();

	return val;
}


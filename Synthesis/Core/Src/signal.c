/*
 * signal.c
 *
 *  Created on: Apr 4, 2025
 *      Author: Connor Michalec
 */

#include "signal.h"
#include "oscillator.h"
#include "midi.h"
#include "voice.h"

#define MIDI_NUM_NOTES 128

int sample_rate;				// Sample rate of DAC

// This array keeps track of of what voice index a key is tacked to. When a keypress happens that key will be set to a voice idx
int key_voices[MIDI_NUM_NOTES];


void initialize_signal(int sample_rate_) {
	sample_rate = sample_rate_;
	init_voices();

	for(int i = 0; i<MIDI_NUM_NOTES; i++) {
		// -1 means no voice assigned
		key_voices[i] = -1;
	}
}

void keyboard_update(uint8_t note, uint8_t state) {
	if(state) {
		// Key turned on, assign an oscillator to that key.
		int idx = enable_voice(SAW, ToFrequency(note), 0.2f);  // apply a slight detune to voice
		key_voices[note] = idx;
	}
	else {
		// Key turned off, progress set ADSR to "release" state
		int idx = key_voices[note];
		key_voices[note] = -1;
		ADSR_set_state(get_voice(idx)->adsr,RELEASE);
	}
}

float signal_next_sample() {

	// Otherwise all oscillators will max out volume automatically and so adding them would not work.
	float osc_scaling_fctr = 0.01f;			// how much to scale each oscillator by - TODO: replace this with a more professional solution.

	float val = 0.0f;

	// TODO: Instead of disabling interrupts when running this, make a queue for functions that need to run mutually exclusive with this one so that when
	// 		 Interrupts are called on them they wont cause a hard fault:

	// Go through oscillators and retrieve a sample, note I am using a phase accumulator to keep track of waveform.
	if(num_enabled_oscillators()==0) {
		return val;
	}

	// Tick voices:
	tick_voices();



	// Go through all voices:
	for(int q = 0; q <get_num_voices(); q++) {

		Voice * v = get_voice(q);

		if(v->enabled) {

			// Get all oscillators of that voice:
			for(int i = 0; i<v->num_osc; i++) {

				Oscillator* o = v->osc[i];

				if(o->enabled) {
					float phase_increment = (o->frequency/(float)sample_rate);						// How much to increment the phase each step.

					o->phase += phase_increment;

					if(o->phase >= 1.0f)
						o->phase = 0.0f;															// reset phase to 0 if phase gets to 1.


					val = val + o->oscillatorFunction(o->phase) * osc_scaling_fctr;					// Get sample value for that part of the phase
					val = val * get_voice_ADSR_val(v);

				}
			}

		}

	}



	//Clip val if it goes above 1:
	if(val>1.0f)
		val=1.0f;

	return val;
}


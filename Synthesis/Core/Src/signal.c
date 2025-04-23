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

int sample_rate;				// Sample rate of DAC


void initialize_signal(int sample_rate_) {
	sample_rate = sample_rate_;
	init_voices();
}

void keyboard_update(uint8_t note, uint8_t state) {
	if(state) {
		// Key turned on, assign a voice to that key.
		enable_voice(SAW, note, 0.2f);  // apply a slight detune to voice
	}
	else {
		// Key turned off, progress set ADSR to "release" state
		ADSR_set_state(get_voice_from_note(note)->adsr,RELEASE);
	}
}

float signal_next_sample() {

	// Otherwise all oscillators will max out volume automatically and so adding them would not work.
	float voice_scaling_fctr = 0.1f;			// how much to scale each voice by - TODO: replace this with a more professional solution.

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

		Voice * v = get_voice_from_idx(q);

		if(v->enabled) {

			float voice_val = 0.0f;

			// Get all oscillators of that voice:
			for(int i = 0; i<v->num_osc; i++) {

				Oscillator* o = v->osc[i];

				if(o->enabled) {
					float phase_increment = (o->frequency/(float)sample_rate);						// How much to increment the phase each step.

					o->phase += phase_increment;

					if(o->phase >= 1.0f)
						o->phase = 0.0f;															// reset phase to 0 if phase gets to 1.


					voice_val = voice_val + o->oscillatorFunction(o->phase);									// Get sample value for that part of the phase

				}
			}
			// Apply ADSR factor:
			voice_val = voice_val * get_voice_ADSR_val(v);

			// Apply voice scaling factor to normalize and add to final val
			val = val + voice_val * voice_scaling_fctr;

		}

	}



	//Clip val if it goes above 1:
	if(val>1.0f)
		val=1.0f;

	return val;
}


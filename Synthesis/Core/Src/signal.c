/*
 * signal.c
 *
 *  Created on: Apr 4, 2025
 *      Author: Connor Michalec
 */

#include "signal.h"
#include "oscillator.h"
#include "midi.h"
#include <math.h>
#include "voice.h"

int sample_rate;				// Sample rate of DAC


#define NUM_CHANNELS 1

float channel_pressures[NUM_CHANNELS];


// From chatgpt:
#define LUT_SIZE 128
float logLUT[LUT_SIZE];

// Function to generate the LUT (precompute)
void generateLogLUT(float k) {
    for (int i = 0; i < LUT_SIZE; ++i) {
        float x = (float)i / (LUT_SIZE - 1);
        logLUT[i] = logf(k * x + 1.0f) / logf(k + 1.0f);
    }
}


// Function to get the log curve at runtime with interpolation
float log_LUT(float x) {
    if (x <= 0.0f) return 0.0f;
    if (x >= 1.0f) return 1.0f;

    float scaled = x * (LUT_SIZE - 1);
    int idx = (int)scaled;
    float frac = scaled - idx;

    float y0 = logLUT[idx];
    float y1 = logLUT[idx + 1];

    return y0 + frac * (y1 - y0);  // Linear interpolation
}



float get_channel_pressure(int channel) {
	return channel_pressures[channel];
}

void initialize_signal(int sample_rate_) {
	sample_rate = sample_rate_;
	init_voices();

	for(int i =0; i<NUM_CHANNELS; i++) {
		channel_pressures[i] = 0.0f;
	}

	// Used later in volume pressure demo, number is steepness of curve for pressure "sensitivity"
	generateLogLUT(10);
}

void keyboard_update(uint8_t val, uint8_t state) {
	if(state == 1) {
		// Key turned on, assign a voice to that key.
		enable_voice(SAW, val, 0.2f);  // apply a slight detune to voice
	}
	else if (state == 0) {
		// Key turned off, progress set ADSR to "release" state
		ADSR_set_state(get_voice_from_note(val)->adsr,RELEASE);
	}
	else if (state == 2) {
		// note pressure update, (not implemented: update pressure for voices of that channel)

		// Update for all voices(todo: only voice of prssure channel is assign to - for looping):
		channel_pressures[0] = (float) val / (float) 127;


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

			// SIGNAL CHAIN:

			// Apply ADSR factor:
			voice_val = voice_val * get_voice_ADSR_val(v);

			// Apply voice pressure factor as a scaling for volume: - apply a log curve to this to not have to blow as hard
			voice_val = voice_val * log_LUT(channel_pressures[get_voice_channel(v)]);
			//set_voice_detune(v, log_LUT(channel_pressures[get_voice_channel(v)]));

			// Apply voice scaling factor to normalize and add to final val
			val = val + voice_val * voice_scaling_fctr;

		}

	}



	//Clip val if it goes above 1:
	if(val>1.0f)
		val=1.0f;

	return val;
}


/*
 * signal.c
 *
 *  Created on: Apr 4, 2025
 *      Author: Connor Michalec
 */

#include "signal.h"
#include "oscillator.h"

int sample_rate;				// Sample rate of DAC

Oscillator oscillators[50];		// Oscillator array, oscillators currently active. Up to 50
int num_oscillators;			// Number of oscillators currently enabled

void initialize_signal(int sample_rate_) {
	sample_rate = sample_rate_;
	num_oscillators = 0;

}

void enable_oscillator(oscillatorTypes oscillator, float frequency) {
	float startPhase = 0.0f;

	switch(oscillator) {
	case SIN:
		oscillators[num_oscillators].oscillatorFunction = &sin_oscillator;
		break;
	case SAW:
		oscillators[num_oscillators].oscillatorFunction = &saw_oscillator;
		break;
	}

	oscillators[num_oscillators].phase = startPhase;
	oscillators[num_oscillators].frequency = frequency;									// Freq this oscillator should be initialized to.
	num_oscillators++;
	return;
}

float signal_next_sample() {

	// Go through oscillators and retrieve a sample, note I am using a phase accumulator to keep track of waveform.

	float val = 0.0f;

	for(int i = 0; i<num_oscillators; i++) {
		Oscillator* o = &oscillators[i];									// Current oscillator we are dealing with
		float phase_increment = (o->frequency/(float)sample_rate);			// How much to increment the phase each step.

		o->phase += phase_increment;
		if(o->phase >= 1.0f)
			o->phase = 0.0f;															// reset phase to 0 if phase gets to 1.

		val = val + o->oscillatorFunction(o->phase);									// Get sample value for that part of the phase
	}

	// Normalize val based on num of oscillators active:
	val = val / (float)num_oscillators;

	return val;
}


/*
 * oscillator.c
 *
 *  Created on: Apr 4, 2025
 *      Author: Connor Michalec
 */

#include <math.h>
#include "oscillator.h"




//TODO: Have a seperate array of all the oscillators enabled (updated when enabled/disabled) so you dont have to go through all of them just to sample the ones currently active

// Oscillator array, all oscillators allocated to the system
Oscillator oscillators[NUM_OSCILLATORS];
int num_osc_enabled;


// OSCILLATOR TYPES:
float sin_oscillator(float phase) {
	return(sinf(2 * M_PI * phase));
}

float saw_oscillator(float phase) {
	return(phase);
}

float square_oscillator(float phase) {
	// High for half the phase, low for other half of phase.
	return(phase < 0.5f);
}

void init_oscillators() {
	for(int i = 0; i < NUM_OSCILLATORS; i++) {
		oscillators[i].enabled = 0;
		oscillators[i].oscillatorFunction = NULL;
		oscillators[i].phase = 0.0f;
		oscillators[i].frequency = 0.0f;
		oscillators[i].osc_idx = i;
	}
}

int get_num_oscillators() {
	return NUM_OSCILLATORS;
}

int num_enabled_oscillators() {
	return num_osc_enabled;
}

int getFirstDisabledOscIdx() {
	for(int x = 0; x<NUM_OSCILLATORS; x++) {
		if(oscillators[x].enabled==0) {
			return x;
		}
	}
	return -1;
}

float get_osc_freq(Oscillator * osc) {
	return(osc->frequency);
}

void set_osc_freq(Oscillator * osc, float freq) {
	osc->frequency = freq;
}

int enable_oscillator(oscillatorTypes oscillator, float frequency) {
	int idx = getFirstDisabledOscIdx();
	// No oscillators avail
	if(idx == -1) return -1;

	Oscillator* o = &oscillators[idx];

	switch(oscillator) {
	case SIN:
		o->oscillatorFunction = &sin_oscillator;
		break;
	case SAW:
		o->oscillatorFunction = &saw_oscillator;
		break;
	case SQUARE:
		o->oscillatorFunction = &square_oscillator;
		break;
	}

	o->phase = 0.0f;
	o->frequency = frequency;
	o->enabled = 1;

	num_osc_enabled++;

	return idx;
}

void disable_oscillator(Oscillator * oscillator) {
	// Check if oscillator is actually enabmled:
	if(oscillator==NULL || !oscillator->enabled)
		return;

	oscillator->oscillatorFunction = NULL;
	oscillator->phase = 0.0f;
	oscillator->frequency = 0.0f;
	oscillator->enabled = 0;
	num_osc_enabled--;

}

Oscillator * get_oscillator(int oscillator_idx) {
	if(oscillator_idx==-1)
		return NULL;

	return &oscillators[oscillator_idx];
}

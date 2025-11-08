/*
 * controlstate.c
 *
 *  Created on: Oct 22, 2025
 *      Author: Connor Michalec
 */

#include "controlstate.h"
#include <stdlib.h>

// global state
SynthesisParameters* state;

void set_profile_defaults(Synthesis_profile* profile) {
	profile->gain = 1.0f;

	profile->channel = -1;				// -1 means profile applied to all channels
	profile->detune = 0.2f;
	profile->voice_num_osc = VOICE_NUM_OSC_MAX;

	profile->oscillatorType = SAW;

	profile->adsr_attack_factor = 0.0f;
	profile->adsr_attack_level = 1.0f;
	profile->adsr_decay_factor = 0.0f;
	profile->adsr_sustain_level = 1.0f;
	profile->adsr_release_factor = 0.03f;

	profile->filter1_type = LOWPASS;
	profile->filter1_cutoff = 0.2f;
	profile->filter1_resonance = 0.0f;
}


void initialize_controlstate() {
	// For now, the synthesizer only uses 1 profile that will just track inputs, in the future consider having independant profiles that can be loaded and "edited" to the current switches states given some button press
	// This would give the flexbiility of having multiple profiles for easy switching, or something like

	// TODO: Deconstruct this malloc
	state = (SynthesisParameters*) malloc(sizeof(SynthesisParameters));		// (we never free since this variable should exist the entire lifetime of the runtime)
	state->profile_amount = 0;
	state->active_profile = -1;


	// First create a profile with default values:
	state->profile_amount++;
	state->active_profile = 0;
	set_profile_defaults(&(state->synthesis_profile[0]));

	// Now we will have one profile to work with

}

SynthesisParameters* get_controlstate() {
	return state;
}


Synthesis_profile* get_controlstate_active_profile() {
	return &(state->synthesis_profile[state->active_profile]);
}

// Just linearlly normalizes fixed point input to a float based on defined input range
float linearNormalize(unsigned int input) {
	return(((float) input) / ((float) CONTROL_RANGE));
}


// When we receive a state update from switchbox, update values accordingly. two psosible inputs, absolute values, or relative values.
void update_parameter(unsigned int parameter_id, unsigned int control_type, unsigned int val) {
	switch(parameter_id) {
		case PARAMTER_ID_GAIN:
			get_controlstate_active_profile()->gain = linearNormalize(val);
			break;
		default:
			break;
	}
}

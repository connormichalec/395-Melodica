/*
 * controlstate.c
 *
 *  Created on: Oct 22, 2025
 *      Author: Connor Michalec
 */

#include "controlstate.h"
#include <stdlib.h>
#include <string.h>
#include "signal.h"

// global state
SynthesisParameters* state;

void set_profile_defaults(Synthesis_profile* profile) {
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

extern float filter1_cutoff_base;
extern float detune_base;

// When we receive a state update from switchbox, update values accordingly. two psosible inputs, absolute values, or relative values.
void update_parameter(unsigned int parameter_id, unsigned int control_type, uint8_t* data) {
	float val_float;
	uint32_t val_uint32;
	switch(parameter_id) {
		case PARAMTER_ID_GAIN:					// Note: gain is not actually in the synthesis profile struct, as it is universal and does not change with profile.
		    memcpy(&val_float, data, sizeof(float));
		    set_master_gain(val_float);
			break;
		case PARAMTER_ID_DETUNE:
		    memcpy(&val_float, data, sizeof(float));
			get_controlstate_active_profile()->detune = val_float;
			detune_base = val_float;			// temporary TODO
			update_all_active_voices(get_controlstate_active_profile());
			break;
		case PARAMETER_ID_VOICE_NUM_OSC:
		    memcpy(&val_uint32, data, sizeof(uint32_t));
			get_controlstate_active_profile()->voice_num_osc = val_uint32;
			update_all_active_voices(get_controlstate_active_profile());
			break;
		case PARAMETER_ID_OSC_TYPE:
		    memcpy(&val_uint32, data, sizeof(uint32_t));
			switch(val_uint32) {
				case 0:
					get_controlstate_active_profile()->oscillatorType = SAW;
					break;
				case 1:
					get_controlstate_active_profile()->oscillatorType = SIN;
					break;
				case 2:
					get_controlstate_active_profile()->oscillatorType = SQUARE;
					break;
				default:
					break;
			}
			update_all_active_voices(get_controlstate_active_profile());
			break;
		case PARAMETER_ID_ADSR_ATTACK_FACTOR:
		    memcpy(&val_float, data, sizeof(float));
			get_controlstate_active_profile()->adsr_attack_factor = val_float;
			update_all_active_voices(get_controlstate_active_profile());
			break;
		case PARAMETER_ID_ADSR_ATTACK_LEVEL:
		    memcpy(&val_float, data, sizeof(float));
			get_controlstate_active_profile()->adsr_attack_level = val_float;
			update_all_active_voices(get_controlstate_active_profile());
			break;
		case PARAMETER_ID_ADSR_DECAY_FACTOR:
		    memcpy(&val_float, data, sizeof(float));
			get_controlstate_active_profile()->adsr_decay_factor = val_float;
			update_all_active_voices(get_controlstate_active_profile());
			break;
		case PARAMETER_ID_ADSR_SUSTAIN_LEVEL:
		    memcpy(&val_float, data, sizeof(float));
			get_controlstate_active_profile()->adsr_sustain_level = val_float;
			update_all_active_voices(get_controlstate_active_profile());
			break;
		case PARAMETER_ID_ADSR_RELEASE_FACTOR:
		    memcpy(&val_float, data, sizeof(float));
			get_controlstate_active_profile()->adsr_release_factor= val_float;
			update_all_active_voices(get_controlstate_active_profile());
			break;
		case PARAMETER_ID_FILTER1_TYPE:
			//TODO: Test once more filter types are implemented
		    memcpy(&val_uint32, data, sizeof(uint32_t));
			switch(val_uint32) {
				case 0:
					get_controlstate_active_profile()->filter1_type = LOWPASS;
					break;
				default:
					break;
			}
			update_all_active_voices(get_controlstate_active_profile());
			break;
		case PARAMETER_ID_FILTER1_CUTOFF:
		    memcpy(&val_float, data, sizeof(float));
			get_controlstate_active_profile()->filter1_cutoff = val_float;
			filter1_cutoff_base = val_float;			// temporary TODO
			update_all_active_voices(get_controlstate_active_profile());
			break;
		case PARAMETER_ID_FILTER1_RESONANCE:
		    memcpy(&val_float, data, sizeof(float));
			get_controlstate_active_profile()->filter1_resonance = val_float;
			update_all_active_voices(get_controlstate_active_profile());
			break;
		case PARAMETER_ID_BREATH_SETTING:
		    memcpy(&val_uint32, data, sizeof(uint32_t));
			get_controlstate_active_profile()->breath_setting = val_uint32;
			set_breath_sensor_setting(val_uint32);
			break;
		default:
			break;
	}
}

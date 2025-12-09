/*
 * controlstate.h
 *
 * Holds the current state of Synthesizer/effects settings, meaning it represents what the user physically wants the system to do and is updated with IO input from the switchboxes.
 * We define a default state with default values, and those values will be updated based on if we receive a switchbox signal for that parameters. (Certain parameters are exposed to these switchboxes)
 *
 * Currently parameters are compartmentalized within profiles, which provides an easy way to manage more than one specific state at once.
 *
 *  Created on: Oct 22, 2025
 *      Author: Connor Michalec
 */

#ifndef INC_CONTROLSTATE_H_
#define INC_CONTROLSTATE_H_

#include "oscillator.h"
#include "voice.h"
#include "ADSR.h"
#include "stm32h7xx_hal.h"
#include "filter.h"


#define MAX_PROFILES 100		// Number of synthesis/audio profiles to allocate aside.

// control type:
#define CONTROL_TYPE_ABSOLUTE 0
#define CONTROL_TYPE_RELATIVE 1

// Maximum value for parameters that have some fixed point input.
#define CONTROL_RANGE 1000


// parameter ids
#define PARAMTER_ID_GAIN 1						// (not in synthesis profile)
#define PARAMTER_ID_DETUNE 2
#define PARAMETER_ID_VOICE_NUM_OSC 3
#define PARAMETER_ID_OSC_TYPE 4
#define PARAMETER_ID_ADSR_ATTACK_FACTOR 5
#define PARAMETER_ID_ADSR_ATTACK_LEVEL 6
#define PARAMETER_ID_ADSR_DECAY_FACTOR 7
#define PARAMETER_ID_ADSR_SUSTAIN_LEVEL 8
#define PARAMETER_ID_ADSR_RELEASE_FACTOR 9
#define PARAMETER_ID_FILTER1_TYPE 10
#define PARAMETER_ID_FILTER1_CUTOFF 11
#define PARAMETER_ID_FILTER1_RESONANCE 12
#define PARAMETER_ID_BREATH_SETTING 13




/** PROFILES FOR SYNTHESIS AND AUDIO EFFECTS **/
typedef struct Synthesis_profile {
	// Voices:
	int channel;						// Channel setting for this profile, maybe have multiple profiles on different channels? Then split keyboard up halfway down the middle for channel 1/channel 2 in the mainboard?
	float detune;						// Detune between oscillators in voice, gives a phat sound, (the more oscillators the better for this)
	int voice_num_osc;					// Number of oscillators per voice, again, phat sound

	// Oscillators:
	oscillatorTypes oscillatorType;		// Type of waveform for oscillator

	// ADSR:
	float adsr_attack_factor;
	float adsr_attack_level;
	float adsr_decay_factor;
	float adsr_sustain_level;
	float adsr_release_factor;

	// Filter1
	FilterType filter1_type;
	float filter1_cutoff;
	float filter1_resonance;

	// Breath setting (TOODO: Think about if we might wanna move this out of the profile like gain - but i dont think that makes a lot of sense)
	int breath_setting;

} Synthesis_profile;
/** **/




/**
 * Holds parameters for synthesis and audio effects control.
 */
typedef struct SynthesisParameters {
	int profile_amount;
	Synthesis_profile synthesis_profile[MAX_PROFILES];
	int active_profile;											// Right now holds the profile currently selected to use. TODO: Active profile should instead indicate which profile will be selected for "editing" and each profile will have an "enabled" status, which means midi notes in that profiles channel will trigger that specific profile ONLY if its active.

} SynthesisParameters;

/**
 * Initialize everything, important!
 */
void initialize_controlstate();

/**
 * Sets the defaults for a synthesis profile
 */
void set_profile_defaults(Synthesis_profile* profile);

/**
 * Get current control state:
 */
SynthesisParameters* get_controlstate();

/**
 * Get active profile of current control state:
 */
Synthesis_profile* get_controlstate_active_profile();

/**
 * Update a parameter, to be called by protocol when receiving an update signal from a switchbox
 */
void update_parameter(unsigned int parameter_id, unsigned int control_type, uint8_t* data);


#endif /* INC_CONTROLSTATE_H_ */

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
#include "filter.h"


#define MAX_PROFILES 100		// Number of synthesis/audio profiles to allocate aside.



/** PROFILES FOR SYNTHESIS AND AUDIO EFFECTS **/
typedef struct Synthesis_profile {
	// Master Synthesis:
	float gain;							// Digital volume (for now) - TODO: Digital dac with controllable gain via i2c to do volume control analogly

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

} Synthesis_profile;
/** **/




/**
 * Holds parameters for synthesis and audio effects control.
 */
typedef struct SynthesisParameters {
	int profile_amount;
	Synthesis_profile synthesis_profile[MAX_PROFILES];
	int active_profile;

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




#endif /* INC_CONTROLSTATE_H_ */

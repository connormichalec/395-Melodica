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
#include "filter.h"

int sample_rate;				// Sample rate of DAC


#define NUM_CHANNELS 8

float channel_pressures[NUM_CHANNELS];

// Digital volume (for now) - TODO: Digital dac with controllable gain via i2c to do volume control analogly
// Master gain
float gain = 1.0f;
// Breath sensor setting: TODO - should this be in voice instead of signal?? (as seen in comments below)
int breathSetting = 0;

float secondary_gain = 1.0f;			// Secondary gain used by voice pressure


//TODO: Implement these in a better manner: They are used to match the potentiometers current setting, that way breath sensor can add on top of the base while keeping that setting in memory
float filter1_cutoff_base = 0.0f;
float detune_base = 0.0f;


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
	generateLogLUT(5);

	//test:
	//keyboard_update(60, 1, 0);
}

void keyboard_update(uint8_t val, uint8_t state, uint8_t channel) {

	if(state == 1) {

		// TODO: Instead of using the the currently active profile, check all enabled (when thats implemented) profiles and find for one that matches the channel of this key.
		Synthesis_profile* profile = get_controlstate_active_profile();

		// Make sure profile is for this channel
		if(profile->channel == -1 || profile->channel == channel) {

			// Key turned on, assign a voice to that key.
			int i = enable_voice(profile->oscillatorType, profile->voice_num_osc, val, profile->detune);

			// DEFAULT SETUP (one filter right now):
			add_voice_filter(get_voice_from_idx(i),profile->filter1_type, profile->filter1_cutoff, profile->filter1_resonance);
			add_voice_ADSR(get_voice_from_idx(i), profile->adsr_attack_factor, profile->adsr_attack_level, profile->adsr_decay_factor, profile->adsr_sustain_level, profile->adsr_release_factor);

			set_voice_channel(get_voice_from_idx(i), channel);
		}

	}
	else if (state == 0) {
		// Key turned off, progress set ADSR to "release" state

		Voice* v = get_voice_from_note(val);
		if (v != NULL) {
			ADSR_set_state(v->adsr, RELEASE);
		}

	}
	else if (state == 2) {
		// note pressure update, (not implemented: update pressure for voices of that channel)

		// Update pressure assigned to that voice
		channel_pressures[channel] = (float) val / (float) 127;

		float pres = channel_pressures[channel];

		Synthesis_profile* p = get_controlstate_active_profile();

		float t;			// TODO: Fix jankiness (rushed here)

		// Breath sensor updates
		switch(breathSetting) {
			case 0:		// Volume and filter
				secondary_gain = log_LUT(pres);
				t = filter1_cutoff_base + log_LUT(pres);
				if(t<0.0f)
					t=0.0f;
				else if(t>1.0f)
					t = 1.0f;
				p->filter1_cutoff = t;
				break;
			case 2:		// Just Filter (set filter0 to pres plus current pot setting)
				t = filter1_cutoff_base + log_LUT(pres);
				if(t<0.0f)
					t=0.0f;
				else if(t>1.0f)
					t = 1.0f;
				p->filter1_cutoff = t;
				break;
			case 1:		// Detune (add to current pot setting)
				t = detune_base + log_LUT(pres);
				if(t<0.0f)
					t=0.0f;
				else if(t>1.0f)
					t = 1.0f;
				p->detune = t;
				break;
			default:
				break;

		}
		update_all_active_voices(p);


	}
}

// Updates live voice to new parameters
void update_voice(Voice* v, Synthesis_profile* newParameters) {
	// Only one filter right now, only set first one:

	set_voice_detune(v, newParameters->detune);
	set_voice_oscillators(v, newParameters->voice_num_osc, newParameters->oscillatorType, v->base_freq);

	//update filter1:
	Filter* f = get_voice_filters(v);
	set_filter_cutoff(f, newParameters->filter1_cutoff);
	set_filter_resonance(f, newParameters->filter1_resonance);
	//TODO: Make function for:
	f->type = newParameters->filter1_type;


	//TODO: Functions for these??
	ADSR* a = get_voice_ADSR(v);
	a->attack_factor = newParameters->adsr_attack_factor;
	a->attack_level = newParameters->adsr_attack_level;
	a->decay_factor = newParameters->adsr_decay_factor;
	a->sustain_level = newParameters->adsr_sustain_level;
	a->release_factor = newParameters->adsr_release_factor;
}

// Updates all active voices to new parameters
void update_all_active_voices(Synthesis_profile* newParameters) {
	// Go through all voices:
	for(int q = 0; q <get_num_voices(); q++) {


		Voice * v = get_voice_from_idx(q);

		if(v->enabled) { // only update active voices (when new voices are created they will match the profile)
			update_voice(v, newParameters);
		}
	}
}


float signal_next_sample() {

	// Otherwise all oscillators will max out volume automatically and so adding them would not work.
	float voice_scaling_fctr = 0.07f;			// how much to scale each voice by - TODO: replace this with a more professional solution.

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
			for(int i = 0; i<v->voice_num_oscillators; i++) {

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

			// Apply voice filters

			Filter * f = get_voice_filters(v);
			while(f!=NULL) {
				voice_val = f->filterFunciton(f, voice_val);
				f = f->next;
			}

			// Apply ADSR factor:
			voice_val = voice_val * get_voice_ADSR_val(v);


			switch(breathSetting) {
				case 0:		// Volume and filter
					voice_val = voice_val * secondary_gain;
					break;
				case 2:		// Just Filter (set filter0 to pres plus current pot setting)
					// (handled in keyboard update)
					break;
				case 1:		// Detune (add to current pot setting)
					// (handled in keyboard update)
					break;
				default:
					break;

			}

			// Apply voice scaling factor to normalize and add to final val
			val = val + voice_val * voice_scaling_fctr * gain;

		}

	}



	//Clip val if it goes above 1:
	if(val>1.0f)
		val=1.0f;

	return val;
}

void set_master_gain(float val) {
	if(val>=0.0f && val <=1.0f) {
		gain = val;
	}
}

// For now this is part of the signal chain using a global variable that will update: TODO - move to voice? especially if we want to have this as profile-specific
void set_breath_sensor_setting(int val) {
	breathSetting = val;
}


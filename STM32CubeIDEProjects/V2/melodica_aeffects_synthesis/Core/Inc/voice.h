/*
 * voice.h
 *
 *  Created on: Apr 16, 2025
 *      Author: conno
 */

#ifndef INC_VOICE_H_
#define INC_VOICE_H_

#include "ADSR.h"
#include "oscillator.h"
#include "filter.h"

#define VOICE_NUM_OSC_MAX 3		// Maximum allowed amount of oscillators to be enabled for for the voice, regardless of what user setting is. Will allocate this much for oscillators, but user can control how much oscillators per voice below this cap
#define NUM_VOICES_MAX 12		// Maximum allowed simultaneous voices active at once.

// TODO: Implement function pointres in struct for more streamlined interfacing (liek a class)
typedef struct {
	Oscillator * osc[VOICE_NUM_OSC_MAX];
	int voice_num_oscillators;		// Number of oscillators to enable for this voice, number should be up to VOICE_NUM_OSC_MAX
	ADSR * adsr;
	int enabled;
	int channel;
	float detune;
	float base_freq;	// Frequency that the oscillators should be detuned around
	Filter * filters;	// Filters added via linked list to voice (head of linked list, null when no filters)
	int NOTE;			// Note that this voice is tracked to, set to -1 when not used.
} Voice;


/*
 * EXTERNALLY CONTROLLABLE PARAMETERS FOR VOICE:
 * detune
 * filters
 * channel
 * pitch bend
 */

/**
 * Initialize voices system
 */
void init_voices();


/**
 * Tick ALL voices (convinience)
 */
void tick_voices();

/**
 * Get the ADSR value, should control amplitude of final sample
 */
float get_voice_ADSR_val(Voice * voice);

/**
 * Adds an adsr to this voice
 */
void add_voice_ADSR(Voice * voice, float attack_factor, float attack_level, float decay_factor, float sustain_level, float release_factor);

/**
 * Get the voice ADSR
 */
ADSR* get_voice_ADSR(Voice * voice);

/**
 * Used to manage voice functions
 */
void tick_voice(Voice * voice);

/**
 * Get Total number of voices
 */
int get_num_voices();

/**
 * Get number of voices currently enabled
 */
int num_enabled_voices();

void construct_voice(oscillatorTypes type, Voice * v, float frequency, float detune);

/**
 * Adjust the current  oscillators for this voice
 */
void set_voice_oscillators(Voice* v, int numOscillators, oscillatorTypes type, float frequency);

/**
 * Set voice channel
 */
void set_voice_channel(Voice * voice, int newChannel);

/**
 * Enable voice using a MIDI note no
 */
int enable_voice(oscillatorTypes type, int numOscillators, int note, float detune);

/**
 * Add filter
 */
void add_voice_filter(Voice * voice, FilterType type, float cutoff, float resonance);

/**
 *
 */
void disable_voice(Voice * voice);

/**
 * Return linked list of all the filters
 */
Filter * get_voice_filters(Voice * v);

/**
 * Get numbers of filters in this voice
 */
int get_num_filters(Voice * v);


int get_voice_channel(Voice * voice);

/**
 * Set this voices detune parameter
 */
void set_voice_detune(Voice * voice, float detune);

float get_voice_detune(Voice * voice);

Voice * get_voice_from_idx(int voice_idx);

Voice * get_voice_from_note(int note);

#endif /* INC_VOICE_H_ */

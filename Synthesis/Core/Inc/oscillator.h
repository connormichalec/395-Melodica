/*
 * oscillator.h
 *
 *  Created on: Apr 4, 2025
 *      Author: conno
 */

#ifndef SRC_OSCILLATOR_H_
#define SRC_OSCILLATOR_H_

#define NUM_OSCILLATORS 36

typedef enum {
	SIN,
	SAW,
	SQUARE
} oscillatorTypes;

typedef struct {
	float (*oscillatorFunction)(float phase);			// Function to get an oscillation based on phase
	float phase;
	float frequency;
	int osc_idx;										// Oscillator index (for easy indexing of all oscillators)
	int enabled;										// Is this oscillator enabled?
} Oscillator;

/**
 * Initialize oscillator system
 */
void init_oscillators();

/**
 * Get Total number of oscillators
 */
int get_num_oscillators();

/**
 * Get number of oscillators currently enabled
 */
int num_enabled_oscillators();

/**
 * Get oscillator given its idx
 */
Oscillator * get_oscillator(int oscillator_idx);

/**
 * Enable an oscillator
 */
int enable_oscillator(oscillatorTypes oscillator, float frequency);

/**
 * Disable an oscillator
 */
void disable_oscillator(Oscillator * oscillator);

/**
 * Set osc freq
 */
void set_osc_freq(Oscillator * oscillator, float freq);

/**
 * Get osc freq
 */
float get_osc_freq(Oscillator * oscillator);

#endif /* SRC_OSCILLATOR_H_ */

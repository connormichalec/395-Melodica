/*
 * oscillator.h
 *
 *  Created on: Apr 4, 2025
 *      Author: conno
 */

#ifndef SRC_OSCILLATOR_H_
#define SRC_OSCILLATOR_H_

typedef struct {
	float (*oscillatorFunction)(float phase);			// Function to get an oscillation based on phase
	float phase;
	float frequency;
} Oscillator;

float sin_oscillator(float phase);

float saw_oscillator(float phase);

#endif /* SRC_OSCILLATOR_H_ */

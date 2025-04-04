/*
 * signal.h
 *
 *	Handles the signal processing chain
 *
 *  Created on: Apr 4, 2025
 *      Author: Connor Michalec
 */

#ifndef SRC_SIGNAL_H_
#define SRC_SIGNAL_H_


typedef enum {
	SIN,
	SAW
} oscillatorTypes;


void initialize_signal(int sample_rate_);

float signal_next_sample();

void enable_oscillator(oscillatorTypes oscillator, float frequency);


#endif /* SRC_SIGNAL_H_ */

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

#include <stdint.h>

void initialize_signal(int sample_rate_);

void keyboard_update(uint8_t val, uint8_t state);

float signal_next_sample();

float get_channel_pressure(int channel);

#endif /* SRC_SIGNAL_H_ */

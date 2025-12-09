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
#include "controlstate.h"
#include "voice.h"



void initialize_signal(int sample_rate_);

void keyboard_update(uint8_t val, uint8_t state, uint8_t channel);

float signal_next_sample();

float get_channel_pressure(int channel);

// not in synth profile (stays constant across all profiles regardless of changing them)
void set_master_gain(float val);

// in synth profile (for now)
void set_breath_sensor_setting(int val);

void update_voice(Voice* v, Synthesis_profile* newParameters);

void update_all_active_voices(Synthesis_profile* newParameters);

#endif /* SRC_SIGNAL_H_ */

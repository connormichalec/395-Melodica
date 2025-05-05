/*
 * audio.h
 *
 *  Created on: Mar 31, 2025
 *      Author: Connor Michalec
 */


#ifndef INC_AUDIO_H_
#define INC_AUDIO_H_
#include "stm32f4xx_hal.h"

/**
 * Initialize everything
 */
void audio_signal_init(TIM_HandleTypeDef * _htim8, DAC_HandleTypeDef * _hdac);

/**
 * Main loop tick for audio signal generation.
 */
void audio_signal_loop_tick();

/**
 * Callback to be triggerd by midi.c when key press occurs, this will eventually assign an oscillator to that key.
 */
void note_callbk(uint8_t note, uint8_t event, uint8_t channel);


#endif /* INC_AUDIO_H_ */

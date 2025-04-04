/*
 * audio.h
 *
 *  Created on: Mar 31, 2025
 *      Author: Connor Michalec
 */


#ifndef INC_AUDIO_H_
#define INC_AUDIO_H_
#include "stm32f4xx_hal.h"

void audio_signal_init(TIM_HandleTypeDef * _htim8, DAC_HandleTypeDef * _hdac);

/**
 * Main loop for audio signal generation.
 */
void audio_signal_loop();



#endif /* INC_AUDIO_H_ */

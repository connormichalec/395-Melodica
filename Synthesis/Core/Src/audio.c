/*
 * audio.c
 *
 *  Created on: Mar 31, 2025
 *      Author: Connor Michalec
 */

#include "audio.h"
#include "stm32f4xx_hal.h"
#include <math.h>

#define SAMPLE_RATE 42000
#define BUFFER_SIZE 256
#define FREQUENCY   440

//USEFUL:
//https://trebledj.me/posts/digital-audio-synthesis-for-dummies-part-3/

uint16_t buffer[BUFFER_SIZE];	// For ring buffer
uint32_t t = 0;					// Timestep, each count represents another sample.

// Provided from system
void * S_htim8;
void * S_hdac;

void audio_signal_init(void * _htim8, void * _hdac) {
	S_htim8 = _htim8;
	S_hdac = _hdac;

	// Start the timer.
	HAL_TIM_Base_Start(S_htim8);

	HAL_DAC_Start(_hdac, DAC_CHANNEL_1);
}

void fill_buffer(uint16_t *buffer, long start, long end) {
	float val;
	// This is the duration each increment will represent.
	float sawtooth_normalization = 1/(SAMPLE_RATE/FREQUENCY);

    for (int i = start; i < end; i++, t++) {
        //val = sinf(2 * M_PI * t* (FREQUENCY/SAMPLE_RATE));
    	val = ((t)%(SAMPLE_RATE/FREQUENCY))*sawtooth_normalization;
        buffer[i] = (uint16_t)(2047 * val + 2048); // Centered properly
    }
}

// Fill second half of buffer after first one starts playing
void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac) {
    fill_buffer(buffer, 0, BUFFER_SIZE/2);
}

// Fill first half of buffer after second half starts playing
void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef *hdac) {
	fill_buffer(buffer, BUFFER_SIZE/2, BUFFER_SIZE);
}

void audio_signal_loop() {
    // Fill first half of buffer initially
    fill_buffer(buffer, 0, BUFFER_SIZE/2);

    // Start DMA with Circular Mode
    HAL_DAC_Start_DMA(S_hdac, DAC_CHANNEL_1, (uint32_t*)buffer, BUFFER_SIZE, DAC_ALIGN_12B_R);

    while (1) {
        // Nothing needed here—DMA & callbacks handle real-time updates
    }
}

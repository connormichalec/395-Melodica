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

uint16_t buffers[2][BUFFER_SIZE]; // Double buffer
volatile uint8_t current_buffer = 0;
volatile uint8_t buffer_ready = 0;
uint32_t t = 0;

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

void fill_buffer(uint16_t *buffer) {
    for (int i = 0; i < BUFFER_SIZE; i++, t++) {
        float val = 0.2 * sinf(2 * M_PI * FREQUENCY * t / SAMPLE_RATE);
        buffer[i] = (uint16_t)(2047 * val + 2048); // Centered properly
    }
}

void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef *hdac) {
    fill_buffer(buffers[current_buffer]);
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac) {
    current_buffer = !current_buffer;
    fill_buffer(buffers[current_buffer]);
}


void audio_signal_loop() {
    // Fill both buffers initially
    fill_buffer(buffers[0]);
    fill_buffer(buffers[1]);

    // Start DMA with Circular Mode
    HAL_DAC_Start_DMA(S_hdac, DAC_CHANNEL_1, (uint32_t*)buffers[0], BUFFER_SIZE, DAC_ALIGN_12B_R);

    while (1) {
        // Nothing needed here—DMA & callbacks handle real-time updates
    }
}

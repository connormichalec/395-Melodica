/*
 * audio.c
 *
 *  DESIGNED FOR THE STMF405!
 *
 *  Created on: Mar 31, 2025
 *      Author: Connor Michalec
 */

#include "audio.h"
#include "signal.h"
#include "oscillator.h"
#include <math.h>

#define SAMPLE_RATE 42000
#define BUFFER_SIZE 2048

//USEFUL:
//https://trebledj.me/posts/digital-audio-synthesis-for-dummies-part-3/

uint16_t buffer[BUFFER_SIZE];			// For ring buffer

// Provided from system, timer and hdac pointer
void * S_htim8;
void * S_hdac;

void audio_signal_init(TIM_HandleTypeDef * _htim8, DAC_HandleTypeDef * _hdac) {
	S_htim8 = _htim8;
	S_hdac = _hdac;

	// Start the timer.
	HAL_TIM_Base_Start(S_htim8);

	HAL_DAC_Start(_hdac, DAC_CHANNEL_1);

	initialize_signal(SAMPLE_RATE);

	// some test oscillators (a 5th apart)
	enable_oscillator(SIN, 200.0f);
	enable_oscillator(SAW, 200.0f*((float)3/2));
}

void fill_buffer(uint16_t *buffer, long start, long end) {
	float val;

	// Fill all samples in buf
    for (int i = start; i < end; i++) {

    	val = signal_next_sample()*0.5f;

    	// DAC SET TO 12 bit SCALE ACCORDINGLY:
    	buffer[i] = (uint16_t)((2047) * val + 2048); // Centered properly
    }

}

// Fill second half of buffer after first one starts playing
void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac) {
    fill_buffer(buffer, BUFFER_SIZE/2, BUFFER_SIZE);
}

// Fill first half of buffer after second half starts playing
void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef *hdac) {
	fill_buffer(buffer, 0, BUFFER_SIZE/2);
}

void audio_signal_loop() {
    // Fill buffer initially
    fill_buffer(buffer, 0, BUFFER_SIZE);

    // Start DMA with Circular Mode
    HAL_DAC_Start_DMA(S_hdac, DAC_CHANNEL_1, (uint32_t*)buffer, BUFFER_SIZE, DAC_ALIGN_12B_R);

    while (1) {
        // Nothing needed here—DMA & callbacks handle real-time updates
    }
}

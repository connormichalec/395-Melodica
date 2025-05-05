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
#include <math.h>
#include "midi.h"

#define SAMPLE_RATE 42000
#define BUFFER_SIZE 2048

//USEFUL:
//https://trebledj.me/posts/digital-audio-synthesis-for-dummies-part-3/

uint16_t buffer[BUFFER_SIZE];			// For ring buffer

// Provided from system, timer and hdac pointer
void * S_htim8;
void * S_hdac;


void note_callbk(uint8_t note, uint8_t event, uint8_t channel) {
	keyboard_update(note,event,channel);
}

void fill_buffer(uint16_t *buffer, long start, long end) {
	float val;


	// Fill all samples in buf
    for (int i = start; i < end; i++) {

    	__disable_irq();
    	val = signal_next_sample();
    	__enable_irq();


    	// DAC SET TO 12 bit SCALE ACCORDINGLY:
    	buffer[i] = (uint16_t)((2047) * val + 2048); // Centered properly

    }
}

void audio_signal_init(TIM_HandleTypeDef * _htim8, DAC_HandleTypeDef * _hdac) {
	S_htim8 = _htim8;
	S_hdac = _hdac;

	// Start the timer.
	HAL_TIM_Base_Start(S_htim8);

	HAL_DAC_Start(_hdac, DAC_CHANNEL_1);

	initialize_signal(SAMPLE_RATE);

    // Fill buffer initially
    fill_buffer(buffer, 0, BUFFER_SIZE);

    // Start DMA with Circular Mode
    HAL_DAC_Start_DMA(S_hdac, DAC_CHANNEL_1, (uint32_t*)buffer, BUFFER_SIZE, DAC_ALIGN_12B_R);

}


// 0 for first half, 1 for second half:
volatile int fill_buffer_0 = 0;
volatile int fill_buffer_1 = 0;

// The reason we are using flags and not just filling buffers directly within the callback is because MIDI reception would be blocked if the interrupt is taking long to run.

// Fill second half of buffer after first one starts playing
void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac) {
	// Set fill buffer second half flag:
    fill_buffer_1 = 1;
}

// Fill first half of buffer after second half starts playing
void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef *hdac) {
	// set fill buffer first half flag:
	fill_buffer_0 = 1;
}

void audio_signal_loop_tick() {
    	if(fill_buffer_1) {
    		fill_buffer_1 = 0;
    		fill_buffer(buffer, BUFFER_SIZE/2, BUFFER_SIZE);
    	}
    	else if(fill_buffer_0) {
    		fill_buffer_0 = 0;
    		fill_buffer(buffer, 0, BUFFER_SIZE/2);
    	}
}


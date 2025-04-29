/*
 * looper.h
 *
 *  Created on: Apr 8, 2025
 *      Author: kobes
 */

#pragma once

#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_tim.h"
#include "midi.h"

#define LOOPER_GPIO_PORT GPIOB
#define LOOPER_GPIO_PIN GPIO_PIN_3

#define NOTE_MSG_COUNT 256
#define CHANNEL_PRESSURE_MSG_COUNT 512
#define LOOPER_CHANNELS 8

#define MAX_TIMESTAMP 0xFFFFFF

// Used to keep track of the state of the looper FSM
typedef enum {
	LOOPER_INACTIVE,
	LOOPER_RECORDING_INIT,
	LOOPER_RECORDING_REPEAT,
	LOOPER_LOOPING
}
looper_state;

typedef struct {
	uint32_t length;
	uint32_t tick;							// Keeps track of what tick the looper is on
	uint32_t ons[NOTE_MSG_COUNT];
	uint32_t offs[NOTE_MSG_COUNT];
	uint32_t channel_pressures[CHANNEL_PRESSURE_MSG_COUNT];
	uint16_t start_on_indices[LOOPER_CHANNELS];
	uint16_t start_off_indices[LOOPER_CHANNELS];
	uint16_t start_pressure_indices[LOOPER_CHANNELS];
	uint16_t playback_on_indices[LOOPER_CHANNELS];
	uint16_t playback_off_indices[LOOPER_CHANNELS];
	uint16_t playback_pressure_indices[LOOPER_CHANNELS];
	uint16_t playback_start_ticks[LOOPER_CHANNELS];
	uint16_t write_on_idx;
	uint16_t write_off_idx;
	uint16_t write_pressure_idx;

	looper_state state;			// Keeps track of state of the FSM
	uint8_t button_pressed;		// Tracks state of whether the looper button is pressed (for posedge detection)
	uint32_t recording_length;
	uint8_t debounce_tick;
	uint16_t hold_tick;

	uint8_t active_channel;	// Number of the current channel being recorded to

} Looper;

void LOOPER_INIT();

// Periodically updates the looper
void looper_tick();

// Generates a packed timestamped note message, stored in a 32-bit int
//	- Lowest 24 bits are timestamp
//	- Highest 8 bits are note
uint32_t timestamped_byte(uint8_t note, uint32_t timestamp);

uint32_t extract_timestamp(uint32_t data);

uint32_t extract_byte(uint32_t data);


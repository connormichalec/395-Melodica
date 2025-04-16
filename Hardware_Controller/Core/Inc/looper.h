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

// Timer peripheral
//extern TIM_HandleTypeDef htim21;

#define NOTE_STORAGE_COUNT 128

typedef struct {
	uint32_t length;
	uint32_t tick;							// Keeps track of what tick the looper is on
	uint32_t ons_[NOTE_STORAGE_COUNT * 2];  // Note on and off commands.
	uint32_t offs_[NOTE_STORAGE_COUNT * 2];
	uint32_t * read_ons;
	uint32_t * read_offs;
	uint32_t * write_ons;
	uint32_t * write_offs;

	uint8_t buffer_state;					// Determines whether buffer 0 or 1 is being written to
	uint16_t on_read_index;
	uint16_t on_write_index;
	uint16_t off_read_index;
	uint16_t off_write_index;


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


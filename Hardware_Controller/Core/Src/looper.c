/*
 * looper.c
 *
 *  Created on: Apr 8, 2025
 *      Author: kobes
 */

#include "looper.h"

extern TIM_HandleTypeDef htim21;
Looper looper;

void LOOPER_INIT() {
	for (int i = 0; i < NOTE_STORAGE_COUNT * 2; i++) {
		looper.ons_[i] = timestamped_byte(0, -1);
		looper.offs_[i] = timestamped_byte(0, -1);
	}

	looper.length = 1024 * 3;
	looper.buffer_state = 0;
	looper.off_read_index = 0;
	looper.off_write_index = 0;
	looper.on_read_index = 0;
	looper.on_write_index = 0;
	looper.tick = 0;
	looper.write_ons = looper.ons_;
	looper.write_offs = looper.offs_;
	looper.read_ons = looper.ons_ + NOTE_STORAGE_COUNT;
	looper.read_offs = looper.offs_ + NOTE_STORAGE_COUNT;
}

void looper_tick() {
	if (__HAL_TIM_GET_FLAG(&htim21, TIM_FLAG_UPDATE)) {
		__HAL_TIM_CLEAR_FLAG(&htim21, TIM_FLAG_UPDATE);
	    looper.tick++;

	    if (looper.tick >= looper.length) {
	        looper.tick = 0;

	        // Swap which buffer is being written to / read from
	        looper.buffer_state = !looper.buffer_state;
	        looper.write_ons = looper.ons_ + looper.buffer_state * NOTE_STORAGE_COUNT;
	        looper.write_offs = looper.offs_ + looper.buffer_state * NOTE_STORAGE_COUNT;
	        looper.read_ons = looper.ons_ + !looper.buffer_state * NOTE_STORAGE_COUNT;
	        looper.read_offs = looper.offs_ + !looper.buffer_state * NOTE_STORAGE_COUNT;

	        looper.on_read_index = 0;
	        looper.on_write_index = 0;
	        looper.off_read_index = 0;
	        looper.off_write_index = 0;

	        printf("%d ", looper.on_read_index);
	    }

	    // Wait for a note on to be hit in the loop
	    uint32_t next_on_timestamp = extract_timestamp(looper.read_ons[looper.on_read_index]);
	    while (next_on_timestamp == looper.tick) {
	        uint32_t note = extract_byte(looper.read_ons[looper.on_read_index]);
	        note_on(0, note, 60);

	        looper.on_read_index += 1;
	        next_on_timestamp = extract_timestamp(looper.read_ons[looper.on_read_index]);
	    }

	    // Wait for a note off to be hit in the loop
	    uint32_t next_off_timestamp = extract_timestamp(looper.read_offs[looper.off_read_index]);
	    while (next_off_timestamp == looper.tick) {
	        uint32_t note = extract_byte(looper.read_offs[looper.off_read_index]);
	        note_off(0, note, 0);

	        looper.off_read_index += 1;
	        next_off_timestamp = extract_timestamp(looper.read_offs[looper.off_read_index]);
	    }
	}
}

uint32_t timestamped_byte(uint8_t note, uint32_t timestamp) {
	return (timestamp & 0x00FFFFFF) | (note << 24);
}

uint32_t extract_timestamp(uint32_t data) {
	return data & 0x00FFFFFF;
}

uint32_t extract_byte(uint32_t data) {
	return (data & 0xFF000000) >> 24;
}


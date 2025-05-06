/*
 * looper.c
 *
 *  Created on: Apr 8, 2025
 *      Author: kobes
 */


/* FSM OUTLINE:
 *
 * States:
 * - INACTIVE: Not recording
 * - RECORDING_INIT: Continuously recording, waiting for second press to end loop
 * - RECORDING_REPEAT: Adding new layer onto the recording. Does not set loop length. Allocates a new channel
 * - LOOPING: Currently playing playing a loop
 *
 * Transitions:
 * - INACTIVE > button pressed > RECORDING_INIT
 * - RECORDING_INIT > button pressed > LOOPING
 * - LOOPING > button pressed > RECORDING_REPEAT
 * - RECORDING_REPEAT > button pressed > LOOPING
 * - RECORDING REPEAT > press and hold > INACTIVE
 */


#include "looper.h"
#include "signal.h"

extern TIM_HandleTypeDef htim2;
Looper looper;

void LOOPER_INIT() {
	for (int i = 0; i < NOTE_MSG_COUNT; i++) {
		looper.ons[i] = timestamped_byte(0, MAX_TIMESTAMP);
		looper.offs[i] = timestamped_byte(0, MAX_TIMESTAMP);
	}

	for (int i = 0; i < CHANNEL_PRESSURE_MSG_COUNT; i++)
		looper.channel_pressures[i] = timestamped_byte(0, MAX_TIMESTAMP);

	looper.start_on_indices[0] = 0;
	looper.start_off_indices[0] = 0;
	looper.start_pressure_indices[0] = 0;
	looper.playback_start_ticks[0] = 0;
	for (int i = 1; i < LOOPER_CHANNELS; i++) {
		looper.start_on_indices[i] = -1;
		looper.start_off_indices[i] = -1;
		looper.start_pressure_indices[i] = -1;
		looper.playback_start_ticks[i] = -1;
	}

	looper.length = -1;
	looper.write_off_idx = 0;
	looper.write_on_idx = 0;
	looper.write_pressure_idx = 0;
	looper.tick = 0;

	looper.state = LOOPER_INACTIVE;
	looper.active_channel = 0;	// Number is one less than channel being recorded to (because 0 is used for live play). Corresponds to active index
}

void looper_press_button() {
	// FSM CONTROL
	switch (looper.state) {
		case LOOPER_INACTIVE:
			looper.recording_length = 0;
			looper.state = LOOPER_RECORDING_INIT;
			break;

		case LOOPER_RECORDING_INIT:
			looper.length = looper.recording_length;
			looper.tick = looper.recording_length;
			looper.state = LOOPER_LOOPING;

			record_note_off_all();
			looper.write_on_idx++;		// Writes a breakpoint into the array
			looper.write_off_idx++;
			looper.write_pressure_idx++;
			looper.active_channel++;
			looper.start_on_indices[looper.active_channel] = looper.write_on_idx;
			looper.start_off_indices[looper.active_channel] = looper.write_off_idx;
			looper.start_pressure_indices[looper.active_channel] = looper.write_pressure_idx;

			break;

		case LOOPER_RECORDING_REPEAT:
			looper.state = LOOPER_LOOPING;

			record_note_off_all();
			looper.write_on_idx++;		// Writes a breakpoint into the array
			looper.write_off_idx++;
			looper.write_pressure_idx++;
			looper.active_channel++;
			looper.start_on_indices[looper.active_channel] = looper.write_on_idx;
			looper.start_off_indices[looper.active_channel] = looper.write_off_idx;
			looper.start_pressure_indices[looper.active_channel] = looper.write_pressure_idx;

			break;

		case LOOPER_LOOPING:
			looper.state = LOOPER_RECORDING_REPEAT;
			looper.playback_start_ticks[looper.active_channel] = looper.tick;
			break;
	}
}

void looper_tick() {
	if (__HAL_TIM_GET_FLAG(&htim2, TIM_FLAG_UPDATE)) {
		__HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE);

		// If not actively looping, ignore all of this
		if (looper.state == LOOPER_INACTIVE) return;
		if (looper.state == LOOPER_RECORDING_INIT) looper.recording_length++;

		looper.tick++;
		// When end of loop hits, reset things
		if (looper.tick >= looper.length) {
			looper.tick = 0;
			looper.playback_on_indices[0] = looper.start_on_indices[0];
			looper.playback_off_indices[0] = looper.start_off_indices[0];
			looper.playback_pressure_indices[0] = looper.start_pressure_indices[0];
		}

		for (int i = 1; i < LOOPER_CHANNELS; i++) {
			if (looper.tick == looper.playback_start_ticks[i]) {
				looper.playback_on_indices[i] = looper.start_on_indices[i];
				looper.playback_off_indices[i] = looper.start_off_indices[i];
				looper.playback_pressure_indices[i] = looper.start_pressure_indices[i];
			}
		}

		if (looper.tick == looper.playback_start_ticks[looper.active_channel]) {
			if (looper.state == LOOPER_RECORDING_INIT) {
				looper.length = looper.recording_length;
				looper.tick = looper.recording_length;
				looper.state = LOOPER_RECORDING_REPEAT;

				record_note_off_all();
				looper.write_on_idx++;		// Writes a breakpoint into the array
				looper.write_off_idx++;
				looper.write_pressure_idx++;
				looper.active_channel++;
				looper.start_on_indices[looper.active_channel] = looper.write_on_idx;
				looper.start_off_indices[looper.active_channel] = looper.write_off_idx;
				looper.start_pressure_indices[looper.active_channel] = looper.write_pressure_idx;
			}

			if (looper.state == LOOPER_RECORDING_REPEAT) {
				record_note_off_all();
				looper.write_on_idx++;		// Writes a breakpoint into the array
				looper.write_off_idx++;
				looper.write_pressure_idx++;
				looper.active_channel++;
				looper.start_on_indices[looper.active_channel] = looper.write_on_idx;
				looper.start_off_indices[looper.active_channel] = looper.write_off_idx;
				looper.start_pressure_indices[looper.active_channel] = looper.write_pressure_idx;
			}
		}

		/// PLAYBACK ///

		// Loop through all active channels
		for (int chan = 0; chan < looper.active_channel; chan++) {
			uint16_t * read_idx = &looper.playback_on_indices[chan];

			// Wait for a note on to be hit in the loop
			uint32_t next_on_timestamp = extract_timestamp(looper.ons[*read_idx]);
			while (next_on_timestamp == looper.tick) {
				uint32_t note = extract_byte(looper.ons[*read_idx]);
				keyboard_update(note, 1, chan + 1);
				note_on(chan + 1, note, 60);

				*read_idx += 1;
				next_on_timestamp = extract_timestamp(looper.ons[*read_idx]);
			}

			// Wait for a note off to be hit in the loop
			read_idx = &looper.playback_off_indices[chan];
			uint32_t next_off_timestamp = extract_timestamp(looper.offs[*read_idx]);
			while (next_off_timestamp == looper.tick) {
				uint32_t note = extract_byte(looper.offs[*read_idx]);
				keyboard_update(note, 0, chan + 1);
				note_off(chan + 1, note, 0);

				*read_idx += 1;
				next_off_timestamp = extract_timestamp(looper.offs[*read_idx]);
			}

			// Wait for a channel pressure message to be hit
			read_idx = &looper.playback_pressure_indices[chan];
			uint32_t next_pressure_timestamp = extract_timestamp(looper.channel_pressures[*read_idx]);
			while (next_pressure_timestamp == looper.tick) {
				uint8_t pressure = extract_byte(looper.channel_pressures[*read_idx]);
				channel_pressure(chan + 1, pressure);
				keyboard_update(pressure, 2, chan + 1);

				*read_idx += 1;
				next_pressure_timestamp = extract_timestamp(looper.channel_pressures[*read_idx]);
			}
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


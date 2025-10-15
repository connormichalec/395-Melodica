/*
 * modules.c
 *
 *  Created on: Sep 18, 2025
 *      Author: kobes
 */

// Code for module management
#include "modules.h"
#include "midi.h"
#include <string.h>

/* LIST OF MODULES
 * 	- Transpose
 * 	- Looper
 * 	- Stops
 */

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef hlpuart1;

TransposeState transpose_state;
StopsState stops_state;
PressureToggleState pressure_toggle_state;

/* Brainstorming
 * 	How do we want to manage modules in software?
 * 	Will need to be able to tell if they're connected
 * 		UART protocol
 * 		All modules have a chip, can send a heartbeat to the software
 * 		Maybe have a heartbeat system for detecting disconnect?
 *
 *	How is the state of the modules going to be stored?
 *		Use a struct for each?
 */


uint8_t msg_buf[256];	// Holds a received message
uint8_t msg_idx;
uint32_t last_rx_timestamp;
uint8_t note_counters[128];

uint8_t switchbox_byte;

ModuleStream *in_stream;		// Pointer to first modifier
ModuleStream out_stream;
ModuleStream transpose_stream;
void Module_Init() {
	for (uint8_t i = 0; i <= 127; i++) note_counters[i] = 0;
	msg_idx = 0;
	last_rx_timestamp = 0;

	HAL_UART_Receive_IT(&hlpuart1, &switchbox_byte, 1);

	// Initializing module states
	pressure_toggle_state.pressure_enabled = 1;

	// Setting up module streams

	in_stream = &out_stream;

	out_stream.data_idx = 0;
	out_stream.output = NULL;
	out_stream.update_noteon = &out_stream_update_noteon;
	out_stream.update_noteoff = &out_stream_update_noteoff;
	out_stream.update_channelpressure = &out_stream_update_channelpressure;

	transpose_stream.data_idx = 0;
	transpose_stream.output = NULL;
	transpose_stream.update_noteon = &transpose_update_noteon;
	transpose_stream.update_noteoff = &transpose_update_noteoff;
	transpose_stream.update_channelpressure = &transpose_update_channelpressure;

}

void send_msg(uint8_t device_id, uint8_t* data, uint8_t len) {
	uint8_t buf[256];
	buf[0] = device_id;
	buf[1] = device_id;	// Redundancy check

	memcpy(buf + 2, data, len);
	HAL_UART_Transmit(&MODULE_UART, buf, (uint16_t) len, 1000);
}

// Processes a byte received over UART (not MIDI). Called from UART callback
void Module_ProcessByte() {
	uint8_t byte = switchbox_byte;

	// Reset system if timeout
	uint32_t timestamp = HAL_GetTick();
	if (timestamp - last_rx_timestamp >= MODULE_RX_TIMEOUT_MS) {
		msg_idx = 0;
	}
	last_rx_timestamp = timestamp;

	// Store into buffer
	msg_buf[msg_idx] = byte;
	msg_idx++;

	// Wait until enough bytes have been read (device ID + redundancy + length + data)
	if (msg_idx >= 3 + msg_buf[2]) {
		if (msg_buf[0] == msg_buf[1]) {	// Redundancy check
			switch (msg_buf[0]) {
				case 0x00:	// Transpose IO
					handle_transpose_msg(msg_buf + 3, msg_buf[2]);
					break;
				case 0x01:  // Stops IO
					handle_stops_msg(msg_buf + 3, msg_buf[2]);
					break;
				case 0x02: 	// Looper IO
					handle_looper_msg(msg_buf + 3, msg_buf[2]);

			}
		}

		// Reset reader
		msg_idx = 0;
	}

	// Re-enable interrupt
	HAL_UART_Receive_IT(&hlpuart1, &switchbox_byte, 1);
}

void append_byte(ModuleStream* module, uint8_t byte) {
	module->data[module->data_idx] = byte;
	module->data_idx += 1;
}

// Simulates in software passing the data between different modules
// Starts at in_stream and follows the chain to out_stream
void MIDI_RunModules() {
	ModuleStream *current_stream = in_stream;

	while (current_stream != NULL) {
		// Loop through all data in the buffer
		uint8_t idx = 0;
		while (idx < current_stream->data_idx) {
			uint8_t midiStatus = current_stream->data[idx];
			uint8_t channel;
			uint8_t key;
			uint8_t velocity;
			uint8_t pressure;
			switch (midiStatus & 0xF0) {
				case 0x80: // Note Off
					channel = midiStatus & 0x0F;
					key = current_stream->data[idx + 1];
					velocity = current_stream->data[idx + 2];
					current_stream->update_noteoff(current_stream->output, channel, key, velocity);
					idx += 3;
					break;

				case 0x90: // Note On
					channel = midiStatus & 0x0F;
					key = current_stream->data[idx + 1];
					velocity = current_stream->data[idx + 2];
					current_stream->update_noteon(current_stream->output, channel, key, velocity);
					idx += 3;
					break;

				case 0xD0: // Channel Pressure
					channel = midiStatus & 0x0F;
					pressure = current_stream->data[idx + 1];
					current_stream->update_channelpressure(current_stream->output, channel, pressure);
					idx += 2;
					break;

				default:
					// If message is unsupported, kill the process
					idx = 0;
					break;
			}

		}

		current_stream = current_stream->output;	// Move to next stream
	}
}

void out_stream_update_noteon(ModuleStream* stream, uint8_t channel, uint8_t key, uint8_t velocity) {
	if (channel == 0) note_counters[key]++;

	note_on(channel, key, velocity);
}

void out_stream_update_noteoff(ModuleStream* stream, uint8_t channel, uint8_t key, uint8_t velocity) {
	if (note_counters[key] > 0) note_counters[key] --;

	if (note_counters[key] == 0) {
		note_off(channel, key, velocity);
	}
}

void out_stream_update_channelpressure(ModuleStream* stream, uint8_t channel, uint8_t pressure) {
	channel_pressure(channel, pressure);
}

void transpose_update_noteon(ModuleStream* target, uint8_t channel, uint8_t key, uint8_t velocity) {
	append_byte(target, 0x80 | channel);				// Channel and status
	append_byte(target, key + transpose_state.shift);	// Transposed key
	append_byte(target, velocity);						// Velocity
}

void transpose_update_noteoff(ModuleStream* target, uint8_t channel, uint8_t key, uint8_t velocity) {
	append_byte(target, 0x90 | channel);				// Channel and status
	append_byte(target, key + transpose_state.shift);	// Transposed key
	append_byte(target, velocity);						// Velocity
}

void transpose_update_channelpresure(ModuleStream* target, uint8_t channel, uint8_t pressure) {
	append_byte(target, 0xD0 | channel);
	append_byte(target, pressure);
}

void pressure_toggle_update_noteon(ModuleStream* target, uint8_t channel, uint8_t key, uint8_t velocity) {
	// Just pass along value
	append_byte(target, 0x80 | channel);
	append_byte(target, key);
	append_byte(target, velocity);
}

void pressure_toggle_update_noteoff(ModuleStream* target, uint8_t channel, uint8_t key, uint8_t velocity) {
	// Just pass along value
	append_byte(target, 0x90 | channel);
	append_byte(target, key);
	append_byte(target, velocity);
}

void pressure_toggle_update_channelpressure(ModuleStream* target, uint8_t channel, uint8_t pressure) {
	append_byte(target, 0xD0 | channel);
	if (pressure_toggle_state.pressure_enabled) {
		append_byte(target, pressure);
	}
	else {
		append_byte(target, 127);	// todo: figure out a good default value
	}

}

void handle_transpose_msg(uint8_t* data, uint8_t len) {
	uint8_t button = data[1];

	if (button == BUTTON_TRANSPOSE_DOWN) {
		if (transpose_state.shift > -MAX_TRANSPOSE_RANGE) transpose_state.shift -= 1;
	}

	else if (button == BUTTON_TRANSPOSE_UP) {
		if (transpose_state.shift < MAX_TRANSPOSE_RANGE) transpose_state.shift += 1;
	}
}



void handle_stops_msg(uint8_t* data, uint8_t len) {
	uint8_t stop = data[0];

	// TODO: Check if this actually works
	stops_state.stops[stop] = !stops_state.stops[stop];

	// TODO: Update all active notes
}

void handle_looper_msg(uint8_t* data, uint8_t len) {


}

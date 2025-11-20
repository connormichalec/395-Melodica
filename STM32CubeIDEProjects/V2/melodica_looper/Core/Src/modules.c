/*
 * modules.c
 *
 *  Created on: Sep 18, 2025
 *      Author: kobes
 */

// Code for module management
#include "modules.h"
#include "protocol.h"
#include "midi.h"
#include <string.h>

/* LIST OF MODULES
 * 	- Transpose
 * 	- Looper
 * 	- Stops
 */

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart2;

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


uint8_t msg_buf[512];	// Holds a received message
uint16_t msg_idx;
uint32_t last_rx_timestamp;
uint8_t note_counters[128];

uint8_t switchbox_byte;

ModuleStream *streams[16];		// Stores the order of the streams
ModuleStream out_stream;
ModuleStream transpose_stream;
ModuleStream stops_stream;
void noTick(uint32_t timestamp) {};
void Module_Init() {
	for (uint8_t i = 0; i <= 127; i++) note_counters[i] = 0;
	msg_idx = 0;
	last_rx_timestamp = 0;

	HAL_UART_Receive_IT(&huart1, &switchbox_byte, 1);

	// Initializing module states
	pressure_toggle_state.pressure_enabled = 1;
	transpose_state.shift = 0;
	stops_state.stops[0] = 0;
	stops_state.stops[1] = 1;
	stops_state.stops[2] = 0;
	stops_state.stops[3] = 0;

	// Setting up module streams

	for (uint8_t i = 1; i < 16; i++) streams[i] = NULL;
	streams[0] = &out_stream;

	out_stream.data_idx = 0;
	out_stream.update_noteon = &out_stream_update_noteon;
	out_stream.update_noteoff = &out_stream_update_noteoff;
	out_stream.update_channelpressure = &out_stream_update_channelpressure;
	out_stream.update_tick = &noTick;

	transpose_stream.data_idx = 0;
	transpose_stream.update_noteon = &transpose_update_noteon;
	transpose_stream.update_noteoff = &transpose_update_noteoff;
	transpose_stream.update_channelpressure = &noupdate_channelpressure;
	transpose_stream.update_tick = &noTick;

	stops_stream.data_idx = 0;
	stops_stream.update_noteon = &stops_update_noteon;
	stops_stream.update_noteoff = &stops_update_noteoff;
	stops_stream.update_channelpressure = &noupdate_channelpressure;
	stops_stream.update_tick = &noTick;
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

	if (msg_idx < sizeof(SwitchboxMsg)) { HAL_UART_Receive_IT(&huart1, &switchbox_byte, 1); return; }

	SwitchboxMsg sb_msg;
	memcpy(&sb_msg, msg_buf, sizeof(SwitchboxMsg));

	// Wait until enough bytes have been read
	if (msg_idx >= sizeof(SwitchboxMsg) + sb_msg.data_length) {

		if (sb_msg.control_type == CONTROL_CONNECTIVITY) {
			handle_connectivity_msg(sb_msg.device_ID, sb_msg.parameter_ID);	// Index is encoded into parameter ID for connectivity messages
		}

		else {
			switch (sb_msg.device_ID) {
				case MODULE_TRANSPOSE_ID:	// Transpose IO
					handle_transpose_msg(msg_buf + sizeof(SwitchboxMsg), sb_msg.data_length);
					break;
				case MODULE_STOPS_ID:  // Stops IO
					handle_stops_msg(msg_buf + sizeof(SwitchboxMsg), sb_msg.data_length);
					break;
				case MODULE_LOOPER_ID: 	// Looper IO
					handle_looper_msg(msg_buf + sizeof(SwitchboxMsg), sb_msg.data_length);

			}
		}

		// Reset reader
		msg_idx = 0;
	}

	// Re-enable interrupt
	HAL_UART_Receive_IT(&huart1, &switchbox_byte, 1);
}

void append_byte(ModuleStream* module, uint8_t byte) {
	module->data[module->data_idx] = byte;
	module->data_idx += 1;
}

void stream_noteon(ModuleStream* module, uint8_t channel, uint8_t key, uint8_t velocity) {
	append_byte(module, NOTE_ON | (channel & 0b00001111));
    append_byte(module, (uint8_t) 0b01111111 & key);
    append_byte(module, (uint8_t) 0b01111111 & velocity);
}

void stream_noteoff(ModuleStream* module, uint8_t channel, uint8_t key, uint8_t velocity) {
	append_byte(module, NOTE_OFF | (channel & 0b00001111));
    append_byte(module, (uint8_t) 0b01111111 & key);
    append_byte(module, (uint8_t) 0b01111111 & velocity);
}

// Simulates in software passing the data between different modules
void MIDI_RunModules() {
	for (uint8_t i = 0; i < 16 && streams[i]; i++) {				// Looping through the module streams array
		ModuleStream* current_stream = streams[i];

		// Loop through all data in the buffer
		uint8_t idx = 0;
		while (idx < current_stream->data_idx) {
			uint8_t midiStatus = current_stream->data[idx];
			uint8_t channel;
			uint8_t key;
			uint8_t velocity;
			uint8_t pressure;
			switch (midiStatus & 0xF0) {
				case NOTE_OFF: // Note Off
					channel = midiStatus & 0x0F;
					key = current_stream->data[idx + 1];
					velocity = current_stream->data[idx + 2];
					current_stream->update_noteoff(streams[i + 1], channel, key, velocity);

					idx += 3;
					break;

				case NOTE_ON: // Note On
					channel = midiStatus & 0x0F;
					key = current_stream->data[idx + 1];
					velocity = current_stream->data[idx + 2];
					current_stream->update_noteon(streams[i + 1], channel, key, velocity);

					idx += 3;
					break;

				case CHANNEL_PRESSURE: // Channel Pressure
					channel = midiStatus & 0x0F;
					pressure = current_stream->data[idx + 1];
					current_stream->update_channelpressure(streams[i + 1], channel, pressure);

					idx += 2;
					break;

				default:
					// If message is unsupported, kill the process
					idx = current_stream->data_idx;
					break;
			}

		}

		current_stream->data_idx = 0;

		// If out stream is found, stop
		if (current_stream == &out_stream) break;
	}
}

void out_stream_update_noteon(ModuleStream* stream, uint8_t channel, uint8_t key, uint8_t velocity) {
	if (channel == 0) note_counters[key]++;

	note_on(channel, key, velocity);
}

void out_stream_update_noteoff(ModuleStream* stream, uint8_t channel, uint8_t key, uint8_t velocity) {
	note_off(channel, key, velocity);
}

void out_stream_update_channelpressure(ModuleStream* stream, uint8_t channel, uint8_t pressure) {
	channel_pressure(channel, pressure);
}

void transpose_update_noteon(ModuleStream* target, uint8_t channel, uint8_t key, uint8_t velocity) {
	stream_noteon(target, channel, key + transpose_state.shift, velocity);
}

void transpose_update_noteoff(ModuleStream* target, uint8_t channel, uint8_t key, uint8_t velocity) {
	stream_noteoff(target, channel, key + transpose_state.shift, velocity);
}

void noupdate_channelpressure(ModuleStream* target, uint8_t channel, uint8_t pressure) {
	append_byte(target, 0xD0 | channel);
	append_byte(target, pressure);
}

void stops_update_noteon(ModuleStream* target, uint8_t channel, uint8_t key, uint8_t velocity) {
	for (uint8_t i = 0; i < 4; i++) {
		if (stops_state.stops[i]) stream_noteon(target, channel, key + ((i - 1) * 12), velocity);
	}
}

void stops_update_noteoff(ModuleStream* target, uint8_t channel, uint8_t key, uint8_t velocity) {
	for (uint8_t i = 0; i < 4; i++) {
		if (stops_state.stops[i]) stream_noteoff(target, channel, key + ((i - 1) * 12), velocity);
	}
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

// TODO: After a certain timeout, if no connectivity messages are received reset the stream
void handle_connectivity_msg(uint8_t device_id, uint16_t idx) {
	switch (device_id) {
		case MODULE_TRANSPOSE_ID:
			streams[idx] = &transpose_stream;
			break;

		case MODULE_STOPS_ID:
			streams[idx] = &stops_stream;
			break;
	}

	streams[idx + 1] = &out_stream;
}

//TODO: Shift all currently played notes
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

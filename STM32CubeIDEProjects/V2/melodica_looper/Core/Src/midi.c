/*
 * midi.c
 *
 *  Created on: Sep 18, 2025
 *      Author: kobes
 */


#include "midi.h"
#include "modules.h"
#include <stdio.h>
#include <stdint.h>

uint8_t midi_buffer;
extern UART_HandleTypeDef huart4;


void MIDI_INIT() {
	HAL_UART_Receive_IT(&huart4, &midi_buffer, 1);
}

void note_on(uint8_t channel, uint8_t key, uint8_t velocity) {
	MIDI_SendByte(NOTE_ON | (channel & 0b00001111));
    MIDI_SendByte((uint8_t) 0b01111111 & key);
    MIDI_SendByte((uint8_t) 0b01111111 & velocity);

    /*if ((looper.state == LOOPER_RECORDING_INIT || looper.state == LOOPER_RECORDING_REPEAT) && channel == 0) {
		looper.ons[looper.write_on_idx] = timestamped_byte(key, looper.tick);
		looper.write_on_idx += 1;
    }*/
}

void note_off(uint8_t channel, uint8_t key, uint8_t velocity) {
	MIDI_SendByte(NOTE_OFF | (channel & 0b00001111));
	MIDI_SendByte((uint8_t) 0b01111111 & key);
	MIDI_SendByte((uint8_t) 0b01111111 & velocity);

	    /*if ((looper.state == LOOPER_RECORDING_INIT || looper.state == LOOPER_RECORDING_REPEAT) && channel == 0) {
			looper.offs[looper.write_off_idx] = timestamped_byte(key, looper.tick);
			looper.write_off_idx += 1;
	    }*/
}

void channel_pressure(uint8_t channel, uint8_t pressure) {
    MIDI_SendByte(CHANNEL_PRESSURE | (channel & 0b00001111));
    MIDI_SendByte((uint8_t) 0b01111111 & pressure);

    /*if ((looper.state == LOOPER_RECORDING_INIT || looper.state == LOOPER_RECORDING_REPEAT) && channel == 0) {
		looper.channel_pressures[looper.write_pressure_idx] = timestamped_byte(pressure, looper.tick);
		looper.write_pressure_idx += 1;
    }*/
}

void MIDI_SendByte(uint8_t byte) {
    HAL_UART_Transmit(&MIDI_UART, &byte, 1, HAL_MAX_DELAY);
}

MIDI_State midiState = MIDI_WAITING_FOR_STATUS;
uint8_t midiStatus = 0;
uint8_t midiData1 = 0;
uint8_t midi_msg_buf[8];
uint8_t midi_msg_idx = 0;
void MIDI_ProcessByte() {
	//append_byte(streams[0], midi_buffer);
	midi_msg_buf[midi_msg_idx] = midi_buffer;
	midi_msg_idx++;

	if (midi_msg_buf[0] == NOTE_ON) {
		if (midi_msg_idx == 3) {
			append_byte(streams[0], midi_msg_buf[0]);
			append_byte(streams[0], midi_msg_buf[1]);
			append_byte(streams[0], midi_msg_buf[2]);
			midi_msg_idx = 0;
		}
	}

	else if (midi_msg_buf[0] == NOTE_OFF) {
		if (midi_msg_idx == 3) {
			append_byte(streams[0], midi_msg_buf[0]);
			append_byte(streams[0], midi_msg_buf[1]);
			append_byte(streams[0], midi_msg_buf[2]);
			midi_msg_idx = 0;
		}
	}

	else if (midi_msg_buf[0] == CHANNEL_PRESSURE) {
		if (midi_msg_idx == 2) {
			append_byte(streams[0], midi_msg_buf[0]);
			append_byte(streams[0], midi_msg_buf[1]);
			append_byte(streams[0], midi_msg_buf[2]);
			midi_msg_idx = 0;
		}
	}

	else {
		midi_msg_idx = 0;
	}

	// Re-enable interrupt
	HAL_UART_Receive_IT(&huart4, &midi_buffer, 1);
}

// Handle various MIDI message types
void HandleMIDIMessage(uint8_t midiStatus, uint8_t midiData1, uint8_t midiData2) {
	switch (midiStatus & 0xF0) {
		case 0x80: // Note Off
			//handle_note_off(midiStatus & 0x0F, midiData1, midiData2);
			break;

		case 0x90: // Note On
			//handle_note_on(midiStatus & 0x0F, midiData1, midiData2);

			break;

		case 0xD0: // Channel Pressure
			//handle_channel_pressure(midiStatus & 0x0F, midiData2);

			break;

		default:
			// Ignore unsupported messages for now
			break;
	}
}

void handle_note_off(uint8_t channel, uint8_t key, uint8_t velocity) {
	/*if (note_counters[key] > 0) note_counters[key] --;

	uint8_t new_key = key;
	new_key += transpose_state.shift;

	note_off(channel, new_key, velocity);*/
}

void handle_note_on(uint8_t channel, uint8_t key, uint8_t velocity) {
	/*uint8_t new_key = key;
	new_key += transpose_state.shift;
	note_on(channel, new_key, velocity);
	*/
}


void handle_channel_pressure(uint8_t channel, uint8_t pressure) {

}

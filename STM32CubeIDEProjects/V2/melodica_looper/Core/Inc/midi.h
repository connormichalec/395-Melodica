/*
 * midi.h
 *
 *  Created on: Sep 18, 2025
 *      Author: kobes
 */

#ifndef INC_MIDI_H_
#define INC_MIDI_H_

#include "stm32g4xx_hal.h"

// Codes for message types
typedef enum MessageType {
    NOTE_ON = 0b10010000,
    NOTE_OFF = 0b10000000,
    KEY_PRESSURE = 0b10100000,
    CONTROLLER_CHANGE = 0b10110000,
    PROGRAM_CHANGE = 0b11000000,
    CHANNEL_PRESSURE = 0b11010000,
    PITCH_BEND = 0b11100000
} MessageType;

typedef enum {
    MIDI_WAITING_FOR_STATUS,
    MIDI_WAITING_FOR_DATA1,
    MIDI_WAITING_FOR_DATA2
} MIDI_State;

extern UART_HandleTypeDef huart4;
#define MIDI_UART huart4

extern uint8_t note_counters[128];

// Sets up everything MIDI related
void MIDI_INIT();

void note_on(uint8_t channel, uint8_t key, uint8_t velocity);
void note_off(uint8_t channel, uint8_t key, uint8_t velocity);
void channel_pressure(uint8_t channel, uint8_t pressure);
void MIDI_SendByte(uint8_t byte);
void MIDI_ProcessByte();
void HandleMIDIMessage(uint8_t midiStatus, uint8_t midiData1, uint8_t midiData2);
void handle_note_off(uint8_t channel, uint8_t key, uint8_t velocity);
void handle_note_on(uint8_t channel, uint8_t key, uint8_t velocity);
void handle_channel_pressure(uint8_t channel, uint8_t pressure);


#endif /* INC_MIDI_H_ */

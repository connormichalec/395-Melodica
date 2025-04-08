/* midi.h
 * 	- Author: Kobe Duda
 * 	- Date: 2/12/2025
 * 	- Description: Handles midi-related processes and transfer of MIDI signals
 * 	 			   from the STM32 microprocessor
 */

#pragma once
#define MIDI_OUTPUT_PORT GPIOB
#define MIDI_OUTPUT_PIN GPIO_PIN_0
#define MIDI_CLK 4

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_uart.h"

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


// Struct for carrying MIDI data
typedef struct MidiMessage {
    uint8_t status;
    uint8_t data1;
    uint8_t data2;
} MidiMessage;

typedef struct NoteListener {
	uint8_t key;
	GPIO_TypeDef * GPIOx;
	uint16_t GPIO_Pin;
	uint8_t state;
} NoteListener;

// Message to turn a note on -- Key value is centered on 60=Middle C
void note_on(uint8_t channel, uint8_t key, uint8_t velocity);

// Message to turn a note off
void note_off(uint8_t channel, uint8_t key, uint8_t velocity);

// Message to change channel aftertouch
void channel_pressure(uint8_t channel, uint8_t pressure);

// Sends a single byte over UART for a MIDI message
void MIDI_SendByte(uint8_t byte);

// Creates a new NoteListener
NoteListener new_note(uint8_t key, GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin);

// Listens for a specific note
void listen(NoteListener * note);

/////// MIDI RX STUFF

typedef enum {
    MIDI_WAITING_FOR_STATUS,
    MIDI_WAITING_FOR_DATA1,
    MIDI_WAITING_FOR_DATA2
} MIDI_State;

typedef struct {
	uint8_t notes[128];
	uint8_t pressure;
} MIDI_Reg;

// Callback function for UART interrupt, initiates reception of MIDI bytes
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

// Needs to be called in main function; Initializes UART setup so MIDI rx can work
void MIDI_Init(void);

// Performs appropriate logic when a MIDI byte is received
void MIDI_ProcessByte(uint8_t byte);

// Converts a MIDI note to a frequency
void ToFrequency(uint8_t note);

// Handles a MIDI message (pretty self-explanatory)
void HandleMIDIMessage(uint8_t midiStatus, uint8_t midiData1, uint8_t midiData2);

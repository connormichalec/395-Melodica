#include "midi.h"
#include <math.h>
#include "looper.h"
#include "signal.h"

extern UART_HandleTypeDef hlpuart1;
extern UART_HandleTypeDef huart1;
extern Looper looper;

uint8_t note_counters[128];

void note_on(uint8_t channel, uint8_t key, uint8_t velocity) {
	MIDI_SendByte(NOTE_ON | (channel & 0b00001111));
    MIDI_SendByte((uint8_t) 0b01111111 & key);
    MIDI_SendByte((uint8_t) 0b01111111 & velocity);
}

void note_off(uint8_t channel, uint8_t key, uint8_t velocity) {
	MIDI_SendByte(NOTE_OFF | (channel & 0b00001111));
	MIDI_SendByte((uint8_t) 0b01111111 & key);
	MIDI_SendByte((uint8_t) 0b01111111 & velocity);
}

void channel_pressure(uint8_t channel, uint8_t pressure) {
    MIDI_SendByte(CHANNEL_PRESSURE | (channel & 0b00001111));
    MIDI_SendByte((uint8_t) 0b01111111 & pressure);
}

void MIDI_SendByte(uint8_t byte) {
    HAL_UART_Transmit(&huart1, &byte, 1, HAL_MAX_DELAY);
}

void record_note_off_all() {
	// For the currently recording channel, find all not-disabled notes and disable them

	// ALGORITHM
	//	- Start at starting index of current active channel
	//	- Loop until max timestamp is found
	//	- How to keep track of if a note has been turned off?
	//		- Have a byte for each note. Use counters for enabled notes
	//		- Note on adds to the counter, note off removes
	//		- For any counter that isn't 0, append a note off

	uint8_t notes[128];

	uint16_t idx = looper.start_on_indices[looper.active_channel];
	uint32_t timestamp = extract_timestamp(looper.ons[idx]);
	while (timestamp != MAX_TIMESTAMP) {
		uint8_t note = extract_byte(looper.ons[idx]);
		notes[note]++;

		idx++;
		timestamp = extract_timestamp(looper.ons[idx]);
	}

	idx = looper.start_off_indices[looper.active_channel];
	timestamp = extract_timestamp(looper.offs[idx]);
	while (timestamp != MAX_TIMESTAMP) {
		uint8_t note = extract_byte(looper.offs[idx]);
		notes[note]--;

		idx++;
		timestamp = extract_timestamp(looper.offs[idx]);
	}

	for (int i = 0; i < 128; i++) {
		if (notes[i] != 0) {
			looper.offs[looper.write_off_idx] = timestamped_byte(i, looper.tick - 1);
			looper.write_off_idx++;
		}
	}
}

NoteListener new_note(uint8_t key, GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin) {
	NoteListener output;
	output.key = key;
	output.GPIOx = GPIOx;
	output.GPIO_Pin = GPIO_Pin;
	output.state = 0;
	return output;
}

void listen(NoteListener * note) {
	if (!(note->state) && HAL_GPIO_ReadPin(note->GPIOx, note->GPIO_Pin)) {
	  note_on(0, note->key, 60);
	  note->state = 1;
	}

	if (note->state && !HAL_GPIO_ReadPin(note->GPIOx, note->GPIO_Pin)) {
	  note_off(0, note->key, 0);
	  note->state = 0;
	}
}

/////// RX STUFF
uint8_t midi_buffer;	// Buffer for incoming midi data

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) { // Ensure this is USART1's interrupt
        // Restart UART reception to keep listening - this is important to do BEFORE processbyte because processbyte could start the synthesis and interrupts wont be enabled until event callback returns.
        HAL_UART_Receive_IT(&huart1, &midi_buffer, 1);

        MIDI_ProcessByte(midi_buffer); // Your custom MIDI parsing logic
    }
}

//MODIFIED BY CONNOR: - this is a quick fix and needs to be more thoroughly implemented
void (*event_callback)(uint8_t, uint8_t);


void MIDI_Init(void (*callbackFunc)(uint8_t, uint8_t)) {
    HAL_UART_Receive_IT(&huart1, &midi_buffer, 1); // Start receiving

    event_callback = callbackFunc;

    for (int i = 0; i < 128; i++) note_counters[i] = 0;
}

MIDI_State midiState = MIDI_WAITING_FOR_STATUS;
uint8_t midiStatus = 0;
uint8_t midiData1 = 0;

char midi_string[50];

void MIDI_ProcessByte(uint8_t byte) {
	if (byte & 0x80) {		// Status byte received
		midiStatus = byte;
		midiState = (midiStatus == 0xC0 || midiStatus == 0xD0)
				  ? MIDI_WAITING_FOR_DATA1  // Program Change/Channel Pressure (1 data byte)
				  : MIDI_WAITING_FOR_DATA2; // Most messages need 2 data bytes
	} else {	// Data byte
		switch (midiState) {
			case MIDI_WAITING_FOR_DATA1:
				midiData1 = byte;

				if (midiStatus == 0xC0 || midiStatus == 0xD0) {
					// Program Change or Channel Pressure (only 1 data byte)
					HandleMIDIMessage(midiStatus, midiData1, 0);
					midiState = MIDI_WAITING_FOR_STATUS;
				} else {
					// Move to second data byte state
					midiState = MIDI_WAITING_FOR_DATA2;
				}
				break;

			case MIDI_WAITING_FOR_DATA2:
				HandleMIDIMessage(midiStatus, midiData1, byte);
				midiState = MIDI_WAITING_FOR_STATUS;
				break;

			default:
				midiState = MIDI_WAITING_FOR_STATUS; // Reset on unexpected data
				break;
		}
	}
}

float ToFrequency(uint8_t note) {
	return 440 * pow(2.0f, (float) (note - 69) / 12);
}


void HandleMIDIMessage(uint8_t midiStatus, uint8_t midiData1, uint8_t midiData2) {
	uint8_t channel = midiStatus & 0x0F;

	switch (midiStatus & 0xF0) {
		case 0x80: // Note Off
			event_callback(midiData2, 0);
		    if ((looper.state == LOOPER_RECORDING_INIT || looper.state == LOOPER_RECORDING_REPEAT) && channel == 0) {
				looper.offs[looper.write_off_idx] = timestamped_byte(midiData2, looper.tick);
				looper.write_off_idx += 1;
		    }
			break;

		case 0x90: // Note On
			event_callback(midiData2, 1);
			if ((looper.state == LOOPER_RECORDING_INIT || looper.state == LOOPER_RECORDING_REPEAT) && channel == 0) {
				looper.ons[looper.write_on_idx] = timestamped_byte(midiData2, looper.tick);
				looper.write_on_idx += 1;
			}
			break;

		case 0xD0: // Channel Pressure
			event_callback(midiData1, 2);
			break;

		case 0xF0:	// Looper button press
			if (channel == 8) {
				looper_press_button();
			} else {
				LOOPER_INIT();
			}
			break;

		default:
			// Ignore unsupported messages for now
			break;
	}
}




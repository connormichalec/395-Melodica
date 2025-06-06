#include "midi.h"

extern UART_HandleTypeDef hlpuart1;
extern UART_HandleTypeDef huart2;
extern Looper looper;

uint8_t note_counters[128];

void note_on(uint8_t channel, uint8_t key, uint8_t velocity) {
	if (channel == 0) note_counters[key]++;
	MIDI_SendByte(NOTE_ON | (channel & 0b00001111));
    MIDI_SendByte((uint8_t) 0b01111111 & key);
    MIDI_SendByte((uint8_t) 0b01111111 & velocity);

    /*if ((looper.state == LOOPER_RECORDING_INIT || looper.state == LOOPER_RECORDING_REPEAT) && channel == 0) {
		looper.ons[looper.write_on_idx] = timestamped_byte(key, looper.tick);
		looper.write_on_idx += 1;
    }*/
}

void note_off(uint8_t channel, uint8_t key, uint8_t velocity) {
	if (channel == 0 && note_counters[key] != 0) note_counters[key]--;

	if (channel != 0 || note_counters[key] == 0) {
		MIDI_SendByte(NOTE_OFF | (channel & 0b00001111));
		MIDI_SendByte((uint8_t) 0b01111111 & key);
		MIDI_SendByte((uint8_t) 0b01111111 & velocity);

	    /*if ((looper.state == LOOPER_RECORDING_INIT || looper.state == LOOPER_RECORDING_REPEAT) && channel == 0) {
			looper.offs[looper.write_off_idx] = timestamped_byte(key, looper.tick);
			looper.write_off_idx += 1;
	    }*/
	}


}

void note_off_all() {
	for (uint8_t chan = 0; chan < LOOPER_CHANNELS; chan++) {
		for(uint8_t i = 0; i < 128; i++) {
			note_counters[i] = 0;
			MIDI_SendByte(NOTE_OFF | (chan & 0b00001111));
			MIDI_SendByte((uint8_t) 0b01111111 & i);
			MIDI_SendByte((uint8_t) 0b01111111 & 0);
		}
	}
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

void channel_pressure(uint8_t channel, uint8_t pressure) {
    MIDI_SendByte(CHANNEL_PRESSURE | (channel & 0b00001111));
    MIDI_SendByte((uint8_t) 0b01111111 & pressure);

    /*if ((looper.state == LOOPER_RECORDING_INIT || looper.state == LOOPER_RECORDING_REPEAT) && channel == 0) {
		looper.channel_pressures[looper.write_pressure_idx] = timestamped_byte(pressure, looper.tick);
		looper.write_pressure_idx += 1;
    }*/
}

void MIDI_SendByte(uint8_t byte) {
    HAL_UART_Transmit(&huart2, &byte, 1, HAL_MAX_DELAY);
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
    if (huart->Instance == USART2) { // Ensure this is USART2's interrupt
        MIDI_ProcessByte(midi_buffer); // Your custom MIDI parsing logic

        // Restart UART reception to keep listening
        HAL_UART_Receive_IT(&huart2, &midi_buffer, 1);
    }
}

void MIDI_Init(void) {
    HAL_UART_Receive_IT(&huart2, &midi_buffer, 1); // Start receiving

    for (int i = 0; i < 128; i++) note_counters[i] = 0;
}

MIDI_State midiState = MIDI_WAITING_FOR_STATUS;
uint8_t midiStatus = 0;
uint8_t midiData1 = 0;

char midi_string[50];

void MIDI_ProcessByte(uint8_t byte) {
	// Just immediately pass on the byte
	MIDI_SendByte(byte);

/*	if (byte & 0x80) {		// Status byte received
		midiStatus = byte;
		midiState = ((midiStatus & 0xF0) == 0xC0 || (midiStatus  & 0xF0) == 0xD0 || (midiStatus & 0xF0) == 0xF0)
				  ? MIDI_WAITING_FOR_DATA1  // Program Change/Channel Pressure (1 data byte)
				  : MIDI_WAITING_FOR_DATA2; // Most messages need 2 data bytes
	} else {	// Data byte
		switch (midiState) {
			case MIDI_WAITING_FOR_DATA1:
				midiData1 = byte;

				if ((midiStatus & 0xF0) == 0xC0 || (midiStatus  & 0xF0) == 0xD0 || (midiStatus & 0xF0) == 0xF0) {
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
	}*/
}

float ToFrequency(uint8_t note) {
	return 440 * pow(2.0f, (float) (note - 69) / 12);
}

// From STM32 side, all received messages are coming from the synthesis chip looper. They need to be basically regurgitated back out
void HandleMIDIMessage(uint8_t midiStatus, uint8_t midiData1, uint8_t midiData2) {
	switch (midiStatus & 0xF0) {
		case 0x80: // Note Off
			note_off(midiStatus & 0x0F, midiData1, 0);

			break;

		case 0x90: // Note On
			note_on(midiStatus & 0x0F, midiData1, 60);

			break;

		case 0xD0: // Channel Pressure
			channel_pressure(midiStatus & 0x0F, midiData2);
			break;

		default:
			// Ignore unsupported messages for now
			break;
	}
}


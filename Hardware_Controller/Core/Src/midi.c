#include "midi.h"

extern UART_HandleTypeDef hlpuart1;
extern UART_HandleTypeDef huart2;
extern Looper looper;

uint8_t note_counters[128];

void note_on(uint8_t channel, uint8_t key, uint8_t velocity) {
	note_counters[key]++;
	MIDI_SendByte(NOTE_ON | (channel & 0b00001111));
    MIDI_SendByte((uint8_t) 0b01111111 & key);
    MIDI_SendByte((uint8_t) 0b01111111 & velocity);
	looper.write_ons[looper.on_write_index] = timestamped_byte(key, looper.tick);
	looper.on_write_index += 1;
}

void note_off(uint8_t channel, uint8_t key, uint8_t velocity) {
	if (note_counters[key] != 0) note_counters[key]--;

	if (note_counters[key] == 0) {
		MIDI_SendByte(NOTE_OFF | (channel & 0b00001111));
		MIDI_SendByte((uint8_t) 0b01111111 & key);
		MIDI_SendByte((uint8_t) 0b01111111 & velocity);
	}
	looper.write_offs[looper.off_write_index] = timestamped_byte(key, looper.tick);
	looper.off_write_index += 1;
}

void channel_pressure(uint8_t channel, uint8_t pressure) {
    MIDI_SendByte(CHANNEL_PRESSURE | (channel & 0b00001111));
    MIDI_SendByte((uint8_t) 0b01111111 & pressure);
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
MIDI_Reg midi_reg;
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
	switch (midiStatus & 0xF0) {
		case 0x80: // Note Off
			midi_reg.notes[midiData1] = 0;

			//looper.write_offs[looper.off_write_index] = timestamped_byte(midiData1, looper.tick);
			//looper.off_write_index++;

			break;

		case 0x90: // Note On
			midi_reg.notes[midiData1] = midiData2;

			//looper.write_ons[looper.on_write_index] = timestamped_byte(midiData1, looper.tick);
			//looper.on_write_index++;

			break;

		case 0xD0: // Channel Pressure
			midi_reg.pressure = midiData1;
			break;

		default:
			// Ignore unsupported messages for now
			break;
	}
}



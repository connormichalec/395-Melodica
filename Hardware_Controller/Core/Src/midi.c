#include "midi.h"

extern UART_HandleTypeDef huart2;

uint8_t midi_buffer;	// Buffer for incoming midi data

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

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) { // Ensure this is USART2's interrupt
        MIDI_ProcessByte(midi_buffer); // Your custom MIDI parsing logic

        // Restart UART reception to keep listening
        HAL_UART_Receive_IT(&huart2, &midi_buffer, 1);
    }
}

void MIDI_Init(void) {
    HAL_UART_Receive_IT(&huart2, &midi_buffer, 1); // Start receiving
}

MIDI_State midiState = MIDI_WAITING_FOR_STATUS;
uint8_t midiStatus = 0;
uint8_t midiData1 = 0;

void MIDI_ProcessByte(uint8_t byte) {
	if (byte & 0x80) {		// Status byte received
		midiStatus = byte;
		midiState = (midiStatus == 0xC0 || midiStatus == 0xD0)
				  ? MIDI_WAITING_FOR_DATA1  // Program Change/Channel Pressure (1 data byte)
				  : MIDI_WAITING_FOR_DATA2; // Most messages need 2 data bytes
	}

}

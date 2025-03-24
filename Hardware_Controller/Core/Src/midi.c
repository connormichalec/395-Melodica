#include "midi.h"

extern UART_HandleTypeDef huart2;

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

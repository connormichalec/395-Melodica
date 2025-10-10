#include "mux.h"
#include "midi.h"
#include "button.h"
#include "debounce_notes.h"

// Stop button handlers - Delete this code later when proper switches are installed
ToggleButton low = { GPIOB, GPIO_PIN_4, 0, 0, 0 };
ToggleButton mid = { GPIOB, GPIO_PIN_5, 0, 0, 1 };
ToggleButton high = { GPIOB, GPIO_PIN_6, 0, 0, 0 };
ToggleButton too_high = { GPIOB, GPIO_PIN_7, 0, 0, 0 };

void tick_buttons() {
	tick_button(&low);
	tick_button(&mid);
	tick_button(&high);
	tick_button(&too_high);
}

uint8_t ReadNote(uint8_t key) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, key & 0x01);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, (key & 0x02) >> 1);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, (key & 0x04) >> 2);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, (key & 0x08) >> 3);

    if (key & 0x20) return (uint8_t) HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15);
    if (key & 0x10) return (uint8_t) HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12);
    return (uint8_t) HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11);
}



void ReadKeyboard(uint8_t status[], uint8_t pressure_on) {
	debounce_notes_tick();
    uint8_t new_status;
	for (uint8_t i = 0; i < 37; i++) {
        //new_status = ReadNote(i) && pressure_on;
        new_status = ReadNote(i);
		if(status[i]!=new_status) {
			if(note_debouncer(i)) {
				if (!status[i] && new_status) {
					if (low.state) note_on(0, 29 + i, 60);
					if (mid.state) note_on(0, 41 + i, 60);
					if (high.state) note_on(0, 53 + i, 60);
					if (too_high.state) note_on(0, 65 + i, 60);
				}
				if (status[i] && !new_status) {
					if (low.state) note_off(0, 29 + i, 0);
					if (mid.state) note_off(0, 41 + i, 0);
					if (high.state) note_off(0, 53 + i, 0);
					if (too_high.state) note_off(0, 65 + i, 0);
				}
				status[i] = new_status;
			}
		}
    }
}


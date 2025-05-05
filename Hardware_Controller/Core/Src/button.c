/*
 * button.c
 *
 *  Created on: May 3, 2025
 *      Author: kobes
 */

#include "button.h"
#include "midi.h"
extern TIM_HandleTypeDef htim21;

void tick_button(ToggleButton * button) {
	// Track button state
	uint8_t new_button_state = HAL_GPIO_ReadPin(button->port, button->pin);
	uint8_t button_edge = (!button->button_pressed && new_button_state); // rising edge

	button->button_pressed = new_button_state; // Update this BEFORE using it to avoid race logic
	if (button->debounce_tick > 0) button->debounce_tick--;

	if (button_edge && button->debounce_tick == 0) {	// Detect positive edge of button presses
		button->debounce_tick = 10;
		button->state = !button->state;
	}

}

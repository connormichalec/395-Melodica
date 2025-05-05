/*
 * button.h
 *
 *  Created on: May 3, 2025
 *      Author: kobes
 */

#include <stdint.h>
#include "stm32l0xx_hal.h"

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

typedef struct {
	GPIO_TypeDef * port;
	uint16_t pin;
	uint8_t button_pressed;		// Tracks state of whether the button is pressed (for posedge detection)
	uint8_t debounce_tick;
	uint8_t state;
} ToggleButton;

void tick_button(ToggleButton * button);


#endif /* INC_BUTTON_H_ */

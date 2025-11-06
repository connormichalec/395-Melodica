/*
 * IODevices.c
 *
 *  Created on: Nov 6, 2025
 *      Author: connor15
 */


#include "IODevices.h"
#include "stm32l0xx_hal.h"

void testIORegister() {
	io_abs i;

	i.device_id = 420;
	i.param_id = 67;
	i.poll_function = &testIOPollFunction;
	i.get_value = &testIOGetValue;
	i.state_value = 0;

	registerAbsIO(i);
}

// for a 3 way switch
int testIOPollFunction(io_abs* io) {
	// Check GPIOs
	int pb1 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
	int pb0 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);


	// states:
	if(pb1 && io->state_value!=1) {
		io->state_value = 1;
		return(1);
	}
	else if (pb0 && io->state_value != 2) {
		io->state_value = 2;
		return(1);
	}
	else if(!pb0 && !pb1 && io->state_value != 0){
		io->state_value = 0;
		return(1);
	}

	// no change
	return(0);
}

unsigned int testIOGetValue(io_abs* io) {
	// Test output to pa7:
	if(io->state_value == 0) {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, 1);
	} else {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, 0);
	}

	return io->state_value;
}



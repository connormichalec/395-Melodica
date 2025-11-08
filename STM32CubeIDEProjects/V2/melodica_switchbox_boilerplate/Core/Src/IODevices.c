/*
 * IODevices.c
 *
 *  Created on: Nov 6, 2025
 *      Author: connor15
 */


#include "IODevices.h"
#include <stdlib.h>

void registerAll3PDTToggleSwitches() {
	// call all of the individual switch register functions:
	void ToggleSwitch3PDTRegister1();
	void ToggleSwitch3PDTRegister2();
	void ToggleSwitch3PDTRegister3();
}

// each one affects different parameter id but functions the same
void ToggleSwitch3PDTRegister1() {
	io_abs i;

	i.device_id = 10;
	i.param_id = 1;
	i.poll_function = &ToggleSwitch3PDTPollFunction;
	i.get_value = &ToggleSwitch3PDTPollGetValue;
	i.state_value = 0;

	struct ToggleSwitch3PDTMisc* m = (struct ToggleSwitch3PDTMisc*) malloc(sizeof(struct ToggleSwitch3PDTMisc));
	m->GPIO1 = GPIOA;
	m->GPIOPin1 = GPIO_PIN_15;
	m->GPIO2 = GPIOB;
	m->GPIOPin2 = GPIO_PIN_3;

	i.misc = m;
	i.destructFunction = &destruct3PDTMiscStrucutre;

	registerAbsIO(i);
}
void ToggleSwitch3PDTRegister2() {
	io_abs i;

	i.device_id = 10;
	i.param_id = 2;
	i.poll_function = &ToggleSwitch3PDTPollFunction;
	i.get_value = &ToggleSwitch3PDTPollGetValue;
	i.state_value = 0;

	struct ToggleSwitch3PDTMisc* m = (struct ToggleSwitch3PDTMisc*) malloc(sizeof(struct ToggleSwitch3PDTMisc));
	m->GPIO1 = GPIOB;
	m->GPIOPin1 = GPIO_PIN_4;
	m->GPIO2 = GPIOB;
	m->GPIOPin2 = GPIO_PIN_5;

	i.misc = m;
	i.destructFunction = &destruct3PDTMiscStrucutre;

	registerAbsIO(i);
}
void ToggleSwitch3PDTRegister3() {
	io_abs i;

	i.device_id = 10;
	i.param_id = 3;
	i.poll_function = &ToggleSwitch3PDTPollFunction;
	i.get_value = &ToggleSwitch3PDTPollGetValue;
	i.state_value = 0;

	struct ToggleSwitch3PDTMisc* m = (struct ToggleSwitch3PDTMisc*) malloc(sizeof(struct ToggleSwitch3PDTMisc));
	m->GPIO1 = GPIOB;
	m->GPIOPin1 = GPIO_PIN_6;
	m->GPIO2 = GPIOB;
	m->GPIOPin2 = GPIO_PIN_7;

	i.misc = m;
	i.destructFunction = &destruct3PDTMiscStrucutre;

	registerAbsIO(i);
}


// for a 3 way switch
int ToggleSwitch3PDTPollFunction(io_abs* io) {

	struct ToggleSwitch3PDTMisc* misc = (struct ToggleSwitch3PDTMisc*) io->misc;

	// Check GPIOs
	int pin1 = HAL_GPIO_ReadPin(misc->GPIO1, misc->GPIOPin1);
	int pin2 = HAL_GPIO_ReadPin(misc->GPIO2, misc->GPIOPin2);

	// states:
	if(pin1 && io->state_value!=1) {
		io->state_value = 1;
		return(1);
	}
	else if (pin2 && io->state_value != 2) {
		io->state_value = 2;
		return(1);
	}
	else if(!pin2 && !pin1 && io->state_value != 0){
		io->state_value = 0;
		return(1);
	}

	// no change
	return(0);
}

unsigned int ToggleSwitch3PDTPollGetValue(io_abs* io) {
	// Test output to pa7:
	if(io->state_value == 0) {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 1);
	} else {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0);
	}

	return io->state_value;
}

void destruct3PDTMiscStrucutres(io_abs* io) {
	// TODO: Implement deallocation of misc structs
}



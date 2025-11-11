/*
 * IODevices.c
 *
 *  Created on: Nov 6, 2025
 *      Author: connor15
 */


#include "IODevices.h"
#include <stdlib.h>
#include <stm32l0xx_hal_adc.h>



/*           3 WAY SWITCHES            */
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
	i.get_value = &ToggleSwitch3PDTGetValue;
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
	i.get_value = &ToggleSwitch3PDTGetValue;
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
	i.get_value = &ToggleSwitch3PDTGetValue;
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

unsigned int ToggleSwitch3PDTGetValue(io_abs* io) {
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

//////////////////////////////////////







/* 						POTENTIOMETERS					*/

ADC_HandleTypeDef* hadc_;

void registerAllPots(ADC_HandleTypeDef* hadc) {
	hadc_ = hadc;
	pot1Register();
	pot2Register();
	pot3Register();
	pot4Register();
	pot5Register();
}

void pot1Register() {
	io_abs i;

	i.device_id = 10;
	i.param_id = 4;
	i.poll_function = &potPollFunction;
	i.get_value = &potGetValue;
	i.state_value = 0;

	struct PotMisc* m = (struct PotMisc*) malloc(sizeof(struct PotMisc));
	m->ADCIdx = 9;

	i.misc = m;
	i.destructFunction = &destructPotMiscStructure;

	registerAbsIO(i);
}
void pot2Register() {
	io_abs i;

	i.device_id = 10;
	i.param_id = 5;
	i.poll_function = &potPollFunction;
	i.get_value = &potGetValue;
	i.state_value = 0;

	struct PotMisc* m = (struct PotMisc*) malloc(sizeof(struct PotMisc));
	m->ADCIdx = 8;

	i.misc = m;
	i.destructFunction = &destructPotMiscStructure;

	registerAbsIO(i);
}
void pot3Register() {
	io_abs i;

	i.device_id = 10;
	i.param_id = 6;
	i.poll_function = &potPollFunction;
	i.get_value = &potGetValue;
	i.state_value = 0;

	struct PotMisc* m = (struct PotMisc*) malloc(sizeof(struct PotMisc));
	m->ADCIdx = 7;

	i.misc = m;
	i.destructFunction = &destructPotMiscStructure;

	registerAbsIO(i);
}
void pot4Register() {
	io_abs i;

	i.device_id = 10;
	i.param_id = 7;
	i.poll_function = &potPollFunction;
	i.get_value = &potGetValue;
	i.state_value = 0;

	struct PotMisc* m = (struct PotMisc*) malloc(sizeof(struct PotMisc));
	m->ADCIdx = 6;

	i.misc = m;
	i.destructFunction = &destructPotMiscStructure;

	registerAbsIO(i);
}
void pot5Register() {
	io_abs i;

	i.device_id = 10;
	i.param_id = 8;
	i.poll_function = &potPollFunction;
	i.get_value = &potGetValue;
	i.state_value = 0;

	struct PotMisc* m = (struct PotMisc*) malloc(sizeof(struct PotMisc));
	m->ADCIdx = 5;

	i.misc = m;
	i.destructFunction = &destructPotMiscStructure;

	registerAbsIO(i);
}

int potPollFunction(io_abs* io) {
	// For this implementation we are fetching the last rad DAC value (not using DMA - although that would be more efficient TODO?)
	// Continous conversion and scan conversion needs to be on for ADC to always be sampling inputs
	// This is not a very efficient way of doing this since we scan all channels for each pot, TODO: Have one poll function for the first pot that stores global data that can be accessed by other poll functions

	struct PotMisc* m = (struct PotMisc*) io->misc;

	uint32_t val;
	for (uint8_t i = 0; i <= m->ADCIdx; i++) {
	    HAL_ADC_PollForConversion(hadc_, HAL_MAX_DELAY);			// TODO: Get a timeout in there
	    val = HAL_ADC_GetValue(hadc_);
	}

	// check if this value is much different than the stored one:
	if(val != io->state_value) {
		// if it is call an update:

		io->state_value = val;
		return 1;
	}


	// otherwise nothing:
	return 0;

}

unsigned potGetValue(io_abs* io) {
	return io->state_value;
}

void destructPotMiscStructure(io_abs* io) {

}


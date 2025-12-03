/*
 * IODevices.c
 *
 *  Created on: Nov 6, 2025
 *      Author: connor15
 */


#include "IODevices.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>


/*           3 WAY SWITCHES            */
void registerAll3PDTToggleSwitches() {
	// call all of the individual switch register functions:
	ToggleSwitch3PDTRegister1();
	ToggleSwitch3PDTRegister2();
	ToggleSwitch3PDTRegister3();
}

// each one affects different parameter id but functions the same
void ToggleSwitch3PDTRegister1() {
	io_abs i;

	i.target_device_id = SYNTH_DEVICE_ID;
	i.param_id = PARAMETER_ID_OSC_TYPE;
	i.poll_function = &ToggleSwitch3PDTPollFunction;
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

	i.target_device_id = 10;
	i.param_id = 2;
	i.poll_function = &ToggleSwitch3PDTPollFunction;
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

	i.target_device_id = 10;
	i.param_id = 3;
	i.poll_function = &ToggleSwitch3PDTPollFunction;
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
	if(pin1 && pin2 && io->state_value != 1) {
		io->state_value = 1;

		return(1);
	}
	else if (pin2 && !pin1 && io->state_value != 2) {
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


void destruct3PDTMiscStrucutre(io_abs* io) {
	// TODO: Implement deallocation of misc structs
}

//////////////////////////////////////







/* 						POTENTIOMETERS					*/


#define NUM_ADC_CHANNELS 5 					// Make sure this matches ADC scan config
uint16_t g_adc_values[NUM_ADC_CHANNELS];

ADC_HandleTypeDef* hadc_;

void registerAllPots(ADC_HandleTypeDef* hadc) {
	hadc_ = hadc;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, 0);

	  // Make sure NUM_ADC_CHANNELS matches ADC's Nb of Conversion
	if (HAL_ADC_Start_DMA(hadc_, (uint32_t*)g_adc_values, NUM_ADC_CHANNELS) != HAL_OK) {
	        // Handle Error
	}


	pot1Register();
	//pot2Register();
	//pot3Register();
	//pot4Register();
	//pot5Register();
}

void pot1Register() {
	io_abs i;

	i.target_device_id = SYNTH_DEVICE_ID;
	i.param_id = PARAMTER_ID_DETUNE;
	i.poll_function = &potPollFunction;
	i.state_value = 0;

	struct PotMisc* m = (struct PotMisc*) malloc(sizeof(struct PotMisc));
	m->ADCNumber = 4;

	i.misc = m;
	i.destructFunction = &destructPotMiscStructure;

	registerAbsIO(i);
}
void pot2Register() {
	io_abs i;

	i.target_device_id = SYNTH_DEVICE_ID;
	i.param_id = PARAMTER_ID_DETUNE;
	i.poll_function = &potPollFunction;
	i.state_value = 0;

	struct PotMisc* m = (struct PotMisc*) malloc(sizeof(struct PotMisc));
	m->ADCNumber = 3;

	i.misc = m;
	i.destructFunction = &destructPotMiscStructure;

	registerAbsIO(i);
}
void pot3Register() {
	io_abs i;

	i.target_device_id = SYNTH_DEVICE_ID;
	i.param_id = PARAMTER_ID_DETUNE;
	i.poll_function = &potPollFunction;
	i.state_value = 0;

	struct PotMisc* m = (struct PotMisc*) malloc(sizeof(struct PotMisc));
	m->ADCNumber = 2;

	i.misc = m;
	i.destructFunction = &destructPotMiscStructure;

	registerAbsIO(i);
}
void pot4Register() {
	io_abs i;

	i.target_device_id = SYNTH_DEVICE_ID;
	i.param_id = PARAMTER_ID_DETUNE;
	i.poll_function = &potPollFunction;
	i.state_value = 0;

	struct PotMisc* m = (struct PotMisc*) malloc(sizeof(struct PotMisc));
	m->ADCNumber = 1;

	i.misc = m;
	i.destructFunction = &destructPotMiscStructure;

	registerAbsIO(i);
}
void pot5Register() {
	io_abs i;

	i.target_device_id = SYNTH_DEVICE_ID;
	i.param_id = PARAMTER_ID_DETUNE;
	i.poll_function = &potPollFunction;
	i.state_value = 0;

	struct PotMisc* m = (struct PotMisc*) malloc(sizeof(struct PotMisc));
	m->ADCNumber = 0;

	i.misc = m;
	i.destructFunction = &destructPotMiscStructure;

	registerAbsIO(i);
}

#define ADC_RESOLUTION 4095.0f			// Maximum read value from ADC (will be normalized to a "1"
#define POT_DEVIATION 0.05f

int potPollFunction(io_abs* io) {
	// For this implementation we are fetching the last rad DAC value (not using DMA - although that would be more efficient TODO?)
	// Continous conversion and scan conversion needs to be on for ADC to always be sampling inputs
	// This is not a very efficient way of doing this since we scan all channels for each pot, TODO: Have one poll function for the first pot that stores global data that can be accessed by other poll functions

	struct PotMisc* m = (struct PotMisc*) io->misc;

	uint16_t val_raw;			// raw adc read value
	float val_float;			// adc value converted to a float

	// Get the value
	val_raw = g_adc_values[m->ADCNumber];


	// Convert to normalized float
	val_float = ((float) val_raw / ADC_RESOLUTION);

	// get current value as a float for comparison:
	float val_current_float;
	memcpy(&val_current_float, &io->state_value, sizeof(val_current_float));


	// check if this value is much different than the stored one:
	if (fabsf(val_float - val_current_float) > POT_DEVIATION) {
		// if it is call an update:
		//HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);

		memcpy(&io->state_value, &val_float, sizeof(val_float));

		return 1;
	}

	// otherwise nothing:
	return 0;

}

void destructPotMiscStructure(io_abs* io) {

}


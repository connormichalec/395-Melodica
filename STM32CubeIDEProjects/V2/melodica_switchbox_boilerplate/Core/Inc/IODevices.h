/*
 * IODevices.h
 *
 *  Created on: Nov 6, 2025
 *      Author: connor15
 */

#ifndef INC_IODEVICES_H_
#define INC_IODEVICES_H_

#include "IOManager.h"
#include "stm32l0xx_hal.h"




/* 3 WAY SWITHCES:*/

// register all 3 way switches:
void registerAll3PDTToggleSwitches();

// register function for each 3 way switch:
void ToggleSwitch3PDTRegister1();
void ToggleSwitch3PDTRegister2();
void ToggleSwitch3PDTRegister3();

// Misc structure for all 3 way pdt switches, each switch should be configured with different gpio pins:
struct ToggleSwitch3PDTMisc {

	// Gpio pin letter and pin numbers for the second 2 positions of the three
	GPIO_TypeDef* GPIO1;
	uint16_t GPIOPin1;

	GPIO_TypeDef* GPIO2;
	uint16_t GPIOPin2;
};

// Used for all 3 pdt 3 way switches (instead of seperate funciton for each one:)
int ToggleSwitch3PDTPollFunction(io_abs* io);
unsigned int ToggleSwitch3PDTGetValue(io_abs* io);
void destruct3PDTMiscStrucutre(io_abs* io);

///////////////////








/*   10k POTS!   */
void registerAllPots(ADC_HandleTypeDef* hadc);

// register function for each specific pot:
void pot1Register();
void pot2Register();
void pot3Register();
void pot4Register();
void pot5Register();

// Misc structure for all pots, keep track of adc pin input
struct PotMisc {

	uint16_t ADCIdx;
};

int potPollFunction(io_abs* io);
unsigned potGetValue(io_abs* io);
void destructPotMiscStructure(io_abs* io);

////////////////////////////


#endif /* INC_IODEVICES_H_ */

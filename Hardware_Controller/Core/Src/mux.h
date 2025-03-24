/*
 * mux.h
 *
 *  Created on: Feb 24, 2025
 *      Author: kobes
 */

#pragma once
#include <stdint.h>
#include "stm32l0xx_hal.h"

void SetSelectBits(uint8_t key);
void ReadKeyboard(uint8_t status[]);

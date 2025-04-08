/*
 * looper.c
 *
 *  Created on: Apr 8, 2025
 *      Author: kobes
 */

#include "looper.h"

extern TIM_HandleTypeDef htim21;

uint16_t loop_tick = 0;
uint32_t loop_ons[512];
uint32_t loop_offs[512];
uint16_t timer;

void LOOPER_INIT() {
	for (int i = 0; i < 512; i++) {
		loop_ons[512] = 0;
		loop_offs[512] = 0;
	}
}

void test() {
	if (__HAL_TIM_GET_FLAG(&htim2, TIM_FLAG_UPDATE)) {
		__HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE);
		loop_tick++;
	}


}

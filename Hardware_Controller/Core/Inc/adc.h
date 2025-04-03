/*
 * adc.h
 *
 *  Created on: Mar 28, 2025
 *      Author: Kobe Duda
 */

#pragma once
#include <stdint.h>

// Applies a smoothing function to input data
//	- Alpha is smoothing factor. Scaled up by 1000.
uint16_t exponential_filter(uint16_t current, uint16_t previous, uint16_t alpha);

// Maps values into the 7-bit input range
uint8_t remap(uint16_t value, uint16_t threshold, uint16_t scale_mult, uint16_t scale_div);

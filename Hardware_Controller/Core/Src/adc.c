/*
 * adc.c
 *
 *  Created on: Mar 28, 2025
 *      Author: Kobe Duda
 */

#include "adc.h"

uint16_t exponential_filter(uint16_t current, uint16_t previous, uint16_t alpha) {
	return (alpha * current + (1000 - alpha) * previous) / 1000;
}

uint8_t remap(uint16_t value, uint16_t threshold, uint16_t scale_mult, uint16_t scale_div) {
	// Shift values to require being above a certain threshold
	// Apply scaling factor

	value = value < threshold ? 0 : value - threshold;
	value *= scale_mult;
	value /= scale_div;
	if (value > 127) value = 127;
	return (uint8_t) value;
}

/*
 * oscillator.c
 *
 *  Created on: Apr 4, 2025
 *      Author: Connor Michalec
 */

#include <math.h>

float sin_oscillator(float phase) {
	return(sinf(2 * M_PI * phase));
}

float saw_oscillator(float phase) {
	return(phase);
}

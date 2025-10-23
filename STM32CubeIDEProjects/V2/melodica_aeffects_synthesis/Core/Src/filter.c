/*
 * filter.c
 *
 *  Created on: Apr 29, 2025
 *      Author: Connor Michalec
 */


#include "filter.h"
#include <stdlib.h>


Filter * create_filter(float cutoff, float resonance, FilterType filter) {
	Filter * f = malloc(sizeof(Filter));

	f->cutoff = cutoff;
	f->resonance = resonance;
	f->next = NULL;
	f->type = filter;

	switch(filter) {
		case LOWPASS:
			f->filterFunciton = &lpf_process;
			lpf_init(f);
			break;
		default:
			break;
	}

	return f;
}

void delete_filter(Filter * filter) {
	free(filter);
}

/* FROM CHATGPT(mostly):*/
void lpf_init(Filter *filt) {
    filt->buf0 = 0.0f;
    filt->feedback = filt->resonance + filt->resonance / (1.0f - filt->cutoff + 1e-6f);
}

float lpf_process(Filter *filt, float input) {
	if (filt->resonance <= 0.0f) {
	    filt->buf0 += filt->cutoff * (input - filt->buf0);
	} else {
	    filt->buf0 += filt->cutoff * (input - filt->buf0 + filt->feedback * (filt->buf0 - input));
	}
    // Simple resonant 1-pole filter
    return filt->buf0;
}


void lpf_set_cutoff(Filter *filt, float cutoff) {
    // Clamp and recalculate feedback
	if (cutoff < 0.0f) cutoff = 0.0f;
	if (cutoff > 0.99f) cutoff = 0.99f;
    filt->cutoff = cutoff;
    //filt->feedback = filt->resonance + filt->resonance / (1.0f - cutoff + 1e-6f);
    filt->feedback = filt->resonance * (1.0f - 0.15f * cutoff * cutoff); // safer
}

void lpf_set_resonance(Filter *filt, float resonance) {
	if (resonance < 0.0f)  resonance = 0.0f;
	if (resonance > 4.0f)  resonance = 4.0f;  // 0–4 is typical

    // Update resonance and feedback
    filt->resonance = resonance;
    //filt->feedback = resonance + resonance / (1.0f - filt->cutoff + 1e-6f);
    filt->feedback = resonance * (1.0f - 0.15f * filt->cutoff * filt->cutoff); // safer
}
/*   */

void set_filter_cutoff(Filter * filter, float cutoff) {
	switch(filter->type) {
		case LOWPASS:
			lpf_set_cutoff(filter, cutoff);
			break;
		default:
			break;
	}
}

void set_filter_resonance(Filter * filter, float resonance) {
	switch(filter->type) {
		case LOWPASS:
			lpf_set_resonance(filter, resonance);
			break;
		default:
			break;
	}
}

/*
 * filter.h
 *
 *  Created on: Apr 29, 2025
 *      Author: Connor Michalec
 */

#ifndef INC_FILTER_H_
#define INC_FILTER_H_

typedef enum {
	LOWPASS
} FilterType;

// Filters are added via linked list to a voice
typedef struct Filter {
	float cutoff;
	float resonance;
	float buf0;
	float feedback;
	FilterType type;
	float (*filterFunciton) (struct Filter * filt,float input);
	struct Filter * next;				// Next in linked list
} Filter;



void lpf_init(Filter *filt);
float lpf_process(Filter * filt, float input);
void lpf_set_cutoff(Filter *filt, float cutoff);
void lpf_set_resonance(Filter *filt, float resonance);


/**
 * Set a given filters cutoff
 */
void set_filter_cutoff(Filter * filter, float cutoff);

/**
 * Set a given filters resonance
 */
void set_filter_resonance(Filter * filter, float resonance);

/**
 * Delete specific filter
 */
void delete_Filter(Filter * f);

Filter * create_filter(float cutoff, float resonance, FilterType type);

/**
 * Clean up filter
 */
void delete_filter(Filter * filter);

#endif /* INC_FILTER_H_ */

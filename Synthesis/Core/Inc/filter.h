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
} filterTypes;

// Filters are added via linked list to a voice
typedef struct Filter {

	struct Filter * next;				// Next in linked list
} Filter;




#endif /* INC_FILTER_H_ */

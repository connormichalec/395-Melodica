/*
 * IOManager.h
 *
 *  Created on: Nov 4, 2025
 *      Author: connor15
 */

#ifndef INC_IOMANAGER_H_
#define INC_IOMANAGER_H_

#include "stm32l0xx_hal.h"

#define MAX_ABS_IO 100
#define MAX_REL_IO 100


// linked list of absolute io (has a current state that will update accordingly)
typedef struct io_abs {
	int enabled;											// whether or not this io is being used
	int sentInitial;										// has the first update been sent yet (when device is powered on)
	uint8_t target_device_id;
	uint8_t param_id;											// what parameter id is this io being tracked to?
	uint32_t state_value;									// value of state (fixed as uint32_t cast from float if needed)
	// callback functions for update:
	int (*poll_function) (struct io_abs*);					// poll function should 1 return if it did change enough, iomanager will send message if changed
	void* misc;												// misc data type pointer, store any additional information relevant to the io device (cast to necessary type)
	void* destructFunction;									// to be called on destruction, for deallocating misc if needed. io_abs pointer is automatically passed to this
} io_abs;


// relative io (just send updates, doesnt have a state)
typedef struct io_rel {
} io_rel;


// keeps track of state of all io:
typedef struct IOstate {
	// List of all both io
	io_abs abs_io[MAX_ABS_IO];
	io_rel rel_io[MAX_REL_IO];
} IOstate;

// To be ticked by main
void pollInputs();

// Initialize data structures
void IOinit();

// Register a new absolute IO device: returns 1 on failure 0 on success
int registerAbsIO(io_abs io_device);



#endif /* INC_IOMANAGER_H_ */

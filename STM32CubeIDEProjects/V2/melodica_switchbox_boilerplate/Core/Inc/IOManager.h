/*
 * IOManager.h
 *
 *  Created on: Nov 4, 2025
 *      Author: connor15
 */

#ifndef INC_IOMANAGER_H_
#define INC_IOMANAGER_H_

#define MAX_ABS_IO 100
#define MAX_REL_IO 100


// linked list of absolute io (has a current state that will update accordingly)
typedef struct io_abs {
	int enabled;											// whether or not this io is being used
	int device_id;											// (should be the same for all swithces here, in this struct for flexibility purposes)
	int param_id;											// what parameter id is this io being tracked to?
	unsigned int state_value;								// value of state
	// callback functions for update:
	int (*poll_function) (struct io_abs*);					// poll function should 1 return if it did change enough, iomanager will send message if changed
	unsigned int (*get_value) (struct io_abs*);				// get message value to send
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

// Send all of the values of the current state, regardless if they have changed or not
// Upon connecting to the devices, this should be called to send everything as it is right now
void initialUpdate();

// Initialize data structures
void IOinit();

// Register a new absolute IO device: returns 1 on failure 0 on success
int registerAbsIO(io_abs io_device);


#endif /* INC_IOMANAGER_H_ */

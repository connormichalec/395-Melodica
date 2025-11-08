/*
 * IOmanager.c
 *
 *  Created on: Nov 4, 2025
 *      Author: connor15
 */

#include "IOManager.h"
#include <stdlib.h>


// Global io state:
IOstate* state;

void IOinit() {
	// TODO: Implement a destructor for this and deconstruct this malloc along with calling each destructFunction for the data types.
	state = (IOstate*) malloc(sizeof(IOstate));

	// initialize all abs to default:
	for(int i = 0; i<MAX_ABS_IO; i++) {
		state->abs_io[i].device_id = 0;
		state->abs_io[i].poll_function = NULL;
		state->abs_io[i].enabled = 0;
		state->abs_io[i].get_value = NULL;
		state->abs_io[i].param_id = 0;
		state->abs_io[i].state_value = 0;
		state->abs_io[i].misc = 0x0;
		state->abs_io[i].destructFunction = 0x0;
	}

	// TODO: implement io_rel for things like encoders
}

int registerAbsIO(io_abs io_device) {
	// register a new absolute io device

	for(int i = 0; i<MAX_ABS_IO; i++) {
		if(state->abs_io[i].enabled == 0) {
			// First non-enabled io device, register here

			state->abs_io[i].device_id = io_device.device_id;
			state->abs_io[i].enabled = 1;
			state->abs_io[i].get_value = io_device.get_value;
			state->abs_io[i].param_id = io_device.param_id;
			state->abs_io[i].poll_function = io_device.poll_function;
			state->abs_io[i].state_value = state->abs_io[i].state_value;
			state->abs_io[i].misc = state->abs_io[i].misc;
			state->abs_io[i].destructFunction = state->abs_io[i].destructFunction;

			return 0;
		}
	}


	// failure
	return 1;

}

// Send all values of the state as they are, not just the ones that have changed
void initialUpdate() {
	for(int i = 0; i<MAX_ABS_IO; i++) {
		if(state->abs_io[i].enabled) {
			// poll IO
			state->abs_io[i].poll_function(&state->abs_io[i]);

			// send the current value of everything for initializaiton
			unsigned int val = state->abs_io[i].get_value(&state->abs_io[i]);

			// send this data over protocol:

		}
	}
}


// Poll for changes and send updates for those
void pollInputs() {
	for(int i = 0; i<MAX_ABS_IO; i++) {
		if(state->abs_io[i].enabled) {
			// poll and check if data has changed and needs to be sent
			if(state->abs_io[i].poll_function(&state->abs_io[i])) {
				// should update! poll got new stuff to send
				unsigned int newVal = state->abs_io[i].get_value(&state->abs_io[i]);

				// send this new data over protocol:

			}
		}
	}
}


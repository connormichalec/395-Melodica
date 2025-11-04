/*
 * IOmanager.c
 *
 *  Created on: Nov 4, 2025
 *      Author: connor15
 */

#include IOManager.h


// Global io state:
IOstate* state;

void IOinit() {
	// TODO: deconstruct this malloc.
	state = (IOstate*) malloc(sizeof(IOstate));

	// initialize all abs:
	for(int i = 0; i<MAX_IO_ABS; i++) {
		state->abs_io[i].device_id = 0;
		state->abs_io[i].poll_function = NULL;
		state->abs_io[i].enabled = 0;
		state->abs_io[i].get_value = NULL;
		state->abs_io[i].param_id = 0;
		state->abs_io[i].state_value = 0;
	}
}


void pollInputs() {
	for(int i = 0; i<MAX_IO_ABS; i++) {
		if(state->abs_io[i].enabled) {
			if(state->abs_io[i].poll_function(abs_io[i])) {
				// should update! poll got new stuff to send
				int newVal = state->abs_io.get_value(abs_io[i]);
			}
		}
	}
}


/*
 * IOmanager.c
 *
 *  Created on: Nov 4, 2025
 *      Author: connor15
 */

#include "IOManager.h"
#include "protocol.h"	// for Control types
#include <stdlib.h>


// Global io state:
IOstate* state;

void IOinit() {
	// TODO: Implement a destructor for this and deconstruct this malloc along with calling each destructFunction for the data types.
	state = (IOstate*) malloc(sizeof(IOstate));

	// initialize all abs to default:
	for(int i = 0; i<MAX_ABS_IO; i++) {
		state->abs_io[i].target_device_id = 0;
		state->abs_io[i].poll_function = NULL;
		state->abs_io[i].enabled = 0;
		state->abs_io[i].sentInitial = 0;
		state->abs_io[i].state_value = 0;
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

			state->abs_io[i].target_device_id = io_device.target_device_id;
			state->abs_io[i].enabled = 1;
			state->abs_io[i].param_id = io_device.param_id;
			state->abs_io[i].poll_function = io_device.poll_function;
			state->abs_io[i].state_value = io_device.state_value;
			state->abs_io[i].misc = io_device.misc;
			state->abs_io[i].destructFunction = io_device.destructFunction;
			state->abs_io[i].sentInitial = 0;

			return 0;
		}
	}


	// failure
	return 1;

}


// Poll for changes and send updates for those
void pollInputs() {

	for(int i = 0; i<MAX_ABS_IO; i++) {

		if(state->abs_io[i].enabled) {

			// poll and check if data has changed and needs to be sent
			if(state->abs_io[i].poll_function(&state->abs_io[i])) {
				// should update! poll got new stuff to send
				uint32_t val = state->abs_io[i].state_value;

				// send this new data over protocol:

				send_parameter_update_toNext(state->abs_io[i].param_id, state->abs_io[i].target_device_id, CONTROL_ABSOLUTE, val);

				return;			// ONLY ONE PARAMETER AT A TIME!! Important since receiver cant handle multiple in a row (as designed currently) - TODO: Cleaner solution -- poll rate must not be too frequent
			}
		}
	}
}



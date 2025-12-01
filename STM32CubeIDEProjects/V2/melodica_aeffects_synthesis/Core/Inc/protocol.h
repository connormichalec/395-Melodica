/*
 * protocol.h
 *
 * For communications with other devices/switchboxes
 *
 *  Created on: Nov 19, 2025
 *      Author: Connor Michalec
 */

#ifndef INC_PROTOCOL_H_
#define INC_PROTOCOL_H_

#include "stm32h7xx_hal.h"

#define CONTROL_ABSOLUTE 0
#define CONTROL_RELATIVE 1
#define CONTROL_CONNECTIVITY 2

typedef enum {
	ABSOLUTE,
	RELATIVE
} ControlType;


// NOTE: The actual message data follows this message struct in memory, so to access it add the sizeof this struct to its address to get to start of data
// Message
typedef struct __attribute__((packed)) Msg {
	uint8_t device_ID;			// device ID of message sender
	uint8_t target_device_ID;	// message destination device ID (if applicable)
	uint8_t control_type;		// 0: Absolute, 1: Relative, 2: Connectivity message
	uint16_t parameter_ID;  	// If this is a connectivity message, index is encoded into this field
	uint16_t data_length;		// data length (to be appended after this message!)
} Msg;

void ProtocolInit();

/**
 * Process data coming in from either the last switchbox or a previous device module (depending whether or not the synth is first in line after switchboxes)
 * pass this data down to next device
 */
void Prev_ProcessByte();

/**
 * Process data coming in from the next device in the chain,
 * to pass down backwards up the chain to switchboxes
 */
void Next_ProcessByte();


#endif /* INC_PROTOCOL_H_ */

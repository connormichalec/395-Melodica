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

typedef enum {
	ABSOLUTE,
	RELATIVE
} ControlType;

// Message
typedef struct __attribute__((packed)) Msg {
	uint8_t device_ID;
	ControlType control_type;
	uint16_t parameter_ID;
	uint16_t data_length;
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

/*
 * protocol.h
 *
 *  Created on: Oct 13, 2025
 *      Author: kobes
 */

#include "stm32g4xx_hal.h"

#ifndef INC_PROTOCOL_H_
#define INC_PROTOCOL_H_

extern uint8_t connectivity_msg[16];
extern uint8_t prev_msg_byte;
extern uint8_t next_msg_byte;
extern uint32_t last_rx_timestamp;

#define CONTROL_ABSOLUTE 0
#define CONTROL_RELATIVE 1
#define CONTROL_CONNECTIVITY 2

// Switchbox message
typedef struct __attribute__((packed)) SwitchboxMsg {
	uint8_t device_ID;
	uint8_t control_type;	// 0: Absolute, 1: Relative, 2: Connectivity message
	uint16_t parameter_ID;  // If this is a connectivity message, index is encoded into this field
	uint16_t data_length;
} SwitchboxMsg;

void Switchbox_Init();
void Prev_ProcessByte();
void Next_ProcessByte();

#endif /* INC_PROTOCOL_H_ */

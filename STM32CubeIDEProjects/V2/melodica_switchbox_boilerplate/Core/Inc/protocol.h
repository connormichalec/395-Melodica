/*
 * protocol.h
 *
 *  Created on: Oct 13, 2025
 *      Author: kobes
 */

#include "stm32l0xx_hal.h"

#ifndef INC_PROTOCOL_H_
#define INC_PROTOCOL_H_

extern uint8_t connectivity_msg[16];
extern uint32_t last_rx_timestamp;


void Switchbox_Init();
void Prev_ProcessByte();
void Next_ProcessByte();

#endif /* INC_PROTOCOL_H_ */

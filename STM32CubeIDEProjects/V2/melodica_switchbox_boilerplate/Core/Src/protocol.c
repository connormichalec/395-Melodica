/*
 * protocol.c
 *
 *  Created on: Oct 13, 2025
 *      Author: kobes
 */

#include "protocol.h"
#include "config.h"
#include "modules.h"
#include <string.h>

extern UART_HandleTypeDef huart2;	// Next device
extern UART_HandleTypeDef hlpuart1; // Previous device

uint8_t prev_msg_byte;
uint8_t prev_msg_buf[512];
uint8_t prev_msg_idx = 0;
uint8_t next_msg_byte;
uint8_t next_msg_buf[512];
uint8_t next_msg_idx;
uint32_t last_rx_timestamp;

uint8_t connectivity_msg[16];

void Switchbox_Init() {
	HAL_UART_Receive_IT(&hlpuart1, &prev_msg_byte, 1);
	HAL_UART_Receive_IT(&huart2, &next_msg_byte, 1);
	last_rx_timestamp = 0;

	// Set up default connectivity message
	SwitchboxMsg cmsg;

	cmsg.control_type = ABSOLUTE;
	cmsg.data_length = 0;
	cmsg.device_ID = DEVICE_ID;
	cmsg.parameter_ID = 0;

	memcpy(connectivity_msg, &cmsg, sizeof(SwitchboxMsg));
}

void Prev_ProcessByte() {

	// Reset system if timeout
	uint32_t timestamp = HAL_GetTick();

	if (timestamp - last_rx_timestamp >= 10) {		// 10ms timeout
		prev_msg_idx = 0;
	}

	last_rx_timestamp = timestamp;


	// Store into buffer
	prev_msg_buf[prev_msg_idx] = prev_msg_byte;
	prev_msg_idx++;



	if (prev_msg_idx < sizeof(SwitchboxMsg)) {
		// Receive the rest of the message
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
		HAL_UART_Receive_IT(&hlpuart1, &prev_msg_byte, 1);
		return;
	}


	SwitchboxMsg sb_msg;
	memcpy(&sb_msg, prev_msg_buf, sizeof(SwitchboxMsg));

	// Check if this is a connectivity message
	if (prev_msg_idx >= sizeof(SwitchboxMsg) + sb_msg.data_length) {
		if (sb_msg.device_ID == MODULE_CONNECTIVITY_MSG) {			// TODO: ytf are we checking if device ID is equal to this, doesnt make sense
			// Prepend connectivity message for this device if the message was from the immediate neighbor
			// (this results in building a "train" whenever the last device sends a message)
			// This ensures modules arrive with indexes in ascending order
			if (sb_msg.parameter_ID == 0) {
				HAL_UART_Transmit(&huart2, connectivity_msg, sizeof(SwitchboxMsg), 1000000);
			}

			// Add 1 to index value before passing along
			sb_msg.parameter_ID += 1;
			memcpy(prev_msg_buf, &sb_msg, sizeof(SwitchboxMsg));
			HAL_UART_Transmit(&huart2, prev_msg_buf, sizeof(SwitchboxMsg), 1000000);
		}

		// For all other messages, just pass along
		else {
			HAL_UART_Transmit(&huart2, prev_msg_buf, sizeof(SwitchboxMsg) + sb_msg.data_length, 1000000);
		}

		// Reset reader index
		prev_msg_idx = 0;
	}

	HAL_UART_Receive_IT(&hlpuart1, &prev_msg_byte, 1);
	return;
}

void Next_ProcessByte() {

}

// only will send the heartbeat if its the last module in the chain, all others will append to the heartbeat coming down the line.
void send_connectivity_message() {

}

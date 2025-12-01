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

	cmsg.device_ID = DEVICE_ID;
	cmsg.control_type = CONTROL_CONNECTIVITY;
	cmsg.data_length = 0;
	cmsg.parameter_ID = 0;	// Encodes index (for connectivity messages only)

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
		//HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
		HAL_UART_Receive_IT(&hlpuart1, &prev_msg_byte, 1);
		return;
	}


	SwitchboxMsg sb_msg;
	memcpy(&sb_msg, prev_msg_buf, sizeof(SwitchboxMsg));

	if (prev_msg_idx >= sizeof(SwitchboxMsg) + sb_msg.data_length) {
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);

		// Check if this is a connectivity message
		if (sb_msg.control_type == CONTROL_CONNECTIVITY) {

			// Prepend connectivity message for this device if the message was from the immediate neighbor
			// (this results in building a "train" whenever the last device sends a message)
			// This ensures modules arrive with indexes in ascending order
			if (sb_msg.parameter_ID == 0) {
				HAL_UART_Transmit(&huart2, connectivity_msg, sizeof(SwitchboxMsg), 1000000);
			}

			// Add 1 to index value (encoded in parameter ID) before passing along
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

// send the parameter udate down the line (in the direction towards synth and midi devices and whatnot)
void send_parameter_update_toNext(uint8_t parameter_ID, uint8_t target_device_ID, uint8_t control_type, uint32_t data) {
	SwitchboxMsg message;

	message.device_ID = DEVICE_ID;
	message.target_device_ID = target_device_ID;
	message.control_type = control_type;
	message.data_length = sizeof(uint32_t);
	message.parameter_ID = parameter_ID;

	// message to send buffer
	uint8_t msg_buf[512];

	memcpy(msg_buf, &message, sizeof(SwitchboxMsg));

	// copy data to end of buffer:
	memcpy(msg_buf + sizeof(SwitchboxMsg), &data, sizeof(uint32_t));

	// transmit to next, we send data as a single uint32_t buffer that way we can capture both floats and normal integer data (and that is sufficient for any parameter update)
	HAL_UART_Transmit(&huart2, msg_buf, sizeof(SwitchboxMsg) + message.data_length, 1000000);
}

// only will send the heartbeat if its the last module in the chain, all others will append to the heartbeat coming down the line.
void send_connectivity_message() {

}

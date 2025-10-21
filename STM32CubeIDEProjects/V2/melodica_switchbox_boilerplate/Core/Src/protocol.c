/*
 * protocol.c
 *
 *  Created on: Oct 13, 2025
 *      Author: kobes
 */

#include "protocol.h"
#include "config.h"

extern UART_HandleTypeDef huart2;	// Next device
extern UART_HandleTypeDef hlpuart1; // Previous device

uint8_t msg_byte;
uint8_t msg_buf[256];
uint8_t msg_idx;
uint32_t last_rx_timestamp;

void Switchbox_Init() {
	HAL_UART_Receive_IT(&huart2, &msg_byte, 1);
	last_rx_timestamp = 0;
}

void Prev_ProcessByte() {
	// Reset interrupt
	HAL_UART_Receive_IT(&huart2, &msg_byte, 1);

	// Reset system if timeout
	uint32_t timestamp = HAL_GetTick();
	if (timestamp - last_rx_timestamp >= 10) {		// 10ms timeout
		msg_idx = 0;
	}
	last_rx_timestamp = timestamp;

	// Store into buffer
	msg_buf[msg_idx] = msg_byte;
	msg_idx++;

	if (msg_idx < 4) return;
	uint8_t length = msg_buf[3];
	// Check if this is a connectivity message
	if (msg_buf[0] == 0) {
		if (msg_idx >= 4) {
			if (msg_buf[0] == msg_buf[1]) {	// Redundancy check
				// Add 1 to index value before passing along
				msg_buf[3] += 1;
				HAL_UART_Transmit(&huart2, msg_buf, 4, 1000000);

				// Append connectivity message for this device if the message was from the immediate neighbor
				// (this results in building a "train" whenever the last device sends a message)
				if (msg_buf[3] == 1) {
					uint8_t send_buf[4] = { 0, 0, DEVICE_ID, 0 };
					HAL_UART_Transmit(&huart2, send_buf, 4, 1000000);
				}
			}

			msg_idx = 0;
		}
	}

	// For other messages, wait until enough bytes have been read (device ID + redundancy + parameter ID + length byte + data)
	else if (msg_idx >= 4 + length) {
		if (msg_buf[0] == msg_buf[1]) {	// Redundancy check
			// Pass message along
			HAL_UART_Transmit(&huart2, msg_buf, 4 + length, 1000000);
		}

		// Reset reader
		msg_idx = 0;
	}
}

void Next_ProcessByte() {

}
void send_connectivity_message() {

}

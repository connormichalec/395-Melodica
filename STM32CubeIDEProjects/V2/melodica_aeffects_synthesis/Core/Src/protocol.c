/*
 * protocol.c
 *
 *  Created on: Nov 19, 2025
 *      Author: Connor Michalec
 */

#include "protocol.h"
#include "config.h"
#include "controlstate.h"
#include <string.h>

extern UART_HandleTypeDef huart2;	// Next device
extern UART_HandleTypeDef huart3; // Previous device

#define NEXT_UART &huart2
#define PREV_UART &huart3

uint8_t prev_msg_byte;
uint8_t prev_msg_buf[512];
uint8_t prev_msg_idx = 0;
uint8_t next_msg_byte;
uint8_t next_msg_buf[512];
uint8_t next_msg_idx;
uint32_t last_rx_timestamp;

//uint8_t connectivity_msg[16];

void ProtocolInit() {
	HAL_UART_Receive_IT(PREV_UART, &prev_msg_byte, 1);
	HAL_UART_Receive_IT(NEXT_UART, &next_msg_byte, 1);
	last_rx_timestamp = 0;
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



	if (prev_msg_idx < sizeof(Msg)) {
		// Receive the rest of the message
		HAL_UART_Receive_IT(PREV_UART, &prev_msg_byte, 1);
		return;
	}

	Msg message;
	memcpy(&message, prev_msg_buf, sizeof(Msg));

	// message fully received:
	if (prev_msg_idx >= sizeof(Msg) + message.data_length) {


		// TODO: Should we implement a sort of connectivity message also for the devices? It might be useful for example the synthesizer to know if a looper exists?
		// I think this is overkill however, as we can simply implement a poll if a synthesizer wants to check if midi is getting looped it will simply send a ping down the line
		// and receive a response if necessary, so probably delete this:

		/*
		if (sb_msg.device_ID == MODULE_CONNECTIVITY_MSG) {
			// Prepend connectivity message for this device if the message was from the immediate neighbor
			// (this results in building a "train" whenever the last device sends a message)
			// This ensures modules arrive with indexes in ascending order
			if (sb_msg.parameter_ID == 0) {
				HAL_UART_Transmit(&huart3, connectivity_msg, sizeof(SwitchboxMsg), 1000000);
			}

			// Add 1 to index value before passing along
			sb_msg.parameter_ID += 1;
			memcpy(prev_msg_buf, &sb_msg, sizeof(SwitchboxMsg));
			HAL_UART_Transmit(&huart3, prev_msg_buf, sizeof(SwitchboxMsg), 1000000);
		}*/

		// For all other messages, just pass along
		//else {
			HAL_UART_Transmit(NEXT_UART, prev_msg_buf, sizeof(Msg) + message.data_length, 1000000);
		//}

		if(message.parameter_ID == 0) {   // if the message is a connectivity message
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 1);
		}

		// Reset reader index
		prev_msg_idx = 0;
	}

	HAL_UART_Receive_IT(PREV_UART, &prev_msg_byte, 1);
	return;
}

void Next_ProcessByte() {

}

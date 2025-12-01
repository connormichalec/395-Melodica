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
		// and receive a response if necessary

		if (message.control_type == CONTROL_CONNECTIVITY) {
			// handle connectivity message
		}
		else {
			// if the message is NOT a connectivity message, check if the messages target is this device:

			if(message.target_device_ID == DEVICE_ID) {

				// Handle the messages accordingly:
				//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 1);

				// Update the corresponding parameter
				update_parameter(message.parameter_ID, message.control_type, prev_msg_buf + sizeof(Msg));
			}
		}

		// Pass along all messages:
		HAL_UART_Transmit(NEXT_UART, prev_msg_buf, sizeof(Msg) + message.data_length, 1000000);

		// Reset reader index
		prev_msg_idx = 0;
	}

	HAL_UART_Receive_IT(PREV_UART, &prev_msg_byte, 1);
	return;
}

void Next_ProcessByte() {

		// For data incoming from next device in chain... might be useful for things like programming the switchboxes and propagating information backwards i guess.
}

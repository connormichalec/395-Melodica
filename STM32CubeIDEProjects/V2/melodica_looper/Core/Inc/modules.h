/*
 * modules.h
 *
 *  Created on: Sep 18, 2025
 *      Author: kobes
 */

#ifndef INC_MODULES_H_
#define INC_MODULES_H_

#include "stm32g4xx_hal.h"

extern UART_HandleTypeDef hlpuart1;

// Configurable parameters
#define MODULE_UART hlpuart1

// Constant definitions
#define MODULE_RX_TIMEOUT_MS 15

#define CONNECTIVITY_MSG_LEN 2
#define MAX_TRANSPOSE_RANGE 12

// Button definitions
#define BUTTON_TRANSPOSE_DOWN 0
#define BUTTON_TRANSPOSE_UP 1

// Internal module struct
typedef struct ModuleStream ModuleStream;
struct ModuleStream {
	uint8_t data[512];
	uint16_t data_idx;
	ModuleStream *output;
	void (*update_noteoff)(ModuleStream*, uint8_t, uint8_t, uint8_t);	// Channel, key, velocity
	void (*update_noteon)(ModuleStream*, uint8_t, uint8_t, uint8_t);	// Channel, key, velocity
	void (*update_channelpressure)(ModuleStream*, uint8_t, uint8_t);	// Channel, pressure
};

extern ModuleStream* in_stream;
extern ModuleStream out_stream;
extern ModuleStream transpose_stream;

// State structs
typedef struct TransposeState {
	int8_t shift;
} TransposeState;

extern TransposeState transpose_state;

typedef struct StopsState {
	uint8_t stops[4];
} StopsState;

extern StopsState stops_state;

typedef struct LooperState {
	uint8_t connected;
} LooperState;

typedef struct PressureToggleState{
	uint8_t pressure_enabled;
} PressureToggleState;

extern PressureToggleState pressure_toggle_state;

void send_msg(uint8_t device, uint8_t *data, uint8_t len);

void Module_ProcessByte();
void append_byte(ModuleStream* module, uint8_t byte);

void handle_looper_msg(uint8_t* data, uint8_t len);
void handle_transpose_msg(uint8_t* data, uint8_t len);
void handle_stops_msg(uint8_t* data, uint8_t len);

void out_stream_update_noteon(ModuleStream* stream, uint8_t channel, uint8_t key, uint8_t velocity);
void out_stream_update_noteoff(ModuleStream* stream, uint8_t channel, uint8_t key, uint8_t velocity);
void out_stream_update_channelpressure(ModuleStream* stream, uint8_t channel, uint8_t pressure);
void transpose_update_noteon(ModuleStream* stream, uint8_t channel, uint8_t key, uint8_t velocity);
void transpose_update_noteoff(ModuleStream* stream, uint8_t channel, uint8_t key, uint8_t velocity);
void transpose_update_channelpressure(ModuleStream* stream, uint8_t channel, uint8_t pressure);

#endif /* INC_MODULES_H_ */

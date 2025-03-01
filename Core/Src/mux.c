#include "mux.h"
#include "midi.h"

uint8_t ReadNote(uint8_t key) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, key & 0x01);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, (key & 0x02) >> 1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, (key & 0x04) >> 2);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, (key & 0x08) >> 3);

    return (uint8_t) HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15);
}



void ReadKeyboard(uint8_t status[], void *delay_func(uint16_t us) ) {
    uint8_t new_status;
	for (uint8_t i = 0; i < 16; i++) {
        new_status = ReadNote(i);
        if (!status[i] && new_status) note_on(0, 53 + i, 60);
        if (status[i] && !new_status) note_off(0, 53 + i, 0);
        status[i] = new_status;
        //delay_func(10);
    }
}


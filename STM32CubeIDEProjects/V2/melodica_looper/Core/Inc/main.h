/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define NEXT_TX_Pin GPIO_PIN_2
#define NEXT_TX_GPIO_Port GPIOA
#define NEXT_RX_Pin GPIO_PIN_3
#define NEXT_RX_GPIO_Port GPIOA
#define LOOPER_IN1_Pin GPIO_PIN_12
#define LOOPER_IN1_GPIO_Port GPIOB
#define LOOPER_IN2_Pin GPIO_PIN_13
#define LOOPER_IN2_GPIO_Port GPIOB
#define TRANSPOSE_DOWN_Pin GPIO_PIN_14
#define TRANSPOSE_DOWN_GPIO_Port GPIOB
#define TRANSPOSE_UP_Pin GPIO_PIN_15
#define TRANSPOSE_UP_GPIO_Port GPIOB
#define PREV_TX_Pin GPIO_PIN_9
#define PREV_TX_GPIO_Port GPIOA
#define PREV_RX_Pin GPIO_PIN_10
#define PREV_RX_GPIO_Port GPIOA
#define MIDI_OUT_Pin GPIO_PIN_10
#define MIDI_OUT_GPIO_Port GPIOC
#define MIDI_IN_Pin GPIO_PIN_11
#define MIDI_IN_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */
#define SWITCH_USART USART1
#define MODULES_USART USART2
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

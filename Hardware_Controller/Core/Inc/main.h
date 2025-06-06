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
#include "stm32l0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32l0xx_hal_tim.h"
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
#define Breath_Sensor_ADC_Pin GPIO_PIN_0
#define Breath_Sensor_ADC_GPIO_Port GPIOA
#define S0_Pin GPIO_PIN_4
#define S0_GPIO_Port GPIOA
#define S1_Pin GPIO_PIN_5
#define S1_GPIO_Port GPIOA
#define S2_Pin GPIO_PIN_6
#define S2_GPIO_Port GPIOA
#define S3_Pin GPIO_PIN_7
#define S3_GPIO_Port GPIOA
#define Note_input_0_Pin GPIO_PIN_11
#define Note_input_0_GPIO_Port GPIOA
#define Note_input_1_Pin GPIO_PIN_12
#define Note_input_1_GPIO_Port GPIOA
#define Note_input_2_Pin GPIO_PIN_15
#define Note_input_2_GPIO_Port GPIOA
#define Looper_input_Pin GPIO_PIN_3
#define Looper_input_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

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
#define KEY1_Pin GPIO_PIN_3
#define KEY1_GPIO_Port GPIOE
#define KEY0_Pin GPIO_PIN_4
#define KEY0_GPIO_Port GPIOE
#define LEDG_Pin GPIO_PIN_5
#define LEDG_GPIO_Port GPIOE
#define M4A_Pin GPIO_PIN_0
#define M4A_GPIO_Port GPIOA
#define M4B_Pin GPIO_PIN_1
#define M4B_GPIO_Port GPIOA
#define M4IN1_Pin GPIO_PIN_2
#define M4IN1_GPIO_Port GPIOA
#define M4IN2_Pin GPIO_PIN_3
#define M4IN2_GPIO_Port GPIOA
#define M2A_Pin GPIO_PIN_6
#define M2A_GPIO_Port GPIOA
#define M2B_Pin GPIO_PIN_7
#define M2B_GPIO_Port GPIOA
#define M2IN1_Pin GPIO_PIN_0
#define M2IN1_GPIO_Port GPIOB
#define M2IN2_Pin GPIO_PIN_1
#define M2IN2_GPIO_Port GPIOB
#define M1P_Pin GPIO_PIN_9
#define M1P_GPIO_Port GPIOE
#define M2P_Pin GPIO_PIN_11
#define M2P_GPIO_Port GPIOE
#define M3P_Pin GPIO_PIN_13
#define M3P_GPIO_Port GPIOE
#define M4P_Pin GPIO_PIN_14
#define M4P_GPIO_Port GPIOE
#define M1IN1_Pin GPIO_PIN_10
#define M1IN1_GPIO_Port GPIOB
#define M1IN2_Pin GPIO_PIN_11
#define M1IN2_GPIO_Port GPIOB
#define M3A_Pin GPIO_PIN_12
#define M3A_GPIO_Port GPIOD
#define M3B_Pin GPIO_PIN_13
#define M3B_GPIO_Port GPIOD
#define M3IN1_Pin GPIO_PIN_14
#define M3IN1_GPIO_Port GPIOD
#define M3IN2_Pin GPIO_PIN_15
#define M3IN2_GPIO_Port GPIOD
#define M1A_Pin GPIO_PIN_15
#define M1A_GPIO_Port GPIOA
#define M1B_Pin GPIO_PIN_3
#define M1B_GPIO_Port GPIOB
#define LEDR_Pin GPIO_PIN_5
#define LEDR_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

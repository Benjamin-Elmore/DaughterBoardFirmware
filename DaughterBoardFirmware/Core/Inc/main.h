/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32g0xx_hal.h"

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
#define RS485__UART_ENABLE_Pin GPIO_PIN_1
#define RS485__UART_ENABLE_GPIO_Port GPIOA
#define RS485_UART_TX_Pin GPIO_PIN_2
#define RS485_UART_TX_GPIO_Port GPIOA
#define RS485_UART_RX_Pin GPIO_PIN_3
#define RS485_UART_RX_GPIO_Port GPIOA
#define I2C_DATA_Pin GPIO_PIN_6
#define I2C_DATA_GPIO_Port GPIOA
#define I2C_CLOCK_Pin GPIO_PIN_7
#define I2C_CLOCK_GPIO_Port GPIOA
#define MUX_RESET_Pin GPIO_PIN_0
#define MUX_RESET_GPIO_Port GPIOB
#define DEBUG_LED_1_Pin GPIO_PIN_1
#define DEBUG_LED_1_GPIO_Port GPIOB
#define DEBUG_LED_2_Pin GPIO_PIN_2
#define DEBUG_LED_2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define RX_BUFFER_SIZE 128U
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

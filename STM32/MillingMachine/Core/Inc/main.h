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
#include "stm32f0xx_hal.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define M2_DIR_Pin GPIO_PIN_13
#define M2_DIR_GPIO_Port GPIOC
#define M2_EN_Pin GPIO_PIN_14
#define M2_EN_GPIO_Port GPIOC
#define USART_VSS_Pin GPIO_PIN_12
#define USART_VSS_GPIO_Port GPIOB
#define M3_VCC_Pin GPIO_PIN_13
#define M3_VCC_GPIO_Port GPIOB
#define M3_EN_Pin GPIO_PIN_14
#define M3_EN_GPIO_Port GPIOB
#define M3_DIR_Pin GPIO_PIN_15
#define M3_DIR_GPIO_Port GPIOB
#define Orange_Led_Pin GPIO_PIN_8
#define Orange_Led_GPIO_Port GPIOC
#define Green_Led_Pin GPIO_PIN_9
#define Green_Led_GPIO_Port GPIOC
#define USART_TX_Pin GPIO_PIN_9
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_10
#define USART_RX_GPIO_Port GPIOA
#define M3_GND_END_Pin GPIO_PIN_15
#define M3_GND_END_GPIO_Port GPIOA
#define M3_END_Pin GPIO_PIN_10
#define M3_END_GPIO_Port GPIOC
#define M3_VCC_END_Pin GPIO_PIN_11
#define M3_VCC_END_GPIO_Port GPIOC
#define M1_GND_END_Pin GPIO_PIN_12
#define M1_GND_END_GPIO_Port GPIOC
#define M1_END_Pin GPIO_PIN_2
#define M1_END_GPIO_Port GPIOD
#define M1_VCC_END_Pin GPIO_PIN_3
#define M1_VCC_END_GPIO_Port GPIOB
#define M2_GND_END_Pin GPIO_PIN_4
#define M2_GND_END_GPIO_Port GPIOB
#define M2_END_Pin GPIO_PIN_5
#define M2_END_GPIO_Port GPIOB
#define M2_VCC_END_Pin GPIO_PIN_6
#define M2_VCC_END_GPIO_Port GPIOB
#define M1_EN_Pin GPIO_PIN_7
#define M1_EN_GPIO_Port GPIOB
#define M1_DIR_Pin GPIO_PIN_8
#define M1_DIR_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

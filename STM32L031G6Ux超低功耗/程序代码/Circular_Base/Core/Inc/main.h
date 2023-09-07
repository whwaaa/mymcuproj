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
#include "stm32l0xx_hal.h"

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
#define NRF_CS_Pin GPIO_PIN_14
#define NRF_CS_GPIO_Port GPIOC
#define NRF_CE_Pin GPIO_PIN_15
#define NRF_CE_GPIO_Port GPIOC
#define NRF_IRQ_Pin GPIO_PIN_0
#define NRF_IRQ_GPIO_Port GPIOA
#define APDS9930_INT_Pin GPIO_PIN_4
#define APDS9930_INT_GPIO_Port GPIOA
#define TIM22_PWM_Pin GPIO_PIN_6
#define TIM22_PWM_GPIO_Port GPIOA
#define W25Q_CS_Pin GPIO_PIN_1
#define W25Q_CS_GPIO_Port GPIOB
#define TIM2_CH_Pin GPIO_PIN_8
#define TIM2_CH_GPIO_Port GPIOA
#define BLT_AT_EN_Pin GPIO_PIN_3
#define BLT_AT_EN_GPIO_Port GPIOB
#define BLT_SLEEP_Pin GPIO_PIN_7
#define BLT_SLEEP_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

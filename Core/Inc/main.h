/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32h7xx_hal.h"

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
#define LED_ON_BOARD_Pin GPIO_PIN_3
#define LED_ON_BOARD_GPIO_Port GPIOE
#define SPI1_FR_Pin GPIO_PIN_2
#define SPI1_FR_GPIO_Port GPIOC
#define SPI2_FR_Pin GPIO_PIN_3
#define SPI2_FR_GPIO_Port GPIOC
#define SPI1_CS_Pin GPIO_PIN_5
#define SPI1_CS_GPIO_Port GPIOC
#define SPI2_BUSY_Pin GPIO_PIN_0
#define SPI2_BUSY_GPIO_Port GPIOB
#define SPI12_DC_Pin GPIO_PIN_1
#define SPI12_DC_GPIO_Port GPIOB
#define SPI2_CS_Pin GPIO_PIN_2
#define SPI2_CS_GPIO_Port GPIOB
#define ROT_ENCOD_CLICK_Pin GPIO_PIN_7
#define ROT_ENCOD_CLICK_GPIO_Port GPIOE
#define ROT_ENCOD_LEFT_Pin GPIO_PIN_8
#define ROT_ENCOD_LEFT_GPIO_Port GPIOE
#define ROT_ENCOD_RIGHT_Pin GPIO_PIN_9
#define ROT_ENCOD_RIGHT_GPIO_Port GPIOE
#define SPI12_RESET_Pin GPIO_PIN_11
#define SPI12_RESET_GPIO_Port GPIOB
#define KEYB_COL_OUT_7_Pin GPIO_PIN_8
#define KEYB_COL_OUT_7_GPIO_Port GPIOD
#define KEYB_COL_OUT_0_Pin GPIO_PIN_9
#define KEYB_COL_OUT_0_GPIO_Port GPIOD
#define KEYB_COL_OUT_6_Pin GPIO_PIN_10
#define KEYB_COL_OUT_6_GPIO_Port GPIOD
#define KEYB_COL_OUT_1_Pin GPIO_PIN_11
#define KEYB_COL_OUT_1_GPIO_Port GPIOD
#define KEYB_COL_OUT_5_Pin GPIO_PIN_12
#define KEYB_COL_OUT_5_GPIO_Port GPIOD
#define KEYB_COL_OUT_2_Pin GPIO_PIN_13
#define KEYB_COL_OUT_2_GPIO_Port GPIOD
#define KEYB_COL_OUT_4_Pin GPIO_PIN_14
#define KEYB_COL_OUT_4_GPIO_Port GPIOD
#define KEYB_COL_OUT_3_Pin GPIO_PIN_15
#define KEYB_COL_OUT_3_GPIO_Port GPIOD
#define KEYB_ROW_IN_0_Pin GPIO_PIN_7
#define KEYB_ROW_IN_0_GPIO_Port GPIOC
#define KEYB_ROW_IN_6_Pin GPIO_PIN_8
#define KEYB_ROW_IN_6_GPIO_Port GPIOC
#define KEYB_ROW_IN_1_Pin GPIO_PIN_9
#define KEYB_ROW_IN_1_GPIO_Port GPIOC
#define KEYB_ROW_IN_5_Pin GPIO_PIN_8
#define KEYB_ROW_IN_5_GPIO_Port GPIOA
#define KEYB_ROW_IN_2_Pin GPIO_PIN_9
#define KEYB_ROW_IN_2_GPIO_Port GPIOA
#define KEYB_ROW_IN_4_Pin GPIO_PIN_10
#define KEYB_ROW_IN_4_GPIO_Port GPIOA
#define KEYB_ROW_IN_3_Pin GPIO_PIN_11
#define KEYB_ROW_IN_3_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

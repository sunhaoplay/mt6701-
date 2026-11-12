/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart3;

/* USER CODE BEGIN Private defines */
#ifndef UART3_RX_DMA_BUFFER_SIZE
#define UART3_RX_DMA_BUFFER_SIZE 256U
#endif
/* UART3-backed shell output queue. Data is copied here before DMA starts. */
#ifndef SHELL_TX_DMA_BUFFER_SIZE
#define SHELL_TX_DMA_BUFFER_SIZE 512U
#endif

#if (SHELL_TX_DMA_BUFFER_SIZE < 2U) || (SHELL_TX_DMA_BUFFER_SIZE > 65535U)
#error "SHELL_TX_DMA_BUFFER_SIZE must be between 2 and 65535 bytes"
#endif

/* Maximum length of one formatted shell response, including its trailing NUL. */
#ifndef SHELL_PRINTF_BUFFER_SIZE
#define SHELL_PRINTF_BUFFER_SIZE 256U
#endif

#if (SHELL_PRINTF_BUFFER_SIZE < 2U) || (SHELL_PRINTF_BUFFER_SIZE > 65535U)
#error "SHELL_PRINTF_BUFFER_SIZE must be between 2 and 65535 bytes"
#endif
/* USER CODE END Private defines */

void MX_USART3_UART_Init(void);

/* USER CODE BEGIN Prototypes */
HAL_StatusTypeDef UART_DMA_Init(void);
HAL_StatusTypeDef uart3_msg_send(const uint8_t *data, uint16_t length);
HAL_StatusTypeDef shell_send_raw(const uint8_t *data, uint16_t length);
int shell_send(const char *format, ...) __attribute__((format(printf, 1, 2)));
const uint8_t *uart3_msg_data(void);
uint16_t uart3_msg_length(void);
void uart3_msg_rev(uint8_t *buf, uint16_t len);

/** Return non-zero while the corresponding UART DMA transmission is active. */
uint8_t uart3_tx_busy(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */


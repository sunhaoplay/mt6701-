/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include <stdarg.h>
#include <stdio.h>

typedef struct
{
  UART_HandleTypeDef *uart;
  uint8_t *buffer[2];
  uint16_t buffer_size;
  uint8_t active_buffer;
  const uint8_t *volatile message_data;
  volatile uint16_t message_length;
} UART_DMA_RxContext;

static uint8_t uart3_rx_dma_buffer[2][UART3_RX_DMA_BUFFER_SIZE];

static UART_DMA_RxContext uart3_rx_context = {
  .uart = &huart3,
  .buffer = {uart3_rx_dma_buffer[0], uart3_rx_dma_buffer[1]},
  .buffer_size = UART3_RX_DMA_BUFFER_SIZE,
};

static uint8_t shell_tx_dma_buffer[SHELL_TX_DMA_BUFFER_SIZE];
static volatile uint16_t shell_tx_head;
static volatile uint16_t shell_tx_tail;
static volatile uint16_t shell_tx_count;
static volatile uint16_t shell_tx_active_length;
static volatile uint8_t shell_tx_active;
static char shell_printf_buffer[SHELL_PRINTF_BUFFER_SIZE];
static volatile uint8_t shell_printf_busy;

static HAL_StatusTypeDef UART_DMA_StartReceive(UART_DMA_RxContext *context);
static void UART_DMA_HandleRxEvent(UART_DMA_RxContext *context, uint16_t length);
static HAL_StatusTypeDef UART_DMA_Transmit(UART_HandleTypeDef *uart,
                                           const uint8_t *data,
                                           uint16_t length,
                                           GPIO_TypeDef *direction_port,
                                           uint16_t direction_pin);
static HAL_StatusTypeDef Shell_TxStartLocked(void);
static void Shell_TxComplete(void);
/* USER CODE END 0 */

UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart3_tx;

/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */
    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PD8     ------> USART3_TX
    PD9     ------> USART3_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* USART3 DMA Init */
    /* USART3_RX Init */
    hdma_usart3_rx.Instance = DMA1_Stream1;
    hdma_usart3_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart3_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart3_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart3_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart3_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart3_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart3_rx.Init.Mode = DMA_NORMAL;
    hdma_usart3_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart3_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart3_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart3_rx);

    /* USART3_TX Init */
    hdma_usart3_tx.Instance = DMA1_Stream3;
    hdma_usart3_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart3_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart3_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart3_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart3_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart3_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart3_tx.Init.Mode = DMA_NORMAL;
    hdma_usart3_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart3_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart3_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart3_tx);

    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspInit 1 */

  /* USER CODE END USART3_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();

    /**USART3 GPIO Configuration
    PD8     ------> USART3_TX
    PD9     ------> USART3_RX
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_8|GPIO_PIN_9);

    /* USART3 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
static HAL_StatusTypeDef UART_DMA_StartReceive(UART_DMA_RxContext *context)
{
  HAL_StatusTypeDef status;

  status = HAL_UARTEx_ReceiveToIdle_DMA(context->uart,
                                        context->buffer[context->active_buffer],
                                        context->buffer_size);
  if (status == HAL_OK)
  {
    /* A frame is delivered only on IDLE or when the receive buffer is full. */
    __HAL_DMA_DISABLE_IT(context->uart->hdmarx, DMA_IT_HT);
  }

  return status;
}

static void UART_DMA_HandleRxEvent(UART_DMA_RxContext *context, uint16_t length)
{
  uint8_t completed_buffer;
  uint8_t *completed_data;

  if (length > context->buffer_size)
  {
    length = context->buffer_size;
  }

  completed_buffer = context->active_buffer;
  completed_data = context->buffer[completed_buffer];
  context->message_data = completed_data;
  context->message_length = length;

  /* Restart reception first, using the other buffer, then notify the user. */
  context->active_buffer ^= 1U;
  (void)UART_DMA_StartReceive(context);

  if (context->uart->Instance == USART3)
  {
    uart3_msg_rev(completed_data, length);
  }
}

// static HAL_StatusTypeDef UART_DMA_Transmit(UART_HandleTypeDef *uart,
//                                            const uint8_t *data,
//                                            uint16_t length,
//                                            GPIO_TypeDef *direction_port,
//                                            uint16_t direction_pin)
// {
//   HAL_StatusTypeDef status;

//   if ((data == NULL) || (length == 0U))
//   {
//     return HAL_ERROR;
//   }

//   if (uart->gState != HAL_UART_STATE_READY)
//   {
//     return HAL_BUSY;
//   }

//   if (direction_port != NULL)
//   {
//     HAL_GPIO_WritePin(direction_port, direction_pin, GPIO_PIN_SET);
//   }

//   status = HAL_UART_Transmit_DMA(uart, data, length);
//   if ((status != HAL_OK) && (direction_port != NULL))
//   {
//     HAL_GPIO_WritePin(direction_port, direction_pin, GPIO_PIN_RESET);
//   }

//   return status;
// }

/* Caller must have interrupts masked while accessing the shell TX state. */
static HAL_StatusTypeDef Shell_TxStartLocked(void)
{
  HAL_StatusTypeDef status;
  uint16_t contiguous_length;
  uint16_t bytes_to_end;

  if ((shell_tx_active != 0U) || (shell_tx_count == 0U))
  {
    return HAL_OK;
  }

  if (huart3.gState != HAL_UART_STATE_READY)
  {
    return HAL_BUSY;
  }

  bytes_to_end = (uint16_t)(SHELL_TX_DMA_BUFFER_SIZE - shell_tx_tail);
  contiguous_length = shell_tx_count;
  if (contiguous_length > bytes_to_end)
  {
    contiguous_length = bytes_to_end;
  }

  status = HAL_UART_Transmit_DMA(&huart3,
                                 &shell_tx_dma_buffer[shell_tx_tail],
                                 contiguous_length);
  if (status == HAL_OK)
  {
    shell_tx_active_length = contiguous_length;
    shell_tx_active = 1U;
  }

  return status;
}

static void Shell_TxComplete(void)
{
  uint32_t primask;

  primask = __get_PRIMASK();
  __disable_irq();

  if (shell_tx_active != 0U)
  {
    shell_tx_tail = (uint16_t)(shell_tx_tail + shell_tx_active_length);
    if (shell_tx_tail >= SHELL_TX_DMA_BUFFER_SIZE)
    {
      shell_tx_tail = 0U;
    }

    shell_tx_count = (uint16_t)(shell_tx_count - shell_tx_active_length);
    shell_tx_active_length = 0U;
    shell_tx_active = 0U;
  }

  (void)Shell_TxStartLocked();
  __set_PRIMASK(primask);
}
HAL_StatusTypeDef UART_DMA_Init(void)
{
  HAL_StatusTypeDef result = HAL_OK;

  uart3_rx_context.active_buffer = 0U;
  uart3_rx_context.message_data = NULL;
  uart3_rx_context.message_length = 0U;

  shell_tx_head = 0U;
  shell_tx_tail = 0U;
  shell_tx_count = 0U;
  shell_tx_active_length = 0U;
  shell_tx_active = 0U;
  shell_printf_busy = 0U;

  if (UART_DMA_StartReceive(&uart3_rx_context) != HAL_OK)
  {
    result = HAL_ERROR;
  }
  return result;
}


HAL_StatusTypeDef uart3_msg_send(const uint8_t *data, uint16_t length)
{
  return shell_send_raw(data, length);
}

HAL_StatusTypeDef shell_send_raw(const uint8_t *data, uint16_t length)
{
  uint32_t primask;
  uint16_t index;
  HAL_StatusTypeDef start_status;

  if ((data == NULL) || (length == 0U))
  {
    return HAL_ERROR;
  }

  primask = __get_PRIMASK();
  __disable_irq();

  if (length > (uint16_t)(SHELL_TX_DMA_BUFFER_SIZE - shell_tx_count))
  {
    __set_PRIMASK(primask);
    return HAL_BUSY;
  }

  for (index = 0U; index < length; ++index)
  {
    shell_tx_dma_buffer[shell_tx_head] = data[index];
    ++shell_tx_head;
    if (shell_tx_head >= SHELL_TX_DMA_BUFFER_SIZE)
    {
      shell_tx_head = 0U;
    }
  }
  shell_tx_count = (uint16_t)(shell_tx_count + length);

  start_status = Shell_TxStartLocked();
  __set_PRIMASK(primask);

  /* HAL_BUSY means another USART1 transfer owns the peripheral for now. */
  return (start_status == HAL_ERROR) ? HAL_ERROR : HAL_OK;
}

int shell_send(const char *format, ...)
{
  uint32_t primask;
  uint8_t acquired = 0U;
  int formatted_length;
  HAL_StatusTypeDef send_status;
  va_list args;

  if (format == NULL)
  {
    return -1;
  }

  /* The static format buffer is non-blocking: a concurrent caller fails. */
  primask = __get_PRIMASK();
  __disable_irq();
  if (shell_printf_busy == 0U)
  {
    shell_printf_busy = 1U;
    acquired = 1U;
  }
  __set_PRIMASK(primask);

  if (acquired == 0U)
  {
    return -1;
  }

  va_start(args, format);
  formatted_length = vsnprintf(shell_printf_buffer,
                               sizeof(shell_printf_buffer),
                               format,
                               args);
  va_end(args);

  if ((formatted_length < 0) ||
      ((size_t)formatted_length >= sizeof(shell_printf_buffer)))
  {
    formatted_length = -1;
  }
  else if (formatted_length > 0)
  {
    send_status = shell_send_raw((const uint8_t *)shell_printf_buffer,
                                 (uint16_t)formatted_length);
    if (send_status != HAL_OK)
    {
      formatted_length = -1;
    }
  }

  primask = __get_PRIMASK();
  __disable_irq();
  shell_printf_busy = 0U;
  __set_PRIMASK(primask);

  return formatted_length;
}

uint8_t uart3_tx_busy(void)
{
  return ((shell_tx_count != 0U) ||
          (huart3.gState == HAL_UART_STATE_BUSY_TX)) ? 1U : 0U;
}
const uint8_t *uart3_msg_data(void)
{
  return uart3_rx_context.message_data;
}

uint16_t uart3_msg_length(void)
{
  return uart3_rx_context.message_length;
}
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *uart, uint16_t length)
{
  HAL_UART_RxEventTypeTypeDef event_type;

  event_type = HAL_UARTEx_GetRxEventType(uart);

  /* HT does not delimit a message; IDLE and a full-buffer TC event do. */
  if (event_type == HAL_UART_RXEVENT_HT)
  {
    return;
  }

  if (uart->Instance == USART3)
  {
    UART_DMA_HandleRxEvent(&uart3_rx_context, length);
  }

}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *uart)
{
  /* Continue the shell queue, or return an RS-485 transceiver to receive. */
  if (uart->Instance == USART3)
  {
    Shell_TxComplete();
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *uart)
{
  UART_DMA_RxContext *context = NULL;

  if (uart->Instance == USART3)
  {
    context = &uart3_rx_context;

    /* If a shell DMA TX failed, retry its current contiguous queue block. */
    if ((shell_tx_active != 0U) && (uart->gState == HAL_UART_STATE_READY))
    {
      uint32_t primask = __get_PRIMASK();
      __disable_irq();
      shell_tx_active_length = 0U;
      shell_tx_active = 0U;
      (void)Shell_TxStartLocked();
      __set_PRIMASK(primask);
    }
  }

  /* Blocking UART/DMA errors stop RX; restart it once HAL reports READY. */
  if ((context != NULL) && (uart->RxState == HAL_UART_STATE_READY))
  {
    (void)UART_DMA_StartReceive(context);
  }
}

__weak void uart3_msg_rev(uint8_t *buf, uint16_t len)
{
  (void)buf;
  (void)len;
}
/* USER CODE END 1 */


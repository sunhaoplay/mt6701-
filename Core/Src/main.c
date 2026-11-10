/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ENCODER_CPR 4096U
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Z 相中断与主循环共享的变量 */
volatile uint8_t  encoder_z_found = 0;
volatile uint16_t encoder_z_offset = 0;
volatile uint16_t encoder_z_last_capture = 0;
volatile uint32_t encoder_z_events = 0;

/* 方便在调试器中观察 */
volatile uint16_t encoder_raw_count = 0;
volatile uint16_t encoder_position = 0;
volatile float   encoder_angle_deg = 0.0f;
/*encoder_z_found：是否已经找到第一次 Z。
encoder_z_offset：第一次 Z 对应的原始计数，作为零点。
encoder_z_last_capture：最近一次 Z 对应的原始计数。
encoder_z_events：Z 边沿触发次数。
encoder_position：换算后的单圈位置。
encoder_angle_deg：机械角度，单位为度。
*/
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
/* 上电时先以当前位置为临时零点 */
__HAL_TIM_SET_COUNTER(&htim3, 0);
/* 清除可能遗留的 CH3 捕获标志 */
__HAL_TIM_CLEAR_FLAG(&htim3,TIM_FLAG_CC3 | TIM_FLAG_CC3OF);
/* 启动 A/B 硬件计数 */
if (HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL) != HAL_OK)
{
    Error_Handler();
}
/* 启动 Z 相输入捕获中断 */
if (HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_3) != HAL_OK)
{
    Error_Handler();
}
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    LED_B_Toggle();
    char tx_buf[128];

/*
 * 短暂关闭中断，取得一组一致的数据。
 * 完成读取后立即恢复，串口发送期间不关闭中断。
 */
uint32_t saved_primask = __get_PRIMASK();
__disable_irq();

uint8_t referenced = encoder_z_found;
uint16_t offset = referenced ? encoder_z_offset : 0U;
uint16_t raw = (uint16_t)__HAL_TIM_GET_COUNTER(&htim3);
uint16_t z_capture = encoder_z_last_capture;
uint32_t z_events = encoder_z_events;

__set_PRIMASK(saved_primask);

/* 相对零点的单圈位置，范围 0～4095 */
int32_t position = (int32_t)raw - (int32_t)offset;

if (position < 0)
{
    position += (int32_t)ENCODER_CPR;
}

/* 更新调试器中可观察的变量 */
encoder_raw_count = raw;
encoder_position = (uint16_t)position;
encoder_angle_deg =
    (float)position * 360.0f / (float)ENCODER_CPR;

/*
 * 用整数格式输出两位小数。
 * 避免依赖 printf 的浮点输出支持。
 */
uint32_t angle_x100 =
    (uint32_t)position * 36000U / ENCODER_CPR;

int len = snprintf(
    tx_buf,
    sizeof(tx_buf),
    "REF=%u CNT=%u POS=%u ANG=%lu.%02lu Z=%lu ZCAP=%u\r\n",
    (unsigned int)referenced,
    (unsigned int)raw,
    (unsigned int)position,
    (unsigned long)(angle_x100 / 100U),
    (unsigned long)(angle_x100 % 100U),
    (unsigned long)z_events,
    (unsigned int)z_capture
);

if ((len > 0) && ((size_t)len < sizeof(tx_buf)))
{
    HAL_UART_Transmit(
        &huart3,
        (uint8_t *)tx_buf,
        (uint16_t)len,
        100
    );
}

HAL_Delay(200);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if ((htim->Instance == TIM3) &&
        (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3))
    {
        /* CCR3 保存的是 Z 边沿到来时的编码器计数 */
        uint16_t captured = (uint16_t)
            HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);

        encoder_z_last_capture = captured;

        /* 第一次经过 Z 时记录零点，之后保持该零点 */
        if (encoder_z_found == 0U)
        {
            encoder_z_offset = captured;
            encoder_z_found = 1U;
        }

        encoder_z_events++;
    }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

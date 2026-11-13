#include "init.h"
#include "tim.h"
#include "adc_drv.h"
#include "motor_ctrl.h"
#include "adc.h"
#include "usart.h"
#include "stdint.h"
volatile uint8_t g_motor_state = 0; 
void ALL_Init(void)
{
    g_motor_state = 0;
/* 上电时先以当前位置为临时零点 */
__HAL_TIM_SET_COUNTER(&htim3, 0);
/* 清除可能遗留的 CH3 捕获标志 */
__HAL_TIM_CLEAR_FLAG(&htim3,TIM_FLAG_CC3 | TIM_FLAG_CC3OF);
  if (UART_DMA_Init() != HAL_OK)
  {
    Error_Handler();
  }
/* 启动 A/B 硬件计数 */
if (HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL) != HAL_OK)
{
    Error_Handler();
}
    // 1. 启动规则组 DMA (慢速信号，传 3 个)
    HAL_ADC_Start_DMA(&hadc2, (uint32_t *)Slow_ADC, 3);

    // 2. 开启注入组的转换和中断 (注意：这里只是挂起等待触发)
    HAL_ADCEx_InjectedStart_IT(&hadc1);

    MotorRun(); // 启动电机

    HAL_Delay(10); // 等待 ADC 稳定  
    AdcSampleOffset(); // 采集 ADC 偏置
    MotorStop();
    PositionToZeroDouble();// /* 启动 Z 相输入捕获中断 */
    MotorRun(); // 启动电机
    g_motor_state = 1;
    // MotorStop(); // 停止电机
// if (HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_3) != HAL_OK)
// {
//     Error_Handler();
// }
}
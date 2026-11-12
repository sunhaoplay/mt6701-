#include "init.h"
#include "tim.h"
#include "adc_drv.h"
#include "motor_ctrl.h"
#include "adc.h"
#include "usart.h"
void ALL_Init(void)
{
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
HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&ADC_Value, 5);		
HAL_Delay(10); // 等待 ADC 稳定  
AdcSampleOffset(); // 采集 ADC 偏置
/* 启动 Z 相输入捕获中断 */
if (HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_3) != HAL_OK)
{
    Error_Handler();
}
  MotorRun(); // 启动电机
}
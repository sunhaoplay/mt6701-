#include "adc_drv.h"
#include "main.h"
#include "motor_para.h"
#include "init.h"
#include "motor_ctrl.h"


ADC_Sample AdcPara = ADC_Sample_DEFAULTS;
ADC_Sample_F AdcFilPara = ADC_Sample_F_DEFAULTS;

// uint16_t ADC_Value[5];
// 1. 注入组：U/W 相电流（由 ADC 硬件中断直接读取）
volatile uint16_t Inj_Current[2]; // [0] 是 U 相 (Rank 1), [1] 是 W 相 (Rank 2)

// 2. 规则组：Vbus, Temp, Vr（由 DMA 循环搬运）
uint16_t Slow_ADC[3];             // [0] 是 Vbus, [1] 是 Temp, [2] 是 Vr
void AdcSampleOffset(void)
{
	// uint8_t i = 0;
	// uint8_t j = 0;
	// uint16_t Sum1 = 0;
	// uint16_t Sum2 = 0;

	// for (i = 0; i < 10; i++)
	// {
	// 	for (j = 0; j < 100; j++)
	// 	{
	// 		AdcPara.OffsetCurU = ADC_Value[0];
	// 		AdcPara.OffsetCurW = ADC_Value[3];

	// 		HAL_Delay(1);
	// 	}
	// 	Sum1 += AdcPara.OffsetCurU;
	// 	Sum2 += AdcPara.OffsetCurW;
	// }
	// AdcFilPara.OffsetCurU = Sum1 / 10;
	// AdcFilPara.OffsetCurW = Sum2 / 10;
	uint32_t SumU = 0;
    uint32_t SumW = 0;

    // 此时电机应该静止且没有电流。我们连续采 1000 次求平均
    for (int i = 0; i < 1000; i++)
    {
        SumU += Inj_Current[0]; // 读注入组的 U 相
        SumW += Inj_Current[1]; // 读注入组的 W 相
        HAL_Delay(1);
    }
    
    AdcFilPara.OffsetCurU = SumU / 1000;
    AdcFilPara.OffsetCurW = SumW / 1000;
    
    // 同时复位一下滤波器状态，防止启动瞬间突变
    AdcFilPara.CurU = 0.0f;
    AdcFilPara.CurV = 0.0f;
    AdcFilPara.CurW = 0.0f;
}

static void AdcSample(void)
{
    // 1. 计算三相电流 (用 Inj_Current 替换原来的 ADC_Value)
    // 注意：Inj_Current 是 volatile uint16_t，最好先强转一下类型再减
	AdcPara.CurU = (float)((int32_t)AdcFilPara.OffsetCurU - (int32_t)Inj_Current[0]) * I_GAIN;
	AdcPara.CurW = (float)((int32_t)AdcFilPara.OffsetCurW - (int32_t)Inj_Current[1]) * I_GAIN;
	AdcPara.CurV = -AdcPara.CurU - AdcPara.CurW;

    // 2. 读取慢速信号 (用 Slow_ADC 替换原来的 ADC_Value)
	AdcPara.Vbus = (float)Slow_ADC[0] * VBUS_GAIN;
	AdcPara.Temp = (float)Slow_ADC[1];
	AdcPara.Vr   = (float)Slow_ADC[2];
}

static void AdcSampleDeal(void)
{
	AdcFilPara.CurU = LPF_I_RUN_B * AdcFilPara.CurU + LPF_I_RUN_A * AdcPara.CurU;
	AdcFilPara.CurV = LPF_I_RUN_B * AdcFilPara.CurV + LPF_I_RUN_A * AdcPara.CurV;
	AdcFilPara.CurW = LPF_I_RUN_B * AdcFilPara.CurW + LPF_I_RUN_A * AdcPara.CurW;
	AdcFilPara.Vbus = AdcFilPara.Vbus * LPF_I_STOP_B + AdcPara.Vbus * LPF_I_STOP_A;
	AdcFilPara.Temp = AdcFilPara.Temp * LPF_I_STOP_B + AdcPara.Temp * LPF_I_STOP_A;
}

void GetAdc(void)
{
	AdcSample();
	AdcSampleDeal();
}
// 存放注入组相电流的全局变量，一定要加 volatile
volatile uint16_t Inj_Current[2]; 

void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC1)
    {
        // 1. 更新原始 Buffer
        Inj_Current[0] = HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_1);
        Inj_Current[1] = HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_2);
        
        // 2. 计算电流、滤波
        GetAdc(); 
        if (g_motor_state == 0)
		{
			return; // 电机未启动时不执行控制逻辑
		}
        // 3. 执行电机控制逻辑
        QtMotor();
        FocSvpwm();
    }
}
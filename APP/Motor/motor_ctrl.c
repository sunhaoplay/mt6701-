#include "motor_ctrl.h"
#include "motor_para.h"
#include "foc.h"
#include "tim.h"

IPARK /*******************/ Ipark = IPARK_DEFAULTS;
motor_p /*****************/ Motor ;
PARA /********************/ Para;
SVPWM Svpwm;

// 强制拖动
void QtMotor(void)
{
	Para.IfFrq++;
	if (Para.IfFrq == QT_FRQ) // 强拖频率
	{
		Para.IfFrq = 0;
		Ipark.Alpha = ualpha_tab[Para.IfAngle];
		Ipark.Beta = ubeta_tab[Para.IfAngle];
		Para.IfAngle++;
		if (Para.IfAngle > 359)
			Para.IfAngle = 0;
	}
}
// SVPWM计算
void FocSvpwm(void)
{
	// 电流环输出的 d/q 电压经 IPark 后送入 SVPWM 更新三相占空比
	Svpwm.Ualpha = Ipark.Alpha; // IPARK的数据传入SVPWM
	Svpwm.Ubeta = Ipark.Beta;
	SvpwmCal((M_SVPWM)&Svpwm); // 将Alpha和Beta电压带入计算SVPWM的占空比
}
// 启动电机
void MotorRun(void)
{
	// Fault_Detection();				//启动前先进行故障检测
	// 使能三相上下桥 PWM 输出，后续占空比由 SVPWM 更新
	TIM1->CCER |= 0x5555;
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
	TIM1->CCR1 = (uint16_t)(TIM1_PERIOD_HALF);
	TIM1->CCR2 = (uint16_t)(TIM1_PERIOD_HALF);
	TIM1->CCR3 = (uint16_t)(TIM1_PERIOD_HALF);
}

#include "motor_ctrl.h"
#include "motor_para.h"
#include "foc.h"
#include "tim.h"
#include "pid.h"
#include "adc_drv.h"
#include "encoder.h"
CLARKE /******************/ Clarke = CLARKE_DEFAULTS;
IPARK /*******************/ Ipark = IPARK_DEFAULTS;
motor_p /*****************/ Motor ;
PARA /********************/ Para;
SVPWM Svpwm;
PARK /********************/ Park = PARK_DEFAULTS;
void Vrsetcurrent(void)
{
		static float AdcVr = 0;
	    AdcVr = (float)(AdcPara.Vr / 2000.0f);
		Para.CurRef = (float)AdcVr;
		Para.CurRef = Limit_Sat(Para.CurRef, 1.0f, 0.05f);
}
// 强制拖动
void QtMotor(void)
{
	Para.IfFrq++;
	if (Para.IfFrq == QT_FRQ) // 强拖频率
	{
		Para.IfFrq = 0;
		Ipark.Alpha = ualpha_tab[Para.IfAngle]*0.4;
		Ipark.Beta = ubeta_tab[Para.IfAngle]*0.4;
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
	TIM1->CCR1 = (uint16_t)(TIM1_PERIOD_HALF);
	TIM1->CCR2 = (uint16_t)(TIM1_PERIOD_HALF);
	TIM1->CCR3 = (uint16_t)(TIM1_PERIOD_HALF);
	TIM1->EGR = TIM_EGR_UG;
    __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);
	TIM1->CCER |= 0x5555;
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);

}
/**
 * @brief 立即禁止 TIM1 三相主/互补 PWM、清零比较值并复位控制动态状态。
 *
 * CCER 与 0xAAAA 相与会清除各通道输出使能位；CCR1..3 置零后调用
 * VariableClear()，确保下一次启动不继承 PI 积分或观测器历史。
 */
void MotorStop(void)
{
	// 关闭三相互补 PWM 输出，防止停机后继续驱动功率管
	TIM1->CCER &= 0xAAAA; // 不使能PWM输出通道OC1/OC1N/OC2/OC2N/OC3/OC3N

	TIM1->CCR1 = 0;
	TIM1->CCR2 = 0;
	TIM1->CCR3 = 0;

	// VariableClear(); // 变量清零
}
/**
 * @brief 用固定 alpha 轴电压吸合转子并把当前位置定义为编码器零点。
 *
 * 先使能六路互补 PWM，输出 Ualpha=1、Ubeta=0 的静止电压矢量并等待转子稳定，
 * 然后清零 TIM3->CNT 和 Motor.E_theta，最后关闭 PWM 输出。
 * @warning 函数包含秒级阻塞延时并会产生保持转矩，只能在停机校准流程调用。
 */
void PositionToZero(void)
{
    TIM1->CCER |= 0x5555;
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
    HAL_Delay(100);

    Svpwm.Ualpha = 1; // 固定定子电压矢量，使转子磁极对齐 alpha 轴。
    Svpwm.Ubeta = 0;
    SvpwmCal((M_SVPWM)&Svpwm);
    HAL_Delay(1000);
    TIM3->CNT = 0;     // 把吸合后的机械位置定义为编码器零点。
    Motor.E_theta = 0;
    HAL_Delay(500);

    TIM1->CCER &= 0xAAAA;
    HAL_Delay(500);
}

/**
 * @brief 先后施加 beta、alpha 两个电压矢量后执行编码器调零。
 *
 * 第一次吸合让转子离开可能的不确定初始位置，第二次吸合到最终 alpha 轴方向，
 * 可减小单次吸合受静摩擦或初始夹角影响产生的零位误差。最后清零编码器并停机。
 * @warning 函数会驱动电机且包含多个阻塞延时，机构必须允许校准运动。
 */
void PositionToZeroDouble(void)
{
    TIM1->CCER |= 0x5555;
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);

    HAL_Delay(100);

    Svpwm.Ualpha = 0.0f;
    Svpwm.Ubeta = 0.5f; // 第一次沿 beta 轴吸合。
    SvpwmCal((M_SVPWM)&Svpwm);
    HAL_Delay(1000);

    Svpwm.Ualpha = 0.5f; // 第二次沿 alpha 轴吸合到最终零位。
    Svpwm.Ubeta = 0.0f;
    SvpwmCal((M_SVPWM)&Svpwm);
    HAL_Delay(1000);

    TIM3->CNT = 0;
    Motor.E_theta = 0;

    HAL_Delay(1000);

    TIM1->CCER &= 0xAAAA;
    HAL_Delay(1000);

    // MotorStop();
}
/**
 * @brief 使用给定电角度完成有感 d/q 电流双闭环。
 * @param IqRef q 轴电流给定，单位 A，正负决定转矩方向。
 * @param Theta 编码器换算得到的转子电角度，单位 rad。
 *
 * 表贴式 PMSM 常用 IdRef=0，使定子电流主要用于产生转矩。两个 PI 输出 Ud/Uq，
 * 再经反 Park 变换形成供 SVPWM 使用的 Ualpha/Ubeta。
 */
void CurCloseloop(float IqRef, float Theta)
{
    pi_id.Ref = 0.0f;
    pi_id.Fbk = Park.Ds;
    PiController((M_PI_Control)&pi_id);

    pi_iq.Ref = IqRef;
    pi_iq.Fbk = Park.Qs;
    PiController((M_PI_Control)&pi_iq);

    Ipark.Theta = Theta;
    Ipark.Ds = pi_id.Out;
    Ipark.Qs = pi_iq.Out;
    IPARK_Cale((M_IPARK)&Ipark);
}

/**
 * @brief 更新一个高速周期的电流坐标、编码器角度和全部相关观测器状态。
 *
 * 数据流为 ADC 三相电流 -> Clarke -> 观测器/编码器角度选择 -> Park。磁链模型
 * 每拍后台更新，融合模式再执行相应 HFI/SMO/IF 切换逻辑。最终 Park.Theta
 * 严格按当前模式选择，得到供内层电流 PI 使用的 Park.Ds/Park.Qs。
 * @note 由 TIM1 更新中断在 GetAdc() 后每 50 us 调用。
 */
void AxisDq(void)
{
	// FOC 主采样链路：三相电流 -> Clarke -> 角度选择 -> Park
	Para.SpeedCalTime++;

	// 1. 采样三相电流并做 Clarke 变换
	Clarke.Iu = AdcPara.CurU;
	Clarke.Iv = AdcPara.CurV;
	Clarke.Iw = AdcPara.CurW;
	ClarkeCale((M_CLARKE)&Clarke);

	MotorAngleGet(); // 获取编码器角度

	Park.Theta = Motor.E_theta; // 编码器角度

	// 5. Park 变换，得到 d/q 轴电流反馈
	Park.Alpha = Clarke.Alpha;
	Park.Beta = Clarke.Beta;
	ParkCale((M_PARK)&Park);
}
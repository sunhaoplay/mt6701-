/**
 * @file pid.c
 * @brief 实现各级闭环共用的 PI 算法、饱和函数和默认参数初始化。
 *
 * pi_id/pi_iq 输出电压指令，pi_spd 输出 q 轴电流指令，pi_pos 预留给位置环。
 * 所有实例使用同一个 TS 更新积分，因此调用频率改变时必须同步审查 Ki 的含义。
 */
#include "motor_ctrl.h"
#include "pid.h"
#include "motor_para.h"
PI_Control pi_spd = PI_Control_DEFAULTS;
PI_Control pi_id = PI_Control_DEFAULTS;
PI_Control pi_iq = PI_Control_DEFAULTS;
PI_Control pi_pos = PI_Control_DEFAULTS;

/**
 * @brief 将输入限制在指定上下限之间。
 * @param Uint 输入值。
 * @param U_max 最大允许值。
 * @param U_min 最小允许值。
 * @return Uint 位于区间内时原样返回，否则返回相应边界。
 */
float Limit_Sat(float Uint, float U_max, float U_min)
{
	float Uout;

	if (Uint <= U_min)
		Uout = U_min;
	else if (Uint >= U_max)
		Uout = U_max;
	else
		Uout = Uint;
	return Uout;
}

/**
 * @brief 执行一拍位置式并联 PI 控制并限制积分项与总输出。
 * @param pv PI 控制器实例指针。
 *
 * e=Ref-Fbk；I[n]=sat(I[n-1]+Ki*e*TS)；Out=sat(Kp*e+I[n])。
 * 对积分状态先限幅可减轻输出饱和时的积分饱和，但不包含反算抗饱和环节。
 */
void PiController(M_PI_Control pv)
{
	pv->err = pv->Ref - pv->Fbk;
	// 离散积分：连续积分 Ki*integral(e dt) 用前向欧拉近似。
	pv->integrator += pv->Ki * pv->err * TS;
	// 限制积分状态，防止执行器饱和时积分项无限增长。
	if (pv->integrator > pv->Umax)
		pv->integrator = pv->Umax;
	if (pv->integrator < pv->Umin)
		pv->integrator = pv->Umin;
	// PI 总输出 = 比例项 + 积分项，并限制到执行器允许范围。
	float u = pv->Kp * pv->err + pv->integrator;
	pv->Out = Limit_Sat(u, pv->Umax, pv->Umin);
}

/**
 * @brief 初始化速度环、d/q 电流环和位置环 PI 参数。
 *
 * 速度环参数来自 Para，输出限制为允许的 q 轴电流；电流环输出限制为 d/q 轴
 * 电压能力；位置环输出限制为速度给定。这里只装载参数，不清除动态状态。
 */
void PiParaInit(void)
{
	pi_spd.Kp = Para.SpdLessKp;
	pi_spd.Ki =Para.SpdLessKi;
	pi_spd.Umax = PI_MAX_SPD;
	pi_spd.Umin = -PI_MAX_SPD;

	pi_id.Kp = 0.3f;
	pi_id.Ki = 1000.0f;
	pi_id.Umax = PI_MAX_Ud;
	pi_id.Umin = -PI_MAX_Ud;

	pi_iq.Kp = 0.3f;
	pi_iq.Ki = 1000.0f;
	pi_iq.Umax = PI_MAX_Uq;
	pi_iq.Umin = -PI_MAX_Uq;

	pi_pos.Kp = 0.02f;
	pi_pos.Ki = 0.0f;
	pi_pos.Umax = SPD_REF_MAX;
	pi_pos.Umin = -SPD_REF_MAX;
}

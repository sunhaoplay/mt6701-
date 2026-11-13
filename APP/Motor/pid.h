/**
 * @file pid.h
 * @brief 电流环、速度环和位置环共用的离散 PI 控制器数据结构与接口。
 *
 * 控制器使用并联形式 u=Kp*e+integrator，积分状态按采样周期 TS 更新并独立限幅，
 * 用于抑制执行器饱和时的积分继续累积。不同环路共享同一算法但使用独立实例。
 */

#ifndef __PID_H
#define __PID_H

#include "motor_ctrl.h"

/** @brief 一套 PI 参数、动态状态和输入输出数据。 */
typedef struct {
        float Ref;        /**< 给定值 r。 */
        float Fbk;        /**< 反馈值 y。 */
        float Out;        /**< 限幅后的 PI 输出 u。 */
        float OutF;       /**< 某些环路额外低通后的输出。 */
        float Kp;         /**< 比例增益。 */
        float Ki;         /**< 积分增益，算法内部还会乘采样周期 TS。 */
        float Umax;       /**< 输出与积分项允许的上限。 */
        float Umin;       /**< 输出与积分项允许的下限。 */
        float err;        /**< 当前误差 e=r-y。 */
        float err_l;      /**< 上一拍误差，供其他 PI 形式或调试保留。 */
        float integrator; /**< 积分状态 sum(Ki*e*TS)。 */
        float v1;         /**< 保留的中间状态，当前算法未使用。 */
        float i1;         /**< 保留的中间状态，当前算法未使用。 */
       } PI_Control, *M_PI_Control;

#define PI_Control_DEFAULTS {0,0,0,0,0,0,0,0,0,0} /**< PI 实例的全零初始化值。 */

extern PI_Control pi_spd; /**< 速度 PI，输出 q 轴电流给定。 */
extern PI_Control pi_id;  /**< d 轴电流 PI，输出 d 轴电压。 */
extern PI_Control pi_iq;  /**< q 轴电流 PI，输出 q 轴电压。 */
extern PI_Control pi_pos; /**< 位置 PI 实例，当前位置代码主要使用 Pos 参数。 */

/** @brief 执行一拍带积分限幅的并联 PI 计算。 @param pv 待更新的 PI 实例。 */
void PiController(M_PI_Control pv);

/** @brief 装载速度、d/q 电流和位置四个环路的默认参数及输出范围。 */
void PiParaInit(void);

/**
 * @brief 把输入值限制在闭区间 [U_min,U_max]。
 * @param Uint 待限制输入。
 * @param U_max 上限。
 * @param U_min 下限。
 * @return 限幅后的数值。
 */
float Limit_Sat(float Uint, float U_max, float U_min);

#endif

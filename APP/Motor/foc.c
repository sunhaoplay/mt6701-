#include "foc.h"
#include "tim.h"
#include "motor_para.h"

/*
**
 * @brief 将三相电流投影到两相静止 alpha/beta 坐标。
 * @param pv Clarke 输入输出对象。
 *
 * 在 Iu+Iv+Iw=0 条件下采用 alpha=Iu，
 * beta=Iu/sqrt(3)+2*Iv/sqrt(3)。常数 0.57735027 和 1.1547004 分别是
 * 1/sqrt(3) 与 2/sqrt(3)。
 */
void ClarkeCale(M_CLARKE pv)
{
    pv->Alpha = pv->Iu;
    pv->Beta = (pv->Iu * 0.57735027f) + (pv->Iv * 1.1547004f);
}

/**
 * @brief 将静止 alpha/beta 矢量旋转到转子同步 d/q 坐标。
 * @param pv Park 输入输出对象，Theta 为电角度 rad。
 *
 * d=alpha*cos(theta)+beta*sin(theta)；
 * q=beta*cos(theta)-alpha*sin(theta)。d 轴与转子磁链对齐，q 轴负责转矩。
 */
void ParkCale(M_PARK pv)
{
    pv->Ds = arm_cos_f32(pv->Theta) * pv->Alpha + arm_sin_f32(pv->Theta) * pv->Beta;
    pv->Qs = arm_cos_f32(pv->Theta) * pv->Beta - arm_sin_f32(pv->Theta) * pv->Alpha;
}

/**
 * @brief 将旋转 d/q 电压矢量反变换到静止 alpha/beta 坐标。
 * @param pv 反 Park 输入输出对象，Theta 必须与电流 Park 变换使用同一电角度定义。
 *
 * alpha=d*cos(theta)-q*sin(theta)；
 * beta=d*sin(theta)+q*cos(theta)。
 */
void IPARK_Cale(M_IPARK pv)
{
    pv->Alpha = arm_cos_f32(pv->Theta) * pv->Ds - arm_sin_f32(pv->Theta) * pv->Qs;
    pv->Beta = arm_sin_f32(pv->Theta) * pv->Ds + arm_cos_f32(pv->Theta) * pv->Qs;
}
void SvpwmCal(M_SVPWM pv)
{
    pv->u1 = pv->Ubeta; 
    pv->u2 = pv->Ubeta * 0.5f + pv->Ualpha * 0.8660254f;
    pv->u3 = pv->u2 - pv->u1;

   
    pv->VecSector = 3;
    pv->VecSector = (pv->u2 > 0) ? (pv->VecSector - 1) : pv->VecSector;
    pv->VecSector = (pv->u3 > 0) ? (pv->VecSector - 1) : pv->VecSector;
    pv->VecSector = (pv->u1 < 0) ? (7 - pv->VecSector) : pv->VecSector;

   
    if ((pv->VecSector == 1) || (pv->VecSector == 4))
    {
        pv->Ta = pv->u2;
        pv->Tb = pv->u1 - pv->u3;
        pv->Tc = -pv->u2;
    }
    else if ((pv->VecSector == 2) || (pv->VecSector == 5))
    {
        pv->Ta = pv->u3 + pv->u2;
        pv->Tb = pv->u1;
        pv->Tc = -pv->u1;
    }
    else if ((pv->VecSector == 3) || (pv->VecSector == 6))
    {
        pv->Ta = pv->u3;
        pv->Tb = -pv->u3;
        pv->Tc = -(pv->u1 + pv->u2);
    }
    else 
    {
        pv->Ta = 0;
        pv->Tb = 0;
        pv->Tc = 0;
    }


    TIM1->CCR1 = (uint16_t)((pv->Ta * SVPWM_KM_BACKW * TIM1_PERIOD_HALF) + TIM1_PERIOD_HALF);
    TIM1->CCR2 = (uint16_t)((pv->Tb * SVPWM_KM_BACKW * TIM1_PERIOD_HALF) + TIM1_PERIOD_HALF);
    TIM1->CCR3 = (uint16_t)((pv->Tc * SVPWM_KM_BACKW * TIM1_PERIOD_HALF) + TIM1_PERIOD_HALF);
}

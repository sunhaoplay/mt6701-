#include "foc.h"
#include "tim.h"
#include "motor_para.h"
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

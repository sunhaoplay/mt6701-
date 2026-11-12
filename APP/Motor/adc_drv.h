#ifndef ADC_DRV_H
#define ADC_DRV_H

#include "main.h"

typedef struct
{
	float Vbus;	   
	float Vr;	   
	uint16_t Temp; 

	float CurU; 
	float CurV; 
	float CurW; 

	int16_t OffsetCurU; 
	int16_t OffsetCurV; 
	int16_t OffsetCurW; 
} ADC_Sample;

#define ADC_Sample_DEFAULTS {0, 0, 0, 0, 0, 0} 

typedef struct
{
	float Vbus; 
	float Temp;

	float CurU;
	float CurV;
	float CurW;

	int16_t OffsetCurU;
	int16_t OffsetCurV;
	int16_t OffsetCurW;
} ADC_Sample_F;

#define ADC_Sample_F_DEFAULTS {0, 0, 0, 0, 0, 0} 

typedef struct
{
	float Yn;
	float Yn_last;
	float Xn;
	float Xn_last;
	float Tau;
} V_Comp, *P_Vcomp;

#define Voltage_Compensation_DEFAULTS {0, 0, 0, 0, 0}
extern uint16_t Slow_ADC[3];             
extern volatile uint16_t Inj_Current[2]; 

// extern uint16_t ADC_Value[5]; 
extern uint8_t NTC_ERROR;	  

extern ADC_Sample AdcPara;
extern ADC_Sample_F AdcFilPara;

void AdcSampleOffset(void);						
void GetAdc(void);								 
void TempTrans(uint16_t ADC_value, float *temp); 

#endif

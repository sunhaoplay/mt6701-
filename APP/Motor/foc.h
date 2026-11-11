#ifndef FOC_H
#define FOC_H
#include "stdint.h"

typedef struct
{
	float Iu;	 
	float Iv;	
	float Iw;	 
	float Alpha; 
	float Beta;	 
} CLARKE, *M_CLARKE;

typedef struct
{
	float Alpha; 
	float Beta;	 
	float Theta; 
	float Ds;	 
	float Qs;	
} PARK, *M_PARK;
typedef struct
{
	float Alpha; 
	float Beta;	
	float Theta; 
	float Ds;	 
	float Qs;	 
} IPARK, *M_IPARK;

typedef struct
{
    float M_theta;             
    float E_theta;             
    float speed_M_rpm;         
    float speed_E_rpm;         
    volatile uint8_t CtrlMode; 
    float Rs;                  
    float Ls;                  
    float Ib;                  
    float Vb;                  
    uint8_t P;                 
} motor_p;

#define CLARKE_DEFAULTS {0, 0, 0, 0, 0} 
#define PARK_DEFAULTS {0, 0, 0, 0, 0}	
#define IPARK_DEFAULTS {0, 0, 0, 0, 0}	

#define MODIFY_TX_TY(x, y, z, s) \
	{                            \
		z = x + y;               \
		if (x + y > s)           \
		{                        \
			x = x / z * s;       \
			y = y / z * s;       \
		}                        \
	}

typedef struct
{
	float Ualpha;	   
	float Ubeta;	  
	float Ta;		   
	float Tb;		   
	float Tc;		  
	float u1;		   
	float u2;		   
	float u3;		   
	float T0;		   
	uint8_t VecSector; 
} SVPWM, *M_SVPWM;

#define SVPWM_DEFAULTS {0, 0, 0, 0, 0, 0, 0, 0, 0, 0} 
extern SVPWM Svpwm;

void SvpwmCal(M_SVPWM pv);

void ClarkeCale(M_CLARKE pv); 
void ParkCale(M_PARK pv);	  
void IPARK_Cale(M_IPARK pv);  
#endif


#ifndef MOTOR_CTRL_H
#define MOTOR_CTRL_H
#include "stdint.h"
#include "stdbool.h"
typedef struct
{
    uint16_t IfFrq;       
    uint16_t IfAngle;     
    uint16_t Locktime;   
    uint8_t SpeedCalTime; 
    float IfTheta;        
    float IfCurrent;      
    int16_t SpeedRef;    
    float CurRef;        
    int32_t PosRef;     
    double RpmCmd;
    float FluxErr;
    float HfiSmoErr;
    float FocAngle;
    uint16_t FluxSpeedCnt;
    uint8_t ToZeroState;
    uint16_t UpTime;
    uint16_t DownTime;
    uint8_t CalTime;
    bool ObsSwFlag;
    bool SmoSwFlag;
    float SpdSenKp;
    float SpdSenKi;
    float SpdLessKp;
    float SpdLessKi;
} PARA;
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
extern motor_p Motor;

extern PARA Para;
void QtMotor(void);
void FocSvpwm(void);
void MotorRun(void);
void AxisDq(void);
void CurCloseloop(float IqRef, float Theta);
void MotorStop(void);
void PositionToZeroDouble(void);
void Vrsetcurrent(void);
extern const float ubeta_tab[360];
extern const float ualpha_tab[360];
#endif // MOTOR_CTRL_H
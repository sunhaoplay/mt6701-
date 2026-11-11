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
void QtMotor(void);
void FocSvpwm(void);
void MotorRun(void);

extern const float ubeta_tab[360];
extern const float ualpha_tab[360];
#endif // MOTOR_CTRL_H
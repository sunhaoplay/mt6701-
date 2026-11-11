#ifndef MOTOR_PARA_H
#define MOTOR_PARA_H

#include "arm_math.h"

#define TIM1_PERIOD /*******************/ 8400                       
#define TIM1_PERIOD_HALF /**************/ (TIM1_PERIOD / 2)          
#define PWM_FRQ /***********************/ 20000                      
#define TS /****************************/ 0.00005f                   
#define SPEED_CNT /*********************/ 2                          
#define ENCODER_FRQ /*******************/ (PWM_FRQ / SPEED_CNT) * 60 // PWM_FRQ/SPEED_CNT*60
#define SPEED_TIME /********************/ TS *SPEED_CNT
#define SPD_CAL_PERIOD /****************/ 20             
#define PIX2 /**************************/ 6.28318530718f // 2PI
#define DEG_10 /************************/ 0.1745329252f 

#define ENCODE_LINES /******************/ 1024
#define ENCODE_PULSE /******************/ (ENCODE_LINES * 4) 
#define TIM3_PULSE /********************/ ENCODE_PULSE       

#define MOTOR_P /***********************/ 10         
#define MOTOR_Ls /**********************/ 0.0000423f 
#define MOTOR_Rs /**********************/ 0.1f      

#define ADC_REFV /**********************/ 3.3f      
#define ADC_REF_VALUE /*****************/ 4095      
#define I_GAIN /************************/ 0.018315f 
// #define I_GAIN /************************/ 0.0061f     
#define VBUS_GAIN /******************* */ 0.0088653554f 

#define SART_OF_3 /*********************/ 0.57735f
#define SVPWM_KM /**********************/ (STAN_VOLTAGE_V * SART_OF_3)
#define SVPWM_KM_BACKW /****************/ 0.1443376f 
#define VBUS_S /************************/ 6.651f     
#define PI_MAX_Uq /*********************/ 6.0f       
#define PI_MAX_SPD /********************/ 10.0f     
#define M_ERR /*************************/ 11 
#define STOP /**************************/ 0  
#define SPEED_LOOP /********************/ 1  
#define CUR_LOOP /**********************/ 2  
#define POS_LOOP /**********************/ 3  
#define TO_ZERO /***********************/ 10  
#define IF_CTRL /***********************/ 4  
#define HFI_CTRL /**********************/ 5  
#define HFI_SMO /***********************/ 6  
#define HFI_FLUX /**********************/ 7 
#define IF_SMO /************************/ 8  
#define IF_FLUX /***********************/ 9  

#define QT_ANGLE /**********************/ 210 
#define QT_FRQ /************************/ 20   
#define QT_IF_MODE /********************/ 3    
#define IF_F_GRAD_TIMER /***************/ 10   
#define IF_F_GRAD_0D1HZ /***************/ 0.1f 
#define IF_CUR_MAX /********************/ 2.0f 

#define MOTOR_FRE_MIN /*****************/ 2   
#define MOTOR_FRE_MAX /*****************/ 200 

#define LPF_I_STOP_A /******************/ 0.00314159265f 
#define LPF_I_STOP_B /******************/ 0.99685840735f

#define LPF_I_RUN_A /*******************/ 0.37699f 
#define LPF_I_RUN_B /*******************/ 0.62301f

#define LPF_U_A /***********************/ 0.015465039f 
#define LPF_U_B /***********************/ 0.984534961f

#define SPD_REF_TIMER /*****************/ 30   
#define SPD_REF_OD /********************/ 5.0f 
#define SPD_REF_MAX /*******************/ 4000 

#define HFI_UIN_OFFSET /****************/ 1.0f 
#define HFI_ID_OFFSET /*****************/ 2.0f 

#define HFI_SPD_REF_MAX /***************/ 350 

#define LPF_HFI_A /*********************/ 0.011f 
#define LPF_HFI_B /*********************/ 0.989f
#define LPF_HFI_I_A /*******************/ 0.9424778f 
#define LPF_HFI_I_B /*******************/ 0.0575222f

#define LPF_SMO_A /*********************/ 0.23905722361f 
#define LPF_SMO_B /*********************/ 0.76094277639f
#define SMO_KSLIDE /********************/ 1.868f     
#define SMO_KSLF /**********************/ 0.12566f    
#define wc /****************************/ 1200 * PIX2 

#define LPF_PLL_A /*********************/ 0.09425f 
#define LPF_PLL_B /*********************/ 0.90575f

#define FLUX_KE /***********************/ 20.0f    
#define PSI_LIMIT /*********************/ 0.05f    
#define PLL_KP_DEF /********************/ 540.35f  
#define PLL_KI_DEF /********************/ 72995.6f 

#define LOW_VOLTAGE_V /*****************/ 9.0f  
#define STAN_VOLTAGE_V /****************/ 12.0f 
#define HIHG_VOLTAGE_V /****************/ 15.0f 
#define LV_PROTECT_TIME /***************/ 500   
#define OV_PROTECT_TIME /***************/ 500   

#define EN_MOS_TEMP_DETECT /************/ (1)   
#define MOS_TEMP_UP_VOL /***************/ 3.3f  
#define MOS_TEMP_UP_RES /***************/ 10.0f 
#define MOS_TEMP_OVER_RES /*************/ 0.98f 
#define RSM_MOS_TEMP_OVER_RES /*********/ 1.66f 

#define MOS_TEMP_OVER_TIME /************/ 500 // λms
#define RSM_MOS_TEMP_OVER_TIME /********/ 300 // λms
#define MOS_TEMP_TIME /*****************/ 300 // λms
#define MOS_TEMP_OVER_RES_RATIO /*******/ (float)(MOS_TEMP_OVER_RES / (MOS_TEMP_OVER_RES + MOS_TEMP_UP_RES))         
#define RSM_MOS_TEMP_OVER_RES_RATIO /***/ (float)(RSM_MOS_TEMP_OVER_RES / (RSM_MOS_TEMP_OVER_RES + MOS_TEMP_UP_RES)) 

#define MOS_TEMP_OVER_THD_ADC /*********/ (uint16_t)(MOS_TEMP_OVER_RES_RATIO * MOS_TEMP_UP_VOL * ADC_REF_VALUE / ADC_REFV)     
#define RSM_MOS_TEMP_OVER_THD_ADC /*****/ (uint16_t)(RSM_MOS_TEMP_OVER_RES_RATIO * MOS_TEMP_UP_VOL * ADC_REF_VALUE / ADC_REFV) 
#define LOCK_TIME /*********************/ 500  
#define LOCK_CUR /**********************/ 2.0f 
#define SPEED_PRO_TIME /****************/ 100  

#define OVER_CURRENT /*******************/ 2.0f 

#define RGB_Luminance /*****************/ 4 
#define VR_OR_PC /**********************/ 1 

#endif

/**
 * @file motor_para.h
 * @brief 电机、采样、控制环、观测器及保护功能的集中参数配置。
 *
 * 本文件中的宏直接决定 20 kHz FOC 的时间尺度、ADC 物理量换算、控制器限幅、
 * 无感观测器切换点及软件保护阈值。修改参数前必须确认单位和调用周期。
 */

#ifndef MOTOR_PARA_H
#define MOTOR_PARA_H

#include "arm_math.h"

/* PWM 和控制周期。TIM1 为中心对齐计数，TS=1/PWM_FRQ=50 us。 */
#define TIM1_PERIOD /*******************/ 8400
#define TIM1_PERIOD_HALF /**************/ (TIM1_PERIOD / 2)
#define PWM_FRQ /***********************/ 20000
#define TS /****************************/ 0.00005f
#define SPEED_CNT /*********************/ 2                          /**< 有感测速/速度环分频系数。 */
#define ENCODER_FRQ /*******************/ (PWM_FRQ / SPEED_CNT) * 60 /**< 每分钟测速采样次数。 */
#define SPEED_TIME /********************/ TS *SPEED_CNT              /**< 有感测速周期，单位 s。 */
#define SPD_CAL_PERIOD /****************/ 20                         /**< 无感速度环软件分频计数。 */
#define PIX2 /**************************/ 6.28318530718f              /**< 2*PI，单位 rad。 */
#define DEG_10 /************************/ 0.1745329252f               /**< 10 度对应弧度。 */

/* 增量编码器：1024 线经 AB 四倍频后每机械圈 4096 个计数。 */
#define ENCODE_LINES /******************/ 1024
#define ENCODE_PULSE /******************/ (ENCODE_LINES * 4)
#define TIM3_PULSE /********************/ ENCODE_PULSE

/* 电机等效相参数。 */
#define MOTOR_P /***********************/ 10          /**< 极对数。 */
#define MOTOR_Ls /**********************/ 0.0000423f  /**< 相电感，单位 H。 */
#define MOTOR_Rs /**********************/ 0.1f        /**< 相电阻，单位 ohm。 */

/* ADC 与模拟前端换算。 */
#define ADC_REFV /**********************/ 3.3f       /**< ADC 参考电压，单位 V。 */
#define ADC_REF_VALUE /*****************/ 4095       /**< 12 位 ADC 满量程码。 */
#define I_GAIN /************************/ 0.018315f  /**< A/码：(3.3/4095)/0.044。 */
// #define I_GAIN /************************/ 0.0061f
#define VBUS_GAIN /******************* */ 0.0088653554f /**< V/码：(3.3/4095)/(4.7/(4.7+47))。 */

/* SVPWM 归一化和 PI 输出能力。SART_OF_3 名称保留，数值实际为 1/sqrt(3)。 */
#define SART_OF_3 /*********************/ 0.57735f
#define SVPWM_KM /**********************/ (STAN_VOLTAGE_V * SART_OF_3)
#define SVPWM_KM_BACKW /****************/ 0.1443376f /**< 1/(STAN_VOLTAGE_V/sqrt(3))。 */
#define VBUS_S /************************/ 6.651f     /**< 约为标准母线线性调制能力的 96%。 */
#define PI_MAX_Ud /*********************/ 6.0f       /**< d 轴电压指令上限，单位 V。 */
#define PI_MAX_Uq /*********************/ 6.0f       /**< q 轴电压指令上限，单位 V。 */
#define PI_MAX_SPD /********************/ 10.0f      /**< 速度 PI 输出电流上限，单位 A。 */

/* Motor.CtrlMode 使用的运行模式编码。 */
#define M_ERR /*************************/ 11 /**< 故障锁定。 */
#define STOP /**************************/ 0  /**< 停机。 */
#define SPEED_LOOP /********************/ 1  /**< 编码器有感速度环。 */
#define CUR_LOOP /**********************/ 2  /**< 编码器有感电流环。 */
#define POS_LOOP /**********************/ 3  /**< 编码器有感位置环。 */
#define TO_ZERO /***********************/ 10 /**< 编码器零位校准。 */
#define IF_CTRL /***********************/ 4  /**< IF 开环角度加电流闭环。 */
#define HFI_CTRL /**********************/ 5  /**< 全程 HFI 低速无感控制。 */
#define HFI_SMO /***********************/ 6  /**< 低速 HFI、高速 SMO。 */
#define HFI_FLUX /**********************/ 7  /**< 低速 HFI、高速磁链观测。 */
#define IF_SMO /************************/ 8  /**< IF 启动后切换 SMO。 */
#define IF_FLUX /***********************/ 9  /**< IF 启动后切换磁链观测。 */

#define PARAM_IDENT /*******************/ 12 /**< Rs/Ld/Lq 参数辨识。 */

/* 参数辨识算法配置（20kHz 控制周期）。 */
#define PARAM_IDENT_RS_POINT_NUM /******/ 3U                                /**< Rs 辨识的电流点数。 */
#define PARAM_IDENT_RS_CURRENT_1 /******/ 0.3f                              /**< 第 1 个 Rs 辨识 d 轴电流点，单位 A。 */
#define PARAM_IDENT_RS_CURRENT_2 /******/ 0.6f                              /**< 第 2 个 Rs 辨识 d 轴电流点，单位 A。 */
#define PARAM_IDENT_RS_CURRENT_3 /******/ 0.9f                              /**< 第 3 个 Rs 辨识 d 轴电流点，单位 A。 */
#define PARAM_IDENT_CURRENT_RAMP_STEP /**/ 0.01f                            /**< 每个 20kHz 控制周期的电流给定变化量，单位 A。 */
#define PARAM_IDENT_INIT_COUNT /********/ ((uint16_t)(PWM_FRQ / 50U))       /**< 初始化零电压等待计数，对应 20ms。 */
#define PARAM_IDENT_RS_SETTLE_COUNT /***/ ((uint16_t)(PWM_FRQ / 10U))       /**< 每个 Rs 电流点稳定计数，对应 100ms。 */
#define PARAM_IDENT_RS_SAMPLE_COUNT /***/ ((uint16_t)(PWM_FRQ / 20U))       /**< 每个 Rs 电流点采样计数，对应 50ms。 */
#define PARAM_IDENT_AXIS_CENTER_SAMPLE_COUNT ((uint16_t)(PWM_FRQ / 20U))    /**< Ld/Lq 中心电压平均计数，对应 50ms。 */
#define PARAM_IDENT_LD_BIAS_CURRENT /***/ 0.3f                              /**< Ld 辨识 d 轴偏置电流，单位 A。 */
#define PARAM_IDENT_LD_EXCITE_VOLTAGE /**/ 0.4f                             /**< Ld 辨识 d 轴双极性激励幅值，单位 V。 */
#define PARAM_IDENT_LD_SAMPLE_COUNT /***/ 512U                              /**< Ld 辨识有效样本数。 */
#define PARAM_IDENT_LQ_BIAS_CURRENT /***/ 0.0f                              /**< Lq 辨识 q 轴偏置电流，单位 A。 */
#define PARAM_IDENT_LQ_EXCITE_VOLTAGE /**/ 0.4f                             /**< Lq 辨识 q 轴双极性激励幅值，单位 V。 */
#define PARAM_IDENT_LQ_SAMPLE_COUNT /***/ 512U                              /**< Lq 辨识有效样本数。 */

/* IF 强拖和开环启动参数。 */
#define QT_ANGLE /**********************/ 210  /**< 查表强拖目标角度索引，范围 0..359。 */
#define QT_FRQ /************************/ 20   /**< 查表角度每 QT_FRQ 个 PWM 周期步进一次。 */
#define QT_IF_MODE /********************/ 3    /**< 1 定角度；2 旋转强拖；3 IF 启动。 */
#define IF_F_GRAD_TIMER /***************/ 10   /**< IF 频率斜坡更新间隔。 */
#define IF_F_GRAD_0D1HZ /***************/ 0.1f /**< 每次斜坡更新的频率增量，单位 Hz。 */
#define IF_FRE_MAX /********************/ 100  /**< IF 电频率上限，单位 Hz。 */
#define IF_CUR_MAX /********************/ 2.0f /**< IF 阶段 q 轴电流上限，单位 A。 */
#define MOTOR_FRE_MIN /*****************/ 2    /**< 电频率下限，单位 Hz。 */
#define MOTOR_FRE_MAX /*****************/ 200  /**< 电频率上限，单位 Hz。 */

/* 一阶低通系数，统一形式 y=B*y_last+A*x。 */
#define LPF_I_STOP_A /******************/ 0.00314159265f /**< 约 10 Hz 新样本权重。 */
#define LPF_I_STOP_B /******************/ 0.99685840735f
#define LPF_I_RUN_A /*******************/ 0.37699f       /**< 约 1200 Hz 新样本权重。 */
#define LPF_I_RUN_B /*******************/ 0.62301f
#define LPF_U_A /***********************/ 0.015465039f   /**< 约 100 Hz 新样本权重。 */
#define LPF_U_B /***********************/ 0.984534961f

/* 机械速度给定斜坡。 */
#define SPD_REF_TIMER /*****************/ 30   /**< 斜坡更新计数间隔。 */
#define SPD_REF_OD /********************/ 5.0f /**< 每次更新的速度增量，单位 rpm。 */
#define SPD_REF_MAX /*******************/ 4000 /**< 机械速度给定绝对上限，单位 rpm。 */

/* 高频方波注入参数。 */
#define HFI_UIN_OFFSET /****************/ 1.0f /**< d 轴高频注入电压幅值，单位 V。 */
#define HFI_ID_OFFSET /*****************/ 2.0f /**< 初始极性辨识的 d 轴偏置电流，单位 A。 */
#define HFI_SPD_REF_MAX /***************/ 350  /**< HFI 机械速度给定绝对上限，单位 rpm。 */
#define LPF_HFI_A /*********************/ 0.011f
#define LPF_HFI_B /*********************/ 0.989f
#define LPF_HFI_I_A /*******************/ 0.9424778f
#define LPF_HFI_I_B /*******************/ 0.0575222f

/* 滑模观测器和通用 PLL 参数。 */
#define LPF_SMO_A /*********************/ 0.23905722361f /**< 反电动势低通新样本权重。 */
#define LPF_SMO_B /*********************/ 0.76094277639f
#define SMO_KSLIDE /********************/ 1.868f        /**< 滑模注入增益。 */
#define SMO_KSLF /**********************/ 0.12566f       /**< 备用滑模低通系数。 */
#define wc /****************************/ 1200 * PIX2   /**< SMO 低通截止角频率，rad/s。 */
#define LPF_PLL_A /*********************/ 0.09425f       /**< PLL 速度低通新样本权重。 */
#define LPF_PLL_B /*********************/ 0.90575f

/* 磁链观测器。 */
#define FLUX_KE /***********************/ 20.0f    /**< 泄漏积分频率参数。 */
#define PSI_LIMIT /*********************/ 0.05f    /**< 磁链积分限幅，单位 Wb。 */
#define PLL_KP_DEF /********************/ 540.35f  /**< 磁链 PLL 比例增益。 */
#define PLL_KI_DEF /********************/ 72995.6f /**< 磁链 PLL 积分增益。 */

/* 母线电压保护，计数时间按 FaultCheck() 的 1 ms 调用周期换算。 */
#define LOW_VOLTAGE_V /*****************/ 9.0f
#define STAN_VOLTAGE_V /****************/ 12.0f
#define HIHG_VOLTAGE_V /****************/ 15.0f
#define LV_PROTECT_TIME /***************/ 500
#define OV_PROTECT_TIME /***************/ 500

/* NTC 分压式 MOS 温度保护及恢复迟滞。 */
#define EN_MOS_TEMP_DETECT /************/ (1)
#define MOS_TEMP_UP_VOL /***************/ 3.3f  /**< 上拉电压，单位 V。 */
#define MOS_TEMP_UP_RES /***************/ 10.0f /**< 上拉电阻，单位 kohm。 */
#define MOS_TEMP_OVER_RES /*************/ 0.98f /**< 过温点 NTC 阻值，单位 kohm。 */
#define RSM_MOS_TEMP_OVER_RES /*********/ 1.66f /**< 恢复点 NTC 阻值，单位 kohm。 */
#define MOS_TEMP_OVER_TIME /************/ 500
#define RSM_MOS_TEMP_OVER_TIME /********/ 300
#define MOS_TEMP_TIME /*****************/ 300
#define MOS_TEMP_OVER_RES_RATIO /*******/ (float)(MOS_TEMP_OVER_RES / (MOS_TEMP_OVER_RES + MOS_TEMP_UP_RES))
#define RSM_MOS_TEMP_OVER_RES_RATIO /***/ (float)(RSM_MOS_TEMP_OVER_RES / (RSM_MOS_TEMP_OVER_RES + MOS_TEMP_UP_RES))
#define MOS_TEMP_OVER_THD_ADC /*********/ (uint16_t)(MOS_TEMP_OVER_RES_RATIO * MOS_TEMP_UP_VOL * ADC_REF_VALUE / ADC_REFV)
#define RSM_MOS_TEMP_OVER_THD_ADC /*****/ (uint16_t)(RSM_MOS_TEMP_OVER_RES_RATIO * MOS_TEMP_UP_VOL * ADC_REF_VALUE / ADC_REFV)

/* 堵转、速度和过流保护阈值。 */
#define LOCK_TIME /*********************/ 500  /**< 堵转确认时间，单位 ms。 */
#define LOCK_CUR /**********************/ 2.0f /**< 堵转 q 轴电流阈值，单位 A。 */
#define SPEED_PRO_TIME /****************/ 100  /**< 速度异常确认时间，单位 ms。 */
#define OVER_CURRENT /*******************/ 2.0f /**< 过流阈值，单位 A。 */

#define RGB_Luminance /*****************/ 4 /**< RGB LED 亮度等级，范围 0..7。 */
#define VR_OR_PC /**********************/ 1 /**< 1 使用电位器给定，0 使用 PC/VOFA 给定。 */

#endif

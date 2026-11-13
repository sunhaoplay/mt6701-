#include "encoder.h"
#include "main.h"          /* htim3, huart3, ENCODER_CPR, 全局变量 */
#include <stdio.h>
#include "tim.h"
#include "usart.h"
#include "adc_drv.h"      /* AdcFilPara.OffsetCurU */
#include "motor_ctrl.h"  /* Motor.E_theta, Motor.P */
#include "motor_para.h"  /* ENCODE_PULSE, PIX2 */
/* 外部共享变量(由 Z 相捕获中断更新) */
extern volatile uint8_t  encoder_z_found;
extern volatile uint16_t encoder_z_offset;
extern volatile uint16_t encoder_z_last_capture;
extern volatile uint32_t encoder_z_events;
#define ENCODER_CPR 4096U

/* 调试器可观察变量 */
volatile uint16_t encoder_raw_count;
volatile uint16_t encoder_position;
volatile float    encoder_angle_deg;
/*
encoder_z_found：是否已经找到第一次 Z。
encoder_z_offset：第一次 Z 对应的原始计数，作为零点。
encoder_z_last_capture：最近一次 Z 对应的原始计数。
encoder_z_events：Z 边沿触发次数。
encoder_position：换算后的单圈位置。
encoder_angle_deg：机械角度，单位为度。
*/
/* ---------- 第 1 步:原子快照 ---------- */
void Encoder_Snapshot(EncoderSnapshot_t *snap)
{
    uint32_t saved = __get_PRIMASK();
    __disable_irq();

    snap->referenced = encoder_z_found;
    snap->offset     = encoder_z_found ? encoder_z_offset : 0U;
    snap->raw        = (uint16_t)__HAL_TIM_GET_COUNTER(&htim3);
    snap->z_capture  = encoder_z_last_capture;
    snap->z_events   = encoder_z_events;

    __set_PRIMASK(saved);
}

/* ---------- 第 2 步:计算 ---------- */
void Encoder_Calc(const EncoderSnapshot_t *snap, EncoderResult_t *res)
{
    int32_t pos = (int32_t)snap->raw - (int32_t)snap->offset;
    if (pos < 0) pos += (int32_t)ENCODER_CPR;

    res->position  = (uint16_t)pos;
    res->angle_x100 = (uint32_t)pos * 36000U / ENCODER_CPR;

    /* 更新调试器可观察变量 */
    encoder_raw_count = snap->raw;
    encoder_position  = res->position;
    encoder_angle_deg = (float)pos * 360.0f / (float)ENCODER_CPR;
}

/* ---------- 第 3 步:格式化 ---------- */
int Encoder_Format(char *buf, size_t size,
                   const EncoderSnapshot_t *snap,
                   const EncoderResult_t *res)
{
        return snprintf(
        buf, size,
        "%lu.%02lu\r\n",
        (unsigned long)(res->angle_x100 / 100U),
        (unsigned long)(res->angle_x100 % 100U)); 
}
/* ---------- 入口:主循环只调这个 ---------- */
void App_EncoderReport(void)
{
    static char tx_buf[128];

    EncoderSnapshot_t snap;
    EncoderResult_t   res;

    Encoder_Snapshot(&snap);
    Encoder_Calc(&snap, &res);

    int len = Encoder_Format(tx_buf, sizeof(tx_buf), &snap, &res);
    if (len > 0 && (size_t)len < sizeof(tx_buf))
    {
        HAL_UART_Transmit(&huart3, (uint8_t *)tx_buf,
                          (uint16_t)len, 100);
    }

    HAL_Delay(50);
}
/**
 * @brief 把编码器机械位置换算为一个周期内的转子电角度。
 *
 * theta_e=CNT/ENCODE_PULSE*2PI*P。乘极对数 P 后可能跨越多个电周期，
 * while 循环将结果回绕到 (0,2PI]，供 Park 变换使用。
 */
void MotorAngleGet(void)
{
	Motor.E_theta = (float)TIM3->CNT / ENCODE_PULSE * PIX2 * Motor.P; // CNT/16384(编码器转一圈计数值)*2PI*极对数

	while (Motor.E_theta >= PIX2)
		Motor.E_theta -= PIX2;
	while (Motor.E_theta <= 0.0f)
		Motor.E_theta += PIX2;
}

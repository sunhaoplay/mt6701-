#ifndef ENCODER_H
#define ENCODER_H

#include "main.h"
#include <stdint.h>

/* 一次采样的一致快照 */
typedef struct {
    uint8_t  referenced;     /* 是否已找到 Z 零点 */
    uint16_t raw;            /* TIM3 原始计数 */
    uint16_t offset;         /* Z 零点偏移 */
    uint16_t z_capture;      /* 最近一次 Z 捕获值 */
    uint32_t z_events;       /* Z 事件累计次数 */
} EncoderSnapshot_t;

/* 计算后的结果 */
typedef struct {
    uint16_t position;       /* 相对零点位置 0~CPR-1 */
    uint32_t angle_x100;     /* 角度 ×100,如 9000 表示 90.00° */
} EncoderResult_t;

void Encoder_Snapshot(EncoderSnapshot_t *snap);
void Encoder_Calc(const EncoderSnapshot_t *snap, EncoderResult_t *res);
int  Encoder_Format(char *buf, size_t size,const EncoderSnapshot_t *snap,const EncoderResult_t *res);
void App_EncoderReport(void);   /* 主循环调用入口 */
#endif // ENCODER_H
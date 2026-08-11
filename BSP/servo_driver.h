#ifndef __SERVO_DRIVER_H
#define __SERVO_DRIVER_H

#include "stm32f1xx_hal.h"
#include "tim.h"

// S20F 180°舵机标准参数 (TIM2 CH1, PA0, 250kHz, 50Hz)
// 计数器 = 36MHz / (143+1) = 250kHz, Period = 4999 → 50Hz
#define SERVO_CCR_MIN    125    // 0°  0.5ms  (250k*0.0005)
#define SERVO_CCR_MID    375    // 90° 1.5ms  (250k*0.0015)
#define SERVO_CCR_MAX    625    // 180° 2.5ms (250k*0.0025)

// 函数声明
void Servo_Init(void);
void Servo_SetAngle(uint16_t angle);
void Servo_SetCcr(uint16_t ccr);

#endif


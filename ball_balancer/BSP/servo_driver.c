#include "servo_driver.h"

// 开启TIM2 CH1 PWM输出(PA0)，上电默认90°中位
void Servo_Init(void)
{
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, SERVO_CCR_MID);
}

// 设置舵机角度 0~180°
void Servo_SetAngle(uint16_t angle)
{
    uint16_t ccr_val;
    // 角度限幅保护
    if(angle > 180) angle = 180;
    // 线性映射 0°~180° 对应 125~625
    ccr_val = SERVO_CCR_MIN + (uint32_t)angle * (SERVO_CCR_MAX - SERVO_CCR_MIN) / 180;
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, ccr_val);
}

// 直接设置PWM比较值，带边界限制
void Servo_SetCcr(uint16_t ccr)
{
    uint16_t ccr_val = ccr;
    if(ccr_val < SERVO_CCR_MIN) ccr_val = SERVO_CCR_MIN;
    if(ccr_val > SERVO_CCR_MAX) ccr_val = SERVO_CCR_MAX;
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, ccr_val);
}
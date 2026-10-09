/**
 * @file    PID.h
 * @brief   位置式PID控制器 (微分先行 + 积分限幅 + 输出偏移)
 * @note    适用于串级控制: 位置/转向环(外环) + 速度环(内环)
 */

#ifndef __PID_H__
#define __PID_H__

#include "ti_msp_dl_config.h"
#include <stdint.h>

typedef struct {
    float Kp;           // 比例系数
    float Ki;           // 积分系数
    float Kd;           // 微分系数
    float setpoint;     // 目标值（循迹时 = 0.55）
    float integral;     // 积分累加
    float integral_max; // 积分限幅
    float prev_error;   // 上一次误差（用于微分）
    float output;       // PID 输出
} PID_TypeDef;

typedef struct {
    float Kp;           // 比例系数
    float Ki;           // 积分系数
    float err_last;     // 上一次误差（用于P项差分）
    float output;       // 累加输出值
    float out_max;      // 输出上限
    float out_min;      // 输出下限
} IncPID_TypeDef;



float Place_PID_Update(PID_TypeDef *pid, float input, float dt);

void PID_Init(PID_TypeDef *pid, float Kp, float Ki, float Kd, float setpoint);

void IncPID_Init(IncPID_TypeDef *pid, float Kp, float Ki, float out_max, float out_min);
float IncPID_Update(IncPID_TypeDef *pid, float error, float dt);

#endif /* __PID_H__ */

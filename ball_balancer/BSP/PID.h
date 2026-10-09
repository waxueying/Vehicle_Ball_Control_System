#ifndef __PID_H
#define __PID_H

#include "stm32f1xx_hal.h"

/* ==================== 增量式PID结构体 ==================== */
typedef struct
{
    float Kp;           // 比例系数
    float Ki;           // 积分系数
    float Kd;           // 微分系数

    float target;       // 目标值
    float current;      // 当前值

    float error[3];     // 误差缓存: [0]=e(k), [1]=e(k-1), [2]=e(k-2)

    float output;       // 当前输出(累计值)
    float delta_output; // 本次增量

    float output_limit; // 输出限幅(绝对值), 0=不限幅

    /* 低通滤波 */
    float filtered_val; // 滤波后的值
    float alpha;        // 滤波系数 0~1, 越小越强
} PID_TypeDef;

/* ==================== 位置式PID结构体 ==================== */
typedef struct
{
    float Kp;
    float Ki;
    float Kd;

    float target;
    float current;

    float error;        // e(k)
    float error_prev;   // e(k-1)
    float integral;     // 积分累加器

    float output;

    float output_limit;   // 输出限幅(绝对值)
    float integral_limit; // 积分限幅(绝对值)
} PosPID_TypeDef;

/* ==================== 轨迹生成器 ==================== */
typedef struct
{
    float omega;   // 自然频率 (rad/s), 3~6
    float zeta;    // 阻尼系数, 0.8~1.0
    float x_ref;   // 参考位置 (cm)
    float v_ref;   // 参考速度 (cm/s)
    float a_ref;   // 参考加速度 (cm/s²)
} TrajGen_TypeDef;

/* ==================== 通用PID函数 ==================== */
void   PID_Init        (PID_TypeDef *pid, float Kp, float Ki, float Kd,
                        float output_limit);
float  PID_Calc        (PID_TypeDef *pid, float target, float current);
void   PID_Reset       (PID_TypeDef *pid);
void   PID_SetTune     (PID_TypeDef *pid, float Kp, float Ki, float Kd);

/* 速度环专用 */
void   PID_VelocityInit(PID_TypeDef *pid, float Kp, float Ki, float Kd,
                        float output_limit, float alpha);
float  PID_VelocityCalc(PID_TypeDef *pid, float target, float raw_velocity);

/* 位置式PID */
void   PosPID_Init(PosPID_TypeDef *pid, float Kp, float Ki, float Kd,
                   float output_limit, float integral_limit);
float  PosPID_Calc(PosPID_TypeDef *pid, float target, float current);

/* ==================== 轨迹生成器函数 ==================== */
void   TrajGen_Init   (TrajGen_TypeDef *traj, float omega, float zeta,
                       float init_pos);
void   TrajGen_Update (TrajGen_TypeDef *traj, float target, float dt);

/* ==================== 模型前馈 ==================== */
float  FeedForward_Calc(float a_ref, float K_ccr);

#endif /* __PID_H */

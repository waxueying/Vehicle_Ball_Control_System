#include "PID.h"

//PID_Init(&pid_turn, Kp, Ki, Kd, 0.55f);

void PID_Init(PID_TypeDef *pid, float Kp, float Ki, float Kd, float setpoint)
{
    pid->Kp         = Kp;
    pid->Ki         = Ki;
    pid->Kd         = Kd;
    pid->setpoint   = setpoint;
    pid->integral   = 0.0f;
    pid->prev_error = 0.0f;
    pid->output     = 0.0f;
}

//推荐dt = 0.02f

float Place_PID_Update(PID_TypeDef *pid, float input, float dt)
{
    // 1. 计算误差（目标在中心 0.55）
    float error = pid->setpoint - input;   // 正=偏左, 负=偏右

    // 2. 比例项
    float P = pid->Kp * error;

    // 3. 积分项
    pid->integral += error * dt;
    float I = pid->Ki * pid->integral;

    // 4. 微分项
    float D = pid->Kd * (error - pid->prev_error) / dt;
    pid->prev_error = error;

    // 5. 合成输出
    pid->output = P + I + D;
    return pid->output;
}

/**
 * @brief  增量式PI初始化
 * @param  pid      控制器实例指针
 * @param  Kp       比例系数
 * @param  Ki       积分系数
 * @param  out_max  输出上限（如 PWM 上限 99）
 * @param  out_min  输出下限（如 PWM 下限 0）
 */
void IncPID_Init(IncPID_TypeDef *pid, float Kp, float Ki, float out_max, float out_min)
{
    pid->Kp       = Kp;
    pid->Ki       = Ki;
    pid->err_last = 0.0f;
    pid->output   = 0.0f;
    pid->out_max  = out_max;
    pid->out_min  = out_min;
}

/**
 * @brief  增量式PI更新
 * @param  pid   控制器实例指针
 * @param  error 当前误差（设定值 - 反馈值）
 * @param  dt    控制周期（秒），如 0.02
 * @retval 当前累加输出值（已限幅）
 * @note   公式: Δu = Kp*(e(k)-e(k-1)) + Ki*e(k)*dt
 */
float IncPID_Update(IncPID_TypeDef *pid, float error, float dt)
{
    pid->output += pid->Kp * (error - pid->err_last)
                 + pid->Ki * error * dt;
    pid->err_last = error;

    if (pid->output > pid->out_max) pid->output = pid->out_max;
    if (pid->output < pid->out_min) pid->output = pid->out_min;

    return pid->output;
}
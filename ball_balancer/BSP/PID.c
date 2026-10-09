#include "PID.h"

/* ================================================================ */
/*                        通用增量式 PID                             */
/* ================================================================ */

/**
 * @brief  初始化增量式PID控制器
 */
void PID_Init(PID_TypeDef *pid, float Kp, float Ki, float Kd, float output_limit)
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;

    pid->target  = 0.0f;
    pid->current = 0.0f;

    pid->error[0] = 0.0f;
    pid->error[1] = 0.0f;
    pid->error[2] = 0.0f;

    pid->output       = 0.0f;
    pid->delta_output = 0.0f;

    pid->output_limit = output_limit;

    /* 默认关闭低通滤波 (alpha=1.0 即全通) */
    pid->filtered_val = 0.0f;
    pid->alpha        = 1.0f;
}

/**
 * @brief  增量式PID计算
 *         公式: Δu = Kp*(e(k)-e(k-1)) + Ki*e(k) + Kd*(e(k)-2e(k-1)+e(k-2))
 *         输出: u(k) = u(k-1) + Δu(k)
 */
float PID_Calc(PID_TypeDef *pid, float target, float current)
{
    /* 误差移位 */
    pid->error[2] = pid->error[1];
    pid->error[1] = pid->error[0];
    pid->error[0] = target - current;

    pid->target  = target;
    pid->current = current;

    /* 增量式核心 */
    pid->delta_output = pid->Kp * (pid->error[0] - pid->error[1])
                      + pid->Ki *  pid->error[0]
                      + pid->Kd * (pid->error[0] - 2.0f * pid->error[1] + pid->error[2]);

    /* 累加 */
    pid->output += pid->delta_output;

    /* 对称限幅 */
    if (pid->output_limit > 0.0f)
    {
        if (pid->output >  pid->output_limit) pid->output =  pid->output_limit;
        if (pid->output < -pid->output_limit) pid->output = -pid->output_limit;
    }

    return pid->output;
}

/**
 * @brief  重置PID
 */
void PID_Reset(PID_TypeDef *pid)
{
    pid->error[0] = 0.0f;
    pid->error[1] = 0.0f;
    pid->error[2] = 0.0f;

    pid->output       = 0.0f;
    pid->delta_output = 0.0f;
    pid->target       = 0.0f;
    pid->current      = 0.0f;
    pid->filtered_val = 0.0f;
}

/**
 * @brief  在线调整PID参数
 */
void PID_SetTune(PID_TypeDef *pid, float Kp, float Ki, float Kd)
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
}

/* ================================================================ */
/*                      速度环专用 (内置低通滤波)                    */
/* ================================================================ */

/**
 * @brief  初始化速度环PID
 */
void PID_VelocityInit(PID_TypeDef *pid, float Kp, float Ki, float Kd,
                       float output_limit, float alpha)
{
    PID_Init(pid, Kp, Ki, Kd, output_limit);
    pid->alpha = alpha;
    if (pid->alpha > 1.0f) pid->alpha = 1.0f;
    if (pid->alpha < 0.0f) pid->alpha = 0.01f;
}

/**
 * @brief  速度环计算 (先低通滤波，再增量式PID)
 */
float PID_VelocityCalc(PID_TypeDef *pid, float target, float raw_velocity)
{
    /* 一阶低通: y(k) = α*x(k) + (1-α)*y(k-1) */
    pid->filtered_val = pid->alpha * raw_velocity
                      + (1.0f - pid->alpha) * pid->filtered_val;

    /* 滤波后送入增量式PID */
    return PID_Calc(pid, target, pid->filtered_val);
}

/* ================================================================ */
/*                      位置式 PID (位置环)                         */
/* ================================================================ */

/**
 * @brief  初始化位置式PID
 */
void PosPID_Init(PosPID_TypeDef *pid, float Kp, float Ki, float Kd,
                  float output_limit, float integral_limit)
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;

    pid->target  = 0.0f;
    pid->current = 0.0f;

    pid->error      = 0.0f;
    pid->error_prev = 0.0f;
    pid->integral   = 0.0f;
    pid->output     = 0.0f;

    pid->output_limit   = output_limit;
    pid->integral_limit = integral_limit;
}

/**
 * @brief  位置式PID计算
 *         u = Kp*e(k) + Ki*∫e + Kd*(e(k)-e(k-1))
 */
float PosPID_Calc(PosPID_TypeDef *pid, float target, float current)
{
    pid->target  = target;
    pid->current = current;
    pid->error_prev = pid->error;
    pid->error = target - current;

    /* 积分累加 + 限幅 */
    pid->integral += pid->error;
    if (pid->integral_limit > 0.0f)
    {
        if (pid->integral >  pid->integral_limit) pid->integral =  pid->integral_limit;
        if (pid->integral < -pid->integral_limit) pid->integral = -pid->integral_limit;
    }

    /* 位置式核心 */
    pid->output = pid->Kp * pid->error
                + pid->Ki * pid->integral
                + pid->Kd * (pid->error - pid->error_prev);

    /* 输出限幅 */
    if (pid->output_limit > 0.0f)
    {
        if (pid->output >  pid->output_limit) pid->output =  pid->output_limit;
        if (pid->output < -pid->output_limit) pid->output = -pid->output_limit;
    }

    return pid->output;
}

/* ================================================================ */
/*                   轨迹生成器 (前馈核心)                           */
/* ================================================================ */

/**
 * @brief  初始化轨迹生成器
 * @param  traj     : 结构体指针
 * @param  omega    : 自然频率 rad/s, 推荐 3~6
 * @param  zeta     : 阻尼比, 1.0=临界阻尼(无超调), 0.8=略快有小超调
 * @param  init_pos : 初始参考位置(cm), 应等于当前实测位置
 */
void TrajGen_Init(TrajGen_TypeDef *traj, float omega, float zeta, float init_pos)
{
    traj->omega = omega;
    traj->zeta  = zeta;
    traj->x_ref = init_pos;
    traj->v_ref = 0.0f;
    traj->a_ref = 0.0f;

    /* 范围钳位 */
    if (traj->omega < 1.0f)  traj->omega = 1.0f;
    if (traj->omega > 15.0f) traj->omega = 15.0f;
    if (traj->zeta  < 0.5f)  traj->zeta  = 0.5f;
    if (traj->zeta  > 2.0f)  traj->zeta  = 2.0f;
}

/**
 * @brief  更新轨迹生成器 (每控制周期调用)
 * @param  traj   : 结构体指针
 * @param  target : 最终目标位置 (cm)
 * @param  dt     : 控制周期 (秒), 如 0.033
 *
 * 二阶系统:  a_ref = ω²*(target - x_ref) - 2ζω*v_ref
 *           等效质量-弹簧-阻尼系统, 光滑趋近目标
 */
void TrajGen_Update(TrajGen_TypeDef *traj, float target, float dt)
{
    traj->a_ref = traj->omega * traj->omega * (target - traj->x_ref)
                - 2.0f * traj->zeta * traj->omega * traj->v_ref;

    /* 欧拉积分 */
    traj->v_ref += traj->a_ref * dt;
    traj->x_ref += traj->v_ref * dt;
}

/* ================================================================ */
/*                      模型前馈                                    */
/* ================================================================ */

/**
 * @brief  根据期望加速度计算前馈CCR偏移
 * @param  a_ref  : 期望加速度 (cm/s²), 来自轨迹生成器
 * @param  K_ccr  : 系统增益 (cm/s² per CCR)
 *                  例: 舵机偏 20CCR 产生 50cm/s² 加速度 → K_ccr = 2.5
 * @return 前馈CCR偏移量
 *
 * 由运动方程 a = K*θ (舵机CCR可代替θ) 推导:
 *   CCR_ff = a_ref / K_ccr  (已考虑符号)
 *
 * 注意: 前馈方向需实验确认, 若反了则将 K_ccr 取负号
 */
float FeedForward_Calc(float a_ref, float K_ccr)
{
    if (K_ccr < 0.01f && K_ccr > -0.01f)
        return 0.0f;   /* K_ccr≈0 时关闭前馈 */

    return a_ref / K_ccr;
}

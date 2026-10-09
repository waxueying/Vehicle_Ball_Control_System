#include "PWM.h"

/**
 * @brief  设置 PWM 初始占空比为 0
 * @note   PWM 已由 SysConfig (SYSCFG_DL_PWM1_init) 自动启动，无需手动开启
 */
void PWM_Init(void)
{
    PWM_SetCompare1(0);
    PWM_SetCompare2(0);
}

/**
 * @brief  设置 PWM 通道 0 的比较值 (PA7)
 * @param  Compare  比较值, 范围 0~99
 */
void PWM_SetCompare1(uint16_t Compare)
{
    DL_TimerG_setCaptureCompareValue(PWM1_INST, Compare, GPIO_PWM1_C0_IDX);
}

/**
 * @brief  设置 PWM 通道 1 的比较值 (PA30)
 * @param  Compare  比较值, 范围 0~99
 */
void PWM_SetCompare2(uint16_t Compare)
{
    DL_TimerG_setCaptureCompareValue(PWM1_INST, Compare, GPIO_PWM1_C1_IDX);
}
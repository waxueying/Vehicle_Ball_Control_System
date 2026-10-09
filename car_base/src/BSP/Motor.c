#include "Motor.h"
#include "PWM.h"

/*
 * 电机引脚映射 (MSPM0G3507):
 *   电机1 (右侧): PWM = PA30, 方向 = PB10(IN1) / PB9(IN2)
 *   电机2 (左侧): PWM = PA7,  方向 = PB7(IN3)  / PB6(IN4)
 * //
 * 注意: main.c 中按 电机2 = 左轮、电机1 = 右轮 使用
 */

/* 方向引脚定义 */
#define MOTOR1_PORT      GPIOB
#define MOTOR1_IN1_PIN   DL_GPIO_PIN_10   // PB10
#define MOTOR1_IN2_PIN   DL_GPIO_PIN_9    // PB9

#define MOTOR2_PORT      GPIOB
#define MOTOR2_IN3_PIN   DL_GPIO_PIN_7    // PB7
#define MOTOR2_IN4_PIN   DL_GPIO_PIN_6    // PB6

// 初始化电机：PWM 已由 SysConfig 自动启动，只需设置占空比和方向引脚
void Motor_Init(void)
{
    // 1. 设置初始占空比为 0
    PWM_SetCompare1(0);  // PA7  → 电机2
    PWM_SetCompare2(0);  // PA30 → 电机1

    // 2. 将所有方向引脚拉低（电机停止）
    DL_GPIO_clearPins(MOTOR1_PORT,
        MOTOR1_IN1_PIN | MOTOR1_IN2_PIN | MOTOR2_IN3_PIN | MOTOR2_IN4_PIN);
}

// 设置电机转速和方向
// n   : 1 或 2 号电机
// Duty: 占空比（0~99）
void Motor_SetPWM(uint8_t n, uint16_t Duty)
{
    if (n == 1)
    {
        // 电机1: PA30 PWM, PB10(IN1) / PB9(IN2)
        // 正转: IN1=0, IN2=1
        DL_GPIO_clearPins(MOTOR1_PORT, MOTOR1_IN1_PIN);
        DL_GPIO_setPins(MOTOR1_PORT, MOTOR1_IN2_PIN);
        PWM_SetCompare2(Duty);  // PA30
    }
    else if (n == 2)
    {
        // 电机2: PA7 PWM, PB7(IN3) / PB6(IN4)
        // 正转: IN3=1, IN4=0
        DL_GPIO_setPins(MOTOR2_PORT, MOTOR2_IN3_PIN);
        DL_GPIO_clearPins(MOTOR2_PORT, MOTOR2_IN4_PIN);
        PWM_SetCompare1(Duty);  // PA7
    }
}

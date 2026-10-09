#ifndef __MOTOR_H
#define __MOTOR_H

#include "ti_msp_dl_config.h"

void Motor_Init(void);                     // 初始化电机（PWM + 方向引脚）
void Motor_SetPWM(uint8_t n, uint16_t Duty); // 设置电机转速和方向

#endif

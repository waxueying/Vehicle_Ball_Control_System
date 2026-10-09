#ifndef __PWM_H
#define __PWM_H

#include "ti_msp_dl_config.h"

void PWM_Init(void);               // 设置初始占空比为0
void PWM_SetCompare1(uint16_t Compare);  // 电机2（左轮）PWM: PA7
void PWM_SetCompare2(uint16_t Compare);  // 电机1（右轮）PWM: PA30

#endif

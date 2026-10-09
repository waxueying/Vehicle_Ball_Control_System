#include "LED.h"

/*
 * LED 引脚:
 *   LED1: PA29 (共阳极, 低电平亮, 高电平灭)
 *   LED2: PB26 (共阳极, 低电平亮, 高电平灭)
 */

void LED1_ON(void)
{
    DL_GPIO_clearPins(LED1_PORT, LED1_PIN_15_PIN);  // 低电平点亮
}

void LED1_OFF(void)
{
    DL_GPIO_setPins(LED1_PORT, LED1_PIN_15_PIN);    // 高电平熄灭
}

void LED1_Toggle(void)
{
    DL_GPIO_togglePins(LED1_PORT, LED1_PIN_15_PIN);
}

void LED2_ON(void)
{
    DL_GPIO_clearPins(LED2_PORT, LED2_PIN_16_PIN);  // 低电平点亮
}

void LED2_OFF(void)
{
    DL_GPIO_setPins(LED2_PORT, LED2_PIN_16_PIN);    // 高电平熄灭
}

void LED2_Toggle(void)
{
    DL_GPIO_togglePins(LED2_PORT, LED2_PIN_16_PIN);
}

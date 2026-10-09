#include "key.h"
#include "delay.h"

/**
 * @brief  读取按键状态
 * @return 1: 按下, 0: 未按下
 * 按下是低电平
 */

uint8_t KEY1_Read(void)
{
    if (DL_GPIO_readPins(GPIOB, DL_GPIO_PIN_27) == KEY_pressed) {
        delay_ms(10);  // 消抖延迟
        if (DL_GPIO_readPins(GPIOB, DL_GPIO_PIN_27) == KEY_pressed) {
            return 1;
        }
    }
    return 0;
}

uint8_t KEY2_Read(void)
{
    if (DL_GPIO_readPins(GPIOB, DL_GPIO_PIN_23) == KEY_pressed) {
        delay_ms(10);  // 消抖延迟
        if (DL_GPIO_readPins(GPIOB, DL_GPIO_PIN_23) == KEY_pressed) {
            return 1;
        }
    }
    return 0;
}

uint8_t KEY3_Read(void)
{
    if (DL_GPIO_readPins(GPIOB, DL_GPIO_PIN_8) == KEY_pressed) {
        delay_ms(10);  // 消抖延迟
        if (DL_GPIO_readPins(GPIOB, DL_GPIO_PIN_8) == KEY_pressed) {
            return 1;
        }
    }
    return 0;
}

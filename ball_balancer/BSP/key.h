#ifndef __KEY_H__
#define __KEY_H__

#include "stm32f1xx_hal.h"

#define KEY1_PIN        GPIO_PIN_12
#define KEY2_PIN        GPIO_PIN_13
#define KEY3_PIN        GPIO_PIN_14
#define KEY_GPIO_PORT   GPIOB

void Key_Init(void);
uint8_t Key_GetState(uint8_t key_index);
void Key_ProcessAndSend(void);
void Key_AckByteHandler(uint8_t byte);
uint8_t Key_GetMode(void);
uint8_t Key_GetModeChanged(void);
void Key_ClearModeChanged(void);

#endif /* __KEY_H__ */

#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "ti_msp_dl_config.h"
#include <stdint.h>
#include "delay.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 按键号枚举（与返回值共用） */
#define KEY_pressed 0



uint8_t KEY1_Read(void);
uint8_t KEY2_Read(void);
uint8_t KEY3_Read(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_KEY_H */

#ifndef __LED_H
#define __LED_H

#include "ti_msp_dl_config.h"

/* LED: PB1, 共阳极接法, 低电平点亮 */


void LED1_ON(void);
void LED1_OFF(void);
void LED1_Toggle(void);
void LED2_ON(void);
void LED2_OFF(void);
void LED2_Toggle(void);

#endif
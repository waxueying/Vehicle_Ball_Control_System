#ifndef __BUZZER_H
#define __BUZZER_H

#include "ti_msp_dl_config.h"

void Buzzer_Init(void);
void Buzzer_ON(void);
void Buzzer_OFF(void);
void Buzzer_Toggle(void);
void Buzzer_Beep(uint16_t ms);

#endif

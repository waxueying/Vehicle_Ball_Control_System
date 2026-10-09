#ifndef __ENCODER_H
#define __ENCODER_H

#include "ti_msp_dl_config.h"

/** @brief GMR编码器: 500PPR × 1(1倍频) × 减速比30 = 15000 */
#define ENCODER_RESOLUTION  (15000)

void    Encoder_Init(void);
int32_t Encoder_Get(uint8_t n);

#endif

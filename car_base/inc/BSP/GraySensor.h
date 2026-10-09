/**
 * @file    GraySensor.h
 * @brief   8路红外灰度循迹传感器驱动 (MSPM0 DriverLib)
 * @note    白底黑线：白色(反射强)→读到1，黑线(反射弱)→读到0
 *
 *          传感器排列（俯视，车头向前）：
 *           D8(最左) D7     D6     D5     D4     D3     D2     D1(最右)
 *           L4       L3     L2     L1     R1     R2     R3     R4
 *           PB19     PB17   PA16   PA14   PB20   PB25   PA27   PB0
 *
 *          GPIO引脚由 SysConfig 自动生成，无需手动修改
 */

#ifndef __GRAYSENSOR_H__
#define __GRAYSENSOR_H__

#include "ti_msp_dl_config.h"
#include <stdint.h>

/*==================================================================*/
/*  传感器数量                                                        */
/*==================================================================*/
#define GRAY_CHANNEL_NUM   8

/*==================================================================*/
/*  硬件引脚定义（由 SysConfig 生成, 从 ti_msp_dl_config.h 引入）       */
/*==================================================================*/
/* D1(最右) = GraySensor_R4: PB0   D2 = GraySensor_R3: PA27 */
/* D3       = GraySensor_R2: PB25  D4 = GraySensor_R1: PB20 */
/* D5       = GraySensor_L1: PA14  D6 = GraySensor_L2: PA16 */
/* D7       = GraySensor_L3: PB17  D8(最左) = GraySensor_L4: PB19 */

#define GRAY_PORT_D1        GraySensor_R4_PORT
#define GRAY_PORT_D2        GraySensor_R3_PORT
#define GRAY_PORT_D3        GraySensor_R2_PORT
#define GRAY_PORT_D4        GraySensor_R1_PORT
#define GRAY_PORT_D5        GraySensor_L1_PORT
#define GRAY_PORT_D6        GraySensor_L2_PORT
#define GRAY_PORT_D7        GraySensor_L3_PORT
#define GRAY_PORT_D8        GraySensor_L4_PORT

#define GRAY_PIN_D1         GraySensor_R4_PIN_17_PIN
#define GRAY_PIN_D2         GraySensor_R3_PIN_23_PIN
#define GRAY_PIN_D3         GraySensor_R2_PIN_22_PIN
#define GRAY_PIN_D4         GraySensor_R1_PIN_10_PIN
#define GRAY_PIN_D5         GraySensor_L1_PIN_9_PIN
#define GRAY_PIN_D6         GraySensor_L2_PIN_8_PIN
#define GRAY_PIN_D7         GraySensor_L3_PIN_7_PIN
#define GRAY_PIN_D8         GraySensor_L4_PIN_6_PIN

/*==================================================================*/
/*  兼容宏定义（D1~D8 对应读取指定通道）                                  */
/*==================================================================*/
#define D1   GraySensor_Read(1)    /* 最右 */
#define D2   GraySensor_Read(2)
#define D3   GraySensor_Read(3)
#define D4   GraySensor_Read(4)
#define D5   GraySensor_Read(5)
#define D6   GraySensor_Read(6)
#define D7   GraySensor_Read(7)
#define D8   GraySensor_Read(8)    /* 最左 */

/*==================================================================*/
/*  函数声明                                                          */
/*==================================================================*/
uint8_t GraySensor_Read(uint8_t channel);          /* channel: 1~8, 返回1=白底/0=黑线 */
uint8_t GraySensor_ReadAll(void);                  /* 返回8位数据: bit0=D1 ... bit7=D8 */
int8_t  GraySensor_GetPosition(void);              /* 获取线位置偏差（待适配8路） */
void xunji_shuai_xin(void);

#endif /* __GRAYSENSOR_H__ */

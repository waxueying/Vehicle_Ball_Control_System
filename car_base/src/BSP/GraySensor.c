/**
 * @file    GraySensor.c
 * @brief   8路红外灰度循迹传感器驱动 (MSPM0 DriverLib)
 * @note    基于参考工程 sensor.c 移植适配，使用 MSPM0 DriverLib 替代 HAL 库
 *
 *          差异对照：
 *          ┌──────────────────────┬──────────────────────────┐
 *          │ HAL 版               │ MSPM0 DriverLib 版       │
 *          ├──────────────────────┼──────────────────────────┤
 *          │ __HAL_RCC_GPIOx_...  │ DL_GPIO_enablePower()    │
 *          │ HAL_GPIO_Init()      │ DL_GPIO_initDigitalIn... │
 *          │ HAL_GPIO_ReadPin()   │ DL_GPIO_readPins()       │
 *          │ GPIO_PIN_SET         │ (返回值 & pin) != 0      │
 *          └──────────────────────┴──────────────────────────┘
 *
 *          工作原理：
 *          红外发射管发射红外线，接收管检测反射光强度。
 *          白色表面反射强 → 三极管导通 → IO口读到高电平(1)
 *          黑色表面反射弱 → 三极管截止 → IO口读到低电平(0)
 *
 *          传感器排列（俯视，车头向前）：
 *          D8(最左) D7     D6     D5     D4     D3     D2     D1(最右)
 *          L4       L3     L2     L1     R1     R2     R3     R4
 *          PB19     PB17   PA16   PA14   PB20   PB25   PA27   PB0
 */

#include "GraySensor.h"

float xunji=0;
float last_xunji=0;

/*==================================================================*/
/*  引脚配置结构体（端口 + 引脚号）                                      */
/*==================================================================*/
typedef struct {
    GPIO_Regs *port;
    uint32_t   pin;
} GraySensor_PinConfig_t;

/*==================================================================*/
/*  用数组来存储引脚号查找表（channel → GPIO_Pin 映射）                   */
/*  channel 1=D1(最右,R4) ~ 8=D8(最左,L4)                              */
/*==================================================================*/
static const GraySensor_PinConfig_t Gray_PinTable[GRAY_CHANNEL_NUM] =
{
    {GRAY_PORT_D1, GRAY_PIN_D1},  /* D1: R4  PB0  最右 */
    {GRAY_PORT_D2, GRAY_PIN_D2},  /* D2: R3  PA27 */
    {GRAY_PORT_D3, GRAY_PIN_D3},  /* D3: R2  PB25 */
    {GRAY_PORT_D4, GRAY_PIN_D4},  /* D4: R1  PB20 */
    {GRAY_PORT_D5, GRAY_PIN_D5},  /* D5: L1  PA14 */
    {GRAY_PORT_D6, GRAY_PIN_D6},  /* D6: L2  PA16 */
    {GRAY_PORT_D7, GRAY_PIN_D7},  /* D7: L3  PB17 */
    {GRAY_PORT_D8, GRAY_PIN_D8},  /* D8: L4  PB19 最左 */
};

/*==================================================================*/
/*  初始化已由 SysConfig (SYSCFG_DL_GPIO_init) 自动完成，无需手动调用   */
/*==================================================================*/

/*==================================================================*/
/*  读取指定通道的数字值                                               */
/*  channel: 1=D1(最右) ~ 8=D8(最左)                                  */
/*  返回:    1 = 白底（未压线），0 = 黑线（压线）                       */
/*==================================================================*/
uint8_t GraySensor_Read(uint8_t channel)
{
    if (channel < 1 || channel > GRAY_CHANNEL_NUM)
        return 0;

    uint32_t level = DL_GPIO_readPins(Gray_PinTable[channel - 1].port,
                                       Gray_PinTable[channel - 1].pin);
    return (level != 0) ? 1 : 0;
}

/*==================================================================*/
/*  一次性读取所有8通道，返回组合字节                                    */
/*  返回:  bit0=D1, bit1=D2, ..., bit7=D8                              */
/*         值为1表示白底，0表示黑线                                     */
/*==================================================================*/
uint8_t GraySensor_ReadAll(void)
{
    uint8_t value = 0;
    uint8_t i;

    for (i = 0; i < GRAY_CHANNEL_NUM; i++)
    {
        if (DL_GPIO_readPins(Gray_PinTable[i].port, Gray_PinTable[i].pin) != 0)
        {
            value |= (1 << i);
        }
    }

    return value;
}

/*==================================================================*/
/*  获取黑线位置偏差值（用于PID循迹控制）  ← 待适配8路                    */
/*==================================================================*/
int8_t GraySensor_GetPosition(void)
{
    uint8_t  data = GraySensor_ReadAll();
    uint8_t  on_line_count = 0;      /* 压线传感器数量 */
    uint16_t weighted_sum  = 0;      /* 加权位置和 */
    uint8_t  i;

    /* 统计压线(读到0)的传感器 */
    for (i = 0; i < GRAY_CHANNEL_NUM; i++)
    {
        if ((data & (1 << i)) == 0)   /* 该位为0表示压到黑线 */
        {
            on_line_count++;
            weighted_sum += (i + 1);  /* 位置值1~8 */
        }
    }

    /* 全部压线 → 到达路口/十字/停车线 */
    if (on_line_count >= GRAY_CHANNEL_NUM)
        return 99;

    /* 全部出线 → 丢失轨迹 */
    if (on_line_count == 0)
        return -99;

    /* 计算加权平均位置，以中间为中心做偏差 */
    float avg_position = (float)weighted_sum / on_line_count;
    return (int8_t)(avg_position - 4.5f);  /* 8路中心 = (1+8)/2 = 4.5 */
}

/*==================================================================*/
/*  xunji_shuai_xin - 循迹刷新（4路版，待适配8路）                        */
/*==================================================================*/
void xunji_shuai_xin()
{
    float sum = 0;
    uint8_t d8=0, d7=0, d6=0, d5=0, d4=0, d3=0, d2=0, d1=0;
    
    if(D8==0) {d8=1; sum++;} else d8=0;
    if(D7==0) {d7=1; sum++;} else d7=0;
    if(D6==0) {d6=1; sum++;} else d6=0;
    if(D5==0) {d5=1; sum++;} else d5=0;
    if(D4==0) {d4=1; sum++;} else d4=0;
    if(D3==0) {d3=1; sum++;} else d3=0;
    if(D2==0) {d2=1; sum++;} else d2=0;
    if(D1==0) {d1=1; sum++;} else d1=0;
    
    if(sum > 0)
        xunji = (d8*1.000f + d7*0.857f + d6*0.714f + d5*0.571f
               + d4*0.429f + d3*0.286f + d2*0.143f) / sum;
    else
        xunji = last_xunji;
    
    last_xunji = xunji;
}
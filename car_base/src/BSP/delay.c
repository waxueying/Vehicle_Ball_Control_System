#include "delay.h"

/**
 * @brief  毫秒级延时
 * @param  ms  延时毫秒数
 * @note   基于 CPUCLK_FREQ 宏（由 SysConfig 自动生成），自动适配主频
 */
void delay_ms(uint32_t ms)
{
    /* delay_cycles 是 MSPM0 DriverLib 自带的内联函数 */
    delay_cycles(ms * (CPUCLK_FREQ / 1000));
}

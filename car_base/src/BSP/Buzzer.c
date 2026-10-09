// #include "Buzzer.h"

// /* 有源蜂鸣器: PB0, 低电平响 */

// void Buzzer_Init(void)
// {
//     /* GPIO 已在 SYSCFG_DL_GPIO_init() 中配置 */
//     Buzzer_OFF();  /* 默认关闭 */
// }

// void Buzzer_OFF(void)
// {
//     DL_GPIO_setPins(Buzzer_PORT, Buzzer_PIN_17_PIN);    /* 高电平灭 */
// }

// void Buzzer_ON(void)
// {
//     DL_GPIO_clearPins(Buzzer_PORT, Buzzer_PIN_17_PIN);  /* 低电平响 */
// }

// void Buzzer_Toggle(void)
// {
//     DL_GPIO_togglePins(Buzzer_PORT, Buzzer_PIN_17_PIN);
// }

// /**
//   * @brief  蜂鸣器鸣叫指定毫秒（阻塞方式）
//   * @param  ms: 鸣叫时长，单位毫秒
//   */
// void Buzzer_Beep(uint16_t ms)
// {
//     Buzzer_ON();
//     DL_Common_delayCycles(CPUCLK_FREQ / 1000 * ms);
//     Buzzer_OFF();
// }

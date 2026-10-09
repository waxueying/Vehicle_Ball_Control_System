#ifndef __UART_PROTOCOL_H
#define __UART_PROTOCOL_H

#include <stdint.h>

/* 协议常量 */
#define FRAME_LEN      12
#define FRAME_HEAD0    0xAA
#define FRAME_HEAD1    0x55
#define FRAME_TAIL0    0x0D
#define FRAME_TAIL1    0x0A

/* 供外部读取的解析结果 */
extern float g_offset_cm;
extern float g_velocity;
extern uint8_t g_frame_valid;

/* 原始帧数据 (调试用, 收满12字节后更新) */
extern uint8_t g_raw_buf[12];
extern volatile uint8_t g_raw_ready;

/* 数据新鲜度标志: 收到校验通过的完整帧后置1, PID消费后清零 */
extern volatile uint8_t g_data_fresh;

/**
 * @brief  阻塞式接收并解析一帧数据（保留兼容）
 */
void UART_RecvFrame(void);

/**
 * @brief  初始化非阻塞中断接收
 * @note   调用后无需在主循环或中断中手动收帧，
 *         每收到一帧自动更新 g_offset_cm / g_velocity
 */
void UART_RecvFrame_Init(void);

/**
 * @brief  RX 字节处理 (由 HAL_UART_RxCpltCallback 调用)
 * @note   不直接调用，放在 main.c 的 HAL_UART_RxCpltCallback 中
 */
void UART_RxByteHandler(uint8_t byte);

/**
 * @brief  IDLE 中断处理 (由 USART2_IRQHandler 调用)
 */
void UART_IdleHandler(void);

#endif

#include "uart_protocol.h"
#include "usart.h"
#include "key.h"
#include <string.h>

float   g_offset_cm    = 0.0f;
float   g_velocity     = 0.0f;
uint8_t g_frame_valid  = 0;

uint8_t          g_raw_buf[12];
volatile uint8_t g_raw_ready  = 0;
volatile uint8_t g_data_fresh = 0;  /* PID数据新鲜度 */

/* ========== 阻塞式接收 (保留兼容) ========== */

static uint8_t uart_rx_byte(void)
{
    while (!(USART2->SR & USART_SR_RXNE));
    return (uint8_t)(USART2->DR & 0xFF);
}

void UART_RecvFrame(void)
{
    uint8_t buf[FRAME_LEN];

    while (uart_rx_byte() != FRAME_HEAD0);
    while (uart_rx_byte() != FRAME_HEAD1);

    buf[0] = FRAME_HEAD0;
    buf[1] = FRAME_HEAD1;

    for (int i = 2; i < FRAME_LEN; i++)
    {
        buf[i] = uart_rx_byte();
    }

    if (buf[10] != FRAME_TAIL0 || buf[11] != FRAME_TAIL1)
    {
        g_frame_valid = 0;
        return;
    }

    memcpy(&g_offset_cm, &buf[2], 4);
    memcpy(&g_velocity,  &buf[6], 4);
    g_frame_valid = 1;
}

/* ========== 非阻塞中断接收 ========== */

typedef enum {
    STATE_HEAD0,
    STATE_HEAD1,
    STATE_DATA
} frame_state_t;

static frame_state_t rx_state = STATE_HEAD0;
static uint8_t rx_buf[FRAME_LEN];
static uint8_t rx_idx;

void UART_RecvFrame_Init(void)
{
    rx_state = STATE_HEAD0;
    rx_idx   = 0;

    /* 使能 USART2 NVIC 中断 */
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(USART2_IRQn);

    /* 使能 IDLE 中断 (总线空闲时触发) */
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);

    /* 注意: HAL_UART_Receive_IT 由调用方 (main.c) 使用独立缓冲发起 */
}

/**
 * @brief  每收到一个字节, 状态机解析帧
 * @note   由 HAL_UART_RxCpltCallback 中调用
 */
void UART_RxByteHandler(uint8_t byte)
{
    Key_AckByteHandler(byte);

    switch (rx_state)
    {
    case STATE_HEAD0:
        if (byte == FRAME_HEAD0)
        {
            rx_buf[0] = byte;
            rx_idx = 1;
            rx_state = STATE_HEAD1;
        }
        break;

    case STATE_HEAD1:
        if (byte == FRAME_HEAD1)
        {
            rx_buf[1] = byte;
            rx_idx = 2;
            rx_state = STATE_DATA;
        }
        else
        {
            rx_state = STATE_HEAD0;
        }
        break;

    case STATE_DATA:
        rx_buf[rx_idx++] = byte;
        if (rx_idx >= FRAME_LEN)
        {
            /* 保存原始帧供调试显示 */
            memcpy(g_raw_buf, rx_buf, 12);
            g_raw_ready = 1;

            if (rx_buf[10] == FRAME_TAIL0 && rx_buf[11] == FRAME_TAIL1)
            {
                memcpy(&g_offset_cm, &rx_buf[2], 4);
                memcpy(&g_velocity,  &rx_buf[6], 4);
                g_frame_valid = 1;
                g_data_fresh  = 1;  /* 新数据到达 */
            }
            else
            {
                g_frame_valid = 0;
            }
            rx_state = STATE_HEAD0;
        }
        break;
    }
}

/**
 * @brief  IDLE 中断处理 (总线空闲时重置状态机)
 */
void UART_IdleHandler(void)
{
    if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE))
    {
        __HAL_UART_CLEAR_IDLEFLAG(&huart2);
    }
    rx_state = STATE_HEAD0;
}

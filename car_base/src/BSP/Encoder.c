#include "Encoder.h"

/*
 * 编码器引脚 (GPIO 上升沿中断 + 判向 B 相电平):
 *   编码器1 (电机1): A=Motor1A=PB12, B=Motor1B=PB13
 *   编码器2 (电机2): A=Motor2A=PB4,  B=Motor2B=PB5
 *
 * 原理: A 相上升沿中断 → 读 B 相电平判向 → 单相计数（1 倍频）
 */

static volatile int32_t g_Encoder1_Count;
static volatile int32_t g_Encoder2_Count;

void Encoder_Init(void)
{
    g_Encoder1_Count = 0;
    g_Encoder2_Count = 0;
    NVIC_EnableIRQ(GPIO_MULTIPLE_GPIOB_INT_IRQN);  /* 提前使能编码器中断 */
}

void GROUP1_IRQHandler(void)
{
    uint32_t mask = Motor1A_PIN_20_PIN | Motor1B_PIN_21_PIN |
                    Motor2A_PIN_18_PIN | Motor2B_PIN_19_PIN;
    uint32_t status = DL_GPIO_getRawInterruptStatus(GPIOB, mask);

    /* --- 编码器1: A=PB12, 1倍频只判 A↑ --- */
    if (status & Motor1A_PIN_20_PIN) {          /* A↑ */
        if (DL_GPIO_readPins(GPIOB, Motor1B_PIN_21_PIN))
            g_Encoder1_Count++; else g_Encoder1_Count--;
    }
    /* B 相不计数 */

    /* --- 编码器2: A=PB4, 1倍频只判 A↑ --- */
    if (status & Motor2A_PIN_18_PIN) {          /* A↑ */
        if (DL_GPIO_readPins(GPIOB, Motor2B_PIN_19_PIN))
            g_Encoder2_Count++; else g_Encoder2_Count--;
    }
    /* B 相不计数 */

    DL_GPIO_clearInterruptStatus(GPIOB, status);
}

int32_t Encoder_Get(uint8_t n)
{
    int32_t temp;
    if (n == 1) {
        NVIC_DisableIRQ(GPIO_MULTIPLE_GPIOB_INT_IRQN);
        temp = g_Encoder1_Count; g_Encoder1_Count = 0;
        NVIC_EnableIRQ(GPIO_MULTIPLE_GPIOB_INT_IRQN);
        return temp;
    }
    if (n == 2) {
        NVIC_DisableIRQ(GPIO_MULTIPLE_GPIOB_INT_IRQN);
        temp = g_Encoder2_Count; g_Encoder2_Count = 0;
        NVIC_EnableIRQ(GPIO_MULTIPLE_GPIOB_INT_IRQN);
        return temp;
    }
    return 0;
}

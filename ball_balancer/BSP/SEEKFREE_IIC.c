/*****************************************************************************
 * 软件 I2C 驱动（PB6=SCL, PB7=SDA）
 * 开漏输出 + 内部上拉，延时使用 volatile 防优化
 *****************************************************************************/

#include "SEEKFREE_IIC.h"

#define SCL_H()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET)
#define SCL_L()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET)
#define SDA_H()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET)
#define SDA_L()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET)
#define SDA_RD() HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7)

__attribute__((noinline))
static void i2c_delay(void)
{
    volatile uint32_t n = 150;
    while (n--) { __NOP(); }
}

__attribute__((noinline))
void bsp_delay_ms(uint32_t ms)
{
    while (ms--) {
        volatile uint32_t n = 5333;
        while (n--) { __NOP(); }
    }
}

__attribute__((noinline))
static void i2c_start(void)
{
    SDA_H(); i2c_delay();
    SCL_H(); i2c_delay();
    SDA_L(); i2c_delay();
    SCL_L(); i2c_delay();
}

__attribute__((noinline))
static void i2c_stop(void)
{
    SDA_L(); i2c_delay();
    SCL_H(); i2c_delay();
    SDA_H(); i2c_delay();
}

__attribute__((noinline))
static void i2c_send_byte(uint8_t byte)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        if (byte & 0x80) SDA_H(); else SDA_L();
        byte <<= 1;
        i2c_delay();
        SCL_H(); i2c_delay();
        SCL_L(); i2c_delay();
    }
    /* 第9位：释放 SDA */
    SDA_H(); i2c_delay();
    SCL_H(); i2c_delay();
    SCL_L(); i2c_delay();
}

__attribute__((noinline))
static uint8_t i2c_recv_byte(uint8_t ack)
{
    uint8_t i, byte = 0;
    SDA_H(); i2c_delay();
    for (i = 0; i < 8; i++)
    {
        SCL_H(); i2c_delay();
        byte <<= 1;
        if (SDA_RD()) byte |= 1;
        SCL_L(); i2c_delay();
    }
    if (ack) SDA_L(); else SDA_H();
    i2c_delay();
    SCL_H(); i2c_delay();
    SCL_L(); i2c_delay();
    SDA_H();
    return byte;
}

/* ── 公开接口 ── */

void simiic_write_reg(uint8_t dev, uint8_t reg, uint8_t val)
{
    i2c_start();
    i2c_send_byte(dev << 1);
    i2c_send_byte(reg);
    i2c_send_byte(val);
    i2c_stop();
}

uint8_t simiic_read_reg(uint8_t dev, uint8_t reg, IIC_type type)
{
    uint8_t val;
    i2c_start();
    i2c_send_byte(dev << 1);
    i2c_send_byte(reg);
    if (type == SCCB) i2c_stop();

    i2c_start();
    i2c_send_byte((dev << 1) | 1);
    val = i2c_recv_byte(0);
    i2c_stop();
    return val;
}

void simiic_read_regs(uint8_t dev, uint8_t reg, uint8_t *buf, uint8_t len, IIC_type type)
{
    i2c_start();
    i2c_send_byte(dev << 1);
    i2c_send_byte(reg);
    if (type == SCCB) i2c_stop();

    i2c_start();
    i2c_send_byte((dev << 1) | 1);
    while (--len)
        *buf++ = i2c_recv_byte(1);
    *buf = i2c_recv_byte(0);
    i2c_stop();
}



#include "BlueSerial.h"

/*
 * 蓝牙串口引脚 (MSPM0G3507):
 *   UART3, PA25(RX), PA26(TX), 9600bps
 *
 * SysConfig 已在 SYSCFG_DL_init() 中调用 SYSCFG_DL_UART_0_init()
 */

/* 环形缓冲区 */
static volatile uint8_t BlueSerial_Buffer[BLUE_SERIAL_BUFFER_SIZE];
static volatile uint16_t BlueSerial_PutIndex = 0;
static volatile uint16_t BlueSerial_GetIndex = 0;

/* 解析用的字符串数组 */
static char BlueSerial_String[100];
static char BlueSerial_StringArray[6][20];

/*============ 初始化 ============*/
void BlueSerial_Init(void)
{
    /* UART 已由 SysConfig 初始化并启用, 开启中断接收 */
    DL_UART_Main_enableInterrupt(UART_0_INST, DL_UART_INTERRUPT_RX);
}

/*============ 发送 ============*/
void BlueSerial_SendByte(uint8_t Byte)
{
    DL_UART_Main_transmitDataBlocking(UART_0_INST, Byte);
}

void BlueSerial_SendArray(uint8_t *Array, uint16_t Length)
{
    for (uint16_t i = 0; i < Length; i++)
        DL_UART_Main_transmitDataBlocking(UART_0_INST, Array[i]);
}

void BlueSerial_SendString(char *String)
{
    while (*String)
        DL_UART_Main_transmitDataBlocking(UART_0_INST, *String++);
}

void BlueSerial_Printf(char *format, ...)
{
    char String[100];
    va_list arg;
    va_start(arg, format);
    vsnprintf(String, sizeof(String), format, arg);
    va_end(arg);
    BlueSerial_SendString(String);
}

/*============ 环形缓冲区 (中断回调调用) ============*/
uint8_t BlueSerial_Put(uint8_t Byte)
{
    if ((BlueSerial_PutIndex + 1) % BLUE_SERIAL_BUFFER_SIZE == BlueSerial_GetIndex)
        return 1;   /* 缓冲区满 */
    BlueSerial_Buffer[BlueSerial_PutIndex] = Byte;
    BlueSerial_PutIndex = (BlueSerial_PutIndex + 1) % BLUE_SERIAL_BUFFER_SIZE;
    return 0;
}

uint8_t BlueSerial_Get(uint8_t *Byte)
{
    if (BlueSerial_GetIndex == BlueSerial_PutIndex)
    {
        *Byte = 0;
        return 1;   /* 缓冲区空 */
    }
    *Byte = BlueSerial_Buffer[BlueSerial_GetIndex];
    BlueSerial_GetIndex = (BlueSerial_GetIndex + 1) % BLUE_SERIAL_BUFFER_SIZE;
    return 0;
}

uint16_t BlueSerial_Length(void)
{
    return (BlueSerial_PutIndex + BLUE_SERIAL_BUFFER_SIZE - BlueSerial_GetIndex)
           % BLUE_SERIAL_BUFFER_SIZE;
}

uint8_t BlueSerial_Read(uint16_t Index)
{
    return BlueSerial_Buffer[(BlueSerial_GetIndex + Index) % BLUE_SERIAL_BUFFER_SIZE];
}

void BlueSerial_ClearBuffer(void)
{
    for (uint16_t i = 0; i < BLUE_SERIAL_BUFFER_SIZE; i++)
        BlueSerial_Buffer[i] = 0;
    BlueSerial_PutIndex = 0;
    BlueSerial_GetIndex = 0;
}

/*============ 帧解析 ============*/
uint8_t BlueSerial_ReceiveFlag(void)
{
    uint8_t Flag = 0;
    uint16_t Length = BlueSerial_Length();
    for (uint16_t i = 0; i < Length; i++)
    {
        if (BlueSerial_Read(i) == '[')
            Flag = 1;
        else if (BlueSerial_Read(i) == ']' && Flag == 1)
            return 1;
    }
    return 0;
}

void BlueSerial_Receive(void)
{
    uint16_t p = 0, k = 0;
    uint8_t Flag = 0;
    uint16_t Length = BlueSerial_Length();

    for (uint16_t i = 0; i < Length; i++)
    {
        uint8_t Byte;
        BlueSerial_Get(&Byte);
        if (Byte == '[')
        {
            Flag = 1;
            p = 0;
        }
        else if (Byte == ']' && Flag == 1)
        {
            BlueSerial_String[p] = '\0';
            break;
        }
        else if (Flag == 1)
        {
            BlueSerial_String[p] = Byte;
            p++;
        }
    }

    p = 0;
    k = 0;
    for (uint16_t i = 0; BlueSerial_String[i] != '\0'; i++)
    {
        if (BlueSerial_String[i] == ',')
        {
            BlueSerial_StringArray[p][k] = '\0';
            p++;
            k = 0;
        }
        else
        {
            BlueSerial_StringArray[p][k] = BlueSerial_String[i];
            k++;
        }
    }
    BlueSerial_StringArray[p][k] = '\0';
}

/*============ UART3 中断服务 ============*/
void UART3_IRQHandler(void)
{
    switch (DL_UART_Main_getPendingInterrupt(UART_0_INST))
    {
    case DL_UART_IIDX_RX:
        BlueSerial_Put(DL_UART_Main_receiveDataBlocking(UART_0_INST));
        break;
    default:
        break;
    }
}

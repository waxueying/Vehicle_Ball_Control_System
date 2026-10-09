#ifndef __BLUESERIAL_H
#define __BLUESERIAL_H

#include "ti_msp_dl_config.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define BLUE_SERIAL_BUFFER_SIZE  512

void    BlueSerial_Init(void);
void    BlueSerial_SendByte(uint8_t Byte);
void    BlueSerial_SendArray(uint8_t *Array, uint16_t Length);
void    BlueSerial_SendString(char *String);
void    BlueSerial_Printf(char *format, ...);
uint8_t BlueSerial_Put(uint8_t Byte);
uint8_t BlueSerial_Get(uint8_t *Byte);
uint16_t BlueSerial_Length(void);
uint8_t BlueSerial_Read(uint16_t Index);
void    BlueSerial_ClearBuffer(void);
uint8_t BlueSerial_ReceiveFlag(void);
void    BlueSerial_Receive(void);

#endif

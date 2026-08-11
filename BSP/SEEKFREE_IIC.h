/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2018,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：179029047
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		模拟IIC
 * @company	   		成都逐飞科技有限公司
 * @author     		逐飞科技(QQ3184284598)
 * @version    		查看common.h内VERSION宏定义
 * @Software 		IAR 7.8 or MDK 5.24a
 * @Target core		LPC54606J512BD100
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2018-05-24
 * @note		
					接线定义：
					------------------------------------ 
						SCL                 查看SEEKFREE_IIC文件内的SEEKFREE_SCL宏定义
						SDA                 查看SEEKFREE_IIC文件内的SEEKFREE_SDA宏定义
					------------------------------------ 
 ********************************************************************************************************************/



#ifndef _SEEKFREE_IIC_H
#define _SEEKFREE_IIC_H

#include "stm32f1xx_hal.h"

typedef enum { SIMIIC, SCCB } IIC_type;

/* 独立于 SysTick 的毫秒延时（任何地方都可以用） */
void bsp_delay_ms(uint32_t ms);

/* I2C 接口：与逐飞原名一致，无需修改 ICM20602 驱动 */
void     simiic_write_reg(uint8_t dev, uint8_t reg, uint8_t val);
uint8_t  simiic_read_reg (uint8_t dev, uint8_t reg, IIC_type type);
void     simiic_read_regs(uint8_t dev, uint8_t reg, uint8_t *buf, uint8_t len, IIC_type type);

#endif


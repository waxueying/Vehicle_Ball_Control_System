/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X
#define CONFIG_MSPM0G3507

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)


#define GPIO_HFXT_PORT                                                     GPIOA
#define GPIO_HFXIN_PIN                                             DL_GPIO_PIN_5
#define GPIO_HFXIN_IOMUX                                         (IOMUX_PINCM10)
#define GPIO_HFXOUT_PIN                                            DL_GPIO_PIN_6
#define GPIO_HFXOUT_IOMUX                                        (IOMUX_PINCM11)
#define GPIO_LFXT_PORT                                                     GPIOA
#define GPIO_LFXIN_PIN                                             DL_GPIO_PIN_3
#define GPIO_LFXIN_IOMUX                                          (IOMUX_PINCM8)
#define GPIO_LFXOUT_PIN                                            DL_GPIO_PIN_4
#define GPIO_LFXOUT_IOMUX                                         (IOMUX_PINCM9)
#define GPIO_ROSC_PORT                                                     GPIOA
#define GPIO_ROSC_PIN                                              DL_GPIO_PIN_2
#define GPIO_ROSC_IOMUX                                           (IOMUX_PINCM7)
#define CPUCLK_FREQ                                                     80000000
/* Defines for SYSPLL_ERR_01 Workaround */
/* Represent 1.000 as 1000 */
#define FLOAT_TO_INT_SCALE                                               (1000U)
#define FCC_EXPECTED_RATIO                                                  2000
#define FCC_UPPER_BOUND                       (FCC_EXPECTED_RATIO * (1 + 0.003))
#define FCC_LOWER_BOUND                       (FCC_EXPECTED_RATIO * (1 - 0.003))

bool SYSCFG_DL_SYSCTL_SYSPLL_init(void);


/* Defines for PWM1 */
#define PWM1_INST                                                          TIMG8
#define PWM1_INST_IRQHandler                                    TIMG8_IRQHandler
#define PWM1_INST_INT_IRQN                                      (TIMG8_INT_IRQn)
#define PWM1_INST_CLK_FREQ                                               2000000
/* GPIO defines for channel 0 */
#define GPIO_PWM1_C0_PORT                                                  GPIOA
#define GPIO_PWM1_C0_PIN                                           DL_GPIO_PIN_7
#define GPIO_PWM1_C0_IOMUX                                       (IOMUX_PINCM14)
#define GPIO_PWM1_C0_IOMUX_FUNC                      IOMUX_PINCM14_PF_TIMG8_CCP0
#define GPIO_PWM1_C0_IDX                                     DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_PWM1_C1_PORT                                                  GPIOA
#define GPIO_PWM1_C1_PIN                                          DL_GPIO_PIN_30
#define GPIO_PWM1_C1_IOMUX                                        (IOMUX_PINCM5)
#define GPIO_PWM1_C1_IOMUX_FUNC                       IOMUX_PINCM5_PF_TIMG8_CCP1
#define GPIO_PWM1_C1_IDX                                     DL_TIMER_CC_1_INDEX



/* Defines for TIMER_0 */
#define TIMER_0_INST                                                     (TIMG0)
#define TIMER_0_INST_IRQHandler                                 TIMG0_IRQHandler
#define TIMER_0_INST_INT_IRQN                                   (TIMG0_INT_IRQn)
#define TIMER_0_INST_LOAD_VALUE                                         (39999U)



/* Defines for UART_0 */
#define UART_0_INST                                                        UART3
#define UART_0_INST_FREQUENCY                                           80000000
#define UART_0_INST_IRQHandler                                  UART3_IRQHandler
#define UART_0_INST_INT_IRQN                                      UART3_INT_IRQn
#define GPIO_UART_0_RX_PORT                                                GPIOA
#define GPIO_UART_0_TX_PORT                                                GPIOA
#define GPIO_UART_0_RX_PIN                                        DL_GPIO_PIN_25
#define GPIO_UART_0_TX_PIN                                        DL_GPIO_PIN_26
#define GPIO_UART_0_IOMUX_RX                                     (IOMUX_PINCM55)
#define GPIO_UART_0_IOMUX_TX                                     (IOMUX_PINCM59)
#define GPIO_UART_0_IOMUX_RX_FUNC                      IOMUX_PINCM55_PF_UART3_RX
#define GPIO_UART_0_IOMUX_TX_FUNC                      IOMUX_PINCM59_PF_UART3_TX
#define UART_0_BAUD_RATE                                                  (9600)
#define UART_0_IBRD_80_MHZ_9600_BAUD                                       (520)
#define UART_0_FBRD_80_MHZ_9600_BAUD                                        (53)





/* Port definition for Pin Group GPIO_GRP_22 */
#define GPIO_GRP_22_PORT                                                 (GPIOB)

/* Defines for PIN_0: GPIOB.22 with pinCMx 50 on package pin 21 */
#define GPIO_GRP_22_PIN_0_PIN                                   (DL_GPIO_PIN_22)
#define GPIO_GRP_22_PIN_0_IOMUX                                  (IOMUX_PINCM50)
/* Port definition for Pin Group key1 */
#define key1_PORT                                                        (GPIOB)

/* Defines for PIN_1: GPIOB.27 with pinCMx 58 on package pin 29 */
#define key1_PIN_1_PIN                                          (DL_GPIO_PIN_27)
#define key1_PIN_1_IOMUX                                         (IOMUX_PINCM58)
/* Port definition for Pin Group key2 */
#define key2_PORT                                                        (GPIOB)

/* Defines for PIN_2: GPIOB.23 with pinCMx 51 on package pin 22 */
#define key2_PIN_2_PIN                                          (DL_GPIO_PIN_23)
#define key2_PIN_2_IOMUX                                         (IOMUX_PINCM51)
/* Port definition for Pin Group key3 */
#define key3_PORT                                                        (GPIOB)

/* Defines for PIN_3: GPIOB.8 with pinCMx 25 on package pin 60 */
#define key3_PIN_3_PIN                                           (DL_GPIO_PIN_8)
#define key3_PIN_3_IOMUX                                         (IOMUX_PINCM25)
/* Port definition for Pin Group OLED_SCL */
#define OLED_SCL_PORT                                                    (GPIOB)

/* Defines for PIN_4: GPIOB.2 with pinCMx 15 on package pin 50 */
#define OLED_SCL_PIN_4_PIN                                       (DL_GPIO_PIN_2)
#define OLED_SCL_PIN_4_IOMUX                                     (IOMUX_PINCM15)
/* Port definition for Pin Group OLED_SDA */
#define OLED_SDA_PORT                                                    (GPIOB)

/* Defines for PIN_5: GPIOB.3 with pinCMx 16 on package pin 51 */
#define OLED_SDA_PIN_5_PIN                                       (DL_GPIO_PIN_3)
#define OLED_SDA_PIN_5_IOMUX                                     (IOMUX_PINCM16)
/* Port definition for Pin Group GraySensor_L4 */
#define GraySensor_L4_PORT                                               (GPIOB)

/* Defines for PIN_6: GPIOB.19 with pinCMx 45 on package pin 16 */
#define GraySensor_L4_PIN_6_PIN                                 (DL_GPIO_PIN_19)
#define GraySensor_L4_PIN_6_IOMUX                                (IOMUX_PINCM45)
/* Port definition for Pin Group GraySensor_L3 */
#define GraySensor_L3_PORT                                               (GPIOB)

/* Defines for PIN_7: GPIOB.17 with pinCMx 43 on package pin 14 */
#define GraySensor_L3_PIN_7_PIN                                 (DL_GPIO_PIN_17)
#define GraySensor_L3_PIN_7_IOMUX                                (IOMUX_PINCM43)
/* Port definition for Pin Group GraySensor_L2 */
#define GraySensor_L2_PORT                                               (GPIOA)

/* Defines for PIN_8: GPIOA.16 with pinCMx 38 on package pin 9 */
#define GraySensor_L2_PIN_8_PIN                                 (DL_GPIO_PIN_16)
#define GraySensor_L2_PIN_8_IOMUX                                (IOMUX_PINCM38)
/* Port definition for Pin Group GraySensor_L1 */
#define GraySensor_L1_PORT                                               (GPIOA)

/* Defines for PIN_9: GPIOA.14 with pinCMx 36 on package pin 7 */
#define GraySensor_L1_PIN_9_PIN                                 (DL_GPIO_PIN_14)
#define GraySensor_L1_PIN_9_IOMUX                                (IOMUX_PINCM36)
/* Port definition for Pin Group GraySensor_R1 */
#define GraySensor_R1_PORT                                               (GPIOB)

/* Defines for PIN_10: GPIOB.20 with pinCMx 48 on package pin 19 */
#define GraySensor_R1_PIN_10_PIN                                (DL_GPIO_PIN_20)
#define GraySensor_R1_PIN_10_IOMUX                               (IOMUX_PINCM48)
/* Port definition for Pin Group M1 */
#define M1_PORT                                                          (GPIOB)

/* Defines for PIN_11: GPIOB.10 with pinCMx 27 on package pin 62 */
#define M1_PIN_11_PIN                                           (DL_GPIO_PIN_10)
#define M1_PIN_11_IOMUX                                          (IOMUX_PINCM27)
/* Port definition for Pin Group M_1 */
#define M_1_PORT                                                         (GPIOB)

/* Defines for PIN_12: GPIOB.9 with pinCMx 26 on package pin 61 */
#define M_1_PIN_12_PIN                                           (DL_GPIO_PIN_9)
#define M_1_PIN_12_IOMUX                                         (IOMUX_PINCM26)
/* Port definition for Pin Group M2 */
#define M2_PORT                                                          (GPIOB)

/* Defines for PIN_13: GPIOB.7 with pinCMx 24 on package pin 59 */
#define M2_PIN_13_PIN                                            (DL_GPIO_PIN_7)
#define M2_PIN_13_IOMUX                                          (IOMUX_PINCM24)
/* Port definition for Pin Group M_2 */
#define M_2_PORT                                                         (GPIOB)

/* Defines for PIN_14: GPIOB.6 with pinCMx 23 on package pin 58 */
#define M_2_PIN_14_PIN                                           (DL_GPIO_PIN_6)
#define M_2_PIN_14_IOMUX                                         (IOMUX_PINCM23)
/* Port definition for Pin Group LED1 */
#define LED1_PORT                                                        (GPIOA)

/* Defines for PIN_15: GPIOA.29 with pinCMx 4 on package pin 36 */
#define LED1_PIN_15_PIN                                         (DL_GPIO_PIN_29)
#define LED1_PIN_15_IOMUX                                         (IOMUX_PINCM4)
/* Port definition for Pin Group LED2 */
#define LED2_PORT                                                        (GPIOB)

/* Defines for PIN_16: GPIOB.26 with pinCMx 57 on package pin 28 */
#define LED2_PIN_16_PIN                                         (DL_GPIO_PIN_26)
#define LED2_PIN_16_IOMUX                                        (IOMUX_PINCM57)
/* Port definition for Pin Group GraySensor_R4 */
#define GraySensor_R4_PORT                                               (GPIOB)

/* Defines for PIN_17: GPIOB.0 with pinCMx 12 on package pin 47 */
#define GraySensor_R4_PIN_17_PIN                                 (DL_GPIO_PIN_0)
#define GraySensor_R4_PIN_17_IOMUX                               (IOMUX_PINCM12)
/* Port definition for Pin Group Motor2A */
#define Motor2A_PORT                                                     (GPIOB)

/* Defines for PIN_18: GPIOB.4 with pinCMx 17 on package pin 52 */
// groups represented: ["Motor2B","Motor1A","Motor1B","Motor2A"]
// pins affected: ["PIN_19","PIN_20","PIN_21","PIN_18"]
#define GPIO_MULTIPLE_GPIOB_INT_IRQN                            (GPIOB_INT_IRQn)
#define GPIO_MULTIPLE_GPIOB_INT_IIDX            (DL_INTERRUPT_GROUP1_IIDX_GPIOB)
#define Motor2A_PIN_18_IIDX                                  (DL_GPIO_IIDX_DIO4)
#define Motor2A_PIN_18_PIN                                       (DL_GPIO_PIN_4)
#define Motor2A_PIN_18_IOMUX                                     (IOMUX_PINCM17)
/* Port definition for Pin Group Motor2B */
#define Motor2B_PORT                                                     (GPIOB)

/* Defines for PIN_19: GPIOB.5 with pinCMx 18 on package pin 53 */
#define Motor2B_PIN_19_IIDX                                  (DL_GPIO_IIDX_DIO5)
#define Motor2B_PIN_19_PIN                                       (DL_GPIO_PIN_5)
#define Motor2B_PIN_19_IOMUX                                     (IOMUX_PINCM18)
/* Port definition for Pin Group Motor1A */
#define Motor1A_PORT                                                     (GPIOB)

/* Defines for PIN_20: GPIOB.12 with pinCMx 29 on package pin 64 */
#define Motor1A_PIN_20_IIDX                                 (DL_GPIO_IIDX_DIO12)
#define Motor1A_PIN_20_PIN                                      (DL_GPIO_PIN_12)
#define Motor1A_PIN_20_IOMUX                                     (IOMUX_PINCM29)
/* Port definition for Pin Group Motor1B */
#define Motor1B_PORT                                                     (GPIOB)

/* Defines for PIN_21: GPIOB.13 with pinCMx 30 on package pin 1 */
#define Motor1B_PIN_21_IIDX                                 (DL_GPIO_IIDX_DIO13)
#define Motor1B_PIN_21_PIN                                      (DL_GPIO_PIN_13)
#define Motor1B_PIN_21_IOMUX                                     (IOMUX_PINCM30)
/* Port definition for Pin Group GraySensor_R2 */
#define GraySensor_R2_PORT                                               (GPIOB)

/* Defines for PIN_22: GPIOB.25 with pinCMx 56 on package pin 27 */
#define GraySensor_R2_PIN_22_PIN                                (DL_GPIO_PIN_25)
#define GraySensor_R2_PIN_22_IOMUX                               (IOMUX_PINCM56)
/* Port definition for Pin Group GraySensor_R3 */
#define GraySensor_R3_PORT                                               (GPIOA)

/* Defines for PIN_23: GPIOA.27 with pinCMx 60 on package pin 31 */
#define GraySensor_R3_PIN_23_PIN                                (DL_GPIO_PIN_27)
#define GraySensor_R3_PIN_23_IOMUX                               (IOMUX_PINCM60)





/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);

bool SYSCFG_DL_SYSCTL_SYSPLL_init(void);
void SYSCFG_DL_PWM1_init(void);
void SYSCFG_DL_TIMER_0_init(void);
void SYSCFG_DL_UART_0_init(void);

void SYSCFG_DL_RTC_init(void);

bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */

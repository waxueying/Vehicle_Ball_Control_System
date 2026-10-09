/**
  * @file    main.c
  * @brief   循迹小车主程序：8 路灰度循迹 + 编码器测速双闭环 + 里程定点停车
  * @note    控制周期 20 ms（TIMG0 中断）内依次完成：
  *            1. 读编码器 → 换算两轮线速度与累计里程
  *            2. 8 路灰度加权求线位置 → 转向环（位置式 PID）
  *            3. 速度环（增量式 PI）→ 基础 PWM
  *            4. 左轮 = 基础 PWM - 转向量，右轮 = 基础 PWM + 转向量（限幅 0~99）
  *          任务模式由按键选择，速度设定值与停车里程见各分支
  */
#include "ti_msp_dl_config.h"

#include "OLED.h"
#include "PID.h"
#include "OLED_Data.h"
#include "Motor.h"
#include "PWM.h"
#include "Encoder.h"
#include "GraySensor.h"
#include "key.h"
#include "LED.h"
#include "key.h"
#include "delay.h"
#include "BlueSerial.h"
// #include "Buzzer.h"

#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

IncPID_TypeDef pid_speed;     /* 增量式PI控制器 (Kp/Ki/output/限幅) */
PID_TypeDef pid_turn;           //z转向

extern float xunji;             /* GraySensor.c 中的黑线位置（0~1，中心0.55） */
extern float last_xunji;
float turn_output = 0.0f;       /* 转向PID输出（叠加到左右轮差速） */

float turn_Kp = 50.0f;
float turn_Ki = 0.0f;
float turn_Kd = 0.7f;

float base_speed = 0.0f;    /* 当前设定速度 (cm/s)，经 ramp 平滑 */
volatile float target_speed = 0.0f;  /* 目标速度 (cm/s)，由模式选择设定 */
volatile float avg_speed = 0.0f;      /* 两轮平均速度 */
#define WHEEL_CIRCUMFERENCE  21.05f  /* 轮胎周长 = π × 6.7cm */
#define SPEED_SETPOINT_STEP  0.1f   /* 每20ms 的设定速度变化量：0.1cm/s => 5cm/s² */
volatile float stop_distance = 0.0f;  /* 当前模式的停车距离 (cm) */
uint8_t turn_gear = 0;               /* 0=低速转向PID, 1=高速转向PID */
uint16_t all_black_cnt = 0;          /* 全黑去抖计数 */
int16_t left_pwm = 0;        /* 左轮最终PWM */
int16_t right_pwm = 0;       /* 右轮最终PWM */

volatile uint8_t carmode = 0;  /* 0=循迹, 1=遥控, 2=停车 */

volatile uint32_t SpeedCnt = 0;
volatile uint16_t SpeedCalc_Cnt = 0; 
volatile float Motor1_Speed_cms = 0.0f;  /* 电机1线速度 (cm/s) */
volatile float Motor2_Speed_cms = 0.0f;  /* 电机2线速度 (cm/s) */
volatile int32_t Enc1_Raw = 0;          /* 编码器1原始计数 */
volatile int32_t Enc2_Raw = 0;          /* 编码器2原始计数 */
volatile float Motor1_dist = 0.0f;   /* 电机1线路程 (cm/s) */
volatile float Motor2_dist= 0.0f;  /* 电机2线路程 (cm/s) */
volatile float distance = 0.0f;  /* 车行驶的总距离 (cm) */
volatile uint32_t elapsed_ms = 0;   /* 运行时间 (ms) */

int32_t c1 = 0;//编码器的原始数字
int32_t c2 = 0;//编码器的原始数字

int main(void)
{
    SYSCFG_DL_init();       /* SysConfig 自动完成时钟+外设初始化 (PLL 已在 ti_msp_dl_config.c 中修正为 80MHz) */

     PWM_Init();
     Motor_Init();
    Encoder_Init();
    OLED_Init();
    IncPID_Init(&pid_speed, 0.3f, 1.2f, 99.0f, 0.0f);
    PID_Init(&pid_turn, turn_Kp, turn_Ki, turn_Kd, 0.50f);
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);  /* 使能 TIMG0 NVIC 中断 */
    BlueSerial_Init();
    // Buzzer_ON();
    LED1_ON();
    LED2_ON();
    

    while (1)
    {
        
    //     Motor_SetPWM(1,20);
    //     Motor_SetPWM(2,20);
    //    int32_t c1= Encoder_Get(1);
    //     int32_t c2 = Encoder_Get(2);
     
        if(carmode == 0)
        {
            uint8_t gray_value = GraySensor_ReadAll();
            OLED_Clear();
            OLED_ShowString(28,0,"READY",OLED_8X16);
            OLED_ShowString(0,16,"D8 D7 D6 D5",OLED_6X8);
            OLED_ShowNum(0,24,(gray_value >> 7) & 1,1,OLED_8X16);
            OLED_ShowNum(16,24,(gray_value >> 6) & 1,1,OLED_8X16);
            OLED_ShowNum(32,24,(gray_value >> 5) & 1,1,OLED_8X16);
            OLED_ShowNum(48,24,(gray_value >> 4) & 1,1,OLED_8X16);
            OLED_ShowString(0,40,"D4 D3 D2 D1",OLED_6X8);
            OLED_ShowNum(0,48,(gray_value >> 3) & 1,1,OLED_8X16);
            OLED_ShowNum(16,48,(gray_value >> 2) & 1,1,OLED_8X16);
            OLED_ShowNum(32,48,(gray_value >> 1) & 1,1,OLED_8X16);
            OLED_ShowNum(48,48,(gray_value >> 0) & 1,1,OLED_8X16);
            OLED_Update();

            if(KEY1_Read() == 1)
            {
                carmode = 1;
                base_speed = 0.0f;
                target_speed = 40.0f;
                stop_distance = 520.0f;
                IncPID_Init(&pid_speed, 0.3f, 1.2f, 99.0f, 0.0f);
                PID_Init(&pid_turn, 50.0f, 5.5f, 1.0f, 0.55f);
                elapsed_ms = 0;
                distance = 0.0f;
                OLED_Clear();
                OLED_ShowString(0,0,"TASK1 40cm/s",OLED_8X16);
                OLED_Update();
                delay_ms(500);
            }
            else if(KEY2_Read() == 1)
            {
                carmode = 2;
                base_speed = 0.0f;
                target_speed = 25.0f;
                PID_Init(&pid_turn, 27.0f, 0.0f, 0.2f, 0.55f);
                stop_distance = 580.0f;
                elapsed_ms = 0;
                distance = 0.0f;
                OLED_Clear();
                OLED_ShowString(0,0,"TASK2 STOP",OLED_8X16);
                OLED_Update();
                delay_ms(500);
            }
            else if(KEY3_Read() == 1)
            {
                carmode = 3;
                base_speed = 0.0f;
                target_speed = 50.0f;
                stop_distance = 160.0f;
                IncPID_Init(&pid_speed, 0.2f, 1.0f, 99.0f, 0.0f);
                PID_Init(&pid_turn, 27.0f, 0.0f, 0.2f, 0.55f);
                elapsed_ms = 0;
                distance = 0.0f;
                OLED_Clear();
                OLED_ShowString(0,0,"TASK3 25cm/s",OLED_8X16);
                OLED_Update();
                delay_ms(500);
            }
            OLED_Update();
        }
        else if (carmode == 4)
        {
            /* 完成状态: 显示上一段时间/距离 */
            uint32_t elapsed_snap;
            float distance_snap;

            __disable_irq();
            elapsed_snap = elapsed_ms;
            distance_snap = distance;
            __enable_irq();

            OLED_Clear();
            OLED_ShowString(0,0,"TASK1 DONE!",OLED_8X16);
            OLED_ShowString(0,16,"time:",OLED_8X16);
            OLED_ShowFloatNum(40,16,elapsed_snap / 1000.0f, 2, 1, OLED_8X16);
            OLED_ShowString(88,16,"s",OLED_8X16);
            OLED_ShowString(0,32,"D:",OLED_8X16);
            OLED_ShowFloatNum(24,32,distance_snap / 100.0f, 2, 2, OLED_8X16);
            OLED_ShowString(80,32,"m",OLED_8X16);
            OLED_ShowString(0,48,"KEY2: back",OLED_6X8);
            OLED_Update();

            if (KEY2_Read() == 1)
            {
                carmode = 0;
                delay_ms(500);
            }
        }
        else if (carmode >= 1)
        {
            uint32_t elapsed_snap;
            float distance_snap;
            float avg_speed_snap;

            __disable_irq();
            elapsed_snap = elapsed_ms;
            distance_snap = distance;
            avg_speed_snap = avg_speed;
            __enable_irq();

            /* KEY2: 随时回到模式选择 */
            if (KEY2_Read() == 1)
            {
                Motor_SetPWM(1, 0);
                Motor_SetPWM(2, 0);
                carmode = 0;
                delay_ms(500);
                continue;
            }

            OLED_Clear();
            /* 第0行: 模式 */
            if (carmode == 1)
                OLED_ShowString(0,0,"TASK1 40",OLED_8X16);
            else if (carmode == 2)
                OLED_ShowString(0,0,"TASK2 STOP",OLED_8X16);
            else
                OLED_ShowString(0,0,"TASK3 20",OLED_8X16);

            /* 第1行: 运行时间 */
            OLED_ShowString(0,16,"T:",OLED_8X16);
            OLED_ShowFloatNum(24,16,elapsed_snap / 1000.0f, 2, 1, OLED_8X16);
            OLED_ShowString(72,16,"s",OLED_8X16);

            /* 第2行: 里程 */
            OLED_ShowString(0,32,"D:",OLED_8X16);
            OLED_ShowFloatNum(24,32,distance_snap / 100.0f, 2, 2, OLED_8X16);
            OLED_ShowString(80,32,"m",OLED_8X16);

            /* 第3行: 当前速度 */
            OLED_ShowString(0,48,"V:",OLED_8X16);
            OLED_ShowFloatNum(24,48,avg_speed_snap, 2, 0, OLED_8X16);
            OLED_ShowString(72,48,"cm/s",OLED_6X8);

            OLED_Update();
        }
        // OLED_Clear();
        // OLED_ShowString(0,0,"D1:",OLED_8X16);
        // OLED_ShowNum(24,0,D1,1,OLED_8X16);
        // OLED_ShowString(48,0,"D2:",OLED_8X16);
        // OLED_ShowNum(72,0,D2,1,OLED_8X16);
        // OLED_ShowString(0,16,"D3:",OLED_8X16);
        // OLED_ShowNum(24,16,D3,1,OLED_8X16);
        // OLED_ShowString(48,16,"D4:",OLED_8X16);
        // OLED_ShowNum(72,16,D4,1,OLED_8X16);
        // OLED_ShowString(0,32,"pos:",OLED_8X16);
        // OLED_ShowFloatNum(48,32,xunji,2,2,OLED_8X16);
        // OLED_ShowString(0,48,"C1:",OLED_8X16);
        // OLED_ShowSignedNum(24,48,c1,5,OLED_8X16);
        // OLED_ShowString(72,48,"C2:",OLED_8X16);
        // OLED_ShowSignedNum(96,48,c2,5,OLED_8X16);
        // OLED_Update();
    }
}

void SpeedPID_Update(void)
{
    avg_speed = (Motor1_Speed_cms + Motor2_Speed_cms) / 2.0f;
    IncPID_Update(&pid_speed, base_speed - avg_speed, 0.02f);
}

void TIMG0_IRQHandler(void)
{
    DL_TimerG_clearInterruptStatus(TIMER_0_INST, DL_TIMERG_INTERRUPT_ZERO_EVENT);
    SpeedCalc_Cnt++;
    if (carmode == 0 || carmode == 4)
    {
        SpeedCalc_Cnt = 0;
        return;
    }

    if (SpeedCalc_Cnt >= 20 && carmode >= 1)   /* 每20次(20ms)执行一次 */
    {
        /*--- 0. 读取编码器原始计数 + 速度 ---*/
            c1 = Encoder_Get(1);
            c2 = Encoder_Get(2);
            Enc1_Raw = c1;
            Enc2_Raw = c2;
            Motor1_Speed_cms = c1 * WHEEL_CIRCUMFERENCE / (float)ENCODER_RESOLUTION / 0.02f;
            Motor2_Speed_cms = c2 * WHEEL_CIRCUMFERENCE / (float)ENCODER_RESOLUTION / 0.02f;

        /*--- 1. 读灰度 → 转向修正 ---*/
            xunji_shuai_xin();

            /* 根据速度自动切换转向PID (TASK1除外，TASK1用自己的固定PID) */
            if (carmode != 1)
            {
                if (avg_speed < 20.0f && turn_gear != 0)
                {
                    turn_gear = 0;
                    pid_turn.Kp = 25.0f; pid_turn.Ki = 0.0f; pid_turn.Kd = 0.2f;  /* 低速 */
                }
                else if (avg_speed >= 20.0f && turn_gear != 1)
                {
                    turn_gear = 1;
                    pid_turn.Kp = 25.0f; pid_turn.Ki = 0.0f; pid_turn.Kd = 0.2f;  /* 高速 */
                }
            }

            turn_output = Place_PID_Update(&pid_turn, xunji, 0.02f);

        /*--- 2. 速度设定值平滑 ramp + 速度环PI（两轮平均速度追 base_speed） ---*/
            if (target_speed - base_speed > SPEED_SETPOINT_STEP)
                base_speed += SPEED_SETPOINT_STEP;
            else if (target_speed - base_speed < -SPEED_SETPOINT_STEP)
                base_speed -= SPEED_SETPOINT_STEP;
            else
                base_speed = target_speed;

            SpeedPID_Update();

        /*--- 2.5 累加运行时间 + 里程 ---*/
            elapsed_ms += 20;
            distance += avg_speed * 0.02f;  /* cm */

        /*--- 2.6 根据 stop_distance 自动停车 ---*/
            if (carmode >= 1 && carmode <= 3 && distance >= stop_distance)
            {
                Motor_SetPWM(1, 0);
                Motor_SetPWM(2, 0);
                carmode = 4;
                SpeedCalc_Cnt = 0;
                return;
            }

        /*--- 3. 速度环输出 ± 转向差速 = 左右轮最终PWM ---*/
            left_pwm  = (int16_t)pid_speed.output - (int16_t)turn_output;
            right_pwm = (int16_t)pid_speed.output + (int16_t)turn_output;

            if (left_pwm  > 99) left_pwm  = 99;
            if (left_pwm  < 0)  left_pwm  = 0;
            if (right_pwm > 99) right_pwm = 99;
            if (right_pwm < 0)  right_pwm = 0;

            Motor_SetPWM(2, (uint16_t)left_pwm);   /* 电机2 = 左轮 */
            Motor_SetPWM(1, (uint16_t)right_pwm);  /* 电机1 = 右轮 */
            SpeedCalc_Cnt = 0;
    }

    
}

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : 串级PID + 球速前馈 + 小车加速度前馈(ICM20602)
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OLED.h"
#include "servo_driver.h"
#include "uart_protocol.h"
#include "PID.h"
#include "math.h"
#include "SEEKFREE_ICM20602.h"
#include "key.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* ==================== 舵机中位 ==================== */
#define SERVO_CENTER     350     /* 水平中位 CCR */

/* ==================== 位置死区 ==================== */
#define DEAD_ZONE        0.1f    /* 位置死区(cm)：|偏移|<此值舵机冻结 */

/* ==================== 球速前馈（MaixCAM2发来的球速） ==================== */
#define FF_GAIN          0.1f    /* 球速前馈系数：球速×此值=反向拦截速度(cm/s) */
#define FF_DEADBAND      0.3f    /* 球速前馈死区(cm/s)：|球速|<此值前馈清零 */

/* ==================== 小车加速度前馈（ICM20602 Y轴） ==================== */
#define K_CART_FF        2.3f   /* 加速度→舵机增益(CCR per cm/s²)          */
#define ACC_FILT_ALPHA   0.03f   /* 加速度低通系数(0~1)：越小越平滑越滞后  */
#define ACC_BIAS        -26.0f   /* Y轴零偏(cm/s²)：静止不为0时减去此值    */
#define ACC_FF_DEADBAND  30.0f   /* 加速度前馈死区(cm/s²)：<此值不触发     */

/* ==================== 舵机硬限幅（PID ±35° ≈ ±98 CCR，加速度前馈余量 ±30 CCR） ==================== */
#define CCR_MIN          227     /* 355 - 128 = 227（≈ -45°） */
#define CCR_MAX          483     /* 355 + 128 = 483（≈ +45°） */

/* ==================== ICM20602 读取与显示 ==================== */
#define ACC_READ_MS      5       /* ICM20602读取间隔(ms) */
#define ACC_DISP_MS      100     /* OLED刷新间隔(ms) */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
PID_TypeDef speed_pid;
PosPID_TypeDef pos_pid;
uint8_t uart_rx_byte;

volatile uint8_t SpeedCalc_Cnt = 0;
float g_target_position = 0.0f;
float g_target_speed    = 0.0f;
float g_pid_output      = 0.0f;
float g_pos_pid_out     = 0.0f;
float g_ff_term         = 0.0f;

float g_cart_acc_filt = 0.0f;
float g_acc_y_raw     = 0.0f;
float g_acc_y_max     = 0.0f;
int32_t g_cart_ff_ccr = 0;

uint32_t g_acc_tick  = 0;
uint32_t g_disp_tick = 0;

static float last_pos_error = 0.0f;

volatile uint16_t g_current_ccr = SERVO_CENTER;   /* 当前舵机 CCR（OLED显示用） */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void MX_USART2_Init(void);
void CartAcc_SampleAndFilter(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        UART_RxByteHandler(uart_rx_byte);
        HAL_UART_Receive_IT(&huart2, &uart_rx_byte, 1);
    }
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1)
    {
      SpeedCalc_Cnt++;
      if (SpeedCalc_Cnt >= 33)
      {
        Key_ProcessAndSend();
        /* ===== 加速度前馈：独立于串口，每33ms运行 ===== */
        if (fabsf(g_cart_acc_filt) > ACC_FF_DEADBAND)
            g_cart_ff_ccr = (int32_t)(g_cart_acc_filt * K_CART_FF);
        else
            g_cart_ff_ccr = 0;

        /* ===== PID控制：依赖串口数据 ===== */
        if (g_data_fresh)
        {
            g_data_fresh = 0;

            float pos_error = g_target_position - g_offset_cm;

            if (fabsf(pos_error) < DEAD_ZONE)
            {
                g_pos_pid_out    = 0.0f;
                g_ff_term        = 0.0f;
                g_target_speed   = 0.0f;
                g_pid_output     = 0.0f;
                g_cart_ff_ccr    = 0;
                PID_Reset(&speed_pid);
                pos_pid.integral = 0.0f;
                SpeedCalc_Cnt = 0;
                return;
            }

            if ((last_pos_error * pos_error) < 0.0f)
            {
                PID_Reset(&speed_pid);
                pos_pid.integral = 0.0f;
            }
            last_pos_error = pos_error;

            g_pos_pid_out = PosPID_Calc(&pos_pid, g_target_position, g_offset_cm);

            if (fabsf(g_velocity) < FF_DEADBAND)
                g_ff_term = 0.0f;
            else
                g_ff_term = -g_velocity * FF_GAIN;

            g_target_speed = g_pos_pid_out + g_ff_term;
            g_pid_output = PID_VelocityCalc(&speed_pid, g_target_speed, g_velocity);
        }
        /* ===== 舵机输出 ===== */
        {
            int16_t ccr = SERVO_CENTER + (int16_t)g_pid_output + (int16_t)g_cart_ff_ccr;
            if (ccr < CCR_MIN) ccr = CCR_MIN;
            if (ccr > CCR_MAX) ccr = CCR_MAX;
            g_current_ccr = (uint16_t)ccr;
            Servo_SetCcr((uint16_t)ccr);
        }
        SpeedCalc_Cnt = 0;
      }
    }
}
/* USER CODE END 0 */

int main(void)
{

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	OLED_Init();
	OLED_ShowString(0, 0, "Cascade+CartFF", OLED_6X8);
	OLED_ShowString(0, 2, "mode:0", OLED_6X8);
	OLED_Update();
	Servo_Init();
	Servo_SetCcr(SERVO_CENTER);
	HAL_Delay(500);
	Key_Init();
	/* ---- PB6(SCL) PB7(SDA) 开漏 ---- */
	{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		__HAL_RCC_GPIOB_CLK_ENABLE();
		GPIO_InitStruct.Pin   = GPIO_PIN_6 | GPIO_PIN_7;
		GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull  = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	}
	HAL_Delay(10);
	icm20602_init_simiic();
	HAL_Delay(100);
	PosPID_Init(&pos_pid,1.35f, 0.0035f, 0.12f, 20.0f, 20.0f);
	PID_VelocityInit(&speed_pid, 6.0f, 0.20f, 0.00f, 98.0f, 0.70f);
	HAL_TIM_Base_Start_IT(&htim1);
	UART_RecvFrame_Init();
	HAL_UART_Receive_IT(&huart2, &uart_rx_byte, 1);
	g_acc_tick  = HAL_GetTick();
	g_disp_tick = HAL_GetTick();
  /* USER CODE END 2 */

  while (1)
  {
    /* USER CODE BEGIN 3 */
    if (HAL_GetTick() - g_acc_tick >= ACC_READ_MS)
    {
        g_acc_tick = HAL_GetTick();
        CartAcc_SampleAndFilter();
    }

    if (HAL_GetTick() - g_disp_tick >= ACC_DISP_MS)
    {
        g_disp_tick = HAL_GetTick();
        int32_t acc_y   = (int32_t)g_acc_y_raw;
        int32_t acc_f   = (int32_t)g_cart_acc_filt;
        int32_t acc_max = (int32_t)g_acc_y_max;
        int32_t ff_ccr  = g_cart_ff_ccr;
        g_acc_y_max = 0.0f;

        if (Key_GetModeChanged())
        {
            Key_ClearModeChanged();
            char buf[16];
            snprintf(buf, sizeof(buf), "mode:%u", Key_GetMode());
            OLED_ShowString(0, 2, buf, OLED_6X8);
            OLED_Update();
        }
    }
  }
  /* USER CODE END 3 */
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) Error_Handler();
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) Error_Handler();
}

/* USER CODE BEGIN 4 */
void CartAcc_SampleAndFilter(void)
{
    get_icm20602_accdata_simiic();
    icm20602_data_change();

    g_acc_y_raw = unit_icm_acc_y * 980.0f;

    {
        float abs_v = fabsf(g_acc_y_raw);
        if (abs_v > g_acc_y_max) g_acc_y_max = abs_v;
    }

    {
        float raw = g_acc_y_raw - ACC_BIAS;
        g_cart_acc_filt = ACC_FILT_ALPHA * raw
                        + (1.0f - ACC_FILT_ALPHA) * g_cart_acc_filt;
    }
}

void MX_USART2_Init(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  GPIOA->CRL &= ~(0x0F << 8);  GPIOA->CRL |= (0x0B << 8);
  GPIOA->CRL &= ~(0x0F << 12); GPIOA->CRL |= (0x04 << 12);
  USART2->BRR = 0x00D0;
  USART2->CR1 = USART_CR1_TE | USART_CR1_RE;
  USART2->CR1 |= USART_CR1_UE;
}
/* USER CODE END 4 */

void Error_Handler(void) { __disable_irq(); while (1) {} }
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) {}
#endif /* USE_FULL_ASSERT */

# ball_balancer — 滚球平衡控制（STM32F103）

接收视觉板（MaixCAM2，YOLO 实时检测）识别的钢球位置与球速，用串级 PID + 双前馈补偿控制舵机驱动摆杆，把钢球稳定在摆杆目标位置。

## 控制结构

```
MaixCAM2 视觉板 ──UART2（12 字节定长帧：球偏移 cm + 球速 cm/s）──▶ STM32F103

TIM1 中断 1 ms，每 33 次（约 33 ms）执行一次控制：

    位置环（位置式 PID，输出/积分限幅）── 目标球速 ──▶ 速度环（增量式 PID + 一阶低通）── 舵机 CCR
              ▲
    前馈补偿：球速阻尼前馈（-v × 0.1）
             + 车体加速度前馈（ICM20602 Y 轴，低通 α=0.03、零偏校正、30 cm/s² 死区、2.3 CCR/(cm/s²)）
```

## 关键实现

- 串级 PID（`BSP/PID.c`）：位置环输出目标球速，速度环输出舵机 CCR；位置死区 0.1 cm 内冻结输出，误差过零时重置速度环与积分，避免反向残留
- 前馈补偿：视觉球速阻尼前馈抑制超调；ICM20602 车体加速度前馈抵消小车加减速带来的惯性扰动，不必等位置误差形成
- 串口协议（`BSP/uart_protocol.c`）：帧头 0xAA55 + 2 个 float（偏移、球速）+ 帧尾 0x0D0A，共 12 字节；逐字节中断状态机解析，IDLE 中断复位状态机，`g_data_fresh` 标志保证控制周期只消费新数据
- ICM20602 驱动：手写软件 I2C（PB6=SCL / PB7=SDA，开漏 + 内部上拉），5 ms 采样，加速度零偏校正
- 模式切换：板载按键消抖后向视觉板下发模式帧，视觉板回 ACK 后本板更新模式，ACK 状态机与数据帧解析互不干扰
- 舵机输出：TIM2 CH1（PA0）50 Hz PWM，CCR 227~483 约 ±45° 硬限幅

## 主要文件

| 文件 | 作用 |
| --- | --- |
| `BSP/PID.c` / `PID.h` | 位置式、增量式 PID 与低通滤波；轨迹生成器（未接入） |
| `BSP/uart_protocol.c` / `.h` | 视觉板串口帧协议与解析状态机 |
| `BSP/SEEKFREE_IIC.c` | 软件 I2C 时序实现 |
| `BSP/SEEKFREE_ICM20602.c` | ICM20602 寄存器配置与数据换算 |
| `BSP/servo_driver.c` | 舵机 PWM 输出与角度映射 |
| `BSP/key.c` | 按键消抖与视觉板模式握手 |
| `Core/Src/main.c` | 控制周期、前馈计算与中断回调 |

## 编译

Keil MDK5 打开 `MDK-ARM/HAL_oled.uvprojx`（目标 `HAL_oled`）编译下载；命令行可用 `UV4 -b MDK-ARM/HAL_oled.uvprojx`。

## 已知问题

- `BSP/MY_can.c` / `MY_can.h` 未加入 Keil 工程，且依赖不存在的 `can.h`，属早期版本遗留文件
- `PID.c` 中的 `TrajGen_*` 与 `FeedForward_Calc` 已实现但未被调用
- `HAL_oled.ioc` 与当前代码部分不同步（部分引脚在 `main.c` 中手工初始化），直接重新生成会覆盖现有配置

# car_base — 循迹底盘控制（MSPM0G3507）

8 路灰度求线位置，串级 PID（外环线位置、内环轮速）差速驱动左右轮，按编码器积分里程定点停车。

## 控制结构

```
20 ms 定时中断（TIMG0）内依次完成：
  1. 读编码器计数 → 换算两轮线速度（cm/s）与累计里程（cm）
  2. 8 路灰度归一化加权 → 线位置 xunji（0~1，目标 0.55）
  3. 外环：位置式 PID（线位置偏差）→ 转向量 turn_output
  4. 内环：增量式 PI（速度设定值 - 两轮平均速度）→ 基础 PWM
  5. 左轮 = 基础 PWM - turn_output，右轮 = 基础 PWM + turn_output（限幅 0~99）
```

- 速度设定值按斜坡步进（每 20 ms），抑制起步打滑
- 里程由轮速积分得到，达到 `stop_distance` 后自动停车并显示用时与里程

## 任务模式

| 模式 | 设定速度 | 停车里程 | 选择方式 |
| --- | --- | --- | --- |
| TASK1 | 40 cm/s | 520 cm | KEY1 |
| TASK2 | 25 cm/s | 580 cm | KEY2 |
| TASK3 | 50 cm/s | 160 cm | KEY3 |

## 主要文件

| 文件 | 作用 |
| --- | --- |
| `src/main.c` | 主循环、20 ms 控制中断、任务模式与里程停车 |
| `src/BSP/GraySensor.c` | 8 路灰度读取与归一化加权求线位置 |
| `src/BSP/Encoder.c` | 编码器中断计数（A 相上升沿 + B 相判向）与读取 |
| `src/BSP/PID.c` | 位置式（转向）与增量式（速度）PID |
| `src/BSP/Motor.c` / `PWM.c` | 双电机 PWM 与方向控制 |
| `src/BSP/BlueSerial.c` | 蓝牙串口环形缓冲与 `[a,b,c]` 文本帧解析 |
| `config/ti_msp_dl_config.c` | SysConfig 生成的外设配置 |

## 引脚

| 功能 | 引脚 |
| --- | --- |
| 灰度 D1~D8（D1 最右） | PB0 / PA27 / PB25 / PB20 / PA14 / PA16 / PB17 / PB19 |
| 编码器 1 | A=PB12，B=PB13 |
| 编码器 2 | A=PB4，B=PB5 |
| 蓝牙 UART3 | PA25(RX) / PA26(TX)，9600 bps |

电机 PWM 与方向引脚见 `src/BSP/Motor.c` 与 `config/ti_msp_dl_config.h`。

## 编译与烧录

- 依赖 MSPM0 SDK（2.11.00.07）与 arm-none-eabi-gcc
- `CMakeLists.txt` 中的 `SDK_DIR`、`gdb_wrapper.bat` 中的工具链路径均为作者本机路径，克隆后需改成自己的
- `recovery.cfg` 是通过 OpenOCD 操作 SECAP/DSSM 恢复被锁 MSPM0 芯片（SWD 失效、无法下载）的脚本

## 已知问题

- 转向 PID 的速度分段切换（低速/高速）两档参数目前相同，切换暂未产生实际效果
- OLED 上 TASK3 文案为 25 cm/s，与代码设定值 50 cm/s 不一致
- 速度斜坡处注释（0.5 cm/s，25 cm/s²）与实际值（0.1，5 cm/s²）不一致

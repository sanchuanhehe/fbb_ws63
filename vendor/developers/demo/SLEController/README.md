# SLEController

> 文档更新时间：2025 年 6 月 3 日

SLEController 是一个基于 Hispark ws63 开发板的多任务低延迟无线控制器项目，通过摇杆控制和 SLE 低延迟通信，实现对设备的精准操控。

## 项目概述

本项目实现了一个高响应性的控制系统，通过采集摇杆输入信号，并借助 SLE 框架实时传输控制数据。系统采用多任务架构设计，确保数据采集、显示与传输的高效协同。


## 作者

Lamonce - 嵌入式软件开发
silenirt - 硬件设计

硬件开源地址：https://oshwhub.com/silenirt2/xing-shan-yao-kong-qi

## 功能特性

- **摇杆数据采集与显示**
  - 支持 4 通道摇杆数据实时采集
  - OLED 屏幕显示当前控制数据和状态信息
  - 支持摇杆校准，自动采集最大值，提升百分比显示准确性
  - 支持按键切换显示内容（摇杆/状态/SLE 连接/电池电压）
- **SLE 低延迟通信**
  - 支持 SLE Server 模式，断开自动重连广播
  - 高效数据传输机制，实时上报摇杆百分比数据
  - 支持自定义设备名
- **多任务系统架构**
  - 并行处理采集、显示、传输、协议等多种功能
  - 任务间通过队列和互斥锁高效通信与同步
  - 资源利用优化
- **异常处理与容错**
  - I2C/ADC 等外设初始化失败检测与反馈
  - 消息队列读写失败的错误提示和处理
  - SLE 通信异常处理，日志输出
- **OLED 显示优化**
  - 优化摇杆数据和电池电压的显示逻辑，提升显示实时性和准确性
  - 增加显示内容切换功能，支持状态与数据界面切换

## 硬件要求

- Hispark ws63 开发板
- SSD1306 OLED 显示屏
- 四通道摇杆模块
- 开源硬件平台（链接）

## 接口定义

### ADC 接口
- ADC 通道 0-3：连接摇杆 X1、Y1、X2、Y2 轴
- ADC 通道 5：连接电池电压采集

### I2C 接口
- 连接 SSD1306 OLED 显示屏

## 项目结构

```
SLEController/
├── sle_uart_server/                # SLE Server 相关驱动与头文件
│   ├── sle_uart_server_adv.c       # Server 端广播驱动实现
│   ├── sle_uart_server_adv.h       # Server 端广播头文件
│   ├── sle_uart_server.c           # Server 端驱动实现
│   └── sle_uart_server.h           # Server 端头文件
├── CMakeLists.txt                  # CMake 构建脚本
├── README.md                       # 项目说明文档
├── SLEController.c                 # 主程序入口
├── SSD1306_OLED.c                  # OLED 驱动实现
└── SSD1306_OLED.h                   # OLED 驱动头文件
```

## 软件依赖

- Hispark Studio
- SLE 通信框架
- OLED 驱动库
- QCOM 串口助手

## 使用方法

1. 硬件连接
   - 将摇杆模块连接到 ADC 接口
   - 将 OLED 显示屏连接到 I2C 接口

2. 烧录文件
   ```
   flash ws63-liteos-app_all.fwpkg / ws63-liteos-app_load_only.fwpkg
   ```

3. 操作说明
   - 开机后系统自动初始化并进入摇杆校准状态，同时启动 SLE Server
   - SLE Server 端会自动广播设备信息，等待客户端连接
   - Server 端和 Client 端通过 Local Name 进行连接，默认名称为 `NearLink`（代码参考https://gitee.com/bearpi/bearpi-pico_h3863/tree/master/application/samples/products/sle_uart）
   - 将摇杆向四个方向移动，系统会自动校准并显示当前摇杆状态，找到摇杆最大值（最小值默认为 0）
   - 完成校准后按下 KEY3 以退出校准状态
   - OLED 显示当前摇杆数据和状态信息，支持按 KEY3 切换显示内容
   - 摇杆操作将直接反映在 OLED 屏幕上
   - 控制数据会通过 SLE 框架实时传输

## 开发方式

本项目采用 C 语言开发，基于 Hispark Studio 、HUAWEI LiteOS 和 SLE 通信框架。

### 主要任务（Task）与功能说明

- **JoystickInputTask**：
  - 初始化 ADC，负责采集 4 路摇杆原始 ADC 数据和电池电压。
  - 校准阶段自动记录各通道最大值，按下 KEY3 结束校准。
  - 采集到的数据写入 `joystick_data_queueID`（摇杆数据队列）和 `battery_voltage_queueID`（电池电压队列）。
  - 校准完成后，计算百分比数据并写入 `SLE_Transfer_QueueID`（SLE 传输队列），并对中值做抖动校准。

- **OLEDDisplayTask**：
  - 负责 OLED 屏幕的初始化和显示，显示内容包括摇杆原始数据、百分比、电池电压、SLE 连接状态等。
  - 支持按下 KEY3 切换显示内容（摇杆数据/状态信息）。
  - 通过互斥锁 `g_mux_id` 保证 OLED 初始化与 ADC 初始化的顺序，防止资源冲突。

- **SELSendTask**：
  - 负责将摇杆百分比数据通过 SLE 框架实时发送给客户端。
  - 从 `SLE_Transfer_QueueID` 读取数据，判断连接状态后进行数据上报。

- **sle_uart_server_task**：
  - 负责 SLE Server 端的消息队列管理、广播和连接状态维护。
  - SLE 通过设备名 `sle_local_name` 进行连接，默认名称为 `NearLink`。
  - 通过内部消息队列实现与 SLE 通信协议的集成，断开时自动重启广播。

### 队列（Queue）说明

- `joystick_data_queueID`：摇杆原始数据队列，供 `JoystickInputTask` 写入，`OLEDDisplayTask` 读取。
- `battery_voltage_queueID`：电池电压数据队列，供 `JoystickInputTask` 写入，`OLEDDisplayTask` 读取。
- `SLE_Transfer_QueueID`：摇杆百分比数据队列，供 `JoystickInputTask` 写入，`SELSendTask` 读取。
- `g_sle_uart_server_msgqueue_id`：SLE Server 端内部消息队列，供 SLE 协议栈相关任务使用。

### 互斥锁（Mutex）说明

- `g_mux_id`：用于保证 OLED 初始化和 ADC 初始化的顺序，防止并发访问导致的资源冲突。

### 主要流程与交互

1. **系统启动**：初始化 GPIO、队列、互斥锁，依次创建各任务。
2. **摇杆校准**：上电后进入校准，移动摇杆采集最大值，按 KEY3 结束校准。
3. **数据采集与显示**：ADC 任务持续采集数据，OLED 任务实时显示，支持内容切换。
4. **数据传输**：SELSendTask 任务将百分比数据通过 SLE 实时上报，Server 端断开自动重连广播。
5. **异常处理**：各任务均有错误检测与日志输出，提升系统健壮性。

### 其他细节

- 支持通过修改 `sle_local_name` 更改 SLE 设备名。
- 电池电压采集采用分压方式，已在代码中做换算。
- 按键 KEY3 既用于校准结束，也用于显示切换。
- 所有任务均有详细日志输出，便于调试。


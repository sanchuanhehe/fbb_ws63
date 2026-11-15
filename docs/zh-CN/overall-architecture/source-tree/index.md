# 目录结构

> SDK (Software Development Kit) 源码的组织方式及各目录用途

```
src/
├── application/
│   ├── ws63/              # 主应用工程入口（main.c）
│   └── samples/            # 示例代码（peripheral / bt / wifi / radar）
├── kernel/
│   ├── liteos/             # WS63 应用固件使用的 LiteOS 内核
│   ├── non_os/             # Boot/辅助镜像使用的裸机运行库
│   ├── osal/               # OSAL 公共头文件及 LiteOS 实现
│   └── osal_adapt/         # SDK 子系统使用的 OSAL 适配辅助组件
├── drivers/
│   ├── chips/              # 芯片级驱动（WS63 寄存器基址、中断映射）
│   ├── drivers/            # 外设驱动（gpio / uart / spi / i2c / pwm / adc / dma …）
│   ├── hal/                # 硬件抽象层
│   ├── boards/             # 板级配置（引脚复用、IO 供电）
│   └── adapter/            # OS 适配层
├── middleware/
│   ├── chips/              # 芯片适配（NV / 分区 / 异常处理）
│   ├── services/           # 能力封装（Wi-Fi 服务 / Matter）
│   └── utils/              # 通用工具（AT 命令 / DFX 诊断 / 升级 / CRC）
├── protocol/
│   ├── bt/                 # 蓝牙协议栈
│   ├── wifi/               # Wi-Fi 协议栈
│   └── radar/              # 雷达协议栈
├── open_source/
│   ├── lwip/               # lwIP 协议栈
│   ├── mbedtls/            # mbedTLS 安全传输
│   ├── cjson/              # cJSON 解析
│   ├── lvgl/               # LVGL 图形库
│   └── littlefs/           # LittleFS 文件系统
├── interim_binary/          # 预编译库及不可见源码组件
├── build/
│   ├── config/             # target 定义（config.py）与 Kconfig 菜单（.config）
│   └── cmake/               # CMake 模块
├── include/                 # 驱动 UAPI 与中间件公开头文件入口
├── bootloader/
│   ├── flashboot_ws63/     # FlashBoot 引导
│   └── commonboot/         # 通用引导逻辑
└── tools/                   # 工具链与打包脚本
```

## 应用层

| 路径 | 说明 |
|------|------|
| `application/ws63/` | SDK 平台入口，含 `main.c`、`reset_vector.S` 和系统任务初始化；产品应用通常不直接修改 |
| `application/samples/` | SDK 内示例代码，按功能分 `peripheral/`、`bt/ble/`、`bt/sle/`、`wifi/`、`radar/` |


## 中间件层

| 路径 | 说明 |
|------|------|
| `middleware/chips/ws63/` | 芯片适配（NV (Non-Volatile)、分区、异常处理） |
| `middleware/services/` | 能力封装（Wi-Fi 服务、Matter 等） |
| `middleware/utils/` | 通用工具（AT 命令、DFX (Diagnostic & Feedback) 诊断、升级、CRC (Cyclic Redundancy Check)） |
| `protocol/bt/` | 蓝牙协议栈 |
| `protocol/wifi/` | Wi-Fi 协议栈 |
| `protocol/radar/` | 雷达协议栈 |
| `open_source/` | 开源组件：lwIP (Lightweight IP (Internet Protocol))、mbedTLS (mbed Transport Layer Security)、cJSON、MQTT (Message Queuing Telemetry Transport)、LVGL (Light and Versatile Graphics Library)、LittleFS |

## 内核与驱动层

| 路径 | 说明 |
|------|------|
| `kernel/liteos/` | WS63 应用固件使用的 LiteOS (Huawei LiteOS) 内核 |
| `kernel/non_os/` | FlashBoot、LoaderBoot 等辅助镜像使用的裸机运行库，不是产品应用的可选内核 |
| `kernel/osal/` | OSAL (Operating System Abstraction Layer) 公共头文件和 LiteOS 实现；应用代码只包含公共头文件 |
| `kernel/osal_adapt/` | SDK 其他子系统使用的 OSAL 适配辅助组件 |
| `drivers/chips/ws63/` | 芯片级驱动（寄存器基址、中断号映射） |
| `drivers/drivers/driver/` | 外设驱动（gpio、uart、spi、i2c、pwm、adc、dma…） |
| `drivers/drivers/hal/` | HAL (Hardware Abstraction Layer) 抽象——芯片无关的外设操作接口 |
| `drivers/boards/ws63/evb/` | 板级配置（引脚复用、内存布局） |
| `drivers/adapter/` | OS (Operating System) 适配层 |

## 可修改代码边界

| 区域 | 建议 |
|------|------|
| 外置工程 `main/`、`components/` | 产品业务代码的推荐位置，可独立版本管理和升级 SDK |
| `include/driver/`、`include/middleware/`、`kernel/osal/include/` | 应用可依赖的公开接口边界 |
| `application/samples/` | 学习和验证用途，可复制设计思路，不建议作为平台入口直接长期修改 |
| `drivers/`、`middleware/`、`protocol/`、`kernel/`、`bootloader/` | SDK 内部实现；修改后需要自行承担升级合并、兼容性和系统验证 |
| `interim_binary/` | 预编译组件，没有可修改源码；通过公开 API、配置项或回调扩展 |
| 芯片 ROM (Read-Only Memory) | 固化在芯片内，通过 ROM API、callback 和补丁机制使用，不能直接修改 |

调试时，`.elf`、`.map` 或调用栈中的地址可能落在 ROM 或预编译库中。此时应先判断符号所属边界，再决定查看源码、公开接口文档还是 ROM/库版本说明。

HAL、Porting、链接脚本和启动代码属于芯片或板级适配范围，不是产品应用接口。应用访问外设使用驱动 UAPI (Unified API)，操作系统能力使用 OSAL。

## 构建与配置

| 路径 | 说明 |
|------|------|
| `build/config/target_config/ws63/` | target 定义（`config.py`）、Kconfig 菜单（`.config`） |
| `build/cmake/` | CMake 模块（`build_component.cmake` 等） |
| `include/` | 公开头文件，按子系统分子目录 |

> 构建系统的详细说明见 [构建系统](../build-output/index.md)。


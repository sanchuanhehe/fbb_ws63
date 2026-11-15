# Kconfig 参考

> 类型：内容页（参考） | 更新：2026-06-15

## 学习目标

- 理解 Kconfig 在 WS63 SDK中的角色——控制编译选项
- 掌握 `fbb config <target>` 进入 menuconfig 的基本操作
- 理解 Kconfig 依赖关系和 `choice` 互斥陷阱

## 基本概念

### Kconfig 层级结构

顶层 `config.in` → 各模块 Kconfig（WiFi / SLE (SparkLink Low Energy) / BLE (Bluetooth Low Energy) / 外设）。`menuconfig` 图形化界面逐层展开。

### 常见配置项分类

| 分类 | 说明 |
|------|------|
| 系统配置 | OS (Operating System) 类型 / 堆大小 / 日志级别 |
| 连接配置 | SLE / BLE / WiFi 功能开关 |
| 外设配置 | UART (Universal Asynchronous Receiver/Transmitter) / GPIO (General Purpose Input/Output) / I2C (Inter-Integrated Circuit) 引脚分配 |

### `choice` 互斥陷阱

多个 `choice` 项不能同时选——如"SPI (Serial Peripheral Interface) Device"和"SPI Host"互斥。开启新选项时需显式关闭同 choice 组内的其他选项。

## 涉及命令

| 命令 | 用途 |
|------|------|
| `fbb config <target>` | 进入 menuconfig |
| `fbb build <target>` | 使用当前 .config 编译 |

## 关键配置示例

```ini
CONFIG_SLE_SUPPORT=y
CONFIG_BLE_SUPPORT=y
CONFIG_WIFI_STA_AUTO_RECONNECT=y
CONFIG_LOG_LEVEL=INFO
```


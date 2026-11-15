<a id="1"></a>
# 产品概述

<a id="1.1"></a>
## 概述

WS63 系列芯片是一款高度集成的2.4GHz SoC Wi-Fi、BLE和SLE的Combo芯片，集成IEEE 802.11b/g/n/ax基带和RF电路，RF电路包括功率放大器PA、低噪声放大器LNA、RF balun、天线开关以及电源管理等模块；支持HT 20MHz/40MHz、HE 20MHz标准带宽，提供最大150Mbit/s物理层速率。

WS63 系列芯片Wi-Fi基带支持正交频分多址（OFDMA）技术，正交频分复用（OFDM）技术，并向下兼容直接序列扩频（DSSS）和补码键控（CCK）技术，支持IEEE 802.11b/g/n协议的各种数据速率，支持IEEE 802.11ax协议的MCS0~MCS9速率。

WS63 系列芯片支持BLE 1MHz/2MHz频宽，支持BLE 5.4协议，支持BLE Mesh和BLE网关功能，最大空口速率2Mbps。

WS63 系列芯片支持SLE 1MHz/2MHz/4MHz频宽，支持SLE1.0协议，支持SLE网关功能，WS63 最大空口速率4Mbps，WS63E最大空口速率12Mbps。

WS63 系列芯片集成高性能32bit微处理器、硬件安全引擎以及丰富的外设接口，外设接口包括SPI、QSPI、UART、I2C、PWM、GPIO和多路ADC；芯片内置SRAM和Flash，可独立运行，并支持在Flash上运行程序。

WS63E 支持雷达感知功能，智能感知房间内是否有人。

WS63 系列芯片支持OpenHarmony和第三方组件，并配套提供开放、易用的开发和调试运行环境。

WS63 系列芯片适应于智能家电等物联网智能终端领域。

<a id="1.2"></a>
## 功能描述

<a id="1.2.1"></a>
### 功能特性

#### Wi-Fi

- 1×1 2.4GHz频段。
- PHY支持IEEE 802.11b/g/n/ax。

MAC支持IEEE 802.11d/e/i/k/v/r/w。

- 支持802.11n 20MHz/40MHz频宽，支持802.11ax 20MHz频宽。
- 支持最大速率：150Mbit/s@HT40 MCS7，114.7Mbit/s@HE20 MCS9。
- 内置PA和LNA，集成TX/RX Switch、Balun等。
- 支持STA和SoftAP形态，作为SoftAP时最大支持6个STA接入。
- 支持A-MPDU、A-MSDU。
- 支持Block-ACK。
- 支持QoS，满足不同业务服务质量需求。
- 支持WPA/WPA2/WPA3 personal、WPS2.0、WAPI。
- 支持RF自校准方案。
- 支持STBC和LDPC。
- 支持雷达感知功能(仅WS63E芯片支持)。

#### 蓝牙

- 低功耗蓝牙Bluetooth Low Energy (BLE)。
- 支持BLE 5.4。
- 支持125Kbit/s、500Kbit/s、1Mbit/s、2Mbit/s速率。
- 支持多路广播。
- 支持Class 1。
- 支持高功率20dBm。
- 支持BLE Mesh，支持BLE网关。

#### 星闪

- 星闪低功耗接入技术Sparklink Low Energy (SLE)。
- 支持SLE 1.0。
- 支持SLE 1MHz/2MHz/4MHz，最大空口速率12Mbit/s。
- 支持Polar信道编码。
- 支持SLE网关。

#### CPU子系统

- 高性能32bit微处理器，最大工作频率240MHz。
- 内嵌SRAM 606KB、ROM 300KB。
- 内嵌4MB Flash。

#### 外围接口

- 1个SPI接口、1个QSPI接口、2个I2C接口、1个I2S接口、3个UART接口、19个GPIO接口、6路ADC输入、8路PWM（注：上述接口通过复用实现）。
- 外部晶体时钟频率24MHz、40MHz。

#### 其他信息

- 电源电压输入：典型值3.3V/5V。

    IO电源电压支持1.8V/3.3V，外接MCU和调试的UART支持5V tolerant。

- 封装：QFN-40，5mm×5mm。
- 工作温度：-40℃～+85℃。

<a id="1.2.2"></a>
### 遵从的标准与协议

WS63 系列芯片支持以下标准协议：

- 802.11-2020 Wireless LAN Medium Access Control(MAC) and Physical Layer (PHY) Specifications.
- Bluetooth Core Specification Version 5.4.
- Sparklink Wireless Communication Low Enery Air-interface Technical Requirements V01.00.

<a id="1.3"></a>
## 逻辑框图

WS63 系列芯片逻辑框图如图1-1所示。

图1-1 WS63 系列芯片逻辑框图

<div style="text-align:left; margin:10px 0;"><img src="images/图1-1 WS63 系列芯片逻辑框图.jpg" alt="" style="max-width:100%; width:720px; height:auto;"></div>

其中，图中的各模块功能描述如表1-1所示。

表1-1 模块功能描述

| 模块名 | 功能描述 |
| --- | --- |
| PMU | 电源管理单元。 |
| REF | 电压参考。 |
| UVLO/OVP/PWR RESET | 欠压/过压保护、电源复位。 |
| LDO | 低压差线性稳压器。 |
| PWRON DET | 上电检测。 |
| CMU | 时钟管理单元。 |
| XO | 晶体振荡器。 |
| Clock Divider | 时钟分频器。 |
| PLL | 锁相环。 |
| Clock Driver | 时钟驱动器。 |
| CPU | 中央处理单元。 |
| DMA | 直接存储器访问单元。 |
| SEC SUBSYS | 安全子系统。 |
| RAM | 随机存取存储器。 |
| ROM | 只读存储器。 |
| QSPI | 4线SPI。 |
| WDT | 看门狗单元。 |
| Timer | 定时器。 |
| RTC | 实时时钟单元。 |
| EFUSE | 加解密和芯片 ID存储。 |
| TSENSOR | 温度传感器。 |
| HPM | 工艺监视单元。 |
| Wi-Fi | WiFi通信模块。 |
| BLE | 低功耗蓝牙通信模块。 |
| SLE | SLE通信模块。 |
| PHY | 信道调制、解调。 |
| MAC | MAC层业务处理。 |
| RF&ABB | 射频&模拟模块。 |
| GPIO | 通用输入输出接口。 |
| UART | 通用异步串行接口控制器。 |
| SPI | 串行外设接口控制器。 |
| PWM | 脉冲宽度调制单元。 |
| LSADC | 低速ADC。 |
| I2C | 集成电路互连总线控制器。 |
| I2S | 集成电路内置音频总线控制器。 |
| FLASH | 闪存。 |
| PSRAM | 伪静态存储器。 |

<a id="1.4"></a>
## 应用场景

WS63 系列芯片适应于智能家电等物联网智能终端领域，典型应用框图如图1-2所示。

图1-2 WS63 系列芯片典型应用框图

<div style="text-align:left; margin:10px 0;"><img src="images/图1-2 WS63 系列芯片典型应用框图.jpg" alt="" style="max-width:100%; width:720px; height:auto;"></div>

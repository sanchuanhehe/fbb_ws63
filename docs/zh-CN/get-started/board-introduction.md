---
title: 开发板选型
description: WS63系列开发板对比选型及详细参数
---

# 开发板选型

WS63 系列支持多种开发板，涵盖传感器、显示、音频、AI (Artificial Intelligence) 等不同场景。

## 开发板概览

| 开发板 | 厂商 | 芯片 | 核心亮点 | 购买 |
|--------|------|:---:|------|------|
| [DyCloud_WF6301_DK](#dycloud_wf6301_dk) | 鼎云物联 | WS63 | 2.01" TFT (Thin Film Transistor) 触摸屏、WS2812B 灯带×8、温湿度/加速度计、EEPROM (Electrically Erasable Programmable Read-Only Memory)、蜂鸣器、16MB 外扩 Flash | [京东](https://ic-item.jd.com/10151635371214.html){ target=_blank } |
| [FS-Hi3863](#hqyj_ws63) | 华清远见 | WS63 | 2.8" TFT 触摸屏、RGB LED (Light Emitting Diode) 、温湿度/光感/接近传感器、NFC (Near Field Communication)，配套 20+ 实验教程 | [淘宝](https://item.taobao.com/item.htm?id=892481769813){ target=_blank } / [京东](https://ic-item.jd.com/10152445103343.html){ target=_blank } |
| [FB36 星闪开发板](#fb36) | 利尔达 | WS63 | Arduino Uno 兼容引脚排列，可选音频 Codec（AI 语音版） | [淘宝](https://item.taobao.com/item.htm?id=787599569531){ target=_blank } |
| [MYF-F63AI01](#myf-f63ai01) | 明裕丰 | WS63 | 板载音频 Codec 和麦克风，面向语音交互场景 | [淘宝](https://item.taobao.com/item.htm?id=922405424292){ target=_blank } |
| [MYF-F63VA01](#myf-f63va01) | 明裕丰 | WS63 | GPIO/UART/PWM/I2C/SPI 全引出，覆盖基本外设例程 | [淘宝](https://item.taobao.com/item.htm?id=893223665987){ target=_blank } |
| [HiHope_NearLink_DK3863E_V03](#hihope_nearlink_dk3863e_v03) | 润和软件 | WS63E | 核心板+底板+子板(OLED/交通灯/环境监测)，支持雷达人体感知 | [淘宝](https://e.tb.cn/h.TyIdVOFouZyhA23?tk=vPA6eoh0e0u){ target=_blank } / [京东](https://ic-item.jd.com/10150874487392.html){ target=_blank } |
| [HH-D111 WS63E 星闪开发板](#hh-d111) | 润和软件 | WS63E | 40pin GPIO (General Purpose Input/Output) 全引出、双天线(PCB (Printed Circuit Board)+IPEX (IPEX Connector){ target=_blank } |
| [KHD-3863B 星闪开发板](#khd-3863b) | 深开鸿 | WS63 | 52×25.5mm 最小体积、单排 25pin 全 GPIO 引出，预装 KaihongOS | [开鸿商城](https://mall.kaihong.com/productDetail?skuId=1839195267891466241&goodsId=1839195267383955458){ target=_blank } |
| [BearPi-Pico H3863](#bearpi-pico-h3863) | 小熊派 | WS63 | Pico 紧凑形态，USB (Universal Serial Bus) Type-C 供电烧录一体 | [淘宝](https://item.taobao.com/item.htm?id=821386760379){ target=_blank } |
| [DFRobot_Beetle_WS63](#dfrobot_beetle_ws63) | DFRobot | WS63 | Beetle 极简设计，板载 BME680 温湿度/气压传感器 | — |
| [WCL-63X-DK01](#wcl-63x-dk01) | 万创 | WS63 | 全功能——40pin GPIO 引出、双天线、I2C (Inter-Integrated Circuit) SHT30 传感器 | [淘宝](https://item.taobao.com/item.htm?id=1037395332271){ target=_blank } |

## 开发板详细参数

### DyCloud_WF6301_DK {#dycloud_wf6301_dk}

| 类别 | 参数 | 详情 |
|------|------|------|
| 芯片 | 型号 | WS63 |
| 板载外设 | 显示屏 | 2.01" TFT 触摸屏（240×296，多点触控） |
| | LED | WS2812B RGB 灯带 ×8（1600 万色，单线控制） |
| | 按键 | 复位键 + 用户按键 ×1 |
| | 传感器 | CHT20 温湿度（±0.3°C / ±2%RH）、SC7A20 三轴加速度计（±2g~±16g） |
| | 存储 | AT24C02 EEPROM 2Kbit（I2C）、W25Q128 外扩 NOR Flash 16MB |
| | 音频 | 无源蜂鸣器 |
| 外部接口 | USB | Type-C ×1（供电+烧录+串口） |
| | GPIO | 2.54mm 排针，19 路 GPIO 引出 |
| | 天线 | 板载 PCB 天线 |
| 调试烧录 | 串口芯片 | CH340（USB 转 UART (Universal Asynchronous Receiver/Transmitter)） |
| | SWD (Serial Wire Debug) | SWCLK=GPIO14，SWDIO=GPIO13 |
| 硬件资料 | 原理图 | [DyCloud_WF6301_DK V1.1.pdf](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/DyCloud_WF6301_DK%20V1.0/doc/hardware/DyCloud_WF6301_DK%20V1.1.pdf){ target=_blank } |
| | 使用手册 | [DyCloud_WF6301 DK 使用手册](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/DyCloud_WF6301_DK%20V1.0/doc/DyCloud_WF6301%20DK%20%E4%BD%BF%E7%94%A8%E6%89%8B%E5%86%8C.md){ target=_blank } |


### FS-Hi3863 {#hqyj_ws63}

| 类别 | 参数 | 详情 |
|------|------|--------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 芯片 | 型号 | WS63 |
| 板载外设 | 显示屏 | 2.8" TFT LCD (SPI (Serial Peripheral Interface)) + FT6336 触摸（I2C）、0.96" OLED（SSD1306，I2C） |
| | LED | AW2013 RGB LED 驱动芯片（I2C） |
| | 按键 | 用户按键 |
| | 传感器 | SHT20 温湿度（-40~+125°C）、AP3216C 光感/红外接近/人体接近 |
| | 其他 | NFC 标签读写模块 |
| 外部接口 | USB | Type-C ×1（供电+烧录+串口） |
| | GPIO | 排针引出，19 路 GPIO（I2C/SPI/UART/PWM/ADC 复用） |
| | 天线 | 板载 PCB 天线 |
| 调试烧录 | 串口芯片 | CH340（USB 转 UART） |
| | 烧录方式 | 拨码开关切换启动模式、BurnTool_H3863 |
| 结构 | 特色 | 配套 20+ 实验教程（OS/外设/WiFi/BLE/SLE/MQTT/华为云） |
| 硬件资料 | 原理图 | [FS-WS63-原理图V2.pdf](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/Hqyj_Ws63/doc/hardware/FS-WS63-%E5%8E%9F%E7%90%86%E5%9B%BEV2.pdf){ target=_blank } |


### FB36 星闪开发板 {#fb36}

| 类别 | 参数 | 详情 |
|------|------|------|
| 芯片 | 型号 | WS63 |
| 板载外设 | LED | 用户 LED (GPIO4) |
| | 按键 | 用户按键（GPIO12） |
| | 音频 | 可选音频 Codec + 麦克风（AI 语音版） |
| 外部接口 | USB | Type-C |
| | GPIO | Arduino Uno 兼容排针，19 路 GPIO（I2C/SPI/UART/PWM/ADC 复用） |
| | 天线 | 板载 PCB 天线 |
| 调试烧录 | 串口芯片 | USB 转 UART |
| 硬件资料 | 原理图 | [FB36 duino开发板原理图参考_Rev1.0.pdf](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/Lierda-FB36/doc/hardware/FB36%20duino%E5%BC%80%E5%8F%91%E6%9D%BF%E5%8E%9F%E7%90%86%E5%9B%BE%E5%8F%82%E8%80%83_Rev1.0.pdf){ target=_blank } |
| | 手册 | [FB36 duino开发板硬件说明书_Rev1.0.pdf](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/Lierda-FB36/doc/hardware/FB36%20duino%E5%BC%80%E5%8F%91%E6%9D%BF%E7%A1%AC%E4%BB%B6%E8%AF%B4%E6%98%8E%E4%B9%A6_Rev1.0.pdf){ target=_blank }、[FB36 系列模组硬件设计手册_Rev1.1.pdf](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/Lierda-FB36/doc/hardware/Lierda%20FB36%E7%B3%BB%E5%88%97%E6%A8%A1%E7%BB%84%E7%A1%AC%E4%BB%B6%E8%AE%BE%E8%AE%A1%E6%89%8B%E5%86%8C_Rev1.1.pdf){ target=_blank } |
| | 使用手册 | [Lierda FB36 开发板示例说明](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/Lierda-FB36/README.md){ target=_blank }（含 LED / PWM (Pulse Width Modulation) / Key / SLE (SparkLink Low Energy) UART 示例） |
| | 快速上手 | [Lierda FB36 快速上手应用指导_Rev1.0](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/Lierda-FB36/doc/Document/Lierda%20FB36%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E5%BA%94%E7%94%A8%E6%8C%87%E5%AF%BC_Rev1.0(1).pdf){ target=_blank } |


### MYF-F63AI01 {#myf-f63ai01}

| 类别 | 参数 | 详情 |
|------|------|------|
| 芯片 | 型号 | WS63 |
| 板载外设 | 音频 | 板载音频 Codec + 麦克风，面向 AI 语音交互 |
| | LED | 用户 LED (GPIO2) |
| | 按键 | 用户按键（GPIO12） |
| 外部接口 | USB | Type-C |
| | GPIO | 排针引出（UART TX=pin26 / RX=pin27，I2C/SPI/PWM 复用） |
| | 天线 | 板载 PCB 天线 |
| 调试烧录 | 串口芯片 | USB 转 UART |
| 硬件资料 | 原理图 | 参见 [MYF-F63 硬件资料](https://gitcode.com/HiSpark/fbb_ws63/tree/master/vendor/MYF_F63/doc/hardware){ target=_blank } |
| | 使用手册 | [MYF-F63 开发板示例说明](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/MYF_F63/README.md){ target=_blank }（含 Blinky / Button / PWM / UART / DMA (Direct Memory Access) / Timer / Watchdog / Systick 示例） |
| | 产品图片 | [MYF-F63AI01开发板.png](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/MYF_F63/doc/media/MYF-F63AI01%E5%BC%80%E5%8F%91%E6%9D%BF.png){ target=_blank } |


### MYF-F63VA01 {#myf-f63va01}

| 类别 | 参数 | 详情 |
|------|------|------|
| 芯片 | 型号 | WS63 |
| 板载外设 | LED | 用户 LED（GPIO2，支持 PWM 呼吸灯） |
| | 按键 | 用户按键（GPIO12，支持中断输入） |
| 外部接口 | USB | Type-C |
| | GPIO | 排针引出（UART TX=pin26 / RX=pin27，I2C/SPI/PWM 复用） |
| | 天线 | 板载 PCB 天线 |
| 调试烧录 | 串口芯片 | USB 转 UART |
| 硬件资料 | 原理图 | [MYF-F63VA01-DK-1V0开发板原理图_BIU20250513.pdf](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/MYF_F63/doc/hardware/MYF-F63VA01-DK-1V0%E5%BC%80%E5%8F%91%E6%9D%BF%E5%8E%9F%E7%90%86%E5%9B%BE_BIU20250513.pdf){ target=_blank } |
| | 使用手册 | [明裕丰MYF-F63VA01-DK开发板用户使用指南_V1.2.pdf](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/MYF_F63/doc/%E6%98%8E%E8%A3%95%E4%B8%B0MYF-F63VA01-DK%E5%BC%80%E5%8F%91%E6%9D%BF%E7%94%A8%E6%88%B7%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97_V1.2.pdf){ target=_blank } |

### HiHope_NearLink_DK3863E_V03 {#hihope_nearlink_dk3863e_v03}

| 类别 | 参数 | 详情 |
|------|------|------|
| 芯片 | 型号 | WS63E（支持雷达人体感知，最远 6m） |
| 板载外设 | 显示屏 | SSD1306 OLED 128×64（I2C，子板） |
| | LED | 红/黄/绿交通灯 LED ×3（子板） |
| | 按键 | 用户按键 ×1（GPIO14，子板） |
| | 传感器 | AHT20 温湿度（子板）、LSM6DSM 陀螺仪/加速度计（SPI，扩展） |
| | 音频 | 无源蜂鸣器（子板） |
| | 其他 | SK6812 RGB LED、HCSR04 超声波测距、SG92R 舵机（扩展支持） |
| 外部接口 | USB | Type-C ×1（供电+烧录+串口） |
| | GPIO | 底板排针引出，19 路 GPIO（I2C/SPI/UART/PWM/ADC） |
| | 天线 | 板载 PCB 天线 |
| 调试烧录 | 串口芯片 | CH341SER（USB 转 UART） |
| | SWD | SWCLK=GPIO14，SWDIO=GPIO13 |
| 结构 | 形态 | 核心板 + 底板 + 子板模块化设计（交通灯板/OLED板/环境监测板可替换） |
| 硬件资料 | 原理图 | [核心板](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/HiHope_NearLink_DK_WS63E_V03/doc/hardware/HIHOPE_NEARLINK_DK_3863E_V03.pdf){ target=_blank }、[底板](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/HiHope_NearLink_DK_WS63E_V03/doc/hardware/HiSpark_WiFi_IoT_EXB_VER.A.pdf){ target=_blank }、[OLED板](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/HiHope_NearLink_DK_WS63E_V03/doc/hardware/HiSpark_WiFi_IoT_OLED_VER.A.pdf){ target=_blank }、[交通灯板](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/HiHope_NearLink_DK_WS63E_V03/doc/hardware/HiSpark_WiFi_IoT_SSL_VER.A.pdf){ target=_blank }、[环境板](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/HiHope_NearLink_DK_WS63E_V03/doc/hardware/HiSpark_WiFi_IoT_EM_VER.A.pdf){ target=_blank } |
| | 使用手册 | [星闪实验指导手册](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/HiHope_NearLink_DK_WS63E_V03/doc/%E6%98%9F%E9%97%AA%E5%AE%9E%E9%AA%8C%E6%8C%87%E5%AF%BC%E6%89%8B%E5%86%8C.md){ target=_blank } |


### HH-D111 WS63E 星闪开发板 {#hh-d111}

| 类别 | 参数 | 详情 |
|------|------|------|
| 芯片 | 型号 | WS63E（支持雷达人体感知） |
| 板载外设 | LED | 电源 LED（绿）+ 用户 LED（红，GPIO12） |
| | 按键 | 用户按键 S1 (GPIO0) + 复位键 S2 |
| 外部接口 | USB | Type-C ×1（供电+烧录+串口） |
| | GPIO | 40pin 排针（2×20），19 路 GPIO，含 I2C×2/SPI×1/UART×3/PWM×8/ADC×6/I2S×1 |
| | 天线 | 板载 PCB 天线 + IPEX 外接天线座（电阻切换） |
| 调试烧录 | 串口芯片 | CH340（USB 转 UART） |
| | SWD | SWCLK=GPIO14，SWDIO=GPIO13 |
| 硬件资料 | 原理图 | [HH-D111 星闪开发板原理图_V.25.07.22.pdf](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/HH-D111/doc/hardware/HH-D111%20%E6%98%9F%E9%97%AA%E5%BC%80%E5%8F%91%E6%9D%BF%E5%8E%9F%E7%90%86%E5%9B%BE_V.25.07.22.pdf){ target=_blank } |
| | 使用手册 | [HH-D111 星闪开发板使用说明书_V26.02.26.pdf](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/HH-D111/doc/hardware/HH-D111%20%E6%98%9F%E9%97%AA%E5%BC%80%E5%8F%91%E6%9D%BF%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E%E4%B9%A6_V26.02.26.pdf){ target=_blank }（含完整引脚功能表） |
| | 规格书 | [HH-D111 星闪开发板规格说明书](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/HH-D111/doc/README.md){ target=_blank } |


### KHD-3863B 星闪开发板 {#khd-3863b}

| 类别 | 参数 | 详情 |
|------|------|------|
| 芯片 | 型号 | WS63（Hi3863V100，240MHz） |
| 板载外设 | 按键 | RST 复位键 + RECOVERY 烧录键（上电时按住进入烧录模式） |
| 外部接口 | USB | Type-C ×1（供电+烧录+串口） |
| | GPIO | 单排 25pin，含 GPIO×15（PWM/SPI/I2S/ADC/UART2/JTAG 复用）+ UART1 + 5V/GND |
| | 天线 | 板载天线 |
| 调试烧录 | 串口芯片 | CH340K（USB 转 UART） |
| | SWD | SWCLK=GPIO14（排针第 8 脚），SWDIO=GPIO13（排针第 9 脚） |
| 结构 | 尺寸 | 52 × 25.5 × 1.6mm（2 层板），最小体积 |
| | OS (Operating System) | 预装 KaihongOS Lite（基于 OpenHarmony） |
| 硬件资料 | 原理图 | [开鸿KHD-3863B星闪开发板.pdf](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/Kaihong_KHD-3863B/doc/hardware/%E5%BC%80%E9%B8%BFKHD-3863B%E6%98%9F%E9%97%AA%E5%BC%80%E5%8F%91%E6%9D%BF.pdf){ target=_blank }（含完整引脚复用表） |


### BearPi-Pico H3863 {#bearpi-pico-h3863}

| 类别 | 参数 | 详情 |
|------|------|------|
| 芯片 | 型号 | WS63（Hi3863 RISC-V，240MHz） |
| 板载外设 | LED | 电源 LED（红）+ 用户 LED（蓝） |
| | 按键 | 复位键 |
| 外部接口 | USB | Type-C ×1（供电+烧录+串口） |
| | GPIO | 排针引出 17 路 GPIO，含 I2C×2/SPI×2/QSPI×1/UART×3/PWM×8/ADC×6/I2S×1 |
| | 天线 | 板载 PCB 天线 + IPX 外接天线座（电阻切换） |
| 调试烧录 | 串口芯片 | USB 转 UART（Type-C 内置） |
| | SWD | SWCLK=GPIO14，SWDIO=GPIO13 |
| 硬件资料 | 原理图 | [BearPi-H3863_Pico.pdf](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/BearPi-Pico H3863/doc/hardware/BearPi-H3863_Pico.pdf){ target=_blank } |
| | 使用手册 | [BearPi-Pico H3863 开发板示例说明](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/BearPi-Pico_H3863/README.md){ target=_blank }（含 Blinky / Button / PWM / UART / I2C / SPI / ADC (Analog-to-Digital Converter) / Timer / Watchdog 等 17 个示例） |

### DFRobot_Beetle_WS63 {#dfrobot_beetle_ws63}

| 类别 | 参数 | 详情 |
|------|------|------|
| 芯片 | 型号 | WS63 |
| 板载外设 | LED | 用户 LED (GPIO2) |
| | 按键 | 用户按键（GPIO12，下降沿中断） |
| 外部接口 | USB | Type-C ×1（供电+烧录+串口） |
| | 天线 | 板载 PCB 天线 |
| 调试烧录 | 串口芯片 | USB 转 UART |
| 硬件资料 | 原理图 | [[DFR1232]Beetle WS63(V0.0.1).pdf](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/DFRobot_Beetle_WS63/doc/hardware/%5BDFR1232%5DBeetle%20WS63%28V0.0.1%29.pdf){ target=_blank } |
| | 使用手册 | [Beetle WS63 部署教程](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/DFRobot_Beetle_WS63/doc/readme.md){ target=_blank }（含 Button / BLE (Bluetooth Low Energy) UART / SLE UART / MQTT (Message Queuing Telemetry Transport) / BME680 / OLED 等 11 个示例） |

### WCL-63X-DK01 {#wcl-63x-dk01}

| 类别 | 参数 | 详情 |
|------|------|------|
| 芯片 | 型号 | WS63 |
| 板载外设 | LED | 用户 LED (GPIO2) |
| | 按键 | 用户按键（GPIO0，上拉，下降沿中断） |
| 外部接口 | USB | Type-C |
| | GPIO | 40pin 排针，19 路 GPIO 引出 |
| | 天线 | 板载 PCB 天线 + IPEX 外接天线座 |
| 调试烧录 | 串口芯片 | USB 转 UART |
| 硬件资料 | 原理图 | [WCL-63X-DK01开发板原理图.pdf](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/Waveconn_WCL-63X-DK01/doc/hardware/WCL-63X-DK01%E5%BC%80%E5%8F%91%E6%9D%BF%E5%8E%9F%E7%90%86%E5%9B%BE.pdf){ target=_blank } |
| | 使用手册 | [WCL-63X-DK01 开发板手册_V1.1.pdf](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/Waveconn_WCL-63X-DK01/doc/WCL-63X-DK01%E5%BC%80%E5%8F%91%E6%9D%BF%E6%89%8B%E5%86%8C_V1.1.pdf){ target=_blank } |
| | 搭建指南 | [开发环境搭建指南](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/Waveconn_WCL-63X-DK01/doc/%E5%BC%80%E5%8F%91%E7%8E%AF%E5%A2%83%E6%90%AD%E5%BB%BA%E6%8C%87%E5%8D%97.md){ target=_blank } |

## 开源硬件概览

以下项目为社区开源硬件，硬件设计文件公开，可供学习和参考。

| 项目 | 作者 | 芯片 | 核心亮点 | 硬件资料 |
|------|------|:---:|------|------|
| DUT_Car | 大连理工大学 | WS63 | 智能小车——SSD1306 OLED、WS2812B 灯带、AHT20/BH1750/BMX055 传感器、STM8S 电机 | [原理图](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/DUT_Car/doc/hardware/SCH_BaseBoard_2025-05-15.pdf){ target=_blank }、[实验手册](https://gitcode.com/HiSpark/fbb_ws63/blob/master/vendor/DUT_Car/doc/dut_car%E5%AE%9E%E9%AA%8C%E6%8C%87%E5%AF%BC%E6%89%8B%E5%86%8C.md){ target=_blank } |

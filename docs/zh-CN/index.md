---
hide:
  - toc
---

# WS63 系列开发指南

<div class="hero-banner" markdown>

<div class="hero-left" markdown>

**Wi-Fi 6 + SLE (SparkLink Low Energy) 1.0 + BLE (Bluetooth Low Energy) 5.4 三模解决方案**

</div>

</div>

## 芯片介绍

WS63 系列是一款高度集成的 2.4GHz Combo 芯片，同时支持 Wi-Fi 6、SLE 1.0、 BLE 5.4 三种无线协议，内置 32bit 处理器、硬件安全引擎及丰富外设。

### 关键参数

| 类别 | 参数 | WS63 | WS63E |
|------|------|:---:|:---:|
| Wi-Fi | 协议 | 802.11b/g/n/ax（PHY (Physical Layer)），802.11d/e/i/k/v/r/w（MAC (Media Access Control)） | 同 WS63 |
| | 频宽 / 速率 | HT20/40 MCS7 150Mbps，HE20 MCS9 114.7Mbps | 同 WS63 |
| | 模式 | STA (Station) + SoftAP（最大 6 路 STA 接入） | 同 WS63 |
| | 安全 | WPA (Wi-Fi Protected Access) / WPA2 (Wi-Fi Protected Access 2) / WPA3 (Wi-Fi Protected Access 3) Personal、WPS2.0、WAPI (WLAN Authentication and Privacy Infrastructure) | 同 WS63 |
| BLE | 协议 | BLE 5.4 | 同 WS63 |
| | 速率 | 125K / 500K / 1M / 2Mbps，高功率 20dBm | 同 WS63 |
| | 特性 | 网关、多路广播 | 同 WS63 |
| SLE | 协议 | SLE 1.0 | 同 WS63 |
| | 频宽 / 速率 | 1 / 2 / 4 MHz，最大 12 Mbps，Polar 信道编码 | 同 WS63 |
| | 特性 | SLE 网关 | 同 WS63 |
| 处理器 | 内核 | 32bit，240MHz | 同 WS63 |
| 存储 | 内置 | SRAM (Static Random Access Memory) 606KB + ROM (Read-Only Memory) 300KB + 4MB Flash | 同 WS63 |
| 外设 | 接口 | 1×SPI (Serial Peripheral Interface)、1×QSPI (Quad Serial Peripheral Interface)、2×I2C (Inter-Integrated Circuit)、1×I2S (Inter-IC Sound)、3×UART (Universal Asynchronous Receiver/Transmitter)<br/>19×GPIO (General Purpose Input/Output)、6×ADC (Analog-to-Digital Converter)、8×PWM（复用） | 同 WS63 |
| 雷达 | 人体感知 | — | **支持** |
| 电源 | 输入 | 3.3V / 5V，IO 支持 1.8V / 3.3V，UART 5V tolerant | 同 WS63 |
| 封装 | 尺寸 | QFN-40，5mm × 5mm | 同 WS63 |
| 温度 | 范围 | -40°C ~ +85°C | 同 WS63 |


---

## 开发流程

从快速入门选择 CLI 或 VS Code，完成第一次开发后再查阅参考案例和常见问题：

<nav class="grid cards" markdown>

-   [**1. 快速入门**](get-started/index.md)

    ---

    确认开发板、Target 和 Hello World 完成判据，选择开发方式。

-   [**2. 使用 CLI 完成第一次开发**](get-started/cli/index.md)

    ---

    使用 Windows PowerShell 或 Linux Bash 完成环境准备、配置、构建、烧录和串口验证。

-   [**3. 使用 VS Code 完成第一次开发**](get-started/vscode/index.md)

    ---

    使用 HiSpark Studio 插件新建工程开始，完成工程的创建、配置、编译、烧录，验证运行结果。

-   [**4. 开发指南**](guides/index.md)

    ---

    按专题了解功能配置、开发流程和验证方法，并阅读完整专题手册。

-   [**5. 参考案例**](samples/index.md)

    ---

    运行简单案例和实际场景案例，在可执行实现基础上继续开发。

-   [**6. 常见问题**](FAQ/index.md)

    ---

    开发过程中遇到问题？查看 FAQ (Frequently Asked Questions) 获取常见问题解答。

</nav>

---

## 应用案例

<nav class="grid cards" markdown>

-   [**AI 语音玩具方案**](https://developers.hisilicon.com/cn/caselibrary/ai_smart_voice_toy){ target=_blank }

    ---

    语音交互｜云端大模型｜星闪智联｜鸿蒙技术

    ---

    ![润开鸿 AI 语音玩具](https://developers.hisilicon.com/admin/asset/v1/pro/view/0aabf54362894430872f8a0eae0087d6.png)

    星闪+鸿蒙技术，低时延高可靠，多模型兼容，智能对话强。润开鸿基于WS63/WS63E提供完整软硬件方案。

-   [**星闪红外遥控车**](https://developers.hisilicon.com/cn/caselibrary/hqyj_smart_car){ target=_blank }

    ---

    星闪红外双模控制 | 寻迹避障 | 远程云控 | 语音交互 | 鸿蒙系统

    ---

    ![华清远见星闪红外遥控车](https://developers.hisilicon.com/admin/asset/v1/pro/view/8b29b75c0cf24137b2d822fd9cd35e22.webp)

    星闪红外双模控制，毫秒级指令响应。完整教学与自动巡线，高精度避障。华清远见基于WS63提供硬件 + 软件 + 教学资料一体化方案。

-   [**星闪Mesh智能家居互联方案**](https://developers.hisilicon.com/cn/caselibrary/wczn_mesh_connection){ target=_blank }

    ---

    无线组网 | 智慧联动 | 场景随心
    
    ---

    ![星闪Mesh智能家居互联方案](https://developers.hisilicon.com/admin/asset/v1/pro/view/9e50f8eddeb94b02bc2e2aa7881938ce.webp)

    即插即用，极简组网。远距离覆盖10000+节点，全程加密。问沧智能基于WS63提供全屋无线智能网络方案

</nav>

[查看更多案例](https://developers.hisilicon.com/cn/caselibrary/caseconstruction){ .md-button target=_blank  }

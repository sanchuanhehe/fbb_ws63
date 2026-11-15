<a id="2"></a>
# 系统

<a id="2.1"></a>
## 复位

<a id="2.1.1"></a>
### 概述

复位模块根据输入复位源产生各个模块的复位信号，支持整个芯片的全局复位和各个模块的单独复位。

复位实现为异步复位，同步撤离。

<a id="2.1.2"></a>
### 复位控制

复位控制的输入信号分为：

- 全局复位控制。
- 单独模块复位控制。

芯片可以使用的复位控制方式如表2-1所示。

表2-1 复位控制

| 复位方式 | 来源 | 复位时间 | 复位范围 | 复位后芯片模式 |
| --- | --- | --- | --- | --- |
| POR复位（Power on Reset） | PMU POR模块输出。 | 400μs | 全芯片 | WORK |
| Watch Dog复位控制 | Watch Dog模块输出，软件可以屏蔽。 | - | 全芯片/模块可配置 | WORK |
| 全局软复位控制 | 软件配置。 | - | 全芯片 | WORK |
| 模块软复位控制 | CRG（Clock and Reset Generator）寄存器，软件配置。 | 软件控制 | 模块 | 该模块处于复位状态，其余模块处于Work。 |

<a id="2.1.3"></a>
### 复位信号

表2-2 复位信号

| 信号名称 | 含义 | 复位时间 | I/O | 说明 |
| --- | --- | --- | --- | --- |
| PWR_ON | 芯片上下电管脚：1. 拉高PWR_ON管脚，芯片完成上电。2. 拉低PWR_ON管脚，芯片完成复位，并且下电。 | 1ms/5ms | I | - |

<a id="2.1.4"></a>
### 复位约束

芯片第一次上电，PWR_ON时间要大于1ms；芯片上电后，拉低PWR_ON再拉高PWR_ON的时间需要大于5ms。

<a id="2.2"></a>
## 时钟

<a id="2.2.1"></a>
### 概述

时钟管理模块对芯片时钟输入、生成、控制进行统一的管理，其功能包括：

- 时钟输入的管理和控制。
- 时钟高频与低频的切换。
- 时钟分频和控制。
- 时钟的门控控制。
- 各模块工作时钟的生成。

<a id="2.2.2"></a>
### 时钟分配

正常工作时，各模块的时钟分配如表2-3所示。

表2-3 模块时钟分配

| 模块名称 | 时钟频率（MHz） | 模块名称 | 时钟频率（MHz） |
| --- | --- | --- | --- |
| CPU | 240 | WDT（WatchDog） | 晶体分频 |
| CPU Bus | 240 | RTC（Real-Time Clock） | 0.032 |
| Timer | 晶体分频 | UART | 160 |
| GPIO | 120 | EFUSE | 晶体 |
| WiFi MAC | 120 | BT MAC | 32 |
| WiFi PHY | 320 | BT PHY | 32 |
| 晶体 | 40/24 | PMU OSC | 0.032 |
| I2C | 80 | SPI | 160 |
| QSPI | 64 | I2S | 8.192等 |

<a id="2.2.3"></a>
### 时钟控制

时钟管理模块主体包括：

- PLL频率控制。
- 时钟分频和时钟源选择控制。
- 时钟门控管理。

<a id="2.3"></a>
## 电源管理与低功耗模式控制

<a id="2.3.1"></a>
### 概述

芯片的低功耗模式用于有效减少芯片的功耗，芯片提供多种低功耗的控制来动态降低芯片的功耗：

- 系统工作模式控制
除了Work模式之外，各种模式对功耗都有一定的减少作用，可以根据实际的功耗要求和功能要求选择不同的工作模式。

- 时钟门控和时钟频率调整
- 提供时钟关断功能，可以关闭没有必要的时钟，减少芯片的功耗。
- 系统工作的时钟频率可以进行调整，在满足功能的情况下可以调节时钟频率，动态降低芯片功耗。
- 模块级低功耗控制
提供模块级的低功耗控制，可以在某模块不工作的情况下，关断该模块或使模块处于低功耗状态，以减少芯片的功耗。

<a id="2.3.2"></a>
### 系统工作模式

系统工作模式分为以下三种模式：

- Work
正常工作状态，所有电源供电均打开，完成正常的WiFi收发等业务。

- Light Sleep
浅睡模式为可快速恢复业务收发的睡眠模式，关闭收发时钟以降低功耗。此时CPU配置为WFI（Wait For Interrupt）模式，等待中断唤醒后恢复收发，Flash、IO保持供电，系统内发生任意中断均可唤醒CPU（如定时中断，外设通信中断，GPIO中断等）；SoC系统可选晶体或PLL时钟，时钟频率和外设的通信速率有关。

- Shutdown
关机模式，芯片通过PWR_ON管脚拉低之后进入Shutdown模式，整芯片下电。当PWR_ON管脚拉高后退出Shutdown模式，进入到Work模式。

<a id="2.4"></a>
## 处理器子系统

系统提供一个自研RISC-V处理器作为主控CPU，完成各种系统任务和控制工作。处理器带有32KB指令Cache、4KB数据Cache。

该芯片CPU具有以下功能特点：

- 处理器的工作频率最高可达240MHz。
- 支持直接模式和向量模式的中断方式，支持1个nmi中断，以及64个非标准外部中断。
- 支持Flash Patch功能，支持192个指令比较器和2个地址比较器。
- 支持边沿和电平两种中断触发方式。
- 支持PMP（Physical Memory Protection）功能。
- 支持JTAG和SWD（Serial Wire Debug）调试接口。

<a id="2.5"></a>
## 存储器空间映射

地址空间映射如表2-4所示。

表2-4 存储器地址空间映射

| Slave Name | 地址范围（Start） | 地址范围（End） |
| --- | --- | --- |
| CPU_ITCM | 0x0010_0000 | 0x0017_FFFF |
| CPU_DTCM | 0x0018_0000 | 0x001C_7FFF |
| \| \|-reserved (读取返回dead_beef) | 0x001C_8000 | 0x001F_FFFF |
| NOR_FLASH (读取范围超过Flash大小时，地址卷绕读取) | 0x0020_0000 | 0x009F_FFFF |
| SHARE_RAM | 0x00A0_0000 | 0x00A9_8FFF |
| \| \|-reserved (读取返回resp_error) | 0x00A9_9000 | 0x00BF_FFFF |
| PKE_ROM | 0x00C0_0000 | 0x00C0_0BFF |
| \| \|-reserved (读取返回随机值) | 0x00C0_0C00 | 0x00C0_0FFF |
| \| \|-reserved (读取返回resp_error) | 0x00C0_1000 | 0x3FFF_FFFF |
| SYS_CTL0 | 0x4000_0000 | 0x4000_3FFF |
| \| \|-reserved (读取返回dead_dead) | 0x4000_4000 | 0x4000_4FFF |
| RTC | 0x4000_5000 | 0x4000_50FF |
| \| \|-reserved (读取返回dead_dead) | 0x4000_5100 | 0x4000_5FFF |
| WDT | 0x4000_6000 | 0x4000_6FFF |
| \| \|-reserved (读取返回dead_dead) | 0x4000_7000 | 0x4000_FFFF |
| \| \|-reserved (读取返回resp_error) | 0x4001_0000 | 0x43FF_FFFF |
| SYS_CTL1 | 0x4400_0000 | 0x4400_1FFF |
| TIMER | 0x4400_2000 | 0x4400_2FFF |
| \| \|-reserved (读取返回dead_dead) | 0x4400_3000 | 0x4400_3FFF |
| SYS_CTL2 | 0x4400_4000 | 0x4400_7FFF |
| EFUSE_CTL | 0x4400_8000 | 0x4400_BFFF |
| LSADC_CTL | 0x4400_C000 | 0x4400_CFFF |
| IO_CONFIG | 0x4400_D000 | 0x4400_DFFF |
| TSENSOR_CTL | 0x4400_E000 | 0x4400_EFFF |
| \| \|-reserved (读取返回dead_dead) | 0x4400_F000 | 0x4400_FFFF |
| UART0 | 0x4401_0000 | 0x4401_0FFF |
| UART1 | 0x4401_1000 | 0x4401_1FFF |
| UART2 | 0x4401_2000 | 0x4401_2FFF |
| \| \|-reserved (读取返回dead_dead) | 0x4401_3000 | 0x4401_7FFF |
| I2C0 | 0x4401_8000 | 0x4401_80FF |
| I2C1 | 0x4401_8100 | 0x4401_81FF |
| \| \|-reserved (读取返回dead_dead) | 0x4401_8200 | 0x4401_FFFF |
| SPI | 0x4402_0000 | 0x4402_00FF |
| \| \|-reserved (读取返回dead_dead) | 0x4402_0100 | 0x4402_0FFF |
| QSPI | 0x4402_1000 | 0x4402_10FF |
| \| \|-reserved (读取返回dead_dead) | 0x4402_1100 | 0x4402_3FFF |
| PWM0~7 | 0x4402_4000 | 0x4402_4FFF |
| I2S | 0x4402_5000 | 0x4402_50FF |
| \| \|-reserved (读取返回dead_dead) | 0x4402_5100 | 0x4402_7FFF |
| GPIO0~7 | 0x4402_8000 | 0x4402_8FFF |
| GPIO8~15 | 0x4402_9000 | 0x4402_9FFF |
| GPIO16~18 | 0x4402_A000 | 0x4402_AFFF |
| \| \|-reserved (读取返回dead_dead) | 0x4402_B000 | 0x440F_FFFF |
| SEC_APB | 0x4410_0000 | 0x4411_4FFF |
| \| \|-reserved (读取返回dead_dead) | 0x4411_5000 | 0x4411_FFFF |
| WIFI_SUB | 0x4421_0000 | 0x4421_3FFF |
| \| \|-reserved (读取返回resp_error) | 0x4421_4000 | 0x47FF_FFFF |
| SFC_CFG | 0x4800_0000 | 0x4800_1FFF |
| \| \|-reserved (读取返回resp_error) | 0x4800_2000 | 0x48FF_FFFF |
| BSLE_SUB | 0x4900_0000 | 0x4903_FFFF |
| \| \|-reserved (读取返回resp_error) | 0x4904_0000 | 0x49FF_FFFF |
| DMA_CFG | 0x4A00_0000 | 0x4A00_0FFF |

<a id="2.6"></a>
## 中断系统

<a id="2.6.1"></a>
### 中断分配

芯片支持向量模式和直接模式的中断方式，支持边沿和电平两种中断触发方式。支持优先级可编程，优先级配置寄存器（共3bit）可配置7级的优先级。

中断系统包括：

- CPU的内部标准中断：中断编号0 ~ 25。
- CPU外部的非标准中断：所支持的非标准中断编号如表2-5所示。

表2-5 非标准中断编号列表

| Int No. | Int Name | Description |
| --- | --- | --- |
| nmi | TEE_NMI_INT | nmi软中断&WDT全局中断。 |
| 26 | TIMER_INT[0] | Timer[0]的全局中断。 |
| 27 | TIMER_INT[1] | Timer[1]的全局中断。 |
| 28 | TIMER_INT[2] | Timer[2]的全局中断。 |
| 29 | RTC_IRQ | RTC的全局中断。 |
| 30 | Reserved | 保留。 |
| 31 | I2C0_INT | I2C0中断。 |
| 32 | I2C1_INT | I2C1中断。 |
| 33 | GPIO_INT[0] | GPIO[7:0]上报的组合中断。 |
| 34 | GPIO_INT[1] | GPIO[15:8]上报的组合中断。 |
| 35 | GPIO_INT[2] | GPIO[23:16]上报的组合中断。 |
| 36 | SOFT_INT[0] | CPU软中断0。 |
| 37 | SOFT_INT[1] | CPU软中断1。 |
| 38 | SOFT_INT[2] | CPU软中断2。 |
| 39 | SOFT_INT[3] | CPU软中断3。 |
| 40 | COEX_WL_INT | 共存WLAN软中断。 |
| 41 | COEX_BT_INT | 共存BT软中断。 |
| 42 | COEX_WIFI_RESUME_INT | 共存WLAN恢复射频中断。 |
| 43 | SPI_INT | SPI中断。 |
| 44 | WLPHY_INT | WLAN PHY中断。 |
| 45 | WLMAC_INT | WLAN MAC中断。 |
| 46 | BLE_INT | BLE中断。 |
| 47 | SLE_INT | SLE中断。 |
| 48 | TSENSOR_INT | TSENSOR中断。 |
| 49 | PMU_CMU_ERR_INT | PMU/CMU异常中断。 |
| 50 | DIAG_INT | 维测中断。 |
| 51 | I2S_INT | I2S中断。 |
| 52 | QSPI_INT | QSPI中断。 |
| 53 | UART0_INT | UART0中断。 |
| 54 | UART1_INT | UART1中断。 |
| 55 | UART2_INT | UART2中断。 |
| 56 | PWM_ABNOR_INT | PWM_ABNOR中断。 |
| 57 | PWM_CFG_INT | PWM_CFG中断。 |
| 58 | SFC_INT | SFC中断。 |
| 59 | DMA_INT | DMA中断。 |
| 60 | TIMER_ABNOR_INT | TIMER[2:0]异常中断。 |
| 61 | I2S_TX_INT | I2S_TX中断。 |
| 62 | I2S_RX_INT | I2S_RX中断。 |
| 63 | PKE_REE_INT | PKE REE中断。 |
| 64 | SPACC_REE_INT | SPACC REE中断。 |
| 65 | RKP_REE_INT | RKP REE中断。 |
| 66 | KLAD_REE_INT | KLAD REE中断。 |
| 67 | GLP UART RX WAKE INT | GLP UART RX唤醒中断。 |
| 68 | TIMING_GEN_INT | TIMING_GEN中断。 |
| 69 | MAC_MONITOR_INT | MAC维测中断。 |
| 70 | MEM_MONITOR_INT | MEM维测中断。 |
| 71 | TCM_MONITOR_INT | TCM维测中断。 |
| 72 | LSADC_INTR | LSADC中断。 |
| 89~73 | Reserved | 保留。 |

<a id="2.6.2"></a>
### 中断结构

芯片使用CPU内部集成的中断控制器，所有的外设或寄存器触发的非标准中断均直接连至CPU内部进行处理。

本节主要介绍非IP中断，各个IP的中断说明请参见各IP对应章节。

NMI中断

须知

TEE_NMI_INT[tee_nmi_int]不会自动清零，在触发中断后需要将该值写回0。

NMI（Non-Maskable Interrupt）中断用于控制软件，由WDOG中断和寄存器触发，通过使能WDOG或写TEE_NMI_INT[tee_nmi_int]为1触发NMI中断。

软中断

软中断为通过配置寄存器触发中断的中断触发方式，包含4个可以通过写寄存器触发的中断（软中断0～软中断3）。

软中断0～软中断3的触发方式相同，以使用软中断0为例，配置步骤如下：

步骤 1 写SOFT_INT_EN[soft_int0_en]为1，打开软中断0的使能。

步骤 2 写SOFT_INT_SET[soft_int0_set]为1，将软中断0置位。

此寄存器为自清零寄存器，写1后自动回零，写0无效。

步骤 3 软件进入软中断0处理程序，此时可读取SOFT_INT_STS[soft_int0_sts]查询中断状态：

0：无中断。
1：有中断。

步骤 4 写SOFT_INT_CLR[soft_int0_clr]为1，清除软中断0。

此寄存器为自清零寄存器，写1后自动回零，写0无效。

\----结束

<a id="2.6.3"></a>
### 寄存器概览

中断系统寄存器概览如表2-6所示。

表2-6 中断系统寄存器概览（基址是0x44000000）

| Offset Address | Register | Description |
| --- | --- | --- |
| 0x0040 | NMI_INT | WDT中断查询&NMI中断配置寄存器。 |
| 0x0150 | SOFT_INT_EN | CPU软中断使能寄存器。 |
| 0x0154 | SOFT_INT_SET | CPU软中断置位寄存器。 |
| 0x0158 | SOFT_INT_CLR | CPU软中断清除寄存器。 |
| 0x015C | SOFT_INT_STS | CPU软中断查询寄存器。 |

<a id="2.6.4"></a>
### 寄存器描述

NMI_INT

NMI_INT为WDT中断查询&NMI中断配置寄存器。  
Offset Address：0x0040 Total Reset Value：0x00000000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| 31:2 | RO | reserved | 保留。 | 0x00000000 |
| 1 | RO | wdt_int | 看门狗中断查询寄存器。0：WDT中断无效；1：WDT中断有效。 | 0x0 |
| 0 | RW | tee_nmi_int | 软控nmi中断配置寄存器。0：拉低CPU NMI软中断；1：拉高CPU NMI软中断。 | 0x0 |

SOFT_INT_EN

SOFT_INT_EN为CPU软中断使能寄存器。

Offset Address：0x0150 Total Reset Value：0x00000000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| 31:4 | RO | reserved | 保留。 | 0x0000000 |
| 3 | RW | soft_int3_en | CPU软中断3使能开关寄存器。0：使能无效；1：使能有效。 | 0x0 |
| 2 | RW | soft_int2_en | CPU软中断2使能开关寄存器。0：使能无效；1：使能有效。 | 0x0 |
| 1 | RW | soft_int1_en | CPU软中断1使能开关寄存器。0：使能无效；1：使能有效。 | 0x0 |
| 0 | RW | soft_int0_en | CPU软中断0使能开关寄存器。0：使能无效；1：使能有效。 | 0x0 |

SOFT_INT_SET

SOFT_INT_SETCPU软中断置位寄存器。

Offset Address：0x0154 Total Reset Value：0x00000000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| 31:4 | RO | reserved | 保留。 | 0x0000000 |
| 3 | W1_PULSE | soft_int3_set | CPU软中断3置位配置寄存器。0：置位无效；1：置位有效。 | 0x0 |
| 2 | W1_PULSE | soft_int2_set | CPU软中断2置位配置寄存器。0：置位无效；1：置位有效。 | 0x0 |
| 1 | W1_PULSE | soft_int1_set | CPU软中断1置位配置寄存器。0：置位无效；1：置位有效。 | 0x0 |
| 0 | W1_PULSE | soft_int0_set | CPU软中断0置位配置寄存器。0：置位无效；1：置位有效。 | 0x0 |

SOFT_INT_CLR

SOFT_INT_CLR为CPU软中断清除寄存器。

Offset Address：0x0158 Total Reset Value：0x00000000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| 31:4 | RO | reserved | 保留。 | 0x0000000 |
| 3 | W1_PULSE | soft_int3_clr | CPU软中断3清除配置寄存器。0：清除无效；1：清除有效。 | 0x0 |
| 2 | W1_PULSE | soft_int2_clr | CPU软中断2清除配置寄存器。0：清除无效；1：清除有效。 | 0x0 |
| 1 | W1_PULSE | soft_int1_clr | CPU软中断1清除配置寄存器。0：清除无效；1：清除有效。 | 0x0 |
| 0 | W1_PULSE | soft_int0_clr | CPU软中断0清除配置寄存器。0：清除无效；1：清除有效。 | 0x0 |

SOFT_INT_STS

SOFT_INT_STS为CPU软中断查询寄存器。

Offset Address：0x015C Total Reset Value：0x00000000

| Bits | Access | Name | Description | Reset |
| --- | --- | --- | --- | --- |
| 31:4 | RO | reserved | 保留。 | 0x0000000 |
| 3 | RO | soft_int3_sts | CPU软中断3状态查询寄存器。0：中断无效；1：中断有效。 | 0x0 |
| 2 | RO | soft_int2_sts | CPU软中断2状态查询寄存器。0：中断无效；1：中断有效。 | 0x0 |
| 1 | RO | soft_int1_sts | CPU软中断1状态查询寄存器。0：中断无效；1：中断有效。 | 0x0 |
| 0 | RO | soft_int0_sts | CPU软中断0状态查询寄存器。0：中断无效；1：中断有效。 | 0x0 |

<a id="2.7"></a>
## RTC

<a id="2.7.1"></a>
### 概述

RTC的功能主要是实现定时、计数功能，可以供操作系统用作系统时钟，也可以供应用程序用作定时和计数。

<a id="2.7.2"></a>
### 功能描述

RTC具有以下功能特点：

- 48bit位宽的free running递加计数器。
- 计数器上电解复位后即开始计数，无需任何使能配置。
- 计数时钟为32kHz时钟。
- 支持配置中断产生寄存器阈值，计数器递加到阈值时产生中断。
- 支持计数器值实时读取。

<a id="2.8"></a>
## Timer

<a id="2.8.1"></a>
### 概述

芯片中有3个相同且可独立配置的定时器（Timer），主要实现定时、计数功能的IP，可供程序用作定时和计数。

<a id="2.8.2"></a>
### 功能描述

Timer具有以下功能特点：

- 3个32bit的可独立配置的Timer单元。
- 每个Timer单元支持三种工作模式：
one-shot模式：加载配置值到计数器，进行递减计数，递减到0后停止计数。
periodic模式：Timer持续计数，加载配置值到计数器，进行递减计数，递减到0后再次载入配置值并继续递减计数。
free running模式：Timer持续计数，计数器起始值为0xFFFF_FFFF，进行递减计数，递减到0后再次载入0xFFFF_FFFF并继续递减计数。
- 每个Timer单元支持独立使能。

<a id="2.9"></a>
## 看门狗

<a id="2.9.1"></a>
### 概述

WatchDog用于系统异常恢复，如果未得到更新则隔一定时间（可编程）产生一个系统复位信号，当WatchDog在此之前关闭工作时钟或更新计数器，复位信号不会产生。

<a id="2.9.2"></a>
### 功能描述

WatchDog内置1个可编程32bit计数器，具有以下功能特点：

- 内置计数器进行递减计数，当由预设值递减到0时产生超时。
- WatchDog具有2种工作方式。
- 方式一：如果超时，则只产生系统复位。
- 方式二：第一次超时，WatchDog产生中断；第二次超时，如果中断未被清除，WatchDog产生系统复位。
- 可配置超时间隔。

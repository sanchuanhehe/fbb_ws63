# 封装与管脚

## 封装与管脚分布

### 封装

WS63V100 系列芯片采用 QFN40 封装，封装尺寸为 5mm × 5mm ，管脚间距为 0.4mm，详细封装请参见图 1-1、图 1-2 和图 1-3。

图1-1 芯片封装顶视图  
![](images/272ae59efe0a59109818f6093d95351d5e197338a98049eee620f1c23749e291.jpg)

图1-2 芯片封装底视图  
![](images/1ac1158db8d9b96d1e1dabdd58ae97dd93354a4482a0be86254bb06c37a7ba26.jpg)

图1-3 芯片侧面放大图  
![](images/3def550cdd74d14f1f54d52de9cb04396670090462e2268a3a461b6aab52c5c8.jpg)  
芯片封装尺寸参数如表 1-1 所示。

表1-1 芯片封装参数说明表

| 参数 | 最小值(mm) | 典型值(mm) | 最大值(mm) | 最小值(inch) | 典型值(inch) | 最大值(inch) |
| ---- | ---------- | ---------- | ---------- | ------------ | ------------ | ------------ |
| A    | 0.85       | 0.90       | 0.95       | 0.033        | 0.035        | 0.037        |
| A1   | 0.00       | 0.02       | 0.05       | 0.000        | 0.001        | 0.002        |
| A3   | 0.20 REF   |            |            | 0.008 REF    |              |              |
| b    | 0.15       | 0.20       | 0.25       | 0.006        | 0.008        | 0.010        |
| D    | 4.90       | 5.00       | 5.10       | 0.193        | 0.197        | 0.201        |
| E    | 4.90       | 5.00       | 5.10       | 0.193        | 0.197        | 0.201        |
| D2   | 3.50       | 3.60       | 3.70       | 0.138        | 0.142        | 0.146        |
| E2   | 3.50       | 3.60       | 3.70       | 0.138        | 0.142        | 0.146        |
| e    | 0.40 BSC   |            |            | 0.016 BSC    |              |              |
| L    | 0.30       | 0.40       | 0.50       | 0.012        | 0.016        | 0.020        |
| L1   | 0.30       | 0.40       | 0.50       | 0.012        | 0.016        | 0.020        |
| K    | 0.20       |            |            | 0.008        |              |              |
| R    | 0.08       | -          | 0.13       | 0.003        | -            | 0.005        |
| aaa  | 0.10       |            |            | 0.004        |              |              |
| bbb  | 0.07       |            |            | 0.003        |              |              |
| ccc  | 0.10       |            |            | 0.004        |              |              |
| ddd  | 0.05       |            |            | 0.002        |              |              |
| eee  | 0.08       |            |            | 0.003        |              |              |
| fff  | 0.10       |            |            | 0.004        |              |              |

### 管脚分布

WS63V100 系列芯片管脚分布如图 1-4 和图 1-5 所示。  
图1-4 WS63V100 TOP View 管脚分布
![](images/fig_p12_0.png)

图1-5 WS63EV100 TOP View 管脚分布
![](images/fig_p13_0.png)

!!! note "说明"

    WS63V100 常规版本芯片 PIN40 管脚为 GND，WS63EV100 雷达版本芯片 PIN40 管脚为 RFI。

## 管脚描述

### 管脚类型说明

管脚 I/O 类型说明如表 1-2 所示。

表1-2 管脚 I/O 类型说明

| I/O             | 说明                                   |
| --------------- | -------------------------------------- |
| I               | 输入信号。                             |
| I<sub>PD</sub>        | 输入信号,内部下拉。                    |
| I<sub>PU</sub>        | 输入信号,内部上拉。                    |
| I<sub>S</sub>         | 输入信号,带施密特触发器。              |
| I<sub>SPD</sub>       | 输入信号,带施密特触发器,内部下拉。     |
| I<sub>SPU</sub>       | 输入信号,带施密特触发器,内部上拉。     |
| O               | 输出信号。                             |
| O<sub>OD</sub>        | 输出,漏极开路。                        |
| I/O             | 双向输入/输出信号。                    |
| I<sub>PD</sub>/O      | 双向,输入下拉。                        |
| I<sub>PU</sub>/O      | 双向,输入上拉。                        |
| I<sub>SPD</sub>/O     | 双向,输入下拉,带施密特触发器。         |
| I<sub>SPU</sub>/O     | 双向,输入上拉,带施密特触发器。         |
| I<sub>PD</sub>/O<sub>OD</sub> | 双向,输入下拉,输出漏极开路。           |
| I<sub>PU</sub>/O<sub>OD</sub> | 双向,输入上拉,输出漏极开路。           |
| I<sub>S</sub>/O       | 双向,输入带施密特触发器。              |
| I<sub>S</sub>/O<sub>OD</sub>  | 双向,输入带施密特触发器,输出漏极开路。 |
| CIN             | Crystal Oscillator:晶振输入。          |
| COUT            | Crystal Oscillator:晶振输出。          |
| P               | 电源。                                 |
| PI              | 电源输入。                             |
| PO              | 电源输出。                             |
| GND             | 地。                                   |

### 管脚排列表

WS63V100 系列芯片采用的封装形式为 QFN 40PIN，管脚按位置排列如表 1-3 所示。

表1-3 WS63EV100 芯片管脚排列

| 位置 | 管脚名称   | 位置 | 管脚名称     |
| ---- | ---------- | ---- | ------------ |
| 1    | RFIO       | 22   | BUCK_OUT     |
| 2    | AVDD33_RF1 | 23   | PWR_SEL      |
| 3    | AVDD33_RF0 | 24   | GPIO_13      |
| 4    | GPIO_00    | 25   | GPIO_14      |
| 5    | GPIO_01    | 26   | UART1_TX     |
| 6    | GPIO_02    | 27   | UART1_RX     |
| 7    | GPIO_03    | 28   | UART0_TX     |
| 8    | GPIO_04    | 29   | UART0_RX     |
| 9    | GPIO_05    | 30   | NC(保持悬空) |
| 10   | GPIO_06    | 31   | XIN          |
| 11   | GPIO_07    | 32   | XOUT         |
| 12   | GPIO_08    | 33   | VDD_DIG      |
| 13   | GPIO_09    | 34   | DVDD3318     |
| 14   | GPIO_10    | 35   | IOLDO18      |
| 15   | GPIO_11    | 36   | AVDD33_1     |
| 16   | GPIO_12    | 37   | AVDD33_0     |
| 17   | VDD33_OUT  | 38   | PWR_ON       |
| 18   | VBAT_IN    | 39   | AVDD33_RF2   |
| 19   | BUCK_IN    | 40   | RFI          |
| 20   | BUCK_LX    | EPAD | GND          |
| 21   | AVSS_PGND  | -    | -            |

!!! note "说明"

    - WS63V100 常规版本芯片 PIN40 管脚为 GND，板级可选择接地或 NC 悬空。

### PMU 控制信号

全局控制信号如表 1-4 所示。

表1-4 全局控制信号管脚列表

<table><tr><th>PIN</th><th>名称</th><th>类型</th><th>电平(V)</th><th>说明</th></tr><tr><td>38</td><td>PWR_ON</td><td>I</td><td>3.3/1.8</td><td>PMU 上电使能管脚(跟随DVDD3318 电平)。<br>0:下电;<br>1:上电。</td></tr><tr><td>23</td><td>PWR_SEL</td><td>I</td><td>3.3</td><td>VBAT_IN 电源方案选择管脚。<br>0:VBAT_IN 采用 5V 供电;<br>1:VBAT_IN 采用 3.3V 供电。</td></tr></table>

### GPIO 管脚

GPIO 接口如表 1-5 所示。

表1-5 GPIO 管脚列表

| PIN | 名称    | 类型 | 电平(V) | 说明                           |
| --- | ------- | ---- | ------- | ------------------------------ |
| 4   | GPIO_00 | I/O  | 3.3/1.8 | 通用 GPIO。                    |
| 5   | GPIO_01 | I/O  | 3.3/1.8 | 通用 GPIO,管脚禁止加上拉电阻。 |
| 6   | GPIO_02 | I/O  | 3.3/1.8 | 通用 GPIO。                    |
| 7   | GPIO_03 | I/O  | 3.3/1.8 | 通用 GPIO。                    |
| 8   | GPIO_04 | I/O  | 3.3/1.8 | 通用 GPIO。                    |
| 9   | GPIO_05 | I/O  | 3.3/1.8 | 通用 GPIO。                    |
| 10  | GPIO_06 | I/O  | 3.3/1.8 | 通用 GPIO。                    |
| 11  | GPIO_07 | I/O  | 3.3/1.8 | 通用 GPIO。                    |
| 12  | GPIO_08 | I/O  | 3.3/1.8 | 通用 GPIO。                    |
| 13  | GPIO_09 | I/O  | 3.3/1.8 | 通用 GPIO,管脚禁止加上拉电阻。 |
| 14  | GPIO_10 | I/O  | 3.3/1.8 | 通用 GPIO。                    |
| 15  | GPIO_11 | I/O  | 3.3/1.8 | 通用 GPIO,管脚禁止加上拉电阻。 |
| 16  | GPIO_12 | I/O  | 3.3/1.8 | 通用 GPIO。                    |
| 24  | GPIO_13 | I/O  | 3.3/1.8 | 通用 GPIO。                    |
| 25  | GPIO_14 | I/O  | 3.3/1.8 | 通用 GPIO。                    |

### 电源管脚

电源管脚如表 1-6 所示。

表1-6 电源管脚列表

| PIN | 名称       | 类型  | 电压(V) | 说明                                                                                                                                |
| --- | ---------- | ----- | ------- | ----------------------------------------------------------------------------------------------------------------------------------- |
| 2   | AVDD33_RF1 | PI    | 3.0~3.6 | RF 电源输入,外接滤波电容 1μF。                                                                                                      |
| 3   | AVDD33_RF0 | PI    | 3.0~3.6 | RF 电源输入,外接滤波电容 1μF。                                                                                                      |
| 17  | VDD33_OUT  | PI/PO | 3.0~3.6 | 芯片采用 VBAT_IN 5V 供电时,该管脚作为 3.3V 电源输出;芯片采用 VBAT_IN 3.3V 供电时,该管脚作为 3.3V 电源输入。管脚外接滤波电容 4.7μF。 |
| 18  | VBAT_IN    | PI    | 3.3/5   | 电源输入,可选择 5V 或 3.3V 供电,外接滤波电容 10μF。                                                                                 |
| 19  | BUCK_IN    | PI    | 3.0~3.6 | 电源输入,外接滤波电容 4.7μF。                                                                                                       |
| 20  | BUCK_LX    | -     | -       | LDO 方案下接电源(3.3V),BUCK 方案下接电感,外接 2.2uH 电感。                                                                          |
| 22  | BUCK_OUT   | PO    | 0.9     | 芯片内部 BUCK 输出,BUCK 电源方案下外接滤波电容 10μF,LDO电源方案下悬空。                                                             |
| 33  | VDD_DIG    | PO    | 0.9     | 芯片内部电源输出,BUCK 电源方案下悬空,LDO 电源方案下外接滤波电容 1μF。                                                               |
| 34  | DVDD3318   | PI    | 3.3/1.8 | IO 电源输入,外接滤波电容 4.7μF。                                                                                                    |
| 35  | IOLDO18    | PI/PO | 1.8     | IO 电源输入为 3.3V 时,IOLDO 输出 1.8V;IO 电源输入为 1.8V 时,IOLDO 需输入 1.8V 电源。引脚外接滤波电容 1μF。                          |
| 36  | AVDD33_1   | PI    | 3.0~3.6 | 电源输入,外接滤波电容 2.2μF。                                                                                                       |
| 37  | AVDD33_0   | PI    | 3.0~3.6 | 电源输入,外接滤波电容 1μF。                                                                                                         |
| 39  | AVDD33_RF2 | PI    | 3.0~3.6 | RF 电源输入,外接滤波电容 1μF。                                                                                                      |

### RF 接口

RF 接口如表 1-7 所示。

表1-7 RF 接口管脚列表

| PIN | 名称 | 类型 | 电平(V) | 说明                     |
| --- | ---- | ---- | ------- | ------------------------ |
| 1   | RFIO | ANA  | -       | WLAN 2.4G RF 输入/输出。 |
| 40  | RFI  | ANA  | -       | WS63E 芯片雷达天线输入。 |

### GND 管脚

GND 管脚如表 1-8 所示。

表1-8 GND 管脚列表

| PIN  | 名称      | 电压(V) | 说明       |
| ---- | --------- | ------- | ---------- |
| 21   | AVSS_PGND | -       | GND 管脚。 |
| EPAD | GND       | -       | GND 管脚。 |

### UART 管脚

芯片独立的 UART0/1 管脚为耐 5V 管脚，输出管脚类型为 OD 开漏输出，使用时建议加 2.2KΩ 上拉电阻。可复用为 I2C 接口。

表1-9 GPIO 复用管脚

<table><tr><th>PIN</th><th>管脚名称</th><th>类型</th><th>电压(V)</th><th>说明</th></tr><tr><td>26</td><td>UART1_TX</td><td>OD</td><td>5/3.3/1.8</td><td>复用信号0: GPIO_15<br>复用信号1: UART1_TX<br>复用信号2: I2C1_SDA</td></tr><tr><td>27</td><td>UART1_RX</td><td>I</td><td>5/3.3/1.8</td><td>复用信号0: GPIO_16<br>复用信号1: UART1_RX<br>复用信号2: I2C1_SCL</td></tr><tr><td>28</td><td>UART0_TX</td><td>OD</td><td>5/3.3/1.8</td><td>复用信号1: UART0_TX<br>复用信号2: I2C0_SDA</td></tr><tr><td>29</td><td>UART0_RX</td><td>I</td><td>5/3.3/1.8</td><td>复用信号1: UART0_RX<br>复用信号2: I2C0_SCL</td></tr></table>

### ADC 通道

!!! warning "须知"

    ADC 管脚：LSADC 通道与 GPIO 功能只支持其中 1 种功能，ADC 通道管脚与 GPIO 管脚的对应关系如表 1-10 所示。

表1-10 ADC 通道管脚与复用管脚对应关系

| 复用管脚名称 | ADC 管脚 |
| ------------ | -------- |
| GPIO_07      | ADC0     |
| GPIO_08      | ADC1     |
| GPIO_09      | ADC2     |
| GPIO_10      | ADC3     |
| GPIO_11      | ADC4     |
| GPIO_12      | ADC5     |

### GPIO 复用管脚

GPIO（General Purpose Input/Output）管脚如表 1-11 所示。其中 PIN5、7、8、10 为硬件配置字，使用时必需注意上电前的默认电平，详细设计请参见“3.1.3 硬件初始化系统配置电路”。

表1-11 GPIO 复用管脚

<table><tr><th>PIN</th><th>管脚名称</th><th>类型</th><th>电压(V)</th><th>说明</th></tr><tr><td>4</td><td>GPIO_00</td><td>I/O</td><td>3.3/1.8</td><td>复用信号 0: GPIO_0 (Default)<br>复用信号 1: PWM0<br>复用信号 2: 保留<br>复用信号 3: SPI1_CSN<br>复用信号 4: JTAG_TDI</td></tr><tr><td>5</td><td>GPIO_01</td><td>I/O</td><td>3.3/1.8</td><td>复用信号 0: GPIO_1 (Default)<br>复用信号 1: PWM1<br>复用信号 2: 保留<br>复用信号 3: SPI1_IO0/SPI1_OUT</td></tr><tr><td>6</td><td>GPIO_02</td><td>I/O</td><td>3.3/1.8</td><td>复用信号 0: GPIO_2 (Default)<br>复用信号 1: PWM2<br>复用信号 2: 保留<br>复用信号3:SPI1_IO3</td></tr><tr><td>7</td><td>GPIO_03</td><td>I/O</td><td>3.3/1.8</td><td>复用信号0:GPIO_3(Default)<br>复用信号1:PWM3<br>复用信号2:保留<br>复用信号3:SPI1_IO1/SPI1_IN</td></tr><tr><td>8</td><td>GPIO_04</td><td>I/O</td><td>3.3/1.8</td><td>复用信号0:保留(Default)<br>复用信号1:PWM4<br>复用信号2:GPIO_4<br>复用信号3:SPI1_IO1/SPI1_IN(优先使用pin8)<br>复用信号4:JTAG_ENABLE,硬件配置字</td></tr><tr><td>9</td><td>GPIO_05</td><td>I/O</td><td>3.3/1.8</td><td>复用信号0:保留(Default)<br>复用信号1:PWM5<br>复用信号2:UART2_CTS<br>复用信号3:SPI1_IO2<br>复用信号4:GPIO_5<br>复用信号5:SPI0_IN</td></tr><tr><td>10</td><td>GPIO_06</td><td>I/O</td><td>3.3/1.8</td><td>复用信号0:GPIO_6(Default)<br>复用信号1:PWM6<br>复用信号2:UART2_RTS<br>复用信号3:SPI1_SCK<br>复用信号4:REFCLK_FREQ_STATUS,硬件配置字<br>复用信号5:保留<br>复用信号6:SPI0_OUT</td></tr><tr><td>11</td><td>GPIO_07</td><td>I/O</td><td>3.3/1.8</td><td>复用信号0:GPIO_7(Default)<br>复用信号1:PWM7<br>复用信号2: UART2_RXD<br>复用信号3: SPI0_SCK<br>复用信号4: I2S_MCLK</td></tr><tr><td>12</td><td>GPIO_08</td><td>I/O</td><td>3.3/1.8</td><td>复用信号0: GPIO_8 (Default)<br>复用信号1: PWM0<br>复用信号2: UART2_TXD<br>复用信号3: SPI0_CS1_N<br>复用信号4: 保留</td></tr><tr><td>13</td><td>GPIO_09</td><td>I/O</td><td>3.3/1.8</td><td>复用信号0: GPIO_9 (Default)<br>复用信号1: PWM1<br>复用信号2: RADAR_ANT0_SW<br>复用信号3: SPI0_OUT<br>复用信号4: I2S_DO<br>复用信号5: 保留<br>复用信号6: 保留<br>复用信号7: JTAG_TDO</td></tr><tr><td>14</td><td>GPIO_10</td><td>I/O</td><td>3.3/1.8</td><td>复用信号0: GPIO_10 (Default)<br>复用信号1: PWM2<br>复用信号2: ANT0_SW<br>复用信号3: SPI0_CS0_N<br>复用信号4: I2S_SCLK</td></tr><tr><td>15</td><td>GPIO_11</td><td>I/O</td><td>3.3/1.8</td><td>复用信号0: GPIO_11 (Default)<br>复用信号1: PWM3<br>复用信号2: RADAR_ANTI_SW<br>复用信号3: SPI0_IN<br>复用信号4: I2S_LRCLK</td></tr><tr><td>16</td><td>GPIO_12</td><td>I/O</td><td>3.3/1.8</td><td>复用信号0: GPIO_12 (Default)<br>复用信号1: PWM4<br>复用信号2: ANT1_SW<br>复用信号4: I2S_DI</td></tr><tr><td>24</td><td>GPIO_13</td><td>I/O</td><td>3.3/1.8</td><td>复用信号0: GPIO_13 (Default)<br>复用信号1: UART1_CTS<br>复用信号2: RADAR_ANT0_SW<br>复用信号3: 保留<br>复用信号4: JTAG_TMS/SWD</td></tr><tr><td>25</td><td>GPIO_14</td><td>I/O</td><td>3.3/1.8</td><td>复用信号0: GPIO_14 (Default)<br>复用信号1: UART1_RTS<br>复用信号2: RADAR_ANTI_SW<br>复用信号3: 保留<br>复用信号4: JTAG_TCK/SWC</td></tr><tr><td>26</td><td>UART1_TX</td><td>OD</td><td>5/3.3/1.8</td><td>复用信号0: GPIO_15<br>复用信号1: UART1_TX<br>复用信号2: I2C1_SDA</td></tr><tr><td>27</td><td>UART1_RX</td><td>I</td><td>5/3.3/1.8</td><td>复用信号0: GPIO_16<br>复用信号1: UART1_RX<br>复用信号2: I2C1_SCL</td></tr></table>

### CLK 管脚

CLK 管脚如表 1-12 所示。

表1-12 CLK 管脚

| PIN | 管脚名称 | 电压(V) | 说明                                 |
| --- | -------- | ------- | ------------------------------------ |
| 31  | XIN      | 1.6     | 晶体 XIN,支持 24MHz 或 40MHz 晶体。  |
| 32  | XOUT     | 1.6     | 晶体 XOUT,支持 24MHz 或 40MHz 晶体。 |

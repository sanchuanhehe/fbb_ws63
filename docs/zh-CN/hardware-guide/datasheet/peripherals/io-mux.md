# IO MUX

<a id="6.1.1"></a>
## 概述

芯片数字管脚数量有限，通过 IO 复用的方式丰富管脚功能。

<a id="6.1.2"></a>
## 软用管脚描述

<hr style="border: none; border-top: 3px solid #555; margin: 16px 0;">

ADC 管脚：LSADC 通道与 GPIO 功能只支持其中 1 种功能，ADC 通道管脚与 GPIO 管脚的对应关系如表 6-2 所示。

<hr style="border: none; border-top: 3px solid #555; margin: 16px 0;">

表6-1 ADC 通道管脚与复用管脚对应关系

<table>
<thead><tr><th>复用管脚名称</th><th>ADC 管脚</th></tr></thead>
<tbody>
<tr><td>GPIO_07</td><td>ADC0</td></tr>
<tr><td>GPIO_08</td><td>ADC1</td></tr>
<tr><td>GPIO_09</td><td>ADC2</td></tr>
<tr><td>GPIO_10</td><td>ADC3</td></tr>
<tr><td>GPIO_11</td><td>ADC4</td></tr>
<tr><td>GPIO_12</td><td>ADC5</td></tr>
</tbody>
</table>

软件复用管脚如表 6-2 所示。

表6-2 软件复用管脚

<table>
<tr><th>Pin</th><th>Pad信号</th><th>复用控制寄存器</th><th>复用信号0</th><th>复用信号1</th><th>复用信号2</th><th>复用信号3</th><th>复用信号4</th><th>复用信号5</th><th>复用信号6</th><th>复用信号7</th></tr>
<tr><td>5</td><td>GPIO_01</td><td><a href="#reg-6.1-gpio_01_sel">GPIO_01_SEL</a></td><td>GPIO_01</td><td>PWM1</td><td>DIAG[1]</td><td>SPI1_IO0</td><td>JTAG_MODE</td><td>BT_SAMPLE</td><td>-</td><td>-</td></tr>
<tr><td>6</td><td>GPIO_02</td><td><a href="#reg-6.1-gpio_02_sel">GPIO_02_SEL</a></td><td>GPIO_02</td><td>PWM2</td><td>DIAG[2]</td><td>SPI1_IO3</td><td>WIFI_TSF_SYNC</td><td>WL_GLP_SYNC_PULSE</td><td>BLE&SLE_GLP_SYNC_PULSE</td><td>-</td></tr>
<tr><td>7</td><td>GPIO_03</td><td><a href="#reg-6.1-gpio_03_sel">GPIO_03_SEL</a></td><td>GPIO_03</td><td>PWM3</td><td>PMU_32_K_TEST</td><td>SPI1_IO1</td><td>HW_ID[0]</td><td>DIAG[3]</td><td>-</td><td>-</td></tr>
<tr><td>8</td><td>GPIO_04</td><td><a href="#reg-6.1-gpio_04_sel">GPIO_04_SEL</a></td><td>SSI_CLK</td><td>PWM4</td><td>GPIO_04</td><td>SPI1_IO1</td><td>JTAG_ENABLE</td><td>DFT_JTAG_TMS</td><td>-</td><td>-</td></tr>
<tr><td>9</td><td>GPIO_05</td><td><a href="#reg-6.1-gpio_05_sel">GPIO_05_SEL</a></td><td>SSI_DATA</td><td>PWM5</td><td>UART2_CTS</td><td>SPI1_IO2</td><td>GPIO_05</td><td>SPI0_IN</td><td>DFT_JTAG_TCK</td><td>-</td></tr>
<tr><td>10</td><td>GPIO_06</td><td><a href="#reg-6.1-gpio_06_sel">GPIO_06_SEL</a></td><td>GPIO_06</td><td>PWM6</td><td>UART2_RTS</td><td>SPI1_SCK</td><td>REF_CLK_FREQ_STATUS</td><td>DIAG[4]</td><td>SPI0_OUT</td><td>DFT_JTAG_TD1</td></tr>
<tr><td>11</td><td>GPIO_07</td><td><a href="#reg-6.1-gpio_07_sel">GPIO_07_SEL</a></td><td>GPIO_07</td><td>PWM7</td><td>UART2_RXD</td><td>SPI0_SCK</td><td>I2S_MCLK</td><td>DIAG[5]</td><td>-</td><td>-</td></tr>
<tr><td>12</td><td>GPIO_08</td><td><a href="#reg-6.1-gpio_08_sel">GPIO_08_SEL</a></td><td>GPIO_08</td><td>PWM0</td><td>UART2_TXD</td><td>SPI0_CS1_N</td><td>DIAG[6]</td><td>-</td><td>-</td><td>-</td></tr>
<tr><td>13</td><td>GPIO_09</td><td><a href="#reg-6.1-gpio_09_sel">GPIO_09_SEL</a></td><td>GPIO_09</td><td>PWM1</td><td>RADAR_ANT0_SW</td><td>SPI0_OUT</td><td>I2S_DO</td><td>HW_ID[1]</td><td>DIAG[7]</td><td>JTAG_TDO</td></tr>
<tr><td>14</td><td>GPIO_10</td><td><a href="#reg-6.1-gpio_10_sel">GPIO_10_SEL</a></td><td>GPIO_10</td><td>PWM2</td><td>ANT0_SW</td><td>SPI0_CS0_N</td><td>I2S_SCLK</td><td>DIAG[0]</td><td>-</td><td>-</td></tr>
<tr><td>15</td><td>GPIO_11</td><td><a href="#reg-6.1-gpio_11_sel">GPIO_11_SEL</a></td><td>GPIO_11</td><td>PWM3</td><td>RADAR_ANT1_SW</td><td>SPI0_IN</td><td>I2S_LRCLK</td><td>DIAG[1]</td><td>HW_ID[2]</td><td>-</td></tr>
<tr><td>16</td><td>GPIO_12</td><td><a href="#reg-6.1-gpio_12_sel">GPIO_12_SEL</a></td><td>GPIO_12</td><td>PWM4</td><td>ANT1_SW</td><td>-</td><td>I2S_DI</td><td>DIAG[7]</td><td>HW_ID[3]</td><td>-</td></tr>
<tr><td>24</td><td>GPIO_13</td><td><a href="#reg-6.1-gpio_13_sel">GPIO_13_SEL</a></td><td>GPIO_13</td><td>UART1_CTS</td><td>RADAR_ANT0_SW</td><td>DFT_JTAG_TDO</td><td>JTAG_TMS</td><td>-</td><td>-</td><td>-</td></tr>
<tr><td>25</td><td>GPIO_14</td><td><a href="#reg-6.1-gpio_14_sel">GPIO_14_SEL</a></td><td>GPIO_14</td><td>UART1_RTS</td><td>RADAR_ANT1_SW</td><td>DFT_JTAG_TRSTN</td><td>JTAG_TCK</td><td>-</td><td>-</td><td>-</td></tr>
<tr><td>26</td><td>UART1_TXD</td><td><a href="#reg-6.1-uart1_txd_sel">UART1_TXD_SEL</a></td><td>GPIO_15</td><td>UART1_TXD</td><td>I2C1_SDA</td><td>-</td><td>-</td><td>-</td><td>-</td><td>-</td></tr>
<tr><td>27</td><td>UART1_RXD</td><td><a href="#reg-6.1-uart1_rxd_sel">UART1_RXD_SEL</a></td><td>GPIO_16</td><td>UART1_RXD</td><td>I2C1_SCL</td><td>-</td><td>-</td><td>-</td><td>-</td><td>-</td></tr>
<tr><td>28</td><td>UART0_TXD</td><td><a href="#reg-6.1-uart0_txd_sel">UART0_TXD_SEL</a></td><td>GPIO_17</td><td>UART0_TXD</td><td>I2C0_SDA</td><td>-</td><td>-</td><td>-</td><td>-</td><td>-</td></tr>
<tr><td>29</td><td>UART0_RXD</td><td><a href="#reg-6.1-uart0_rxd_sel">UART0_RXD_SEL</a></td><td>GPIO_18</td><td>UART0_RXD</td><td>I2C0_SCL</td><td>-</td><td>-</td><td>-</td><td>-</td><td>-</td></tr>
<tr><td colspan="11">注:Flash为芯片内置,SFC管脚无PIN序号</td></tr>
</table>

GPIO 的软件复用管脚说明如表 6-3 所示。

表6-3 GPIO 的软件复用管脚说明

<table>
<thead><tr><th>信号名</th><th>方向</th><th>说明</th></tr></thead>
<tbody>
<tr><td>REFCLK_FREQ_STATUS</td><td>I</td><td>晶体时钟频率的指示信号：<br>
• 1'b0:40M。<br>
• 1'b1:24M。</td></tr>
<tr><td>JTAG_ENABLE</td><td>I</td><td>jtag 使能：<br>
• 1'b0:普通 IO。<br>
• 1'b1:jtag 使能。</td></tr>
<tr><td>JTAG_MODE</td><td>I</td><td>DFT 使能：<br>
• 1'b0:正常功能模式。<br>
• 1'b1:DFT (Design For Testability) 测试模式。</td></tr>
<tr><td>HW_ID</td><td>I</td><td>HW_ID(上电硬件控制字)</td></tr>
<tr><td>DFT_JTAG_TDI</td><td>I</td><td>DFT_JTAG 数据输入。</td></tr>
<tr><td>DFT_JTAG_TRSTN</td><td>I</td><td>DFT_JTAG 复位输入,低电平有效,默认状态为复位。</td></tr>
<tr><td>DFT_JTAG_TCK</td><td>I</td><td>DFT_JTAG 时钟输入。</td></tr>
<tr><td>DFT_JTAG_TMS</td><td>I</td><td>DFT_JTAG 模式选择输入。</td></tr>
<tr><td>DFT_JTAG_TDO</td><td>B</td><td>DFT_JTAG 数据输出。</td></tr>
<tr><td>JTAG_TDI</td><td>I</td><td>JTAG 数据输入。</td></tr>
<tr><td>JTAG_TCK</td><td>I</td><td>JTAG 时钟输入。</td></tr>
<tr><td>JTAG_TMS</td><td>B</td><td>JTAG 模式选择输入。</td></tr>
<tr><td>JTAG_TDO</td><td>B</td><td>JTAG 数据输出。</td></tr>
<tr><td>UART0_RXD</td><td>I</td><td>UART0 RX。</td></tr>
<tr><td>UART0_TXD</td><td>O</td><td>UART0 TX。</td></tr>
<tr><td>UART1_RXD</td><td>I</td><td>UART1 RX。</td></tr>
<tr><td>UART1_TXD</td><td>O</td><td>UART1 TX。</td></tr>
<tr><td>UART1_RTS</td><td>O</td><td>UART1 流控信号。</td></tr>
<tr><td>UART1_CTS</td><td>I</td><td>UART1 流控信号。</td></tr>
<tr><td>UART2_RXD</td><td>I</td><td>UART2 RX。</td></tr>
<tr><td>UART2_TXD</td><td>O</td><td>UART2 TX。</td></tr>
<tr><td>UART2_RTS</td><td>O</td><td>UART2 流控信号。</td></tr>
<tr><td>UART2_CTS</td><td>I</td><td>UART2 流控信号。</td></tr>
<tr><td>PWM0</td><td>B</td><td>PWM0 输出。</td></tr>
<tr><td>PWM1</td><td>B</td><td>PWM1 输出。</td></tr>
<tr><td>PWM2</td><td>B</td><td>PWM2 输出。</td></tr>
<tr><td>PWM3</td><td>B</td><td>PWM3 输出。</td></tr>
<tr><td>PWM4</td><td>B</td><td>PWM4 输出。</td></tr>
<tr><td>PWM5</td><td>B</td><td>PWM5 输出。</td></tr>
<tr><td>PWM6</td><td>B</td><td>PWM6 输出。</td></tr>
<tr><td>PWM7</td><td>B</td><td>PWM7 输出。</td></tr>
<tr><td>GPIO_00</td><td>B</td><td>GPIO</td></tr>
<tr><td>GPIO_01</td><td>B</td><td>GPIO</td></tr>
<tr><td>GPIO_02</td><td>B</td><td>GPIO</td></tr>
<tr><td>GPIO_03</td><td>B</td><td>GPIO</td></tr>
<tr><td>GPIO_04</td><td>B</td><td>GPIO</td></tr>
<tr><td>GPIO_05</td><td>B</td><td>GPIO</td></tr>
<tr><td>GPIO_06</td><td>B</td><td>GPIO</td></tr>
<tr><td>GPIO_07</td><td>B</td><td>GPIO</td></tr>
<tr><td>GPIO_08</td><td>B</td><td>GPIO</td></tr>
<tr><td>GPIO_09</td><td>B</td><td>GPIO</td></tr>
<tr><td>GPIO_10</td><td>B</td><td>GPIO</td></tr>
<tr><td>GPIO_11</td><td>B</td><td>GPIO</td></tr>
<tr><td>GPIO_12</td><td>B</td><td>GPIO</td></tr>
<tr><td>GPIO_13</td><td>B</td><td>GPIO</td></tr>
<tr><td>GPIO_14</td><td>B</td><td>GPIO</td></tr>
<tr><td>GPIO_15</td><td>B</td><td>GPIO</td></tr>
<tr><td>GPIO_16</td><td>B</td><td>GPIO</td></tr>
<tr><td>GPIO_17</td><td>B</td><td>GPIO</td></tr>
<tr><td>GPIO_18</td><td>B</td><td>GPIO</td></tr>
<tr><td>SSI_CLK</td><td>I</td><td>SSI 时钟</td></tr>
<tr><td>SSI_DATA</td><td>B</td><td>SSI 数据</td></tr>
<tr><td>SPI0_SCK</td><td>B</td><td>SPI0 时钟信号</td></tr>
<tr><td>SPI0_CS0_N</td><td>B</td><td>SPI0 片选信号 0 chip select0, active low</td></tr>
<tr><td>SPI0_CS1_N</td><td>B</td><td>SPI0 片选信号 1 chip select1, active low</td></tr>
<tr><td>SPI0_IN</td><td>I</td><td>SPI0 数据接收信号 data input</td></tr>
<tr><td>SPI0_OUT</td><td>B</td><td>SPI0 数据发送信号 data output, with tri-state output</td></tr>
<tr><td>SPI1_SCK</td><td>O</td><td>QSPI (SPI1 时钟信号)</td></tr>
<tr><td>SPI1_CSN</td><td>O</td><td>QSPI (SPI1 片选信号)</td></tr>
<tr><td>SPI1_IO0</td><td>B</td><td>QSPI (SPI1)数据 0。</td></tr>
<tr><td>SPI1_IO1</td><td>B</td><td>QSPI (SPI1)数据 1。</td></tr>
<tr><td>SPI1_IO2</td><td>B</td><td>QSPI (SPI1)数据 2。</td></tr>
<tr><td>SPI1_IO3</td><td>B</td><td>QSPI (SPI1)数据 3。</td></tr>
<tr><td>SFC_CLK</td><td>O</td><td>Flash 控制信号,不支持双沿。Flash 时钟范围:CMU 中 PLL 源头时钟二分频产生 96M 或 80M 的时钟。上电使用晶体时钟二分频:20M 或 12M。</td></tr>
<tr><td>SFC_CSN</td><td>O</td><td>Flash 控制信号,不支持双沿,默认上拉。</td></tr>
<tr><td>SFC_IO0</td><td>B</td><td>Flash 数据信号,不支持双沿,默认上拉。</td></tr>
<tr><td>SFC_IO1</td><td>B</td><td>Flash 数据信号,不支持双沿,默认上拉。</td></tr>
<tr><td>SFC_IO2</td><td>B</td><td>Flash 数据信号,不支持双沿,默认上拉。</td></tr>
<tr><td>SFC_IO3</td><td>B</td><td>Flash 数据信号,不支持双沿,默认上拉。</td></tr>
<tr><td>I2C0_SCL</td><td>B</td><td>I2C 时钟</td></tr>
<tr><td>I2C0_SDA</td><td>B</td><td>I2C 数据</td></tr>
<tr><td>I2C1_SCL</td><td>B</td><td>I2C 时钟</td></tr>
<tr><td>I2C1_SDA</td><td>B</td><td>I2C 数据</td></tr>
<tr><td>I2S_MCLK</td><td>O</td><td>I2S MCLK</td></tr>
<tr><td>I2S_SCLK</td><td>B</td><td>I2S CLK</td></tr>
<tr><td>I2S_LRCLK</td><td>B</td><td>I2S WS</td></tr>
<tr><td>I2S_DI</td><td>I</td><td>I2S RX</td></tr>
<tr><td>I2S_DO</td><td>O</td><td>I2S TX</td></tr>
<tr><td>WIFI_TSF_SYNC</td><td>O</td><td>WiFi 输出的音频同步信号。</td></tr>
<tr><td>WL_GLP_SYNC_PULSE</td><td>O</td><td>WiFi 输出的 GLP 同步信号。</td></tr>
<tr><td>BLE&amp;SLE_GLP_SYNC_PULSE</td><td>O</td><td>BLE&amp;SLE 输出的 GLP 同步信号。</td></tr>
<tr><td>BT_SAMPLE</td><td>I</td><td>BT 维测信号。</td></tr>
<tr><td>DIAG</td><td>O</td><td>内部时钟、信号的维测,端口观测。</td></tr>
<tr><td>PMU_32K_TEST</td><td>O</td><td>PMU 32k 时钟观测。</td></tr>
<tr><td>ANT0_SW</td><td>O</td><td>WiFi 输出的物理天线选择信号 0。</td></tr>
<tr><td>ANT1_SW</td><td>O</td><td>WiFi 输出的物理天线选择信号 1。</td></tr>
<tr><td>RADAR_ANT0_SW</td><td>O</td><td>雷达感知输出的物理天线选择信号 0。</td></tr>
<tr><td>RADAR_ANT1_SW</td><td>O</td><td>雷达感知输出的物理天线选择信号 1。</td></tr>
</tbody>
</table>

<a id="6.1.3"></a>
## 寄存器概览

IO_CONFIG 寄存器概览如表 6-4 所示。

表6-4 IO_CONFIG 寄存器概览（基址是 0x4400_d000）

<table>
<thead><tr><th>偏移地址</th><th>名称</th><th>描述</th></tr></thead>
<tbody>
<tr><td>0x0</td><td><a href="#reg-6.1-gpio_00_sel">GPIO_00_SEL</a></td><td>GPIO_00 管脚复用控制寄存器。</td></tr>
<tr><td>0x4</td><td><a href="#reg-6.1-gpio_01_sel">GPIO_01_SEL</a></td><td>GPIO_01 管脚复用控制寄存器。</td></tr>
<tr><td>0x8</td><td><a href="#reg-6.1-gpio_02_sel">GPIO_02_SEL</a></td><td>GPIO_02 管脚复用控制寄存器。</td></tr>
<tr><td>0xc</td><td><a href="#reg-6.1-gpio_03_sel">GPIO_03_SEL</a></td><td>GPIO_03 管脚复用控制寄存器。</td></tr>
<tr><td>0x10</td><td><a href="#reg-6.1-gpio_04_sel">GPIO_04_SEL</a></td><td>GPIO_04 管脚复用控制寄存器。</td></tr>
<tr><td>0x14</td><td><a href="#reg-6.1-gpio_05_sel">GPIO_05_SEL</a></td><td>GPIO_05 管脚复用控制寄存器。</td></tr>
<tr><td>0x18</td><td><a href="#reg-6.1-gpio_06_sel">GPIO_06_SEL</a></td><td>GPIO_06 管脚复用控制寄存器。</td></tr>
<tr><td>0x1c</td><td><a href="#reg-6.1-gpio_07_sel">GPIO_07_SEL</a></td><td>GPIO_07 管脚复用控制寄存器。</td></tr>
<tr><td>0x20</td><td><a href="#reg-6.1-gpio_08_sel">GPIO_08_SEL</a></td><td>GPIO_08 管脚复用控制寄存器。</td></tr>
<tr><td>0x24</td><td><a href="#reg-6.1-gpio_09_sel">GPIO_09_SEL</a></td><td>GPIO_09 管脚复用控制寄存器。</td></tr>
<tr><td>0x28</td><td><a href="#reg-6.1-gpio_10_sel">GPIO_10_SEL</a></td><td>GPIO_10 管脚复用控制寄存器。</td></tr>
<tr><td>0x2c</td><td><a href="#reg-6.1-gpio_11_sel">GPIO_11_SEL</a></td><td>GPIO_11 管脚复用控制寄存器。</td></tr>
<tr><td>0x30</td><td><a href="#reg-6.1-gpio_12_sel">GPIO_12_SEL</a></td><td>GPIO_12 管脚复用控制寄存器。</td></tr>
<tr><td>0x34</td><td><a href="#reg-6.1-gpio_13_sel">GPIO_13_SEL</a></td><td>GPIO_13 管脚复用控制寄存器。</td></tr>
<tr><td>0x38</td><td><a href="#reg-6.1-gpio_14_sel">GPIO_14_SEL</a></td><td>GPIO_14 管脚复用控制寄存器。</td></tr>
<tr><td>0x3c</td><td><a href="#reg-6.1-uart1_txd_sel">UART1_TXD_SEL</a></td><td>UART1_TXD 管脚复用控制寄存器。</td></tr>
<tr><td>0x40</td><td><a href="#reg-6.1-uart1_rxd_sel">UART1_RXD_SEL</a></td><td>UART1_RXD 管脚复用控制寄存器。</td></tr>
<tr><td>0x44</td><td><a href="#reg-6.1-uart0_txd_sel">UART0_TXD_SEL</a></td><td>UART0_TXD 管脚复用控制寄存器。</td></tr>
<tr><td>0x48</td><td><a href="#reg-6.1-uart0_rxd_sel">UART0_RXD_SEL</a></td><td>UART0_RXD 管脚复用控制寄存器。</td></tr>
<tr><td>0x800</td><td><a href="#reg-6.1-pad_gpio_00_ctrl">PAD_GPIO_00_CTRL</a></td><td>GPIO_00 功能管脚控制寄存器。</td></tr>
<tr><td>0x804</td><td><a href="#reg-6.1-pad_gpio_01_ctrl">PAD_GPIO_01_CTRL</a></td><td>GPIO_01 功能管脚控制寄存器。</td></tr>
<tr><td>0x808</td><td><a href="#reg-6.1-pad_gpio_02_ctrl">PAD_GPIO_02_CTRL</a></td><td>GPIO_02 功能管脚控制寄存器。</td></tr>
<tr><td>0x80c</td><td><a href="#reg-6.1-pad_gpio_03_ctrl">PAD_GPIO_03_CTRL</a></td><td>GPIO_03 功能管脚控制寄存器。</td></tr>
<tr><td>0x810</td><td><a href="#reg-6.1-pad_gpio_04_ctrl">PAD_GPIO_04_CTRL</a></td><td>GPIO_04 功能管脚控制寄存器。</td></tr>
<tr><td>0x814</td><td><a href="#reg-6.1-pad_gpio_05_ctrl">PAD_GPIO_05_CTRL</a></td><td>GPIO_05 功能管脚控制寄存器。</td></tr>
<tr><td>0x818</td><td><a href="#reg-6.1-pad_gpio_06_ctrl">PAD_GPIO_06_CTRL</a></td><td>GPIO_06 功能管脚控制寄存器。</td></tr>
<tr><td>0x81c</td><td><a href="#reg-6.1-pad_gpio_07_ctrl">PAD_GPIO_07_CTRL</a></td><td>GPIO_07 功能管脚控制寄存器。</td></tr>
<tr><td>0x820</td><td><a href="#reg-6.1-pad_gpio_08_ctrl">PAD_GPIO_08_CTRL</a></td><td>GPIO_08 功能管脚控制寄存器。</td></tr>
<tr><td>0x824</td><td><a href="#reg-6.1-pad_gpio_09_ctrl">PAD_GPIO_09_CTRL</a></td><td>GPIO_09 功能管脚控制寄存器。</td></tr>
<tr><td>0x828</td><td><a href="#reg-6.1-pad_gpio_10_ctrl">PAD_GPIO_10_CTRL</a></td><td>GPIO_10 功能管脚控制寄存器。</td></tr>
<tr><td>0x82c</td><td><a href="#reg-6.1-pad_gpio_11_ctrl">PAD_GPIO_11_CTRL</a></td><td>GPIO_11 功能管脚控制寄存器。</td></tr>
<tr><td>0x830</td><td><a href="#reg-6.1-pad_gpio_12_ctrl">PAD_GPIO_12_CTRL</a></td><td>GPIO_12 功能管脚控制寄存器。</td></tr>
<tr><td>0x834</td><td><a href="#reg-6.1-pad_gpio_13_ctrl">PAD_GPIO_13_CTRL</a></td><td>GPIO_13 功能管脚控制寄存器。</td></tr>
<tr><td>0x838</td><td><a href="#reg-6.1-pad_gpio_14_ctrl">PAD_GPIO_14_CTRL</a></td><td>GPIO_14 功能管脚控制寄存器。</td></tr>
<tr><td>0x83c</td><td>PAD_UART1_TXD_C TRL</td><td>UART1_TXD 功能管脚控制寄存器。</td></tr>
<tr><td>0x840</td><td>PAD_UART1_RXD_C TRL</td><td>UART1_RXD 功能管脚控制寄存器。</td></tr>
<tr><td>0x844</td><td>PAD_UART0_TXD_C TRL</td><td>UART0_TXD 功能管脚控制寄存器。</td></tr>
<tr><td>0x848</td><td>PAD_UART0_RXD_C TRL</td><td>UART0_RXD 功能管脚控制寄存器。</td></tr>
<tr><td>0x868</td><td>PAD_SFC_CLK_CTR L</td><td>SFC_CLK 功能管脚控制寄存器。</td></tr>
<tr><td>0x86c</td><td>PAD_SFC_CSN_CTR L</td><td>SFC_CSN 功能管脚控制寄存器。</td></tr>
<tr><td>0x870</td><td><a href="#reg-6.1-pad_sfc_io0_ctrl">PAD_SFC_IO0_CTRL</a></td><td>SFC_IO0 功能管脚控制寄存器。</td></tr>
<tr><td>0x874</td><td><a href="#reg-6.1-pad_sfc_io1_ctrl">PAD_SFC_IO1_CTRL</a></td><td>SFC_IO1 功能管脚控制寄存器。</td></tr>
<tr><td>0x878</td><td><a href="#reg-6.1-pad_sfc_io2_ctrl">PAD_SFC_IO2_CTRL</a></td><td>SFC_IO2 功能管脚控制寄存器。</td></tr>
<tr><td>0x87c</td><td><a href="#reg-6.1-pad_sfc_io3_ctrl">PAD_SFC_IO3_CTRL</a></td><td>SFC_IO3 功能管脚控制寄存器。</td></tr>
</tbody>
</table>

<a id="6.1.4"></a>
## 寄存器描述

<a id="reg-6.1-gpio_00_sel"></a>
### GPIO_00_SEL

GPIO_00_SEL 为 GPIO_00 复用关系配置。

Offset Address: 0x0 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:3]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000000</td></tr>
<tr><td>[2:0]</td><td>RW</td><td>gpio_00_sel</td><td>GPIO_00 管脚复用：<br>
0: GPIO_00<br>
1: PWM0<br>
2: DIAG[0]<br>
3: SPI1_CSN<br>
4: JTAG_TDI<br>
其他: 保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-gpio_01_sel"></a>
### GPIO_01_SEL

GPIO_01_SEL 为 GPIO_01 复用关系配置。

Offset Address: 0x4 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:3]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000000</td></tr>
<tr><td>[2:0]</td><td>RW</td><td>gpio_01_sel</td><td>GPIO_01 管脚复用：<br>
0: GPIO_01<br>
1: PWM1<br>
2: DIAG[1]<br>
3: SPI1_IO0<br>
4: JTAG_MODE<br>
5: BT_SAMPLE<br>
其他: 保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-gpio_02_sel"></a>
### GPIO_02_SEL

GPIO_02_SEL 为 GPIO_02 复用关系配置。

Offset Address: 0x8 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:3]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000000</td></tr>
<tr><td>[2:0]</td><td>RW</td><td>gpio_02_sel</td><td>GPIO_02 管脚复用：<br>
0: GPIO_02<br>
1: PWM2<br>
2: DIAG[2]<br>
3: SPI1_IO3<br>
4: WIFI_TSF_SYNC<br>
5: WL_GLP_SYNC_PULSE<br>
6:BSLE_GLP_SYNC_PULSE<br>
其他: 保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-gpio_03_sel"></a>
### GPIO_03_SEL

GPIO_03_SEL 为 GPIO_03 复用关系配置。  
Offset Address: 0xc Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:3]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000000</td></tr>
<tr><td>[2:0]</td><td>RW</td><td>gpio_03_sel</td><td>GPIO_03 管脚复用：<br>
0: GPIO_03<br>
1: PWM3<br>
2: PMU_32K_TEST<br>
3: SPI1_IO1<br>
4: HW_ID[0]<br>
5: DIAG[3]<br>
其他: 保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-gpio_04_sel"></a>
### GPIO_04_SEL

GPIO_04_SEL 为 GPIO_04 复用关系配置。

Offset Address: 0x10 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:3]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000000</td></tr>
<tr><td>[2:0]</td><td>RW</td><td>gpio_04_sel</td><td>GPIO_04 管脚复用：<br>
0: SSI_CLK<br>
1: PWM4<br>
2: GPIO_04<br>
3: SPI1_IO1<br>
4: JTAG_ENABLE<br>
5: DFT_JTAG_TMS<br>
其他: 保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-gpio_05_sel"></a>
### GPIO_05_SEL

GPIO_05_SEL 为 GPIO_05 复用关系配置。

Offset Address: 0x14 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:3]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000000</td></tr>
<tr><td>[2:0]</td><td>RW</td><td>gpio_05_sel</td><td>GPIO_05 管脚复用：<br>
0: SSI_DATA<br>
1: PWM5<br>
2: UART2_CTS<br>
3: SPI1_IO2<br>
4: GPIO_05<br>
5: SPI0_IN<br>
6: DFT_JTAG_TCK<br>
其他: 保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-gpio_06_sel"></a>
### GPIO_06_SEL

GPIO_06_SEL 为 GPIO_06 复用关系配置。  
Offset Address: 0x18 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:3]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000000</td></tr>
<tr><td>[2:0]</td><td>RW</td><td>gpio_06_sel</td><td>GPIO_06 管脚复用：<br>
0: GPIO_06<br>
1: PWM6<br>
2: UART2_RTS<br>
3: SPI1_SCK<br>
4:REFCLK_FREQ_STATUS<br>
5: DIAG[4]<br>
6: SPI0_OUT<br>
7: DFT_JTAG_TDI<br>
其他: 保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-gpio_07_sel"></a>
### GPIO_07_SEL

GPIO_07_SEL 为 GPIO_07 复用关系配置。

Offset Address: 0x1c Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:3]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000000</td></tr>
<tr><td>[2:0]</td><td>RW</td><td>gpio_07_sel</td><td>GPIO_07 管脚复用：<br>
0: GPIO_07<br>
1: PWM7<br>
2: UART2_RXD<br>
3: SPI0_SCK<br>
4: I2S_MCLK<br>
5: DIAG[5]<br>
其他:保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-gpio_08_sel"></a>
### GPIO_08_SEL

GPIO_08_SEL 为 GPIO_08 复用关系配置。  
Offset Address: 0x20 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:3]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000000</td></tr>
<tr><td>[2:0]</td><td>RW</td><td>gpio_08_sel</td><td>GPIO_08 管脚复用：<br>
0: GPIO_08<br>
1: PWM0<br>
2: UART2_TXD<br>
3: SPI0_CS1_N<br>
4: DIAG[6]<br>
其他: 保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-gpio_09_sel"></a>
### GPIO_09_SEL

GPIO_09_SEL 为 GPIO_09 复用关系配置  
Offset Address: 0x24 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:3]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000000</td></tr>
<tr><td>[2:0]</td><td>RW</td><td>gpio_09_sel</td><td>GPIO_09 管脚复用：<br>
0: GPIO_09<br>
1: PWM1<br>
2: RADAR_ANT0_SW<br>
3: SPI0_OUT<br>
4: I2S_DO<br>
5: HW_ID[1]<br>
6: DIAG[7]<br>
7: JTAG_TDO<br>
其他:保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-gpio_10_sel"></a>
### GPIO_10_SEL

GPIO_10_SEL 为 GPIO_10 复用关系配置。  
Offset Address: 0x28 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:3]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000000</td></tr>
<tr><td>[2:0]</td><td>RW</td><td>gpio_10_sel</td><td>GPIO_10 管脚复用：<br>
0: GPIO_10<br>
1: PWM2<br>
2: ANT0_SW<br>
3: SPI0_CS0_N<br>
4: I2S_SCLK<br>
5: DIAG[0]<br>
其他: 保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-gpio_11_sel"></a>
### GPIO_11_SEL

GPIO_11_SEL 为 GPIO_11 复用关系配置。  
Offset Address: 0x2c Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:3]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000000</td></tr>
<tr><td>[2:0]</td><td>RW</td><td>gpio_11_sel</td><td>GPIO_11 管脚复用：<br>
0: GPIO_11<br>
1: PWM3<br>
2: RADAR_ANT1_SW<br>
3: SPI0_IN<br>
4: I2S_LRCLK<br>
5: DIAG[1]<br>
6: HW_ID[2]<br>
其他: 保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-gpio_12_sel"></a>
### GPIO_12_SEL

GPIO_12_SEL 为 GPIO_12 复用关系配置。

Offset Address: 0x30 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:3]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000000</td></tr>
<tr><td>[2:0]</td><td>RW</td><td>gpio_12_sel</td><td>GPIO_12 管脚复用：<br>
0: GPIO_12<br>
1: PWM4<br>
2: ANT1_SW<br>
4: I2S_DI<br>
6: HW_ID[3]<br>
其他: 保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-gpio_13_sel"></a>
### GPIO_13_SEL

GPIO_13_SEL 为 GPIO_13 复用关系配置。

Offset Address: 0x34 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:3]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000000</td></tr>
<tr><td>[2:0]</td><td>RW</td><td>gpio_13_sel</td><td>GPIO_13 管脚复用:</td><td>0x0</td></tr>
<tr><td></td><td></td><td></td><td>0: GPIO_13：<br>
1: UART1_CTS<br>
2: RADAR_ANT0_SW<br>
3: DFT_JTAG_TDO<br>
4: JTAG_TMS<br>
其他:保留。</td><td></td></tr>
</tbody>
</table>

<a id="reg-6.1-gpio_14_sel"></a>
### GPIO_14_SEL

GPIO_14_SEL 为 GPIO_14 复用关系配置。  
Offset Address: 0x38 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:3]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000000</td></tr>
<tr><td>[2:0]</td><td>RW</td><td>gpio_14_sel</td><td>GPIO_14 管脚复用：<br>
0: GPIO_14<br>
1: UART1_RTS<br>
2: RADAR_ANT1_SW<br>
3: DFT_JTAG_TRSTN<br>
4: JTAG_TCK<br>
其他: 保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-uart1_txd_sel"></a>
### UART1_TXD_SEL

UART1_TXD_SEL 为 UART1_TXD 复用关系配置。  
Offset Address: 0x3c Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:2]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000000</td></tr>
<tr><td>[1:0]</td><td>RW</td><td>uart1_txd_sel</td><td>UART1_TXD 管脚复用：<br>
0: GPIO_15<br>
1: UART1_TXD<br>
2: I2C1_SDA<br>
其他:保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-uart1_rxd_sel"></a>
### UART1_RXD_SEL

UART1_RXD_SEL 为 UART1_RXD 复用关系配置。  
Offset Address: 0x40 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:2]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000000</td></tr>
<tr><td>[1:0]</td><td>RW</td><td>uart1_rxd_sel</td><td>UART1_RXD 管脚复用：<br>
0: GPIO_16<br>
1: UART1_RXD<br>
2: I2C1_SCL<br>
其他: 保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-uart0_txd_sel"></a>
### UART0_TXD_SEL

UART0_TXD_SEL 为 UART0_TXD 复用关系配置。

Offset Address: 0x44 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:2]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000000</td></tr>
<tr><td>[1:0]</td><td>RW</td><td>uart0_txd_sel</td><td>UART0_TXD 管脚复用：<br>
0: GPIO_17<br>
1: UART0_TXD<br>
2: I2C0_SDA<br>
其他: 保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-uart0_rxd_sel"></a>
### UART0_RXD_SEL

UART0_RXD_SEL 为 UART0_RXD 复用关系配置。

Offset Address: 0x48 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:2]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000000</td></tr>
<tr><td>[1:0]</td><td>RW</td><td>uart0_rxd_sel</td><td>UART0_RXD 管脚复用：<br>
0: GPIO_18<br>
1: UART0_RXD<br>
2: I2C0_SCL<br>
其他: 保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-pad_gpio_00_ctrl"></a>
### PAD_GPIO_00_CTRL

PAD_GPIO_00_CTRL 为 GPIO_00 控制寄存器。

Offset Address: 0x800 Total Reset Value: 0x0000_0800

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:12]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000</td></tr>
<tr><td>[11]</td><td>RW</td><td>pad_gpio_00_ctrl_ie</td><td>GPIO_00.IE 管脚控制：<br>
0:输入禁止<br>
1:输入使能。</td><td>0x1</td></tr>
<tr><td>[10]</td><td>RW</td><td>pad_gpio_00_ctrl_ps</td><td>GPIO_00.PS 管脚控制:需与 GPIO_00.PE 管脚控制结合使用。PE/PS 对应上下拉能力控制如下：<br>
00:无上下拉<br>
01:强上拉<br>
10:下拉<br>
11:上拉。</td><td>0x0</td></tr>
<tr><td>[9]</td><td>RW</td><td>pad_gpio_00_ctrl_pe</td><td>GPIO_00.PE 管脚控制。</td><td>0x0</td></tr>
<tr><td>[8:7]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
<tr><td>[6]</td><td>RW</td><td>pad_gpio_00_ctrl_ds2</td><td>GPIO_00.DS2 管脚控制:需与GPIO_00.DS1/GPIO_00.DS0管脚控制结合使用。DS2~DS0 对应驱动能力调节:111~000,驱动能力依次减弱。</td><td>0x0</td></tr>
<tr><td>[5]</td><td>RW</td><td>pad_gpio_00_ctrl_ds1</td><td>GPIO_00.DS1 管脚控制。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>RW</td><td>pad_gpio_00_ctrl_ds0</td><td>GPIO_00.DS0 管脚控制。</td><td>0x0</td></tr>
<tr><td>[3]</td><td>RW</td><td>pad_gpio_00_ctrl_st</td><td>GPIO_00.ST 管脚控制：<br>
0: No Schmitt<br>
1: Schmitt Enable。</td><td>0x0</td></tr>
<tr><td>[2:0]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-pad_gpio_01_ctrl"></a>
### PAD_GPIO_01_CTRL

PAD_GPIO_01_CTRL 为 GPIO_01 控制寄存器。  
Offset Address: 0x804 Total Reset Value: 0x0000_0A00

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:12]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000</td></tr>
<tr><td>[11]</td><td>RW</td><td>pad_gpio_01_ctrl_ie</td><td>GPIO_01.IE 管脚控制：<br>
0:输入禁止<br>
1:输入使能。</td><td>0x1</td></tr>
<tr><td>[10]</td><td>RW</td><td>pad_gpio_01_ctrl_ps</td><td>GPIO_01.PS 管脚控制:需与 GPIO_01.PE 管脚控制结合使用。PE/PS对应上下拉能力控制如下：<br>
00:无上下拉<br>
01:强上拉<br>
10:下拉<br>
11:上拉。</td><td>0x0</td></tr>
<tr><td>[9]</td><td>RW</td><td>pad_gpio_01_ctrl_pe</td><td>GPIO_01.PE管脚控制。</td><td>0x1</td></tr>
<tr><td>[8:7]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
<tr><td>[6]</td><td>RW</td><td>pad_gpio_01_ctrl_ds2</td><td>GPIO_01.DS2管脚控制:需与GPIO_01.DS1/GPIO_01.DS0管脚控制结合使用。DS2~DS0对应驱动能力调节:111至000,驱动能力依次减弱。</td><td>0x0</td></tr>
<tr><td>[5]</td><td>RW</td><td>pad_gpio_01_ctrl_ds1</td><td>GPIO_01.DS1管脚控制。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>RW</td><td>pad_gpio_01_ctrl_ds0</td><td>GPIO_01.DS0管脚控制。</td><td>0x0</td></tr>
<tr><td>[3]</td><td>RW</td><td>pad_gpio_01_ctrl_st</td><td>GPIO_01.ST管脚控制：<br>
0:No Schmitt<br>
1:Schmitt Enable。</td><td>0x0</td></tr>
<tr><td>[2:0]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-pad_gpio_02_ctrl"></a>
### PAD_GPIO_02_CTRL

PAD_GPIO_02_CTRL 为 GPIO_02 控制寄存器。

Offset Address: 0x808 Total Reset Value: 0x0000_0800

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:12]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000</td></tr>
<tr><td>[11]</td><td>RW</td><td>pad_gpio_02_ctrl_ie</td><td>GPIO_02.IE 管脚控制：<br>
0:输入禁止<br>
1:输入使能。</td><td>0x1</td></tr>
<tr><td>[10]</td><td>RW</td><td>pad_gpio_02_ctrl_ps</td><td>GPIO_02.PS 管脚控制:需与 GPIO_02.PE 管脚控制结合使用。PE/PS 对应上下拉能力控制如下：<br>
00:无上下拉<br>
01:强上拉<br>
10:下拉<br>
11:上拉。</td><td>0x0</td></tr>
<tr><td>[9]</td><td>RW</td><td>pad_gpio_02_ctrl_pe</td><td>GPIO_02.PE 管脚控制。</td><td>0x0</td></tr>
<tr><td>[8:7]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
<tr><td>[6]</td><td>RW</td><td>pad_gpio_02_ctrl_ds2</td><td>GPIO_02.DS2 管脚控制:需与GPIO_02.DS1/GPIO_02.DS0 管脚控制结合使用。DS2~DS0 对应驱动能力调节:111~000,驱动能力依次减弱。</td><td>0x0</td></tr>
<tr><td>[5]</td><td>RW</td><td>pad_gpio_02_ctrl_ds1</td><td>GPIO_02.DS1 管脚控制。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>RW</td><td>pad_gpio_02_ctrl_ds0</td><td>GPIO_02.DS0 管脚控制。</td><td>0x0</td></tr>
<tr><td>[3]</td><td>RW</td><td>pad_gpio_02_ctrl_st</td><td>GPIO_02.ST 管脚控制：<br>
0:No Schmitt<br>
1:Schmitt Enable。</td><td>0x0</td></tr>
<tr><td>[2:0]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-pad_gpio_03_ctrl"></a>
### PAD_GPIO_03_CTRL

PAD_GPIO_03_CTRL 为 GPIO_03 控制寄存器。  
Offset Address: 0x80c Total Reset Value: 0x0000_0A00

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:12]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000</td></tr>
<tr><td>[11]</td><td>RW</td><td>pad_gpio_03_ctrl_ie</td><td>GPIO_03.IE 管脚控制：<br>
0:输入禁止<br>
1:输入使能。</td><td>0x1</td></tr>
<tr><td>[10]</td><td>RW</td><td>pad_gpio_03_ctrl_ps</td><td>GPIO_03.PS 管脚控制:需与 GPIO_03.PE 管脚控制结合使用。PE/PS 对应上下拉能力控制如下：<br>
00:无上下拉<br>
01:强上拉<br>
10:下拉<br>
11:上拉。</td><td>0x0</td></tr>
<tr><td>[9]</td><td>RW</td><td>pad_gpio_03_ctrl_pe</td><td>GPIO_03.PE 管脚控制。</td><td>0x1</td></tr>
<tr><td>[8:7]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
<tr><td>[6]</td><td>RW</td><td>pad_gpio_03_ctrl_ds2</td><td>GPIO_03.DS2 管脚控制:需与GPIO_03.DS1/GPIO_03.DS0 管脚控制结合使用。DS2~DS0 对应驱动能力调节:111 至 000,驱动能力依次减弱。</td><td>0x0</td></tr>
<tr><td>[5]</td><td>RW</td><td>pad_gpio_03_ctrl_ds1</td><td>GPIO_03.DS1 管脚控制。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>RW</td><td>pad_gpio_03_ctrl_ds0</td><td>GPIO_03.DS0 管脚控制。</td><td>0x0</td></tr>
<tr><td>[3]</td><td>RW</td><td>pad_gpio_03_ctrl_st</td><td>GPIO_03.ST 管脚控制：<br>
0: No Schmitt<br>
1: Schmitt Enable。</td><td>0x0</td></tr>
<tr><td>[2:0]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-pad_gpio_04_ctrl"></a>
### PAD_GPIO_04_CTRL

PAD_GPIO_04_CTRL 为 GPIO_04 控制寄存器。

Offset Address: 0x810 Total Reset Value: 0x0000_0A00

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:12]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000</td></tr>
<tr><td>[11]</td><td>RW</td><td>pad_gpio_04_ctrl_ie</td><td>GPIO_04.IE 管脚控制：<br>
0:输入禁止<br>
1:输入使能。</td><td>0x1</td></tr>
<tr><td>[10]</td><td>RW</td><td>pad_gpio_04_ctrl_ps</td><td>GPIO_04.PS 管脚控制:需与 GPIO_04.PE 管脚控制结合使用。PE/PS 对应上下拉能力控制如下：<br>
00:无上下拉<br>
01:强上拉<br>
10:下拉<br>
11:上拉。</td><td>0x0</td></tr>
<tr><td>[9]</td><td>RW</td><td>pad_gpio_04_ctrl_pe</td><td>GPIO_04.PE 管脚控制。</td><td>0x1</td></tr>
<tr><td>[8:7]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
<tr><td>[6]</td><td>RW</td><td>pad_gpio_04_ctrl_ds2</td><td>GPIO_04.DS2 管脚控制:需与GPIO_04.DS1/GPIO_04.DS0管脚控制结合使用。DS2~DS0对应驱动能力调节:111~000,驱动能力依次减弱。</td><td>0x0</td></tr>
<tr><td>[5]</td><td>RW</td><td>pad_gpio_04_ctrl_ds1</td><td>GPIO_04.DS1 管脚控制。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>RW</td><td>pad_gpio_04_ctrl_ds0</td><td>GPIO_04.DS0 管脚控制。</td><td>0x0</td></tr>
<tr><td>[3]</td><td>RW</td><td>pad_gpio_04_ctrl_st</td><td>GPIO_04.ST 管脚控制：<br>
0: No Schmitt<br>
1: Schmitt Enable。</td><td>0x0</td></tr>
<tr><td>[2:0]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-pad_gpio_05_ctrl"></a>
### PAD_GPIO_05_CTRL

PAD_GPIO_05_CTRL 为 GPIO_05 控制寄存器。  
Offset Address: 0x814 Total Reset Value: 0x0000_0820

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:12]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000</td></tr>
<tr><td>[11]</td><td>RW</td><td>pad_gpio_05_ctrl_ie</td><td>GPIO_05.IE 管脚控制：<br>
0:输入禁止<br>
1:输入使能。</td><td>0x1</td></tr>
<tr><td>[10]</td><td>RW</td><td>pad_gpio_05_ctrl_ps</td><td>GPIO_05.PS 管脚控制:需与 GPIO_05.PE 管脚控制结合使用。PE/PS 对应上下拉能力控制如下：<br>
00:无上下拉<br>
01: 强上拉<br>
10: 下拉<br>
11: 上拉。</td><td>0x0</td></tr>
<tr><td>[9]</td><td>RW</td><td>pad_gpio_05_ctrl_pe</td><td>GPIO_05.PE 管脚控制。</td><td>0x0</td></tr>
<tr><td>[8:7]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
<tr><td>[6]</td><td>RW</td><td>pad_gpio_05_ctrl_ds2</td><td>GPIO_05.DS2 管脚控制:需与GPIO_05.DS1/GPIO_05.DS0 管脚控制结合使用。DS2~DS0 对应驱动能力调节:111~000, 驱动能力依次减弱。</td><td>0x0</td></tr>
<tr><td>[5]</td><td>RW</td><td>pad_gpio_05_ctrl_ds1</td><td>GPIO_05.DS1 管脚控制。</td><td>0x1</td></tr>
<tr><td>[4]</td><td>RW</td><td>pad_gpio_05_ctrl_ds0</td><td>GPIO_05.DS0 管脚控制。</td><td>0x0</td></tr>
<tr><td>[3]</td><td>RW</td><td>pad_gpio_05_ctrl_st</td><td>GPIO_05.ST 管脚控制：<br>
0: No Schmitt<br>
1: Schmitt Enable。</td><td>0x0</td></tr>
<tr><td>[2:0]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-pad_gpio_06_ctrl"></a>
### PAD_GPIO_06_CTRL

PAD_GPIO_06_CTRL 为 GPIO_06 控制寄存器。  
Offset Address: 0x818 Total Reset Value: 0x0000_0A00

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:12]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000</td></tr>
<tr><td>[11]</td><td>RW</td><td>pad_gpio_06_ctrl_ie</td><td>GPIO_06.IE 管脚控制：<br>
0:输入禁止<br>
1:输入使能。</td><td>0x1</td></tr>
<tr><td>[10]</td><td>RW</td><td>pad_gpio_06_ctrl_ps</td><td>GPIO_06.PS 管脚控制:需与 GPIO_06.PE 管脚控制结合使用。PE/PS 对应上下拉能力控制如下：<br>
00:无上下拉<br>
01:强上拉<br>
10:下拉<br>
11:上拉。</td><td>0x0</td></tr>
<tr><td>[9]</td><td>RW</td><td>pad_gpio_06_ctrl_pe</td><td>GPIO_06.PE 管脚控制。</td><td>0x1</td></tr>
<tr><td>[8:7]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
<tr><td>[6]</td><td>RW</td><td>pad_gpio_06_ctrl_ds2</td><td>GPIO_06.DS2 管脚控制:需与GPIO_06.DS1/GPIO_06.DS0 管脚控制结合使用。DS2~DS0 对应驱动能力调节:111~000,驱动能力依次减弱。</td><td>0x0</td></tr>
<tr><td>[5]</td><td>RW</td><td>pad_gpio_06_ctrl_ds1</td><td>GPIO_06.DS1 管脚控制。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>RW</td><td>pad_gpio_06_ctrl_ds0</td><td>GPIO_06.DS0 管脚控制。</td><td>0x0</td></tr>
<tr><td>[3]</td><td>RW</td><td>pad_gpio_06_ctrl_st</td><td>GPIO_06.ST 管脚控制：<br>
0:No Schmitt<br>
1:Schmitt Enable。</td><td>0x0</td></tr>
<tr><td>[2:0]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-pad_gpio_07_ctrl"></a>
### PAD_GPIO_07_CTRL

PAD_GPIO_07_CTRL 为 GPIO_07 控制寄存器。

Offset Address: 0x81c Total Reset Value: 0x0000_0800

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:12]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000</td></tr>
<tr><td>[11]</td><td>RW</td><td>pad_gpio_07_ctrl_ie</td><td>GPIO_07.IE 管脚控制：<br>
0:输入禁止<br>
1:输入使能。</td><td>0x1</td></tr>
<tr><td>[10]</td><td>RW</td><td>pad_gpio_07_ctrl_ps</td><td>GPIO_07.PS 管脚控制:需与 GPIO_07.PE 管脚控制结合使用。PE/PS 对应上下拉能力控制如下：<br>
00:无上下拉<br>
01:强上拉<br>
10:下拉<br>
11:上拉。</td><td>0x0</td></tr>
<tr><td>[9]</td><td>RW</td><td>pad_gpio_07_ctrl_pe</td><td>GPIO_07.PE 管脚控制。</td><td>0x0</td></tr>
<tr><td>[8:7]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
<tr><td>[6]</td><td>RW</td><td>pad_gpio_07_ctrl_ds2</td><td>GPIO_07.DS2 管脚控制:需与GPIO_07.DS1/GPIO_07.DS0 管脚控制结合使用。DS2~DS0 对应驱动能力调节:111~000,驱动能力依次减弱。</td><td>0x0</td></tr>
<tr><td>[5]</td><td>RW</td><td>pad_gpio_07_ctrl_ds1</td><td>GPIO_07.DS1 管脚控制。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>RW</td><td>pad_gpio_07_ctrl_ds0</td><td>GPIO_07.DS0 管脚控制。</td><td>0x0</td></tr>
<tr><td>[3]</td><td>RW</td><td>pad_gpio_07_ctrl_st</td><td>GPIO_07.ST 管脚控制：<br>
0:No Schmitt<br>
1:Schmitt Enable。</td><td>0x0</td></tr>
<tr><td>[2:0]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-pad_gpio_08_ctrl"></a>
### PAD_GPIO_08_CTRL

PAD_GPIO_08_CTRL 为 GPIO_08 控制寄存器。

Offset Address: 0x820 Total Reset Value: 0x0000_0800

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:12]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000</td></tr>
<tr><td>[11]</td><td>RW</td><td>pad_gpio_08_ctrl_ie</td><td>GPIO_08.IE 管脚控制：<br>
0:输入禁止<br>
1:输入使能。</td><td>0x1</td></tr>
<tr><td>[10]</td><td>RW</td><td>pad_gpio_08_ctrl_ps</td><td>GPIO_08.PS 管脚控制:需与 GPIO_08.PE 管脚控制结合使用。PE/PS 对应上下拉能力控制如下：<br>
00:无上下拉<br>
01:强上拉<br>
10:下拉<br>
11:上拉。</td><td>0x0</td></tr>
<tr><td>[9]</td><td>RW</td><td>pad_gpio_08_ctrl_pe</td><td>GPIO_08.PE 管脚控制。</td><td>0x0</td></tr>
<tr><td>[8:7]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
<tr><td>[6]</td><td>RW</td><td>pad_gpio_08_ctrl_ds2</td><td>GPIO_08.DS2 管脚控制:需与 GPIO_08.DS1/GPIO_08.DS0 管脚控制结合使用。DS2~DS0 对应驱动能力调节:111~000,驱动能力依次减弱。</td><td>0x0</td></tr>
<tr><td>[5]</td><td>RW</td><td>pad_gpio_08_ctrl_ds1</td><td>GPIO_08.DS1 管脚控制。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>RW</td><td>pad_gpio_08_ctrl_ds0</td><td>GPIO_08.DS0 管脚控制。</td><td>0x0</td></tr>
<tr><td>[3]</td><td>RW</td><td>pad_gpio_08_ctrl_st</td><td>GPIO_08.ST 管脚控制：<br>
0: No Cchmitt<br>
1: Schmitt tEnable。</td><td>0x0</td></tr>
<tr><td>[2:0]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-pad_gpio_09_ctrl"></a>
### PAD_GPIO_09_CTRL

PAD_GPIO_09_CTRL 为 GPIO_09 控制寄存器。

Offset Address: 0x824 Total Reset Value: 0x0000_0A00

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:12]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000</td></tr>
<tr><td>[11]</td><td>RW</td><td>pad_gpio_09_ctrl_ie</td><td>GPIO_09.IE 管脚控制：<br>
0:输入禁止<br>
1:输入使能。</td><td>0x1</td></tr>
<tr><td>[10]</td><td>RW</td><td>pad_gpio_09_ctrl_ps</td><td>GPIO_09.PS 管脚控制:需与 GPIO_09.PE 管脚控制结合使用。PE/PS 对应上下拉能力控制如下：<br>
00:无上下拉<br>
01:强上拉<br>
10:下拉<br>
11:上拉。</td><td>0x0</td></tr>
<tr><td>[9]</td><td>RW</td><td>pad_gpio_09_ctrl_pe</td><td>GPIO_09.PE 管脚控制。</td><td>0x1</td></tr>
<tr><td>[8:7]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
<tr><td>[6]</td><td>RW</td><td>pad_gpio_09_ctrl_ds2</td><td>GPIO_09.DS2 管脚控制:需与GPIO_09.DS1/GPIO_09.DS0 管脚控制结合使用。DS2~DS0 对应驱动能力调节:111~000,驱动能力依次减弱。</td><td>0x0</td></tr>
<tr><td>[5]</td><td>RW</td><td>pad_gpio_09_ctrl_ds1</td><td>GPIO_09.DS1 管脚控制。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>RW</td><td>pad_gpio_09_ctrl_ds0</td><td>GPIO_09.DS0 管脚控制。</td><td>0x0</td></tr>
<tr><td>[3]</td><td>RW</td><td>pad_gpio_09_ctrl_st</td><td>GPIO_09.ST 管脚控制：<br>
0: No Schmitt<br>
1: Schmitt Enable。</td><td>0x0</td></tr>
<tr><td>[2:0]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-pad_gpio_10_ctrl"></a>
### PAD_GPIO_10_CTRL

PAD_GPIO_10_CTRL 为 GPIO_10 控制寄存器。  
Offset Address: 0x828 Total Reset Value: 0x0000_0800

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:12]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000</td></tr>
<tr><td>[11]</td><td>RW</td><td>pad_gpio_10_ctrl_ie</td><td>GPIO_10.IE 管脚控制：<br>
0:输入禁止<br>
1:输入使能。</td><td>0x1</td></tr>
<tr><td>[10]</td><td>RW</td><td>pad_gpio_10_ctrl_ps</td><td>GPIO_10.PS 管脚控制:需与 GPIO_10.PE 管脚控制结合使用。PE/PS 对应上下拉能力控制如下：<br>
00:无上下拉<br>
01:强上拉<br>
10:下拉<br>
11:上拉。</td><td>0x0</td></tr>
<tr><td>[9]</td><td>RW</td><td>pad_gpio_10_ctrl_pe</td><td>GPIO_10.PE 管脚控制。</td><td>0x0</td></tr>
<tr><td>[8:7]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
<tr><td>[6]</td><td>RW</td><td>pad_gpio_10_ctrl_ds2</td><td>GPIO_10.DS2 管脚控制:需与GPIO_10.DS1/GPIO_10.DS0 管脚控制结合使用。DS2~DS0 对应驱动能力调节:111~000,驱动能力依次减弱。</td><td>0x0</td></tr>
<tr><td>[5]</td><td>RW</td><td>pad_gpio_10_ctrl_ds1</td><td>GPIO_10.DS1 管脚控制。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>RW</td><td>pad_gpio_10_ctrl_ds0</td><td>GPIO_10.DS0 管脚控制。</td><td>0x0</td></tr>
<tr><td>[3]</td><td>RW</td><td>pad_gpio_10_ctrl_st</td><td>GPIO_10.ST 管脚控制：<br>
0: No Schmitt<br>
1: Schmitt Enable。</td><td>0x0</td></tr>
<tr><td>[2:0]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-pad_gpio_11_ctrl"></a>
### PAD_GPIO_11_CTRL

PAD_GPIO_11_CTRL 为 GPIO_11 控制寄存器。  
Offset Address: 0x82c Total Reset Value: 0x0000_0A00

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:12]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000</td></tr>
<tr><td>[11]</td><td>RW</td><td>pad_gpio_11_ctrl_ie</td><td>GPIO_11.IE 管脚控制：<br>
0:输入禁止<br>
1:输入使能。</td><td>0x1</td></tr>
<tr><td>[10]</td><td>RW</td><td>pad_gpio_11_ctrl_ps</td><td>GPIO_11.PS 管脚控制:需与 GPIO_11.PE 管脚控制结合使用。PE/PS 对应上下拉能力控制如下：<br>
00:无上下拉<br>
01:强上拉<br>
10:下拉<br>
11:上拉。</td><td>0x0</td></tr>
<tr><td>[9]</td><td>RW</td><td>pad_gpio_11_ctrl_pe</td><td>GPIO_11.PE 管脚控制。</td><td>0x1</td></tr>
<tr><td>[8:7]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
<tr><td>[6]</td><td>RW</td><td>pad_gpio_11_ctrl_ds2</td><td>GPIO_11.DS2 管脚控制:需与GPIO_11.DS1/GPIO_11.DS0 管脚控制结合使用。DS2~DS0 对应驱动能力调节:111~000,驱动能力依次减弱。</td><td>0x0</td></tr>
<tr><td>[5]</td><td>RW</td><td>pad_gpio_11_ctrl_ds1</td><td>GPIO_11.DS1 管脚控制。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>RW</td><td>pad_gpio_11_ctrl_ds0</td><td>GPIO_11.DS0 管脚控制。</td><td>0x0</td></tr>
<tr><td>[3]</td><td>RW</td><td>pad_gpio_11_ctrl_st</td><td>GPIO_11.ST 管脚控制：<br>
0:No Schmitt<br>
1:Schmitt Enable。</td><td>0x0</td></tr>
<tr><td>[2:0]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-pad_gpio_12_ctrl"></a>
### PAD_GPIO_12_CTRL

PAD_GPIO_12_CTRL 为 GPIO_12 控制寄存器。

Offset Address: 0x830 Total Reset Value: 0x0000_0A00

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:12]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000</td></tr>
<tr><td>[11]</td><td>RW</td><td>pad_gpio_12_ctrl_ie</td><td>GPIO_12.IE 管脚控制：<br>
0:输入禁止<br>
1:输入使能。</td><td>0x1</td></tr>
<tr><td>[10]</td><td>RW</td><td>pad_gpio_12_ctrl_ps</td><td>GPIO_12.PS 管脚控制:需与 GPIO_12.PE 管脚控制结合使用。PE/PS 对应上下拉能力控制如下：<br>
00:无上下拉<br>
01:强上拉<br>
10:下拉<br>
11:上拉。</td><td>0x0</td></tr>
<tr><td>[9]</td><td>RW</td><td>pad_gpio_12_ctrl_pe</td><td>GPIO_12.PE 管脚控制。</td><td>0x1</td></tr>
<tr><td>[8:7]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
<tr><td>[6]</td><td>RW</td><td>pad_gpio_12_ctrl_ds2</td><td>GPIO_12.DS2 管脚控制:需与GPIO_12.DS1/GPIO_12.DS0 管脚控制结合使用。DS2~DS0 对应驱动能力调节:111~000,驱动能力依次减弱。</td><td>0x0</td></tr>
<tr><td>[5]</td><td>RW</td><td>pad_gpio_12_ctrl_ds1</td><td>GPIO_12.DS1 管脚控制。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>RW</td><td>pad_gpio_12_ctrl_ds0</td><td>GPIO_12.DS0 管脚控制。</td><td>0x0</td></tr>
<tr><td>[3]</td><td>RW</td><td>pad_gpio_12_ctrl_st</td><td>GPIO_12.ST 管脚控制：<br>
0: No Schmitt<br>
1: Schmitt Enable。</td><td>0x0</td></tr>
<tr><td>[2:0]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-pad_gpio_13_ctrl"></a>
### PAD_GPIO_13_CTRL

PAD_GPIO_13_CTRL 为 GPIO_13 控制寄存器。

Offset Address: 0x834 Total Reset Value: 0x0000_0800

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:12]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000</td></tr>
<tr><td>[11]</td><td>RW</td><td>pad_gpio_13_ctrl_ie</td><td>GPIO_13.IE 管脚控制：<br>
0:输入禁止<br>
1:输入使能。</td><td>0x1</td></tr>
<tr><td>[10]</td><td>RW</td><td>pad_gpio_13_ctrl_ps</td><td>GPIO_13.PS 管脚控制:需与 GPIO_13.PE 管脚控制结合使用。PE/PS 对应上下拉能力控制如下：<br>
00:无上下拉<br>
01:强上拉<br>
10:下拉<br>
11:上拉。</td><td>0x0</td></tr>
<tr><td>[9]</td><td>RW</td><td>pad_gpio_13_ctrl_pe</td><td>GPIO_13.PE 管脚控制。</td><td>0x0</td></tr>
<tr><td>[8:7]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
<tr><td>[6]</td><td>RW</td><td>pad_gpio_13_ctrl_ds2</td><td>GPIO_13.DS2 管脚控制:需与GPIO_13.DS1/GPIO_13.DS0 管脚控制结合使用。DS2~DS0 对应驱动能力调节:111~000,驱动能力依次减弱。</td><td>0x0</td></tr>
<tr><td>[5]</td><td>RW</td><td>pad_gpio_13_ctrl_ds1</td><td>GPIO_13.DS1 管脚控制。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>RW</td><td>pad_gpio_13_ctrl_ds0</td><td>GPIO_13.DS0 管脚控制。</td><td>0x0</td></tr>
<tr><td>[3]</td><td>RW</td><td>pad_gpio_13_ctrl_st</td><td>GPIO_13.ST 管脚控制：<br>
0: No Schmitt<br>
1: Schmitt Enable。</td><td>0x0</td></tr>
<tr><td>[2:0]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-pad_gpio_14_ctrl"></a>
### PAD_GPIO_14_CTRL

PAD_GPIO_14_CTRL 为 GPIO_14 控制寄存器。  
Offset Address: 0x838 Total Reset Value: 0x0000_0800

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:12]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000</td></tr>
<tr><td>[11]</td><td>RW</td><td>pad_gpio_14_ctrl_ie</td><td>GPIO_14.IE 管脚控制：<br>
0:输入禁止<br>
1:输入使能。</td><td>0x1</td></tr>
<tr><td>[10]</td><td>RW</td><td>pad_gpio_14_ctrl_ps</td><td>GPIO_14.PS 管脚控制:需与 GPIO_14.PE 管脚控制结合使用。PE/PS 对应上下拉能力控制如下：<br>
00:无上下拉<br>
01:强上拉<br>
10:下拉<br>
11:上拉。</td><td>0x0</td></tr>
<tr><td>[9]</td><td>RW</td><td>pad_gpio_14_ctrl_pe</td><td>GPIO_14.PE 管脚控制。</td><td>0x0</td></tr>
<tr><td>[8:7]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
<tr><td>[6]</td><td>RW</td><td>pad_gpio_14_ctrl_ds2</td><td>GPIO_14.DS2 管脚控制:需与GPIO_14.DS1/GPIO_14.DS0 管脚控制结合使用。DS2~DS0 对应驱动能力调节:111~000,驱动能力依次减弱。</td><td>0x0</td></tr>
<tr><td>[5]</td><td>RW</td><td>pad_gpio_14_ctrl_ds1</td><td>GPIO_14.DS1 管脚控制。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>RW</td><td>pad_gpio_14_ctrl_ds0</td><td>GPIO_14.DS0 管脚控制。</td><td>0x0</td></tr>
<tr><td>[3]</td><td>RW</td><td>pad_gpio_14_ctrl_st</td><td>GPIO_14.ST 管脚控制：<br>
0:No Schmitt<br>
1:Schmitt Enable。</td><td>0x0</td></tr>
<tr><td>[2:0]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-pad_uart1_txd_ctrl"></a>
### PAD_UART1_TXD_CTRL

PAD_UART1_TXD_CTRL 为 UART1_TXD 控制寄存器。

Offset Address: 0x83c Total Reset Value: 0x0000_0800

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:12]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000</td></tr>
<tr><td>[11]</td><td>RW</td><td>pad_uart1_txd_ctrl_ie</td><td>UART1_TXD.IE 管脚控制：<br>
0:输入禁止<br>
1:输入使能。</td><td>0x1</td></tr>
<tr><td>[10:4]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00</td></tr>
<tr><td>[3]</td><td>RW</td><td>pad_uart1_txd_ctrl_st</td><td>UART1_TXD.ST 管脚控制：<br>
0: No Schmitt<br>
1: Schmitt Enable。</td><td>0x0</td></tr>
<tr><td>[2:0]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-pad_uart1_rxd_ctrl"></a>
### PAD_UART1_RXD_CTRL

PAD_UART1_RXD_CTRL 为 UART1_RXD 控制寄存器。

Offset Address: 0x840 Total Reset Value: 0x0000_0800

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:12]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000</td></tr>
<tr><td>[11]</td><td>RW</td><td>pad_uart1_rxd_ctrl_ie</td><td>UART1_RXD.IE 管脚控制:default：<br>
10:禁止<br>
1:使能。</td><td>0x1</td></tr>
<tr><td>[10:4]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00</td></tr>
<tr><td>[3]</td><td>RW</td><td>pad_uart1_rxd_ctrl_st</td><td>UART1_RXD.ST 管脚控制：<br>
0: No Schmitt<br>
1: Schmitt Enable。</td><td>0x0</td></tr>
<tr><td>[2:0]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-pad_uart0_txd_ctrl"></a>
### PAD_UART0_TXD_CTRL

PAD_UART0_TXD_CTRL 为 UART0_TXD 控制寄存器。

Offset Address: 0x844 Total Reset Value: 0x0000_0800

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:12]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000</td></tr>
<tr><td>[11]</td><td>RW</td><td>pad_uart0_txd_ctrl_ie</td><td>UART0_TXD.IE 管脚控制：<br>
0:输入禁止<br>
1:输入使能。</td><td>0x1</td></tr>
<tr><td>[10:4]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00</td></tr>
<tr><td>[3]</td><td>RW</td><td>pad_uart0_txd_ctrl_st</td><td>UART0_TXD.ST 管脚控制：<br>
0:No Schmitt<br>
1:Schmitt Enable。</td><td>0x0</td></tr>
<tr><td>[2:0]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-pad_uart0_rxd_ctrl"></a>
### PAD_UART0_RXD_CTRL

PAD_UART0_RXD_CTRL 为 UART0_RXD 控制寄存器。

Offset Address: 0x848 Total Reset Value: 0x0000_0800

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:12]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000</td></tr>
<tr><td>[11]</td><td>RW</td><td>pad_uart0_rxd_ctrl_ie</td><td>UART0_RXD.IE 管脚控制：<br>
0: 输入禁止<br>
1: 输入使能。</td><td>0x1</td></tr>
<tr><td>[10:4]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00</td></tr>
<tr><td>[3]</td><td>RW</td><td>pad_uart0_rxd_ctrl_st</td><td>UART0_RXD.ST 管脚控制：<br>
0: No Schmitt<br>
1: Schmitt Enable。</td><td>0x0</td></tr>
<tr><td>[2:0]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-pad_sfc_clk_ctrl"></a>
### PAD_SFC_CLK_CTRL

PAD_SFC_CLK_CTRL 为 SFC_CLK 控制寄存器。

Offset Address: 0x868 Total Reset Value: 0x0000_0810

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:12]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000</td></tr>
<tr><td>[11]</td><td>RW</td><td>pad_sfc_clk_ctrl_ie</td><td>SFC_CLK.IE 管脚控制：<br>
0:输入禁止<br>
1:输入使能。</td><td>0x1</td></tr>
<tr><td>[10]</td><td>RW</td><td>pad_sfc_clk_ctrl_ps</td><td>SFC_CLK.PS 管脚控制:需与 SFC_CLK.PE 管脚控制结合使用。PE/PS 对应上下拉能力控制如下：<br>
00:无上下拉<br>
01:强上拉<br>
10:下拉<br>
11:上拉。</td><td>0x0</td></tr>
<tr><td>[9]</td><td>RW</td><td>pad_sfc_clk_ctrl_pe</td><td>SFC_CLK.PE 管脚控制。</td><td>0x0</td></tr>
<tr><td>[8:7]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
<tr><td>[6]</td><td>RW</td><td>pad_sfc_clk_ctrl_ds2</td><td>SFC_CLK.DS2 管脚控制:需与SFC_CLK.DS1/SFC_CLK.DS0 管脚控制结合使用。DS2~DS0 对应驱动能力调节:111~000,驱动能力依次减弱。</td><td>0x0</td></tr>
<tr><td>[5]</td><td>RW</td><td>pad_sfc_clk_ctrl_ds1</td><td>SFC_CLK.DS1 管脚控制。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>RW</td><td>pad_sfc_clk_ctrl_ds0</td><td>SFC_CLK.DS0 管脚控制。</td><td>0x1</td></tr>
<tr><td>[3]</td><td>RW</td><td>pad_sfc_clk_ctrl_st</td><td>SFC_CLK.ST 管脚控制：<br>
0: No Schmitt<br>
1: Schmitt Enable。</td><td>0x0</td></tr>
<tr><td>[2:0]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-pad_sfc_csn_ctrl"></a>
### PAD_SFC_CSN_CTRL

PAD_SFC_CSN_CTRL 为 SFC_CSN 控制寄存器。  
Offset Address: 0x86c Total Reset Value: 0x0000_0E10

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:12]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000</td></tr>
<tr><td>[11]</td><td>RW</td><td>pad_sfc_csn_ctrl_ie</td><td>SFC_CSN.IE 管脚控制：<br>
0:输入禁止<br>
1:输入使能。</td><td>0x1</td></tr>
<tr><td>[10]</td><td>RW</td><td>pad_sfc_csn_ctrl_ps</td><td>SFC_CSN.PS 管脚控制:需与SFC_CSN.PE管脚控制结合使用。PE/PS对应上下拉能力控制如下：<br>
00:无上下拉<br>
01:强上拉<br>
10:下拉<br>
11:上拉。</td><td>0x1</td></tr>
<tr><td>[9]</td><td>RW</td><td>pad_sfc_csn_ctrl_pe</td><td>SFC_CSN.PE管脚控制。</td><td>0x1</td></tr>
<tr><td>[8:7]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
<tr><td>[6]</td><td>RW</td><td>pad_sfc_csn_ctrl_ds2</td><td>SFC_CSN.DS2管脚控制:需与SFC_CSN.DS1/SFC_CSN.DS0管脚控制结合使用。DS2~DS0对应驱动能力调节:111~000,驱动能力依次减弱。</td><td>0x0</td></tr>
<tr><td>[5]</td><td>RW</td><td>pad_sfc_csn_ctrl_ds1</td><td>SFC_CSN.DS1管脚控制。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>RW</td><td>pad_sfc_csn_ctrl_ds0</td><td>SFC_CSN.DS0管脚控制。</td><td>0x1</td></tr>
<tr><td>[3]</td><td>RW</td><td>pad_sfc_csn_ctrl_st</td><td>SFC_CSN.ST管脚控制：<br>
0:No Schmitt<br>
1:Schmitt Enable。</td><td>0x0</td></tr>
<tr><td>[2:0]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-pad_sfc_io0_ctrl"></a>
### PAD_SFC_IO0_CTRL

PAD_SFC_IO0_CTRL 为 SFC_IO0 控制寄存器。  
Offset Address: 0x870 Total Reset Value: 0x0000_0E10

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:12]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000</td></tr>
<tr><td>[11]</td><td>RW</td><td>pad_sfc_io0_ctrl_ie</td><td>SFC_IO0.IE 管脚控制：<br>
0:输入禁止<br>
1:输入使能。</td><td>0x1</td></tr>
<tr><td>[10]</td><td>RW</td><td>pad_sfc_io0_ctrl_ps</td><td>SFC_IO0.PS 管脚控制:需与 SFC_IO0.PE 管脚控制结合使用。PE/PS 对应上下拉能力控制如下：<br>
00:无上下拉<br>
01:强上拉<br>
10:下拉<br>
11:上拉。</td><td>0x1</td></tr>
<tr><td>[9]</td><td>RW</td><td>pad_sfc_io0_ctrl_pe</td><td>SFC_IO0.PE 管脚控制。</td><td>0x1</td></tr>
<tr><td>[8:7]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
<tr><td>[6]</td><td>RW</td><td>pad_sfc_io0_ctrl_ds2</td><td>SFC_IO0.DS2 管脚控制:需与SFC_IO0.DS1/SFC_IO0.DS0 管脚控制结合使用。DS2~DS0 对应驱动能力调节:111~000,驱动能力依次减弱。</td><td>0x0</td></tr>
<tr><td>[5]</td><td>RW</td><td>pad_sfc_io0_ctrl_ds1</td><td>SFC_IO0.DS1 管脚控制。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>RW</td><td>pad_sfc_io0_ctrl_ds0</td><td>SFC_IO0.DS0 管脚控制。</td><td>0x1</td></tr>
<tr><td>[3]</td><td>RW</td><td>pad_sfc_io0_ctrl_st</td><td>SFC_IO0.ST 管脚控制：<br>
0: No Schmitt<br>
1: Schmitt Enable。</td><td>0x0</td></tr>
<tr><td>[2:0]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-pad_sfc_io1_ctrl"></a>
### PAD_SFC_IO1_CTRL

PAD_SFC_IO1_CTRL 为 SFC_IO1 控制寄存器。

Offset Address: 0x874 Total Reset Value: 0x0000_0E10

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:12]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000</td></tr>
<tr><td>[11]</td><td>RW</td><td>pad_sfc_io1_ctrl_ie</td><td>SFC_IO1.IE 管脚控制：<br>
0:输入禁止<br>
1:输入使能。</td><td>0x1</td></tr>
<tr><td>[10]</td><td>RW</td><td>pad_sfc_io1_ctrl_ps</td><td>SFC_IO1.PS 管脚控制:需与 SFC_IO1.PE 管脚控制结合使用。PE/PS 对应上下拉能力控制如下：<br>
00:无上下拉<br>
01:强上拉<br>
10:下拉<br>
11:上拉。</td><td>0x1</td></tr>
<tr><td>[9]</td><td>RW</td><td>pad_sfc_io1_ctrl_pe</td><td>SFC_IO1.PE 管脚控制。</td><td>0x1</td></tr>
<tr><td>[8:7]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
<tr><td>[6]</td><td>RW</td><td>pad_sfc_io1_ctrl_ds2</td><td>SFC_IO1.DS2 管脚控制:需与SFC_IO1.DS1/SFC_IO1.DS0 管脚控制结合使用。DS2~DS0 对应驱动能力调节:111~000,驱动能力依次减弱。</td><td>0x0</td></tr>
<tr><td>[5]</td><td>RW</td><td>pad_sfc_io1_ctrl_ds1</td><td>SFC_IO1.DS1 管脚控制。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>RW</td><td>pad_sfc_io1_ctrl_ds0</td><td>SFC_IO1.DS0 管脚控制。</td><td>0x1</td></tr>
<tr><td>[3]</td><td>RW</td><td>pad_sfc_io1_ctrl_st</td><td>SFC_IO1.ST 管脚控制：<br>
0: No Schmitt<br>
1: Schmitt Enable。</td><td>0x0</td></tr>
<tr><td>[2:0]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-pad_sfc_io2_ctrl"></a>
### PAD_SFC_IO2_CTRL

PAD_SFC_IO2_CTRL 为 SFC_IO2 控制寄存器。  
Offset Address: 0x878 Total Reset Value: 0x0000_0E10

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:12]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000</td></tr>
<tr><td>[11]</td><td>RW</td><td>pad_sfc_io2_ctrl_ie</td><td>SFC_IO2.IE 管脚控制：<br>
0:输入禁止<br>
1:输入使能。</td><td>0x1</td></tr>
<tr><td>[10]</td><td>RW</td><td>pad_sfc_io2_ctrl_ps</td><td>SFC_IO2.PS 管脚控制:需与 SFC_IO2.PE 管脚控制结合使用。PE/PS 对应上下拉能力控制如下：<br>
00:无上下拉<br>
01:强上拉<br>
10:下拉<br>
11:上拉。</td><td>0x1</td></tr>
<tr><td>[9]</td><td>RW</td><td>pad_sfc_io2_ctrl_pe</td><td>SFC_IO2.PE 管脚控制。</td><td>0x1</td></tr>
<tr><td>[8:7]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
<tr><td>[6]</td><td>RW</td><td>pad_sfc_io2_ctrl_ds2</td><td>SFC_IO2.DS2 管脚控制:需与SFC_IO2.DS1/SFC_IO2.DS0 管脚控制结合使用。DS2~DS0 对应驱动能力调节:111 至 000,驱动能力依次减弱。</td><td>0x0</td></tr>
<tr><td>[5]</td><td>RW</td><td>pad_sfc_io2_ctrl_ds1</td><td>SFC_IO2.DS1 管脚控制。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>RW</td><td>pad_sfc_io2_ctrl_ds0</td><td>SFC_IO2.DS0 管脚控制。</td><td>0x1</td></tr>
<tr><td>[3]</td><td>RW</td><td>pad_sfc_io2_ctrl_st</td><td>SFC_IO2.ST 管脚控制：<br>
0: No Schmitt<br>
1: Schmitt Enable。</td><td>0x0</td></tr>
<tr><td>[2:0]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.1-pad_sfc_io3_ctrl"></a>
### PAD_SFC_IO3_CTRL

PAD_SFC_IO3_CTRL 为 SFC_IO3 控制寄存器。  
Offset Address: 0x87c Total Reset Value: 0x0000_0E10

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:12]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000</td></tr>
<tr><td>[11]</td><td>RW</td><td>pad_sfc_io3_ctrl_ie</td><td>SFC_IO3.IE 管脚控制：<br>
0:输入禁止<br>
1:输入使能。</td><td>0x1</td></tr>
<tr><td>[10]</td><td>RW</td><td>pad_sfc_io3_ctrl_ps</td><td>SFC_IO3.PS 管脚控制:需与 SFC_IO3.PE 管脚控制结合使用。PE/PS 对应上下拉能力控制如下：<br>
00:无上下拉<br>
01:强上拉<br>
10:下拉<br>
11:上拉。</td><td>0x1</td></tr>
<tr><td>[9]</td><td>RW</td><td>pad_sfc_io3_ctrl_pe</td><td>SFC_IO3.PE 管脚控制。</td><td>0x1</td></tr>
<tr><td>[8:7]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
<tr><td>[6]</td><td>RW</td><td>pad_sfc_io3_ctrl_ds2</td><td>SFC_IO3.DS2 管脚控制:需与SFC_IO3.DS1/SFC_IO3.DS0 管脚控制结合使用。DS2~DS0 对应驱动能力调节:111~000,驱动能力依次减弱。</td><td>0x0</td></tr>
<tr><td>[5]</td><td>RW</td><td>pad_sfc_io3_ctrl_ds1</td><td>SFC_IO3.DS1 管脚控制。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>RW</td><td>pad_sfc_io3_ctrl_ds0</td><td>SFC_IO3.DS0 管脚控制。</td><td>0x1</td></tr>
<tr><td>[3]</td><td>RW</td><td>pad_sfc_io3_ctrl_st</td><td>SFC_IO3.ST 管脚控制：<br>
0: No Schmitt<br>
1: Schmitt Enable。</td><td>0x0</td></tr>
<tr><td>[2:0]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
</tbody>
</table>

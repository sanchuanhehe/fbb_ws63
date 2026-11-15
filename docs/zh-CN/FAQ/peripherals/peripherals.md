# 外设问题

WS63 外设（UART、复位引脚等）使用过程中常见的通信异常问题。

---

## UART 配置错误通信异常

问题描述：【WS63】UART 初始化后，管脚配置是正确的，但 UART 还是无法正常通信。

**解决方案：**

检查UART配置是否正确，重点关注data_bits、stop_bits、parity配置，这3个配置一定要使用hal_uart_data_bit_t、hal_uart_stop_bit_t、hal_uart_parity_t枚举定义的值。

注意：UART_DATA_BIT_8 枚举值是 3，不是 8；UART_STOP_BIT_1 枚举值是 0，不是 1。

初始化时需要注意枚举变量是否设置正确。

uart_line_config.baud_rate = baud_rate;

uart_line_config.data_bits = UART_DATA_BIT_8;

uart_line_config.parity = UART_PARITY_NONE;

uart_line_config.stop_bits = UART_STOP_BIT_1;

---

## reset 引脚电压过高无法重启

问题描述：【WS63】模组通信串口断电后（单独抽拔 VCC），此时再上电接上 VCC，有时会无法正常启动。

**解决方案：**

模组通信串口断电后（拔掉 VCC），测的 power_on 的电压为 1.44V，此时有电流倒灌现象，power_on 复位要求 1.4V 故无法正常重新启动

1、调测过程中，可以用power_on接地复位或者同时使整个串口断电来避免UART口倒灌；

2、可以增大UART口的RX通路的上拉电阻（或者不加RX口的上拉）来减少倒灌的电压，但是要保证对端设备的TX有足够的上拉能力，实测RX口上拉电阻为2.2K时，power_on的

倒灌电压有 1.25V 左右，当上拉电阻为 47K 时，power_on 的倒灌电压为 0.7V 左右，满足 power_on 的下电电压。

---

## UART RX 无法通信

问题描述：【WS63】模组 UART 与主板无法正常通信，发现 TX 能正常发送数据，RX 无法接受数据

**解决方案：**

出现此类问题，第一时间测量RX端异常通信时的波形，发现低电平有1.1V，而UART的低电平识别阈值为-0.3V-0.8V；芯片端低电平识别错误，故无法通信；

1、在驱动能力足够的情况下去除 RX 处的上拉电阻，避免与主板的上拉电阻形成分压，造成芯片端的电压偏大。

2、在无法去除上拉的情况下，增大RX端的上拉电阻的阻值。

---

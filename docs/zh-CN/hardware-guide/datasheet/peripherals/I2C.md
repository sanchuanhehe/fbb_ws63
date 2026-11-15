<a id="6.4"></a>
# I2C

<a id="6.4.1"></a>
## 概述

I2C 模块是 APB 总线上的从设备，是 I2C 总线上的主设备。I2C 模块的作用是完成 CPU 对 I2C 总线上从设备的数据读写，CPU 可以连续配置多个发送的数据和接收多个数据。I2C 总线上可挂载多个从设备，芯片支持 2 个 I2C 模块（I2C0 和 I2C1）。

<a id="6.4.2"></a>
## 功能描述

I2C 具有以下功能特点：

- 2.0 版本的 I2C 总线协议，只支持 Master 模式。

- I2C 模块在 APB 总线上执行 APB Slave 的功能，在 I2C 总线上作为 Master，支持多主设备时的总线仲裁。

- I2C 主机可以向从机写入数据，也可以接收从机发来的数据。

- 支持 Clock synchronization 和 Bit and Byte waiting。

- 支持中断或轮询操作。

- I2C 模块支持标准地址（7bit）和扩展地址（10bit）。

- 可以工作在两种速度模式下：标准模式（100Kbit/s）、快速模式（400Kbit/s）。

- I2C 模块支持 General Call 和 Start Byte 功能。

- I2C 总线上不支持 CBUS 器件。

- 对接收到的 SDA（Serial Data and Address）和 SCL（Serial Clock Line）信号进行滤波。

- 内部包含 1 个 32×8 bit 的发送 FIFO 和 1 个 32×8 bit 的接收 FIFO。

- 支持硬件检测FIFO数据深度并发出相应中断。

- 兼容不使用FIFO和使用FIFO两种工作方式

<a id="6.4.3"></a>
## 工作方式

I2C 包含以下两种工作场景：

- 主机仅对单个数据发送和接收（不使用FIFO）。

- 主机连续发送多个数据、连续接收多个数据（使用FIFO）。

<a id="6.4.3.1"></a>
### 不使用 FIFO

#### I2C 主机发送数据流程

I2C 主机发送数据流程如图 6-2 所示。

图6-2 I2C 主机发送数据（不使用 FIFO）流程图

<div style="text-align:left; margin:10px 0;"><img src="images/图6-2 I2C 主机发送数据（不使用 FIFO）流程图.jpg" alt="" style="max-width:100%; max-height:560px; width:auto; height:auto;"></div>

#### I2C 主机接收数据流程

I2C 主机接收数据流程如图 6-3 所示。

图6-3 I2C 主机接收数据（不使用 FIFO）流程图

<div style="text-align:left; margin:10px 0;"><img src="images/图6-3 I2C 主机接收数据（不使用 FIFO）流程图.jpg" alt="" style="max-width:100%; max-height:560px; width:auto; height:auto;"></div>

<a id="6.4.3.2"></a>
### 使用 FIFO

#### I2C 主机发送数据流程

I2C 主机发送数据流程图如图 6-4 所示。

图6-4 I2C 主机发送数据（使用 FIFO）流程图

<div style="text-align:left; margin:10px 0;"><img src="images/图6-4 I2C 主机发送数据（使用 FIFO）流程图.jpg" alt="" style="max-width:100%; max-height:560px; width:auto; height:auto;"></div>

#### I2C 主机接收数据流程

I2C 主机接收数据流程如图 6-5 所示。

图6-5 I2C 主机接收数据（使用 FIFO）流程图

<div style="text-align:left; margin:10px 0;"><img src="images/图6-5 I2C 主机接收数据（使用 FIFO）流程图.jpg" alt="" style="max-width:100%; width:720px; height:auto;"></div>

<a id="6.4.4"></a>
## 寄存器概览

I2C 寄存器概览如表 6-9 所示。

表6-9 I2C 寄存器概览（基址是 I2C0：0x44018000、I2C1：0x44008100）

<table>
<thead><tr><th>偏移地址</th><th>名称</th><th>描述</th></tr></thead>
<tbody>
<tr><td>0x00</td><td><a href="#reg-6.4-i2c_ctrl">I2C_CTRL</a></td><td>I2C 控制寄存器。</td></tr>
<tr><td>0x04</td><td><a href="#reg-6.4-i2c_com">I2C_COM</a></td><td>I2C 模块的命令寄存器。</td></tr>
<tr><td>0x08</td><td><a href="#reg-6.4-i2c_icr">I2C_ICR</a></td><td>I2C 模块的中断清除寄存器。</td></tr>
<tr><td>0x0C</td><td><a href="#reg-6.4-i2c_sr">I2C_SR</a></td><td>I2C 模块状态寄存器。</td></tr>
<tr><td>0x10</td><td><a href="#reg-6.4-i2c_scl_h">I2C_SCL_H</a></td><td>I2C 总线 SCL 信号高电平周期数寄存器。</td></tr>
<tr><td>0x14</td><td><a href="#reg-6.4-i2c_scl_l">I2C_SCL_L</a></td><td>I2C 总线 SCL 信号低电平周期数寄存器。</td></tr>
<tr><td>0x18</td><td><a href="#reg-6.4-i2c_txr">I2C_TXR</a></td><td>I2C 发送数据寄存器。</td></tr>
<tr><td>0x1C</td><td><a href="#reg-6.4-i2c_rxr">I2C_RXR</a></td><td>I2C 接收数据寄存器。</td></tr>
<tr><td>0x20</td><td><a href="#reg-6.4-i2c_fifostatus">I2C_FIFOSTATUS</a></td><td>FIFO 状态寄存器。</td></tr>
<tr><td>0x24</td><td><a href="#reg-6.4-i2c_txcount">I2C_TXCOUNT</a></td><td>发送 FIFO 数据个数寄存器。</td></tr>
<tr><td>0x28</td><td><a href="#reg-6.4-i2c_rxcount">I2C_RXCOUNT</a></td><td>接收 FIFO 数据个数寄存器。</td></tr>
<tr><td>0x2C</td><td><a href="#reg-6.4-i2c_rxtide">I2C_RXTIDE</a></td><td>接收 FIFO 的溢出阈值寄存器。</td></tr>
<tr><td>0x30</td><td><a href="#reg-6.4-i2c_txtide">I2C_TXTIDE</a></td><td>发送 FIFO 的溢出阈值寄存器。</td></tr>
<tr><td>0x34</td><td><a href="#reg-6.4-i2c_ftrper">I2C_FTRPER</a></td><td>毛刺过滤配置寄存器。</td></tr>
</tbody>
</table>

<a id="6.4.5"></a>
## 寄存器描述

<a id="reg-6.4-i2c_ctrl"></a>
### I2C_CTRL

I2C_CTRL 为 I2C 控制寄存器。

Offset Address: 0x00 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:13]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000</td></tr>
<tr><td>[12]</td><td>RW</td><td>int_txfifo_over_mask</td><td>发送FIFO数据发送完成中断屏蔽。0: 屏蔽：<br>
1: 不屏蔽。</td><td>0x0</td></tr>
<tr><td>[11]</td><td>RW</td><td>mode_ctrl</td><td>I2C工作模式选择。0: 不使用FIFO传输模式：<br>
1: 使用FIFO传输模式。</td><td>0x0</td></tr>
<tr><td>[10]</td><td>RW</td><td>int_txtide_mask</td><td>发送FIFO溢出中断屏蔽。0: 屏蔽：<br>
1: 不屏蔽。</td><td>0x0</td></tr>
<tr><td>[9]</td><td>RW</td><td>int_rxtide_mask</td><td>接收 FIFO 溢出中断屏蔽。0: 屏蔽：<br>
1: 不屏蔽。</td><td>0x0</td></tr>
<tr><td>[8]</td><td>RW</td><td>i2c_en</td><td>I2C 使能。0: 禁止：<br>
1: 使能。</td><td>0x0</td></tr>
<tr><td>[7]</td><td>RW</td><td>int_mask</td><td>I2C 中断总屏蔽。0: 屏蔽：<br>
1: 不屏蔽。</td><td>0x0</td></tr>
<tr><td>[6]</td><td>RW</td><td>int_start_mask</td><td>主机开始条件发送结束中断屏蔽。0: 屏蔽：<br>
1: 不屏蔽。</td><td>0x0</td></tr>
<tr><td>[5]</td><td>RW</td><td>int_stop_mask</td><td>主机停止条件发送结束中断屏蔽。0: 屏蔽：<br>
1: 不屏蔽。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>RW</td><td>int_tx_mask</td><td>主机发送中断屏蔽。0: 屏蔽：<br>
1: 不屏蔽。</td><td>0x0</td></tr>
<tr><td>[3]</td><td>RW</td><td>int_rx_mask</td><td>主机接收中断屏蔽。0: 屏蔽：<br>
1: 不屏蔽。</td><td>0x0</td></tr>
<tr><td>[2]</td><td>RW</td><td>int_ack_err_mask</td><td>从机 ACK 错误中断屏蔽。0: 屏蔽：<br>
1: 不屏蔽。</td><td>0x0</td></tr>
<tr><td>[1]</td><td>RW</td><td>int_arb_loss_mask</td><td>总线仲裁失败中断屏蔽。0: 屏蔽：<br>
1: 不屏蔽。</td><td>0x0</td></tr>
<tr><td>[0]</td><td>RW</td><td>int_done_mask</td><td>总线传输完成中断屏蔽。0: 屏蔽：<br>
1: 不屏蔽。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.4-i2c_com"></a>
### I2C_COM

I2C_COM 为 I2C 模块的命令寄存器。

<CH>在系统初始化时配置或配置前，需要清除对应中断标志。<a href="#reg-6.4-i2c_com">I2C_COM</a> bit[3:0]在操作结后将自动清0。

Offset Address: 0x04 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:5]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000000</td></tr>
<tr><td>[4]</td><td>RW</td><td>op_ack</td><td>主机作为接收器是否发送 ACK。0: 发送：<br>
1: 不发送。</td><td>0x0</td></tr>
<tr><td>[3]</td><td>RW</td><td>op_start</td><td>产生开始条件操作。0: 操作结束：<br>
1: 操作有效。</td><td>0x0</td></tr>
<tr><td>[2]</td><td>RW</td><td>op_rd</td><td>产生读操作。0: 操作结束：<br>
1: 操作有效。</td><td>0x0</td></tr>
<tr><td>[1]</td><td>RW</td><td>op_we</td><td>产生写操作。0: 操作结束：<br>
1: 操作有效。</td><td>0x0</td></tr>
<tr><td>[0]</td><td>RW</td><td>op_stop</td><td>产生停止条件操作。0: 操作结束：<br>
1: 操作有效。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.4-i2c_icr"></a>
### I2C_ICR

I2C_ICR 为 I2C 模块的中断清除寄存器。

**说明:**

新中断到来时，I2C 模块会自动将 <a href="#reg-6.4-i2c_icr">I2C_ICR</a> 相应位清 0。

Offset Address: 0x08 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:10]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x000000</td></tr>
<tr><td>[9]</td><td>WC</td><td>clr_int_txfifo_over</td><td>发送FIFO数据发送完成中断标志清除。0:不清除：<br>
1:清除。</td><td>0x0</td></tr>
<tr><td>[8]</td><td>WC</td><td>clr_int_txtide</td><td>发送FIFO溢出中断标志清除。0:不清除：<br>
1:清除。</td><td>0x0</td></tr>
<tr><td>[7]</td><td>WC</td><td>clr_int_rxtide</td><td>接收FIFO溢出中断标志清除。0:不清除：<br>
1:清除。</td><td>0x0</td></tr>
<tr><td>[6]</td><td>WC</td><td>clr_int_start</td><td>主机开始条件发送结束中断标志清除。0:不清除：<br>
1: 清除。</td><td>0x0</td></tr>
<tr><td>[5]</td><td>WC</td><td>clr_int_stop</td><td>主机停止条件发送结束中断标志清除。0: 不清除：<br>
1: 清除。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>WC</td><td>clr_int_tx</td><td>主机发送中断标志清除。0: 不清除：<br>
1: 清除。</td><td>0x0</td></tr>
<tr><td>[3]</td><td>WC</td><td>clr_int_rx</td><td>主机接收中断标志清除。0: 不清除：<br>
1: 清除。</td><td>0x0</td></tr>
<tr><td>[2]</td><td>WC</td><td>clr_int_ack_err</td><td>从机 ACK 错误中断标志清除。0: 不清除：<br>
1: 清除。</td><td>0x0</td></tr>
<tr><td>[1]</td><td>WC</td><td>clr_int_arb_loss</td><td>总线仲裁失败中断标志清除。0: 不清除：<br>
1: 清除。</td><td>0x0</td></tr>
<tr><td>[0]</td><td>WC</td><td>clr_int_done</td><td>总线传输完成中断标志清除。0: 不清除：<br>
1: 清除。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.4-i2c_sr"></a>
### I2C_SR

I2C_SR 为 I2C 模块状态寄存器。

**说明:**

<a href="#reg-6.4-i2c_sr">I2C_SR</a> bit[1]表示 I2C 总线仲裁失败。当 <a href="#reg-6.4-i2c_sr">I2C_SR</a> bit[1]有效时，当前操作失败。在清 <a href="#reg-6.4-i2c_sr">I2C_SR</a> bit[1]之前，需要清除其他中断标志，然后清除 <a href="#reg-6.4-i2c_com">I2C_COM</a> 或向 <a href="#reg-6.4-i2c_com">I2C_COM</a> 写入新的操作命令，最后清除 <a href="#reg-6.4-i2c_sr">I2C_SR</a> bit[1]。

Offset Address: 0x0C Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:11]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x000000</td></tr>
<tr><td>[10]</td><td>RO</td><td>int_txfifo_over</td><td>发送FIFO数据发送完成中断标志。0:无中断标志产生：<br>
1:中断标志产生。</td><td>0x0</td></tr>
<tr><td>[9]</td><td>RO</td><td>int_txtide</td><td>发送FIFO溢出中断标志。0:无中断标志产生：<br>
1:中断标志产生。</td><td>0x0</td></tr>
<tr><td>[8]</td><td>RO</td><td>int_rxtide</td><td>接收FIFO溢出中断标志。0:无中断标志产生：<br>
1:中断标志产生。</td><td>0x0</td></tr>
<tr><td>[7]</td><td>RO</td><td>bus_busy</td><td>总线忙。0:空闲：<br>
1:忙。</td><td>0x0</td></tr>
<tr><td>[6]</td><td>RO</td><td>int_start</td><td>主机开始条件发送结束中断标志。0:无中断标志产生：<br>
1:中断标志产生。</td><td>0x0</td></tr>
<tr><td>[5]</td><td>RO</td><td>int_stop</td><td>主机停止条件发送结束中断标志。0:无中断标志产生：<br>
1:中断标志产生。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>RO</td><td>int_tx</td><td>主机发送中断标志。0:无中断标志产生：<br>
1: 中断标志产生。</td><td>0x0</td></tr>
<tr><td>[3]</td><td>RO</td><td>int_rx</td><td>主机接收中断标志。0: 无中断标志产生：<br>
1: 中断标志产生。</td><td>0x0</td></tr>
<tr><td>[2]</td><td>RO</td><td>int_ack_err</td><td>从机 ACK 错误中断标志。0: 无中断标志产生：<br>
1: 中断标志产生。</td><td>0x0</td></tr>
<tr><td>[1]</td><td>RO</td><td>int_arb_loss</td><td>总线仲裁失败中断标志。0: 无中断标志产生：<br>
1: 中断标志产生。</td><td>0x0</td></tr>
<tr><td>[0]</td><td>RO</td><td>int_done</td><td>总线传输完成中断标志。0: 无中断标志产生：<br>
1: 中断标志产生。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.4-i2c_scl_h"></a>
### I2C_SCL_H

I2C_SCL_H 为 I2C 总线 SCL 信号高电平周期数寄存器。

**说明:**

在系统初始化时配置或配置前使 <a href="#reg-6.4-i2c_ctrl">I2C_CTRL</a> bit[7]=0。  
Offset Address: 0x10 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:16]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>[15:0]</td><td>RW</td><td>scl_h</td><td>I2C 总线 SCL 信号高电平周期数寄存器。用于配置 I2C 模块工作时 SCL 高电平周期数,配置数值乘 2 等于 SCL 高电平周期数。</td><td>0x0000</td></tr>
</tbody>
</table>

<a id="reg-6.4-i2c_scl_l"></a>
### I2C_SCL_L

I2C_SCL_L 为 I2C 总线 SCL 信号低电平周期数寄存器。

说明

在系统初始化时配置或配置前使 <a href="#reg-6.4-i2c_ctrl">I2C_CTRL</a> bit[7]=0。

Offset Address: 0x14 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:16]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>[15:0]</td><td>RW</td><td>scl_I</td><td>I2C 总线 SCL 信号低电平周期数寄存器。用于配置 I2C 模块工作时SCL 低电平周期数,配置数值乘 2等于 SCL 低电平周期数。</td><td>0x0000</td></tr>
</tbody>
</table>

<a id="reg-6.4-i2c_txr"></a>
### I2C_TXR

I2C_TXR 为 I2C 发送数据寄存器。

**说明:**

不使用 FIFO 模式下，发送结束后，I2C 模块不会修改 <a href="#reg-6.4-i2c_txr">I2C_TXR</a> 内容；使用 FIFO 模式下，写入的数据会自动载入到发送 FIFO 中保存直到该数据发送结束。

Offset Address: 0x18 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:8]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x000000</td></tr>
<tr><td>[7:0]</td><td>RW</td><td>i2c_txr</td><td>主机发送数据。用于配置 I2C 模块工作时发送数据。</td><td>0x00</td></tr>
</tbody>
</table>

<a id="reg-6.4-i2c_rxr"></a>
### I2C_RXR

I2C_RXR 为 I2C 接收数据寄存器。

说明

不使用 FIFO 模式下，<a href="#reg-6.4-i2c_rxr">I2C_RXR</a> 数据在 <a href="#reg-6.4-i2c_sr">I2C_SR</a> bit[3]=1 时，数据有效。同时数据将保持到下一个读操作前。使用 FIFO 模式下，读取 <a href="#reg-6.4-i2c_rxr">I2C_RXR</a> 会直接从接收 FIFO 中取数据。

Offset Address: 0x1C Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:8]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x000000</td></tr>
<tr><td>[7:0]</td><td>RO</td><td>i2c_rxr</td><td>主机接收数据。用于主机接收从机数据。</td><td>0x00</td></tr>
</tbody>
</table>

<a id="reg-6.4-i2c_fifostatus"></a>
### I2C_FIFOSTATUS

I2C_FIFOSTATUS 为 FIFO 状态寄存器。

Offset Address: 0x20 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:4]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000000</td></tr>
<tr><td>[3]</td><td>RO</td><td>rxfe</td><td>接收FIFO空状态。0:非空：<br>
1:空。</td><td>0x0</td></tr>
<tr><td>[2]</td><td>RO</td><td>rxff</td><td>接收FIFO满状态。0:未满：<br>
1:满。</td><td>0x0</td></tr>
<tr><td>[1]</td><td>RO</td><td>txfe</td><td>发送FIFO空状态。0:非空：<br>
1:空。</td><td>0x0</td></tr>
<tr><td>[0]</td><td>RO</td><td>txff</td><td>发送FIFO满状态。0:未满：<br>
1:满。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.4-i2c_txcount"></a>
### I2C_TXCOUNT

I2C_TXCOUNT 为发送 FIFO 数据个数寄存器。

Offset Address: 0x24 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:6]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000000</td></tr>
<tr><td>[5:0]</td><td>WC</td><td>txcount</td><td>读该寄存器返回发送 FIFO 中的字符数,写该寄存器(任意值)将清空发送 FIFO。</td><td>0x00</td></tr>
</tbody>
</table>

<a id="reg-6.4-i2c_rxcount"></a>
### I2C_RXCOUNT

I2C_RXCOUNT 为接收 FIFO 数据个数寄存器。

Offset Address: 0x28 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:6]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000000</td></tr>
<tr><td>[5:0]</td><td>WC</td><td>rxcount</td><td>读该寄存器返回接收 FIFO 中的字符数,写该寄存器(任意值)将清空接收 FIFO。</td><td>0x00</td></tr>
</tbody>
</table>

<a id="reg-6.4-i2c_rxtide"></a>
### I2C_RXTIDE

I2C_RXTIDE 为接收 FIFO 的溢出阈值寄存器。

Offset Address: 0x2C Total Reset Value: 0x0000_0001

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:6]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000000</td></tr>
<tr><td>[5:0]</td><td>RW</td><td>rxtide</td><td>设置 int_rxtide 中断的触发值。RXFIFO 中的字符个数≥<a href="#reg-6.4-i2c_rxtide">I2C_RXTIDE</a>[rxtide]时会触发接收FIFO 溢出中断。</td><td>0x01</td></tr>
</tbody>
</table>

<a id="reg-6.4-i2c_txtide"></a>
### I2C_TXTIDE

I2C_TXTIDE 为发送 FIFO 的溢出阈值寄存器。

<CH>TXFIFO 中的字符只有在成功发送后才会被移除。

Offset Address: 0x30 Total Reset Value: 0x0000_0001

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:6]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000000</td></tr>
<tr><td>[5:0]</td><td>RW</td><td>txtide</td><td>设置 int_txtide 中断的触发值。TXFIFO 中的字符个数≤<a href="#reg-6.4-i2c_txtide">I2C_TXTIDE</a>[txtide]时会触发发送 FIFO 溢出中断。</td><td>0x01</td></tr>
</tbody>
</table>

<a id="reg-6.4-i2c_ftrper"></a>
### I2C_FTRPER

I2C_FTRPER 为毛刺过滤配置寄存器。

Offset Address: 0x34 Total Reset Value: 0x0000_000F

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:4]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000000</td></tr>
<tr><td>[3:0]</td><td>RW</td><td>ftrper</td><td>毛刺过滤配置寄存器。默认 15 个时钟周期,需要 SDA 需要再 SCL为高时,电平保持的时间。<br>scl 为高时,判断 sda 电平持续时间,单位为 ic_clk 的时钟个数,持续时间大于该值才认为该电平为接收值。<br>注意:配置值需要≤((fic_clk/(8*fscl))-2),其中 fic_clk 为 I2C 工作时钟频率,fscl 为 I2C 总线传输速率。</td><td>0xF</td></tr>
</tbody>
</table>

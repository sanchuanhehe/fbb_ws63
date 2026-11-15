<a id="6.10"></a>
# DMA

<a id="6.10.1"></a>
## 概述

直接存储器访问（DMA）方式，是一种完全由硬件执行 I/O 交换的工作方式。在这种方式中，直接存储器访问控制器（DMAC）直接在存储器和外设、外设和外设、存储器和存储器之间进行数据传输，减少处理器的干涉和开销。

DMA（Directory Memory Access）方式一般用于高速传输成组的数据。DMAC（Directory Memory Access Controller）在收到 DMA 传输请求后根据 CPU 对通道的配置启动总线主控制器，向存储器和外设发出地址和控制信号，对传输数据的个数计数，并且以中断方式向 CPU 报告传输操作的结束或错误。

<a id="6.10.2"></a>
## 功能描述

DMA 控制器有如下特点:

- 支持单 MASTER，支持 4 通道，每个通道可配置用于一种单向传输。

- 支持 UART0/UART1/UART2/SPI/QSPI/I2S 硬握手通道，可通过配置设为传输的源端请求或目的端请求。

- 支持四种方向的搬移场景：

- MEMORY 到 MEMORY。

- 外设到外设。

- 外设到 MEMORY。

- MEMORY 到外设。

- DMA 通道优先级固定，优先级从高到低对应的通道号依次为 0~3 。

- DMAC 通道 0 ~ 通道 3 中各包含 1 个 16×32bit 的 FIFO。

- 支持总线位宽为 32bit 的 AHB 总线接口；支持一组 AHB SLAVE 接口和一组 AHB MASTER 接口。

- 外设可使用单次传输（single）和连续传输（burst）2种DMA请求。

- 支持软件控制的 DMA 请求。

- 支持源地址和目的地址可分别配置为自动递增或不递增，递增步长取决于传输位宽。

- 支持分别配置源端和目的端的传输位宽：8/16/32bit。

- 支持链表 DMA 传输。

- 提供1个可屏蔽电平中断输出，中断可清除。

- 支持 DMA 错误和 DMA 传输完成中断屏蔽前后状态查询，及两者的组合中断状态查询。

<a id="6.10.3"></a>
## 工作方式

DMA 初始化配置步骤如下:

步骤 1 读 <a href="#reg-6.10-dmac_en_chns">DMAC_EN_CHNS</a>[en_chns]，获取空闲的通道编号 ch_num，以通道 0 为例。

步骤 2 写 <a href="#reg-6.10-dmac_config">DMAC_CONFIG</a>[dmac_en]为 0x1，使能 DMAC。

步骤 3 写 <a href="#reg-6.10-dmac_chn_control_0">DMAC_CHN_CONTROL_0</a>[dmac_chn_en_0]为 0x0，关闭通道 0 使能。

步骤 4 如果需要通过软请求方式进行 DMA 搬数，则需要根据表 1 中的保留编号配置 <a href="#reg-6.10-dmac_burst_req">DMAC_BURST_REQ</a>[burst_req]、<a href="#reg-6.10-dmac_single_req">DMAC_SINGLE_REQ</a>[single_req]。

说明

如果通过硬请求方式进行 DMA 搬数或传输方向为 MEMORY 到 MEMORY，则忽略此步骤。

表6-14 DMA 请求接口信号描述

<table>
<thead><tr><th>外设编号</th><th>外设端口</th><th>功能描述</th></tr></thead>
<tbody>
<tr><td>0</td><td>reserved</td><td>保留。</td></tr>
<tr><td>1</td><td>uart0_tx</td><td>UART0 的发送信号。</td></tr>
<tr><td>2</td><td>uart0_rx</td><td>UART0 的接收信号。</td></tr>
<tr><td>3</td><td>uart1_tx</td><td>UART1 的发送信号。</td></tr>
<tr><td>4</td><td>uart1_rx</td><td>UART1 的接收信号。</td></tr>
<tr><td>5</td><td>uart2_tx</td><td>UART2 的发送信号。</td></tr>
<tr><td>6</td><td>uart2_rx</td><td>UART2 的接收信号。</td></tr>
<tr><td>7</td><td>spi_tx</td><td>SPI 的发送信号。</td></tr>
<tr><td>8</td><td>spi_rx</td><td>SPI 的接收信号。</td></tr>
<tr><td>9</td><td>qspi_tx</td><td>QSPI 的发送信号。</td></tr>
<tr><td>10</td><td>qspi_rx</td><td>QSPI 的接收信号。</td></tr>
<tr><td>11</td><td>i2s_tx</td><td>I2S 的发送信号。</td></tr>
<tr><td>12</td><td>i2s_rx</td><td>I2S 的接收信号。</td></tr>
<tr><td>13~15</td><td>reserved</td><td>保留。</td></tr>
</tbody>
</table>

步骤 5 配置通道 0 的源地址 <a href="#reg-6.10-dmac_s_addr_0">DMAC_S_ADDR_0</a>[dmac_s_addr_0] 和目的地址 <a href="#reg-6.10-dmac_d_addr_0">DMAC_D_ADDR_0</a>[dmac_d_addr_0]。

步骤 6 根据具体需求配置 DMA 通道 0 控制寄存器 <a href="#reg-6.10-dmac_chn_control_0">DMAC_CHN_CONTROL_0</a>，例如传输位宽、传输长度、Burst 长度等。

步骤 7 如果需要进行链表传输，则配置链式地址 <a href="#reg-6.10-dmac_lli_0">DMAC_LLI_0</a>[dmac_lli_0]。

步骤 8 根据具体需求配置 DMA 通道 0 配置寄存器 <a href="#reg-6.10-dmac_chn_control_0">DMAC_CHN_CONTROL_0</a>:

- 写[dmac_flow_ctl_0]，配置流控和传输类型。

- 写[dmac_d_peripheral_0]，配置目的设备，配置值为表1中的外设编号。

- 写[dmac_s_peripheral_0]，配置源设备，配置值为表1中的外设编号。

- 写[dmac_int_tc_0]，配置完成中断屏蔽位。

- 写[dmac_int_en_0]，配置错误中断屏蔽位。

- 写[dmac_chn_en_0]为 0x1，启动通道 0。

步骤 9 若 <a href="#reg-6.10-dmac_chn_control_0">DMAC_CHN_CONTROL_0</a>[dmac_int_tc_0]未屏蔽，则当 DMA 通道 0 传输完成后上报完成中断，或轮询读取 <a href="#reg-6.10-dmac_ori_int_st">DMAC_ORI_INT_ST</a>[ori_int_trans_st]查询完成状态。

----结束

<a id="6.10.4"></a>
## 寄存器概览

AHB_DMA_RB 寄存器概览如表 6-18 所示。

表6-15 AHB_DMA_RB 寄存器概览（基址是 0x4A000000）

<table>
<thead><tr><th>偏移地址</th><th>名称</th><th>描述</th></tr></thead>
<tbody>
<tr><td>0x0004</td><td><a href="#reg-6.10-dmac_int_st">DMAC_INT_ST</a></td><td>中断状态寄存器。</td></tr>
<tr><td>0x0008</td><td><a href="#reg-6.10-dmac_int_clr">DMAC_INT_CLR</a></td><td>传输中断寄存器。</td></tr>
<tr><td>0x000C</td><td><a href="#reg-6.10-dmac_ori_int_st">DMAC_ORI_INT_ST</a></td><td>原始中断状态寄存器。</td></tr>
<tr><td>0x0010</td><td><a href="#reg-6.10-dmac_en_chns">DMAC_EN_CHNS</a></td><td>通道使能查询寄存器。</td></tr>
<tr><td>0x0014</td><td><a href="#reg-6.10-dmac_burst_req">DMAC_BURST_REQ</a></td><td>BURST 软件配置寄存器。</td></tr>
<tr><td>0x0018</td><td><a href="#reg-6.10-dmac_single_req">DMAC_SINGLE_REQ</a></td><td>SINGLE 软件配置寄存器。</td></tr>
<tr><td>0x001C</td><td><a href="#reg-6.10-dmac_config">DMAC_CONFIG</a></td><td>配置寄存器。</td></tr>
<tr><td>0x0020</td><td><a href="#reg-6.10-dmac_sync">DMAC_SYNC</a></td><td>同步寄存器。</td></tr>
<tr><td>0x0100</td><td><a href="#reg-6.10-dmac_lli_0">DMAC_LLI_0</a></td><td>通道 0 链表寄存器。</td></tr>
<tr><td>0x0120</td><td><a href="#reg-6.10-dmac_lli_1">DMAC_LLI_1</a></td><td>通道 1 链表寄存器。</td></tr>
<tr><td>0x0140</td><td><a href="#reg-6.10-dmac_lli_2">DMAC_LLI_2</a></td><td>通道 2 链表寄存器。</td></tr>
<tr><td>0x0160</td><td><a href="#reg-6.10-dmac_lli_3">DMAC_LLI_3</a></td><td>通道 3 链表寄存器。</td></tr>
<tr><td>0x0110</td><td><a href="#reg-6.10-dmac_s_addr_0">DMAC_S_ADDR_0</a></td><td>通道 0 源地址寄存器。</td></tr>
<tr><td>0x0130</td><td><a href="#reg-6.10-dmac_s_addr_1">DMAC_S_ADDR_1</a></td><td>通道 1 源地址寄存器。</td></tr>
<tr><td>0x0150</td><td><a href="#reg-6.10-dmac_s_addr_2">DMAC_S_ADDR_2</a></td><td>通道 2 源地址寄存器。</td></tr>
<tr><td>0x0170</td><td><a href="#reg-6.10-dmac_s_addr_3">DMAC_S_ADDR_3</a></td><td>通道 3 源地址寄存器。</td></tr>
<tr><td>0x0104</td><td><a href="#reg-6.10-dmac_d_addr_0">DMAC_D_ADDR_0</a></td><td>通道 0 目的地址寄存器。</td></tr>
<tr><td>0x0124</td><td><a href="#reg-6.10-dmac_d_addr_1">DMAC_D_ADDR_1</a></td><td>通道 1 目的地址寄存器。</td></tr>
<tr><td>0x0144</td><td><a href="#reg-6.10-dmac_d_addr_2">DMAC_D_ADDR_2</a></td><td>通道 2 目的地址寄存器。</td></tr>
<tr><td>0x0164</td><td><a href="#reg-6.10-dmac_d_addr_3">DMAC_D_ADDR_3</a></td><td>通道 3 目的地址寄存器。</td></tr>
<tr><td>0x0114</td><td>DMAC_CHN_CONTR OL_0</td><td>通道 0 控制寄存器。</td></tr>
<tr><td>0x0134</td><td>DMAC_CHN_CONTR OL_1</td><td>通道 1 控制寄存器。</td></tr>
<tr><td>0x0154</td><td>DMAC_CHN_CONTR OL_2</td><td>通道 2 控制寄存器。</td></tr>
<tr><td>0x0174</td><td>DMAC_CHN_CONTR OL_3</td><td>通道 3 控制寄存器。</td></tr>
<tr><td>0x0108</td><td><a href="#reg-6.10-dmac_chn_config_0">DMAC_CHN_CONFIG_0</a></td><td>通道 0 配置寄存器。</td></tr>
<tr><td>0x0128</td><td><a href="#reg-6.10-dmac_chn_config_1">DMAC_CHN_CONFIG_1</a></td><td>通道 1 配置寄存器。</td></tr>
<tr><td>0x0148</td><td><a href="#reg-6.10-dmac_chn_config_2">DMAC_CHN_CONFIG_2</a></td><td>通道 2 配置寄存器。</td></tr>
<tr><td>0x0168</td><td><a href="#reg-6.10-dmac_chn_config_3">DMAC_CHN_CONFIG_3</a></td><td>通道 3 配置寄存器。</td></tr>
</tbody>
</table>

<a id="6.10.5"></a>
## 寄存器描述

<a id="reg-6.10-dmac_int_st"></a>
### DMAC_INT_ST

DMAC_INT_ST 为中断状态寄存器。

说明

本 IP 仅支持 4 通道，i=0'3 有效，i=4'7 无效。

Offset Address: 0x0004 Total Reset Value: 0x00000000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:24]</td><td>RO</td><td>reserved</td><td>保留。</td><td>0x00</td></tr>
<tr><td>[23:16]</td><td>RO</td><td>int_err_st</td><td>i=0~7int_err_st[i]=：<br>
• 1'b0:通道i未产生错误中断(经过中断屏蔽);int_err_st[i]=<br>
• 1'b1:通道i产生错误中断(经过中断屏蔽)。</td><td>0x00</td></tr>
<tr><td>[15:8]</td><td>RO</td><td>int_trans_st</td><td>i=0~7int_trans_st[i]=：<br>
• 1'b0:通道i未产生传输中断(经过中断屏蔽);int_trans_st[i]=<br>
• 1'b1:通道i产生传输中断(经过中断屏蔽)。</td><td>0x00</td></tr>
<tr><td>[7:0]</td><td>RO</td><td>int_st</td><td>i=0~7int_st[i]=：<br>
• 1'b0:通道i未产生中断(经过中断屏蔽);int_st[i]=<br>
• 1'b1:通道i产生中断(经过中断屏蔽)。</td><td>0x00</td></tr>
</tbody>
</table>

<a id="reg-6.10-dmac_int_clr"></a>
### DMAC_INT_CLR

DMAC_INT_CLR 为传输中断寄存器。

说明

本 IP 仅支持 4 通道，i=0'3 有效，i=4'7 无效。

Offset Address: 0x0008 Total Reset Value: 0x00000000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:16]</td><td>RO</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>[15:8]</td><td>RW</td><td>int_err_clr</td><td>i=0~7int_err_clr[i]=：<br>
• 1'b0:不清除通道i的错误中断;int_err_clr[i]=<br>
• 1'b1:清除通道i的错误中断。</td><td>0x00</td></tr>
<tr><td>[7:0]</td><td>RW</td><td>int_trans_clr</td><td>i=0~7int_trans_clr[i]=：<br>
• 1'b0:不清除通道i的传输中断;int_trans_clr[i]=<br>
• 1'b1:清除通道i的传输中断。</td><td>0x00</td></tr>
</tbody>
</table>

<a id="reg-6.10-dmac_ori_int_st"></a>
### DMAC_ORI_INT_ST

DMAC_ORI_INT_ST 为原始中断状态寄存器。

说明

本 IP 仅支持 4 通道，i=0'3 有效，i=4'7 无效。

Offset Address: 0x000C Total Reset Value: 0x00000000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>31:16</td><td>RO</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>15:8</td><td>RO</td><td>ori_int_err_st</td><td>i=0~7ori_int_err_st[i]=：<br>
• 1'b0:通道i未产生错误中断(未经中断屏蔽);ori_int_err_st[i]=<br>
• 1'b1:通道i产生错误中断(未经中断屏蔽)。</td><td>0x00</td></tr>
<tr><td>7:0</td><td>RO</td><td>ori_int_trans_st</td><td>i=0~7ori_int_trans_st[i]=：<br>
• 1'b0:通道i未产生传输中断(未经中断屏蔽);ori_int_trans_st[i]=<br>
• 1'b1:通道i产生传输中断(未经中断屏蔽)。</td><td>0x00</td></tr>
</tbody>
</table>

<a id="reg-6.10-dmac_en_chns"></a>
### DMAC_EN_CHNS

DMAC_EN_CHNS 为通道使能查询寄存器。

说明

本 IP 仅支持 4 通道，i=0'3 有效，i=4'7 无效。

Offset Address: 0x0010 Total Reset Value: 0x00000000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>31:8</td><td>RO</td><td>reserved</td><td>保留。</td><td>0x000000</td></tr>
<tr><td>7:0</td><td>RO</td><td>en_chns</td><td>i=0~7en_chns[i]=：<br>
• 1'b0:通道i未使能;en_chns[i]=<br>
• 1'b1:通道i使能。</td><td>0x00</td></tr>
</tbody>
</table>

<a id="reg-6.10-dmac_burst_req"></a>
### DMAC_BURST_REQ

DMAC_BURST_REQ 为 BURST 软件配置寄存器。

Offset Address: 0x0014 Total Reset Value: 0x00000000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>31:16</td><td>RW</td><td>burst_req</td><td>i=0~15burst_req[i]=：<br>
• 1'b0:无影响;burst_req[i]=<br>
• 1'b1:产生DMA burst传输请求,当传输结束时该寄存器中的相应位被清零。</td><td>0x0000</td></tr>
<tr><td>15:0</td><td>RW</td><td>last_burst_req</td><td>i=0~15last_burst_req[i]=：<br>
• 1'b0:无影响;last_burst_req[i]=<br>
• 1'b1:产生DMA last burst传输请求,当传输结束时该寄存器中的相应位被清零。</td><td>0x0000</td></tr>
</tbody>
</table>

<a id="reg-6.10-dmac_single_req"></a>
### DMAC_SINGLE_REQ

DMAC_SINGLE_REQ 为 SINGLE 软件配置寄存器。

Offset Address: 0x0018 Total Reset Value: 0x00000000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>31:16</td><td>RW</td><td>single_req</td><td>i=0~15single_req[i]=：<br>
• 1'b0:无影响single_req[i]=<br>
• 1'b1:产生DMA single传输请求,当传输结束时该寄存器中的相应位被清零</td><td>0x0000</td></tr>
<tr><td>15:0</td><td>RW</td><td>last_single_req</td><td>i=0~15last_single_req[i]=：<br>
• 1'b0:无影响last_single_req[i]=<br>
• 1'b1:产生DMA last single传输请求,当传输结束时该寄存器中的相应位被清零</td><td>0x0000</td></tr>
</tbody>
</table>

<a id="reg-6.10-dmac_config"></a>
### DMAC_CONFIG

DMAC_CONFIG 为配置寄存器。

说明

本 IP 仅支持单 master，master2 配置无效。

Offset Address: 0x001C Total Reset Value: 0x00000000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>31:3</td><td>RO</td><td>reserved</td><td>保留。</td><td>0x00000000</td></tr>
<tr><td>2</td><td>RW</td><td>dmac_m2</td><td>Master 2 endianness 配置位。0: little endian 模式：<br>
1: big endian 模式。</td><td>0x0</td></tr>
<tr><td>1</td><td>RW</td><td>dmac_m1</td><td>Master 1 endianness 配置位。0: little endian 模式：<br>
1: big endian 模式。</td><td>0x0</td></tr>
<tr><td>0</td><td>RW</td><td>dmac_en</td><td>DMAC 使能。0: 禁止 DMAC：<br>
1: 使能 DMAC。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.10-dmac_sync"></a>
### DMAC_SYNC

DMAC_SYNC 为同步寄存器。

Offset Address: 0x0020 Total Reset Value: 0x00000000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>31:16</td><td>RO</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>15:0</td><td>RW</td><td>damc_sync</td><td>控制是否需要对请求线进行同步。0:使能对应外设的DMA请求信号同步逻辑：<br>
1:禁止对应外设的DMA请求信号同步逻辑。</td><td>0x0000</td></tr>
</tbody>
</table>

<a id="reg-6.10-dmac_lli_0"></a>
### DMAC_LLI_0

DMAC_LLI_0 为通道 0 链表寄存器。

说明

本 IP 仅支持单 master，master2 相关配置无效。

Offset Address: 0x0100 Total Reset Value: 0x00000000

表6-16 Table9  
<a href="#reg-6.10-dmac_lli_0">DMAC_LLI_0</a> Register Field Description

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>31:2</td><td>RW</td><td>dmac_lli_0</td><td>链式地址[31:2]</td><td>0x00000000</td></tr>
<tr><td>1</td><td>RO</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
<tr><td>0</td><td>RW</td><td>dmac_lm_0</td><td>用于载入下一个链表结点的Master。0: Master1：<br>
1: Master2。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.10-dmac_lli_1"></a>
### DMAC_LLI_1

DMAC_LLI_为通道 1 链表寄存器。

说明

本 IP 仅支持单 master，master2 相关配置无效。

Offset Address: 0x0120 Total Reset Value: 0x00000000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>31:2</td><td>RW</td><td>dmac_lli_1</td><td>链式地址[31:2]</td><td>0x00000000</td></tr>
<tr><td>1</td><td>RO</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
<tr><td>0</td><td>RW</td><td>dmac_lm_1</td><td>用于载入下一个链表结点的 Master。0:Master1：<br>
1:Master2。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.10-dmac_lli_2"></a>
### DMAC_LLI_2

DMAC_LLI_2 为通道 2 链表寄存器。

说明

本 IP 仅支持单 master，master2 相关配置无效。

Offset Address: 0x0140 Total Reset Value: 0x00000000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>31:2</td><td>RW</td><td>dmac_lli_2</td><td>链式地址[31:2]</td><td>0x00000000</td></tr>
<tr><td>1</td><td>RO</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
<tr><td>0</td><td>RW</td><td>dmac_lm_2</td><td>用于载入下一个链表结点的 Master。0: Master1：<br>
1: Master2。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.10-dmac_lli_3"></a>
### DMAC_LLI_3

DMAC_LLI_3 为通道 3 链表寄存器。

说明

本 IP 仅支持单 master，master2 相关配置无效。

Offset Address: 0x0160 Total Reset Value: 0x00000000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>31:2</td><td>RW</td><td>dmac_lli_3</td><td>链式地址[31:2]</td><td>0x00000000</td></tr>
<tr><td>1</td><td>RO</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
<tr><td>0</td><td>RW</td><td>dmac_lm_3</td><td>用于载入下一个链表结点的 Master。0:Master1：<br>
1:Master2。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.10-dmac_s_addr_0"></a>
### DMAC_S_ADDR_0

DMAC_S_ADDR_0 为通道 0 源地址寄存器。

Offset Address: 0x0110 Total Reset Value: 0x00000000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>31:0</td><td>RW</td><td>dmac_s_addr_0</td><td>DMAC 通道 0 源地址寄存器</td><td>0x00000000</td></tr>
</tbody>
</table>

<a id="reg-6.10-dmac_s_addr_1"></a>
### DMAC_S_ADDR_1

DMAC_S_ADDR_1 为通道 1 源地址寄存器。

Offset Address: 0x0130 Total Reset Value: 0x00000000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>31:0</td><td>RW</td><td>dmac_s_addr_1</td><td>DMAC 通道 1 源地址寄存器</td><td>0x00000000</td></tr>
</tbody>
</table>

<a id="reg-6.10-dmac_s_addr_2"></a>
### DMAC_S_ADDR_2

DMAC_S_ADDR_2 为通道 2 源地址寄存器。

Offset Address: 0x0150 Total Reset Value: 0x00000000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>31:0</td><td>RW</td><td>dmac_s_addr_2</td><td>DMAC 通道 2 源地址寄存器</td><td>0x00000000</td></tr>
</tbody>
</table>

<a id="reg-6.10-dmac_s_addr_3"></a>
### DMAC_S_ADDR_3

DMAC_S_ADDR_3 为通道 3 源地址寄存器。

Offset Address: 0x0170 Total Reset Value: 0x00000000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>31:0</td><td>RW</td><td>dmac_s_addr_3</td><td>DMAC 通道 3 源地址寄存器</td><td>0x00000000</td></tr>
</tbody>
</table>

<a id="reg-6.10-dmac_d_addr_0"></a>
### DMAC_D_ADDR_0

DMAC_D_ADDR_0 为通道 0 目的地址寄存器。

Offset Address: 0x0104 Total Reset Value: 0x00000000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>31:0</td><td>RW</td><td>dmac_d_addr_0</td><td>DMAC 通道 0 目的地址寄存器</td><td>0x00000000</td></tr>
</tbody>
</table>

<a id="reg-6.10-dmac_d_addr_1"></a>
### DMAC_D_ADDR_1

DMAC_D_ADDR_1 为通道 1 目的地址寄存器。

Offset Address: 0x0124 Total Reset Value: 0x00000000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>31:0</td><td>RW</td><td>dmac_d_addr_1</td><td>DMAC 通道 1 目的地址寄存器</td><td>0x00000000</td></tr>
</tbody>
</table>

<a id="reg-6.10-dmac_d_addr_2"></a>
### DMAC_D_ADDR_2

DMAC_D_ADDR_2 为通道 2 目的地址寄存器。

Offset Address: 0x0144 Total Reset Value: 0x00000000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>31:0</td><td>RW</td><td>dmac_d_addr_2</td><td>DMAC 通道 2 目的地址寄存器。</td><td>0x00000000</td></tr>
</tbody>
</table>

<a id="reg-6.10-dmac_d_addr_3"></a>
### DMAC_D_ADDR_3

DMAC_D_ADDR_3 为通道 3 目的地址寄存器。

Offset Address: 0x0164 Total Reset Value: 0x00000000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>31:0</td><td>RW</td><td>dmac_d_addr_3</td><td>DMAC 通道 3 目的地址寄存器。</td><td>0x00000000</td></tr>
</tbody>
</table>

<a id="reg-6.10-dmac_chn_control_0"></a>
### DMAC_CHN_CONTROL_0

DMAC_CHN_CONTROL_0 为通道 0 控制寄存器。

说明

本 IP 仅支持单 master，master2 相关配置无效。

Offset Address: 0x0114 Total Reset Value: 0x00000000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>31</td><td>RW</td><td>dmac_trans_int_0</td><td>传输完成中断使能位。该位用于决定当前链表结点是否触发传输完成中断。0:当前链表结点不触发传输完成中断：<br>
1:当前链表结点触发传输完成中断。</td><td>0x0</td></tr>
<tr><td>30:28</td><td>RW</td><td>dmac_prot_0</td><td>master 发出的访问保护HPROT[2:0]信号。</td><td>0x0</td></tr>
<tr><td>27</td><td>RW</td><td>dmac_d_inc_0</td><td>目的地址递增。0:目的地址不递增：<br>
1:目的地址每传一个数就递增一次。目的设备为外设时目的地址不递增<br>
目的设备为存储器时目的地址递增。</td><td>0x0</td></tr>
<tr><td>26</td><td>RW</td><td>dmac_s_inc_0</td><td>源地址递增。0:源地址不递增：<br>
1:源地址每传一个数就递增一次。源设备为外设时源地址不递增<br>
源设备为存储器时源地址递增。</td><td>0x0</td></tr>
<tr><td>25</td><td>RW</td><td>dmac_d_master_0</td><td>设置访问目的设备的 master。0:使用 Master1 作为目的设备传输：<br>
1: 使用 Master2 作为目的设备传输。</td><td>0x0</td></tr>
<tr><td>24</td><td>RW</td><td>dmac_s_master_0</td><td>设置访问源设备的 master。0: 使用 Master1 作为源设备传输：<br>
1: 使用 Master2 作为源设备传输。</td><td>0x0</td></tr>
<tr><td>23:21</td><td>RW</td><td>dmac_d_width_0</td><td>目的设备传输位宽。000: Byte (8bit)001: Halfword (16bit)010: Word (32bit)</td><td>0x0</td></tr>
<tr><td>20:18</td><td>RW</td><td>dmac_s_width_0</td><td>源设备传输位宽。000: Byte (8bit)001: Halfword (16bit)010: Word (32bit)</td><td>0x0</td></tr>
<tr><td>17:15</td><td>RW</td><td>dmac_d_bsize_0</td><td>目的设备 burst 长度。000: 1001: 4010: 8011: 16100: 32101: 64110: 128111: 256</td><td>0x0</td></tr>
<tr><td>14:12</td><td>RW</td><td>dmac_s_bsize_0</td><td>源设备 burst 长度。000: 1001: 4010: 8011: 16100: 32101: 64110: 128111: 256</td><td>0x0</td></tr>
<tr><td>11:0</td><td>RW</td><td>dmac_trans_size_0</td><td>当DMAC是流控制器时,通过写该寄存器可设定DMA传输的长度。</td><td>0x000</td></tr>
</tbody>
</table>

<a id="reg-6.10-dmac_chn_control_1"></a>
### DMAC_CHN_CONTROL_1

DMAC_CHN_CONTROL_1 为通道 1 控制寄存器。

说明

本 IP 仅支持单 master，master2 相关配置无效。

Offset Address: 0x0134 Total Reset Value: 0x00000000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>31</td><td>RW</td><td>dmac_trans_int_1</td><td>传输完成中断使能位。该位用于决定当前链表结点是否触发传输完成中断。0:当前链表结点不触发传输完成中断：<br>
1:当前链表结点触发传输完成中断。</td><td>0x0</td></tr>
<tr><td>30:28</td><td>RW</td><td>dmac_prot_1</td><td>master 发出的访问保护 HPROT[2:0]信号。</td><td>0x0</td></tr>
<tr><td>27</td><td>RW</td><td>dmac_d_inc_1</td><td>目的地址递增。0:目的地址不递增：<br>
1:目的地址每传一个数就递增一次。目的设备为外设时目的地址不递增<br>
目的设备为存储器时目的地址递增。</td><td>0x0</td></tr>
<tr><td>26</td><td>RW</td><td>dmac_s_inc_1</td><td>源地址递增。0:源地址不递增：<br>
1:源地址每传一个数就递增一次。源设备为外设时源地址不递增<br>
源设备为存储器时源地址递增。</td><td>0x0</td></tr>
<tr><td>25</td><td>RW</td><td>dmac_d_master_1</td><td>设置访问目的设备的 master。0:使用 Master1 作为目的设备传输：<br>
1:使用 Master2 作为目的设备传输。</td><td>0x0</td></tr>
<tr><td>24</td><td>RW</td><td>dmac_s_master_1</td><td>设置访问源设备的 master。0:使用 Master1 作为源设备传输：<br>
1:使用 Master2 作为源设备传输。</td><td>0x0</td></tr>
<tr><td>23:21</td><td>RW</td><td>dmac_d_width_1</td><td>目的设备传输位宽。000:Byte (8bit)001:Halfword (16bit)010:Word (32bit)</td><td>0x0</td></tr>
<tr><td>20:18</td><td>RW</td><td>dmac_s_width_1</td><td>源设备传输位宽。000:Byte (8bit)001:Halfword (16bit)010:Word (32bit)</td><td>0x0</td></tr>
<tr><td>17:15</td><td>RW</td><td>dmac_d_bsize_1</td><td>目的设备 burst 长度。000:1001: 4010: 8011: 16100: 32101: 64110: 128111: 256</td><td>0x0</td></tr>
<tr><td>14:12</td><td>RW</td><td>dmac_s_bsize_1</td><td>源设备 burst 长度。000: 1001: 4010: 8011: 16100: 32101: 64110: 128111: 256</td><td>0x0</td></tr>
<tr><td>11:0</td><td>RW</td><td>dmac_trans_size_1</td><td>当 DMAC 是流控制器时,通过写该寄存器可设定 DMA 传输的长度。</td><td>0x000</td></tr>
</tbody>
</table>

<a id="reg-6.10-dmac_chn_control_2"></a>
### DMAC_CHN_CONTROL_2

DMAC_CHN_CONTROL_2 为通道 2 控制寄存器。

说明

本 IP 仅支持单 master，master2 相关配置无效。

Offset Address: 0x0154 Total Reset Value: 0x00000000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>31</td><td>RW</td><td>dmac_trans_int_2</td><td>传输完成中断使能位。该位用于决定当前链表结点是否触发传输完成中断。0:当前链表结点不触发传输完成中断：<br>
1:当前链表结点触发传输完成中断。</td><td>0x0</td></tr>
<tr><td>30:28</td><td>RW</td><td>dmac_prot_2</td><td>master 发出的访问保护 HPROT[2:0]信号。</td><td>0x0</td></tr>
<tr><td>27</td><td>RW</td><td>dmac_d_inc_2</td><td>目的地址递增。0:目的地址不递增：<br>
1:目的地址每传一个数就递增一次。目的设备为外设时目的地址不递增<br>
目的设备为存储器时目的地址递增。</td><td>0x0</td></tr>
<tr><td>26</td><td>RW</td><td>dmac_s_inc_2</td><td>源地址递增。0:源地址不递增：<br>
1:源地址每传一个数就递增一次。源设备为外设时源地址不递增<br>
源设备为存储器时源地址递增。</td><td>0x0</td></tr>
<tr><td>25</td><td>RW</td><td>dmac_d_master_2</td><td>设置访问目的设备的 master。0:使用 Master1 作为目的设备传输：<br>
1:使用 Master2 作为目的设备传输。</td><td>0x0</td></tr>
<tr><td>24</td><td>RW</td><td>dmac_s_master_2</td><td>设置访问源设备的 master。0:使用 Master1 作为源设备传输：<br>
1:使用 Master2 作为源设备传输。</td><td>0x0</td></tr>
<tr><td>23:21</td><td>RW</td><td>dmac_d_width_2</td><td>目的设备传输位宽。000: Byte (8bit)001: Halfword (16bit)010: Word (32bit)</td><td>0x0</td></tr>
<tr><td>20:18</td><td>RW</td><td>dmac_s_width_2</td><td>源设备传输位宽。000: Byte (8bit)001: Halfword (16bit)010: Word (32bit)</td><td>0x0</td></tr>
<tr><td>17:15</td><td>RW</td><td>dmac_d_bsize_2</td><td>目的设备 burst 长度。000: 1001: 4010: 8011: 16100: 32101: 64110: 128111: 256</td><td>0x0</td></tr>
<tr><td>14:12</td><td>RW</td><td>dmac_s_bsize_2</td><td>源设备 burst 长度。000: 1001: 4010: 8011: 16100: 32101: 64110: 128111: 256</td><td>0x0</td></tr>
<tr><td>11:0</td><td>RW</td><td>dmac_trans_size_2</td><td>当 DMAC 是流控制器时,通过写该寄存器可设定 DMA 传输的长度。</td><td>0x000</td></tr>
</tbody>
</table>

<a id="reg-6.10-dmac_chn_control_3"></a>
### DMAC_CHN_CONTROL_3

DMAC_CHN_CONTROL_3 为通道 3 控制寄存器。

**说明:**

本 IP 仅支持单 master，master2 相关配置无效。

Offset Address: 0x0174 Total Reset Value: 0x00000000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>31</td><td>RW</td><td>dmac_trans_int_3</td><td>传输完成中断使能位。该位用于决定当前链表结点是否触发传输完成中断。0:当前链表结点不触发传输完成中断：<br>
1:当前链表结点触发传输完成中断。</td><td>0x0</td></tr>
<tr><td>30:28</td><td>RW</td><td>dmac_prot_3</td><td>master 发出的访问保护HPROT[2:0]信号。</td><td>0x0</td></tr>
<tr><td>27</td><td>RW</td><td>dmac_d_inc_3</td><td>目的地址递增。0:目的地址不递增：<br>
1:目的地址每传一个数就递增一次。目的设备为外设时目的地址不递增<br>
目的设备为存储器时目的地址递增。</td><td>0x0</td></tr>
<tr><td>26</td><td>RW</td><td>dmac_s_inc_3</td><td>源地址递增。0:源地址不递增：<br>
1:源地址每传一个数就递增一次。源设备为外设时源地址不递增<br>
源设备为存储器时源地址递增。</td><td>0x0</td></tr>
<tr><td>25</td><td>RW</td><td>dmac_d_master_3</td><td>设置访问目的设备的 master。0: 使用 Master1 作为目的设备传输：<br>
1: 使用 Master2 作为目的设备传输。</td><td>0x0</td></tr>
<tr><td>24</td><td>RW</td><td>dmac_s_master_3</td><td>设置访问源设备的 master。0: 使用 Master1 作为源设备传输：<br>
1: 使用 Master2 作为源设备传输。</td><td>0x0</td></tr>
<tr><td>23:21</td><td>RW</td><td>dmac_d_width_3</td><td>目的设备传输位宽。000: Byte (8bit)001: Halfword (16bit)010: Word (32bit)</td><td>0x0</td></tr>
<tr><td>20:18</td><td>RW</td><td>dmac_s_width_3</td><td>源设备传输位宽。000: Byte (8bit)001: Halfword (16bit)010: Word (32bit)</td><td>0x0</td></tr>
<tr><td>17:15</td><td>RW</td><td>dmac_d_bsize_3</td><td>目的设备 burst 长度。000: 1001: 4010: 8011: 16100: 32101: 64110: 128111: 256</td><td>0x0</td></tr>
<tr><td>14:12</td><td>RW</td><td>dmac_s_bsize_3</td><td>源设备 burst 长度000: 1001: 4010: 8011: 16100: 32101: 64110: 128111: 256</td><td>0x0</td></tr>
<tr><td>11:0</td><td>RW</td><td>dmac_trans_size_3</td><td>当 DMAC 是流控制器时,通过写该寄存器可设定 DMA 传输的长度。</td><td>0x000</td></tr>
</tbody>
</table>

<a id="reg-6.10-dmac_chn_config_0"></a>
### DMAC_CHN_CONFIG_0

DMAC_CHN_CONFIG_0 为通道 0 配置寄存器。

Offset Address: 0x0108 Total Reset Value: 0x00000000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>31:17</td><td>RO</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>16</td><td>RW</td><td>dmac_halt_0</td><td>Halt位。0:允许DMA请求：<br>
1:忽略后来的DMA请求,通道FIFO中的内容都被传完。</td><td>0x0</td></tr>
<tr><td>15</td><td>RO</td><td>dmac_active_0</td><td>Active位。0:通道FIFO中没有数据：<br>
1:通道FIFO中有数据。</td><td>0x0</td></tr>
<tr><td>14</td><td>RW</td><td>dmac_lock_0</td><td>Lock位。0:禁止总线上 lock 传输：<br>
1:使能总线上 lock 传输。</td><td>0x0</td></tr>
<tr><td>13</td><td>RW</td><td>dmac_int_tc_0</td><td>传输完成中断屏蔽位。0:屏蔽本通道的传输完成中断：<br>
1:不屏蔽本通道的传输完成中断。</td><td>0x0</td></tr>
<tr><td>12</td><td>RW</td><td>dmac_int_en_0</td><td>错误中断屏蔽位。0:屏蔽本通道的错误中断：<br>
1:不屏蔽本通道的错误中断。</td><td>0x0</td></tr>
<tr><td>11:9</td><td>RW</td><td>dmac_flow_ctl_0</td><td>流控及传输类型字段。000:存储器至存储器DMAC：<br>
001:存储器至外设 DMAC<br>
010:外设至存储器 DMAC<br>
011:源设备至目的设备DMAC<br>
100:源设备至目的设备目的设备<br>
101:存储器至外设目的设备<br>
110:外设至存储器源设备<br>
111:源设备至目的设备源设备。</td><td>0x0</td></tr>
<tr><td>8:5</td><td>RW</td><td>dmac_d_periphe ral_0</td><td>目的设备。该字段用于选择一个外设请求信号作为本通道的DMA 目的设备的请求信号。如果 DMA 传输的目的设备是存储器则该字段被忽略。</td><td>0x0</td></tr>
<tr><td>4:1</td><td>RW</td><td>dmac_s_periphe ral_0</td><td>源设备。该字段用于选择一个外设请求信号作为本通道的DMA源设备的请求信号。如果DMA传输的源设备是存储器则该字段被忽略。</td><td>0x0</td></tr>
<tr><td>0</td><td>RW</td><td>dmac_chn_en_0</td><td>通道使能位。0:关闭通道：<br>
1:启动通道。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.10-dmac_chn_config_1"></a>
### DMAC_CHN_CONFIG_1

DMAC_CHN_CONFIG_1 为通道 1 配置寄存器。

Offset Address: 0x0128 Total Reset Value: 0x00000000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>31:17</td><td>RO</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>16</td><td>RW</td><td>dmac_halt_1</td><td>Halt 位。0: 允许 DMA 请求：<br>
1: 忽略后来的 DMA 请求,通道 FIFO 中的内容都被传完。</td><td>0x0</td></tr>
<tr><td>15</td><td>RO</td><td>dmac_active_1</td><td>Active 位。0: 通道 FIFO 中没有数据：<br>
1: 通道 FIFO 中有数据。</td><td>0x0</td></tr>
<tr><td>14</td><td>RW</td><td>dmac_lock_1</td><td>Lock 位。0: 禁止总线上 lock 传输：<br>
1: 使能总线上 lock 传输。</td><td>0x0</td></tr>
<tr><td>13</td><td>RW</td><td>dmac_int_tc_1</td><td>传输完成中断屏蔽位。0: 屏蔽本通道的传输完成中断：<br>
1:不屏蔽本通道的传输完成中断。</td><td>0x0</td></tr>
<tr><td>12</td><td>RW</td><td>dmac_int_en_1</td><td>错误中断屏蔽位。0:屏蔽本通道的错误中断：<br>
1:不屏蔽本通道的错误中断。</td><td>0x0</td></tr>
<tr><td>11:9</td><td>RW</td><td>dmac_flow_ctl_1</td><td>流控及传输类型字段。000:存储器至存储器DMAC：<br>
001:存储器至外设DMAC<br>
010:外设至存储器DMAC<br>
011:源设备至目的设备DMAC<br>
100:源设备至目的设备目的设备<br>
101:存储器至外设目的设备<br>
110:外设至存储器源设备<br>
111:源设备至目的设备源设备。</td><td>0x0</td></tr>
<tr><td>8:5</td><td>RW</td><td>dmac_d_peripheral_1</td><td>目的设备。该字段用于选择一个外设请求信号作为本通道的DMA目的设备的请求信号。如果DMA传输的目的设备是存储器则该字段被忽略。</td><td>0x0</td></tr>
<tr><td>4:1</td><td>RW</td><td>dmac_s_peripheral_1</td><td>源设备。该字段用于选择一个外设请求信号作为本通道的 DMA 源设备的请求信号。如果 DMA 传输的源设备是存储器则该字段被忽略。</td><td>0x0</td></tr>
<tr><td>0</td><td>RW</td><td>dmac_chn_en_1</td><td>通道使能位0:关闭通道：<br>
1:启动通道。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.10-dmac_chn_config_2"></a>
### DMAC_CHN_CONFIG_2

DMAC_CHN_CONFIG_2 为通道 2 配置寄存器。

Offset Address: 0x0148 Total Reset Value: 0x00000000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>31:17</td><td>RO</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>16</td><td>RW</td><td>dmac_halt_2</td><td>Halt位。0:允许DMA请求：<br>
1:忽略后来的DMA请求,通道FIFO中的内容都被传完。</td><td>0x0</td></tr>
<tr><td>15</td><td>RO</td><td>dmac_active_2</td><td>Active位。0:通道FIFO中没有数据：<br>
1:通道FIFO中有数据。</td><td>0x0</td></tr>
<tr><td>14</td><td>RW</td><td>dmac_lock_2</td><td>Lock位。0:禁止总线上lock传输：<br>
1:使能总线上lock传输。</td><td>0x0</td></tr>
<tr><td>13</td><td>RW</td><td>dmac_int_tc_2</td><td>传输完成中断屏蔽位。0:屏蔽本通道的传输完成中断：<br>
1: 不屏蔽本通道的传输完成中断。</td><td>0x0</td></tr>
<tr><td>12</td><td>RW</td><td>dmac_int_en_2</td><td>错误中断屏蔽位。0: 屏蔽本通道的错误中断：<br>
1: 不屏蔽本通道的错误中断。</td><td>0x0</td></tr>
<tr><td>11:9</td><td>RW</td><td>dmac_flow_ctl_2</td><td>流控及传输类型字段000: 存储器至存储器DMAC：<br>
001: 存储器至外设DMAC<br>
010: 外设至存储器DMAC<br>
011: 源设备至目的设备DMAC<br>
100: 源设备至目的设备目的设备<br>
101: 存储器至外设目的设备<br>
110: 外设至存储器源设备<br>
111: 源设备至目的设备源设备。</td><td>0x0</td></tr>
<tr><td>8:5</td><td>RW</td><td>dmac_d_peripheral_2</td><td>目的设备。该字段用于选择一个外设请求信号作为本通道的DMA目的设备的请求信号。如果DMA传输的目的设备是存储器则该字段被忽略。</td><td>0x0</td></tr>
<tr><td>4:1</td><td>RW</td><td>dmac_s_peripheral_2</td><td>源设备。该字段用于选择一个外设请求信号作为本通道的 DMA 源设备的请求信号。如果 DMA 传输的源设备是存储器则该字段被忽略。</td><td>0x0</td></tr>
<tr><td>0</td><td>RW</td><td>dmac_chn_en_2</td><td>通道使能位0:关闭通道：<br>
1:启动通道。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.10-dmac_chn_config_3"></a>
### DMAC_CHN_CONFIG_3

DMAC_CHN_CONFIG_3 为通道 3 配置寄存器。

Offset Address: 0x0168 Total Reset Value: 0x00000000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>31:17</td><td>RO</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>16</td><td>RW</td><td>dmac_halt_3</td><td>Halt位。0:允许DMA请求：<br>
1:忽略后来的DMA请求,通道FIFO中的内容都被传完。</td><td>0x0</td></tr>
<tr><td>15</td><td>RO</td><td>dmac_active_3</td><td>Active位。0:通道FIFO中没有数据：<br>
1:通道FIFO中有数据。</td><td>0x0</td></tr>
<tr><td>14</td><td>RW</td><td>dmac_lock_3</td><td>Lock位。0:禁止总线上lock传输：<br>
1:使能总线上lock传输。</td><td>0x0</td></tr>
<tr><td>13</td><td>RW</td><td>dmac_int_tc_3</td><td>传输完成中断屏蔽位。0:屏蔽本通道的传输完成中断：<br>
1:不屏蔽本通道的传输完成中断。</td><td>0x0</td></tr>
<tr><td>12</td><td>RW</td><td>dmac_int_en_3</td><td>错误中断屏蔽位。0:屏蔽本通道的错误中断：<br>
1:不屏蔽本通道的错误中断。</td><td>0x0</td></tr>
<tr><td>11:9</td><td>RW</td><td>dmac_flow_ctl_3</td><td>流控及传输类型字段000:存储器至存储器DMAC：<br>
001:存储器至外设DMAC<br>
010:外设至存储器DMAC<br>
011:源设备至目的设备DMAC<br>
100:源设备至目的设备目的设备<br>
101:存储器至外设目的设备<br>
110:外设至存储器源设备<br>
111:源设备至目的设备源设备。</td><td>0x0</td></tr>
<tr><td>8:5</td><td>RW</td><td>dmac_d_periphe ral_3</td><td>目的设备。该字段用于选择一个外设请求信号作为本通道的DMA目的设备的请求信号。如果DMA传输的目的设备是存储器则该字段被忽略。</td><td>0x0</td></tr>
<tr><td>4:1</td><td>RW</td><td>dmac_s_periphe ral_3</td><td>源设备。该字段用于选择一个外设请求信号作为本通道的DMA源设备的请求信号。如果DMA传输的源设备是存储器则该字段被忽略。</td><td>0x0</td></tr>
<tr><td>0</td><td>RW</td><td>dmac_chn_en_3</td><td>通道使能位。0:关闭通道：<br>
1:启动通道。</td><td>0x0</td></tr>
</tbody>
</table>

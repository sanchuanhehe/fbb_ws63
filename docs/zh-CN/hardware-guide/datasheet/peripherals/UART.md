<a id="6.3"></a>
# UART

<a id="6.3.1"></a>
## 概述

通用异步收发器 UART（universal asynchronous receiver/transmitter）是一个异步串行的通信接口，UART 主要用于和外部芯片的 UART 进行对接，实现两芯片间的通信。

芯片提供 3 个 UART 单元（UART0/1/2），UART0 仅支持两线模式，UART1/2 支持流控功能。

<a id="6.3.2"></a>
## 功能描述

UART 具有以下功能特点:

- 支持 64x8bit 的发送 FIFO 和 64x10bit 的接收 FIFO (First In First Out)。

- 支持可编程数据位宽 5/6/7/8 bit。

- 支持可编程停止位宽 1/1.5/2 bit。

- 支持奇/偶校验或者无校验位，支持校验位为软件设定值。

- 支持波特率可编程（整数分频和小数分频，小数分频的参数 DLF_SIZE 为 6）。

- 支持接收FIFO中断、发送FIFO中断、接收超时中断、错误中断。

- 支持中断状态查询。

- UART0 不支持硬件流控，UART1 和 UART2 支持硬件流控。

- 支持三种数据搬运方式：

- 支持 DMA 方式。

- 支持中断方式。

- 支持软件查询方式。

<a id="6.3.3"></a>
## 工作方式

### 接口信号

UART 接口信号描述如表 UART 接口信号描述所示。

表6-6 UART 接口信号描述

<table>
<thead><tr><th>信号名</th><th>宽度(bit)</th><th>方向</th><th>功能描述</th></tr></thead>
<tbody>
<tr><td>RXD</td><td>1</td><td>I</td><td>输入数据。</td></tr>
<tr><td>TXD</td><td>1</td><td>O</td><td>输出数据。</td></tr>
<tr><td>CTS</td><td>1</td><td>I</td><td>清除发送信号,用于硬件流控,低有效。</td></tr>
<tr><td>RTS</td><td>1</td><td>O</td><td>请求发送信号,用于硬件流控,低有效。</td></tr>
</tbody>
</table>

### UART 数据格式

<hr style="border: none; border-top: 3px solid #555; margin: 16px 0;">

整数波特率寄存器和小数波特率寄存器的值必须等到当前数据发送和接收完毕才更新。

<hr style="border: none; border-top: 3px solid #555; margin: 16px 0;">

UART 的数据帧格式如图 6-1 所示。其中数据帧长度、停止位位数和奇偶检验可配置。

图6-1 UART 数据帧格式

<div style="text-align:left; margin:10px 0;"><img src="images/图6-1 UART 数据帧格式.jpg" alt="" style="max-width:100%; width:720px; height:auto;"></div>

### 中断或查询方式下的数据传输

初始化步骤如下：

步骤 1 配置 IO 复用关系，IO 暂不复用为 UART，防止初始化过程中 UART 被对端影响。

步骤 2 配置 <a href="#reg-6.3-halt_tx">HALT_TX</a>[halt_tx]=1，停止 TX 功能，防止初始化过程中触发 TX。

步骤 3 配置 <a href="#reg-6.3-intr_en">INTR_EN</a>=0，屏蔽 UART 所有中断。

步骤 4 向 <a href="#reg-6.3-fifo_ctl">FIFO_CTL</a>[rx_fifo_rst] 和 <a href="#reg-6.3-fifo_ctl">FIFO_CTL</a>[tx_fifo_rst] 写 1，分别复位 RX FIFO 和 TX FIFO。

步骤 5 配置 <a href="#reg-6.3-baud_ctl">BAUD_CTL</a>[baud_div]，根据需要配置波特率过采样倍数。

步骤 6 配置 <a href="#reg-6.3-uart_ctl">UART_CTL</a>[div_en]=1，准备开始配置波特率。

步骤 7 配置 <a href="#reg-6.3-div_h">DIV_H</a>、<a href="#reg-6.3-div_l">DIV_L</a>、<a href="#reg-6.3-div_fra">DIV_FRA</a>，确定波特率。配置值计算方法为：分频值=工作时钟频率/（波特率×波特率过采样倍数），{<a href="#reg-6.3-div_h">DIV_H</a>, <a href="#reg-6.3-div_l">DIV_L</a>}组成分频值的整数部分，<a href="#reg-6.3-div_fra">DIV_FRA</a> = 分频率的小数部分×64。

步骤 8 配置 <a href="#reg-6.3-uart_ctl">UART_CTL</a>[stp]、<a href="#reg-6.3-uart_ctl">UART_CTL</a>[pen]、<a href="#reg-6.3-uart_ctl">UART_CTL</a>[eps]、<a href="#reg-6.3-uart_ctl">UART_CTL</a>[dlen]，根据需要配置帧格式，以上寄存器分别控制停止位位宽，奇偶校验使能，奇偶校验选择和数据位长度。

步骤 9 配置 <a href="#reg-6.3-modem_ctl">MODEM_CTL</a>[afc_en] 和 MODEM_CTLL[rts]，根据需要配置自动流控。

表6-7 自动流控配置参考

<table>
<thead><tr><th>场景</th><th><a href="#reg-6.3-modem_ctl">MODEM_CTL</a>[afc_en]配置值</th><th><a href="#reg-6.3-modem_ctl">MODEM_CTL</a>[rts]配置值</th></tr></thead>
<tbody>
<tr><td>打开自动流控</td><td>1</td><td>1</td></tr>
<tr><td>关闭自动流控,反压对端</td><td>0</td><td>0</td></tr>
<tr><td>关闭自动流控,正常 RX</td><td>0</td><td>1</td></tr>
</tbody>
</table>

步骤 10 配置 <a href="#reg-6.3-fifo_ctl">FIFO_CTL</a>[rx_empty_trig] 和 <a href="#reg-6.3-fifo_ctl">FIFO_CTL</a>[tx_empty_trig]，设定发送及接收 FIFO 水线。

步骤 11 配置 <a href="#reg-6.3-fifo_ctl">FIFO_CTL</a>[fifo_en]=1，使能 TX FIFO 和 RX FIFO。

步骤 12 配置 <a href="#reg-6.3-intr_en">INTR_EN</a>=0x1f，恢复中断使能。

步骤 13 恢复 IO 复用，将 IO 复用为 UART。

步骤 14 配置 <a href="#reg-6.3-halt_tx">HALT_TX</a>[halt_tx]=0，使能 TX 功能。

----结束

数据发送步骤如下：

步骤 1 将发送数据写入 <a href="#reg-6.3-data">DATA</a>[data]步骤，启动数据发送。若为查询模式则跳转至步骤 2，若为中断模式则跳转至步骤 3。

步骤 2 查询方式下，如果进行连续数据发送，需要通过读取 <a href="#reg-6.3-fifo_status">FIFO_STATUS</a>[tx_fifo_full]检测 TX FIFO 状态。如果 <a href="#reg-6.3-fifo_status">FIFO_STATUS</a>[tx_fifo_full]为 0，即 TX FIFO 未满，则可以向 TX FIFO 中发送数据。直到无数据需要发送，跳转至步骤 4。

步骤 3 中断方式下，在中断服务程序中查询 <a href="#reg-6.3-intr_status">INTR_STATUS</a>[thre_intr_status]发送中断状态位，决定是否向 TX FIFO 中发送数据。当 <a href="#reg-6.3-intr_status">INTR_STATUS</a>[thre_intr_status]置 1，此时 TX FIFO 内数据量小于发送数据水线，可以向 TX FIFO 中发送数据。直到无数据需要发送，跳转至步骤 4。

步骤 4 通过检测 <a href="#reg-6.3-intr_status">INTR_STATUS</a>[tx_fifo_empty]是否为 1，判断 UART 是否完成全部数据发送。

----结束

数据接收的处理方式如下：

步骤1 等待数据接收，若为查询模式则跳转至步骤2。若为中断模式则跳转至步骤3。

步骤 2 查询方式下，进行数据接收时通过读取 <a href="#reg-6.3-intr_status">INTR_STATUS</a>[rx_fifo_empty] 检测 RX_FIFO 状态，如果 <a href="#reg-6.3-intr_status">INTR_STATUS</a>[rx_fifo_empty] 为 0，则 RX_FIFO 非空，可以读取 RX_FIFO 中的数据，跳转至步骤 4。

步骤 3 中断方式下，则检测 <a href="#reg-6.3-intr_status">INTR_STATUS</a>[data_avail_intr_status]接收中断状态位，决定是否读取 RX_FIFO 中的数据。当 <a href="#reg-6.3-intr_status">INTR_STATUS</a>[data_avail_intr_status]置 1，此时 RX_FIFO 内数据量大于接收 FIFO 水线，可以读取 RX_FIFO 中数据，跳转至步骤 4。

步骤 4 回读 <a href="#reg-6.3-data">DATA</a>[data]，读出数据即为 RX 数据。

----结束

<a id="6.3.4"></a>
## 寄存器概览

UART 寄存器概览如表 6-8 所示。

表6-8 UART 寄存器概览（基址是 UART0：0x44010000、UART1：0x44011000、UART2：0x44012000）

<table>
<thead><tr><th>偏移地址</th><th>名称</th><th>描述</th></tr></thead>
<tbody>
<tr><td>0x00</td><td><a href="#reg-6.3-intr_id">INTR_ID</a></td><td>中断 ID 寄存器。</td></tr>
<tr><td>0x4</td><td><a href="#reg-6.3-data">DATA</a></td><td>数据寄存器。</td></tr>
<tr><td>0x8</td><td><a href="#reg-6.3-uart_ctl">UART_CTL</a></td><td>UART 控制寄存器。</td></tr>
<tr><td>0xC</td><td><a href="#reg-6.3-div_h">DIV_H</a></td><td>分频系数(高位)寄存器。</td></tr>
<tr><td>0x10</td><td><a href="#reg-6.3-div_l">DIV_L</a></td><td>分频系数(低位)寄存器。</td></tr>
<tr><td>0x14</td><td><a href="#reg-6.3-div_fra">DIV_FRA</a></td><td>分频系数(小数部分)寄存器。</td></tr>
<tr><td>0x18</td><td><a href="#reg-6.3-intr_en">INTR_EN</a></td><td>中断使能寄存器。</td></tr>
<tr><td>0x1C</td><td><a href="#reg-6.3-intr_status">INTR_STATUS</a></td><td>中断状态寄存器。</td></tr>
<tr><td>0x24</td><td><a href="#reg-6.3-fifo_ctl">FIFO_CTL</a></td><td>FIFO 控制寄存器。</td></tr>
<tr><td>0x28</td><td><a href="#reg-6.3-far">FAR</a></td><td>FIFO 存取模式使能寄存器。</td></tr>
<tr><td>0x2C</td><td><a href="#reg-6.3-modem_ctl">MODEM_CTL</a></td><td>Modem 控制寄存器。</td></tr>
<tr><td>0x30</td><td><a href="#reg-6.3-modem_status">MODEM_STATUS</a></td><td>Modem 状态寄存器。</td></tr>
<tr><td>0x34</td><td><a href="#reg-6.3-line_status">LINE_STATUS</a></td><td>Line 状态寄存器。</td></tr>
<tr><td>0x38</td><td><a href="#reg-6.3-uart_gp_reg">UART_GP_REG</a></td><td>Uart 通用寄存器。</td></tr>
<tr><td>0x3C</td><td><a href="#reg-6.3-tx_fifo_read">TX_FIFO_READ</a></td><td>发送 FIFO 读取寄存器。</td></tr>
<tr><td>0x40</td><td><a href="#reg-6.3-rx_fifo_write">RX_FIFO_WRITE</a></td><td>接收 FIFO 写入寄存器。</td></tr>
<tr><td>0x44</td><td><a href="#reg-6.3-fifo_status">FIFO_STATUS</a></td><td>FIFO 状态寄存器。</td></tr>
<tr><td>0x48</td><td><a href="#reg-6.3-tx_fifo_cnt">TX_FIFO_CNT</a></td><td>发送 FIFO 数据计数器。</td></tr>
<tr><td>0x4C</td><td><a href="#reg-6.3-rx_fifo_cnt">RX_FIFO_CNT</a></td><td>接收 FIFO 数据计数器。</td></tr>
<tr><td>0x50</td><td><a href="#reg-6.3-halt_tx">HALT_TX</a></td><td>传输挂起寄存器。</td></tr>
<tr><td>0x54</td><td><a href="#reg-6.3-dma_sw_ack">DMA_SW_ACK</a></td><td>DMA 应答寄存器。</td></tr>
<tr><td>0x58</td><td><a href="#reg-6.3-baud_ctl">BAUD_CTL</a></td><td>波特率控制寄存器。</td></tr>
<tr><td>0x5C</td><td><a href="#reg-6.3-stp_ctl">STP_CTL</a></td><td>停止位控制寄存器。</td></tr>
<tr><td>0x60</td><td><a href="#reg-6.3-uart_parameter">UART_PARAMETER</a></td><td>UART 参数寄存器。</td></tr>
</tbody>
</table>

<a id="6.3.5"></a>
## 寄存器描述

<a id="reg-6.3-intr_id"></a>
### INTR_ID

INTR_ID 为中断 ID 寄存器。

Offset Address: 0x00 Total Reset Value: 0x0001

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:5]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x000</td></tr>
<tr><td>[4]</td><td>RO</td><td>fifo_en_s</td><td>FIFO 使能控制。0: FIFO 禁用：<br>
1: FIFO 使能。</td><td>0x0</td></tr>
<tr><td>[3:0]</td><td>RO</td><td>intr_id</td><td>中断 ID。<br>0x0: modem 状态;<br>0x1: 无中断请求;<br>0x2: THR 空标志;<br>0x4: 接收数据到达;<br>0x6: 接收数据线状态;<br>0x7: busy 状态;<br>0xc: 字符超时;<br>others: 未定义。</td><td>0x1</td></tr>
</tbody>
</table>

<a id="reg-6.3-data"></a>
### DATA

DATA 为数据寄存器。

Offset Address: 0x4 Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:8]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00</td></tr>
<tr><td>[7:0]</td><td>RW</td><td>data</td><td>寄存器写入:若使能<a href="#reg-6.3-fifo_ctl">FIFO_CTL</a>[fifo_en],写入该寄存器的数据将被发送至tx fifo;若未使能<a href="#reg-6.3-fifo_ctl">FIFO_CTL</a>[fifo_en],<br>写入该寄存器的数据将被存储于transmitter holding寄存器(简称thr,<br>tx fifo的底端存储位置)。<br>寄存器读取:该寄存器包含从串行输入端口(sin)接收的数据。<br>若<a href="#reg-6.3-fifo_ctl">FIFO_CTL</a>[fifo_en]未使能,当前数据必须在下一个接收数据到来前读取,否则将被覆盖,并产生over-run错误;若使能<a href="#reg-6.3-fifo_ctl">FIFO_CTL</a>[fifo_en],通过该寄存器可访问rx fifo。<br>若rx fifo已满,且在下一接收数据到来前没有读取该寄存器,虽然rx fifo内已有的数据不受影响,但后续到来的数据将丢失,并产生over-run错误。</td><td>0x00</td></tr>
</tbody>
</table>

<a id="reg-6.3-uart_ctl"></a>
### UART_CTL

UART_CTL 为 UART 控制寄存器。

Offset Address: 0x8 Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:8]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00</td></tr>
<tr><td>[7]</td><td>RW</td><td>stp</td><td>停止位宽。0: 1bit 停止位：<br>
1: <a href="#reg-6.3-uart_ctl">UART_CTL</a>[dlen]为零时 1.5bit 停止位,否则 2bit 停止位。注意: 只有当<a href="#reg-6.3-stp_ctl">STP_CTL</a>[stp_mode]==0 时,该寄存器才生效。</td><td>0x0</td></tr>
<tr><td>[6]</td><td>RW</td><td>sps</td><td>粘性奇偶校验位选择。0: 禁用粘性奇偶校验：<br>
1: 使能粘性就校验。</td><td>0x0</td></tr>
<tr><td>[5]</td><td>RW</td><td>pen</td><td>奇偶校验使能。0: 禁用奇偶校验：<br>
1: 使能奇偶校验。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>RW</td><td>eps</td><td>奇偶校验模式选择。0: 选择奇校验模式：<br>
1: 选择偶校验模式。</td><td>0x0</td></tr>
<tr><td>[3:2]</td><td>RW</td><td>dlen</td><td>数据长度。00: 每个字符 5bit 数据：<br>
01: 每个字符 6bit 数据<br>
10: 每个字符 7bit 数据<br>
11: 每个字符 8bit 数据。</td><td>0x0</td></tr>
<tr><td>[1]</td><td>RW</td><td>xbreak</td><td>Break 控制位。0: 串行输出已经释放可用于数据传输：<br>
1: 串行输出强制置于空白状态。</td><td>0x0</td></tr>
<tr><td>[0]</td><td>RW</td><td>div_en</td><td>UART 分频器使能寄存器。0: <a href="#reg-6.3-div_l">DIV_L</a> 与 <a href="#reg-6.3-div_h">DIV_H</a> 两寄存器仅在UART 非 Busy 状态可写入：<br>
1:<a href="#reg-6.3-div_l">DIV_L</a> 与 <a href="#reg-6.3-div_h">DIV_H</a> 两寄存器可在任意时刻读写。注意:本芯片 UART 始终处于非Busy 状态。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.3-div_h"></a>
### DIV_H

DIV_H 为分频系数(高位)寄存器。  
Offset Address: 0xC Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:8]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00</td></tr>
<tr><td>[7:0]</td><td>RW</td><td>div_h</td><td>分频器分频系数整数部分高 8bit。该寄存器仅在 <a href="#reg-6.3-uart_ctl">UART_CTL</a>[div_en]置位且 UART 非忙状态下可以存取。</td><td>0x00</td></tr>
</tbody>
</table>

<a id="reg-6.3-div_l"></a>
### DIV_L

DIV_L 为分频系数(低位) 寄存器。

Offset Address: 0x10 Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:8]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00</td></tr>
<tr><td>[7:0]</td><td>RW</td><td>div_I</td><td>分频器分频系数整数部分低 8bit。该寄存器仅在 <a href="#reg-6.3-uart_ctl">UART_CTL</a>[div_en]置位且 UART 非忙状态下可以存取。</td><td>0x00</td></tr>
</tbody>
</table>

<a id="reg-6.3-div_fra"></a>
### DIV_FRA

DIV_FRA 为分频系数（小数部分）寄存器。

Offset Address: 0x14 Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:6]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x000</td></tr>
<tr><td>[5:0]</td><td>RW</td><td>div_fra</td><td>分频器分频系数小数部分。分频器所用实际分频系数的小数部分为该值除以2^6。</td><td>0x00</td></tr>
</tbody>
</table>

<a id="reg-6.3-intr_en"></a>
### INTR_EN

INTR_EN 为中断使能寄存器。  
Offset Address: 0x18 Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:5]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x000</td></tr>
<tr><td>[4]</td><td>RW</td><td>ptim_en</td><td>可编程THRE中断模式使能。0:禁用可编程THRE中断模式：<br>
1:使能可编程THRE中断模式。</td><td>0x0</td></tr>
<tr><td>[3]</td><td>RW</td><td>tran_em_intr_en</td><td>发送数据为空中断使能。0:禁用发送为空中断：<br>
1:使能发送为空中断。</td><td>0x0</td></tr>
<tr><td>[2]</td><td>RW</td><td>rece_data_intr_en</td><td>接收数据到达中断使能。0:禁用接收数据到达中断：<br>
1:使能接收数据到达中断。</td><td>0x0</td></tr>
<tr><td>[1]</td><td>RW</td><td>modem_intr_en</td><td>Modem状态中断使能。0(DISABLED):禁用Modem状态中断;1(ENABLED):使能Modem状态中断。</td><td>0x0</td></tr>
<tr><td>[0]</td><td>RW</td><td>rece_line_stat_intr_en</td><td>接收数据线状态中断使能。0:禁用接收数据线状态中断：<br>
1:使能接收数据线状态中断。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.3-intr_status"></a>
### INTR_STATUS

INTR_STATUS 为中断状态寄存器。

Offset Address: 0x1C Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:6]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x000</td></tr>
<tr><td>[5]</td><td>RO</td><td>line_intr_status</td><td>接收数据线中断。1: 中断有效：<br>
0: 无中断。注意: 需要使能<a href="#reg-6.3-intr_en">INTR_EN</a>[rece_line_stat_intr_en], 否则本中断保持为0。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>RO</td><td>data_avail_intr_status</td><td>接收数据到达中断。1: 中断有效：<br>
0: 无中断。注意: 需要使能<a href="#reg-6.3-intr_en">INTR_EN</a>[rece_data_intr_en], 否则本中断保持为0。</td><td>0x0</td></tr>
<tr><td>[3]</td><td>RO</td><td>char_to_intr_status</td><td>字符超时中断。1: 中断有效：<br>
0: 无中断。注意: 需要使能<a href="#reg-6.3-fifo_ctl">FIFO_CTL</a>[fifo_en]和<a href="#reg-6.3-intr_en">INTR_EN</a>[rece_data_intr_en], 否则本中断保持为0。</td><td>0x0</td></tr>
<tr><td>[2]</td><td>RO</td><td>thre_intr_status</td><td>THR 空中断。1: 中断有效：<br>
0: 无中断。注意:需要使能<a href="#reg-6.3-intr_en">INTR_EN</a>[tran_em_intr_en],否则本中断保持为0。</td><td>0x0</td></tr>
<tr><td>[1]</td><td>RO</td><td>modem_intr_status</td><td>Modem状态中断。1:中断有效：<br>
0:无中断。注意:需要使能<a href="#reg-6.3-intr_en">INTR_EN</a>[modem_intr_en],否则本中断保持为0。</td><td>0x0</td></tr>
<tr><td>[0]</td><td>RO</td><td>busy_det_intr</td><td>忙状态监测中断。1:中断有效：<br>
0:无中断。注意:本芯片UART始终处于非Busy状态,即该寄存器保持0。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.3-fifo_ctl"></a>
### FIFO_CTL

FIFO_CTL 为 FIFO 控制寄存器。

Offset Address: 0x24 Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:7]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x000</td></tr>
<tr><td>[6]</td><td>WO</td><td>rx_fifo_rst</td><td>接收FIFO复位请求。0:不复位：<br>
1:复位。</td><td>0x0</td></tr>
<tr><td>[5]</td><td>WO</td><td>tx_fifo_rst</td><td>发送FIFO复位请求。0:不复位：<br>
1:复位。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>WO</td><td>fifo_en</td><td>FIFO使能。0:禁用FIFO：<br>
1:使能FIFO。</td><td>0x0</td></tr>
<tr><td>[3:2]</td><td>WO</td><td>rx_empty_trig</td><td>接收空中断触发条件设置。00:FIFO 内存在 1 个字符：<br>
01:FIFO 1/4 满<br>
10:FIFO 1/2 满<br>
11:距FIFO 满状态少 2 个字符以内。</td><td>0x0</td></tr>
<tr><td>[1:0]</td><td>WO</td><td>tx_empty_trig</td><td>发送空中断触发条件设置。00:FIFO 为空：<br>
01:FIFO 内存在 2 个字符<br>
10:FIFO 1/4 满<br>
11:FIFO 1/2 满。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.3-far"></a>
### FAR

FAR 为 FIFO 存取模式使能寄存器。

Offset Address: 0x28 Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:1]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>[0]</td><td>RW</td><td>far</td><td>使能 FIFO 存取模式。出于测试目的,FIFO 存取模式允许 Master 写入接收 FIFO、读取发送 FIFO。0:禁用：<br>
1:使能。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.3-modem_ctl"></a>
### MODEM_CTL

MODEM_CTL 为 Modem 控制寄存器。

Offset Address: 0x2C Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:6]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x000</td></tr>
<tr><td>[5]</td><td>RW</td><td>out2</td><td>可编程接口 OUT2。0: out2_n 端口撤离：<br>
1: out2_n 端口有效。注意: 该寄存器无用。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>RW</td><td>out1</td><td>可编程接口 OUT1。0: out1_n 端口撤离：<br>
1: out1_n 端口有效。注意: 该寄存器无用。</td><td>0x0</td></tr>
<tr><td>[3]</td><td>RW</td><td>dtr</td><td>数据终端 ready。0: dtr_n 端口撤离：<br>
1: dtr_n 端口有效。注意: 该寄存器无用。</td><td>0x0</td></tr>
<tr><td>[2]</td><td>RW</td><td>rts</td><td>自动流控 RTS 软控信号。0: 反压对端, rts_n=1：<br>
1: 自动流控 RTS 软控信号。注意: 当MODEL_CTL[afc_en]==1 且<a href="#reg-6.3-fifo_ctl">FIFO_CTL</a>[fifo_en]==1 时, 自动流控 RTS 由硬件逻辑接管, 此时本寄存器无效。</td><td>0x0</td></tr>
<tr><td>[1]</td><td>RW</td><td>lb_mode</td><td>环回模式控制。0: 禁用：<br>
1: 使能。</td><td>0x0</td></tr>
<tr><td>[0]</td><td>RW</td><td>afc_en</td><td>自动流控模式使能。0: 禁用：<br>
1: 使能。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.3-modem_status"></a>
### MODEM_STATUS

MODEM_STATUS 为 Modem 状态寄存器。

Offset Address: 0x30 Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:8]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00</td></tr>
<tr><td>[7]</td><td>RO</td><td>dsr</td><td>数据已准备指示寄存器。用于指示调制解调器控制线 dsr_n 的当前状态。在环回模式(<a href="#reg-6.3-modem_ctl">MODEM_CTL</a>[lb_mode==1])下,本寄存器值取自与<a href="#reg-6.3-modem_ctl">MODEM_CTL</a>[dtr]。0: dsr_n 信号线高电平：<br>
1: dsr_n 信号线为低电平。注意:本芯片 UART 的 dsr_n 恒为 1。</td><td>0x0</td></tr>
<tr><td>[6]</td><td>RO</td><td>ddsr</td><td>dsr_n 跳变指示寄存器,指示 dsr 是否有跳变。0: dsr_n 信号线无变化：<br>
1: dsr_n 信号线存在变化。</td><td>0x0</td></tr>
<tr><td>[5]</td><td>RO</td><td>ri</td><td>响铃指示寄存器。用于指示调制解调器控制线 ri_n 的当前状态。在环回模式(<a href="#reg-6.3-modem_ctl">MODEM_CTL</a>[lb_mode==1])下,本寄存器值取自<a href="#reg-6.3-modem_ctl">MODEM_CTL</a>[out1]。0: ri_n 信号线为高电平：<br>
1: ri_n 信号线为低电平。注意:本芯片 UART 的 ri_n 恒为 1。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>RO</td><td>teri</td><td>ri_n 上升沿指示寄存器。0: 未检测到 ri_n 上升沿：<br>
1: 检测到 ri_n 的上升沿。</td><td>0x0</td></tr>
<tr><td>[3]</td><td>RO</td><td>dcd</td><td>数据载波检测寄存器。用于指示modem 控制线 dcd_n 的当前状态。在环回模式(<a href="#reg-6.3-modem_ctl">MODEM_CTL</a>[lb_mode==1])下,本寄存器值取自MODEL_CTL[out2]。0: dcd_n 信号线为高电平：<br>
1: dcd_n 信号线为低电平。注意:本芯片 UART 的 dcd_n 恒为 1。</td><td>0x0</td></tr>
<tr><td>[2]</td><td>RO</td><td>ddcd</td><td>dcd_n 跳变指示寄存器,指示 dcd 是否有跳变。0: dcd_n 信号线无变化：<br>
1: dcd_n 信号线存在变化。</td><td>0x0</td></tr>
<tr><td>[1]</td><td>RO</td><td>cts</td><td>CTS 信号状态。0: cts_n 信号线为高电平：<br>
1: cts_n 信号线为低电平。</td><td>0x0</td></tr>
<tr><td>[0]</td><td>RO</td><td>dcts</td><td>该 bit 位用于指示 Modem 的CTS_N 数据线从上一次读取<a href="#reg-6.3-modem_status">MODEM_STATUS</a> 寄存器后是否发生过变化。0: 无变化：<br>
1: 存在变化。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.3-line_status"></a>
### LINE_STATUS

LINE_STATUS 为 Line 状态寄存器。

Offset Address: 0x34 Total Reset Value: 0x00C0

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:8]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00</td></tr>
<tr><td>[7]</td><td>RO</td><td>tx_empty_s</td><td>发射器空标志。若FIFO使能,该bit在发送器的移位寄存器与发送FIFO同时为空时置位：<br>
当FIFO禁用时,该bit在thr寄存器与移位寄存器同时位空时置位。0:发送器非空<br>
1:发送器为空。</td><td>0x1</td></tr>
<tr><td>[6]</td><td>RO</td><td>thre_s</td><td>thr寄存器为空标志。在<a href="#reg-6.3-intr_en">INTR_EN</a>[ptim_en]使能的前提下,如果<a href="#reg-6.3-fifo_ctl">FIFO_CTL</a>[fifo_en]也使能,该bit位在发送FIFO位满时置位：<br>
当<a href="#reg-6.3-fifo_ctl">FIFO_CTL</a>[fifo_en]未使能,该bit位在thr寄存器为空时置位。0:THRE中断控制禁用<br>
1:THRE中断控制使能。</td><td>0x1</td></tr>
<tr><td>[5]</td><td>RO</td><td>data_available</td><td>指示RBR或接收FIFO内至少存在1个字符。非FIFO模式下该bit位在RBR倍读取后清零,在FIFO模式下该bit位在接收FIFO为空时清零。0:数据未准备好：<br>
1:数据已准备好。</td><td>0x0</td></tr>
<tr><td>[4]</td><td>RO</td><td>break_intr</td><td>指示串行输入数据中是否检测到break序列。0:无break序列：<br>
1:检测到break序列。</td><td>0x0</td></tr>
<tr><td>[3]</td><td>RO</td><td>overrun_err</td><td>Overrun错误,读取<a href="#reg-6.3-line_status">LINE_STATUS</a>寄存器清零该bit位。0:无 overrun 错误：<br>
1:发送 overrun 错误。</td><td>0x0</td></tr>
<tr><td>[2]</td><td>RO</td><td>parity_err</td><td>奇偶校验错误,读取<a href="#reg-6.3-line_status">LINE_STATUS</a> 寄存器清零该 bit 位。0:无奇偶校验错误：<br>
1:奇偶校验报错。</td><td>0x0</td></tr>
<tr><td>[1]</td><td>RO</td><td>frame_err</td><td>帧错误,读取 <a href="#reg-6.3-line_status">LINE_STATUS</a> 寄存器清零该 bit 位。0:无帧错误：<br>
1:帧错误。</td><td>0x0</td></tr>
<tr><td>[0]</td><td>RO</td><td>rx_fifo_err</td><td>接收 FIFO 错误状态指示。该 bit 位仅在 FIFO 使能时生效。当存在错误的字符位于接收 FIFO 顶部且后续数据无错误,那么在读取 LSR 后该 bit 位清零。0:接收 FIFO 无错误：<br>
1:接收 FIFO 报错。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.3-uart_gp_reg"></a>
### UART_GP_REG

UART_GP_REG 为 Uart 通用寄存器。

Offset Address: 0x38 Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:8]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00</td></tr>
<tr><td>[7:0]</td><td>RW</td><td>uart_gp_reg</td><td>为开发者提供临时存储空间,在UART ip内无明确定义。</td><td>0x00</td></tr>
</tbody>
</table>

<a href="#reg-6.3-tx_fifo_read">TX_FIFO_READ</a> 为发送 FIFO 读取寄存器。

<a id="reg-6.3-tx_fifo_read"></a>
### TX_FIFO_READ

Offset Address: 0x3C Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:8]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00</td></tr>
<tr><td>[7:0]</td><td>RO</td><td>tx_fifo_read</td><td>tx fifo 读数据寄存器。<br>当<a href="#reg-6.3-fifo_ctl">FIFO_CTL</a>[fifo_en]被使能,读取该寄存器将返回 tx fifo 顶部数据,<br>每个连续的读取会对 tx fifo 进行 POP 操作,并返回当前在 tx fifo 顶部的数据;当 <a href="#reg-6.3-fifo_ctl">FIFO_CTL</a>[fifo_en]未被使能,<br>读取该寄存器将返回 DR 中的数据。</td><td>0x00</td></tr>
</tbody>
</table>

<a id="reg-6.3-rx_fifo_write"></a>
### RX_FIFO_WRITE

RX_FIFO_WRITE 为接收 FIFO 写入寄存器。  
Offset Address: 0x40 Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:10]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00</td></tr>
<tr><td>[9]</td><td>WO</td><td>rx_fifo_fe</td><td>接收FIFO帧错误。0:无错误：<br>
1:帧错误。</td><td>0x0</td></tr>
<tr><td>[8]</td><td>WO</td><td>rx_fifo_pe</td><td>接收FIFO奇偶校验错误。0:校验位错误：<br>
1:无错误。</td><td>0x0</td></tr>
<tr><td>[7:0]</td><td>RW</td><td>rx_fifo_write</td><td>写操作:向rx fifo写数据。<br>该寄存器仅在FIFO存取模式使能(<a href="#reg-6.3-far">FAR</a>[far]==1)时生效。<br>当<a href="#reg-6.3-fifo_ctl">FIFO_CTL</a>[fifo_en]使能时,写入该寄存器的数据将被压入rx fifo。<br>每个连续的写操作会连续压入新数据至 rx fifo 的下一写入位置;当<a href="#reg-6.3-fifo_ctl">FIFO_CTL</a>[fifo_en]未被使能,写入该寄存器的数据将被压入 DR。<br>读操作:返回 rx_fifo_level[6:0],指示 rx fifo 内的数据量。</td><td>0x00</td></tr>
</tbody>
</table>

<a id="reg-6.3-fifo_status"></a>
### FIFO_STATUS

FIFO_STATUS 为 FIFO 状态寄存器。

Offset Address: 0x44 Total Reset Value: 0x0002

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:4]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x000</td></tr>
<tr><td>[3]</td><td>RO</td><td>rx_fifo_empty</td><td>rx fifo 空标志。0: 接收 FIFO 非空：<br>
1: 接收 FIFO 为空。</td><td>0x1</td></tr>
<tr><td>[2]</td><td>RO</td><td>rx_fifo_full</td><td>rx fifo 满标志。0: 接收 FIFO 非满：<br>
1: 接收 FIFO 为满。</td><td>0x0</td></tr>
<tr><td>[1]</td><td>RO</td><td>tx_fifo_empty</td><td>tx fifo 空标志。0: 发送 FIFO 非空：<br>
1: 发送 FIFO 为空状态。</td><td>0x1</td></tr>
<tr><td>[0]</td><td>RO</td><td>tx_fifo_full</td><td>tx fifo 满标志。0: 发送 FIFO 非满：<br>
1: 发送 FIFO 为满。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.3-tx_fifo_cnt"></a>
### TX_FIFO_CNT

TX_FIFO_CNT 为发送 FIFO 数据计数器。

Offset Address: 0x48 Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:7]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x000</td></tr>
<tr><td>[6:0]</td><td>RO</td><td>tx_fifo_level</td><td>tx fifo 数据计数器。用于指示 tx fifo 内数据量。</td><td>0x00</td></tr>
</tbody>
</table>

<a id="reg-6.3-rx_fifo_cnt"></a>
### RX_FIFO_CNT

RX_FIFO_CNT 为接收 FIFO 数据计数器。

Offset Address: 0x4C Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:7]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x000</td></tr>
<tr><td>[6:0]</td><td>RO</td><td>reserved</td><td>保留。</td><td>0x00</td></tr>
</tbody>
</table>

<a id="reg-6.3-halt_tx"></a>
### HALT_TX

HALT_TX 为传输挂起寄存器。

Offset Address: 0x50 Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:1]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>[0]</td><td>RW</td><td>halt_tx</td><td>TX 功能挂起使能。0:禁用传输挂起功能：<br>
1:使能传输挂起功能。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.3-dma_sw_ack"></a>
### DMA_SW_ACK

DMA_SW_ACK 为 DMA 应答寄存器。

Offset Address: 0x54 Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:1]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>[0]</td><td>WC</td><td>dma_sw_ack</td><td>DMA 软件应答。0: 未应答：<br>
1: DMA 软件应答。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.3-baud_ctl"></a>
### BAUD_CTL

BAUD_CTL 为波特率控制寄存器。

Offset Address: 0x58 Total Reset Value: 0x007F

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:8]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00</td></tr>
<tr><td>[7:4]</td><td>RW</td><td>sample_phase</td><td>接收采样相位。</td><td>0x7</td></tr>
<tr><td>[3:0]</td><td>RW</td><td>baud_div</td><td>波特率过采样倍数。0x7: 8 倍波特率采样：<br>
0xF: 16 倍波特率采样<br>
其他: 不支持。</td><td>0xF</td></tr>
</tbody>
</table>

<a id="reg-6.3-stp_ctl"></a>
### STP_CTL

STP_CTL 为停止位控制寄存器。

Offset Address: 0x5C Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:3]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>[2]</td><td>RW</td><td>stp_mode</td><td>停止位控制模式。0: 接收与发送的停止位位宽由<a href="#reg-6.3-uart_ctl">UART_CTL</a>[stp]控制：<br>
1: 发送停止位位宽受<a href="#reg-6.3-stp_ctl">STP_CTL</a>[tx_sp]控制,接收停止位位宽受 <a href="#reg-6.3-stp_ctl">STP_CTL</a>[rx_sp]控制。</td><td>0x0</td></tr>
<tr><td>[1]</td><td>RW</td><td>tx_sp</td><td>发送停止位位宽。0: 1bit 停止位：<br>
1: 当 <a href="#reg-6.3-uart_ctl">UART_CTL</a>[dlen]为零,1.5bit 停止位,否则 2bit 停止位。注意: 只有当 stp_mode 置位,<a href="#reg-6.3-stp_ctl">STP_CTL</a>[tx_sp]才生效。</td><td>0x0</td></tr>
<tr><td>[0]</td><td>RW</td><td>rx_sp</td><td>接收停止位位宽。0: 1bit 停止位：<br>
1: 当 <a href="#reg-6.3-uart_ctl">UART_CTL</a>[dlen]为零,1.5bit 停止位,否则 2bit 停止位。注意: 只有当 stp_mode 置位,<a href="#reg-6.3-stp_ctl">STP_CTL</a>[rx_sp]才生效。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.3-uart_parameter"></a>
### UART_PARAMETER

UART_PARAMETER 为 UART 参数寄存器。

Offset Address: 0x60 Total Reset Value: 0x0D04

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:13]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
<tr><td>[12]</td><td>RO</td><td>shadow</td><td>SHADOW 功能使能。0:禁用：<br>
1:使能。</td><td>0x0</td></tr>
<tr><td>[11]</td><td>RO</td><td>dma_mode</td><td>DMA 模式查询。0: DMA_EXTRA 禁用：<br>
1: DMA_EXTRA 使能。</td><td>0x1</td></tr>
<tr><td>[10]</td><td>RO</td><td>afce_mode</td><td>AFCE 模式查询。0: AFCE 模式禁用：<br>
1: AFCE 模式使能。</td><td>0x1</td></tr>
<tr><td>[9:8]</td><td>RO</td><td>apb_data_width</td><td>总线接口宽度查询寄存器。00: APB 数据位宽 8bit：<br>
01: APB 数据位宽 16bit<br>
10: APB 数据位宽 32bit<br>
11: 未定义。注意:本芯片 UART APB 数据位宽固定为 16bit,故该寄存器值固定为 0x1。</td><td>0x1</td></tr>
<tr><td>[7:0]</td><td>RO</td><td>fifo_depth</td><td>UART 接收发送 FIFO 深度。0x0: FIFO 深度为 0：<br>
0x1: FIFO 深度为 16<br>
0x2: FIFO 深度为 32<br>
0x4: FIFO 深度为 64<br>
0x8: FIFO 深度为 128<br>
0x10: FIFO 深度为 256<br>
0x20: FIFO 深度为 512<br>
0x40: FIFO 深度为 1024<br>
0x80: FIFO 深度为 2048<br>
其他:未定义。注意:本芯片 UART FIFO 深度固定为 64,故该寄存器值固定为 0x4。</td><td>0x04</td></tr>
</tbody>
</table>

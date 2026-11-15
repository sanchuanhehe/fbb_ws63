<a id="6.7"></a>
# Tsensor

<a id="6.7.1"></a>
## 概述

Tsensor 是模拟温度检测 IP，检测芯片的节温并以二进制形式输出温度信息。

检查温度范围：-40°C ~ +125°C的温度检测，10bit SARADC 量化温度，分辨率0.208°C/LSB。

支持 IP 校准后温度精度在 ±2°C 以内。

<a id="6.7.2"></a>
## 功能描述

- 支持 Tsensor 三种测温模式：单次 16 点平均测温模式，周期 16 点平均测温模式，单点测温模式。

- 支持 Tsensor 测温完成中断上报。

- 支持 Tsensor 测温 overtemp 中断上报。

- 支持高温低温门限的使用。

- 支持软件分别可配高温门限和低温门限。

- 支持周期采样温度上报。

- 支持软件可配周期采样间隔。

- 支持 Tsensor 测温门限中断上报。

<a id="6.7.3"></a>
## 工作方式

Tsensor 模块的工作模式分为以下 3 种:

- 正常检测温度模式。

- 高低温门限中断模式。

- 过温保护中断模式。

说明

以上 3 种模式均为在检测温度值基础上进行，检测温度的模式有多种，此处检测温度的模式均为周期采样配合 16 次单点平均计算。

<a id="6.7.3.1"></a>
### 正常检测温度模式

正常检测温度模式配置步骤如下：

步骤 1 写 <a href="#reg-6.7-tsensor_sts">TSENSOR_STS</a>[tsensor_clr]为 0x1，清除所有模式的状态信息。

步骤 2 写 <a href="#reg-6.7-tsensor_temp_int_clr">TSENSOR_TEMP_INT_CLR</a>[tsensor_int_clr]为 0x1，清除 Tsensor 中断信号。

步骤 3 写 <a href="#reg-6.7-tsensor_temp_int_en">TSENSOR_TEMP_INT_EN</a>[tsensor_done_int_en]为 0x1，打开 Tsensor 的采集温度完成中断使能。

步骤 4 写 <a href="#reg-6.7-tsensor_ctrl">TSENSOR_CTRL</a>[tsensor_mode]为 0x0，选择 16 次平均值的单次上报方式。

步骤 5 写 <a href="#reg-6.7-tsensor_ctrl">TSENSOR_CTRL</a>[tsensor_enable]为 0x1，开启 Tsensor 的使能信号。

步骤 6 写 <a href="#reg-6.7-tsensor_auto_refresh_period">TSENSOR_AUTO_REFRESH_PERIOD</a>[tsensor_auto_refresh_period]，设置合适的周期采样的时间间隔。

步骤 7 写 <a href="#reg-6.7-tsensor_auto_refresh_cfg">TSENSOR_AUTO_REFRESH_CFG</a>[tsensor_auto_refresh_enable]为 0x1，开启周期采样的使能信号。

步骤 8 读 <a href="#reg-6.7-tsensor_temp_int_sts">TSENSOR_TEMP_INT_STS</a>[tsensor_done_int_sts]为 0x1，等待 16 点平均计算温度模式下的中断信号的产生。

步骤 9 读 <a href="#reg-6.7-tsensor_sts">TSENSOR_STS</a>[tsensor_data]，获取 16 点平均单次上报模式下的温度值。

----结束

<a id="6.7.3.2"></a>
### 高低温门限中断模式

高低温门限中断模式配置步骤如下：

步骤 1 写 <a href="#reg-6.7-tsensor_sts">TSENSOR_STS</a>[tsensor_clr]为 0x1，清除自动模式下产生的 rdy 信号。

步骤 2 写 <a href="#reg-6.7-tsensor_temp_int_clr">TSENSOR_TEMP_INT_CLR</a>[tsensor_int_clr]为 0x1，清除 Tsensor 中断信号。

步骤 3 写 <a href="#reg-6.7-tsensor_temp_int_en">TSENSOR_TEMP_INT_EN</a>[tsensor_out_thresh_int_en]为 0x1，打开 Tsensor 的超门限范围中断使能。

步骤 4 写 <a href="#reg-6.7-tsensor_temp_high_limit">TSENSOR_TEMP_HIGH_LIMIT</a>[tsensor_temp_high_limit]、<a href="#reg-6.7-tsensor_temp_low_limit">TSENSOR_TEMP_LOW_LIMIT</a>[tsensor_temp_low_limit]，设置合适的高低温门限值。

步骤 5 写 <a href="#reg-6.7-tsensor_ctrl">TSENSOR_CTRL</a>[tsensor_mode]为 0x0，选择 16 次平均值的单次上报方式。

步骤 6 写 <a href="#reg-6.7-tsensor_ctrl">TSENSOR_CTRL</a>[tsensor_enable]为 0x1，开启 Tsensor 的使能信号。

步骤 7 写 <a href="#reg-6.7-tsensor_auto_refresh_period">TSENSOR_AUTO_REFRESH_PERIOD</a>[tsensor_auto_refresh_period]，设置合适的周期采样的时间间隔。

步骤 8 写 <a href="#reg-6.7-tsensor_auto_refresh_cfg">TSENSOR_AUTO_REFRESH_CFG</a>[tsensor_auto_refresh_enable]为 0x1，开启周期采样的使能信号。

步骤 9 读 <a href="#reg-6.7-tsensor_temp_int_sts">TSENSOR_TEMP_INT_STS</a>[tsensor_out_thresh_int_sts]是否为 1，如果为 1，则当前的温度值高于高温门限或低于低温门限。

----结束

<a id="6.7.3.3"></a>
### 过温保护中断模式

过温保护中断模式配置步骤如下：

步骤 1 写 <a href="#reg-6.7-tsensor_sts">TSENSOR_STS</a>[tsensor_clr]为 0x1，清除自动模式下产生的 rdy 信号。

步骤 2 写 <a href="#reg-6.7-tsensor_temp_int_clr">TSENSOR_TEMP_INT_CLR</a>[tsensor_int_clr]为 0x1，清除 Tsensor 中断信号。

步骤 3 写 <a href="#reg-6.7-tsensor_temp_int_en">TSENSOR_TEMP_INT_EN</a>[tsensor_overtemp_int_en]为 0x1，打开 Tsensor 的过温中断使能。

步骤 4 写 <a href="#reg-6.7-tsensor_over_temp">TSENSOR_OVER_TEMP</a>[tsensor_overtemp_thresh]，设置合适的过温门限值。

步骤 5 写 <a href="#reg-6.7-tsensor_over_temp">TSENSOR_OVER_TEMP</a>[tsensor_overtemp_thresh_en]为 0x1，打开过温保护使能信号。

步骤 6 写 <a href="#reg-6.7-tsensor_ctrl">TSENSOR_CTRL</a>[tsensor_mode]为 0x0，选择 16 次平均值的单次上报方式。

步骤 7 写 <a href="#reg-6.7-tsensor_ctrl">TSENSOR_CTRL</a>[tsensor_enable]为 0x1，开启 Tsensor 的使能信号。

步骤 8 写 <a href="#reg-6.7-tsensor_auto_refresh_period">TSENSOR_AUTO_REFRESH_PERIOD</a>[tsensor_auto_refresh_period]，设置合适的周期采样的时间间隔。

步骤 9 写 <a href="#reg-6.7-tsensor_auto_refresh_cfg">TSENSOR_AUTO_REFRESH_CFG</a>[tsensor_auto_refresh_enable]为 0x1，开启周期采样的使能信号。

读 <a href="#reg-6.7-tsensor_temp_int_sts">TSENSOR_TEMP_INT_STS</a>[tsensor_overtemp_int_sts]是否为 1，如果为 1，则当前的温度值超过了设置的过温门限值。

----结束

<a id="6.7.4"></a>
## 寄存器概览

Tsensor 寄存器概览如表 1-1 所示。

表6-12 Tsensor 寄存器概览（基址是 0x4000_0000）

<table>
<thead><tr><th>偏移地址</th><th>名称</th><th>描述</th></tr></thead>
<tbody>
<tr><td>0x0000 + 0x1000×CH_NUM</td><td><a href="#reg-6.7-tsensor_ctl_id">TSENSOR_CTL_ID</a></td><td>TSENSOR CTL ID 寄存器。</td></tr>
<tr><td>0x0010 + 0x1000×CH_NUM</td><td><a href="#reg-6.7-tsensor_reg0">TSENSOR_REG0</a></td><td>通用寄存器。</td></tr>
<tr><td>0x0014 + 0x1000×CH_NUM</td><td><a href="#reg-6.7-tsensor_reg1">TSENSOR_REG1</a></td><td>通用寄存器。</td></tr>
<tr><td>0x0018 + 0x1000×CH_NUM</td><td><a href="#reg-6.7-tsensor_reg2">TSENSOR_REG2</a></td><td>通用寄存器。</td></tr>
<tr><td>0x001C + 0x1000×CH_NUM</td><td><a href="#reg-6.7-tsensor_reg3">TSENSOR_REG3</a></td><td>通用寄存器。</td></tr>
<tr><td>0x0300 + 0x1000×CH_NUM</td><td><a href="#reg-6.7-tsensor_start">TSENSOR_START</a></td><td>TSENSOR 启动寄存器。</td></tr>
<tr><td>0x0304 + 0x1000×CH_NUM</td><td><a href="#reg-6.7-tsensor_ctrl">TSENSOR_CTRL</a></td><td>TSENSOR 控制寄存器。</td></tr>
<tr><td>0x0308 + 0x1000×CH_NUM</td><td><a href="#reg-6.7-tsensor_sts">TSENSOR_STS</a></td><td>TSENSOR 状态寄存器。</td></tr>
<tr><td>0x0310 + 0x1000×CH_NUM</td><td><a href="#reg-6.7-tsensor_ctrl1">TSENSOR_CTRL1</a></td><td>TSENSOR 控制寄存器1。</td></tr>
<tr><td>0x0314 + 0x1000×CH_NUM</td><td><a href="#reg-6.7-tsensor_temp_high_limit">TSENSOR_TEMP_HIGH_LIMIT</a></td><td>TSENSOR 温度门限上限。</td></tr>
<tr><td>0x0318 + 0x1000×CH_NUM</td><td><a href="#reg-6.7-tsensor_temp_low_limit">TSENSOR_TEMP_LOW_LIMIT</a></td><td>TSENSOR 温度门限下限。</td></tr>
<tr><td>0x031C + 0x1000×CH_NUM</td><td><a href="#reg-6.7-tsensor_over_temp">TSENSOR_OVER_TEMP</a></td><td>TSENSOR 过温控制寄存器。</td></tr>
<tr><td>0x0320 + 0x1000×CH_NUM</td><td><a href="#reg-6.7-tsensor_temp_int_en">TSENSOR_TEMP_INT_EN</a></td><td>TSENSOR 中断控制寄存器。</td></tr>
<tr><td>0x0324 + 0x1000×CH_NUM</td><td><a href="#reg-6.7-tsensor_temp_int_clr">TSENSOR_TEMP_INT_CLR</a></td><td>TSENSOR 中断控制寄存器。</td></tr>
<tr><td>0x0328 + 0x1000×CH_NUM</td><td><a href="#reg-6.7-tsensor_temp_int_sts">TSENSOR_TEMP_INT_STS</a></td><td>TSENSOR 中断控制寄存器。</td></tr>
<tr><td>0x0330 + 0x1000×CH_NUM</td><td><a href="#reg-6.7-tsensor_auto_refresh_period">TSENSOR_AUTO_REFRESH_PERIOD</a></td><td>TSENSOR 自动检测控制寄存器。</td></tr>
<tr><td>0x0334 + 0x1000×CH_NUM</td><td><a href="#reg-6.7-tsensor_auto_refresh_cfg">TSENSOR_AUTO_REFRESH_CFG</a></td><td>TSENSOR 自动检测控制寄存器。</td></tr>
</tbody>
</table>

Tsensor 寄存器偏移地址中变量的取值范围和含义如表 6-15 所示。

表6-13 Tsensor 寄存器偏移地址变量表

<table>
<thead><tr><th>变量名称</th><th>取值范围</th><th>描述</th></tr></thead>
<tbody>
<tr><td>CH_NUM</td><td>0</td><td>Tsensor 组数</td></tr>
</tbody>
</table>

<a id="6.7.5"></a>
## 寄存器描述

<a id="reg-6.7-tsensor_ctl_id"></a>
### TSENSOR_CTL_ID

TSENSOR_CTL_ID 为 TSENSOR CTL ID 寄存器

Offset Address: 0x0000 + 0x1000 × CH_NUM Total Reset Value: 0x0106

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:0]</td><td>RO</td><td>tsensor_ctl_id</td><td>TSENSOR CTL ID 寄存器。</td><td>0x0106</td></tr>
</tbody>
</table>

<a id="reg-6.7-tsensor_reg0"></a>
### TSENSOR_REG0

TSENSOR_REG0 为通用寄存器。

Offset Address: 0x0010 + 0x1000×CH_NUM Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:0]</td><td>RW</td><td>tsensor_reg0</td><td>Tsensor 通用寄存器 0。</td><td>0x0000</td></tr>
</tbody>
</table>

<a id="reg-6.7-tsensor_reg1"></a>
### TSENSOR_REG1

TSENSOR_REG1 为通用寄存器。

Offset Address: 0x0014 + 0x1000×CH_NUM Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:0]</td><td>RW</td><td>tsensor_reg1</td><td>Tsensor 通用寄存器 1。</td><td>0x0000</td></tr>
</tbody>
</table>

<a id="reg-6.7-tsensor_reg2"></a>
### TSENSOR_REG2

TSENSOR_REG2 为通用寄存器。

Offset Address: 0x0018 + 0x1000×CH_NUM Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:0]</td><td>RW</td><td>tsensor_reg2</td><td>Tsensor 通用寄存器 2。</td><td>0x0000</td></tr>
</tbody>
</table>

<a id="reg-6.7-tsensor_reg3"></a>
### TSENSOR_REG3

TSENSOR_REG3 为通用寄存器。

Offset Address: 0x001C + 0x1000×CH_NUM Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:0]</td><td>RW</td><td>tsensor_reg3</td><td>Tsensor 通用寄存器 3。</td><td>0x0000</td></tr>
</tbody>
</table>

<a id="reg-6.7-tsensor_start"></a>
### TSENSOR_START

TSENSOR_START 为 TSENSOR 启动寄存器。

Offset Address: 0x0300 + 0x1000×CH_NUM Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:1]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>[0]</td><td>W1_PULSE</td><td>tsensor_start</td><td>自动模式下,写1刷新一次温度码,回读tsensor_data_auto获取当前温度值,当tsensor_rdy_auto为1时,表明温度值有效。写0无效。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.7-tsensor_ctrl"></a>
### TSENSOR_CTRL

TSENSOR_CTRL 为 TSENSOR 控制寄存器。

Offset Address: 0x0304 + 0x1000×CH_NUM Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:3]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>[2:1]</td><td>RW</td><td>tsensor_mode</td><td>模式选择2'b00: 16 点平均单次上报模式;2'b01: 16 点平均循环上报模式;2'b10、2'b11: 单点循环上报模式(该模式不比较阈值,仅上报温度码)。</td><td>0x0</td></tr>
<tr><td>[0]</td><td>RW</td><td>tsensor_enable</td><td><a href="#reg-6.7-tsensor_ctrl">TSENSOR_CTRL</a> 开关0: 关闭 TENSOR_CTRL：<br>
1: 打开 TENSOR_CTRL。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.7-tsensor_sts"></a>
### TSENSOR_STS

TSENSOR_STS 为 TSENSOR 状态寄存器。

Offset Address: 0x0308 + 0x1000×CH_NUM Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:12]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0</td></tr>
<tr><td>[11:2]</td><td>RO</td><td>tsensor_data</td><td>所有模式下获取到的温度值。<br>10bit 温度区间码值输出,线性分布;-40C~dec'114125C~dec'896即是 tsensor 直接输出码值对温度°C单位换算公式 T_°C=[BIN2DEC(temp_out &lt;9:0&gt;)-114]/(896-114)*[125-(-40)]+(-40)</td><td>0x000</td></tr>
<tr><td>[1]</td><td>RO</td><td>tsensor_rdy</td><td>所有模式下。0:检测未启动或手动检测中：<br>
1:tsensor_data 值为有效的温度值。</td><td>0x0</td></tr>
<tr><td>[0]</td><td>W1_PULSE</td><td>tsensor_clr</td><td>清除所有模式的状态。0:无效：<br>
1:清除。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.7-tsensor_ctrl1"></a>
### TSENSOR_CTRL1

TSENSOR_CTRL1 为 TSENSOR 控制寄存器 1。

Offset Address: 0x0310 + 0x1000×CH_NUM Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:5]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x000</td></tr>
<tr><td>[4]</td><td>RW</td><td>temp_scan_dft</td><td>DFT使能0:功能输出：<br>
1:DFT输出。</td><td>0x0</td></tr>
<tr><td>[3]</td><td>RW</td><td>temp_set</td><td>0:功能模式下 temp_out 正常输出(默认)：<br>
1:功能模式下 temp_out 可以通过数模接口输入来配置输出值。</td><td>0x0</td></tr>
<tr><td>[2:1]</td><td>RW</td><td>temp_ct_sel</td><td>对于输入 1M 精准时钟情况下：<br>
00:0.512ms<br>
01:0.256ms<br>
10:1.024ms<br>
11:2.048ms。</td><td>0x0</td></tr>
<tr><td>[0]</td><td>RW</td><td>temp_calib</td><td>0:选择开启校准算法(默认);1.:不开启校准算法。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.7-tsensor_temp_high_limit"></a>
### TSENSOR_TEMP_HIGH_LIMIT

TSENSOR_TEMP_HIGH_LIMIT 为 TSENSOR 温度门限上限。

Offset Address: 0x0314 + 0x1000×CH_NUM Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:10]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00</td></tr>
<tr><td>[9:0]</td><td>RW</td><td>tsensor_temp_high_limit</td><td>过高温度阈值门限。</td><td>0x000</td></tr>
</tbody>
</table>

<a id="reg-6.7-tsensor_temp_low_limit"></a>
### TSENSOR_TEMP_LOW_LIMIT

TSENSOR_TEMP_LOW_LIMIT 为 TSENSOR 温度门限下限。

Offset Address: 0x0318 + 0x1000×CH_NUM Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:10]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00</td></tr>
<tr><td>[9:0]</td><td>RW</td><td>tsensor_temp_low_limit</td><td>过低温度阈值门限。</td><td>0x000</td></tr>
</tbody>
</table>

<a id="reg-6.7-tsensor_over_temp"></a>
### TSENSOR_OVER_TEMP

TSENSOR_OVER_TEMP 为 TSENSOR 过温控制寄存器

Offset Address: 0x031C + 0x1000×CH_NUM Total Reset Value: 0x03FF

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:11]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00</td></tr>
<tr><td>[10]</td><td>RW</td><td>tsensor_overtemp_thresh_en</td><td>16点平均单次上报模式或16点平均循环上报模式下过温PA保护使能。0: PA保护使能关闭：<br>
1: PA保护使能打开。</td><td>0x0</td></tr>
<tr><td>[9:0]</td><td>RW</td><td>tsensor_overtemp_thresh</td><td>过温保护阈值门限。</td><td>0x3FF</td></tr>
</tbody>
</table>

<a id="reg-6.7-tsensor_temp_int_en"></a>
### TSENSOR_TEMP_INT_EN

TSENSOR_TEMP_INT_EN 为 TSENSOR 中断控制寄存器

Offset Address: 0x0320 + 0x1000×CH_NUM Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:3]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>[2]</td><td>RW</td><td>tsensor_overemp_int_en</td><td>TSENSOR 过温保护中断使能。</td><td>0x0</td></tr>
<tr><td>[1]</td><td>RW</td><td>tsensor_out_thresh_int_en</td><td>TSENSOR 温度超门限范围中断使能。0:禁止：<br>
1:使能。</td><td>0x0</td></tr>
<tr><td>[0]</td><td>RW</td><td>tsensor_done_int_en</td><td>TSENSOR 温度采集完毕中断使能。0:禁止：<br>
1:使能。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.7-tsensor_temp_int_clr"></a>
### TSENSOR_TEMP_INT_CLR

TSENSOR_TEMP_INT_CLR 为 TSENSOR 中断控制寄存器。

Offset Address: 0x0324 + 0x1000×CH_NUM Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:1]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>[0]</td><td>W1_PULSE</td><td>tsensor_int_clr</td><td>0:无效：<br>
1:清除中断。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.7-tsensor_temp_int_sts"></a>
### TSENSOR_TEMP_INT_STS

TSENSOR_TEMP_INT_STS 为 TSENSOR 中断控制寄存器。

Offset Address: 0x0328 + 0x1000×CH_NUM Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:3]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>[2]</td><td>RO</td><td>tsensor_overtemp_int_sts</td><td>TSENSOR 过温保护中断状态。</td><td>0x0</td></tr>
<tr><td>[1]</td><td>RO</td><td>tsensor_out_thresh_int_sts</td><td>TSENSOR 温度超门限范围中断状态。</td><td>0x0</td></tr>
<tr><td>[0]</td><td>RO</td><td>tsensor_done_int_sts</td><td>TSENSOR 温度采集完毕中断状态。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.7-tsensor_auto_refresh_period"></a>
### TSENSOR_AUTO_REFRESH_PERIOD

TSENSOR_AUTO_REFRESH_PERIOD 为 TSENSOR 自动检测控制寄存器。

Offset Address: 0x0330 + 0x1000×CH_NUM Total Reset Value: 0xFFFF

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:0]</td><td>RW</td><td>tsensor_auto_refresh_period</td><td>TSENSOR 自动检测周期,32k 时钟周期数。</td><td>0xFFFF</td></tr>
</tbody>
</table>

<a id="reg-6.7-tsensor_auto_refresh_cfg"></a>
### TSENSOR_AUTO_REFRESH_CFG

TSENSOR_AUTO_REFRESH_CFG 为 TSENSOR 自动检测控制寄存器。

Offset Address: 0x0334 + 0x1000×CH_NUM Total Reset Value: 0x0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[15:1]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>[0]</td><td>RW</td><td>tsensor_auto_refresh_enable</td><td>16 点平均单次上报模式下周期检测使能。0:定时周期检测关闭：<br>
1:定时周期检测打开。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="6.6"></a>
# PWM

<a id="6.6.1"></a>
## 概述

PWM 模块用于生成 PWM 信号。

<a id="6.6.2"></a>
## 功能描述

PWM 模式具有以下功能特点:

- 支持8路PWM。

- 支持将 PWM 分组绑定，实现互补输出，最多支持 4 组分组。

- 支持 PWM 输出恒为 0、1 或高阻。

- PWM 占空比支持 0~100% (256 档)。

- 支持 PWM 不同配置间的平滑切换。

<a id="6.6.3"></a>
## 工作方式

PWM0~7 模块支持 2 个时钟源：晶振时钟；PLL 时钟。

以配置 PWM0 信号对 80M 时钟分频、分频倍数为 25 倍、占空比为 1/5 为例，配置步骤如下：

步骤 1 写 CLDO_CRG_CLK_SEL[pwm_cksel]为 0x1，将 PWM 时钟置为 80M。

步骤 2 写 <a href="#reg-6.6-pwm_abnor_state_clr0">PWM_ABNOR_STATE_CLR0</a>[pwm_abnor_state_clr0]为 0xFFFF，写 <a href="#reg-6.6-pwm_abnor_state_clr1">PWM_ABNOR_STATE_CLR1</a>[pwm_abnor_state_clr1]为 0xFFFF，清除异常状态。

步骤 3 写 <a href="#reg-6.6-pwm_en">PWM_EN</a>[pwm_en_0]为 1，使能 PWM0。

步骤 4 写 <a href="#reg-6.6-pwm_freq_l">PWM_FREQ_L</a>[pwm_freq_I_0]为 0x19，配置分频倍数为 25。

步骤 5 写 <a href="#reg-6.6-pwm_duty_l">PWM_DUTY_L</a>[pwm_duty_l_0]为 0x5，配出占空比 1/5。

步骤 6 写 <a href="#reg-6.6-pwm_portity">PWM_PORTITY</a>[pwm_poarity_0]为 0，PWM 输出极性为正向极性。

步骤 7 写 <a href="#reg-6.6-pwm_sel">PWM_SEL</a>[pwm_sel_0]为 0x1，配置第一组使用的 PWM0。

步骤 8 写 <a href="#reg-6.6-pwm_startclrcnt_en">PWM_STARTCLRCNT_EN</a>[pwm_startclrcnt_en_0]为 1，在 start 时刻对 PWM 内部寄存器载入配置并清零计数器。

步骤 9 写 <a href="#reg-6.6-pwm_start">PWM_START</a>[pwm_start_0]为 1，配置 PWM0 新配置生效。

----结束

<a id="6.6.4"></a>
## 寄存器概览

PWM 寄存器概览如表 6-12 所示。

表6-10 PWM 寄存器概览（基址是 0x4402_4000）

<table>
<thead><tr><th>偏移地址</th><th>名称</th><th>描述</th></tr></thead>
<tbody>
<tr><td>0x000 + 0x10×i</td><td><a href="#reg-6.6-pwm_sel">PWM_SEL</a></td><td>pwm 分组选择控制寄存器。</td></tr>
<tr><td>0x004 +</td><td>PWM_STARTCLRCN T_EN</td><td>pwm 配置生效计数清零使能寄存器。</td></tr>
<tr><td>0x10×i</td><td></td><td></td></tr>
<tr><td>0x008 + 0x10×i</td><td><a href="#reg-6.6-pwm_start">PWM_START</a></td><td>PWM 配置生效寄存器。</td></tr>
<tr><td>0x0100 + 0x40×j</td><td><a href="#reg-6.6-pwm_en">PWM_EN</a></td><td>PWM 使能寄存器。</td></tr>
<tr><td>0x0104 + 0x40×j</td><td><a href="#reg-6.6-pwm_portity">PWM_PORTITY</a></td><td>PWM 的输出极性配置寄存器。</td></tr>
<tr><td>0x0108 + 0x40×j</td><td><a href="#reg-6.6-pwm_oen_cfg">PWM_OEN_CFG</a></td><td>Pwm 无效电平高阻使能配置寄存器。</td></tr>
<tr><td>0x010C + 0x40×j</td><td><a href="#reg-6.6-pwm_offset_l">PWM_OFFSET_L</a></td><td><a href="#reg-6.6-pwm_start">PWM_START</a> 为 PWM 相位控制计数值低 16bit 寄存器。</td></tr>
<tr><td>0x0110 + 0x40×j</td><td><a href="#reg-6.6-pwm_offset_h">PWM_OFFSET_H</a></td><td><a href="#reg-6.6-pwm_start">PWM_START</a> 为 PWM 相位控制计数值高 16bit 寄存器。</td></tr>
<tr><td>0x0114 + 0x40×j</td><td><a href="#reg-6.6-pwm_freq_l">PWM_FREQ_L</a></td><td>PWM_FREQ 为 PWM 频率控制计数值低 16bit 寄存器。</td></tr>
<tr><td>0x0118 + 0x40×j</td><td><a href="#reg-6.6-pwm_freq_h">PWM_FREQ_H</a></td><td>PWM_FREQ 为 PWM 频率控制计数值高 16bit 寄存器。</td></tr>
<tr><td>0x011C + 0x40×j</td><td><a href="#reg-6.6-pwm_duty_l">PWM_DUTY_L</a></td><td>PWM_DUTY 为 PWM 占空比计数值寄存器。</td></tr>
<tr><td>0x0120 + 0x40×j</td><td><a href="#reg-6.6-pwm_duty_h">PWM_DUTY_H</a></td><td>PWM_DUTY 为 PWM 占空比计数值寄存器。</td></tr>
<tr><td>0x0124 + 0x40×j</td><td><a href="#reg-6.6-pwm_periodload_flag">PWM_PERIODLOAD_FLAG</a></td><td>PWM 周期配置允许标志寄存器。</td></tr>
<tr><td>0x0128 + 0x40×j</td><td><a href="#reg-6.6-pwm_period_val">PWM_PERIOD_VAL</a></td><td>PWM 周期脉冲周期值寄存器。</td></tr>
<tr><td>0x012C + 0x40×j</td><td><a href="#reg-6.6-pwm_periodcnt">PWM_PERIODCNT</a></td><td>PWM 周期脉冲周期值寄存器。</td></tr>
<tr><td>0x500</td><td><a href="#reg-6.6-pwm_abnor_state0">PWM_ABNOR_STATE0</a></td><td>pwm 异常状态寄存器 0。</td></tr>
<tr><td>0x504</td><td><a href="#reg-6.6-pwm_abnor_state1">PWM_ABNOR_STATE1</a></td><td>pwm 异常状态寄存器 1。</td></tr>
<tr><td>0x508</td><td><a href="#reg-6.6-pwm_abnor_state_clr0">PWM_ABNOR_STATE_CLR0</a></td><td>pwm 异常状态清除寄存器 0。</td></tr>
<tr><td>0x50C</td><td><a href="#reg-6.6-pwm_abnor_state_clr1">PWM_ABNOR_STATE_CLR1</a></td><td>pwm 异常状态清除寄存器 1。</td></tr>
<tr><td>0x510</td><td><a href="#reg-6.6-pwm_int_mask">PWM_INT_MASK</a></td><td>pwm 中断屏蔽寄存器。</td></tr>
<tr><td>0x0514</td><td><a href="#reg-6.6-pwm_dma_en">PWM_DMA_EN</a></td><td>pwm dma 使能寄存器。</td></tr>
<tr><td>0x0518</td><td><a href="#reg-6.6-pwm_cfg_int_clr0">PWM_CFG_INT_CLR0</a></td><td>pwm 步进模式循环结束中断清除。</td></tr>
</tbody>
</table>

PWM 寄存器偏移地址中变量的取值范围和含义如表 6-13 所示。

表6-11 PWM 寄存器偏移地址变量表

<table>
<thead><tr><th>变量名称</th><th>取值范围</th><th>描述</th></tr></thead>
<tbody>
<tr><td>i</td><td>0~3</td><td>PWM 分组</td></tr>
<tr><td>j</td><td>0~7</td><td>pwm 个数</td></tr>
</tbody>
</table>

<a id="6.6.5"></a>
## 寄存器描述

<a id="reg-6.6-pwm_sel"></a>
### PWM_SEL

PWM_SEL 为 pwm 分组选择控制寄存器。

说明

本 IP 仅支持 8 个 PWM，最大可以支持分组数为 4；

即下述寄存器 i 支持 0~3，j 支持 0~7

Offset Address: 0x000 + 0x10×i Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:16]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>[15:0]</td><td>RW</td><td>pwm_sel_i</td><td>分组 pwm 选择信号,最多支持4 组,每组对应 8bit pwm_sel,每 bit 对应一路 pwm。</td><td>0x0000</td></tr>
</tbody>
</table>

<a id="reg-6.6-pwm_startclrcnt_en"></a>
### PWM_STARTCLRCNT_EN

PWM_STARTCLRCNT_EN 为 pwm 配置生效计数清零使能寄存器。

Offset Address: 0x004 + 0x10×i Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:1]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000000</td></tr>
<tr><td>[0]</td><td>RW</td><td>pwm_startclrcnt_en_i</td><td>star 时刻对 pwm 内部寄存器清零使能。每组对应 1bitpwm_startclrcnt_en。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.6-pwm_start"></a>
### PWM_START

PWM_START 为 PWM 配置生效寄存器。

Offset Address: 0x008 + 0x10×i Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:1]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000000</td></tr>
<tr><td>[0]</td><td>W1_PULSE</td><td>pwm_start_i</td><td>pwm <br>配置生效寄存器。配成 1时,此前赋值的 pwm 相关寄存器生效。每组对应 1bitpwm_start。<br>注意:因逻辑内部模块需要对相关寄存器配置值进行同步和处理,建议该寄存器的配置间隔时间至少 10μs 以上。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.6-pwm_en"></a>
### PWM_EN

PWM_EN 为 PWM 使能寄存器。

Offset Address: 0x0100 + 0x40×j Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:1]</td><td>-</td><td>reserved</td><td>保留</td><td>0x00000000</td></tr>
<tr><td>[0]</td><td>RW</td><td>pwm_en_j</td><td>pwm 功能使能,每路 pwm 对应一个 pwm_en。0: pwm 关闭,pwm_out 输出持续为 0：<br>
1: pwm 使能打开。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.6-pwm_portity"></a>
### PWM_PORTITY

PWM_PORTITY 为 PWM 的输出极性配置寄存器。

Offset Address: 0x0104 + 0x40×j Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:1]</td><td>-</td><td>reserved</td><td>保留</td><td>0x00000000</td></tr>
<tr><td>[0]</td><td>RW</td><td>pwm_poarity_j</td><td>Pwm 输出极性配置：<br>
0: 正向极性<br>
1: 反向极性。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.6-pwm_oen_cfg"></a>
### PWM_OEN_CFG

PWM_OEN_CFG 为 Pwm 无效电平高阻使能配置寄存器。

Offset Address: 0x0108 + 0x40×j Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:1]</td><td>-</td><td>reserved</td><td>保留</td><td>0x00000000</td></tr>
<tr><td>[0]</td><td>RW</td><td>pwm_oen_cfg_j</td><td>Pwm 无效电平高阻使能配置：<br>
0:无效电平跟随极性等其他配置<br>
1:无效电平时 Pwm 输出高阻。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.6-pwm_offset_l"></a>
### PWM_OFFSET_L

PWM_OFFSET_L 为 <a href="#reg-6.6-pwm_start">PWM_START</a> 为 PWM 相位控制计数值低 16bit 寄存器。

Offset Address: 0x010C + 0x40×j Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:16]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>[15:0]</td><td>RW</td><td>pwm_offset_l_j</td><td>Pwm 相位控制计数值的低 16bit,控制 pwm 在一个周期 内为 1 的位置,对 PWM,取 值范围 1~65535。每路 pwm 对应一个 offset 参数。</td><td>0x0000</td></tr>
</tbody>
</table>

<a id="reg-6.6-pwm_offset_h"></a>
### PWM_OFFSET_H

PWM_OFFSET_H 为 <a href="#reg-6.6-pwm_start">PWM_START</a> 为 PWM 相位控制计数值高 16bit 寄存器。

Offset Address: 0x0110 + 0x40×j Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:16]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>[15:0]</td><td>RW</td><td>pwm_offset_h_j</td><td>Pwm 相位控制计数值的高 16bit,在 pwm_offset_l 计满后+1,控制 pwm 在一个周期内为 1 的位置,对 PWM,取值范围 1~65535。<br>每路 pwm 对应一个 offset 参数。</td><td>0x0000</td></tr>
</tbody>
</table>

<a id="reg-6.6-pwm_freq_l"></a>
### PWM_FREQ_L

PWM_FREQ_L 为 PWM_FREQ 为 PWM 频率控制计数值低 16bit 寄存器。

Offset Address: 0x0114 + 0x40×j Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:16]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>[15:0]</td><td>RW</td><td>pwm_freq_l_j</td><td>pwm 频率控制计数值的低 16bit,含义为对 PWM 时钟的分频倍数,取值范围 1~65535。每路 pwm 对应一个 freq 参数。</td><td>0x0000</td></tr>
</tbody>
</table>

<a id="reg-6.6-pwm_freq_h"></a>
### PWM_FREQ_H

PWM_FREQ_H 为 PWM_FREQ 为 PWM 频率控制计数值高 16bit 寄存器。

Offset Address: 0x0118 + 0x40×j Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:16]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>[15:0]</td><td>RW</td><td>pwm_freq_h_j</td><td>pwm 频率控制计数值的高 16bit,在 pwm_freq_l 计满后 +1,含义为对 PWM 时钟的分频倍数,取值范围 1~65535。每路 pwm 对应一个 freq 参数。</td><td>0x0000</td></tr>
</tbody>
</table>

<a id="reg-6.6-pwm_duty_l"></a>
### PWM_DUTY_L

PWM_DUTY_L 为 PWM_DUTY 为 PWM 占空比计数值寄存器。

Offset Address: 0x011C + 0x40×j Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:16]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>[15:0]</td><td>RW</td><td>pwm_duty_l_j</td><td>pwm 占空比控制计数值的低 16bit,取值范围 1~65535。pwm_duty 与 pwm_freq 的比值为占空比。每路 pwm 对应一个 duty 参数。</td><td>0x0000</td></tr>
</tbody>
</table>

<a id="reg-6.6-pwm_duty_h"></a>
### PWM_DUTY_H

PWM_DUTY_H 为 PWM_DUTY 为 PWM 占空比计数值寄存器。

Offset Address: 0x0120 + 0x40×j Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:16]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>[15:0]</td><td>RW</td><td>pwm_duty_h_j</td><td>pwm 占空比控制计数值的高 16bit,在 pwm_duty_l 计满后 +1,取值范围 1~65535。<br>pwm_duty 与 pwm_freq 的比值为占空比。<br>每路 pwm 对应一个 duty 参数。</td><td>0x0000</td></tr>
</tbody>
</table>

<a id="reg-6.6-pwm_periodload_flag"></a>
### PWM_PERIODLOAD_FLAG

PWM_PERIODLOAD_FLAG 为 PWM 周期配置允许标志寄存器。

Offset Address: 0x0124 + 0x40×j Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:1]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000000</td></tr>
<tr><td>[0]</td><td>RO</td><td>pwm_periodload_flag_j</td><td>pwm 平滑切换周期结束,可进行新参数配置标志寄存器。周期计数结束产生。配置 start 清除/或者配置 pwm_cfg_int_clr0 寄存可清除。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.6-pwm_period_val"></a>
### PWM_PERIOD_VAL

PWM_PERIOD_VAL 为 PWM 周期脉冲周期值寄存器。

Offset Address: 0x0128 + 0x40×j Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:16]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>[15:0]</td><td>RW</td><td>pwm_period_val_j</td><td>Pwm 输出脉冲数值,非零时触发 pwm 输出脉冲计数功能,输出脉冲个数大于该值时会加载新参数继续计数。</td><td>0x0000</td></tr>
</tbody>
</table>

<a id="reg-6.6-pwm_periodcnt"></a>
### PWM_PERIODCNT

PWM_PERIODCNT 为 PWM 周期脉冲周期值寄存器。

Offset Address: 0x012C + 0x40×j Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:16]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>[15:0]</td><td>RO</td><td>pwm_periodcnt_j</td><td>Pwm 输出脉冲计数值。</td><td>0x0000</td></tr>
</tbody>
</table>

<a id="reg-6.6-pwm_abnor_state0"></a>
### PWM_ABNOR_STATE0

PWM_ABNOR_STATE0 为 pwm 异常状态寄存器 0。

Offset Address: 0x500 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:16]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>[15:0]</td><td>RO</td><td>pwm_abnor_state0</td><td>bit[15:0]为 Pwm 多路使用配置异常,每路 pwm 对应1bit。</td><td>0x0000</td></tr>
</tbody>
</table>

<a id="reg-6.6-pwm_abnor_state1"></a>
### PWM_ABNOR_STATE1

PWM_ABNOR_STATE1 为 pwm 异常状态寄存器 1。

Offset Address: 0x504 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:16]</td><td>RO</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>[15:0]</td><td>RO</td><td>pwm_abnor_state1</td><td>bit[15:0]为 Pwm 计数值配置异常,每路 pwm 对应 1bit。</td><td>0x0000</td></tr>
</tbody>
</table>

<a id="reg-6.6-pwm_abnor_state_clr0"></a>
### PWM_ABNOR_STATE_CLR0

PWM_ABNOR_STATE_CLR0 为 pwm 异常状态清除寄存器 0。

Offset Address: 0x508 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:16]</td><td>RO</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>[15:0]</td><td>W1_PULSE</td><td>pwm_abnor_state_clr0</td><td>Pwm 异常清除寄存器 0,对应寄存器pwm_abnor_state0。</td><td>0x0000</td></tr>
</tbody>
</table>

<a id="reg-6.6-pwm_abnor_state_clr1"></a>
### PWM_ABNOR_STATE_CLR1

PWM_ABNOR_STATE_CLR1 为 pwm 异常状态清除寄存器 1。

Offset Address: 0x50C Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:16]</td><td>RO</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>[15:0]</td><td>W1_PULSE</td><td>pwm_abnor_state_clr1</td><td>Pwm 异常清除寄存器,对应寄存器pwm_abnor_state1。</td><td>0x0000</td></tr>
</tbody>
</table>

<a id="reg-6.6-pwm_int_mask"></a>
### PWM_INT_MASK

PWM_INT_MASK 为 pwm 中断屏蔽寄存器。

Offset Address: 0x510 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:2]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000000</td></tr>
<tr><td>[1:0]</td><td>RW</td><td>pwm_int_mask</td><td>pwm 中断屏蔽寄存器,0: 屏蔽：<br>
1: 不屏蔽,默认屏蔽:bit[1]: 步进模式一轮周期循环结束中断屏蔽<br>
bit[0]: 异常中断屏蔽。</td><td>0x0</td></tr>
</tbody>
</table>

<a id="reg-6.6-pwm_dma_en"></a>
### PWM_DMA_EN

PWM_DMA_EN 为 pwm dma 使能寄存器。

Offset Address: 0x0514 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:1]</td><td>-</td><td>reserved</td><td>保留。</td><td>0x00000000</td></tr>
<tr><td>[0]</td><td>RW</td><td>pwm_dma_en</td><td>dma 功能使能:0:禁止。1:使能;</td><td>0x0</td></tr>
</tbody>
</table>
<a id="reg-6.6-pwm_cfg_int_clr0"></a>
### PWM_CFG_INT_CLR0

PWM_CFG_INT_CLR0 为 pwm 步进模式循环结束中断清除。

Offset Address: 0x0518 Total Reset Value: 0x0000_0000

<table>
<thead><tr><th>Bits</th><th>Access</th><th>Name</th><th>Description</th><th>Reset</th></tr></thead>
<tbody>
<tr><td>[31:16]</td><td>RO</td><td>reserved</td><td>保留。</td><td>0x0000</td></tr>
<tr><td>[15:0]</td><td>W1_PUL SE</td><td>pwm_cfg_int_clr0</td><td>Pwmpwm 步进模式循环结束中断清除寄存器。对应 pwm_cfg_int 的中断和pwm_periodload_flag_j 寄存器。</td><td>0x0000</td></tr>
</tbody>
</table>

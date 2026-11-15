<a id="6.11"></a>
# ADC

<a id="6.11.1"></a>
## 概述

LSADC 为一款 SAR（Successive Approximations Register） ADC（逐次逼近型数模转换设备），实现将模拟信号转变成数字信号的功能。

<table>
  <thead>
    <tr><th>参数</th><th>最小值</th><th>典型值</th><th>最大值</th><th>单位</th><th>描述</th></tr>
  </thead>
  <tbody>
    <tr><td colspan="6"><strong>Power supply</strong></td></tr>
    <tr><td>AVDD18</td><td>1.71</td><td>1.8</td><td>1.89</td><td>V</td><td>模拟1.8V电压</td></tr>
    <tr><td>AVDD3P3</td><td>2.97</td><td>3.3</td><td>3.63</td><td>V</td><td>模拟3.3V电压</td></tr>
    <tr><td>DVDD</td><td>0.99</td><td>1.1</td><td>1.21</td><td>V</td><td>数字电源电压</td></tr>
    <tr><td colspan="6"><strong>LSADC规范</strong></td></tr>
    <tr><td>Full Scale Intput</td><td>0.3</td><td>-</td><td>3.3</td><td>V</td><td>ADC输入范围</td></tr>
    <tr><td>DNL</td><td>-</td><td>±1.5</td><td>±3</td><td>LSB</td><td>差分非线性</td></tr>
    <tr><td>INL</td><td>-</td><td>±2</td><td>±4</td><td>LSB</td><td>积分非线性</td></tr>
    <tr><td>Resolution</td><td>-</td><td>12</td><td>-</td><td>bit</td><td>精度</td></tr>
    <tr><td rowspan="3">Power Dissipation</td><td>-</td><td>1.3</td><td>1.5</td><td>mA</td><td>工作时功耗</td></tr>
    <tr><td>-</td><td>32</td><td>500</td><td>μA</td><td>关机时功耗（DVDD,AVDD33 及 AVDD18 均在位）</td></tr>
    <tr><td>-</td><td>3.6</td><td>4.32</td><td>μA</td><td>关机时功耗（DVDD 掉电,AVDD33 及 AVDD18 在位）</td></tr>
    <tr><td colspan="6"><strong>Clock规范</strong></td></tr>
    <tr><td>f CLK</td><td>-</td><td>-</td><td>32</td><td>MHz</td><td>输入时钟频率</td></tr>
    <tr><td>Duty-Cycle</td><td>45</td><td>50</td><td>55</td><td>%</td><td>占空比</td></tr>
    <tr><td>f S</td><td>-</td><td>-</td><td>1000</td><td>Ksps</td><td>采样率</td></tr>
  </tbody>
</table>

<a id="6.11.2"></a>
## 功能描述

LSADC 模块具有以下功能特点：

- 输入时钟32MHz，12bit分辨率，单通道采样率最大为 1Msps。

- 共6个通道，支持软件配置 0～5任意通道使能，逻辑按通道编号先低后高发起切换，完成单通道采样并完成平均值滤波后自动进行通道切换。

- 支持128×17bit FIFO用于数据缓存，数据存储格式：高 3bit为通道编号，低14bit为有效数据。

- 支持对ADC采样数据进行平均滤波处理，平均次数支持1（不进行平均）、2、4、8；多通道时，每个通道接收N个数据（平均滤波个数）再切换通道。

- 支持FIFO 水线中断、满中断上报,ADC忙状态、控制器FIFO空满状态查询。

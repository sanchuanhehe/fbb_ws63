<a id="4"></a>
# WiFi / BLE & SLE 系统

<a id="4.1"></a>
## WiFi / BLE & SLE RF

<a id="4.1.1"></a>
### 概述

RF 部分包含 2.4G RX、TX、PLL 三个模块。支持 IEEE 802.11b/g/n/ax 20M 模式。

RF 的电路功能主要包含：

- 集成 TX/RX Switch。

- RX 通路包含 LNA、Mixer、LPF（Low Pass Filter）、VGA（Variable Gain Amplifier）。

- TX通路包含LPF、UPC（UP Converter）、PA（Power Amplifier）。

- 集成PLL/LO（Local Oscillator）通路，为信号通路提供LO。

- 集成了Radar功能

图4-1 RF电路模块架构

<div style="text-align:left; margin:10px 0;"><img src="images/图4-1 RF 电路模块架构.jpg" alt="" style="max-width:100%; width:720px; height:auto;"></div>

<a id="4.1.2"></a>
### 功能描述

WiFi RF 具有以下功能特点：

- RF 电路提供稳定的 LO 信号，支持收发信号的上下变频功能。

- 支持校准功能，包含：RX DC（Direct Current）校准、TX LO Leakage 校准、TX Power 校准、TRX IQ 校准等。

<a id="4.1.3"></a>
### RF 性能

芯片集成 2.4G WiFi/BLE/SLE 收发机，支持雷达功能。除雷达在 RFI 口接收之外，其他的功能都在 RFIO 口测试。

<table>
  <thead>
    <tr>
      <th>参数</th>
      <th>Sub-Item</th>
      <th>最小值</th>
      <th>典型值</th>
      <th>最大值</th>
      <th>单位</th>
      <th>测试条件</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>RF 工作频率段</td>
      <td>-</td>
      <td>2400</td>
      <td>-</td>
      <td>2500</td>
      <td>MHz</td>
      <td>2401MHz以下和2483.5MHz以上频点无法满足无委会辐射要求。<br>信道频率的选择需要遵循协议和法规要求。</td>
    </tr>
    <tr>
      <td rowspan="4">WIF RX 11b 灵敏度</td>
      <td>1 Mbps DSSS</td>
      <td>-</td>
      <td>-99</td>
      <td>-98</td>
      <td>dBm</td>
      <td rowspan="4">PER 8%, 1024 octet PSDU</td>
    </tr>
    <tr>
      <td>2 Mbps DSSS</td>
      <td>-</td>
      <td>-96</td>
      <td>-95</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>5.5 Mbps DSSS/CCK</td>
      <td>-</td>
      <td>-94</td>
      <td>-93</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>11 Mbps DSSS/CCK</td>
      <td>-</td>
      <td>-91</td>
      <td>-90</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td rowspan="8">WIF RX 11g 灵敏度</td>
      <td>BPSK，R=1/2 (6Mbps OFDM)</td>
      <td>-</td>
      <td>-96</td>
      <td>-95</td>
      <td>dBm</td>
      <td rowspan="8">PER 8%, 1000 octets PSDU</td>
    </tr>
    <tr>
      <td>BPSK，R=3/4 (9Mbps OFDM)</td>
      <td>-</td>
      <td>-94</td>
      <td>-92</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>QPSK，R=1/2 (12Mbps OFDM)</td>
      <td>-</td>
      <td>-93</td>
      <td>-91</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>QPSK，R=3/4 (18Mbps OFDM)</td>
      <td>-</td>
      <td>-90</td>
      <td>-89</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>16-QAM，R=1/2 (24Mbps OFDM)</td>
      <td>-</td>
      <td>-87</td>
      <td>-86</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>16-QAM，R=3/4 (36Mbps OFDM)</td>
      <td>-</td>
      <td>-84</td>
      <td>-82</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>16-QAM，R=1/2 (48Mbps OFDM)</td>
      <td>-</td>
      <td>-80</td>
      <td>-78</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>64-QAM，R=3/4 (54Mbps OFDM)</td>
      <td>-</td>
      <td>-78</td>
      <td>-76</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td rowspan="8">WIF RX 11n HT20-MF 灵敏度</td>
      <td>HT20 MCS0</td>
      <td>-</td>
      <td>-95</td>
      <td>-94</td>
      <td>dBm</td>
      <td rowspan="8">BCC Long PER 10%, 4096 octets PSDU</td>
    </tr>
    <tr>
      <td>HT20 MCS1</td>
      <td>-</td>
      <td>-92</td>
      <td>-91</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>HT20 MCS2</td>
      <td>-</td>
      <td>-90</td>
      <td>-88</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>HT20 MCS3</td>
      <td>-</td>
      <td>-87</td>
      <td>-85</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>HT20 MCS4</td>
      <td>-</td>
      <td>-83</td>
      <td>-82</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>HT20 MCS5</td>
      <td>-</td>
      <td>-79</td>
      <td>-78</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>HT20 MCS6</td>
      <td>-</td>
      <td>-77</td>
      <td>-75</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>HT20 MCS7</td>
      <td>-</td>
      <td>-76</td>
      <td>-74</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td rowspan="8">WIF RX 11ac HT40 灵敏度</td>
      <td>HT40 MCS0</td>
      <td>-</td>
      <td>-93</td>
      <td>-92</td>
      <td>dBm</td>
      <td rowspan="8">BCC Long PER 10%, 4096 octets PSDU</td>
    </tr>
    <tr>
      <td>HT40 MCS1</td>
      <td>-</td>
      <td>-90</td>
      <td>-88</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>HT40 MCS2</td>
      <td>-</td>
      <td>-87</td>
      <td>-86</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>HT40 MCS3</td>
      <td>-</td>
      <td>-84</td>
      <td>-83</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>HT40 MCS4</td>
      <td>-</td>
      <td>-81</td>
      <td>-79</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>HT40 MCS5</td>
      <td>-</td>
      <td>-76</td>
      <td>-75</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>HT40 MCS6</td>
      <td>-</td>
      <td>-74</td>
      <td>-73</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>HT40 MCS7</td>
      <td>-</td>
      <td>-73</td>
      <td>-72</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td rowspan="10">WIF RX 11ax HE20 灵敏度</td>
      <td>HE20 MCS0</td>
      <td>-</td>
      <td>-96</td>
      <td>-95</td>
      <td>dBm</td>
      <td rowspan="10">LDPC 4x3.2us PER 10%, 4096 octets PSDU</td>
    </tr>
    <tr>
      <td>HE20 MCS1</td>
      <td>-</td>
      <td>-93</td>
      <td>-92</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>HE20 MCS2</td>
      <td>-</td>
      <td>-91</td>
      <td>-89</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>HE20 MCS3</td>
      <td>-</td>
      <td>-88</td>
      <td>-87</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>HE20 MCS4</td>
      <td>-</td>
      <td>-84</td>
      <td>-83</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>HE20 MCS5</td>
      <td>-</td>
      <td>-80</td>
      <td>-79</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>HE20 MCS6</td>
      <td>-</td>
      <td>-79</td>
      <td>-77</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>HE20 MCS7</td>
      <td>-</td>
      <td>-77</td>
      <td>-75</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>HE20 MCS8</td>
      <td>-</td>
      <td>-73</td>
      <td>-72</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>HE20 MCS9</td>
      <td>-</td>
      <td>-71</td>
      <td>-70</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td rowspan="4">WIFI RX 11b 最大解调电平</td>
      <td>1 Mbps DSSS</td>
      <td>-</td>
      <td>0</td>
      <td>-</td>
      <td>dBm</td>
      <td rowspan="4">PER 8%, 1024 octets PPDU</td>
    </tr>
    <tr>
      <td>2 Mbps DSSS</td>
      <td>-</td>
      <td>0</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>5.5 Mbps DSSS/CCK</td>
      <td>-</td>
      <td>0</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>11 Mbps DSSS/CCK</td>
      <td>-</td>
      <td>0</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>WIFI RX 11g 最大解调电平</td>
      <td>64-QAM，R=3/4 (54Mbps OFDM)</td>
      <td>-</td>
      <td>0</td>
      <td>-</td>
      <td>dBm</td>
      <td>PER 8%, 1000 octets PPDU</td>
    </tr>
    <tr>
      <td>WIFI RX 11n HT20 最大解调电平</td>
      <td>HT20 MCS7</td>
      <td>-</td>
      <td>0</td>
      <td>-</td>
      <td>dBm</td>
      <td>PER 10%, 4096 octets PPDU</td>
    </tr>
    <tr>
      <td rowspan="4">WIFI RX 11b 邻道抑制比</td>
      <td>1Mbps DSSS</td>
      <td>-</td>
      <td>48</td>
      <td>-</td>
      <td>dB</td>
      <td>有用信号-74dBm, PER 8%, 1024 octets PPDU</td>
    </tr>
    <tr>
      <td>2Mbps DSSS</td>
      <td>-</td>
      <td>59</td>
      <td>-</td>
      <td>dB</td>
      <td>有用信号-74dBm, PER 8%, 1024 octets PPDU</td>
    </tr>
    <tr>
      <td>5.5Mbps DSSS/CCK</td>
      <td>-</td>
      <td>44</td>
      <td>-</td>
      <td>dB</td>
      <td>有用信号-70dBm, PER 8%, 1024 octets PPDU</td>
    </tr>
    <tr>
      <td>11Mbps DSSS/CCK</td>
      <td>-</td>
      <td>44</td>
      <td>-</td>
      <td>dB</td>
      <td>有用信号-70dBm, PER 8%, 1024 octets PPDU</td>
    </tr>
    <tr>
      <td rowspan="8">WIFI RX 11g 邻道抑制比</td>
      <td>BPSK，R=1/2 (6Mbps OFDM)</td>
      <td>-</td>
      <td>34</td>
      <td>-</td>
      <td>dB</td>
      <td>有用信号-79dBm, PER 8%, 1000 octets PPDU</td>
    </tr>
    <tr>
      <td>BPSK，R=3/4 (9Mbps OFDM)</td>
      <td>-</td>
      <td>30</td>
      <td>-</td>
      <td>dB</td>
      <td>有用信号-78dBm, PER 8%, 1000 octets PPDU</td>
    </tr>
    <tr>
      <td>QPSK，R=1/2 (12Mbps OFDM)</td>
      <td>-</td>
      <td>31</td>
      <td>-</td>
      <td>dB</td>
      <td>有用信号-76dBm, PER 8%, 1000 octets PPDU</td>
    </tr>
    <tr>
      <td>QPSK，R=3/4 (18Mbps OFDM)</td>
      <td>-</td>
      <td>27</td>
      <td>-</td>
      <td>dB</td>
      <td>有用信号-74dBm, PER 8%, 1000 octets PPDU</td>
    </tr>
    <tr>
      <td>16-QAM，R=1/2 (24Mbps OFDM)</td>
      <td>-</td>
      <td>27</td>
      <td>-</td>
      <td>dB</td>
      <td>有用信号-71dBm, PER 8%, 1000 octets PPDU</td>
    </tr>
    <tr>
      <td>16-QAM，R=3/4 (36Mbps OFDM)</td>
      <td>-</td>
      <td>22</td>
      <td>-</td>
      <td>dB</td>
      <td>有用信号-67dBm, PER 8%, 1000 octets PPDU</td>
    </tr>
    <tr>
      <td>16-QAM，R=1/2 (48Mbps OFDM)</td>
      <td>-</td>
      <td>19</td>
      <td>-</td>
      <td>dB</td>
      <td>有用信号-63dBm, PER 8%, 1000 octets PPDU</td>
    </tr>
    <tr>
      <td>64-QAM，R=3/4 (54Mbps OFDM)</td>
      <td>-</td>
      <td>20</td>
      <td>-</td>
      <td>dB</td>
      <td>有用信号-62dBm, PER 8%, 1000 octets PPDU</td>
    </tr>
    <tr>
      <td rowspan="8">WIFI RX 11n TH20 邻道抑制比</td>
      <td>HT20 MCS0</td>
      <td>-</td>
      <td>30</td>
      <td>-</td>
      <td>dB</td>
      <td>有用信号-79dBm, PER 10%, 4096 octets PPDU</td>
    </tr>
    <tr>
      <td>HT20 MCS1</td>
      <td>-</td>
      <td>29</td>
      <td>-</td>
      <td>dB</td>
      <td>有用信号-76dBm, PER 10%, 4096 octets PPDU</td>
    </tr>
    <tr>
      <td>HT20 MCS2</td>
      <td>-</td>
      <td>26</td>
      <td>-</td>
      <td>dB</td>
      <td>有用信号-74dBm, PER 10%, 4096 octets PPDU</td>
    </tr>
    <tr>
      <td>HT20 MCS3</td>
      <td>-</td>
      <td>24</td>
      <td>-</td>
      <td>dB</td>
      <td>有用信号-71dBm, PER 10%, 4096 octets PPDU</td>
    </tr>
    <tr>
      <td>HT20 MCS4</td>
      <td>-</td>
      <td>19</td>
      <td>-</td>
      <td>dB</td>
      <td>有用信号-67dBm, PER 10%, 4096 octets PPDU</td>
    </tr>
    <tr>
      <td>HT20 MCS5</td>
      <td>-</td>
      <td>17</td>
      <td>-</td>
      <td>dB</td>
      <td>有用信号-63dBm, PER 10%, 4096 octets PPDU</td>
    </tr>
    <tr>
      <td>HT20 MCS6</td>
      <td>-</td>
      <td>15</td>
      <td>-</td>
      <td>dB</td>
      <td>有用信号-62dBm, PER 10%, 4096 octets PPDU</td>
    </tr>
    <tr>
      <td>HT20 MCS7</td>
      <td>-</td>
      <td>13</td>
      <td>-</td>
      <td>dB</td>
      <td>有用信号-61dBm, PER 10%, 4096 octets PPDU</td>
    </tr>
    <tr>
      <td rowspan="8">WIFI RX 11n TH40 邻道抑制比</td>
      <td>HT40 MCS0</td>
      <td>-</td>
      <td>29</td>
      <td>-</td>
      <td>dB</td>
      <td>有用信号-76dBm, PER 10%, 4096 octets PPDU</td>
    </tr>
    <tr>
      <td>HT40 MCS1</td>
      <td>-</td>
      <td>27</td>
      <td>-</td>
      <td>dB</td>
      <td>有用信号-73dBm, PER 10%, 4096 octets PPDU</td>
    </tr>
    <tr>
      <td>HT40 MCS2</td>
      <td>-</td>
      <td>24</td>
      <td>-</td>
      <td>dB</td>
      <td>有用信号-71dBm, PER 10%, 4096 octets PPDU</td>
    </tr>
    <tr>
      <td>HT40 MCS3</td>
      <td>-</td>
      <td>21</td>
      <td>-</td>
      <td>dB</td>
      <td>有用信号-68dBm, PER 10%, 4096 octets PPDU</td>
    </tr>
    <tr>
      <td>HT40 MCS4</td>
      <td>-</td>
      <td>17</td>
      <td>-</td>
      <td>dB</td>
      <td>有用信号-64dBm, PER 10%, 4096 octets PPDU</td>
    </tr>
    <tr>
      <td>HT40 MCS5</td>
      <td>-</td>
      <td>13</td>
      <td>-</td>
      <td>dB</td>
      <td>有用信号-60dBm, PER 10%, 4096 octets PPDU</td>
    </tr>
    <tr>
      <td>HT40 MCS6</td>
      <td>-</td>
      <td>14</td>
      <td>-</td>
      <td>dB</td>
      <td>有用信号-59dBm, PER 10%, 4096 octets PPDU</td>
    </tr>
    <tr>
      <td>HT40 MCS7</td>
      <td>-</td>
      <td>10</td>
      <td>-</td>
      <td>dB</td>
      <td>有用信号-58dBm, PER 10%, 4096 octets PPDU</td>
    </tr>
    <tr>
      <td rowspan="4">WIFI TX11b 最大发射功率</td>
      <td>1Mbps DSSS</td>
      <td>-</td>
      <td>23</td>
      <td>-</td>
      <td>dBm</td>
      <td rowspan="12">Mask margin 大于5dB</td>
    </tr>
    <tr>
      <td>2Mbps DSSS</td>
      <td>-</td>
      <td>23</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>5.5Mbps DSSS/CCK</td>
      <td>-</td>
      <td>23</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>11Mbps DSSS/CCK</td>
      <td>-</td>
      <td>23</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td rowspan="8">WIFI TX11g 最大发射功率</td>
      <td>BPSK，R=1/2 (6Mbps OFDM)</td>
      <td>-</td>
      <td>21</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>BPSK，R=3/4 (9Mbps OFDM)</td>
      <td>-</td>
      <td>21</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>QPSK，R=1/2 (12Mbps OFDM)</td>
      <td>-</td>
      <td>21</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>QPSK，R=3/4 (18Mbps OFDM)</td>
      <td>-</td>
      <td>21</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>16-QAM，R=1/2 (24Mbps OFDM)</td>
      <td>-</td>
      <td>21</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>16-QAM，R=3/4 (36Mbps OFDM)</td>
      <td>-</td>
      <td>21</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>16-QAM，R=1/2 (48Mbps OFDM)</td>
      <td>-</td>
      <td>20</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>64-QAM，R=3/4 (54Mbps OFDM)</td>
      <td>-</td>
      <td>19</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td rowspan="8">WIFI TX HT20-MF 最大发射功率</td>
      <td>MCS0</td>
      <td>-</td>
      <td>20</td>
      <td>-</td>
      <td>dBm</td>
      <td rowspan="8">BCC long GI</td>
    </tr>
    <tr>
      <td>MCS1</td>
      <td>-</td>
      <td>20</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>MCS2</td>
      <td>-</td>
      <td>20</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>MCS3</td>
      <td>-</td>
      <td>19</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>MCS4</td>
      <td>-</td>
      <td>19</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>MCS5</td>
      <td>-</td>
      <td>19</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>MCS6</td>
      <td>-</td>
      <td>19</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>MCS7</td>
      <td>-</td>
      <td>18</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td rowspan="8">WIFI TX HT40-MF 最大发射功率</td>
      <td>MCS0</td>
      <td>-</td>
      <td>20</td>
      <td>-</td>
      <td>dBm</td>
      <td rowspan="8">BCC long GI</td>
    </tr>
    <tr>
      <td>MCS1</td>
      <td>-</td>
      <td>20</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>MCS2</td>
      <td>-</td>
      <td>20</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>MCS3</td>
      <td>-</td>
      <td>19</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>MCS4</td>
      <td>-</td>
      <td>19</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>MCS5</td>
      <td>-</td>
      <td>19</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>MCS6</td>
      <td>-</td>
      <td>19</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>MCS7</td>
      <td>-</td>
      <td>18</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td rowspan="10">WIFI TX HE20 最大发射功率</td>
      <td>MCS0</td>
      <td>-</td>
      <td>20</td>
      <td>-</td>
      <td>dBm</td>
      <td rowspan="10">LDPC 4x3.2us</td>
    </tr>
    <tr>
      <td>MCS1</td>
      <td>-</td>
      <td>20</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>MCS2</td>
      <td>-</td>
      <td>20</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>MCS3</td>
      <td>-</td>
      <td>19</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>MCS4</td>
      <td>-</td>
      <td>19</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>MCS5</td>
      <td>-</td>
      <td>19</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>MCS6</td>
      <td>-</td>
      <td>19</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>MCS7</td>
      <td>-</td>
      <td>18</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>MCS8</td>
      <td>-</td>
      <td>17</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>MCS9</td>
      <td>-</td>
      <td>15</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td rowspan="6">LE RX 灵敏度</td>
      <td>LE 1M</td>
      <td>-</td>
      <td>-99</td>
      <td>-98</td>
      <td>dBm</td>
      <td rowspan="6">PER< 30%</td>
    </tr>
    <tr>
      <td>LE 2M</td>
      <td>-</td>
      <td>-96</td>
      <td>-95</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>LR S2 255byte</td>
      <td>-</td>
      <td>-100</td>
      <td>-99</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>LR S8 255byte</td>
      <td>-</td>
      <td>-105</td>
      <td>-103</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>LR S2 37byte</td>
      <td>-</td>
      <td>-101</td>
      <td>-100</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>LR S8 37byte</td>
      <td>-</td>
      <td>-105</td>
      <td>-104</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td rowspan="15">SLE RX 灵敏度</td>
      <td>SLE_1M GFSK</td>
      <td>-</td>
      <td>-99</td>
      <td>-97</td>
      <td>dBm</td>
      <td rowspan="15">PER <10%</td>
    </tr>
    <tr>
      <td>SLE_2M GFSK</td>
      <td>-</td>
      <td>-96</td>
      <td>-94</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>SLE_4M GFSK</td>
      <td>-</td>
      <td>-93</td>
      <td>-91</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>1M QPSK shortHD pilot16:1 polar3/4</td>
      <td>-</td>
      <td>-101</td>
      <td>-100</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>2M QPSK shortHD pilot16:1 polar3/4</td>
      <td>-</td>
      <td>-98</td>
      <td>-96</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>4M QPSK shortHD pilot16:1 polar3/4</td>
      <td>-</td>
      <td>-95</td>
      <td>-93</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>1M 8PSK shortHD pilot16:1 polar3/4</td>
      <td>-</td>
      <td>-96</td>
      <td>-94</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>2M 8PSK shortHD pilot16:1 polar3/4</td>
      <td>-</td>
      <td>-93</td>
      <td>-91</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>4M 8PSK shortHD pilot16:1 polar3/4</td>
      <td>-</td>
      <td>-90</td>
      <td>-88</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>1M QPSK shortHD pilot_no polar1/1</td>
      <td>-</td>
      <td>-96</td>
      <td>-94</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>2M QPSK shortHD pilot_no polar1/1</td>
      <td>-</td>
      <td>-93</td>
      <td>-92</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>4M QPSK shortHD pilot_no polar1/1</td>
      <td>-</td>
      <td>-89</td>
      <td>-88</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>1M 8PSK shortHD pilot_no polar1/1</td>
      <td>-</td>
      <td>-90</td>
      <td>-88</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>2M 8PSK shortHD pilot_no polar1/1</td>
      <td>-</td>
      <td>-87</td>
      <td>-86</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>4M 8PSK shortHD pilot_no polar1/1</td>
      <td>-</td>
      <td>-82</td>
      <td>-81</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td rowspan="4">LE TX 最大发射功率</td>
      <td>LE 1M</td>
      <td>-</td>
      <td>20</td>
      <td>-</td>
      <td>dBm</td>
      <td rowspan="4">满足EVM和ACP要求</td>
    </tr>
    <tr>
      <td>LE 2M</td>
      <td>-</td>
      <td>20</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>LR S2 500K</td>
      <td>-</td>
      <td>20</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>LR S8 125K</td>
      <td>-</td>
      <td>20</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td rowspan="15">SLE TX 最大发射功率</td>
      <td>SLE_1M GFSK</td>
      <td>-</td>
      <td>20</td>
      <td>-</td>
      <td>dBm</td>
      <td rowspan="15"></td>
    </tr>
    <tr>
      <td>SLE_2M GFSK</td>
      <td>-</td>
      <td>20</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>SLE_4M GFSK</td>
      <td>-</td>
      <td>20</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>1M QPSK shortHD pilot16:1 polar3/4</td>
      <td>-</td>
      <td>14</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>2M QPSK shortHD pilot16:1 polar3/4</td>
      <td>-</td>
      <td>14</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>4M QPSK shortHD pilot16:1 polar3/4</td>
      <td>-</td>
      <td>14</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>1M 8PSK shortHD pilot16:1 polar3/4</td>
      <td>-</td>
      <td>14</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>2M 8PSK shortHD pilot16:1 polar3/4</td>
      <td>-</td>
      <td>14</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>4M 8PSK shortHD pilot16:1 polar3/4</td>
      <td>-</td>
      <td>14</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>1M QPSK shortHD pilot_no polar1/1</td>
      <td>-</td>
      <td>14</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>2M QPSK shortHD pilot_no polar1/1</td>
      <td>-</td>
      <td>14</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>4M QPSK shortHD pilot_no polar1/1</td>
      <td>-</td>
      <td>14</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>1M 8PSK shortHD pilot_no polar1/1</td>
      <td>-</td>
      <td>14</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>2M 8PSK shortHD pilot_no polar1/1</td>
      <td>-</td>
      <td>14</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>4M 8PSK shortHD pilot_no polar1/1</td>
      <td>-</td>
      <td>14</td>
      <td>-</td>
      <td>dBm</td>
    </tr>
    <tr>
      <td>TX输出功率精度</td>
      <td>-</td>
      <td>-2</td>
      <td>-</td>
      <td>2</td>
      <td>dB</td>
      <td>-</td>
    </tr>
    <tr>
      <td>TX输出功率分辨率</td>
      <td>-</td>
      <td>-</td>
      <td>1</td>
      <td>-</td>
      <td>dB</td>
      <td>BT只能发送固定功率</td>
    </tr>
  </tbody>
</table>

**说明:**

以上数据仿真条件为 VBAT=3.3V。

<a id="4.2"></a>
## WiFi/BLE & SLE ABB

<a id="4.2.1"></a>
### 概述

ABB IP 用于 Connectivity SoC 芯片，是支持 WiFi 802.11b/g/n/ax（2.4G mode）系统的模拟数字接口模块，根据功能分为以下 2 个功能模块：

- WiFi IQ-ADC

- WiFi IQ DAC

完成发送时的数模转换及接收时的模数转换功能。

WiFi ADC（1个通道，通道有IQ）、WiFi DAC（1个通道，通道有IQ），以及时钟buf模块和LDO，共同包括在WS63 WL ABB 中，时钟buf和LDO不作为独立功能模块，不在行为模型中独立体现。

图4-2 ABB模块组成

<div style="text-align:left; margin:10px 0;"><img src="images/图4-2 ABB 模块组成.jpg" alt="" style="max-width:100%; width:720px; height:auto;"></div>
注：WADC (WiFi Analog Digital Converter), WDAC (WiFi Digital Analog Converter)。

<a id="4.2.2"></a>
### 功能描述

ABB IP 具有以下功能特点：

- 提供 1 路 WiFi IQ ADC、1 路 WiFi IQ DAC。

- 支持 WiFi 802.11b/g/n (2.4G mode)。

<a id="4.2.3"></a>
### 工作方式

业务模式寄存器配置为固定一次性配置，业务工作期间无需重复配置，仅在逻辑电源掉电重新上电时需要重新配置；校正算法在温度、电压漂移下受影响，如果温度、电压变化较大，需要重新运行算法并刷新寄存器，除此情况之外无需重复配置。

校准包括：

- WLAN（Wireless Local Area Network）的 ADC 比较器校准。

- WLAN DC Offset 校准。

- WLAN 电容校准。

校准步骤：

步骤 1 比较器校准。

步骤 2 电容校准。

步骤 3 DC Offset 校准。

----结束

<a id="4.3"></a>
## WiFi PHY

<a id="4.3.1"></a>
### 概述

WLAN PHY 实现 802.11 协议定义的物理层功能，包括：

- 802.11b 协议定义的 DSSS、CCK 调制解调。

- 802.11g、802.11n、802.11ax协议定义的OFDM调制解调包括发送的加扰、交织、编码、OFDM调制等处理；接收方向OFDM解调、Viterbi译码、解交织、解扰等处理；同时实现AGC（Automatic Gain Control）、CCA（Clear Channel Assessment）、RSSI(Receive Signal Strength Indicator)功能。

- 实现RF/ABB校准功能。

<a id="4.3.2"></a>
### 功能描述

WiFi PHY 具有以下功能特点：

- 支持 IEEE802.11b/g/n/ax 无线局域网络通信协议，其中 ax 支持 su/ersu 的收发、tb 帧的发送、mu 帧的接收。

- 支持 802.11b 的 DSSS、CCK，802.11g/n/ax 的 BCC(Binary Convolutional Code) 编解码，802.11n/ax 的 LDPC(Low Density Parity Check)的编码。

- 支持 2.4G Band，802.11b/g/n/ax 支持 20MHz 信号带宽，802.11n 支持 40MHz 信号带宽，802.11ax（tb/mu）支持 20MHz-only 信号带宽。

- 支持 4 选 1 多天线分集，最大支持 1 个空间流；802.11n/ax 支持 STBC(Space-Time Block Code)接收；802.11n/ax 支持 4x1 TxBF；802.11ax 最多支持 4 用户识别并支持配置其中任一个用户接收。

- 支持雷达感知(Radar Sensing)。

- 支持 GLP(Green-tooth Low-energy Positioning)辅助同步。

- 支持 PSD(Power Spectral Desity)上报。

- 支持 CSI(Channel State Infomation)上报。

- 支持ABB/RF校准功能。

<a id="4.3.3"></a>
### 工作方式

PHY 模式初始化配置支持物理带宽为 20MHz 的 WiFi 业务收发，在业务模式下可以根据与 AP 的交互完成不同物理带宽的切换，也可以再测试模式下配置不同的物理带宽用于性能测试或者问题定位。PHY 会根据不同的带宽，自适应驱动配置，完成基带数据发送或者接收。根据上层业务的需求，PHY 主要支持以下几种工作模式。

#### 校准模式

在上电时对 ABB/RF 进行离线校准，RF 配置校准模式，复用 PHY 中部分逻辑通路进行校准计算，校准项主要包括 TXDC、TXIQ、TXPWR、RXDC、RXIQ、RXRC 等，校准完成后将校准结果存储在 PHY 中对应配置寄存器中，在测试或者业务模式下自动线控调用，优化 ABB/RF 性能。

#### 测试模式

测试模式，主要是常发、常收测试。其中常发测试主要是指基于描述符后来者配置寄存器下发 TXVECTOR 来启动 RF 线控及 PHY 内部编码调制等，最终将数字 DAC 数据送给 ABB/RF 输出，多帧连续输出，配合仪器用于测试发送时各种性能指标或者基本问题定位；常收测试主要将进过 ABB/RF 的数字 ADC 数据送给 PHY 进行解调，并将解调后的数据送给 MAC 进行 FCS(Frame Check Sequence)校验，来统计接收数据的是否正确，多帧连续输入，配合仪器用于测试接收时各种性能指标或者基本问题定位。

#### 业务模式

业务模式下，PHY 受上层 MAC 主控，与 AP 进行收发通信。业务发送时，PHY 接收来自 MAC 的 TXVECTOR，启动 RF 线控及 PHY 编码调制等，最终将数字 DAC 送给 ABB/RF 数采。业务接收时，PHY 将来自 ABB/RF 的数字 ADC 数据经过 AGC 控制后进行解调译码，并将解析后数据送给上层 MAC 进一步处理。

#### 雷达感知模式

雷达感知模式下，PHY 受上层 MAC 主控，根据业务需求启动雷达感知使能，PHY 会从 PKTRAM 中读取 DAC 采样率下的雷达数据，经过校准后送给 ABB/RF；同时会将经过 ABB/RF 的数字 ADC 数据经过固定增益控制后的 ADC 数据进行校准处理，然后校准后的 ADC 数据储存在 PKTRAM 中，并给出中断信息，CPU 收到中断信息后对存储在 PKTRAM 中的雷达数据做进一步处理，满足雷达感知业务需求。

#### PSD 模式

PSD 模式下，PHY 将来自 ABB/RF 的数字 ADC 数据经过 AGC 控制后进行 FFT 计算等统计，最终将 PSD 存储在内部存储空间，并以中断形式上报 CPU，CPU 收到中断后顺序将 PSD 信息读出。可以通过配置不同信道多次统计，收集数据用于再次开发利用。

#### GLP 联合测距模式

支持与 GLP 联合测距，WiFi 业务下生成发送/接收初始化同步脉冲，给 GLP 提供精确的脉冲定时以及频偏估计值上报。

<a id="4.4"></a>
## WiFi MAC

<a id="4.4.1"></a>
### 概述

DBB（Digital Baseband） MAC 主要完成 WiFi MAC 层的硬件处理，包括信道接入、组解帧、数据收发、加解密、节能控制等功能。

<a id="4.4.2"></a>
### 功能描述

WiFi MAC 具有以下功能特点：

- 支持 IEEE802.11b/g/n/ax 无线局域网络通信协议。

- 支持STA模式和AP模式。

- 支持 2.4G Band、802.11b/a/g 20MHz；802.11n 20MHz/40MHz；802.11ax 20MHz。最大支持 1 流、1 天线。

- 支持 WPA、WPA2、AES 加解密。

- 支持WPS2.0。

- 支持协议低功耗：PSM（Power Saving Mode）、UAPSD（Unscheduled Automatic Power Save Delivery）、P2P（Peer-to-Peer）Power Save。

<a id="4.4.3"></a>
### 工作方式

<a id="4.4.3.1"></a>
#### AP 模式

在一个基础 BSS（Basic Service Set）网络中提供所有接入点的基本功能，包括：

- 发送 Beacon 帧声明 BSS 的存在和能力。

- 为BSS中的客户端提供无线关联和认证服务。

- 管理 BSS 网络中与之关联的客户端。

- 芯片支持 1 个 AP。

<a id="4.4.3.2"></a>
#### STA 模式

在一个基础 BSS 网络中提供扫描发现网络、加入网络并管理与 AP 的连接以提供数据收发服务的功能。

芯片支持 2 个 STA。

<a id="4.4.3.3"></a>
#### Monitor 模式

芯片进入 Monitor 模式，实现网卡的功能，MAC 将接收到的所有帧上报软件。

<a id="4.4.3.4"></a>
#### AP 与 STA 共存

芯片支持 1 个 AP 和 1 个 STA 同时工作。

芯片支持 2.4G 下 AP/STA 在相同或不同信道的并发，分别对应同频共存和异频共存。

约束：STA 上电后会进行信道扫描，导致信道切换，因此开启 AP/STA 动态共存时，需要先创建 STA，再创建 AP，否则将会影响 AP 的工作信道。

<a id="4.4.3.5"></a>
#### CSI 模式

CSI（Channel State Information）模式支持将 PHY 上报的信道状态信息（CSI）过滤后上报软件：

- 支持 11g/11n/11ax 的 CSI 信息上报，不支持 11b。

- 支持对将提取 CSI 的帧进行源地址过滤，源地址过滤列表（白名单）共 6 个（关联设备使用 LUT（Lookup Table）中的地址内容）。

- 支持 6 个 CSI 采样周期，CSI 采样周期与白名单绑定，一个白名单对应一个采集周期。

- 支持白名单、采样周期、特定帧类型等匹配条件，满足匹配条件才上报 CSI 信息。

- 支持带宽（20MHz、40MHz）、帧格式（NON-HT、HT-MF）、RSSI（Received Signal Strength Indicator）、SNR（Signal Noise Ratio）随 CSI 信息上报（不支持 STBC 帧上报），上报 L-LTF H 矩阵数据。

<a id="4.5"></a>
## BLE/SLE

<a id="4.5.1"></a>
### 概述

BLE/SLE 部分包含 MODEM 和 MAC，MODEM 实现调制解调功能，MAC 部分实现调度、收发控制和组包解包功能。

<a id="4.5.2"></a>
### 功能描述

BLE 主要特性如表 4-1 所示。

表4-1 BLE主要特性

| 标题 | 描述 |
| --- | --- |
| 蓝牙协议版本 | 支持蓝牙核心规范 5.4。 |
| 蓝牙模式 | 仅支持 Low Energy only。 |
| BT4.0 特性 | 支持蓝牙规范 4.0 特性。 |
| Low Energy Physical | Low Energy Physical Layer。 |
| Low Energy Link | Low Energy Link Layer。 |
| Enhancements to HCI for Low Energy | 支持 BLE 模式相关的 HCI 功能。 |
| Low Energy Direct Test Mode | 支持 BLE 直接测试模式。 |
| AES Encryption | 支持对数据包进行 AES 加解密。 |
| BT4.1 特性 | 支持蓝牙规范 4.1 特性。 |
| Low duty cycle directed advertising | 支持低占空比定向广播。 |
| LE Dual mode topology | BLE 设备可同时为 master 和 Slave。 |
| Fast Advertising interval | 支持高占空比定向广播。 |
| LE privacy v1.1 | 支持 LE 隐私策略 v1.1。 |
| LE Ping | 支持 LE Ping 功能。 |
| Private address changes | 支持私有地址变更功能。 |
| BT4.2 特性 | 支持蓝牙规范 4.2 特性。 |
| LE Data Packet Length Extension | 支持数据包长度扩展，最大可支持 250Byte。 |
| LE Secure Connections | 支持低功耗蓝牙安全连接。 |
| Link Layer privacy | 支持低功耗蓝牙链路层隐私策略。 |
| Link Layer Extended Scanner Filter policies | 支持扩展扫描过滤机制。 |
| BT5.0 特性 | 支持蓝牙规范 5.0 特性。 |
| 2 Msym/s PHY for LE | 支持 2M 传输速率。 |
| LE Channel Selection Algorithm #2 | 支持自适应跳频算法 2。 |
| High Duty Cycle Non-Connectable Advertising | 支持高占空比非连接广播。 |
| LE Long Range | 支持 BLE Long Range。 |
| BT5.2 特性 | 支持蓝牙规范 5.2 特性。 |
| BLE Power Control | 支持功率控制功能。 |
| 连接个数 | 支持 4 条 BLE 连接（可选 8 条）。 |
| BLE dual mode | BLE 设备支持的角色。 |
| Master | 支持 LE 的 Master role。 |
| Slave | 支持 LE 的 Slave Role。 |
| PHY Update | 支持选择 PHY 信道。 |
| Data Length Update | 支持选择数据包的长度。 |
| 白名单个数 | 白名单个数最大支持 8 条。 |
| BLE RPA 列表 | Device 能支持的最大的 BLE RPA 名单数目到 4 个。 |
| RPA 功能 | 支持私有可解析地址功能。<br>广播、扫描、Init 支持 RPA 功能。 |
| RPA 名单个数 | 最大支持 4 条 RPA 条目。 |
| 快速信道干扰检测 | 支持业务间隙扫描蓝牙信道所有频点，以判断空口的干扰程度。 |
| Channel map update | 支持信道位图更新功能。 |
| 信道扫描 | 支持扫描所有的蓝牙信道，根据扫描结果评估信道干扰程度。 |

SLE 主要特性如表 4-2 所示。

表4-2 SLE主要特性

| 标题 | 描述 |
| --- | --- |
| SLE 协议 1.0 | 支持 SLE1.0 协议核心规范内容。 |
| SLE 链路管理 | 支持 SLE 链路管理。 |
| 时隙调度 | 支持系统基础时隙按 125μs 调度。 |
| SLE 广播业务 | 支持 SLE 广播链路业务。 |
| Channel Scan 业务 | 支持对通信信道进行扫描，上报信道 rssi。 |
| SLE 帧格式 | 支持 SLE1.0 协议无线帧类型。<br>支持 SLE1.0 协议无线帧类型 1。<br>支持 SLE1.0 协议无线帧类型 2。 |
| 白名单个数 | 白名单个数最大支持 8 条。 |
| SLE 调制模式和物理层带宽 | 支持调制解调带宽 1M/2M/4M 三种速率。<br>支持 SLE 调制方式 GFSK-1M/GFSK-2M/GFSK-4M 三种速率。<br>支持 QPSK 调制方式 QPSK-1M/QPSK-2M/QPSK-4M 三种速率。<br>支持 8PSK 调制方式 8PSK-1M/8PSK-2M/8PSK-4M 三种速率。 |
| SLE 码率 | 支持帧类型 2 下，QPSK 调制 Polar 码率为 3/4。<br>支持帧类型 2 下，8PSK 调制 Polar 码率为 3/4、1。 |
| SLE 导频插值比例 | 支持帧类型 2 下，数据信息符号导频比为 4:1、16:1。 |
| 信道干扰检测 | 支持信道扫描业务进行干扰检测。 |
| 连接个数 | 支持默认最大支持 4 条 SLE link（可选 8 条，与 BLE 共享连接数）。 |

<a id="4.5.3"></a>
### 工作方式

<a id="4.5.3.1"></a>
#### 中断

BLE&SLE CPU 只有 2 个主中断源 ble_irq/SLE_irq，每个中断源由多个子中断源汇和而成，CPU 响应相应中断源，通过查询中断状态寄存器来查询子中断类型。

<a id="4.5.3.2"></a>
#### 加密

BLE 支持 AES-128 加密方式，SLE 支持 SM4 和 AES-128 加密。

<a id="4.6"></a>
## 雷达特性

<a id="4.6.1"></a>
### 概述

雷达模块通过芯片内置的雷达信号发送、接收以及信号处理模块，实现近距离的运动人体感知功能。

<a id="4.6.2"></a>
### 功能描述

芯片可同时提供“靠近检测”和“存在检测”两种感知能力

<a id="4.6.2.1"></a>
#### 靠近检测

**须知:**

指标是在公版模组上测试得到，在其他自研模组上，天线设计需要满足天线隔离度、天线方向性和天线增益等要求，否则性能会受到影响。

可以检测距离芯片 2.5m 范围内运动人体，并分[0, 1.5m]、[1.5m, 2.5m]两档上报，上报延时＜1s，准确率＞99%

<a id="4.6.2.2"></a>
#### 存在检测

**须知:**

1. 上述指标是在公版模组上测试得到，在其他自研模组上，天线设计需要满足天线隔离度、天线方向性和天线增益等要求，否则性能会受到影响

2. 芯片提供一定抗非人体干扰能力，比如晃动的绿植、风吹窗帘等，但是要占用额外的内存和算力，识别延时＜5s，准确率＞95%。

可以检测距离芯片 6m 内是否存在运动人体，上报延时＜2s，准确率＞99%。

<a id="4.6.3"></a>
### 工作方式

雷达模块与 WIFI 业务之间通过分时共存，雷达信号本身有如下特点：

- 雷达信号间隔 5ms。

- 雷达信号占空比＜2%。

- 最快每 0.32s 上报一次检测结果。

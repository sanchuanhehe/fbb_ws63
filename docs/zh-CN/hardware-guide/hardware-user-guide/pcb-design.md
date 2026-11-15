# PCB 设计建议

## 叠层和布局

WS63 系列芯片封装为 QFN40，规格大小为 5mm × 5 mm ，PCB 支持 2/4 层板，支持器件单面贴设计。

- 两层板分层设计建议：

    \- TOP 层：信号走线，信号线和电源线尽量走 TOP 层。

    \- BOTTOM 层：地平面层，尽量保持地平面的完整。

- 四层板分层设计建议：

    \- TOP 层：信号走线，信号线尽量走 TOP 层。

    \- 内一层：地平面层，保持一个完整的地平面层。

    \- 内二层：电源平面层，电源走线尽量走第三层，且电源之间需要用地隔开。

    \- BOTTOM 层：可以走少量的信号线，尽量保持 BOTTOM 层为一个完整的地平面层。

- PCB 设计注意事项:

    \- 推荐 PCB 板厚 On Board 方案一般≥1mm，防止翘曲，过孔 8/18 mil。

    \- PCB 典型材料 FR4 介电常数为 4.0~4.3，表层铜箔厚度建议为 1.2mil (0.5oz+plating)，PCB 板厚度一般≥1.0mm，典型值为 1.2mm，可选用 1.0mm。

    \- 2 层板设计中，EPAD 和外部地用细线连通，改善 RF 回流。

常用的叠层设计和阻抗控制可参考表 4-1。

表4-1 2 层板 1.0mm 参考叠层信息

<table><tr><th>层标识</th><th>设计要求层叠图示</th><th>设计要求介质厚度(oz/mil)</th><th>PCB厂家设计调整介质厚度(oz/mil)</th><th>PCB厂家设计调整层叠图示</th></tr><tr><td rowspan="2">Art 1</td><td colspan="2">0.5oz+plating</td><td colspan="2">0.5oz+plating</td></tr><tr><td>CORE</td><td>35.4</td><td>34.06</td><td>CORE</td></tr><tr><td>Art 2</td><td colspan="2">0.5oz+plating</td><td colspan="2">0.5oz+plating</td></tr><tr><td>板厚</td><td colspan="2">客户设计板厚:1.0±0.10 mm</td><td colspan="2">厂家理论板厚:1.0±0.10 mm</td></tr></table>

表4-2 单线线宽、阻抗、参考层控制信息参考

<table><tr><th>层标识</th><th>设计线宽</th><th>设计阻抗</th><th>调整线宽</th><th>调整阻抗</th><th>参考层</th></tr><tr><td>Art 1</td><td>5/19/5(到地距离/线宽/到地距离)</td><td>50±10%</td><td>5/19/5(到地距离/线宽/到地距离)</td><td>50±10%</td><td>L1&amp;L2</td></tr></table>

注：线宽的计量单位为 mil，阻抗的计量单位为 Ω 。

表4-3 4 层板 1.2mm 参考叠层信息

<table><tr><th>层标识</th><th>设计要求层叠图示</th><th>设计要求介质厚度(oz/mil)</th><th>PCB厂家设计调整介质厚度(oz/mil)</th><th>PCB厂家设计调整层叠图示</th></tr><tr><td rowspan="2">Art 1</td><td colspan="2">0.5oz+plating</td><td colspan="2">0.5oz+plating</td></tr><tr><td>PP</td><td>8.2</td><td>10.88</td><td>PP</td></tr><tr><td rowspan="2">Art 2</td><td colspan="2">1oz</td><td colspan="2">1oz</td></tr><tr><td>CORE</td><td>23.8</td><td>18</td><td>CORE</td></tr><tr><td rowspan="2">Art 3</td><td colspan="2">1oz</td><td colspan="2">1oz</td></tr><tr><td>PP</td><td>8.2</td><td>10.88</td><td>PP</td></tr><tr><td>Art 4</td><td colspan="2">0.5oz+plating</td><td colspan="2">0.5oz+plating</td></tr><tr><td>板厚</td><td colspan="2">客户设计板厚:1.2±0.12 mm</td><td colspan="2">厂家理论板厚:1.2±0.12 mm</td></tr></table>

表4-4 单线线宽、阻抗、参考层控制信息

| 信号层 | 接地层    | 阻抗目标  | 阻抗公差 | 设计线宽(mil) | 距铜(mil) |
| ------ | --------- | --------- | -------- | ------------- | --------- |
| L1     | L1&amp;L2 | L1&amp;L2 | 10%      | 11            | 6         |

## Fanout 封装设计建议

四层板 Fanout 如图 4-1 所示。

图4-1 PCB 四层板 Fanout 参考设计  
![](images/0c45d8a99701a7cbd01f53b6015e5f00511e3f609d11d8b5f454da055e321c5e.jpg)

其中：

- 黄色：AVDD33_RF0，AVDD33_RF1，AVDD33_RF2，AVDD33_0，AVDD33_1，VDD33_OUT

- 绿色：IOLDO18，VDD_DIG

- 紫色: PWR_ON

- 深蓝色：DVDD3318

- 淡蓝色：RF_ANT，RFIO_ANT

- 橙色：BUCK_IN，BUCK_OUT

- 粉色：VBAT_IN

## PCB 布局

应用支持 On Board 和模组两种方案。

- On Board 方案

    \- 支持 2 层板设计。

    \- On Board 可双面贴片，空间允许可以选择 0402 封装，空间不足可选择 0201 封装（inch）。

- 模组

    \- 建议用 2 层板。

    \- 贴片器件建议用 0201 封装 (inch)。

PCB 设计以模组两层板为例，参考设计如图 4-2 和图 4-3 所示。

图4-2 LDO 供电方案模组 PCB 布局参考  
![](images/dfd5b1810547f04f2f67fb4042026be383ffd8c73b4f07856ddf98012aab1f59.jpg)  
TOP VIEW

![](images/336414980af0d90fcef563683700ddd32fe941ae841b248a466e042cfccf6d25.jpg)  
BOTTOM VIEW

其中：

- 黄色：AVDD33_RF0，AVDD33_RF1，AVDD33_RF2，AVDD33_0，AVDD33_1，VDD33_OUT，DVDD3318，BUCK_IN，VBAT_IN

- 绿色：IOLDO18，VDD_DIG

- 紫色: PWR_ON

- 深蓝色：RFIO_ANT

- 橙色: BUCK_OUT

- 白色：XIN，XOUT

图4-3 BUCK 供电方案模组 PCB 布局参考  
![](images/3edc89125146b1eb895344bfa0e7b1a1c623784988df95237e0157ff61b34cee.jpg)  
TOP VIEW

![](images/aa048ba84840930b4ec197ff05f3e9a89be9746b0b95be01afb766cc836e272a.jpg)  
BOTTOM VIEW

其中：

- 黄色：AVDD33_RF0，AVDD33_RF1，AVDD33_RF2，AVDD33_0，AVDD33_1，VDD33_OUT，DVDD3318

- 绿色：IOLDO18，VDD_DIG

- 紫色：PWR_ON

- 深蓝色：RFIO_ANT

- 橙色：BUCK_IN，BUCK_OUT

- 粉色：VBAT_IN;

!!! note "说明"

    通常RF器件布局比较紧凑，这样会导致近芯片侧π型LC滤波网络的两个接地电容的接地过孔靠的比较近，这样会影响到RF的谐波抑制性能，建议两个接地电容分布在RF走线的两边，这样可以提高RF电路的谐波抑制效果。

## 电源

- 电源走线宽度需要满足“3.2.2 电源规格”中的建议电流值，建议按照100mA/4mil的通流能力来设计。

- 所有电源走线需先经过滤波电容再到对应的芯片管脚，滤波电容靠近被滤波芯片管脚放置。电容就近接地，避免长走线引入电感效应。

- RF 电源对外干扰较大，需远离模拟电源和敏感信号。RF 电源各个管脚需要有各自的滤波电容，经过各自的滤波电容后分别走线，避免振荡问题。

- AVDD33 电源走线支持串行走线，但星型走线可以带来更好的性能，图 4-4 黄色走线即为星形走线，深蓝色为 RFIO_ANT 走线。

AVDD33 电源走线（黄色）、DVDD3318、IOLDO18 及 VDD_DIG 上的滤波电容摆放位置如图 4-4 所示。

图4-4 模组两层板电源布线参考  
![](images/6d085551c27d43caacdb899ea9068f9bbaad8e0058f17324736d4290314bd52b.jpg)

## RF 布线指导

- RF 走线控制阻抗 50Ω，线尽量短不允许有锐角和直角。2 层板采用共面波导设计，走线两边包地多打地孔。

- 硬件设计中，如果射频通道上有引入 ESD 风险的点位，例如裸露的射频测试点、金属天线、外接天线座等，在 ESD 风险点靠近芯片侧预留 ESD 电感或 TVS 管，增加 ESD 防护能力。

- EPAD 四个角向外走，建议和表层 GND 连接，以增加隔离度。

- RF 匹配滤波电路的π型匹配电路的电容需要单点接地，不能直接在 TOP 层接地。如果是两层板需要打一个过孔连接到 BOTTOM 层的地，如果是多层板过孔不与中间层的地相连，过孔在中间层需要跟 TOP 层一样做禁空处理。这样处理的过孔相当于一个小电感与电容一起组成一个 LC 电路，起到抑制谐波辐射的目的。过孔的位置分布在 RF 线的两边。

图4-5 RF 布线参考  
![](images/1fe32550ba459aad9e137a6c2377b8be1d86127e6348b86a149a7887d7c0ad96.jpg)

- RF 远离晶体，参考值>5mm，晶体 XOUT 与 RF 的隔离度优于-55dB。

- RF 走线的 S11 参数需要优于-15dB。

- RFI 和 RFIO 隔离度要求-32dB 以上。

- RF 远离时钟钱、电源线、DDR 和 CPU 等强干扰源。

- 射频走线参考地保证完整，不允许有交叉、换层。若需要换层，换层孔周围打一圈地孔，形成类同轴结构。

- RF connector、滤波电容电感等大焊盘器件，射频信号对应的 PIN 邻层地需挖空（2 层板除外），避免寄生电容效应射频信号耦合到地。

- IPEX 座子 TOP 层和 BOTTOM 需要就近接地并打地孔。

!!! note "说明"

    雷达检测模组 PCB 设计约束:

    - 雷达模组与整机底板单点接地，避免与整机底板主地大面积连接。

    - 雷达模组电源选择低噪声、低纹波的线性电源，或者与整机底板主供电进行滤波隔离。

    - 雷达模组贴片的下方整机底板区域禁止走线、铺铜。

    - 雷达模组射频走线避免走在靠近整机底板贴片的电气层，例如常规设计中，模组射频走线不要走BOTTOM层，消除底板噪声耦合到模组射频通道的风险。

    - 雷达模组接口插针远离天线放置，插针选择L型侧接类型，焊接建议选用表贴方式，垂直板面的金属高度尽量短。

## 时钟布线指导

- 远离天线禁布区，参考值>10mm；远离数据线，参考值>5mm。

- 晶体及其走线远离噪声源和热源。WiFi 系统对时钟要求很高，噪声源（例如 RF、BUCK）会引起系统相噪变差，热源辐射会造成晶体温漂。

- XIN/XOUT 信号走线尽量短，做过孔和包地处理。

- PCB 空间受限的场景，时钟可能会耦合 RF 干扰到芯片内部，建议在 XIN、XOUT 走线靠近芯片端串接一个 0Ω电阻用于调试。

- PCB 为 4 层板时，晶体的 GND pad 建议在 TOP 层和其他地分割，通过过孔连接到主地，防止单板上的器件发热影响时钟精度。

## GND 布线指导

EPAD 四个角向外走 GND 线，和表层 GND 连接。

EPAD 过孔建议打 6×6 个。两层板受空间限制，EPAD 对应的 BOTTOM 层分 2 个区域。

整机尽量保留一个完整地平面，保证各芯片信号良好共地和回流，完善地孔，避免孤立铜皮出现。

## 屏蔽罩

TBD

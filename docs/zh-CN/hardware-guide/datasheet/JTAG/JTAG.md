<a id="7"></a>
# JTAG

<a id="7.1"></a>
## 概述

芯片内部集成一个自研CPU，在内部集成Coresight调试架构，支持基于Coresight的JTAG和SWD（Serial Wire Debug）调试接口，通过Coresight的JTAG或SWD调试接口实现调试，支持Lauterbach仿真器和JLINK仿真器。

<a id="7.2"></a>
## 调试接口

<hr style="border: none; border-top: 3px solid #555; margin: 16px 0;">

芯片调试接口默认复用为其他功能，如果需要使用调试功能，则管脚GPIO_04在上电时置高电平，系统复位解除后，对应管脚即可复位为调试接口，此后GPIO_04管脚功能不受影响，可以作为正常管脚使用。

<hr style="border: none; border-top: 3px solid #555; margin: 16px 0;">

调试管脚与PAD名字的对应关系请参见《WS63V100 SoC WiFi、BLE和SLE和 SLE Combo芯片 硬件用户指南》。

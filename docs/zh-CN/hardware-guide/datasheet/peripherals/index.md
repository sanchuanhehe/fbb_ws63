---
hide:
  - toc
---

# 外围设备

WS63 芯片提供丰富的外围设备接口，涵盖 IO 复用、通用输入输出、串行通信、脉宽调制、模数转换、直接存储器访问等功能，满足多样化的嵌入式应用需求。

## 快速导航

---

- [IO MUX](io-mux.md)

    芯片数字管脚数量有限，通过 IO 复用的方式丰富管脚功能。

---

- [GPIO](GPIO.md)

    通用可编程输入输出外设（General Purpose Programmable Input/Output），用于生成和采集特定应用的输入或输出信号，实现系统和外设之间的通信。

---

- [UART](UART.md)

    通用异步收发器（Universal Asynchronous Receiver/Transmitter），异步串行通信接口，用于和外部芯片的 UART 进行对接，实现两芯片间的通信。

---

- [I2C](I2C.md)

    I2C 总线主设备模块，完成 CPU 对 I2C 总线上从设备的数据读写，芯片支持 2 个 I2C 模块（I2C0 和 I2C1）。

---

- [SPI](SPI.md)

    同步串行通信接口，支持 Master/Slave 模式，实现数据的串并、并串转换，工作参考时钟为 240MHz。

---

- [PWM](PWM.md)

    PWM 模块，用于生成脉宽调制信号。

---

- [Tsensor](Tsensor.md)

    模拟温度检测 IP，检测芯片的节温并以二进制形式输出温度信息。

---

- [I2S](I2S.md)

    I2S 总线主/从设备模块，用于音频数据的串行传输。

---

- [QSPI](QSPI.md)

    Quad SPI 接口，6 线组成的同步串行通信外设。

---

- [DMA](DMA.md)

    直接存储器访问控制器（DMAC），在存储器和外设、外设和外设、存储器和存储器之间进行数据传输，减少处理器开销。

---

- [ADC](ADC.md)

    逐次逼近型数模转换设备（SAR ADC），将模拟信号转变成数字信号。

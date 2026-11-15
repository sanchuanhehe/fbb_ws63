---
hide:
  - toc
---

# API 参考

WS63 SDK完整的接口签名参考文档，按模块分类。

> **API 参考** vs **参考案例**：API 参考提供接口签名、参数和返回值的字典式查阅；参考案例提供场景驱动的教程式讲解。如需学习如何使用，请前往 [参考案例](../samples/index.md)。

## 快速导航

---

- [驱动](./driver/index.md)
    
    芯片级外设驱动接口，包括 GPIO (General Purpose Input/Output)、UART (Universal Asynchronous Receiver/Transmitter)、I2C (Inter-Integrated Circuit)、SPI (Serial Peripheral Interface)、PWM (Pulse Width Modulation)、ADC (Analog-to-Digital Converter)、DMA (Direct Memory Access)、Timer、Watchdog 等 24 个模块。

---
    
- [中间件](./middleware/index.md)

    中间件层接口，包括 Wi-Fi、SLE (SparkLink Low Energy)（星闪）、BTS (Bluetooth Subsystem)（蓝牙）、Radar（雷达）、SLP (SparkLink Positioning)、Utils 等。
    
---

- [OS 抽象层](./osal/index.md)
    
    操作系统抽象层（OSAL (Operating System Abstraction Layer)）接口，包括任务调度、中断、信号量、互斥锁、消息队列、定时器、内存管理等。

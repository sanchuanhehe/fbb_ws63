# beetle_sle_uart

## 一、介绍

本例程演示了如何使用两块WS63开发板通过SLE进行双向数据传输：

- 开发板A（Server端）和开发板B（Client端）配对连接
- A板：串口接收数据 → SLE发送 → B板：SLE接收 → 串口打印
- B板：串口接收数据 → SLE发送 → A板：SLE接收 → 串口打印

## 二、实验流程

打开kconfig勾选beetle_demo例程中的SLE_UART示例，准备两块WS63开发板A和B，A烧录SLE_Server示例，B烧录SLE_Client示例：

![img](.\images\sleuart_img1.PNG)

烧录完成后，启动两个串口助手分别连接到A和B，测试AB互发消息：

![img](.\images\sleuart_img2.PNG)

然后交换AB固件烧录，再次观察打印。

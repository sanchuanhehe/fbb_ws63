# Uart Demo

## 1.1 介绍

- **功能介绍：** 本实验演示了如何使用轮询和中断方式读取数据，其中中断方式加入了中断接收回调写事件和线程读事件的机制，提升线程运行的稳定性。  同时演示了如何读取UART 0的数据，预留了读取UART 1数据的示例代码。

- **软件概述：** UART是通用异步收发传输器缩写，通过设定固定的帧格式与波特率来实现串行数据通信，波特率就是指每秒钟所能传输的二进制位数，直接决定了数据传输的速度与同步精度。

- **硬件概述：** WS63核心板。

## 1.2 约束与限制：

- 支持应用运行的芯片和开发板，示例如下：
  - 本示例仅支持在开发板上运行，支持开发板：WCL-63X-DK01;
- API版本、SDK版本，示例如下：
  - 本示例支持版本号：1.10.101及以上;
- 支持的IDE版本，示例如下：
  - 本示例支持IDE版本号：1.0.0.6及以上；

## 1.3 效果预览：

- 轮询模式：
  - 在串口调试助手内不断向数据传输线（UART 0）发送数据，存在一定概率出现串口丢失数据。
  
![轮询方式读取](https://files.mdnice.com/user/92315/7ca762d5-ee14-4e30-98fe-8ad02c07f060.png)

  
- 中断模式：
  - 在串口调试助手内不断向数据传输线（UART 0）发送数据，基本不存在串口丢失数据。
  
![中断方式读取](https://files.mdnice.com/user/92315/f72f0afd-e684-4f27-b13d-9cc705eacc72.png)

  - 通过“中断接收”+“读写事件”的方式接收数据，不存在数据为16的倍数导致的丢包问题。
  
![中断方式读取为16倍数的数据](https://files.mdnice.com/user/92315/ccf1d9e2-84e7-4a6d-9d03-8cfe40169bce.png)


## 1.4 API接口介绍

- 主要描述该案例使用了哪些API、参数意义、返回值、头文件路径等。示例如下：

### 1.4.1 uapi_pin_set_mode()


| **定义：**   | errcode_t uapi_pin_set_mode(pin_t pin, pin_mode_t mode); |
| ------------ | -------------------------------------------------------- |
| **功能：**   | 设置引脚复用模式                                         |
| **参数：**   | pin：指定的IO口<br/>mode：复用模式                               |
| **返回值：** | ERRCODE_SUCC：成功    Other：失败                        |
| **依赖：**   | include\driver\pinctrl.h                                 |

### 1.4.2 uapi_uart_init()


| **定义：**   | errcode_t uapi_uart_init(uart_bus_t bus,  const uart_pin_config_t  *pins, const uart_attr_t  *attr,   const uart_extra_attr_t       *extra_attr, uart_buffer_config_t *uart_buffer_config); |
| ------------ | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **功能：**   | 初始化指定的串口                                                                                                                                                                            |
| **参数：**   | bus：串口号<br/>pins：UART中使用的PIN，包括TX, RX, RTS和CTS<br/>attr：UART的基础配置参数<br/>extra_attr ：UART的高级配置参数<br/>uart_buffer_config：指定UART的接收Buffer                   |
| **返回值：** | ERRCODE_SUCC：成功    Other：失败                                                                                                                                                           |
| **依赖：**   | include\driver\uart.h                                                                                                                                                                       |

### 1.4.3 uapi_uart_deinit()


| **定义：**   | errcode_t uapi_uart_deinit(uart_bus_t bus); |
| ------------ | ------------------------------------------- |
| **功能：**   | 去初始化指定的串口                          |
| **参数：**   | bus：串口号                                 |
| **返回值：** | ERRCODE_SUCC：成功    Other：失败           |
| **依赖：**   | include\driver\uart.h                       |


### 1.4.4 uapi_uart_read()


| **定义：**   | int32_t uapi_uart_read(uart_bus_t bus, const uint8_t *buffer, uint32_t length, uint32_t timeout);          |
| ------------ | ---------------------------------------------------------------------------------------------------------- |
| **功能：**   | 从UART读取数据                                                                                             |
| **参数：**   | bus：串口号<br/>buffer： 存储接收数据的Buffer  <br/>length：存储接收数据Buffer长度  <br/>timeout：超时时间 |
| **返回值：** | 读取到的数据长度                                |
| **依赖：**   | include\driver\uart.h                    |

### 1.4.5 uapi_uart_register_rx_callback()


| **定义：**   | errcode_t uapi_uart_register_rx_callback(uart_bus_t bus, uart_rx_condition_t condition, uint32_t size, uart_rx_callback_t callback);             |
| ------------ | ------------------------------------------------------------------------------------------------------------------------------------------------ |
| **功能：**   | 注册接收回调函数，这个回调函数会根据触发条件和Size触发。                                                                                         |
| **参数：**   | bus：串口号<br/>condition：回调触发的条件  <br/>size：如果触发条件涉及到数据长度，这个参数就表示需要的数据长度 <br/>callback：接收数据的回调函数 |
| **返回值：** | ERRCODE_SUCC：成功    Other：失败                                                                                                                |
| **依赖：**   | include\driver\uart.h                                                                                                                            |

## 1.5 具体实现

步骤一：设置GPIO复用UART功能。

步骤二：配置UART参数，例如波特率、数据位、停止位、校验位等。

步骤三：初始化UART。

步骤四：通过uapi_uart_read函数或者app_uart_read_int_handler中断回调函数读数据。

## 1.6 实验流程

- 步骤一：将xxx\vendor\Waveconn_WS63\demo\uart_demo文件里面内容拷贝到xxx\src\application\samples文件夹中。

![](https://files.mdnice.com/user/92315/f7d518d6-dc8d-4d4d-8a6c-32d6d9e9c3ea.png)

![](https://files.mdnice.com/user/92315/bb0df3df-e25d-4e6e-b50b-6a20eca3bd66.png)

- 步骤二：在xxx\src\application\samples\CMakeLists.txt文件中新增编译案例，具体如下图所示。
  
![](https://files.mdnice.com/user/92315/8d810951-afe2-494a-8a12-fc8067b2bc26.png)

- 步骤三：在xxx\src\application\samples\Kconfig文件中新增编译案例，具体如下图所示。


![](https://files.mdnice.com/user/92315/6a87672a-7cc4-49ce-b90f-e09365aff958.png)


- 步骤四：在终端命令行输入“./build.py menuconfig ws63-liteos-app”打开配置界面，通过“↑”、“↓”键切换到Application内，勾选Enable Sample后，勾选Enable the Sample of Uart，再勾选弹出的Support UART POLL Sample或者Support UART INT Sample选择串口接收模式。

![](https://files.mdnice.com/user/92315/6e86061b-2969-43ee-aa36-b87cc5e9713c.png)

![](https://files.mdnice.com/user/92315/6f592373-1f32-477b-beba-590fda86092c.png)

- 步骤五：在终端命令行输入“./build.py -c ws63-liteos-app”进行快速的**重编译**，在遇到报错时输入“./build.py -j1 ws63-liteos-app”进行慢速的**单核编译**，获取报错信息，方便根据报错信息进行调试。

![](https://files.mdnice.com/user/92315/537782bf-a2d3-44ce-b66d-68c13bf1c388.png)

![](https://files.mdnice.com/user/92315/81c514ba-bc7c-4426-99de-385f143a23be.png)

- 步骤六：编译完成如下图所示。

![](https://files.mdnice.com/user/92315/20393d6d-f2c3-4707-a71d-18397cda179d.png)

- 步骤七：找到xxx\output\ws63\fwpkg\ws63-liteos-app文件夹，右键点击ws63-liteos-app_all.fwpkg文件，并点击**在文件资源管理器中显示**，随后左键点击上方地址栏，**复制该文件位置**。
  
![](https://files.mdnice.com/user/92315/43bb8e0d-de20-415c-a75f-670ce1ce4bee.png)

![](https://files.mdnice.com/user/92315/11efee78-1357-4d9e-bfbb-b0239557fc08.png)

- 步骤八：打开BurnTool烧录工具，点击Option选择WS63芯片，点击Setting设置波特率（Baud）为100 0000，勾选Auto burn和Auto disconnect，选择对应的COM口（可以在设备管理器/端口中查看核心板COM口）。点击Select file，将**步骤七中的文件位置复制到上方地址栏**即可跳转到固件位置，随后双击ws63-liteos-app_all.fwpkg文件。<br>点击Connect并按下核心板的Reset按键即可完成烧录。

![](https://files.mdnice.com/user/92315/9102a345-3718-4972-9ffe-65c2be396be8.png)

![](https://files.mdnice.com/user/92315/bf2bd984-d943-40d0-b6f0-8b92a9cd99ed.png)


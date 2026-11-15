# GPIO OUTPUT Demo

## 1.1 介绍

- **功能介绍：** 本实验演示了如何设置GPIO输出模式并输出不同的电平状态。

- **软件概述：** GPIO（General-Purpose Input/Output，通用输入/输出）输入通过浮空、上拉或下拉三种模式接收外部电平，并可采用轮询不断查询或中断在上升沿、下降沿、双边沿或电平触发时即时响应，实现对外部信号的可靠检测。

- **硬件概述：** WS63核心板。

## 1.2 约束与限制：

- 支持应用运行的芯片和开发板，示例如下：
  - 本示例仅支持在开发板上运行，支持开发板：WCL-63X-DK01;
- API版本、SDK版本，示例如下：
  - 本示例支持版本号：1.10.101及以上;
- 支持的IDE版本，示例如下：
  - 本示例支持IDE版本号：1.0.0.6及以上；

## 1.3 效果预览：

- 闪烁LED灯：使用杜邦线连接GPIO 2到LED灯上，每隔1秒切换一次LED状态。

![](https://files.mdnice.com/user/92315/1978f365-13e3-4198-9201-9fd58ab88b3c.png)

## 1.4 API接口介绍

### 1.4.1 uapi_gpio_init()

| **定义：**   | void uapi_gpio_init(void); |
| ------------ | -------------------------------------------------------- |
| **功能：**   | 初始化GPIO                                         |
| **参数：**   | 无参数                                             |
| **返回值：** | 无返回值                        |
| **依赖：**   | drivers/drivers/driver/gpio/gpio.h                     |

### 1.4.2 uapi_gpio_deinit()

| **定义：**   | void uapi_gpio_deinit(void); |
| ------------ | -------------------------------------------------------- |
| **功能：**   | 去初始化GPIO                                         |
| **参数：**   | 无参数                                             |
| **返回值：** | 无返回值                        |
| **依赖：**   | drivers/drivers/driver/gpio/gpio.h                     |

### 1.4.3 uapi_gpio_set_dir()

| **定义：**   | errcode_t uapi_gpio_set_dir(pin_t pin, gpio_direction_t dir); |
| ------------ | -------------------------------------------------------- |
| **功能：**   | 设置GPIO的输入输出方向函数                                    |
| **参数：**   | pin：指定的IO口<br/>dir：输入输出方向                         |
| **返回值：** | ERRCODE_SUCC：成功    Other：失败                        |
| **依赖：**   | drivers/drivers/driver/gpio/gpio.h                     |


### 1.4.4 uapi_gpio_get_dir()

| **定义：**   | errcode_t uapi_gpio_get_dir(pin_t pin); |
| ------------ | -------------------------------------------------------- |
| **功能：**   | 设置GPIO的输入输出方向函数                                    |
| **参数：**   | pin：指定的IO口                                          |
| **返回值：** | ERRCODE_SUCC：成功    Other：失败                        |
| **依赖：**   | drivers/drivers/driver/gpio/gpio.h                     |

### 1.4.5 uapi_gpio_set_val()

| **定义：**   | errcode_t uapi_gpio_set_val(pin_t pin, gpio_level_t level); |
| ------------ | -------------------------------------------------------- |
| **功能：**   | 设置GPIO的输入输出方向函数                                    |
| **参数：**   | pin：指定的IO口<br/>level：输出设置为高或者低                 |
| **返回值：** | ERRCODE_SUCC：成功    Other：失败                        |
| **依赖：**   | drivers/drivers/driver/gpio/gpio.h                     |

### 1.4.6 uapi_gpio_get_val()

| **定义：**   | errcode_t uapi_gpio_get_val(pin_t pin); |
| ------------ | -------------------------------------------------------- |
| **功能：**   | 设置GPIO的输入输出方向函数                                    |
| **参数：**   | pin：指定的IO口                                         |
| **返回值：** | ERRCODE_SUCC：成功    Other：失败                        |
| **依赖：**   | drivers/drivers/driver/gpio/gpio.h                     |

### 1.4.7 uapi_gpio_register_isr_func()

| **定义：**   | errcode_t uapi_gpio_register_isr_func(pin_t pin, uint32_t trigger, gpio_callback_t callback);             |
| ------------ | ------------------------------------------- |
| **功能：**   | 注册GPIO的中断                               |
| **参数：**   | pin：指定的IO口<br/>trigger：GPIO中断类型<br/>callback：指定的回调函数 |
| **返回值：** | ERRCODE_SUCC：成功    Other：失败                      |
| **依赖：**   | drivers/drivers/driver/gpio/gpio.h                          |

### 1.4.8 uapi_gpio_unregister_isr_func()

| **定义：**   | errcode_t uapi_gpio_unregister_isr_func(pin_t pin);             |
| ------------ | -------------------------------------------------------------- |
| **功能：**   |去注册GPIO的中断                                        |
| **参数：**   | pin：指定的IO口                                         |
| **返回值：** | ERRCODE_SUCC：成功    Other：失败                      |
| **依赖：**   | drivers/drivers/driver/gpio/gpio.h                                |

### 1.4.9 uapi_gpio_disable_interrupt()

| **定义：**   | errcode_t uapi_gpio_disable_interrupt(pin_t pin);             |
| ------------ | -------------------------------------------------------------- |
| **功能：**   | 去使能GPIO指定端口的中断                               |
| **参数：**   | pin：指定的IO口                                         |
| **返回值：** | ERRCODE_SUCC：成功    Other：失败                      |
| **依赖：**   | drivers/drivers/driver/gpio/gpio.h                                |

### 1.4.10 uapi_gpio_enable_interrupt()

| **定义：**   | errcode_t uapi_gpio_enable_interrupt(pin_t pin);             |
| ------------ | -------------------------------------------------------------- |
| **功能：**   | 使能GPIO指定端口的中断                               |
| **参数：**   | pin：指定的IO口                                         |
| **返回值：** | ERRCODE_SUCC：成功    Other：失败                      |
| **依赖：**   | drivers/drivers/driver/gpio/gpio.h                                |

### 1.4.11 uapi_gpio_clear_interrupt()

| **定义：**   | errcode_t uapi_gpio_clear_interrupt(pin_t pin);             |
| ------------ | -------------------------------------------------------------- |
| **功能：**   | 清除GPIO指定端口的中断                               |
| **参数：**   | pin：指定的IO口                                         |
| **返回值：** | ERRCODE_SUCC：成功    Other：失败                      |
| **依赖：**   | drivers/drivers/driver/gpio/gpio.h                                |                                                                                             
## 1.5 具体实现

步骤一：初始化Pinctrl口和GPIO口。

步骤二：设置GPIO 2为普通GPIO模式、输出方向。

步骤三：在线程内不断设置GPIO 2的电平状态。

## 1.6 实验流程

- 步骤一：将xxx\vendor\Waveconn_WS63\demo\led_demo文件里面内容拷贝到xxx\src\application\samples文件夹中。

![](https://files.mdnice.com/user/92315/9c11338a-edaf-4534-810d-5b0fb71a5694.png)

![](https://files.mdnice.com/user/92315/c66d71f5-b15c-4ae2-8ce2-e2846dd5aad9.png)

- 步骤二：在xxx\src\application\samples\CMakeLists.txt文件中新增编译案例，具体如下图所示。
  
![](https://files.mdnice.com/user/92315/e7263c5c-cdfd-4e62-b3bf-c177f9f6bbb5.png)

- 步骤三：在xxx\src\application\samples\Kconfig文件中新增编译案例，具体如下图所示。

![](https://files.mdnice.com/user/92315/6629d413-87fc-4992-a6ae-c4d8832325d4.png)

- 步骤四：在终端命令行输入“./build.py menuconfig ws63-liteos-app”打开配置界面，通过“↑”、“↓”键切换到Application内，勾选Enable Sample后，勾选Enable the Sample of LED。

![](https://files.mdnice.com/user/92315/6e86061b-2969-43ee-aa36-b87cc5e9713c.png)

![](https://files.mdnice.com/user/92315/09f5b247-6343-444c-8b7e-8dc9d99d8730.png)

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


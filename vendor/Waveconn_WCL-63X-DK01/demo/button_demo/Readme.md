# GPIO INPUT Demo

## 1.1 介绍

- **功能介绍：** 本实验演示了如何使用轮询和中断方式查询GPIO引脚的输入状态。

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

- 轮询模式：
  - 存在一定概率出现按下按键后，程序没有即刻输出“Button down”。

![](https://files.mdnice.com/user/92315/25e4b13e-17d3-496f-aee0-6f7b2245d6b8.png)

- 中断模式：
  - 按下按键后程序即刻输出“PIN:0 interrupt success.”。
  
![](https://files.mdnice.com/user/92315/7f966357-e0af-4f2c-a672-791f937e64ed.png)



## 1.4 API接口介绍

- 主要描述该案例使用了哪些API、参数意义、返回值、头文件路径等。示例如下：

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

步骤二：设置GPIO 0为普通GPIO模式、输入方向和上拉模式。

步骤三：轮询读取GPIO 0的电平状态，并延时后重复检测电平状态做按键消抖；注册GPIO 0的下降沿中断回调函数，在回调函数中输出日志。

## 1.6 实验流程

- 步骤一：将xxx\vendor\Waveconn_WS63\demo\button_demo文件里面内容拷贝到xxx\src\application\samples文件夹中。

![](https://files.mdnice.com/user/92315/39b3a286-ead5-49c2-b922-dcce67acd11e.png)

![](https://files.mdnice.com/user/92315/84cd422e-af85-44b5-b045-ea0e73121800.png)

- 步骤二：在xxx\src\application\samples\CMakeLists.txt文件中新增编译案例，具体如下图所示。
  
![](https://files.mdnice.com/user/92315/88beadd4-e1bf-49eb-8bc6-e86a30487f48.png)

- 步骤三：在xxx\src\application\samples\Kconfig文件中新增编译案例，具体如下图所示。

![](https://files.mdnice.com/user/92315/a6521efb-bc54-4fc2-9d6a-6209d0d74629.png)

- 步骤四：在终端命令行输入“./build.py menuconfig ws63-liteos-app”打开配置界面，通过“↑”、“↓”键切换到Application内，勾选Enable Sample后，勾选Enable the Sample of Button，再选择使用轮询还是中断检测GPIO的电平状态。

![](https://files.mdnice.com/user/92315/6e86061b-2969-43ee-aa36-b87cc5e9713c.png)

![](https://files.mdnice.com/user/92315/58976a10-40f0-4590-bb9d-f275fea69a00.png)


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


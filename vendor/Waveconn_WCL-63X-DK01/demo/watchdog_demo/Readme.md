# Watchdog Demo

## 1.1 介绍

- **功能介绍：** 本实验演示了在线程循环中，无看门狗导致的线程崩溃和有看门狗维持的线程正常运作。

- **软件概述：** 看门狗定时器（Watch Dog Timer，WDT）是单片机的一个组成部分，本质是一个递增（或者递减）的定时器，程序开始执行的时候，看门狗的值就开始递增或者由某固定值递减，到达设定的值的时候单片机就触发中断或者产生系统复位，重新运行。是用来监测单片机运行状态和解决程序引起的故障的模块。

- **硬件概述：** WS63核心板。

## 1.2 约束与限制：

- 支持应用运行的芯片和开发板，示例如下：
  - 本示例仅支持在开发板上运行，支持开发板：WCL-63X-DK01;
- API版本、SDK版本，示例如下：
  - 本示例支持版本号：1.10.101及以上;
- 支持的IDE版本，示例如下：
  - 本示例支持IDE版本号：1.0.0.6及以上；

## 1.3 效果预览：

- 无看门狗循环：在线程的循环开始后，经过一定时间线程崩溃、系统重启。
  
![](https://files.mdnice.com/user/92315/b706f842-4d98-4196-8a8f-e1a17c9a81aa.png)
  
- 有看门狗循环：在线程的循环开始后，每经过500ms进行一次喂狗，维持线程正常运转。
  
![](https://files.mdnice.com/user/92315/1faeb24a-4c56-4935-b718-4b8f30d32d71.png)


## 1.4 API接口介绍

### 1.4.1 uapi_watchdog_init()

| **定义：**   | errcode_t uapi_watchdog_init(uint32_t timeout); |
| ------------ | ---------------------------------------------- |
| **功能：**   | 初始化看门狗                                      |
| **参数：**   | timeout：设置看门狗超时时间                         |
| **返回值：** | ERRCODE_SUCC：成功    Other：失败                  |
| **依赖：**   | include\driver\watchdog.h                       |

### 1.4.2 uapi_watchdog_deinit()

| **定义：**   | errcode_t uapi_watchdog_deinit(void); |
| ------------ | -------------------------------------------------------- |
| **功能：**   | 去初始化看门狗                                              |
| **参数：**   | 无参数                                                     |
| **返回值：** | ERRCODE_SUCC：成功    Other：失败                           |
| **依赖：**   | include\driver\watchdog.h                                 |

### 1.4.3 uapi_watchdog_enable()

| **定义：**   | errcode_t uapi_watchdog_enable(wdt_mode_t mode); |
| ------------ | ----------------------------------------------- |
| **功能：**   | 使能看门狗                                         |
| **参数：**   | mode：设置看门狗模式                                |
| **返回值：** | ERRCODE_SUCC：成功    Other：失败                   |
| **依赖：**   | include\driver\watchdog.h                        |


### 1.4.4 uapi_watchdog_disable()

| **定义：**   | errcode_t uapi_watchdog_disable(void); |
| ------------ | ----------------------------------------------- |
| **功能：**   | 去使能看门狗                                         |
| **参数：**   | 无参数                                |
| **返回值：** | ERRCODE_SUCC：成功    Other：失败                   |
| **依赖：**   | include\driver\watchdog.h                        |

### 1.4.5 uapi_watchdog_kick()

| **定义：**   | errcode_t uapi_watchdog_kick(void); |
| ------------ | ----------------------------------------------- |
| **功能：**   | 看门狗喂狗                                         |
| **参数：**   | 无参数                                            |
| **返回值：** | ERRCODE_SUCC：成功    Other：失败                   |
| **依赖：**   | include\driver\watchdog.h                        |

### 1.4.6 uapi_watchdog_set_time()

| **定义：**   | errcode_t uapi_watchdog_set_time(uint32_t timeout);         |
| ------------ | ----------------------------------------------------------- |
| **功能：**   | 设置Watchdog超时时间。                   |
| **参数：**   | timeout：设置看门狗超时时间，单位秒。       |
| **返回值：** | ERRCODE_SUCC：成功    Other：失败                   |
| **依赖：**   | include\driver\watchdog.h                        |

### 1.4.7 uapi_watchdog_get_left_time()

| **定义：**   | errcode_t uapi_watchdog_get_left_time(uint32_t *timeout);    |
| ------------ | ----------------------------------------------------------- |
| **功能：**   | 获取看门狗计数器的剩余值。                           |
| **参数：**   | timeout：时间剩余值。                               |
| **返回值：** | ERRCODE_SUCC：成功    Other：失败                   |
| **依赖：**   | include\driver\watchdog.h                         |

### 1.4.8 uapi_register_watchdog_callback()

| **定义：**   | errcode_t uapi_register_watchdog_callback(watchdog_callback_t callback);    |
| ------------ | ----------------------------------------------------------- |
| **功能：**   | 注册看门狗回调。                           |
| **参数：**   | callback：如果看门狗超时，则调用该回调函数来处理异常。    |
| **返回值：** | ERRCODE_SUCC：成功    Other：失败                   |
| **依赖：**   | include\driver\watchdog.h                         |

## 1.5 具体实现

步骤一：初始化看门狗，并配置超时时间。

步骤二：使能看门狗，并配置看门狗模式。

步骤三：注册看门狗超时回调函数。

步骤四：执行线程循环。

## 1.6 实验流程

- 步骤一：将xxx\vendor\Waveconn_WS63\demo\watchdog_demo文件里面内容拷贝到xxx\src\application\samples文件夹中。

![](https://files.mdnice.com/user/92315/e700a6b7-5e2f-494d-9aec-ac337117fc04.png)

![](https://files.mdnice.com/user/92315/043ca8a9-ff44-4696-8a3c-b6b8274a7525.png)

- 步骤二：在xxx\src\application\samples\CMakeLists.txt文件中新增编译案例，具体如下图所示。
  
![](https://files.mdnice.com/user/92315/d45d3fc0-5ca7-4da1-980b-de78cbb8a844.png)

- 步骤三：在xxx\src\application\samples\Kconfig文件中新增编译案例，具体如下图所示。

![](https://files.mdnice.com/user/92315/c695c4b5-2c36-424b-b72e-5032bc554529.png)

- 步骤四：在终端命令行输入“./build.py menuconfig ws63-liteos-app”打开配置界面，通过“↑”、“↓”键切换到Application内，勾选Enable Sample后，勾选Enable the Sample of Watchdog，再勾选弹出的Choose watchdog kick timeout或者Choose watchdog kick normally选择是否在线程循环中喂狗。

![](https://files.mdnice.com/user/92315/6e86061b-2969-43ee-aa36-b87cc5e9713c.png)

![](https://files.mdnice.com/user/92315/71fd6f7f-7584-4ebc-b184-1d148ee822df.png)

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


# I2C Demo

## 1.1 介绍

- **功能介绍：** 本实验演示了如何使用I2C协议驱动型号SHT30的温湿度传感器获取温度和湿度。

- **软件概述：** I2C（Inter-Integrated  Circuit，集成电路总线）是内部整合电路的称呼，是一种串行通信总线，使用多主从架构。I2C总线支持任何IC生产过程(NMOS CMOS、双极性）。两线――SDA（Serial Data，串行数据）和SCL（Serial Clock，串行时钟）线在连接到总线的器件间传递信息。每个器件都有一个唯一的地址识别，而且都可以作为一个发送器或接收器（由器件的功能决定）。

- **硬件概述：** WS63核心板。

## 1.2 约束与限制：

- 支持应用运行的芯片和开发板，示例如下：
  - 本示例仅支持在开发板上运行，支持开发板：WCL-63X-DK01;
- API版本、SDK版本，示例如下：
  - 本示例支持版本号：1.10.101及以上;
- 支持的IDE版本，示例如下：
  - 本示例支持IDE版本号：1.0.0.6及以上；

## 1.3 效果预览：

- 读取温湿度：

![](https://files.mdnice.com/user/92315/ac79ef26-1b7f-4846-be0c-b2837322a86c.png)


## 1.4 API接口介绍

### 1.4.1 uapi_i2c_master_init()

| **定义：**   | errcode_t uapi_i2c_master_init(i2c_bus_t bus, uint32_t baudrate, uint8_t hscode); |
| ------------ | -------------------------------------------------------- |
| **功能：**   | 初始化该I2C设备为主机                                         |
| **参数：**   | bus：I2C总线<br/>baudrate：波特率<br/>hscode：I2C高速模式主机码       |
| **返回值：** | ERRCODE_SUCC：成功    Other：失败                        |
| **依赖：**   | drivers/drivers/driver/i2c/i2c.h                     |

### 1.4.2 uapi_i2c_deinit()

| **定义：**   | errcode_t uapi_i2c_deinit(i2c_bus_t bus); |
| ------------ | -------------------------------------------------------- |
| **功能：**   | 去初始化I2C设备，支持主从机                                        |
| **参数：**   | bus：I2C总线                                             |
| **返回值：** | ERRCODE_SUCC：成功    Other：失败                        |
| **依赖：**   | drivers/drivers/driver/i2c/i2c.h                      |

### 1.4.3 uapi_i2c_master_write()

| **定义：**   | errcode_t uapi_i2c_master_write(i2c_bus_t bus, uint16_t dev_addr, i2c_data_t *data); |
| ------------ | -------------------------------------------------------- |
| **功能：**   | I2C主机将数据发送到目标从机                                    |
| **参数：**   | bus：I2C总线<br/>dev_addr：目标从机地址<br/>data：发送的数据   |
| **返回值：** | ERRCODE_SUCC：成功    Other：失败                        |
| **依赖：**   | drivers/drivers/driver/i2c/i2c.h                      |


### 1.4.4 uapi_i2c_master_read()

| **定义：**   | errcode_t uapi_i2c_master_read(i2c_bus_t bus, uint16_t dev_addr, i2c_data_t *data); |
| ------------ | -------------------------------------------------------- |
| **功能：**   | 主机接收来自目标I2C从机的数据                                    |
| **参数：**   | bus：I2C总线<br/>dev_addr：目标从机地址<br/>data：接收到的数据       |
| **返回值：** | ERRCODE_SUCC：成功    Other：失败                        |
| **依赖：**   | drivers/drivers/driver/i2c/i2c.h                      |

### 1.4.5 uapi_i2c_master_writeread()

| **定义：**   | errcode_t uapi_i2c_master_writeread(i2c_bus_t bus, uint16_t dev_addr, i2c_data_t *data); |
| ------------ | -------------------------------------------------------- |
| **功能：**   | 主机发送数据到目标I2C从机，并接收来自此从机的数据             |
| **参数：**   | bus：I2C总线<br/>dev_addr：目标从机地址<br/>data：接收到的数据    |
| **返回值：** | ERRCODE_SUCC：成功    Other：失败                        |
| **依赖：**   | drivers/drivers/driver/i2c/i2c.h                      |

### 1.4.6 uapi_i2c_set_baudrate()

| **定义：**   | errcode_t uapi_i2c_set_baudrate(i2c_bus_t bus, uint32_t baudrate); |
| ------------ | ------------------------------------ |
| **功能：**   | 对已初始化的I2C重置波特率，支持主从机                    |
| **参数：**   | bus：I2C总线<br/>baudrate：波特率                |
| **返回值：** | ERRCODE_SUCC：成功    Other：失败                        |
| **依赖：**   | drivers/drivers/driver/i2c/i2c.h            |                                                                           
## 1.5 具体实现

步骤一：初始化I2C引脚和I2C总线1。

步骤二：初始化SHT30传感器。

步骤三：在线程内通过I2C不断循环读取SHT30传感器的温湿度数据。

## 1.6 实验流程

- 步骤一：将xxx\vendor\Waveconn_WS63\demo\i2c_sht30_demo文件里面内容拷贝到xxx\src\application\samples文件夹中。

![](https://files.mdnice.com/user/92315/1ce1fa1d-31c8-44a2-aedc-5af6fb367759.png)

![](https://files.mdnice.com/user/92315/cabc96ba-a6d9-4c13-87f5-bb5cd5656bc2.png)

- 步骤二：在xxx\src\application\samples\CMakeLists.txt文件中新增编译案例，具体如下图所示。
  
![](https://files.mdnice.com/user/92315/6453fb7c-1e97-4b3e-8e12-85b5d31cd63e.png)

- 步骤三：在xxx\src\application\samples\Kconfig文件中新增编译案例，具体如下图所示。

![](https://files.mdnice.com/user/92315/8302886e-47a9-480c-afbc-e9b6afd70d5d.png)

- 步骤四：在终端命令行输入“./build.py menuconfig ws63-liteos-app”打开配置界面，通过“↑”、“↓”键切换到Application内，勾选Enable Sample后，勾选Enable the sample of I2C。

![](https://files.mdnice.com/user/92315/6e86061b-2969-43ee-aa36-b87cc5e9713c.png)

![](https://files.mdnice.com/user/92315/1b646f59-e65c-4a77-9dc4-a03b79c911b7.png)

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


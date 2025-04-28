## hispark_t2_board_qc

### 介绍

**功能介绍：** 对设备进行全面测试。

**软件概述：** 

1. 启动小车搭载的所有传感器，包括AHT20、BH1750、BMX055、CW2015、INS5699S 并将数据显示于SSD1306液晶屏，测试各传感器工作是否正常。

2. 点亮WS2812B灯带，灯带颜色将循环闪烁。

3. 连接用于产生电机PWM信号和用于扩展IO的两个STM8s芯片，测试电机和编码器功能。

**硬件概述：** HiSpark_T2小车。

###  约束与限制

#### 支持应用运行的芯片和开发板

本示例支持开发板：HiSpark_T2

#### 支持API版本、SDK版本

本示例支持版本号：1.10.101 及以上

#### 支持IDE版本、支持配套工具版本

本示例支持IDE版本号：1.0.0.6 及以上

## 实验流程

- 步骤一：在xxx\src\application\samples\peripheral文件夹新建一个sample文件夹，在peripheral上右键选择“新建文件夹”，创建Sample文件夹，例如名称”hispark_t2_board_qc“。

- 步骤二：将xxx\vendor\HiSpark_T2\demo\hispark_t2_board_qc文件里面内容拷贝到**步骤一创建的Sample文件夹中“hispark_t2_board_qc”**。


* 步骤三：在xxx\src\application\samples\peripheral\CMakeLists.txt文件中新增编译案例。

* 步骤四：在xxx\src\application\samples\peripheral\Kconfig文件中新增编译案例。

- 步骤五：点击如下图标，选择”**系统配置**“，具体选择路径“Application/Enable the Sample of peripheral”，在弹出框中选择“support hispark_t2_board_qc Sample”，点击Save，关闭弹窗。

- 步骤六：点击“build”或者“rebuild”编译

- 步骤七：编译完成。

- 步骤八：在HiSpark Studio工具中点击“工程配置”按钮，选择“程序加载”，传输方式选择“serial”，端口选择“comxxx”，com口在设备管理器中查看（如果找不到com口，请参考windows环境搭建）。

- 步骤九：配置完成后，点击工具“程序加载”按钮烧录。

- 步骤十：出现“Connecting, please reset device...”字样时，复位开发板，等待烧录结束。


* 步骤十一：烧录完成后，液晶屏上显示所有传感器数据，灯带交替闪烁，电机和编码器均正常运行。

## 测试项目

### 传感器与液晶屏

上电后，液晶屏应正常显示数据，其中：

**第一行：**光照强度（0~3000且正常随外界光照强度变化）、温湿度（合理范围内且正常随外界温湿度变化）。

**第二行：**格式为yy/mm/dd hh:mm:ss 的rtc数据，默认从24/12/18 15:30:45开始累加。

**第三行：**格式为 ax | ay | az gx | gy | gz 的imu数据，数据应正常合理。

**第四行：**格式为 mx | my | mz 的imu磁力计数据、电池电压数据（满电时约4100mV）。

### 编码器与按钮

上电后，顺/逆时针旋转编码器，RGB交通灯应从呈现左至右/从右至左循环流水灯效果，单击button1、button2，交通灯应向左、向右点亮。

若编码器或按钮无法正常工作，按下 Expanded IO 区域的按钮以重启对应的stm8s。

### 电机

上电后，电机应自动工作，正转5s后反转5s

若电机无法正常工作，按下 PWM Generate 区域的按钮以重启对应的stm8s。
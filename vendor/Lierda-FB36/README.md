# Lierda FB36 开发板示例

## 介绍

### 功能介绍

Lierda FB36 开发板示例集合了多种外设演示案例，包括 GPIO 输出、按键中断、PWM 呼吸灯以及 SLE 串口通信等功能。

| 示例 | 功能描述 |
|------|---------|
| LED | GPIO 输出控制 LED 循环闪烁 |
| PWM LED | PWM 控制实现呼吸灯效果 |
| Key | GPIO 按键中断检测 |
| SLE UART | SLE 星闪串口透传演示（Server/Client） |

### 软件概述

- **LED 示例**：通过 GPIO 引脚输出高低电平，控制 LED 灯循环亮灭
- **PWM LED 示例**：通过 PWM 输出实现占空比渐变，产生呼吸灯效果
- **Key 示例**：通过 GPIO 双向边沿中断检测按键按下
- **SLE UART 示例**：基于 SLE 星闪协议的 UART 透传演示

###  硬件概述

Lierda FB36 开发板基于海思 WS63 系列芯片，详情请参考：
- [FB36 Duino 开发板原理图参考](../../doc/hardware/FB36%20duino开发板原理图参考_Rev1.0.pdf)
- [FB36 开发板硬件说明书](../../doc/hardware/FB36%20duino开发板硬件说明书_Rev1.0.pdf)
- [FB36 系列模组硬件设计手册](../../doc/hardware/Lierda%20FB36系列模组硬件设计手册_Rev1.1.pdf)

## 约束与限制

### 支持应用运行的芯片和开发板

本示例支持开发板：**Lierda FB36**

### 支持 API 版本、SDK 版本

本示例支持版本号：**1.10.101 及以上**

### 支持 IDE 插件版本

本示例支持 IDE 插件版本号：**1.0.1 及以上**

## 目录结构

```
Lierda-FB36/
├── demo/
│   ├── led/                    # LED 闪烁示例
│   │   ├── led_demo.c
│   │   ├── CMakeLists.txt
│   │   └── Kconfig
│   ├── pwm_led/                # PWM 呼吸灯示例
│   │   ├── pwm_led_demo.c
│   │   ├── CMakeLists.txt
│   │   └── Kconfig
│   ├── key/                    # 按键中断示例
│   │   ├── key_demo.c
│   │   ├── CMakeLists.txt
│   │   └── Kconfig
│   └── sle_uart/               # SLE UART 透传示例
│       ├── sle_uart_client/    # 客户端
│       │   └── sle_uart_client.c
│       └── sle_uart_server/    # 服务端
│           ├── sle_uart_server.c
│           ├── sle_uart_server.h
│           ├── sle_uart_server_adv.c
│           └── sle_uart_server_adv.h
├── doc/
│   └── hardware/               # 硬件文档
│       ├── FB36 duino开发板原理图参考_Rev1.0.pdf
│       ├── FB36 duino开发板硬件说明书_Rev1.0.pdf
│       └── Lierda FB36系列模组硬件设计手册_Rev1.1.pdf
├── build_config.json           # 构建配置
└── README.md                   # 本文档
```

## 效果预览

| 示例 | 效果 |
|------|------|
| LED | 开发板上的 LED 灯以 500ms 间隔循环闪烁 |
| PWM LED | LED 灯呈现平滑的呼吸灯渐变效果 |
| Key | 按下按键后串口输出 "Key(pinX) is pressed." |
| SLE UART | 串口数据通过 SLE 无线传输，实现双向透传 |

## 接口介绍

### GPIO 接口

#### uapi_gpio_set_dir()

| **定义** | errcode_t uapi_gpio_set_dir(pin_t pin, gpio_direction_t dir); |
|---------|-------------------------------------------------------------|
| **功能** | 设置 GPIO 的输入输出方向 |
| **参数** | pin：IO 引脚号<br/>dir：输入输出方向（GPIO_DIRECTION_INPUT / GPIO_DIRECTION_OUTPUT） |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/gpio.h |

#### uapi_gpio_set_val()

| **定义** | errcode_t uapi_gpio_set_val(pin_t pin, gpio_level_t level); |
|---------|----------------------------------------------------------- |
| **功能** | 设置 GPIO 的输出电平 |
| **参数** | pin：IO 引脚号<br/>level：输出电平（GPIO_LEVEL_HIGH / GPIO_LEVEL_LOW） |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/gpio.h |

#### uapi_gpio_toggle()

| **定义** | errcode_t uapi_gpio_toggle(pin_t pin); |
|---------|-------------------------------------- |
| **功能** | 翻转 GPIO 输出电平状态 |
| **参数** | pin：IO 引脚号 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/gpio.h |

#### uapi_gpio_register_isr_func()

| **定义** | errcode_t uapi_gpio_register_isr_func(pin_t pin, gpio_interrupt_mode_t mode, gpio_callback_t func); |
|---------|------------------------------------------------------------------------------------------------------|
| **功能** | 注册 GPIO 中断回调函数 |
| **参数** | pin：IO 引脚号<br/>mode：中断触发模式<br/>func：中断回调函数 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/gpio.h |

#### uapi_gpio_enable_interrupt()

| **定义** | errcode_t uapi_gpio_enable_interrupt(pin_t pin); |
|---------|--------------------------------------------------|
| **功能** | 使能 GPIO 中断 |
| **参数** | pin：IO 引脚号 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/gpio.h |

###  PWM 接口

#### uapi_pwm_init()

| **定义** | errcode_t uapi_pwm_init(void); |
|---------|-------------------------------|
| **功能** | 初始化 PWM 模块 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/pwm.h |

#### uapi_pwm_open()

| **定义** | errcode_t uapi_pwm_open(uint8_t channel, const pwm_config_t *config); |
|---------|----------------------------------------------------------------------|
| **功能** | 打开 PWM 通道并配置 |
| **参数** | channel：PWM 通道号<br/>config：PWM 配置参数 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/pwm.h |

#### uapi_pwm_set_group()

| **定义** | errcode_t uapi_pwm_set_group(uint8_t group_id, const uint8_t *channels, uint8_t channel_count); |
|---------|-----------------------------------------------------------------------------------------------|
| **功能** | 设置 PWM 组通道 |
| **参数** | group_id：组 ID<br/>channels：通道列表<br/>channel_count：通道数量 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/pwm.h |

#### uapi_pwm_start_group()

| **定义** | errcode_t uapi_pwm_start_group(uint8_t group_id); |
|---------|---------------------------------------------------|
| **功能** | 启动 PWM 组输出 |
| **参数** | group_id：组 ID |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/pwm.h |

#### uapi_pwm_close()

| **定义** | errcode_t uapi_pwm_close(uint8_t group_id); |
|---------|--------------------------------------------|
| **功能** | 关闭 PWM 组 |
| **参数** | group_id：组 ID |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/pwm.h |

###  PIN 接口

#### uapi_pin_set_mode()

| **定义** | errcode_t uapi_pin_set_mode(pin_t pin, pin_mode_t mode); |
|---------|---------------------------------------------------------|
| **功能** | 设置引脚模式 |
| **参数** | pin：引脚号<br/>mode：模式（PIN_MODE_0 / PIN_MODE_1 / PIN_MODE_2 等） |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/pinctrl.h |

###  UART 接口

#### uapi_uart_init()

| **定义** | errcode_t uapi_uart_init(uint8_t id, const uart_pin_config_t *pin_config, const uart_attr_t *attr, ...); |
|---------|-----------------------------------------------------------------------------------------------|
| **功能** | 初始化 UART |
| **参数** | id：UART ID<br/>pin_config：引脚配置<br/>attr：串口属性 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/uart.h |

#### uapi_uart_register_rx_callback()

| **定义** | errcode_t uapi_uart_register_rx_callback(uint8_t id, uart_rx_condition_t condition, ...); |
|---------|------------------------------------------------------------------------------------------|
| **功能** | 注册 UART 接收回调 |
| **参数** | id：UART ID<br/>condition：触发条件<br/>callback：回调函数 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/uart.h |

#### uapi_uart_write()

| **定义** | errcode_t uapi_uart_write(uint8_t id, const uint8_t *data, uint16_t length, uint16_t timeout); |
|---------|-----------------------------------------------------------------------------------------------|
| **功能** | UART 发送数据 |
| **参数** | id：UART ID<br/>data：数据缓冲区<br/>length：数据长度<br/>timeout：超时时间 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/uart.h |

### SLE 接口

#### enable_sle()

| **定义** | errcode_t enable_sle(void); |
|---------|-----------------------------|
| **功能** | 使能 SLE 星闪 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | sle_common.h |

#### sle_start_seek()

| **定义** | errcode_t sle_start_seek(void); |
|---------|--------------------------------|
| **功能** | 启动扫描 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | sle_device_discovery.h |

#### ssapc_write_req()

| **定义** | errcode_t ssapc_write_req(uint8_t client_id, uint16_t conn_id, const ssapc_write_param_t *param); |
|---------|-----------------------------------------------------------------------------------------------|
| **功能** | SLE 客户端写入数据 |
| **参数** | client_id：客户端 ID<br/>conn_id：连接 ID<br/>param：写入参数 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | sle_ssap_client.h |

#### ssaps_notify_indicate()

| **定义** | errcode_t ssaps_notify_indicate(uint8_t server_id, uint16_t conn_id, const ssaps_ntf_ind_t *param); |
|---------|-----------------------------------------------------------------------------------------------|
| **功能** | SLE 服务端通知数据 |
| **参数** | server_id：服务端 ID<br/>conn_id：连接 ID<br/>param：通知参数 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | sle_ssap_server.h |

## LED 示例实现

###  具体实现

步骤一：设置 GPIO 引脚为输出模式

步骤二：设置 GPIO 初始电平为低

步骤三：循环翻转 GPIO 电平状态，实现 LED 闪烁

###  核心代码

```c
// 配置 LED 引脚为输出模式
errcode_t ret = uapi_pin_set_mode(CONFIG_LED_PIN, PIN_MODE_2);
ret = uapi_gpio_set_dir(CONFIG_LED_PIN, GPIO_DIRECTION_OUTPUT);
ret = uapi_gpio_set_val(CONFIG_LED_PIN, GPIO_LEVEL_LOW);

// 主循环
while (1) {
    osal_msleep(LED_DURATION_MS);
    uapi_gpio_toggle(CONFIG_LED_PIN);
    osal_printk("~LED toggled~\r\n");
}
```

## PWM LED 示例实现

###  具体实现

步骤一：设置 GPIO 引脚为 PWM 模式

步骤二：初始化 PWM 模块

步骤三：配置 PWM 参数并打开通道

步骤四：循环改变占空比实现呼吸灯效果

###  核心代码

```c
// PWM 配置
pwm_config_t pwm_cfg = {
    .low_time = PWM_LOW_TIME,
    .high_time = PWM_HIGH_TIME,
    .offset_time = 0,
    .cycles = 0,
    .repeat = true
};

// 主循环 - 呼吸灯效果
while (1) {
    pwm_cfg.high_time = (duty * t) / t;
    pwm_cfg.low_time = t - pwm_cfg.high_time;
    uapi_pwm_open(PWM_CHANNEL, &pwm_cfg);
    uapi_pwm_set_group(PWM_GROUP_ID, &channel_id, 1);
    uapi_pwm_start_group(PWM_GROUP_ID);
    duty -= step;
    if (duty >= PWM_HIGH_TIME || duty <= 0) {
        step = -step;
    }
    osal_msleep(BREATH_DELAY_MS);
    uapi_pwm_close(PWM_GROUP_ID);
}
```

## Key 示例实现

### 具体实现

步骤一：设置 GPIO 引脚为输入模式

步骤二：注册 GPIO 双向边沿中断回调函数

步骤三：使能 GPIO 中断

###  核心代码

```c
// 配置按键引脚
errcode_t ret = uapi_pin_set_mode(CONFIG_KEY_PIN, PIN_MODE_0);
ret = uapi_gpio_set_dir(CONFIG_KEY_PIN, GPIO_DIRECTION_INPUT);

// 注册中断回调（双向边沿触发）
ret = uapi_gpio_register_isr_func(CONFIG_KEY_PIN, GPIO_INTERRUPT_DEDGE, gpio_callback_func);

// 使能中断
ret = uapi_gpio_enable_interrupt(CONFIG_KEY_PIN);

// 中断回调函数
static void gpio_callback_func(pin_t pin, uintptr_t param)
{
    osal_printk("Key(pin%d) is pressed.\r\n", pin);
}
```

## SLE UART 示例实现

###  具体实现

#### Server 端

步骤一：初始化 UART 引脚和参数

步骤二：注册 SLE 服务端回调

步骤三：添加服务和属性

步骤四：启动广播

#### Client 端

步骤一：初始化 UART 引脚和参数

步骤二：注册 SLE 客户端回调

步骤三：启动扫描并连接服务端

步骤四：收发 UART 数据

###  核心代码

```c
// UART 初始化
uart_attr_t attr = {
    .baud_rate = SLE_UART_BAUDRATE,  // 115200
    .data_bits = UART_DATA_BIT_8,
    .stop_bits = UART_STOP_BIT_1,
    .parity = UART_PARITY_NONE
};
uapi_uart_init(CONFIG_SLE_UART_BUS, &pin_config, &attr, NULL, &g_app_uart_buffer_config);

// 注册 UART 接收回调
uapi_uart_register_rx_callback(CONFIG_SLE_UART_BUS,
    UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
    1, uart_rx_handler);
```

## 实验流程

- **步骤一**：在 `application/samples/peripheral` 文件夹下创建示例文件夹，名称为 `led`（以 LED 示例为例）

- **步骤二**：将 `vendor/Lierda-FB36/demo/led` 文件夹内容拷贝到创建的示例文件夹中

- **步骤三**：在 `application/samples/peripheral/CMakeLists.txt` 中在最后set(....)的上一行加新增编译配置

  ```c
  if(DEFINED CONFIG_SAMPLE_SUPPORT_LED)
  
    add_subdirectory_if_exist(led)
  
  endif()
  ```

- **步骤四**：在 `application/samples/peripheral/Kconfig` 中新增 Kconfig 配置

- ```
  config SAMPLE_SUPPORT_LED
      bool
      prompt "Support LED Sample."
      default n
      depends on ENABLE_PERIPHERAL_SAMPLE
      help
          This option means support LED Sample.
  
  if SAMPLE_SUPPORT_LED
  menu "LED Sample Configuration"
      osource "application/samples/peripheral/led/Kconfig"
  endmenu
  endif
  ```

- **步骤五**：点击系统配置图标，选择 KConfig，找到路径 `Application/Enable the Sample of peripheral`，在弹出框中选择对应示例（如 `support LED Sample`），点击 Save 保存后关闭

- **步骤六**：点击 `build` 或 `rebuild` 编译

- **步骤七**：编译完成后，点击""端口设置传输方式选择 `serial`，端口选择 `comxxx`，点击 "程序加载" 按钮烧录

- **步骤八**：烧录出现 "Connecting, please reset device..." 字样时，复位开发板

- **步骤九**：观察实验现象

  - LED 示例：LED 灯循环闪烁
  - PWM LED 示例：LED 灯呈现呼吸灯效果
  - Key 示例：按下按键串口输出按键信息
  - SLE UART 示例：双机进行 SLE UART 数据传输

## 注意事项

- 不同示例使用的引脚不同，请参考 `Kconfig` 文件中的默认引脚配置
- SLE UART 示例需要两台开发板配合使用，一台作为 Server，一台作为 Client
- 进行 PWM 输出时需要确认引脚支持 PWM 功能
- 按键中断使用双向边沿触发，可以检测按下和释放事件
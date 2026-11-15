# BearPi-Pico H3863 开发板示例

## 介绍

### 功能介绍

BearPi-Pico H3863 开发板示例集合了多种外设演示案例和通信应用，包括 GPIO 输出、按键中断、PWM 输出、串口通信、I2C 通信、SPI 通信、ADC 采集、定时器、看门狗、Systick 延时以及 BLE/UART、SLE/UART、SLE网关、WiFi联网等多种通信功能。

| 示例 | 功能描述 |
|------|---------|
| Blinky | GPIO 输出控制 LED 循环闪烁 |
| Button | GPIO 按键中断检测 |
| PWM | PWM 波输出控制 |
| UART | 串口数据收发（轮询/中断模式） |
| I2C | I2C master 读写 + OLED 显示 |
| SPI | SPI master 读写 + OLED 显示 |
| ADC | ADC 电压采集 |
| Timer | 定时器周期性任务 |
| Watchdog | 看门狗超时与喂狗 |
| Systick | Systick 微秒延时 |
| Tasks | 多任务创建与调度 |
| TCXO | TCXO 时钟输出 |
| BLE UART | BLE UART 透传 Server/Client |
| SLE UART | SLE UART 透传 Server/Client |
| SLE Gateway | SLE 星闪网关 |
| WiFi STA | WiFi Station 模式连接 |
| WiFi SoftAP | WiFi 热点模式 |
| UDP Client | UDP 客户端通信 |

### 软件概述

- **Blinky 示例**：通过 GPIO 引脚输出高低电平，控制 LED 灯循环亮灭
- **Button 示例**：通过 GPIO 双向边沿中断检测按键按下
- **PWM 示例**：通过 PWM 输出方波，可用于控制电机速度
- **UART 示例**：支持轮询和中断两种模式进行串口数据收发
- **I2C 示例**：I2C master 模式读写传感器，通过 SSD1306 OLED 显示信息
- **SPI 示例**：SPI master 模式读写，通过 SSD1306 OLED 显示信息
- **ADC 示例**：读取指定通道的 ADC 电压值
- **Timer 示例**：使用 Timer 实现周期性任务
- **Watchdog 示例**：初始化看门狗、喂狗、获取剩余超时时间
- **Systick 示例**：使用 Systick 实现微秒级延时
- **Tasks 示例**：创建多个任务，展示 RTOS 多任务调度
- **TCXO 示例**：配置 TCXO 时钟输出
- **BLE UART 示例**：基于 BLE 的 UART 透传功能
- **SLE UART 示例**：基于 SLE 星闪协议的 UART 透传功能
- **SLE Gateway 示例**：SLE 星闪网关应用
- **WiFi STA 示例**：连接无线路由器
- **WiFi SoftAP 示例**：创建 WiFi 热点
- **UDP Client 示例**：UDP 客户端数据收发

### 硬件概述

BearPi-Pico H3863 开发板基于海思 WS63 系列芯片，详情请参考开发板相关硬件文档。

## 约束与限制

### 支持应用运行的芯片和开发板

本示例支持开发板：**BearPi-Pico H3863**

### 支持 API 版本、SDK 版本

本示例支持版本号：**1.10.101 及以上**

### 支持 IDE 插件版本

本示例支持 IDE 插件版本号：**1.0.1 及以上**

## 目录结构

```
BearPi-Pico_H3863/
├── build_config.json               # 构建配置
├── doc/
│   └── media/                     # 媒体资源
│       └── BearPi-Pico_H3863/
│           ├── bearpi_pico_h3863.png
│           └── image.png
├── peripheral/                     # 外设示例
│   ├── adc/
│   │   ├── adc_demo.c
│   │   ├── CMakeLists.txt
│   │   └── Kconfig
│   ├── blinky/
│   │   ├── blinky_cmsis.c
│   │   ├── CMakeLists.txt
│   │   └── Kconfig
│   ├── button/
│   │   ├── button.c
│   │   ├── CMakeLists.txt
│   │   └── Kconfig
│   ├── i2c/
│   │   ├── i2c_master_demo.c
│   │   ├── ssd1306.c/.h
│   │   ├── ssd1306_fonts.c/.h
│   │   ├── CMakeLists.txt
│   │   └── Kconfig
│   ├── pinctrl/
│   │   ├── pinctrl_demo.c
│   │   ├── CMakeLists.txt
│   │   └── Kconfig
│   ├── pwm/
│   │   ├── pwm_demo.c
│   │   ├── CMakeLists.txt
│   │   └── Kconfig
│   ├── spi/
│   │   ├── spi_master_demo.c
│   │   ├── ssd1306.c/.h
│   │   ├── ssd1306_fonts.c/.h
│   │   ├── CMakeLists.txt
│   │   └── Kconfig
│   ├── systick/
│   │   ├── systick_demo.c
│   │   └── (no CMakeLists)
│   ├── tasks/
│   │   └── tasks.c
│   │   └── (no CMakeLists)
│   ├── tcxo/
│   │   └── tcxo_demo.c
│   │   └── (no CMakeLists)
│   ├── timer/
│   │   ├── timer_demo.c
│   │   ├── CMakeLists.txt
│   │   └── Kconfig
│   ├── uart/
│   │   ├── uart_demo.c
│   │   ├── CMakeLists.txt
│   │   └── Kconfig
│   ├── watchdog/
│   │   ├── watchdog_demo.c
│   │   ├── CMakeLists.txt
│   │   └── Kconfig
│   ├── CMakeLists.txt
│   └── Kconfig
├── products/                        # 产品应用
│   ├── ble_uart/                  # BLE UART 透传
│   │   ├── ble_uart.c
│   │   ├── ble_uart_client/
│   │   │   ├── ble_uart_client.c
│   │   │   ├── ble_uart_client.h
│   │   │   ├── ble_uart_client_scan.c
│   │   │   └── ble_uart_client_scan.h
│   │   ├── ble_uart_server/
│   │   │   ├── ble_uart_server.c
│   │   │   ├── ble_uart_server.h
│   │   │   ├── ble_uart_server_adv.c
│   │   │   └── ble_uart_server_adv.h
│   │   ├── CMakeLists.txt
│   │   └── Kconfig
│   ├── sle_uart/                 # SLE UART 透传
│   │   ├── sle_uart.c
│   │   ├── sle_uart_client/
│   │   │   ├── sle_uart_client.c
│   │   │   └── sle_uart_client.h
│   │   ├── sle_uart_server/
│   │   │   ├── sle_uart_server.c
│   │   │   ├── sle_uart_server.h
│   │   │   ├── sle_uart_server_adv.c
│   │   │   └── sle_uart_server_adv.h
│   │   ├── CMakeLists.txt
│   │   └── Kconfig
│   ├── sle_gateway/              # SLE 网关
│   │   ├── sle_gateway.c
│   │   ├── sle_uart_client/
│   │   │   ├── sle_uart_client.c
│   │   │   └── sle_uart_client.h
│   │   ├── sle_uart_server/
│   │   │   ├── sle_uart_server.c
│   │   │   ├── sle_uart_server.h
│   │   │   ├── sle_uart_server_adv.c
│   │   │   └── sle_uart_server_adv.h
│   │   ├── wifi/
│   │   │   ├── wifi_connect.c
│   │   │   └── wifi_connect.h
│   │   ├── CMakeLists.txt
│   │   └── Kconfig
│   ├── CMakeLists.txt
│   └── Kconfig
└── wifi/                           # WiFi 示例
    ├── softap_sample/
    │   └── softap_sample.c
    │   └── (no CMakeLists)
    ├── sta_sample/
    │   └── sta_sample.c
    │   └── (no CMakeLists)
    ├── udp_client/
    │   ├── udp_client.c
    │   └── wifi/
    │       ├── wifi_connect.c
    │       └── wifi_connect.h
    │   └── (no CMakeLists)
    ├── CMakeLists.txt
    └── Kconfig
```

## 效果预览

| 示例 | 效果 |
|------|------|
| Blinky | 开发板上的 LED 灯以配置间隔循环闪烁 |
| Button | 按下按键后串口输出按键信息 |
| PWM | PWM 通道输出方波信号 |
| UART | 串口发送 "Hello BearPi"，接收数据后回传 |
| I2C | I2C 读写传感器，OLED 显示信息 |
| SPI | SPI 读写传感器，OLED 显示信息 |
| ADC | 串口输出 ADC 采集的电压值 |
| Timer | 串口输出定时器周期性信息 |
| Watchdog | 看门狗初始化、喂狗、获取剩余时间 |
| Systick | 串口输出 Systick 延时计数 |
| Tasks | 多个任务轮转执行 |
| TCXO | TCXO 时钟输出配置 |
| BLE UART | BLE 连接后串口数据透传 |
| SLE UART | SLE 星闪连接后串口数据透传 |
| SLE Gateway | SLE 星闪与 WiFi 双向转发 |
| WiFi STA | 连接无线路由，获取 IP 地址 |
| WiFi SoftAP | 创建 WiFi 热点，其他设备连接 |
| UDP Client | UDP 数据收发测试 |

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

### PWM 接口

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

#### uapi_pwm_register_interrupt()

| **定义** | errcode_t uapi_pwm_register_interrupt(uint8_t channel, pwm_callback_t callback); |
|---------|------------------------------------------------------------------------------------|
| **功能** | 注册 PWM 中断回调函数 |
| **参数** | channel：PWM 通道号<br/>callback：回调函数 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/pwm.h |

### PIN 接口

#### uapi_pin_set_mode()

| **定义** | errcode_t uapi_pin_set_mode(pin_t pin, pin_mode_t mode); |
|---------|---------------------------------------------------------|
| **功能** | 设置引脚模式 |
| **参数** | pin：引脚号<br/>mode：模式（PIN_MODE_0 / PIN_MODE_1 / PIN_MODE_2 等） |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/pinctrl.h |

### UART 接口

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

#### uapi_uart_read()

| **定义** | int uapi_uart_read(uint8_t id, uint8_t *data, uint16_t length, uint16_t timeout); |
|---------|----------------------------------------------------------------------------------------|
| **功能** | UART 接收数据 |
| **参数** | id：UART ID<br/>data：数据缓冲区<br/>length：数据长度<br/>timeout：超时时间 |
| **返回值** | 成功读取的字节数 |
| **依赖** | driver/uart.h |

### I2C 接口

#### uapi_i2c_master_init()

| **定义** | errcode_t uapi_i2c_master_init(uint8_t bus_id, uint32_t baudrate, uint8_t hscode); |
|---------|--------------------------------------------------------------------------------------|
| **功能** | 初始化 I2C master |
| **参数** | bus_id：总线号<br/>baudrate：波特率<br/>hscode：高速码 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/i2c.h |

#### uapi_i2c_master_write()

| **定义** | errcode_t uapi_i2c_master_write(uint8_t bus_id, uint16_t dev_addr, const i2c_data_t *data); |
|---------|---------------------------------------------------------------------------------------------|
| **功能** | I2C master 写数据 |
| **参数** | bus_id：总线号<br/>dev_addr：设备地址<br/>data：写数据配置 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/i2c.h |

#### uapi_i2c_master_writeread()

| **定义** | errcode_t uapi_i2c_master_writeread(uint8_t bus_id, uint16_t dev_addr, const i2c_data_t *data); |
|---------|-----------------------------------------------------------------------------------------------|
| **功能** | I2C master 写后读 |
| **参数** | bus_id：总线号<br/>dev_addr：设备地址<br/>data：数据配置 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/i2c.h |

### SPI 接口

#### uapi_spi_init()

| **定义** | errcode_t uapi_spi_init(uint8_t bus_id, const spi_attr_t *attr); |
|---------|--------------------------------------------------------------|
| **功能** | 初始化 SPI |
| **参数** | bus_id：总线号<br/>attr：SPI 属性配置 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/spi.h |

#### uapi_spi_transfer()

| **定义** | errcode_t uapi_spi_transfer(uint8_t bus_id, const spi_transfer_t *transfer); |
|---------|-----------------------------------------------------------------------------|
| **功能** | SPI 数据传输 |
| **参数** | bus_id：总线号<br/>transfer：传输配置 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/spi.h |

### ADC 接口

#### uapi_adc_init()

| **定义** | errcode_t uapi_adc_init(adc_clock_t clock); |
|---------|---------------------------------------------|
| **功能** | 初始化 ADC |
| **参数** | clock：ADC 时钟源 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/adc.h |

#### uapi_adc_read()

| **定义** | errcode_t uapi_adc_read(uint8_t channel, uint16_t *data); |
|---------|------------------------------------------------------|
| **功能** | 读取 ADC 值 |
| **参数** | channel：ADC 通道<br/>data：读取结果 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/adc.h |

### Timer 接口

#### uapi_timer_init()

| **定义** | errcode_t uapi_timer_init(void); |
|---------|--------------------------------|
| **功能** | 初始化定时器模块 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/timer.h |

#### uapi_timer_create()

| **定义** | errcode_t uapi_timer_create(timer_index_t timer_index, timer_handle_t *handle); |
|---------|------------------------------------------------------------------------------------|
| **功能** | 创建定时器软件句柄 |
| **参数** | timer_index：定时器编号<br/>handle：定时器句柄输出 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/timer.h |

#### uapi_timer_start()

| **定义** | errcode_t uapi_timer_start(timer_handle_t handle, uint32_t timeout, timer_callback_t callback, uintptr_t data); |
|---------|---------------------------------------------------------------------------------------------------------------|
| **功能** | 启动定时器 |
| **参数** | handle：定时器句柄<br/>timeout：超时时间（微秒）<br/>callback：回调函数<br/>data：回调参数 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/timer.h |

### Watchdog 接口

#### uapi_watchdog_init()

| **定义** | errcode_t uapi_watchdog_init(chip_wdt_timeout_t timeout); |
|---------|----------------------------------------------------------|
| **功能** | 初始化看门狗 |
| **参数** | timeout：超时时间（chip_wdt_timeout_t 枚举值） |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/watchdog.h |

#### uapi_watchdog_enable()

| **定义** | errcode_t uapi_watchdog_enable(uint8_t mode); |
|---------|---------------------------------------------|
| **功能** | 使能看门狗 |
| **参数** | mode：模式（复位模式等） |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/watchdog.h |

#### uapi_watchdog_kick()

| **定义** | errcode_t uapi_watchdog_kick(void); |
|---------|-----------------------------------|
| **功能** | 喂狗 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/watchdog.h |

### Systick 接口

#### uapi_systick_init()

| **定义** | errcode_t uapi_systick_init(void); |
|---------|----------------------------------|
| **功能** | 初始化 Systick 模块 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/systick.h |

#### uapi_systick_delay_ms()

| **定义** | void uapi_systick_delay_ms(uint32_t ms); |
|---------|----------------------------------------|
| **功能** | Systick 毫秒延时 |
| **参数** | ms：延时时间（毫秒） |
| **返回值** | 无 |
| **依赖** | driver/systick.h |

#### uapi_systick_get_us()

| **定义** | uint64_t uapi_systick_get_us(void); |
|---------|-------------------------------------|
| **功能** | 获取当前 Systick 计数（微秒） |
| **返回值** | 当前计数（微秒） |
| **依赖** | driver/systick.h |

## Blinky 示例实现

### 具体实现

设置 GPIO 引脚为输出模式，然后设置 GPIO 初始电平为低，接着循环翻转 GPIO 电平状态实现 LED 闪烁。

### 核心代码

```c
uapi_pin_set_mode(CONFIG_BLINKY_PIN, PIN_MODE_0);
uapi_gpio_set_dir(CONFIG_BLINKY_PIN, GPIO_DIRECTION_OUTPUT);
uapi_gpio_set_val(CONFIG_BLINKY_PIN, GPIO_LEVEL_LOW);

while (1) {
    osal_msleep(CONFIG_BLINKY_DURATION_MS);
    uapi_gpio_toggle(CONFIG_BLINKY_PIN);
}
```

## Button 示例实现

### 具体实现

初始化 GPIO 引脚为输入模式，然后注册 GPIO 双向边沿中断回调函数，接着使能 GPIO 中断。

### 核心代码

```c
uapi_pin_set_mode(CONFIG_BUTTON_PIN, PIN_MODE_0);
uapi_gpio_set_dir(CONFIG_BUTTON_PIN, GPIO_DIRECTION_INPUT);
uapi_gpio_register_isr_func(CONFIG_BUTTON_PIN, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)gpio_callback_func);
uapi_gpio_enable_interrupt(CONFIG_BUTTON_PIN);
```

## PWM 示例实现

### 具体实现

设置 GPIO 引脚为 PWM 模式，然后初始化 PWM 模块，配置 PWM 参数并打开通道，注册中断回调，最后启动 PWM 组输出。

### 核心代码

```c
pwm_config_t cfg_no_repeat = {50, 200, 0, 0, true};

uapi_pin_set_mode(PWM_GPIO, PWM_PIN_MODE);
uapi_pwm_deinit();
uapi_pwm_init();
uapi_pwm_open(PWM_CHANNEL, &cfg_no_repeat);
uapi_pwm_register_interrupt(PWM_CHANNEL, pwm_sample_callback);

uint8_t channel_id = PWM_CHANNEL;
uapi_pwm_set_group(PWM_GROUP_ID, &channel_id, 1);
uapi_pwm_start_group(PWM_GROUP_ID);
```

## UART 示例实现

### 具体实现

初始化 UART 引脚和参数，然后根据配置选择轮询模式或中断模式进行数据收发。

### 核心代码

```c
uart_attr_t attr = {
    .baud_rate = UART_BAUDRATE,
    .data_bits = UART_DATA_BITS,
    .stop_bits = UART_STOP_BITS,
    .parity = UART_PARITY_BIT
};

uart_pin_config_t pin_config = {
    .tx_pin = CONFIG_UART_TXD_PIN,
    .rx_pin = CONFIG_UART_RXD_PIN,
    .cts_pin = PIN_NONE,
    .rts_pin = PIN_NONE
};

uapi_uart_deinit(CONFIG_UART_BUS_ID);
uapi_uart_init(CONFIG_UART_BUS_ID, &pin_config, &attr, NULL, &g_app_uart_buffer_config);

// 轮询模式发送
uapi_uart_write(CONFIG_UART_BUS_ID, g_app_uart_tx_buff, UART_TRANSFER_SIZE, 0);

// 中断模式接收
uapi_uart_register_rx_callback(CONFIG_UART_BUS_ID, UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
                              1, app_uart_read_int_handler);
```

## I2C 示例实现

### 具体实现

初始化 I2C 总线，配置 SSD1306 OLED 显示器，然后进行 I2C 读写操作并在 OLED 上显示信息。

### 核心代码

```c
uapi_i2c_master_init(CONFIG_I2C_MASTER_BUS_ID, 400000, 0);

// SSD1306 OLED 初始化
ssd1306_Init();

// 写入数据
i2c_data_t i2c_data = {
    .send_buf = data,
    .send_len = len
};
uapi_i2c_master_write(CONFIG_I2C_MASTER_BUS_ID, SSD1306_I2C_ADDR, &i2c_data);
```

## SPI 示例实现

### 具体实现

初始化 SPI 总线，配置 SSD1306 OLED 显示器，然后进行 SPI 数据传输并在 OLED 上显示信息。

### 核心代码

```c
spi_attr_t spi_attr = {
    .mode = SPI_MODE_MASTER,
    .baud_rate = 1000000,
    .bits = 8,
    .polar = 0,
    .phase = 0
};

uapi_spi_init(CONFIG_SPI_MASTER_BUS_ID, &spi_attr);

spi_transfer_t transfer = {
    .tx_buf = tx_data,
    .rx_buf = rx_data,
    .len = len
};
uapi_spi_transfer(CONFIG_SPI_MASTER_BUS_ID, &transfer);
```

## ADC 示例实现

### 具体实现

初始化 ADC 模块，读取指定通道的 ADC 值并打印。

### 核心代码

```c
uapi_adc_init(ADC_CLOCK_NONE);

uint16_t adc_value = 0;
uapi_adc_read(CONFIG_ADC_CHANNEL, &adc_value);
osal_printk("ADC value = %d\r\n", adc_value);
```

## Timer 示例实现

### 具体实现

初始化定时器模块，配置定时器硬件参数，创建定时器句柄，启动定时器并在回调中输出信息。

### 核心代码

```c
uapi_timer_init();
uapi_timer_adapter(TIMER_INDEX, TIMER_1_IRQN, TIMER_PRIO);
uapi_timer_create(TIMER_INDEX_1, &timer1_handle);
uapi_timer_start(timer1_handle, TIMER1_DELAY_1000 * TIMER1_DELAY_1000US, timer_timeout_callback, 0);

static void timer_timeout_callback(uintptr_t data)
{
    unused(data);
    osal_printk("------1s------\r\n");
}
```

## Watchdog 示例实现

### 具体实现

初始化看门狗超时时间，使能看门狗，延时后喂狗，获取剩余超时时间。

### 核心代码

```c
uapi_watchdog_init(CHIP_WDT_TIMEOUT_32S);
uapi_watchdog_enable(WDT_MODE_RESET);
osal_mdelay(5000);
uapi_watchdog_kick();

uint32_t sample_remain_ms;
uapi_watchdog_get_left_time(&sample_remain_ms);
osal_printk("sample_remain_ms = %x! \n", sample_remain_ms);
```

## 实验流程

- **步骤一**：在 `application/samples/peripheral` 文件夹下创建示例文件夹，名称为 `blinky`（以 Blinky 示例为例）

- **步骤二**：将 `vendor/BearPi-Pico_H3863/peripheral/blinky` 文件夹内容拷贝到创建的示例文件夹中

- **步骤三**：在 `application/samples/peripheral/CMakeLists.txt` 中在最后set(...)的上一行加新增编译配置

  ```c
  if(DEFINED CONFIG_SAMPLE_SUPPORT_BLINKY)

    add_subdirectory_if_exist(blinky)

  endif()
  ```

- **步骤四**：在 `application/samples/peripheral/Kconfig` 中新增 Kconfig 配置

  ```
  config SAMPLE_SUPPORT_BLINKY
      bool
      prompt "Support BLINKY Sample."
      default n
      depends on ENABLE_PERIPHERAL_SAMPLE
      help
          This option means support BLINKY Sample.

  if SAMPLE_SUPPORT_BLINKY
  menu "BLINKY Sample Configuration"
      osource "application/samples/peripheral/blinky/Kconfig"
  endmenu
  endif
  ```

- **步骤五**：点击系统配置图标，选择 KConfig，找到路径 `Application/Enable the Sample of peripheral`，在弹出框中选择对应示例（如 `support BLINKY Sample`），点击 Save 保存后关闭

- **步骤六**：点击 `build` 或 `rebuild` 编译

- **步骤七**：编译完成后，点击 ""端口设置传输方式选择 `serial`，端口选择 `comxxx`，点击 "程序加载" 按钮烧录

- **步骤八**：烧录出现 "Connecting, please reset device..." 字样时，复位开发板

- **步骤九**：观察实验现象

  - Blinky 示例：LED 灯循环闪烁
  - Button 示例：按下按键串口输出按键信息
  - PWM 示例：PWM 输出方波
  - UART 示例：串口发送接收数据
  - I2C 示例：OLED 显示信息
  - SPI 示例：OLED 显示信息
  - ADC 示例：串口输出 ADC 电压值
  - Timer 示例：串口每秒输出信息
  - Watchdog 示例：串口输出看门狗信息
  - Systick 示例：串口输出延时计数

## 注意事项

- 不同示例使用的引脚不同，请参考各示例中的引脚配置定义
- 进行 PWM 输出时需要确认引脚支持 PWM 功能
- 按键中断使用双向边沿触发，可以检测按下和释放事件
- I2C 和 SPI 示例需要正确连接 OLED 屏幕
- BLE 和 SLE 示例需要两台开发板配合使用
- WiFi 示例需要正确配置 SSID 和密码
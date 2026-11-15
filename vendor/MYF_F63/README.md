# MYF-F63 开发板示例

## 介绍

### 功能介绍

MYF-F63 开发板示例集合了多种外设演示案例，包括 GPIO 输出、按键中断、PWM 呼吸灯、串口通信、DMA 传输、定时器、看门狗以及 Systick 延时等功能。

| 示例 | 功能描述 |
|------|---------|
| Blinky | GPIO 输出控制 LED 循环闪烁 |
| Button | GPIO 按键中断检测 |
| PWM | PWM 控制实现呼吸灯效果 |
| UART | 串口数据收发 |
| DMA | 串口 DMA 模式数据传输 |
| Timer | 定时器周期性任务 |
| Watchdog | 看门狗超时与喂狗 |
| Systick | Systick 微秒延时 |

### 软件概述

- **Blinky 示例**：通过 GPIO 引脚输出高低电平，控制 LED 灯循环亮灭，间隔 1 秒
- **Button 示例**：通过 GPIO 双向边沿中断检测按键按下，串口输出按键信息
- **PWM 示例**：通过 PWM 输出实现占空比渐变，产生呼吸灯效果
- **UART 示例**：串口 1 接收数据后发送到串口 0，支持中断模式
- **DMA 示例**：通过 DMA 方式实现串口数据收发，提高 CPU 效率
- **Timer 示例**：使用 Timer1 实现 1 秒周期的定时任务
- **Watchdog 示例**：初始化看门狗、喂狗、获取剩余超时时间
- **Systick 示例**：使用 Systick 实现微秒级延时

### 硬件概述

MYF-F63 开发板基于海思 WS63 系列芯片，详情请参考开发板相关硬件文档。

## 约束与限制

### 支持应用运行的芯片和开发板

本示例支持开发板：**MYF-F63**

### 支持 API 版本、SDK 版本

本示例支持版本号：**1.10.101 及以上**

### 支持 IDE 插件版本

本示例支持 IDE 插件版本号：**1.0.1 及以上**

## 目录结构

```
MYF_F63/
├── build_config.json               # 构建配置
├── doc/
│   └── media/                      # 媒体资源
│       ├── MYF-F63AI01开发板.png
│       └── MYF-F63VA01开发板.png
├── peripheral/                      # 外设示例
│   ├── blinky/                     # LED 闪烁示例
│   │   ├── led.c
│   │   └── CMakeLists.txt
│   ├── button/                    # 按键中断示例
│   │   ├── button.c
│   │   └── CMakeLists.txt
│   ├── dma/                        # DMA 传输示例
│   │   ├── dma_demo.c
│   │   └── CMakeLists.txt
│   ├── pwm/                        # PWM 呼吸灯示例
│   │   ├── pwm_demo.c
│   │   └── CMakeLists.txt
│   ├── systick/                   # Systick 延时示例
│   │   ├── systick_demo.c
│   │   └── CMakeLists.txt
│   ├── timer/                     # 定时器示例
│   │   ├── timer_demo.c
│   │   └── CMakeLists.txt
│   ├── uart/                      # 串口示例
│   │   ├── uart_demo.c
│   │   └── CMakeLists.txt
│   ├── watchdog/                  # 看门狗示例
│   │   ├── watchdog_demo.c
│   │   └── CMakeLists.txt
│   ├── CMakeLists.txt
│   └── Kconfig
└── products/                       # 产品示例
    └── at_test/                    # AT 指令测试
        ├── CMakeLists.txt
        ├── at_test.c
        ├── at_test.h
        ├── sle_client/
        │   ├── sle_uuid_client.c
        │   └── sle_uuid_client.h
        └── sle_server/
            ├── sle_server_adv.c
            ├── sle_server_adv.h
            ├── sle_uuid_server.c
            └── sle_uuid_server.h
```

## 效果预览

| 示例 | 效果 |
|------|------|
| Blinky | 开发板上的 LED 灯以 1 秒间隔循环闪烁 |
| Button | 按下按键后串口输出 "PIN:xx interrupt success." |
| PWM | LED 灯呈现平滑的呼吸灯渐变效果 |
| UART | 串口 1 接收数据后发送到串口 0 |
| DMA | 串口 DMA 模式收发数据，回环测试 |
| Timer | 串口每秒输出 "------1s------" |
| Watchdog | 看门狗初始化、喂狗、获取剩余时间 |
| Systick | 串口输出延时计数差值 |

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

#### uapi_pwm_close()

| **定义** | errcode_t uapi_pwm_close(uint8_t group_id); |
|---------|--------------------------------------------|
| **功能** | 关闭 PWM 组 |
| **参数** | group_id：组 ID |
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

#### uapi_uart_read_by_dma()

| **定义** | errcode_t uapi_uart_read_by_dma(uint8_t id, uint8_t *buf, uint16_t len, const uart_write_dma_config_t *config); |
|---------|-----------------------------------------------------------------------------------------------------------------|
| **功能** | UART DMA 模式读取数据 |
| **参数** | id：UART ID<br/>buf：数据缓冲区<br/>len：数据长度<br/>config：DMA 配置 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/uart.h |

#### uapi_uart_write_by_dma()

| **定义** | errcode_t uapi_uart_write_by_dma(uint8_t id, const uint8_t *data, uint16_t length, const uart_write_dma_config_t *config); |
|---------|----------------------------------------------------------------------------------------------------------------------|
| **功能** | UART DMA 模式发送数据 |
| **参数** | id：UART ID<br/>data：数据缓冲区<br/>length：数据长度<br/>config：DMA 配置 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/uart.h |

### Timer 接口

#### uapi_timer_init()

| **定义** | errcode_t uapi_timer_init(void); |
|---------|--------------------------------|
| **功能** | 初始化定时器模块 |
| **返回值** | ERRCODE_SUCC：成功 Other：失败 |
| **依赖** | driver/timer.h |

#### uapi_timer_adapter()

| **定义** | errcode_t uapi_timer_adapter(uint8_t timer_index, uint16_t irq_line, uint8_t priority); |
|---------|---------------------------------------------------------------------------------------|
| **功能** | 配置定时器硬件参数 |
| **参数** | timer_index：定时器编号<br/>irq_line：中断号<br/>priority：优先级 |
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

#### uapi_watchdog_get_left_time()

| **定义** | errcode_t uapi_watchdog_get_left_time(uint32_t *time); |
|---------|------------------------------------------------------|
| **功能** | 获取剩余超时时间 |
| **参数** | time：剩余时间输出（毫秒） |
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
// 配置 LED 引脚为输出模式
uapi_pin_set_mode(CONFIG_BLINKY_PIN, HAL_PIO_FUNC_GPIO);
uapi_gpio_set_dir(CONFIG_BLINKY_PIN, GPIO_DIRECTION_OUTPUT);
uapi_gpio_set_val(CONFIG_BLINKY_PIN, GPIO_LEVEL_LOW);

// 主循环
while (1) {
    osal_msleep(BLINKY_DURATION_MS);  // 延时 1 秒
    uapi_gpio_toggle(CONFIG_BLINKY_PIN);
    osal_printk("Blinky working.\r\n");
}
```

## Button 示例实现

### 具体实现

初始化 GPIO 引脚为输入模式，然后注册 GPIO 双向边沿中断回调函数，接着使能 GPIO 中断。

### 核心代码

```c
// 配置按键引脚
uapi_pin_init();
uapi_gpio_init();
uapi_pin_set_mode(BUTTON_GPIO, 0);
uapi_gpio_set_dir(BUTTON_GPIO, GPIO_DIRECTION_INPUT);

// 注册中断回调（双向边沿触发）
uapi_gpio_register_isr_func(BUTTON_GPIO, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)NetCfgCallbackFunc);
uapi_gpio_enable_interrupt(BUTTON_GPIO);

// 中断回调函数
void NetCfgCallbackFunc(pin_t pin, uintptr_t param)
{
    osal_printk("PIN:%d interrupt success.\r\n", pin);
}
```

## PWM 示例实现

### 具体实现

设置 GPIO 引脚为 PWM 模式，然后初始化 PWM 模块，接着配置 PWM 参数并打开通道，最后循环改变占空比实现呼吸灯效果。

### 核心代码

```c
// PWM 配置
pwm_config_t cfg_repeat = {50, 300, 0, 0, true};

uapi_pin_set_mode(BREATHING_LIGHT_PIN, BREATHING_LIGHT_PIN_MODE);

// 主循环 - 呼吸灯效果
while (1) {
    uapi_pwm_deinit();
    uapi_pwm_init();

    cfg_repeat.low_time = brightness;
    cfg_repeat.high_time = BREATHING_LIGHT_MAX_TIME - brightness;
    uapi_pwm_open(BREATHING_LIGHT_CHANNEL, &cfg_repeat);

    uint8_t channel_id = BREATHING_LIGHT_CHANNEL;
    uapi_pwm_set_group(BREATHING_LIGHT_GROUP_ID, &channel_id, 1);
    uapi_pwm_start_group(BREATHING_LIGHT_GROUP_ID);

    brightness += step;
    if (brightness >= BREATHING_LIGHT_MAX_TIME || brightness <= BREATHING_LIGHT_MIN_TIME) {
        step = -step;
    }

    uapi_tcxo_delay_ms(DELAY_TIME);
    uapi_pwm_close(BREATHING_LIGHT_GROUP_ID);
    uapi_pwm_deinit();
}
```

## UART 示例实现

### 具体实现

初始化 UART 引脚和参数，然后注册 UART 接收回调，接着在回调中将串口 1 接收的数据发送到串口 0。

### 核心代码

```c
// UART 配置
uart_attr_t attr = {
    .baud_rate = UART_BAUDRATE,
    .data_bits = UART_DATA_BIT_8,
    .stop_bits = UART_STOP_BIT_1,
    .parity = UART_PARITY_NONE
};

uart_pin_config_t pin_config = {
    .tx_pin = UART_TXD_PIN, .rx_pin = UART_RXD_PIN, .cts_pin = PIN_NONE, .rts_pin = PIN_NONE
};

uapi_uart_deinit(CONFIG_UART_BUS_ID);
uapi_uart_init(CONFIG_UART_BUS_ID, &pin_config, &attr, NULL, &g_app_uart_buffer_config);

// 注册串口接收中断
uapi_uart_register_rx_callback(CONFIG_UART_BUS_ID, UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
                                CONFIG_UART_TRANSFER_SIZE, app_uart_read_int_handler);

// 接收回调函数
static void app_uart_read_int_handler(const void *buffer, uint16_t length, bool error)
{
    unused(error);
    uapi_uart_write(UART_BUS_0, buffer, length, 0);
}
```

## DMA 示例实现

### 具体实现

初始化 UART 和 DMA 模块，配置 UART 为 DMA 模式，然后循环进行 DMA 收发测试。

### 核心代码

```c
// DMA 配置
uart_extra_attr_t extra_attr = {
    .tx_dma_enable = true,
    .tx_int_threshold = UART_FIFO_INT_TX_LEVEL_EQ_0_CHARACTER,
    .rx_dma_enable = true,
    .rx_int_threshold = UART_FIFO_INT_RX_LEVEL_1_CHARACTER
};

uapi_dma_init();
uapi_dma_open();
uapi_uart_init(CONFIG_UART_BUS_ID, &pin_config, &attr, &extra_attr, &g_app_uart_buffer_config);

// 主循环 - DMA 收发测试
while (1) {
    if (uapi_uart_read_by_dma(CONFIG_UART_BUS_ID, g_app_uart_rx_buff, CONFIG_UART_TRANSFER_SIZE, &g_app_dma_cfg) == ERRCODE_SUCC) {
        osal_printk("uart%d dma mode receive succ!\r\n", CONFIG_UART_BUS_ID);
    }
    if (uapi_uart_write_by_dma(CONFIG_UART_BUS_ID, g_app_uart_rx_buff, CONFIG_UART_TRANSFER_SIZE, &g_app_dma_cfg) == ERRCODE_SUCC) {
        osal_printk("uart%d dma mode send back succ!\r\n", CONFIG_UART_BUS_ID);
    }
}
```

## Timer 示例实现

### 具体实现

初始化定时器模块，配置 Timer1 硬件参数，创建定时器句柄，启动定时器并在回调中输出信息。

### 核心代码

```c
// 定时器回调函数
static void timer_timeout_callback(uintptr_t data)
{
    unused(data);
    osal_printk("------1s------\r\n");
    uapi_timer_start(timer1_handle, TIMER1_DELAY_1000 * TIMER1_DELAY_1000US, timer_timeout_callback, 0);
}

// 定时器任务
uapi_timer_init();
uapi_timer_adapter(TIMER_INDEX, TIMER_1_IRQN, TIMER_PRIO);
uapi_timer_create(TIMER_INDEX_1, &timer1_handle);
uapi_timer_start(timer1_handle, TIMER1_DELAY_1000 * TIMER1_DELAY_1000US, timer_timeout_callback, 0);
```

## Watchdog 示例实现

### 具体实现

初始化看门狗超时时间，使能看门狗，延时后喂狗，获取剩余超时时间，最后关闭看门狗。

### 核心代码

```c
// 设置看门狗超时时间
uapi_watchdog_init(CHIP_WDT_TIMEOUT_32S);
// 使能看门狗
uapi_watchdog_enable(WDT_MODE_RESET);
// 延时 5 秒
osal_mdelay(5000);
// 喂狗
uapi_watchdog_kick();
// 获取剩余超时时间
uint32_t sample_remain_ms;
uapi_watchdog_get_left_time(&sample_remain_ms);
osal_printk("sample_remain_ms = %x! \n", sample_remain_ms);
// 关闭看门狗
uapi_watchdog_disable();
```

## Systick 示例实现

### 具体实现

初始化 Systick 模块，获取延时前计数，执行延时，获取延时后计数，计算差值并打印。

### 核心代码

```c
// Systick 模块初始化
uapi_systick_init();

// 通过 count 数差值验证延迟时间
uint64_t count_before_get_us = uapi_systick_get_us();
uapi_systick_delay_ms(SYSTICK_DELAY_MS);
uint64_t count_after_get_us = uapi_systick_get_us();
osal_printk("count_before_get_us = %llu, count_after_get_us = %llu\r\n", count_before_get_us, count_after_get_us);
osal_printk("test case delay count %llu.\r\n", count_after_get_us - count_before_get_us);
```

## 实验流程

- **步骤一**：在 `application/samples/peripheral` 文件夹下创建示例文件夹，名称为 `blinky`（以 Blinky 示例为例）

- **步骤二**：将 `vendor/MYF_F63/peripheral/blinky` 文件夹内容拷贝到创建的示例文件夹中

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
  - PWM 示例：LED 灯呈现呼吸灯效果
  - UART 示例：串口 1 发数据，串口 0 回传
  - DMA 示例：串口 DMA 模式收发数据
  - Timer 示例：串口每秒输出定时信息
  - Watchdog 示例：串口输出看门狗信息
  - Systick 示例：串口输出延时计数

## 注意事项

- 不同示例使用的引脚不同，请参考各示例中的引脚配置定义
- 进行 PWM 输出时需要确认引脚支持 PWM 功能
- 按键中断使用双向边沿触发，可以检测按下和释放事件
- DMA 示例需要确保 UART 硬件支持 DMA 模式
- 看门狗使能后需要定期喂狗，否则会导致复位
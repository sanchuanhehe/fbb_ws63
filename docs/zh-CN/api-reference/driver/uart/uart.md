# UART

UART (Universal Asynchronous Receiver/Transmitter) 异步串行通信，TX/RX/GND 三线制，无需时钟线（双方约定波特率）。WS63 UART 支持三种传输模式：轮询、中断和 DMA (Direct Memory Access)。

DMA 模式下零 CPU 开销收发大批量数据，RX (Receive) 回调可按字节数或超时触发。配置流程：引脚复用 → 波特率/数据位/停止位/校验位 → 中断/DMA 使能。适用串口日志、AT 指令、透传模块等场景。

**头文件清单**

```c
#include "include/driver/uart.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_uart_init](#uapi_uart_init) | 初始化指定的UART串口 |
| [uapi_uart_deinit](#uapi_uart_deinit) | 去初始化指定的UART串口 |
| [uapi_uart_set_attr](#uapi_uart_set_attr) | 设置UART基础配置参数 |
| [uapi_uart_get_attr](#uapi_uart_get_attr) | 获取UART基础配置参数 |
| [uapi_uart_has_pending_transmissions](#uapi_uart_has_pending_transmissions) | 判断是否存在正在等待的传输 |
| [uapi_uart_rx_fifo_is_empty](#uapi_uart_rx_fifo_is_empty) | 判断RX FIFO (First-In First-Out) 是否为空 |
| [uapi_uart_tx_fifo_is_empty](#uapi_uart_tx_fifo_is_empty) | 判断TX (Transmit) FIFO是否为空 |
| [uapi_uart_register_rx_callback](#uapi_uart_register_rx_callback) | 注册接收数据回调函数 |
| [uapi_uart_unregister_rx_callback](#uapi_uart_unregister_rx_callback) | 去注册接收数据回调函数 |
| [uapi_uart_register_parity_error_callback](#uapi_uart_register_parity_error_callback) | 注册奇偶校验错误处理回调函数 |
| [uapi_uart_register_frame_error_callback](#uapi_uart_register_frame_error_callback) | 注册帧错误处理回调函数 |
| [uapi_uart_register_overrun_error_callback](#uapi_uart_register_overrun_error_callback) | 注册溢出错误处理回调函数 |
| [uapi_uart_write](#uapi_uart_write) | 轮询模式发送数据 |
| [uapi_uart_write_nolock](#uapi_uart_write_nolock) | 轮询模式发送数据（不锁中断） |
| [uapi_uart_write_int](#uapi_uart_write_int) | 中断模式发送数据 |
| [uapi_uart_write_by_dma](#uapi_uart_write_by_dma) | DMA模式发送数据 |
| [uapi_uart_read_by_dma](#uapi_uart_read_by_dma) | DMA模式接收数据 |
| [uapi_uart_read](#uapi_uart_read) | 轮询模式读取数据 |
| [uapi_uart_flush_rx_data](#uapi_uart_flush_rx_data) | 刷新UART接收Buffer中的数据 |
| [uapi_uart_update_rx_buff](#uapi_uart_update_rx_buff) | 更新UART接收Buffer的地址和长度 |
| [uapi_uart_suspend](#uapi_uart_suspend) | 挂起所有UART通道 |
| [uapi_uart_resume](#uapi_uart_resume) | 恢复所有UART通道 |

## Functions

### uapi_uart_init <a id="uapi_uart_init"></a>

```c
errcode_t uapi_uart_init(uart_bus_t bus, const uart_pin_config_t *pins, const uart_attr_t *attr, const uart_extra_attr_t *extra_attr, uart_buffer_config_t *uart_buffer_config)
```

**头文件清单**

```c
#include "include/driver/uart.h"
```

**功能说明**

- 初始化指定的UART串口，配置引脚、基础属性、扩展属性和接收缓冲区

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [uart_bus_t](#uart_bus_t) | UART总线号 | UART_BUS_0 ~ UART_BUS_2 |
| pins | const [uart_pin_config_t](#uart_pin_config_t) * | UART的引脚配置（TX/RX/RTS/CTS） | 不为NULL |
| attr | const [uart_attr_t](#uart_attr_t) * | 基础属性配置结构体指针 | 不为NULL |
| extra_attr | const [uart_extra_attr_t](#uart_extra_attr_t) * | 扩展属性配置结构体指针 | 可为NULL |
| uart_buffer_config | [uart_buffer_config_t](#uart_buffer_config_t) * | 接收缓冲区配置结构体指针 | - |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 初始化成功 |
| Other | 其他错误码，参考[errcode_t](#errcode_t) | 初始化失败 |

**参考案例**

- [uart_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/uart/uart_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_UART_SUPPORT_TX | 特性宏 | 启用UART发送支持 | y |
| CONFIG_UART_SUPPORT_RX | 特性宏 | 启用UART接收支持 | y |
| CONFIG_UART_SUPPORT_DMA | 特性宏 | 启用UART DMA支持 | y |

### uapi_uart_deinit <a id="uapi_uart_deinit"></a>

```c
errcode_t uapi_uart_deinit(uart_bus_t bus)
```

**头文件清单**

```c
#include "include/driver/uart.h"
```

**功能说明**

- 去初始化指定的UART串口，释放资源

**前置条件**

- 已调用 `uapi_uart_init` 完成初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [uart_bus_t](#uart_bus_t) | UART总线号 | UART_BUS_0 ~ UART_BUS_2 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 去初始化成功 |
| Other | 其他错误码，参考[errcode_t](#errcode_t) | 去初始化失败 |

**参考案例**

- [uart_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/uart/uart_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_UART_SUPPORT_RX | 特性宏 | 启用UART接收支持 | y |
| CONFIG_UART_SUPPORT_TX | 特性宏 | 启用UART发送支持 | y |
| CONFIG_UART_SUPPORT_DMA | 特性宏 | 启用UART DMA支持 | y |

### uapi_uart_set_attr <a id="uapi_uart_set_attr"></a>

```c
errcode_t uapi_uart_set_attr(uart_bus_t bus, const uart_attr_t *attr)
```

**头文件清单**

```c
#include "include/driver/uart.h"
```

**功能说明**

- 设置UART基础配置参数，包括波特率、数据位、校验位、停止位等

**前置条件**

- 已调用 `uapi_uart_init` 完成初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [uart_bus_t](#uart_bus_t) | UART总线号 | UART_BUS_0 ~ UART_BUS_2 |
| attr | const [uart_attr_t](#uart_attr_t) * | 基础属性配置结构体指针 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 设置成功 |
| Other | 其他错误码，参考[errcode_t](#errcode_t) | 设置失败 |

**参考案例**

- [radar_wireless_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/wireless_sample/radar_wireless_sample.c)


### uapi_uart_get_attr <a id="uapi_uart_get_attr"></a>

```c
errcode_t uapi_uart_get_attr(uart_bus_t bus, const uart_attr_t *attr)
```

**头文件清单**

```c
#include "include/driver/uart.h"
```

**功能说明**

- 获取UART当前基础配置参数

**前置条件**

- 已调用 `uapi_uart_init` 完成初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [uart_bus_t](#uart_bus_t) | UART总线号 | UART_BUS_0 ~ UART_BUS_2 |
| attr | const [uart_attr_t](#uart_attr_t) * | 用于接收当前基础配置的结构体指针（注意：源码签名使用const限定，实际无法通过该参数返回配置数据） | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 获取成功 |
| Other | 其他错误码，参考[errcode_t](#errcode_t) | 获取失败 |

### uapi_uart_has_pending_transmissions <a id="uapi_uart_has_pending_transmissions"></a>

```c
bool uapi_uart_has_pending_transmissions(uart_bus_t bus)
```

**头文件清单**

```c
#include "include/driver/uart.h"
```

**功能说明**

- 判断指定UART是否存在正在等待发送的数据

**前置条件**

- 已调用 `uapi_uart_init` 完成初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [uart_bus_t](#uart_bus_t) | UART总线号 | UART_BUS_0 ~ UART_BUS_2 |

**返回值**

- 返回类型：bool

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| true | 存在正在等待的传输 | 总线存在待发送数据 |
| false | 无正在等待的传输 | 总线无待发送数据 |

### uapi_uart_rx_fifo_is_empty <a id="uapi_uart_rx_fifo_is_empty"></a>

```c
bool uapi_uart_rx_fifo_is_empty(uart_bus_t bus)
```

**头文件清单**

```c
#include "include/driver/uart.h"
```

**功能说明**

- 判断指定UART的接收FIFO是否为空

**前置条件**

- 已调用 `uapi_uart_init` 完成初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [uart_bus_t](#uart_bus_t) | UART总线号 | UART_BUS_0 ~ UART_BUS_2 |

**返回值**

- 返回类型：bool

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| true | RX FIFO为空 | 接收FIFO为空 |
| false | RX FIFO非空 | 接收FIFO非空 |

### uapi_uart_tx_fifo_is_empty <a id="uapi_uart_tx_fifo_is_empty"></a>

```c
bool uapi_uart_tx_fifo_is_empty(uart_bus_t bus)
```

**头文件清单**

```c
#include "include/driver/uart.h"
```

**功能说明**

- 判断指定UART的发送FIFO是否为空

**前置条件**

- 已调用 `uapi_uart_init` 完成初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [uart_bus_t](#uart_bus_t) | UART总线号 | UART_BUS_0 ~ UART_BUS_2 |

**返回值**

- 返回类型：bool

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| true | TX FIFO为空 | 发送FIFO为空 |
| false | TX FIFO非空 | 发送FIFO非空 |

### uapi_uart_register_rx_callback <a id="uapi_uart_register_rx_callback"></a>

```c
errcode_t uapi_uart_register_rx_callback(uart_bus_t bus, uart_rx_condition_t condition, uint32_t size, uart_rx_callback_t callback)
```

**头文件清单**

```c
#include "include/driver/uart.h"
```

**功能说明**

- 注册接收数据回调函数，回调会根据触发条件和数据长度触发，运行于中断上下文

**前置条件**

- 已调用 `uapi_uart_init` 完成初始化
- 使用中断模式或DMA模式

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [uart_bus_t](#uart_bus_t) | UART总线号 | UART_BUS_0 ~ UART_BUS_2 |
| condition | [uart_rx_condition_t](#uart_rx_condition_t) | 回调触发的条件 | 见枚举定义 |
| size | uint32_t | 触发条件涉及长度时表示所需的数据长度 | - |
| callback | [uart_rx_callback_t](#uart_rx_callback_t) | 接收数据的回调函数 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 注册成功 |
| Other | 其他错误码，参考[errcode_t](#errcode_t) | 注册失败 |

**参考案例**

- [uart_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/uart/uart_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_UART_SUPPORT_RX | 特性宏 | 启用UART接收支持 | y |

### uapi_uart_unregister_rx_callback <a id="uapi_uart_unregister_rx_callback"></a>

```c
void uapi_uart_unregister_rx_callback(uart_bus_t bus)
```

**头文件清单**

```c
#include "include/driver/uart.h"
```

**功能说明**

- 去注册UART接收数据回调函数

**前置条件**

- 已调用 `uapi_uart_register_rx_callback` 注册回调

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [uart_bus_t](#uart_bus_t) | UART总线号 | UART_BUS_0 ~ UART_BUS_2 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_UART_SUPPORT_RX | 特性宏 | 启用UART接收支持 | y |

### uapi_uart_register_parity_error_callback <a id="uapi_uart_register_parity_error_callback"></a>

```c
errcode_t uapi_uart_register_parity_error_callback(uart_bus_t bus, uart_error_callback_t callback)
```

**头文件清单**

```c
#include "include/driver/uart.h"
```

**功能说明**

- 注册奇偶校验错误处理回调函数，当UART检测到奇偶校验错误时触发回调，运行于中断上下文

**前置条件**

- 已调用 `uapi_uart_init` 完成初始化
- 配置了奇偶校验功能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [uart_bus_t](#uart_bus_t) | UART总线号 | UART_BUS_0 ~ UART_BUS_2 |
| callback | [uart_error_callback_t](#uart_error_callback_t) | 奇偶校验错误处理回调函数 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 注册成功 |
| Other | 其他错误码，参考[errcode_t](#errcode_t) | 注册失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_UART_SUPPORT_RX | 特性宏 | 启用UART接收支持 | y |

### uapi_uart_register_frame_error_callback <a id="uapi_uart_register_frame_error_callback"></a>

```c
errcode_t uapi_uart_register_frame_error_callback(uart_bus_t bus, uart_error_callback_t callback)
```

**头文件清单**

```c
#include "include/driver/uart.h"
```

**功能说明**

- 注册帧错误处理回调函数，当UART检测到帧错误时触发回调，运行于中断上下文

**前置条件**

- 已调用 `uapi_uart_init` 完成初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [uart_bus_t](#uart_bus_t) | UART总线号 | UART_BUS_0 ~ UART_BUS_2 |
| callback | [uart_error_callback_t](#uart_error_callback_t) | 帧错误处理回调函数 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 注册成功 |
| Other | 其他错误码，参考[errcode_t](#errcode_t) | 注册失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_UART_SUPPORT_RX | 特性宏 | 启用UART接收支持 | y |

### uapi_uart_register_overrun_error_callback <a id="uapi_uart_register_overrun_error_callback"></a>

```c
errcode_t uapi_uart_register_overrun_error_callback(uart_bus_t bus, uart_error_callback_t callback)
```

**头文件清单**

```c
#include "include/driver/uart.h"
```

**功能说明**

- 注册溢出错误处理回调函数，当UART检测到溢出错误时触发回调，运行于中断上下文

**前置条件**

- 已调用 `uapi_uart_init` 完成初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [uart_bus_t](#uart_bus_t) | UART总线号 | UART_BUS_0 ~ UART_BUS_2 |
| callback | [uart_error_callback_t](#uart_error_callback_t) | 溢出错误处理回调函数 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 注册成功 |
| Other | 其他错误码，参考[errcode_t](#errcode_t) | 注册失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_UART_SUPPORT_RX | 特性宏 | 启用UART接收支持 | y |

### uapi_uart_write <a id="uapi_uart_write"></a>

```c
int32_t uapi_uart_write(uart_bus_t bus, const uint8_t *buffer, uint32_t length, uint32_t timeout)
```

**头文件清单**

```c
#include "include/driver/uart.h"
```

**功能说明**

- 以轮询（直接发送）方式将数据发送到已打开的UART上

**前置条件**

- 已调用 `uapi_uart_init` 完成初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [uart_bus_t](#uart_bus_t) | UART总线号 | UART_BUS_0 ~ UART_BUS_2 |
| buffer | const uint8_t * | 要发送的数据Buffer | 不为NULL |
| length | uint32_t | 要发送的数据Buffer长度（字节） | - |
| timeout | uint32_t | 超时时间（ms） | - |

**返回值**

- 返回类型：int32_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| >=0 | 实际发送到UART的数据长度（字节） | 发送完成 |
| <0 | 错误码，参考[errcode_t](#errcode_t) | 发送失败 |

**参考案例**

- [uart_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/uart/uart_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_UART_SUPPORT_TX | 特性宏 | 启用UART发送支持 | y |

### uapi_uart_write_nolock <a id="uapi_uart_write_nolock"></a>

```c
int32_t uapi_uart_write_nolock(uart_bus_t bus, const uint8_t *buffer, uint32_t length, uint32_t timeout)
```

**头文件清单**

```c
#include "include/driver/uart.h"
```

**功能说明**

- 以轮询（直接发送）方式将数据发送到已打开的UART上，发送过程不锁中断

**前置条件**

- 已调用 `uapi_uart_init` 完成初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [uart_bus_t](#uart_bus_t) | UART总线号 | UART_BUS_0 ~ UART_BUS_2 |
| buffer | const uint8_t * | 要发送的数据Buffer | 不为NULL |
| length | uint32_t | 要发送的数据Buffer长度（字节） | - |
| timeout | uint32_t | 超时时间（ms） | - |

**返回值**

- 返回类型：int32_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| >=0 | 实际发送到UART的数据长度（字节） | 发送完成 |
| <0 | 错误码，参考[errcode_t](#errcode_t) | 发送失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_UART_SUPPORT_TX | 特性宏 | 启用UART发送支持 | y |

### uapi_uart_write_int <a id="uapi_uart_write_int"></a>

```c
errcode_t uapi_uart_write_int(uart_bus_t bus, const uint8_t *buffer, uint32_t length, void *params, uart_tx_callback_t finished_with_buffer_func)
```

**头文件清单**

```c
#include "include/driver/uart.h"
```

**功能说明**

- 以中断模式将数据发送到已打开的UART上，发送完成后调用回调函数（运行于中断上下文）

**前置条件**

- 已调用 `uapi_uart_init` 完成初始化
- 使用中断模式

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [uart_bus_t](#uart_bus_t) | UART总线号 | UART_BUS_0 ~ UART_BUS_2 |
| buffer | const uint8_t * | 要发送的数据Buffer | 不为NULL |
| length | uint32_t | 要发送的数据Buffer长度（字节） | - |
| params | void * | 传递到完成传输回调函数的参数 | 可为NULL |
| finished_with_buffer_func | [uart_tx_callback_t](#uart_tx_callback_t) | 数据发送完成的回调函数 | 可为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 发送成功 |
| Other | 其他错误码，参考[errcode_t](#errcode_t) | 发送失败 |

**参考案例**

- [uart_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/uart/uart_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_UART_SUPPORT_TX | 特性宏 | 启用UART发送支持 | y |
| CONFIG_UART_SUPPORT_TX_INT | 特性宏 | 支持UART中断发送 | y |

### uapi_uart_write_by_dma <a id="uapi_uart_write_by_dma"></a>

```c
int32_t uapi_uart_write_by_dma(uart_bus_t bus, const void *buffer, uint32_t length, uart_write_dma_config_t *dma_cfg)
```

**头文件清单**

```c
#include "include/driver/uart.h"
```

**功能说明**

- 通过DMA将数据发送到UART

**前置条件**

- 已调用 `uapi_uart_init` 完成初始化
- 使用DMA模式

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [uart_bus_t](#uart_bus_t) | UART总线号 | UART_BUS_0 ~ UART_BUS_2 |
| buffer | const void * | 要发送的数据Buffer | 不为NULL |
| length | uint32_t | 要发送的数据Buffer长度（字节） | - |
| dma_cfg | [uart_write_dma_config_t](#uart_write_dma_config_t) * | 发送数据时的DMA配置 | 不为NULL |

**返回值**

- 返回类型：int32_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| >=0 | 完成发送的数据长度（字节） | 发送完成 |
| <0 | 错误码，参考[errcode_t](#errcode_t) | 发送失败 |

**参考案例**

- [uart_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/uart/uart_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_UART_SUPPORT_TX | 特性宏 | 启用UART发送支持 | y |
| CONFIG_UART_SUPPORT_DMA | 特性宏 | 启用UART DMA支持 | y |

### uapi_uart_read_by_dma <a id="uapi_uart_read_by_dma"></a>

```c
int32_t uapi_uart_read_by_dma(uart_bus_t bus, const void *buffer, uint32_t length, uart_write_dma_config_t *dma_cfg)
```

**头文件清单**

```c
#include "include/driver/uart.h"
```

**功能说明**

- 通过DMA从UART读取数据

**前置条件**

- 已调用 `uapi_uart_init` 完成初始化
- 使用DMA模式

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [uart_bus_t](#uart_bus_t) | UART总线号 | UART_BUS_0 ~ UART_BUS_2 |
| buffer | const void * | 存储接收数据的Buffer | 不为NULL |
| length | uint32_t | 存储接收数据的Buffer长度（字节） | - |
| dma_cfg | [uart_write_dma_config_t](#uart_write_dma_config_t) * | 接收数据时的DMA配置 | 不为NULL |

**返回值**

- 返回类型：int32_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| >=0 | 完成接收的数据长度（字节） | 接收完成 |
| <0 | 错误码，参考[errcode_t](#errcode_t) | 接收失败 |

**参考案例**

- [uart_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/uart/uart_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_UART_SUPPORT_TX | 特性宏 | 启用UART发送支持 | y |
| CONFIG_UART_SUPPORT_DMA | 特性宏 | 启用UART DMA支持 | y |

### uapi_uart_read <a id="uapi_uart_read"></a>

```c
int32_t uapi_uart_read(uart_bus_t bus, const uint8_t *buffer, uint32_t length, uint32_t timeout)
```

**头文件清单**

```c
#include "include/driver/uart.h"
```

**功能说明**

- 以轮询模式从UART读取数据

**前置条件**

- 已调用 `uapi_uart_init` 完成初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [uart_bus_t](#uart_bus_t) | UART总线号 | UART_BUS_0 ~ UART_BUS_2 |
| buffer | const uint8_t * | 存储接收数据的Buffer | 不为NULL |
| length | uint32_t | 存储接收数据的Buffer长度（字节） | - |
| timeout | uint32_t | 超时时间（ms） | - |

**返回值**

- 返回类型：int32_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| >=0 | 实际读到的数据长度（字节） | 读取完成 |
| <0 | 错误码，参考[errcode_t](#errcode_t) | 读取失败 |

**参考案例**

- [uart_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/uart/uart_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_UART_SUPPORT_RX | 特性宏 | 启用UART接收支持 | y |

### uapi_uart_flush_rx_data <a id="uapi_uart_flush_rx_data"></a>

```c
errcode_t uapi_uart_flush_rx_data(uart_bus_t bus)
```

**头文件清单**

```c
#include "include/driver/uart.h"
```

**功能说明**

- 刷新UART接收Buffer中的数据

**前置条件**

- 已调用 `uapi_uart_init` 完成初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [uart_bus_t](#uart_bus_t) | UART总线号 | UART_BUS_0 ~ UART_BUS_2 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 刷新成功 |
| Other | 其他错误码，参考[errcode_t](#errcode_t) | 刷新失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_UART_SUPPORT_RX | 特性宏 | 启用UART接收支持 | y |

### uapi_uart_update_rx_buff <a id="uapi_uart_update_rx_buff"></a>

```c
errcode_t uapi_uart_update_rx_buff(uart_bus_t bus, uint8_t *rx_buffer, uint16_t rx_buffer_size)
```

**头文件清单**

```c
#include "include/driver/uart.h"
```

**功能说明**

- 更新UART接收Buffer的地址和长度

**前置条件**

- 已调用 `uapi_uart_init` 完成初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bus | [uart_bus_t](#uart_bus_t) | UART总线号 | UART_BUS_0 ~ UART_BUS_2 |
| rx_buffer | uint8_t * | 新的接收Buffer地址 | 不为NULL |
| rx_buffer_size | uint16_t | 新的接收Buffer长度（字节） | - |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 更新成功 |
| Other | 其他错误码，参考[errcode_t](#errcode_t) | 更新失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_UART_SUPPORT_RX | 特性宏 | 启用UART接收支持 | y |

### uapi_uart_suspend <a id="uapi_uart_suspend"></a>

```c
errcode_t uapi_uart_suspend(uintptr_t arg)
```

**头文件清单**

```c
#include "include/driver/uart.h"
```

**功能说明**

- 挂起所有UART通道，暂停UART操作

**前置条件**

- 已调用 `uapi_uart_init` 完成初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| arg | uintptr_t | 挂起所需要的参数 | - |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 挂起成功 |
| Other | 其他错误码，参考[errcode_t](#errcode_t) | 挂起失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_UART_SUPPORT_LPM | 特性宏 | 启用UART低功耗管理 (Low Power Management)支持 | y |

### uapi_uart_resume <a id="uapi_uart_resume"></a>

```c
errcode_t uapi_uart_resume(uintptr_t arg)
```

**头文件清单**

```c
#include "include/driver/uart.h"
```

**功能说明**

- 恢复所有UART通道，从挂起状态恢复UART操作

**前置条件**

- 已调用 `uapi_uart_suspend` 挂起UART

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| arg | uintptr_t | 恢复所需要的参数 | - |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 恢复成功 |
| Other | 其他错误码，参考[errcode_t](#errcode_t) | 恢复失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_UART_SUPPORT_LPM | 特性宏 | 启用UART低功耗管理 (Low Power Management)支持 | y |

## Type definitions

### uart_pin_config_t <a id="uart_pin_config_t"></a>

```c
typedef hal_uart_pin_config_t uart_pin_config_t;
```

**使用说明**

- UART引脚配置类型，等价于 [hal_uart_pin_config_t](#hal_uart_pin_config_t)

### uart_data_bit_t <a id="uart_data_bit_t"></a>

```c
typedef hal_uart_data_bit_t uart_data_bit_t;
```

**使用说明**

- UART数据位定义类型，等价于 [hal_uart_data_bit_t](#hal_uart_data_bit_t)

### uart_parity_t <a id="uart_parity_t"></a>

```c
typedef hal_uart_parity_t uart_parity_t;
```

**使用说明**

- UART奇偶校验位定义类型，等价于 [hal_uart_parity_t](#hal_uart_parity_t)

### uart_stop_bit_t <a id="uart_stop_bit_t"></a>

```c
typedef hal_uart_stop_bit_t uart_stop_bit_t;
```

**使用说明**

- UART停止位定义类型，等价于 [hal_uart_stop_bit_t](#hal_uart_stop_bit_t)

### uart_attr_t <a id="uart_attr_t"></a>

```c
typedef hal_uart_attr_t uart_attr_t;
```

**使用说明**

- UART基本属性定义类型，等价于 [hal_uart_attr_t](#hal_uart_attr_t)

### uart_extra_attr_t <a id="uart_extra_attr_t"></a>

```c
typedef hal_uart_extra_attr_t uart_extra_attr_t;
```

**使用说明**

- UART扩展属性定义类型，等价于 [hal_uart_extra_attr_t](#hal_uart_extra_attr_t)

### uart_flow_ctrl_t <a id="uart_flow_ctrl_t"></a>

```c
typedef hal_uart_flow_ctrl_t uart_flow_ctrl_t;
```

**使用说明**

- UART流控类型定义，等价于 [hal_uart_flow_ctrl_t](#hal_uart_flow_ctrl_t)

### uart_rx_callback_t <a id="uart_rx_callback_t"></a>

```c
typedef void (*uart_rx_callback_t)(const void *buffer, uint16_t length, bool error);
```

**使用说明**

- UART接收数据回调函数类型。在中断上下文中执行，接收缓冲区在回调返回后会被释放

**参数**

| 参数名 | 类型 | 说明 |
| ------ | ---- | ---- |
| buffer | const void * | 读取数据时用于存储数据的Buffer |
| length | uint16_t | Buffer的长度 |
| error | bool | true表示接收数据时产生了错误，false表示正常 |

### uart_rx_by_dma_callback_t <a id="uart_rx_by_dma_callback_t"></a>

```c
typedef void (*uart_rx_by_dma_callback_t)(uart_bus_t bus, const void *buffer, uint32_t length, uart_write_dma_config_t *dma_cfg);
```

**使用说明**

- UART RX中断触发DMA搬运回调函数类型。在中断上下文中执行

**参数**

| 参数名 | 类型 | 说明 |
| ------ | ---- | ---- |
| bus | uart_bus_t | UART总线号 |
| buffer | const void * | DMA搬运的目的地址 |
| length | uint32_t | DMA搬运的数据长度 |
| dma_cfg | uart_write_dma_config_t * | DMA配置结构体指针 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_UART_SUPPORT_RX | 特性宏 | 启用UART接收支持 | y |
| CONFIG_UART_SUPPORT_DMA | 特性宏 | 启用UART DMA支持 | y |
| CONFIG_UART_SUPPORT_INT_TRIGGER_DMA | 特性宏 | 支持中断触发DMA | 由构建目标决定 |

### uart_tx_callback_t <a id="uart_tx_callback_t"></a>

```c
typedef void (*uart_tx_callback_t)(const void *buffer, uint32_t length, const void *params);
```

**使用说明**

- UART发送数据回调函数类型，在中断上下文中执行

**参数**

| 参数名 | 类型 | 说明 |
| ------ | ---- | ---- |
| buffer | const void * | 发送时的数据缓存 |
| length | uint32_t | 发送时的数据长度 |
| params | const void * | 传递的参数 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_UART_SUPPORT_TX | 特性宏 | 启用UART发送支持 | y |

### uart_error_callback_t <a id="uart_error_callback_t"></a>

```c
typedef void (*uart_error_callback_t)(uint32_t *err_info, uint32_t len);
```

**使用说明**

- UART错误处理回调函数类型，在中断上下文中执行，执行完成后会自动释放内存。用于奇偶校验、帧、溢出错误回调注册

**参数**

| 参数名 | 类型 | 说明 |
| ------ | ---- | ---- |
| err_info | uint32_t * | 错误信息，每一个成员都是32-bit |
| len | uint32_t | 错误信息的长度 |

### errcode_t <a id="errcode_t"></a>

```c
typedef uint32_t errcode_t;
```

**使用说明**

- 通用错误码类型，定义于 `errcode.h`。`ERRCODE_SUCC`（0x0）表示成功，其余非零值表示失败，具体含义参考 `errcode.h`

## Enumerations

### uart_bus_t <a id="uart_bus_t"></a>

```c
typedef enum {
    UART_BUS_0 = 0,
    UART_BUS_1 = 1,
    UART_BUS_2 = 2,
    UART_BUS_NONE = UART_BUS_MAX_NUMBER
} uart_bus_t;
```

**使用说明**

- UART总线号枚举，定义于 `platform_core.h`（`UART_BUS_MAX_NUMBER` 为 3）

| 枚举成员 | 取值 | 描述 |
| ------ | ---- | ---- |
| UART_BUS_0 | 0 | UART总线0 |
| UART_BUS_1 | 1 | UART总线1 |
| UART_BUS_2 | 2 | UART总线2 |
| UART_BUS_NONE | 3 | 无效总线号（UART_BUS_MAX_NUMBER） |

### hal_uart_data_bit_t <a id="hal_uart_data_bit_t"></a>

```c
typedef enum hal_uart_data_bit {
    UART_DATA_BIT_5,
    UART_DATA_BIT_6,
    UART_DATA_BIT_7,
    UART_DATA_BIT_8
} hal_uart_data_bit_t;
```

**使用说明**

- UART数据位枚举，定义于 `hal_uart.h`

| 枚举成员 | 取值 | 描述 |
| ------ | ---- | ---- |
| UART_DATA_BIT_5 | 0 | 5位数据位 |
| UART_DATA_BIT_6 | 1 | 6位数据位 |
| UART_DATA_BIT_7 | 2 | 7位数据位 |
| UART_DATA_BIT_8 | 3 | 8位数据位 |

### hal_uart_parity_t <a id="hal_uart_parity_t"></a>

```c
typedef enum hal_uart_parity {
    UART_PARITY_NONE,
    UART_PARITY_ODD,
    UART_PARITY_EVEN
} hal_uart_parity_t;
```

**使用说明**

- UART奇偶校验位枚举，定义于 `hal_uart.h`

| 枚举成员 | 取值 | 描述 |
| ------ | ---- | ---- |
| UART_PARITY_NONE | 0 | 无校验 |
| UART_PARITY_ODD | 1 | 奇校验 |
| UART_PARITY_EVEN | 2 | 偶校验 |

### hal_uart_stop_bit_t <a id="hal_uart_stop_bit_t"></a>

```c
typedef enum hal_uart_stop_bit {
    UART_STOP_BIT_1,
    UART_STOP_BIT_2
} hal_uart_stop_bit_t;
```

**使用说明**

- UART停止位枚举，定义于 `hal_uart.h`

| 枚举成员 | 取值 | 描述 |
| ------ | ---- | ---- |
| UART_STOP_BIT_1 | 0 | 1位停止位 |
| UART_STOP_BIT_2 | 1 | 2位停止位 |

### hal_uart_flow_ctrl_t <a id="hal_uart_flow_ctrl_t"></a>

```c
typedef enum {
    UART_FLOW_CTRL_NONE = 0x0,
    UART_FLOW_CTS = 0x1,
    UART_FLOW_RTS = 0x2,
    UART_FLOW_CTRL_RTS_CTS = UART_FLOW_CTS | UART_FLOW_RTS,
    UART_FLOW_CTRL_SOFT = 0x4
} hal_uart_flow_ctrl_t;
```

**使用说明**

- UART流控类型枚举，定义于 `hal_uart.h`

| 枚举成员 | 取值 | 描述 |
| ------ | ---- | ---- |
| UART_FLOW_CTRL_NONE | 0x0 | 无流控 |
| UART_FLOW_CTS | 0x1 | 仅CTS (Clear To Send) 流控 |
| UART_FLOW_RTS | 0x2 | 仅RTS (Request To Send) 流控 |
| UART_FLOW_CTRL_RTS_CTS | 0x3 | RTS/CTS硬件流控 |
| UART_FLOW_CTRL_SOFT | 0x4 | 软件流控 |

### uart_rx_condition_t <a id="uart_rx_condition_t"></a>

```c
typedef enum uart_rx_condition {
    UART_RX_CONDITION_FULL_OR_IDLE = (UART_RX_CONDITION_MASK_FULL | UART_RX_CONDITION_MASK_IDLE),
    UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA = (UART_RX_CONDITION_MASK_FULL | UART_RX_CONDITION_MASK_SUFFICIENT_DATA),
    UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE = (UART_RX_CONDITION_MASK_FULL | UART_RX_CONDITION_MASK_SUFFICIENT_DATA | UART_RX_CONDITION_MASK_IDLE)
} uart_rx_condition_t;
```

**使用说明**

- UART接收数据触发回调的条件枚举，定义于 `uart.h`

| 枚举成员 | 取值 | 描述 |
| ------ | ---- | ---- |
| UART_RX_CONDITION_FULL_OR_IDLE | - | 接收缓存已满或接收暂停时触发回调 |
| UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA | - | 接收缓存已满或接收数据量达到指定长度时触发回调 |
| UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE | - | 接收缓存已满、接收数据量达到指定长度或接收暂停时触发回调 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_UART_SUPPORT_RX | 特性宏 | 启用UART接收支持 | y |

## Structures

### hal_uart_pin_config_t <a id="hal_uart_pin_config_t"></a>

```c
typedef struct {
    pin_t tx_pin;
    pin_t rx_pin;
    pin_t cts_pin;
    pin_t rts_pin;
} hal_uart_pin_config_t;
```

**使用说明**

- UART引脚配置结构体，定义于 `hal_uart.h`

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ---- | ---- | ---- |
| tx_pin | pin_t | 发送引脚 |
| rx_pin | pin_t | 接收引脚 |
| cts_pin | pin_t | 发送就绪引脚 |
| rts_pin | pin_t | 接收就绪引脚 |

### hal_uart_attr_t <a id="hal_uart_attr_t"></a>

```c
typedef struct uart_attr {
    uint32_t baud_rate;
    uint8_t data_bits;
    uint8_t stop_bits;
    uint8_t parity;
    uint8_t flow_ctrl;
} hal_uart_attr_t;
```

**使用说明**

- UART基本属性配置结构体，定义于 `hal_uart.h`

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ---- | ---- | ---- |
| baud_rate | uint32_t | 波特率 |
| data_bits | uint8_t | 数据位，参考 [uart_data_bit_t](#uart_data_bit_t) |
| stop_bits | uint8_t | 停止位，参考 [uart_stop_bit_t](#uart_stop_bit_t) |
| parity | uint8_t | 校验位，参考 [uart_parity_t](#uart_parity_t) |
| flow_ctrl | uint8_t | 流控类型，参考 [uart_flow_ctrl_t](#uart_flow_ctrl_t) |

### hal_uart_extra_attr_t <a id="hal_uart_extra_attr_t"></a>

```c
typedef struct uart_extra_attr {
    bool tx_dma_enable;
    uint8_t tx_int_threshold;
    bool rx_dma_enable;
    uint8_t rx_int_threshold;
#if defined(CONFIG_UART_SUPPORT_RX_THREAD)
    bool rx_thread_enable;
#endif
} hal_uart_extra_attr_t;
```

**使用说明**

- UART扩展属性配置结构体，定义于 `hal_uart.h`

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ---- | ---- | ---- |
| tx_dma_enable | bool | TX是否使用DMA。false: 使用uapi_uart_write发送; true: 使用uapi_uart_write_by_dma发送 |
| tx_int_threshold | uint8_t | 触发中断的TX FIFO水线 |
| rx_dma_enable | bool | RX是否使用DMA。false: 使用uapi_uart_read接收; true: 使用uapi_uart_read_by_dma接收 |
| rx_int_threshold | uint8_t | 触发中断的RX FIFO水线 |
| rx_thread_enable | bool | RX是否允许通过线程处理（仅在 CONFIG_UART_SUPPORT_RX_THREAD 使能时存在） |

### uart_buffer_config_t <a id="uart_buffer_config_t"></a>

```c
typedef struct uart_buffer_config {
    void *rx_buffer;
    size_t rx_buffer_size;
} uart_buffer_config_t;
```

**使用说明**

- UART缓冲区配置结构体，定义于 `uart.h`

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ---- | ---- | ---- |
| rx_buffer | void * | 接收数据Buffer指针 |
| rx_buffer_size | size_t | 接收Buffer的长度 |

### uart_write_dma_config_t <a id="uart_write_dma_config_t"></a>

```c
typedef struct uart_write_dma_config {
    uint8_t src_width;
    uint8_t dest_width;
    uint8_t burst_length;
    uint8_t priority;
} uart_write_dma_config_t;
```

**使用说明**

- UART DMA发送配置结构体，定义于 `uart.h`

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ---- | ---- | ---- |
| src_width | uint8_t | 源端传输数据宽度：0-1字节，1-2字节，2-4字节 |
| dest_width | uint8_t | 目的端传输数据宽度：0-1字节，1-2字节，2-4字节 |
| burst_length | uint8_t | burst长度：0-1，1-4，2-8，3-16 |
| priority | uint8_t | 传输通道优先级（最小0，最大3） |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_UART_SUPPORT_TX | 特性宏 | 启用UART发送支持 | y |
| CONFIG_UART_SUPPORT_DMA | 特性宏 | 启用UART DMA支持 | y |

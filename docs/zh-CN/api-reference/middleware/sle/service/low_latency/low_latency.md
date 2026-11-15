# SLE Low Latency

SLE (SparkLink Low Energy) 低延迟传输 API。管理低延迟连接参数配置、传输通道设置和低延迟模式启停。优化 SLE 协议栈参数以最小化数据从发送端到接收端的延迟。

配合 GLP (Generic Layer Protocol) 使用，为 HID (Human Interface Device)、传感器等延迟敏感应用提供物理层加速。

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_low_latency.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [sle_low_latency_mouse_enable](#sle_low_latency_mouse_enable) | 使能SLE低时延Mouse模式 |
| [sle_low_latency_mouse_register_callbacks](#sle_low_latency_mouse_register_callbacks) | 注册Mouse模式低时延数据发送回调 |
| [sle_low_latency_dongle_enable](#sle_low_latency_dongle_enable) | 使能SLE低时延Dongle模式 |
| [sle_low_latency_set](#sle_low_latency_set) | 配置低时延使能状态与调度参数 |
| [sle_low_latency_dongle_register_callbacks](#sle_low_latency_dongle_register_callbacks) | 注册Dongle模式低时延数据接收回调 |
| [sle_low_latency_tx_register_callbacks](#sle_low_latency_tx_register_callbacks) | 注册TX (Transmit) 模式低时延数据发送回调 |
| [sle_low_latency_rx_register_callbacks](#sle_low_latency_rx_register_callbacks) | 注册RX (Receive) 模式低时延数据接收回调 |
| [sle_low_latency_tx_enable](#sle_low_latency_tx_enable) | 使能SLE低时延TX模式 |
| [sle_low_latency_rx_enable](#sle_low_latency_rx_enable) | 使能SLE低时延RX模式 |

## Functions

### sle_low_latency_mouse_enable <a id="sle_low_latency_mouse_enable"></a>

```c
errcode_t sle_low_latency_mouse_enable(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_low_latency.h"
```

**功能说明**

- 使能SLE低时延Mouse模式，初始化Mouse端低时延通信
- 调用后Mouse端低时延调度功能启动，可通过注册回调获取鼠标数据发送能力
- 需在SLE连接建立后调用，配合sle_low_latency_mouse_register_callbacks使用

**前置条件**

- SLE服务已初始化完成
- SLE连接已建立

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_FEATURE_GLE_LOW_LATENCY | 特性宏 | 支持低时延通信特性 | n |

### sle_low_latency_mouse_register_callbacks <a id="sle_low_latency_mouse_register_callbacks"></a>

```c
errcode_t sle_low_latency_mouse_register_callbacks(sle_low_latency_mouse_callbacks_t *mouse_cbk)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_low_latency.h"
```

**功能说明**

- 注册Mouse模式低时延数据发送回调函数
- 通过回调获取鼠标键值与坐标数据，用于低时延调度发送
- 需在sle_low_latency_mouse_enable之前或之后调用，确保回调已注册以接收数据发送请求

**前置条件**

- SLE服务已初始化完成
- 入参回调结构体指针非NULL且回调函数指针有效

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| mouse_cbk | [sle_low_latency_mouse_callbacks_t](#struct_sle_low_latency_mouse_callbacks_t)* | Mouse模式回调函数结构体 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 注册成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

### sle_low_latency_dongle_enable <a id="sle_low_latency_dongle_enable"></a>

```c
errcode_t sle_low_latency_dongle_enable(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_low_latency.h"
```

**功能说明**

- 使能SLE低时延Dongle模式，初始化Dongle端低时延通信
- 调用后Dongle端低时延调度功能启动，可通过注册回调接收Mouse端发送的键值坐标数据
- 需在SLE连接建立后调用，配合sle_low_latency_dongle_register_callbacks使用

**前置条件**

- SLE服务已初始化完成
- SLE连接已建立

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

### sle_low_latency_set <a id="sle_low_latency_set"></a>

```c
errcode_t sle_low_latency_set(uint16_t conn_id, uint8_t enable, uint16_t rate)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_low_latency.h"
```

**功能说明**

- 配置低时延使能状态与调度参数
- 通过指定连接句柄、使能开关与回报率，控制低时延调度行为
- 需在SLE连接建立后且对应端（Mouse/Dongle/TX/RX）已使能后调用

**前置条件**

- SLE服务已初始化完成
- SLE连接已建立，conn_id为有效连接句柄
- 对应低时延模式（TX/RX/Dongle/Mouse）已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| conn_id | uint16_t | 连接句柄 | 有效连接句柄 |
| enable | uint8_t | 低时延使能状态 | [SLE_LOW_LATENCY_DISABLE](#enum_sle_low_latency_status_t)(0)<br>[SLE_LOW_LATENCY_ENABLE](#enum_sle_low_latency_status_t)(1) |
| rate | uint16_t | 低时延调度回报率 | [SLE_LOW_LATENCY_125HZ](#enum_sle_low_latency_rate_t)(0)<br>[SLE_LOW_LATENCY_500HZ](#enum_sle_low_latency_rate_t)(1)<br>[SLE_LOW_LATENCY_1K](#enum_sle_low_latency_rate_t)(2)<br>[SLE_LOW_LATENCY_2K](#enum_sle_low_latency_rate_t)(3)<br>[SLE_LOW_LATENCY_3K](#enum_sle_low_latency_rate_t)(4)<br>[SLE_LOW_LATENCY_4K](#enum_sle_low_latency_rate_t)(5)<br>[SLE_LOW_LATENCY_5K](#enum_sle_low_latency_rate_t)(6)<br>[SLE_LOW_LATENCY_6K](#enum_sle_low_latency_rate_t)(7)<br>[SLE_LOW_LATENCY_7K](#enum_sle_low_latency_rate_t)(8)<br>[SLE_LOW_LATENCY_8K](#enum_sle_low_latency_rate_t)(9) |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_FEATURE_GLE_LOW_LATENCY | 特性宏 | 支持低时延通信特性 | n |

### sle_low_latency_dongle_register_callbacks <a id="sle_low_latency_dongle_register_callbacks"></a>

```c
errcode_t sle_low_latency_dongle_register_callbacks(sle_low_latency_dongle_callbacks_t *dongle_cbk)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_low_latency.h"
```

**功能说明**

- 注册Dongle模式低时延数据接收回调函数
- 通过回调接收Mouse端发送的键值与坐标数据
- 需在sle_low_latency_dongle_enable之前或之后调用，确保回调已注册以接收数据

**前置条件**

- SLE服务已初始化完成
- 入参回调结构体指针非NULL且回调函数指针有效

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| dongle_cbk | [sle_low_latency_dongle_callbacks_t](#struct_sle_low_latency_dongle_callbacks_t)* | Dongle模式回调函数结构体 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 注册成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

### sle_low_latency_tx_register_callbacks <a id="sle_low_latency_tx_register_callbacks"></a>

```c
errcode_t sle_low_latency_tx_register_callbacks(sle_low_latency_tx_callbacks_t *tx_cbk)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_low_latency.h"
```

**功能说明**

- 注册TX模式低时延数据发送回调函数
- 通过回调获取HID设备TLV (Tag-Length-Value) 数据，用于低时延调度发送
- 需在sle_low_latency_tx_enable之前或之后调用，确保回调已注册以接收数据发送请求

**前置条件**

- SLE服务已初始化完成
- 入参回调结构体指针非NULL且回调函数指针有效

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| tx_cbk | [sle_low_latency_tx_callbacks_t](#struct_sle_low_latency_tx_callbacks_t)* | TX模式回调函数结构体 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 注册成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_FEATURE_GLE_LOW_LATENCY | 特性宏 | 支持低时延通信特性 | n |

### sle_low_latency_rx_register_callbacks <a id="sle_low_latency_rx_register_callbacks"></a>

```c
errcode_t sle_low_latency_rx_register_callbacks(sle_low_latency_rx_callbacks_t *rx_cbk)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_low_latency.h"
```

**功能说明**

- 注册RX模式低时延数据接收回调函数
- 通过回调接收TX端发送的TLV数据
- 需在sle_low_latency_rx_enable之前或之后调用，确保回调已注册以接收数据

**前置条件**

- SLE服务已初始化完成
- 入参回调结构体指针非NULL且回调函数指针有效

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| rx_cbk | [sle_low_latency_rx_callbacks_t](#struct_sle_low_latency_rx_callbacks_t)* | RX模式回调函数结构体 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 注册成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_FEATURE_GLE_LOW_LATENCY | 特性宏 | 支持低时延通信特性 | n |

### sle_low_latency_tx_enable <a id="sle_low_latency_tx_enable"></a>

```c
errcode_t sle_low_latency_tx_enable(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_low_latency.h"
```

**功能说明**

- 使能SLE低时延TX模式，初始化TX端低时延通信
- 调用后TX端低时延调度功能启动，可通过注册回调获取HID设备TLV数据发送能力
- 需在SLE连接建立后调用，配合sle_low_latency_tx_register_callbacks使用

**前置条件**

- SLE服务已初始化完成
- SLE连接已建立

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_FEATURE_GLE_LOW_LATENCY | 特性宏 | 支持低时延通信特性 | n |

### sle_low_latency_rx_enable <a id="sle_low_latency_rx_enable"></a>

```c
errcode_t sle_low_latency_rx_enable(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_low_latency.h"
```

**功能说明**

- 使能SLE低时延RX模式，初始化RX端低时延通信
- 调用后RX端低时延调度功能启动，可通过注册回调接收TX端发送的TLV数据
- 需在SLE连接建立后调用，配合sle_low_latency_rx_register_callbacks使用

**前置条件**

- SLE服务已初始化完成
- SLE连接已建立

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_FEATURE_GLE_LOW_LATENCY | 特性宏 | 支持低时延通信特性 | n |

## Enumerations

### sle_low_latency_rate_t <a id="enum_sle_low_latency_rate_t"></a>

```c
typedef enum {
    SLE_LOW_LATENCY_125HZ = 0,  /*!< 125HZ 调度 */
    SLE_LOW_LATENCY_500HZ,      /*!< 500HZ 调度 */
    SLE_LOW_LATENCY_1K,         /*!< 1000Hz 调度 */
    SLE_LOW_LATENCY_2K,         /*!< 2000HZ 调度 */
    SLE_LOW_LATENCY_3K,         /*!< 3000HZ 调度 */
    SLE_LOW_LATENCY_4K,         /*!< 4000Hz 调度 */
    SLE_LOW_LATENCY_5K,         /*!< 5000HZ 调度 */
    SLE_LOW_LATENCY_6K,         /*!< 6000HZ 调度 */
    SLE_LOW_LATENCY_7K,         /*!< 7000HZ 调度 */
    SLE_LOW_LATENCY_8K,         /*!< 8000HZ 调度 */
    SLE_LOW_LATENCY_MAX,
} sle_low_latency_rate_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SLE_LOW_LATENCY_125HZ | 0 | 125HZ调度 |
| SLE_LOW_LATENCY_500HZ | 1 | 500HZ调度 |
| SLE_LOW_LATENCY_1K | 2 | 1000Hz调度 |
| SLE_LOW_LATENCY_2K | 3 | 2000HZ调度 |
| SLE_LOW_LATENCY_3K | 4 | 3000HZ调度 |
| SLE_LOW_LATENCY_4K | 5 | 4000Hz调度 |
| SLE_LOW_LATENCY_5K | 6 | 5000HZ调度 |
| SLE_LOW_LATENCY_6K | 7 | 6000HZ调度 |
| SLE_LOW_LATENCY_7K | 8 | 7000HZ调度 |
| SLE_LOW_LATENCY_8K | 9 | 8000HZ调度 |
| SLE_LOW_LATENCY_MAX | 10 | 调度速率最大值 |

### sle_low_latency_status_t <a id="enum_sle_low_latency_status_t"></a>

```c
typedef enum {
    SLE_LOW_LATENCY_DISABLE = 0, /*!< 关闭低时延调度 */
    SLE_LOW_LATENCY_ENABLE       /*!< 打开低时延 */
} sle_low_latency_status_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SLE_LOW_LATENCY_DISABLE | 0 | 关闭低时延调度 |
| SLE_LOW_LATENCY_ENABLE | 1 | 打开低时延 |

### sle_low_latency_value_set_status_t <a id="enum_sle_low_latency_value_set_status_t"></a>

```c
typedef enum {
    SLE_LOW_LATENCY_VALUE_GET_SUCCESS = 0, /*!< 获取鼠标数据成功 */
    SLE_LOW_LATENCY_VALUE_GET_FAIL        /*!< 获取鼠标数据失败, 失败后不会发送数据 */
} sle_low_latency_value_set_status_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SLE_LOW_LATENCY_VALUE_GET_SUCCESS | 0 | 获取鼠标数据成功 |
| SLE_LOW_LATENCY_VALUE_GET_FAIL | 1 | 获取鼠标数据失败，失败后不会发送数据 |

## Structures

### sle_set_acb_low_latency_t <a id="struct_sle_set_acb_low_latency_t"></a>

```c
typedef struct {
    uint16_t conn_id; /*!< 连接ID */
    uint8_t  enable;  /*!< 低时延使能状态, { @ref sle_low_latency_rate_t } */
    uint8_t  rate;    /*!< 低时延调度速率, { @ref sle_low_latency_rate_t } */
} sle_set_acb_low_latency_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| conn_id | uint16_t | 连接ID |
| enable | uint8_t | 低时延使能状态，参考 [sle_low_latency_status_t](#enum_sle_low_latency_status_t) |
| rate | uint8_t | 低时延调度速率，参考 [sle_low_latency_rate_t](#enum_sle_low_latency_rate_t) |

### sle_low_latency_mouse_callbacks_t <a id="struct_sle_low_latency_mouse_callbacks_t"></a>

```c
typedef struct {
    low_latency_key_value_set_callback set_value_cb;  /*!< 设置键值坐标数据回调函数 */
} sle_low_latency_mouse_callbacks_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| set_value_cb | [low_latency_key_value_set_callback](#low_latency_key_value_set_callback) | 设置键值坐标数据回调函数 |

### sle_low_latency_tx_callbacks_t <a id="struct_sle_low_latency_tx_callbacks_t"></a>

```c
typedef struct {
    low_latency_general_tx_callback low_latency_tx_cb;  /*!< 设置TX数据的通用接口 */
} sle_low_latency_tx_callbacks_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| low_latency_tx_cb | [low_latency_general_tx_callback](#low_latency_general_tx_callback) | 设置TX数据的通用接口 |

### sle_low_latency_dongle_callbacks_t <a id="struct_sle_low_latency_dongle_callbacks_t"></a>

```c
typedef struct {
    low_latency_report_callback report_cb;           /*!< 接收键值坐标数据回调函数 */
} sle_low_latency_dongle_callbacks_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| report_cb | [low_latency_report_callback](#low_latency_report_callback) | 接收键值坐标数据回调函数 |

### sle_low_latency_rx_callbacks_t <a id="struct_sle_low_latency_rx_callbacks_t"></a>

```c
typedef struct {
    low_latency_general_rx_callback low_latency_rx_cb; /*!< 接收RX数据的通用接口 */
} sle_low_latency_rx_callbacks_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| low_latency_rx_cb | [low_latency_general_rx_callback](#low_latency_general_rx_callback) | 接收RX数据的通用接口 |

## Type definitions

### low_latency_key_value_set_callback <a id="low_latency_key_value_set_callback"></a>

```c
typedef errcode_t (*low_latency_key_value_set_callback)(int8_t *button_mask, int16_t *x, int16_t *y, int8_t *wheel);
```

**功能说明**

- Mouse模式数据发送回调接口定义
- SLE服务通过此回调获取鼠标键值与坐标数据，用于低时延调度发送
- 返回值指示数据获取是否成功，失败时不会发送数据

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| button_mask | int8_t* | 按键值，出参 | 非NULL |
| x | int16_t* | 鼠标X坐标，出参 | 非NULL |
| y | int16_t* | 鼠标Y坐标，出参 | 非NULL |
| wheel | int8_t* | 鼠标滚轮，出参 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| SLE_LOW_LATENCY_VALUE_GET_SUCCESS(0) | 获取鼠标数据成功 | 数据获取成功 |
| SLE_LOW_LATENCY_VALUE_GET_FAIL(1) | 获取鼠标数据失败 | 数据获取失败，不会发送数据 |

### low_latency_general_tx_callback <a id="low_latency_general_tx_callback"></a>

```c
typedef uint8_t *(*low_latency_general_tx_callback)(uint8_t *len);
```

**功能说明**

- HID设备TLV数据发送通用回调接口定义
- SLE服务通过此回调获取待发送数据，len作为入参时表示最大支持数据长度，作为出参时表示用户需发送的数据长度
- 返回值为用户数据指针，返回NULL时数据不会被发送

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| len | uint8_t* | 数据长度，入参时表示最大支持数据长度，出参时表示需发送的数据长度 | 非NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| len | uint8_t* | 用户需发送的数据长度 |

**返回值**

- 返回类型：uint8_t*

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非NULL指针 | 用户数据指针，指向的数据将被发送 | 数据获取成功 |
| NULL | 数据不会被发送 | 数据获取异常 |

### low_latency_report_callback <a id="low_latency_report_callback"></a>

```c
typedef void(*low_latency_report_callback)(uint8_t *data, uint8_t len);
```

**功能说明**

- Dongle模式数据接收回调接口定义
- SLE服务通过此回调通知接收到的键值与坐标数据
- 在SLE service上下文中调用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| data | uint8_t* | 接收到的数据 | 非NULL |
| len | uint8_t | 数据长度 | - |

### low_latency_general_rx_callback <a id="low_latency_general_rx_callback"></a>

```c
typedef void(*low_latency_general_rx_callback)(uint8_t len, uint8_t *value);
```

**功能说明**

- RX模式通用数据接收回调接口定义
- SLE服务通过此回调通知接收到的TLV数据
- 在SLE service上下文中调用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| len | uint8_t | 数据长度 | - |
| value | uint8_t* | 接收到的数据 | 非NULL |

### errcode_t <a id="typedef_errcode_t"></a>

```c
typedef uint32_t errcode_t;
```

**使用说明**

作为SLE low_latency接口的返回值类型，用于表示接口调用的执行结果。



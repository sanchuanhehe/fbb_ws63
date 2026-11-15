# SLE Factory

SLE (SparkLink Low Energy) 产测模式 API。管理产测模式使能/去使能和射频测试参数配置。产测模式下设备关闭正常协议栈行为，进入射频测试态——支持连续发射/接收、信道切换、功率调节等 RF (Radio Frequency) 认证测试所需的操作。

SLE 协议栈的工厂模块，用于创建和管理 SLE 实例、注册回调函数。

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_factory_manager.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [sle_rf_tx_start](#sle_rf_tx_start) | 开启射频长发 |
| [sle_rf_rx_start](#sle_rf_rx_start) | 开启射频长收 |
| [sle_rf_trx_end](#sle_rf_trx_end) | 关闭射频收发 |
| [sle_rf_reset](#sle_rf_reset) | 重置射频收发 |
| [sle_factory_register_callbacks](#sle_factory_register_callbacks) | 注册SLE factory管理回调函数 |

## Functions

### sle_rf_tx_start <a id="sle_rf_tx_start"></a>

```c
errcode_t sle_rf_tx_start(sle_rf_tx_start_t* rf_tx_start)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_factory_manager.h"
```

**功能说明**

- 开启SLE射频长发测试
- 调用后射频长发启动，执行结果通过 [sle_rf_tx_start_callback](#sle_rf_tx_start_callback) 回调返回
- 长发参数通过入参结构体指定，包括频率、功率、物理层、速率、格式等

**前置条件**

- SLE服务已初始化完成
- 已通过 sle_factory_register_callbacks 注册回调函数，否则无法接收长发启动结果

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| rf_tx_start | [sle_rf_tx_start_t](#struct_sle_rf_tx_start_t)* | 射频长发参数结构体 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

### sle_rf_rx_start <a id="sle_rf_rx_start"></a>

```c
errcode_t sle_rf_rx_start(sle_rf_rx_start_t* rf_rx_start)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_factory_manager.h"
```

**功能说明**

- 开启SLE射频长收测试
- 调用后射频长收启动，执行结果通过 [sle_rf_rx_start_callback](#sle_rf_rx_start_callback) 回调返回
- 长收参数通过入参结构体指定，包括频率、物理层、格式、导频比等

**前置条件**

- SLE服务已初始化完成
- 已通过 sle_factory_register_callbacks 注册回调函数，否则无法接收长收启动结果

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| rf_rx_start | [sle_rf_rx_start_t](#struct_sle_rf_rx_start_t)* | 射频长收参数结构体 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

### sle_rf_trx_end <a id="sle_rf_trx_end"></a>

```c
errcode_t sle_rf_trx_end(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_factory_manager.h"
```

**功能说明**

- 关闭SLE射频收发
- 调用后停止当前射频长发或长收，执行结果通过 [sle_rf_trx_end_callback](#sle_rf_trx_end_callback) 回调返回
- 回调中返回收发结束事件结构体，包含状态、数据包数、RSSI (Received Signal Strength Indicator) 等信息

**前置条件**

- SLE服务已初始化完成
- 已通过 sle_rf_tx_start 或 sle_rf_rx_start 开启了射频收发
- 已通过 sle_factory_register_callbacks 注册回调函数，否则无法接收收发结束结果

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

### sle_rf_reset <a id="sle_rf_reset"></a>

```c
errcode_t sle_rf_reset(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_factory_manager.h"
```

**功能说明**

- 重置SLE射频收发
- 调用后重置射频收发状态，执行结果通过 [sle_rf_reset_callback](#sle_rf_reset_callback) 回调返回
- 重置后需重新配置长发或长收参数

**前置条件**

- SLE服务已初始化完成
- 已通过 sle_factory_register_callbacks 注册回调函数，否则无法接收重置结果

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

### sle_factory_register_callbacks <a id="sle_factory_register_callbacks"></a>

```c
errcode_t sle_factory_register_callbacks(sle_factory_callbacks_t *func)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_factory_manager.h"
```

**功能说明**

- 注册SLE factory管理回调函数
- 回调函数包括射频长发启动回调、射频长收启动回调、射频收发结束回调、射频收发复位回调
- 需在使用射频长发/长收/关闭/重置接口之前调用，否则无法接收异步执行结果

**前置条件**

- SLE服务已初始化完成
- 入参回调结构体指针非NULL且各回调函数指针有效

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func | [sle_factory_callbacks_t](#struct_sle_factory_callbacks_t)* | 回调函数结构体 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 成功 | 注册成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

## Enumerations

### sle_rf_power_t <a id="enum_sle_rf_power_t"></a>

```c
typedef enum {
    SLE_RF_POWER_LEVEL_0 = 0x00,    /*!< radio frequency power 0 level:-14dbm */
    SLE_RF_POWER_LEVEL_1 = 0x01,    /*!< radio frequency power 1 level:-10dbm */
    SLE_RF_POWER_LEVEL_2 = 0x02,    /*!< radio frequency power 2 level:-6dbm */
    SLE_RF_POWER_LEVEL_3 = 0x03,    /*!< radio frequency power 3 level:-2dbm */
    SLE_RF_POWER_LEVEL_4 = 0x04,    /*!< radio frequency power 4 level:2dbm */
    SLE_RF_POWER_LEVEL_5 = 0x05,    /*!< radio frequency power 5 level:6dbm */
} sle_rf_power_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SLE_RF_POWER_LEVEL_0 | 0x00 | 射频功率0等级:-14dbm |
| SLE_RF_POWER_LEVEL_1 | 0x01 | 射频功率1等级:-10dbm |
| SLE_RF_POWER_LEVEL_2 | 0x02 | 射频功率2等级:-6dbm |
| SLE_RF_POWER_LEVEL_3 | 0x03 | 射频功率3等级:-2dbm |
| SLE_RF_POWER_LEVEL_4 | 0x04 | 射频功率4等级:2dbm |
| SLE_RF_POWER_LEVEL_5 | 0x05 | 射频功率5等级:6dbm |

### sle_rf_payload_type_t <a id="enum_sle_rf_payload_type_t"></a>

```c
typedef enum {
    SLE_RF_PATLOAD_TYPE_0 = 0x00,    /*!< radio frequency payload type:PRBS9 */
    SLE_RF_PATLOAD_TYPE_1 = 0x01,    /*!< radio frequency payload type:11110000 */
    SLE_RF_PATLOAD_TYPE_2 = 0x02,    /*!< radio frequency payload type:10101010 */
    SLE_RF_PATLOAD_TYPE_3 = 0x03,    /*!< radio frequency payload type:PRBS15 */
    SLE_RF_PATLOAD_TYPE_4 = 0x04,    /*!< radio frequency payload type:11111111 */
    SLE_RF_PATLOAD_TYPE_5 = 0x05,    /*!< radio frequency payload type:00000000 */
    SLE_RF_PATLOAD_TYPE_6 = 0x06,    /*!< radio frequency payload type:00001111 */
    SLE_RF_PATLOAD_TYPE_7 = 0x07,    /*!< radio frequency payload type:01010101 */
} sle_rf_payload_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SLE_RF_PATLOAD_TYPE_0 | 0x00 | 射频信息体类型:PRBS9 |
| SLE_RF_PATLOAD_TYPE_1 | 0x01 | 射频信息体类型:11110000 |
| SLE_RF_PATLOAD_TYPE_2 | 0x02 | 射频信息体类型:10101010 |
| SLE_RF_PATLOAD_TYPE_3 | 0x03 | 射频信息体类型:PRBS15 |
| SLE_RF_PATLOAD_TYPE_4 | 0x04 | 射频信息体类型:11111111 |
| SLE_RF_PATLOAD_TYPE_5 | 0x05 | 射频信息体类型:00000000 |
| SLE_RF_PATLOAD_TYPE_6 | 0x06 | 射频信息体类型:00001111 |
| SLE_RF_PATLOAD_TYPE_7 | 0x07 | 射频信息体类型:01010101 |

### sle_rf_phy_t <a id="enum_sle_rf_phy_t"></a>

```c
typedef enum {
    SLE_RF_PHY_1M = 0x00,      /*!< radio frequency physical 1M */
    SLE_RF_PHY_2M = 0x01,      /*!< radio frequency physical 2M */
    SLE_RF_PHY_4M = 0x04,      /*!< radio frequency physical 4M */
} sle_rf_phy_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SLE_RF_PHY_1M | 0x00 | 射频物理层1M |
| SLE_RF_PHY_2M | 0x01 | 射频物理层2M |
| SLE_RF_PHY_4M | 0x04 | 射频物理层4M |

### sle_rf_format_t <a id="enum_sle_rf_format_t"></a>

```c
typedef enum {
    SLE_RF_FORMAT_FRAME_TYPE_1 = 0x00,      /*!< radio frequency format: Wireless Frame Type 1 */
    SLE_RF_FORMAT_FRAME_TYPE_2 = 0x01,      /*!< radio frequency format: Wireless Frame Type 2 */
} sle_rf_format_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SLE_RF_FORMAT_FRAME_TYPE_1 | 0x00 | 射频格式:无线帧类型1 |
| SLE_RF_FORMAT_FRAME_TYPE_2 | 0x01 | 射频格式:无线帧类型2 |

### sle_rf_tx_rate_t <a id="enum_sle_rf_tx_rate_t"></a>

```c
typedef enum {
    SLE_RF_TX_RATE_GFSK = 0x00,      /*!< radio frequency tx rate:GFSK */
    SLE_RF_TX_RATE_QPSK = 0x02,      /*!< radio frequency tx rate:QPSK */
    SLE_RF_TX_RATE_8PSK = 0x03,      /*!< radio frequency tx rate:8PSK */
} sle_rf_tx_rate_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SLE_RF_TX_RATE_GFSK | 0x00 | 射频长发速率:GFSK (Gaussian Frequency Shift Keying) |
| SLE_RF_TX_RATE_QPSK | 0x02 | 射频长发速率:QPSK (Quadrature Phase Shift Keying) |
| SLE_RF_TX_RATE_8PSK | 0x03 | 射频长发速率:8PSK |

### sle_rf_pilot_ratio_t <a id="enum_sle_rf_pilot_ratio_t"></a>

```c
typedef enum {
    SLE_RF_PILOT_RATIO_NO = 0x00,        /*!< radio frequency pilot ratio,No */
    SLE_RF_PILOT_RATIO_1_1 = 0x01,       /*!< radio frequency pilot ratio,1:1 */
    SLE_RF_PILOT_RATIO_4_1 = 0x02,       /*!< radio frequency pilot ratio,4:1 */
    SLE_RF_PILOT_RATIO_16_1 = 0x03,      /*!< radio frequency pilot ratio,16:1 */
} sle_rf_pilot_ratio_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SLE_RF_PILOT_RATIO_NO | 0x00 | 射频导频比:No |
| SLE_RF_PILOT_RATIO_1_1 | 0x01 | 射频导频比:1:1 |
| SLE_RF_PILOT_RATIO_4_1 | 0x02 | 射频导频比:4:1 |
| SLE_RF_PILOT_RATIO_16_1 | 0x03 | 射频导频比:16:1 |

### sle_rf_tx_polar_t <a id="enum_sle_rf_tx_polar_t"></a>

```c
typedef enum {
    SLE_RF_TX_POLAR_NO = 0x00,        /*!< radio frequency tx polar,No */
    SLE_RF_TX_POLAR_2_3 = 0x01,       /*!< radio frequency tx polar,2/3 */
    SLE_RF_TX_POLAR_3_4 = 0x02,       /*!< radio frequency tx polar,3/4 */
    SLE_RF_TX_POLAR_5_6 = 0x03,       /*!< radio frequency tx polar,5/6 */
} sle_rf_tx_polar_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SLE_RF_TX_POLAR_NO | 0x00 | 射频发射极化编码:No |
| SLE_RF_TX_POLAR_2_3 | 0x01 | 射频发射极化编码:2/3 |
| SLE_RF_TX_POLAR_3_4 | 0x02 | 射频发射极化编码:3/4 |
| SLE_RF_TX_POLAR_5_6 | 0x03 | 射频发射极化编码:5/6 |

## Structures

### sle_rf_tx_start_t <a id="struct_sle_rf_tx_start_t"></a>

```c
typedef struct {
    uint8_t tx_freq;            /*!< tx frequency,Scope:0x00~0x4E,2402+x */
    uint8_t tx_power;           /*!< tx power { sle_rf_power_t } */
    uint16_t test_data_len;     /*!< tx test data len,Scope:0x00~0xFF */
    uint8_t pk_payload_type;    /*!< radio frequency payload type { sle_rf_payload_type_t } */
    uint8_t tx_phy;             /*!< radio frequency physical { sle_rf_phy_t } */
    uint8_t tx_format;          /*!< tx format { sle_rf_format_t } */
    uint8_t tx_rate;            /*!< radio frequency tx rate { sle_rf_tx_rate_t } */
    uint8_t tx_pilot_ratio;     /*!< radio frequency pilot ratio { sle_rf_pilot_ratio_t } */
    uint8_t tx_polar_r;         /*!< radio frequency tx polar { sle_rf_tx_polar_t } */
    uint16_t tx_interval;       /*!< radio frequency tx interval,Scope:0x0006~0xFFFF,
                                          one unit length is 125 us. */
} sle_rf_tx_start_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| tx_freq | uint8_t | 发送频率，范围:0x00~0x4E，2402+x |
| tx_power | uint8_t | 发射功率，参考 [sle_rf_power_t](#enum_sle_rf_power_t) |
| test_data_len | uint16_t | 发射测试数据长度，范围:0x00~0xFF |
| pk_payload_type | uint8_t | 信息体类型，参考 [sle_rf_payload_type_t](#enum_sle_rf_payload_type_t) |
| tx_phy | uint8_t | 射频物理层，参考 [sle_rf_phy_t](#enum_sle_rf_phy_t) |
| tx_format | uint8_t | 发射格式，参考 [sle_rf_format_t](#enum_sle_rf_format_t) |
| tx_rate | uint8_t | 射频长发速率，参考 [sle_rf_tx_rate_t](#enum_sle_rf_tx_rate_t) |
| tx_pilot_ratio | uint8_t | 射频导频比，参考 [sle_rf_pilot_ratio_t](#enum_sle_rf_pilot_ratio_t) |
| tx_polar_r | uint8_t | 射频发射极化编码，参考 [sle_rf_tx_polar_t](#enum_sle_rf_tx_polar_t) |
| tx_interval | uint16_t | 射频发射间隔，范围:0x0006~0xFFFF，1个单位长度为125us |

### sle_rf_rx_start_t <a id="struct_sle_rf_rx_start_t"></a>

```c
typedef struct {
    uint8_t rx_freq;            /*!< tx frequency,Scope:0x00~0x4E */
    uint8_t rx_phy;             /*!< radio frequency physical { sle_rf_phy_t } */
    uint8_t rx_format;          /*!< tx format { sle_rf_format_t } */
    uint8_t rx_pilot_ratio;     /*!< radio frequency pilot ratio { sle_rf_pilot_ratio_t } */
    uint16_t rx_interval;       /*!< radio frequency rx interval,Scope:0x0000~0xFFFF,
                                          one unit length is 125 us. */
} sle_rf_rx_start_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| rx_freq | uint8_t | 接收频率，范围:0x00~0x4E |
| rx_phy | uint8_t | 射频物理层，参考 [sle_rf_phy_t](#enum_sle_rf_phy_t) |
| rx_format | uint8_t | 接收格式，参考 [sle_rf_format_t](#enum_sle_rf_format_t) |
| rx_pilot_ratio | uint8_t | 射频导频比，参考 [sle_rf_pilot_ratio_t](#enum_sle_rf_pilot_ratio_t) |
| rx_interval | uint16_t | 射频接收间隔，范围:0x0000~0xFFFF，1个单位长度为125us |

### sle_rf_trx_end_cmp_evt_t <a id="struct_sle_rf_trx_end_cmp_evt_t"></a>

```c
typedef struct {
    uint8_t status;             /*!< event complete status */
    uint16_t num_packets;       /*!< num packets */
    uint8_t rssi;               /*!< rssi,Default:-127 */
} sle_rf_trx_end_cmp_evt_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| status | uint8_t | 事件完成状态 |
| num_packets | uint16_t | 数据包数 |
| rssi | uint8_t | 接收信号强度指示，默认-127 |

### sle_factory_callbacks_t <a id="struct_sle_factory_callbacks_t"></a>

```c
typedef struct {
    sle_rf_tx_start_callback rf_tx_start_cb;    /*!< radio frequency tx start callback. */
    sle_rf_rx_start_callback rf_rx_start_cb;    /*!< radio frequency rx start callback. */
    sle_rf_trx_end_callback rf_trx_end_cb;      /*!< radio frequency tx and rx end callback. */
    sle_rf_reset_callback rf_reset_cb;          /*!< radio frequency tx and rx reset callback. */
} sle_factory_callbacks_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| rf_tx_start_cb | [sle_rf_tx_start_callback](#sle_rf_tx_start_callback) | 射频长发回调函数 |
| rf_rx_start_cb | [sle_rf_rx_start_callback](#sle_rf_rx_start_callback) | 射频长收回调函数 |
| rf_trx_end_cb | [sle_rf_trx_end_callback](#sle_rf_trx_end_callback) | 射频收发结束回调函数 |
| rf_reset_cb | [sle_rf_reset_callback](#sle_rf_reset_callback) | 射频收发复位回调 |

## Type definitions

### sle_rf_tx_start_callback <a id="sle_rf_tx_start_callback"></a>

```c
typedef void (*sle_rf_tx_start_callback)(errcode_t status);
```

**功能说明**

- 射频长发启动时的回调函数
- 在SLE service上下文中调用，不应阻塞或长时间等待
- 指针由SLE service申请内存并释放，回调中不应释放

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| status | [errcode_t](#typedef_errcode_t) | 执行结果错误码 | - |

### sle_rf_rx_start_callback <a id="sle_rf_rx_start_callback"></a>

```c
typedef void (*sle_rf_rx_start_callback)(errcode_t status);
```

**功能说明**

- 射频长收启动时的回调函数
- 在SLE service上下文中调用，不应阻塞或长时间等待
- 指针由SLE service申请内存并释放，回调中不应释放

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| status | [errcode_t](#typedef_errcode_t) | 执行结果错误码 | - |

### sle_rf_trx_end_callback <a id="sle_rf_trx_end_callback"></a>

```c
typedef void (*sle_rf_trx_end_callback)(sle_rf_trx_end_cmp_evt_t* cmp_evt);
```

**功能说明**

- 关闭射频收发时的回调函数
- 在SLE service上下文中调用，不应阻塞或长时间等待
- 指针由SLE service申请内存并释放，回调中不应释放

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| cmp_evt | [sle_rf_trx_end_cmp_evt_t](#struct_sle_rf_trx_end_cmp_evt_t)* | 执行结果返回结构体 | 非NULL |

### sle_rf_reset_callback <a id="sle_rf_reset_callback"></a>

```c
typedef void (*sle_rf_reset_callback)(errcode_t status);
```

**功能说明**

- 重置射频收发时的回调函数
- 在SLE service上下文中调用，不应阻塞或长时间等待
- 指针由SLE service申请内存并释放，回调中不应释放

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| status | [errcode_t](#typedef_errcode_t) | 执行结果错误码 | - |

### errcode_t <a id="typedef_errcode_t"></a>

```c
typedef uint32_t errcode_t;
```

**使用说明**

作为SLE factory manager接口的返回值类型，用于表示接口调用的执行结果。



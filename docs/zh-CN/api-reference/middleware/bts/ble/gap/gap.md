# BLE GAP

GAP (Generic Access Profile) 是 BLE (Bluetooth Low Energy) 协议栈的基础层 API。管理 BLE 协议栈使能/去使能、本地设备地址/名称/外观设置、广播数据和参数配置（启动/停止/过滤）、扫描参数和扫描启停、配对/绑定管理、连接参数更新和 PHY (Physical Layer) 配置。

GAP 是 BLE 通信的入口——广播和扫描是设备发现的前提，配对和绑定是安全通信的基础。

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [enable_ble](#enable_ble) | 使能BLE协议栈 |
| [disable_ble](#disable_ble) | 去使能BLE协议栈 |
| [ble_is_enable](#ble_is_enable) | 检测BLE协议栈是否使能 |
| [gap_ble_set_local_addr](#gap_ble_set_local_addr) | 设置本地设备地址 |
| [gap_ble_get_local_addr](#gap_ble_get_local_addr) | 获取本地设备地址 |
| [gap_ble_set_local_appearance](#gap_ble_set_local_appearance) | 设置本地设备外观类型 |
| [gap_ble_set_local_name](#gap_ble_set_local_name) | 设置本地设备名称 |
| [gap_ble_get_local_name](#gap_ble_get_local_name) | 获取本地设备名称 |
| [gap_ble_set_adv_data](#gap_ble_set_adv_data) | 设置广播数据 |
| [gap_ble_set_adv_param](#gap_ble_set_adv_param) | 设置广播参数 |
| [gap_ble_start_adv](#gap_ble_start_adv) | 开始发送广播 |
| [gap_ble_stop_adv](#gap_ble_stop_adv) | 停止发送广播 |
| [gap_ble_set_adv_random_delay](#gap_ble_set_adv_random_delay) | 设置广播随机延迟开关 |
| [gap_ble_set_scan_parameters](#gap_ble_set_scan_parameters) | 设置扫描参数 |
| [gap_ble_set_scan_extern_parameters](#gap_ble_set_scan_extern_parameters) | 设置扫描扩展参数 |
| [gap_ble_set_adv_data_filter](#gap_ble_set_adv_data_filter) | 设置扫描结果广播数据过滤条件 |
| [gap_ble_clean_adv_data_filter](#gap_ble_clean_adv_data_filter) | 清理扫描结果广播数据过滤条件 |
| [gap_ble_start_scan](#gap_ble_start_scan) | 开始扫描 |
| [gap_ble_stop_scan](#gap_ble_stop_scan) | 停止扫描 |
| [gap_ble_set_phy](#gap_ble_set_phy) | 设置BLE PHY参数 |
| [gap_ble_set_data_length](#gap_ble_set_data_length) | 设置BLE发包参数 |
| [gap_ble_pair_remote_device](#gap_ble_pair_remote_device) | 启动与远端设备配对 |
| [gap_ble_get_paired_devices_num](#gap_ble_get_paired_devices_num) | 获取配对设备数量 |
| [gap_ble_get_paired_devices](#gap_ble_get_paired_devices) | 获取配对设备地址列表 |
| [gap_ble_get_pair_state](#gap_ble_get_pair_state) | 获取指定设备的配对状态 |
| [gap_ble_remove_pair](#gap_ble_remove_pair) | 与指定设备取消配对 |
| [gap_ble_remove_all_pairs](#gap_ble_remove_all_pairs) | 删除所有BLE配对设备 |
| [gap_ble_get_bonded_devices](#gap_ble_get_bonded_devices) | 获取已绑定设备列表 |
| [gap_ble_connect_param_update](#gap_ble_connect_param_update) | 更新BLE连接参数 |
| [ble_set_feature](#ble_set_feature) | 配置连接特性 |
| [gap_ble_connect_remote_device](#gap_ble_connect_remote_device) | 与远端设备建立连接 |
| [gap_ble_disconnect_remote_device](#gap_ble_disconnect_remote_device) | 断开与远端设备的连接 |
| [gap_ble_set_sec_param](#gap_ble_set_sec_param) | 设置安全参数 |
| [gap_ble_read_remote_device_rssi](#gap_ble_read_remote_device_rssi) | 读取远端设备RSSI (Received Signal Strength Indicator) |
| [gap_ble_create_connection_cancel](#gap_ble_create_connection_cancel) | 取消创建连接 |
| [gap_ble_register_callbacks](#gap_ble_register_callbacks) | 注册BLE GAP回调函数 |
| [bth_ota_init](#bth_ota_init) | 初始化BTH OTA (Over-The-Air)通道 |
| [ble_customize_max_pwr](#ble_customize_max_pwr) | 配置BLE和SLE (SparkLink Low Energy)最大功率 |

## Functions

### enable_ble <a id="enable_ble"></a>

```c
errcode_t enable_ble(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 使能BLE协议栈，启动BLE底层协议栈服务
- 启动结果通过回调函数 [gap_ble_enable_callback](#typedef_gap_ble_enable_callback) 异步返回
- 调用后需等待回调确认启动成功后再进行广播、扫描等后续操作

**前置条件**

- BLE协议栈未使能，即 [ble_is_enable](#ble_is_enable) 返回 false
- 已通过 [gap_ble_register_callbacks](#gap_ble_register_callbacks) 注册回调函数，以接收启动结果

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考errcode_bt_t | 执行失败 |

**参考案例**

- [ble_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_client/src/ble_speed_client.c)

### disable_ble <a id="disable_ble"></a>

```c
errcode_t disable_ble(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 去使能BLE协议栈，关闭BLE底层协议栈服务
- 关闭结果通过回调函数 [gap_ble_disable_callback](#typedef_gap_ble_disable_callback) 异步返回
- 调用后BLE相关功能不可用，直到再次调用 [enable_ble](#enable_ble)

**前置条件**

- BLE协议栈已使能，即 [ble_is_enable](#ble_is_enable) 返回 true

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 去使能成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [hilink_ble_adapter.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/hilink_ble_adapter.c)

### ble_is_enable <a id="ble_is_enable"></a>

```c
bool ble_is_enable(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 检测BLE协议栈是否已使能
- 返回布尔值指示协议栈当前使能状态
- 可在调用 [enable_ble](#enable_ble) 或 [disable_ble](#disable_ble) 后调用此接口确认状态

**前置条件**

- 无

**返回值**

- 返回类型：bool

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| true | BLE协议栈已使能 | BLE协议栈已启动 |
| false | BLE协议栈未使能 | BLE协议栈未启动 |

### gap_ble_set_local_addr <a id="gap_ble_set_local_addr"></a>

```c
errcode_t gap_ble_set_local_addr(const bd_addr_t *addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 设置本地蓝牙设备地址
- 地址类型由 [bd_addr_t](#struct_bd_addr_t) 中的 type 字段指定，支持公有地址和随机地址
- 需在BLE协议栈使能后、启动广播前设置

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功
- 入参 addr 不为 NULL，且指向有效的蓝牙地址数据

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | const [bd_addr_t](#struct_bd_addr_t) * | 本地设备地址指针 | 非NULL，地址长度为6字节 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 地址设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [ble_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_server/src/ble_speed_server.c)

### gap_ble_get_local_addr <a id="gap_ble_get_local_addr"></a>

```c
errcode_t gap_ble_get_local_addr(bd_addr_t *addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 获取本地蓝牙设备地址
- 地址通过出参 addr 返回
- 需在BLE协议栈使能后调用

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功
- 入参 addr 不为 NULL，且指向已分配的内存空间

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | [bd_addr_t](#struct_bd_addr_t) * | 本地设备地址输出缓冲区 | 非NULL，缓冲区大小不小于sizeof(bd_addr_t) |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| addr | [bd_addr_t](#struct_bd_addr_t) | 本地设备地址 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 地址获取成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [hilink_ble_adapter.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/hilink_ble_adapter.c)

### gap_ble_set_local_appearance <a id="gap_ble_set_local_appearance"></a>

```c
errcode_t gap_ble_set_local_appearance(uint16_t appearance)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 设置本地设备外观类型，用于标识设备类别
- 外观值参考 Bluetooth SIG Appearance Values 定义
- 需在BLE协议栈使能后、启动广播前设置

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| appearance | uint16_t | 本地设备外观类型 | [gap_ble_appearance_type_t](#enum_gap_ble_appearance_type_t) 中的枚举值 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 外观设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### gap_ble_set_local_name <a id="gap_ble_set_local_name"></a>

```c
errcode_t gap_ble_set_local_name(const uint8_t *name, const uint8_t len)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 设置本地蓝牙设备名称
- 名称长度包含结束符
- 需在BLE协议栈使能后、启动广播前设置

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功
- 入参 name 不为 NULL，且指向的内存空间长度不小于 len

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| name | const uint8_t * | 设备名称字符串 | 非NULL，最大长度BT_DEVICE_NAME_MAX_LEN(32) |
| len | const uint8_t | 名称长度，包含结束符 | 1 ~ 32 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 名称设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### gap_ble_get_local_name <a id="gap_ble_get_local_name"></a>

```c
errcode_t gap_ble_get_local_name(uint8_t *name, uint8_t *len)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 获取本地蓝牙设备名称
- len 作为入参时为用户分配的缓冲区大小，作为出参时为设备名称实际长度
- 需在BLE协议栈使能后调用

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功
- 入参 name 不为 NULL，且指向已分配的内存空间
- 入参 len 不为 NULL，且指向的值不小于缓冲区大小

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| name | uint8_t * | 设备名称输出缓冲区 | 非NULL，缓冲区大小不小于BT_DEVICE_NAME_MAX_LEN(32) |
| len | uint8_t * | 入参为缓冲区大小，出参为名称实际长度 | 非NULL，入参值不小于名称实际长度 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| name | uint8_t * | 本地设备名称字符串 |
| len | uint8_t * | 本地设备名称实际长度 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 名称获取成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### gap_ble_set_adv_data <a id="gap_ble_set_adv_data"></a>

```c
errcode_t gap_ble_set_adv_data(uint8_t adv_id, const gap_ble_config_adv_data_t *data)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 设置指定广播ID的广播数据，包括广播数据和扫描响应数据
- 设置结果通过回调函数 [gap_ble_set_adv_data_callback](#typedef_gap_ble_set_adv_data_callback) 异步返回
- 需在设置广播参数后、启动广播前调用

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功
- 已通过 [gap_ble_set_adv_param](#gap_ble_set_adv_param) 设置广播参数
- 入参 data 不为 NULL，且指向有效的广播数据结构

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| adv_id | uint8_t | 广播ID | 0 |
| data | const [gap_ble_config_adv_data_t](#struct_gap_ble_config_adv_data_t) * | 广播数据指针 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 数据设置请求成功发起 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [ble_speed_server_adv.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_server/src/ble_speed_server_adv.c)

### gap_ble_set_adv_param <a id="gap_ble_set_adv_param"></a>

```c
errcode_t gap_ble_set_adv_param(uint8_t adv_id, const gap_ble_adv_params_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 设置指定广播ID的广播参数，包括广播间隔、类型、通道、过滤策略等
- 设置结果通过回调函数 [gap_ble_set_adv_param_callback](#typedef_gap_ble_set_adv_param_callback) 异步返回
- 需在启动广播前设置广播参数

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功
- 入参 param 不为 NULL，且指向有效的广播参数结构

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| adv_id | uint8_t | 广播ID | 0 |
| param | const [gap_ble_adv_params_t](#struct_gap_ble_adv_params_t) * | 广播参数指针 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 参数设置请求成功发起 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [ble_speed_server_adv.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_server/src/ble_speed_server_adv.c)

### gap_ble_start_adv <a id="gap_ble_start_adv"></a>

```c
errcode_t gap_ble_start_adv(uint8_t adv_id)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 启动指定广播ID的广播发送
- 启动结果通过回调函数 [gap_ble_start_adv_callback](#typedef_gap_ble_start_adv_callback) 异步返回
- 需在设置广播参数和广播数据后调用

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功
- 已通过 [gap_ble_set_adv_param](#gap_ble_set_adv_param) 设置广播参数
- 已通过 [gap_ble_set_adv_data](#gap_ble_set_adv_data) 设置广播数据

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| adv_id | uint8_t | 广播ID | 0 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 广播启动请求成功发起 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [ble_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_server/src/ble_speed_server.c)

### gap_ble_stop_adv <a id="gap_ble_stop_adv"></a>

```c
errcode_t gap_ble_stop_adv(uint8_t adv_id)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 停止指定广播ID的广播发送
- 停止结果通过回调函数 [gap_ble_stop_adv_callback](#typedef_gap_ble_stop_adv_callback) 异步返回
- 需在广播进行中时调用

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功
- 指定广播ID的广播已通过 [gap_ble_start_adv](#gap_ble_start_adv) 启动

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| adv_id | uint8_t | 广播ID | 0 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 广播停止请求成功发起 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [hilink_ble_adapter.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/hilink_ble_adapter.c)

### gap_ble_set_adv_random_delay <a id="gap_ble_set_adv_random_delay"></a>

```c
errcode_t gap_ble_set_adv_random_delay(uint8_t adv_id, uint8_t enable)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 设置指定广播ID的广播随机延迟开关
- 开启后广播发送间隔会增加随机延迟，减少多设备间广播冲突
- 需在启动广播前设置

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| adv_id | uint8_t | 广播ID | 0 |
| enable | uint8_t | 是否开启随机延迟 | 1: 开启，0: 关闭 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 设置成功 |

### gap_ble_set_scan_parameters <a id="gap_ble_set_scan_parameters"></a>

```c
errcode_t gap_ble_set_scan_parameters(const gap_ble_scan_params_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 设置BLE扫描参数，包括扫描间隔、扫描窗长、扫描类型、PHY类型和过滤策略
- 设置结果通过回调函数 [gap_ble_set_scan_param_callback](#typedef_gap_ble_set_scan_param_callback) 异步返回
- 需在启动扫描前设置

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功
- 入参 param 不为 NULL，且指向有效的扫描参数结构

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| param | const [gap_ble_scan_params_t](#struct_gap_ble_scan_params_t) * | 扫描参数指针 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 参数设置请求成功发起 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [ble_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_client/src/ble_speed_client.c)

### gap_ble_set_scan_extern_parameters <a id="gap_ble_set_scan_extern_parameters"></a>

```c
errcode_t gap_ble_set_scan_extern_parameters(const gap_ble_extern_scan_params_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 设置BLE扫描扩展参数，包括重复广播过滤配置、扫描持续时间和扫描周期
- 需在启动扫描前设置

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功
- 入参 param 不为 NULL，且指向有效的扩展扫描参数结构

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| param | const [gap_ble_extern_scan_params_t](#struct_gap_ble_extern_scan_params_t) * | 扫描扩展参数指针 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### gap_ble_set_adv_data_filter <a id="gap_ble_set_adv_data_filter"></a>

```c
errcode_t gap_ble_set_adv_data_filter(uint8_t filter_count, gap_ble_adv_data_filter_t *filter_key)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 设置对扫描结果中的广播数据进行过滤的条件，最多设置5个过滤条件
- 设置结果通过回调函数 [gap_ble_set_adv_data_filter_callback](#typedef_gap_ble_set_adv_data_filter_callback) 异步返回
- 需在启动扫描前设置

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功
- 入参 filter_key 不为 NULL，且指向有效的过滤条件数组

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| filter_count | uint8_t | 过滤条件个数 | 1 ~ 5 |
| filter_key | [gap_ble_adv_data_filter_t](#struct_gap_ble_adv_data_filter_t) * | 过滤条件数值数组 | 非NULL，filter_key_len最大128字节 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 设置请求成功发起 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### gap_ble_clean_adv_data_filter <a id="gap_ble_clean_adv_data_filter"></a>

```c
errcode_t gap_ble_clean_adv_data_filter(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 清理对扫描结果中的广播数据进行过滤的条件
- 清理结果通过回调函数 [gap_ble_clean_adv_data_filter_callback](#typedef_gap_ble_clean_adv_data_filter_callback) 异步返回
- 清理后扫描结果将不再按过滤条件筛选

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功
- 已通过 [gap_ble_set_adv_data_filter](#gap_ble_set_adv_data_filter) 设置过过滤条件

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 清理请求成功发起 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### gap_ble_start_scan <a id="gap_ble_start_scan"></a>

```c
errcode_t gap_ble_start_scan(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 启动BLE扫描
- 扫描结果通过回调函数 [gap_ble_scan_result_callback](#typedef_gap_ble_scan_result_callback) 异步返回
- 需在设置扫描参数后调用

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功
- 已通过 [gap_ble_set_scan_parameters](#gap_ble_set_scan_parameters) 设置扫描参数

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 扫描启动请求成功发起 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [ble_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_client/src/ble_speed_client.c)

### gap_ble_stop_scan <a id="gap_ble_stop_scan"></a>

```c
errcode_t gap_ble_stop_scan(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 停止BLE扫描
- 需在扫描进行中时调用

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功
- 扫描已通过 [gap_ble_start_scan](#gap_ble_start_scan) 启动

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 扫描停止成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [ble_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_client/src/ble_speed_client.c)

### gap_ble_set_phy <a id="gap_ble_set_phy"></a>

```c
errcode_t gap_ble_set_phy(gap_le_set_phy_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 设置BLE PHY参数，包括连接句柄、发送/接收PHY类型和PHY选项
- 需在BLE连接已建立后调用

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功
- 已与远端设备建立BLE连接
- 入参 param 不为 NULL，且指向有效的PHY参数结构

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| param | [gap_le_set_phy_t](#struct_gap_le_set_phy_t) * | BLE PHY参数指针 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | PHY设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [ble_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_server/src/ble_speed_server.c)

### gap_ble_set_data_length <a id="gap_ble_set_data_length"></a>

```c
errcode_t gap_ble_set_data_length(gap_le_set_data_length_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 设置BLE发包参数，包括连接句柄、最大发送字节数和最大发送时间
- 需在BLE连接已建立后调用

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功
- 已与远端设备建立BLE连接
- 入参 param 不为 NULL，且指向有效的发包参数结构

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| param | [gap_le_set_data_length_t](#struct_gap_le_set_data_length_t) * | BLE发包参数指针 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 发包参数设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [ble_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_server/src/ble_speed_server.c)

### gap_ble_pair_remote_device <a id="gap_ble_pair_remote_device"></a>

```c
errcode_t gap_ble_pair_remote_device(const bd_addr_t *addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 启动与指定远端设备的配对流程
- 配对结果通过回调函数 [gap_ble_paired_complete_callback](#typedef_gap_ble_paired_complete_callback) 异步返回
- 需在BLE连接已建立后调用

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功
- 已与远端设备建立BLE连接
- 入参 addr 不为 NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | const [bd_addr_t](#struct_bd_addr_t) * | 远端设备地址 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 配对请求成功发起 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [ble_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_client/src/ble_speed_client.c)

### gap_ble_get_paired_devices_num <a id="gap_ble_get_paired_devices_num"></a>

```c
errcode_t gap_ble_get_paired_devices_num(uint16_t *number)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 获取已配对设备的数量
- 数量通过出参 number 返回

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功
- 入参 number 不为 NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| number | uint16_t * | 配对设备数量输出指针 | 非NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| number | uint16_t | 配对设备数量 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 获取成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### gap_ble_get_paired_devices <a id="gap_ble_get_paired_devices"></a>

```c
errcode_t gap_ble_get_paired_devices(bd_addr_t *addr, uint16_t *number)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 获取已配对设备的地址列表
- number 作为入参时为用户分配的缓冲区大小，作为出参时为实际配对设备数量
- 需先调用 [gap_ble_get_paired_devices_num](#gap_ble_get_paired_devices_num) 获取设备数量以分配足够缓冲区

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功
- 入参 addr 不为 NULL，且缓冲区大小不小于 number * sizeof(bd_addr_t)
- 入参 number 不为 NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | [bd_addr_t](#struct_bd_addr_t) * | 配对设备地址输出缓冲区 | 非NULL |
| number | uint16_t * | 入参为缓冲区大小，出参为实际配对设备数量 | 非NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| addr | [bd_addr_t](#struct_bd_addr_t) | 配对设备地址列表 |
| number | uint16_t | 实际配对设备数量 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 获取成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### gap_ble_get_pair_state <a id="gap_ble_get_pair_state"></a>

```c
errcode_t gap_ble_get_pair_state(const bd_addr_t *addr, gap_ble_pair_state_t *status)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 获取指定设备的配对状态
- 配对状态通过出参 status 返回

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功
- 入参 addr 不为 NULL
- 入参 status 不为 NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | const [bd_addr_t](#struct_bd_addr_t) * | 待查询的设备地址 | 非NULL |
| status | [gap_ble_pair_state_t](#enum_gap_ble_pair_state_t) * | 配对状态输出指针 | 非NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| status | [gap_ble_pair_state_t](#enum_gap_ble_pair_state_t) | 配对状态 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 获取成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### gap_ble_remove_pair <a id="gap_ble_remove_pair"></a>

```c
errcode_t gap_ble_remove_pair(const bd_addr_t *addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 与指定远端设备取消配对
- 取消配对后该设备的配对信息将被清除

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功
- 入参 addr 不为 NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端设备地址 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 取消配对成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [ble_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_client/src/ble_speed_client.c)

### gap_ble_remove_all_pairs <a id="gap_ble_remove_all_pairs"></a>

```c
errcode_t gap_ble_remove_all_pairs(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 删除所有BLE配对设备信息
- 清除后所有配对记录将被移除

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 删除成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [ble_wifi_cfg_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_wifi_cfg_client/src/ble_wifi_cfg_client.c)

### gap_ble_get_bonded_devices <a id="gap_ble_get_bonded_devices"></a>

```c
errcode_t gap_ble_get_bonded_devices(bd_addr_t *addr, uint16_t *number)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 获取本端设备的已绑定设备列表
- number 作为入参时为用户分配的缓冲区大小，作为出参时为实际绑定设备数量

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功
- 入参 addr 不为 NULL，且缓冲区大小不小于 number * sizeof(bd_addr_t)
- 入参 number 不为 NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | [bd_addr_t](#struct_bd_addr_t) * | 绑定设备地址输出缓冲区 | 非NULL |
| number | uint16_t * | 入参为缓冲区大小，出参为实际绑定设备数量 | 非NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| addr | [bd_addr_t](#struct_bd_addr_t) | 绑定设备地址列表 |
| number | uint16_t | 实际绑定设备数量 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 获取成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### gap_ble_connect_param_update <a id="gap_ble_connect_param_update"></a>

```c
errcode_t gap_ble_connect_param_update(gap_conn_param_update_t *params)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 更新BLE连接参数，包括连接间隔、从设备延迟和超时时间
- 需在BLE连接已建立后调用

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功
- 已与远端设备建立BLE连接
- 入参 params 不为 NULL，且指向有效的连接参数更新结构

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| params | [gap_conn_param_update_t](#struct_gap_conn_param_update_t) * | 待更新连接参数 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 参数更新成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [ble_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_server/src/ble_speed_server.c)

### ble_set_feature <a id="ble_set_feature"></a>

```c
errcode_t ble_set_feature(ble_feature_type_t feature, bool val)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 配置BLE连接特性，通过特性编号启用或禁用对应特性
- 当前支持的特性编号见 [ble_feature_type_t](#enum_ble_feature_type_t)

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| feature | [ble_feature_type_t](#enum_ble_feature_type_t) | 特性编号 | [ble_feature_type_t](#enum_ble_feature_type_t) 中的枚举值 |
| val | bool | 启用/禁用对应特性 | true: 启用，false: 禁用 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 特性配置成功 |

### gap_ble_connect_remote_device <a id="gap_ble_connect_remote_device"></a>

```c
errcode_t gap_ble_connect_remote_device(const bd_addr_t *addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 与远端设备建立ACL (Asynchronous Connection-Less) 连接
- 连接状态通过回调函数 [gap_ble_connect_state_changed_callback](#typedef_gap_ble_connect_state_changed_callback) 异步返回
- 需在BLE协议栈使能后调用

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功
- 入参 addr 不为 NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | const [bd_addr_t](#struct_bd_addr_t) * | 待连接的设备地址 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 连接请求成功发起 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [ble_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_client/src/ble_speed_client.c)

### gap_ble_disconnect_remote_device <a id="gap_ble_disconnect_remote_device"></a>

```c
errcode_t gap_ble_disconnect_remote_device(const bd_addr_t *addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 断开与远端设备的连接，包括所有profile连接
- 连接状态通过回调函数 [gap_ble_connect_state_changed_callback](#typedef_gap_ble_connect_state_changed_callback) 异步返回

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功
- 已与远端设备建立BLE连接
- 入参 addr 不为 NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | const [bd_addr_t](#struct_bd_addr_t) * | 待断开的设备地址 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 断开请求成功发起 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [hilink_ble_adapter.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/hilink_ble_adapter.c)

### gap_ble_set_sec_param <a id="gap_ble_set_sec_param"></a>

```c
errcode_t gap_ble_set_sec_param(gap_ble_sec_params_t *params)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 设置BLE安全参数，包括绑定能力、输入输出能力、安全配对能力和安全模式
- 需在BLE协议栈使能后、发起连接前设置

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功
- 入参 params 不为 NULL，且指向有效的安全参数结构

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| params | [gap_ble_sec_params_t](#struct_gap_ble_sec_params_t) * | 安全参数指针 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 安全参数设置成功 |

**参考案例**

- [hilink_ble_adapter.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/hilink_ble_adapter.c)

### gap_ble_read_remote_device_rssi <a id="gap_ble_read_remote_device_rssi"></a>

```c
errcode_t gap_ble_read_remote_device_rssi(uint16_t conn_id)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 通过连接ID读取远端设备的RSSI值
- RSSI结果通过回调函数 [gap_ble_read_rssi_callback](#typedef_gap_ble_read_rssi_callback) 异步返回
- 需在BLE连接已建立后调用

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功
- 已与远端设备建立BLE连接
- conn_id 为有效的连接ID

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| conn_id | uint16_t | 连接ID | 有效的连接ID |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | RSSI读取请求成功发起 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### gap_ble_create_connection_cancel <a id="gap_ble_create_connection_cancel"></a>

```c
errcode_t gap_ble_create_connection_cancel(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 取消正在创建中的BLE连接
- 需在连接创建过程中（已调用 [gap_ble_connect_remote_device](#gap_ble_connect_remote_device) 但尚未收到连接回调）调用

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功
- 已发起连接请求但连接尚未建立

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 取消连接成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### gap_ble_register_callbacks <a id="gap_ble_register_callbacks"></a>

```c
errcode_t gap_ble_register_callbacks(gap_ble_callbacks_t *func)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 注册BLE GAP回调函数，用于接收BLE协议栈各类事件通知
- 包含BLE启停、广播、扫描、连接、配对、RSSI读取等事件回调
- 需在调用 [enable_ble](#enable_ble) 前注册

**前置条件**

- BLE协议栈未使能或已使能
- 入参 func 不为 NULL，且指向有效的回调函数结构体

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func | [gap_ble_callbacks_t](#struct_gap_ble_callbacks_t) * | 回调函数接口定义指针 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 注册成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [ble_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_client/src/ble_speed_client.c)

### bth_ota_init <a id="bth_ota_init"></a>

```c
errcode_t bth_ota_init(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 初始化BTH OTA通道
- 需在BLE协议栈使能后调用

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | OTA通道初始化成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### ble_customize_max_pwr <a id="ble_customize_max_pwr"></a>

```c
errcode_t ble_customize_max_pwr(int8_t ble_pwr, int8_t sle_pwr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_le_gap.h"
```

**功能说明**

- 配置BLE和SLE的最大发射功率
- 需在BLE协议栈使能后调用

**前置条件**

- BLE协议栈已通过 [enable_ble](#enable_ble) 使能成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| ble_pwr | int8_t | BLE最大功率 | -127 ~ 20 dBm |
| sle_pwr | int8_t | SLE最大功率 | -127 ~ 20 dBm |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 功率配置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

## Type definitions

### errcode_t <a id="typedef_errcode_t"></a>

```c
typedef uint32_t errcode_t;
```

**使用说明**

作为模块内所有接口的返回值类型，表示接口执行结果错误码

### bt_addr_type <a id="typedef_bt_addr_type"></a>

```c
typedef enum {
    BT_ADDRESS_TYPE_PUBLIC_DEVICE_ADDRESS = 0x00,
    BT_ADDRESS_TYPE_RANDOM_DEVICE_ADDRESS = 0x01,
} bt_addr_type;
```

**使用说明**

bd_addr_t 结构体中 type 字段的取值枚举

### gap_ble_enable_callback <a id="typedef_gap_ble_enable_callback"></a>

```c
typedef void (*gap_ble_enable_callback)(errcode_t status);
```

**使用说明**

enable_ble 启动结果回调函数类型

### gap_ble_disable_callback <a id="typedef_gap_ble_disable_callback"></a>

```c
typedef void (*gap_ble_disable_callback)(errcode_t status);
```

**使用说明**

disable_ble 关闭结果回调函数类型

### gap_ble_start_adv_callback <a id="typedef_gap_ble_start_adv_callback"></a>

```c
typedef void (*gap_ble_start_adv_callback)(uint8_t adv_id, adv_status_t status);
```

**使用说明**

gap_ble_start_adv 启动广播结果回调函数类型

### gap_ble_stop_adv_callback <a id="typedef_gap_ble_stop_adv_callback"></a>

```c
typedef void (*gap_ble_stop_adv_callback)(uint8_t adv_id, adv_status_t status);
```

**使用说明**

gap_ble_stop_adv 停止广播结果回调函数类型

### gap_ble_set_adv_data_callback <a id="typedef_gap_ble_set_adv_data_callback"></a>

```c
typedef void (*gap_ble_set_adv_data_callback)(uint8_t adv_id, errcode_t status);
```

**使用说明**

gap_ble_set_adv_data 设置广播数据结果回调函数类型

### gap_ble_set_adv_param_callback <a id="typedef_gap_ble_set_adv_param_callback"></a>

```c
typedef void (*gap_ble_set_adv_param_callback)(uint8_t adv_id, errcode_t status);
```

**使用说明**

gap_ble_set_adv_param 设置广播参数结果回调函数类型

### gap_ble_scan_result_callback <a id="typedef_gap_ble_scan_result_callback"></a>

```c
typedef void (*gap_ble_scan_result_callback)(gap_scan_result_data_t *scan_result_data);
```

**使用说明**

gap_ble_start_scan 扫描结果回调函数类型

### gap_ble_set_scan_param_callback <a id="typedef_gap_ble_set_scan_param_callback"></a>

```c
typedef void (*gap_ble_set_scan_param_callback)(errcode_t status);
```

**使用说明**

gap_ble_set_scan_parameters 设置扫描参数结果回调函数类型

### gap_ble_set_adv_data_filter_callback <a id="typedef_gap_ble_set_adv_data_filter_callback"></a>

```c
typedef void (*gap_ble_set_adv_data_filter_callback)(errcode_t status);
```

**使用说明**

gap_ble_set_adv_data_filter 设置过滤条件结果回调函数类型

### gap_ble_clean_adv_data_filter_callback <a id="typedef_gap_ble_clean_adv_data_filter_callback"></a>

```c
typedef void (*gap_ble_clean_adv_data_filter_callback)(errcode_t status);
```

**使用说明**

gap_ble_clean_adv_data_filter 清理过滤条件结果回调函数类型

### gap_ble_connect_state_changed_callback <a id="typedef_gap_ble_connect_state_changed_callback"></a>

```c
typedef void (*gap_ble_connect_state_changed_callback)(uint16_t conn_id, bd_addr_t *addr,
    gap_ble_conn_state_t conn_state, gap_ble_pair_state_t pair_state, gap_ble_disc_reason_t disc_reason);
```

**使用说明**

gap_ble_connect_remote_device 和 gap_ble_disconnect_remote_device 连接状态变化回调函数类型

### gap_ble_paired_complete_callback <a id="typedef_gap_ble_paired_complete_callback"></a>

```c
typedef void (*gap_ble_paired_complete_callback)(uint16_t conn_id, const bd_addr_t *addr, errcode_t status);
```

**使用说明**

gap_ble_pair_remote_device 配对完成回调函数类型

### gap_ble_terminate_adv_callback <a id="typedef_gap_ble_terminate_adv_callback"></a>

```c
typedef void (*gap_ble_terminate_adv_callback)(uint8_t adv_id, adv_status_t status);
```

**使用说明**

广播被动中止回调函数类型

### gap_ble_read_rssi_callback <a id="typedef_gap_ble_read_rssi_callback"></a>

```c
typedef void (*gap_ble_read_rssi_callback)(uint16_t conn_id, int8_t rssi, errcode_t status);
```

**使用说明**

gap_ble_read_remote_device_rssi RSSI读取结果回调函数类型

### gap_ble_auth_complete_callback <a id="typedef_gap_ble_auth_complete_callback"></a>

```c
typedef void (*gap_ble_auth_complete_callback)(uint16_t conn_id, const bd_addr_t *addr, errcode_t status,
    const ble_auth_info_evt_t* evt);
```

**使用说明**

认证完成回调函数类型

### gap_ble_connect_param_update_callback <a id="typedef_gap_ble_connect_param_update_callback"></a>

```c
typedef void (*gap_ble_connect_param_update_callback)(uint16_t conn_id, errcode_t status,
    const gap_ble_conn_param_update_t *param);
```

**使用说明**

gap_ble_connect_param_update 连接参数更新结果回调函数类型

## Enumerations

### errcode_bt_t <a id="enum_errcode_bt_t"></a>

```c
typedef enum {
    ERRCODE_BT_SUCCESS = 0x00,
    ERRCODE_BT_FAIL = ERRCODE_BT_COMMON_BASE,
    ERRCODE_BT_NOT_READY,
    ERRCODE_BT_MALLOC_FAIL,
    ERRCODE_BT_MEMCPY_FAIL,
    ERRCODE_BT_BUSY,
    ERRCODE_BT_DONE,
    ERRCODE_BT_UNSUPPORTED,
    ERRCODE_BT_PARAM_ERR,
    ERRCODE_BT_STATE_ERR,
    ERRCODE_BT_UNHANDLED,
    ERRCODE_BT_AUTH_FAIL,
    ERRCODE_BT_RMT_DEV_DOWN,
    ERRCODE_BT_AUTH_REJECTED,
    ERRCODE_BT_MAX = ERRCODE_BT_COMMON_END
} errcode_bt_t;
```

**使用说明**

BLE接口返回值中 Other 错误码的参考枚举类型

### gap_ble_appearance_type_t <a id="enum_gap_ble_appearance_type_t"></a>

```c
typedef enum {
    GAP_BLE_APPEARANCE_TYPE_UNKNOWN = 00,
    GAP_BLE_APPEARANCE_TYPE_GENERIC_PHONE = 64,
    GAP_BLE_APPEARANCE_TYPE_GENERIC_COMPUTER = 128,
    GAP_BLE_APPEARANCE_TYPE_GENERIC_WATCH = 192,
    GAP_BLE_APPEARANCE_TYPE_GENERIC_DISPLAY = 320,
    GAP_BLE_APPEARANCE_TYPE_GENERIC_HID = 960,
    GAP_BLE_APPEARANCE_TYPE_KEYBOARD = 961,
    GAP_BLE_APPEARANCE_TYPE_MOUSE = 962,
    GAP_BLE_APPEARANCE_TYPE_DIGITAL_PEN = 967,
} gap_ble_appearance_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GAP_BLE_APPEARANCE_TYPE_UNKNOWN | 0 | 未知类型 |
| GAP_BLE_APPEARANCE_TYPE_GENERIC_PHONE | 64 | 通用手机 |
| GAP_BLE_APPEARANCE_TYPE_GENERIC_COMPUTER | 128 | 通用电脑 |
| GAP_BLE_APPEARANCE_TYPE_GENERIC_WATCH | 192 | 通用手表 |
| GAP_BLE_APPEARANCE_TYPE_GENERIC_DISPLAY | 320 | 通用显示器 |
| GAP_BLE_APPEARANCE_TYPE_GENERIC_HID | 960 | 通用人机界面设备 |
| GAP_BLE_APPEARANCE_TYPE_KEYBOARD | 961 | 键盘 |
| GAP_BLE_APPEARANCE_TYPE_MOUSE | 962 | 鼠标 |
| GAP_BLE_APPEARANCE_TYPE_DIGITAL_PEN | 967 | 电子笔 |

### gap_ble_adv_filter_allow_scan_t <a id="enum_gap_ble_adv_filter_allow_scan_t"></a>

```c
typedef enum {
    GAP_BLE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY = 0x00,
    GAP_BLE_ADV_FILTER_ALLOW_SCAN_WLST_CON_ANY = 0x01,
    GAP_BLE_ADV_FILTER_ALLOW_SCAN_ANY_CON_WLST = 0x02,
    GAP_BLE_ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST = 0x03,
} gap_ble_adv_filter_allow_scan_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GAP_BLE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY | 0x00 | 处理所有设备的扫描和连接请求 |
| GAP_BLE_ADV_FILTER_ALLOW_SCAN_WLST_CON_ANY | 0x01 | 处理所有连接请求，仅处理白名单的扫描请求 |
| GAP_BLE_ADV_FILTER_ALLOW_SCAN_ANY_CON_WLST | 0x02 | 处理所有扫描请求，仅处理白名单的连接请求 |
| GAP_BLE_ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST | 0x03 | 仅处理白名单中扫描请求和连接请求 |

### gap_ble_adv_type_t <a id="enum_gap_ble_adv_type_t"></a>

```c
typedef enum {
    GAP_BLE_ADV_CONN_SCAN_UNDIR = 0,
    GAP_BLE_ADV_CONN_NONSCAN_DIR,
    GAP_BLE_ADV_NONCONN_SCAN_UNDIR,
    GAP_BLE_ADV_NONCONN_NONSCAN_UNDIR,
    GAP_BLE_ADV_CONN_NONSCAN_DIR_LOW_DUTY,
} gap_ble_adv_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GAP_BLE_ADV_CONN_SCAN_UNDIR | 0 | 可连接可扫描非定向广播(默认) |
| GAP_BLE_ADV_CONN_NONSCAN_DIR | 1 | 可连接不可扫描高频定向广播 |
| GAP_BLE_ADV_NONCONN_SCAN_UNDIR | 2 | 不可连接可扫描非定向广播 |
| GAP_BLE_ADV_NONCONN_NONSCAN_UNDIR | 3 | 不可连接不可扫描非定向广播 |
| GAP_BLE_ADV_CONN_NONSCAN_DIR_LOW_DUTY | 4 | 可连接不可扫描低频定向广播 |

### gap_ble_scan_type_t <a id="enum_gap_ble_scan_type_t"></a>

```c
typedef enum {
    GAP_BLE_SCAN_TYPE_PASSIVE = 0x00,
    GAP_BLE_SCAN_TYPE_ACTIVE,
} gap_ble_scan_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GAP_BLE_SCAN_TYPE_PASSIVE | 0x00 | 被动扫描 |
| GAP_BLE_SCAN_TYPE_ACTIVE | 0x01 | 主动扫描 |

### gap_ble_scan_filter_policy_t <a id="enum_gap_ble_scan_filter_policy_t"></a>

```c
typedef enum {
    GAP_BLE_SCAN_FILTER_POLICY_ACCEPT_ALL = 0x00,
    GAP_BLE_SCAN_FILTER_POLICY_ONLY_WHITE_LIST,
    GAP_BLE_SCAN_FILTER_POLICY_ACCEPT_ALL_AND_RPA,
    GAP_BLE_SCAN_FILTER_POLICY_ONLY_WHITE_LIST_AND_RPA,
} gap_ble_scan_filter_policy_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GAP_BLE_SCAN_FILTER_POLICY_ACCEPT_ALL | 0x00 | 接收所有广播(默认) |
| GAP_BLE_SCAN_FILTER_POLICY_ONLY_WHITE_LIST | 0x01 | 只接收白名单里设备的广播 |
| GAP_BLE_SCAN_FILTER_POLICY_ACCEPT_ALL_AND_RPA | 0x02 | 接收所有非定向广播、可解析私有地址的定向广播、发给本设备的定向广播 |
| GAP_BLE_SCAN_FILTER_POLICY_ONLY_WHITE_LIST_AND_RPA | 0x03 | 接收白名单中非定向广播、可解析私有地址的定向广播、发给本设备的定向广播 |

### gap_ble_scan_result_evt_type_t <a id="enum_gap_ble_scan_result_evt_type_t"></a>

```c
typedef enum {
    GAP_BLE_EVT_NON_CONNECTABLE_NON_SCANNABLE = 0x00,
    GAP_BLE_EVT_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED = 0x04,
    GAP_BLE_EVT_CONNECTABLE = 0x01,
    GAP_BLE_EVT_CONNECTABLE_DIRECTED = 0x05,
    GAP_BLE_EVT_SCANNABLE = 0x02,
    GAP_BLE_EVT_SCANNABLE_DIRECTED = 0x06,
    GAP_BLE_EVT_LEGACY_NON_CONNECTABLE = 0x10,
    GAP_BLE_EVT_LEGACY_SCANNABLE = 0x12,
    GAP_BLE_EVT_LEGACY_CONNECTABLE = 0x13,
    GAP_BLE_EVT_LEGACY_CONNECTABLE_DIRECTED = 0x15,
    GAP_BLE_EVT_LEGACY_SCAN_RSP_TO_ADV_SCAN = 0x1A,
    GAP_BLE_EVT_LEGACY_SCAN_RSP_TO_ADV = 0x1B,
} gap_ble_scan_result_evt_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GAP_BLE_EVT_NON_CONNECTABLE_NON_SCANNABLE | 0x00 | 扩展的不可连接不可扫描非定向 |
| GAP_BLE_EVT_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED | 0x04 | 扩展的不可连接不可扫描定向 |
| GAP_BLE_EVT_CONNECTABLE | 0x01 | 扩展的可连接非定向 |
| GAP_BLE_EVT_CONNECTABLE_DIRECTED | 0x05 | 扩展的可连接定向 |
| GAP_BLE_EVT_SCANNABLE | 0x02 | 扩展的可扫描非定向 |
| GAP_BLE_EVT_SCANNABLE_DIRECTED | 0x06 | 扩展的可扫描定向 |
| GAP_BLE_EVT_LEGACY_NON_CONNECTABLE | 0x10 | 传统的不可连接非定向 |
| GAP_BLE_EVT_LEGACY_SCANNABLE | 0x12 | 传统的可扫描非定向 |
| GAP_BLE_EVT_LEGACY_CONNECTABLE | 0x13 | 传统的可连接可扫描非定向 |
| GAP_BLE_EVT_LEGACY_CONNECTABLE_DIRECTED | 0x15 | 传统的可连接定向 |
| GAP_BLE_EVT_LEGACY_SCAN_RSP_TO_ADV_SCAN | 0x1A | 传统的与ADV_SCAN_IND对应的扫描响应 |
| GAP_BLE_EVT_LEGACY_SCAN_RSP_TO_ADV | 0x1B | 传统的与ADV_IND对应的扫描响应 |

### gap_ble_pair_info_switch_t <a id="enum_gap_ble_pair_info_switch_t"></a>

```c
typedef enum {
    GAP_BLE_PAIR_INFO_UNAVAILABLE = 0x00,
    GAP_BLE_PAIR_INFO_AVAILABLE = 0x01,
} gap_ble_pair_info_switch_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GAP_BLE_PAIR_INFO_UNAVAILABLE | 0x00 | 配对信息不可获取 |
| GAP_BLE_PAIR_INFO_AVAILABLE | 0x01 | 配对信息可获取 |

### gap_ble_save_pair_keys_mode_switch_t <a id="enum_gap_ble_save_pair_keys_mode_switch_t"></a>

```c
typedef enum {
    GAP_BLE_SAVE_SMP_KEYS_AUTO = 0x00,
    GAP_BLE_SAVE_SMP_KEYS_MANU = 0x01,
} gap_ble_save_pair_keys_mode_switch_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GAP_BLE_SAVE_SMP_KEYS_AUTO | 0x00 | 秘钥自动保存 |
| GAP_BLE_SAVE_SMP_KEYS_MANU | 0x01 | 秘钥用户手动保存 |

### gap_ble_scan_result_data_status_t <a id="enum_gap_ble_scan_result_data_status_t"></a>

```c
typedef enum {
    GAP_BLE_DATA_COMPLETE = 0x00,
    GAP_BLE_DATA_INCOMPLETE_MORE_TO_COME = 0x01,
    GAP_BLE_DATA_INCOMPLETE_TRUNCATED = 0x02,
} gap_ble_scan_result_data_status_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GAP_BLE_DATA_COMPLETE | 0x00 | 完整数据或最后一个片段 |
| GAP_BLE_DATA_INCOMPLETE_MORE_TO_COME | 0x01 | 不完整的数据 |
| GAP_BLE_DATA_INCOMPLETE_TRUNCATED | 0x02 | 被截断不完整的数据 |

### gap_ble_phy_type_t <a id="enum_gap_ble_phy_type_t"></a>

```c
typedef enum {
    GAP_BLE_PHY_NO_PACKET = 0x00,
    GAP_BLE_PHY_1M = 0x01,
    GAP_BLE_PHY_2M = 0x02,
    GAP_BLE_PHY_CODED = 0x03,
} gap_ble_phy_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GAP_BLE_PHY_NO_PACKET | 0x00 | 无广播包 |
| GAP_BLE_PHY_1M | 0x01 | 1M PHY |
| GAP_BLE_PHY_2M | 0x02 | 2M PHY |
| GAP_BLE_PHY_CODED | 0x03 | Coded PHY |

### adv_status_t <a id="enum_adv_status_t"></a>

```c
typedef enum {
    ADV_STATUS_STOPPED = 0x00,
    ADV_STATUS_ADVERTISING,
} adv_status_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| ADV_STATUS_STOPPED | 0x00 | 广播停止 |
| ADV_STATUS_ADVERTISING | 0x01 | 正在广播 |

### gap_ble_filter_duplicates_t <a id="enum_gap_ble_filter_duplicates_t"></a>

```c
typedef enum {
    GAP_BLE_FILTER_DUPLICATES_DISABLE = 0,
    GAP_BLE_FILTER_DUPLICATES_ENABLE,
    GAP_BLE_FILTER_DUPLICATES_ENABLE_FOR_PERIOD,
} gap_ble_filter_duplicates_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GAP_BLE_FILTER_DUPLICATES_DISABLE | 0 | 上报每个收到的广播包 |
| GAP_BLE_FILTER_DUPLICATES_ENABLE | 1 | 不上报重复的广播包 |
| GAP_BLE_FILTER_DUPLICATES_ENABLE_FOR_PERIOD | 2 | 周期内不上报重复的广播包 |

### gap_ble_pair_state_t <a id="enum_gap_ble_pair_state_t"></a>

```c
typedef enum {
    GAP_BLE_PAIR_NONE = 0x01,
    GAP_BLE_PAIR_PAIRING = 0x02,
    GAP_BLE_PAIR_PAIRED = 0x03,
} gap_ble_pair_state_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GAP_BLE_PAIR_NONE | 0x01 | 未配对状态 |
| GAP_BLE_PAIR_PAIRING | 0x02 | 正在配对 |
| GAP_BLE_PAIR_PAIRED | 0x03 | 已完成配对 |

### gap_ble_disc_reason_t <a id="enum_gap_ble_disc_reason_t"></a>

```c
typedef enum {
    GAP_BLE_DISCONN_UNKNOWN = 0x00,
    GAP_BLE_ERR_CONN_TIMEOUT = 0x8,
    GAP_BLE_DICSCONNECT_BY_REMOTE_USER = 0x13,
    GAP_BLE_CONN_TERMINATE_BY_LOCAL_HOST = 0x16,
} gap_ble_disc_reason_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GAP_BLE_DISCONN_UNKNOWN | 0x00 | 未知原因断链 |
| GAP_BLE_ERR_CONN_TIMEOUT | 0x08 | 连接超时断链 |
| GAP_BLE_DICSCONNECT_BY_REMOTE_USER | 0x13 | 远端用户断链 |
| GAP_BLE_CONN_TERMINATE_BY_LOCAL_HOST | 0x16 | 本端HOST断链 |

### gap_ble_conn_state_t <a id="enum_gap_ble_conn_state_t"></a>

```c
typedef enum {
    GAP_BLE_STATE_DISCONNECTED,
    GAP_BLE_STATE_CONNECTED,
} gap_ble_conn_state_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GAP_BLE_STATE_DISCONNECTED | 0 | BLE GAP ACL 已断连 |
| GAP_BLE_STATE_CONNECTED | 1 | BLE GAP ACL 已连接 |

### gap_ble_sec_mode_t <a id="enum_gap_ble_sec_mode_t"></a>

```c
typedef enum {
    GAP_BLE_GAP_SECURITY_MODE1_LEVEL1 = 0,
    GAP_BLE_GAP_SECURITY_MODE1_LEVEL2,
    GAP_BLE_GAP_SECURITY_MODE1_LEVEL3,
    GAP_BLE_GAP_SECURITY_MODE1_LEVEL4,
    GAP_BLE_GAP_SECURITY_MODE2_LEVEL1,
    GAP_BLE_GAP_SECURITY_MODE2_LEVEL2,
} gap_ble_sec_mode_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GAP_BLE_GAP_SECURITY_MODE1_LEVEL1 | 0 | 没有安全能力 |
| GAP_BLE_GAP_SECURITY_MODE1_LEVEL2 | 1 | 不需要认证基于链路进行配对和加密 |
| GAP_BLE_GAP_SECURITY_MODE1_LEVEL3 | 2 | 需要认证基于链路进行配对和加密 |
| GAP_BLE_GAP_SECURITY_MODE1_LEVEL4 | 3 | 需要认证基于链路采用ECDH (Elliptic Curve Diffie-Hellman) 算法进行加密和配对 |
| GAP_BLE_GAP_SECURITY_MODE2_LEVEL1 | 4 | 不需要认证基于数据进行配对和加密 |
| GAP_BLE_GAP_SECURITY_MODE2_LEVEL2 | 5 | 需要认证基于数据进行配对和加密 |

### gap_ble_io_ability_t <a id="enum_gap_ble_io_ability_t"></a>

```c
typedef enum {
    GAP_BLE_IO_CAPABILITY_DISPLAYONLY = 0,
    GAP_BLE_IO_CAPABILITY_DISPLAYYESNO,
    GAP_BLE_IO_CAPABILITY_KEYBOARDONLY,
    GAP_BLE_IO_CAPABILITY_NOINPUTNOOUTPUT,
    GAP_BLE_IO_CAPABILITY_KEYBOARDDISPLAY,
} gap_ble_io_ability_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GAP_BLE_IO_CAPABILITY_DISPLAYONLY | 0 | 只展示 |
| GAP_BLE_IO_CAPABILITY_DISPLAYYESNO | 1 | 展示，并且可以选择Yes或者No |
| GAP_BLE_IO_CAPABILITY_KEYBOARDONLY | 2 | 只支持键盘 |
| GAP_BLE_IO_CAPABILITY_NOINPUTNOOUTPUT | 3 | 没有输入输出 |
| GAP_BLE_IO_CAPABILITY_KEYBOARDDISPLAY | 4 | 支持键盘和展示 |

### ble_feature_type_t <a id="enum_ble_feature_type_t"></a>

```c
typedef enum {
    BLE_FEATURE_CONNNECTION_ASYNC = 0x00,
} ble_feature_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| BLE_FEATURE_CONNNECTION_ASYNC | 0x00 | 异步连接 |

## Structures

### bd_addr_t <a id="struct_bd_addr_t"></a>

```c
typedef struct {
    uint8_t addr[BD_ADDR_LEN];
    uint8_t type;
} bd_addr_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| addr | uint8_t[6] | 蓝牙地址，长度为BD_ADDR_LEN(6) |
| type | uint8_t | 蓝牙地址类型，参考bt_addr_type |

### gap_conn_param_update_t <a id="struct_gap_conn_param_update_t"></a>

```c
typedef struct {
    uint16_t conn_handle;
    uint16_t interval_min;
    uint16_t interval_max;
    uint16_t slave_latency;
    uint16_t timeout_multiplier;
} gap_conn_param_update_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| conn_handle | uint16_t | 连接ID |
| interval_min | uint16_t | 最小间隔 |
| interval_max | uint16_t | 最大间隔 |
| slave_latency | uint16_t | 从设备回复最小间隔 |
| timeout_multiplier | uint16_t | 超时断连间隔 |

### gap_le_set_phy_t <a id="struct_gap_le_set_phy_t"></a>

```c
typedef struct {
    uint16_t conn_handle;
    uint8_t all_phys;
    uint8_t tx_phys;
    uint8_t rx_phys;
    uint16_t phy_options;
} gap_le_set_phy_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| conn_handle | uint16_t | 连接句柄 |
| all_phys | uint8_t | 所有PHY |
| tx_phys | uint8_t | 发送PHY |
| rx_phys | uint8_t | 接收PHY |
| phy_options | uint16_t | PHY选项 |

### gap_le_set_data_length_t <a id="struct_gap_le_set_data_length_t"></a>

```c
typedef struct {
    uint16_t conn_handle;
    uint16_t maxtxoctets;
    uint16_t maxtxtime;
} gap_le_set_data_length_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| conn_handle | uint16_t | 连接句柄 |
| maxtxoctets | uint16_t | 最大字节数 |
| maxtxtime | uint16_t | 最大发送时间 |

### gap_ble_config_adv_data_t <a id="struct_gap_ble_config_adv_data_t"></a>

```c
typedef struct {
    uint16_t adv_length;
    uint8_t *adv_data;
    uint16_t scan_rsp_length;
    uint8_t *scan_rsp_data;
} gap_ble_config_adv_data_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| adv_length | uint16_t | 广播数据长度 |
| adv_data | uint8_t * | 广播数据 |
| scan_rsp_length | uint16_t | 扫描响应数据长度 |
| scan_rsp_data | uint8_t * | 扫描响应数据 |

### gap_ble_adv_data_filter_t <a id="struct_gap_ble_adv_data_filter_t"></a>

```c
typedef struct {
    uint8_t *filter_key;
    uint8_t filter_key_len;
    uint8_t filter_key_offset;
} gap_ble_adv_data_filter_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| filter_key | uint8_t * | 扫描结果广播数据过滤键值，键值最大长度128字节 |
| filter_key_len | uint8_t | 扫描结果广播数据过滤键值数据长度 |
| filter_key_offset | uint8_t | 扫描结果广播数据过滤键值在广播数据中的偏移位置，无效设置为0xFF |

### gap_ble_adv_params_t <a id="struct_gap_ble_adv_params_t"></a>

```c
typedef struct {
    uint32_t min_interval;
    uint32_t max_interval;
    uint8_t adv_type;
    bd_addr_t own_addr;
    bd_addr_t peer_addr;
    uint8_t channel_map;
    uint8_t adv_filter_policy;
    int8_t   tx_power;
    uint32_t duration;
    uint8_t max_events;
} gap_ble_adv_params_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| min_interval | uint32_t | 最小的广播间隔[N * 0.625ms] |
| max_interval | uint32_t | 最大的广播间隔[N * 0.625ms] |
| adv_type | uint8_t | 广播类型，参考gap_ble_adv_type_t |
| own_addr | bd_addr_t | 本端地址 |
| peer_addr | bd_addr_t | 对端地址 |
| channel_map | uint8_t | 广播通道选择：0x01使用37通道，0x07使用37/38/39三个通道 |
| adv_filter_policy | uint8_t | 白名单过滤策略，参考gap_ble_adv_filter_allow_scan_t |
| tx_power | int8_t | 发送功率，单位dbm，范围-127~20 |
| duration | uint32_t | 广播持续发送时长 |
| max_events | uint8_t | 广播发送次数，单位：次 |

### gap_ble_scan_params_t <a id="struct_gap_ble_scan_params_t"></a>

```c
typedef struct {
    uint16_t scan_interval;
    uint16_t scan_window;
    uint8_t scan_type;
    uint8_t scan_phy;
    uint8_t scan_filter_policy;
} gap_ble_scan_params_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| scan_interval | uint16_t | 扫描间隔[N * 0.625ms] |
| scan_window | uint16_t | 扫描窗长[N * 0.625ms] |
| scan_type | uint8_t | 扫描类型，参考gap_ble_scan_type_t |
| scan_phy | uint8_t | PHY类型，参考gap_ble_phy_type_t |
| scan_filter_policy | uint8_t | 扫描过滤策略，参考gap_ble_scan_filter_policy_t |

### gap_scan_result_data_t <a id="struct_gap_scan_result_data_t"></a>

```c
typedef struct {
    uint8_t event_type;
    uint8_t data_status;
    bd_addr_t addr;
    uint8_t primary_phy;
    uint8_t secondary_phy;
    uint8_t adv_sid;
    int8_t tx_power;
    int8_t rssi;
    uint16_t periodic_adv_interval;
    bd_addr_t direct_addr;
    uint8_t adv_len;
    uint8_t *adv_data;
} gap_scan_result_data_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| event_type | uint8_t | 广播类型，参考gap_ble_scan_result_evt_type_t |
| data_status | uint8_t | 扫描结果数据状态，参考gap_ble_scan_result_data_status_t |
| addr | bd_addr_t | 地址 |
| primary_phy | uint8_t | 主广播PHY类型，参考gap_ble_phy_type_t |
| secondary_phy | uint8_t | 辅广播PHY类型，参考gap_ble_phy_type_t |
| adv_sid | uint8_t | 广播SID |
| tx_power | int8_t | 发送功率，范围: -127 ~ +20dBm |
| rssi | int8_t | 信号强度，范围: -127 ~ +20dBm |
| periodic_adv_interval | uint16_t | 周期广播间隔[N * 1.25ms] |
| direct_addr | bd_addr_t | 定向广播地址 |
| adv_len | uint8_t | 广播数据长度 |
| adv_data | uint8_t * | 广播数据 |

### gap_ble_extern_scan_params_t <a id="struct_gap_ble_extern_scan_params_t"></a>

```c
typedef struct {
    uint8_t filter_duplicate;
    uint8_t limited;
    uint16_t duration;
    uint16_t period;
} gap_ble_extern_scan_params_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| filter_duplicate | uint8_t | 扫描的过滤配置，参考gap_ble_filter_duplicates_t |
| limited | uint8_t | 保留字段 |
| duration | uint16_t | 扫描的持续时间，0表示持续扫描，默认值为0，单位: 10ms |
| period | uint16_t | 扫描周期，取0时扫描执行duration时间后会超时结束，默认值为0，单位: 1.28s |

### gap_ble_conn_param_update_t <a id="struct_gap_ble_conn_param_update_t"></a>

```c
typedef struct {
    uint16_t interval;
    uint16_t latency;
    uint16_t timeout;
} gap_ble_conn_param_update_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| interval | uint16_t | 链路调度间隔，单位slot |
| latency | uint16_t | 延迟周期，单位slot |
| timeout | uint16_t | 超时时间，单位10ms |

### gap_ble_sec_params_t <a id="struct_gap_ble_sec_params_t"></a>

```c
typedef struct {
    uint8_t bondable;
    uint8_t io_capability;
    uint8_t sc_enable;
    uint8_t sc_mode;
} gap_ble_sec_params_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| bondable | uint8_t | 绑定能力选择：0x01支持绑定，0x00不支持绑定 |
| io_capability | uint8_t | 输入输出能力，参考gap_ble_io_ability_t |
| sc_enable | uint8_t | 安全配对能力选择：0x01支持安全配对，0x00不支持安全配对 |
| sc_mode | uint8_t | 安全模式，参考gap_ble_sec_mode_t |

### ble_auth_info_evt_t <a id="struct_ble_auth_info_evt_t"></a>

```c
typedef struct {
    uint8_t ltk_len;
    uint8_t ltk[BLE_PAIRED_LTK_LEN];
} ble_auth_info_evt_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| ltk_len | uint8_t | 链路密钥长度 |
| ltk | uint8_t[16] | 链路密钥，长度为BLE_PAIRED_LTK_LEN(16) |

### gap_ble_callbacks_t <a id="struct_gap_ble_callbacks_t"></a>

```c
typedef struct {
    gap_ble_enable_callback ble_enable_cb;
    gap_ble_disable_callback ble_disable_cb;
    gap_ble_set_adv_data_callback set_adv_data_cb;
    gap_ble_set_adv_param_callback set_adv_param_cb;
    gap_ble_set_scan_param_callback set_scan_param_cb;
    gap_ble_start_adv_callback start_adv_cb;
    gap_ble_stop_adv_callback stop_adv_cb;
    gap_ble_scan_result_callback scan_result_cb;
    gap_ble_connect_state_changed_callback conn_state_change_cb;
    gap_ble_paired_complete_callback pair_result_cb;
    gap_ble_read_rssi_callback read_rssi_cb;
    gap_ble_terminate_adv_callback terminate_adv_cb;
    gap_ble_auth_complete_callback auth_complete_cb;
    gap_ble_connect_param_update_callback conn_param_update_cb;
    gap_ble_set_adv_data_filter_callback set_data_data_filter_cb;
    gap_ble_clean_adv_data_filter_callback clean_data_data_filter_cb;
} gap_ble_callbacks_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| ble_enable_cb | gap_ble_enable_callback | BLE启动回调函数 |
| ble_disable_cb | gap_ble_disable_callback | BLE关闭回调函数 |
| set_adv_data_cb | gap_ble_set_adv_data_callback | 设置广播数据回调函数 |
| set_adv_param_cb | gap_ble_set_adv_param_callback | 设置广播参数回调函数 |
| set_scan_param_cb | gap_ble_set_scan_param_callback | 设置扫描参数回调函数 |
| start_adv_cb | gap_ble_start_adv_callback | 开启广播回调函数 |
| stop_adv_cb | gap_ble_stop_adv_callback | 关闭广播回调函数 |
| scan_result_cb | gap_ble_scan_result_callback | 扫描结果回调函数 |
| conn_state_change_cb | gap_ble_connect_state_changed_callback | 连接状态改变回调函数 |
| pair_result_cb | gap_ble_paired_complete_callback | 配对完成回调函数 |
| read_rssi_cb | gap_ble_read_rssi_callback | 读取rssi回调函数 |
| terminate_adv_cb | gap_ble_terminate_adv_callback | 被动中止广播回调函数 |
| auth_complete_cb | gap_ble_auth_complete_callback | 认证完成回调函数 |
| conn_param_update_cb | gap_ble_connect_param_update_callback | 连接参数更新回调函数 |
| set_data_data_filter_cb | gap_ble_set_adv_data_filter_callback | 设置广播数据过滤回调函数 |
| clean_data_data_filter_cb | gap_ble_clean_adv_data_filter_callback | 清理广播数据过滤回调函数 |





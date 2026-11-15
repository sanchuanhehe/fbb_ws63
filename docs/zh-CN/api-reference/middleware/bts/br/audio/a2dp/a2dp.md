# A2DP

A2DP (Advanced Audio Distribution Profile) 蓝牙音频传输规范 Source 端 API。管理音频流连接建立/断开、音频编码参数（SBC/AAC 等）配置和音频数据推送。

WS63 作为音频源（Source）向蓝牙音箱/耳机等 Sink 设备推送音频流，适用蓝牙音频发射场景。

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_a2dp_source.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [a2dp_src_get_device_connect_state](#a2dp_src_get_device_connect_state) | 通过设备地址查询A2DP连接状态 |
| [a2dp_src_get_playing_state](#a2dp_src_get_playing_state) | 查询对端蓝牙设备A2DP播放状态 |
| [a2dp_src_connect](#a2dp_src_connect) | 与对端设备建立A2DP连接 |
| [a2dp_src_disconnect](#a2dp_src_disconnect) | 与对端设备断开A2DP连接 |
| [a2dp_src_get_active_device](#a2dp_src_get_active_device) | 查询当前活跃的A2DP对端设备地址 |
| [a2dp_src_start_playing](#a2dp_src_start_playing) | 设置A2DP音频流为开始播放状态 |
| [a2dp_src_suspend_playing](#a2dp_src_suspend_playing) | 设置A2DP音频流为暂停播放状态 |
| [a2dp_src_stop_playing](#a2dp_src_stop_playing) | 设置A2DP音频流为停止播放状态 |
| [a2dp_src_register_callbacks](#a2dp_src_register_callbacks) | 注册A2DP SRC上层应用回调函数 |
| [a2dp_src_deregister_callbacks](#a2dp_src_deregister_callbacks) | 去注册A2DP SRC上层应用回调函数 |

## Functions

### a2dp_src_get_device_connect_state <a id="a2dp_src_get_device_connect_state"></a>

```c
int a2dp_src_get_device_connect_state(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_a2dp_source.h"
```

**功能说明**

- 通过对端蓝牙设备地址查询该设备的A2DP SRC连接状态
- 返回值为[profile_connect_state_t](#enum_profile_connect_state_t)枚举，包含正在连接、已连接、正在断连、已断连四种状态
- 调用该接口不会触发连接状态变更，仅做查询操作

**前置条件**

- 蓝牙协议栈已初始化完成
- 入参bd_addr不为NULL，且指向有效的蓝牙设备地址

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息指针 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| PROFILE_STATE_CONNECTING(0x01) | 正在连接 | 正在建立A2DP连接 |
| PROFILE_STATE_CONNECTED(0x02) | 已连接 | A2DP连接已建立 |
| PROFILE_STATE_DISCONNECTING(0x03) | 正在断连 | 正在断开A2DP连接 |
| PROFILE_STATE_DISCONNECTED(0x04) | 已断连 | A2DP连接已断开 |

### a2dp_src_get_playing_state <a id="a2dp_src_get_playing_state"></a>

```c
int a2dp_src_get_playing_state(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_a2dp_source.h"
```

**功能说明**

- 当对端设备已处于连接状态时，通过设备地址查询对端蓝牙设备的A2DP播放状态
- 返回值为[a2dp_playing_state_t](#enum_a2dp_playing_state_t)枚举，包含未播放和正在播放两种状态
- 需在对端设备已连接后调用，否则查询结果无意义

**前置条件**

- 蓝牙协议栈已初始化完成
- 对端设备已建立A2DP连接
- 入参bd_addr不为NULL，且指向已连接设备的蓝牙地址

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息指针 | 不为NULL，设备已连接 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| A2DP_NOT_PLAYING(0x00) | 未播放 | A2DP音频流未在播放 |
| A2DP_IS_PLAYING(0x01) | 正在播放 | A2DP音频流正在播放 |

### a2dp_src_connect <a id="a2dp_src_connect"></a>

```c
int a2dp_src_connect(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_a2dp_source.h"
```

**功能说明**

- 与对端蓝牙设备建立A2DP SRC连接
- 调用后异步发起连接请求，连接结果通过[connectstate_changed_cb](#struct_a2dp_src_callbacks_t)回调通知上层
- 重复调用可能导致连接异常，需在已断连状态下发起连接

**前置条件**

- 蓝牙协议栈已初始化完成
- 入参bd_addr不为NULL，且指向有效的对端蓝牙设备地址
- 当前未与目标设备建立A2DP连接

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息指针 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 连接请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### a2dp_src_disconnect <a id="a2dp_src_disconnect"></a>

```c
int a2dp_src_disconnect(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_a2dp_source.h"
```

**功能说明**

- 与对端蓝牙设备断开A2DP SRC连接
- 调用后异步发起断连请求，断连结果通过[connectstate_changed_cb](#struct_a2dp_src_callbacks_t)回调通知上层
- 需在已连接状态下调用，否则可能返回错误码

**前置条件**

- 蓝牙协议栈已初始化完成
- 入参bd_addr不为NULL，且指向已连接的对端蓝牙设备地址
- 当前与目标设备已建立A2DP连接

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息指针 | 不为NULL，设备已连接 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 断连请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### a2dp_src_get_active_device <a id="a2dp_src_get_active_device"></a>

```c
bd_addr_t a2dp_src_get_active_device(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_a2dp_source.h"
```

**功能说明**

- 查询当前A2DP SRC活跃的对端蓝牙设备地址
- 返回值为[bd_addr_t](#struct_bd_addr_t)结构体，包含设备地址和地址类型
- 当无活跃设备时返回全零地址

**前置条件**

- 蓝牙协议栈已初始化完成

**返回值**

- 返回类型：[bd_addr_t](#struct_bd_addr_t)

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| NOT NULL | 成功，返回活跃设备地址 | 存在活跃的A2DP连接设备 |
| NULL | 失败，无活跃设备 | 无活跃的A2DP连接设备 |

### a2dp_src_start_playing <a id="a2dp_src_start_playing"></a>

```c
int a2dp_src_start_playing(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_a2dp_source.h"
```

**功能说明**

- 设置A2DP音频流为开始播放（start streaming）状态
- 调用后向对端设备发送音频流开始指令，播放状态变更通过[playing_state_changed_cb](#struct_a2dp_src_callbacks_t)回调通知上层
- 需在已连接且未播放状态下调用

**前置条件**

- 蓝牙协议栈已初始化完成
- 入参bd_addr不为NULL，且指向已连接的对端蓝牙设备地址
- 当前A2DP音频流未处于播放状态

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息指针 | 不为NULL，设备已连接 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 播放请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### a2dp_src_suspend_playing <a id="a2dp_src_suspend_playing"></a>

```c
int a2dp_src_suspend_playing(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_a2dp_source.h"
```

**功能说明**

- 设置A2DP音频流为暂停播放（suspend streaming）状态
- 调用后向对端设备发送音频流暂停指令，播放状态变更通过[playing_state_changed_cb](#struct_a2dp_src_callbacks_t)回调通知上层
- 暂停后可通过a2dp_src_start_playing恢复播放

**前置条件**

- 蓝牙协议栈已初始化完成
- 入参bd_addr不为NULL，且指向已连接的对端蓝牙设备地址
- 当前A2DP音频流处于播放状态

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息指针 | 不为NULL，设备已连接 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 暂停请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### a2dp_src_stop_playing <a id="a2dp_src_stop_playing"></a>

```c
int a2dp_src_stop_playing(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_a2dp_source.h"
```

**功能说明**

- 设置A2DP音频流为停止播放（stop streaming）状态
- 调用后向对端设备发送音频流停止指令，播放状态变更通过[playing_state_changed_cb](#struct_a2dp_src_callbacks_t)回调通知上层
- 停止后需通过a2dp_src_start_playing重新开始播放

**前置条件**

- 蓝牙协议栈已初始化完成
- 入参bd_addr不为NULL，且指向已连接的对端蓝牙设备地址
- 当前A2DP音频流处于播放或暂停状态

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息指针 | 不为NULL，设备已连接 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 停止请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### a2dp_src_register_callbacks <a id="a2dp_src_register_callbacks"></a>

```c
int a2dp_src_register_callbacks(a2dp_src_callbacks_t *func)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_a2dp_source.h"
```

**功能说明**

- 注册A2DP SRC上层应用回调函数，用于接收连接状态变更、播放状态变更、编码格式变更等事件通知
- 回调函数运行于BTS (Bluetooth Subsystem) 线程上下文，回调中不应阻塞或长时间等待
- 回调中bd_addr相关内存由BTS申请和释放，回调中不应释放该内存

**前置条件**

- 蓝牙协议栈已初始化完成
- 入参func不为NULL，且指向有效的[a2dp_src_callbacks_t](#struct_a2dp_src_callbacks_t)回调结构体

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func | [a2dp_src_callbacks_t](#struct_a2dp_src_callbacks_t) * | 回调函数结构体指针 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 回调注册成功 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### a2dp_src_deregister_callbacks <a id="a2dp_src_deregister_callbacks"></a>

```c
int a2dp_src_deregister_callbacks(a2dp_src_callbacks_t *func)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_a2dp_source.h"
```

**功能说明**

- 去注册A2DP SRC上层应用回调函数，取消连接状态变更、播放状态变更、编码格式变更等事件通知
- 去注册后BTS不再调用该回调结构体中的回调函数
- 入参func应与注册时传入的指针一致

**前置条件**

- 蓝牙协议栈已初始化完成
- 入参func不为NULL，且指向已注册的[a2dp_src_callbacks_t](#struct_a2dp_src_callbacks_t)回调结构体
- 该回调结构体已通过a2dp_src_register_callbacks注册成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func | [a2dp_src_callbacks_t](#struct_a2dp_src_callbacks_t) * | 回调函数结构体指针 | 不为NULL，已注册 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 回调去注册成功 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

## Type definitions

### a2dp_src_connect_state_changed_callback <a id="typedef_a2dp_src_connect_state_changed_callback"></a>

```c
typedef void (*a2dp_src_connect_state_changed_callback)(bd_addr_t *bd_addr, int conn_state);
```

**使用说明**

作为[a2dp_src_callbacks_t](#struct_a2dp_src_callbacks_t)结构体中connectstate_changed_cb成员的类型，在A2DP SRC连接状态变更时由BTS调用

### a2dp_src_playing_state_changed_callback <a id="typedef_a2dp_src_playing_state_changed_callback"></a>

```c
typedef void (*a2dp_src_playing_state_changed_callback)(bd_addr_t *bd_addr, int playing_state);
```

**使用说明**

作为[a2dp_src_callbacks_t](#struct_a2dp_src_callbacks_t)结构体中playing_state_changed_cb成员的类型，在A2DP SRC播放状态变更时由BTS调用

### a2dp_src_configuration_changed_callback <a id="typedef_a2dp_src_configuration_changed_callback"></a>

```c
typedef void (*a2dp_src_configuration_changed_callback)(bd_addr_t *bd_addr, a2dp_codec_info_t *info);
```

**使用说明**

作为[a2dp_src_callbacks_t](#struct_a2dp_src_callbacks_t)结构体中configuration_changed_cb成员的类型，在A2DP SRC编码格式变更时由BTS调用

## Enumerations

### a2dp_codec_type_t <a id="enum_a2dp_codec_type_t"></a>

```c
typedef enum {
    A2DP_CODEC_TYPE_SBC = 0x00,        /*!< 编码格式类型为SBC。 */
    A2DP_CODEC_TYPE_AAC,               /*!< 编码格式类型为AAC。 */
    A2DP_CODEC_TYPE_APTX,              /*!< 编码格式类型为APTX。 */
    A2DP_CODEC_TYPE_APTX_HD,           /*!< 编码格式类型为APTX_HD。 */
    A2DP_CODEC_TYPE_LDAC,              /*!< 编码格式类型为LDAC。 */
    A2DP_CODEC_TYPE_LHDC,              /*!< 编码格式类型为LHDC。 */
    A2DP_CODEC_TYPE_LC3,               /*!< 编码格式类型为LC3。 */
    A2DP_CODEC_TYPE_L2HC,              /*!< 编码格式类型为L2HC。 */
    A2DP_CODEC_TYPE_INVALID = 0xF4240, /*!< 无效编码格式类型。 */
} a2dp_codec_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| A2DP_CODEC_TYPE_SBC | 0x00 | SBC (Sub-Band Coding) 编码格式 |
| A2DP_CODEC_TYPE_AAC | 0x01 | AAC (Advanced Audio Coding) 编码格式 |
| A2DP_CODEC_TYPE_APTX | 0x02 | APTX编码格式 |
| A2DP_CODEC_TYPE_APTX_HD | 0x03 | APTX_HD编码格式 |
| A2DP_CODEC_TYPE_LDAC | 0x04 | LDAC编码格式 |
| A2DP_CODEC_TYPE_LHDC | 0x05 | LHDC编码格式 |
| A2DP_CODEC_TYPE_LC3 | 0x06 | LC3编码格式 |
| A2DP_CODEC_TYPE_L2HC | 0x07 | L2HC编码格式 |
| A2DP_CODEC_TYPE_INVALID | 0xF4240 | 无效编码格式 |

### a2dp_optional_codec_support_state_t <a id="enum_a2dp_optional_codec_support_state_t"></a>

```c
typedef enum {
    A2DP_OPTIONAL_CODEC_NOT_SUPPORT = 0x00, /*!< 不支持设置编码格式类型。 */
    A2DP_OPTIONAL_CODEC_SUPPORT,            /*!< 支持设置编码格式类型。 */
    A2DP_OPTIONAL_CODEC_SUPPORT_UNKNOWN,    /*!< 支持用户定义编码格式类型。 */
} a2dp_optional_codec_support_state_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| A2DP_OPTIONAL_CODEC_NOT_SUPPORT | 0x00 | 不支持可选编码格式 |
| A2DP_OPTIONAL_CODEC_SUPPORT | 0x01 | 支持可选编码格式 |
| A2DP_OPTIONAL_CODEC_SUPPORT_UNKNOWN | 0x02 | 支持用户定义编码格式 |

### profile_connect_state_t <a id="enum_profile_connect_state_t"></a>

```c
typedef enum {
    PROFILE_STATE_CONNECTING = 0x01,    /*!< 正在连接。 */
    PROFILE_STATE_CONNECTED = 0x02,     /*!< 已连接。 */
    PROFILE_STATE_DISCONNECTING = 0x03, /*!< 正在断连。 */
    PROFILE_STATE_DISCONNECTED = 0x04   /*!< 已断连。 */
} profile_connect_state_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| PROFILE_STATE_CONNECTING | 0x01 | 正在连接 |
| PROFILE_STATE_CONNECTED | 0x02 | 已连接 |
| PROFILE_STATE_DISCONNECTING | 0x03 | 正在断连 |
| PROFILE_STATE_DISCONNECTED | 0x04 | 已断连 |

### a2dp_playing_state_t <a id="enum_a2dp_playing_state_t"></a>

```c
typedef enum {
    A2DP_NOT_PLAYING = 0x00, /*!< a2dp未播放。 */
    A2DP_IS_PLAYING          /*!< a2dp正在播放。 */
} a2dp_playing_state_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| A2DP_NOT_PLAYING | 0x00 | 未播放 |
| A2DP_IS_PLAYING | 0x01 | 正在播放 |

### bt_addr_type <a id="enum_bt_addr_type"></a>

```c
typedef enum {
    BT_ADDRESS_TYPE_PUBLIC_DEVICE_ADDRESS          =  0x00, /*!< 蓝牙公有地址。 */
    BT_ADDRESS_TYPE_RANDOM_DEVICE_ADDRESS          =  0x01, /*!< 蓝牙随机地址。 */
} bt_addr_type;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| BT_ADDRESS_TYPE_PUBLIC_DEVICE_ADDRESS | 0x00 | 蓝牙公有地址 |
| BT_ADDRESS_TYPE_RANDOM_DEVICE_ADDRESS | 0x01 | 蓝牙随机地址 |

### errcode_bt_t <a id="enum_errcode_bt_t"></a>

```c
#define ERRCODE_BT_COMMON_BASE 0x80006000
#define ERRCODE_BT_COMMON_END  0x800067FF
typedef enum {
    ERRCODE_BT_SUCCESS = 0x00,                /*!< 执行成功错误码 */
    ERRCODE_BT_FAIL = ERRCODE_BT_COMMON_BASE, /*!< 执行失败错误码 */
    ERRCODE_BT_NOT_READY,                     /*!< 执行状态未就绪错误码 */
    ERRCODE_BT_MALLOC_FAIL,                   /*!< 内存不足错误码 */
    ERRCODE_BT_MEMCPY_FAIL,                   /*!< 内存拷贝错误码 */
    ERRCODE_BT_BUSY,                          /*!< 繁忙无法响应错误码 */
    ERRCODE_BT_DONE,                          /*!< 执行完成错误码 */
    ERRCODE_BT_UNSUPPORTED,                   /*!< 不支持错误码 */
    ERRCODE_BT_PARAM_ERR,                     /*!< 无效参数错误码 */
    ERRCODE_BT_STATE_ERR,                     /*!< 状态错误 */
    ERRCODE_BT_UNHANDLED,                     /*!< 未处理错误码 */
    ERRCODE_BT_AUTH_FAIL,                     /*!< 鉴权失败错误码 */
    ERRCODE_BT_RMT_DEV_DOWN,                  /*!< 远端设备关闭错误码 */
    ERRCODE_BT_AUTH_REJECTED,                 /*!< 鉴权被拒错误码 */
    ERRCODE_BT_MAX = ERRCODE_BT_COMMON_END    /*!< 蓝牙错误码最大值 */
} errcode_bt_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| ERRCODE_BT_SUCCESS | 0x00 | 执行成功 |
| ERRCODE_BT_FAIL | 0x80006000 | 执行失败 |
| ERRCODE_BT_NOT_READY | 0x80006001 | 状态未就绪 |
| ERRCODE_BT_MALLOC_FAIL | 0x80006002 | 内存不足 |
| ERRCODE_BT_MEMCPY_FAIL | 0x80006003 | 内存拷贝失败 |
| ERRCODE_BT_BUSY | 0x80006004 | 繁忙无法响应 |
| ERRCODE_BT_DONE | 0x80006005 | 执行完成 |
| ERRCODE_BT_UNSUPPORTED | 0x80006006 | 不支持 |
| ERRCODE_BT_PARAM_ERR | 0x80006007 | 无效参数 |
| ERRCODE_BT_STATE_ERR | 0x80006008 | 状态错误 |
| ERRCODE_BT_UNHANDLED | 0x80006009 | 未处理 |
| ERRCODE_BT_AUTH_FAIL | 0x8000600A | 鉴权失败 |
| ERRCODE_BT_RMT_DEV_DOWN | 0x8000600B | 远端设备关闭 |
| ERRCODE_BT_AUTH_REJECTED | 0x8000600C | 鉴权被拒 |
| ERRCODE_BT_MAX | 0x800067FF | 错误码最大值 |

## Structures

### a2dp_codec_info_t <a id="struct_a2dp_codec_info_t"></a>

```c
typedef struct {
    int codec_priority;            /*!< 编码格式优先级。 */
    unsigned int codec_type;       /*!< 编码格式类型。 */
    unsigned char sample_rate;     /*!< 编码器采样率。 */
    unsigned char bits_per_sample; /*!< 采样比特位。 */
    unsigned char channel_mode;    /*!< 声道模式。 */
    unsigned long codec_specific1; /*!< 自定义参数1。 */
    unsigned long codec_specific2; /*!< 自定义参数2。 */
    unsigned long codec_specific3; /*!< 自定义参数3。 */
    unsigned long codec_specific4; /*!< 自定义参数4。 */
} a2dp_codec_info_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| codec_priority | int | 编码格式优先级，取值参考A2DP_CODEC_PRIORITY_DISABLED(-1)、A2DP_CODEC_PRIORITY_DEFAULT(0)、A2DP_CODEC_PRIORITY_HIGHEST(1000000) |
| codec_type | unsigned int | 编码格式类型，取值参考[a2dp_codec_type_t](#enum_a2dp_codec_type_t) |
| sample_rate | unsigned char | 编码器采样率，取值参考A2DP_CODEC_SAMPLE_RATE_*系列宏 |
| bits_per_sample | unsigned char | 采样比特位，取值参考A2DP_CODEC_BITS_PER_SAMPLE_*系列宏 |
| channel_mode | unsigned char | 声道模式，取值参考A2DP_CODEC_CHANNEL_MODE_*系列宏 |
| codec_specific1 | unsigned long | 自定义参数1 |
| codec_specific2 | unsigned long | 自定义参数2 |
| codec_specific3 | unsigned long | 自定义参数3 |
| codec_specific4 | unsigned long | 自定义参数4 |

### a2dp_src_callbacks_t <a id="struct_a2dp_src_callbacks_t"></a>

```c
typedef struct {
    a2dp_src_connect_state_changed_callback connectstate_changed_cb;
    a2dp_src_playing_state_changed_callback playing_state_changed_cb;
    a2dp_src_configuration_changed_callback configuration_changed_cb;
} a2dp_src_callbacks_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| connectstate_changed_cb | a2dp_src_connect_state_changed_callback | A2DP SRC连接状态变更回调函数指针 |
| playing_state_changed_cb | a2dp_src_playing_state_changed_callback | A2DP SRC播放状态变更回调函数指针 |
| configuration_changed_cb | a2dp_src_configuration_changed_callback | A2DP SRC编码格式变更回调函数指针 |

### bd_addr_t <a id="struct_bd_addr_t"></a>

```c
#define BD_ADDR_LEN 6
typedef struct {
    uint8_t addr[BD_ADDR_LEN];     /*!< 蓝牙地址。 */
    uint8_t type;                  /*!< 蓝牙地址类型，参考 bt_addr_type。 */
} bd_addr_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| addr | uint8_t[6] | 蓝牙设备地址，长度为6字节 |
| type | uint8_t | 蓝牙地址类型，取值参考[bt_addr_type](#enum_bt_addr_type) |



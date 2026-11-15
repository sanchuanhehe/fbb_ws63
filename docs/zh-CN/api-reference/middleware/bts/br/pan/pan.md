# PAN

PAN（Personal Area Network，个人区域网络）提供基于蓝牙的网络接入服务。本模块支持打开或关闭 PAN 服务与对端蓝牙设备建立连接、发送网络数据包以及查询网络连接状态。

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_pan.h"
```
- include/middleware/services/bts/common/bts_def.h

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [pan_service_open](#pan_service_open) | 打开PAN服务，与对端蓝牙设备建立PAN连接 |
| [pan_service_close](#pan_service_close) | 关闭PAN服务，断开与对端蓝牙设备的PAN连接 |
| [pan_service_write_data](#pan_service_write_data) | 发送网络数据包 |
| [pan_service_net_state_get](#pan_service_net_state_get) | 获取PAN网络连接状态 |
| [pan_register_callbacks](#pan_register_callbacks) | 注册上层应用回调函数 |
| [pan_deregister_callbacks](#pan_deregister_callbacks) | 去注册上层应用回调函数 |

## Functions

### pan_service_open <a id="pan_service_open"></a>

```c
int pan_service_open(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_pan.h"
```

**功能说明**

- 打开PAN服务，与指定对端蓝牙设备建立PAN网络连接
- 调用后发起PAN连接请求，连接结果通过回调函数通知上层应用
- 需在对端设备支持PAN Profile的前提下调用

**前置条件**

- 蓝牙协议栈已通过 enable_bt_stack() 初始化完成
- 入参 bd_addr 不为 NULL，且指向有效的蓝牙设备地址

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备地址指针 | 非NULL，指向有效蓝牙地址 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | PAN连接请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### pan_service_close <a id="pan_service_close"></a>

```c
int pan_service_close(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_pan.h"
```

**功能说明**

- 关闭PAN服务，断开与指定对端蓝牙设备的PAN网络连接
- 调用后发起PAN断连请求，断连结果通过回调函数通知上层应用
- 需在PAN连接已建立的状态下调用

**前置条件**

- 蓝牙协议栈已通过 enable_bt_stack() 初始化完成
- 入参 bd_addr 不为 NULL，且指向已建立PAN连接的对端蓝牙设备地址

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备地址指针 | 非NULL，指向已建立PAN连接的蓝牙地址 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | PAN断连请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### pan_service_write_data <a id="pan_service_write_data"></a>

```c
int pan_service_write_data(const unsigned char *ip_packet, const unsigned short ip_length)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_pan.h"
```

**功能说明**

- 通过PAN连接发送网络数据包
- 调用前需确保PAN连接已建立且网络状态为在线
- 数据包内容为以太网帧格式的IP (Internet Protocol) 数据包

**前置条件**

- 蓝牙协议栈已通过 enable_bt_stack() 初始化完成
- PAN服务已通过 pan_service_open() 打开且网络状态为 PAN_NET_STATE_ON
- 入参 ip_packet 不为 NULL，且指向的内存空间长度不小于 ip_length

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| ip_packet | const unsigned char * | 网络数据包指针 | 非NULL，指向有效以太网帧数据 |
| ip_length | const unsigned short | 网络数据包长度 | 大于0，不超过PAN最大传输单元 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 数据包发送成功 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### pan_service_net_state_get <a id="pan_service_net_state_get"></a>

```c
unsigned char pan_service_net_state_get(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_pan.h"
```

**功能说明**

- 获取PAN网络连接状态
- 返回值表示当前PAN网络是否在线
- 可用于在发送数据前判断网络是否可用

**前置条件**

- 蓝牙协议栈已通过 enable_bt_stack() 初始化完成

**返回值**

- 返回类型：unsigned char

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| PAN_NET_STATE_OFF(0) | 离线 | PAN网络未连接 |
| PAN_NET_STATE_ON(1) | 在线 | PAN网络已连接 |

### pan_register_callbacks <a id="pan_register_callbacks"></a>

```c
int pan_register_callbacks(pan_callbacks_t *func)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_pan.h"
```

**功能说明**

- 注册上层应用回调函数，用于接收PAN网络状态变更通知和接收网络数据包
- 注册后，当PAN网络状态变化时通过 net_state_cb 回调通知上层应用
- 注册后，当接收到网络数据包时通过 rpt_data_cb 回调通知上层应用

**前置条件**

- 蓝牙协议栈已通过 enable_bt_stack() 初始化完成
- 入参 func 不为 NULL，且指向的回调结构体中各回调函数指针已赋值

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func | [pan_callbacks_t](#struct_pan_callbacks_t) * | 回调函数结构体指针 | 非NULL，包含有效的回调函数指针 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 回调注册成功 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### pan_deregister_callbacks <a id="pan_deregister_callbacks"></a>

```c
int pan_deregister_callbacks(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_pan.h"
```

**功能说明**

- 去注册上层应用回调函数，取消PAN网络状态变更通知和数据接收通知
- 调用后不再接收PAN网络状态变更和网络数据包的回调通知
- 需在已通过 pan_register_callbacks() 注册回调的前提下调用

**前置条件**

- 蓝牙协议栈已通过 enable_bt_stack() 初始化完成
- 已通过 pan_register_callbacks() 注册回调函数

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 回调去注册成功 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

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

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| ERRCODE_BT_SUCCESS | 0x00 | 执行成功 |
| ERRCODE_BT_FAIL | 0x80006000 | 执行失败 |
| ERRCODE_BT_NOT_READY | 0x80006001 | 状态未就绪 |
| ERRCODE_BT_MALLOC_FAIL | 0x80006002 | 内存不足 |
| ERRCODE_BT_MEMCPY_FAIL | 0x80006003 | 内存拷贝失败 |
| ERRCODE_BT_BUSY | 0x80006004 | 系统繁忙 |
| ERRCODE_BT_DONE | 0x80006005 | 执行完成 |
| ERRCODE_BT_UNSUPPORTED | 0x80006006 | 不支持 |
| ERRCODE_BT_PARAM_ERR | 0x80006007 | 无效参数 |
| ERRCODE_BT_STATE_ERR | 0x80006008 | 状态错误 |
| ERRCODE_BT_UNHANDLED | 0x80006009 | 未处理 |
| ERRCODE_BT_AUTH_FAIL | 0x8000600A | 鉴权失败 |
| ERRCODE_BT_RMT_DEV_DOWN | 0x8000600B | 远端设备关闭 |
| ERRCODE_BT_AUTH_REJECTED | 0x8000600C | 鉴权被拒 |
| ERRCODE_BT_MAX | 0x800067FF | 蓝牙错误码最大值 |

### pan_net_state_t <a id="enum_pan_net_state_t"></a>

```c
typedef enum {
    PAN_NET_STATE_OFF,
    PAN_NET_STATE_ON,
} pan_net_state_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| PAN_NET_STATE_OFF | 0 | 离线 |
| PAN_NET_STATE_ON | 1 | 在线 |

### bt_addr_type <a id="enum_bt_addr_type"></a>

```c
typedef enum {
    BT_ADDRESS_TYPE_PUBLIC_DEVICE_ADDRESS = 0x00,
    BT_ADDRESS_TYPE_RANDOM_DEVICE_ADDRESS = 0x01,
} bt_addr_type;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| BT_ADDRESS_TYPE_PUBLIC_DEVICE_ADDRESS | 0x00 | 蓝牙公有地址 |
| BT_ADDRESS_TYPE_RANDOM_DEVICE_ADDRESS | 0x01 | 蓝牙随机地址 |

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
| addr | uint8_t[6] | 蓝牙设备地址，长度为6字节 |
| type | uint8_t | 蓝牙地址类型，参考 [bt_addr_type](#enum_bt_addr_type) |

### pan_callbacks_t <a id="struct_pan_callbacks_t"></a>

```c
typedef struct {
    pan_net_state_callback net_state_cb;
    pan_rpt_data_callback rpt_data_cb;
} pan_callbacks_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| net_state_cb | pan_net_state_callback | PAN网络状态变更回调函数指针 |
| rpt_data_cb | pan_rpt_data_callback | 网络数据包接收通知回调函数指针 |

## Type definitions

### pan_net_state_callback <a id="typedef_pan_net_state_callback"></a>

```c
typedef void (*pan_net_state_callback)(unsigned char net_state);
```

**使用说明**

PAN网络状态变更回调函数类型，在 [pan_callbacks_t](#struct_pan_callbacks_t) 结构体的 net_state_cb 成员中使用

### pan_rpt_data_callback <a id="typedef_pan_rpt_data_callback"></a>

```c
typedef void (*pan_rpt_data_callback)(unsigned char *packet, unsigned short packet_length);
```

**使用说明**

网络数据包接收通知回调函数类型，在 [pan_callbacks_t](#struct_pan_callbacks_t) 结构体的 rpt_data_cb 成员中使用



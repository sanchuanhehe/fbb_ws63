# CHBA

CHBA (Converged Host Bus Adapter) 提供基于 SLE (SparkLink Low Energy) 的 CHBA 网络设备管理能力，支持按角色（AP/STA）与模式创建、销毁网络设备，添加与删除链路，查询链路统计信息，并允许注册发送队列控制、链路启用/禁用及数据上报等回调函数。本模块涵盖网络设备生命周期管理、链路维护与回调注册等接口。

**头文件清单**

```c
#include "include/middleware/services/bts/sle_chba/sle_chba_manager.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [sle_chba_netdev_create](#sle_chba_netdev_create) | 创建CHBA网络设备，指定角色与模式 |
| [sle_chba_netdev_destroy](#sle_chba_netdev_destroy) | 销毁CHBA网络设备 |
| [sle_chba_netdev_add_link](#sle_chba_netdev_add_link) | 向CHBA网络设备添加链路 |
| [sle_chba_netdev_del_link](#sle_chba_netdev_del_link) | 从CHBA网络设备删除链路 |
| [sle_chba_netdev_get_linkinfo](#sle_chba_netdev_get_linkinfo) | 获取指定链路的统计信息 |
| [sle_chba_netdev_driver_send](#sle_chba_netdev_driver_send) | 通过CHBA网络设备发送数据 |
| [sle_chba_netdev_register_callbacks](#sle_chba_netdev_register_callbacks) | 注册CHBA网络设备回调函数 |

## Functions

### sle_chba_netdev_create <a id="sle_chba_netdev_create"></a>

```c
errcode_t sle_chba_netdev_create(uint8_t chba_role, uint8_t chba_mode)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle_chba/sle_chba_manager.h"
```

**功能说明**

- 创建CHBA网络设备，根据指定的角色和模式初始化CHBA网络设备实例
- 角色参数决定设备作为AP (Access Point)（无线网络中心，管理STA (Station)，提供数据中继，SLE作为client端）或STA（叶子节点，终端设备连接到无线网络进行数据传输，SLE作为server端）
- 创建后需调用[sle_chba_netdev_register_callbacks](#sle_chba_netdev_register_callbacks)注册回调函数以处理链路状态变更与数据上报

**前置条件**

- SLE协议栈已初始化完成，通过enable_sle()返回成功状态
- CHBA网络设备尚未创建，即未调用过sle_chba_netdev_create或已通过sle_chba_netdev_destroy销毁

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| chba_role | uint8_t | CHBA设备角色 | [sle_chba_role](#enum_sle_chba_role)枚举值：CHBA_ROLE_AP(0)、CHBA_ROLE_STA(1) |
| chba_mode | uint8_t | CHBA工作模式 | 0：标准CHBA模式；其他值视平台支持 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | CHBA网络设备创建成功 |
| Other | 其他错误码，参考[errcode_sle_t](#enum_errcode_sle_t) | 执行失败 |

**参考案例**

- [sle_chba_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle_chba/src/sle_chba_server.c)

### sle_chba_netdev_destroy <a id="sle_chba_netdev_destroy"></a>

```c
errcode_t sle_chba_netdev_destroy(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle_chba/sle_chba_manager.h"
```

**功能说明**

- 销毁已创建的CHBA网络设备，释放相关资源
- 销毁后CHBA网络设备不可用，需重新调用[sle_chba_netdev_create](#sle_chba_netdev_create)创建
- 销毁前需确保已断开所有链路连接

**前置条件**

- CHBA网络设备已通过[sle_chba_netdev_create](#sle_chba_netdev_create)成功创建

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | CHBA网络设备销毁成功 |
| Other | 其他错误码，参考[errcode_sle_t](#enum_errcode_sle_t) | 执行失败 |

### sle_chba_netdev_add_link <a id="sle_chba_netdev_add_link"></a>

```c
errcode_t sle_chba_netdev_add_link(uint16_t conn_id, const sle_addr_t *remote_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle_chba/sle_chba_manager.h"
```

**功能说明**

- 向CHBA网络设备添加指定连接ID与对端地址的链路
- 链路添加后，CHBA网络设备可对该链路进行数据收发与状态管理
- AP模式下可添加多条链路，STA模式下通常添加一条链路

**前置条件**

- CHBA网络设备已通过[sle_chba_netdev_create](#sle_chba_netdev_create)成功创建
- SLE连接已建立成功，conn_id与remote_addr有效

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| conn_id | uint16_t | SLE连接ID | 有效连接ID，非0xFFFF |
| remote_addr | const [sle_addr_t](#struct_sle_addr_t) * | 对端设备地址指针 | 不为NULL，指向有效[sle_addr_t](#struct_sle_addr_t)结构体 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 链路添加成功 |
| Other | 其他错误码，参考[errcode_sle_t](#enum_errcode_sle_t) | 执行失败 |

**参考案例**

- [sle_chba_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle_chba/src/sle_chba_server.c)

### sle_chba_netdev_del_link <a id="sle_chba_netdev_del_link"></a>

```c
errcode_t sle_chba_netdev_del_link(uint16_t conn_id, const sle_addr_t *remote_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle_chba/sle_chba_manager.h"
```

**功能说明**

- 从CHBA网络设备删除指定连接ID与对端地址的链路
- 链路删除后，CHBA网络设备不再对该链路进行数据收发与状态管理
- 通常在SLE连接断开时调用

**前置条件**

- CHBA网络设备已通过[sle_chba_netdev_create](#sle_chba_netdev_create)成功创建
- 待删除的链路已通过[sle_chba_netdev_add_link](#sle_chba_netdev_add_link)添加

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| conn_id | uint16_t | SLE连接ID | 有效连接ID，非0xFFFF |
| remote_addr | const [sle_addr_t](#struct_sle_addr_t) * | 对端设备地址指针 | 不为NULL，指向有效[sle_addr_t](#struct_sle_addr_t)结构体 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 链路删除成功 |
| Other | 其他错误码，参考[errcode_sle_t](#enum_errcode_sle_t) | 执行失败 |

**参考案例**

- [sle_chba_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle_chba/src/sle_chba_server.c)

### sle_chba_netdev_get_linkinfo <a id="sle_chba_netdev_get_linkinfo"></a>

```c
errcode_t sle_chba_netdev_get_linkinfo(uint16_t conn_id, sle_ip_link_info *link)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle_chba/sle_chba_manager.h"
```

**功能说明**

- 获取指定连接ID对应的链路统计信息，包括收发包计数与字节数
- 通过出参结构体返回链路的连接ID、对端地址、发送包数、发送字节数、接收包数、接收字节数
- 用于链路状态监控与流量统计

**前置条件**

- CHBA网络设备已通过[sle_chba_netdev_create](#sle_chba_netdev_create)成功创建
- 待查询的链路已通过[sle_chba_netdev_add_link](#sle_chba_netdev_add_link)添加

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| conn_id | uint16_t | SLE连接ID | 有效连接ID，非0xFFFF |
| link | [sle_ip_link_info](#struct_sle_ip_link_info) * | 链路信息输出缓冲区指针 | 不为NULL，指向已分配的[sle_ip_link_info](#struct_sle_ip_link_info)结构体 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| link | [sle_ip_link_info](#struct_sle_ip_link_info) * | 填充指定链路的统计信息，包括conn_id、remote_addr、tx_pkts_cnt、tx_bytes、rx_pkts_cnt、rx_bytes |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 链路信息获取成功 |
| Other | 其他错误码，参考[errcode_sle_t](#enum_errcode_sle_t) | 执行失败 |

### sle_chba_netdev_driver_send <a id="sle_chba_netdev_driver_send"></a>

```c
errcode_t sle_chba_netdev_driver_send(uint8_t *data, uint16_t len)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle_chba/sle_chba_manager.h"
```

**功能说明**

- 通过CHBA网络设备发送数据到对端设备
- 数据通过SLE链路发送，需确保链路已建立并处于可用状态
- 发送数据长度受SLE链路MTU (Maximum Transmission Unit) 约束

**前置条件**

- CHBA网络设备已通过[sle_chba_netdev_create](#sle_chba_netdev_create)成功创建
- 至少有一条链路已通过[sle_chba_netdev_add_link](#sle_chba_netdev_add_link)添加且链路状态正常

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| data | uint8_t * | 待发送数据缓冲区指针 | 不为NULL，指向待发送数据 |
| len | uint16_t | 待发送数据长度 | 大于0，不超过SLE链路MTU |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 数据发送成功 |
| Other | 其他错误码，参考[errcode_sle_t](#enum_errcode_sle_t) | 执行失败 |

**参考案例**

- [sle_chba_bridge.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle_chba/src/sle_chba_bridge.c)

### sle_chba_netdev_register_callbacks <a id="sle_chba_netdev_register_callbacks"></a>

```c
errcode_t sle_chba_netdev_register_callbacks(sle_chba_netdev_callbacks_t *func)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle_chba/sle_chba_manager.h"
```

**功能说明**

- 注册CHBA网络设备回调函数集合，包括停止发送队列、唤醒发送队列、设置链路启用、设置链路禁用、数据上报回调
- 注册后，CHBA网络设备在链路状态变更与数据收发时触发对应回调
- 重复注册将覆盖先前注册的回调函数

**前置条件**

- CHBA网络设备已通过[sle_chba_netdev_create](#sle_chba_netdev_create)成功创建

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func | [sle_chba_netdev_callbacks_t](#struct_sle_chba_netdev_callbacks_t) * | 回调函数集合指针 | 不为NULL，各回调函数成员可为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 回调函数注册成功 |
| Other | 其他错误码，参考[errcode_sle_t](#enum_errcode_sle_t) | 执行失败 |

**参考案例**

- [sle_chba_netif_mng.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle_chba/src/sle_chba_netif_mng.c)


## Type definitions

### sle_chba_netdev_stop_queue_callback <a id="typedef_sle_chba_netdev_stop_queue_callback"></a>

```c
typedef errcode_t (*sle_chba_netdev_stop_queue_callback)(void);
```

**使用说明**

作为[sle_chba_netdev_callbacks_t](#struct_sle_chba_netdev_callbacks_t)结构体中stop_queue_cb成员的类型，用于CHBA网络设备停止发送队列时的回调通知。

### sle_chba_netdev_wake_queue_callback <a id="typedef_sle_chba_netdev_wake_queue_callback"></a>

```c
typedef errcode_t (*sle_chba_netdev_wake_queue_callback)(void);
```

**使用说明**

作为[sle_chba_netdev_callbacks_t](#struct_sle_chba_netdev_callbacks_t)结构体中wake_queue_cb成员的类型，用于CHBA网络设备唤醒发送队列时的回调通知。

### sle_chba_netdev_set_link_up_callback <a id="typedef_sle_chba_netdev_set_link_up_callback"></a>

```c
typedef errcode_t (*sle_chba_netdev_set_link_up_callback)(void);
```

**使用说明**

作为[sle_chba_netdev_callbacks_t](#struct_sle_chba_netdev_callbacks_t)结构体中set_link_up_cb成员的类型，用于CHBA网络设备设置链路启用时的回调通知。

### sle_chba_netdev_set_link_down_callback <a id="typedef_sle_chba_netdev_set_link_down_callback"></a>

```c
typedef errcode_t (*sle_chba_netdev_set_link_down_callback)(void);
```

**使用说明**

作为[sle_chba_netdev_callbacks_t](#struct_sle_chba_netdev_callbacks_t)结构体中set_link_down_cb成员的类型，用于CHBA网络设备设置链路禁用时的回调通知。

### sle_chba_netdev_input_callback <a id="typedef_sle_chba_netdev_input_callback"></a>

```c
typedef errcode_t (*sle_chba_netdev_input_callback)(uint8_t *data, uint16_t len);
```

**使用说明**

作为[sle_chba_netdev_callbacks_t](#struct_sle_chba_netdev_callbacks_t)结构体中netdev_input_cb成员的类型，用于CHBA网络设备接收数据上报时的回调通知。

## Enumerations

### sle_chba_role <a id="enum_sle_chba_role"></a>

```c
enum sle_chba_role {
    CHBA_ROLE_AP, // AP模式下，设备作为无线网络中心，管理STA，提供数据中继，支持多设备连接。sle作为client端
    CHBA_ROLE_STA, // STA模式下，为叶子节点。终端设备连接到无线网络，进行数据传输。sle作为server端
};
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| CHBA_ROLE_AP | 0 | AP模式，设备作为无线网络中心，管理STA  ，提供数据中继，支持多设备连接，SLE作为client端 |
| CHBA_ROLE_STA | 1 | STA模式，为叶子节点，终端设备连接到无线网络进行数据传输，SLE作为server端 |

### errcode_sle_t <a id="enum_errcode_sle_t"></a>

```c
typedef enum {
    ERRCODE_SLE_SUCCESS = 0,
    ERRCODE_SLE_CONTINUE = ERRCODE_SLE_COMMON_BASE,
    ERRCODE_SLE_DIRECT_RETURN,
    ERRCODE_SLE_NO_ATTATION,
    ERRCODE_SLE_PARAM_ERR,
    ERRCODE_SLE_FAIL,
    ERRCODE_SLE_TIMEOUT,
    ERRCODE_SLE_UNSUPPORTED,
    ERRCODE_SLE_GETRECORD_FAIL,
    ERRCODE_SLE_POINTER_NULL,
    ERRCODE_SLE_NO_RECORD,
    ERRCODE_SLE_STATUS_ERR,
    ERRCODE_SLE_NOMEM,
    ERRCODE_SLE_AUTH_FAIL,
    ERRCODE_SLE_AUTH_PKEY_MISS,
    ERRCODE_SLE_RMT_DEV_DOWN,
    ERRCODE_SLE_PAIRING_REJECT,
    ERRCODE_SLE_BUSY,
    ERRCODE_SLE_NOT_READY,
    ERRCODE_SLE_CONN_FAIL,
    ERRCODE_SLE_OUT_OF_RANGE,
    ERRCODE_SLE_MEMCPY_FAIL,
    ERRCODE_SLE_MALLOC_FAIL,
    ERRCODE_SLE_MAX = ERRCODE_SLE_COMMON_END
} errcode_sle_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| ERRCODE_SLE_SUCCESS | 0 | 执行成功 |
| ERRCODE_SLE_CONTINUE | 0x80006000 | 继续执行 |
| ERRCODE_SLE_DIRECT_RETURN | 0x80006001 | 直接返回 |
| ERRCODE_SLE_NO_ATTATION | 0x80006002 | 无关注错误码 |
| ERRCODE_SLE_PARAM_ERR | 0x80006003 | 参数错误 |
| ERRCODE_SLE_FAIL | 0x80006004 | 配置失败 |
| ERRCODE_SLE_TIMEOUT | 0x80006005 | 配置超时 |
| ERRCODE_SLE_UNSUPPORTED | 0x80006006 | 参数不支持 |
| ERRCODE_SLE_GETRECORD_FAIL | 0x80006007 | 获取当前记录失败 |
| ERRCODE_SLE_POINTER_NULL | 0x80006008 | 指针为空 |
| ERRCODE_SLE_NO_RECORD | 0x80006009 | 无记录返回 |
| ERRCODE_SLE_STATUS_ERR | 0x8000600A | 状态错误 |
| ERRCODE_SLE_NOMEM | 0x8000600B | 内存不足 |
| ERRCODE_SLE_AUTH_FAIL | 0x8000600C | 认证失败 |
| ERRCODE_SLE_AUTH_PKEY_MISS | 0x8000600D | PIN码或密钥丢失致认证失败 |
| ERRCODE_SLE_RMT_DEV_DOWN | 0x8000600E | 对端设备关闭 |
| ERRCODE_SLE_PAIRING_REJECT | 0x8000600F | 配对拒绝 |
| ERRCODE_SLE_BUSY | 0x80006010 | 系统繁忙 |
| ERRCODE_SLE_NOT_READY | 0x80006011 | 系统未准备好 |
| ERRCODE_SLE_CONN_FAIL | 0x80006012 | 连接失败 |
| ERRCODE_SLE_OUT_OF_RANGE | 0x80006013 | 越界 |
| ERRCODE_SLE_MEMCPY_FAIL | 0x80006014 | 拷贝失败 |
| ERRCODE_SLE_MALLOC_FAIL | 0x80006015 | 内存申请失败 |
| ERRCODE_SLE_MAX | 0x800067FF | SLE错误码最大值 |

### sle_addr_type_t <a id="enum_sle_addr_type_t"></a>

```c
typedef enum {
    SLE_ADDRESS_TYPE_PUBLIC = 0,
    SLE_ADDRESS_TYPE_RANDOM = 6,
} sle_addr_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SLE_ADDRESS_TYPE_PUBLIC | 0 | 公有地址 |
| SLE_ADDRESS_TYPE_RANDOM | 6 | 随机地址 |

## Structures

### sle_ip_link_info <a id="struct_sle_ip_link_info"></a>

```c
typedef struct {
    uint8_t conn_id;
    sle_addr_t remote_addr;
    uint32_t tx_pkts_cnt;
    uint32_t tx_bytes;
    uint32_t rx_pkts_cnt;
    uint32_t rx_bytes;
} sle_ip_link_info;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| conn_id | uint8_t | 连接ID |
| remote_addr | [sle_addr_t](#struct_sle_addr_t) | 对端设备地址 |
| tx_pkts_cnt | uint32_t | 发送包计数 |
| tx_bytes | uint32_t | 发送字节数 |
| rx_pkts_cnt | uint32_t | 接收包计数 |
| rx_bytes | uint32_t | 接收字节数 |

### sle_chba_netdev_callbacks_t <a id="struct_sle_chba_netdev_callbacks_t"></a>

```c
typedef struct {
    sle_chba_netdev_stop_queue_callback stop_queue_cb;              /*!< @if Eng Chba net device stop queue callback.
                                                                    @else   CHBA网络设备停止发送队列回调函数。 @endif */
    sle_chba_netdev_wake_queue_callback wake_queue_cb;              /*!< @if Eng Chba net device wake queue callback.
                                                                    @else   CHBA网络设备唤醒发送队列回调函数。 @endif */
    sle_chba_netdev_set_link_up_callback set_link_up_cb;            /*!< @if Eng Chba net device set link up callback.
                                                                    @else   CHBA网络设备设置链路启用回调函数。 @endif */
    sle_chba_netdev_set_link_down_callback set_link_down_cb;        /*!< @if Eng Chba net device set link down callback.
                                                                    @else   CHBA网络设备设置链路禁用回调函数。 @endif */
    sle_chba_netdev_input_callback netdev_input_cb;                 /*!< @if Eng Chba net device input callback.
                                                                    @else   CHBA网络设备数据上报回调函数。 @endif */
} sle_chba_netdev_callbacks_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| stop_queue_cb | [sle_chba_netdev_stop_queue_callback](#typedef_sle_chba_netdev_stop_queue_callback) | CHBA网络设备停止发送队列回调函数 |
| wake_queue_cb | [sle_chba_netdev_wake_queue_callback](#typedef_sle_chba_netdev_wake_queue_callback) | CHBA网络设备唤醒发送队列回调函数 |
| set_link_up_cb | [sle_chba_netdev_set_link_up_callback](#typedef_sle_chba_netdev_set_link_up_callback) | CHBA网络设备设置链路启用回调函数 |
| set_link_down_cb | [sle_chba_netdev_set_link_down_callback](#typedef_sle_chba_netdev_set_link_down_callback) | CHBA网络设备设置链路禁用回调函数 |
| netdev_input_cb | [sle_chba_netdev_input_callback](#typedef_sle_chba_netdev_input_callback) | CHBA网络设备数据上报回调函数 |

### sle_addr_t <a id="struct_sle_addr_t"></a>

```c
typedef struct {
    uint8_t type;                         /*!< @if Eng SLE device address type { @ref sle_addr_type_t }
                                                @else   SLE设备地址类型 { @ref sle_addr_type_t } @endif */
    unsigned char addr[SLE_ADDR_LEN];     /*!< @if Eng SLE device address
                                                @else   SLE设备地址 @endif */
} sle_addr_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| type | uint8_t | SLE设备地址类型，取值参考[sle_addr_type_t](#enum_sle_addr_type_t) |
| addr | unsigned char[6] | SLE设备地址，长度为SLE_ADDR_LEN(6) |





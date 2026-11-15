# SPP

SPP (Serial Port Profile) 基于经典蓝牙 RFCOMM (Radio Frequency Communication) 协议实现串口透传。管理 SPP 连接/断开、写入数据和回调注册。

上位机通过蓝牙虚拟串口与 WS63 数据交互，透明传输任意二进制数据。适用蓝牙串口调试、无线数据采集等需要串口替代的场景。

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_spp.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [spp_server_create](#spp_server_create) | 创建SPP Server监听socket |
| [spp_server_accept](#spp_server_accept) | 等待远端设备连接此Server |
| [spp_server_close](#spp_server_close) | 关闭Server并释放相关资源 |
| [spp_connect](#spp_connect) | 连接远端设备 |
| [spp_disconnect](#spp_disconnect) | 断开连接并释放相关资源 |
| [is_spp_connected](#is_spp_connected) | 获取指定SPP socket的连接状态 |
| [spp_get_remote_addr](#spp_get_remote_addr) | 获取SPP socket对应远端设备的地址 |
| [spp_write](#spp_write) | 向SPP socket写入数据 |
| [spp_register_callbacks](#spp_register_callbacks) | 注册SPP回调函数 |

## Functions

### spp_server_create <a id="spp_server_create"></a>

```c
int spp_server_create(spp_create_socket_para_t *socket_para, const char *name, unsigned int len)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_spp.h"
```

**功能说明**

- 基于服务记录创建SPP Server监听socket
- 创建成功后返回有效的Server ID，用于后续accept/close操作
- 返回的Server ID用于标识当前Server socket

**前置条件**

- 模块初始化状态：蓝牙协议栈已通过enable_bt_stack()初始化完成
- 参数合法性要求：socket_para不为NULL且指向有效内存，name不为NULL且指向以'\0'结尾的字符串，len取值为strlen(name) + 1

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| socket_para | [spp_create_socket_para_t](#struct_spp_create_socket_para_t) * | SPP Server创建参数 | 不为NULL |
| name | const char * | SPP Server名称 | 不为NULL，以'\0'结尾的字符串 |
| len | unsigned int | SPP Server名称长度 | 取值为strlen(name) + 1 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| SPP_INVALID_ID(-1) | 创建Server失败 | 创建失败 |
| 其他正整数 | 创建Server成功，返回Server ID | 创建成功 |

### spp_server_accept <a id="spp_server_accept"></a>

```c
int spp_server_accept(int server_id)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_spp.h"
```

**功能说明**

- 等待远端设备来连接此Server，返回一个Client ID用于与远端设备进行数据收发
- 这是一个同步接口，调用后将阻塞等待远端连接
- 返回的Client ID可用于spp_disconnect、is_spp_connected、spp_get_remote_addr、spp_write操作

**前置条件**

- 模块初始化状态：已通过spp_server_create()成功创建Server并获得有效Server ID
- 参数合法性要求：server_id为spp_server_create()返回的有效Server ID

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | int | 标识当前Server socket的ID | spp_server_create()返回的有效Server ID |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| SPP_INVALID_ID(-1) | 等待失败 | Accept失败 |
| 其他正整数 | 等待成功，返回有效的Client ID | Accept成功 |

### spp_server_close <a id="spp_server_close"></a>

```c
int spp_server_close(int server_id)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_spp.h"
```

**功能说明**

- 关闭Server socket并释放相关资源
- 这是一个同步接口
- 关闭后对应的Server ID不再有效

**前置条件**

- 模块初始化状态：已通过spp_server_create()成功创建Server并获得有效Server ID
- 参数合法性要求：server_id为spp_server_create()返回的有效Server ID

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | int | 标识当前Server socket的ID | spp_server_create()返回的有效Server ID |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 关闭成功 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### spp_connect <a id="spp_connect"></a>

```c
int spp_connect(spp_create_socket_para_t *socket_para, const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_spp.h"
```

**功能说明**

- 创建Client socket并连接远端设备
- 这是一个同步接口
- 连接成功后返回有效的Client ID，用于后续数据收发操作

**前置条件**

- 模块初始化状态：蓝牙协议栈已通过enable_bt_stack()初始化完成
- 参数合法性要求：socket_para不为NULL且指向有效内存，bd_addr不为NULL且指向有效蓝牙地址

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| socket_para | [spp_create_socket_para_t](#struct_spp_create_socket_para_t) * | 创建Client socket并连接远端设备的参数 | 不为NULL |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 远端设备的蓝牙地址 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| SPP_INVALID_ID(-1) | 连接失败 | 连接失败 |
| 其他正整数 | 连接成功，返回有效Client ID | 连接成功 |

### spp_disconnect <a id="spp_disconnect"></a>

```c
int spp_disconnect(int client_id)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_spp.h"
```

**功能说明**

- 断开连接并释放相关资源
- 这是一个同步接口
- 断开后对应的Client ID不再有效

**前置条件**

- 模块初始化状态：已通过spp_server_accept()或spp_connect()获得有效Client ID
- 参数合法性要求：client_id为有效的Client ID

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| client_id | int | 标识当前Client socket的ID | spp_server_accept()或spp_connect()返回的有效Client ID |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 断开成功 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### is_spp_connected <a id="is_spp_connected"></a>

```c
bool is_spp_connected(int client_id)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_spp.h"
```

**功能说明**

- 获取指定SPP socket的连接状态
- 返回true表示已连接，false表示未连接
- 可在spp_server_accept()或spp_connect()之后调用

**前置条件**

- 模块初始化状态：已通过spp_server_accept()或spp_connect()获得有效Client ID
- 参数合法性要求：client_id为有效的Client ID

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| client_id | int | 标识当前Client socket的ID | spp_server_accept()或spp_connect()返回的有效Client ID |

**返回值**

- 返回类型：bool

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| true | 已连接 | SPP socket处于连接状态 |
| false | 未连接 | SPP socket处于未连接状态 |

### spp_get_remote_addr <a id="spp_get_remote_addr"></a>

```c
int spp_get_remote_addr(int client_id, bd_addr_t *remote_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_spp.h"
```

**功能说明**

- 获取SPP socket对应远端设备的蓝牙地址
- 远端地址由调用者分配内存存储
- 可在spp_server_accept()或spp_connect()之后调用

**前置条件**

- 模块初始化状态：已通过spp_server_accept()或spp_connect()获得有效Client ID
- 参数合法性要求：client_id为有效的Client ID，remote_addr不为NULL且指向已分配的内存空间

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| client_id | int | 标识当前Client socket的ID | spp_server_accept()或spp_connect()返回的有效Client ID |
| remote_addr | [bd_addr_t](#struct_bd_addr_t) * | 远端设备的地址（由调用者分配内存） | 不为NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| remote_addr | [bd_addr_t](#struct_bd_addr_t) | 远端设备的蓝牙地址 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 获取地址成功 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### spp_write <a id="spp_write"></a>

```c
int spp_write(int client_id, const char *data, const unsigned int len)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_spp.h"
```

**功能说明**

- 向SPP socket写入数据
- 操作成功时返回实际写入的字节数
- 操作失败时返回SPP_WRITE_FAILED

**前置条件**

- 模块初始化状态：已通过spp_server_accept()或spp_connect()获得有效Client ID，且SPP socket处于已连接状态
- 参数合法性要求：client_id为有效的Client ID，data不为NULL且指向有效数据缓冲区，len不超过数据缓冲区实际长度

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| client_id | int | 标识当前Client socket的ID | spp_server_accept()或spp_connect()返回的有效Client ID |
| data | const char * | 存储待写入数据的缓冲区 | 不为NULL |
| len | const unsigned int | 待写入数据的长度 | 不超过data缓冲区实际长度 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| SPP_WRITE_FAILED(-1) | 写入失败 | 操作失败 |
| 其他正整数 | 实际写入长度（单位字节） | 写入成功 |

### spp_register_callbacks <a id="spp_register_callbacks"></a>

```c
int spp_register_callbacks(spp_callbacks_t *func)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_spp.h"
```

**功能说明**

- 注册SPP回调函数，用于接收连接状态变化和接收数据事件
- 回调函数运行于bts线程，不能阻塞或长时间等待
- devices内存由bts申请和释放，回调中不应释放

**前置条件**

- 模块初始化状态：蓝牙协议栈已通过enable_bt_stack()初始化完成
- 参数合法性要求：func不为NULL且指向有效的回调函数结构体

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func | [spp_callbacks_t](#struct_spp_callbacks_t) * | 指向回调函数接口定义的指针 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 注册成功 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

## Enumerations

### spp_socket_type_t <a id="enum_spp_socket_type_t"></a>

```c
typedef enum {
    SPP_SOCKET_RFCOMM = 0x0,    /*!< RFCOMM */
} spp_socket_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SPP_SOCKET_RFCOMM | 0x0 | RFCOMM socket类型 |

### errcode_bt_t <a id="enum_errcode_bt_t"></a>

```c
typedef enum {
    ERRCODE_BT_SUCCESS = 0x00,                /*!< 执行成功错误码 */
    ERRCODE_BT_FAIL = ERRCODE_BT_COMMON_BASE, /*!< 执行失败错误码 */
    ERRCODE_BT_NOT_READY,                     /*!< 执行状态未就绪错误码 */
    ERRCODE_BT_MALLOC_FAIL,                   /*!< 内存不足错误码 */
    ERRCODE_BT_MEMCPY_FAIL,                   /*!< 内存拷贝错误错误码 */
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
| ERRCODE_BT_NOT_READY | 0x80006001 | 执行状态未就绪 |
| ERRCODE_BT_MALLOC_FAIL | 0x80006002 | 内存不足 |
| ERRCODE_BT_MEMCPY_FAIL | 0x80006003 | 内存拷贝错误 |
| ERRCODE_BT_BUSY | 0x80006004 | 繁忙无法响应 |
| ERRCODE_BT_DONE | 0x80006005 | 执行完成 |
| ERRCODE_BT_UNSUPPORTED | 0x80006006 | 不支持 |
| ERRCODE_BT_PARAM_ERR | 0x80006007 | 无效参数 |
| ERRCODE_BT_STATE_ERR | 0x80006008 | 状态错误 |
| ERRCODE_BT_UNHANDLED | 0x80006009 | 未处理 |
| ERRCODE_BT_AUTH_FAIL | 0x8000600A | 鉴权失败 |
| ERRCODE_BT_RMT_DEV_DOWN | 0x8000600B | 远端设备关闭 |
| ERRCODE_BT_AUTH_REJECTED | 0x8000600C | 鉴权被拒 |
| ERRCODE_BT_MAX | 0x800067FF | 蓝牙错误码最大值 |

### uuid_type_t <a id="enum_uuid_type_t"></a>

```c
typedef enum {
    UUID_TYPE_NULL = 0x00, /*!< 空uuid */
    UUID_TYPE_16_BIT,      /*!< 16bit长度的uuid */
    UUID_TYPE_32_BIT,      /*!< 32bit长度的uuid */
    UUID_TYPE_128_BIT,     /*!< 128bit长度的uuid */
} uuid_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| UUID_TYPE_NULL | 0x00 | 空UUID (Universally Unique Identifier) |
| UUID_TYPE_16_BIT | 0x01 | 16位UUID |
| UUID_TYPE_32_BIT | 0x02 | 32位UUID |
| UUID_TYPE_128_BIT | 0x03 | 128位UUID |

### bt_addr_type <a id="enum_bt_addr_type"></a>

```c
typedef enum {
    BT_ADDRESS_TYPE_PUBLIC_DEVICE_ADDRESS          =  0x00, /*!< 蓝牙公有地址 */
    BT_ADDRESS_TYPE_RANDOM_DEVICE_ADDRESS          =  0x01, /*!< 蓝牙随机地址 */
} bt_addr_type;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| BT_ADDRESS_TYPE_PUBLIC_DEVICE_ADDRESS | 0x00 | 蓝牙公有地址 |
| BT_ADDRESS_TYPE_RANDOM_DEVICE_ADDRESS | 0x01 | 蓝牙随机地址 |

### profile_connect_state_t <a id="enum_profile_connect_state_t"></a>

```c
typedef enum {
    PROFILE_STATE_CONNECTING = 0x01,    /*!< 正在连接 */
    PROFILE_STATE_CONNECTED = 0x02,     /*!< 已连接 */
    PROFILE_STATE_DISCONNECTING = 0x03, /*!< 正在断连 */
    PROFILE_STATE_DISCONNECTED = 0x04   /*!< 已断连 */
} profile_connect_state_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| PROFILE_STATE_CONNECTING | 0x01 | 正在连接 |
| PROFILE_STATE_CONNECTED | 0x02 | 已连接 |
| PROFILE_STATE_DISCONNECTING | 0x03 | 正在断连 |
| PROFILE_STATE_DISCONNECTED | 0x04 | 已断连 |

## Structures

### spp_create_socket_para_t <a id="struct_spp_create_socket_para_t"></a>

```c
typedef struct {
    bt_uuid_t uuid;                /*!< SPP socket对应的服务UUID */
    spp_socket_type_t socket_type; /*!< SPP socket类型 */
    bool is_encrypt;               /*!< 是否加密（目前此参数未使用，默认加密） */
} spp_create_socket_para_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| uuid | [bt_uuid_t](#struct_bt_uuid_t) | SPP socket对应的服务UUID |
| socket_type | [spp_socket_type_t](#enum_spp_socket_type_t) | SPP socket类型，取值参见spp_socket_type_t枚举 |
| is_encrypt | bool | 是否加密（目前此参数未使用，默认加密） |

### bd_addr_t <a id="struct_bd_addr_t"></a>

```c
typedef struct {
    uint8_t addr[BD_ADDR_LEN];     /*!< 蓝牙地址 */
    uint8_t type;                  /*!< 蓝牙地址类型，参考bt_addr_type */
} bd_addr_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| addr | uint8_t[6] | 蓝牙设备地址，长度为BD_ADDR_LEN(6)字节 |
| type | uint8_t | 蓝牙地址类型，取值参见[bt_addr_type](#enum_bt_addr_type) |

### bt_uuid_t <a id="struct_bt_uuid_t"></a>

```c
typedef struct {
    uint8_t uuid_len;               /*!< UUID长度 */
    uint8_t uuid[BT_UUID_MAX_LEN]; /*!< UUID字段 */
} bt_uuid_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| uuid_len | uint8_t | UUID长度，取值参见[uuid_type_t](#enum_uuid_type_t) |
| uuid | uint8_t[16] | UUID字段，最大长度为BT_UUID_MAX_LEN(16)字节 |

### spp_callbacks_t <a id="struct_spp_callbacks_t"></a>

```c
typedef struct {
    spp_receive_data_callback receive_data_cb;
    spp_conn_state_changed_callback conn_state_changed_cb;
} spp_callbacks_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| receive_data_cb | [spp_receive_data_callback](#typedef_spp_receive_data_callback) | 接收数据回调函数 |
| conn_state_changed_cb | [spp_conn_state_changed_callback](#typedef_spp_conn_state_changed_callback) | 连接状态变化回调函数 |

## Type definitions

### spp_conn_state_changed_callback <a id="typedef_spp_conn_state_changed_callback"></a>

```c
typedef void (*spp_conn_state_changed_callback)(uint8_t client_id, profile_connect_state_t state);
```

**使用说明**

在spp_callbacks_t结构体中作为conn_state_changed_cb成员使用，用于上报SPP连接状态变化事件。

### spp_receive_data_callback <a id="typedef_spp_receive_data_callback"></a>

```c
typedef void (*spp_receive_data_callback)(uint8_t client_id, uint8_t *data, uint32_t data_len);
```

**使用说明**

在spp_callbacks_t结构体中作为receive_data_cb成员使用，用于上报接收到的SPP数据。



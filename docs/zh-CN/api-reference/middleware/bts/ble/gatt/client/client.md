# BLE GATT Client

GATT (Generic Attribute Profile) Client 端 API（中心角色）。管理 Client 注册/注销、服务/特征/描述符发现、按句柄和 UUID (Universally Unique Identifier) 读写、Write Request/Write Command 以及 MTU (Maximum Transmission Unit) 交换请求。

Client 端主动发现 Server 端公布的服务结构，根据句柄或 UUID 读写特征值和描述符。发现服务后才能知道有哪些 Characteristic 可以操作。

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_client.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [gattc_register_client](#gattc_register_client) | 注册GATT客户端 |
| [gattc_unregister_client](#gattc_unregister_client) | 注销GATT客户端 |
| [gattc_discovery_service](#gattc_discovery_service) | 发现远端GATT服务 |
| [gattc_discovery_character](#gattc_discovery_character) | 发现远端GATT特征 |
| [gattc_discovery_descriptor](#gattc_discovery_descriptor) | 发现远端GATT特征描述符 |
| [gattc_read_req_by_handle](#gattc_read_req_by_handle) | 按照句柄发起读取请求 |
| [gattc_read_req_by_uuid](#gattc_read_req_by_uuid) | 按照UUID发起读取请求 |
| [gattc_write_req](#gattc_write_req) | 发起写请求 |
| [gattc_write_cmd](#gattc_write_cmd) | 发起写命令 |
| [gattc_exchange_mtu_req](#gattc_exchange_mtu_req) | 发送交换MTU请求 |
| [gattc_register_callbacks](#gattc_register_callbacks) | 注册GATT客户端回调函数 |

## Functions

### gattc_register_client <a id="gattc_register_client"></a>

```c
errcode_t gattc_register_client(bt_uuid_t *app_uuid, uint8_t *client_id)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_client.h"
```

**功能说明**

- 注册GATT客户端，向协议栈申请客户端资源并分配客户端ID
- 注册成功后，通过出参client_id返回分配的客户端标识，后续所有GATT Client操作均需携带该ID
- 同一应用可通过不同的app_uuid注册多个客户端实例

**前置条件**

- BLE (Bluetooth Low Energy) 协议栈已初始化完成，enable_ble()已调用成功
- app_uuid指向的内存空间已申请成功且uuid_len与uuid字段已正确赋值

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| app_uuid | [bt_uuid_t](#struct_bt_uuid_t) * | 上层应用UUID，用于标识客户端应用 | uuid_len取值参考[uuid_type_t](#enum_uuid_type_t) |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| client_id | uint8_t * | 分配的客户端ID，用于后续GATT Client操作标识 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 客户端注册成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [ble_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_client/src/ble_speed_client.c)

### gattc_unregister_client <a id="gattc_unregister_client"></a>

```c
errcode_t gattc_unregister_client(uint8_t client_id)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_client.h"
```

**功能说明**

- 注销已注册的GATT客户端，释放协议栈分配的客户端资源
- 注销后该client_id不再有效，后续使用该ID的操作将返回错误
- 应在断开所有连接后调用此接口注销客户端

**前置条件**

- 已通过gattc_register_client()成功注册客户端，获得有效的client_id

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| client_id | uint8_t | 客户端ID | gattc_register_client()分配的有效ID |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 客户端注销成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

### gattc_discovery_service <a id="gattc_discovery_service"></a>

```c
errcode_t gattc_discovery_service(uint8_t client_id, uint16_t conn_id, bt_uuid_t *uuid)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_client.h"
```

**功能说明**

- 向远端GATT Server发起服务发现请求
- 若uuid长度为0，发现所有服务；否则按uuid过滤发现指定服务
- 发现结果通过gattc_discovery_service_callback和gattc_discovery_service_complete_callback回调异步返回

**前置条件**

- 已通过gattc_register_client()成功注册客户端，获得有效的client_id
- 已与远端设备建立BLE连接，conn_id为有效的连接标识
- 已通过gattc_register_callbacks()注册回调函数，用于接收发现结果

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| client_id | uint8_t | 客户端ID | gattc_register_client()分配的有效ID |
| conn_id | uint16_t | 连接ID | 已建立的BLE连接标识 |
| uuid | [bt_uuid_t](#struct_bt_uuid_t) * | 服务UUID，uuid长度为0时发现所有服务 | uuid_len取值参考[uuid_type_t](#enum_uuid_type_t) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 请求发起失败 |

**参考案例**

- [ble_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_client/src/ble_speed_client.c)

### gattc_discovery_character <a id="gattc_discovery_character"></a>

```c
errcode_t gattc_discovery_character(uint8_t client_id, uint16_t conn_id, gattc_discovery_character_param_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_client.h"
```

**功能说明**

- 向远端GATT Server发起特征发现请求，发现指定服务下的特征
- 若param中uuid长度为0，发现该服务内所有特征；否则按uuid过滤发现指定特征
- 发现结果通过gattc_discovery_character_callback和gattc_discovery_character_complete_callback回调异步返回

**前置条件**

- 已通过gattc_discovery_service()完成服务发现，获得有效的service_handle
- 已与远端设备建立BLE连接，conn_id为有效的连接标识
- 已通过gattc_register_callbacks()注册回调函数，用于接收发现结果

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| client_id | uint8_t | 客户端ID | gattc_register_client()分配的有效ID |
| conn_id | uint16_t | 连接ID | 已建立的BLE连接标识 |
| param | [gattc_discovery_character_param_t](#struct_gattc_discovery_character_param_t) * | 发现特征参数 | service_handle为服务发现返回的起始句柄 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 请求发起失败 |

**参考案例**

- [ble_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_client/src/ble_speed_client.c)

### gattc_discovery_descriptor <a id="gattc_discovery_descriptor"></a>

```c
errcode_t gattc_discovery_descriptor(uint8_t client_id, uint16_t conn_id, uint16_t character_handle)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_client.h"
```

**功能说明**

- 向远端GATT Server发起特征描述符发现请求
- 发现结果通过gattc_discovery_descriptor_callback和gattc_discovery_descriptor_complete_callback回调异步返回
- 用于获取特征的配置描述符（如CCCD (Client Characteristic Configuration Descriptor)）等信息

**前置条件**

- 已通过gattc_discovery_character()完成特征发现，获得有效的character_handle
- 已与远端设备建立BLE连接，conn_id为有效的连接标识
- 已通过gattc_register_callbacks()注册回调函数，用于接收发现结果

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| client_id | uint8_t | 客户端ID | gattc_register_client()分配的有效ID |
| conn_id | uint16_t | 连接ID | 已建立的BLE连接标识 |
| character_handle | uint16_t | 特征声明句柄 | gattc_discovery_character()返回的declare_handle |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 请求发起失败 |

**参考案例**

- [ble_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_client/src/ble_speed_client.c)

### gattc_read_req_by_handle <a id="gattc_read_req_by_handle"></a>

```c
errcode_t gattc_read_req_by_handle(uint8_t client_id, uint16_t conn_id, uint16_t handle)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_client.h"
```

**功能说明**

- 向远端GATT Server按照属性句柄发起读取请求
- 读取结果通过gattc_read_cfm_callback回调异步返回
- 请求需要对端响应确认，属于确认型读取操作

**前置条件**

- 已通过gattc_discovery_character()或gattc_discovery_descriptor()获得有效的属性句柄
- 已与远端设备建立BLE连接，conn_id为有效的连接标识
- 已通过gattc_register_callbacks()注册回调函数，用于接收读取结果

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| client_id | uint8_t | 客户端ID | gattc_register_client()分配的有效ID |
| conn_id | uint16_t | 连接ID | 已建立的BLE连接标识 |
| handle | uint16_t | 属性句柄 | 服务发现或特征发现返回的有效句柄 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 请求发起失败 |

### gattc_read_req_by_uuid <a id="gattc_read_req_by_uuid"></a>

```c
errcode_t gattc_read_req_by_uuid(uint8_t client_id, uint16_t conn_id, gattc_read_req_by_uuid_param_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_client.h"
```

**功能说明**

- 向远端GATT Server按照UUID发起读取请求，在指定句柄范围内查找匹配UUID的属性值
- 读取结果通过gattc_read_cfm_callback和gattc_read_by_uuid_complete_callback回调异步返回
- 请求需要对端响应确认，属于确认型读取操作

**前置条件**

- 已与远端设备建立BLE连接，conn_id为有效的连接标识
- param中start_hdl和end_hdl指定了有效的句柄范围
- 已通过gattc_register_callbacks()注册回调函数，用于接收读取结果

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| client_id | uint8_t | 客户端ID | gattc_register_client()分配的有效ID |
| conn_id | uint16_t | 连接ID | 已建立的BLE连接标识 |
| param | [gattc_read_req_by_uuid_param_t](#struct_gattc_read_req_by_uuid_param_t) * | 按UUID读取请求参数 | start_hdl和end_hdl为有效句柄范围 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 请求发起失败 |

### gattc_write_req <a id="gattc_write_req"></a>

```c
errcode_t gattc_write_req(uint8_t client_id, uint16_t conn_id, gattc_handle_value_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_client.h"
```

**功能说明**

- 向远端GATT Server发起写请求，需要对端响应确认
- 写入结果通过gattc_write_cfm_callback回调异步返回
- 适用于需要对端确认写入结果的场景

**前置条件**

- 已通过服务发现或特征发现获得有效的属性句柄
- 已与远端设备建立BLE连接，conn_id为有效的连接标识
- 已通过gattc_register_callbacks()注册回调函数，用于接收写入结果

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| client_id | uint8_t | 客户端ID | gattc_register_client()分配的有效ID |
| conn_id | uint16_t | 连接ID | 已建立的BLE连接标识 |
| param | [gattc_handle_value_t](#struct_gattc_handle_value_t) * | 写请求参数 | handle为有效属性句柄，data_len不超过当前MTU限制 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 请求发起失败 |

### gattc_write_cmd <a id="gattc_write_cmd"></a>

```c
errcode_t gattc_write_cmd(uint8_t client_id, uint16_t conn_id, gattc_handle_value_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_client.h"
```

**功能说明**

- 向远端GATT Server发起写命令，无需对端响应确认
- 该接口不触发gattc_write_cfm_callback回调，属于无确认型写入操作
- 适用于对写入可靠性要求不高但需要快速发送数据的场景

**前置条件**

- 已通过服务发现或特征发现获得有效的属性句柄
- 已与远端设备建立BLE连接，conn_id为有效的连接标识
- param中handle为有效属性句柄，data指向的内存空间已申请成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| client_id | uint8_t | 客户端ID | gattc_register_client()分配的有效ID |
| conn_id | uint16_t | 连接ID | 已建立的BLE连接标识 |
| param | [gattc_handle_value_t](#struct_gattc_handle_value_t) * | 写命令参数 | handle为有效属性句柄，data_len不超过当前MTU限制 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 命令成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 命令发起失败 |

### gattc_exchange_mtu_req <a id="gattc_exchange_mtu_req"></a>

```c
errcode_t gattc_exchange_mtu_req(uint8_t client_id, uint16_t conn_id, uint16_t mtu_size)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_client.h"
```

**功能说明**

- 向远端GATT Server发送交换MTU请求，协商后续通信的最大传输单元
- MTU协商结果通过gattc_mtu_changed_callback回调异步返回
- 协商后的实际MTU为双方支持值的较小值

**前置条件**

- 已与远端设备建立BLE连接，conn_id为有效的连接标识
- mtu_size在协议允许范围内，取值范围[SDK_BLE_MTU_MIN, SDK_BLE_MTU_MAX]

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| client_id | uint8_t | 客户端ID | gattc_register_client()分配的有效ID |
| conn_id | uint16_t | 连接ID | 已建立的BLE连接标识 |
| mtu_size | uint16_t | 客户端接收MTU大小 | [23, 517]，即[SDK_BLE_MTU_MIN, SDK_BLE_MTU_MAX] |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 请求发起失败 |

**参考案例**

- [ble_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_server/src/ble_speed_server.c)

### gattc_register_callbacks <a id="gattc_register_callbacks"></a>

```c
errcode_t gattc_register_callbacks(gattc_callbacks_t *func)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_client.h"
```

**功能说明**

- 注册GATT客户端回调函数集合，用于接收服务发现、特征发现、读写操作、MTU变更、通知及指示等异步结果
- 回调函数运行于BTS (Bluetooth Subsystem) 线程，不可阻塞或长时间等待
- 回调中涉及的内存在BTS线程中自动申请和释放，回调中不应释放

**前置条件**

- BLE协议栈已初始化完成，enable_ble()已调用成功
- func指向的回调函数结构体已正确赋值所有成员

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func | [gattc_callbacks_t](#struct_gattc_callbacks_t) * | 回调函数结构体指针 | 各回调成员函数指针已正确赋值 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 回调注册成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 注册失败 |

**参考案例**

- [ble_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_client/src/ble_speed_client.c)


## Type definitions

### errcode_t <a id="typedef_errcode_t"></a>

```c
typedef uint32_t errcode_t;
```

**使用说明**

作为GATT Client所有接口的返回值类型，用于表示接口调用的执行结果

## Enumerations

### uuid_type_t <a id="enum_uuid_type_t"></a>

```c
typedef enum {
    UUID_TYPE_NULL = 0x00, /*!< 空uuid。 */
    UUID_TYPE_16_BIT,      /*!< 16bit长度的uuid。 */
    UUID_TYPE_32_BIT,      /*!< 32bit长度的uuid。 */
    UUID_TYPE_128_BIT,     /*!< 128bit长度的uuid。 */
} uuid_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| UUID_TYPE_NULL | 0x00 | 空UUID |
| UUID_TYPE_16_BIT | 0x01 | 16bit长度的UUID |
| UUID_TYPE_32_BIT | 0x02 | 32bit长度的UUID |
| UUID_TYPE_128_BIT | 0x03 | 128bit长度的UUID |

### gatt_characteristic_property_t <a id="enum_gatt_characteristic_property_t"></a>

```c
typedef enum {
    GATT_CHARACTER_PROPERTY_BIT_BROADCAST = 0x01,         /*!< 广播特征值 */
    GATT_CHARACTER_PROPERTY_BIT_READ = 0x02,              /*!< 读特征值 */
    GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP = 0x04,      /*!< 写特征值并且不需要响应 */
    GATT_CHARACTER_PROPERTY_BIT_WRITE = 0x08,             /*!< 写特征值 */
    GATT_CHARACTER_PROPERTY_BIT_NOTIFY = 0x10,            /*!< 通知特征值 */
    GATT_CHARACTER_PROPERTY_BIT_INDICATE = 0x20,          /*!< 指示特征值 */
    GATT_CHARACTER_PROPERTY_BIT_SIGNED_WRITE = 0x40,      /*!< 签名写特征值 */
    GATT_CHARACTER_PROPERTY_BIT_EXTENDED_PROPERTY = 0x80, /*!< 在特征扩展特性描述符中定义了附加的特征特性 */
} gatt_characteristic_property_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GATT_CHARACTER_PROPERTY_BIT_BROADCAST | 0x01 | 广播特征值 |
| GATT_CHARACTER_PROPERTY_BIT_READ | 0x02 | 读特征值 |
| GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP | 0x04 | 写特征值并且不需要响应 |
| GATT_CHARACTER_PROPERTY_BIT_WRITE | 0x08 | 写特征值 |
| GATT_CHARACTER_PROPERTY_BIT_NOTIFY | 0x10 | 通知特征值 |
| GATT_CHARACTER_PROPERTY_BIT_INDICATE | 0x20 | 指示特征值 |
| GATT_CHARACTER_PROPERTY_BIT_SIGNED_WRITE | 0x40 | 签名写特征值 |
| GATT_CHARACTER_PROPERTY_BIT_EXTENDED_PROPERTY | 0x80 | 在特征扩展特性描述符中定义了附加的特征特性 |

### gatt_status_t <a id="enum_gatt_status_t"></a>

```c
typedef enum {
    GATT_STATUS_SUCCESS = 0x00,                          /*!< 执行成功错误码 */
    GATT_STATUS_INVALID_HANDLE = 0x01,                   /*!< 无效handle错误码 */
    GATT_STATUS_READ_NOT_PERMITTED = 0x02,               /*!< 不可读错误码 */
    GATT_STATUS_WRITE_NOT_PERMITTED = 0x03,              /*!< 不可写错误码 */
    GATT_STATUS_INVALID_PDU = 0x04,                      /*!< 无效PDU错误码 */
    GATT_STATUS_INSUFFICIENT_AUTHENTICATION = 0x05,      /*!< 未认证的读写错误码 */
    GATT_STATUS_REQUEST_NOT_SUPPORTED = 0x06,            /*!< 不支持client发送的请求 */
    GATT_STATUS_INVALID_OFFSET = 0x07,                   /*!< 无效偏移的读写错误码 */
    GATT_STATUS_INSUFFICIENT_AUTHORIZATION = 0x08,       /*!< 未授权错误码 */
    GATT_STATUS_PREPARE_QUEUE_FULL = 0x09,               /*!< 排队的prepare writes太多，队列已满错误码 */
    GATT_STATUS_ATTRIBUTE_NOT_FOUND = 0x0A,              /*!< 属性未找到错误码 */
    GATT_STATUS_ATTRIBUTE_NOT_LONG = 0x0B,               /*!< 该attribute不是long attribute, 不能使用ATT_READ_BLOB_REQ PDU */
    GATT_STATUS_INSUFFICIENT_ENCRYPTION_KEY_SIZE = 0x0C, /*!< 用于加密此连接的加密密钥大小不足 */
    GATT_STATUS_INVALID_ATTRIBUTE_VALUE_LENGTH = 0x0D,   /*!< 无效的属性值长度 */
    GATT_STATUS_UNLIKELY_ERROR = 0x0E,                   /*!< 不可能的错误码 */
    GATT_STATUS_INSUFFICIENT_ENCRYPTION = 0x0F,          /*!< 加密不足错误码 */
    GATT_STATUS_UNSUPPORTED_GROUP_TYPE = 0x10,           /*!< 不支持的grouping attribute错误码 */
    GATT_STATUS_INSUFFICIENT_RESOURCES = 0x11,           /*!< 资源不足错误码 */
    GATT_STATUS_DATABASE_OUT_OF_SYNC = 0x12,             /*!< 服务端与客户端未同步数据库错误码 */
    GATT_STATUS_VALUE_NOT_ALLOWED = 0x13,                /*!< 非法的参数值错误码 */
} gatt_status_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GATT_STATUS_SUCCESS | 0x00 | 执行成功 |
| GATT_STATUS_INVALID_HANDLE | 0x01 | 无效句柄 |
| GATT_STATUS_READ_NOT_PERMITTED | 0x02 | 不可读 |
| GATT_STATUS_WRITE_NOT_PERMITTED | 0x03 | 不可写 |
| GATT_STATUS_INVALID_PDU | 0x04 | 无效PDU (Protocol Data Unit) |
| GATT_STATUS_INSUFFICIENT_AUTHENTICATION | 0x05 | 未认证的读写 |
| GATT_STATUS_REQUEST_NOT_SUPPORTED | 0x06 | 不支持client发送的请求 |
| GATT_STATUS_INVALID_OFFSET | 0x07 | 无效偏移的读写 |
| GATT_STATUS_INSUFFICIENT_AUTHORIZATION | 0x08 | 未授权 |
| GATT_STATUS_PREPARE_QUEUE_FULL | 0x09 | 排队队列已满 |
| GATT_STATUS_ATTRIBUTE_NOT_FOUND | 0x0A | 属性未找到 |
| GATT_STATUS_ATTRIBUTE_NOT_LONG | 0x0B | 非long attribute |
| GATT_STATUS_INSUFFICIENT_ENCRYPTION_KEY_SIZE | 0x0C | 加密密钥大小不足 |
| GATT_STATUS_INVALID_ATTRIBUTE_VALUE_LENGTH | 0x0D | 无效属性值长度 |
| GATT_STATUS_UNLIKELY_ERROR | 0x0E | 不可能的错误 |
| GATT_STATUS_INSUFFICIENT_ENCRYPTION | 0x0F | 加密不足 |
| GATT_STATUS_UNSUPPORTED_GROUP_TYPE | 0x10 | 不支持的grouping attribute |
| GATT_STATUS_INSUFFICIENT_RESOURCES | 0x11 | 资源不足 |
| GATT_STATUS_DATABASE_OUT_OF_SYNC | 0x12 | 数据库未同步 |
| GATT_STATUS_VALUE_NOT_ALLOWED | 0x13 | 非法的参数值 |

## Structures

### bt_uuid_t <a id="struct_bt_uuid_t"></a>

```c
typedef struct {
    uint8_t uuid_len;               /*!< UUID长度。 */
    uint8_t uuid[BT_UUID_MAX_LEN]; /*!< UUID字段。 */
} bt_uuid_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| uuid_len | uint8_t | UUID长度，取值参考[uuid_type_t](#enum_uuid_type_t) |
| uuid | uint8_t[BT_UUID_MAX_LEN] | UUID字段，BT_UUID_MAX_LEN为16字节 |

### gattc_handle_value_t <a id="struct_gattc_handle_value_t"></a>

```c
typedef struct {
    uint16_t handle;   /*!< 属性句柄。 */
    uint16_t data_len; /*!< 数据长度。 */
    uint8_t *data;     /*!< 数据。 */
} gattc_handle_value_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| handle | uint16_t | 属性句柄 |
| data_len | uint16_t | 数据长度 |
| data | uint8_t * | 数据指针 |

### gattc_discovery_character_param_t <a id="struct_gattc_discovery_character_param_t"></a>

```c
typedef struct {
    uint16_t service_handle; /*!< 服务起始句柄。 */
    bt_uuid_t uuid;          /*!< 特征uuid，如果uuid长度为0，发现服务内的所有特征，否则按照uuid过滤。 */
} gattc_discovery_character_param_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| service_handle | uint16_t | 服务起始句柄 |
| uuid | [bt_uuid_t](#struct_bt_uuid_t) | 特征UUID，uuid长度为0时发现所有特征，否则按UUID过滤 |

### gattc_discovery_service_result_t <a id="struct_gattc_discovery_service_result_t"></a>

```c
typedef struct {
    uint16_t start_hdl; /*!< 服务起始句柄。 */
    uint16_t end_hdl;   /*!< 服务结束句柄。 */
    bt_uuid_t uuid;     /*!< 服务uuid。 */
} gattc_discovery_service_result_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| start_hdl | uint16_t | 服务起始句柄 |
| end_hdl | uint16_t | 服务结束句柄 |
| uuid | [bt_uuid_t](#struct_bt_uuid_t) | 服务UUID |

### gattc_discovery_character_result_t <a id="struct_gattc_discovery_character_result_t"></a>

```c
typedef struct {
    bt_uuid_t uuid;          /*!< 特征uuid。 */
    uint16_t declare_handle; /*!< 特征声明句柄。 */
    uint16_t value_handle;   /*!< 特征值句柄。 */
    uint8_t properties;      /*!< 特征特性，参考gatt_characteristic_property_t。 */
} gattc_discovery_character_result_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| uuid | [bt_uuid_t](#struct_bt_uuid_t) | 特征UUID |
| declare_handle | uint16_t | 特征声明句柄 |
| value_handle | uint16_t | 特征值句柄 |
| properties | uint8_t | 特征特性，取值参考[gatt_characteristic_property_t](#enum_gatt_characteristic_property_t) |

### gattc_discovery_descriptor_result_t <a id="struct_gattc_discovery_descriptor_result_t"></a>

```c
typedef struct {
    uint16_t descriptor_hdl; /*!< 特征描述符句柄。 */
    bt_uuid_t uuid;          /*!< 特征描述符uuid。 */
} gattc_discovery_descriptor_result_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| descriptor_hdl | uint16_t | 特征描述符句柄 |
| uuid | [bt_uuid_t](#struct_bt_uuid_t) | 特征描述符UUID |

### gattc_read_req_by_uuid_param_t <a id="struct_gattc_read_req_by_uuid_param_t"></a>

```c
typedef struct {
    uint16_t start_hdl; /*!< 起始句柄。 */
    uint16_t end_hdl;   /*!< 结束句柄。 */
    bt_uuid_t uuid;     /*!< uuid。 */
} gattc_read_req_by_uuid_param_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| start_hdl | uint16_t | 起始句柄 |
| end_hdl | uint16_t | 结束句柄 |
| uuid | [bt_uuid_t](#struct_bt_uuid_t) | UUID |

### gattc_callbacks_t <a id="struct_gattc_callbacks_t"></a>

```c
typedef struct ble_gattc_callbacks {
    gattc_discovery_service_callback discovery_svc_cb;                  /*!< 发现服务回调函数。 */
    gattc_discovery_service_complete_callback discovery_svc_cmp_cb;     /*!< 发现服务完成回调函数。 */
    gattc_discovery_character_callback discovery_chara_cb;              /*!< 发现特征回调函数。 */
    gattc_discovery_character_complete_callback discovery_chara_cmp_cb; /*!< 发现特征完成回调函数。 */
    gattc_discovery_descriptor_callback discovery_desc_cb;              /*!< 发现特征描述符回调函数。 */
    gattc_discovery_descriptor_complete_callback discovery_desc_cmp_cb; /*!< 发现特征描述符完成回调函数。 */
    gattc_read_cfm_callback read_cb;                                    /*!< 收到读响应回调函数。 */
    gattc_read_by_uuid_complete_callback read_cmp_cb;                   /*!< 按照uuid读取完成回调函数。 */
    gattc_write_cfm_callback write_cb;                                  /*!< 收到写响应回调函数。 */
    gattc_mtu_changed_callback mtu_changed_cb;                          /*!< mtu改变回调函数，serverId作为预留参数，固定为0。 */
    gattc_notification_callback notification_cb;                        /*!< 收到通知回调函数。 */
    gattc_indication_callback indication_cb;                            /*!< 收到指示回调函数。 */
} gattc_callbacks_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| discovery_svc_cb | gattc_discovery_service_callback | 发现服务回调函数 |
| discovery_svc_cmp_cb | gattc_discovery_service_complete_callback | 发现服务完成回调函数 |
| discovery_chara_cb | gattc_discovery_character_callback | 发现特征回调函数 |
| discovery_chara_cmp_cb | gattc_discovery_character_complete_callback | 发现特征完成回调函数 |
| discovery_desc_cb | gattc_discovery_descriptor_callback | 发现特征描述符回调函数 |
| discovery_desc_cmp_cb | gattc_discovery_descriptor_complete_callback | 发现特征描述符完成回调函数 |
| read_cb | gattc_read_cfm_callback | 收到读响应回调函数 |
| read_cmp_cb | gattc_read_by_uuid_complete_callback | 按照uuid读取完成回调函数 |
| write_cb | gattc_write_cfm_callback | 收到写响应回调函数 |
| mtu_changed_cb | gattc_mtu_changed_callback | MTU改变回调函数 |
| notification_cb | gattc_notification_callback | 收到通知回调函数 |
| indication_cb | gattc_indication_callback | 收到指示回调函数 |





# BLE GATT Server

GATT (Generic Attribute Profile) Server 端 API（外设角色）。管理 Server 注册/注销、服务/特征/描述符的异步和同步添加、服务启动/停止/删除、发送 GATT 响应和通知/指示（按句柄和 UUID (Universally Unique Identifier)）以及 MTU (Maximum Transmission Unit) 配置。

GATT 服务模型为四级结构：Server → Service → Characteristic → Descriptor。Server 端通过 Notify/Indicate 主动推送数据给已订阅的 Client 端。

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_server.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [gatts_register_server](#gatts_register_server) | 注册GATT服务端 |
| [gatts_unregister_server](#gatts_unregister_server) | 注销GATT服务端 |
| [gatts_add_service](#gatts_add_service) | 异步添加GATT服务 |
| [gatts_add_characteristic](#gatts_add_characteristic) | 异步添加GATT特征 |
| [gatts_add_descriptor](#gatts_add_descriptor) | 异步添加GATT特征描述符 |
| [gatts_add_service_sync](#gatts_add_service_sync) | 同步添加GATT服务 |
| [gatts_add_characteristic_sync](#gatts_add_characteristic_sync) | 同步添加GATT特征 |
| [gatts_add_descriptor_sync](#gatts_add_descriptor_sync) | 同步添加GATT特征描述符 |
| [gatts_start_service](#gatts_start_service) | 启动GATT服务 |
| [gatts_stop_service](#gatts_stop_service) | 停止GATT服务 |
| [gatts_delete_service](#gatts_delete_service) | 删除GATT服务 |
| [gatts_delete_all_services](#gatts_delete_all_services) | 删除所有GATT服务 |
| [gatts_send_response](#gatts_send_response) | 发送读写响应 |
| [gatts_notify_indicate](#gatts_notify_indicate) | 通过属性句柄发送通知或指示 |
| [gatts_notify_indicate_by_uuid](#gatts_notify_indicate_by_uuid) | 通过UUID发送通知或指示 |
| [gatts_set_mtu_size](#gatts_set_mtu_size) | 设置服务端接收MTU大小 |
| [gatts_register_callbacks](#gatts_register_callbacks) | 注册GATT服务端回调函数 |

## Functions

### gatts_register_server <a id="gatts_register_server"></a>

```c
errcode_t gatts_register_server(bt_uuid_t *app_uuid, uint8_t *server_id)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_server.h"
```

**功能说明**

- 注册GATT服务端，向协议栈申请一个服务端实例
- 通过app_uuid标识上层应用，协议栈分配server_id作为服务端标识
- 注册成功后方可进行服务、特征的添加和操作

**前置条件**

- BLE (Bluetooth Low Energy)协议栈已初始化完成（enable_ble()已调用且返回成功）
- app_uuid不为NULL，且指向有效内存空间

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| app_uuid | [bt_uuid_t](#struct_bt_uuid_t) * | 上层应用UUID | 不为NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| server_id | uint8_t * | 协议栈分配的服务端ID |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 注册成功 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

**参考案例**

- [ble_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_server/src/ble_speed_server.c)

### gatts_unregister_server <a id="gatts_unregister_server"></a>

```c
errcode_t gatts_unregister_server(uint8_t server_id)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_server.h"
```

**功能说明**

- 注销GATT服务端，释放协议栈中的服务端实例资源
- 注销后该server_id不再有效，不可用于后续服务操作
- 注销前应确保已停止并删除所有已添加的服务

**前置条件**

- 对应server_id已通过gatts_register_server()注册成功
- 已停止并删除该server_id下的所有服务

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | 服务端ID | gatts_register_server()分配的有效ID |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 注销成功 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### gatts_add_service <a id="gatts_add_service"></a>

```c
errcode_t gatts_add_service(uint8_t server_id, bt_uuid_t *service_uuid, bool is_primary)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_server.h"
```

**功能说明**

- 异步添加GATT服务，通过回调返回服务句柄
- 支持添加首要服务（primary）或次要服务（secondary）
- 服务添加结果通过gatts_add_service_callback回调通知

**前置条件**

- server_id已通过gatts_register_server()注册成功
- service_uuid不为NULL，指向有效UUID

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | 服务端ID | gatts_register_server()分配的有效ID |
| service_uuid | [bt_uuid_t](#struct_bt_uuid_t) * | 服务UUID | 不为NULL |
| is_primary | bool | 是否为首要服务 | true: 首要服务; false: 次要服务 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败，服务句柄将在gatts_add_service_callback中返回 |

### gatts_add_characteristic <a id="gatts_add_characteristic"></a>

```c
errcode_t gatts_add_characteristic(uint8_t server_id, uint16_t service_handle, gatts_add_chara_info_t *character)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_server.h"
```

**功能说明**

- 异步添加GATT特征到指定服务，通过回调返回特征句柄
- 特征包括UUID、权限、特性、初始值等信息
- 特征添加结果通过gatts_add_characteristic_callback回调通知

**前置条件**

- server_id已通过gatts_register_server()注册成功
- service_handle对应的服务已通过gatts_add_service()或gatts_add_service_sync()添加成功
- character不为NULL，指向有效特征信息

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | 服务端ID | gatts_register_server()分配的有效ID |
| service_handle | uint16_t | 服务属性句柄 | gatts_add_service回调返回的有效句柄 |
| character | [gatts_add_chara_info_t](#struct_gatts_add_chara_info_t) * | 特征信息 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败，特征句柄将在gatts_add_characteristic_callback中返回 |

### gatts_add_descriptor <a id="gatts_add_descriptor"></a>

```c
errcode_t gatts_add_descriptor(uint8_t server_id, uint16_t service_handle, gatts_add_desc_info_t *descriptor)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_server.h"
```

**功能说明**

- 异步添加GATT特征描述符到指定服务，通过回调返回描述符句柄
- 描述符包括UUID、权限、初始值等信息
- 描述符添加结果通过gatts_add_descriptor_callback回调通知

**前置条件**

- server_id已通过gatts_register_server()注册成功
- service_handle对应的服务已添加成功
- descriptor不为NULL，指向有效描述符信息

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | 服务端ID | gatts_register_server()分配的有效ID |
| service_handle | uint16_t | 服务属性句柄 | gatts_add_service回调返回的有效句柄 |
| descriptor | [gatts_add_desc_info_t](#struct_gatts_add_desc_info_t) * | 特征描述符信息 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败，描述符句柄将在gatts_add_descriptor_callback中返回 |

### gatts_add_service_sync <a id="gatts_add_service_sync"></a>

```c
errcode_t gatts_add_service_sync(uint8_t server_id, bt_uuid_t *service_uuid, bool is_primary, uint16_t *handle)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_server.h"
```

**功能说明**

- 同步添加GATT服务，通过出参直接返回服务句柄
- 支持添加首要服务（primary）或次要服务（secondary）
- 与异步接口gatts_add_service()不同，本接口阻塞等待直到服务添加完成

**前置条件**

- server_id已通过gatts_register_server()注册成功
- service_uuid不为NULL，指向有效UUID
- handle不为NULL，指向有效内存空间

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | 服务端ID | gatts_register_server()分配的有效ID |
| service_uuid | [bt_uuid_t](#struct_bt_uuid_t) * | 服务UUID | 不为NULL |
| is_primary | bool | 是否为首要服务 | true: 首要服务; false: 次要服务 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| handle | uint16_t * | 服务属性句柄 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 服务添加成功 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

**参考案例**

- [ble_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_server/src/ble_speed_server.c)

### gatts_add_characteristic_sync <a id="gatts_add_characteristic_sync"></a>

```c
errcode_t gatts_add_characteristic_sync(uint8_t server_id, uint16_t service_handle, gatts_add_chara_info_t *character, gatts_add_character_result_t *result)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_server.h"
```

**功能说明**

- 同步添加GATT特征到指定服务，通过出参直接返回特征句柄
- 特征包括UUID、权限、特性、初始值等信息
- 与异步接口gatts_add_characteristic()不同，本接口阻塞等待直到特征添加完成

**前置条件**

- server_id已通过gatts_register_server()注册成功
- service_handle对应的服务已通过gatts_add_service_sync()添加成功
- character不为NULL，指向有效特征信息
- result不为NULL，指向有效内存空间

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | 服务端ID | gatts_register_server()分配的有效ID |
| service_handle | uint16_t | 服务属性句柄 | gatts_add_service_sync()返回的有效句柄 |
| character | [gatts_add_chara_info_t](#struct_gatts_add_chara_info_t) * | 特征信息 | 不为NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| result | [gatts_add_character_result_t](#struct_gatts_add_character_result_t) * | 特征句柄结果 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 特征添加成功 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

**参考案例**

- [ble_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_server/src/ble_speed_server.c)

### gatts_add_descriptor_sync <a id="gatts_add_descriptor_sync"></a>

```c
errcode_t gatts_add_descriptor_sync(uint8_t server_id, uint16_t service_handle, gatts_add_desc_info_t *descriptor, uint16_t *handle)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_server.h"
```

**功能说明**

- 同步添加GATT特征描述符到指定服务，通过出参直接返回描述符句柄
- 描述符包括UUID、权限、初始值等信息
- 与异步接口gatts_add_descriptor()不同，本接口阻塞等待直到描述符添加完成

**前置条件**

- server_id已通过gatts_register_server()注册成功
- service_handle对应的服务已添加成功
- descriptor不为NULL，指向有效描述符信息
- handle不为NULL，指向有效内存空间

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | 服务端ID | gatts_register_server()分配的有效ID |
| service_handle | uint16_t | 服务属性句柄 | gatts_add_service_sync()返回的有效句柄 |
| descriptor | [gatts_add_desc_info_t](#struct_gatts_add_desc_info_t) * | 特征描述符信息 | 不为NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| handle | uint16_t * | 特征描述符属性句柄 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 描述符添加成功 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

**参考案例**

- [ble_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_server/src/ble_speed_server.c)

### gatts_start_service <a id="gatts_start_service"></a>

```c
errcode_t gatts_start_service(uint8_t server_id, uint16_t service_handle)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_server.h"
```

**功能说明**

- 启动指定GATT服务，使其对外可见并响应远端设备请求
- 启动结果通过gatts_start_service_callback回调通知
- 服务启动后，远端设备可发现和访问该服务下的特征

**前置条件**

- server_id已通过gatts_register_server()注册成功
- service_handle对应的服务已添加完成且包含所需特征

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | 服务端ID | gatts_register_server()分配的有效ID |
| service_handle | uint16_t | 服务属性句柄 | gatts_add_service回调返回的有效句柄 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败，服务启动结果将在gatts_start_service_callback中返回 |

**参考案例**

- [ble_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_server/src/ble_speed_server.c)

### gatts_stop_service <a id="gatts_stop_service"></a>

```c
errcode_t gatts_stop_service(uint8_t server_id, uint16_t service_handle)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_server.h"
```

**功能说明**

- 停止指定GATT服务，使其不再对外可见
- 停止结果通过gatts_stop_service_callback回调通知
- 服务停止后，远端设备无法发现和访问该服务

**前置条件**

- server_id已通过gatts_register_server()注册成功
- service_handle对应的服务已处于启动状态

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | 服务端ID | gatts_register_server()分配的有效ID |
| service_handle | uint16_t | 服务属性句柄 | gatts_add_service回调返回的有效句柄 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败，服务停止结果将在gatts_stop_service_callback中返回 |

### gatts_delete_service <a id="gatts_delete_service"></a>

```c
errcode_t gatts_delete_service(uint8_t server_id, uint16_t service_handle)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_server.h"
```

**功能说明**

- 删除指定GATT服务，释放该服务及其下属特征和描述符的资源
- 删除结果通过gatts_delete_service_callback回调通知
- 删除前应先停止该服务

**前置条件**

- server_id已通过gatts_register_server()注册成功
- service_handle对应的服务已存在

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | 服务端ID | gatts_register_server()分配的有效ID |
| service_handle | uint16_t | 服务属性句柄 | gatts_add_service回调返回的有效句柄 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败，服务删除结果将在gatts_delete_service_callback中返回 |

### gatts_delete_all_services <a id="gatts_delete_all_services"></a>

```c
errcode_t gatts_delete_all_services(uint8_t server_id)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_server.h"
```

**功能说明**

- 删除指定服务端下的所有GATT服务，释放全部服务资源
- 删除结果通过gatts_delete_service_callback回调通知
- 用于清理服务端所有服务，一般用于注销服务端前的清理

**前置条件**

- server_id已通过gatts_register_server()注册成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | 服务端ID | gatts_register_server()分配的有效ID |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败，服务删除结果将在gatts_delete_service_callback中返回 |

### gatts_send_response <a id="gatts_send_response"></a>

```c
errcode_t gatts_send_response(uint8_t server_id, uint16_t conn_id, gatts_send_rsp_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_server.h"
```

**功能说明**

- 对远端设备的读写请求发送响应
- 当收到gatts_read_request_callback或gatts_write_request_callback回调且need_rsp为true时，应用需调用此接口发送响应
- 响应中包含请求ID、状态码、偏移和数据

**前置条件**

- server_id已通过gatts_register_server()注册成功
- conn_id对应的BLE连接已建立
- param不为NULL，param->request_id与收到的读写请求ID一致

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | 服务端ID | gatts_register_server()分配的有效ID |
| conn_id | uint16_t | 连接ID | 已建立的BLE连接ID |
| param | [gatts_send_rsp_t](#struct_gatts_send_rsp_t) * | 响应参数 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 响应发送成功 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

**参考案例**

- [hilink_ble_adapter.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/hilink_ble_adapter.c)

### gatts_notify_indicate <a id="gatts_notify_indicate"></a>

```c
errcode_t gatts_notify_indicate(uint8_t server_id, uint16_t conn_id, gatts_ntf_ind_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_server.h"
```

**功能说明**

- 通过属性句柄向远端设备发送通知或指示
- 发送通知还是指示取决于客户端特征配置描述符（CCC (Common Configuration Characteristic)）的值
- 通知不需要对端确认，指示需要对端确认

**前置条件**

- server_id已通过gatts_register_server()注册成功
- conn_id对应的BLE连接已建立
- param不为NULL，param->attr_handle为有效特征值句柄
- 对端已通过CCC描述符使能通知或指示

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | 服务端ID | gatts_register_server()分配的有效ID |
| conn_id | uint16_t | 连接ID | 已建立的BLE连接ID |
| param | [gatts_ntf_ind_t](#struct_gatts_ntf_ind_t) * | 通知或指示参数 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 通知/指示发送成功 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

**参考案例**

- [ble_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_server/src/ble_speed_server.c)

### gatts_notify_indicate_by_uuid <a id="gatts_notify_indicate_by_uuid"></a>

```c
errcode_t gatts_notify_indicate_by_uuid(uint8_t server_id, uint16_t conn_id, gatts_ntf_ind_by_uuid_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_server.h"
```

**功能说明**

- 通过UUID向远端设备发送通知或指示
- 发送状态取决于客户端特征配置描述符（CCC）值：0x0000不允许通知和指示，0x0001允许通知，0x0002允许指示
- 通过start_handle和end_handle指定搜索范围

**前置条件**

- server_id已通过gatts_register_server()注册成功
- conn_id对应的BLE连接已建立
- param不为NULL，param->chara_uuid为有效特征UUID
- 对端已通过CCC描述符使能通知或指示

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | 服务端ID | gatts_register_server()分配的有效ID |
| conn_id | uint16_t | 连接ID | 已建立的BLE连接ID |
| param | [gatts_ntf_ind_by_uuid_t](#struct_gatts_ntf_ind_by_uuid_t) * | 通知或指示参数 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 通知/指示发送成功 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

**参考案例**

- [ble_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_server/src/ble_speed_server.c)

### gatts_set_mtu_size <a id="gatts_set_mtu_size"></a>

```c
errcode_t gatts_set_mtu_size(uint8_t server_id, uint16_t mtu_size)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_server.h"
```

**功能说明**

- 在BLE连接建立之前设置服务端接收MTU大小
- MTU大小影响单次传输的数据量上限
- 需在连接前调用，连接建立后MTU由协议栈协商决定

**前置条件**

- server_id已通过gatts_register_server()注册成功
- 尚未建立BLE连接

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | 服务端ID | gatts_register_server()分配的有效ID |
| mtu_size | uint16_t | 服务端接收MTU大小 | [SDK_BLE_MTU_MIN(23), SDK_BLE_MTU_MAX(517)] |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | MTU设置成功 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### gatts_register_callbacks <a id="gatts_register_callbacks"></a>

```c
errcode_t gatts_register_callbacks(gatts_callbacks_t *func)
```

**头文件清单**

```c
#include "include/middleware/services/bts/ble/bts_gatt_server.h"
```

**功能说明**

- 注册GATT服务端回调函数，用于接收服务添加、特征添加、读写请求、MTU变更等事件通知
- 回调函数运行于BTS (Bluetooth Subsystem) 线程，不能阻塞或长时间等待
- 回调中内存由BTS协议栈申请和释放，回调中不应释放

**前置条件**

- BLE协议栈已初始化完成
- func不为NULL，指向有效回调函数结构体

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func | [gatts_callbacks_t](#struct_gatts_callbacks_t) * | 回调函数结构体 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 回调注册成功 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

**参考案例**

- [ble_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_server/src/ble_speed_server.c)


## Type definitions

### errcode_t <a id="typedef_errcode_t"></a>

```c
typedef uint32_t errcode_t;
```

**使用说明**

作为GATT Server所有接口的返回值类型，用于表示接口调用的执行结果

## Enumerations

### uuid_type_t <a id="enum_uuid_type_t"></a>

```c
typedef enum {
    UUID_TYPE_NULL = 0x00,
    UUID_TYPE_16_BIT,
    UUID_TYPE_32_BIT,
    UUID_TYPE_128_BIT,
} uuid_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| UUID_TYPE_NULL | 0x00 | 空UUID |
| UUID_TYPE_16_BIT | 0x01 | 16位UUID |
| UUID_TYPE_32_BIT | 0x02 | 32位UUID |
| UUID_TYPE_128_BIT | 0x03 | 128位UUID |

### errcode_bt_t <a id="enum_errcode_bt_t"></a>

```c
typedef enum {
    ERRCODE_BT_SUCCESS = 0x00,
    ERRCODE_BT_FAIL = 0x80006000,
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
    ERRCODE_BT_MAX = 0x800067FF
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

### gatt_characteristic_property_t <a id="enum_gatt_characteristic_property_t"></a>

```c
typedef enum {
    GATT_CHARACTER_PROPERTY_BIT_BROADCAST = 0x01,
    GATT_CHARACTER_PROPERTY_BIT_READ = 0x02,
    GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP = 0x04,
    GATT_CHARACTER_PROPERTY_BIT_WRITE = 0x08,
    GATT_CHARACTER_PROPERTY_BIT_NOTIFY = 0x10,
    GATT_CHARACTER_PROPERTY_BIT_INDICATE = 0x20,
    GATT_CHARACTER_PROPERTY_BIT_SIGNED_WRITE = 0x40,
    GATT_CHARACTER_PROPERTY_BIT_EXTENDED_PROPERTY = 0x80,
} gatt_characteristic_property_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GATT_CHARACTER_PROPERTY_BIT_BROADCAST | 0x01 | 广播特征值 |
| GATT_CHARACTER_PROPERTY_BIT_READ | 0x02 | 读特征值 |
| GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP | 0x04 | 写特征值且不需要响应 |
| GATT_CHARACTER_PROPERTY_BIT_WRITE | 0x08 | 写特征值 |
| GATT_CHARACTER_PROPERTY_BIT_NOTIFY | 0x10 | 通知特征值 |
| GATT_CHARACTER_PROPERTY_BIT_INDICATE | 0x20 | 指示特征值 |
| GATT_CHARACTER_PROPERTY_BIT_SIGNED_WRITE | 0x40 | 签名写特征值 |
| GATT_CHARACTER_PROPERTY_BIT_EXTENDED_PROPERTY | 0x80 | 扩展特征特性 |

### gatt_attribute_permission_t <a id="enum_gatt_attribute_permission_t"></a>

```c
typedef enum {
    GATT_ATTRIBUTE_PERMISSION_READ = 0x01,
    GATT_ATTRIBUTE_PERMISSION_WRITE = 0x02,
    GATT_ATTRIBUTE_PERMISSION_ENCRYPTION_NEED = 0x04,
    GATT_ATTRIBUTE_PERMISSION_AUTHENTICATION_NEED = 0x08,
    GATT_ATTRIBUTE_PERMISSION_AUTHORIZATION_NEED = 0x10,
    GATT_ATTRIBUTE_PERMISSION_MITM_NEED = 0x20,
} gatt_attribute_permission_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GATT_ATTRIBUTE_PERMISSION_READ | 0x01 | 可读 |
| GATT_ATTRIBUTE_PERMISSION_WRITE | 0x02 | 可写 |
| GATT_ATTRIBUTE_PERMISSION_ENCRYPTION_NEED | 0x04 | 需要加密 |
| GATT_ATTRIBUTE_PERMISSION_AUTHENTICATION_NEED | 0x08 | 需要认证 |
| GATT_ATTRIBUTE_PERMISSION_AUTHORIZATION_NEED | 0x10 | 需要授权 |
| GATT_ATTRIBUTE_PERMISSION_MITM_NEED | 0x20 | 需要MITM (Man-In-The-Middle) 保护 |

### gatt_status_t <a id="enum_gatt_status_t"></a>

```c
typedef enum {
    GATT_STATUS_SUCCESS = 0x00,
    GATT_STATUS_INVALID_HANDLE = 0x01,
    GATT_STATUS_READ_NOT_PERMITTED = 0x02,
    GATT_STATUS_WRITE_NOT_PERMITTED = 0x03,
    GATT_STATUS_INVALID_PDU = 0x04,
    GATT_STATUS_INSUFFICIENT_AUTHENTICATION = 0x05,
    GATT_STATUS_REQUEST_NOT_SUPPORTED = 0x06,
    GATT_STATUS_INVALID_OFFSET = 0x07,
    GATT_STATUS_INSUFFICIENT_AUTHORIZATION = 0x08,
    GATT_STATUS_PREPARE_QUEUE_FULL = 0x09,
    GATT_STATUS_ATTRIBUTE_NOT_FOUND = 0x0A,
    GATT_STATUS_ATTRIBUTE_NOT_LONG = 0x0B,
    GATT_STATUS_INSUFFICIENT_ENCRYPTION_KEY_SIZE = 0x0C,
    GATT_STATUS_INVALID_ATTRIBUTE_VALUE_LENGTH = 0x0D,
    GATT_STATUS_UNLIKELY_ERROR = 0x0E,
    GATT_STATUS_INSUFFICIENT_ENCRYPTION = 0x0F,
    GATT_STATUS_UNSUPPORTED_GROUP_TYPE = 0x10,
    GATT_STATUS_INSUFFICIENT_RESOURCES = 0x11,
    GATT_STATUS_DATABASE_OUT_OF_SYNC = 0x12,
    GATT_STATUS_VALUE_NOT_ALLOWED = 0x13,
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
| GATT_STATUS_REQUEST_NOT_SUPPORTED | 0x06 | 不支持客户端请求 |
| GATT_STATUS_INVALID_OFFSET | 0x07 | 无效偏移 |
| GATT_STATUS_INSUFFICIENT_AUTHORIZATION | 0x08 | 未授权 |
| GATT_STATUS_PREPARE_QUEUE_FULL | 0x09 | 准备写队列已满 |
| GATT_STATUS_ATTRIBUTE_NOT_FOUND | 0x0A | 属性未找到 |
| GATT_STATUS_ATTRIBUTE_NOT_LONG | 0x0B | 非长属性，不能使用ATT_READ_BLOB_REQ |
| GATT_STATUS_INSUFFICIENT_ENCRYPTION_KEY_SIZE | 0x0C | 加密密钥大小不足 |
| GATT_STATUS_INVALID_ATTRIBUTE_VALUE_LENGTH | 0x0D | 属性值长度无效 |
| GATT_STATUS_UNLIKELY_ERROR | 0x0E | 不可能的错误 |
| GATT_STATUS_INSUFFICIENT_ENCRYPTION | 0x0F | 加密不足 |
| GATT_STATUS_UNSUPPORTED_GROUP_TYPE | 0x10 | 不支持的分组属性类型 |
| GATT_STATUS_INSUFFICIENT_RESOURCES | 0x11 | 资源不足 |
| GATT_STATUS_DATABASE_OUT_OF_SYNC | 0x12 | 数据库未同步 |
| GATT_STATUS_VALUE_NOT_ALLOWED | 0x13 | 非法的参数值 |

## Structures

### bd_addr_t <a id="struct_bd_addr_t"></a>

```c
typedef struct {
    uint8_t addr[6];
    uint8_t type;
} bd_addr_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| addr | uint8_t[6] | 蓝牙地址 |
| type | uint8_t | 蓝牙地址类型，参考[bt_addr_type](#enum_bt_addr_type) |

### bt_uuid_t <a id="struct_bt_uuid_t"></a>

```c
typedef struct {
    uint8_t uuid_len;
    uint8_t uuid[16];
} bt_uuid_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| uuid_len | uint8_t | UUID长度 |
| uuid | uint8_t[16] | UUID字段 |

### gatts_add_chara_info_t <a id="struct_gatts_add_chara_info_t"></a>

```c
typedef struct {
    bt_uuid_t chara_uuid;
    uint8_t permissions;
    uint8_t properties;
    uint16_t value_len;
    uint8_t *value;
} gatts_add_chara_info_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| chara_uuid | [bt_uuid_t](#struct_bt_uuid_t) | GATT特征UUID |
| permissions | uint8_t | 特征权限，参考[gatt_attribute_permission_t](#enum_gatt_attribute_permission_t) |
| properties | uint8_t | 特征特性，参考[gatt_characteristic_property_t](#enum_gatt_characteristic_property_t) |
| value_len | uint16_t | 响应的数据长度 |
| value | uint8_t * | 响应的数据 |

### gatts_add_desc_info_t <a id="struct_gatts_add_desc_info_t"></a>

```c
typedef struct {
    bt_uuid_t desc_uuid;
    uint8_t permissions;
    uint16_t value_len;
    uint8_t *value;
} gatts_add_desc_info_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| desc_uuid | [bt_uuid_t](#struct_bt_uuid_t) | GATT描述符UUID |
| permissions | uint8_t | 描述符权限，参考[gatt_attribute_permission_t](#enum_gatt_attribute_permission_t) |
| value_len | uint16_t | 响应的数据长度 |
| value | uint8_t * | 响应的数据 |

### gatts_req_read_cb_t <a id="struct_gatts_req_read_cb_t"></a>

```c
typedef struct {
    uint16_t request_id;
    uint16_t handle;
    uint16_t offset;
    bool need_rsp;
    bool need_authorize;
    bool is_long;
} gatts_req_read_cb_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| request_id | uint16_t | 请求ID |
| handle | uint16_t | 请求读的属性句柄 |
| offset | uint16_t | 请求读的字节偏移 |
| need_rsp | bool | 是否需要发送响应 |
| need_authorize | bool | 是否需要授权 |
| is_long | bool | 请求是否是读长特征 |

### gatts_req_write_cb_t <a id="struct_gatts_req_write_cb_t"></a>

```c
typedef struct {
    uint16_t request_id;
    uint16_t handle;
    uint16_t offset;
    bool need_rsp;
    bool need_authorize;
    bool is_prep;
    uint16_t length;
    uint8_t *value;
} gatts_req_write_cb_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| request_id | uint16_t | 请求ID |
| handle | uint16_t | 请求写的属性句柄 |
| offset | uint16_t | 请求写的字节偏移 |
| need_rsp | bool | 是否需要发送响应 |
| need_authorize | bool | 是否需要授权 |
| is_prep | bool | 请求是否是准备写 |
| length | uint16_t | 请求写的数据长度 |
| value | uint8_t * | 请求写的数据 |

### gatts_send_rsp_t <a id="struct_gatts_send_rsp_t"></a>

```c
typedef struct {
    uint16_t request_id;
    uint8_t status;
    uint16_t offset;
    uint16_t value_len;
    uint8_t *value;
} gatts_send_rsp_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| request_id | uint16_t | 请求ID |
| status | uint8_t | 读写结果的状态，参考[gatt_status_t](#enum_gatt_status_t) |
| offset | uint16_t | 属性偏移 |
| value_len | uint16_t | 响应的数据长度 |
| value | uint8_t * | 响应的数据 |

### gatts_ntf_ind_t <a id="struct_gatts_ntf_ind_t"></a>

```c
typedef struct {
    uint16_t attr_handle;
    uint16_t value_len;
    uint8_t *value;
} gatts_ntf_ind_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| attr_handle | uint16_t | 属性句柄 |
| value_len | uint16_t | 通知/指示数据长度 |
| value | uint8_t * | 发送的通知/指示数据 |

### gatts_ntf_ind_by_uuid_t <a id="struct_gatts_ntf_ind_by_uuid_t"></a>

```c
typedef struct {
    bt_uuid_t chara_uuid;
    uint16_t start_handle;
    uint16_t end_handle;
    uint16_t value_len;
    uint8_t *value;
} gatts_ntf_ind_by_uuid_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| chara_uuid | [bt_uuid_t](#struct_bt_uuid_t) | 特征UUID |
| start_handle | uint16_t | 起始句柄 |
| end_handle | uint16_t | 结束句柄 |
| value_len | uint16_t | 通知/指示数据长度 |
| value | uint8_t * | 发送的通知/指示数据 |

### gatts_add_character_result_t <a id="struct_gatts_add_character_result_t"></a>

```c
typedef struct {
    uint16_t handle;
    uint16_t value_handle;
} gatts_add_character_result_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| handle | uint16_t | 特征句柄 |
| value_handle | uint16_t | 特征值句柄 |

### gatts_callbacks_t <a id="struct_gatts_callbacks_t"></a>

```c
typedef struct {
    gatts_add_service_callback add_service_cb;
    gatts_add_characteristic_callback add_characteristic_cb;
    gatts_add_descriptor_callback add_descriptor_cb;
    gatts_start_service_callback start_service_cb;
    gatts_stop_service_callback stop_service_cb;
    gatts_delete_service_callback delete_service_cb;
    gatts_read_request_callback read_request_cb;
    gatts_write_request_callback write_request_cb;
    gatts_mtu_changed_callback mtu_changed_cb;
    gatts_indication_confirm_callback indicate_confirm_cb;
} gatts_callbacks_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| add_service_cb | gatts_add_service_callback | 添加服务回调函数 |
| add_characteristic_cb | gatts_add_characteristic_callback | 添加特征回调函数 |
| add_descriptor_cb | gatts_add_descriptor_callback | 添加描述符回调函数 |
| start_service_cb | gatts_start_service_callback | 启动服务回调函数 |
| stop_service_cb | gatts_stop_service_callback | 停止服务回调函数 |
| delete_service_cb | gatts_delete_service_callback | 删除所有服务回调函数 |
| read_request_cb | gatts_read_request_callback | 收到远端读请求回调函数 |
| write_request_cb | gatts_write_request_callback | 收到远端写请求回调函数 |
| mtu_changed_cb | gatts_mtu_changed_callback | MTU大小更新回调函数 |
| indicate_confirm_cb | gatts_indication_confirm_callback | Indication confirm回调函数 |





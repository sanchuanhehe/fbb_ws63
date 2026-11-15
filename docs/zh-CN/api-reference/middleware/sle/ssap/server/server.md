# SLE SSAP Server

SSAP (SLE Service Access Protocol)Server 端 API。管理 Server 注册/注销、服务/特征/描述符的异步和同步添加、服务启动/全删、发送响应、通知/指示（按句柄和 UUID (Universally Unique Identifier)）以及回调注册。

SSAP 服务模型为四级结构：Server → Service → Property → Descriptor。服务包含一组特征（Property），每个特征可附带描述符（Descriptor）。Server 端通过 Notify/Indicate 主动推送数据给已订阅的 Client 端。

> 前置依赖：SSAP Server 接口依赖 [SLE (SparkLink Low Energy) Connection](../../connection/connection.md) 建立连接后才能进行服务交互。

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ssap_server.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [ssaps_register_server](#ssaps_register_server) | 注册SSAP服务端 |
| [ssaps_unregister_server](#ssaps_unregister_server) | 注销SSAP服务端 |
| [ssaps_add_service](#ssaps_add_service) | 异步添加一个SSAP服务 |
| [ssaps_add_property](#ssaps_add_property) | 异步添加一个SSAP特征 |
| [ssaps_add_descriptor](#ssaps_add_descriptor) | 异步添加一个SSAP特征描述符 |
| [ssaps_add_service_sync](#ssaps_add_service_sync) | 同步添加一个SSAP服务 |
| [ssaps_add_property_sync](#ssaps_add_property_sync) | 同步添加一个SSAP特征 |
| [ssaps_add_descriptor_sync](#ssaps_add_descriptor_sync) | 同步添加一个SSAP特征描述符 |
| [ssaps_start_service](#ssaps_start_service) | 启动一个SSAP服务 |
| [ssaps_delete_all_services](#ssaps_delete_all_services) | 删除所有SSAP服务 |
| [ssaps_send_response](#ssaps_send_response) | 对读/写请求发送响应 |
| [ssaps_notify_indicate](#ssaps_notify_indicate) | 向对端发送通知或指示 |
| [ssaps_notify_indicate_by_uuid](#ssaps_notify_indicate_by_uuid) | 通过UUID向对端发送通知或指示 |
| [ssaps_set_info](#ssaps_set_info) | 连接前设置服务端信息 |
| [ssaps_register_callbacks](#ssaps_register_callbacks) | 注册SSAP服务端回调函数 |

## Functions

### ssaps_register_server <a id="ssaps_register_server"></a>

```c
errcode_t ssaps_register_server(sle_uuid_t *app_uuid, uint8_t *server_id)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ssap_server.h"
```

**功能说明**

- 注册SSAP服务端，分配服务端ID用于后续服务操作
- 调用后通过出参返回分配的server_id，作为后续添加服务、添加特征等操作的标识
- 适用于SLE设备作为服务端角色时，在初始化阶段调用以获取服务端身份

**前置条件**

- 模块初始化状态：SLE协议栈已初始化完成，SLE服务已就绪
- 参数合法性要求：app_uuid不为NULL，server_id不为NULL，且指向的内存空间已分配

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| app_uuid | [sle_uuid_t](#struct_sle_uuid_t) * | 上层应用UUID，用于标识应用身份 | 不为NULL |
| server_id | uint8_t * | 服务端ID输出指针，用于接收分配的服务端ID | 不为NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| server_id | uint8_t * | 填充分配的服务端ID，用于后续服务操作 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 注册成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_server/src/sle_speed_server.c)

### ssaps_unregister_server <a id="ssaps_unregister_server"></a>

```c
errcode_t ssaps_unregister_server(uint8_t server_id)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ssap_server.h"
```

**功能说明**

- 注销SSAP服务端，释放已分配的服务端ID及相关资源
- 注销后该server_id不再有效，后续服务操作不可使用该ID
- 适用于SLE设备不再需要服务端角色时，释放资源

**前置条件**

- 模块初始化状态：已通过ssaps_register_server注册并获得有效的server_id
- 依赖资源状态：该server_id下所有服务已停止或删除

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | 服务端ID | 已通过ssaps_register_server分配的有效ID |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 注销成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_server/src/sle_speed_server.c)

### ssaps_add_service <a id="ssaps_add_service"></a>

```c
errcode_t ssaps_add_service(uint8_t server_id, sle_uuid_t *service_uuid, bool is_primary)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ssap_server.h"
```

**功能说明**

- 异步添加一个SSAP服务，服务句柄通过`ssaps_add_service_callback`回调返回
- 支持指定首要服务或引用服务类型
- 适用于SLE服务端构建服务结构时的异步操作场景

**前置条件**

- 模块初始化状态：已通过ssaps_register_server注册并获得有效的server_id
- 参数合法性要求：service_uuid不为NULL
- 依赖资源状态：已通过ssaps_register_callbacks注册回调函数，add_service_cb已设置

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | 服务端ID | 已通过ssaps_register_server分配的有效ID |
| service_uuid | [sle_uuid_t](#struct_sle_uuid_t) * | 服务UUID | 不为NULL |
| is_primary | bool | 是否为首要服务 | true: 首要服务; false: 引用服务 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 异步添加服务请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

### ssaps_add_property <a id="ssaps_add_property"></a>

```c
errcode_t ssaps_add_property(uint8_t server_id, uint16_t service_handle, ssaps_property_info_t *property)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ssap_server.h"
```

**功能说明**

- 异步添加一个SSAP特征，特征句柄通过`ssaps_add_property_callback`回调返回
- 特征包含UUID、权限、操作指示及初始值
- 适用于SLE服务端在已有服务下添加特征的异步操作场景

**前置条件**

- 模块初始化状态：已通过ssaps_register_server注册并获得有效的server_id，已通过ssaps_add_service或ssaps_add_service_sync添加服务并获得有效的service_handle
- 参数合法性要求：property不为NULL
- 依赖资源状态：已通过ssaps_register_callbacks注册回调函数，add_property_cb已设置

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | 服务端ID | 已通过ssaps_register_server分配的有效ID |
| service_handle | uint16_t | 服务句柄 | 已通过ssaps_add_service或ssaps_add_service_sync获得的有效句柄 |
| property | [ssaps_property_info_t](#struct_ssaps_property_info_t) * | SSAP特征信息 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 异步添加特征请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

### ssaps_add_descriptor <a id="ssaps_add_descriptor"></a>

```c
errcode_t ssaps_add_descriptor(uint8_t server_id, uint16_t service_handle, uint16_t property_handle, ssaps_desc_info_t *descriptor)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ssap_server.h"
```

**功能说明**

- 异步添加一个SSAP特征描述符，描述符注册结果通过`ssaps_add_descriptor_callback`回调返回
- 描述符包含UUID、权限、操作指示、类型及值
- 适用于SLE服务端在已有特征下添加描述符的异步操作场景

**前置条件**

- 模块初始化状态：已通过ssaps_register_server注册并获得有效的server_id，已添加服务并获得有效的service_handle，已添加特征并获得有效的property_handle
- 参数合法性要求：descriptor不为NULL
- 依赖资源状态：已通过ssaps_register_callbacks注册回调函数，add_descriptor_cb已设置

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | 服务端ID | 已通过ssaps_register_server分配的有效ID |
| service_handle | uint16_t | 服务句柄 | 已通过ssaps_add_service或ssaps_add_service_sync获得的有效句柄 |
| property_handle | uint16_t | 特征句柄 | 已通过ssaps_add_property或ssaps_add_property_sync获得的有效句柄 |
| descriptor | [ssaps_desc_info_t](#struct_ssaps_desc_info_t) * | SSAP特征描述符信息 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 异步添加描述符请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

### ssaps_add_service_sync <a id="ssaps_add_service_sync"></a>

```c
errcode_t ssaps_add_service_sync(uint8_t server_id, sle_uuid_t *service_uuid, bool is_primary, uint16_t *handle)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ssap_server.h"
```

**功能说明**

- 同步添加一个SSAP服务，服务句柄通过出参直接返回
- 支持指定首要服务或引用服务类型
- 适用于SLE服务端构建服务结构时需要同步获取服务句柄的场景

**前置条件**

- 模块初始化状态：已通过ssaps_register_server注册并获得有效的server_id
- 参数合法性要求：service_uuid不为NULL，handle不为NULL，且指向的内存空间已分配

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | 服务端ID | 已通过ssaps_register_server分配的有效ID |
| service_uuid | [sle_uuid_t](#struct_sle_uuid_t) * | 服务UUID | 不为NULL |
| is_primary | bool | 是否为首要服务 | true: 首要服务; false: 引用服务 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| handle | uint16_t * | 填充分配的服务句柄，用于后续添加特征和启动服务 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 添加服务成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_server/src/sle_speed_server.c)

### ssaps_add_property_sync <a id="ssaps_add_property_sync"></a>

```c
errcode_t ssaps_add_property_sync(uint8_t server_id, uint16_t service_handle, ssaps_property_info_t *property, uint16_t *handle)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ssap_server.h"
```

**功能说明**

- 同步添加一个SSAP特征，特征句柄通过出参直接返回
- 特征包含UUID、权限、操作指示及初始值
- 适用于SLE服务端在已有服务下同步添加特征并获取特征句柄的场景

**前置条件**

- 模块初始化状态：已通过ssaps_register_server注册并获得有效的server_id，已通过ssaps_add_service_sync添加服务并获得有效的service_handle
- 参数合法性要求：property不为NULL，handle不为NULL，且指向的内存空间已分配

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | 服务端ID | 已通过ssaps_register_server分配的有效ID |
| service_handle | uint16_t | 服务句柄 | 已通过ssaps_add_service_sync获得的有效句柄 |
| property | [ssaps_property_info_t](#struct_ssaps_property_info_t) * | SSAP特征信息 | 不为NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| handle | uint16_t * | 填充分配的特征句柄，用于后续添加描述符和通知/指示 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 添加特征成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_server/src/sle_speed_server.c)

### ssaps_add_descriptor_sync <a id="ssaps_add_descriptor_sync"></a>

```c
errcode_t ssaps_add_descriptor_sync(uint8_t server_id, uint16_t service_handle, uint16_t property_handle, ssaps_desc_info_t *descriptor)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ssap_server.h"
```

**功能说明**

- 同步添加一个SSAP特征描述符
- 描述符包含UUID、权限、操作指示、类型及值
- 适用于SLE服务端在已有特征下同步添加描述符的场景

**前置条件**

- 模块初始化状态：已通过ssaps_register_server注册并获得有效的server_id，已添加服务并获得有效的service_handle，已添加特征并获得有效的property_handle
- 参数合法性要求：descriptor不为NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | 服务端ID | 已通过ssaps_register_server分配的有效ID |
| service_handle | uint16_t | 服务句柄 | 已通过ssaps_add_service_sync获得的有效句柄 |
| property_handle | uint16_t | 特征句柄 | 已通过ssaps_add_property_sync获得的有效句柄 |
| descriptor | [ssaps_desc_info_t](#struct_ssaps_desc_info_t) * | SSAP特征描述符信息 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 添加描述符成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_server/src/sle_speed_server.c)

### ssaps_start_service <a id="ssaps_start_service"></a>

```c
errcode_t ssaps_start_service(uint8_t server_id, uint16_t service_handle)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ssap_server.h"
```

**功能说明**

- 启动一个SSAP服务，服务启动结果通过`ssaps_start_service_callback`回调返回
- 服务启动后对端可发现并访问该服务下的特征
- 适用于SLE服务端完成服务和特征添加后，启动服务对外提供访问

**前置条件**

- 模块初始化状态：已通过ssaps_register_server注册并获得有效的server_id，已添加服务及至少一个特征
- 依赖资源状态：已通过ssaps_register_callbacks注册回调函数，start_service_cb已设置

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | 服务端ID | 已通过ssaps_register_server分配的有效ID |
| service_handle | uint16_t | 服务句柄 | 已通过ssaps_add_service或ssaps_add_service_sync获得的有效句柄 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 启动服务请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_server/src/sle_speed_server.c)

### ssaps_delete_all_services <a id="ssaps_delete_all_services"></a>

```c
errcode_t ssaps_delete_all_services(uint8_t server_id)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ssap_server.h"
```

**功能说明**

- 删除指定服务端下的所有SSAP服务及其包含的特征和描述符
- 删除结果通过`ssaps_delete_all_service_callback`回调返回
- 适用于SLE服务端需要重建服务结构或不再提供服务的场景

**前置条件**

- 模块初始化状态：已通过ssaps_register_server注册并获得有效的server_id
- 依赖资源状态：已通过ssaps_register_callbacks注册回调函数，delete_all_service_cb已设置

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | 服务端ID | 已通过ssaps_register_server分配的有效ID |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 删除请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [oh_sle_srv_ssap_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/oh_sle_srv_ssap_server.c)

### ssaps_send_response <a id="ssaps_send_response"></a>

```c
errcode_t ssaps_send_response(uint8_t server_id, uint16_t conn_id, ssaps_send_rsp_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ssap_server.h"
```

**功能说明**

- 当收到需要用户回复响应的读/写请求时，通过该接口发送响应
- 响应触发的读请求回调：`ssaps_read_request_callback`，写请求回调：`ssaps_write_request_callback`
- 适用于SLE服务端在收到对端读/写请求后需要回复响应的场景

**前置条件**

- 模块初始化状态：已通过ssaps_register_server注册并获得有效的server_id，服务已启动
- 参数合法性要求：param不为NULL
- 依赖资源状态：已收到对端读/写请求回调，且回调中need_rsp为true

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | 服务端ID | 已通过ssaps_register_server分配的有效ID |
| conn_id | uint16_t | 连接ID | 已建立的有效连接ID |
| param | [ssaps_send_rsp_t](#struct_ssaps_send_rsp_t) * | 响应参数 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 发送响应成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [oh_sle_srv_ssap_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/oh_sle_srv_ssap_server.c)

### ssaps_notify_indicate <a id="ssaps_notify_indicate"></a>

```c
errcode_t ssaps_notify_indicate(uint8_t server_id, uint16_t conn_id, ssaps_ntf_ind_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ssap_server.h"
```

**功能说明**

- 向对端设备发送通知或指示，具体发送状态取决于客户端特征配置描述符值：0x0000不允许通知和指示，0x0001允许通知，0x0002允许指示
- conn_id为0xffff时向全部对端发送
- 适用于SLE服务端主动向对端推送数据的场景

**前置条件**

- 模块初始化状态：已通过ssaps_register_server注册并获得有效的server_id，服务已启动
- 参数合法性要求：param不为NULL
- 依赖资源状态：对端已开启对应的通知或指示配置

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | 服务端ID | 已通过ssaps_register_server分配的有效ID |
| conn_id | uint16_t | 连接ID | 已建立的有效连接ID; 0xFFFF: 向全部对端发送 |
| param | [ssaps_ntf_ind_t](#struct_ssaps_ntf_ind_t) * | 通知/指示参数 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 发送通知/指示成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_server/src/sle_speed_server.c)

### ssaps_notify_indicate_by_uuid <a id="ssaps_notify_indicate_by_uuid"></a>

```c
errcode_t ssaps_notify_indicate_by_uuid(uint8_t server_id, uint16_t conn_id, ssaps_ntf_ind_by_uuid_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ssap_server.h"
```

**功能说明**

- 通过UUID向对端设备发送通知或指示，具体发送状态取决于客户端特征配置描述符值：0x0000不允许通知和指示，0x0001允许通知，0x0002允许指示
- conn_id为0xffff时向全部对端发送
- 适用于SLE服务端通过UUID匹配特征并向对端推送数据的场景

**前置条件**

- 模块初始化状态：已通过ssaps_register_server注册并获得有效的server_id，服务已启动
- 参数合法性要求：param不为NULL
- 依赖资源状态：对端已开启对应的通知或指示配置

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | 服务端ID | 已通过ssaps_register_server分配的有效ID |
| conn_id | uint16_t | 连接ID | 已建立的有效连接ID; 0xFFFF: 向全部对端发送 |
| param | [ssaps_ntf_ind_by_uuid_t](#struct_ssaps_ntf_ind_by_uuid_t) * | 通知/指示参数 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 发送通知/指示成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [oh_sle_srv_ssap_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/oh_sle_srv_ssap_server.c)

### ssaps_set_info <a id="ssaps_set_info"></a>

```c
errcode_t ssaps_set_info(uint8_t server_id, ssap_exchange_info_t *info)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ssap_server.h"
```

**功能说明**

- 在连接建立之前设置服务端信息交换参数，包括MTU (Maximum Transmission Unit) 大小和版本
- 设置的MTU大小影响后续连接的数据传输单元长度
- 适用于SLE服务端在连接建立前预配置信息交换参数的场景

**前置条件**

- 模块初始化状态：已通过ssaps_register_server注册并获得有效的server_id
- 参数合法性要求：info不为NULL
- 上下文限制：需在连接建立之前调用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | 服务端ID | 已通过ssaps_register_server分配的有效ID |
| info | [ssap_exchange_info_t](#struct_ssap_exchange_info_t) * | 服务端信息交换参数 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_server/src/sle_speed_server.c)

### ssaps_register_callbacks <a id="ssaps_register_callbacks"></a>

```c
errcode_t ssaps_register_callbacks(ssaps_callbacks_t *func)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ssap_server.h"
```

**功能说明**

- 注册SSAP服务端回调函数集合，包括服务添加、特征添加、描述符添加、服务启动、服务删除、读请求、写请求、MTU变更、指示确认等回调
- 回调函数运行于SLE service线程，不能阻塞或长时间等待
- 适用于SLE服务端初始化阶段注册事件处理回调的场景

**前置条件**

- 模块初始化状态：SLE协议栈已初始化完成
- 参数合法性要求：func不为NULL，且各回调函数指针已正确设置

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func | [ssaps_callbacks_t](#struct_ssaps_callbacks_t) * | 回调函数集合 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 注册成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_server/src/sle_speed_server.c)


## Type definitions

### errcode_t <a id="typedef_errcode_t"></a>

```c
typedef uint32_t errcode_t;
```

**使用说明**

所有SSAP server接口的返回值类型，表示接口执行结果

## Enumerations

### sle_indication_cfm_result_t <a id="enum_sle_indication_cfm_result_t"></a>

```c
typedef enum {
    SLE_INDICATION_CFM_FAIL           = 0x00,    /*!< 指示接收失败 */
    SLE_INDICATION_CFM_SUCESS         = 0x01,    /*!< 指示接收成功 */
} sle_indication_cfm_result_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SLE_INDICATION_CFM_FAIL | 0x00 | 指示接收失败 |
| SLE_INDICATION_CFM_SUCESS | 0x01 | 指示接收成功 |

### ssap_write_type_t <a id="enum_ssap_write_type_t"></a>

```c
typedef enum {
    SSAP_WRITE_NO_RSP           = 0x01, /*!< 只写，且不需要server回复响应 */
    SSAP_WRITE_DEFAULT_WITH_RSP = 0x02, /*!< 只写，且需要server回复响应 */
} ssap_write_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SSAP_WRITE_NO_RSP | 0x01 | 只写，不需要服务端回复响应 |
| SSAP_WRITE_DEFAULT_WITH_RSP | 0x02 | 只写，需要服务端回复响应 |

### ssap_permission_t <a id="enum_ssap_permission_t"></a>

```c
typedef enum {
    SSAP_PERMISSION_READ                = 0x01, /*!< 可读 */
    SSAP_PERMISSION_WRITE               = 0x02, /*!< 可写 */
    SSAP_PERMISSION_ENCRYPTION_NEED     = 0x04, /*!< 需要加密 */
    SSAP_PERMISSION_AUTHENTICATION_NEED = 0x08, /*!< 需要认证 */
    SSAP_PERMISSION_AUTHORIZATION_NEED  = 0x10, /*!< 需要授权 */
} ssap_permission_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SSAP_PERMISSION_READ | 0x01 | 可读 |
| SSAP_PERMISSION_WRITE | 0x02 | 可写 |
| SSAP_PERMISSION_ENCRYPTION_NEED | 0x04 | 需要加密 |
| SSAP_PERMISSION_AUTHENTICATION_NEED | 0x08 | 需要认证 |
| SSAP_PERMISSION_AUTHORIZATION_NEED | 0x10 | 需要授权 |

### ssap_find_type_t <a id="enum_ssap_find_type_t"></a>

```c
typedef enum {
    SSAP_FIND_TYPE_SERVICE_STRUCTURE = 0x00, /*!< 服务结构 */
    SSAP_FIND_TYPE_PRIMARY_SERVICE   = 0x01, /*!< 首要服务 */
    SSAP_FIND_TYPE_REFERENCE_SERVICE = 0x02, /*!< 引用服务 */
    SSAP_FIND_TYPE_PROPERTY          = 0x03, /*!< 属性 */
    SSAP_FIND_TYPE_METHOD            = 0x04, /*!< 方法 */
    SSAP_FIND_TYPE_EVENT             = 0x05, /*!< 事件 */
} ssap_find_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SSAP_FIND_TYPE_SERVICE_STRUCTURE | 0x00 | 服务结构 |
| SSAP_FIND_TYPE_PRIMARY_SERVICE | 0x01 | 首要服务 |
| SSAP_FIND_TYPE_REFERENCE_SERVICE | 0x02 | 引用服务 |
| SSAP_FIND_TYPE_PROPERTY | 0x03 | 属性 |
| SSAP_FIND_TYPE_METHOD | 0x04 | 方法 |
| SSAP_FIND_TYPE_EVENT | 0x05 | 事件 |

### ssap_property_type_t <a id="enum_ssap_property_type_t"></a>

```c
typedef enum {
    SSAP_PROPERTY_TYPE_VALUE             = 0x00, /*!< 特征值 */
    SSAP_DESCRIPTOR_USER_DESCRIPTION     = 0x01, /*!< 属性说明描述符 */
    SSAP_DESCRIPTOR_CLIENT_CONFIGURATION = 0x02, /*!< 客户端配置描述符 */
    SSAP_DESCRIPTOR_SERVER_CONFIGURATION = 0x03, /*!< 服务端配置描述符 */
    SSAP_DESCRIPTOR_PRESENTATION_FORMAT  = 0x04, /*!< 格式描述符 */
    SSAP_DESCRIPTOR_RFU                  = 0x05, /*!< 服务管理保留描述符，0x05 - 0x1F */
    SSAP_DESCRIPTOR_CUSTOM               = 0xFF, /*!< 厂商自定义描述符 */
} ssap_property_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SSAP_PROPERTY_TYPE_VALUE | 0x00 | 特征值 |
| SSAP_DESCRIPTOR_USER_DESCRIPTION | 0x01 | 属性说明描述符 |
| SSAP_DESCRIPTOR_CLIENT_CONFIGURATION | 0x02 | 客户端配置描述符 |
| SSAP_DESCRIPTOR_SERVER_CONFIGURATION | 0x03 | 服务端配置描述符 |
| SSAP_DESCRIPTOR_PRESENTATION_FORMAT | 0x04 | 格式描述符 |
| SSAP_DESCRIPTOR_RFU | 0x05 | 服务管理保留描述符，0x05 - 0x1F |
| SSAP_DESCRIPTOR_CUSTOM | 0xFF | 厂商自定义描述符 |

### ssap_operate_indication_t <a id="enum_ssap_operate_indication_t"></a>

```c
typedef enum {
    SSAP_OPERATE_INDICATION_BIT_READ = 0x01,              /*!< 数据值可被读取 */
    SSAP_OPERATE_INDICATION_BIT_WRITE_NO_RSP = 0x02,      /*!< 数据值可被写入，写入后无反馈 */
    SSAP_OPERATE_INDICATION_BIT_WRITE = 0x04,             /*!< 数据值可被写入，写入后产生反馈给客户端 */
    SSAP_OPERATE_INDICATION_BIT_NOTIFY = 0x08,            /*!< 数据值通过通知方式传递给客户端 */
    SSAP_OPERATE_INDICATION_BIT_INDICATE = 0x10,          /*!< 数据值通过指示方式传递给客户端 */
    SSAP_OPERATE_INDICATION_BIT_BROADCAST = 0x20,         /*!< 数据值可携带在广播中 */
    SSAP_OPERATE_INDICATION_BIT_DESCRITOR_WRITE = 0x100,  /*!< 数据值说明描述符可被写入 */
    SSAP_OPERATE_INDICATION_BIT_DESCRIPTOR_CLIENT_CONFIGURATION_WRITE = 0x200,
                                                           /*!< 客户端描述符可被写入 */
    SSAP_OPERATE_INDICATION_BIT_DESCRIPTOR_SERVER_CONFIGURATION_WRITE = 0x400,
                                                           /*!< 服务端描述符可被写入 */
    SSAP_OPERATE_INDICATION_MAX,                          /*!< 操作指示最大值 */
} ssap_operate_indication_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SSAP_OPERATE_INDICATION_BIT_READ | 0x01 | 数据值可被读取 |
| SSAP_OPERATE_INDICATION_BIT_WRITE_NO_RSP | 0x02 | 数据值可被写入，写入后无反馈 |
| SSAP_OPERATE_INDICATION_BIT_WRITE | 0x04 | 数据值可被写入，写入后产生反馈给客户端 |
| SSAP_OPERATE_INDICATION_BIT_NOTIFY | 0x08 | 数据值通过通知方式传递给客户端 |
| SSAP_OPERATE_INDICATION_BIT_INDICATE | 0x10 | 数据值通过指示方式传递给客户端 |
| SSAP_OPERATE_INDICATION_BIT_BROADCAST | 0x20 | 数据值可携带在广播中 |
| SSAP_OPERATE_INDICATION_BIT_DESCRITOR_WRITE | 0x100 | 数据值说明描述符可被写入 |
| SSAP_OPERATE_INDICATION_BIT_DESCRIPTOR_CLIENT_CONFIGURATION_WRITE | 0x200 | 客户端描述符可被写入 |
| SSAP_OPERATE_INDICATION_BIT_DESCRIPTOR_SERVER_CONFIGURATION_WRITE | 0x400 | 服务端描述符可被写入 |
| SSAP_OPERATE_INDICATION_MAX | 0x401 | 操作指示最大值 |

## Structures

### sle_uuid_t <a id="struct_sle_uuid_t"></a>

```c
typedef struct {
    uint8_t len;                /*!< UUID 长度 */
    uint8_t uuid[SLE_UUID_LEN]; /*!< UUID字段 */
} sle_uuid_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| len | uint8_t | UUID长度 |
| uuid | uint8_t[SLE_UUID_LEN] | UUID字段，SLE_UUID_LEN为16 |

### ssap_exchange_info_t <a id="struct_ssap_exchange_info_t"></a>

```c
typedef struct {
    uint32_t mtu_size; /*!< mtu大小 */
    uint16_t version;  /*!< 版本，预留字段 */
} ssap_exchange_info_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| mtu_size | uint32_t | MTU大小 |
| version | uint16_t | 版本，预留字段 |

### ssaps_property_info_t <a id="struct_ssaps_property_info_t"></a>

```c
typedef struct {
    sle_uuid_t uuid;             /*!< SSAP 特征 UUID */
    uint16_t permissions;        /*!< 特征权限 { ssap_permission_t } */
    uint32_t operate_indication; /*!< 操作指示 { ssap_operate_indication_t } */
    uint16_t value_len;          /*!< 响应的数据长度 */
    uint8_t *value;              /*!< 响应的数据 */
} ssaps_property_info_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| uuid | [sle_uuid_t](#struct_sle_uuid_t) | SSAP特征UUID |
| permissions | uint16_t | 特征权限，参考[ssap_permission_t](#enum_ssap_permission_t) |
| operate_indication | uint32_t | 操作指示，参考[ssap_operate_indication_t](#enum_ssap_operate_indication_t) |
| value_len | uint16_t | 响应的数据长度 |
| value | uint8_t * | 响应的数据 |

### ssaps_desc_info_t <a id="struct_ssaps_desc_info_t"></a>

```c
typedef struct {
    sle_uuid_t uuid;             /*!< SSAP 描述符 UUID */
    uint16_t permissions;        /*!< 特征权限 { ssap_permission_t } */
    uint32_t operate_indication; /*!< 操作指示 { ssap_operate_indication_t } */
    uint8_t type;                /*!< 描述符类型 { ssap_property_type_t } */
    uint16_t value_len;          /*!< 数据长度 */
    uint8_t *value;              /*!< 数据 */
} ssaps_desc_info_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| uuid | [sle_uuid_t](#struct_sle_uuid_t) | SSAP描述符UUID |
| permissions | uint16_t | 特征权限，参考[ssap_permission_t](#enum_ssap_permission_t) |
| operate_indication | uint32_t | 操作指示，参考[ssap_operate_indication_t](#enum_ssap_operate_indication_t) |
| type | uint8_t | 描述符类型，参考[ssap_property_type_t](#enum_ssap_property_type_t) |
| value_len | uint16_t | 数据长度 |
| value | uint8_t * | 数据 |

### ssaps_req_read_cb_t <a id="struct_ssaps_req_read_cb_t"></a>

```c
typedef struct {
    uint16_t request_id;  /*!< 请求id */
    uint16_t handle;      /*!< 请求读的属性句柄 */
    uint8_t type;         /*!< 属性类型 { ssap_property_type_t } */
    bool need_rsp;        /*!< 是否需要发送响应 */
    bool need_authorize;  /*!< 是否授权 */
} ssaps_req_read_cb_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| request_id | uint16_t | 请求ID |
| handle | uint16_t | 请求读的属性句柄 |
| type | uint8_t | 属性类型，参考[ssap_property_type_t](#enum_ssap_property_type_t) |
| need_rsp | bool | 是否需要发送响应 |
| need_authorize | bool | 是否授权 |

### ssaps_req_read_by_uuid_cb_t <a id="struct_ssaps_req_read_by_uuid_cb_t"></a>

```c
typedef struct {
    uint16_t request_id;   /*!< 请求id */
    uint16_t begin_handle; /*!< 请求读的起始属性句柄 */
    uint16_t end_handle;   /*!< 请求读的结束属性句柄 */
    uint8_t type;          /*!< 属性类型 { ssap_property_type_t } */
    sle_uuid_t uuid;       /*!< 属性UUID */
    bool need_rsp;         /*!< 是否需要发送响应 */
    bool need_authorize;   /*!< 是否授权 */
} ssaps_req_read_by_uuid_cb_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| request_id | uint16_t | 请求ID |
| begin_handle | uint16_t | 请求读的起始属性句柄 |
| end_handle | uint16_t | 请求读的结束属性句柄 |
| type | uint8_t | 属性类型，参考[ssap_property_type_t](#enum_ssap_property_type_t) |
| uuid | [sle_uuid_t](#struct_sle_uuid_t) | 属性UUID |
| need_rsp | bool | 是否需要发送响应 |
| need_authorize | bool | 是否授权 |

### ssaps_req_write_cb_t <a id="struct_ssaps_req_write_cb_t"></a>

```c
typedef struct {
    uint16_t request_id;  /*!< 请求id */
    uint16_t handle;      /*!< 请求写的属性句柄 */
    uint8_t type;         /*!< 属性类型 { ssap_property_type_t } */
    bool need_rsp;        /*!< 是否需要发送响应 */
    bool need_authorize;  /*!< 是否授权 */
    uint16_t length;      /*!< 请求写的数据长度 */
    uint8_t *value;       /*!< 请求写的数据 */
} ssaps_req_write_cb_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| request_id | uint16_t | 请求ID |
| handle | uint16_t | 请求写的属性句柄 |
| type | uint8_t | 属性类型，参考[ssap_property_type_t](#enum_ssap_property_type_t) |
| need_rsp | bool | 是否需要发送响应 |
| need_authorize | bool | 是否授权 |
| length | uint16_t | 请求写的数据长度 |
| value | uint8_t * | 请求写的数据 |

### ssaps_send_rsp_t <a id="struct_ssaps_send_rsp_t"></a>

```c
typedef struct {
    uint16_t request_id; /*!< 请求 ID */
    uint8_t status;      /*!< 读写结果的状态, 成功ERRCODE_SLE_SUCCESS 异常参考errcode_sle_ssap_t */
    uint16_t value_len;  /*!< 响应的数据长度 */
    uint8_t *value;      /*!< 响应的数据 */
} ssaps_send_rsp_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| request_id | uint16_t | 请求ID |
| status | uint8_t | 读写结果状态，成功为ERRCODE_SLE_SUCCESS，异常参考errcode_sle_ssap_t |
| value_len | uint16_t | 响应的数据长度 |
| value | uint8_t * | 响应的数据 |

### ssaps_ntf_ind_t <a id="struct_ssaps_ntf_ind_t"></a>

```c
typedef struct {
    uint16_t handle;      /*!< 属性句柄 */
    uint8_t type;         /*!< 属性类型 { ssap_property_type_t } */
    uint16_t value_len;   /*!< 通知/指示数据长度 */
    uint8_t *value;       /*!< 发送的通知/指示数据 */
} ssaps_ntf_ind_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| handle | uint16_t | 属性句柄 |
| type | uint8_t | 属性类型，参考[ssap_property_type_t](#enum_ssap_property_type_t) |
| value_len | uint16_t | 通知/指示数据长度 |
| value | uint8_t * | 发送的通知/指示数据 |

### ssaps_ntf_ind_by_uuid_t <a id="struct_ssaps_ntf_ind_by_uuid_t"></a>

```c
typedef struct {
    sle_uuid_t uuid;       /*!< 特征UUID */
    uint16_t start_handle; /*!< 起始句柄 */
    uint16_t end_handle;   /*!< 结束句柄 */
    uint8_t type;          /*!< 属性类型 { ssap_property_type_t } */
    uint16_t value_len;    /*!< 通知/指示数据长度 */
    uint8_t *value;        /*!< 发送的通知/指示数据 */
} ssaps_ntf_ind_by_uuid_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| uuid | [sle_uuid_t](#struct_sle_uuid_t) | 特征UUID |
| start_handle | uint16_t | 起始句柄 |
| end_handle | uint16_t | 结束句柄 |
| type | uint8_t | 属性类型，参考[ssap_property_type_t](#enum_ssap_property_type_t) |
| value_len | uint16_t | 通知/指示数据长度 |
| value | uint8_t * | 发送的通知/指示数据 |

### ssaps_callbacks_t <a id="struct_ssaps_callbacks_t"></a>

```c
typedef struct {
    ssaps_add_service_callback add_service_cb;                   /*!< 添加服务回调函数 */
    ssaps_add_property_callback add_property_cb;                 /*!< 添加特征回调函数 */
    ssaps_add_descriptor_callback add_descriptor_cb;             /*!< 添加描述符回调函数 */
    ssaps_start_service_callback start_service_cb;               /*!< 启动服务回调函数 */
    ssaps_delete_all_service_callback delete_all_service_cb;     /*!< 删除服务回调函数 */
    ssaps_read_request_callback read_request_cb;                 /*!< 收到远端读请求回调函数 */
    ssaps_read_by_uuid_request_callback read_by_uuid_request_cb; /*!< 收到远端基于UUID读请求回调函数 */
    ssaps_write_request_callback write_request_cb;               /*!< 收到远端写请求回调函数 */
    ssaps_mtu_changed_callback mtu_changed_cb;                   /*!< mtu 大小更新回调函数 */
    ssaps_indicate_cfm_callback indicate_cfm_cb;                 /*!< 指示确认回调函数 */
} ssaps_callbacks_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| add_service_cb | ssaps_add_service_callback | 添加服务回调函数 |
| add_property_cb | ssaps_add_property_callback | 添加特征回调函数 |
| add_descriptor_cb | ssaps_add_descriptor_callback | 添加描述符回调函数 |
| start_service_cb | ssaps_start_service_callback | 启动服务回调函数 |
| delete_all_service_cb | ssaps_delete_all_service_callback | 删除服务回调函数 |
| read_request_cb | ssaps_read_request_callback | 收到远端读请求回调函数 |
| read_by_uuid_request_cb | ssaps_read_by_uuid_request_callback | 收到远端基于UUID读请求回调函数 |
| write_request_cb | ssaps_write_request_callback | 收到远端写请求回调函数 |
| mtu_changed_cb | ssaps_mtu_changed_callback | MTU大小更新回调函数 |
| indicate_cfm_cb | ssaps_indicate_cfm_callback | 指示确认回调函数 |





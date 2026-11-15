# SLE SSAP Client

SSAP (SLE (SparkLink Low Energy) Service Access Protocol) Client 端 API。管理 Client 注册/注销、服务/特征/描述符发现、按 UUID (Universally Unique Identifier) 和句柄读写、Write Request/Write Command 以及客户端回调注册。

Client 端主动发现 Server 端公布的服务结构，根据句柄或 UUID 读写特征值和描述符。发现服务后才能知道有哪些 Property 可以读写或订阅。

> 前置依赖：SSAP Client 接口依赖 [SLE Connection](../../connection/connection.md) 建立连接后才能进行服务发现和交互。

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ssap_client.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [ssapc_register_client](#ssapc_register_client) | 注册SSAP客户端 |
| [ssapc_unregister_client](#ssapc_unregister_client) | 注销SSAP客户端 |
| [ssapc_find_structure](#ssapc_find_structure) | 查找服务、特征、描述符 |
| [ssapc_read_req_by_uuid](#ssapc_read_req_by_uuid) | 发起按照UUID读取请求 |
| [ssapc_read_req](#ssapc_read_req) | 发起按照句柄读取请求 |
| [ssapc_write_req](#ssapc_write_req) | 发起写请求 |
| [ssapc_write_cmd](#ssapc_write_cmd) | 发起写命令 |
| [ssapc_exchange_info_req](#ssapc_exchange_info_req) | 发送交换info请求 |
| [ssapc_register_callbacks](#ssapc_register_callbacks) | 注册SSAP客户端回调函数 |

## Functions

### ssapc_register_client <a id="ssapc_register_client"></a>

```c
errcode_t ssapc_register_client(sle_uuid_t *app_uuid, uint8_t *client_id)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ssap_client.h"
```

**功能说明**

- 注册SSAP客户端，分配客户端ID用于后续服务访问操作
- 调用后通过出参返回分配的client_id，作为后续服务发现、读写等操作的标识
- 适用于SLE设备作为客户端角色时，在初始化阶段调用以获取客户端身份

**前置条件**

- 模块初始化状态：SLE协议栈已初始化完成，SLE服务已就绪
- 参数合法性要求：app_uuid不为NULL，client_id不为NULL，且指向的内存空间已分配

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| app_uuid | [sle_uuid_t](#struct_sle_uuid_t) * | 上层应用UUID，用于标识应用身份 | 不为NULL |
| client_id | uint8_t * | 客户端ID输出指针，用于接收分配的客户端ID | 不为NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| client_id | uint8_t * | 填充分配的客户端ID，用于后续服务操作 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 注册成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [oh_sle_srv_ssap_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/oh_sle_srv_ssap_client.c)

### ssapc_unregister_client <a id="ssapc_unregister_client"></a>

```c
errcode_t ssapc_unregister_client(uint8_t client_id)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ssap_client.h"
```

**功能说明**

- 注销SSAP客户端，释放已分配的客户端ID及相关资源
- 注销后该client_id不再有效，后续服务操作不可使用该ID
- 适用于SLE设备不再需要客户端角色时，释放资源

**前置条件**

- 模块初始化状态：已通过ssapc_register_client注册并获得有效的client_id
- 依赖资源状态：该client_id下无未完成的服务发现或读写操作

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| client_id | uint8_t | 客户端ID | 已通过ssapc_register_client分配的有效ID |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 注销成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [oh_sle_srv_ssap_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/oh_sle_srv_ssap_client.c)

### ssapc_find_structure <a id="ssapc_find_structure"></a>

```c
errcode_t ssapc_find_structure(uint8_t client_id, uint16_t conn_id, ssapc_find_structure_param_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ssap_client.h"
```

**功能说明**

- 向对端设备发起服务/特征/描述符查找请求，支持按类型和UUID过滤
- 查找结果通过ssapc_find_structure_callback和ssapc_find_structure_complete_callback回调异步返回
- 适用于SLE客户端发现对端设备提供的服务结构及特征属性

**前置条件**

- 模块初始化状态：已通过ssapc_register_client注册并获得有效的client_id
- 依赖资源状态：与对端设备已建立SLE连接，conn_id有效
- 参数合法性要求：param不为NULL，param中type、start_hdl、end_hdl设置合法

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| client_id | uint8_t | 客户端ID | 已通过ssapc_register_client分配的有效ID |
| conn_id | uint16_t | 连接ID | 已建立的SLE连接ID |
| param | [ssapc_find_structure_param_t](#struct_ssapc_find_structure_param_t) * | 查找参数 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 查找请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 查找请求发起失败 |

**参考案例**

- [sle_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_client/src/sle_speed_client.c)

### ssapc_read_req_by_uuid <a id="ssapc_read_req_by_uuid"></a>

```c
errcode_t ssapc_read_req_by_uuid(uint8_t client_id, uint16_t conn_id, ssapc_read_req_by_uuid_param_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ssap_client.h"
```

**功能说明**

- 向对端设备发起按照UUID读取请求，读取指定UUID对应的属性值
- 读取结果通过ssapc_read_cfm_callback和ssapc_read_by_uuid_complete_callback回调异步返回
- 适用于SLE客户端按照UUID获取对端设备的特征值或描述符值

**前置条件**

- 模块初始化状态：已通过ssapc_register_client注册并获得有效的client_id
- 依赖资源状态：与对端设备已建立SLE连接，conn_id有效；已完成服务发现，目标UUID已知
- 参数合法性要求：param不为NULL，param中uuid、start_hdl、end_hdl设置合法

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| client_id | uint8_t | 客户端ID | 已通过ssapc_register_client分配的有效ID |
| conn_id | uint16_t | 连接ID | 已建立的SLE连接ID |
| param | [ssapc_read_req_by_uuid_param_t](#struct_ssapc_read_req_by_uuid_param_t) * | 按照UUID读取请求参数 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 读取请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 读取请求发起失败 |

**参考案例**

- [oh_sle_srv_ssap_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/oh_sle_srv_ssap_client.c)

### ssapc_read_req <a id="ssapc_read_req"></a>

```c
errcode_t ssapc_read_req(uint8_t client_id, uint16_t conn_id, uint16_t handle, uint8_t type)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ssap_client.h"
```

**功能说明**

- 向对端设备发起按照句柄读取请求，读取指定handle对应的属性值
- 读取结果通过ssapc_read_cfm_callback回调异步返回
- 适用于SLE客户端按照已知句柄获取对端设备的特征值或描述符值

**前置条件**

- 模块初始化状态：已通过ssapc_register_client注册并获得有效的client_id
- 依赖资源状态：与对端设备已建立SLE连接，conn_id有效；已完成服务发现，目标handle已知
- 参数合法性要求：handle为有效的属性句柄，type为合法的属性类型

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| client_id | uint8_t | 客户端ID | 已通过ssapc_register_client分配的有效ID |
| conn_id | uint16_t | 连接ID | 已建立的SLE连接ID |
| handle | uint16_t | 属性句柄 | 已发现的合法属性句柄 |
| type | uint8_t | 特征类型 | 参考[ssap_property_type_t](#enum_ssap_property_type_t) |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 读取请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 读取请求发起失败 |

**参考案例**

- [sle_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_client/src/sle_speed_client.c)

### ssapc_write_req <a id="ssapc_write_req"></a>

```c
errcode_t ssapc_write_req(uint8_t client_id, uint16_t conn_id, ssapc_write_param_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ssap_client.h"
```

**功能说明**

- 向对端设备发起写请求，写入数据后需要服务端回复响应
- 写结果通过ssapc_write_cfm_callback回调异步返回
- 适用于SLE客户端需要确认对端设备已接收写入数据的场景

**前置条件**

- 模块初始化状态：已通过ssapc_register_client注册并获得有效的client_id
- 依赖资源状态：与对端设备已建立SLE连接，conn_id有效；已完成服务发现，目标handle已知
- 参数合法性要求：param不为NULL，param中handle为有效属性句柄，data不为NULL且data_len不超过MTU (Maximum Transmission Unit) 限制

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| client_id | uint8_t | 客户端ID | 已通过ssapc_register_client分配的有效ID |
| conn_id | uint16_t | 连接ID | 已建立的SLE连接ID |
| param | [ssapc_write_param_t](#struct_ssapc_handle_value_t) * | 写请求参数 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 写请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 写请求发起失败 |

**参考案例**

- [sle_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_client/src/sle_speed_client.c)

### ssapc_write_cmd <a id="ssapc_write_cmd"></a>

```c
errcode_t ssapc_write_cmd(uint8_t client_id, uint16_t conn_id, ssapc_write_param_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ssap_client.h"
```

**功能说明**

- 向对端设备发起写命令，写入数据后不需要服务端回复响应
- 适用于SLE客户端仅需向对端发送数据、无需确认接收结果的场景
- 写命令不触发ssapc_write_cfm_callback回调，发送完成即视为成功

**前置条件**

- 模块初始化状态：已通过ssapc_register_client注册并获得有效的client_id
- 依赖资源状态：与对端设备已建立SLE连接，conn_id有效；已完成服务发现，目标handle已知
- 参数合法性要求：param不为NULL，param中handle为有效属性句柄，data不为NULL且data_len不超过MTU限制

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| client_id | uint8_t | 客户端ID | 已通过ssapc_register_client分配的有效ID |
| conn_id | uint16_t | 连接ID | 已建立的SLE连接ID |
| param | [ssapc_write_param_t](#struct_ssapc_handle_value_t) * | 写命令参数 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 写命令成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 写命令发起失败 |

**参考案例**

- [oh_sle_srv_ssap_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/oh_sle_srv_ssap_client.c)

### ssapc_exchange_info_req <a id="ssapc_exchange_info_req"></a>

```c
errcode_t ssapc_exchange_info_req(uint8_t client_id, uint16_t conn_id, ssap_exchange_info_t* param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ssap_client.h"
```

**功能说明**

- 向对端设备发送交换info请求，协商MTU大小等连接参数
- 交换结果通过ssapc_exchange_info_callback回调异步返回
- 适用于SLE客户端需要调整MTU大小以适配数据传输需求的场景

**前置条件**

- 模块初始化状态：已通过ssapc_register_client注册并获得有效的client_id
- 依赖资源状态：与对端设备已建立SLE连接，conn_id有效
- 参数合法性要求：param不为NULL，param中mtu_size为合法的MTU值

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| client_id | uint8_t | 客户端ID | 已通过ssapc_register_client分配的有效ID |
| conn_id | uint16_t | 连接ID | 已建立的SLE连接ID |
| param | [ssap_exchange_info_t](#struct_ssap_exchange_info_t) * | 客户端info，包含MTU大小和版本 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 交换请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 交换请求发起失败 |

**参考案例**

- [sle_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_client/src/sle_speed_client.c)

### ssapc_register_callbacks <a id="ssapc_register_callbacks"></a>

```c
errcode_t ssapc_register_callbacks(ssapc_callbacks_t *func)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ssap_client.h"
```

**功能说明**

- 注册SSAP客户端回调函数，包括服务发现、属性发现、读写响应、通知/指示等回调
- 回调函数运行于SLE service线程，不能阻塞或长时间等待
- 回调中指针由SLE service申请内存并释放，回调中不应释放

**前置条件**

- 模块初始化状态：SLE协议栈已初始化完成
- 参数合法性要求：func不为NULL，且指向的回调结构体中所需回调函数已赋值

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func | [ssapc_callbacks_t](#struct_ssapc_callbacks_t) * | 回调函数结构体指针 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 注册成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 注册失败 |

**参考案例**

- [sle_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_client/src/sle_speed_client.c)


## Type definitions

### errcode_t <a id="typedef_errcode_t"></a>

```c
typedef uint32_t errcode_t;
```

**使用说明**

所有SSAP客户端接口的返回值类型

## Enumerations

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

### ssap_write_type_t <a id="enum_ssap_write_type_t"></a>

```c
typedef enum {
    SSAP_WRITE_NO_RSP           = 0x01, /*!< 只写，且不需要server回复响应 */
    SSAP_WRITE_DEFAULT_WITH_RSP = 0x02, /*!< 只写，且需要server回复响应 */
} ssap_write_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SSAP_WRITE_NO_RSP | 0x01 | 只写，且不需要server回复响应 |
| SSAP_WRITE_DEFAULT_WITH_RSP | 0x02 | 只写，且需要server回复响应 |

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

### ssap_property_type_t <a id="enum_ssap_property_type_t"></a>

```c
typedef enum {
    SSAP_PROPERTY_TYPE_VALUE             = 0x00, /*!< 特征值 */
    SSAP_DESCRIPTOR_USER_DESCRIPTION     = 0x01, /*!< 属性说明描述符 */
    SSAP_DESCRIPTOR_CLIENT_CONFIGURATION = 0x02, /*!< 客户端配置描述符 */
    SSAP_DESCRIPTOR_SERVER_CONFIGURATION = 0x03, /*!< 服务端配置描述符 */
    SSAP_DESCRIPTOR_PRESENTATION_FORMAT  = 0x04, /*!< 格式描述符 */
    SSAP_DESCRIPTOR_RFU                  = 0x05, /*!< 服务管理保留描述符，0x05 – 0x1F */
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
| SSAP_DESCRIPTOR_RFU | 0x05 | 服务管理保留描述符，0x05 – 0x1F |
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

### ssapc_find_service_result_t <a id="struct_ssapc_find_service_result_t"></a>

```c
typedef struct {
    uint16_t   start_hdl;  /*!< 服务起始句柄 */
    uint16_t   end_hdl;    /*!< 服务结束句柄 */
    sle_uuid_t uuid;       /*!< 服务UUID */
} ssapc_find_service_result_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| start_hdl | uint16_t | 服务起始句柄 |
| end_hdl | uint16_t | 服务结束句柄 |
| uuid | [sle_uuid_t](#struct_sle_uuid_t) | 服务UUID |

### ssapc_find_property_result_t <a id="struct_ssapc_find_property_result_t"></a>

```c
typedef struct {
    uint16_t   handle;                 /*!< 属性句柄 */
    uint32_t   operate_indication;     /*!< 操作指示 */
    sle_uuid_t uuid;                   /*!< UUID标识 */
    uint8_t    descriptors_count;      /*!< 属性描述符类型列表长度 */
    uint8_t    descriptors_type[0];    /*!< 属性描述符类型列表 */
} ssapc_find_property_result_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| handle | uint16_t | 属性句柄 |
| operate_indication | uint32_t | 操作指示，参考[ssap_operate_indication_t](#enum_ssap_operate_indication_t) |
| uuid | [sle_uuid_t](#struct_sle_uuid_t) | UUID标识 |
| descriptors_count | uint8_t | 属性描述符类型列表长度 |
| descriptors_type | uint8_t[0] | 属性描述符类型列表，柔性数组 |

### ssapc_handle_value_t <a id="struct_ssapc_handle_value_t"></a>

```c
typedef struct {
    uint16_t handle;    /*!< 属性句柄 */
    uint8_t  type;      /*!< 属性类型 */
    uint16_t data_len;  /*!< 数据长度 */
    uint8_t  *data;     /*!< 数据内容 */
} ssapc_handle_value_t, ssapc_write_param_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| handle | uint16_t | 属性句柄 |
| type | uint8_t | 属性类型，参考[ssap_property_type_t](#enum_ssap_property_type_t) |
| data_len | uint16_t | 数据长度 |
| data | uint8_t * | 数据内容 |

### ssapc_write_result_t <a id="struct_ssapc_write_result_t"></a>

```c
typedef struct {
    uint16_t handle;    /*!< 属性句柄 */
    uint8_t  type;      /*!< 属性类型 */
    uint16_t data_len;  /*!< 数据长度 */
    uint8_t  *data;     /*!< 数据内容 */
} ssapc_write_result_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| handle | uint16_t | 属性句柄 |
| type | uint8_t | 属性类型，参考[ssap_property_type_t](#enum_ssap_property_type_t) |
| data_len | uint16_t | 数据长度 |
| data | uint8_t * | 数据内容 |

### ssapc_read_by_uuid_cmp_result_t <a id="struct_ssapc_read_by_uuid_cmp_result_t"></a>

```c
typedef struct {
    sle_uuid_t uuid;      /*!< 属性句柄 */
    uint8_t type;         /*!< 属性类型 */
    uint16_t   start_hdl; /*!< 起始句柄 */
    uint16_t   end_hdl;   /*!< 结束句柄 */
} ssapc_read_by_uuid_cmp_result_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| uuid | [sle_uuid_t](#struct_sle_uuid_t) | UUID标识 |
| type | uint8_t | 属性类型，参考[ssap_property_type_t](#enum_ssap_property_type_t) |
| start_hdl | uint16_t | 起始句柄 |
| end_hdl | uint16_t | 结束句柄 |

### ssapc_find_structure_param_t <a id="struct_ssapc_find_structure_param_t"></a>

```c
typedef struct {
    uint8_t    type;      /*!< 查找类型 { ssap_find_type_t } */
    uint16_t   start_hdl; /*!< 起始句柄 */
    uint16_t   end_hdl;   /*!< 结束句柄 */
    sle_uuid_t uuid;      /*!< uuid，按照uuid查找时生效，其余不生效 */
    uint8_t    reserve;   /*!< 预留，默认值写0 */
} ssapc_find_structure_param_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| type | uint8_t | 查找类型，参考[ssap_find_type_t](#enum_ssap_find_type_t) |
| start_hdl | uint16_t | 起始句柄 |
| end_hdl | uint16_t | 结束句柄 |
| uuid | [sle_uuid_t](#struct_sle_uuid_t) | UUID，按照UUID查找时生效，其余不生效 |
| reserve | uint8_t | 预留，默认值写0 |

### ssapc_find_structure_result_t <a id="struct_ssapc_find_structure_result_t"></a>

```c
typedef struct {
    uint8_t    type;      /*!< 查找类型 { ssap_find_type_t } */
    sle_uuid_t uuid;      /*!< uuid */
} ssapc_find_structure_result_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| type | uint8_t | 查找类型，参考[ssap_find_type_t](#enum_ssap_find_type_t) |
| uuid | [sle_uuid_t](#struct_sle_uuid_t) | UUID |

### ssapc_read_req_by_uuid_param_t <a id="struct_ssapc_read_req_by_uuid_param_t"></a>

```c
typedef struct {
    uint8_t    type;        /*!< 查找类型 { ssap_find_type_t } */
    uint16_t   start_hdl;   /*!< 起始句柄 */
    uint16_t   end_hdl;     /*!< 结束句柄 */
    sle_uuid_t uuid;        /*!< uuid */
} ssapc_read_req_by_uuid_param_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| type | uint8_t | 查找类型，参考[ssap_find_type_t](#enum_ssap_find_type_t) |
| start_hdl | uint16_t | 起始句柄 |
| end_hdl | uint16_t | 结束句柄 |
| uuid | [sle_uuid_t](#struct_sle_uuid_t) | UUID |

### ssapc_callbacks_t <a id="struct_ssapc_callbacks_t"></a>

```c
typedef struct {
    ssapc_find_structure_callback find_structure_cb;              /*!< 发现服务回调函数 */
    ssapc_find_property_callback ssapc_find_property_cbk;         /*!< 发现特征回调函数 */
    ssapc_find_structure_complete_callback find_structure_cmp_cb; /*!< 发现特征完成回调函数 */
    ssapc_read_cfm_callback read_cfm_cb;                          /*!< 收到读响应回调函数 */
    ssapc_read_by_uuid_complete_callback read_by_uuid_cmp_cb;     /*!< 读特征值完成回调钩子 */
    ssapc_write_cfm_callback write_cfm_cb;                        /*!< 收到写响应回调函数 */
    ssapc_exchange_info_callback exchange_info_cb;                /*!< 更新mtu大小回调钩子 */
    ssapc_notification_callback notification_cb;                  /*!< 通知事件上报钩子 */
    ssapc_indication_callback indication_cb;                      /*!< 指示事件上报钩子 */
} ssapc_callbacks_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| find_structure_cb | ssapc_find_structure_callback | 发现服务回调函数 |
| ssapc_find_property_cbk | ssapc_find_property_callback | 发现特征回调函数 |
| find_structure_cmp_cb | ssapc_find_structure_complete_callback | 发现特征完成回调函数 |
| read_cfm_cb | ssapc_read_cfm_callback | 收到读响应回调函数 |
| read_by_uuid_cmp_cb | ssapc_read_by_uuid_complete_callback | 读特征值完成回调钩子 |
| write_cfm_cb | ssapc_write_cfm_callback | 收到写响应回调函数 |
| exchange_info_cb | ssapc_exchange_info_callback | 更新MTU大小回调钩子 |
| notification_cb | ssapc_notification_callback | 通知事件上报钩子 |
| indication_cb | ssapc_indication_callback | 指示事件上报钩子 |





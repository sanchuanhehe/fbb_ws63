# SLE OTA

SLE (SparkLink Low Energy)OTA (Over-The-Air) 固件无线升级 API。管理 OTA 服务端初始化/去初始化、切换传输速率、设置加密密钥等功能。

SLE OTA 与系统级 UPG (Upgrade)（固件升级框架）配合使用——SLE OTA 负责无线链路上的固件包传输，UPG 负责固件包的校验写入和升级流程管理。

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ota.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [sle_ota_service_init](#sle_ota_service_init) | SLE OTA服务器初始化 |
| [sle_ota_data_ack](#sle_ota_data_ack) | SLE OTA服务器发送数据ACK (Acknowledgment) |
| [sle_ota_reg_chan_data_report_cbk](#sle_ota_reg_chan_data_report_cbk) | 注册服务端接收OTA升级数据回调 |

## Functions

### sle_ota_service_init <a id="sle_ota_service_init"></a>

```c
errcode_t sle_ota_service_init(uint8_t server_id)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ota.h"
```

**功能说明**

- 初始化SLE OTA服务端，基于指定server_id添加OTA服务及属性
- OTA服务端通过SSAP (SLE Service Access Protocol)服务框架添加服务与属性，完成OTA升级通道的建立
- 依赖sle_ssap_server.h，需在SSAP服务端初始化完成后调用

**前置条件**

- SLE协议栈已初始化完成
- SSAP服务端已通过ssaps_register_server注册，且传入的server_id为已注册的有效服务端标识

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| server_id | uint8_t | SSAP服务端标识 | 已注册的有效服务端ID |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | OTA服务初始化成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| SLE_UPG_ENABLE | 特性宏 | 支持SLE OTA升级功能 | n |

### sle_ota_data_ack <a id="sle_ota_data_ack"></a>

```c
errcode_t sle_ota_data_ack(uint16_t value_len, uint8_t *value)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ota.h"
```

**功能说明**

- SLE OTA服务端向客户端发送数据确认（ACK）响应
- 服务端在接收到OTA升级数据后，通过该接口发送ACK确认，通知客户端数据接收状态
- 依赖sle_ssap_server.h，需在OTA服务初始化完成后调用

**前置条件**

- SLE OTA服务已通过sle_ota_service_init初始化完成
- SLE连接已建立，且OTA服务属性已添加成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| value_len | uint16_t | 发送数据长度 | 大于0 |
| value | uint8_t * | 发送数据指针 | 非NULL，指向内存空间长度不小于value_len |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 数据ACK发送成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| SLE_UPG_ENABLE | 特性宏 | 支持SLE OTA升级功能 | n |

### sle_ota_reg_chan_data_report_cbk <a id="sle_ota_reg_chan_data_report_cbk"></a>

```c
void sle_ota_reg_chan_data_report_cbk(sle_ota_chan_data_report data_report)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_ota.h"
```

**功能说明**

- 注册服务端接收OTA升级数据的回调函数
- 当SLE OTA客户端发送升级数据时，通过注册的回调函数将数据上报给应用层处理
- 需在OTA服务初始化前或初始化后调用，注册回调后方可接收OTA数据

**前置条件**

- SLE OTA服务已初始化或即将初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| data_report | [sle_ota_chan_data_report](#typedef_sle_ota_chan_data_report) | 数据接收回调函数指针 | 非NULL |

## Type definitions

### errcode_t <a id="typedef_errcode_t"></a>

```c
typedef uint32_t errcode_t;
```

**使用说明**

作为sle_ota_service_init、sle_ota_data_ack接口的返回值类型，表示接口执行结果

### sle_ota_chan_data_report <a id="typedef_sle_ota_chan_data_report"></a>

```c
typedef void (*sle_ota_chan_data_report)(const uint8_t *data_ptr, const uint16_t data_len);
```

**使用说明**

作为sle_ota_reg_chan_data_report_cbk接口的入参类型，用于注册OTA数据接收回调函数



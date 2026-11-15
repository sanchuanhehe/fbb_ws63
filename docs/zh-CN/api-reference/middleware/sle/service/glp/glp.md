# SLE GLP

GLP (Generic Layer Protocol) 通用低延迟应用框架。提供标准化低延迟连接的建立、数据传输和连接参数配置。在 SLE (SparkLink Low Energy) 物理层低延迟特性的基础上封装应用层接口。

适用电竞鼠标/键盘、游戏手柄等对延迟敏感（毫秒级）的交互场景。

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_glp_manager.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [sle_glp_register_callbacks](#sle_glp_register_callbacks) | 注册SLE GLP回调函数 |

## Functions

### sle_glp_register_callbacks <a id="sle_glp_register_callbacks"></a>

```c
errcode_t sle_glp_register_callbacks(sle_cs_glp_callbacks_t *func)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_glp_manager.h"
```

**功能说明**

- 注册SLE GLP回调函数，用于接收CS (Carrier Synchronization)GLP上报数据
- 回调函数注册后，当CS GLP上报事件触发时，通过 [sle_cs_glp_report_callback](#typedef_sle_cs_glp_report_callback) 回调上报GLP报告数据
- 需在SLE连接建立后、CS GLP数据上报前完成回调注册

**前置条件**

- SLE协议栈已初始化
- SLE连接已建立

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func | [sle_cs_glp_callbacks_t](#struct_sle_cs_glp_callbacks_t) * | GLP回调函数结构体指针 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SLE_SUCCESS(0) | 执行成功 | 回调注册成功 |
| Other | 其他错误码，参考[errcode_sle_t](#enum_errcode_sle_t) | 执行失败 |

## Type definitions

### sle_cs_glp_report_callback <a id="typedef_sle_cs_glp_report_callback"></a>

```c
typedef void (*sle_cs_glp_report_callback)(uint16_t conn_id,
    sle_cs_glp_report_t *report);
```

**使用说明**

作为 [sle_cs_glp_callbacks_t](#struct_sle_cs_glp_callbacks_t) 结构体成员 `cs_glp_report_cb` 的类型，在 [sle_glp_register_callbacks](#sle_glp_register_callbacks) 接口中注册使用

## Enumerations

### errcode_sle_t <a id="enum_errcode_sle_t"></a>

```c
typedef enum {
    ERRCODE_SLE_SUCCESS = 0,                               /*!< 执行成功错误码 */
    ERRCODE_SLE_CONTINUE = ERRCODE_SLE_COMMON_BASE,        /*!< 继续执行错误码 */
    ERRCODE_SLE_DIRECT_RETURN,                             /*!< 直接返回错误码 */
    ERRCODE_SLE_NO_ATTATION,                               /*!< 错误码 */
    ERRCODE_SLE_PARAM_ERR,                                 /*!< 参数错误错误码 */
    ERRCODE_SLE_FAIL,                                      /*!< 配置失败错误码 */
    ERRCODE_SLE_TIMEOUT,                                   /*!< 配置超时错误码 */
    ERRCODE_SLE_UNSUPPORTED,                               /*!< 参数不支持错误码 */
    ERRCODE_SLE_GETRECORD_FAIL,                            /*!< 获取当前记录失败错误码 */
    ERRCODE_SLE_POINTER_NULL,                              /*!< 指针为空错误码 */
    ERRCODE_SLE_NO_RECORD,                                 /*!< 无记录返回错误码 */
    ERRCODE_SLE_STATUS_ERR,                                /*!< 状态错误错误码 */
    ERRCODE_SLE_NOMEM,                                     /*!< 内存不足错误码 */
    ERRCODE_SLE_AUTH_FAIL,                                 /*!< 认证失败错误码 */
    ERRCODE_SLE_AUTH_PKEY_MISS,                            /*!< PIN码或密钥丢失致认证失败错误码 */
    ERRCODE_SLE_RMT_DEV_DOWN,                              /*!< 对端设备关闭错误码 */
    ERRCODE_SLE_PAIRING_REJECT,                            /*!< 配对拒绝错误码 */
    ERRCODE_SLE_BUSY,                                      /*!< 系统繁忙错误码 */
    ERRCODE_SLE_NOT_READY,                                 /*!< 系统未准备好错误码 */
    ERRCODE_SLE_CONN_FAIL,                                 /*!< 连接失败错误码 */
    ERRCODE_SLE_OUT_OF_RANGE,                              /*!< 越界错误码 */
    ERRCODE_SLE_MEMCPY_FAIL,                               /*!< 拷贝失败错误码 */
    ERRCODE_SLE_MALLOC_FAIL,                               /*!< 内存申请失败错误码 */
    ERRCODE_SLE_MAX = ERRCODE_SLE_COMMON_END               /*!< SLE 错误码最大值 */
} errcode_sle_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| ERRCODE_SLE_SUCCESS | 0 | 执行成功 |
| ERRCODE_SLE_CONTINUE | 0x80006000 | 继续执行 |
| ERRCODE_SLE_DIRECT_RETURN | 0x80006001 | 直接返回 |
| ERRCODE_SLE_NO_ATTATION | 0x80006002 | 无关注 |
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

## Structures

### sle_cs_glp_report_t <a id="struct_sle_cs_glp_report_t"></a>

```c
typedef struct {
    uint8_t status;
    uint16_t con_hdl;
    int32_t  cfo;
} sle_cs_glp_report_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| status | uint8_t | CS GLP上报状态 |
| con_hdl | uint16_t | 连接句柄 |
| cfo | int32_t | 载波频偏（Carrier Frequency Offset） |

### sle_cs_glp_callbacks_t <a id="struct_sle_cs_glp_callbacks_t"></a>

```c
typedef struct {
    sle_cs_glp_report_callback cs_glp_report_cb;
} sle_cs_glp_callbacks_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| cs_glp_report_cb | [sle_cs_glp_report_callback](#typedef_sle_cs_glp_report_callback) | CS GLP上报回调函数 |



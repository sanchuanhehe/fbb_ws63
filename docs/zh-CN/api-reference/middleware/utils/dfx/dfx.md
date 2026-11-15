# DFX/Diag

DFX (Diagnostic & Feedback) 诊断框架。管理诊断命令注册/注销、诊断报文上报（单包/关键多包/普通多包）、系统消息上报和统计对象注册。

提供控制台命令处理和遥测上报框架，是串口调试命令、日志输出和运行时诊断的核心通道。

**头文件清单**

```c
#include "include/middleware/utils/diag.h"
#include "include/middleware/utils/diag_log.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_diag_register_cmd](#uapi_diag_register_cmd) | 注册diag命令处理函数 |
| [uapi_diag_unregister_cmd](#uapi_diag_unregister_cmd) | 解注册diag命令处理函数 |
| [uapi_diag_report_packet](#uapi_diag_report_packet) | 上报单条diag报文给DIAG (Diagnostic) 客户端 |
| [uapi_diag_report_packets_critical](#uapi_diag_report_packets_critical) | 上报多条关键diag报文给DIAG客户端 |
| [uapi_diag_report_packets_normal](#uapi_diag_report_packets_normal) | 上报多条普通diag报文给DIAG客户端 |
| [uapi_diag_report_sys_msg](#uapi_diag_report_sys_msg) | 上报消息给DIAG客户端 |
| [uapi_diag_register_ind](#uapi_diag_register_ind) | 注册diag应答处理函数 |
| [uapi_diag_run_cmd](#uapi_diag_run_cmd) | 根据命令ID执行diag命令处理 |
| [uapi_diag_register_stat_obj](#uapi_diag_register_stat_obj) | 注册统计量对象 |

## Functions

### uapi_diag_register_cmd <a id="uapi_diag_register_cmd"></a>

```c
errcode_t uapi_diag_register_cmd(const diag_cmd_reg_obj_t *cmd_tbl, uint16_t cmd_num)
```

**头文件清单**

```c
#include "include/middleware/utils/diag.h"
```

**功能说明**

- 注册diag命令处理函数，DIAG子系统支持通过该接口注册新的命令
- cmd_tbl须声明为常量数组并传入该参数
- cmd_num不能为0，表示注册的命令条数

**前置条件**

- DIAG模块已初始化完成
- 命令注册表槽位未满（最大槽位数 CONFIG_DIAG_CMD_TBL_NUM = 10）

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| cmd_tbl | const [diag_cmd_reg_obj_t](#struct_diag_cmd_reg_obj_t) * | diag命令注册表 | 不为NULL，须声明为常量数组 |
| cmd_num | uint16_t | 命令条数 | 不为0 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 命令注册成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DFX_SUPPORT_DIAG | 特性宏 | 支持DIAG功能 | y |

### uapi_diag_unregister_cmd <a id="uapi_diag_unregister_cmd"></a>

```c
errcode_t uapi_diag_unregister_cmd(const diag_cmd_reg_obj_t *cmd_tbl, uint16_t cmd_num)
```

**头文件清单**

```c
#include "include/middleware/utils/diag.h"
```

**功能说明**

- 解注册diag命令处理函数，将已注册的命令行表清除
- cmd_tbl须与注册时传入的命令表一致
- cmd_num不能为0

**前置条件**

- 对应命令表已通过 uapi_diag_register_cmd 注册成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| cmd_tbl | const [diag_cmd_reg_obj_t](#struct_diag_cmd_reg_obj_t) * | diag命令注册表 | 不为NULL，须与注册时的表一致 |
| cmd_num | uint16_t | 命令条数 | 不为0，须与注册时的条数一致 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 命令解注册成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DFX_SUPPORT_DIAG | 特性宏 | 支持DIAG功能 | y |

### uapi_diag_report_packet <a id="uapi_diag_report_packet"></a>

```c
errcode_t uapi_diag_report_packet(uint16_t cmd_id, diag_option_t *option, const uint8_t *packet, uint16_t packet_size, bool sync)
```

**头文件清单**

```c
#include "include/middleware/utils/diag.h"
```

**功能说明**

- 上报单条diag报文给DIAG客户端
- option参数用于通知DIAG子系统报文是本地报文还是远端报文
- sync为TRUE时表示同步推送，操作阻塞；sync为FALSE时表示异步推送，操作不阻塞

**前置条件**

- DIAG连接已建立（zdiag_is_enable()为true）
- option不为NULL时，peer_addr字段有效

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| cmd_id | uint16_t | 报文上报ID | 与命令回调函数中的cmd_id一致 |
| option | [diag_option_t](#struct_diag_option_t) * | option选项，识别报文是本地还是远端 | 可为NULL |
| packet | const uint8_t * | 数据包地址 | 不为NULL |
| packet_size | uint16_t | 数据包大小（单位：字节） | 大于0 |
| sync | bool | 上报方式，同步或异步 | true: 同步阻塞; false: 异步非阻塞 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 报文上报成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DFX_SUPPORT_DIAG | 特性宏 | 支持DIAG功能 | y |

### uapi_diag_report_packets_critical <a id="uapi_diag_report_packets_critical"></a>

```c
errcode_t uapi_diag_report_packets_critical(uint16_t cmd_id, diag_option_t *option, uint8_t **packet, uint16_t *packet_size, uint8_t pkt_cnt)
```

**头文件清单**

```c
#include "include/middleware/utils/diag.h"
```

**功能说明**

- 上报多条关键diag报文给DIAG客户端
- 关键报文具有较高优先级，用于关键数据传输场景
- 报文数量受DIAG_PKT_DATA_ID_USR_MAX限制

**前置条件**

- DIAG连接已建立（zdiag_is_enable()为true）
- pkt_cnt不超过DIAG_PKT_DATA_ID_USR_MAX - 1

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| cmd_id | uint16_t | 报文上报ID | 与命令回调函数中的cmd_id一致 |
| option | [diag_option_t](#struct_diag_option_t) * | option选项，识别报文是本地还是远端 | 可为NULL |
| packet | uint8_t ** | 指向数据指针数组的指针 | 不为NULL |
| packet_size | uint16_t * | 指向数据包大小数组的指针 | 不为NULL |
| pkt_cnt | uint8_t | 数据包个数 | 大于0，不超过DIAG_PKT_DATA_ID_USR_MAX - 1 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 关键报文上报成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DFX_SUPPORT_DIAG | 特性宏 | 支持DIAG功能 | y |

### uapi_diag_report_packets_normal <a id="uapi_diag_report_packets_normal"></a>

```c
errcode_t uapi_diag_report_packets_normal(uint16_t cmd_id, diag_option_t *option, uint8_t **packet, uint16_t *packet_size, uint8_t pkt_cnt)
```

**头文件清单**

```c
#include "include/middleware/utils/diag.h"
```

**功能说明**

- 上报多条普通diag报文给DIAG客户端
- 普通报文优先级低于关键报文，用于常规数据传输场景
- 报文数量受DIAG_PKT_DATA_ID_USR_MAX限制

**前置条件**

- DIAG连接已建立（zdiag_is_enable()为true）
- pkt_cnt不超过DIAG_PKT_DATA_ID_USR_MAX - 1

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| cmd_id | uint16_t | 报文上报ID | 与命令回调函数中的cmd_id一致 |
| option | [diag_option_t](#struct_diag_option_t) * | option选项，识别报文是本地还是远端 | 可为NULL |
| packet | uint8_t ** | 指向数据指针数组的指针 | 不为NULL |
| packet_size | uint16_t * | 指向数据包大小数组的指针 | 不为NULL |
| pkt_cnt | uint8_t | 数据包个数 | 大于0，不超过DIAG_PKT_DATA_ID_USR_MAX - 1 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 普通报文上报成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DFX_SUPPORT_DIAG | 特性宏 | 支持DIAG功能 | y |

### uapi_diag_report_sys_msg <a id="uapi_diag_report_sys_msg"></a>

```c
errcode_t uapi_diag_report_sys_msg(uint32_t module_id, uint32_t msg_id, const uint8_t *buf, uint16_t buf_size, uint8_t level)
```

**头文件清单**

```c
#include "include/middleware/utils/diag.h"
```

**功能说明**

- 上报消息给DIAG客户端
- 消息受日志级别过滤，仅当级别与模块ID均通过过滤时才上报
- module_id标识消息来源模块，msg_id标识消息ID

**前置条件**

- DIAG连接已建立，或离线日志已使能
- 日志级别与模块ID通过过滤许可

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| module_id | uint32_t | 模块ID | 由dfx_resource_id定义的模块ID |
| msg_id | uint32_t | 消息ID | 由diag_log_msg_mk_id_xxx宏构造 |
| buf | const uint8_t * | 上报内容 | 不为NULL（buf_size不为0时） |
| buf_size | uint16_t | 内容大小（单位：字节） | 可为0 |
| level | uint8_t | 日志级别 | DIAG_LOG_LEVEL_ALERT(0) ~ DIAG_LOG_LEVEL_TRACE(7) |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 消息上报成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DFX_SUPPORT_DIAG | 特性宏 | 支持DIAG功能 | y |
| CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE | 特性宏 | 支持日志离线存储到本地特性 | n |

### uapi_diag_register_ind <a id="uapi_diag_register_ind"></a>

```c
errcode_t uapi_diag_register_ind(const diag_cmd_reg_obj_t *cmd_tbl, uint16_t cmd_num)
```

**头文件清单**

```c
#include "include/middleware/utils/diag.h"
```

**功能说明**

- 注册diag应答（IND）处理函数
- DIAG子系统支持通过该接口注册应答回调
- cmd_tbl须声明为常量数组并传入该参数

**前置条件**

- DIAG模块已初始化完成
- 应答注册表槽位未满（最大槽位数 CONFIG_DIAG_IND_TBL_NUM = 3）

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| cmd_tbl | const [diag_cmd_reg_obj_t](#struct_diag_cmd_reg_obj_t) * | 注册应答表 | 不为NULL，须声明为常量数组 |
| cmd_num | uint16_t | 应答个数 | 不为0 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 应答注册成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DFX_SUPPORT_DIAG | 特性宏 | 支持DIAG功能 | y |

### uapi_diag_run_cmd <a id="uapi_diag_run_cmd"></a>

```c
errcode_t uapi_diag_run_cmd(uint16_t cmd_id, uint8_t *data, uint16_t data_size, diag_option_t *option)
```

**头文件清单**

```c
#include "include/middleware/utils/diag.h"
```

**功能说明**

- 根据命令ID执行diag命令处理
- option参数用于通知DIAG子系统报文是本地还是远端
- 命令以异步方式发送

**前置条件**

- DIAG模块已初始化完成
- option不为NULL，peer_addr字段有效

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| cmd_id | uint16_t | 命令ID | 有效的diag命令ID |
| data | uint8_t * | 数据内容 | 不为NULL（data_size不为0时） |
| data_size | uint16_t | 数据大小（单位：字节） | 可为0 |
| option | [diag_option_t](#struct_diag_option_t) * | option选项，识别报文是本地还是远端 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 命令发送成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DFX_SUPPORT_DIAG | 特性宏 | 支持DIAG功能 | y |

### uapi_diag_register_stat_obj <a id="uapi_diag_register_stat_obj"></a>

```c
errcode_t uapi_diag_register_stat_obj(const diag_sys_stat_obj_t *stat_obj_tbl, uint16_t obj_num)
```

**头文件清单**

```c
#include "include/middleware/utils/diag.h"
```

**功能说明**

- 注册统计量对象，DIAG子系统支持通过该接口注册新的统计量
- stat_obj_tbl须声明为常量数组并传入该参数
- 注册后统计量可通过DIAG通道查询和上报

**前置条件**

- DIAG模块已初始化完成
- 统计量注册表槽位未满（最大槽位数 CONFIG_STAT_CMD_LIST_NUM = 10）

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| stat_obj_tbl | const [diag_sys_stat_obj_t](#struct_diag_sys_stat_obj_t) * | 统计量注册表 | 不为NULL，须声明为常量数组 |
| obj_num | uint16_t | 统计量个数 | 不为0 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 统计量注册成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DFX_SUPPORT_DIAG | 特性宏 | 支持DIAG功能 | y |
| CONFIG_DFX_STATS | 特性宏 | 支持DFX统计信息功能 | y |

## Type definitions

### diag_addr <a id="typedef_diag_addr"></a>

```c
typedef uint8_t diag_addr;
```

**使用说明**

在 [diag_option_t](#struct_diag_option_t) 中作为 peer_addr 字段的类型使用。

### diag_cmd_f <a id="typedef_diag_cmd_f"></a>

```c
typedef errcode_t (*diag_cmd_f)(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
```

**使用说明**

在 [diag_cmd_reg_obj_t](#struct_diag_cmd_reg_obj_t) 中作为 fn_input_cmd 字段的类型使用，为diag命令处理函数指针。

## Enumerations

### diag_channel_id_t <a id="enum_diag_channel_id_t"></a>

```c
typedef enum {
    DIAG_CHANNEL_ID_0,
    DIAG_CHANNEL_ID_1,
    DIAG_CHANNEL_ID_2,
    DIAG_SUPPORT_CHANNEL_CNT,
    DIAG_CHANNEL_ID_INVALID = 0xFF,
} diag_channel_id_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| DIAG_CHANNEL_ID_0 | 0 | 诊断物理通道ID 0 |
| DIAG_CHANNEL_ID_1 | 1 | 诊断物理通道ID 1 |
| DIAG_CHANNEL_ID_2 | 2 | 诊断物理通道ID 2 |
| DIAG_SUPPORT_CHANNEL_CNT | 3 | 诊断支持的通道数量 |
| DIAG_CHANNEL_ID_INVALID | 0xFF | 无效通道ID |

## Structures

### diag_option_t <a id="struct_diag_option_t"></a>

```c
typedef struct {
    diag_addr peer_addr;        /*!< 地址 */
    uint8_t pad[3];             /*!< 预留字段 */
} diag_option_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| peer_addr | [diag_addr](#typedef_diag_addr) | 对端地址 |
| pad | uint8_t[3] | 预留字段 |

### diag_cmd_reg_obj_t <a id="struct_diag_cmd_reg_obj_t"></a>

```c
typedef struct {
    uint16_t min_id;               /*!< Diag最小命令ID */
    uint16_t max_id;               /*!< Diag最大命令ID */
    diag_cmd_f fn_input_cmd;      /*!< Diag命令处理函数 */
} diag_cmd_reg_obj_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| min_id | uint16_t | Diag最小命令ID |
| max_id | uint16_t | Diag最大命令ID |
| fn_input_cmd | [diag_cmd_f](#typedef_diag_cmd_f) | Diag命令处理函数 |

### diag_sys_stat_obj_t <a id="struct_diag_sys_stat_obj_t"></a>

```c
typedef struct {
    uint16_t id;                 /*!< 统计量ID */
    uint16_t array_cnt;          /*!< 统计量数量 */
    uint32_t stat_packet_size;   /*!< 每个统计量的大小 */
    void *stat_packet;           /*!< 指向统计量的指针 */
} diag_sys_stat_obj_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| id | uint16_t | 统计量ID |
| array_cnt | uint16_t | 统计量数量 |
| stat_packet_size | uint32_t | 每个统计量的大小（单位：字节） |
| stat_packet | void * | 指向统计量的指针 |



# AT 命令

**头文件清单**
```c
#include "include/middleware/utils/at.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_at_cmd_table_register](#uapi_at_cmd_table_register) | 注册AT命令列表 |
| [uapi_at_cmd_abort_register](#uapi_at_cmd_abort_register) | 注册AT命令打断函数 |
| [uapi_at_send_async_result](#uapi_at_send_async_result) | 发送异步阻塞式AT命令执行结果 |
| [uapi_at_interactivity_func_register](#uapi_at_interactivity_func_register) | 注册AT交互命令处理函数 |
| [uapi_at_report](#uapi_at_report) | 向默认通道输出AT打印信息 |
| [uapi_at_print](#uapi_at_print) | 向默认通道输出格式化AT打印信息 |
| [uapi_at_report_to_single_channel](#uapi_at_report_to_single_channel) | 向指定通道输出AT打印信息 |
| [uapi_at_urc_to_channel](#uapi_at_urc_to_channel) | 向指定通道发送主动上报信息 |

## Functions

### uapi_at_cmd_table_register <a id="uapi_at_cmd_table_register"></a>

```c
errcode_t uapi_at_cmd_table_register(const at_cmd_entry_t *table, uint32_t len, uint32_t struct_max_size)
```

**头文件清单** ``include/middleware/utils/at.h``

**功能说明**

- 注册AT命令列表到AT框架，使AT命令可被识别和执行
- 注册时需指定命令列表的起始地址、列表长度以及设置函数输入参数结构体的最大大小
- AT框架根据注册的命令列表解析和分发AT命令

**前置条件**

- AT模块已完成初始化
- 命令列表中各命令名称不可重复

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| table | const [at_cmd_entry_t](#struct_at_cmd_entry_t) * | AT命令列表起始地址 | 非NULL，指向已填充的命令列表 |
| len | uint32_t | AT命令列表长度 | 大于0 |
| struct_max_size | uint32_t | 设置函数输入参数结构体的最大大小 | 大于0 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 注册成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_AT_SUPPORT_CMD_TABLE_CHECK | 特性宏 | 支持AT命令表校验特性 | 由构建目标决定 |
| CONFIG_AT_SUPPORT_QUERY | 特性宏 | 支持AT命令查询类型特性 | 由构建目标决定 |

### uapi_at_cmd_abort_register <a id="uapi_at_cmd_abort_register"></a>

```c
errcode_t uapi_at_cmd_abort_register(at_abort_func_t func, void *arg)
```

**头文件清单** ``include/middleware/utils/at.h``

**功能说明**

- 注册AT命令打断函数，用于在AT命令执行过程中进行打断处理
- 当AT命令设置了AT_FLAG_ABORTABLE标志时，打断函数可被调用
- 打断函数在AT命令被中止时触发

**前置条件**

- AT模块已完成初始化
- CONFIG_AT_SUPPORT_ASYNCHRONOUS已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func | [at_abort_func_t](#typedef_at_abort_func_t) | AT命令打断函数 | 非NULL |
| arg | void * | AT命令打断函数入参 | - |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 注册成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_AT_SUPPORT_ASYNCHRONOUS | 特性宏 | 支持AT异步命令功能 | n |

### uapi_at_send_async_result <a id="uapi_at_send_async_result"></a>

```c
errcode_t uapi_at_send_async_result(uint16_t err)
```

**头文件清单** ``include/middleware/utils/at.h``

**功能说明**

- 发送异步阻塞式AT命令的执行结果
- 成功时输入0，其他值表示失败
- 仅用于异步阻塞式AT命令场景

**前置条件**

- AT模块已完成初始化
- CONFIG_AT_SUPPORT_ASYNCHRONOUS已开启
- 当前正在执行异步阻塞式AT命令

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| err | uint16_t | AT命令执行结果 | 0表示成功，其他值表示失败 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 发送成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_AT_SUPPORT_ASYNCHRONOUS | 特性宏 | 支持AT异步命令功能 | n |

### uapi_at_interactivity_func_register <a id="uapi_at_interactivity_func_register"></a>

```c
errcode_t uapi_at_interactivity_func_register(at_interactivity_func_t func)
```

**头文件清单** ``include/middleware/utils/at.h``

**功能说明**

- 注册AT交互命令处理函数，用于处理AT命令交互过程中的数据上报
- 交互函数在AT命令等待交互数据时被调用
- 交互函数的data参数必须为字符串

**前置条件**

- AT模块已完成初始化
- CONFIG_AT_SUPPORT_ASYNCHRONOUS已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func | [at_interactivity_func_t](#typedef_at_interactivity_func_t) | AT交互命令处理函数 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 注册成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_AT_SUPPORT_ASYNCHRONOUS | 特性宏 | 支持AT异步命令功能 | n |

### uapi_at_report <a id="uapi_at_report"></a>

```c
void uapi_at_report(const char *str)
```

**头文件清单** ``include/middleware/utils/at.h``

**功能说明**

- 向默认AT通道输出打印信息
- 输出信息必须携带字符串结束符
- 用于AT命令响应结果输出

**前置条件**

- AT模块已完成初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| str | const char * | AT打印信息 | 非NULL，必须携带字符串结束符 |

### uapi_at_print <a id="uapi_at_print"></a>

```c
void uapi_at_print(const char *str, ...)
```

**头文件清单** ``include/middleware/utils/at.h``

**功能说明**

- 向默认AT通道输出格式化打印信息
- 支持可变参数格式化输出
- 输出信息必须携带字符串结束符

**前置条件**

- AT模块已完成初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| str | const char * | AT格式化打印信息 | 非NULL，必须携带字符串结束符 |
| ... | - | 可变参数 | - |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_AT_PRINT_BUFFER_SIZE | 特性宏 | 支持AT打印缓冲区大小配置特性 | 384 |

### uapi_at_report_to_single_channel <a id="uapi_at_report_to_single_channel"></a>

```c
void uapi_at_report_to_single_channel(at_channel_id_t channel_id, const char *str)
```

**头文件清单** ``include/middleware/utils/at.h``

**功能说明**

- 向指定AT通道输出打印信息
- 通道号在at_config.h中由产品定义
- 输出信息必须携带字符串结束符

**前置条件**

- AT模块已完成初始化
- 指定通道号已配置

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| channel_id | [at_channel_id_t](#enum_at_channel_id_t) | AT通道号 | AT_UART_PORT(0x00) |
| str | const char * | AT打印信息 | 非NULL，必须携带字符串结束符 |

### uapi_at_urc_to_channel <a id="uapi_at_urc_to_channel"></a>

```c
errcode_t uapi_at_urc_to_channel(at_channel_id_t channel_id, const char *msg, uint32_t msg_len)
```

**头文件清单** ``include/middleware/utils/at.h``

**功能说明**

- 向指定通道发送主动上报（URC）信息
- 通道号在at_config.h中由产品定义
- URC信息用于设备主动向主机端上报事件通知

**前置条件**

- AT模块已完成初始化
- CONFIG_AT_SUPPORT_NOTIFY_REPORT已开启
- 指定通道号已配置

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| channel_id | [at_channel_id_t](#enum_at_channel_id_t) | AT通道号 | AT_UART_PORT(0x00) |
| msg | const char * | 主动上报消息 | 非NULL |
| msg_len | uint32_t | 主动上报消息长度 | 大于0 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 发送成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_AT_SUPPORT_NOTIFY_REPORT | 特性宏 | 支持AT主动上报功能 | n |

## Type definitions

### at_abort_func_t <a id="typedef_at_abort_func_t"></a>
```c
typedef at_ret_t (*at_abort_func_t)(void *arg);
```
AT命令打断函数类型。

**参数**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| arg | void * | AT命令打断函数入参 | 无特殊约束 |

**返回值**

- 返回类型：[at_ret_t](#enum_at_ret_t)
- AT_RET_OK：成功
- 其他：失败

**头文件清单** ``include/middleware/utils/at.h``

### at_interactivity_func_t <a id="typedef_at_interactivity_func_t"></a>
```c
typedef at_ret_t (*at_interactivity_func_t)(const char *data, uint32_t len);
```
AT命令交互函数类型。

**参数**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| data | const char * | AT命令交互函数上报参数，必须为字符串 | 非NULL |
| len | uint32_t | AT命令交互函数上报参数长度 | 大于0 |

**返回值**

- 返回类型：[at_ret_t](#enum_at_ret_t)
- AT_RET_OK：成功
- 其他：失败

**头文件清单** ``include/middleware/utils/at.h``

### at_cmd_func_t <a id="typedef_at_cmd_func_t"></a>
```c
typedef at_ret_t (*at_cmd_func_t)(void);
```
AT命令执行函数类型。

**参数**

- 无

**返回值**

- 返回类型：[at_ret_t](#enum_at_ret_t)
- AT_RET_OK：成功
- 其他：失败

**头文件清单** ``include/middleware/utils/at.h``

### at_set_func_t <a id="typedef_at_set_func_t"></a>
```c
typedef at_ret_t (*at_set_func_t)(const void *arg);
```
AT命令设置函数类型。

**参数**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| arg | const void * | AT命令设置函数入参 | 无特殊约束 |

**返回值**

- 返回类型：[at_ret_t](#enum_at_ret_t)
- AT_RET_OK：成功
- 其他：失败

**头文件清单** ``include/middleware/utils/at.h``

### at_read_func_t <a id="typedef_at_read_func_t"></a>
```c
typedef at_ret_t (*at_read_func_t)(void);
```
AT命令读函数类型。

**参数**

- 无

**返回值**

- 返回类型：[at_ret_t](#enum_at_ret_t)
- AT_RET_OK：成功
- 其他：失败

**头文件清单** ``include/middleware/utils/at.h``

### at_test_func_t <a id="typedef_at_test_func_t"></a>
```c
typedef at_ret_t (*at_test_func_t)(void);
```
AT命令测试函数类型。

**参数**

- 无

**返回值**

- 返回类型：[at_ret_t](#enum_at_ret_t)
- AT_RET_OK：成功
- 其他：失败

**头文件清单** ``include/middleware/utils/at.h``

## Associations

### at_para_parse_syntax_t.entry <a id="assoc_at_para_parse_syntax_t_entry"></a>
```c
union {
    at_token_int_range_t int_range;
    at_token_int_list_t  int_list;
    at_token_string_t string;
    at_token_string_values_t string_list;
    at_token_bit_string_range_t bit_string_range;
    at_token_bit_string_list_t bit_string_list;
    at_token_hex_string_t octet_string;
} entry;
```
AT命令参数校验语法的联合体，根据参数类型选择不同的校验结构。

**成员**

| 名称 | 类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| int_range | [at_token_int_range_t](#struct_at_token_int_range_t) | 基于取值范围的整型校验 | type为AT_SYNTAX_TYPE_INT且attribute为AT_SYNTAX_ATTR_AT_MIN_VALUE或AT_SYNTAX_ATTR_AT_MAX_VALUE时有效 |
| int_list | [at_token_int_list_t](#struct_at_token_int_list_t) | 基于白名单的整型校验 | type为AT_SYNTAX_TYPE_INT且attribute为AT_SYNTAX_ATTR_LIST_VALUE时有效 |
| string | [at_token_string_t](#struct_at_token_string_t) | 基于长度的字符串校验 | type为AT_SYNTAX_TYPE_STRING时有效 |
| string_list | [at_token_string_values_t](#struct_at_token_string_values_t) | 基于白名单的字符串校验 | type为AT_SYNTAX_TYPE_STRING且attribute为AT_SYNTAX_ATTR_LIST_VALUE时有效 |
| bit_string_range | [at_token_bit_string_range_t](#struct_at_token_bit_string_range_t) | 基于范围值的二进制字符串校验 | type为AT_SYNTAX_TYPE_BIT_STRING时有效 |
| bit_string_list | [at_token_bit_string_list_t](#struct_at_token_bit_string_list_t) | 基于白名单的二进制字符串校验 | type为AT_SYNTAX_TYPE_BIT_STRING且attribute为AT_SYNTAX_ATTR_LIST_VALUE时有效 |
| octet_string | [at_token_hex_string_t](#struct_at_token_hex_string_t) | 基于长度的十六进制字符串校验 | type为AT_SYNTAX_TYPE_OCTET_STRING时有效 |

**头文件清单** ``include/middleware/utils/at.h``

## Enumerations

### at_ret_t <a id="enum_at_ret_t"></a>
```c
typedef enum {
    AT_RET_OK = 0,
    AT_RET_SYNTAX_ERROR,
    AT_RET_MALLOC_ERROR,
    AT_RET_MEM_API_ERROR,
    AT_RET_CHANNEL_PARA_ERROR,
    AT_RET_CHANNEL_NOT_INIT,
    AT_RET_CHANNEL_DATA_NULL,
    AT_RET_CMD_PARA_ERROR,
    AT_RET_CMD_FORMAT_ERROR,
    AT_RET_CMD_NO_MATCH,
    AT_RET_CMD_TYPE_ERROR,
    AT_RET_CMD_IN_PROGRESS_BLOCK,
    AT_RET_CMD_ATTR_NOT_ALLOW,
    AT_RET_PROC_CMD_FUNC_MISSING,
    AT_RET_PROC_READ_FUNC_MISSING,
    AT_RET_PROC_TEST_FUNC_MISSING,
    AT_RET_PROC_SET_FUNC_MISSING,
    AT_RET_PROC_WAIT_INTERACTIVITY,
    AT_RET_PROC_ABORT_CURRENT_COMMAND,
    AT_RET_PARSE_PARA_ERROR,
    AT_RET_PARSE_PARA_MISSING_ERROR,
    AT_RET_PROGRESS_BLOCK,
    AT_RET_TIMER_ERROR,
    AT_RET_ABORT_DELAY
} at_ret_t;
```
AT错误码枚举。

**枚举值**

| 名称 | 值 | 详细说明 |
| ---- | ---- | ---- |
| AT_RET_OK | 0 | 成功 |
| AT_RET_SYNTAX_ERROR | 1 | 语法错误 |
| AT_RET_MALLOC_ERROR | 2 | 内存分配错误 |
| AT_RET_MEM_API_ERROR | 3 | 内存API错误 |
| AT_RET_CHANNEL_PARA_ERROR | 4 | 通道参数错误 |
| AT_RET_CHANNEL_NOT_INIT | 5 | 通道未初始化 |
| AT_RET_CHANNEL_DATA_NULL | 6 | 通道数据为空 |
| AT_RET_CMD_PARA_ERROR | 7 | 命令参数错误 |
| AT_RET_CMD_FORMAT_ERROR | 8 | 命令格式错误 |
| AT_RET_CMD_NO_MATCH | 9 | 命令无匹配 |
| AT_RET_CMD_TYPE_ERROR | 10 | 命令类型错误 |
| AT_RET_CMD_IN_PROGRESS_BLOCK | 11 | 命令正在执行中阻塞 |
| AT_RET_CMD_ATTR_NOT_ALLOW | 12 | 命令属性不允许 |
| AT_RET_PROC_CMD_FUNC_MISSING | 13 | 命令执行函数缺失 |
| AT_RET_PROC_READ_FUNC_MISSING | 14 | 读取函数缺失 |
| AT_RET_PROC_TEST_FUNC_MISSING | 15 | 测试函数缺失 |
| AT_RET_PROC_SET_FUNC_MISSING | 16 | 设置函数缺失 |
| AT_RET_PROC_WAIT_INTERACTIVITY | 17 | 等待交互中 |
| AT_RET_PROC_ABORT_CURRENT_COMMAND | 18 | 打断当前命令 |
| AT_RET_PARSE_PARA_ERROR | 19 | 解析参数错误 |
| AT_RET_PARSE_PARA_MISSING_ERROR | 20 | 解析参数缺失 |
| AT_RET_PROGRESS_BLOCK | 21 | 进度阻塞 |
| AT_RET_TIMER_ERROR | 22 | 定时器错误 |
| AT_RET_ABORT_DELAY | 23 | 打断延迟 |

**头文件清单** ``include/middleware/utils/at.h``

### at_cmd_type_t <a id="enum_at_cmd_type_t"></a>
```c
typedef enum {
    AT_CMD_TYPE_CMD = 0x00,
    AT_CMD_TYPE_SET,
    AT_CMD_TYPE_READ,
    AT_CMD_TYPE_TEST,
    AT_CMD_TYPE_ERROR
} at_cmd_type_t;
```
AT命令类型枚举。

**枚举值**

| 名称 | 值 | 详细说明 |
| ---- | ---- | ---- |
| AT_CMD_TYPE_CMD | 0x00 | AT执行命令，如"AT+TEST" |
| AT_CMD_TYPE_SET | 1 | AT设置命令，如"AT+TEST=520" |
| AT_CMD_TYPE_READ | 2 | AT读取命令，如"AT+TEST?" |
| AT_CMD_TYPE_TEST | 3 | AT测试命令，如"AT+TEST=?" |
| AT_CMD_TYPE_ERROR | 4 | AT命令类型错误 |

**头文件清单** ``include/middleware/utils/at.h``

### at_syntax_type_t <a id="enum_at_syntax_type_t"></a>
```c
typedef enum {
    AT_SYNTAX_TYPE_INT,
    AT_SYNTAX_TYPE_STRING,
    AT_SYNTAX_TYPE_BIT_STRING,
    AT_SYNTAX_TYPE_OCTET_STRING,
    AT_SYNTAX_TYPE_NUM
} at_syntax_type_t;
```
AT命令参数类型枚举。

**枚举值**

| 名称 | 值 | 详细说明 |
| ---- | ---- | ---- |
| AT_SYNTAX_TYPE_INT | 0 | 整型参数 |
| AT_SYNTAX_TYPE_STRING | 1 | 字符串参数 |
| AT_SYNTAX_TYPE_BIT_STRING | 2 | 二进制字符串参数 |
| AT_SYNTAX_TYPE_OCTET_STRING | 3 | 十六进制字符串参数 |
| AT_SYNTAX_TYPE_NUM | 4 | 参数类型数量，不作为有效类型使用 |

**头文件清单** ``include/middleware/utils/at.h``

### at_syntax_attribute_t <a id="enum_at_syntax_attribute_t"></a>
```c
typedef enum {
    AT_SYNTAX_ATTR_NOT_SUPPORTED    = 0x0001,
    AT_SYNTAX_ATTR_OPTIONAL         = 0x0002,
    AT_SYNTAX_ATTR_AT_MIN_VALUE     = 0x0004,
    AT_SYNTAX_ATTR_AT_MAX_VALUE     = 0x0008,
    AT_SYNTAX_ATTR_LIST_VALUE       = 0x0010,
    AT_SYNTAX_ATTR_MAX_LENGTH       = 0x0020,
    AT_SYNTAX_ATTR_ADD_LENGTH       = 0x0040,
    AT_SYNTAX_ATTR_FIX_CASE         = 0x0080,
    AT_SYNTAX_ATTR_LENGTH_FIELD     = 0x0100
} at_syntax_attribute_t;
```
AT命令参数校验属性枚举。

**枚举值**

| 名称 | 值 | 详细说明 |
| ---- | ---- | ---- |
| AT_SYNTAX_ATTR_NOT_SUPPORTED | 0x0001 | 标识该参数当前不支持 |
| AT_SYNTAX_ATTR_OPTIONAL | 0x0002 | 标识该参数可缺省，为保证参数顺序，缺省时','不可缺省 |
| AT_SYNTAX_ATTR_AT_MIN_VALUE | 0x0004 | 校验方式为最小值校验 |
| AT_SYNTAX_ATTR_AT_MAX_VALUE | 0x0008 | 校验方式为最大值校验 |
| AT_SYNTAX_ATTR_LIST_VALUE | 0x0010 | 校验方式为白名单校验 |
| AT_SYNTAX_ATTR_MAX_LENGTH | 0x0020 | 校验方式为长度校验 |
| AT_SYNTAX_ATTR_ADD_LENGTH | 0x0040 | 为该参数新增此参数的长度字段 |
| AT_SYNTAX_ATTR_FIX_CASE | 0x0080 | 标识字符串支持大小写混合 |
| AT_SYNTAX_ATTR_LENGTH_FIELD | 0x0100 | 标识该参数已预设长度字段 |

**头文件清单** ``include/middleware/utils/at.h``

### at_channel_id_t <a id="enum_at_channel_id_t"></a>
```c
typedef enum {
    AT_UART_PORT = 0x00,
    AT_MAX_PORT_NUMBER
} at_channel_id_t;
```
AT命令通道号枚举。

**枚举值**

| 名称 | 值 | 详细说明 |
| ---- | ---- | ---- |
| AT_UART_PORT | 0x00 | UART (Universal Asynchronous Receiver/Transmitter) 通道 |
| AT_MAX_PORT_NUMBER | 1 | 最大通道数，不作为有效通道号使用 |

**头文件清单** ``include/middleware/utils/at_config.h``

**Kconfig配置**

- CONFIG_AT_SUPPORT_ZDIAG：启用后增加AT_ZDIAG_PORT通道，默认n

## Structures

### at_token_int_range_t <a id="struct_at_token_int_range_t"></a>
```c
typedef struct {
    int32_t min_val;
    int32_t max_val;
} at_token_int_range_t;
```
基于取值范围的整型校验语法。

**成员**

| 名称 | 类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| min_val | int32_t | 最小值 | 无特殊约束 |
| max_val | int32_t | 最大值 | 大于等于min_val |

**头文件清单** ``include/middleware/utils/at.h``

### at_token_int_list_t <a id="struct_at_token_int_list_t"></a>
```c
typedef struct {
    uint32_t num;
    const int32_t *values;
} at_token_int_list_t;
```
基于白名单的整型校验语法。

**成员**

| 名称 | 类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| num | uint32_t | 白名单中值的数量 | 大于0 |
| values | const int32_t * | 白名单值数组指针 | 非NULL |

**头文件清单** ``include/middleware/utils/at.h``

### at_token_string_t <a id="struct_at_token_string_t"></a>
```c
typedef struct {
    uint32_t max_length;
} at_token_string_t;
```
基于长度的字符串校验语法。

**成员**

| 名称 | 类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| max_length | uint32_t | 字符串最大长度 | 大于0 |

**头文件清单** ``include/middleware/utils/at.h``

### at_token_string_values_t <a id="struct_at_token_string_values_t"></a>
```c
typedef struct {
    uint32_t num;
    const uint8_t * const *values;
} at_token_string_values_t;
```
基于白名单的字符串校验语法。

**成员**

| 名称 | 类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| num | uint32_t | 白名单中字符串的数量 | 大于0 |
| values | const uint8_t * const * | 白名单字符串指针数组 | 非NULL |

**头文件清单** ``include/middleware/utils/at.h``

### at_token_bit_string_range_t <a id="struct_at_token_bit_string_range_t"></a>
```c
typedef struct {
    uint32_t max_value;
} at_token_bit_string_range_t;
```
基于范围值的二进制字符串校验语法。

**成员**

| 名称 | 类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| max_value | uint32_t | 最大值 | 大于0 |

**头文件清单** ``include/middleware/utils/at.h``

### at_token_bit_string_list_t <a id="struct_at_token_bit_string_list_t"></a>
```c
typedef struct {
    uint32_t num;
    const uint32_t *values;
} at_token_bit_string_list_t;
```
基于白名单的二进制字符串校验语法。

**成员**

| 名称 | 类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| num | uint32_t | 白名单中值的数量 | 大于0 |
| values | const uint32_t * | 白名单值数组指针 | 非NULL |

**头文件清单** ``include/middleware/utils/at.h``

### at_token_hex_string_t <a id="struct_at_token_hex_string_t"></a>
```c
typedef struct {
    uint32_t length_field_offset;
    uint32_t max_length;
} at_token_hex_string_t;
```
基于长度的十六进制字符串校验语法。

**成员**

| 名称 | 类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| length_field_offset | uint32_t | 记录标识长度字段的偏移，标识长度字段用来存储十六进制字符串转换后的数据长度 | 大于等于0 |
| max_length | uint32_t | 十六进制字符串最大长度 | 大于0 |

**头文件清单** ``include/middleware/utils/at.h``

### at_para_parse_syntax_t <a id="struct_at_para_parse_syntax_t"></a>
```c
typedef struct {
    uint32_t type : 4;
    uint32_t last : 1;
    uint32_t attribute : 12;
    uint32_t offset : 15;
    union {
        at_token_int_range_t int_range;
        at_token_int_list_t  int_list;
        at_token_string_t string;
        at_token_string_values_t string_list;
        at_token_bit_string_range_t bit_string_range;
        at_token_bit_string_list_t bit_string_list;
        at_token_hex_string_t octet_string;
    } entry;
} at_para_parse_syntax_t;
```
AT命令某个参数校验结构。

**成员**

| 名称 | 类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| type | uint32_t : 4 | 参数类型（[at_syntax_type_t](#enum_at_syntax_type_t)） | AT_SYNTAX_TYPE_INT ~ AT_SYNTAX_TYPE_OCTET_STRING |
| last | uint32_t : 1 | 标识是否为最后一个参数 | 0：否，1：是 |
| attribute | uint32_t : 12 | 参数校验属性（[at_syntax_attribute_t](#enum_at_syntax_attribute_t)） | 有效的属性组合值 |
| offset | uint32_t : 15 | 参数在para blob中的偏移 | 大于等于0 |
| entry | union | 参数校验语法联合体，参考 [at_para_parse_syntax_t.entry](#assoc_at_para_parse_syntax_t_entry) | 根据type选择对应成员 |

**头文件清单** ``include/middleware/utils/at.h``

### at_cmd_entry_t <a id="struct_at_cmd_entry_t"></a>
```c
typedef struct {
    const char *name;
    const uint16_t cmd_id;
    const uint16_t attribute;
    const at_para_parse_syntax_t *syntax;
    at_cmd_func_t cmd;
    at_set_func_t set;
    at_read_func_t read;
    at_test_func_t test;
} at_cmd_entry_t;
```
AT命令实体。

**成员**

| 名称 | 类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| name | const char * | AT命令名称，不可重复 | 非NULL，最大长度AT_CMD_NAME_MAX_LENGTH |
| cmd_id | const uint16_t | AT命令ID，不可重复 | 有效的命令ID |
| attribute | const uint16_t | AT命令属性 | AT_FLAG_NONE、AT_FLAG_ABORTABLE、AT_FLAG_NOT_BLOCK_URC等组合 |
| syntax | const [at_para_parse_syntax_t](#struct_at_para_parse_syntax_t) * | 参数校验语法数组指针 | NULL表示无参数 |
| cmd | [at_cmd_func_t](#typedef_at_cmd_func_t) | AT执行命令处理函数 | 可为NULL |
| set | [at_set_func_t](#typedef_at_set_func_t) | AT设置命令处理函数 | 可为NULL |
| read | [at_read_func_t](#typedef_at_read_func_t) | AT读取命令处理函数 | 可为NULL |
| test | [at_test_func_t](#typedef_at_test_func_t) | AT测试命令处理函数 | 可为NULL |

**头文件清单** ``include/middleware/utils/at.h``


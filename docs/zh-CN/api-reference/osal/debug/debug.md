# Debug

内核调试与诊断接口。提供格式化日志输出（`osal_printk`）、内核 Panic（`osal_panic`，触发系统重启并输出栈回溯）、调用栈转储（`osal_dump_stack`）以及断言宏（`OSAL_ASSERT` / `OSAL_BUG_ON`）。

适用于开发调试阶段的异常定位和运行时错误捕获。

**头文件清单**

```c
#include "kernel/osal/include/debug/osal_debug.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [osal_printk](#osal_printk) | 日志打印函数，支持格式化输出 |
| [osal_panic](#osal_panic) | 内核panic函数，打印panic信息后系统停止响应 |
| [osal_dump_stack](#osal_dump_stack) | 内核回溯函数，打印当前任务调用栈信息 |
| [osal_bug_on](#osal_bug_on) | 条件panic函数，条件为真时触发系统异常 |
| [osal_flush_cache](#osal_flush_cache) | 刷新CPU DCache (Data Cache)|

## Functions

### osal_printk <a id="osal_printk"></a>

```c
void osal_printk(const char *fmt, ...)
```

**头文件清单**

```c
#include "kernel/osal/include/debug/osal_debug.h"
```

**功能说明**

- 提供内核日志打印功能，支持可变参数格式化输出
- 支持linux、liteos、seliteos、freertos系统
- 当fmt为NULL时，函数直接返回，不执行打印操作

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| fmt | const char * | 格式化字符串指针 | 非NULL，指向有效的格式化字符串 |
| ... | ... | 可变参数列表 | 与fmt中的格式说明符匹配 |

**参考案例**

- [ble_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_client/src/ble_speed_client.c)

### osal_panic <a id="osal_panic"></a>

```c
void osal_panic(const char *fmt, const char *fun, int line, const char *cond)
```

**头文件清单**

```c
#include "kernel/osal/include/debug/osal_debug.h"
```

**功能说明**

- 内核panic函数，打印内核panic信息及调用栈后系统停止响应
- 支持linux、liteos系统
- 调用后系统将无法继续运行，属于致命错误处理接口

**前置条件**

- 仅在linux、liteos系统下可用
- 仅在发生不可恢复的致命错误时调用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| fmt | const char * | 格式化字符串指针 | 非NULL |
| fun | const char * | 发生panic的函数名 | 非NULL |
| line | int | 发生panic的行号 | 有效源码行号 |
| cond | const char * | 触发panic的条件表达式 | 非NULL |

### osal_dump_stack <a id="osal_dump_stack"></a>

```c
void osal_dump_stack(void)
```

**头文件清单**

```c
#include "kernel/osal/include/debug/osal_debug.h"
```

**功能说明**

- 内核回溯函数，打印当前运行任务的调用栈信息
- 支持linux、liteos系统
- 用于调试定位问题时的调用链追踪

**前置条件**

- 仅在linux、liteos系统下可用

### osal_bug_on <a id="osal_bug_on"></a>

```c
void osal_bug_on(unsigned char condition)
```

**头文件清单**

```c
#include "kernel/osal/include/debug/osal_debug.h"
```

**功能说明**

- 条件panic函数，当条件为真时触发系统异常
- 支持nonos系统
- 内部通过向地址0写入触发异常指令，使系统进入异常状态
- 使用内部标志位防止重复触发异常

**前置条件**

- 仅在nonos系统下可用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| condition | unsigned char | 条件判断值 | 0：不触发异常；非0：触发系统异常 |

### osal_flush_cache <a id="osal_flush_cache"></a>

```c
void osal_flush_cache(void)
```

**头文件清单**

```c
#include "kernel/osal/include/debug/osal_debug.h"
```

**功能说明**

- 刷新CPU DCache，将数据缓存写回内存
- 支持liteos系统
- 在DMA (Direct Memory Access) 传输等需要Cache一致性操作的场景下调用

**前置条件**

- 仅在liteos系统下可用




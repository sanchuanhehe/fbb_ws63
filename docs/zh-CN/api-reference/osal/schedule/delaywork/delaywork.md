# OSAL Delayed Work

OSAL (Operating System Abstraction Layer) Delayed Work 提供延迟工作项机制，用于在指定延迟时间后调度执行工作任务。支持创建/销毁延迟工作项、调度、取消与刷新等操作，适用于定时回调、周期性任务等场景。

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_delaywork.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [osal_delayedwork_init](#osal_delayedwork_init) | 初始化延迟工作对象 |
| [osal_delayedwork_destroy](#osal_delayedwork_destroy) | 销毁延迟工作对象 |
| [osal_delayedwork_schedule](#osal_delayedwork_schedule) | 将延迟工作提交到全局工作队列 |
| [osal_delayedwork_cancel_sync](#osal_delayedwork_cancel_sync) | 同步取消延迟工作并等待其完成 |

## Functions

### osal_delayedwork_init <a id="osal_delayedwork_init"></a>

```c
int osal_delayedwork_init(osal_delayedwork *work, osal_delayedwork_handler handler)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_delaywork.h"
```

**功能说明**

- 初始化延迟工作对象，绑定回调处理函数
- 初始化后的延迟工作对象可通过 `osal_delayedwork_schedule` 提交到全局工作队列
- 初始化后的延迟工作对象必须通过 `osal_delayedwork_destroy` 释放

**前置条件**

- 调用前需确保 work 指针指向的内存空间已合法分配
- 该接口仅在 Linux 内核模式下可用（`__linux__` 且 `__KERNEL__` 宏已定义）

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| work | [osal_delayedwork](#struct_osal_delayedwork_) * | 待初始化的延迟工作对象 | 非NULL |
| handler | [osal_delayedwork_handler](#typedef_osal_delayedwork_handler) | 延迟工作回调处理函数 | 非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 初始化成功 | 延迟工作对象初始化完成 |
| OSAL_FAILURE(-1) | 初始化失败 | 延迟工作对象初始化失败 |

### osal_delayedwork_destroy <a id="osal_delayedwork_destroy"></a>

```c
void osal_delayedwork_destroy(osal_delayedwork *work)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_delaywork.h"
```

**功能说明**

- 销毁延迟工作对象，释放相关资源
- work 必须由 `osal_delayedwork_init` 初始化获得
- 该接口可能释放内存，需确保 work 来源合法

**前置条件**

- work 指针指向的延迟工作对象已通过 `osal_delayedwork_init` 初始化成功
- 该接口仅在 Linux 内核模式下可用（`__linux__` 且 `__KERNEL__` 宏已定义）

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| work | [osal_delayedwork](#struct_osal_delayedwork_) * | 待销毁的延迟工作对象 | 非NULL，须由 `osal_delayedwork_init` 初始化 |

### osal_delayedwork_schedule <a id="osal_delayedwork_schedule"></a>

```c
int osal_delayedwork_schedule(osal_delayedwork *work, int timeout)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_delaywork.h"
```

**功能说明**

- 将延迟工作提交到内核全局工作队列，在指定延迟时间后执行
- timeout 为 0 时表示立即执行
- 延迟时间到达后，系统在全局工作队列中执行绑定的回调处理函数

**前置条件**

- work 指针指向的延迟工作对象已通过 `osal_delayedwork_init` 初始化成功
- 该接口仅在 Linux 内核模式下可用（`__linux__` 且 `__KERNEL__` 宏已定义）

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| work | [osal_delayedwork](#struct_osal_delayedwork_) * | 待提交的延迟工作对象 | 非NULL，须已初始化 |
| timeout | int | 延迟等待的 jiffies 数 | 0表示立即执行，>0表示延迟执行的 jiffies 数 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 提交成功 | 延迟工作成功提交到全局工作队列 |
| OSAL_FAILURE(-1) | 提交失败 | 延迟工作提交到全局工作队列失败 |

### osal_delayedwork_cancel_sync <a id="osal_delayedwork_cancel_sync"></a>

```c
int osal_delayedwork_cancel_sync(osal_delayedwork *work)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_delaywork.h"
```

**功能说明**

- 取消延迟工作并同步等待其执行完成
- 若延迟工作正在执行中，该接口将阻塞等待直到工作执行完成
- 若延迟工作尚未开始执行，该接口将直接取消该工作

**前置条件**

- work 指针指向的延迟工作对象已通过 `osal_delayedwork_init` 初始化成功
- 该接口仅在 Linux 内核模式下可用（`__linux__` 且 `__KERNEL__` 宏已定义）

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| work | [osal_delayedwork](#struct_osal_delayedwork_) * | 待取消的延迟工作对象 | 非NULL，须已初始化 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 取消成功 | 延迟工作取消成功 |
| OSAL_FAILURE(-1) | 取消失败 | 延迟工作取消失败 |

## Type definitions

### osal_delayedwork_handler <a id="typedef_osal_delayedwork_handler"></a>

```c
typedef void (*osal_delayedwork_handler)(osal_delayedwork *delayedwork);
```

**使用说明**

作为 `osal_delayedwork_init` 的 handler 参数类型，定义延迟工作回调处理函数的原型。

## Structures

### osal_delayedwork_ <a id="struct_osal_delayedwork_"></a>

```c
typedef struct osal_delayedwork_ {
    void *work;
    void (*handler)(struct osal_delayedwork_ *delayedwork);
} osal_delayedwork;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| work | void * | 延迟工作底层实现句柄 |
| handler | void (*)(struct osal_delayedwork_ *delayedwork) | 延迟工作回调处理函数指针 |



# OSAL Wait

OSAL (Operating System Abstraction Layer) Wait 提供等待队列机制，用于线程在指定条件满足前阻塞等待，并在条件变化时被唤醒。支持等待队列的初始化、条件等待、唤醒等操作，适用于事件驱动与条件同步场景。

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_wait.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [osal_wait_init](#osal_wait_init) | 初始化等待队列 |
| [osal_wait_interruptible](#osal_wait_interruptible) | 可中断方式等待条件为真 |
| [osal_wait_uninterruptible](#osal_wait_uninterruptible) | 不可中断方式等待条件为真 |
| [osal_wait_timeout_interruptible](#osal_wait_timeout_interruptible) | 可中断方式等待条件为真或超时 |
| [osal_wait_timeout_uninterruptible](#osal_wait_timeout_uninterruptible) | 不可中断方式等待条件为真或超时 |
| [osal_wait_wakeup](#osal_wait_wakeup) | 唤醒等待队列上的所有阻塞线程 |
| [osal_wait_wakeup_interruptible](#osal_wait_wakeup_interruptible) | 唤醒等待队列上的可中断阻塞线程 |
| [osal_wait_destroy](#osal_wait_destroy) | 销毁等待队列 |

## Functions

### osal_wait_init <a id="osal_wait_init"></a>

```c
int osal_wait_init(osal_wait *wait)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_wait.h"
```

**功能说明**

- 初始化一个等待队列，分配内部资源并完成初始化
- 调用前需确保wait指针有效且wait->wait为NULL，否则返回失败
- 支持linux、liteos、freertos系统

**前置条件**

- wait指针不为NULL，且wait->wait成员为NULL
- 调用上下文无中断限制

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| wait | [osal_wait](#struct_osal_wait) * | 待初始化的等待队列指针 | 非NULL，且wait->wait为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 初始化成功 | 等待队列初始化完成 |
| OSAL_FAILURE(-1) | 初始化失败 | 参数无效或内存分配失败 |

### osal_wait_interruptible <a id="osal_wait_interruptible"></a>

```c
int osal_wait_interruptible(osal_wait *wait, osal_wait_condition_func func, const void *param)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_wait.h"
```

**功能说明**

- 将当前线程置于可中断睡眠状态（TASK_INTERRUPTIBLE），等待条件函数返回真或收到信号
- 条件函数在每次等待队列被唤醒时重新检查
- LiteOS (Huawei LiteOS) 不支持可中断等待，该接口实际与osal_wait_uninterruptible行为一致
- 需在改变等待条件变量后调用osal_wait_wakeup唤醒

**前置条件**

- wait已通过osal_wait_init初始化成功
- 调用上下文无中断限制

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| wait | [osal_wait](#struct_osal_wait) * | 等待队列指针 | 非NULL，已初始化 |
| func | [osal_wait_condition_func](#typedef_osal_wait_condition_func) | 条件判断函数指针 | 可为NULL，NULL时直接返回成功 |
| param | const void * | 传递给条件函数的参数 | - |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 等待条件满足 | 条件函数返回真 |
| OSAL_FAILURE(-1) | 等待失败 | 参数无效；linux下被信号中断时返回-ERESTARTSYS |

### osal_wait_uninterruptible <a id="osal_wait_uninterruptible"></a>

```c
int osal_wait_uninterruptible(osal_wait *wait, osal_wait_condition_func func, const void *param)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_wait.h"
```

**功能说明**

- 将当前线程置于不可中断睡眠状态（TASK_UNINTERRUPTIBLE），等待条件函数返回真
- 条件函数在每次等待队列被唤醒时重新检查
- 需在改变等待条件变量后调用osal_wait_wakeup唤醒
- 支持linux、liteos、freertos系统

**前置条件**

- wait已通过osal_wait_init初始化成功
- 调用上下文无中断限制

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| wait | [osal_wait](#struct_osal_wait) * | 等待队列指针 | 非NULL，已初始化 |
| func | [osal_wait_condition_func](#typedef_osal_wait_condition_func) | 条件判断函数指针 | 可为NULL，NULL时直接返回成功 |
| param | const void * | 传递给条件函数的参数 | - |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 等待条件满足 | 条件函数返回真 |
| OSAL_FAILURE(-1) | 等待失败 | 参数无效 |

### osal_wait_timeout_interruptible <a id="osal_wait_timeout_interruptible"></a>

```c
int osal_wait_timeout_interruptible(osal_wait *wait, osal_wait_condition_func func, const void *param, unsigned long ms)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_wait.h"
```

**功能说明**

- 将当前线程置于可中断睡眠状态，等待条件函数返回真或超时
- 条件函数在每次等待队列被唤醒时重新检查
- ms为0且条件为真时立即返回1；ms为OSAL_WAIT_FOREVER时无限等待
- 支持linux、liteos、freertos系统

**前置条件**

- wait已通过osal_wait_init初始化成功
- 调用上下文无中断限制

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| wait | [osal_wait](#struct_osal_wait) * | 等待队列指针 | 非NULL，已初始化 |
| func | [osal_wait_condition_func](#typedef_osal_wait_condition_func) | 条件判断函数指针 | 可为NULL，NULL时默认返回超时值 |
| param | const void * | 传递给条件函数的参数 | - |
| ms | unsigned long | 超时时间，单位毫秒 | [0, 0xFFFFFFFF]，0xFFFFFFFF表示永久等待 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_FAILURE(-1) | 等待失败 | 参数无效或超时转换溢出 |
| 0 | 条件未满足且超时已过 | 超时后条件仍为假 |
| 1 | 条件满足 | 条件函数返回真（含ms=0且条件为真） |
| >1 | 剩余超时tick数 | 条件在超时前满足 |

### osal_wait_timeout_uninterruptible <a id="osal_wait_timeout_uninterruptible"></a>

```c
int osal_wait_timeout_uninterruptible(osal_wait *wait, osal_wait_condition_func func, const void *param, unsigned long ms)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_wait.h"
```

**功能说明**

- 将当前线程置于不可中断睡眠状态，等待条件函数返回真或超时
- 条件函数在每次等待队列被唤醒时重新检查
- LiteOS不支持不可中断超时等待，该接口实际与osal_wait_timeout_interruptible行为一致
- 支持linux、liteos系统

**前置条件**

- wait已通过osal_wait_init初始化成功
- 调用上下文无中断限制

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| wait | [osal_wait](#struct_osal_wait) * | 等待队列指针 | 非NULL，已初始化 |
| func | [osal_wait_condition_func](#typedef_osal_wait_condition_func) | 条件判断函数指针 | 可为NULL，NULL时默认返回超时值 |
| param | const void * | 传递给条件函数的参数 | - |
| ms | unsigned long | 超时时间，单位毫秒 | [0, 0xFFFFFFFF]，0xFFFFFFFF表示永久等待 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_FAILURE(-1) | 等待失败 | 参数无效或超时转换溢出 |
| 0 | 条件未满足且超时已过 | 超时后条件仍为假 |
| 1 | 条件满足 | 条件函数返回真 |
| >1 | 剩余超时tick数 | 条件在超时前满足 |

### osal_wait_wakeup <a id="osal_wait_wakeup"></a>

```c
void osal_wait_wakeup(osal_wait *wait)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_wait.h"
```

**功能说明**

- 唤醒等待队列上的所有阻塞线程，与wait_event配对使用
- 需在改变任何可能影响等待条件结果的变量后调用
- 支持linux、liteos、freertos系统

**前置条件**

- wait已通过osal_wait_init初始化成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| wait | [osal_wait](#struct_osal_wait) * | 待唤醒的等待队列指针 | 非NULL，已初始化 |

### osal_wait_wakeup_interruptible <a id="osal_wait_wakeup_interruptible"></a>

```c
void osal_wait_wakeup_interruptible(osal_wait *wait)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_wait.h"
```

**功能说明**

- 唤醒等待队列上的可中断阻塞线程，与wait_event_interruptible配对使用
- LiteOS下该接口与osal_wait_wakeup行为一致
- 支持linux、liteos系统

**前置条件**

- wait已通过osal_wait_init初始化成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| wait | [osal_wait](#struct_osal_wait) * | 待唤醒的等待队列指针 | 非NULL，已初始化 |

### osal_wait_destroy <a id="osal_wait_destroy"></a>

```c
void osal_wait_destroy(osal_wait *wait)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_wait.h"
```

**功能说明**

- 销毁等待队列，释放内部资源
- wait必须由osal_wait_init初始化获得，该接口可能释放内存
- 销毁后wait->wait置为NULL
- 支持linux、liteos、freertos系统

**前置条件**

- wait已通过osal_wait_init初始化成功
- 确保无线程仍在该等待队列上阻塞

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| wait | [osal_wait](#struct_osal_wait) * | 待销毁的等待队列指针 | 非NULL，已初始化 |

## Type definitions

### osal_wait_condition_func <a id="typedef_osal_wait_condition_func"></a>

```c
typedef int (*osal_wait_condition_func)(const void *param);
```

**使用说明**

作为osal_wait_interruptible、osal_wait_uninterruptible、osal_wait_timeout_interruptible、osal_wait_timeout_uninterruptible接口的条件判断回调函数类型，函数返回值非0表示条件为真，0表示条件为假。

## Structures

### osal_wait <a id="struct_osal_wait"></a>

```c
typedef struct {
    void *wait;
} osal_wait;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| wait | void * | 等待队列内部实现指针，由osal_wait_init分配，由osal_wait_destroy释放 |



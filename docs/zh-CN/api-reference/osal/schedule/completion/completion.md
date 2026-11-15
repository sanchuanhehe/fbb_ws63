# OSAL Completion

OSAL (Operating System Abstraction Layer) Completion 提供完成量（completion）机制，用于线程间的执行完成同步。一个线程可通过完成量等待另一个线程完成特定操作后被唤醒，支持初始化、唤醒等待者、等待完成与重置等操作。

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_completion.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [osal_completion_init](#osal_completion_init) | 初始化动态分配的完成量结构 |
| [osal_complete](#osal_complete) | 唤醒单个等待该完成量的线程 |
| [osal_wait_for_completion](#osal_wait_for_completion) | 等待完成量信号，不可中断且无超时 |
| [osal_wait_for_completion_timeout](#osal_wait_for_completion_timeout) | 等待完成量信号，支持超时 |
| [osal_complete_all](#osal_complete_all) | 唤醒所有等待该完成量的线程 |
| [osal_complete_destory](#osal_complete_destory) | 释放动态分配的完成量资源 |

## Functions

### osal_completion_init <a id="osal_completion_init"></a>

```c
int osal_completion_init(osal_completion *com)
```

**头文件清单** ``kernel/osal/include/schedule/osal_completion.h``

**功能说明**

- 初始化动态分配的完成量结构体
- 内部动态分配内存并初始化完成量状态
- 必须通过 [osal_complete_destory](#osal_complete_destory) 释放资源

**前置条件**

- com 指针不为 NULL，且 com->completion 为 NULL（未被初始化）
- 系统内存资源充足，可完成动态内存分配

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| com | [osal_completion](#struct_osal_completion) * | 待初始化的完成量结构体指针 | 非NULL，且completion字段为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 初始化成功 | 动态内存分配成功且完成量初始化完成 |
| OSAL_FAILURE(-1) | 初始化失败 | com 为 NULL、com->completion 非 NULL 或动态内存分配失败 |

### osal_complete <a id="osal_complete"></a>

```c
void osal_complete(osal_completion *com)
```

**头文件清单** ``kernel/osal/include/schedule/osal_completion.h``

**功能说明**

- 向完成量发送信号，唤醒单个等待该完成量的线程
- 线程按排队顺序依次唤醒
- 唤醒任务前执行全内存屏障

**前置条件**

- com 指针不为 NULL，且 com->completion 不为 NULL（已完成初始化）

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| com | [osal_completion](#struct_osal_completion) * | 已初始化的完成量结构体指针 | 非NULL，且completion字段非NULL |

### osal_wait_for_completion <a id="osal_wait_for_completion"></a>

```c
void osal_wait_for_completion(osal_completion *com)
```

**头文件清单** ``kernel/osal/include/schedule/osal_completion.h``

**功能说明**

- 等待完成量信号，阻塞当前线程直到收到信号
- 不可中断，无超时机制
- 调用线程将阻塞直至其他线程调用 [osal_complete](#osal_complete) 或 [osal_complete_all](#osal_complete_all)

**前置条件**

- com 指针不为 NULL，且 com->completion 不为 NULL（已完成初始化）
- 不可在中断上下文中调用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| com | [osal_completion](#struct_osal_completion) * | 已初始化的完成量结构体指针 | 非NULL，且completion字段非NULL |

### osal_wait_for_completion_timeout <a id="osal_wait_for_completion_timeout"></a>

```c
unsigned long osal_wait_for_completion_timeout(osal_completion *com, unsigned long timeout)
```

**头文件清单** ``kernel/osal/include/schedule/osal_completion.h``

**功能说明**

- 等待完成量信号，支持超时机制
- 在 Linux 系统下 timeout 单位为 jiffies，在 LiteOS (Huawei LiteOS) 系统下 timeout 单位为 tick
- 不可中断
- 超时到期仍未收到信号则返回 0

**前置条件**

- com 指针不为 NULL，且 com->completion 不为 NULL（已完成初始化）
- 不可在中断上下文中调用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| com | [osal_completion](#struct_osal_completion) * | 已初始化的完成量结构体指针 | 非NULL，且completion字段非NULL |
| timeout | unsigned long | 超时时间，Linux 下为 jiffies，LiteOS 下为 tick | > 0 |

**返回值**

- 返回类型：unsigned long

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 超时 | 等待超时到期仍未收到完成量信号 |
| 正值 | 剩余超时时间 | 在超时到期前收到完成量信号 |
| -1 | 执行失败 | com 为 NULL 或 com->completion 为 NULL |

### osal_complete_all <a id="osal_complete_all"></a>

```c
void osal_complete_all(osal_completion *com)
```

**头文件清单** ``kernel/osal/include/schedule/osal_completion.h``

**功能说明**

- 向完成量发送信号，唤醒所有等待该完成量的线程
- 唤醒任务前执行全内存屏障
- 适用于广播通知场景

**前置条件**

- com 指针不为 NULL，且 com->completion 不为 NULL（已完成初始化）

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| com | [osal_completion](#struct_osal_completion) * | 已初始化的完成量结构体指针 | 非NULL，且completion字段非NULL |

### osal_complete_destory <a id="osal_complete_destory"></a>

```c
void osal_complete_destory(osal_completion *com)
```

**头文件清单** ``kernel/osal/include/schedule/osal_completion.h``

**功能说明**

- 释放动态分配的完成量资源，释放内部内存并将 completion 字段置 NULL
- com 必须由 [osal_completion_init](#osal_completion_init) 初始化获得

**前置条件**

- com 指针不为 NULL，且 com->completion 不为 NULL（已完成初始化）
- com 必须由 osal_completion_init 初始化，禁止释放未初始化的完成量

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| com | [osal_completion](#struct_osal_completion) * | 已初始化的完成量结构体指针 | 非NULL，且completion字段非NULL |

## Structures

### osal_completion <a id="struct_osal_completion"></a>

```c
typedef struct {
    void *completion;
} osal_completion;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| completion | void * | 完成量内部实现指针，由 osal_completion_init 动态分配 |



# OSAL Workqueue

OSAL (Operating System Abstraction Layer) Workqueue 提供工作队列机制，用于将工作任务提交到内核工作线程异步执行。支持工作队列的创建/销毁、工作项的调度、刷新与取消等操作，适用于中断下半部处理与延迟执行场景。

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_workqueue.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [osal_workqueue_init](#osal_workqueue_init) | 初始化工作队列，绑定回调处理函数 |
| [osal_workqueue_schedule](#osal_workqueue_schedule) | 将工作任务提交至内核全局工作队列 |
| [osal_workqueue_destroy](#osal_workqueue_destroy) | 销毁工作队列，释放关联资源 |
| [osal_workqueue_flush](#osal_workqueue_flush) | 等待工作队列中最后一次排队实例执行完成 |

## Functions

### osal_workqueue_init <a id="osal_workqueue_init"></a>

```c
int osal_workqueue_init(osal_workqueue *work, osal_workqueue_handler handler)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_workqueue.h"
```

**功能说明**

- 初始化工作队列对象，绑定回调处理函数
- 内部分配 work_struct 及管理节点内存，将工作队列注册到全局链表
- 支持系统：linux liteos freertos

**前置条件**

- 模块初始化状态：OSAL 内核调度子系统已就绪
- 参数合法性要求：work 指针不为 NULL，且 work->work 为 NULL（未重复初始化）
- 上下文限制：需在主线程调用，禁止在中断上下文中调用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| work | [osal_workqueue](#struct_osal_workqueue) * | 待初始化的工作队列对象 | 非NULL，且 work->work 为 NULL |
| handler | [osal_workqueue_handler](#typedef_osal_workqueue_handler) | 工作队列回调处理函数 | 非NULL函数指针 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 初始化成功 | 工作队列对象初始化并注册成功 |
| OSAL_FAILURE(-1) | 初始化失败 | 参数无效或内存分配失败 |

### osal_workqueue_schedule <a id="osal_workqueue_schedule"></a>

```c
int osal_workqueue_schedule(osal_workqueue *work)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_workqueue.h"
```

**功能说明**

- 将工作任务提交至内核全局工作队列执行
- 若工作已在队列中则保持在原位置，不重复入队
- 支持系统：linux liteos freertos

**前置条件**

- 模块初始化状态：工作队列已通过 osal_workqueue_init() 初始化完成
- 参数合法性要求：work 指针不为 NULL，且 work->work 不为 NULL
- 上下文限制：需在主线程调用，禁止在中断上下文中调用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| work | [osal_workqueue](#struct_osal_workqueue) * | 待调度的工作队列对象 | 非NULL，且 work->work 非 NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| TRUE(1) | 调度成功 | 工作任务成功加入队列 |
| FALSE(0) | 调度失败 | 工作已在队列中或参数无效 |

### osal_workqueue_destroy <a id="osal_workqueue_destroy"></a>

```c
void osal_workqueue_destroy(osal_workqueue *work)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_workqueue.h"
```

**功能说明**

- 销毁工作队列对象，释放内部分配的 work_struct 及管理节点内存
- 从全局链表中移除该工作队列，并将 work->work 置 NULL
- 支持系统：linux liteos freertos

**前置条件**

- 模块初始化状态：工作队列已通过 osal_workqueue_init() 初始化完成
- 参数合法性要求：work 指针不为 NULL，且 work->work 不为 NULL
- 上下文限制：需在主线程调用，禁止在中断上下文中调用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| work | [osal_workqueue](#struct_osal_workqueue) * | 待销毁的工作队列对象 | 非NULL，且 work->work 非 NULL |

**返回值**

当前接口无返回值。

### osal_workqueue_flush <a id="osal_workqueue_flush"></a>

```c
int osal_workqueue_flush(osal_workqueue *work)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_workqueue.h"
```

**功能说明**

- 等待工作队列中最后一次排队实例执行完成
- 工作在返回时保证处于空闲状态，前提是 flush 开始后未被重新入队
- 支持系统：linux liteos

**前置条件**

- 模块初始化状态：工作队列已通过 osal_workqueue_init() 初始化完成
- 参数合法性要求：work 指针不为 NULL，且 work->work 不为 NULL
- 上下文限制：需在主线程调用，禁止在中断上下文中调用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| work | [osal_workqueue](#struct_osal_workqueue) * | 待刷新的工作队列对象 | 非NULL，且 work->work 非 NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| TRUE(1) | 刷新成功 | 工作已执行完成 |
| FALSE(0) | 刷新失败 | 工作未被成功执行 |
| OSAL_FAILURE(-1) | 参数无效 | work 为 NULL 或 work->work 为 NULL |

## Type definitions

### osal_workqueue_handler <a id="typedef_osal_workqueue_handler"></a>

```c
typedef void (*osal_workqueue_handler)(osal_workqueue *workqueue);
```

**使用说明**

作为 osal_workqueue_init 的 handler 参数类型，工作队列回调处理函数指针

## Structures

### osal_workqueue <a id="struct_osal_workqueue"></a>

```c
typedef struct osal_workqueue_ {
    int queue_flag;
    void *work;
    void (*handler)(struct osal_workqueue_ *workqueue);
} osal_workqueue;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| queue_flag | int | 工作队列标志位 |
| work | void * | 底层工作结构体指针，初始化后指向内部分配的 work_struct |
| handler | void (*)(struct osal_workqueue_ *workqueue) | 工作队列回调处理函数指针 |



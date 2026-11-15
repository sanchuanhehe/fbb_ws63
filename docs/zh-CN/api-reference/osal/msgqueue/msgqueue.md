# Msgqueue

消息队列用于任务间以消息为单位的异步通信。支持阻塞和非阻塞模式（带超时），消息可从队尾写入、从队头读出，也可从队头写入（高优先级消息插队）。

提供队列创建、消息读写、队列满检测和消息计数等功能。适用 ISR (Interrupt Service Routine) 到任务的数据传递、多任务间结构化数据交换等场景。

**头文件清单**

```c
#include "kernel/osal/include/msgqueue/osal_msgqueue.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [osal_msg_queue_create](#osal_msg_queue_create) | 创建消息队列 |
| [osal_msg_queue_write_copy](#osal_msg_queue_write_copy) | 向队列尾部写入数据 |
| [osal_msg_queue_read_copy](#osal_msg_queue_read_copy) | 从队列头部读取数据 |
| [osal_msg_queue_write_head_copy](#osal_msg_queue_write_head_copy) | 向队列头部写入数据 |
| [osal_msg_queue_delete](#osal_msg_queue_delete) | 删除消息队列 |
| [osal_msg_queue_is_full](#osal_msg_queue_is_full) | 检查消息队列是否已满 |
| [osal_msg_queue_get_msg_num](#osal_msg_queue_get_msg_num) | 获取消息队列中当前消息数量 |

## Functions

### osal_msg_queue_create <a id="osal_msg_queue_create"></a>

```c
int osal_msg_queue_create(const char *name, unsigned short queue_len, unsigned long *queue_id, unsigned int flags, unsigned short max_msgsize)
```

**头文件清单**

```c
#include "kernel/osal/include/msgqueue/osal_msgqueue.h"
```

**功能说明**

- 创建消息队列，分配队列控制结构并返回队列ID
- 队列长度和节点大小由入参指定，队列数量受 LOSCFG_BASE_IPC (Inter-Process Communication)_QUEUE_LIMIT 约束
- 该接口仅在 LOSCFG_QUEUE_DYNAMIC_ALLOCATION 定义时可用
- queue_id 在 LiteOS (Huawei LiteOS) 系统中为整数，在 FreeRTOS (Free Real-Time Operating System) 系统中为地址

**前置条件**

- LiteOS 内核已完成初始化，LOSCFG_BASE_IPC_QUEUE 及 LOSCFG_QUEUE_DYNAMIC_ALLOCATION 已开启
- 系统可用队列数量未超过 LOSCFG_BASE_IPC_QUEUE_LIMIT 配置上限

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| name | const char * | 消息队列名称，保留参数，当前未使用 | 可为 NULL |
| queue_len | unsigned short | 队列长度，即可容纳的消息条数 | [1, 0xFFFF] |
| queue_id | unsigned long * | 输出参数，成功创建后返回队列ID | 非 NULL |
| flags | unsigned int | 队列模式，保留参数，当前未使用 | - |
| max_msgsize | unsigned short | 单条消息最大节点大小 | [1, 0xFFFF] |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| queue_id | unsigned long * | 成功创建后返回队列控制结构ID |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 创建成功 | 队列创建成功 |
| OSAL_FAILURE(-1) | 创建失败 | 队列创建失败，如队列资源不足或参数非法 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| LOSCFG_BASE_IPC_QUEUE | 特性宏 | 支持消息队列功能 | y |
| LOSCFG_QUEUE_DYNAMIC_ALLOCATION | 特性宏 | 支持动态分配方式创建消息队列 | y |
| TINY_KERNEL | 特性宏 | 未定义时支持创建消息队列接口 | n |

### osal_msg_queue_write_copy <a id="osal_msg_queue_write_copy"></a>

```c
int osal_msg_queue_write_copy(unsigned long queue_id, void *buffer_addr, unsigned int buffer_size, unsigned int timeout)
```

**头文件清单**

```c
#include "kernel/osal/include/msgqueue/osal_msgqueue.h"
```

**功能说明**

- 将指定长度的数据写入消息队列尾部，数据以拷贝方式写入
- 写入操作遵循 FIFO (First-In First-Out) 顺序，先写入的数据先被读取
- timeout 参数为相对时间，支持永久等待和不可等待模式
- 禁止在中断上下文和软件定时器回调中调用

**前置条件**

- 目标队列已通过 osal_msg_queue_create 创建成功
- LiteOS 内核已完成初始化
- 不在中断上下文或软件定时器回调中调用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| queue_id | unsigned long | 队列ID，由 osal_msg_queue_create 创建 | 有效队列ID |
| buffer_addr | void * | 待写入数据的起始地址 | 非 NULL |
| buffer_size | unsigned int | 待写入数据的缓冲区大小 | [1, 0xFFFFFFFF] |
| timeout | unsigned int | 超时时间，单位为 Tick | [OSAL_MSGQ_NO_WAIT(0), OSAL_MSGQ_WAIT_FOREVER(0xFFFFFFFF)] 或具体 Tick 数 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 写入成功 | 数据成功写入队列 |
| OSAL_FAILURE(-1) | 写入失败 | 队列已满、参数非法或超时 |

### osal_msg_queue_read_copy <a id="osal_msg_queue_read_copy"></a>

```c
int osal_msg_queue_read_copy(unsigned long queue_id, void *buffer_addr, unsigned int *buffer_size, unsigned int timeout)
```

**头文件清单**

```c
#include "kernel/osal/include/msgqueue/osal_msgqueue.h"
```

**功能说明**

- 从消息队列头部读取数据，以拷贝方式存储到指定缓冲区
- 读取采用 FIFO 模式，先写入的数据先被读取
- buffer_size 入参时为期望读取大小，出参时为实际读取大小
- 禁止在中断上下文和软件定时器回调中调用

**前置条件**

- 目标队列已通过 osal_msg_queue_create 创建成功
- LiteOS 内核已完成初始化
- 不在中断上下文或软件定时器回调中调用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| queue_id | unsigned long | 队列ID，由 osal_msg_queue_create 创建 | 有效队列ID |
| buffer_addr | void * | 存储读取数据的起始地址 | 非 NULL |
| buffer_size | unsigned int * | 读取前为期望读取大小，读取后为实际读取大小 | 非 NULL |
| timeout | unsigned int | 超时时间，单位为 Tick | [OSAL_MSGQ_NO_WAIT(0), OSAL_MSGQ_WAIT_FOREVER(0xFFFFFFFF)] 或具体 Tick 数 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| buffer_addr | void * | 存储从队列中读取的数据 |
| buffer_size | unsigned int * | 实际读取的数据大小 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 读取成功 | 数据成功从队列读取 |
| OSAL_FAILURE(-1) | 读取失败 | 队列为空、参数非法或超时 |

### osal_msg_queue_write_head_copy <a id="osal_msg_queue_write_head_copy"></a>

```c
int osal_msg_queue_write_head_copy(unsigned long queue_id, void *buffer_addr, unsigned int buffer_size, unsigned int timeout)
```

**头文件清单**

```c
#include "kernel/osal/include/msgqueue/osal_msgqueue.h"
```

**功能说明**

- 将指定长度的数据写入消息队列头部，实现优先级插入，数据以拷贝方式写入
- 与 osal_msg_queue_write_copy 不同，写入头部的数据将被优先读取
- timeout 参数为相对时间，支持永久等待和不可等待模式
- 禁止在中断上下文和软件定时器回调中调用

**前置条件**

- 目标队列已通过 osal_msg_queue_create 创建成功
- LiteOS 内核已完成初始化
- 不在中断上下文或软件定时器回调中调用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| queue_id | unsigned long | 队列ID，由 osal_msg_queue_create 创建 | 有效队列ID |
| buffer_addr | void * | 待写入数据的起始地址 | 非 NULL |
| buffer_size | unsigned int | 待写入数据的缓冲区大小 | [1, 0xFFFFFFFF] |
| timeout | unsigned int | 超时时间，单位为 Tick | [OSAL_MSGQ_NO_WAIT(0), OSAL_MSGQ_WAIT_FOREVER(0xFFFFFFFF)] 或具体 Tick 数 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 写入成功 | 数据成功写入队列头部 |
| OSAL_FAILURE(-1) | 写入失败 | 队列已满、参数非法或超时 |

### osal_msg_queue_delete <a id="osal_msg_queue_delete"></a>

```c
void osal_msg_queue_delete(unsigned long queue_id)
```

**头文件清单**

```c
#include "kernel/osal/include/msgqueue/osal_msgqueue.h"
```

**功能说明**

- 删除指定消息队列，释放队列资源
- 若有任务阻塞在该队列上，或队列正在被读写，删除操作将失败
- 不能删除未创建的队列

**前置条件**

- 目标队列已通过 osal_msg_queue_create 创建成功
- 无任务阻塞在该队列上，队列未被读写占用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| queue_id | unsigned long | 队列ID，由 osal_msg_queue_create 创建 | 有效队列ID |

### osal_msg_queue_is_full <a id="osal_msg_queue_is_full"></a>

```c
int osal_msg_queue_is_full(unsigned long queue_id)
```

**头文件清单**

```c
#include "kernel/osal/include/msgqueue/osal_msgqueue.h"
```

**功能说明**

- 检查指定消息队列是否已满
- 队列已满时返回 TRUE（1），未满时返回 FALSE（0）
- 获取队列信息失败时返回 TRUE

**前置条件**

- 目标队列已通过 osal_msg_queue_create 创建成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| queue_id | unsigned long | 队列ID，由 osal_msg_queue_create 创建 | 有效队列ID |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 1 | 队列已满 | 队列中可读消息数等于队列长度，或获取队列信息失败 |
| 0 | 队列未满 | 队列中可读消息数小于队列长度 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_SEC_CORE | 特性宏 | 支持安全核队列信息结构体访问特性 | n |

### osal_msg_queue_get_msg_num <a id="osal_msg_queue_get_msg_num"></a>

```c
unsigned int osal_msg_queue_get_msg_num(unsigned long queue_id)
```

**头文件清单**

```c
#include "kernel/osal/include/msgqueue/osal_msgqueue.h"
```

**功能说明**

- 获取指定消息队列中当前的消息数量
- 获取失败时返回 OSAL_INVALID_MSG_NUM

**前置条件**

- 目标队列已通过 osal_msg_queue_create 创建成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| queue_id | unsigned long | 队列ID，由 osal_msg_queue_create 创建 | 有效队列ID |

**返回值**

- 返回类型：unsigned int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0~0xFFFFFFFF | 当前消息数量 | 成功获取队列消息数 |
| OSAL_INVALID_MSG_NUM(0xFFFFFFFF) | 获取失败 | 获取队列信息失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_SEC_CORE | 特性宏 | 支持安全核队列信息结构体访问特性 | n |

## Type definitions

### OSAL_MSGQ_WAIT_FOREVER <a id="typedef_osal_msgq_wait_forever"></a>

```c
#ifdef LOS_WAIT_FOREVER
#define OSAL_MSGQ_WAIT_FOREVER LOS_WAIT_FOREVER
#else
#define OSAL_MSGQ_WAIT_FOREVER 0xFFFFFFFF
#endif
```

**使用说明**

用于 osal_msg_queue_write_copy、osal_msg_queue_read_copy、osal_msg_queue_write_head_copy 的 timeout 参数，表示永久等待

### OSAL_MSGQ_NO_WAIT <a id="typedef_osal_msgq_no_wait"></a>

```c
#ifdef LOS_NO_WAIT
#define OSAL_MSGQ_NO_WAIT LOS_NO_WAIT
#else
#define OSAL_MSGQ_NO_WAIT 0
#endif
```

**使用说明**

用于 osal_msg_queue_write_copy、osal_msg_queue_read_copy、osal_msg_queue_write_head_copy 的 timeout 参数，表示不可等待

### OSAL_SUCCESS <a id="typedef_osal_success"></a>

```c
#define OSAL_SUCCESS 0
```

**使用说明**

用于 osal_msg_queue_create、osal_msg_queue_write_copy、osal_msg_queue_read_copy、osal_msg_queue_write_head_copy 的返回值，表示操作成功

### OSAL_FAILURE <a id="typedef_osal_failure"></a>

```c
#define OSAL_FAILURE (-1)
```

**使用说明**

用于 osal_msg_queue_create、osal_msg_queue_write_copy、osal_msg_queue_read_copy、osal_msg_queue_write_head_copy 的返回值，表示操作失败

### OSAL_INVALID_MSG_NUM <a id="typedef_osal_invalid_msg_num"></a>

```c
#define OSAL_INVALID_MSG_NUM 0xFFFFFFFF
```

**使用说明**

用于 osal_msg_queue_get_msg_num 的返回值，表示获取消息数量失败



# Semaphore

信号量用于任务间同步和资源访问控制。支持计数信号量（管理多份同类资源）和二元信号量（互斥访问）。提供阻塞获取（down）、带超时获取（down_timeout）、非阻塞尝试（trydown）和释放（up）操作。

与互斥锁的区别：信号量没有"所有者"概念，可以由任务 A 获取、任务 B 释放。适用生产者-消费者模型、资源池管理等场景。

**头文件清单**

```c
#include "kernel/osal/include/semaphore/osal_semaphore.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [osal_sem_init](#osal_sem_init) | 创建计数信号量 |
| [osal_sem_binary_sem_init](#osal_sem_binary_sem_init) | 创建二值信号量 |
| [osal_sem_down](#osal_sem_down) | 请求信号量（永久阻塞） |
| [osal_sem_down_timeout](#osal_sem_down_timeout) | 限时请求信号量 |
| [osal_sem_down_interruptible](#osal_sem_down_interruptible) | 可中断方式请求信号量 |
| [osal_sem_trydown](#osal_sem_trydown) | 非阻塞方式尝试获取信号量 |
| [osal_sem_up](#osal_sem_up) | 释放信号量 |
| [osal_sem_destroy](#osal_sem_destroy) | 删除信号量 |

## Functions

### osal_sem_init <a id="osal_sem_init"></a>

```c
int osal_sem_init(osal_semaphore *sem, int val)
```

**头文件清单**

```c
#include "kernel/osal/include/semaphore/osal_semaphore.h"
```

**功能说明**

- 创建计数信号量控制结构，根据指定初始值初始化信号量可用计数
- 支持多任务间资源同步与互斥访问
- 支持系统：linux liteos freertos

**前置条件**

- 信号量结构体指针 sem 已分配有效内存空间
- 调用前信号量尚未被初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| sem | osal_semaphore * | 信号量控制结构指针 | 非 NULL |
| val | int | 信号量初始可用计数 | val >= 0 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| sem | osal_semaphore * | 初始化完成的信号量控制结构 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 创建成功 | 信号量初始化完成 |
| OSAL_FAILURE(-1) | 创建失败 | 参数无效或底层信号量创建失败 |

### osal_sem_binary_sem_init <a id="osal_sem_binary_sem_init"></a>

```c
int osal_sem_binary_sem_init(osal_semaphore *sem, int val)
```

**头文件清单**

```c
#include "kernel/osal/include/semaphore/osal_semaphore.h"
```

**功能说明**

- 创建二值信号量控制结构，初始值限定为 0 或 1
- 适用于任务间简单同步场景，信号量计数不超过 1
- 支持系统：liteos freertos

**前置条件**

- 信号量结构体指针 sem 已分配有效内存空间
- 调用前信号量尚未被初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| sem | osal_semaphore * | 信号量控制结构指针 | 非 NULL |
| val | int | 二值信号量初始可用计数 | [0, 1] |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| sem | osal_semaphore * | 初始化完成的二值信号量控制结构 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 创建成功 | 二值信号量初始化完成 |
| OSAL_FAILURE(-1) | 创建失败 | 参数无效或底层信号量创建失败 |

### osal_sem_down <a id="osal_sem_down"></a>

```c
int osal_sem_down(osal_semaphore *sem)
```

**头文件清单**

```c
#include "kernel/osal/include/semaphore/osal_semaphore.h"
```

**功能说明**

- 请求获取信号量，若信号量可用计数大于 0 则立即获取并计数减 1
- 若信号量可用计数为 0，调用任务将进入睡眠阻塞，直到信号量被释放
- 禁止在中断上下文中调用
- 禁止在系统任务（idle、swtmr）中调用
- 不推荐在软件定时器回调中使用
- 支持系统：linux liteos freertos

**前置条件**

- 信号量已通过 osal_sem_init 或 osal_sem_binary_sem_init 创建成功
- 调用上下文为普通任务，禁止中断上下文、系统任务上下文

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| sem | osal_semaphore * | 待获取的信号量指针 | 非 NULL，已初始化 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 获取成功 | 成功获取信号量 |
| OSAL_FAILURE(-1) | 获取失败 | 参数无效或等待超时 |

### osal_sem_down_timeout <a id="osal_sem_down_timeout"></a>

```c
int osal_sem_down_timeout(osal_semaphore *sem, unsigned int timeout)
```

**头文件清单**

```c
#include "kernel/osal/include/semaphore/osal_semaphore.h"
```

**功能说明**

- 在指定超时时间内请求获取信号量
- 若信号量可用计数大于 0 则立即获取；若为 0，任务将阻塞等待，直到信号量被释放或超时
- 超时时间单位为毫秒
- 禁止在中断上下文中调用
- 禁止在系统任务（idle、swtmr）中调用
- 不推荐在软件定时器回调中使用
- 支持系统：liteos freertos

**前置条件**

- 信号量已通过 osal_sem_init 或 osal_sem_binary_sem_init 创建成功
- 调用上下文为普通任务，禁止中断上下文、系统任务上下文

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| sem | osal_semaphore * | 待获取的信号量指针 | 非 NULL，已初始化 |
| timeout | unsigned int | 超时等待时间 | 单位 ms；OSAL_SEM_WAIT_FOREVER(-1) 表示永久等待 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 获取成功 | 在超时时间内成功获取信号量 |
| OSAL_FAILURE(-1) | 获取失败 | 参数无效或底层信号量请求失败 |
| OSAL_ETIME(-62) | 超时 | 等待超时未获取到信号量 |

### osal_sem_down_interruptible <a id="osal_sem_down_interruptible"></a>

```c
int osal_sem_down_interruptible(osal_semaphore *sem)
```

**头文件清单**

```c
#include "kernel/osal/include/semaphore/osal_semaphore.h"
```

**功能说明**

- 以可中断方式请求获取信号量
- 若信号量可用计数大于 0 则立即获取；若为 0，任务将进入睡眠阻塞，直到信号量被释放或被信号中断
- 支持系统：linux liteos freertos

**前置条件**

- 信号量已通过 osal_sem_init 或 osal_sem_binary_sem_init 创建成功
- 调用上下文为普通任务

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| sem | osal_semaphore * | 待获取的信号量指针 | 非 NULL，已初始化 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 获取成功 | 成功获取信号量 |
| OSAL_FAILURE(-1) | 获取失败 | 信号量请求失败 |
| OSAL_EINTR(-4) | 被信号中断 | 睡眠等待期间被信号中断 |

### osal_sem_trydown <a id="osal_sem_trydown"></a>

```c
int osal_sem_trydown(osal_semaphore *sem)
```

**头文件清单**

```c
#include "kernel/osal/include/semaphore/osal_semaphore.h"
```

**功能说明**

- 非阻塞方式尝试获取信号量，不等待
- 若信号量可用则立即获取，否则立即返回失败
- 可在中断上下文中调用
- 信号量可由任何任务或中断释放
- 返回值语义与 spin_trylock / mutex_trylock 相反：0 表示获取成功，1 表示获取失败
- 支持系统：linux liteos freertos

**前置条件**

- 信号量已通过 osal_sem_init 或 osal_sem_binary_sem_init 创建成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| sem | osal_semaphore * | 待获取的信号量指针 | 非 NULL，已初始化 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 获取成功 | 信号量可用，成功获取 |
| 1 | 获取失败 | 信号量不可用，未获取 |

### osal_sem_up <a id="osal_sem_up"></a>

```c
void osal_sem_up(osal_semaphore *sem)
```

**头文件清单**

```c
#include "kernel/osal/include/semaphore/osal_semaphore.h"
```

**功能说明**

- 释放信号量，信号量可用计数加 1
- 可在任意上下文中调用，包括中断上下文
- 可由未调用 osal_sem_down 的任务释放信号量
- 支持系统：linux liteos freertos

**前置条件**

- 信号量已通过 osal_sem_init 或 osal_sem_binary_sem_init 创建成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| sem | osal_semaphore * | 待释放的信号量指针 | 非 NULL，已初始化 |

### osal_sem_destroy <a id="osal_sem_destroy"></a>

```c
void osal_sem_destroy(osal_semaphore *sem)
```

**头文件清单**

```c
#include "kernel/osal/include/semaphore/osal_semaphore.h"
```

**功能说明**

- 删除信号量，释放信号量占用的资源空间
- 信号量删除后不可再使用
- 支持系统：linux liteos freertos

**前置条件**

- 信号量已通过 osal_sem_init 或 osal_sem_binary_sem_init 创建成功
- 无其他任务正在等待该信号量

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| sem | osal_semaphore * | 待删除的信号量指针 | 非 NULL，已初始化 |

## Type definitions

### osal_semaphore <a id="typedef_osal_semaphore"></a>

```c
typedef struct {
    void *sem;
} osal_semaphore;
```

**使用说明**

信号量控制结构体，在 osal_sem_init、osal_sem_binary_sem_init、osal_sem_down、osal_sem_down_timeout、osal_sem_down_interruptible、osal_sem_trydown、osal_sem_up、osal_sem_destroy 接口中作为信号量标识载体使用

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| sem | void * | 底层信号量句柄指针 |



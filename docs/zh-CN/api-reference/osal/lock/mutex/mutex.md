# Mutex

互斥锁（Mutex）为临界区提供独占访问保护——同一时刻只有一个任务能持有锁。支持阻塞锁（lock，无限等待）、超时锁（lock_timeout）和非阻塞尝试锁（trylock，获取失败立即返回）。

持有 Mutex 的任务不能再次获取同一把锁（非递归）。适用保护全局变量、硬件寄存器、复杂数据结构等任务间共享资源。

**头文件清单**

```c
#include "kernel/osal/include/lock/osal_mutex.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [osal_mutex_init](#osal_mutex_init) | 初始化互斥锁 |
| [osal_mutex_lock](#osal_mutex_lock) | 获取互斥锁（永久等待） |
| [osal_mutex_lock_timeout](#osal_mutex_lock_timeout) | 获取互斥锁（限时等待） |
| [osal_mutex_lock_interruptible](#osal_mutex_lock_interruptible) | 获取互斥锁（可被信号中断） |
| [osal_mutex_trylock](#osal_mutex_trylock) | 尝试获取互斥锁（非阻塞） |
| [osal_mutex_unlock](#osal_mutex_unlock) | 释放互斥锁 |
| [osal_mutex_is_locked](#osal_mutex_is_locked) | 查询互斥锁是否处于锁定状态 |
| [osal_mutex_destroy](#osal_mutex_destroy) | 销毁互斥锁 |

## Functions

### osal_mutex_init <a id="osal_mutex_init"></a>

```c
int osal_mutex_init(osal_mutex *mutex)
```

**头文件清单**

```c
#include "kernel/osal/include/lock/osal_mutex.h"
```

**功能说明**

- 初始化互斥锁，为互斥锁分配底层资源并完成初始状态设置
- 互斥锁初始化后方可被锁定、解锁或销毁
- 禁止对已初始化的互斥锁重复调用初始化，禁止使用 memset 将互斥锁清零

**前置条件**

- 互斥锁未被初始化或已被销毁，mutex 指针指向的内存空间已分配
- 互斥锁结构体中 mutex 字段为 NULL 或已通过 osal_mutex_destroy 释放

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| mutex | osal_mutex * | 指向待初始化的互斥锁结构体指针 | 非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 初始化成功 | 互斥锁创建成功 |
| OSAL_FAILURE(-1) | 初始化失败 | mutex 为 NULL、mutex 已初始化或底层创建互斥锁失败 |

### osal_mutex_lock <a id="osal_mutex_lock"></a>

```c
int osal_mutex_lock(osal_mutex *mutex)
```

**头文件清单**

```c
#include "kernel/osal/include/lock/osal_mutex.h"
```

**功能说明**

- 获取互斥锁，独占式锁定，若互斥锁不可用则当前任务休眠等待直到获取成功
- 互斥锁必须由获取它的同一任务释放，禁止递归锁定
- 任务持有互斥锁期间不可退出，互斥锁所在内核内存不可在锁定状态下释放

**前置条件**

- 互斥锁已通过 osal_mutex_init 初始化成功
- 当前任务未持有该互斥锁，禁止递归锁定

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| mutex | osal_mutex * | 指向待获取的互斥锁结构体指针 | 非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 获取成功 | 互斥锁成功锁定 |
| OSAL_FAILURE(-1) | 获取失败 | mutex 为 NULL 或底层获取互斥锁失败 |

### osal_mutex_lock_timeout <a id="osal_mutex_lock_timeout"></a>

```c
int osal_mutex_lock_timeout(osal_mutex *mutex, unsigned int timeout)
```

**头文件清单**

```c
#include "kernel/osal/include/lock/osal_mutex.h"
```

**功能说明**

- 在指定超时时间内获取互斥锁，若互斥锁不可用则当前任务休眠等待，超时后返回失败
- 当 timeout 设置为 OSAL_MUTEX_WAIT_FOREVER 时，行为与 osal_mutex_lock 一致，永久等待
- LiteOS (Huawei LiteOS) 和 FreeRTOS (Free Real-Time Operating System) 支持嵌套锁特性

**前置条件**

- 互斥锁已通过 osal_mutex_init 初始化成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| mutex | osal_mutex * | 指向待获取的互斥锁结构体指针 | 非NULL |
| timeout | unsigned int | 等待超时时间，单位为毫秒 | OSAL_MUTEX_WAIT_FOREVER(-1)表示永久等待；大于0表示限时等待毫秒数 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 获取成功 | 在超时时间内成功获取互斥锁 |
| OSAL_FAILURE(-1) | 获取失败 | mutex 为 NULL、超时未获取或底层获取互斥锁失败 |

### osal_mutex_lock_interruptible <a id="osal_mutex_lock_interruptible"></a>

```c
int osal_mutex_lock_interruptible(osal_mutex *mutex)
```

**头文件清单**

```c
#include "kernel/osal/include/lock/osal_mutex.h"
```

**功能说明**

- 获取互斥锁，可被信号中断，若等待过程中收到信号则返回而不获取互斥锁
- 与 osal_mutex_lock 类似，区别在于等待过程可被信号打断
- LiteOS 支持嵌套锁特性

**前置条件**

- 互斥锁已通过 osal_mutex_init 初始化成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| mutex | osal_mutex * | 指向待获取的互斥锁结构体指针 | 非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 获取成功 | 互斥锁成功锁定 |
| OSAL_EINTR(-4) | 被信号中断 | 等待过程中收到信号 |
| OSAL_FAILURE(-1) | 获取失败 | mutex 为 NULL 或底层获取互斥锁失败 |

### osal_mutex_trylock <a id="osal_mutex_trylock"></a>

```c
int osal_mutex_trylock(osal_mutex *mutex)
```

**头文件清单**

```c
#include "kernel/osal/include/lock/osal_mutex.h"
```

**功能说明**

- 尝试获取互斥锁，非阻塞方式，若互斥锁不可用则立即返回而不等待
- LiteOS 和 FreeRTOS 支持嵌套锁特性

**前置条件**

- 互斥锁已通过 osal_mutex_init 初始化成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| mutex | osal_mutex * | 指向待尝试获取的互斥锁结构体指针 | 非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| TRUE(1) | 获取成功 | 互斥锁成功锁定 |
| FALSE(0) | 获取失败 | mutex 为 NULL 或互斥锁不可用 |

### osal_mutex_unlock <a id="osal_mutex_unlock"></a>

```c
void osal_mutex_unlock(osal_mutex *mutex)
```

**头文件清单**

```c
#include "kernel/osal/include/lock/osal_mutex.h"
```

**功能说明**

- 释放互斥锁，解锁由当前任务先前锁定的互斥锁
- 禁止在中断上下文中调用
- 禁止解锁未被锁定的互斥锁

**前置条件**

- 互斥锁已通过 osal_mutex_init 初始化成功
- 当前任务已持有该互斥锁

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| mutex | osal_mutex * | 指向待释放的互斥锁结构体指针 | 非NULL |

### osal_mutex_is_locked <a id="osal_mutex_is_locked"></a>

```c
int osal_mutex_is_locked(osal_mutex *mutex)
```

**头文件清单**

```c
#include "kernel/osal/include/lock/osal_mutex.h"
```

**功能说明**

- 查询互斥锁是否处于锁定状态
- 返回 true 表示互斥锁已锁定，返回 false 表示未锁定

**前置条件**

- 互斥锁已通过 osal_mutex_init 初始化成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| mutex | osal_mutex * | 指向待查询的互斥锁结构体指针 | 非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| true(非0) | 互斥锁已锁定 | 互斥锁处于锁定状态 |
| false(0) | 互斥锁未锁定 | 互斥锁处于未锁定状态 |

### osal_mutex_destroy <a id="osal_mutex_destroy"></a>

```c
void osal_mutex_destroy(osal_mutex *mutex)
```

**头文件清单**

```c
#include "kernel/osal/include/lock/osal_mutex.h"
```

**功能说明**

- 销毁互斥锁，释放互斥锁占用的底层资源
- 必须在模块退出时调用，否则将导致内存泄漏
- 调用后应将 mutex 指针置为 NULL

**前置条件**

- 互斥锁已通过 osal_mutex_init 初始化成功
- 互斥锁未被任何任务持有（处于未锁定状态）

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| mutex | osal_mutex * | 指向待销毁的互斥锁结构体指针 | 非NULL |

## Structures

### osal_mutex <a id="struct_osal_mutex"></a>

```c
typedef struct {
    void *mutex;
} osal_mutex;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| mutex | void * | 底层互斥锁句柄指针，初始化后指向底层系统互斥锁资源，销毁后置为NULL |



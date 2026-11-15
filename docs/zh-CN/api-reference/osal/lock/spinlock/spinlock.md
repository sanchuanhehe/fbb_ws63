# Spinlock

自旋锁（Spinlock）通过忙等待（不断循环检查）获取锁而非睡眠。持锁期间关闭调度和中断下半部，确保临界区极短。是中断上下文中唯一的锁选择——信号量和互斥锁在 ISR (Interrupt Service Routine) 中不能用。

提供 irqsave 变体（保存/恢复中断状态）、trylock（非阻塞尝试）和 is_locked（状态查询）。

**头文件清单**
```c
#include "kernel/osal/include/lock/osal_spinlock.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [osal_spin_lock_init](#osal_spin_lock_init) | 初始化自旋锁 |
| [osal_spin_lock](#osal_spin_lock) | 获取自旋锁 |
| [osal_spin_lock_bh](#osal_spin_lock_bh) | 禁用软中断并获取自旋锁 |
| [osal_spin_trylock](#osal_spin_trylock) | 尝试获取自旋锁 |
| [osal_spin_trylock_irq](#osal_spin_trylock_irq) | 尝试获取自旋锁并禁用CPU中断 |
| [osal_spin_trylock_irqsave](#osal_spin_trylock_irqsave) | 保存中断状态并尝试获取自旋锁 |
| [osal_spin_unlock](#osal_spin_unlock) | 释放自旋锁 |
| [osal_spin_unlock_bh](#osal_spin_unlock_bh) | 释放自旋锁并使能软中断 |
| [osal_spin_lock_irqsave](#osal_spin_lock_irqsave) | 保存中断状态并获取自旋锁 |
| [osal_spin_unlock_irqrestore](#osal_spin_unlock_irqrestore) | 释放自旋锁并恢复中断状态 |
| [osal_spin_lock_destroy](#osal_spin_lock_destroy) | 销毁自旋锁 |

## Functions

### osal_spin_lock_init <a id="osal_spin_lock_init"></a>

```c
int osal_spin_lock_init(osal_spinlock *lock)
```

**头文件清单**

```c
#include "kernel/osal/include/lock/osal_spinlock.h"
```

**功能说明**

- 初始化自旋锁，分配底层锁资源并完成初始化
- 必须与 `osal_spin_lock_destroy` 配对使用，否则将导致内存泄漏
- 支持系统：linux、liteos

**前置条件**

- 调用前 lock 指针不为 NULL，且 lock->lock 为 NULL（未被初始化）
- 底层系统内存资源充足，能够完成锁资源的分配

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| lock | [osal_spinlock](#struct_osal_spinlock) * | 待初始化的自旋锁指针 | 非NULL，lock->lock须为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 初始化成功 | 锁资源分配并初始化完成 |
| OSAL_FAILURE(-1) | 初始化失败 | 参数无效或内存分配失败 |

### osal_spin_lock <a id="osal_spin_lock"></a>

```c
void osal_spin_lock(osal_spinlock *lock)
```

**头文件清单**

```c
#include "kernel/osal/include/lock/osal_spinlock.h"
```

**功能说明**

- 获取自旋锁，若锁已被其他线程持有则忙等待直到获取成功
- 同一任务内不可对同一自旋锁多次加锁，否则将导致死锁
- 若自旋锁将在任务与中断中同时使用，应使用 `osal_spin_lock_irqsave` 替代本接口
- 支持系统：linux、liteos

**前置条件**

- 自旋锁已通过 `osal_spin_lock_init` 初始化完成
- 当前任务未持有同一自旋锁

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| lock | [osal_spinlock](#struct_osal_spinlock) * | 待获取的自旋锁指针 | 非NULL，lock->lock非NULL |

### osal_spin_lock_bh <a id="osal_spin_lock_bh"></a>

```c
void osal_spin_lock_bh(osal_spinlock *lock)
```

**头文件清单**

```c
#include "kernel/osal/include/lock/osal_spinlock.h"
```

**功能说明**

- 禁用软中断并获取自旋锁（linux）；在 LiteOS (Huawei LiteOS) 和 FreeRTOS (Free Real-Time Operating System) 上禁用调度
- 与 `osal_spin_unlock_bh` 配对使用
- 支持系统：linux、liteos、freertos

**前置条件**

- 自旋锁已通过 `osal_spin_lock_init` 初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| lock | [osal_spinlock](#struct_osal_spinlock) * | 待获取的自旋锁指针 | 非NULL，lock->lock非NULL |

### osal_spin_trylock <a id="osal_spin_trylock"></a>

```c
int osal_spin_trylock(osal_spinlock *lock)
```

**头文件清单**

```c
#include "kernel/osal/include/lock/osal_spinlock.h"
```

**功能说明**

- 尝试获取自旋锁，若锁立即可用则获取成功，否则立即返回失败
- 不会忙等待，适用于非阻塞场景
- 支持系统：linux、liteos

**前置条件**

- 自旋锁已通过 `osal_spin_lock_init` 初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| lock | [osal_spinlock](#struct_osal_spinlock) * | 待尝试获取的自旋锁指针 | 非NULL，lock->lock非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| true(1) | 获取成功 | 锁立即可用 |
| false(0) | 获取失败 | 锁已被其他线程持有 |

### osal_spin_trylock_irq <a id="osal_spin_trylock_irq"></a>

```c
int osal_spin_trylock_irq(osal_spinlock *lock)
```

**头文件清单**

```c
#include "kernel/osal/include/lock/osal_spinlock.h"
```

**功能说明**

- 尝试获取自旋锁并禁用CPU中断，若锁立即可用则获取成功并禁用中断，否则立即返回失败
- 不会忙等待
- 支持系统：linux

**前置条件**

- 自旋锁已通过 `osal_spin_lock_init` 初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| lock | [osal_spinlock](#struct_osal_spinlock) * | 待尝试获取的自旋锁指针 | 非NULL，lock->lock非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| true(1) | 获取成功 | 锁立即可用 |
| false(0) | 获取失败 | 锁已被其他线程持有 |

### osal_spin_trylock_irqsave <a id="osal_spin_trylock_irqsave"></a>

```c
void osal_spin_trylock_irqsave(osal_spinlock *lock, unsigned long *flags)
```

**头文件清单**

```c
#include "kernel/osal/include/lock/osal_spinlock.h"
```

**功能说明**

- 保存当前CPU中断状态，尝试获取自旋锁并禁用CPU中断
- 通过 flags 参数保存中断状态，后续由 `osal_spin_unlock_irqrestore` 恢复
- 支持系统：linux

**前置条件**

- 自旋锁已通过 `osal_spin_lock_init` 初始化完成
- flags 指针不为 NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| lock | [osal_spinlock](#struct_osal_spinlock) * | 待尝试获取的自旋锁指针 | 非NULL，lock->lock非NULL |
| flags | unsigned long * | 保存中断状态的指针 | 非NULL |

### osal_spin_unlock <a id="osal_spin_unlock"></a>

```c
void osal_spin_unlock(osal_spinlock *lock)
```

**头文件清单**

```c
#include "kernel/osal/include/lock/osal_spinlock.h"
```

**功能说明**

- 释放自旋锁
- 与 `osal_spin_lock` 配对使用
- 支持系统：linux、liteos

**前置条件**

- 自旋锁已通过 `osal_spin_lock_init` 初始化完成
- 当前任务已持有该自旋锁

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| lock | [osal_spinlock](#struct_osal_spinlock) * | 待释放的自旋锁指针 | 非NULL，lock->lock非NULL |

### osal_spin_unlock_bh <a id="osal_spin_unlock_bh"></a>

```c
void osal_spin_unlock_bh(osal_spinlock *lock)
```

**头文件清单**

```c
#include "kernel/osal/include/lock/osal_spinlock.h"
```

**功能说明**

- 释放自旋锁并使能软中断（linux）；在 LiteOS 和 FreeRTOS 上恢复调度
- 与 `osal_spin_lock_bh` 配对使用
- 支持系统：linux、liteos、freertos

**前置条件**

- 自旋锁已通过 `osal_spin_lock_init` 初始化完成
- 当前任务已通过 `osal_spin_lock_bh` 获取该自旋锁

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| lock | [osal_spinlock](#struct_osal_spinlock) * | 待释放的自旋锁指针 | 非NULL，lock->lock非NULL |

### osal_spin_lock_irqsave <a id="osal_spin_lock_irqsave"></a>

```c
void osal_spin_lock_irqsave(osal_spinlock *lock, unsigned long *flags)
```

**头文件清单**

```c
#include "kernel/osal/include/lock/osal_spinlock.h"
```

**功能说明**

- 保存当前CPU中断状态，获取自旋锁并禁用CPU中断
- 通过 flags 参数保存中断状态，后续由 `osal_spin_unlock_irqrestore` 恢复
- 适用于自旋锁在任务与中断上下文中同时使用的场景
- 支持系统：linux、liteos、freertos

**前置条件**

- 自旋锁已通过 `osal_spin_lock_init` 初始化完成
- flags 指针不为 NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| lock | [osal_spinlock](#struct_osal_spinlock) * | 待获取的自旋锁指针 | 非NULL，lock->lock非NULL |
| flags | unsigned long * | 保存中断状态的指针 | 非NULL |

### osal_spin_unlock_irqrestore <a id="osal_spin_unlock_irqrestore"></a>

```c
void osal_spin_unlock_irqrestore(osal_spinlock *lock, unsigned long *flags)
```

**头文件清单**

```c
#include "kernel/osal/include/lock/osal_spinlock.h"
```

**功能说明**

- 释放自旋锁并恢复CPU中断状态
- 与 `osal_spin_lock_irqsave` 配对使用，flags 值须为 `osal_spin_lock_irqsave` 保存的值
- 支持系统：linux、liteos、freertos

**前置条件**

- 自旋锁已通过 `osal_spin_lock_init` 初始化完成
- 当前任务已通过 `osal_spin_lock_irqsave` 获取该自旋锁
- flags 值为 `osal_spin_lock_irqsave` 保存的中断状态

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| lock | [osal_spinlock](#struct_osal_spinlock) * | 待释放的自旋锁指针 | 非NULL，lock->lock非NULL |
| flags | unsigned long * | 中断状态指针 | 非NULL，值为osal_spin_lock_irqsave保存的状态 |

### osal_spin_lock_destroy <a id="osal_spin_lock_destroy"></a>

```c
void osal_spin_lock_destroy(osal_spinlock *lock)
```

**头文件清单**

```c
#include "kernel/osal/include/lock/osal_spinlock.h"
```

**功能说明**

- 销毁自旋锁，释放底层锁资源
- 必须与 `osal_spin_lock_init` 配对使用，否则将导致内存泄漏
- lock 必须为 `osal_spin_lock_init` 初始化返回的锁
- 支持系统：linux、liteos

**前置条件**

- 自旋锁已通过 `osal_spin_lock_init` 初始化完成
- 自旋锁当前未被任何任务持有

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| lock | [osal_spinlock](#struct_osal_spinlock) * | 待销毁的自旋锁指针 | 非NULL，lock->lock非NULL |

## Structures

### osal_spinlock <a id="struct_osal_spinlock"></a>

```c
typedef struct {
    void *lock;
} osal_spinlock;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| lock | void * | 底层锁资源指针，由 osal_spin_lock_init 分配，由 osal_spin_lock_destroy 释放 |



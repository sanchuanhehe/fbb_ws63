# Rwlock

读写锁允许多个读者同时持有锁（共享读），但写者独占（排他写）。适合读多写少的场景——读者之间不互斥，写者与所有读者和其他写者互斥。

操作：read_lock（共享读锁）、read_unlock（释放读锁）、write_lock（独占写锁）、write_unlock（释放写锁）。仅在 Linux 上可用。

**头文件清单**

```c
#include "kernel/osal/include/lock/osal_rwlock.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [osal_rwlock_init](#osal_rwlock_init) | 读写锁初始化 |
| [osal_rwlock_read_lock](#osal_rwlock_read_lock) | 获取读锁 |
| [osal_rwlock_read_unlock](#osal_rwlock_read_unlock) | 释放读锁 |
| [osal_rwlock_write_lock](#osal_rwlock_write_lock) | 获取写锁 |
| [osal_rwlock_write_unlock](#osal_rwlock_write_unlock) | 释放写锁 |
| [osal_rwlock_destory](#osal_rwlock_destory) | 销毁读写锁 |

## Functions

### osal_rwlock_init <a id="osal_rwlock_init"></a>

```c
int osal_rwlock_init(osal_rwlock *rw_lock)
```

**头文件清单**

```c
#include "kernel/osal/include/lock/osal_rwlock.h"
```

**功能说明**

- 读写锁初始化，在使用读写锁之前必须调用此接口完成初始化
- 初始化后读写锁处于未锁定状态，可用于后续读写操作
- 仅支持 Linux 内核模式

**前置条件**

- 调用上下文为 Linux 内核态（__KERNEL__ 宏已定义）
- 入参 rw_lock 指针不为 NULL，且指向的内存空间已申请成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| rw_lock | osal_rwlock * | 指向待初始化的读写锁结构体指针 | 非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 初始化成功 | 读写锁初始化完成 |
| OSAL_FAILURE(-1) | 初始化失败 | 读写锁初始化过程中出错 |

### osal_rwlock_read_lock <a id="osal_rwlock_read_lock"></a>

```c
void osal_rwlock_read_lock(osal_rwlock *rw_lock)
```

**头文件清单**

```c
#include "kernel/osal/include/lock/osal_rwlock.h"
```

**功能说明**

- 获取读锁，进入临界区进行读操作
- 多个读操作可同时持有读锁，读锁与写锁互斥
- 若当前写锁已被持有，调用线程将阻塞等待直到写锁释放
- 仅支持 Linux 内核模式

**前置条件**

- 调用上下文为 Linux 内核态（__KERNEL__ 宏已定义）
- 入参 rw_lock 指向的读写锁已通过 osal_rwlock_init() 初始化成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| rw_lock | osal_rwlock * | 指向已初始化的读写锁结构体指针 | 非NULL，且已初始化 |

### osal_rwlock_read_unlock <a id="osal_rwlock_read_unlock"></a>

```c
void osal_rwlock_read_unlock(osal_rwlock *rw_lock)
```

**头文件清单**

```c
#include "kernel/osal/include/lock/osal_rwlock.h"
```

**功能说明**

- 释放读锁，退出读临界区
- 必须与 osal_rwlock_read_lock() 成对调用，确保锁的获取与释放匹配
- 释放读锁后，等待的写锁请求可能被唤醒
- 仅支持 Linux 内核模式

**前置条件**

- 调用上下文为 Linux 内核态（__KERNEL__ 宏已定义）
- 入参 rw_lock 指向的读写锁已通过 osal_rwlock_init() 初始化成功
- 当前线程已通过 osal_rwlock_read_lock() 获取读锁

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| rw_lock | osal_rwlock * | 指向已初始化的读写锁结构体指针 | 非NULL，且已初始化 |

### osal_rwlock_write_lock <a id="osal_rwlock_write_lock"></a>

```c
void osal_rwlock_write_lock(osal_rwlock *rw_lock)
```

**头文件清单**

```c
#include "kernel/osal/include/lock/osal_rwlock.h"
```

**功能说明**

- 获取写锁，进入临界区进行写操作
- 写锁与读锁、写锁均互斥，同一时刻仅允许一个线程持有写锁
- 若当前读锁或写锁已被持有，调用线程将阻塞等待直到所有锁释放
- 仅支持 Linux 内核模式

**前置条件**

- 调用上下文为 Linux 内核态（__KERNEL__ 宏已定义）
- 入参 rw_lock 指向的读写锁已通过 osal_rwlock_init() 初始化成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| rw_lock | osal_rwlock * | 指向已初始化的读写锁结构体指针 | 非NULL，且已初始化 |

### osal_rwlock_write_unlock <a id="osal_rwlock_write_unlock"></a>

```c
void osal_rwlock_write_unlock(osal_rwlock *rw_lock)
```

**头文件清单**

```c
#include "kernel/osal/include/lock/osal_rwlock.h"
```

**功能说明**

- 释放写锁，退出写临界区
- 必须与 osal_rwlock_write_lock() 成对调用，确保锁的获取与释放匹配
- 释放写锁后，等待的读锁或写锁请求可能被唤醒
- 仅支持 Linux 内核模式

**前置条件**

- 调用上下文为 Linux 内核态（__KERNEL__ 宏已定义）
- 入参 rw_lock 指向的读写锁已通过 osal_rwlock_init() 初始化成功
- 当前线程已通过 osal_rwlock_write_lock() 获取写锁

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| rw_lock | osal_rwlock * | 指向已初始化的读写锁结构体指针 | 非NULL，且已初始化 |

### osal_rwlock_destory <a id="osal_rwlock_destory"></a>

```c
void osal_rwlock_destory(osal_rwlock *rw_lock)
```

**头文件清单**

```c
#include "kernel/osal/include/lock/osal_rwlock.h"
```

**功能说明**

- 销毁读写锁，释放相关资源
- 不再需要读写锁时调用此接口，防止资源泄漏
- 销毁前需确保无任何线程持有读锁或写锁
- 仅支持 Linux 内核模式

**前置条件**

- 调用上下文为 Linux 内核态（__KERNEL__ 宏已定义）
- 入参 rw_lock 指向的读写锁已通过 osal_rwlock_init() 初始化成功
- 当前无任何线程持有该读写锁的读锁或写锁

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| rw_lock | osal_rwlock * | 指向待销毁的读写锁结构体指针 | 非NULL，且已初始化 |

## Structures

### osal_rwlock <a id="struct_osal_rwlock"></a>

```c
typedef struct {
    void *rwlock;
} osal_rwlock;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| rwlock | void * | 底层读写锁实现指针，屏蔽不同操作系统差异 |



# Atomic

原子操作提供对整型变量的不可分割读写——无需互斥锁即可保证线程安全。支持原子读、写、加、减、自增、自减和条件测试操作。

使用场景：任务间共享计数、标志位等无需复杂锁保护的轻量级变量。比互斥锁开销更低，但不适合保护复杂数据结构。

**头文件清单**

```c
#include "kernel/osal/include/atomic/osal_atomic.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [osal_atomic_read](#osal_atomic_read) | 原子读取变量值 |
| [osal_atomic_set](#osal_atomic_set) | 原子设置变量值 |
| [osal_atomic_inc_return](#osal_atomic_inc_return) | 原子自增并返回结果 |
| [osal_atomic_add_return](#osal_atomic_add_return) | 原子加指定值并返回结果 |
| [osal_atomic_dec_return](#osal_atomic_dec_return) | 原子自减并返回结果 |
| [osal_atomic_inc](#osal_atomic_inc) | 原子自增 |
| [osal_atomic_sub](#osal_atomic_sub) | 原子减指定值 |
| [osal_atomic_dec](#osal_atomic_dec) | 原子自减 |
| [osal_atomic_add](#osal_atomic_add) | 原子加指定值 |
| [osal_atomic_dec_and_test](#osal_atomic_dec_and_test) | 原子自减并测试是否为0 |
| [osal_atomic_inc_and_test](#osal_atomic_inc_and_test) | 原子自增并测试是否为0 |
| [osal_atomic_inc_not_zero](#osal_atomic_inc_not_zero) | 非零时原子自增 |

## Functions

### osal_atomic_read <a id="osal_atomic_read"></a>

```c
int osal_atomic_read(osal_atomic *atomic)
```

**头文件清单**

```c
#include "kernel/osal/include/atomic/osal_atomic.h"
```

**功能说明**

- 实现原子读取操作，返回原子变量的当前值
- 读取过程通过关中断保证原子性，确保读取期间变量不被修改
- 适用于多线程/多任务环境下对共享计数器的安全读取
- 支持 linux、liteos、freertos 系统

**前置条件**

- 入参 atomic 不为 NULL，且指向已初始化的原子变量内存空间

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| atomic | osal_atomic * | 指向待读取的原子变量指针 | 非NULL，指向有效内存 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非负整数 | 原子变量当前值 | 读取成功 |
| -1 (OSAL_FAILURE) | 读取失败 | atomic 为 NULL |

### osal_atomic_set <a id="osal_atomic_set"></a>

```c
void osal_atomic_set(osal_atomic *atomic, int i)
```

**头文件清单**

```c
#include "kernel/osal/include/atomic/osal_atomic.h"
```

**功能说明**

- 实现原子设置操作，将原子变量的值设为指定值
- 设置过程通过关中断保证原子性，确保设置操作不被打断
- 适用于多线程/多任务环境下对共享计数器的安全赋值
- 支持 linux、liteos、freertos 系统

**前置条件**

- 入参 atomic 不为 NULL，且指向已初始化的原子变量内存空间

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| atomic | osal_atomic * | 指向待设置的原子变量指针 | 非NULL，指向有效内存 |
| i | int | 待设置的值 | int取值范围 |

### osal_atomic_inc_return <a id="osal_atomic_inc_return"></a>

```c
int osal_atomic_inc_return(osal_atomic *atomic)
```

**头文件清单**

```c
#include "kernel/osal/include/atomic/osal_atomic.h"
```

**功能说明**

- 实现原子自增操作并返回自增后的结果
- 自增过程通过关中断或底层原子操作保证原子性
- 适用于多线程/多任务环境下对共享计数器的安全递增并获取最新值
- 支持 linux、liteos、freertos 系统

**前置条件**

- 入参 atomic 不为 NULL，且指向已初始化的原子变量内存空间

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| atomic | osal_atomic * | 指向待自增的原子变量指针 | 非NULL，指向有效内存 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非负整数 | 自增后的原子变量值 | 自增成功 |
| -1 (OSAL_FAILURE) | 自增失败 | atomic 为 NULL |

### osal_atomic_add_return <a id="osal_atomic_add_return"></a>

```c
int osal_atomic_add_return(osal_atomic *atomic, int count)
```

**头文件清单**

```c
#include "kernel/osal/include/atomic/osal_atomic.h"
```

**功能说明**

- 实现原子加指定值操作并返回增加后的结果
- 加操作通过底层原子操作保证原子性
- 适用于多线程/多任务环境下对共享计数器安全增加指定值并获取最新值
- 仅支持 freertos 系统

**前置条件**

- 入参 atomic 不为 NULL，且指向已初始化的原子变量内存空间

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| atomic | osal_atomic * | 指向待修改的原子变量指针 | 非NULL，指向有效内存 |
| count | int | 待增加的值 | int取值范围 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 整数 | 增加后的原子变量值 | 操作成功 |

### osal_atomic_dec_return <a id="osal_atomic_dec_return"></a>

```c
int osal_atomic_dec_return(osal_atomic *atomic)
```

**头文件清单**

```c
#include "kernel/osal/include/atomic/osal_atomic.h"
```

**功能说明**

- 实现原子自减操作并返回自减后的结果
- 自减过程通过底层原子操作保证原子性
- 适用于多线程/多任务环境下对共享计数器的安全递减并获取最新值
- 支持 linux、liteos、freertos 系统

**前置条件**

- 入参 atomic 不为 NULL，且指向已初始化的原子变量内存空间

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| atomic | osal_atomic * | 指向待自减的原子变量指针 | 非NULL，指向有效内存 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非负整数 | 自减后的原子变量值 | 自减成功 |
| -1 (OSAL_FAILURE) | 自减失败 | atomic 为 NULL |

### osal_atomic_inc <a id="osal_atomic_inc"></a>

```c
void osal_atomic_inc(osal_atomic *atomic)
```

**头文件清单**

```c
#include "kernel/osal/include/atomic/osal_atomic.h"
```

**功能说明**

- 实现原子自增操作，不返回自增结果
- 自增过程通过关中断或底层原子操作保证原子性
- 适用于多线程/多任务环境下对共享计数器的安全递增，无需获取结果
- 支持 linux、liteos、freertos 系统

**前置条件**

- 入参 atomic 不为 NULL，且指向已初始化的原子变量内存空间

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| atomic | osal_atomic * | 指向待自增的原子变量指针 | 非NULL，指向有效内存 |

### osal_atomic_sub <a id="osal_atomic_sub"></a>

```c
void osal_atomic_sub(osal_atomic *atomic, unsigned int count)
```

**头文件清单**

```c
#include "kernel/osal/include/atomic/osal_atomic.h"
```

**功能说明**

- 实现原子减指定值操作，不返回结果
- 减操作通过底层原子操作保证原子性
- 适用于多线程/多任务环境下对共享计数器安全减少指定值
- 仅支持 freertos 系统

**前置条件**

- 入参 atomic 不为 NULL，且指向已初始化的原子变量内存空间

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| atomic | osal_atomic * | 指向待修改的原子变量指针 | 非NULL，指向有效内存 |
| count | unsigned int | 待减少的值 | unsigned int取值范围 |

### osal_atomic_dec <a id="osal_atomic_dec"></a>

```c
void osal_atomic_dec(osal_atomic *atomic)
```

**头文件清单**

```c
#include "kernel/osal/include/atomic/osal_atomic.h"
```

**功能说明**

- 实现原子自减操作，不返回自减结果
- 自减过程通过关中断或底层原子操作保证原子性
- 适用于多线程/多任务环境下对共享计数器的安全递减，无需获取结果
- 支持 linux、liteos、freertos 系统

**前置条件**

- 入参 atomic 不为 NULL，且指向已初始化的原子变量内存空间

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| atomic | osal_atomic * | 指向待自减的原子变量指针 | 非NULL，指向有效内存 |

### osal_atomic_add <a id="osal_atomic_add"></a>

```c
void osal_atomic_add(osal_atomic *atomic, int count)
```

**头文件清单**

```c
#include "kernel/osal/include/atomic/osal_atomic.h"
```

**功能说明**

- 实现原子加指定值操作，不返回结果
- 加操作通过关中断或底层原子操作保证原子性
- 适用于多线程/多任务环境下对共享计数器安全增加指定值
- 仅支持 freertos 系统

**前置条件**

- 入参 atomic 不为 NULL，且指向已初始化的原子变量内存空间

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| atomic | osal_atomic * | 指向待修改的原子变量指针 | 非NULL，指向有效内存 |
| count | int | 待增加的值 | int取值范围 |

### osal_atomic_dec_and_test <a id="osal_atomic_dec_and_test"></a>

```c
int osal_atomic_dec_and_test(osal_atomic *atomic)
```

**头文件清单**

```c
#include "kernel/osal/include/atomic/osal_atomic.h"
```

**功能说明**

- 对原子变量自减1，并检查结果是否为0，为0返回true，否则返回false
- 自减与判断过程通过底层原子操作保证原子性
- 适用于资源引用计数归零检测等场景
- 仅支持 linux 系统

**前置条件**

- 入参 atomic 不为 NULL，且指向已初始化的原子变量内存空间

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| atomic | osal_atomic * | 指向待自减并测试的原子变量指针 | 非NULL，指向有效内存 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非0 (true) | 自减后值为0 | 自减后原子变量值为0 |
| 0 (false) | 自减后值不为0 | 自减后原子变量值不为0 |

### osal_atomic_inc_and_test <a id="osal_atomic_inc_and_test"></a>

```c
int osal_atomic_inc_and_test(osal_atomic *atomic)
```

**头文件清单**

```c
#include "kernel/osal/include/atomic/osal_atomic.h"
```

**功能说明**

- 对原子变量自增1，并检查结果是否为0，为0返回true，否则返回false
- 自增与判断过程通过底层原子操作保证原子性
- 适用于溢出检测等特殊场景
- 仅支持 linux 系统

**前置条件**

- 入参 atomic 不为 NULL，且指向已初始化的原子变量内存空间

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| atomic | osal_atomic * | 指向待自增并测试的原子变量指针 | 非NULL，指向有效内存 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非0 (true) | 自增后值为0 | 自增后原子变量值为0（溢出场景） |
| 0 (false) | 自增后值不为0 | 自增后原子变量值不为0 |

### osal_atomic_inc_not_zero <a id="osal_atomic_inc_not_zero"></a>

```c
int osal_atomic_inc_not_zero(osal_atomic *atomic)
```

**头文件清单**

```c
#include "kernel/osal/include/atomic/osal_atomic.h"
```

**功能说明**

- 若原子变量非零，则原子自增1；若为零，则不自增
- 自增操作通过底层原子操作保证原子性
- 适用于引用计数场景，防止对已释放资源重新引用
- 仅支持 linux 系统

**前置条件**

- 入参 atomic 不为 NULL，且指向已初始化的原子变量内存空间

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| atomic | osal_atomic * | 指向待条件自增的原子变量指针 | 非NULL，指向有效内存 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非0 (true) | 自增操作已执行 | 原子变量非零，自增成功 |
| 0 (false) | 自增操作未执行 | 原子变量为零，未执行自增 |

## Structures

### osal_atomic <a id="struct_osal_atomic"></a>

```c
typedef struct {
    volatile int counter;
} osal_atomic;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| counter | volatile int | 原子计数器值，所有原子操作均基于此成员 |



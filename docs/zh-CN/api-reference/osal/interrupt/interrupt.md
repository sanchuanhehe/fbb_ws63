# Interrupt

硬件中断管理抽象层。提供中断请求/释放、使能/禁用、优先级设置和 CPU 亲和性配置功能。支持全局中断锁（`osal_irq_lock/unlock/restore`）。

同时提供 Tasklet（中断底半部）机制——将中断上半部中的耗时操作延迟到下半部执行，减少中断屏蔽时间。适用所有需要响应硬件中断的场景。

**头文件清单**

```c
#include "kernel/osal/include/interrupt/osal_interrupt.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [osal_irq_get_private_dev](#osal_irq_get_private_dev) | 获取中断回调函数的私有设备参数 |
| [osal_irq_request](#osal_irq_request) | 申请注册中断处理函数 |
| [osal_irq_free](#osal_irq_free) | 释放已注册的中断 |
| [osal_irq_set_priority](#osal_irq_set_priority) | 设置中断优先级 |
| [osal_irq_set_affinity](#osal_irq_set_affinity) | 设置中断的CPU亲和性 |
| [osal_irq_enable](#osal_irq_enable) | 使能指定中断 |
| [osal_irq_disable](#osal_irq_disable) | 禁用指定中断 |
| [osal_irq_lock](#osal_irq_lock) | 关闭全局中断 |
| [osal_irq_unlock](#osal_irq_unlock) | 打开全局中断 |
| [osal_irq_restore](#osal_irq_restore) | 恢复全局中断状态 |
| [osal_irq_clear](#osal_irq_clear) | 清除指定中断的pending状态 |
| [osal_in_interrupt](#osal_in_interrupt) | 判断当前是否处于中断上下文 |
| [osal_tasklet_init](#osal_tasklet_init) | 初始化tasklet |
| [osal_tasklet_schedule](#osal_tasklet_schedule) | 调度tasklet执行 |
| [osal_tasklet_kill](#osal_tasklet_kill) | 关闭tasklet |
| [osal_tasklet_update](#osal_tasklet_update) | 更新tasklet |

## Functions

### osal_irq_get_private_dev <a id="osal_irq_get_private_dev"></a>

```c
void *osal_irq_get_private_dev(void *param_dev)
```

**头文件清单**

```c
#include "kernel/osal/include/interrupt/osal_interrupt.h"
```

**功能说明**

- 获取中断回调函数中传入的私有设备参数
- 从中断回调参数中提取可直接使用的设备标识
- 适用于中断处理函数中需要访问设备私有数据的场景

**前置条件**

- 中断已通过 osal_irq_request 注册完成
- 入参 param_dev 不为 NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| param_dev | void * | 传递给中断回调函数的参数 | 非NULL |

**返回值**

- 返回类型：void *

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非NULL指针 | 设备私有数据指针 | 正常获取设备参数 |
| NULL | 获取失败 | param_dev为NULL |

### osal_irq_request <a id="osal_irq_request"></a>

```c
int osal_irq_request(unsigned int irq, osal_irq_handler handler, osal_irq_handler thread_fn, const char *name, void *dev)
```

**头文件清单**

```c
#include "kernel/osal/include/interrupt/osal_interrupt.h"
```

**功能说明**

- 申请注册中断处理函数，分配指定中断号
- 支持注册主中断处理函数（handler）和线程化中断处理函数（thread_fn）
- 若handler为NULL且thread_fn不为NULL，则安装默认主处理函数
- 若thread_fn为NULL，则不创建中断线程

**前置条件**

- 中断号 irq 未被其他模块占用
- 入参 handler 不为 NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| irq | unsigned int | 要申请的中断号 | 有效中断号 |
| handler | [osal_irq_handler](#typedef_osal_irq_handler) | 主中断处理函数，硬中断发生时调用 | 非NULL |
| thread_fn | [osal_irq_handler](#typedef_osal_irq_handler) | 线程化中断处理函数，若为NULL则不创建中断线程 | NULL或有效函数指针 |
| name | const char * | 申请设备的ASCII (American Standard Code for Information Interchange) 名称 | 有效字符串指针 |
| dev | void * | 传递回处理函数的设备标识 | Linux用户空间下类型必须为(drval_irq_arg *) |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 中断注册成功 | 中断申请成功 |
| OSAL_FAILURE(-1) | 中断注册失败 | handler为NULL或底层创建中断失败 |

### osal_irq_free <a id="osal_irq_free"></a>

```c
void osal_irq_free(unsigned int irq, void *dev)
```

**头文件清单**

```c
#include "kernel/osal/include/interrupt/osal_interrupt.h"
```

**功能说明**

- 释放已通过 osal_irq_request 注册的中断
- 释放指定中断号，使其可被重新分配
- 不可在中断上下文中调用

**前置条件**

- 指定中断已通过 osal_irq_request 注册
- Linux用户空间下 dev 参数必须与 osal_irq_request 中的 dev 参数一致

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| irq | unsigned int | 要释放的中断号 | 已注册的中断号 |
| dev | void * | 释放设备的标识 | Linux用户空间下须与注册时的dev一致 |

### osal_irq_set_priority <a id="osal_irq_set_priority"></a>

```c
int osal_irq_set_priority(unsigned int irq, unsigned short priority)
```

**头文件清单**

```c
#include "kernel/osal/include/interrupt/osal_interrupt.h"
```

**功能说明**

- 设置指定中断的优先级
- 依赖于中断控制器和CPU架构的硬件实现

**前置条件**

- 指定中断已注册
- 硬件中断控制器支持优先级设置

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| irq | unsigned int | 中断号 | 有效中断号 |
| priority | unsigned short | 优先级值 | 依硬件实现 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 设置成功 | 优先级设置成功 |
| OSAL_FAILURE(-1) | 设置失败 | 底层设置优先级失败 |

### osal_irq_set_affinity <a id="osal_irq_set_affinity"></a>

```c
int osal_irq_set_affinity(unsigned int irq, const char *name, int cpu_mask)
```

**头文件清单**

```c
#include "kernel/osal/include/interrupt/osal_interrupt.h"
```

**功能说明**

- 设置中断的CPU亲和性，指定中断在哪个CPU上处理
- 通过CPU掩码指定目标CPU

**前置条件**

- 指定中断已注册
- 系统支持多核CPU亲和性配置

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| irq | unsigned int | 中断号 | 有效中断号 |
| name | const char * | 中断名称 | 有效字符串指针 |
| cpu_mask | int | CPU掩码 | [OSAL_CPU_ALL](#OSAL_CPU_ALL)(0), [OSAL_CPU_0](#OSAL_CPU_0)(2), [OSAL_CPU_1](#OSAL_CPU_1)(4), [OSAL_CPU_2](#OSAL_CPU_2)(8), [OSAL_CPU_3](#OSAL_CPU_3)(16) |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 设置成功 | 亲和性设置成功 |

### osal_irq_enable <a id="osal_irq_enable"></a>

```c
void osal_irq_enable(unsigned int irq)
```

**头文件清单**

```c
#include "kernel/osal/include/interrupt/osal_interrupt.h"
```

**功能说明**

- 使能指定中断，撤销 osal_irq_disable 的效果
- 若与最后一次 disable 匹配，则恢复该中断线上中断的处理
- 若中断号未创建，则打印错误日志

**前置条件**

- 指定中断已通过 osal_irq_request 注册

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| irq | unsigned int | 要使能的中断号 | 已注册的中断号 |

### osal_irq_disable <a id="osal_irq_disable"></a>

```c
void osal_irq_disable(unsigned int irq)
```

**头文件清单**

```c
#include "kernel/osal/include/interrupt/osal_interrupt.h"
```

**功能说明**

- 禁用指定中断线
- 依赖于中断控制器的硬件实现

**前置条件**

- 指定中断已通过 osal_irq_request 注册

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| irq | unsigned int | 要禁用的中断号 | 已注册的中断号 |

### osal_irq_lock <a id="osal_irq_lock"></a>

```c
unsigned int osal_irq_lock(void)
```

**头文件清单**

```c
#include "kernel/osal/include/interrupt/osal_interrupt.h"
```

**功能说明**

- 关闭全局中断，禁用CPSR中所有IRQ (Interrupt Request) 和FIQ中断
- 返回关闭中断前的CPSR值，用于后续恢复
- 用于临界区保护，防止中断干扰

**前置条件**

- 无特殊前置条件

**返回值**

- 返回类型：unsigned int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| CPSR值 | 关闭中断前的CPSR状态值 | 全局中断关闭成功 |

**参考案例**

- [sfc_perf_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/sfc_perf/sfc_perf_demo.c)

### osal_irq_unlock <a id="osal_irq_unlock"></a>

```c
unsigned int osal_irq_unlock(void)
```

**头文件清单**

```c
#include "kernel/osal/include/interrupt/osal_interrupt.h"
```

**功能说明**

- 打开全局中断，使能CPSR中所有IRQ和FIQ中断
- 返回使能中断后的CPSR值

**前置条件**

- 无特殊前置条件

**返回值**

- 返回类型：unsigned int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| CPSR值 | 使能中断后的CPSR状态值 | 全局中断使能成功 |

### osal_irq_restore <a id="osal_irq_restore"></a>

```c
void osal_irq_restore(unsigned int irq_status)
```

**头文件清单**

```c
#include "kernel/osal/include/interrupt/osal_interrupt.h"
```

**功能说明**

- 恢复全局中断状态到 osal_irq_lock 调用前的值
- 用于与 osal_irq_lock 配对使用，恢复中断上下文

**前置条件**

- 必须在 osal_irq_lock 调用之后使用
- 入参 irq_status 应为 osal_irq_lock 的返回值

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| irq_status | unsigned int | osal_irq_lock 返回的CPSR值 | osal_irq_lock的返回值 |

**参考案例**

- [sfc_perf_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/sfc_perf/sfc_perf_demo.c)

### osal_irq_clear <a id="osal_irq_clear"></a>

```c
unsigned int osal_irq_clear(unsigned int vector)
```

**头文件清单**

```c
#include "kernel/osal/include/interrupt/osal_interrupt.h"
```

**功能说明**

- 清除指定中断的pending状态
- 用于清除中断挂起标志，防止重复触发

**前置条件**

- 指定中断已注册

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| vector | unsigned int | 中断向量号 | 有效中断向量号 |

**返回值**

- 返回类型：unsigned int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 清除成功 | 中断pending状态清除成功 |
| OSAL_FAILURE(-1) | 清除失败 | 底层清除中断失败 |

### osal_in_interrupt <a id="osal_in_interrupt"></a>

```c
int osal_in_interrupt(void)
```

**头文件清单**

```c
#include "kernel/osal/include/interrupt/osal_interrupt.h"
```

**功能说明**

- 判断当前是否处于中断上下文中
- 检查当前是否在硬中断、软中断、不可屏蔽中断上下文中
- 用于区分中断上下文与任务上下文，决定是否可执行阻塞操作

**前置条件**

- 无特殊前置条件

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| false(0) | 不在中断上下文 | 当前处于任务上下文 |
| true(非0) | 在中断上下文 | 当前处于硬中断、软中断或不可屏蔽中断上下文 |

### osal_tasklet_init <a id="osal_tasklet_init"></a>

```c
int osal_tasklet_init(osal_tasklet *tasklet)
```

**头文件清单**

```c
#include "kernel/osal/include/interrupt/osal_interrupt.h"
```

**功能说明**

- 初始化tasklet结构体
- 在调用此接口前，须对 osal_tasklet 的 handler 和 data 成员赋值，tasklet 成员留空

**前置条件**

- tasklet 结构体的 handler 和 data 成员已赋值
- tasklet 成员留空

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| tasklet | [osal_tasklet](#struct_osal_tasklet) * | 待初始化的tasklet结构体指针 | 非NULL，handler和data已赋值 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 初始化成功 | tasklet初始化完成 |

### osal_tasklet_schedule <a id="osal_tasklet_schedule"></a>

```c
int osal_tasklet_schedule(osal_tasklet *tasklet)
```

**头文件清单**

```c
#include "kernel/osal/include/interrupt/osal_interrupt.h"
```

**功能说明**

- 将tasklet添加到调度队列并启动执行
- tasklet在同一时刻只在一个CPU上执行

**前置条件**

- tasklet已通过 osal_tasklet_init 初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| tasklet | [osal_tasklet](#struct_osal_tasklet) * | 待调度的tasklet结构体指针 | 已初始化的tasklet |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 调度成功 | tasklet已加入调度队列 |

### osal_tasklet_kill <a id="osal_tasklet_kill"></a>

```c
int osal_tasklet_kill(osal_tasklet *tasklet)
```

**头文件清单**

```c
#include "kernel/osal/include/interrupt/osal_interrupt.h"
```

**功能说明**

- 关闭tasklet，停止其执行
- 确保tasklet不再被调度执行

**前置条件**

- tasklet已通过 osal_tasklet_init 初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| tasklet | [osal_tasklet](#struct_osal_tasklet) * | 待关闭的tasklet结构体指针 | 已初始化的tasklet |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 关闭成功 | tasklet已关闭 |

### osal_tasklet_update <a id="osal_tasklet_update"></a>

```c
int osal_tasklet_update(osal_tasklet *tasklet)
```

**头文件清单**

```c
#include "kernel/osal/include/interrupt/osal_interrupt.h"
```

**功能说明**

- 更新tasklet配置
- 须在 osal_tasklet_init 之后调用

**前置条件**

- tasklet已通过 osal_tasklet_init 初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| tasklet | [osal_tasklet](#struct_osal_tasklet) * | 待更新的tasklet结构体指针 | 已初始化的tasklet |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 更新成功 | tasklet更新完成 |

## Type definitions

### osal_irq_handler <a id="typedef_osal_irq_handler"></a>

```c
typedef int (*osal_irq_handler)(int, void *);
```

**使用说明**

中断处理函数类型，用于 osal_irq_request 的 handler 和 thread_fn 参数，以及 osal_irq_get_private_dev 回调参数的关联类型。

## Enumerations

### osal_irqreturn <a id="enum_osal_irqreturn"></a>

```c
enum osal_irqreturn {
    OSAL_IRQ_NONE = (0 << 0),
    OSAL_IRQ_HANDLED = (1 << 0),
    OSAL_IRQ_WAKE_THREAD = (1 << 1),
};
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| OSAL_IRQ_NONE | 0 | 中断未被处理 |
| OSAL_IRQ_HANDLED | 1 | 中断已被处理 |
| OSAL_IRQ_WAKE_THREAD | 2 | 唤醒中断线程处理 |

## Structures

### osal_tasklet <a id="struct_osal_tasklet"></a>

```c
typedef struct {
    void *tasklet;
    void (*handler)(unsigned long data);
    unsigned long data;
} osal_tasklet;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| tasklet | void * | tasklet内部实现指针，初始化前留空 |
| handler | void (*)(unsigned long data) | tasklet处理函数指针 |
| data | unsigned long | 传递给handler的参数数据 |

## Macros

### OSAL_CPU_ALL <a id="OSAL_CPU_ALL"></a>

```c
#define OSAL_CPU_ALL    0
```

### OSAL_CPU_0 <a id="OSAL_CPU_0"></a>

```c
#define OSAL_CPU_0    (1 << 1)
```

### OSAL_CPU_1 <a id="OSAL_CPU_1"></a>

```c
#define OSAL_CPU_1    (1 << 2)
```

### OSAL_CPU_2 <a id="OSAL_CPU_2"></a>

```c
#define OSAL_CPU_2    (1 << 3)
```

### OSAL_CPU_3 <a id="OSAL_CPU_3"></a>

```c
#define OSAL_CPU_3    (1 << 4)
```


# Time

OS (Operating System) 定时器与时间管理。支持创建/启动/停止/修改/销毁软件定时器，回调在任务上下文中执行（非 ISR (Interrupt Service Routine)）。同时提供高精度定时器（hrtimer）和时间转换工具（ms↔jiffies 互转、sched_clock 纳秒级计时）。

定时器是一次性或周期性的，超时后自动触发回调。与硬件 Timer（ISR 上下文）不同，OS 定时器适合较长周期（≥ 1 个 tick）的定时任务。

**头文件清单**

```c
#include "kernel/osal/include/time/osal_timer.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [osal_timer_init](#osal_timer_init) | 初始化定时器 |
| [osal_timer_start](#osal_timer_start) | 启动定时器 |
| [osal_timer_mod](#osal_timer_mod) | 修改定时器超时时间 |
| [osal_timer_start_on](#osal_timer_start_on) | 在指定CPU上启动定时器 |
| [osal_timer_stop](#osal_timer_stop) | 停止定时器 |
| [osal_timer_destroy](#osal_timer_destroy) | 销毁定时器 |
| [osal_timer_get_private_data](#osal_timer_get_private_data) | 获取定时器回调函数的私有数据 |
| [osal_timer_destroy_sync](#osal_timer_destroy_sync) | 同步销毁定时器并等待回调完成 |
| [osal_sched_clock](#osal_sched_clock) | 获取系统时间（纳秒） |
| [osal_get_jiffies](#osal_get_jiffies) | 获取系统Tick/jiffies数 |
| [osal_msecs_to_jiffies](#osal_msecs_to_jiffies) | 毫秒转换为Tick/jiffies |
| [osal_jiffies_to_msecs](#osal_jiffies_to_msecs) | Tick/jiffies转换为毫秒 |
| [osal_get_cycle_per_tick](#osal_get_cycle_per_tick) | 获取单个Tick对应的时钟周期数 |
| [osal_gettimeofday](#osal_gettimeofday) | 获取当前系统内核时间 |
| [osal_hrtimer_create](#osal_hrtimer_create) | 创建高精度定时器 |
| [osal_hrtimer_start](#osal_hrtimer_start) | 启动高精度定时器 |
| [osal_hrtimer_destroy](#osal_hrtimer_destroy) | 销毁高精度定时器 |

## Functions

### osal_timer_init <a id="osal_timer_init"></a>

```c
int osal_timer_init(osal_timer *timer)
```

**头文件清单**

```c
#include "kernel/osal/include/time/osal_timer.h"
```

**功能说明**

- 初始化定时器，创建内核定时器资源
- 调用前须对 timer->handler 和 timer->data 赋值，初始化后不可再修改
- timer->interval 指定定时器超时时间，单位为毫秒

**前置条件**

- timer 指针不为 NULL，且 timer->handler 不为 NULL
- timer->timer 须为 NULL（未初始化状态）
- timer->interval 对应的 Tick 数须大于 0

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| timer | [osal_timer](#struct_osal_timer) * | 待初始化的定时器结构体指针 | 非NULL，timer->handler非NULL，timer->timer为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 初始化成功 | 定时器创建成功 |
| OSAL_FAILURE(-1) | 初始化失败 | 参数无效或内核定时器创建失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| LOSCFG_COMPAT_LINUX_TIMER | 特性宏 | 支持 OSAL (Operating System Abstraction Layer) Timer 接口功能 | n |

### osal_timer_start <a id="osal_timer_start"></a>

```c
int osal_timer_start(osal_timer *timer)
```

**头文件清单**

```c
#include "kernel/osal/include/time/osal_timer.h"
```

**功能说明**

- 启动已初始化的定时器
- 定时器超时后，内核将执行回调函数
- 若定时器已在运行，调用此接口将重新启动定时器

**前置条件**

- timer 指针不为 NULL
- 定时器已通过 osal_timer_init() 初始化成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| timer | [osal_timer](#struct_osal_timer) * | 待启动的定时器结构体指针 | 非NULL，已初始化 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 启动成功 | 定时器启动成功 |
| OSAL_FAILURE(-1) | 启动失败 | 参数无效或内核定时器启动失败 |

### osal_timer_mod <a id="osal_timer_mod"></a>

```c
int osal_timer_mod(osal_timer *timer, unsigned int interval)
```

**头文件清单**

```c
#include "kernel/osal/include/time/osal_timer.h"
```

**功能说明**

- 修改定时器超时时间，若定时器未激活则激活
- 修改后定时器将按新的超时时间重新启动
- 效率高于先停止再重新创建

**前置条件**

- timer 指针不为 NULL，且 timer->handler 不为 NULL
- interval 对应的 Tick 数须大于 0

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| timer | [osal_timer](#struct_osal_timer) * | 待修改的定时器结构体指针 | 非NULL，timer->handler非NULL |
| interval | unsigned int | 新的超时时间 | 单位：ms，对应Tick数须大于0 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 修改成功 | 定时器超时时间修改并重启成功 |
| OSAL_FAILURE(-1) | 修改失败 | 参数无效或内核操作失败 |

### osal_timer_start_on <a id="osal_timer_start_on"></a>

```c
int osal_timer_start_on(osal_timer *timer, unsigned long delay, int cpu)
```

**头文件清单**

```c
#include "kernel/osal/include/time/osal_timer.h"
```

**功能说明**

- 在指定CPU上启动定时器
- 支持 SMP (Security Manager Protocol) 场景下的定时器绑定

**前置条件**

- timer 指针不为 NULL
- 定时器已通过 osal_timer_init() 初始化成功
- 仅支持 Linux 系统

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| timer | [osal_timer](#struct_osal_timer) * | 待启动的定时器结构体指针 | 非NULL，已初始化 |
| delay | unsigned long | 延迟时间 | - |
| cpu | int | 目标CPU编号 | - |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 启动成功 | 定时器在指定CPU上启动成功 |
| OSAL_FAILURE(-1) | 启动失败 | 启动失败 |

### osal_timer_stop <a id="osal_timer_stop"></a>

```c
int osal_timer_stop(osal_timer *timer)
```

**头文件清单**

```c
#include "kernel/osal/include/time/osal_timer.h"
```

**功能说明**

- 停止定时器，对活动和非活动定时器均有效
- 停止处于活动状态的定时器返回 1，停止已停止的定时器返回 0

**前置条件**

- timer 指针不为 NULL
- 定时器已通过 osal_timer_init() 初始化成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| timer | [osal_timer](#struct_osal_timer) * | 待停止的定时器结构体指针 | 非NULL，已初始化 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 1(1) | 停止成功，定时器处于活动状态 | 定时器正在运行时被停止（仅 Linux 和 LiteOS (Huawei LiteOS) 支持） |
| OSAL_SUCCESS(0) | 停止成功，定时器已停止 | 定时器已处于停止状态 |
| OSAL_FAILURE(-1) | 停止失败 | 内核定时器停止操作失败 |

### osal_timer_destroy <a id="osal_timer_destroy"></a>

```c
int osal_timer_destroy(osal_timer *timer)
```

**头文件清单**

```c
#include "kernel/osal/include/time/osal_timer.h"
```

**功能说明**

- 销毁定时器，释放内核定时器资源
- 模块退出时必须调用此接口释放定时器，否则将导致内存泄漏

**前置条件**

- timer 指针不为 NULL
- 定时器已通过 osal_timer_init() 初始化成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| timer | [osal_timer](#struct_osal_timer) * | 待销毁的定时器结构体指针 | 非NULL，已初始化 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 销毁成功 | 定时器销毁成功 |
| OSAL_FAILURE(-1) | 销毁失败 | 参数无效或内核定时器删除失败 |

### osal_timer_get_private_data <a id="osal_timer_get_private_data"></a>

```c
unsigned long osal_timer_get_private_data(const void *sys_data)
```

**头文件清单**

```c
#include "kernel/osal/include/time/osal_timer.h"
```

**功能说明**

- 在定时器回调函数中获取可用的私有数据参数
- 定时器回调函数的参数不能直接使用，需通过此接口转换后获取
- 将系统传入的回调参数转换为用户初始化时设置的 data 值

**前置条件**

- sys_data 为定时器回调函数的入参

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| sys_data | const void * | 传递给回调函数的参数 | 非NULL |

**返回值**

- 返回类型：unsigned long

&emsp;&emsp;返回可直接使用的私有数据参数值。

### osal_timer_destroy_sync <a id="osal_timer_destroy_sync"></a>

```c
int osal_timer_destroy_sync(osal_timer *timer)
```

**头文件清单**

```c
#include "kernel/osal/include/time/osal_timer.h"
```

**功能说明**

- 同步销毁定时器，停止定时器并等待回调函数执行完成
- 与 osal_timer_destroy 的区别在于 SMP 场景下可确保其他 CPU 上的回调已执行完毕
- 仅支持 Linux 系统

**前置条件**

- timer 指针不为 NULL
- 调用者须确保定时器不会被重新启动
- 不可在中断上下文中调用（irqsafe 定时器除外）
- 调用者不可持有阻止回调完成的锁
- 定时器回调不可调用 add_timer_on()

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| timer | [osal_timer](#struct_osal_timer) * | 待同步销毁的定时器结构体指针 | 非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 销毁成功 | 定时器同步销毁成功 |
| OSAL_FAILURE(-1) | 销毁失败 | 销毁失败 |

### osal_sched_clock <a id="osal_sched_clock"></a>

```c
unsigned long long osal_sched_clock(void)
```

**头文件清单**

```c
#include "kernel/osal/include/time/osal_timer.h"
```

**功能说明**

- 获取系统时间，单位为纳秒
- 仅支持 Linux 和 LiteOS 系统

**返回值**

- 返回类型：unsigned long long

&emsp;&emsp;返回当前系统时间（纳秒）。

### osal_get_jiffies <a id="osal_get_jiffies"></a>

```c
unsigned long long osal_get_jiffies(void)
```

**头文件清单**

```c
#include "kernel/osal/include/time/osal_timer.h"
```

**功能说明**

- 获取系统 Tick 数（LiteOS）或 jiffies 数（Linux）
- 支持 Linux、LiteOS、FreeRTOS (Free Real-Time Operating System) 系统

**返回值**

- 返回类型：unsigned long long

&emsp;&emsp;返回系统 Tick/jiffies 数。

### osal_msecs_to_jiffies <a id="osal_msecs_to_jiffies"></a>

```c
unsigned long osal_msecs_to_jiffies(const unsigned int m)
```

**头文件清单**

```c
#include "kernel/osal/include/time/osal_timer.h"
```

**功能说明**

- 将毫秒时间转换为 Tick/jiffies 数
- 支持 Linux、LiteOS、FreeRTOS 系统

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| m | const unsigned int | 待转换的毫秒时间 | 单位：ms |

**返回值**

- 返回类型：unsigned long

&emsp;&emsp;返回转换后的 Tick/jiffies 数。

### osal_jiffies_to_msecs <a id="osal_jiffies_to_msecs"></a>

```c
unsigned int osal_jiffies_to_msecs(const unsigned int n)
```

**头文件清单**

```c
#include "kernel/osal/include/time/osal_timer.h"
```

**功能说明**

- 将 Tick/jiffies 数转换为毫秒时间
- 若转换结果超过 0xFFFFFFFF，返回 0xFFFFFFFF
- 支持 Linux、LiteOS、FreeRTOS 系统

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| n | const unsigned int | 待转换的 Tick/jiffies 数 | - |

**返回值**

- 返回类型：unsigned int

&emsp;&emsp;返回转换后的毫秒时间，若超过 0xFFFFFFFF 则返回 0xFFFFFFFF。

### osal_get_cycle_per_tick <a id="osal_get_cycle_per_tick"></a>

```c
unsigned int osal_get_cycle_per_tick(void)
```

**头文件清单**

```c
#include "kernel/osal/include/time/osal_timer.h"
```

**功能说明**

- 获取单个 Tick 对应的时钟周期数
- 仅支持 LiteOS 系统

**返回值**

- 返回类型：unsigned int

&emsp;&emsp;返回单个 Tick 对应的时钟周期数。

### osal_gettimeofday <a id="osal_gettimeofday"></a>

```c
void osal_gettimeofday(osal_timeval *tv)
```

**头文件清单**

```c
#include "kernel/osal/include/time/osal_timer.h"
```

**功能说明**

- 获取当前系统内核时间
- 支持 Linux、LiteOS、FreeRTOS 系统

**前置条件**

- tv 指针不为 NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| tv | [osal_timeval](#struct_osal_timeval) * | 输出时间结构体指针 | 非NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| tv | [osal_timeval](#struct_osal_timeval) * | 当前系统内核时间，包含秒和微秒 |

### osal_hrtimer_create <a id="osal_hrtimer_create"></a>

```c
int osal_hrtimer_create(osal_hrtimer *hrtimer)
```

**头文件清单**

```c
#include "kernel/osal/include/time/osal_timer.h"
```

**功能说明**

- 创建高精度定时器并初始化定时器参数
- 调用前须对 hrtimer->handler 和 hrtimer->interval 赋值，创建后不可再修改
- 模块退出时必须调用 osal_hrtimer_destroy 释放定时器，否则将导致内存泄漏
- 仅支持 LiteOS 系统

**前置条件**

- hrtimer 指针不为 NULL，且 hrtimer->timer 须为 NULL
- hrtimer->handler 已赋值
- hrtimer->interval 不得超过 ULONG_MAX / 1000

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| hrtimer | [osal_hrtimer](#struct_osal_hrtimer) * | 待创建的高精度定时器结构体指针 | 非NULL，hrtimer->timer为NULL，hrtimer->handler非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 创建成功 | 高精度定时器创建成功 |
| OSAL_FAILURE(-1) | 创建失败 | 参数无效或内核定时器创建失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| LOSCFG_COMPAT_LINUX_HRTIMER | 特性宏 | 支持 OSAL HRTimer 接口功能 | n |

### osal_hrtimer_start <a id="osal_hrtimer_start"></a>

```c
int osal_hrtimer_start(osal_hrtimer *hrtimer)
```

**头文件清单**

```c
#include "kernel/osal/include/time/osal_timer.h"
```

**功能说明**

- 启动高精度定时器，将定时器节点加入全局链表并开始计时
- 仅支持 LiteOS 系统

**前置条件**

- hrtimer 指针不为 NULL，且 hrtimer->timer 不为 NULL
- 高精度定时器已通过 osal_hrtimer_create() 创建成功
- hrtimer->interval 不得超过 ULONG_MAX / 1000

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| hrtimer | [osal_hrtimer](#struct_osal_hrtimer) * | 待启动的高精度定时器结构体指针 | 非NULL，hrtimer->timer非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0(0) | 启动成功 | 高精度定时器启动成功 |
| -1(-1) | 启动失败 | 高精度定时器启动失败 |
| 1(1) | 定时器节点已在链表中 | 高精度定时器节点已存在于链表 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| LOSCFG_COMPAT_LINUX_HRTIMER | 特性宏 | 支持 OSAL HRTimer 接口功能 | n |

### osal_hrtimer_destroy <a id="osal_hrtimer_destroy"></a>

```c
int osal_hrtimer_destroy(osal_hrtimer *hrtimer)
```

**头文件清单**

```c
#include "kernel/osal/include/time/osal_timer.h"
```

**功能说明**

- 销毁高精度定时器，取消定时器并释放内存资源
- 若指针为 NULL 或定时器节点不存在，则销毁失败
- 仅支持 LiteOS 系统

**前置条件**

- hrtimer 指针不为 NULL，且 hrtimer->timer 不为 NULL
- 高精度定时器已通过 osal_hrtimer_create() 创建成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| hrtimer | [osal_hrtimer](#struct_osal_hrtimer) * | 待销毁的高精度定时器结构体指针 | 非NULL，hrtimer->timer非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 销毁成功 | 高精度定时器销毁成功 |
| OSAL_FAILURE(-1) | 销毁失败 | 参数无效 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| LOSCFG_COMPAT_LINUX_HRTIMER | 特性宏 | 支持 OSAL HRTimer 接口功能 | n |

## Enumerations

### osal_hrtimer_restart <a id="enum_osal_hrtimer_restart"></a>

```c
typedef enum {
    OSAL_HRTIMER_NORESTART,
    OSAL_HRTIMER_RESTART
} osal_hrtimer_restart;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| OSAL_HRTIMER_NORESTART | 0 | 定时器不复位 |
| OSAL_HRTIMER_RESTART | 1 | 定时器须复位 |

## Structures

### osal_timer <a id="struct_osal_timer"></a>

```c
typedef struct {
    void *timer;
    void (*handler)(unsigned long);
    unsigned long data;    // data for handler
    unsigned int interval; // timer timing duration, unit: ms.
} osal_timer;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| timer | void * | 内核定时器句柄，初始化前须置 NULL |
| handler | void (*)(unsigned long) | 定时器超时回调函数指针 |
| data | unsigned long | 传递给回调函数的私有数据 |
| interval | unsigned int | 定时器超时时间，单位：ms |

### osal_timeval <a id="struct_osal_timeval"></a>

```c
typedef struct {
    long tv_sec;
    long tv_usec;
} osal_timeval;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| tv_sec | long | 秒 |
| tv_usec | long | 微秒 |

### osal_rtc_time <a id="struct_osal_rtc_time"></a>

```c
typedef struct {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
} osal_rtc_time;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| tm_sec | int | 秒 |
| tm_min | int | 分 |
| tm_hour | int | 时 |
| tm_mday | int | 月内日 |
| tm_mon | int | 月 |
| tm_year | int | 年 |
| tm_wday | int | 周内日 |
| tm_yday | int | 年内日 |
| tm_isdst | int | 夏令时标志 |

### osal_hrtimer <a id="struct_osal_hrtimer"></a>

```c
typedef struct osal_hrtimer {
    void *timer;
    osal_hrtimer_restart (*handler)(void *timer);
    unsigned long interval; /* Unit ms */
} osal_hrtimer;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| timer | void * | 内核高精度定时器句柄，创建前须置 NULL |
| handler | osal_hrtimer_restart (*)(void *timer) | 高精度定时器超时回调函数指针 |
| interval | unsigned long | 定时器超时时间，单位：ms |



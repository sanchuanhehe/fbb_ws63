# Task

任务管理是 OSAL (Operating System Abstraction Layer) 的核心调度模块。支持创建/删除任务、挂起/恢复任务、设置/获取优先级、调度锁/解锁和延时操作。

**任务/线程**：RTOS (Real-Time Operating System) 中独立执行的调度单元，每个任务有自己的栈空间和优先级。

**优先级**：数字越小优先级越高（`OSAL_TASK_PRIORITY_HIGH` = 3 高于 `OSAL_TASK_PRIORITY_LOW` = 10）。高优先级任务就绪时立即抢占低优先级任务。

**调度锁**：`osal_kthread_lock/unlock()` 临时禁止任务调度，用于保护任务间共享数据。不禁止中断——仅影响任务切换。

**延时 vs 空转**：`osal_msleep()` 让出 CPU 给其他任务（期间 CPU 可休眠），而裸机 `delay()` 是 CPU 空转。

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_task.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [osal_kthread_create](#osal_kthread_create) | 创建内核线程 |
| [osal_kthread_set_priority](#osal_kthread_set_priority) | 设置线程优先级 |
| [osal_kthread_set_affinity](#osal_kthread_set_affinity) | 设置线程CPU亲和性 |
| [osal_kthread_should_stop](#osal_kthread_should_stop) | 检查线程是否应停止运行 |
| [osal_kthread_wakeup_process](#osal_kthread_wakeup_process) | 唤醒指定线程 |
| [osal_kthread_bind](#osal_kthread_bind) | 绑定线程到指定CPU核心 |
| [osal_kthread_lock](#osal_kthread_lock) | 锁定任务调度 |
| [osal_kthread_unlock](#osal_kthread_unlock) | 解锁任务调度 |
| [osal_kthread_destroy](#osal_kthread_destroy) | 销毁已创建的线程 |
| [osal_kthread_schedule](#osal_kthread_schedule) | 线程进入不可中断睡眠状态 |
| [osal_kthread_set_uninterrupt](#osal_kthread_set_uninterrupt) | 设置当前线程为不可中断状态 |
| [osal_kthread_set_running](#osal_kthread_set_running) | 设置当前线程为可运行状态 |
| [osal_cond_resched](#osal_cond_resched) | 主动让出CPU资源 |
| [osal_schedule](#osal_schedule) | 将当前任务放回就绪队列并触发调度 |
| [osal_kneon_begin](#osal_kneon_begin) | 启用NEON (ARM (Advanced RISC (RISC-V) Machines) NEON) 算法加速 |
| [osal_kneon_end](#osal_kneon_end) | 禁用NEON算法加速 |
| [osal_yield](#osal_yield) | 释放当前线程CPU时间片 |
| [osal_get_current_pid](#osal_get_current_pid) | 获取当前线程的PID (Process ID / Parameter ID) |
| [osal_get_current_tid](#osal_get_current_tid) | 获取当前线程的TID |
| [osal_get_current_tgid](#osal_get_current_tgid) | 获取当前线程的TGID |
| [osal_get_current_taskname](#osal_get_current_taskname) | 获取当前线程名称 |
| [osal_msleep](#osal_msleep) | 线程休眠（毫秒） |
| [osal_msleep_uninterruptible](#osal_msleep_uninterruptible) | 线程不可中断休眠（毫秒） |
| [osal_udelay](#osal_udelay) | 忙等待延时（微秒） |
| [osal_mdelay](#osal_mdelay) | 忙等待延时（毫秒） |
| [osal_kthread_suspend](#osal_kthread_suspend) | 挂起指定任务 |
| [osal_kthread_resume](#osal_kthread_resume) | 恢复已挂起的任务 |

## Functions

### osal_kthread_create <a id="osal_kthread_create"></a>

```c
osal_task * osal_kthread_create(osal_kthread_handler handler, void *data, const char *name, unsigned int stack_size)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_task.h"
```

**功能说明**

- 创建内核线程并启动运行
- 若待创建线程的栈大小小于或等于MINIMAL_STACK_SIZE，则将stack_size设置为MINIMAL_STACK_SIZE作为默认栈大小
- 栈大小需足够大以避免任务栈溢出

**前置条件**

- handler参数不为NULL
- 系统内存资源充足，可分配线程控制块与栈空间

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| handler | [osal_kthread_handler](#typedef_osal_kthread_handler) | 线程入口函数 | 非NULL，函数签名须为int (*)(void *) |
| data | void * | 传递给线程入口函数的参数 | 可为NULL |
| name | const char * | 线程名称 | 非NULL，用于标识线程 |
| stack_size | unsigned int | 线程栈空间大小（字节） | 大于0；若小于等于MINIMAL_STACK_SIZE则使用默认值 |

**返回值**

- 返回类型：osal_task *

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非NULL指针 | 线程创建成功 | 线程资源分配成功且任务创建成功 |
| NULL | 线程创建失败 | handler为NULL、内存分配失败或任务创建失败 |

**参考案例**

- [ble_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_client/src/ble_speed_client.c)

### osal_kthread_set_priority <a id="osal_kthread_set_priority"></a>

```c
int osal_kthread_set_priority(osal_task *task, unsigned int priority)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_task.h"
```

**功能说明**

- 设置指定线程的优先级
- 优先级数值越小，线程优先级越高
- 需配合osal_kthread_lock/osal_kthread_unlock使用以保证设置过程原子性

**前置条件**

- task参数不为NULL
- 目标线程已通过osal_kthread_create创建成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| task | [osal_task](#struct_osal_task) * | 待设置优先级的线程 | 非NULL，须由osal_kthread_create返回 |
| priority | unsigned int | 优先级数值 | [OSAL_TASK_PRIORITY_ABOVE_HIGH](#OSAL_TASK_PRIORITY_ABOVE_HIGH)(2)、[OSAL_TASK_PRIORITY_HIGH](#OSAL_TASK_PRIORITY_HIGH)(3)、[OSAL_TASK_PRIORITY_BELOW_HIGH](#OSAL_TASK_PRIORITY_BELOW_HIGH)(4)、[OSAL_TASK_PRIORITY_ABOVE_MIDDLE](#OSAL_TASK_PRIORITY_ABOVE_MIDDLE)(5)、[OSAL_TASK_PRIORITY_MIDDLE](#OSAL_TASK_PRIORITY_MIDDLE)(6)、[OSAL_TASK_PRIORITY_BELOW_MIDDLE](#OSAL_TASK_PRIORITY_BELOW_MIDDLE)(7)、[OSAL_TASK_PRIORITY_ABOVE_LOW](#OSAL_TASK_PRIORITY_ABOVE_LOW)(8)、[OSAL_TASK_PRIORITY_LOW](#OSAL_TASK_PRIORITY_LOW)(10)、[OSAL_TASK_PRIORITY_BELOW_LOW](#OSAL_TASK_PRIORITY_BELOW_LOW)(11) |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 设置成功 | 优先级设置成功 |
| OSAL_FAILURE(-1) | 设置失败 | task为NULL或底层优先级设置失败 |

**参考案例**

- [ble_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_client/src/ble_speed_client.c)

### osal_kthread_set_affinity <a id="osal_kthread_set_affinity"></a>

```c
void osal_kthread_set_affinity(osal_task *task, int cpu_mask)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_task.h"
```

**功能说明**

- 设置指定线程的CPU亲和性，约束线程运行在特定CPU核心上
- 仅在多核系统中有实际意义

**前置条件**

- task参数不为NULL
- 目标线程已创建且系统支持多核调度

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| task | [osal_task](#struct_osal_task) * | 待设置亲和性的线程 | 非NULL |
| cpu_mask | int | CPU掩码 | OSAL_CPU_ALL(0)、OSAL_CPU_0(2)、OSAL_CPU_1(4)、OSAL_CPU_2(8)、OSAL_CPU_3(16) |

### osal_kthread_should_stop <a id="osal_kthread_should_stop"></a>

```c
int osal_kthread_should_stop(void)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_task.h"
```

**功能说明**

- 检查内核线程是否应停止运行
- 线程函数中可循环调用此接口判断是否需要退出
- 配合osal_kthread_destroy使用

**前置条件**

- 调用上下文为内核线程函数内部

**入参**

当前接口无入参

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 线程继续运行 | 线程未被请求停止 |
| 1 | 线程应停止 | 线程被kthread_stop请求停止 |

### osal_kthread_wakeup_process <a id="osal_kthread_wakeup_process"></a>

```c
int osal_kthread_wakeup_process(osal_task *task)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_task.h"
```

**功能说明**

- 唤醒指定线程
- 仅对处于睡眠/等待状态的线程有效

**前置条件**

- task参数不为NULL
- 目标线程处于可唤醒状态

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| task | [osal_task](#struct_osal_task) * | 待唤醒的线程 | 非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 唤醒成功 | 线程成功被唤醒 |
| -1 | 唤醒失败 | 线程唤醒失败 |

### osal_kthread_bind <a id="osal_kthread_bind"></a>

```c
void osal_kthread_bind(osal_task *task, unsigned int cpu)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_task.h"
```

**功能说明**

- 将已创建的线程绑定到指定CPU核心上运行
- 绑定后线程仅在被绑定的CPU核心上调度执行

**前置条件**

- task参数不为NULL
- 目标线程已创建且系统支持多核

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| task | [osal_task](#struct_osal_task) * | 待绑定的线程 | 非NULL |
| cpu | unsigned int | 绑定的CPU编号 | 有效的CPU核心编号 |

### osal_kthread_lock <a id="osal_kthread_lock"></a>

```c
void osal_kthread_lock(void)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_task.h"
```

**功能说明**

- 锁定任务调度，调用后任务切换不会发生
- 若任务调度被锁定但中断未禁用，任务仍可被中断
- 每次调用锁定计数加1，需与osal_kthread_unlock配对使用
- 常用于线程创建过程中保证原子性

**前置条件**

- 需与osal_kthread_unlock配对使用
- 调用次数与解锁次数须一致

**入参**

当前接口无入参

**参考案例**

- [ble_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_client/src/ble_speed_client.c)

### osal_kthread_unlock <a id="osal_kthread_unlock"></a>

```c
void osal_kthread_unlock(void)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_task.h"
```

**功能说明**

- 解锁任务调度，调用后锁定计数减1
- 当锁定计数降为0时，任务调度恢复
- 需与osal_kthread_lock配对使用

**前置条件**

- 前序已调用osal_kthread_lock
- 解锁次数不超过锁定次数

**入参**

当前接口无入参

**参考案例**

- [ble_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_client/src/ble_speed_client.c)

### osal_kthread_destroy <a id="osal_kthread_destroy"></a>

```c
void osal_kthread_destroy(osal_task *task, unsigned int stop_flag)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_task.h"
```

**功能说明**

- 销毁已创建的线程，释放线程占用的资源
- 会释放task指向的内存，调用者需将指针置NULL
- 线程函数不能在调用此接口前自行结束，否则将产生异常
- task须由osal_kthread_create返回

**前置条件**

- task参数不为NULL
- 待销毁线程的线程函数尚未自行退出
- task须由osal_kthread_create创建

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| task | [osal_task](#struct_osal_task) * | 待销毁的线程 | 非NULL，须由osal_kthread_create返回 |
| stop_flag | unsigned int | 线程退出标志 | 0：当前线程不退出；非0：当前线程退出 |

**参考案例**

- [sle_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_server/src/sle_speed_server.c)

### osal_kthread_schedule <a id="osal_kthread_schedule"></a>

```c
void osal_kthread_schedule(unsigned int sleep_ns)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_task.h"
```

**功能说明**

- 当前线程进入TASK_UNINTERRUPTIBLE状态，不可被外部信号唤醒
- 内核在睡眠时间到达后唤醒线程

**前置条件**

- 调用上下文为内核线程

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| sleep_ns | unsigned int | 睡眠时间（纳秒） | 大于0 |

### osal_kthread_set_uninterrupt <a id="osal_kthread_set_uninterrupt"></a>

```c
void osal_kthread_set_uninterrupt(void)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_task.h"
```

**功能说明**

- 设置当前线程为TASK_UNINTERRUPTIBLE状态
- 此状态下不可被外部信号唤醒，仅能被内核自身唤醒

**前置条件**

- 调用上下文为内核线程

**入参**

当前接口无入参

### osal_kthread_set_running <a id="osal_kthread_set_running"></a>

```c
void osal_kthread_set_running(void)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_task.h"
```

**功能说明**

- 设置当前线程状态为TASK_RUNNING
- 线程处于可运行状态，等待调度器选中后即可执行
- 不需要等待外部事件

**前置条件**

- 调用上下文为内核线程

**入参**

当前接口无入参

### osal_cond_resched <a id="osal_cond_resched"></a>

```c
void osal_cond_resched(void)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_task.h"
```

**功能说明**

- 内核态程序主动让出CPU资源
- 防止内核态长时间运行导致软锁死或长调度延迟
- 调用后若有更高优先级任务就绪，将触发调度

**前置条件**

- 调用上下文为内核线程

**入参**

当前接口无入参

### osal_schedule <a id="osal_schedule"></a>

```c
void osal_schedule(void)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_task.h"
```

**功能说明**

- 将当前任务放回就绪队列并触发调度
- 当前任务让出CPU，由调度器选择下一个运行任务
- 可用于主动让出CPU给同优先级或更高优先级任务

**前置条件**

- 调用上下文为可调度任务

**入参**

当前接口无入参

### osal_kneon_begin <a id="osal_kneon_begin"></a>

```c
void osal_kneon_begin(void)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_task.h"
```

**功能说明**

- 启用NEON算法加速
- 仅在CONFIG_KERNEL_MODE_NEON定义时有效，否则不执行任何操作

**前置条件**

- CONFIG_KERNEL_MODE_NEON已开启

**入参**

当前接口无入参

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_KERNEL_MODE_NEON | 特性宏 | 支持内核态NEON算法加速特性 | n |

### osal_kneon_end <a id="osal_kneon_end"></a>

```c
void osal_kneon_end(void)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_task.h"
```

**功能说明**

- 禁用NEON算法加速
- 仅在CONFIG_KERNEL_MODE_NEON定义时有效，否则不执行任何操作

**前置条件**

- CONFIG_KERNEL_MODE_NEON已开启
- 前序已调用osal_kneon_begin

**入参**

当前接口无入参

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_KERNEL_MODE_NEON | 特性宏 | 支持内核态NEON算法加速特性 | n |

### osal_yield <a id="osal_yield"></a>

```c
void osal_yield(void)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_task.h"
```

**功能说明**

- 释放当前线程的CPU时间片
- 当前线程变为就绪状态，重新参与CPU调度竞争
- 可能由当前线程或其他线程获得CPU

**前置条件**

- 调用上下文为可调度任务

**入参**

当前接口无入参

### osal_get_current_pid <a id="osal_get_current_pid"></a>

```c
long osal_get_current_pid(void)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_task.h"
```

**功能说明**

- 获取当前线程的PID
- 在LiteOS (Huawei LiteOS) 中内部调用osal_get_current_tid实现

**前置条件**

- 调用上下文为有效线程

**入参**

当前接口无入参

**返回值**

- 返回类型：long

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非负值 | 当前线程的PID | 线程ID获取成功 |
| OSAL_FAILURE(-1) | 获取失败 | 线程ID无效 |

### osal_get_current_tid <a id="osal_get_current_tid"></a>

```c
long osal_get_current_tid(void)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_task.h"
```

**功能说明**

- 获取当前线程的TID
- 在LiteOS中通过LOS_CurTaskIDGet获取

**前置条件**

- 调用上下文为有效线程

**入参**

当前接口无入参

**返回值**

- 返回类型：long

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非负值 | 当前线程的TID | 线程ID获取成功 |
| OSAL_FAILURE(-1) | 获取失败 | LOS_CurTaskIDGet返回无效ID |

### osal_get_current_tgid <a id="osal_get_current_tgid"></a>

```c
int osal_get_current_tgid(void)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_task.h"
```

**功能说明**

- 获取当前线程的TGID（线程组ID）

**前置条件**

- 调用上下文为有效线程

**入参**

当前接口无入参

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非负值 | 当前线程的TGID | 获取成功 |

### osal_get_current_taskname <a id="osal_get_current_taskname"></a>

```c
char *osal_get_current_taskname(void)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_task.h"
```

**功能说明**

- 获取当前线程的名称

**前置条件**

- 调用上下文为有效线程

**入参**

当前接口无入参

**返回值**

- 返回类型：char *

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非NULL指针 | 当前线程名称字符串 | 获取成功 |

### osal_msleep <a id="osal_msleep"></a>

```c
unsigned long osal_msleep(unsigned int msecs)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_task.h"
```

**功能说明**

- 使当前线程休眠指定毫秒数
- 休眠期间线程让出CPU，可被调度器切换到其他线程
- 定时器到期后线程被唤醒

**前置条件**

- 调用上下文为可调度任务

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| msecs | unsigned int | 休眠时间（毫秒） | 大于0 |

**返回值**

- 返回类型：unsigned long

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 定时器已到期 | 休眠时间完整度过 |
| 非零值 | 剩余休眠时间（毫秒） | 休眠被信号中断，返回未休眠的剩余时间 |

**参考案例**

- [ble_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_server/src/ble_speed_server.c)

### osal_msleep_uninterruptible <a id="osal_msleep_uninterruptible"></a>

```c
void osal_msleep_uninterruptible(unsigned int msecs)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_task.h"
```

**功能说明**

- 使当前线程进入不可中断休眠指定毫秒数
- 休眠期间不会被外部信号唤醒
- 在LiteOS中内部调用LOS_Msleep实现

**前置条件**

- 调用上下文为可调度任务

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| msecs | unsigned int | 休眠时间（毫秒） | 大于0 |

### osal_udelay <a id="osal_udelay"></a>

```c
void osal_udelay(unsigned int usecs)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_task.h"
```

**功能说明**

- 忙等待延时，精度为微秒
- 延时期间CPU忙等，不释放CPU资源
- 适用于短延时场景

**前置条件**

- 调用上下文无特殊限制

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| usecs | unsigned int | 延时时间（微秒） | 大于0 |

### osal_mdelay <a id="osal_mdelay"></a>

```c
void osal_mdelay(unsigned int msecs)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_task.h"
```

**功能说明**

- 忙等待延时，精度为毫秒
- 延时期间CPU忙等，不释放CPU资源
- 适用于短延时场景

**前置条件**

- 调用上下文无特殊限制

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| msecs | unsigned int | 延时时间（毫秒） | 大于0 |

**参考案例**

- [ssd1306.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/helloworld_oled/ssd1306.c)

### osal_kthread_suspend <a id="osal_kthread_suspend"></a>

```c
void osal_kthread_suspend(osal_task *task)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_task.h"
```

**功能说明**

- 挂起指定任务，任务将从就绪队列中移除
- 挂起后任务不再参与调度，直至被osal_kthread_resume恢复

**前置条件**

- task参数不为NULL
- 目标线程未被重复挂起

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| task | [osal_task](#struct_osal_task) * | 待挂起的线程 | 非NULL，须由osal_kthread_create返回 |

### osal_kthread_resume <a id="osal_kthread_resume"></a>

```c
void osal_kthread_resume(osal_task *task)
```

**头文件清单**

```c
#include "kernel/osal/include/schedule/osal_task.h"
```

**功能说明**

- 恢复已挂起的任务
- 恢复后任务重新加入就绪队列参与调度

**前置条件**

- task参数不为NULL
- 目标线程已被osal_kthread_suspend挂起

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| task | [osal_task](#struct_osal_task) * | 待恢复的线程 | 非NULL，须由osal_kthread_create返回且已被挂起 |

## Type definitions

### osal_kthread_handler <a id="typedef_osal_kthread_handler"></a>

```c
typedef int (*osal_kthread_handler)(void *data);
```

**使用说明**

作为osal_kthread_create的handler参数类型，定义线程入口函数签名

## Structures

### osal_task <a id="struct_osal_task"></a>

```c
typedef struct {
    void *task;
} osal_task;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| task | void * | 线程底层句柄，指向内核线程控制结构 |

## Macros

### OSAL_TASK_PRIORITY_ABOVE_HIGH <a id="OSAL_TASK_PRIORITY_ABOVE_HIGH"></a>

```c
#define OSAL_TASK_PRIORITY_ABOVE_HIGH   2
```

### OSAL_TASK_PRIORITY_HIGH <a id="OSAL_TASK_PRIORITY_HIGH"></a>

```c
#define OSAL_TASK_PRIORITY_HIGH         3
```

### OSAL_TASK_PRIORITY_BELOW_HIGH <a id="OSAL_TASK_PRIORITY_BELOW_HIGH"></a>

```c
#define OSAL_TASK_PRIORITY_BELOW_HIGH   4
```

### OSAL_TASK_PRIORITY_ABOVE_MIDDLE <a id="OSAL_TASK_PRIORITY_ABOVE_MIDDLE"></a>

```c
#define OSAL_TASK_PRIORITY_ABOVE_MIDDLE 5
```

### OSAL_TASK_PRIORITY_MIDDLE <a id="OSAL_TASK_PRIORITY_MIDDLE"></a>

```c
#define OSAL_TASK_PRIORITY_MIDDLE       6
```

### OSAL_TASK_PRIORITY_BELOW_MIDDLE <a id="OSAL_TASK_PRIORITY_BELOW_MIDDLE"></a>

```c
#define OSAL_TASK_PRIORITY_BELOW_MIDDLE 7
```

### OSAL_TASK_PRIORITY_ABOVE_LOW <a id="OSAL_TASK_PRIORITY_ABOVE_LOW"></a>

```c
#define OSAL_TASK_PRIORITY_ABOVE_LOW    8
```

### OSAL_TASK_PRIORITY_LOW <a id="OSAL_TASK_PRIORITY_LOW"></a>

```c
#define OSAL_TASK_PRIORITY_LOW          10
```

### OSAL_TASK_PRIORITY_BELOW_LOW <a id="OSAL_TASK_PRIORITY_BELOW_LOW"></a>

```c
#define OSAL_TASK_PRIORITY_BELOW_LOW    11
```




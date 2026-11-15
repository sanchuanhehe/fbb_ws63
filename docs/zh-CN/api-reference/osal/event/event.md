# Event

事件标志（Event Flags）用于任务间同步——一个任务等待多个事件标志位中的任意一个（OR）或全部（AND）满足后继续执行。支持阻塞等待（带超时）和事件清除模式。

与信号量不同，事件标志可以一次等待多个条件。适用任务需要等待多个独立事件就绪后再执行的场景。

**头文件清单**

```c
#include "kernel/osal/include/event/osal_event.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [osal_event_init](#osal_event_init) | 初始化事件控制块 |
| [osal_event_write](#osal_event_write) | 写入事件，设置指定事件掩码位 |
| [osal_event_read](#osal_event_read) | 读取事件，按指定模式等待事件发生 |
| [osal_event_clear](#osal_event_clear) | 清除指定事件掩码位 |
| [osal_event_destroy](#osal_event_destroy) | 销毁事件控制块，释放资源 |

## Functions

### osal_event_init <a id="osal_event_init"></a>

```c
int osal_event_init(osal_event *event_obj)
```

**头文件清单**

```c
#include "kernel/osal/include/event/osal_event.h"
```

**功能说明**

- 初始化事件控制块，为其分配底层系统资源
- 事件控制块未初始化时，event 成员为 NULL，初始化后指向底层事件控制结构
- 支持 liteos、freertos 系统

**前置条件**

- event_obj 指针不为 NULL 且指向有效内存
- event_obj->event 为 NULL（即未被初始化过），否则返回失败

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| event_obj | [osal_event](#struct_osal_event) * | 指向待初始化的事件控制块 | 非 NULL，且 event_obj->event 为 NULL |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 初始化成功 | 事件控制块初始化成功 |
| OSAL_FAILURE(-1) | 初始化失败 | 参数无效或底层内存分配失败 |

**参考案例**

- [i2s_dma_lli_slave_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/i2s_dma_lli/i2s_dma_lli_slave_demo.c)

### osal_event_write <a id="osal_event_write"></a>

```c
int osal_event_write(osal_event *event_obj, unsigned int mask)
```

**头文件清单**

```c
#include "kernel/osal/include/event/osal_event.h"
```

**功能说明**

- 向事件控制块写入指定掩码的事件
- 唤醒等待该事件的任务
- mask 的 bit 31 不可使用，在 liteos 系统中 bit 25 亦被禁止使用

**前置条件**

- event_obj 已通过 osal_event_init() 初始化成功
- event_obj 指针不为 NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| event_obj | [osal_event](#struct_osal_event) * | 指向已初始化的事件控制块 | 非 NULL，已初始化 |
| mask | unsigned int | 要写入的事件掩码 | bit 31 不可使用；bit 25 在 liteos 下不可使用；有效范围：0x0 ~ 0x7FFFFFFF |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 写入成功 | 事件写入成功 |
| OSAL_FAILURE(-1) | 写入失败 | 参数无效或底层写入失败 |

**参考案例**

- [i2s_dma_lli_slave_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/i2s_dma_lli/i2s_dma_lli_slave_demo.c)

### osal_event_read <a id="osal_event_read"></a>

```c
int osal_event_read(osal_event *event_obj, unsigned int mask, unsigned int timeout_ms, unsigned int mode)
```

**头文件清单**

```c
#include "kernel/osal/include/event/osal_event.h"
```

**功能说明**

- 按指定模式和超时时间读取事件，阻塞或调度当前任务
- 支持三种读取模式：AND（等待所有期望事件发生）、OR（等待任一期望事件发生）、CLR（读取后立即清除事件标志）
- 不允许在中断上下文中调用
- 不推荐在软件定时器回调中调用
- mask 的 bit 31 不可使用，在 liteos 系统中 bit 25 亦被禁止使用

**前置条件**

- event_obj 已通过 osal_event_init() 初始化成功
- 不可在中断上下文中调用
- 不推荐在软件定时器回调中调用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| event_obj | [osal_event](#struct_osal_event) * | 指向已初始化的事件控制块 | 非 NULL，已初始化 |
| mask | unsigned int | 期望发生的事件掩码 | bit 31 不可使用；bit 25 在 liteos 下不可使用；有效范围：0x0 ~ 0x7FFFFFFF |
| timeout_ms | unsigned int | 读取超时时间（单位：ms） | 0（不等待）~ 0xFFFFFFFF（[OSAL_EVENT_FOREVER](#OSAL_EVENT_FOREVER) 永久等待） |
| mode | unsigned int | 事件读取模式 | [OSAL_WAITMODE_AND](#OSAL_WAITMODE_AND)(4)：等待所有期望事件；[OSAL_WAITMODE_OR](#OSAL_WAITMODE_OR)(2)：等待任一期望事件；[OSAL_WAITMODE_CLR](#OSAL_WAITMODE_CLR)(1)：读取后立即清除；可组合使用，如 OSAL_WAITMODE_AND \| OSAL_WAITMODE_CLR |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 正整数 | 实际读取到的事件掩码 | 期望事件发生并成功读取 |
| OSAL_FAILURE(-1) | 读取失败 | 参数无效、超时或底层读取失败 |

**参考案例**

- [i2s_dma_lli_slave_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/i2s_dma_lli/i2s_dma_lli_slave_demo.c)

### osal_event_clear <a id="osal_event_clear"></a>

```c
int osal_event_clear(osal_event *event_obj, unsigned int mask)
```

**头文件清单**

```c
#include "kernel/osal/include/event/osal_event.h"
```

**功能说明**

- 清除事件控制块中指定掩码的事件标志位
- 清除操作将指定 mask 对应的事件位设置为 0

**前置条件**

- event_obj 已通过 osal_event_init() 初始化成功
- event_obj 指针不为 NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| event_obj | [osal_event](#struct_osal_event) * | 指向已初始化的事件控制块 | 非 NULL，已初始化 |
| mask | unsigned int | 要清除的事件掩码 | 0x0 ~ 0xFFFFFFFF |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 清除成功 | 事件清除成功 |
| OSAL_FAILURE(-1) | 清除失败 | 参数无效或底层清除失败 |

### osal_event_destroy <a id="osal_event_destroy"></a>

```c
int osal_event_destroy(osal_event *event_obj)
```

**头文件清单**

```c
#include "kernel/osal/include/event/osal_event.h"
```

**功能说明**

- 销毁事件控制块，释放底层系统资源
- 销毁后 event_obj->event 被置为 NULL
- event_obj 必须由 osal_event_init() 初始化获得，否则可能导致内存释放异常

**前置条件**

- event_obj 已通过 osal_event_init() 初始化成功
- event_obj 指针不为 NULL
- 确保没有任务正在等待该事件

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| event_obj | [osal_event](#struct_osal_event) * | 指向待销毁的事件控制块 | 非 NULL，已初始化 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| OSAL_SUCCESS(0) | 销毁成功 | 事件销毁成功 |
| OSAL_FAILURE(-1) | 销毁失败 | 参数无效或底层销毁失败 |

## Macros

### OSAL_EVENT_FOREVER <a id="OSAL_EVENT_FOREVER"></a>

```c
#define OSAL_EVENT_FOREVER 0xFFFFFFFF
```

**使用说明**

作为 osal_event_read 的 timeout_ms 参数值，表示永久等待

### OSAL_WAITMODE_AND <a id="OSAL_WAITMODE_AND"></a>

```c
#define OSAL_WAITMODE_AND 4U
```

**使用说明**

事件读取模式，任务等待所有期望事件发生

### OSAL_WAITMODE_OR <a id="OSAL_WAITMODE_OR"></a>

```c
#define OSAL_WAITMODE_OR 2U
```

**使用说明**

事件读取模式，任务等待任一期望事件发生

### OSAL_WAITMODE_CLR <a id="OSAL_WAITMODE_CLR"></a>

```c
#define OSAL_WAITMODE_CLR 1U
```

**使用说明**

事件读取模式，事件标志在读取后立即清除

## Type definitions

### osal_event <a id="typedef_osal_event"></a>

```c
typedef struct {
    void *event;
} osal_event;
```

**使用说明**

事件控制块类型，作为 osal_event_init、osal_event_write、osal_event_read、osal_event_clear、osal_event_destroy 的入参类型

## Structures

### osal_event <a id="struct_osal_event"></a>

```c
typedef struct {
    void *event;
} osal_event;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| event | void * | 底层事件控制结构指针，初始化前为 NULL，初始化后指向系统事件控制块 |



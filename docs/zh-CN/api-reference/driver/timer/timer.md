# Timer

硬件定时器提供微秒级精度的定时功能。与 OS (Operating System) 软件定时器不同——硬件定时器回调在 ISR (Interrupt Service Routine) 上下文中执行，精度达到微秒级。

完整生命周期：init → create → start → stop → delete。适用精确延时、周期性硬件操作等需要高精度定时的场景。

**头文件清单**

```c
#include "include/driver/timer.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_timer_init](#uapi_timer_init) | 初始化定时器模块 |
| [uapi_timer_adapter](#uapi_timer_adapter) | 适配指定的硬件定时器，配置中断ID与优先级 |
| [uapi_timer_deinit](#uapi_timer_deinit) | 去初始化定时器模块 |
| [uapi_timer_create](#uapi_timer_create) | 创建定时器实例 |
| [uapi_timer_delete](#uapi_timer_delete) | 删除定时器实例 |
| [uapi_timer_get_max_us](#uapi_timer_get_max_us) | 获取定时器最大可设置延时时间 |
| [uapi_timer_start](#uapi_timer_start) | 启动指定定时器 |
| [uapi_timer_stop](#uapi_timer_stop) | 停止指定定时器 |
| [uapi_timer_get_current_time_us](#uapi_timer_get_current_time_us) | 获取底层定时器当前时间 |
| [uapi_timer_suspend](#uapi_timer_suspend) | 挂起定时器 |
| [uapi_timer_resume](#uapi_timer_resume) | 恢复定时器 |

## Functions

### uapi_timer_init <a id="uapi_timer_init"></a>

```c
errcode_t uapi_timer_init(void)
```

**头文件清单**

```c
#include "include/driver/timer.h"
```

**功能说明**

- 初始化定时器模块，完成软件定时器管理器内存清零与软定时器列表配置
- 模块支持重复初始化，若已初始化则直接返回成功
- 初始化过程中需关闭中断，防止并发访问管理器数据

**前置条件**

- 模块未初始化或已初始化均可调用
- Kconfig配置项 CONFIG_DRIVER_SUPPORT_TIMER 已开启

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 初始化完成 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [timer_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/timer/timer_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_TIMER | 特性宏 | 支持定时器接口功能 | n |
| CONFIG_TIMER_SUPPORT_LPC | 特性宏 | 支持低功耗时钟控制特性 | n |

### uapi_timer_adapter <a id="uapi_timer_adapter"></a>

```c
errcode_t uapi_timer_adapter(timer_index_t index, uint32_t int_id, uint16_t int_priority)
```

**头文件清单**

```c
#include "include/driver/timer.h"
```

**功能说明**

- 适配指定的硬件定时器，初始化底层HAL (Hardware Abstraction Layer) 定时器并注册中断回调
- 配置硬件定时器的中断ID与中断优先级
- 同一硬件定时器索引重复适配时直接返回成功

**前置条件**

- 模块已通过 uapi_timer_init() 初始化完成，返回 ERRCODE_SUCC
- Kconfig配置项 CONFIG_DRIVER_SUPPORT_TIMER 已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| index | [timer_index_t](#enum_timer_index_t) | 硬件定时器索引 | TIMER_INDEX_0、TIMER_INDEX_1、TIMER_INDEX_2 |
| int_id | uint32_t | 硬件定时器中断ID | TIMER_0_IRQN、TIMER_1_IRQN、TIMER_2_IRQN |
| int_priority | uint16_t | 硬件定时器中断优先级 | 0~255 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 适配完成 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [timer_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/timer/timer_demo.c)


### uapi_timer_deinit <a id="uapi_timer_deinit"></a>

```c
errcode_t uapi_timer_deinit(void)
```

**头文件清单**

```c
#include "include/driver/timer.h"
```

**功能说明**

- 去初始化定时器模块，停止所有已适配的硬件定时器并注销中断
- 清零定时器管理器数据，释放底层HAL定时器资源
- 模块未初始化时调用直接返回成功

**前置条件**

- 无特殊前置条件，未初始化状态调用也返回成功

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 去初始化完成 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_TIMER | 特性宏 | 支持定时器接口功能 | n |
| CONFIG_TIMER_SUPPORT_LPC | 特性宏 | 支持低功耗时钟控制特性 | n |

### uapi_timer_create <a id="uapi_timer_create"></a>

```c
errcode_t uapi_timer_create(timer_index_t index, timer_handle_t *timer)
```

**头文件清单**

```c
#include "include/driver/timer.h"
```

**功能说明**

- 在指定硬件定时器索引下创建软件定时器实例，返回定时器句柄
- 每个硬件定时器索引下可创建的软件定时器数量由 CONFIG_TIMER_MAX_TIMERS_NUM 配置
- 软件定时器资源耗尽时返回 ERRCODE_TIMER_NO_ENOUGH

**前置条件**

- 模块已通过 uapi_timer_init() 初始化完成
- 指定硬件定时器索引已通过 uapi_timer_adapter() 适配完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| index | [timer_index_t](#enum_timer_index_t) | 硬件定时器索引 | TIMER_INDEX_0、TIMER_INDEX_1、TIMER_INDEX_2 |
| timer | [timer_handle_t](#typedef_timer_handle_t) * | 定时器句柄输出指针 | 不为NULL，指向已申请的内存空间 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| timer | [timer_handle_t](#typedef_timer_handle_t) * | 创建成功时返回定时器句柄；创建失败时输出NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 定时器创建成功 |
| ERRCODE_INVALID_PARAM(0x80000001) | 参数无效 | timer为NULL |
| ERRCODE_TIMER_NO_ENOUGH(0x80001320) | 定时器资源不足 | 软件定时器数量已满 |

**参考案例**

- [timer_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/timer/timer_demo.c)


### uapi_timer_delete <a id="uapi_timer_delete"></a>

```c
errcode_t uapi_timer_delete(timer_handle_t timer)
```

**头文件清单**

```c
#include "include/driver/timer.h"
```

**功能说明**

- 删除已创建的定时器实例，释放软件定时器资源
- 删除后定时器句柄不可再用于 start/stop 操作
- 定时器无需先停止即可删除

**前置条件**

- timer 为 uapi_timer_create() 创建的有效句柄

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| timer | [timer_handle_t](#typedef_timer_handle_t) | 定时器句柄 | 不为NULL，由 uapi_timer_create() 创建 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 定时器删除成功 |
| ERRCODE_INVALID_PARAM(0x80000001) | 参数无效 | timer为NULL |

**参考案例**

- [timer_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/timer/timer_demo.c)


### uapi_timer_get_max_us <a id="uapi_timer_get_max_us"></a>

```c
uint32_t uapi_timer_get_max_us(void)
```

**头文件清单**

```c
#include "include/driver/timer.h"
```

**功能说明**

- 获取定时器最大可设置的延时时间，单位为微秒（us）
- 返回值由硬件定时器最大计数值与时钟频率换算得出
- 用于约束 uapi_timer_start() 的 time_us 参数上限

**前置条件**

- 模块已通过 uapi_timer_init() 初始化完成

**返回值**

- 返回类型：uint32_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 最大延时时间(us) | 定时器最大可设置延时 | 正常调用 |

### uapi_timer_start <a id="uapi_timer_start"></a>

```c
errcode_t uapi_timer_start(timer_handle_t timer, uint32_t time_us, timer_callback_t callback, uintptr_t data)
```

**头文件清单**

```c
#include "include/driver/timer.h"
```

**功能说明**

- 启动指定的定时器，设定超时时间并注册回调函数
- 超时时间到达后，在定时器中断上下文中调用回调函数
- time_us 不得超过 uapi_timer_get_max_us() 返回值，且不可为0

**前置条件**

- timer 为 uapi_timer_create() 创建的有效句柄
- 定时器未处于运行状态（否则将更新超时时间与回调）

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| timer | [timer_handle_t](#typedef_timer_handle_t) | 定时器句柄 | 不为NULL，由 uapi_timer_create() 创建 |
| time_us | uint32_t | 定时器超时时间（us） | 1 ~ uapi_timer_get_max_us() |
| callback | [timer_callback_t](#typedef_timer_callback_t) | 定时器回调函数 | 不为NULL |
| data | uintptr_t | 传递给回调函数的参数 | 无限制 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 定时器启动成功 |
| ERRCODE_INVALID_PARAM(0x80000001) | 参数无效 | timer为NULL、callback为NULL、time_us为0或超限 |
| ERRCODE_TIEMR_NOT_CREATED(0x80001321) | 定时器未创建 | timer句柄对应的定时器未启用 |

**参考案例**

- [timer_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/timer/timer_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_TIMER_USING_OLD_VERSION | 特性宏 | 支持旧版定时器IP (Intellectual Property) 特性 | n |

### uapi_timer_stop <a id="uapi_timer_stop"></a>

```c
errcode_t uapi_timer_stop(timer_handle_t timer)
```

**头文件清单**

```c
#include "include/driver/timer.h"
```

**功能说明**

- 停止指定的定时器，已注册的回调函数不再被调用
- 清除定时器的运行状态、剩余周期、回调与数据
- 若当前硬件定时器下无其他运行中的软件定时器，将停止硬件定时器

**前置条件**

- timer 为 uapi_timer_create() 创建的有效句柄

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| timer | [timer_handle_t](#typedef_timer_handle_t) | 定时器句柄 | 不为NULL，由 uapi_timer_create() 创建 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 定时器停止成功或定时器未运行 |
| ERRCODE_INVALID_PARAM(0x80000001) | 参数无效 | timer为NULL |
| ERRCODE_TIEMR_NOT_CREATED(0x80001321) | 定时器未创建 | timer句柄对应的定时器未启用 |

**参考案例**

- [timer_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/timer/timer_demo.c)


### uapi_timer_get_current_time_us <a id="uapi_timer_get_current_time_us"></a>

```c
errcode_t uapi_timer_get_current_time_us(timer_index_t index, uint32_t *current_time_us)
```

**头文件清单**

```c
#include "include/driver/timer.h"
```

**功能说明**

- 获取指定底层硬件定时器的当前时间，单位为微秒（us）
- 读取硬件定时器当前计数值并转换为微秒
- 在中断保护下读取，保证数据一致性

**前置条件**

- 模块已通过 uapi_timer_init() 初始化完成
- 指定硬件定时器索引已通过 uapi_timer_adapter() 适配完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| index | [timer_index_t](#enum_timer_index_t) | 底层定时器索引 | 0 ~ TIMER_MAX_NUM - 1 |
| current_time_us | uint32_t * | 当前时间输出指针（us） | 不为NULL，指向已申请的内存空间 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| current_time_us | uint32_t * | 底层定时器当前时间值（us） |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 获取时间成功 |
| ERRCODE_INVALID_PARAM(0x80000001) | 参数无效 | index超限或current_time_us为NULL |

### uapi_timer_suspend <a id="uapi_timer_suspend"></a>

```c
errcode_t uapi_timer_suspend(uintptr_t val)
```

**头文件清单**

```c
#include "include/driver/timer.h"
```

**功能说明**

- 挂起定时器，更新所有已适配硬件定时器的剩余时间
- 在低功耗场景下调用，暂停定时器运行
- 在中断保护下执行，保证数据一致性

**前置条件**

- 模块已通过 uapi_timer_init() 初始化完成
- Kconfig配置项 CONFIG_TIMER_SUPPORT_LPM (Low Power Management) 已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| val | uintptr_t | 挂起参数 | 当前未使用 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 挂起成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_TIMER | 特性宏 | 支持定时器接口功能 | n |
| CONFIG_TIMER_SUPPORT_LPM | 特性宏 | 支持定时器低功耗挂起/恢复功能 | n |

### uapi_timer_resume <a id="uapi_timer_resume"></a>

```c
errcode_t uapi_timer_resume(uintptr_t val)
```

**头文件清单**

```c
#include "include/driver/timer.h"
```

**功能说明**

- 恢复定时器，根据补偿计数值重新加载所有已适配硬件定时器
- 在低功耗场景下调用，恢复定时器运行
- 在中断保护下执行，保证数据一致性

**前置条件**

- 模块已通过 uapi_timer_init() 初始化完成
- 定时器已通过 uapi_timer_suspend() 挂起
- Kconfig配置项 CONFIG_TIMER_SUPPORT_LPM 已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| val | uintptr_t | 恢复参数，指向补偿计数值 | 指向 uint64_t 类型内存，不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 恢复成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_TIMER | 特性宏 | 支持定时器接口功能 | n |
| CONFIG_TIMER_SUPPORT_LPM | 特性宏 | 支持定时器低功耗挂起/恢复功能 | n |

## Type definitions

### errcode_t <a id="typedef_errcode_t"></a>

```c
typedef uint32_t errcode_t;
```

**使用说明**

作为所有timer接口的返回值类型使用

### timer_handle_t <a id="typedef_timer_handle_t"></a>

```c
typedef void *timer_handle_t;
```

**使用说明**

作为 uapi_timer_create 的出参类型、uapi_timer_delete/uapi_timer_start/uapi_timer_stop 的入参类型使用

### timer_callback_t <a id="typedef_timer_callback_t"></a>

```c
typedef void (*timer_callback_t)(uintptr_t data);
```

**使用说明**

作为 uapi_timer_start 的回调函数参数类型使用

## Enumerations

### timer_index_t <a id="enum_timer_index_t"></a>

```c
typedef enum timer_index {
    TIMER_INDEX_0,                      /*!< Timer0 index. */
    TIMER_INDEX_1,                      /*!< Timer1 index. */
    TIMER_INDEX_2,                      /*!< Timer2 index. */
    TIMER_MAX_NUM
} timer_index_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| TIMER_INDEX_0 | 0 | 硬件定时器0索引 |
| TIMER_INDEX_1 | 1 | 硬件定时器1索引 |
| TIMER_INDEX_2 | 2 | 硬件定时器2索引 |
| TIMER_MAX_NUM | 3 | 定时器数量上限，不可用作索引 |



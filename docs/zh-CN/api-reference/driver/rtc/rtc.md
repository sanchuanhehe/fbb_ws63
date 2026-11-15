# RTC

RTC (Real-Time Clock) 是独立于系统主时钟的硬件定时器，拥有独立电源域。即使在深度睡眠（CPU 时钟停止）时仍继续运行，超时后通过中断唤醒 CPU。

提供应用级定时器（create/start/stop/delete）和底层硬件定时器两种操作层级。适用定时唤醒、周期性任务触发、超时保护等场景。

**头文件清单**

```c
#include "include/driver/rtc.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_rtc_init](#uapi_rtc_init) | 初始化RTC定时器模块 |
| [uapi_rtc_adapter](#uapi_rtc_adapter) | 适配指定底层RTC定时器，注册中断 |
| [uapi_rtc_deinit](#uapi_rtc_deinit) | 去初始化RTC定时器模块 |
| [uapi_rtc_create](#uapi_rtc_create) | 创建RTC定时器，获取句柄 |
| [uapi_rtc_delete](#uapi_rtc_delete) | 删除已创建的RTC定时器 |
| [uapi_rtc_start](#uapi_rtc_start) | 启动指定RTC定时器，设置超时回调 |
| [uapi_rtc_stop](#uapi_rtc_stop) | 停止指定RTC定时器 |
| [uapi_rtc_get_max_ms](#uapi_rtc_get_max_ms) | 获取RTC最大可设置的延时时间 |
| [uapi_rtc_int_cnt_record_get](#uapi_rtc_int_cnt_record_get) | 获取RTC中断发生次数 |
| [uapi_rtc_get_current_time_count](#uapi_rtc_get_current_time_count) | 获取指定底层RTC定时器的当前计数值 |
| [uapi_rtc_get_current_time_us](#uapi_rtc_get_current_time_us) | 获取指定底层RTC定时器的当前时间（微秒） |
| [uapi_rtc_start_hw_rtc](#uapi_rtc_start_hw_rtc) | 启动底层RTC定时器，用于计时，不处理中断 |
| [uapi_rtc_stop_hw_rtc](#uapi_rtc_stop_hw_rtc) | 停止底层RTC定时器 |
| [uapi_rtc_get_latest_timeout](#uapi_rtc_get_latest_timeout) | 获取所有RTC最近的超时时间 |
| [uapi_rtc_suspend](#uapi_rtc_suspend) | 挂起RTC模块 |
| [uapi_rtc_resume](#uapi_rtc_resume) | 恢复RTC模块 |

## Functions

### uapi_rtc_init <a id="uapi_rtc_init"></a>

```c
errcode_t uapi_rtc_init(void)
```

**头文件清单**

```c
#include "include/driver/rtc.h"
```

**功能说明**

- 初始化RTC定时器模块，完成内部管理结构体的清零与各底层RTC软定时器列表的配置
- 重复调用时返回成功，不重复执行初始化流程
- 初始化过程中对内部数据结构进行中断安全保护

**前置条件**

- 模块未初始化或已初始化均可调用，重复调用返回成功
- CONFIG_RTC_MAX_NUM、CONFIG_RTC_MAX_RTCS_NUM等配置项已正确配置

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 初始化成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RTC_MAX_NUM | 特性宏 | 支持RTC底层定时器数量配置功能 | 5 |
| CONFIG_RTC_MAX_RTCS_NUM | 特性宏 | 支持RTC软定时器数量配置功能 | 16 |

> **注意**：以下配置项当前未在Kconfig中定义，由驱动代码内部 `#ifndef` fallback机制提供默认值：

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RTC_MAX_RTCS_NUM_0 | 特性宏 | 支持RTC0软定时器数量配置功能 | 16 |
| CONFIG_RTC_MAX_RTCS_NUM_1 | 特性宏 | 支持RTC1软定时器数量配置功能 | 16 |
| CONFIG_RTC_MAX_RTCS_NUM_2 | 特性宏 | 支持RTC2软定时器数量配置功能 | 16 |
| CONFIG_RTC_MAX_RTCS_NUM_3 | 特性宏 | 支持RTC3软定时器数量配置功能 | 16 |
| CONFIG_RTC_MAX_RTCS_NUM_EXTRA | 特性宏 | 支持扩展RTC软定时器数量配置功能 | 16 |

### uapi_rtc_adapter <a id="uapi_rtc_adapter"></a>

```c
errcode_t uapi_rtc_adapter(rtc_index_t index, uint32_t int_id, uint16_t int_priority)
```

**头文件清单**

```c
#include "include/driver/rtc.h"
```

**功能说明**

- 适配指定底层RTC定时器，完成HAL (Hardware Abstraction Layer) 层初始化并注册中断回调
- 重复适配同一索引时返回成功，不重复执行适配流程
- 适配过程中对内部状态进行中断安全保护

**前置条件**

- 模块初始化状态：该接口所属模块已通过 uapi_rtc_init() 初始化完成，返回初始化成功状态（返回值为ERRCODE_SUCC）
- 入参index须小于RTC_MAX_NUM

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| index | [rtc_index_t](#enum_rtc_index_t) | 底层RTC定时器索引 | RTC_0(0), RTC_1(1), RTC_2(2), RTC_3(3) |
| int_id | uint32_t | 底层RTC定时器中断ID | 芯片中断号范围 |
| int_priority | uint16_t | RTC定时器中断优先级 | 0~255 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 适配成功 |
| ERRCODE_INVALID_PARAM(0x80000001) | 参数无效 | index >= RTC_MAX_NUM |
| ERRCODE_RTC_NOT_INITED(0x80001405) | RTC未初始化 | 模块未调用uapi_rtc_init() |
| Other | 其他错误码，参考errcode_t | HAL层初始化失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RTC_MAX_NUM | 特性宏 | 支持RTC底层定时器数量配置功能 | 5 |

### uapi_rtc_deinit <a id="uapi_rtc_deinit"></a>

```c
errcode_t uapi_rtc_deinit(void)
```

**头文件清单**

```c
#include "include/driver/rtc.h"
```

**功能说明**

- 去初始化RTC定时器模块，停止所有已适配的底层RTC定时器，注销中断，清零管理结构体
- 未初始化时调用返回成功
- 去初始化过程中对内部数据结构进行中断安全保护

**前置条件**

- 模块已初始化或未初始化均可调用，未初始化时调用返回成功
- 调用前应停止所有正在运行的RTC定时器

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 去初始化成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RTC_MAX_NUM | 特性宏 | 支持RTC底层定时器数量配置功能 | 5 |

### uapi_rtc_create <a id="uapi_rtc_create"></a>

```c
errcode_t uapi_rtc_create(rtc_index_t index, rtc_handle_t *rtc)
```

**头文件清单**

```c
#include "include/driver/rtc.h"
```

**功能说明**

- 在指定底层RTC索引上创建软定时器，分配资源并返回句柄
- 从软定时器列表中查找空闲槽位分配，全部占用时返回错误
- 创建过程中对软定时器列表进行中断安全保护

**前置条件**

- 模块初始化状态：该接口所属模块已通过 uapi_rtc_init() 初始化完成
- 入参rtc指针不为NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| index | [rtc_index_t](#enum_rtc_index_t) | 底层RTC定时器索引 | RTC_0(0), RTC_1(1), RTC_2(2), RTC_3(3) |
| rtc | [rtc_handle_t](#typedef_rtc_handle_t) * | 返回的RTC定时器句柄指针 | 不为NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| rtc | [rtc_handle_t](#typedef_rtc_handle_t) * | RTC定时器句柄；创建失败时为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 创建成功 |
| ERRCODE_INVALID_PARAM(0x80000001) | 参数无效 | index >= RTC_MAX_NUM 或 rtc为NULL |
| ERRCODE_RTC_NO_ENOUGH(0x80001407) | 软定时器资源不足 | 无空闲软定时器槽位 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_RTC | 特性宏 | 支持RTC接口功能 | n |
| CONFIG_RTC_MAX_NUM | 特性宏 | 支持RTC底层定时器数量配置功能 | 5 |
| CONFIG_RTC_MAX_RTCS_NUM_0 | 特性宏 | 支持RTC0软定时器数量配置功能 | 16 |
| CONFIG_RTC_MAX_RTCS_NUM_1 | 特性宏 | 支持RTC1软定时器数量配置功能 | 16 |
| CONFIG_RTC_MAX_RTCS_NUM_2 | 特性宏 | 支持RTC2软定时器数量配置功能 | 16 |
| CONFIG_RTC_MAX_RTCS_NUM_3 | 特性宏 | 支持RTC3软定时器数量配置功能 | 16 |
| CONFIG_RTC_MAX_RTCS_NUM_EXTRA | 特性宏 | 支持扩展RTC软定时器数量配置功能 | 16 |

### uapi_rtc_delete <a id="uapi_rtc_delete"></a>

```c
errcode_t uapi_rtc_delete(rtc_handle_t rtc)
```

**头文件清单**

```c
#include "include/driver/rtc.h"
```

**功能说明**

- 删除已创建的RTC定时器，释放软定时器资源
- 删除后句柄不再有效，禁止继续使用
- 删除过程中对软定时器列表进行中断安全保护

**前置条件**

- 入参rtc不为NULL
- rtc句柄由 uapi_rtc_create() 成功创建

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| rtc | [rtc_handle_t](#typedef_rtc_handle_t) | 由uapi_rtc_create创建的RTC定时器句柄 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 删除成功 |
| ERRCODE_INVALID_PARAM(0x80000001) | 参数无效 | rtc为NULL |

### uapi_rtc_start <a id="uapi_rtc_start"></a>

```c
errcode_t uapi_rtc_start(rtc_handle_t rtc, uint32_t rtc_ms, rtc_callback_t callback, uintptr_t data)
```

**头文件清单**

```c
#include "include/driver/rtc.h"
```

**功能说明**

- 启动指定RTC定时器，设置超时时间（毫秒）与超时回调函数
- 超时时间到达后触发回调函数，回调函数在RTC中断上下文中执行
- 超时时间受最大延时限制，参考 uapi_rtc_get_max_ms() 获取上限值
- 启动过程中对软定时器列表与硬件计时器进行中断安全保护

**前置条件**

- 模块初始化状态：该接口所属模块已通过 uapi_rtc_init() 初始化完成
- rtc句柄已通过 uapi_rtc_create() 成功创建
- 入参callback不为NULL
- 入参rtc_ms取值范围为1 ~ uapi_rtc_get_max_ms()返回值

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| rtc | [rtc_handle_t](#typedef_rtc_handle_t) | 由uapi_rtc_create创建的RTC定时器句柄 | 不为NULL |
| rtc_ms | uint32_t | RTC定时器超时时间，单位毫秒 | 1 ~ uapi_rtc_get_max_ms()返回值 |
| callback | [rtc_callback_t](#typedef_rtc_callback_t) | RTC定时器超时回调函数 | 不为NULL |
| data | uintptr_t | 传递给RTC回调函数的参数 | - |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 启动成功 |
| ERRCODE_INVALID_PARAM(0x80000001) | 参数无效 | rtc为NULL、callback为NULL、rtc_ms为0或超过最大值 |
| ERRCODE_RTC_NOT_CREATED(0x80001408) | RTC定时器未创建 | rtc句柄对应的软定时器未启用 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RTC_USING_OLD_VERSION | 特性宏 | 支持RTC旧版本load count对齐特性 | n |

### uapi_rtc_stop <a id="uapi_rtc_stop"></a>

```c
errcode_t uapi_rtc_stop(rtc_handle_t rtc)
```

**头文件清单**

```c
#include "include/driver/rtc.h"
```

**功能说明**

- 停止指定RTC定时器，停止后不会调用用户传入的callback
- 若当前无其他运行中的定时器，将停止硬件计时
- 停止过程中对软定时器列表与硬件计时器进行中断安全保护

**前置条件**

- 入参rtc不为NULL
- rtc句柄由 uapi_rtc_create() 成功创建

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| rtc | [rtc_handle_t](#typedef_rtc_handle_t) | 由uapi_rtc_create创建的RTC定时器句柄 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 停止成功 |
| ERRCODE_INVALID_PARAM(0x80000001) | 参数无效 | rtc为NULL |
| ERRCODE_RTC_NOT_CREATED(0x80001408) | RTC定时器未创建 | rtc句柄对应的软定时器未启用 |

### uapi_rtc_get_max_ms <a id="uapi_rtc_get_max_ms"></a>

```c
uint32_t uapi_rtc_get_max_ms(void)
```

**头文件清单**

```c
#include "include/driver/rtc.h"
```

**功能说明**

- 获取RTC最大可设置的延时时间，单位毫秒
- 返回值由宏 RTC_MAX_MS 计算得出，与 CONFIG_RTC_CLOCK_VALUE 配置项相关
- 用于约束 uapi_rtc_start() 的 rtc_ms 参数取值上限

**前置条件**

- CONFIG_RTC_CLOCK_VALUE配置项已正确配置

**返回值**

- 返回类型：uint32_t
- 延时时间（毫秒），取值由 RTC_MAX_MS 宏决定

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RTC_CLOCK_VALUE | 特性宏 | 支持RTC时钟频率配置功能 | 32768 |

### uapi_rtc_int_cnt_record_get <a id="uapi_rtc_int_cnt_record_get"></a>

```c
uint32_t uapi_rtc_int_cnt_record_get(rtc_index_t index)
```

**头文件清单**

```c
#include "include/driver/rtc.h"
```

**功能说明**

- 获取RTC硬件发生中断的次数
- 返回HAL层统计的中断计数
- index无效时返回0

**前置条件**

- 入参index须小于RTC_MAX_NUM，否则返回0

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| index | [rtc_index_t](#enum_rtc_index_t) | 底层RTC定时器索引 | RTC_0(0), RTC_1(1), RTC_2(2), RTC_3(3) |

**返回值**

- 返回类型：uint32_t
- 中断次数；index无效时返回0

### uapi_rtc_get_current_time_count <a id="uapi_rtc_get_current_time_count"></a>

```c
errcode_t uapi_rtc_get_current_time_count(rtc_index_t index, uint64_t *current_time_count)
```

**头文件清单**

```c
#include "include/driver/rtc.h"
```

**功能说明**

- 获取指定底层RTC定时器的当前硬件计数值
- 读取过程中对硬件寄存器访问进行中断安全保护
- 返回值为硬件计数器的原始计数值

**前置条件**

- 入参current_time_count不为NULL
- 入参index须小于RTC_MAX_NUM

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| index | [rtc_index_t](#enum_rtc_index_t) | 底层RTC定时器索引 | RTC_0(0), RTC_1(1), RTC_2(2), RTC_3(3) |
| current_time_count | uint64_t * | 底层RTC定时器当前计数值输出指针 | 不为NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| current_time_count | uint64_t * | 底层RTC定时器当前计数值 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 获取成功 |
| ERRCODE_INVALID_PARAM(0x80000001) | 参数无效 | index >= RTC_MAX_NUM 或 current_time_count为NULL |

### uapi_rtc_get_current_time_us <a id="uapi_rtc_get_current_time_us"></a>

```c
errcode_t uapi_rtc_get_current_time_us(rtc_index_t index, uint32_t *current_time_us)
```

**头文件清单**

```c
#include "include/driver/rtc.h"
```

**功能说明**

- 获取指定底层RTC定时器的当前时间，单位微秒
- 将硬件计数值转换为微秒时间值输出
- 读取过程中对硬件寄存器访问进行中断安全保护

**前置条件**

- 入参current_time_us不为NULL
- 入参index须小于RTC_MAX_NUM

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| index | [rtc_index_t](#enum_rtc_index_t) | 底层RTC定时器索引 | RTC_0(0), RTC_1(1), RTC_2(2), RTC_3(3) |
| current_time_us | uint32_t * | 底层RTC定时器当前时间us值输出指针 | 不为NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| current_time_us | uint32_t * | 底层RTC定时器当前时间（微秒） |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 获取成功 |
| ERRCODE_INVALID_PARAM(0x80000001) | 参数无效 | index >= RTC_MAX_NUM 或 current_time_us为NULL |

### uapi_rtc_start_hw_rtc <a id="uapi_rtc_start_hw_rtc"></a>

```c
errcode_t uapi_rtc_start_hw_rtc(rtc_index_t index, uint64_t rtc_ms)
```

**头文件清单**

```c
#include "include/driver/rtc.h"
```

**功能说明**

- 启动指定底层RTC定时器，用于计时，不处理中断
- 直接操作硬件RTC，不经过软定时器管理层
- 超时时间受 RTC_HW_MAX_MS 宏限制
- 不能使用已经在使用中的index（与软定时器共用底层资源）

**前置条件**

- 入参index须小于RTC_MAX_NUM
- 入参rtc_ms取值范围为1 ~ RTC_HW_MAX_MS
- 指定index未被软定时器管理层占用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| index | [rtc_index_t](#enum_rtc_index_t) | 底层RTC定时器索引 | RTC_0(0), RTC_1(1), RTC_2(2), RTC_3(3) |
| rtc_ms | uint64_t | RTC定时器超时时间，单位毫秒 | 1 ~ RTC_HW_MAX_MS |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 启动成功 |
| ERRCODE_INVALID_PARAM(0x80000001) | 参数无效 | index >= RTC_MAX_NUM、rtc_ms为0或超过RTC_HW_MAX_MS |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RTC_USING_OLD_VERSION | 特性宏 | 支持RTC旧版本load count对齐特性 | n |

### uapi_rtc_stop_hw_rtc <a id="uapi_rtc_stop_hw_rtc"></a>

```c
errcode_t uapi_rtc_stop_hw_rtc(rtc_index_t index)
```

**头文件清单**

```c
#include "include/driver/rtc.h"
```

**功能说明**

- 停止指定底层RTC定时器
- 直接操作硬件RTC，不经过软定时器管理层
- 停止过程中对硬件寄存器访问进行中断安全保护

**前置条件**

- 入参index须小于RTC_MAX_NUM

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| index | [rtc_index_t](#enum_rtc_index_t) | 底层RTC定时器索引 | RTC_0(0), RTC_1(1), RTC_2(2), RTC_3(3) |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 停止成功 |
| ERRCODE_INVALID_PARAM(0x80000001) | 参数无效 | index >= RTC_MAX_NUM |

### uapi_rtc_get_latest_timeout <a id="uapi_rtc_get_latest_timeout"></a>

```c
uint32_t uapi_rtc_get_latest_timeout(void)
```

**头文件清单**

```c
#include "include/driver/rtc.h"
```

**功能说明**

- 获取所有已适配RTC定时器中最近的超时时间，单位毫秒
- 遍历所有底层RTC定时器，取硬件计数器剩余值最小者转换为毫秒
- 用于低功耗场景判断最近超时时间

**前置条件**

- CONFIG_RTC_SUPPORT_LPM (Low Power Management)已开启
- 至少一个底层RTC已通过uapi_rtc_adapter()适配

**返回值**

- 返回类型：uint32_t
- 所有RTC最近的超时时间（毫秒）

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RTC_SUPPORT_LPM | 特性宏 | 支持RTC低功耗恢复功能 | n |

### uapi_rtc_suspend <a id="uapi_rtc_suspend"></a>

```c
errcode_t uapi_rtc_suspend(uintptr_t val)
```

**头文件清单**

```c
#include "include/driver/rtc.h"
```

**功能说明**

- 挂起RTC模块，更新所有已适配RTC定时器的软定时器剩余cycle
- 保存当前硬件计数器状态，为低功耗恢复做准备
- 挂起过程中对软定时器列表与硬件计时器进行中断安全保护

**前置条件**

- CONFIG_RTC_SUPPORT_LPM已开启
- 至少一个底层RTC已通过uapi_rtc_adapter()适配

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| val | uintptr_t | 挂起参数（当前未使用） | - |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 挂起成功 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RTC_SUPPORT_LPM | 特性宏 | 支持RTC低功耗恢复功能 | n |

### uapi_rtc_resume <a id="uapi_rtc_resume"></a>

```c
errcode_t uapi_rtc_resume(uintptr_t val)
```

**头文件清单**

```c
#include "include/driver/rtc.h"
```

**功能说明**

- 恢复RTC模块，根据补偿计数值重新启动所有已适配的RTC定时器
- val参数指向uint64_t类型的补偿计数值
- 恢复过程中对软定时器列表与硬件计时器进行中断安全保护

**前置条件**

- 入参val不为NULL，指向uint64_t类型的补偿计数值

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| val | uintptr_t | 指向uint64_t补偿计数值的指针 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 恢复成功 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RTC_SUPPORT_LPM | 特性宏 | 支持RTC低功耗恢复功能 | n |

## Type definitions

### rtc_handle_t <a id="typedef_rtc_handle_t"></a>

```c
typedef void *rtc_handle_t;
```

**使用说明**

在 uapi_rtc_create、uapi_rtc_delete、uapi_rtc_start、uapi_rtc_stop 接口中作为RTC定时器句柄使用

### rtc_callback_t <a id="typedef_rtc_callback_t"></a>

```c
typedef void (*rtc_callback_t)(uintptr_t data);
```

**使用说明**

在 uapi_rtc_start 接口中作为超时回调函数类型使用

### errcode_t <a id="typedef_errcode_t"></a>

```c
typedef uint32_t errcode_t;
```

**使用说明**

在所有返回errcode_t类型的接口中作为返回值类型使用

## Enumerations

### rtc_index_t <a id="enum_rtc_index_t"></a>

```c
typedef enum {
    RTC_0,                            /*!< RTC0 index. */
    RTC_1,                            /*!< RTC1 index. */
    RTC_2,                            /*!< RTC2 index. */
    RTC_3,                            /*!< RTC3 index. */
    RTC_MAX_NUM = RTC_CLOCK_MAX_NUM
} rtc_index_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| RTC_0 | 0 | RTC0索引 |
| RTC_1 | 1 | RTC1索引 |
| RTC_2 | 2 | RTC2索引 |
| RTC_3 | 3 | RTC3索引 |
| RTC_MAX_NUM | 4 | RTC最大索引数量，等于RTC_CLOCK_MAX_NUM(4) |



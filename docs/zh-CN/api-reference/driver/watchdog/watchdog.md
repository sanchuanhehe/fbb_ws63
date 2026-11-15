# Watchdog

看门狗是独立于 CPU 的硬件倒计时器——如果超时前未执行 "喂狗"（kick）操作，硬件触发系统复位。这是嵌入式系统最底层的故障恢复机制，防止程序跑飞或死锁后设备永久挂死。

使用流程：init → enable → 周期性 kick。需根据业务容忍度选择合适的超时时间和喂狗策略。

**头文件清单**

```c
#include "include/driver/watchdog.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_watchdog_init](#uapi_watchdog_init) | 初始化看门狗，设置超时时间 |
| [uapi_watchdog_deinit](#uapi_watchdog_deinit) | 去初始化看门狗 |
| [uapi_watchdog_enable](#uapi_watchdog_enable) | 使能看门狗，指定触发模式 |
| [uapi_watchdog_disable](#uapi_watchdog_disable) | 去使能看门狗 |
| [uapi_watchdog_kick](#uapi_watchdog_kick) | 喂狗，重置看门狗计数器 |
| [uapi_watchdog_set_time](#uapi_watchdog_set_time) | 设置看门狗超时时间 |
| [uapi_watchdog_get_left_time](#uapi_watchdog_get_left_time) | 获取看门狗计数器剩余时间 |
| [uapi_register_watchdog_callback](#uapi_register_watchdog_callback) | 注册看门狗超时回调函数 |
| [uapi_watchdog_resume](#uapi_watchdog_resume) | 恢复看门狗模块（低功耗模式） |
| [uapi_watchdog_suspend](#uapi_watchdog_suspend) | 挂起看门狗模块（低功耗模式） |

## Functions

### uapi_watchdog_init <a id="uapi_watchdog_init"></a>

```c
errcode_t uapi_watchdog_init(uint32_t timeout)
```

**头文件清单**

```c
#include "include/driver/watchdog.h"
```

**功能说明**

- 初始化看门狗模块，注册HAL (Hardware Abstraction Layer) 函数与中断处理
- 设置看门狗超时时间，单位为秒
- 当未定义CONFIG_WATCHDOG_ALREADY_START时，初始化过程中将配置超时属性到硬件

**前置条件**

- 模块未初始化状态：调用前看门狗模块尚未初始化
- 硬件资源就绪：看门狗硬件时钟及寄存器已就绪

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| timeout | uint32_t | 看门狗超时时间，单位秒 | 大于0 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 看门狗初始化成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [watchdog_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/watchdog/watchdog_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_WDT | 特性宏 | 支持看门狗接口功能 | n |
| CONFIG_WATCHDOG_ALREADY_START | 特性宏 | 支持看门狗已启动跳过属性配置特性 | n |
| CONFIG_WATCHDOG_SUPPORT_LPM | 特性宏 | 支持低功耗模式下保存超时时间特性 | n |

### uapi_watchdog_deinit <a id="uapi_watchdog_deinit"></a>

```c
errcode_t uapi_watchdog_deinit(void)
```

**头文件清单**

```c
#include "include/driver/watchdog.h"
```

**功能说明**

- 去初始化看门狗模块，释放硬件资源
- 若看门狗仍处于使能状态，将先调用disable关闭
- 注销HAL函数注册

**前置条件**

- 模块初始化状态：看门狗模块已通过uapi_watchdog_init()初始化

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 去初始化成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [watchdog_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/watchdog/watchdog_demo.c)


### uapi_watchdog_enable <a id="uapi_watchdog_enable"></a>

```c
errcode_t uapi_watchdog_enable(wdt_mode_t mode)
```

**头文件清单**

```c
#include "include/driver/watchdog.h"
```

**功能说明**

- 使能看门狗，指定触发模式
- 当未定义CONFIG_WATCHDOG_ALREADY_START时，将调用HAL使能硬件看门狗
- 当定义CONFIG_WATCHDOG_ALREADY_START时，仅记录使能状态，不操作硬件

**前置条件**

- 模块初始化状态：看门狗模块已通过uapi_watchdog_init()初始化
- 参数合法性：mode取值须小于WDT_MODE_MAX

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| mode | [wdt_mode_t](#enum_wdt_mode_t) | 看门狗触发模式 | WDT_MODE_RESET(0), WDT_MODE_INTERRUPT(1) |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 使能看门狗成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [watchdog_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/watchdog/watchdog_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_WATCHDOG_ALREADY_START | 特性宏 | 支持看门狗已启动跳过硬件使能特性 | n |
| CONFIG_WATCHDOG_SUPPORT_LPM | 特性宏 | 支持低功耗模式下保存模式配置特性 | n |

### uapi_watchdog_disable <a id="uapi_watchdog_disable"></a>

```c
errcode_t uapi_watchdog_disable(void)
```

**头文件清单**

```c
#include "include/driver/watchdog.h"
```

**功能说明**

- 去使能看门狗，关闭看门狗计数
- 通过HAL调用硬件关闭看门狗
- 将看门狗使能状态标记为false

**前置条件**

- 模块初始化状态：看门狗模块已通过uapi_watchdog_init()初始化

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 去使能成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [sle_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_server/src/sle_speed_server.c)


### uapi_watchdog_kick <a id="uapi_watchdog_kick"></a>

```c
errcode_t uapi_watchdog_kick(void)
```

**头文件清单**

```c
#include "include/driver/watchdog.h"
```

**功能说明**

- 喂狗操作，重置看门狗计数器，防止超时触发
- 当定义CONFIG_WATCHDOG_SUPPORT_ULP_WDT且值为1时，同时执行ULP看门狗喂狗
- 看门狗使能状态下方可调用，否则返回失败

**前置条件**

- 模块使能状态：看门狗已通过uapi_watchdog_enable()使能

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 喂狗成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [i2s_dma_lli_master_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/i2s_dma_lli/i2s_dma_lli_master_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_WATCHDOG_SUPPORT_ULP_WDT | 特性宏 | 支持ULP看门狗喂狗特性 | n |

### uapi_watchdog_set_time <a id="uapi_watchdog_set_time"></a>

```c
errcode_t uapi_watchdog_set_time(uint32_t timeout)
```

**头文件清单**

```c
#include "include/driver/watchdog.h"
```

**功能说明**

- 设置看门狗超时时间，单位为秒
- 若看门狗当前处于使能状态，将先去使能再设置超时时间
- 设置完成后需重新调用uapi_watchdog_enable()使能看门狗

**前置条件**

- 模块初始化状态：看门狗模块已通过uapi_watchdog_init()初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| timeout | uint32_t | 看门狗超时时间，单位秒 | 大于0 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 设置超时时间成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_WATCHDOG_SUPPORT_LPM | 特性宏 | 支持低功耗模式下保存超时时间特性 | n |

### uapi_watchdog_get_left_time <a id="uapi_watchdog_get_left_time"></a>

```c
errcode_t uapi_watchdog_get_left_time(uint32_t *timeout)
```

**头文件清单**

```c
#include "include/driver/watchdog.h"
```

**功能说明**

- 获取看门狗计数器的剩余时间值
- 通过HAL读取硬件计数器当前值
- 看门狗使能状态下方可调用，否则返回失败

**前置条件**

- 模块使能状态：看门狗已通过uapi_watchdog_enable()使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| timeout | uint32_t * | 输出剩余时间的指针 | 非NULL，指向有效内存空间 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| timeout | uint32_t * | 看门狗计数器剩余时间值 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 获取剩余时间成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### uapi_register_watchdog_callback <a id="uapi_register_watchdog_callback"></a>

```c
errcode_t uapi_register_watchdog_callback(watchdog_callback_t callback)
```

**头文件清单**

```c
#include "include/driver/watchdog.h"
```

**功能说明**

- 注册看门狗超时回调函数，当看门狗触发时调用回调处理异常
- 在中断模式下，回调中须执行喂狗操作，否则系统将重启
- 回调函数通过HAL注册到硬件中断处理

**前置条件**

- 模块初始化状态：看门狗模块已通过uapi_watchdog_init()初始化
- 参数合法性：callback不为NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| callback | [watchdog_callback_t](#typedef_watchdog_callback_t) | 看门狗超时回调函数指针 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 注册回调成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [watchdog_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/watchdog/watchdog_demo.c)


### uapi_watchdog_resume <a id="uapi_watchdog_resume"></a>

```c
errcode_t uapi_watchdog_resume(uintptr_t arg)
```

**头文件清单**

```c
#include "include/driver/watchdog.h"
```

**功能说明**

- 低功耗模式下恢复看门狗模块
- 恢复看门狗超时时间与触发模式配置
- 重新使能看门狗硬件

**前置条件**

- 模块初始化状态：看门狗模块已通过uapi_watchdog_init()初始化
- 低功耗特性已使能：CONFIG_WATCHDOG_SUPPORT_LPM (Low Power Management)已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| arg | uintptr_t | 恢复参数 | - |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 恢复看门狗成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_WATCHDOG_SUPPORT_LPM | 特性宏 | 支持看门狗低功耗模式特性 | n |

### uapi_watchdog_suspend <a id="uapi_watchdog_suspend"></a>

```c
errcode_t uapi_watchdog_suspend(uintptr_t arg)
```

**头文件清单**

```c
#include "include/driver/watchdog.h"
```

**功能说明**

- 低功耗模式下挂起看门狗模块
- 当前实现直接返回成功，不执行硬件操作

**前置条件**

- 低功耗特性已使能：CONFIG_WATCHDOG_SUPPORT_LPM已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| arg | uintptr_t | 挂起参数 | - |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 挂起看门狗成功 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_WATCHDOG_SUPPORT_LPM | 特性宏 | 支持看门狗低功耗模式特性 | n |

## Type definitions

### watchdog_callback_t <a id="typedef_watchdog_callback_t"></a>

```c
typedef errcode_t (*watchdog_callback_t)(uintptr_t param);
```

**使用说明**

- 作为uapi_register_watchdog_callback接口的入参类型，用于注册看门狗超时回调函数

## Enumerations

### wdt_mode_t <a id="enum_wdt_mode_t"></a>

```c
typedef enum {
    WDT_MODE_RESET = 0,     /** 当看门狗触发时，将重启系统。 */
    WDT_MODE_INTERRUPT,     /** 当看门狗触发时，将进入中断。如果在中断中没有喂狗，系统将重启。 */
    WDT_MODE_MAX
} wdt_mode_t;
```

**使用说明**

- 看门狗触发模式枚举，作为 `uapi_watchdog_enable` 接口的入参类型

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| WDT_MODE_RESET | 0 | 看门狗触发时直接重启系统 |
| WDT_MODE_INTERRUPT | 1 | 看门狗触发时进入中断，若中断中未喂狗则系统重启 |
| WDT_MODE_MAX | 2 | 模式上限值，非法取值 |




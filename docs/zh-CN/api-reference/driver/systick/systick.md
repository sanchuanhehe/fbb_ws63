# SysTick

SysTick 是 RISC-V 内核内置的硬件定时器，通常在 LiteOS/FreeRTOS 中被配置为 1ms 周期性中断作为系统心跳（System Tick）。所有 OS (Operating System) 延时、任务调度、软件定时器都依赖这个心跳。

提供精确计时（`uapi_systick_get_s/ms/us()`）和阻塞延时（`uapi_systick_delay_s/ms/us()`）接口。

**头文件清单**

```c
#include "include/driver/systick.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_systick_init](#uapi_systick_init) | 初始化Systick |
| [uapi_systick_deinit](#uapi_systick_deinit) | 去初始化Systick |
| [uapi_systick_count_clear](#uapi_systick_count_clear) | 清除Systick计数值 |
| [uapi_systick_get_count](#uapi_systick_get_count) | 获取Systick当前计数值 |
| [uapi_systick_get_s](#uapi_systick_get_s) | 获取Systick当前计数秒值 |
| [uapi_systick_get_ms](#uapi_systick_get_ms) | 获取Systick当前计数毫秒值 |
| [uapi_systick_get_us](#uapi_systick_get_us) | 获取Systick当前计数微秒值 |
| [uapi_systick_delay_count](#uapi_systick_delay_count) | 按计数值延时 |
| [uapi_systick_delay_s](#uapi_systick_delay_s) | 按秒数延时 |
| [uapi_systick_delay_ms](#uapi_systick_delay_ms) | 按毫秒数延时 |
| [uapi_systick_delay_us](#uapi_systick_delay_us) | 按微秒数延时 |
| [uapi_systick_suspend](#uapi_systick_suspend) | 挂起Systick |
| [uapi_systick_resume](#uapi_systick_resume) | 恢复Systick |

## Functions

### uapi_systick_init <a id="uapi_systick_init"></a>

```c
void uapi_systick_init(void)
```

**头文件清单**

```c
#include "include/driver/systick.h"
```

**功能说明**

- 初始化Systick定时器，启动硬件计数
- 内部使用中断锁保护，防止并发初始化
- 重复调用时直接返回，不会重复初始化

**前置条件**

- Systick模块尚未初始化，或已通过uapi_systick_deinit()去初始化
- CONFIG_SYSTICK_SUPPORT_LPM (Low Power Management)宏未开启时，不涉及低功耗补偿逻辑

**参考案例**

- [systick_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/systick/systick_demo.c)


### uapi_systick_deinit <a id="uapi_systick_deinit"></a>

```c
void uapi_systick_deinit(void)
```

**头文件清单**

```c
#include "include/driver/systick.h"
```

**功能说明**

- 去初始化Systick定时器，停止硬件计数
- 内部使用中断锁保护，防止并发去初始化
- 未初始化时调用直接返回

**前置条件**

- Systick模块已通过uapi_systick_init()初始化完成

### uapi_systick_count_clear <a id="uapi_systick_count_clear"></a>

```c
errcode_t uapi_systick_count_clear(void)
```

**头文件清单**

```c
#include "include/driver/systick.h"
```

**功能说明**

- 清除Systick当前计数值，将计数归零
- 内部使用中断锁保护，确保清零操作的原子性
- 清除后重新从零开始计数

**前置条件**

- Systick模块已通过uapi_systick_init()初始化完成

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 计数清零成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

### uapi_systick_get_count <a id="uapi_systick_get_count"></a>

```c
uint64_t uapi_systick_get_count(void)
```

**头文件清单**

```c
#include "include/driver/systick.h"
```

**功能说明**

- 获取Systick当前原始计数值
- 内部使用中断锁保护，确保读取的原子性
- 开启CONFIG_SYSTICK_SUPPORT_LPM时，返回值包含低功耗休眠补偿计数

**前置条件**

- Systick模块已通过uapi_systick_init()初始化完成

**返回值**

- 返回类型：uint64_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 计数值为零 | 模块未初始化或计数已清零 |
| 其他值 | 当前Systick计数值 | 模块已初始化且正常计数中 |

### uapi_systick_get_s <a id="uapi_systick_get_s"></a>

```c
uint64_t uapi_systick_get_s(void)
```

**头文件清单**

```c
#include "include/driver/systick.h"
```

**功能说明**

- 获取Systick当前计数对应的秒值
- 内部通过uapi_systick_get_count()获取原始计数后转换为秒
- 开启CONFIG_SYSTICK_SUPPORT_LPM时，返回值包含低功耗休眠补偿

**前置条件**

- Systick模块已通过uapi_systick_init()初始化完成

**返回值**

- 返回类型：uint64_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 其他值 | Systick当前计数秒值 | 模块已初始化且正常计数中 |

**参考案例**

- [systick_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/systick/systick_demo.c)


### uapi_systick_get_ms <a id="uapi_systick_get_ms"></a>

```c
uint64_t uapi_systick_get_ms(void)
```

**头文件清单**

```c
#include "include/driver/systick.h"
```

**功能说明**

- 获取Systick当前计数对应的毫秒值
- 内部通过uapi_systick_get_count()获取原始计数后转换为毫秒
- 开启CONFIG_SYSTICK_SUPPORT_LPM时，返回值包含低功耗休眠补偿

**前置条件**

- Systick模块已通过uapi_systick_init()初始化完成

**返回值**

- 返回类型：uint64_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 其他值 | Systick当前计数毫秒值 | 模块已初始化且正常计数中 |

**参考案例**

- [systick_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/systick/systick_demo.c)


### uapi_systick_get_us <a id="uapi_systick_get_us"></a>

```c
uint64_t uapi_systick_get_us(void)
```

**头文件清单**

```c
#include "include/driver/systick.h"
```

**功能说明**

- 获取Systick当前计数对应的微秒值
- 内部通过uapi_systick_get_count()获取原始计数后转换为微秒
- 开启CONFIG_SYSTICK_SUPPORT_LPM时，返回值包含低功耗休眠补偿

**前置条件**

- Systick模块已通过uapi_systick_init()初始化完成

**返回值**

- 返回类型：uint64_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 其他值 | Systick当前计数微秒值 | 模块已初始化且正常计数中 |

**参考案例**

- [ble_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/ble/ble_speed_client/src/ble_speed_client.c)


### uapi_systick_delay_count <a id="uapi_systick_delay_count"></a>

```c
errcode_t uapi_systick_delay_count(uint64_t c_delay)
```

**头文件清单**

```c
#include "include/driver/systick.h"
```

**功能说明**

- 按Systick原始计数值进行忙等延时
- 通过轮询uapi_systick_get_count()直到差值达到指定计数值
- 延时期间占用CPU，不释放线程资源

**前置条件**

- Systick模块已通过uapi_systick_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| c_delay | uint64_t | 延时计数值 | 大于0 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 延时完成 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

### uapi_systick_delay_s <a id="uapi_systick_delay_s"></a>

```c
errcode_t uapi_systick_delay_s(uint32_t s_delay)
```

**头文件清单**

```c
#include "include/driver/systick.h"
```

**功能说明**

- 按秒数进行忙等延时
- 内部将秒数转换为计数值后调用uapi_systick_delay_count()
- 延时期间占用CPU，不释放线程资源

**前置条件**

- Systick模块已通过uapi_systick_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| s_delay | uint32_t | 延时秒数 | 大于0 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 延时完成 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [systick_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/systick/systick_demo.c)


### uapi_systick_delay_ms <a id="uapi_systick_delay_ms"></a>

```c
errcode_t uapi_systick_delay_ms(uint32_t m_delay)
```

**头文件清单**

```c
#include "include/driver/systick.h"
```

**功能说明**

- 按毫秒数进行忙等延时
- 内部将毫秒数转换为计数值后调用uapi_systick_delay_count()
- 延时期间占用CPU，不释放线程资源

**前置条件**

- Systick模块已通过uapi_systick_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| m_delay | uint32_t | 延时毫秒数 | 大于0 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 延时完成 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [systick_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/systick/systick_demo.c)


### uapi_systick_delay_us <a id="uapi_systick_delay_us"></a>

```c
errcode_t uapi_systick_delay_us(uint32_t u_delay)
```

**头文件清单**

```c
#include "include/driver/systick.h"
```

**功能说明**

- 按微秒数进行忙等延时
- 内部将微秒数转换为计数值后调用uapi_systick_delay_count()
- 延时期间占用CPU，不释放线程资源

**前置条件**

- Systick模块已通过uapi_systick_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| u_delay | uint32_t | 延时微秒数 | 大于0 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 延时完成 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [systick_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/systick/systick_demo.c)


### uapi_systick_suspend <a id="uapi_systick_suspend"></a>

```c
errcode_t uapi_systick_suspend(uintptr_t arg)
```

**头文件清单**

```c
#include "include/driver/systick.h"
```

**功能说明**

- 挂起Systick，保存当前计数值用于低功耗恢复补偿
- 内部使用中断锁保护，保存当前休眠补偿计数
- 若arg不为NULL，将当前计数值写入arg指向的地址

**前置条件**

- Systick模块已通过uapi_systick_init()初始化完成
- CONFIG_SYSTICK_SUPPORT_LPM宏已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| arg | uintptr_t | 挂起参数，指向uint64_t类型变量的指针，用于接收当前计数值 | NULL或指向有效的uint64_t变量地址 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 挂起成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_SYSTICK_SUPPORT_LPM | 特性宏 | 支持Systick低功耗挂起/恢复功能 | n |

### uapi_systick_resume <a id="uapi_systick_resume"></a>

```c
errcode_t uapi_systick_resume(uintptr_t arg)
```

**头文件清单**

```c
#include "include/driver/systick.h"
```

**功能说明**

- 恢复Systick，从低功耗状态恢复计数并补偿休眠期间丢失的计数值
- 内部先去初始化再重新初始化，清除硬件计数后设置补偿值
- 补偿值 = 挂起时保存的计数值 + arg指向的休眠期间补偿值

**前置条件**

- Systick模块已通过uapi_systick_suspend()挂起
- CONFIG_SYSTICK_SUPPORT_LPM宏已开启
- arg指向有效的uint64_t变量，包含休眠期间的补偿计数值

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| arg | uintptr_t | 恢复参数，指向uint64_t类型变量的指针，提供休眠期间补偿计数值 | 指向有效的uint64_t变量地址，不可为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 恢复成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_SYSTICK_SUPPORT_LPM | 特性宏 | 支持Systick低功耗挂起/恢复功能 | n |

## Type definitions

### errcode_t <a id="typedef_errcode_t"></a>

```c
typedef uint32_t errcode_t;
```

**使用说明**

在uapi_systick_count_clear、uapi_systick_delay_count、uapi_systick_delay_s、uapi_systick_delay_ms、uapi_systick_delay_us、uapi_systick_suspend、uapi_systick_resume接口中作为返回值类型使用




# TCXO

TCXO (Temperature Compensated Crystal Oscillator) 温度补偿晶振，通过温度补偿电路确保射频通信时钟的频率精度。在 2.4GHz 频段，50ppm 频率误差约等于 120kHz 偏移，超出接收机容限——TCXO 是保证无线通信可靠性的关键组件。

使用前需从 eFuse (Electronic Fuse) 中读取出厂校准值写入 TCXO，校准需在 RF (Radio Frequency) 协议栈启动前完成。提供高精度延时和微秒级时间戳接口。

**头文件清单**

```c
#include "include/driver/tcxo.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_tcxo_init](#uapi_tcxo_init) | 初始化TCXO |
| [uapi_tcxo_deinit](#uapi_tcxo_deinit) | 去初始化TCXO |
| [uapi_tcxo_get_count](#uapi_tcxo_get_count) | 获取TCXO计数值 |
| [uapi_tcxo_get_ms](#uapi_tcxo_get_ms) | 获取TCXO计数毫秒值 |
| [uapi_tcxo_get_us](#uapi_tcxo_get_us) | 获取TCXO计数微秒值 |
| [uapi_tcxo_delay_ms](#uapi_tcxo_delay_ms) | 设置延迟毫秒数 |
| [uapi_tcxo_delay_us](#uapi_tcxo_delay_us) | 设置延迟微秒数 |
| [uapi_tcxo_suspend](#uapi_tcxo_suspend) | 挂起TCXO |
| [uapi_tcxo_resume](#uapi_tcxo_resume) | 恢复TCXO |

## Functions

### uapi_tcxo_init <a id="uapi_tcxo_init"></a>

```c
errcode_t uapi_tcxo_init(void)
```

**头文件清单**

```c
#include "include/driver/tcxo.h"
```

**功能说明**

- 初始化TCXO模块，完成底层硬件计数器初始化
- 重复调用时直接返回成功，不会重复初始化
- 初始化过程中通过中断锁保护全局初始化状态

**前置条件**

- TCXO模块未被初始化或已通过uapi_tcxo_deinit()去初始化
- 底层HAL (Hardware Abstraction Layer) 层tcxo硬件资源可用

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 初始化成功或已初始化 |
| Other | 其他错误码，参考errcode_t | 初始化失败 |

**参考案例**

- [tcxo_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/tcxo/tcxo_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_TCXO_SUPPORT_LPM | 特性宏 | 支持TCXO低功耗特性 | n |

### uapi_tcxo_deinit <a id="uapi_tcxo_deinit"></a>

```c
errcode_t uapi_tcxo_deinit(void)
```

**头文件清单**

```c
#include "include/driver/tcxo.h"
```

**功能说明**

- 去初始化TCXO模块，释放底层硬件资源
- 未初始化时调用直接返回成功
- 去初始化过程中通过中断锁保护全局初始化状态

**前置条件**

- TCXO模块已通过uapi_tcxo_init()初始化完成

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 去初始化成功或未初始化 |
| Other | 其他错误码，参考errcode_t | 去初始化失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_TCXO_SUPPORT_LPM | 特性宏 | 支持TCXO低功耗特性 | n |

### uapi_tcxo_get_count <a id="uapi_tcxo_get_count"></a>

```c
uint64_t uapi_tcxo_get_count(void)
```

**头文件清单**

```c
#include "include/driver/tcxo.h"
```

**功能说明**

- 获取TCXO当前计数值（tick数）
- 低功耗模式下返回值包含休眠补偿计数
- 未初始化时返回0

**前置条件**

- TCXO模块已通过uapi_tcxo_init()初始化完成

**返回值**

- 返回类型：uint64_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 计数值为0 | TCXO未初始化 |
| 非零值 | TCXO当前计数值 | TCXO已初始化，正常返回计数 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_TCXO_SUPPORT_LPM | 特性宏 | 支持TCXO低功耗特性 | n |

### uapi_tcxo_get_ms <a id="uapi_tcxo_get_ms"></a>

```c
uint64_t uapi_tcxo_get_ms(void)
```

**头文件清单**

```c
#include "include/driver/tcxo.h"
```

**功能说明**

- 获取TCXO当前计数毫秒值
- 返回值由tick数除以每微秒tick数与1000的乘积换算得到
- 未初始化时返回0

**前置条件**

- TCXO模块已通过uapi_tcxo_init()初始化完成

**返回值**

- 返回类型：uint64_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 毫秒值为0 | TCXO未初始化 |
| 非零值 | TCXO当前计数毫秒值 | TCXO已初始化，正常返回毫秒值 |

**参考案例**

- [tcxo_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/tcxo/tcxo_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_TCXO_SUPPORT_LPM | 特性宏 | 支持TCXO低功耗特性 | n |

### uapi_tcxo_get_us <a id="uapi_tcxo_get_us"></a>

```c
uint64_t uapi_tcxo_get_us(void)
```

**头文件清单**

```c
#include "include/driver/tcxo.h"
```

**功能说明**

- 获取TCXO当前计数微秒值
- 返回值由tick数除以每微秒tick数换算得到
- 未初始化时返回0

**前置条件**

- TCXO模块已通过uapi_tcxo_init()初始化完成

**返回值**

- 返回类型：uint64_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 微秒值为0 | TCXO未初始化 |
| 非零值 | TCXO当前计数微秒值 | TCXO已初始化，正常返回微秒值 |

**参考案例**

- [sle_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_client/src/sle_speed_client.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_TCXO_SUPPORT_LPM | 特性宏 | 支持TCXO低功耗特性 | n |

### uapi_tcxo_delay_ms <a id="uapi_tcxo_delay_ms"></a>

```c
errcode_t uapi_tcxo_delay_ms(uint32_t m_delay)
```

**头文件清单**

```c
#include "include/driver/tcxo.h"
```

**功能说明**

- 基于TCXO计数器设置延迟毫秒数，阻塞等待指定毫秒数
- 延迟期间通过忙等方式循环检测计数器值
- 未初始化时返回失败

**前置条件**

- TCXO模块已通过uapi_tcxo_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| m_delay | uint32_t | 延迟毫秒数 | 0 ~ 4294967295 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 延迟完成 |
| ERRCODE_FAIL(0xFFFFFFFF) | 失败 | TCXO未初始化 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [pwm_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/pwm/pwm_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_TCXO_SUPPORT_LPM | 特性宏 | 支持TCXO低功耗特性 | n |

### uapi_tcxo_delay_us <a id="uapi_tcxo_delay_us"></a>

```c
errcode_t uapi_tcxo_delay_us(uint32_t u_delay)
```

**头文件清单**

```c
#include "include/driver/tcxo.h"
```

**功能说明**

- 基于TCXO计数器设置延迟微秒数，阻塞等待指定微秒数
- 延迟期间通过忙等方式循环检测计数器值
- 未初始化时返回失败

**前置条件**

- TCXO模块已通过uapi_tcxo_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| u_delay | uint32_t | 延迟微秒数 | 0 ~ 4294967295 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 延迟完成 |
| ERRCODE_FAIL(0xFFFFFFFF) | 失败 | TCXO未初始化 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [tcxo_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/tcxo/tcxo_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_TCXO_SUPPORT_LPM | 特性宏 | 支持TCXO低功耗特性 | n |

### uapi_tcxo_suspend <a id="uapi_tcxo_suspend"></a>

```c
errcode_t uapi_tcxo_suspend(uintptr_t arg)
```

**头文件清单**

```c
#include "include/driver/tcxo.h"
```

**功能说明**

- 挂起TCXO，保存当前计数值到休眠挂起计数
- 若arg非NULL，将当前计数值写入arg指向的内存
- 用于低功耗场景下保存TCXO计数状态

**前置条件**

- TCXO模块已通过uapi_tcxo_init()初始化完成
- 已开启CONFIG_TCXO_SUPPORT_LPM (Low Power Management)配置

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| arg | uintptr_t | 挂起所需要的参数，指向uint64_t类型内存地址 | NULL或有效的uint64_t指针地址 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 挂起成功 |
| Other | 其他错误码，参考errcode_t | 挂起失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_TCXO_SUPPORT_LPM | 特性宏 | 支持TCXO低功耗管理功能 | n |

### uapi_tcxo_resume <a id="uapi_tcxo_resume"></a>

```c
errcode_t uapi_tcxo_resume(uintptr_t arg)
```

**头文件清单**

```c
#include "include/driver/tcxo.h"
```

**功能说明**

- 恢复TCXO，根据休眠补偿微秒数计算补偿计数
- 内部先去初始化再重新初始化TCXO模块
- 用于低功耗场景下恢复TCXO计数状态

**前置条件**

- TCXO模块已通过uapi_tcxo_suspend()挂起
- 已开启CONFIG_TCXO_SUPPORT_LPM配置
- arg指向的内存包含有效的休眠补偿微秒值

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| arg | uintptr_t | 恢复所需要的参数，指向uint64_t类型内存地址，内容为休眠补偿微秒数 | 有效的uint64_t指针地址，指向值非空 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 成功 | 恢复成功 |
| Other | 其他错误码，参考errcode_t | 恢复失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_TCXO_SUPPORT_LPM | 特性宏 | 支持TCXO低功耗管理功能 | n |

## Type definitions

### errcode_t <a id="typedef_errcode_t"></a>

```c
typedef uint32_t errcode_t;
```

**使用说明**

uapi_tcxo_init、uapi_tcxo_deinit、uapi_tcxo_delay_ms、uapi_tcxo_delay_us、uapi_tcxo_suspend、uapi_tcxo_resume接口的返回值类型




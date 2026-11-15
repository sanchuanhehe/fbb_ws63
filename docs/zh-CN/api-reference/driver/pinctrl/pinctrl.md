# Pinctrl

WS63 引脚为多功能复用——同一物理引脚可通过 Pinctrl (Pin Control) 动态切换为 GPIO (General Purpose Input/Output)、UART (Universal Asynchronous Receiver/Transmitter)、SPI (Serial Peripheral Interface)、I2C (Inter-Integrated Circuit) 等不同功能。支持三种可配置属性：功能模式（Pin Mode）、驱动强度（Drive Strength）、上下拉（Pull-up/Pull-down）。

配置时遵循 get → set → 回读验证的标准流程，确保引脚配置正确生效。

**头文件清单**

```c
#include "include/driver/pinctrl.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_pin_init](#uapi_pin_init) | 初始化Pinctrl模块 |
| [uapi_pin_deinit](#uapi_pin_deinit) | 去初始化Pinctrl模块 |
| [uapi_pin_set_mode](#uapi_pin_set_mode) | 设置引脚复用模式 |
| [uapi_pin_get_mode](#uapi_pin_get_mode) | 获取引脚复用模式 |
| [uapi_pin_set_ds](#uapi_pin_set_ds) | 设置引脚驱动能力 |
| [uapi_pin_get_ds](#uapi_pin_get_ds) | 获取引脚驱动能力 |
| [uapi_pin_set_pull](#uapi_pin_set_pull) | 设置引脚上下拉 |
| [uapi_pin_get_pull](#uapi_pin_get_pull) | 获取引脚上下拉状态 |
| [uapi_pin_set_ie](#uapi_pin_set_ie) | 设置引脚输入使能状态 |
| [uapi_pin_get_ie](#uapi_pin_get_ie) | 获取引脚输入使能状态 |
| [uapi_pin_set_st](#uapi_pin_set_st) | 设置引脚施密特触发状态 |
| [uapi_pin_get_st](#uapi_pin_get_st) | 获取引脚施密特触发状态 |
| [uapi_pin_suspend](#uapi_pin_suspend) | 挂起PINCTRL |
| [uapi_pin_resume](#uapi_pin_resume) | 恢复PINCTRL |

## Functions

### uapi_pin_init <a id="uapi_pin_init"></a>

```c
void uapi_pin_init(void)
```

**头文件清单**

```c
#include "include/driver/pinctrl.h"
```

**功能说明**

- 初始化Pinctrl模块，注册HAL (Hardware Abstraction Layer) 层函数句柄
- 该函数应在本模块其他函数被调用前执行
- 重复调用不会产生异常，但会重新注册HAL层函数句柄

**前置条件**

- 无

**参考案例**

- [pinctrl_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/pinctrl/pinctrl_demo.c)


### uapi_pin_deinit <a id="uapi_pin_deinit"></a>

```c
void uapi_pin_deinit(void)
```

**头文件清单**

```c
#include "include/driver/pinctrl.h"
```

**功能说明**

- 去初始化Pinctrl模块，注销HAL层函数句柄
- 去初始化后，本模块其他函数不可再调用，否则返回ERRCODE_PIN_NOT_INIT
- 应与uapi_pin_init配对使用

**前置条件**

- 模块初始化状态：已通过uapi_pin_init()初始化完成

**参考案例**

- [pinctrl_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/pinctrl/pinctrl_demo.c)


### uapi_pin_set_mode <a id="uapi_pin_set_mode"></a>

```c
errcode_t uapi_pin_set_mode(pin_t pin, pin_mode_t mode)
```

**头文件清单**

```c
#include "include/driver/pinctrl.h"
```

**功能说明**

- 设置指定引脚的复用模式
- 调用前会校验引脚编号与复用模式的合法性，以及该引脚是否支持所设置的模式
- 设置操作在中断保护下执行，保证并发安全

**前置条件**

- 模块初始化状态：已通过uapi_pin_init()初始化完成
- 参数合法性：pin取值小于PIN_MAX_NUMBER(25)，mode取值小于PIN_MODE_MAX(8)，且该引脚支持所设置的复用模式

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pin | [pin_t](#enum_pin_t) | 引脚编号 | GPIO_00(0) ~ GPIO_18(18), SFC_CLK(19), SFC_CSN(20), SFC_IO0(21), SFC_IO1(22), SFC_IO2(23), SFC_IO3(24) |
| mode | [pin_mode_t](#enum_pin_mode_t) | 复用模式 | PIN_MODE_0(0) ~ PIN_MODE_7(7) |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [blinky_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/blinky/blinky_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_PINCTRL | 特性宏 | 支持 PINCTRL 接口功能 | y |

### uapi_pin_get_mode <a id="uapi_pin_get_mode"></a>

```c
pin_mode_t uapi_pin_get_mode(pin_t pin)
```

**头文件清单**

```c
#include "include/driver/pinctrl.h"
```

**功能说明**

- 获取指定引脚的复用模式
- 若引脚编号无效或模块未初始化，返回PIN_MODE_MAX
- 读取操作在中断保护下执行，保证并发安全

**前置条件**

- 模块初始化状态：已通过uapi_pin_init()初始化完成，否则返回PIN_MODE_MAX(8)

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pin | [pin_t](#enum_pin_t) | 引脚编号 | GPIO_00(0) ~ GPIO_18(18), SFC_CLK(19), SFC_CSN(20), SFC_IO0(21), SFC_IO1(22), SFC_IO2(23), SFC_IO3(24) |

**返回值**

- 返回类型：pin_mode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| PIN_MODE_0(0) ~ PIN_MODE_7(7) | 当前引脚复用模式 | 获取成功 |
| PIN_MODE_MAX(8) | 获取失败 | 引脚编号无效或模块未初始化 |

**参考案例**

- [pinctrl_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/pinctrl/pinctrl_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_PINCTRL | 特性宏 | 支持 PINCTRL 接口功能 | y |

### uapi_pin_set_ds <a id="uapi_pin_set_ds"></a>

```c
errcode_t uapi_pin_set_ds(pin_t pin, pin_drive_strength_t ds)
```

**头文件清单**

```c
#include "include/driver/pinctrl.h"
```

**功能说明**

- 设置指定引脚的驱动能力
- 调用前会校验引脚编号与驱动能力值的合法性
- 设置操作在中断保护下执行，保证并发安全

**前置条件**

- 模块初始化状态：已通过uapi_pin_init()初始化完成
- 参数合法性：pin取值小于PIN_MAX_NUMBER(25)，ds取值小于PIN_DS_MAX(8)

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pin | [pin_t](#enum_pin_t) | 引脚编号 | GPIO_00(0) ~ GPIO_18(18), SFC_CLK(19), SFC_CSN(20), SFC_IO0(21), SFC_IO1(22), SFC_IO2(23), SFC_IO3(24) |
| ds | [pin_drive_strength_t](#enum_pin_drive_strength_t) | 驱动能力 | PIN_DS_0(0) ~ PIN_DS_7(7) |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [pinctrl_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/pinctrl/pinctrl_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_PINCTRL | 特性宏 | 支持 PINCTRL 接口功能 | y |

### uapi_pin_get_ds <a id="uapi_pin_get_ds"></a>

```c
pin_drive_strength_t uapi_pin_get_ds(pin_t pin)
```

**头文件清单**

```c
#include "include/driver/pinctrl.h"
```

**功能说明**

- 获取指定引脚的驱动能力
- 若引脚编号无效或模块未初始化，返回PIN_DS_MAX
- 读取操作在中断保护下执行，保证并发安全

**前置条件**

- 模块初始化状态：已通过uapi_pin_init()初始化完成，否则返回PIN_DS_MAX(8)

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pin | [pin_t](#enum_pin_t) | 引脚编号 | GPIO_00(0) ~ GPIO_18(18), SFC_CLK(19), SFC_CSN(20), SFC_IO0(21), SFC_IO1(22), SFC_IO2(23), SFC_IO3(24) |

**返回值**

- 返回类型：pin_drive_strength_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| PIN_DS_0(0) ~ PIN_DS_7(7) | 当前引脚驱动能力 | 获取成功 |
| PIN_DS_MAX(8) | 获取失败 | 引脚编号无效或模块未初始化 |

**参考案例**

- [pinctrl_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/pinctrl/pinctrl_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_PINCTRL | 特性宏 | 支持 PINCTRL 接口功能 | y |

### uapi_pin_set_pull <a id="uapi_pin_set_pull"></a>

```c
errcode_t uapi_pin_set_pull(pin_t pin, pin_pull_t pull_type)
```

**头文件清单**

```c
#include "include/driver/pinctrl.h"
```

**功能说明**

- 设置指定引脚的上下拉状态
- 调用前会校验引脚编号与上下拉类型的合法性
- 设置操作在中断保护下执行，保证并发安全

**前置条件**

- 模块初始化状态：已通过uapi_pin_init()初始化完成
- 参数合法性：pin取值小于PIN_MAX_NUMBER(25)，pull_type取值小于PIN_PULL_MAX(4)

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pin | [pin_t](#enum_pin_t) | 引脚编号 | GPIO_00(0) ~ GPIO_18(18), SFC_CLK(19), SFC_CSN(20), SFC_IO0(21), SFC_IO1(22), SFC_IO2(23), SFC_IO3(24) |
| pull_type | [pin_pull_t](#enum_pin_pull_t) | 上下拉类型 | PIN_PULL_TYPE_DISABLE(0), PIN_PULL_TYPE_DOWN(1), PIN_PULL_TYPE_STRONG_UP(2), PIN_PULL_TYPE_UP(3) |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [pinctrl_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/pinctrl/pinctrl_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_PINCTRL | 特性宏 | 支持 PINCTRL 接口功能 | y |

### uapi_pin_get_pull <a id="uapi_pin_get_pull"></a>

```c
pin_pull_t uapi_pin_get_pull(pin_t pin)
```

**头文件清单**

```c
#include "include/driver/pinctrl.h"
```

**功能说明**

- 获取指定引脚的上下拉状态
- 若引脚编号无效或模块未初始化，返回PIN_PULL_MAX
- 读取操作在中断保护下执行，保证并发安全

**前置条件**

- 模块初始化状态：已通过uapi_pin_init()初始化完成，否则返回PIN_PULL_MAX(4)

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pin | [pin_t](#enum_pin_t) | 引脚编号 | GPIO_00(0) ~ GPIO_18(18), SFC_CLK(19), SFC_CSN(20), SFC_IO0(21), SFC_IO1(22), SFC_IO2(23), SFC_IO3(24) |

**返回值**

- 返回类型：pin_pull_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| PIN_PULL_TYPE_DISABLE(0) | 上下拉禁用 | 当前引脚无上下拉 |
| PIN_PULL_TYPE_DOWN(1) | 下拉 | 当前引脚为下拉 |
| PIN_PULL_TYPE_STRONG_UP(2) | 强上拉 | 当前引脚为强上拉 |
| PIN_PULL_TYPE_UP(3) | 上拉 | 当前引脚为上拉 |
| PIN_PULL_MAX(4) | 获取失败 | 引脚编号无效或模块未初始化 |

**参考案例**

- [pinctrl_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/pinctrl/pinctrl_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_PINCTRL | 特性宏 | 支持 PINCTRL 接口功能 | y |

### uapi_pin_set_ie <a id="uapi_pin_set_ie"></a>

```c
errcode_t uapi_pin_set_ie(pin_t pin, pin_input_enable_t ie)
```

**头文件清单**

```c
#include "include/driver/pinctrl.h"
```

**功能说明**

- 设置指定引脚的输入使能状态
- 调用前会校验引脚编号与输入使能值的合法性
- 设置操作在中断保护下执行，保证并发安全

**前置条件**

- 模块初始化状态：已通过uapi_pin_init()初始化完成
- 参数合法性：pin取值小于PIN_MAX_NUMBER(25)，ie取值小于PIN_IE_MAX(2)

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pin | [pin_t](#enum_pin_t) | 引脚编号 | GPIO_00(0) ~ GPIO_18(18), SFC_CLK(19), SFC_CSN(20), SFC_IO0(21), SFC_IO1(22), SFC_IO2(23), SFC_IO3(24) |
| ie | [pin_input_enable_t](#enum_pin_input_enable_t) | 输入使能状态 | PIN_IE_DISABLE(0), PIN_IE_ENABLE(1) |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [uart_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/uart/uart_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_PINCTRL | 特性宏 | 支持 PINCTRL 接口功能 | y |
| CONFIG_PINCTRL_SUPPORT_IE | 特性宏 | 支持引脚输入使能特性 | n |

### uapi_pin_get_ie <a id="uapi_pin_get_ie"></a>

```c
pin_input_enable_t uapi_pin_get_ie(pin_t pin)
```

**头文件清单**

```c
#include "include/driver/pinctrl.h"
```

**功能说明**

- 获取指定引脚的输入使能状态
- 若引脚编号无效或模块未初始化，返回PIN_IE_MAX
- 读取操作在中断保护下执行，保证并发安全

**前置条件**

- 模块初始化状态：已通过uapi_pin_init()初始化完成，否则返回PIN_IE_MAX(2)

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pin | [pin_t](#enum_pin_t) | 引脚编号 | GPIO_00(0) ~ GPIO_18(18), SFC_CLK(19), SFC_CSN(20), SFC_IO0(21), SFC_IO1(22), SFC_IO2(23), SFC_IO3(24) |

**返回值**

- 返回类型：pin_input_enable_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| PIN_IE_DISABLE(0) | 输入禁用 | 当前引脚输入未使能 |
| PIN_IE_ENABLE(1) | 输入使能 | 当前引脚输入已使能 |
| PIN_IE_MAX(2) | 获取失败 | 引脚编号无效或模块未初始化 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_PINCTRL | 特性宏 | 支持 PINCTRL 接口功能 | y |
| CONFIG_PINCTRL_SUPPORT_IE | 特性宏 | 支持引脚输入使能特性 | n |

### uapi_pin_set_st <a id="uapi_pin_set_st"></a>

```c
errcode_t uapi_pin_set_st(pin_t pin, pin_schmitt_trigger_t st)
```

**头文件清单**

```c
#include "include/driver/pinctrl.h"
```

**功能说明**

- 设置指定引脚的施密特触发状态
- 调用前会校验引脚编号与施密特触发值的合法性
- 设置操作在中断保护下执行，保证并发安全

**前置条件**

- 模块初始化状态：已通过uapi_pin_init()初始化完成
- 参数合法性：pin取值小于PIN_MAX_NUMBER(25)，st取值小于PIN_ST_MAX(2)

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pin | [pin_t](#enum_pin_t) | 引脚编号 | GPIO_00(0) ~ GPIO_18(18), SFC_CLK(19), SFC_CSN(20), SFC_IO0(21), SFC_IO1(22), SFC_IO2(23), SFC_IO3(24) |
| st | [pin_schmitt_trigger_t](#enum_pin_schmitt_trigger_t) | 施密特触发状态 | PIN_ST_DISABLE(0), PIN_ST_ENABLE(1) |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_PINCTRL | 特性宏 | 支持 PINCTRL 接口功能 | y |
| CONFIG_PINCTRL_SUPPORT_ST | 特性宏 | 支持引脚施密特触发特性 | n |

### uapi_pin_get_st <a id="uapi_pin_get_st"></a>

```c
pin_schmitt_trigger_t uapi_pin_get_st(pin_t pin)
```

**头文件清单**

```c
#include "include/driver/pinctrl.h"
```

**功能说明**

- 获取指定引脚的施密特触发状态
- 若引脚编号无效或模块未初始化，返回PIN_ST_MAX
- 读取操作在中断保护下执行，保证并发安全

**前置条件**

- 模块初始化状态：已通过uapi_pin_init()初始化完成，否则返回PIN_ST_MAX(2)

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pin | [pin_t](#enum_pin_t) | 引脚编号 | GPIO_00(0) ~ GPIO_18(18), SFC_CLK(19), SFC_CSN(20), SFC_IO0(21), SFC_IO1(22), SFC_IO2(23), SFC_IO3(24) |

**返回值**

- 返回类型：pin_schmitt_trigger_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| PIN_ST_DISABLE(0) | 施密特触发禁用 | 当前引脚施密特触发未使能 |
| PIN_ST_ENABLE(1) | 施密特触发使能 | 当前引脚施密特触发已使能 |
| PIN_ST_MAX(2) | 获取失败 | 引脚编号无效或模块未初始化 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_PINCTRL | 特性宏 | 支持 PINCTRL 接口功能 | y |
| CONFIG_PINCTRL_SUPPORT_ST | 特性宏 | 支持引脚施密特触发特性 | n |

### uapi_pin_suspend <a id="uapi_pin_suspend"></a>

```c
errcode_t uapi_pin_suspend(uintptr_t arg)
```

**头文件清单**

```c
#include "include/driver/pinctrl.h"
```

**功能说明**

- 挂起PINCTRL模块，用于低功耗场景
- 当前实现直接返回ERRCODE_SUCC，arg参数未使用

**前置条件**

- 模块初始化状态：已通过uapi_pin_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| arg | uintptr_t | 挂起参数 | 当前实现未使用 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 挂起成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_PINCTRL | 特性宏 | 支持 PINCTRL 接口功能 | y |
| CONFIG_PINCTRL_SUPPORT_LPM | 特性宏 | 支持低功耗挂起/恢复特性 | n |

### uapi_pin_resume <a id="uapi_pin_resume"></a>

```c
errcode_t uapi_pin_resume(uintptr_t arg)
```

**头文件清单**

```c
#include "include/driver/pinctrl.h"
```

**功能说明**

- 恢复PINCTRL模块，用于低功耗场景中从挂起状态恢复
- 当前实现直接返回ERRCODE_SUCC，arg参数未使用

**前置条件**

- 模块初始化状态：已通过uapi_pin_init()初始化完成
- 调用时序约束：应与uapi_pin_suspend配对调用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| arg | uintptr_t | 恢复参数 | 当前实现未使用 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 恢复成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_PINCTRL | 特性宏 | 支持 PINCTRL 接口功能 | y |
| CONFIG_PINCTRL_SUPPORT_LPM | 特性宏 | 支持低功耗挂起/恢复特性 | n |

## Enumerations

### pin_t <a id="enum_pin_t"></a>

```c
typedef enum {
    GPIO_00 = 0,
    GPIO_01 = 1,
    GPIO_02 = 2,
    GPIO_03 = 3,
    GPIO_04 = 4,
    GPIO_05 = 5,
    GPIO_06 = 6,
    GPIO_07 = 7,
    GPIO_08 = 8,
    GPIO_09 = 9,
    GPIO_10 = 10,
    GPIO_11 = 11,
    GPIO_12 = 12,
    GPIO_13 = 13,
    GPIO_14 = 14,
    GPIO_15 = 15,
    GPIO_16 = 16,
    GPIO_17 = 17,
    GPIO_18 = 18,
    SFC_CLK = 19,
    SFC_CSN = 20,
    SFC_IO0 = 21,
    SFC_IO1 = 22,
    SFC_IO2 = 23,
    SFC_IO3 = 24,
    PIN_NONE = 25,
} pin_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GPIO_00 | 0 | GPIO引脚0 |
| GPIO_01 | 1 | GPIO引脚1 |
| GPIO_02 | 2 | GPIO引脚2 |
| GPIO_03 | 3 | GPIO引脚3 |
| GPIO_04 | 4 | GPIO引脚4 |
| GPIO_05 | 5 | GPIO引脚5 |
| GPIO_06 | 6 | GPIO引脚6 |
| GPIO_07 | 7 | GPIO引脚7 |
| GPIO_08 | 8 | GPIO引脚8 |
| GPIO_09 | 9 | GPIO引脚9 |
| GPIO_10 | 10 | GPIO引脚10 |
| GPIO_11 | 11 | GPIO引脚11 |
| GPIO_12 | 12 | GPIO引脚12 |
| GPIO_13 | 13 | GPIO引脚13 |
| GPIO_14 | 14 | GPIO引脚14 |
| GPIO_15 | 15 | GPIO引脚15 |
| GPIO_16 | 16 | GPIO引脚16 |
| GPIO_17 | 17 | GPIO引脚17 |
| GPIO_18 | 18 | GPIO引脚18 |
| SFC_CLK | 19 | SFC (Serial Flash Controller) 时钟引脚 |
| SFC_CSN | 20 | SFC片选引脚 |
| SFC_IO0 | 21 | SFC数据引脚0 |
| SFC_IO1 | 22 | SFC数据引脚1 |
| SFC_IO2 | 23 | SFC数据引脚2 |
| SFC_IO3 | 24 | SFC数据引脚3 |
| PIN_NONE | 25 | 无效/未使用引脚编号，用于边界校验 |

### pin_mode_t <a id="enum_pin_mode_t"></a>

```c
typedef enum {
    PIN_MODE_0        = 0,
    PIN_MODE_1        = 1,
    PIN_MODE_2        = 2,
    PIN_MODE_3        = 3,
    PIN_MODE_4        = 4,
    PIN_MODE_5        = 5,
    PIN_MODE_6        = 6,
    PIN_MODE_7        = 7,
    PIN_MODE_MAX      = 8
} pin_mode_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| PIN_MODE_0 | 0 | 复用模式0（GPIO功能） |
| PIN_MODE_1 | 1 | 复用模式1 |
| PIN_MODE_2 | 2 | 复用模式2 |
| PIN_MODE_3 | 3 | 复用模式3 |
| PIN_MODE_4 | 4 | 复用模式4 |
| PIN_MODE_5 | 5 | 复用模式5 |
| PIN_MODE_6 | 6 | 复用模式6 |
| PIN_MODE_7 | 7 | 复用模式7 |
| PIN_MODE_MAX | 8 | 无效值，用于边界校验 |

### pin_drive_strength_t <a id="enum_pin_drive_strength_t"></a>

```c
typedef enum {
    PIN_DS_0 = 0,
    PIN_DS_1 = 1,
    PIN_DS_2 = 2,
    PIN_DS_3 = 3,
    PIN_DS_4 = 4,
    PIN_DS_5 = 5,
    PIN_DS_6 = 6,
    PIN_DS_7 = 7,
    PIN_DS_MAX = 8
} pin_drive_strength_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| PIN_DS_0 | 0 | 驱动能力等级0 |
| PIN_DS_1 | 1 | 驱动能力等级1 |
| PIN_DS_2 | 2 | 驱动能力等级2 |
| PIN_DS_3 | 3 | 驱动能力等级3 |
| PIN_DS_4 | 4 | 驱动能力等级4 |
| PIN_DS_5 | 5 | 驱动能力等级5 |
| PIN_DS_6 | 6 | 驱动能力等级6 |
| PIN_DS_7 | 7 | 驱动能力等级7 |
| PIN_DS_MAX | 8 | 无效值，用于边界校验 |

### pin_pull_t <a id="enum_pin_pull_t"></a>

```c
typedef enum {
    PIN_PULL_TYPE_DISABLE   = 0,
    PIN_PULL_TYPE_DOWN      = 1,
    PIN_PULL_TYPE_STRONG_UP = 2,
    PIN_PULL_TYPE_UP        = 3,
    PIN_PULL_MAX            = 4
} pin_pull_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| PIN_PULL_TYPE_DISABLE | 0 | 上下拉禁用 |
| PIN_PULL_TYPE_DOWN | 1 | 下拉 |
| PIN_PULL_TYPE_STRONG_UP | 2 | 强上拉 |
| PIN_PULL_TYPE_UP | 3 | 上拉 |
| PIN_PULL_MAX | 4 | 无效值，用于边界校验 |

### pin_input_enable_t <a id="enum_pin_input_enable_t"></a>

```c
typedef enum {
    PIN_IE_DISABLE = 0,
    PIN_IE_ENABLE = 1,
    PIN_IE_MAX,
} pin_input_enable_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| PIN_IE_DISABLE | 0 | 输入禁用 |
| PIN_IE_ENABLE | 1 | 输入使能 |
| PIN_IE_MAX | 2 | 无效值，用于边界校验 |

### pin_schmitt_trigger_t <a id="enum_pin_schmitt_trigger_t"></a>

```c
typedef enum {
    PIN_ST_DISABLE = 0,
    PIN_ST_ENABLE = 1,
    PIN_ST_MAX,
} pin_schmitt_trigger_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| PIN_ST_DISABLE | 0 | 施密特触发禁用 |
| PIN_ST_ENABLE | 1 | 施密特触发使能 |
| PIN_ST_MAX | 2 | 无效值，用于边界校验 |




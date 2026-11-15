# GPIO

WS63 提供 19 路 GPIO (General Purpose Input/Output)，每路可独立配置为输入或输出方向。输出模式支持电平控制和引脚翻转，输入模式支持电平读取和中断触发（上升沿/下降沿/双边沿/低电平/高电平），并可配置去抖动和中断屏蔽。

适用 LED (Light Emitting Diode) 闪烁、按键检测、继电器控制等通用数字 I/O 场景。

**头文件清单**

```c
#include "include/driver/gpio.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_gpio_init](#uapi_gpio_init) | 初始化GPIO模块 |
| [uapi_gpio_deinit](#uapi_gpio_deinit) | 去初始化GPIO模块 |
| [uapi_gpio_set_dir](#uapi_gpio_set_dir) | 设置GPIO引脚输入输出方向 |
| [uapi_gpio_get_dir](#uapi_gpio_get_dir) | 获取GPIO引脚输入输出方向 |
| [uapi_gpio_set_val](#uapi_gpio_set_val) | 设置GPIO引脚输出电平 |
| [uapi_gpio_get_output_val](#uapi_gpio_get_output_val) | 获取GPIO引脚输出电平值 |
| [uapi_gpio_get_val](#uapi_gpio_get_val) | 读取GPIO引脚输入电平值 |
| [uapi_gpio_toggle](#uapi_gpio_toggle) | 翻转GPIO引脚输出电平状态 |
| [uapi_gpio_set_isr_mode](#uapi_gpio_set_isr_mode) | 设置GPIO引脚中断触发模式 |
| [uapi_gpio_register_isr_func](#uapi_gpio_register_isr_func) | 注册GPIO引脚中断回调函数 |
| [uapi_gpio_unregister_isr_func](#uapi_gpio_unregister_isr_func) | 注销GPIO引脚中断回调函数 |
| [uapi_gpio_enable_interrupt](#uapi_gpio_enable_interrupt) | 使能GPIO引脚中断 |
| [uapi_gpio_disable_interrupt](#uapi_gpio_disable_interrupt) | 去使能GPIO引脚中断 |
| [uapi_gpio_clear_interrupt](#uapi_gpio_clear_interrupt) | 清除GPIO引脚中断状态 |
| [uapi_gpio_suspend](#uapi_gpio_suspend) | 挂起所有GPIO通道 |
| [uapi_gpio_resume](#uapi_gpio_resume) | 恢复所有GPIO通道 |
| [uapi_gpio_select_core](#uapi_gpio_select_core) | 选择GPIO引脚所属核心 |

## Functions

### uapi_gpio_init <a id="uapi_gpio_init"></a>

```c
void uapi_gpio_init(void)
```

**头文件清单**

```c
#include "include/driver/gpio.h"
```

**功能说明**

- 初始化GPIO模块，完成底层硬件资源分配与寄存器配置
- 该接口应在调用模块内其他任何接口之前执行
- 重复调用时内部判断已初始化状态，直接返回，不重复执行初始化流程

**前置条件**

- GPIO模块未被初始化，或处于去初始化状态

**参考案例**

- [radar_sta_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/sta_sample/radar_sta_sample.c)


### uapi_gpio_deinit <a id="uapi_gpio_deinit"></a>

```c
void uapi_gpio_deinit(void)
```

**头文件清单**

```c
#include "include/driver/gpio.h"
```

**功能说明**

- 去初始化GPIO模块，释放底层硬件资源
- 重复调用时内部判断未初始化状态，直接返回，不重复执行去初始化流程
- 调用后GPIO模块不可使用，需重新调用[uapi_gpio_init](#uapi_gpio_init)后方可再次使用

**前置条件**

- 已通过[uapi_gpio_init](#uapi_gpio_init)完成初始化

### uapi_gpio_set_dir <a id="uapi_gpio_set_dir"></a>

```c
errcode_t uapi_gpio_set_dir(pin_t pin, gpio_direction_t dir)
```

**头文件清单**

```c
#include "include/driver/gpio.h"
```

**功能说明**

- 设置指定GPIO引脚的输入输出方向
- 该接口在中断保护下执行方向设置操作
- 引脚方向设置后方可进行输入电平读取或输出电平控制

**前置条件**

- GPIO模块已通过[uapi_gpio_init](#uapi_gpio_init)初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pin | [pin_t](#enum_pin_t) | GPIO引脚编号 | GPIO_00(0) ~ GPIO_18(18)，PIN_NONE(25)为无效值 |
| dir | [gpio_direction_t](#enum_gpio_direction) | 输入输出方向 | [GPIO_DIRECTION_INPUT](#enum_gpio_direction)(0) / [GPIO_DIRECTION_OUTPUT](#enum_gpio_direction)(1) |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 方向设置成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [blinky_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/blinky/blinky_demo.c)


### uapi_gpio_get_dir <a id="uapi_gpio_get_dir"></a>

```c
gpio_direction_t uapi_gpio_get_dir(pin_t pin)
```

**头文件清单**

```c
#include "include/driver/gpio.h"
```

**功能说明**

- 获取指定GPIO引脚的输入输出方向
- 模块未初始化时默认返回GPIO_DIRECTION_INPUT
- 该接口通过读取底层硬件寄存器获取当前方向配置

**前置条件**

- GPIO模块已通过[uapi_gpio_init](#uapi_gpio_init)初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pin | [pin_t](#enum_pin_t) | GPIO引脚编号 | GPIO_00(0) ~ GPIO_18(18)，PIN_NONE(25)为无效值 |

**返回值**

- 返回类型：[gpio_direction_t](#enum_gpio_direction)

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| GPIO_DIRECTION_INPUT(0) | 输入方向 | 引脚配置为输入 |
| GPIO_DIRECTION_OUTPUT(1) | 输出方向 | 引脚配置为输出 |

### uapi_gpio_set_val <a id="uapi_gpio_set_val"></a>

```c
errcode_t uapi_gpio_set_val(pin_t pin, gpio_level_t level)
```

**头文件清单**

```c
#include "include/driver/gpio.h"
```

**功能说明**

- 设置指定GPIO引脚的输出电平状态
- 该接口在中断保护下执行输出设置操作
- 引脚需配置为输出方向后方可设置输出电平

**前置条件**

- GPIO模块已通过[uapi_gpio_init](#uapi_gpio_init)初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pin | [pin_t](#enum_pin_t) | GPIO引脚编号 | GPIO_00(0) ~ GPIO_18(18)，PIN_NONE(25)为无效值 |
| level | [gpio_level_t](#enum_gpio_level) | 输出电平 | GPIO_LEVEL_LOW(0)，GPIO_LEVEL_HIGH(1) |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 电平设置成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [blinky_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/blinky/blinky_demo.c)


### uapi_gpio_get_output_val <a id="uapi_gpio_get_output_val"></a>

```c
gpio_level_t uapi_gpio_get_output_val(pin_t pin)
```

**头文件清单**

```c
#include "include/driver/gpio.h"
```

**功能说明**

- 获取指定GPIO引脚的输出电平值
- 模块未初始化时默认返回GPIO_LEVEL_LOW
- 该接口读取的是输出寄存器值，非实际引脚电平

**前置条件**

- GPIO模块已通过[uapi_gpio_init](#uapi_gpio_init)初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pin | [pin_t](#enum_pin_t) | GPIO引脚编号 | GPIO_00(0) ~ GPIO_18(18)，PIN_NONE(25)为无效值 |

**返回值**

- 返回类型：[gpio_level_t](#enum_gpio_level)

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| GPIO_LEVEL_LOW(0) | 低电平 | 输出为低电平 |
| GPIO_LEVEL_HIGH(1) | 高电平 | 输出为高电平 |

### uapi_gpio_get_val <a id="uapi_gpio_get_val"></a>

```c
gpio_level_t uapi_gpio_get_val(pin_t pin)
```

**头文件清单**

```c
#include "include/driver/gpio.h"
```

**功能说明**

- 读取指定GPIO引脚的输入电平值
- 模块未初始化时默认返回GPIO_LEVEL_LOW
- 该接口读取的是实际引脚输入电平

**前置条件**

- GPIO模块已通过[uapi_gpio_init](#uapi_gpio_init)初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pin | [pin_t](#enum_pin_t) | GPIO引脚编号 | GPIO_00(0) ~ GPIO_18(18)，PIN_NONE(25)为无效值 |

**返回值**

- 返回类型：[gpio_level_t](#enum_gpio_level)

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| GPIO_LEVEL_LOW(0) | 低电平 | 输入为低电平 |
| GPIO_LEVEL_HIGH(1) | 高电平 | 输入为高电平 |

### uapi_gpio_toggle <a id="uapi_gpio_toggle"></a>

```c
errcode_t uapi_gpio_toggle(pin_t pin)
```

**头文件清单**

```c
#include "include/driver/gpio.h"
```

**功能说明**

- 翻转指定GPIO引脚的输出电平状态
- 该接口在中断保护下执行翻转操作
- 引脚需配置为输出方向后方可执行翻转

**前置条件**

- GPIO模块已通过[uapi_gpio_init](#uapi_gpio_init)初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pin | [pin_t](#enum_pin_t) | GPIO引脚编号 | GPIO_00(0) ~ GPIO_18(18)，PIN_NONE(25)为无效值 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 电平翻转成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [blinky_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/blinky/blinky_demo.c)


### uapi_gpio_set_isr_mode <a id="uapi_gpio_set_isr_mode"></a>

```c
errcode_t uapi_gpio_set_isr_mode(pin_t pin, uint32_t trigger)
```

**头文件清单**

```c
#include "include/driver/gpio.h"
```

**功能说明**

- 设置指定GPIO引脚的中断触发模式
- 该接口在中断保护下执行中断模式设置
- 可配置上升沿、下降沿、双边沿、低电平、高电平等中断触发类型

**前置条件**

- GPIO模块已通过[uapi_gpio_init](#uapi_gpio_init)初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pin | [pin_t](#enum_pin_t) | GPIO引脚编号 | GPIO_00(0) ~ GPIO_18(18)，PIN_NONE(25)为无效值 |
| trigger | uint32_t | 中断触发类型 | 0x00000001: 上升沿中断<br>0x00000002: 下降沿中断<br>0x00000003: 双边沿中断<br>0x00000004: 低电平中断<br>0x00000008: 高电平中断 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 中断模式设置成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

### uapi_gpio_register_isr_func <a id="uapi_gpio_register_isr_func"></a>

```c
errcode_t uapi_gpio_register_isr_func(pin_t pin, uint32_t trigger, gpio_callback_t callback)
```

**头文件清单**

```c
#include "include/driver/gpio.h"
```

**功能说明**

- 注册指定GPIO引脚的中断回调函数
- 该接口在中断保护下执行中断注册操作
- 同时设置中断触发模式并注册回调函数

**前置条件**

- GPIO模块已通过[uapi_gpio_init](#uapi_gpio_init)初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pin | [pin_t](#enum_pin_t) | GPIO引脚编号 | GPIO_00(0) ~ GPIO_18(18)，PIN_NONE(25)为无效值 |
| trigger | uint32_t | 中断触发类型 | 0x00000001: 上升沿中断<br>0x00000002: 下降沿中断<br>0x00000003: 双边沿中断<br>0x00000004: 低电平中断<br>0x00000008: 高电平中断 |
| callback | [gpio_callback_t](#typedef_gpio_callback_t) | 中断回调函数指针 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 中断回调注册成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

### uapi_gpio_unregister_isr_func <a id="uapi_gpio_unregister_isr_func"></a>

```c
errcode_t uapi_gpio_unregister_isr_func(pin_t pin)
```

**头文件清单**

```c
#include "include/driver/gpio.h"
```

**功能说明**

- 注销指定GPIO引脚的中断回调函数
- 该接口在中断保护下执行中断注销操作
- 注销后该引脚不再响应中断事件

**前置条件**

- GPIO模块已通过[uapi_gpio_init](#uapi_gpio_init)初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pin | [pin_t](#enum_pin_t) | GPIO引脚编号 | GPIO_00(0) ~ GPIO_18(18)，PIN_NONE(25)为无效值 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 中断回调注销成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

### uapi_gpio_enable_interrupt <a id="uapi_gpio_enable_interrupt"></a>

```c
errcode_t uapi_gpio_enable_interrupt(pin_t pin)
```

**头文件清单**

```c
#include "include/driver/gpio.h"
```

**功能说明**

- 使能指定GPIO引脚的中断
- 该接口在中断保护下执行中断使能操作
- 使能前需先通过[uapi_gpio_register_isr_func](#uapi_gpio_register_isr_func)注册中断回调

**前置条件**

- GPIO模块已通过[uapi_gpio_init](#uapi_gpio_init)初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pin | [pin_t](#enum_pin_t) | GPIO引脚编号 | GPIO_00(0) ~ GPIO_18(18)，PIN_NONE(25)为无效值 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 中断使能成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

### uapi_gpio_disable_interrupt <a id="uapi_gpio_disable_interrupt"></a>

```c
errcode_t uapi_gpio_disable_interrupt(pin_t pin)
```

**头文件清单**

```c
#include "include/driver/gpio.h"
```

**功能说明**

- 去使能指定GPIO引脚的中断
- 该接口在中断保护下执行中断去使能操作
- 去使能后该引脚不再触发中断，但回调注册仍保留

**前置条件**

- GPIO模块已通过[uapi_gpio_init](#uapi_gpio_init)初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pin | [pin_t](#enum_pin_t) | GPIO引脚编号 | GPIO_00(0) ~ GPIO_18(18)，PIN_NONE(25)为无效值 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 中断去使能成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

### uapi_gpio_clear_interrupt <a id="uapi_gpio_clear_interrupt"></a>

```c
errcode_t uapi_gpio_clear_interrupt(pin_t pin)
```

**头文件清单**

```c
#include "include/driver/gpio.h"
```

**功能说明**

- 清除指定GPIO引脚的中断状态
- 该接口在中断保护下执行中断清除操作
- 在中断回调中调用以清除中断标志位

**前置条件**

- GPIO模块已通过[uapi_gpio_init](#uapi_gpio_init)初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pin | [pin_t](#enum_pin_t) | GPIO引脚编号 | GPIO_00(0) ~ GPIO_18(18)，PIN_NONE(25)为无效值 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 中断清除成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

### uapi_gpio_suspend <a id="uapi_gpio_suspend"></a>

```c
errcode_t uapi_gpio_suspend(uintptr_t arg)
```

**头文件清单**

```c
#include "include/driver/gpio.h"
```

**功能说明**

- 挂起所有GPIO通道，进入低功耗状态
- 该接口在低功耗模式下调用
- 挂起后GPIO模块停止工作

**前置条件**

- GPIO模块已通过[uapi_gpio_init](#uapi_gpio_init)初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| arg | uintptr_t | 挂起参数 | - |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 挂起成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_GPIO_SUPPORT_LPM | 特性宏 | 支持GPIO低功耗挂起/恢复功能 | y |

### uapi_gpio_resume <a id="uapi_gpio_resume"></a>

```c
errcode_t uapi_gpio_resume(uintptr_t arg)
```

**头文件清单**

```c
#include "include/driver/gpio.h"
```

**功能说明**

- 恢复所有GPIO通道，从低功耗状态恢复
- 该接口在低功耗恢复时调用
- 恢复后GPIO模块恢复工作

**前置条件**

- GPIO模块已通过[uapi_gpio_suspend](#uapi_gpio_suspend)挂起

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| arg | uintptr_t | 恢复参数 | - |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 恢复成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_GPIO_SUPPORT_LPM | 特性宏 | 支持GPIO低功耗挂起/恢复功能 | y |

### uapi_gpio_select_core <a id="uapi_gpio_select_core"></a>

```c
void uapi_gpio_select_core(pin_t pin, cores_t core)
```

**头文件清单**

```c
#include "include/driver/gpio.h"
```

**功能说明**

- 选择指定GPIO引脚所属的核心
- 该接口将GPIO引脚中断路由到指定核心处理
- 仅在多核场景下使用

**前置条件**

- GPIO模块已通过[uapi_gpio_init](#uapi_gpio_init)初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pin | [pin_t](#enum_pin_t) | GPIO引脚编号 | GPIO_00(0) ~ GPIO_18(18)，PIN_NONE(25)为无效值 |
| core | [cores_t](#enum_cores_t) | 目标核心 | CORES_BT_CORE(0)，CORES_PROTOCOL_CORE(1)，CORES_APPS_CORE(2) |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_GPIO_SELECT_CORE | 特性宏 | 支持GPIO选择核心功能 | n |

## Type definitions

### errcode_t <a id="typedef_errcode_t"></a>

```c
typedef uint32_t errcode_t;
```

**使用说明**

作为GPIO模块所有返回errcode_t类型接口的返回值类型

### gpio_callback_t <a id="typedef_gpio_callback_t"></a>

```c
typedef void (*gpio_callback_t)(pin_t pin, uintptr_t param);
```

**使用说明**

作为[uapi_gpio_register_isr_func](#uapi_gpio_register_isr_func)接口的回调函数参数类型

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
    PIN_NONE = 25, // used as invalid/unused PIN number
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
| PIN_NONE | 25 | 无效/未使用的引脚编号 |

### gpio_direction_t <a id="enum_gpio_direction"></a>

```c
typedef enum gpio_direction {
    GPIO_DIRECTION_INPUT,
    GPIO_DIRECTION_OUTPUT
} gpio_direction_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GPIO_DIRECTION_INPUT | 0 | 输入方向 |
| GPIO_DIRECTION_OUTPUT | 1 | 输出方向 |

### gpio_level_t <a id="enum_gpio_level"></a>

```c
typedef enum gpio_level {
    GPIO_LEVEL_LOW,
    GPIO_LEVEL_HIGH
} gpio_level_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GPIO_LEVEL_LOW | 0 | 低电平 |
| GPIO_LEVEL_HIGH | 1 | 高电平 |

### cores_t <a id="enum_cores_t"></a>

```c
typedef enum {
    CORES_BT_CORE = 0,
    CORES_PROTOCOL_CORE = 1,
    CORES_APPS_CORE = 2,
    CORES_EXTERN0_CORE = 3,
    CORES_EXTERN1_CORE = 4,
    CORES_MAX_NUMBER_PHYSICAL,
    CORES_NONE = CORES_MAX_NUMBER_PHYSICAL,
    CORES_ASSET_CORE = CORES_MAX_NUMBER_PHYSICAL,
    CORES_UNKNOWN = CORES_MAX_NUMBER_PHYSICAL + 1,
} cores_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| CORES_BT_CORE | 0 | BT (Bluetooth) 核心 |
| CORES_PROTOCOL_CORE | 1 | 协议栈核心 |
| CORES_APPS_CORE | 2 | 应用核心 |
| CORES_EXTERN0_CORE | 3 | 扩展核心0 |
| CORES_EXTERN1_CORE | 4 | 扩展核心1 |
| CORES_MAX_NUMBER_PHYSICAL | 3 | 物理核心数量上限 |
| CORES_NONE | 3 | 无核心（等于CORES_MAX_NUMBER_PHYSICAL） |
| CORES_ASSET_CORE | 3 | 安全资产核心（等于CORES_MAX_NUMBER_PHYSICAL） |
| CORES_UNKNOWN | 4 | 未知核心（等于CORES_MAX_NUMBER_PHYSICAL + 1） |




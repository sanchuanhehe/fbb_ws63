# PWM

PWM (Pulse Width Modulation) 通过调节高低电平的时间比例（占空比）来控制平均输出电压，频率保持不变。WS63 PWM 支持 8 路通道，可独立配置周期和占空比，支持通道分组和中断回调。

运行时可通过 `uapi_pwm_update_duty_ratio()` 动态调整占空比而无需关闭通道。适用 LED (Light Emitting Diode) 亮度调节、舵机角度控制、蜂鸣器音调控制等场景。

**头文件清单**

```c
#include "include/driver/pwm.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_pwm_init](#uapi_pwm_init) | 初始化PWM模块 |
| [uapi_pwm_deinit](#uapi_pwm_deinit) | 去初始化PWM模块 |
| [uapi_pwm_open](#uapi_pwm_open) | 打开指定PWM通道并配置参数 |
| [uapi_pwm_close](#uapi_pwm_close) | 关闭指定PWM通道 |
| [uapi_pwm_start](#uapi_pwm_start) | 启动指定PWM通道输出 |
| [uapi_pwm_get_frequency](#uapi_pwm_get_frequency) | 获取PWM工作频率 |
| [uapi_pwm_stop](#uapi_pwm_stop) | 停止正在运行的PWM |
| [uapi_pwm_update_duty_ratio](#uapi_pwm_update_duty_ratio) | 更新已打开PWM的占空比 |
| [uapi_pwm_isr](#uapi_pwm_isr) | PWM中断服务例程 |
| [uapi_pwm_register_interrupt](#uapi_pwm_register_interrupt) | 注册PWM中断回调函数 |
| [uapi_pwm_unregister_interrupt](#uapi_pwm_unregister_interrupt) | 去注册PWM中断回调函数 |
| [uapi_pwm_set_group](#uapi_pwm_set_group) | 为PWM通道分组 |
| [uapi_pwm_clear_group](#uapi_pwm_clear_group) | 清理PWM通道分组 |
| [uapi_pwm_start_group](#uapi_pwm_start_group) | 启动指定分组的PWM |
| [uapi_pwm_stop_group](#uapi_pwm_stop_group) | 停止指定分组的PWM |
| [uapi_pwm_update_cfg](#uapi_pwm_update_cfg) | 更新指定PWM通道的配置 |
| [uapi_pwm_config_preload](#uapi_pwm_config_preload) | PWM预配置，上一配置完成后自动加载 |

## Functions

### uapi_pwm_init <a id="uapi_pwm_init"></a>

```c
errcode_t uapi_pwm_init(void)
```

**头文件清单**

```c
#include "include/driver/pwm.h"
```

**功能说明**

- 初始化PWM模块，使能PWM时钟，注册HAL (Hardware Abstraction Layer) 层函数接口
- 重复调用时直接返回成功，不会重复初始化
- 初始化完成后方可调用PWM模块其他接口

**前置条件**

- PWM硬件时钟已就绪
- HAL层驱动已注册

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 初始化成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [pwm_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/pwm/pwm_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PWM_USING_V150 | 特性宏 | 支持 PWM V150 功能 | y |
| CONFIG_PWM_USING_V151 | 特性宏 | 支持 PWM V151 功能 | y |

### uapi_pwm_deinit <a id="uapi_pwm_deinit"></a>

```c
void uapi_pwm_deinit(void)
```

**头文件清单**

```c
#include "include/driver/pwm.h"
```

**功能说明**

- 去初始化PWM模块，关闭所有已打开的PWM通道
- 注销HAL层函数接口，关闭PWM时钟
- 未初始化时调用直接返回

**前置条件**

- 无（未初始化状态调用直接返回）

**参考案例**

- [pwm_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/pwm/pwm_demo.c)


### uapi_pwm_open <a id="uapi_pwm_open"></a>

```c
errcode_t uapi_pwm_open(uint8_t channel, const pwm_config_t *cfg)
```

**头文件清单**

```c
#include "include/driver/pwm.h"
```

**功能说明**

- 打开指定PWM通道，设置低电平时间、高电平时间、重复周期、连续输出标志等配置参数
- 若指定通道已打开，先关闭再重新打开
- 配置参数中cycles超过32767时返回参数错误

**前置条件**

- 模块初始化状态：已通过 uapi_pwm_init() 初始化完成
- 参数合法性：cfg不为NULL，cfg->cycles ≤ 32767

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| channel | uint8_t | PWM通道编号 | 0 ~ CONFIG_PWM_CHANNEL_NUM-1，参考[pwm_channel_t](#enum_pwm_channel_t) |
| cfg | const [pwm_config_t](#struct_pwm_config) * | PWM配置信息 | 非NULL；cycles范围0~32767；V151模式下low_time+high_time≤65535且offset_time≤low_time |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 通道打开成功 |
| ERRCODE_PWM_INVALID_PARAMETER(0x80001082) | 参数无效 | 通道号越界、cycles超限、配置参数校验失败 |
| ERRCODE_PWM_NOT_INIT(0x80001080) | 模块未初始化 | PWM模块未初始化 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [pwm_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/pwm/pwm_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PWM_USING_V150 | 特性宏 | 支持 PWM V150 功能 | y |
| CONFIG_PWM_USING_V151 | 特性宏 | 支持 PWM V151 功能 | y |

### uapi_pwm_close <a id="uapi_pwm_close"></a>

```c
errcode_t uapi_pwm_close(uint8_t channel)
```

**头文件清单**

```c
#include "include/driver/pwm.h"
```

**功能说明**

- 关闭指定PWM通道，停止PWM输出并注销中断回调
- V150模式下直接停止通道，V151模式下通过分组停止

**前置条件**

- 模块初始化状态：已通过 uapi_pwm_init() 初始化完成
- 通道已打开：指定通道已通过 uapi_pwm_open() 打开

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| channel | uint8_t | PWM通道编号 | 0 ~ CONFIG_PWM_CHANNEL_NUM-1，参考[pwm_channel_t](#enum_pwm_channel_t) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 通道关闭成功 |
| ERRCODE_PWM_INVALID_PARAMETER(0x80001082) | 参数无效 | 通道号越界 |
| ERRCODE_PWM_NOT_INIT(0x80001080) | 模块未初始化 | PWM模块未初始化 |
| ERRCODE_PWM_NOT_OPEN(0x80001081) | 通道未打开 | 指定通道未打开 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [pwm_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/pwm/pwm_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PWM_USING_V150 | 特性宏 | 支持 PWM V150 功能 | y |
| CONFIG_PWM_USING_V151 | 特性宏 | 支持 PWM V151 功能 | y |

### uapi_pwm_start <a id="uapi_pwm_start"></a>

```c
errcode_t uapi_pwm_start(uint8_t channel)
```

**头文件清单**

```c
#include "include/driver/pwm.h"
```

**功能说明**

- 启动指定PWM通道输出
- V150模式下直接启动通道，V151模式下通过分组启动

**前置条件**

- 模块初始化状态：已通过 uapi_pwm_init() 初始化完成
- 通道已打开：指定通道已通过 uapi_pwm_open() 打开

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| channel | uint8_t | PWM通道编号 | 0 ~ CONFIG_PWM_CHANNEL_NUM-1，参考[pwm_channel_t](#enum_pwm_channel_t) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 启动成功 |
| ERRCODE_PWM_INVALID_PARAMETER(0x80001082) | 参数无效 | 通道号越界 |
| ERRCODE_PWM_NOT_INIT(0x80001080) | 模块未初始化 | PWM模块未初始化 |
| ERRCODE_PWM_NOT_OPEN(0x80001081) | 通道未打开 | 指定通道未打开 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [pwm_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/pwm/pwm_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PWM_USING_V150 | 特性宏 | 支持 PWM V150 功能 | y |
| CONFIG_PWM_USING_V151 | 特性宏 | 支持 PWM V151 功能 | y |

### uapi_pwm_get_frequency <a id="uapi_pwm_get_frequency"></a>

```c
uint32_t uapi_pwm_get_frequency(uint8_t channel)
```

**头文件清单**

```c
#include "include/driver/pwm.h"
```

**功能说明**

- 获取指定PWM通道的工作频率，单位为HZ
- 通道号越界时返回0
- 返回值可用于计算实际高低电平时间

**前置条件**

- 无

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| channel | uint8_t | PWM通道编号 | 0 ~ CONFIG_PWM_CHANNEL_NUM-1，参考[pwm_channel_t](#enum_pwm_channel_t) |

**返回值**

- 返回类型：uint32_t
- PWM工作频率，单位HZ；通道号越界时返回0

### uapi_pwm_stop <a id="uapi_pwm_stop"></a>

```c
errcode_t uapi_pwm_stop(uint8_t channel)
```

**头文件清单**

```c
#include "include/driver/pwm.h"
```

**功能说明**

- 停止正在运行的PWM通道输出
- 仅在CONFIG_PWM_USING_V150配置下可用

**前置条件**

- 模块初始化状态：已通过 uapi_pwm_init() 初始化完成
- 通道已打开：指定通道已通过 uapi_pwm_open() 打开

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| channel | uint8_t | PWM通道编号 | 0 ~ CONFIG_PWM_CHANNEL_NUM-1，参考[pwm_channel_t](#enum_pwm_channel_t) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 停止成功 |
| ERRCODE_PWM_INVALID_PARAMETER(0x80001082) | 参数无效 | 通道号越界 |
| ERRCODE_PWM_NOT_INIT(0x80001080) | 模块未初始化 | PWM模块未初始化 |
| ERRCODE_PWM_NOT_OPEN(0x80001081) | 通道未打开 | 指定通道未打开 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PWM_USING_V150 | 特性宏 | 支持 PWM V150 stop 功能 | y |

### uapi_pwm_update_duty_ratio <a id="uapi_pwm_update_duty_ratio"></a>

```c
errcode_t uapi_pwm_update_duty_ratio(uint8_t channel, uint32_t low_time, uint32_t high_time)
```

**头文件清单**

```c
#include "include/driver/pwm.h"
```

**功能说明**

- 更新已打开PWM通道的占空比，通过设置低电平和高电平时钟周期计数实现
- 更新后立即刷新生效
- 仅在CONFIG_PWM_USING_V150配置下可用

**前置条件**

- 模块初始化状态：已通过 uapi_pwm_init() 初始化完成
- 通道已打开：指定通道已通过 uapi_pwm_open() 打开

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| channel | uint8_t | PWM通道编号 | 0 ~ CONFIG_PWM_CHANNEL_NUM-1，参考[pwm_channel_t](#enum_pwm_channel_t) |
| low_time | uint32_t | PWM工作时钟周期计数个数低电平部分 | 实际低电平时间 = low_time × T_us |
| high_time | uint32_t | PWM工作时钟周期计数个数高电平部分 | 实际高电平时间 = high_time × T_us |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 更新成功 |
| ERRCODE_PWM_INVALID_PARAMETER(0x80001082) | 参数无效 | 通道号越界 |
| ERRCODE_PWM_NOT_INIT(0x80001080) | 模块未初始化 | PWM模块未初始化 |
| ERRCODE_PWM_NOT_OPEN(0x80001081) | 通道未打开 | 指定通道未打开 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PWM_USING_V150 | 特性宏 | 支持 PWM V150 duty ratio update 功能 | y |

### uapi_pwm_isr <a id="uapi_pwm_isr"></a>

```c
errcode_t uapi_pwm_isr(uint8_t channel)
```

**头文件清单**

```c
#include "include/driver/pwm.h"
```

**功能说明**

- PWM中断服务例程，清除指定通道的中断标志
- 通道号越界时返回参数错误

**前置条件**

- 通道号合法

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| channel | uint8_t | PWM通道编号 | 0 ~ CONFIG_PWM_CHANNEL_NUM-1，参考[pwm_channel_t](#enum_pwm_channel_t) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 中断清除成功 |
| ERRCODE_PWM_INVALID_PARAMETER(0x80001082) | 参数无效 | 通道号越界 |

### uapi_pwm_register_interrupt <a id="uapi_pwm_register_interrupt"></a>

```c
errcode_t uapi_pwm_register_interrupt(uint8_t channel, pwm_callback_t callback)
```

**头文件清单**

```c
#include "include/driver/pwm.h"
```

**功能说明**

- 为指定PWM通道注册中断回调函数
- 注册中断后，当PWM周期完成或配置完成时触发回调
- 注册前需先打开PWM通道

**前置条件**

- 模块初始化状态：已通过 uapi_pwm_init() 初始化完成
- 通道已打开：指定通道已通过 uapi_pwm_open() 打开

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| channel | uint8_t | PWM通道编号 | 0 ~ CONFIG_PWM_CHANNEL_NUM-1，参考[pwm_channel_t](#enum_pwm_channel_t) |
| callback | [pwm_callback_t](#typedef_pwm_callback_t) | 中断回调函数 | 非NULL |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 注册成功 |
| ERRCODE_PWM_INVALID_PARAMETER(0x80001082) | 参数无效 | 通道号越界 |
| ERRCODE_PWM_NOT_INIT(0x80001080) | 模块未初始化 | PWM模块未初始化 |
| ERRCODE_PWM_NOT_OPEN(0x80001081) | 通道未打开 | 指定通道未打开 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [pwm_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/pwm/pwm_demo.c)


### uapi_pwm_unregister_interrupt <a id="uapi_pwm_unregister_interrupt"></a>

```c
errcode_t uapi_pwm_unregister_interrupt(uint8_t channel)
```

**头文件清单**

```c
#include "include/driver/pwm.h"
```

**功能说明**

- 去注册指定PWM通道的中断回调函数
- 注销后该通道不再触发中断回调

**前置条件**

- 模块初始化状态：已通过 uapi_pwm_init() 初始化完成
- 通道已打开：指定通道已通过 uapi_pwm_open() 打开

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| channel | uint8_t | PWM通道编号 | 0 ~ CONFIG_PWM_CHANNEL_NUM-1，参考[pwm_channel_t](#enum_pwm_channel_t) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 去注册成功 |
| ERRCODE_PWM_INVALID_PARAMETER(0x80001082) | 参数无效 | 通道号越界 |
| ERRCODE_PWM_NOT_INIT(0x80001080) | 模块未初始化 | PWM模块未初始化 |
| ERRCODE_PWM_NOT_OPEN(0x80001081) | 通道未打开 | 指定通道未打开 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### uapi_pwm_set_group <a id="uapi_pwm_set_group"></a>

```c
errcode_t uapi_pwm_set_group(uint8_t group, const uint8_t *channel_set, uint32_t channel_set_len)
```

**头文件清单**

```c
#include "include/driver/pwm.h"
```

**功能说明**

- 将多个PWM通道归入同一分组，同组通道可同步启动和停止
- 同一通道不能同时属于不同分组
- 仅在CONFIG_PWM_USING_V151配置下可用

**前置条件**

- 模块初始化状态：已通过 uapi_pwm_init() 初始化完成
- 参数合法性：channel_set不为NULL，channel_set_len不为0，通道未被其他分组占用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| group | uint8_t | PWM组编号 | 0 ~ CONFIG_PWM_GROUP_NUM-1，参考[pwm_v151_group_t](#enum_pwm_v151_group_t) |
| channel_set | const uint8_t * | 进行分组设置的通道集合 | 非NULL，元素值为0~CONFIG_PWM_CHANNEL_NUM-1 |
| channel_set_len | uint32_t | 通道集合长度 | > 0 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 分组设置成功 |
| ERRCODE_PWM_INVALID_PARAMETER(0x80001082) | 参数无效 | 组号越界、channel_set为NULL、channel_set_len为0、通道已被占用 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [pwm_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/pwm/pwm_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PWM_USING_V151 | 特性宏 | 支持 PWM V151 分组功能 | y |

### uapi_pwm_clear_group <a id="uapi_pwm_clear_group"></a>

```c
errcode_t uapi_pwm_clear_group(uint8_t group)
```

**头文件清单**

```c
#include "include/driver/pwm.h"
```

**功能说明**

- 清理指定PWM分组，移除该分组下所有通道的关联
- 仅在CONFIG_PWM_USING_V151配置下可用

**前置条件**

- 组号合法

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| group | uint8_t | PWM组编号 | 0 ~ CONFIG_PWM_GROUP_NUM-1，参考[pwm_v151_group_t](#enum_pwm_v151_group_t) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 分组清理成功 |
| ERRCODE_PWM_INVALID_PARAMETER(0x80001082) | 参数无效 | 组号越界 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PWM_USING_V151 | 特性宏 | 支持 PWM V151 分组功能 | y |

### uapi_pwm_start_group <a id="uapi_pwm_start_group"></a>

```c
errcode_t uapi_pwm_start_group(uint8_t group)
```

**头文件清单**

```c
#include "include/driver/pwm.h"
```

**功能说明**

- 启动指定分组下所有PWM通道的输出
- 仅在CONFIG_PWM_USING_V151配置下可用

**前置条件**

- 组号合法
- 分组已通过 uapi_pwm_set_group() 设置

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| group | uint8_t | PWM组编号 | 0 ~ CONFIG_PWM_GROUP_NUM-1，参考[pwm_v151_group_t](#enum_pwm_v151_group_t) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 启动成功 |
| ERRCODE_PWM_INVALID_PARAMETER(0x80001082) | 参数无效 | 组号越界 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [pwm_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/pwm/pwm_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PWM_USING_V151 | 特性宏 | 支持 PWM V151 分组功能 | y |

### uapi_pwm_stop_group <a id="uapi_pwm_stop_group"></a>

```c
errcode_t uapi_pwm_stop_group(uint8_t group)
```

**头文件清单**

```c
#include "include/driver/pwm.h"
```

**功能说明**

- 停止指定分组下所有PWM通道的输出
- 仅在CONFIG_PWM_USING_V151配置下可用

**前置条件**

- 组号合法
- 分组已通过 uapi_pwm_set_group() 设置

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| group | uint8_t | PWM组编号 | 0 ~ CONFIG_PWM_GROUP_NUM-1，参考[pwm_v151_group_t](#enum_pwm_v151_group_t) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 停止成功 |
| ERRCODE_PWM_INVALID_PARAMETER(0x80001082) | 参数无效 | 组号越界 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [pwm_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/pwm/pwm_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PWM_USING_V151 | 特性宏 | 支持 PWM V151 分组功能 | y |

### uapi_pwm_update_cfg <a id="uapi_pwm_update_cfg"></a>

```c
errcode_t uapi_pwm_update_cfg(uint8_t channel, const pwm_config_t *cfg)
```

**头文件清单**

```c
#include "include/driver/pwm.h"
```

**功能说明**

- 更新指定PWM通道的完整配置参数，包括低电平时间、高电平时间、相位偏移和重复周期
- 更新后立即刷新生效
- 仅在CONFIG_PWM_USING_V151配置下可用

**前置条件**

- 模块初始化状态：已通过 uapi_pwm_init() 初始化完成
- 通道已打开：指定通道已通过 uapi_pwm_open() 打开
- 参数合法性：cfg不为NULL，cfg->cycles ≤ 32767

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| channel | uint8_t | PWM通道编号 | 0 ~ CONFIG_PWM_CHANNEL_NUM-1，参考[pwm_channel_t](#enum_pwm_channel_t) |
| cfg | const [pwm_config_t](#struct_pwm_config) * | PWM配置信息 | 非NULL；cycles范围0~32767；low_time+high_time≤65535且offset_time≤low_time |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 更新成功 |
| ERRCODE_PWM_INVALID_PARAMETER(0x80001082) | 参数无效 | 通道号越界、cycles超限、配置参数校验失败 |
| ERRCODE_PWM_NOT_INIT(0x80001080) | 模块未初始化 | PWM模块未初始化 |
| ERRCODE_PWM_NOT_OPEN(0x80001081) | 通道未打开 | 指定通道未打开 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [pwm_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/pwm/pwm_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PWM_USING_V151 | 特性宏 | 支持 PWM V151 配置更新功能 | y |

### uapi_pwm_config_preload <a id="uapi_pwm_config_preload"></a>

```c
errcode_t uapi_pwm_config_preload(uint8_t group, uint8_t channel, const pwm_config_t *cfg)
```

**头文件清单**

```c
#include "include/driver/pwm.h"
```

**功能说明**

- PWM预配置，当上一个PWM配置完成时，此配置会自动加载生效
- 可实现PWM输出参数的无缝切换
- 仅在CONFIG_PWM_USING_V151且CONFIG_PWM_PRELOAD配置下可用

**前置条件**

- 模块初始化状态：已通过 uapi_pwm_init() 初始化完成
- 参数合法性：cfg不为NULL，cfg->cycles ≤ 32767

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| group | uint8_t | PWM组编号 | 0 ~ CONFIG_PWM_GROUP_NUM-1，参考[pwm_v151_group_t](#enum_pwm_v151_group_t) |
| channel | uint8_t | PWM通道编号 | 0 ~ CONFIG_PWM_CHANNEL_NUM-1，参考[pwm_channel_t](#enum_pwm_channel_t) |
| cfg | const [pwm_config_t](#struct_pwm_config) * | PWM配置信息 | 非NULL；cycles范围0~32767 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 预配置成功 |
| ERRCODE_PWM_INVALID_PARAMETER(0x80001082) | 参数无效 | 通道号越界、组号越界、cfg为NULL、cycles超限 |
| ERRCODE_PWM_NOT_INIT(0x80001080) | 模块未初始化 | PWM模块未初始化 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [pwm_preload_demo.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/pwm/pwm_preload_demo.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PWM_USING_V151 | 特性宏 | 支持 PWM V151 功能 | y |
| CONFIG_PWM_PRELOAD | 特性宏 | 支持 PWM 预配置特性 | n |

## Type definitions

### errcode_t <a id="typedef_errcode_t"></a>

```c
typedef uint32_t errcode_t;
```

**使用说明**

PWM模块所有返回errcode_t的接口均使用此类型作为返回值

### pwm_callback_t <a id="typedef_pwm_callback_t"></a>

```c
typedef errcode_t (*pwm_callback_t)(uint8_t channel);
```

**使用说明**

用于 uapi_pwm_register_interrupt() 注册中断回调函数

## Enumerations

### pwm_channel_t <a id="enum_pwm_channel_t"></a>

```c
typedef enum {
    PWM_0,                  /* < PWM Peripheral 0. */
    PWM_1,                  /* < PWM Peripheral 1. */
    PWM_2,                  /* < PWM Peripheral 2. */
    PWM_3,                  /* < PWM Peripheral 3. */
    PWM_4,                  /* < PWM Peripheral 4. */
    PWM_5,                  /* < PWM Peripheral 5. */
    PWM_6,                  /* < PWM Peripheral 6. */
    PWM_7,                  /* < PWM Peripheral 7. */
    PWM_8,                  /* < PWM Peripheral 8. */
    PWM_9,                  /* < PWM Peripheral 9. */
    PWM_10,                 /* < PWM Peripheral 10. */
    PWM_11,                 /* < PWM Peripheral 11. */
    PWM_12,                 /* < PWM Peripheral 12. */
    PWM_13,                 /* < PWM Peripheral 13. */
    PWM_14,                 /* < PWM Peripheral 14. */
    PWM_15,                 /* < PWM Peripheral 15. */
    PWM_NONE = CONFIG_PWM_CHANNEL_NUM
} pwm_channel_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| PWM_0 | 0 | PWM外设0 |
| PWM_1 | 1 | PWM外设1 |
| PWM_2 | 2 | PWM外设2 |
| PWM_3 | 3 | PWM外设3 |
| PWM_4 | 4 | PWM外设4 |
| PWM_5 | 5 | PWM外设5 |
| PWM_6 | 6 | PWM外设6 |
| PWM_7 | 7 | PWM外设7 |
| PWM_8 | 8 | PWM外设8 |
| PWM_9 | 9 | PWM外设9 |
| PWM_10 | 10 | PWM外设10 |
| PWM_11 | 11 | PWM外设11 |
| PWM_12 | 12 | PWM外设12 |
| PWM_13 | 13 | PWM外设13 |
| PWM_14 | 14 | PWM外设14 |
| PWM_15 | 15 | PWM外设15 |
| PWM_NONE | CONFIG_PWM_CHANNEL_NUM | 无效通道 |

### pwm_v151_group_t <a id="enum_pwm_v151_group_t"></a>

```c
typedef enum {
    PWM_GROUP_0,
    PWM_GROUP_1,
    PWM_GROUP_2,
    PWM_GROUP_3,
    PWM_GROUP_4,
    PWM_GROUP_5,
    PWM_GROUP_6,
    PWM_GROUP_7,
    PWM_GROUP_8,
    PWM_GROUP_9,
    PWM_GROUP_10,
    PWM_GROUP_11,
    PWM_GROUP_12,
    PWM_GROUP_13,
    PWM_GROUP_14,
    PWM_GROUP_15
} pwm_v151_group_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| PWM_GROUP_0 | 0 | PWM组0 |
| PWM_GROUP_1 | 1 | PWM组1 |
| PWM_GROUP_2 | 2 | PWM组2 |
| PWM_GROUP_3 | 3 | PWM组3 |
| PWM_GROUP_4 | 4 | PWM组4 |
| PWM_GROUP_5 | 5 | PWM组5 |
| PWM_GROUP_6 | 6 | PWM组6 |
| PWM_GROUP_7 | 7 | PWM组7 |
| PWM_GROUP_8 | 8 | PWM组8 |
| PWM_GROUP_9 | 9 | PWM组9 |
| PWM_GROUP_10 | 10 | PWM组10 |
| PWM_GROUP_11 | 11 | PWM组11 |
| PWM_GROUP_12 | 12 | PWM组12 |
| PWM_GROUP_13 | 13 | PWM组13 |
| PWM_GROUP_14 | 14 | PWM组14 |
| PWM_GROUP_15 | 15 | PWM组15 |

## Structures

### pwm_config_t <a id="struct_pwm_config"></a>

```c
typedef struct pwm_config {
    uint32_t low_time;               /*!< PWM工作时钟周期计数个数低电平部分，
                                              频率参考 uapi_pwm_get_frequency()。
                                              如果PWM工作周期为Tus, 实际低电平时间 = low_time * Tus */
    uint32_t high_time;              /*!< PWM工作时钟周期计数个数高电平部分，
                                              频率参考 uapi_pwm_get_frequency()。
                                              如果PWM工作周期为Tus, 实际高电平时间 = high_time * Tus */
    uint32_t offset_time;            /*!< PWM相位。 */
    uint16_t cycles;                 /*!< PWM重复周期，范围：0~32767 (15bit)。 */
    bool repeat;                     /*!< 指示PWM应连续输出的标志。 */
} pwm_config_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| low_time | uint32_t | PWM工作时钟周期计数个数低电平部分，V151模式下low_time+high_time≤65535 |
| high_time | uint32_t | PWM工作时钟周期计数个数高电平部分，V151模式下low_time+high_time≤65535 |
| offset_time | uint32_t | PWM相位偏移，V151模式下offset_time≤low_time |
| cycles | uint16_t | PWM重复周期，范围：0~32767 (15bit) |
| repeat | bool | 指示PWM应连续输出的标志，true为连续输出，false为有限周期输出 |




# ADC

WS63 片内集成 SAR (Successive Approximation Register) ADC (Analog-to-Digital Converter)，分辨率 12-bit（0~4095），提供 6 路单端通道和差分通道。支持 4 档采样时钟（500K/250K/125K/15KHz），一次采样需 16 个时钟周期，采样速率 = adc_clk / 16。

支持手动和自动两种采样模式。手动模式下单次触发转换；自动模式下可配置通道扫描列表，硬件按序完成多通道连续采样。适用电池电压检测、传感器模拟量读取、电位器位置检测等场景。

> **WS63/WS63E 可用性说明**：WS63 采用 V154 ADC IP，仅支持 GADC（通用 ADC）模式与手动/自动采样。差分通道（`CONFIG_ADC_SUPPORT_DIFFERENTIAL`，依赖 V153）、AFE/HAFE 高精度模式（`CONFIG_ADC_SUPPORT_AFE`/`CONFIG_ADC_SUPPORT_HAFE`，依赖 V152/V153）在 WS63 上均不可用——相关接口（`uapi_adc_open_differential_channel`、`uapi_adc_close_differential_channel`、`uapi_adc_auto_sample` 等）在 WS63 构建中不会被编译，请勿调用。

**头文件清单**

```c
#include "include/driver/adc.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_adc_init](#uapi_adc_init) | 初始化ADC模块，配置采样时钟 |
| [uapi_adc_deinit](#uapi_adc_deinit) | 去初始化ADC模块，释放资源 |
| [uapi_adc_power_en](#uapi_adc_power_en) | ADC上下电控制 |
| [uapi_adc_is_using](#uapi_adc_is_using) | 查询ADC是否正在使用 |
| [uapi_adc_open_channel](#uapi_adc_open_channel) | 开启ADC单通道 |
| [uapi_adc_close_channel](#uapi_adc_close_channel) | 关闭ADC单通道 |
| [uapi_adc_open_differential_channel](#uapi_adc_open_differential_channel) | 开启ADC差分通道 |
| [uapi_adc_close_differential_channel](#uapi_adc_close_differential_channel) | 关闭ADC差分通道 |
| [uapi_adc_auto_scan_ch_enable](#uapi_adc_auto_scan_ch_enable) | 启用ADC自动扫描通道 |
| [uapi_adc_auto_scan_ch_disable](#uapi_adc_auto_scan_ch_disable) | 禁用ADC单通道自动扫描 |
| [uapi_adc_auto_scan_disable](#uapi_adc_auto_scan_disable) | 禁用ADC自动扫描总控制并关闭ADC电源 |
| [uapi_adc_auto_scan_is_enabled](#uapi_adc_auto_scan_is_enabled) | 查询ADC自动扫描是否使能 |
| [uapi_adc_manual_sample](#uapi_adc_manual_sample) | ADC手动采样 |
| [uapi_adc_auto_sample](#uapi_adc_auto_sample) | ADC自动采样 |

## Functions

### uapi_adc_init <a id="uapi_adc_init"></a>

```c
errcode_t uapi_adc_init(adc_clock_t clock)
```

**头文件清单**

```c
#include "include/driver/adc.h"
```

**功能说明**

- 初始化ADC模块，配置ADC采样时钟频率
- ADC源时钟为2MHz，一次采样需要16个时钟周期，采样速率 = adc_clk / 16
- 重复调用时若已初始化则直接返回成功

**前置条件**

- ADC模块未被初始化，或已通过uapi_adc_deinit()去初始化
- CONFIG_ADC_SUPPORT_AFE或相关宏已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| clock | [adc_clock_t](#enum_adc_clock) | ADC采样时钟配置 | ADC_CLOCK_500KHZ(0) / ADC_CLOCK_250KHZ(1) / ADC_CLOCK_125KHZ(2) / ADC_CLOCK_015KHZ(3) |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 初始化成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [adc_demo_inc.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/adc/adc_demo_inc.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_ADC | 特性宏 | 支持ADC接口功能 | n |
| CONFIG_ADC_SUPPORT_AFE | 特性宏 | 支持AFE模式特性 | y |
| CONFIG_ADC_SUPPORT_AUTO_SCAN | 特性宏 | 支持自动扫描注册中断特性 | y |

### uapi_adc_deinit <a id="uapi_adc_deinit"></a>

```c
errcode_t uapi_adc_deinit(void)
```

**头文件清单**

```c
#include "include/driver/adc.h"
```

**功能说明**

- 去初始化ADC模块，注销HAL (Hardware Abstraction Layer) 函数接口，关闭时钟
- 若自动扫描已启用，同时注销中断
- 重复调用时若未初始化则直接返回成功

**前置条件**

- ADC模块已通过uapi_adc_init()初始化完成

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 去初始化成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [adc_demo_inc.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/peripheral/adc/adc_demo_inc.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_ADC | 特性宏 | 支持ADC接口功能 | n |
| CONFIG_ADC_SUPPORT_AUTO_SCAN | 特性宏 | 支持自动扫描注销中断特性 | y |

### uapi_adc_power_en <a id="uapi_adc_power_en"></a>

```c
void uapi_adc_power_en(afe_scan_mode_t afe_scan_mode, bool en)
```

**头文件清单**

```c
#include "include/driver/adc.h"
```

**功能说明**

- 控制ADC上电或下电，并启用或关闭ADC
- 若当前上下电状态与目标状态一致，则直接返回不操作
- 支持AFE高精度模式时，中断注册与AFE模式关联

**前置条件**

- ADC模块已通过uapi_adc_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| afe_scan_mode | [afe_scan_mode_t](#enum_afe_scan_mode) | AFE扫描精度模式 | AFE_GADC_MODE(0) / CONFIG_ADC_SUPPORT_HAFE开启时: AFE_HADC_MODE(1) / AFE_SCAN_MODE_MAX_NUM |
| en | bool | 上电或下电标志 | true: 上电 / false: 下电 |

### uapi_adc_is_using <a id="uapi_adc_is_using"></a>

```c
bool uapi_adc_is_using(void)
```

**头文件清单**

```c
#include "include/driver/adc.h"
```

**功能说明**

- 查询ADC是否正在使用
- 支持AFE高精度模式时，GADC或HADC任一上电即返回true

**前置条件**

- ADC模块已通过uapi_adc_init()初始化完成

**返回值**

- 返回类型：bool

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| true | ADC使用中 | ADC已上电 |
| false | ADC未使用 | ADC未上电 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_ADC_SUPPORT_AFE | 特性宏 | 支持AFE高精度模式判断特性 | y |
| CONFIG_ADC_SUPPORT_HAFE | 特性宏 | 支持HAFE高精度模式判断特性 | y |

### uapi_adc_open_channel <a id="uapi_adc_open_channel"></a>

```c
errcode_t uapi_adc_open_channel(uint8_t channel)
```

**头文件清单**

```c
#include "include/driver/adc.h"
```

**功能说明**

- 开启指定的ADC单通道
- 自动扫描启用期间不允许开启通道
- 开启成功后记录当前工作通道

**前置条件**

- ADC模块已通过uapi_adc_init()初始化完成
- ADC已通过uapi_adc_power_en()上电
- 自动扫描未启用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| channel | uint8_t | ADC通道号 | 0 ~ ADC_CHANNEL_MAX_NUM-1 (WS63最大为5) |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 通道开启成功 |
| ERRCODE_ADC_INVALID_PARAMETER(0x80001141) | 参数无效 | 通道号超出范围 |
| ERRCODE_ADC_SCAN_NOT_DISABLE(0x80001142) | 自动扫描未禁用 | 自动扫描启用中尝试开启通道 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_ADC | 特性宏 | 支持ADC接口功能 | n |
| CONFIG_ADC_SUPPORT_AUTO_SCAN | 特性宏 | 支持自动扫描状态检查特性 | y |
| CONFIG_ADC_SUPPORT_AFE | 特性宏 | 支持AFE通道设置特性 | y |
| CONFIG_ADC_SUPPORT_DIFFERENTIAL | 特性宏 | 支持差分通道记录特性 | y |

### uapi_adc_close_channel <a id="uapi_adc_close_channel"></a>

```c
errcode_t uapi_adc_close_channel(uint8_t channel)
```

**头文件清单**

```c
#include "include/driver/adc.h"
```

**功能说明**

- 关闭指定的ADC单通道
- 仅允许关闭当前已开启的工作通道
- 自动扫描启用期间不允许关闭通道

**前置条件**

- ADC模块已通过uapi_adc_init()初始化完成
- 指定通道已通过uapi_adc_open_channel()开启
- 自动扫描未启用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| channel | uint8_t | ADC通道号 | 必须与当前已开启的工作通道一致 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 通道关闭成功 |
| ERRCODE_ADC_INVALID_PARAMETER(0x80001141) | 参数无效 | 通道号与当前工作通道不匹配 |
| ERRCODE_ADC_SCAN_NOT_DISABLE(0x80001142) | 自动扫描未禁用 | 自动扫描启用中尝试关闭通道 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_ADC | 特性宏 | 支持ADC接口功能 | n |
| CONFIG_ADC_SUPPORT_DIFFERENTIAL | 特性宏 | 支持差分通道匹配特性 | y |
| CONFIG_ADC_SUPPORT_AUTO_SCAN | 特性宏 | 支持自动扫描状态检查特性 | y |

### uapi_adc_open_differential_channel <a id="uapi_adc_open_differential_channel"></a>

```c
errcode_t uapi_adc_open_differential_channel(uint8_t positive_ch, uint8_t negative_ch)
```

**头文件清单**

```c
#include "include/driver/adc.h"
```

**功能说明**

- 开启ADC差分通道，配置正极通道和负极通道
- 开启成功后记录当前差分工作通道对

**前置条件**

- ADC模块已通过uapi_adc_init()初始化完成
- ADC已通过uapi_adc_power_en()上电
- CONFIG_ADC_SUPPORT_DIFFERENTIAL宏已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| positive_ch | uint8_t | ADC正极通道 | 0 ~ ADC_CHANNEL_MAX_NUM-1 (WS63最大为5) |
| negative_ch | uint8_t | ADC负极通道 | 0 ~ ADC_CHANNEL_MAX_NUM-1 (WS63最大为5) |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 差分通道开启成功 |
| ERRCODE_ADC_INVALID_PARAMETER(0x80001141) | 参数无效 | 通道号超出范围 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_ADC_SUPPORT_DIFFERENTIAL | 特性宏 | 支持ADC差分采样特性 | y |

### uapi_adc_close_differential_channel <a id="uapi_adc_close_differential_channel"></a>

```c
errcode_t uapi_adc_close_differential_channel(uint8_t positive_ch, uint8_t negative_ch)
```

**头文件清单**

```c
#include "include/driver/adc.h"
```

**功能说明**

- 关闭ADC差分通道
- 仅允许关闭当前已开启的差分工作通道对
- 自动扫描启用期间不允许关闭差分通道

**前置条件**

- 差分通道已通过uapi_adc_open_differential_channel()开启
- 自动扫描未启用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| positive_ch | uint8_t | ADC正极通道 | 必须与当前已开启的正极工作通道一致 |
| negative_ch | uint8_t | ADC负极通道 | 必须与当前已开启的负极工作通道一致 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 差分通道关闭成功 |
| ERRCODE_ADC_INVALID_PARAMETER(0x80001141) | 参数无效 | 通道号与当前差分工作通道不匹配 |
| ERRCODE_ADC_SCAN_NOT_DISABLE(0x80001142) | 自动扫描未禁用 | 自动扫描启用中尝试关闭差分通道 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_ADC_SUPPORT_DIFFERENTIAL | 特性宏 | 支持ADC差分采样特性 | y |
| CONFIG_ADC_SUPPORT_AUTO_SCAN | 特性宏 | 支持自动扫描状态检查特性 | y |

### uapi_adc_auto_scan_ch_enable <a id="uapi_adc_auto_scan_ch_enable"></a>

```c
errcode_t uapi_adc_auto_scan_ch_enable(uint8_t channel, adc_scan_config_t config, adc_callback_t callback)
```

**头文件清单**

```c
#include "include/driver/adc.h"
```

**功能说明**

- 启用指定通道的ADC自动扫描，配置扫描模式（FIFO (First-In First-Out) 全扫描或阈值扫描）及回调函数
- 启用前需确保ADC已上电
- 配置失败时自动恢复时钟为ADC_CLOCK_015KHZ

**前置条件**

- ADC模块已通过uapi_adc_init()初始化完成
- ADC已通过uapi_adc_power_en()上电
- callback不为NULL
- CONFIG_ADC_SUPPORT_AUTO_SCAN宏已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| channel | uint8_t | ADC通道号 | 0 ~ ADC_CHANNEL_MAX_NUM-1 (WS63最大为5) |
| config | [adc_scan_config_t](#struct_adc_scan_config) | 自动扫描配置 | type: 0(FIFO全扫描) / 1(阈值扫描); freq: 0~7; threshold_l/threshold_h: 电压范围 |
| callback | [adc_callback_t](#typedef_adc_callback_t) | 自动扫描中断回调函数 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 自动扫描启用成功 |
| ERRCODE_ADC_INVALID_PARAMETER(0x80001141) | 参数无效 | 通道号超出范围、callback为NULL、config参数非法 |
| ERRCODE_ADC_NOT_POWER_ON(0x80001146) | ADC未上电 | ADC未通过uapi_adc_power_en()上电 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_ADC_SUPPORT_AUTO_SCAN | 特性宏 | 支持ADC自动扫描功能 | y |
| CONFIG_ADC_SUPPORT_LONG_SAMPLE | 特性宏 | 支持长采样上报周期特性 | n |

### uapi_adc_auto_scan_ch_disable <a id="uapi_adc_auto_scan_ch_disable"></a>

```c
errcode_t uapi_adc_auto_scan_ch_disable(uint8_t channel)
```

**头文件清单**

```c
#include "include/driver/adc.h"
```

**功能说明**

- 禁用指定通道的ADC自动扫描
- 仅禁用单通道，不影响自动扫描总控制状态

**前置条件**

- ADC模块已通过uapi_adc_init()初始化完成
- CONFIG_ADC_SUPPORT_AUTO_SCAN宏已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| channel | uint8_t | ADC通道号 | 0 ~ ADC_CHANNEL_MAX_NUM-1 (WS63最大为5) |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 通道自动扫描禁用成功 |
| ERRCODE_ADC_INVALID_PARAMETER(0x80001141) | 参数无效 | 通道号超出范围 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_ADC_SUPPORT_AUTO_SCAN | 特性宏 | 支持ADC自动扫描功能 | y |

### uapi_adc_auto_scan_disable <a id="uapi_adc_auto_scan_disable"></a>

```c
void uapi_adc_auto_scan_disable(void)
```

**头文件清单**

```c
#include "include/driver/adc.h"
```

**功能说明**

- 禁用ADC自动扫描总控制，禁用所有扫描通道，并关闭ADC电源
- 支持AFE高精度模式时，关闭当前工作AFE模式的电源

**前置条件**

- ADC模块已通过uapi_adc_init()初始化完成
- CONFIG_ADC_SUPPORT_AUTO_SCAN宏已开启

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_ADC_SUPPORT_AUTO_SCAN | 特性宏 | 支持ADC自动扫描功能 | y |
| CONFIG_ADC_SUPPORT_AFE | 特性宏 | 支持AFE高精度模式电源关闭特性 | y |
| CONFIG_ADC_SUPPORT_HAFE | 特性宏 | 支持HAFE模式电源关闭特性 | y |

### uapi_adc_auto_scan_is_enabled <a id="uapi_adc_auto_scan_is_enabled"></a>

```c
bool uapi_adc_auto_scan_is_enabled(void)
```

**头文件清单**

```c
#include "include/driver/adc.h"
```

**功能说明**

- 查询ADC自动扫描总控制是否使能
- 返回HAL层自动扫描使能状态

**前置条件**

- CONFIG_ADC_SUPPORT_AUTO_SCAN宏已开启

**返回值**

- 返回类型：bool

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| true | 自动扫描已使能 | 自动扫描总控制已开启 |
| false | 自动扫描未使能 | 自动扫描总控制已关闭 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_ADC_SUPPORT_AUTO_SCAN | 特性宏 | 支持ADC自动扫描功能 | y |

### uapi_adc_manual_sample <a id="uapi_adc_manual_sample"></a>

```c
int32_t uapi_adc_manual_sample(uint8_t channel)
```

**头文件清单**

```c
#include "include/driver/adc.h"
```

**功能说明**

- 触发ADC手动采样，获取指定通道的采样值
- 通道号超出范围时返回0

**前置条件**

- ADC模块已通过uapi_adc_init()初始化完成
- ADC已通过uapi_adc_power_en()上电
- 指定通道已通过uapi_adc_open_channel()开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| channel | uint8_t | ADC通道号 | 0 ~ ADC_CHANNEL_MAX_NUM-1 (WS63最大为5) |

**返回值**

- 返回类型：int32_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 采样失败 | 通道号超出范围 |
| 非0 | ADC采样值 | 采样成功，返回采样值 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_DRIVER_SUPPORT_ADC | 特性宏 | 支持ADC接口功能 | n |

### uapi_adc_auto_sample <a id="uapi_adc_auto_sample"></a>

```c
int32_t uapi_adc_auto_sample(uint8_t channel)
```

**头文件清单**

```c
#include "include/driver/adc.h"
```

**功能说明**

- 触发ADC自动采样，依据前序配置的参数自动采样
- 通道号超出范围时返回0

**前置条件**

- ADC模块已通过uapi_adc_init()初始化完成
- ADC已通过uapi_adc_power_en()上电
- CONFIG_ADC_SUPPORT_AFE宏已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| channel | uint8_t | ADC通道号 | 0 ~ ADC_CHANNEL_MAX_NUM-1 (WS63最大为5) |

**返回值**

- 返回类型：int32_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 采样失败 | 通道号超出范围 |
| 非0 | ADC采样值 | 采样成功，返回采样值 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_ADC_SUPPORT_AFE | 特性宏 | 支持ADC自动采样功能 | y |

## Type definitions

### adc_callback_t <a id="typedef_adc_callback_t"></a>

```c
typedef void (*adc_callback_t)(uint8_t channel, uint32_t *buffer, uint32_t length, bool *next);
```

**使用说明**

在[uapi_adc_auto_scan_ch_enable](#uapi_adc_auto_scan_ch_enable)接口中作为入参，用于自动扫描中断回调。

## Enumerations

### adc_clock_t <a id="enum_adc_clock"></a>

```c
typedef enum adc_clock {
    ADC_CLOCK_500KHZ = 0,               /*!< ADC时钟频率：500KHZ。 */
    ADC_CLOCK_250KHZ = 1,               /*!< ADC时钟频率：250KHZ。 */
    ADC_CLOCK_125KHZ = 2,               /*!< ADC时钟频率：125KHZ。 */
    ADC_CLOCK_015KHZ = 3,               /*!< ADC时钟频率：015KHZ。 */
    ADC_CLOCK_MAX,
    ADC_CLOCK_NONE = ADC_CLOCK_MAX
} adc_clock_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| ADC_CLOCK_500KHZ | 0 | ADC时钟频率：500KHZ |
| ADC_CLOCK_250KHZ | 1 | ADC时钟频率：250KHZ |
| ADC_CLOCK_125KHZ | 2 | ADC时钟频率：125KHZ |
| ADC_CLOCK_015KHZ | 3 | ADC时钟频率：15KHZ |
| ADC_CLOCK_MAX | 4 | 时钟枚举上限 |
| ADC_CLOCK_NONE | 4 | 无时钟配置 |

### afe_scan_mode_t <a id="enum_afe_scan_mode"></a>

```c
typedef enum afe_scan_mode {
    AFE_GADC_MODE = 0,                  /*!< 模拟前端ADC常规精度模式。 */
#if defined (CONFIG_ADC_SUPPORT_HAFE)
    AFE_HADC_MODE,                      /*!< 模拟前端ADC高精度模式。 */
#elif (defined CONFIG_ADC_SUPPORT_AMIC)
    AFE_AMIC_MODE,                      /*!< 模拟前端ADC麦克风模式。 */
    AFE_BIO_MODE,                      /*!< 模拟前端ADC生物测量模式。 */
#endif
    AFE_SCAN_MODE_MAX_NUM
} afe_scan_mode_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| AFE_GADC_MODE | 0 | 常规精度模式 |
| AFE_HADC_MODE | 1 | 高精度模式（CONFIG_ADC_SUPPORT_HAFE开启时可用） |
| AFE_AMIC_MODE | 1 | 麦克风模式（CONFIG_ADC_SUPPORT_AMIC开启时可用） |
| AFE_BIO_MODE | 2 | 生物测量模式（CONFIG_ADC_SUPPORT_AMIC开启时可用） |
| AFE_SCAN_MODE_MAX_NUM | 2/3 | 扫描模式枚举上限 |

## Structures

### adc_scan_config_t <a id="struct_adc_scan_config"></a>

```c
typedef struct adc_scan_config {
    uint8_t type;                       /*!< FIFO全扫描或阈值扫描。 */
    float threshold_l;                  /*!< 阈值扫描电压（v）下限。 */
    float threshold_h;                  /*!< 阈值扫描电压（v）上限。 */
    uint8_t freq;                       /*!< ADC扫描频率，用于所有频道。 */
#if defined(CONFIG_ADC_SUPPORT_LONG_SAMPLE)
    uint32_t long_sample_time;           /*!< ADC长采样上报周期（单位：毫秒）。 */
#endif
} adc_scan_config_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| type | uint8_t | 扫描类型：0-FIFO全扫描，1-阈值扫描 |
| threshold_l | float | 阈值扫描电压下限（单位：V） |
| threshold_h | float | 阈值扫描电压上限（单位：V） |
| freq | uint8_t | ADC扫描频率，取值范围0~7，对应2Hz~256Hz |
| long_sample_time | uint32_t | ADC长采样上报周期（单位：ms），CONFIG_ADC_SUPPORT_LONG_SAMPLE开启时可用 |



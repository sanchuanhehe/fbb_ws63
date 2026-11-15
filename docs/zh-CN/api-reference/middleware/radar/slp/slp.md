# Radar SLP

Radar SLP (SparkLink Positioning) 提供基于 SLP 雷达的感知检测能力，支持配置雷达硬件参数与算法参数（如射频开关、AGC (Automatic Gain Control) 、波形、天线通道等），并允许注册检测结果、原始数据及芯片上下电状态的回调函数以获取目标检测输出。本模块涵盖参数下发、结果查询与回调注册等雷达检测全流程接口。

**头文件清单**
```c
#include "include/middleware/services/radar/radar_service_slp.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_radar_set_hardware_para](#uapi_radar_set_hardware_para) | 设置雷达硬件参数 |
| [uapi_radar_set_alg_para](#uapi_radar_set_alg_para) | 设置雷达算法参数 |
| [uapi_radar_register_result_cb](#uapi_radar_register_result_cb) | 注册雷达检测结果回调函数 |
| [uapi_radar_register_raw_data_cb](#uapi_radar_register_raw_data_cb) | 注册雷达原始数据回调函数 |
| [uapi_radar_get_result](#uapi_radar_get_result) | 获取雷达检测结果 |
| [uapi_radar_register_set_power_status_cb](#uapi_radar_register_set_power_status_cb) | 注册芯片上下电状态回调函数 |
| [uapi_radar_register_report_errcode_cb](#uapi_radar_register_report_errcode_cb) | 注册雷达错误码上报回调函数 |
| [uapi_radar_set_status](#uapi_radar_set_status) | 设置雷达启停状态 |
| [uapi_radar_set_power_status](#uapi_radar_set_power_status) | 控制芯片上下电 |
| [uapi_radar_get_status](#uapi_radar_get_status) | 获取雷达状态 |

## Functions

### uapi_radar_set_hardware_para <a id="uapi_radar_set_hardware_para"></a>

```c
errcode_radar_client_t uapi_radar_set_hardware_para(radar_hardware_para_t *para)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_slp.h"
```

**功能说明**

- 设置雷达硬件参数，包括射频参数、帧参数、原始数据上报配置参数、波形参数及厂商信息
- 调用前需确保芯片已上电，否则参数配置不生效
- 重复调用将覆盖上次配置的硬件参数

**前置条件**

- 芯片已通过 uapi_radar_set_power_status(CHIP_STATUS_POWERON) 上电完成
- 雷达处于未启动状态（RADAR_STATUS_STOP）

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| para | [radar_hardware_para_t](#struct_radar_hardware_para_t) * | 雷达硬件参数指针 | 非NULL，指向已初始化的参数结构体 |

**返回值**

- 返回类型：errcode_radar_client_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_RC_SUCCESS(0) | 执行成功 | 参数设置成功 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 参数校验失败或内部异常 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_PLT_SLP | 特性宏 | 支持SLP雷达接口功能 | n |

### uapi_radar_set_alg_para <a id="uapi_radar_set_alg_para"></a>

```c
errcode_radar_client_t uapi_radar_set_alg_para(radar_alg_para_t *para, uint8_t is_write_to_nv)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_slp.h"
```

**功能说明**

- 设置雷达算法参数，包括基础参数（上报类型、灵敏度、距离检测边界、退出时延、端子线朝向、静目标创建距离门限、天线间距缩放系数、角度检测边界、抗多径虚警模式、校准参数存储模式）、灵敏度参数和AI (Artificial Intelligence) 补偿参数
- 可通过is_write_to_nv控制是否将输入参数写入NV (Non-Volatile Memory) 保存
- 调用前需确保芯片已上电，否则参数配置不生效
- 重复调用将覆盖上次配置的算法参数

**前置条件**

- 芯片已通过 uapi_radar_set_power_status(CHIP_STATUS_POWERON) 上电完成
- 雷达处于未启动状态（RADAR_STATUS_STOP）

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| para | [radar_alg_para_t](#struct_radar_alg_para_t) * | 雷达算法参数指针 | 非NULL，指向已初始化的参数结构体 |
| is_write_to_nv | uint8_t | 是否将参数写入NV | 0表示不写入，非0表示写入 |

**返回值**

- 返回类型：errcode_radar_client_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_RC_SUCCESS(0) | 执行成功 | 参数设置成功 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 参数校验失败或内部异常 |

**参考案例**

- [radar_slp_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/slp_sample/radar_slp_sample.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_PLT_SLP | 特性宏 | 支持SLP雷达接口功能 | n |

### uapi_radar_register_result_cb <a id="uapi_radar_register_result_cb"></a>

```c
errcode_radar_client_t uapi_radar_register_result_cb(radar_result_cb_t cb)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_slp.h"
```

**功能说明**

- 注册雷达检测结果回调函数，当雷达检测完成时触发回调返回检测结果
- 回调函数内不能阻塞或长时间等待，不能使用较大栈空间
- 重复注册将覆盖上次注册的回调函数

**前置条件**

- 回调函数指针非NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| cb | [radar_result_cb_t](#typedef_radar_result_cb_t) | 雷达检测结果回调函数 | 非NULL函数指针 |

**返回值**

- 返回类型：errcode_radar_client_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_RC_SUCCESS(0) | 执行成功 | 回调注册成功 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 回调注册异常 |

**参考案例**

- [radar_slp_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/slp_sample/radar_slp_sample.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_PLT_SLP | 特性宏 | 支持SLP雷达接口功能 | n |

### uapi_radar_register_raw_data_cb <a id="uapi_radar_register_raw_data_cb"></a>

```c
errcode_radar_client_t uapi_radar_register_raw_data_cb(radar_raw_data_cb_t cb)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_slp.h"
```

**功能说明**

- 注册雷达原始数据回调函数，获取雷达采集的原始数据
- 回调函数内不能阻塞或长时间等待，不能使用较大栈空间
- data数据与结构体内存不连续，不能直接memcpy整个结构体

**前置条件**

- 回调函数指针非NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| cb | [radar_raw_data_cb_t](#typedef_radar_raw_data_cb_t) | 雷达原始数据回调函数 | 非NULL函数指针 |

**返回值**

- 返回类型：errcode_radar_client_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_RC_SUCCESS(0) | 执行成功 | 回调注册成功 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 回调注册异常 |

**参考案例**

- [radar_slp_tcp_socket.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/slp_wireless_sample/radar_slp_tcp_socket.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_PLT_SLP | 特性宏 | 支持SLP雷达接口功能 | n |

### uapi_radar_get_result <a id="uapi_radar_get_result"></a>

```c
errcode_radar_client_t uapi_radar_get_result(radar_result_t *result)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_slp.h"
```

**功能说明**

- 获取雷达检测结果，包含有效目标个数及各目标的距离、角度、速度信息
- 主动查询接口，可在任意时刻调用获取最近一次检测结果
- 雷达未启动时调用返回无效数据

**前置条件**

- 雷达已通过 uapi_radar_set_status(RADAR_STATUS_START) 启动

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| result | [radar_result_t](#struct_radar_result_t) * | 雷达检测结果输出指针 | 非NULL，指向已分配内存的结构体 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| result | [radar_result_t](#struct_radar_result_t) * | 雷达检测结果，包含有效目标个数及目标信息数组 |

**返回值**

- 返回类型：errcode_radar_client_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_RC_SUCCESS(0) | 执行成功 | 获取结果成功 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 获取结果失败 |

**参考案例**

- [radar_slp_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/slp_sample/radar_slp_sample.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_PLT_SLP | 特性宏 | 支持SLP雷达接口功能 | n |

### uapi_radar_register_set_power_status_cb <a id="uapi_radar_register_set_power_status_cb"></a>

```c
errcode_radar_client_t uapi_radar_register_set_power_status_cb(radar_set_power_status_cb_t cb)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_slp.h"
```

**功能说明**

- 注册芯片上下电状态回调函数，当芯片上下电流程完成时触发回调
- 回调函数运行于radar client线程，不能阻塞或长时间等待，不能使用较大栈空间
- 重复注册将覆盖上次注册的回调函数

**前置条件**

- 回调函数指针非NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| cb | [radar_set_power_status_cb_t](#typedef_radar_set_power_status_cb_t) | 芯片上下电状态回调函数 | 非NULL函数指针 |

**返回值**

- 返回类型：errcode_radar_client_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_RC_SUCCESS(0) | 执行成功 | 回调注册成功 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 回调注册异常 |

**参考案例**

- [radar_slp_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/slp_sample/radar_slp_sample.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_PLT_SLP | 特性宏 | 支持SLP雷达接口功能 | n |

### uapi_radar_register_report_errcode_cb <a id="uapi_radar_register_report_errcode_cb"></a>

```c
errcode_radar_client_t uapi_radar_register_report_errcode_cb(radar_report_errcode_cb_t cb)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_slp.h"
```

**功能说明**

- 注册雷达错误码上报回调函数，当雷达运行过程中发生错误时触发回调
- 回调函数内不能阻塞或长时间等待，不能使用较大栈空间
- 重复注册将覆盖上次注册的回调函数

**前置条件**

- 回调函数指针非NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| cb | [radar_report_errcode_cb_t](#typedef_radar_report_errcode_cb_t) | 雷达错误码上报回调函数 | 非NULL函数指针 |

**返回值**

- 返回类型：errcode_radar_client_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_RC_SUCCESS(0) | 执行成功 | 回调注册成功 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 回调注册异常 |

**参考案例**

- [radar_slp_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/slp_sample/radar_slp_sample.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_PLT_SLP | 特性宏 | 支持SLP雷达接口功能 | n |

### uapi_radar_set_status <a id="uapi_radar_set_status"></a>

```c
errcode_radar_client_t uapi_radar_set_status(uint8_t status)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_slp.h"
```

**功能说明**

- 设置雷达启停状态，启动或停止雷达检测功能
- 启动前需完成芯片上电和参数配置
- 停止雷达时先调用本接口停止，再下电芯片

**前置条件**

- 芯片已通过 uapi_radar_set_power_status(CHIP_STATUS_POWERON) 上电完成
- 启动前需完成硬件参数和算法参数配置

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| status | uint8_t | 雷达状态 | [radar_set_status_t](#enum_radar_set_status_t): RADAR_STATUS_STOP(0), RADAR_STATUS_START(1) |

**返回值**

- 返回类型：errcode_radar_client_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_RC_SUCCESS(0) | 执行成功 | 状态设置成功 |
| ERRCODE_RC_RADAR_ENABLED(0x8000A105) | 雷达已经使能 | 重复启动雷达 |
| ERRCODE_RC_RADAR_DISABLED(0x8000A106) | 雷达已经关闭 | 重复停止雷达 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 启动参数异常或内部异常 |

**参考案例**

- [radar_slp_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/slp_sample/radar_slp_sample.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_PLT_SLP | 特性宏 | 支持SLP雷达接口功能 | n |

### uapi_radar_set_power_status <a id="uapi_radar_set_power_status"></a>

```c
errcode_radar_client_t uapi_radar_set_power_status(uint8_t status)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_slp.h"
```

**功能说明**

- 控制雷达芯片上下电，上电后需等待约1.5s完成版本加载
- 上电后需先配置参数再启动雷达
- 下电前需先停止雷达

**前置条件**

- 下电前需先通过 uapi_radar_set_status(RADAR_STATUS_STOP) 停止雷达

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| status | uint8_t | 芯片上下电状态 | [radar_set_power_status_t](#enum_radar_set_power_status_t): CHIP_STATUS_POWEROFF(0), CHIP_STATUS_POWERON(1) |

**返回值**

- 返回类型：errcode_radar_client_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_RC_SUCCESS(0) | 执行成功 | 上下电操作成功 |
| ERRCODE_RC_POWERED_ON(0x8000A103) | 已经上电 | 重复上电 |
| ERRCODE_RC_POWERED_OFF(0x8000A104) | 已经下电 | 重复下电 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 上电加载失败或内部异常 |

**参考案例**

- [radar_slp_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/slp_sample/radar_slp_sample.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_PLT_SLP | 特性宏 | 支持SLP雷达接口功能 | n |

### uapi_radar_get_status <a id="uapi_radar_get_status"></a>

```c
errcode_radar_client_t uapi_radar_get_status(uint8_t *status)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_slp.h"
```

**功能说明**

- 获取雷达当前使能状态
- 主动查询接口，可在任意时刻调用
- 雷达未初始化时调用返回无效状态

**前置条件**

- status指针非NULL，指向已分配内存的空间

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| status | uint8_t * | 雷达状态输出指针 | 非NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| status | uint8_t * | 雷达状态，参考[radar_get_status_t](#enum_radar_get_status_t): RADAR_STATUS_RADAR_ON(0), RADAR_STATUS_RADAR_OFF(1) |

**返回值**

- 返回类型：errcode_radar_client_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_RC_SUCCESS(0) | 执行成功 | 获取状态成功 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 获取状态失败 |

**参考案例**

- [radar_softap_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/softap_sample/radar_softap_sample.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_PLT_SLP | 特性宏 | 支持SLP雷达接口功能 | n |

## Type definitions

### radar_result_cb_t <a id="typedef_radar_result_cb_t"></a>

```c
typedef void (*radar_result_cb_t)(const radar_result_t *result);
```

**使用说明**

用于 uapi_radar_register_result_cb 入参，雷达检测完成时回调

### radar_set_power_status_cb_t <a id="typedef_radar_set_power_status_cb_t"></a>

```c
typedef void (*radar_set_power_status_cb_t)(uint8_t status, errcode_radar_client_t err_code);
```

**使用说明**

用于 uapi_radar_register_set_power_status_cb 入参，芯片上下电流程完成时回调

### radar_report_errcode_cb_t <a id="typedef_radar_report_errcode_cb_t"></a>

```c
typedef void (*radar_report_errcode_cb_t)(errcode_radar_client_t err_code);
```

**使用说明**

用于 uapi_radar_register_report_errcode_cb 入参，雷达运行错误时回调

### radar_raw_data_cb_t <a id="typedef_radar_raw_data_cb_t"></a>

```c
typedef void (*radar_raw_data_cb_t)(radar_raw_data_msg_t *dataMsg);
```

**使用说明**

用于 uapi_radar_register_raw_data_cb 入参，获取雷达原始数据时回调

## Associations

### radar_rf_sw_bit <a id="union_radar_rf_sw_bit"></a>

```c
typedef union {
    struct {
        uint8_t ctrl0 : 1;   /*!< 开启或关闭天线控制管脚 0 的对应功能，0:关闭, 1:开启 */
        uint8_t ctrl1 : 1;   /*!< 开启或关闭天线控制管脚 1 的对应功能，0:关闭, 1:开启 */
        uint8_t ctrl2 : 1;   /*!< 开启或关闭天线控制管脚 2 的对应功能，0:关闭, 1:开启 */
        uint8_t ctrl3 : 1;   /*!< 开启或关闭天线控制管脚 3 的对应功能，0:关闭, 1:开启 */
        uint8_t ctrl4 : 1;   /*!< 开启或关闭天线控制管脚 4 的对应功能，0:关闭, 1:开启 */
        uint8_t ctrl5 : 1;   /*!< 开启或关闭天线控制管脚 5 的对应功能，0:关闭, 1:开启 */
        uint8_t reserve : 2; /*!< 保留比特位 */
    } bits;                  /*!< 位域结构体 */
    uint8_t u8;              /*!< 8位无符号整数 */
} radar_rf_sw_bit;
```

| 成员名称 | 类型 | 描述 | 接口使用逻辑 |
| ------- | ---- | ---- | ----------- |
| bits | struct | 位域结构体，包含各管脚控制位 | radar_rf_para_t.ant_sw_ctrl_en 成员，通过 uapi_radar_set_hardware_para 入参 |
| u8 | uint8_t | 8位无符号整数整体访问 | radar_rf_para_t.ant_sw_ctrl_en 成员，通过 uapi_radar_set_hardware_para 入参 |

## Enumerations

### radar_set_power_status_t <a id="enum_radar_set_power_status_t"></a>

```c
typedef enum {
    CHIP_STATUS_POWEROFF = 0, /*!< 芯片下电 */
    CHIP_STATUS_POWERON,      /*!< 芯片上电 */
} radar_set_power_status_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| CHIP_STATUS_POWEROFF | 0 | 芯片下电 |
| CHIP_STATUS_POWERON | 1 | 芯片上电 |

### radar_set_status_t <a id="enum_radar_set_status_t"></a>

```c
typedef enum {
    RADAR_STATUS_STOP = 0, /*!< 雷达状态配置停止 */
    RADAR_STATUS_START,    /*!< 雷达状态配置启动 */
} radar_set_status_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| RADAR_STATUS_STOP | 0 | 雷达停止 |
| RADAR_STATUS_START | 1 | 雷达启动 |

### radar_get_status_t <a id="enum_radar_get_status_t"></a>

```c
typedef enum {
    RADAR_STATUS_RADAR_ON,     /*!< 雷达使能状态 */
    RADAR_STATUS_RADAR_OFF     /*!< 雷达关闭状态 */
} radar_get_status_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| RADAR_STATUS_RADAR_ON | 0 | 雷达使能状态 |
| RADAR_STATUS_RADAR_OFF | 1 | 雷达关闭状态 |

### radar_frame_mode_t <a id="enum_radar_frame_mode_t"></a>

```c
typedef enum {
    RADAR_FRAME_MODE_RANGING,               /*!< SLP测距共存帧模式 */
    RADAR_FRAME_MODE_DEFAULT,               /*!< 单雷达帧模式 */
    RADAR_FRAME_MODE_VENDOR,                /*!< 单雷达模式：厂商帧模式 */
} radar_frame_mode_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| RADAR_FRAME_MODE_RANGING | 0 | SLP测距共存帧模式 |
| RADAR_FRAME_MODE_DEFAULT | 1 | 单雷达帧模式 |
| RADAR_FRAME_MODE_VENDOR | 2 | 单雷达模式：厂商帧模式 |

### radar_anti_multipath_mode_t <a id="enum_radar_anti_multipath_mode_t"></a>

```c
typedef enum {
    RADAR_ANTI_MULTIPATH_DEFAULT_MODE,      /*!< 抗多径虚警默认模式 */
    RADAR_ANTI_MULTIPATH_STRENGTHEN_MODE,   /*!< 抗多径虚警能力强化模式 */
    RADAR_ANTI_MULTIPATH_WEAKEN_MODE,       /*!< 抗多径虚警能力弱化模式 */
} radar_anti_multipath_mode_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| RADAR_ANTI_MULTIPATH_DEFAULT_MODE | 0 | 抗多径虚警默认模式 |
| RADAR_ANTI_MULTIPATH_STRENGTHEN_MODE | 1 | 抗多径虚警能力强化模式 |
| RADAR_ANTI_MULTIPATH_WEAKEN_MODE | 2 | 抗多径虚警能力弱化模式 |

### radar_cali_res_save_mode_t <a id="enum_radar_cali_res_save_mode_t"></a>

```c
typedef enum {
    RADAR_CALI_RES_SAVE_IN_NV,              /*!< 雷达校准参数存储在host NV中 */
    RADAR_CALI_RES_SAVE_IN_EEPROM,          /*!< 雷达校准参数存储在device EEPROM中 */
} radar_cali_res_save_mode_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| RADAR_CALI_RES_SAVE_IN_NV | 0 | 雷达校准参数存储在host NV中 |
| RADAR_CALI_RES_SAVE_IN_EEPROM | 1 | 雷达校准参数存储在device EEPROM中 |

### radar_report_mode_t <a id="enum_radar_report_mode_t"></a>

```c
typedef enum {
    RADAR_REPORT_RES = 0,                   /*!< 只上报检测结果 */
    RADAR_REPORT_SLEEP_RAW_DATA = 1,        /*!< 同时上报检测结果和睡眠原始数据 */
} radar_report_mode_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| RADAR_REPORT_RES | 0 | 只上报检测结果 |
| RADAR_REPORT_SLEEP_RAW_DATA | 1 | 同时上报检测结果和睡眠原始数据 |

### errcode_radar_client_t <a id="enum_errcode_radar_client_t"></a>

```c
typedef enum {
    ERRCODE_RC_SUCCESS = 0,                                /*!< 执行成功错误码 */
    ERRCODE_RC_MALLOC_FAILED = ERRCODE_RADAR_CLIENT_BASE,  /*!< 申请动态内存失败错误码 */
    ERRCODE_RC_QUEUE_WRITE_FAILED,                         /*!< 消息队列写入失败错误码 */
    ERRCODE_RC_POWER_ON_FAILED,                            /*!< 上电加载失败错误码 */
    ERRCODE_RC_POWERED_ON,                                 /*!< 已经上电加载错误码 */
    ERRCODE_RC_POWERED_OFF,                                /*!< 已经下电错误码 */
    ERRCODE_RC_RADAR_ENABLED,                              /*!< 雷达已经使能错误码 */
    ERRCODE_RC_RADAR_DISABLED,                             /*!< 雷达已经关闭错误码 */
    ERRCODE_RC_PTR_NULL,                                   /*!< 雷达指针为空 */
    ERRCODE_RC_PARA_INVALID,                               /*!< 雷达参数不合规 */
    ERRCODE_RC_STATUS_ERROR = ERRCODE_RADAR_CLIENT_UAPI_BASE, /*!< 启动参数异常 */
    ERRCODE_RC_CALL_BACK,                                  /*!< 回调注册异常 */
    ERRCODE_RC_CHANNEL_IDX,                                /*!< 信道索引 */
    ERRCODE_RC_GPIO_PINMUX,                                /*!< 开关GPIO PINMUX配置 */
    ERRCODE_RC_ANT_CHANNEL_NUM,                            /*!< 天线对有效值可控 */
    ERRCODE_RC_ANT_CODE,                                   /*!< 天线码字可配 */
    ERRCODE_RC_TX_POWER,                                   /*!< 发射功率 */
    ERRCODE_RC_AGC_LNA,                                    /*!< AGC增益配置LNA */
    ERRCODE_RC_AGC_VGA,                                    /*!< AGC增益配置VGA */
    ERRCODE_RC_ANT_SW_CTRL,                                /*!< 射频开关控制管脚 */
    ERRCODE_RC_SUBFRAME_PERIOD,                            /*!< 子帧间隔 */
    ERRCODE_RC_ANT_SWITCH_INTERVAL,                        /*!< 雷达天线对切换间隔 */
    ERRCODE_RC_START_BIN,                                  /*!< 雷达原始数据参数：起始bin索引 */
    ERRCODE_RC_END_BIN,                                    /*!< 雷达原始数据参数：终止bin索引 */
    ERRCODE_RC_TX_WAVE_SPREAD_FACTOR,                      /*!< 雷达波形参数：波形序列扩频因子 */
    ERRCODE_RC_TX_WAVE_ACC_RSHIFT_BIT,                     /*!< 雷达波形参数：累加位宽保护右移位数 */
    ERRCODE_RC_TX_WAVE_ACC_NUM,                            /*!< 雷达波形参数：累加次数 */
    ERRCODE_RC_TX_WAVE_CORR_DIV,                           /*!< 雷达波形参数：相关后除数因子 */
    ERRCODE_RC_TX_WAVE_TX_WAVE_LEN,                        /*!< 雷达波形参数：发射波形长度 */
    ERRCODE_RC_TX_WAVE_TX_WAVE_BITS,                       /*!< 雷达波形参数：波形扩频前的三元码序列 */
    ERRCODE_RC_RADAR_VERSION_NOT_INIT,                     /*!< 雷达版本未初始化 */
    ERRCODE_RC_NV_WRITE_FAILED,                             /*!< 雷达写NV失败 */
    ERRCODE_RC_NV_READ_FAILED,                              /*!< 雷达读NV失败 */
    ERRCODE_RC_START_UP = ERRCODE_RADAR_CLIENT_SW_BASE,    /*!< 启动异常 */
    ERRCODE_RC_CLOSE,                                      /*!< 关闭异常 */
    ERRCODE_RC_FRAME_BUFFER_FULL,                          /*!< 上报数据缓冲满，bufferfull，丢帧 */
    ERRCODE_RC_RAINGING_DURATION,                          /*!< 共存指向间隔异常 */
    ERRCODE_RC_TX_AHEAD_READ_ADC,                          /*!< 雷达TX发送在读取ADC数据前 */
    ERRCODE_RC_AGC_CALI_FAILED,                            /*!< AGC校准异常 */
    ERRCODE_RC_PC_PEAK_OVERFLOWED,                         /*!< 雷达脉压峰值溢出 */
    ERRCODE_RC_EEPROM_READ_FAILED,                         /*!< 雷达从eeprom中读取校准结果时发生错误 */
    ERRCODE_RC_IMAGE_CHECK_FAILED,                         /*!< 雷达固件镜像校验错误 */
    ERRCODE_RC_MAX = ERRCODE_RADAR_CLIENT_END,             /*!< RC错误码最大值 */
} errcode_radar_client_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| ERRCODE_RC_SUCCESS | 0 | 执行成功 |
| ERRCODE_RC_MALLOC_FAILED | 0x8000A100 | 申请动态内存失败 |
| ERRCODE_RC_QUEUE_WRITE_FAILED | 0x8000A101 | 消息队列写入失败 |
| ERRCODE_RC_POWER_ON_FAILED | 0x8000A102 | 上电加载失败 |
| ERRCODE_RC_POWERED_ON | 0x8000A103 | 已经上电 |
| ERRCODE_RC_POWERED_OFF | 0x8000A104 | 已经下电 |
| ERRCODE_RC_RADAR_ENABLED | 0x8000A105 | 雷达已经使能 |
| ERRCODE_RC_RADAR_DISABLED | 0x8000A106 | 雷达已经关闭 |
| ERRCODE_RC_PTR_NULL | 0x8000A107 | 指针为空 |
| ERRCODE_RC_PARA_INVALID | 0x8000A108 | 参数不合规 |
| ERRCODE_RC_STATUS_ERROR | 0x8000A11E | 启动参数异常 |
| ERRCODE_RC_CALL_BACK | 0x8000A11F | 回调注册异常 |
| ERRCODE_RC_CHANNEL_IDX | 0x8000A120 | 信道索引 |
| ERRCODE_RC_GPIO_PINMUX | 0x8000A121 | 开关GPIO (General Purpose Input/Output) PINMUX (Pin Multiplexing) 配置 |
| ERRCODE_RC_ANT_CHANNEL_NUM | 0x8000A122 | 天线对有效值可控 |
| ERRCODE_RC_ANT_CODE | 0x8000A123 | 天线码字可配 |
| ERRCODE_RC_TX_POWER | 0x8000A124 | 发射功率 |
| ERRCODE_RC_AGC_LNA | 0x8000A125 | AGC增益配置LNA (Low Noise Amplifier) |
| ERRCODE_RC_AGC_VGA | 0x8000A126 | AGC增益配置VGA (Variable Gain Amplifier) |
| ERRCODE_RC_ANT_SW_CTRL | 0x8000A127 | 射频开关控制管脚 |
| ERRCODE_RC_SUBFRAME_PERIOD | 0x8000A128 | 子帧间隔 |
| ERRCODE_RC_ANT_SWITCH_INTERVAL | 0x8000A129 | 雷达天线对切换间隔 |
| ERRCODE_RC_START_BIN | 0x8000A12A | 起始bin索引 |
| ERRCODE_RC_END_BIN | 0x8000A12B | 终止bin索引 |
| ERRCODE_RC_TX_WAVE_SPREAD_FACTOR | 0x8000A12C | 波形序列扩频因子 |
| ERRCODE_RC_TX_WAVE_ACC_RSHIFT_BIT | 0x8000A12D | 累加位宽保护右移位数 |
| ERRCODE_RC_TX_WAVE_ACC_NUM | 0x8000A12E | 累加次数 |
| ERRCODE_RC_TX_WAVE_CORR_DIV | 0x8000A12F | 相关后除数因子 |
| ERRCODE_RC_TX_WAVE_TX_WAVE_LEN | 0x8000A130 | 发射波形长度 |
| ERRCODE_RC_TX_WAVE_TX_WAVE_BITS | 0x8000A131 | 波形扩频前的三元码序列 |
| ERRCODE_RC_RADAR_VERSION_NOT_INIT | 0x8000A132 | 雷达版本未初始化 |
| ERRCODE_RC_NV_WRITE_FAILED | 0x8000A133 | 雷达写NV失败 |
| ERRCODE_RC_NV_READ_FAILED | 0x8000A134 | 雷达读NV失败 |
| ERRCODE_RC_START_UP | 0x8000A183 | 启动异常 |
| ERRCODE_RC_CLOSE | 0x8000A184 | 关闭异常 |
| ERRCODE_RC_FRAME_BUFFER_FULL | 0x8000A185 | 上报数据缓冲满，丢帧 |
| ERRCODE_RC_RAINGING_DURATION | 0x8000A186 | 共存指向间隔异常 |
| ERRCODE_RC_TX_AHEAD_READ_ADC | 0x8000A187 | 雷达TX (Transmit) 发送在读取ADC (Analog-to-Digital Converter) 数据前 |
| ERRCODE_RC_AGC_CALI_FAILED | 0x8000A188 | AGC校准异常 |
| ERRCODE_RC_PC_PEAK_OVERFLOWED | 0x8000A189 | 雷达脉压峰值溢出 |
| ERRCODE_RC_EEPROM_READ_FAILED | 0x8000A18A | eeprom读取校准结果错误 |
| ERRCODE_RC_IMAGE_CHECK_FAILED | 0x8000A18B | 雷达固件镜像校验错误 |
| ERRCODE_RC_MAX | 0x8000A1FF | RC错误码最大值 |

## Structures

### radar_agc_para_t <a id="struct_radar_agc_para_t"></a>

```c
typedef struct {
    uint8_t lna_code[RADAR_MAX_ANT_CH_NUM]; /*!< 雷达AGC参数：LNA码字 */
    uint8_t vga_code[RADAR_MAX_ANT_CH_NUM]; /*!< 雷达AGC参数：VGA码字 */
} radar_agc_para_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| lna_code | uint8_t[8] | AGC参数：LNA码字 |
| vga_code | uint8_t[8] | AGC参数：VGA码字 |

### radar_wave_para_t <a id="struct_radar_wave_para_t"></a>

```c
typedef struct {
    uint8_t spread_factor;                   /*!< 雷达波形参数：波形序列扩频因子 */
    uint8_t acc_rshift_bit;                  /*!< 雷达波形参数：累加位宽保护右移位数 */
    uint16_t acc_num;                        /*!< 雷达波形参数：累加次数 */
    uint8_t corr_div;                        /*!< 雷达波形参数：相关后除数因子 */
    uint8_t tx_wave_len;                     /*!< 雷达波形参数：发射波形长度 */
    int8_t tx_wave_bits[RADAR_MAX_WAVE_LEN]; /*!< 雷达波形参数：波形扩频前的三元码序列 */
} radar_wave_para_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| spread_factor | uint8_t | 波形序列扩频因子 |
| acc_rshift_bit | uint8_t | 累加位宽保护右移位数 |
| acc_num | uint16_t | 累加次数 |
| corr_div | uint8_t | 相关后除数因子 |
| tx_wave_len | uint8_t | 发射波形长度，取值范围[0, 64] |
| tx_wave_bits | int8_t[64] | 波形扩频前的三元码序列 |

### radar_frame_para_t <a id="struct_radar_frame_para_t"></a>

```c
typedef struct {
    uint16_t subframe_period;     /*!< 雷达帧参数：子帧间隔，单位0.01ms */
    uint16_t ant_switch_interval; /*!< 雷达帧参数：天线切换间隔，单位0.01ms */
} radar_frame_para_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| subframe_period | uint16_t | 子帧间隔，单位0.01ms |
| ant_switch_interval | uint16_t | 天线切换间隔，单位0.01ms |

### radar_raw_data_para_t <a id="struct_radar_raw_data_para_t"></a>

```c
typedef struct {
    uint8_t start_bin; /*!< 雷达原始数据参数：起始bin索引 */
    uint8_t end_bin;   /*!< 雷达原始数据参数：终止bin索引 */
} radar_raw_data_para_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| start_bin | uint8_t | 起始bin索引 |
| end_bin | uint8_t | 终止bin索引 |

### complex_short_t <a id="struct_complex_short_t"></a>

```c
typedef struct {
    int16_t real; /*!< 复数数据：实部 */
    int16_t imag; /*!< 复数数据：虚部 */
} complex_short_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| real | int16_t | 复数数据：实部 |
| imag | int16_t | 复数数据：虚部 |

### radar_target_info_t <a id="struct_radar_target_info_t"></a>

```c
typedef struct {
    uint16_t id; /*!< 雷达检测结果：目标序号 */
    int16_t rng; /*!< 雷达检测结果：目标距离(厘米) */
    int16_t agl; /*!< 雷达检测结果：目标角度(0.01度) */
    int16_t vel; /*!< 雷达检测结果：目标速度(厘米/秒) */
    uint16_t activity_intensity; /*!< 雷达检测结果：目标活动强度 */
    uint16_t reserve0; /*!< 雷达数据：保留字段0 */
    uint16_t reserve1; /*!< 雷达数据：保留字段1 */
} radar_target_info_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| id | uint16_t | 目标序号 |
| rng | int16_t | 目标距离，单位厘米 |
| agl | int16_t | 目标角度，单位0.01度 |
| vel | int16_t | 目标速度，单位厘米/秒 |
| activity_intensity | uint16_t | 目标活动强度 |
| reserve0 | uint16_t | 保留字段0 |
| reserve1 | uint16_t | 保留字段1 |

### radar_result_t <a id="struct_radar_result_t"></a>

```c
typedef struct {
    uint8_t target_num;                                        /*!< 雷达检测结果：有效目标个数 */
    radar_target_info_t target_info[RADAR_MAX_RPT_TARGET_NUM]; /*!< 雷达检测结果：目标检测结果数组 */
} radar_result_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| target_num | uint8_t | 有效目标个数，取值范围[0, 3] |
| target_info | [radar_target_info_t](#struct_radar_target_info_t)[3] | 目标检测结果数组 |

### radar_raw_data_msg_t <a id="struct_radar_raw_data_msg_t"></a>

```c
typedef struct {
    uint16_t data_len;                                   /*!< 雷达数据：数据长度 */
    uint16_t crc;                                        /*!< 雷达数据：数据crc校验值 */
    uint16_t counter;                                    /*!< 雷达数据：数据上报帧号 */
    uint64_t tick_cnt;                                   /*!< 雷达数据：数据帧32k时钟计数 */
    radar_raw_data_para_t data_para;                     /*!< 雷达数据: 数据参数 */
    radar_frame_mode_t mode;                               /*!< 雷达数据：雷达帧模式 */
    uint8_t vendor_info[RADAR_VENDOR_INFO_BYTE];         /*!< 雷达数据：vendor参数 */
    uint64_t reserve;                                    /*!< 雷达数据：保留字段 */
    complex_short_t *data;                               /*!< 雷达数据：数据指针 */
} radar_raw_data_msg_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| data_len | uint16_t | 数据长度 |
| crc | uint16_t | 数据crc校验值 |
| counter | uint16_t | 数据上报帧号 |
| tick_cnt | uint64_t | 数据帧32k时钟计数 |
| data_para | [radar_raw_data_para_t](#struct_radar_raw_data_para_t) | 数据参数 |
| mode | [radar_frame_mode_t](#enum_radar_frame_mode_t) | 雷达帧模式 |
| vendor_info | uint8_t[2] | vendor参数 |
| reserve | uint64_t | 保留字段 |
| data | [complex_short_t](#struct_complex_short_t) * | 数据指针，数据格式按距离bin数目I/Q顺序存放，与结构体内存不连续 |

### radar_rf_para_t <a id="struct_radar_rf_para_t"></a>

```c
typedef struct {
    bool pwr_ctrl;                              /*!< 雷达射频参数：射频开关电源是否独立控制 */
    uint8_t ch_idx;                             /*!< 雷达射频参数：信道号索引 */
    uint8_t ant_ch_num;                         /*!< 雷达射频参数：有效天线通道个数 */
    uint8_t tx_power_idx[RADAR_MAX_ANT_CH_NUM]; /*!< 雷达射频参数：Tx发射功率索引 */
    uint8_t ant_code[RADAR_MAX_ANT_CH_NUM];     /*!< 雷达射频参数：天线码字 */
    uint32_t board_id;                          /*!< 雷达硬件参数：模组型号 */
    radar_agc_para_t agc_para;                  /*!< 雷达射频参数：AGC参数 */
    radar_rf_sw_bit ant_sw_ctrl_en;             /*!< 雷达射频参数：射频开关控制管脚使能 */
} radar_rf_para_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| pwr_ctrl | bool | 射频开关电源是否独立控制 |
| ch_idx | uint8_t | 信道号索引 |
| ant_ch_num | uint8_t | 有效天线通道个数，取值范围[1, 8] |
| tx_power_idx | uint8_t[8] | Tx发射功率索引 |
| ant_code | uint8_t[8] | 天线码字 |
| board_id | uint32_t | 模组型号 |
| agc_para | [radar_agc_para_t](#struct_radar_agc_para_t) | AGC参数 |
| ant_sw_ctrl_en | [radar_rf_sw_bit](#union_radar_rf_sw_bit) | 射频开关控制管脚使能 |

### radar_hardware_para_t <a id="struct_radar_hardware_para_t"></a>

```c
typedef struct {
    radar_rf_para_t rf_para;                            /*!< 雷达硬件参数：射频参数 */
    radar_frame_para_t frame_para;                      /*!< 雷达硬件参数：帧参数 */
    radar_raw_data_para_t raw_data_para;                /*!< 雷达硬件参数：原始数据上报配置参数 */
    radar_wave_para_t wave_para;                        /*!< 雷达硬件参数：波形参数 */
    uint8_t vendor_info[RADAR_VENDOR_INFO_BYTE];        /*!< 雷达硬件参数：vendor参数 */
    uint8_t reserve[RADAR_HW_PARA_RSV_BYTE];            /*!< 雷达硬件参数：预留位 */
} radar_hardware_para_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| rf_para | [radar_rf_para_t](#struct_radar_rf_para_t) | 射频参数 |
| frame_para | [radar_frame_para_t](#struct_radar_frame_para_t) | 帧参数 |
| raw_data_para | [radar_raw_data_para_t](#struct_radar_raw_data_para_t) | 原始数据上报配置参数 |
| wave_para | [radar_wave_para_t](#struct_radar_wave_para_t) | 波形参数 |
| vendor_info | uint8_t[2] | vendor参数 |
| reserve | uint8_t[12] | 预留位 |

### radar_alg_basic_para_t <a id="struct_radar_alg_basic_para_t"></a>

```c
typedef struct {
    uint32_t rpt_mode;      /*!< 雷达算法基础参数: 数据上报类型，默认值为0，参考 radar_report_mode_t */
    uint32_t sensitivity;   /*!< 雷达算法基础参数: 检测灵敏度, 默认值为0 */
    uint32_t rng_boundary;  /*!< 雷达算法基础参数: 距离检测边界，单位cm，默认值700 */
    uint32_t delay_time;    /*!< 雷达算法基础参数: 退出时延，单位S，默认值20 */
    uint16_t ant_space_amp_coef;    /*!< 雷达算法基础参数: 天线间距缩放系数，默认值10000 */
    uint16_t static_create_range;   /*!< 雷达算法基础参数: 静目标允许创建的距离门限。单位cm，默认为0 */
    uint8_t is_wire_down;   /*!< 雷达算法基础参数: 端子线朝向，影响角度上报值的正负。1表示朝下，0表示朝上 */
    uint8_t angle_boundary; /*!< 雷达算法基础参数: 角度检测边界，单位°，默认值60 */
    uint8_t anti_multipath_mode; /*!< 雷达算法基础参数: 抗多径虚警模式。默认值0，参考 radar_anti_multipath_mode_t */
    uint8_t cali_res_save_mode;  /*!< 雷达算法基础参数: 校准参数存储模式，默认值为0，表示存入Host芯片的NV中，参考 radar_cali_res_save_mode_t */
} radar_alg_basic_para_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| rpt_mode | uint32_t | 数据上报类型，默认值为0，参考[radar_report_mode_t](#enum_radar_report_mode_t) |
| sensitivity | uint32_t | 检测灵敏度，默认值为0 |
| rng_boundary | uint32_t | 距离检测边界，单位cm，默认值700 |
| delay_time | uint32_t | 退出时延，单位S，默认值20 |
| ant_space_amp_coef | uint16_t | 天线间距缩放系数，默认值10000 |
| static_create_range | uint16_t | 静目标允许创建的距离门限，单位cm，默认为0 |
| is_wire_down | uint8_t | 端子线朝向，1表示朝下，0表示朝上 |
| angle_boundary | uint8_t | 角度检测边界，单位°，默认值60 |
| anti_multipath_mode | uint8_t | 抗多径虚警模式，默认值0，参考[radar_anti_multipath_mode_t](#enum_radar_anti_multipath_mode_t) |
| cali_res_save_mode | uint8_t | 校准参数存储模式，默认值0表示存入Host芯片的NV中，参考[radar_cali_res_save_mode_t](#enum_radar_cali_res_save_mode_t) |

### radar_ai_para_t <a id="struct_radar_ai_para_t"></a>

```c
typedef struct {
    int32_t dynamic_offset_direction[RADAR_AI_OFFSET_PARA_NUM]; /*!< 雷达AI参数: AI动检测补偿参数。取值范围[-100000, 100000]，默认值0 */
    int32_t static_offset_direction[RADAR_AI_OFFSET_PARA_NUM]; /*!< 雷达AI参数: AI静检测补偿参数。取值范围[-100000, 100000]，默认值0 */
    uint16_t ai_status;       /*!< 雷达AI参数: AI抗干扰功能开关. 0表示关闭，1表示打开。默认是1 */
} radar_ai_para_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| dynamic_offset_direction | int32_t[8] | AI动检测补偿参数，取值范围[-100000, 100000]，默认值0 |
| static_offset_direction | int32_t[8] | AI静检测补偿参数，取值范围[-100000, 100000]，默认值0 |
| ai_status | uint16_t | AI抗干扰功能开关，0表示关闭，1表示打开，默认值1 |

### radar_sensitivity_para_t <a id="struct_radar_sensitivity_para_t"></a>

```c
typedef struct {
    uint8_t para_len;                                         /*!< 雷达检测灵敏度参数: 灵敏度参数长度 */
    uint8_t dyn_sensitivity_0[RADAR_MAX_SENSITIVITY_ARR_LEN]; /*!< 雷达检测灵敏度参数: 动目标检测灵敏度参数0 */
    uint8_t dyn_sensitivity_1[RADAR_MAX_SENSITIVITY_ARR_LEN]; /*!< 雷达检测灵敏度参数: 动目标检测灵敏度参数1 */
    uint8_t sta_sensitivity_0[RADAR_MAX_SENSITIVITY_ARR_LEN]; /*!< 雷达检测灵敏度参数: 静目标检测灵敏度参数0 */
} radar_sensitivity_para_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| para_len | uint8_t | 灵敏度参数长度 |
| dyn_sensitivity_0 | uint8_t[60] | 动目标检测灵敏度参数0 |
| dyn_sensitivity_1 | uint8_t[60] | 动目标检测灵敏度参数1 |
| sta_sensitivity_0 | uint8_t[60] | 静目标检测灵敏度参数0 |

### radar_alg_para_t <a id="struct_radar_alg_para_t"></a>

```c
typedef struct {
    radar_alg_basic_para_t basic_para;          /*!< 雷达算法参数: 基础参数 */
    radar_ai_para_t ai_para;                    /*!< 雷达算法参数: AI补偿参数 */
    radar_sensitivity_para_t sensitivity_para;  /*!< 雷达算法参数: 灵敏度参数 */
    uint8_t reserve[RADAR_ALG_PARA_RSV_BYTE];   /*!< 雷达算法参数：预留位 */
} radar_alg_para_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| basic_para | [radar_alg_basic_para_t](#struct_radar_alg_basic_para_t) | 基础参数 |
| ai_para | [radar_ai_para_t](#struct_radar_ai_para_t) | AI补偿参数 |
| sensitivity_para | [radar_sensitivity_para_t](#struct_radar_sensitivity_para_t) | 灵敏度参数 |
| reserve | uint8_t[8] | 预留位 |




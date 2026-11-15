# Radar Sub-6G

Radar Sub-6G 提供 Sub-6G 频段雷达的感知检测能力，支持设置与查询雷达工作状态（启动、停止、复位、恢复）及硬件状态，并允许注册检测结果变化与当前帧结果的回调函数以获取雷达融合输出。本模块涵盖状态控制、状态查询与结果回调注册等接口。

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_sub_6g.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_radar_set_status](#uapi_radar_set_status) | 设置雷达工作状态 |
| [uapi_radar_get_status](#uapi_radar_get_status) | 获取雷达软件工作状态 |
| [uapi_radar_get_hardware_status](#uapi_radar_get_hardware_status) | 获取雷达硬件状态 |
| [uapi_radar_register_result_cb](#uapi_radar_register_result_cb) | 注册雷达检测结果变化回调函数 |
| [uapi_radar_register_current_frame_result_cb](#uapi_radar_register_current_frame_result_cb) | 注册雷达当前帧结果回调函数 |
| [uapi_radar_get_result](#uapi_radar_get_result) | 获取雷达检测结果 |
| [uapi_radar_get_current_frame_result](#uapi_radar_get_current_frame_result) | 获取雷达当前帧检测结果 |
| [uapi_radar_register_raw_data_cb](#uapi_radar_register_raw_data_cb) | 注册雷达原始数据回调函数 |
| [uapi_radar_set_delay_time](#uapi_radar_set_delay_time) | 设置雷达无人档位退出延迟时间 |
| [uapi_radar_get_delay_time](#uapi_radar_get_delay_time) | 获取雷达无人档位退出延迟时间 |
| [uapi_radar_get_isolation](#uapi_radar_get_isolation) | 获取天线隔离度信息 |
| [uapi_radar_set_debug_para](#uapi_radar_set_debug_para) | 设置雷达维测参数 |
| [uapi_radar_select_alg_para](#uapi_radar_select_alg_para) | 设置雷达算法参数套选择参数 |
| [uapi_radar_set_alg_para](#uapi_radar_set_alg_para) | 设置雷达算法参数 |
| [uapi_radar_get_debug_info](#uapi_radar_get_debug_info) | 查询雷达维测数据 |
| [uapi_radar_register_debug_info_cb](#uapi_radar_register_debug_info_cb) | 注册雷达维测数据回调函数 |
| [uapi_radar_register_channel_switch_cb](#uapi_radar_register_channel_switch_cb) | 注册雷达信道切换回调函数 |
| [uapi_radar_set_mwo_mode_para](#uapi_radar_set_mwo_mode_para) | 设置微波模式检测参数 |

## Functions

### uapi_radar_set_status <a id="uapi_radar_set_status"></a>

```c
errcode_t uapi_radar_set_status(uint8_t sts)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_sub_6g.h"
```

**功能说明**

- 设置雷达工作状态，包括启动、停止、复位、恢复等状态配置
- 通过配置雷达状态控制雷达检测功能的启停，支持启动检测、停止检测、复位、恢复等操作
- 重复调用时以最后一次设置为准

**前置条件**

- 雷达模块已初始化完成
- 配置项 CONFIG_RADAR_PLT_SUB_6G 已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| sts | uint8_t | 雷达状态配置值 | [radar_set_sts_t](#enum_radar_set_sts_t): RADAR_STATUS_STOP(0), RADAR_STATUS_START(1), RADAR_STATUS_RESET(2), RADAR_STATUS_RESUME(3) |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 状态设置成功 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 执行失败 |

**参考案例**

- [radar_slp_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/slp_sample/radar_slp_sample.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_PLT_SUB_6G | 特性宏 | 支持雷达 Sub_6G 接口功能 | n |

### uapi_radar_get_status <a id="uapi_radar_get_status"></a>

```c
errcode_t uapi_radar_get_status(uint8_t *sts)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_sub_6g.h"
```

**功能说明**

- 获取雷达软件工作状态，包括空闲状态和运行状态
- 用于查询雷达当前是否处于检测工作状态
- 调用后通过出参返回当前软件状态

**前置条件**

- 雷达模块已初始化完成
- 配置项 CONFIG_RADAR_PLT_SUB_6G 已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| sts | uint8_t* | 雷达软件状态输出指针 | [radar_get_sts_t](#enum_radar_get_sts_t): RADAR_STATUS_IDLE(0), RADAR_STATUS_RUNNING(1) |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| *sts | uint8_t | 雷达软件状态，参考[radar_get_sts_t](#enum_radar_get_sts_t) |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 状态获取成功 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 执行失败 |

**参考案例**

- [radar_softap_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/softap_sample/radar_softap_sample.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_PLT_SUB_6G | 特性宏 | 支持雷达 Sub_6G 接口功能 | n |

### uapi_radar_get_hardware_status <a id="uapi_radar_get_hardware_status"></a>

```c
errcode_t uapi_radar_get_hardware_status(uint8_t *sts)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_sub_6g.h"
```

**功能说明**

- 获取雷达硬件状态，包括故障状态和正常状态
- 用于查询雷达硬件是否处于正常工作状态
- 调用后通过出参返回当前硬件状态

**前置条件**

- 雷达模块已初始化完成
- 配置项 CONFIG_RADAR_PLT_SUB_6G 已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| sts | uint8_t* | 雷达硬件状态输出指针 | [radar_get_hardware_sts_t](#enum_radar_get_hardware_sts_t): RADAR_STATUS_HW_FAULT(0), RADAR_STATUS_HW_NORMAL(1) |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| *sts | uint8_t | 雷达硬件状态，参考[radar_get_hardware_sts_t](#enum_radar_get_hardware_sts_t) |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 硬件状态获取成功 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_PLT_SUB_6G | 特性宏 | 支持雷达 Sub_6G 接口功能 | n |

### uapi_radar_register_result_cb <a id="uapi_radar_register_result_cb"></a>

```c
errcode_t uapi_radar_register_result_cb(radar_result_cb_t cb)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_sub_6g.h"
```

**功能说明**

- 注册雷达检测结果变化回调函数，当雷达检测完成且结果发生变化时触发回调
- 回调函数运行于 radar feature 线程，不能阻塞或长时间等待，不能使用较大栈空间
- 重复注册会覆盖上一次注册的回调函数

**前置条件**

- 雷达模块已初始化完成
- 配置项 CONFIG_RADAR_PLT_SUB_6G 已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| cb | [radar_result_cb_t](#typedef_radar_result_cb_t) | 雷达检测结果变化回调函数 | 非空函数指针 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 回调注册成功 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 执行失败 |

**参考案例**

- [radar_slp_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/slp_sample/radar_slp_sample.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_PLT_SUB_6G | 特性宏 | 支持雷达 Sub_6G 接口功能 | n |

### uapi_radar_register_current_frame_result_cb <a id="uapi_radar_register_current_frame_result_cb"></a>

```c
errcode_t uapi_radar_register_current_frame_result_cb(radar_current_frame_result_cb_t cb)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_sub_6g.h"
```

**功能说明**

- 注册雷达当前帧结果回调函数，每一个雷达帧计算完成后触发回调
- 回调函数运行于 radar feature 线程，不能阻塞或长时间等待，不能使用较大栈空间
- 重复注册会覆盖上一次注册的回调函数

**前置条件**

- 雷达模块已初始化完成
- 配置项 CONFIG_RADAR_PLT_SUB_6G 已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| cb | [radar_current_frame_result_cb_t](#typedef_radar_current_frame_result_cb_t) | 雷达当前帧结果回调函数 | 非空函数指针 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 回调注册成功 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 执行失败 |

**参考案例**

- [radar_softap_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/softap_sample/radar_softap_sample.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_PLT_SUB_6G | 特性宏 | 支持雷达 Sub_6G 接口功能 | n |

### uapi_radar_get_result <a id="uapi_radar_get_result"></a>

```c
errcode_t uapi_radar_get_result(radar_result_t *res)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_sub_6g.h"
```

**功能说明**

- 获取雷达上报结果，包括检测边界及人体存在信息
- 主动拉取雷达融合检测结果，包含上下边界和人体存在标志
- 调用后通过出参返回当前检测结果

**前置条件**

- 雷达模块已初始化完成
- 配置项 CONFIG_RADAR_PLT_SUB_6G 已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| res | [radar_result_t](#struct_radar_result_t)* | 雷达上报结果指针 | 非空指针，指向已分配的内存空间 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| *res | [radar_result_t](#struct_radar_result_t) | 雷达融合检测结果 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 结果获取成功 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 执行失败 |

**参考案例**

- [radar_slp_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/slp_sample/radar_slp_sample.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_PLT_SUB_6G | 特性宏 | 支持雷达 Sub_6G 接口功能 | n |

### uapi_radar_get_current_frame_result <a id="uapi_radar_get_current_frame_result"></a>

```c
errcode_t uapi_radar_get_current_frame_result(radar_current_frame_result_t *res)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_sub_6g.h"
```

**功能说明**

- 获取雷达当前帧上报结果，包含档位、距离、速度、信噪比信息
- 主动拉取当前帧检测结果，包含档位、距离测量值、速度测量值、信噪比
- 调用后通过出参返回当前帧结果

**前置条件**

- 雷达模块已初始化完成
- 配置项 CONFIG_RADAR_PLT_SUB_6G 已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| res | [radar_current_frame_result_t](#struct_radar_current_frame_result_t)* | 雷达当前帧结果指针 | 非空指针，指向已分配的内存空间 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| *res | [radar_current_frame_result_t](#struct_radar_current_frame_result_t) | 雷达当前帧检测结果 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 结果获取成功 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_PLT_SUB_6G | 特性宏 | 支持雷达 Sub_6G 接口功能 | n |

### uapi_radar_register_raw_data_cb <a id="uapi_radar_register_raw_data_cb"></a>

```c
errcode_t uapi_radar_register_raw_data_cb(radar_raw_data_cb_t cb)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_sub_6g.h"
```

**功能说明**

- 注册雷达原始数据回调函数，雷达回波接收完成时触发回调
- 回调函数运行于 radar driver 线程，不能阻塞或长时间等待，不能使用较大栈空间
- 重复注册会覆盖上一次注册的回调函数

**前置条件**

- 雷达模块已初始化完成
- 配置项 CONFIG_RADAR_PLT_SUB_6G 已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| cb | [radar_raw_data_cb_t](#typedef_radar_raw_data_cb_t) | 雷达原始数据回调函数 | 非空函数指针 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 回调注册成功 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 执行失败 |

**参考案例**

- [radar_slp_tcp_socket.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/slp_wireless_sample/radar_slp_tcp_socket.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_PLT_SUB_6G | 特性宏 | 支持雷达 Sub_6G 接口功能 | n |

### uapi_radar_set_delay_time <a id="uapi_radar_set_delay_time"></a>

```c
errcode_t uapi_radar_set_delay_time(uint16_t time)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_sub_6g.h"
```

**功能说明**

- 设置雷达无人档位退出延迟时间
- 控制雷达在检测不到人体后延迟退出无人档位的时间
- 重复调用以最后一次设置为准

**前置条件**

- 雷达模块已初始化完成
- 配置项 CONFIG_RADAR_PLT_SUB_6G 已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| time | uint16_t | 雷达退出延迟时间 | 大于0 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 延迟时间设置成功 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 执行失败 |

**参考案例**

- [radar_softap_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/softap_sample/radar_softap_sample.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_PLT_SUB_6G | 特性宏 | 支持雷达 Sub_6G 接口功能 | n |

### uapi_radar_get_delay_time <a id="uapi_radar_get_delay_time"></a>

```c
errcode_t uapi_radar_get_delay_time(uint16_t *time)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_sub_6g.h"
```

**功能说明**

- 获取雷达无人档位退出延迟时间
- 查询当前配置的雷达退出延迟时间
- 调用后通过出参返回延迟时间

**前置条件**

- 雷达模块已初始化完成
- 配置项 CONFIG_RADAR_PLT_SUB_6G 已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| time | uint16_t* | 雷达退出延迟时间输出指针 | 非空指针 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| *time | uint16_t | 雷达退出延迟时间 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 延迟时间获取成功 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 执行失败 |

**参考案例**

- [radar_softap_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/softap_sample/radar_softap_sample.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_PLT_SUB_6G | 特性宏 | 支持雷达 Sub_6G 接口功能 | n |

### uapi_radar_get_isolation <a id="uapi_radar_get_isolation"></a>

```c
errcode_t uapi_radar_get_isolation(uint16_t *iso)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_sub_6g.h"
```

**功能说明**

- 获取天线隔离度信息
- 查询雷达天线之间的隔离度参数
- 调用后通过出参返回隔离度信息

**前置条件**

- 雷达模块已初始化完成
- 配置项 CONFIG_RADAR_PLT_SUB_6G 已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| iso | uint16_t* | 天线隔离度信息输出指针 | 非空指针 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| *iso | uint16_t | 天线隔离度信息 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 隔离度信息获取成功 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 执行失败 |

**参考案例**

- [radar_softap_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/softap_sample/radar_softap_sample.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_PLT_SUB_6G | 特性宏 | 支持雷达 Sub_6G 接口功能 | n |

### uapi_radar_set_debug_para <a id="uapi_radar_set_debug_para"></a>

```c
errcode_t uapi_radar_set_debug_para(radar_dbg_para_t *para)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_sub_6g.h"
```

**功能说明**

- 设置雷达维测参数，包括子帧发送次数、循环次数、天线选择、波形选择、维测信息输出选择、子帧间隔等
- 用于雷达调试场景下配置维测参数，控制维测数据输出行为
- 重复调用以最后一次设置为准

**前置条件**

- 雷达模块已初始化完成
- 配置项 CONFIG_RADAR_PLT_SUB_6G 已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| para | [radar_dbg_para_t](#struct_radar_dbg_para_t)* | 雷达维测参数指针 | 非空指针，指向已分配的内存空间 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 维测参数设置成功 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 执行失败 |

**参考案例**

- [radar_softap_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/softap_sample/radar_softap_sample.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_PLT_SUB_6G | 特性宏 | 支持雷达 Sub_6G 接口功能 | n |

### uapi_radar_select_alg_para <a id="uapi_radar_select_alg_para"></a>

```c
errcode_t uapi_radar_select_alg_para(radar_sel_para_t *para)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_sub_6g.h"
```

**功能说明**

- 设置雷达算法参数套选择参数，包括模组安装架高、场景类型、遮挡材料类型、融合跟踪开关、AI (Artificial Intelligence) 融合开关
- 通过选择不同的算法参数套适配不同安装场景和环境条件
- 重复调用以最后一次设置为准

**前置条件**

- 雷达模块已初始化完成
- 配置项 CONFIG_RADAR_PLT_SUB_6G 已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| para | [radar_sel_para_t](#struct_radar_sel_para_t)* | 雷达算法参数套选择参数指针 | 非空指针，指向已分配的内存空间 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 算法参数套选择成功 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_PLT_SUB_6G | 特性宏 | 支持雷达 Sub_6G 接口功能 | n |

### uapi_radar_set_alg_para <a id="uapi_radar_set_alg_para"></a>

```c
errcode_t uapi_radar_set_alg_para(radar_alg_para_t *para, bool write_to_flash)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_sub_6g.h"
```

**功能说明**

- 设置雷达算法参数，包括各档位门限、跟踪门限、干扰抵抗参数、AI识别参数、点云参数等
- 支持选择是否将参数写入flash持久化保存，写入flash后断电不丢失
- 重复调用以最后一次设置为准

**前置条件**

- 雷达模块已初始化完成
- 配置项 CONFIG_RADAR_PLT_SUB_6G 已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| para | [radar_alg_para_t](#struct_radar_alg_para_t)* | 雷达算法参数指针 | 非空指针，指向已分配的内存空间 |
| write_to_flash | bool | 是否写入flash | true: 写入flash持久化保存; false: 仅运行时生效 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 算法参数设置成功 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 执行失败 |

**参考案例**

- [radar_slp_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/slp_sample/radar_slp_sample.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_PLT_SUB_6G | 特性宏 | 支持雷达 Sub_6G 接口功能 | n |

### uapi_radar_get_debug_info <a id="uapi_radar_get_debug_info"></a>

```c
errcode_t uapi_radar_get_debug_info(int16_t *arr, uint8_t len)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_sub_6g.h"
```

**功能说明**

- 查询雷达维测数据，包括静态维测和测量数据
- 通过数组存储维测数据，数组长度不能超过16
- 调用后通过出参返回维测数据

**前置条件**

- 雷达模块已初始化完成
- 配置项 CONFIG_RADAR_PLT_SUB_6G 已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| arr | int16_t* | 雷达维测数据存储数组地址 | 非空指针，指向已分配的内存空间 |
| len | uint8_t | 雷达维测数据存储数组长度 | 不超过16 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| *arr | int16_t | 雷达维测数据 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 维测数据获取成功 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 执行失败 |

**参考案例**

- [radar_softap_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/softap_sample/radar_softap_sample.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_PLT_SUB_6G | 特性宏 | 支持雷达 Sub_6G 接口功能 | n |

### uapi_radar_register_debug_info_cb <a id="uapi_radar_register_debug_info_cb"></a>

```c
errcode_t uapi_radar_register_debug_info_cb(radar_debug_info_cb_t cb, uint16_t period)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_sub_6g.h"
```

**功能说明**

- 注册雷达维测数据回调函数，雷达维测数据收集完成时触发回调
- 回调函数运行于 radar feature 线程，不能阻塞或长时间等待，不能使用较大栈空间
- 支持配置维测数据统计时长，单位为分钟

**前置条件**

- 雷达模块已初始化完成
- 配置项 CONFIG_RADAR_PLT_SUB_6G 已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| cb | [radar_debug_info_cb_t](#typedef_radar_debug_info_cb_t) | 雷达维测数据回调函数 | 非空函数指针 |
| period | uint16_t | 雷达维测数据统计时长，单位: 分钟 | 范围: 1~1440 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 回调注册成功 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 执行失败 |

**参考案例**

- [radar_softap_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/softap_sample/radar_softap_sample.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_PLT_SUB_6G | 特性宏 | 支持雷达 Sub_6G 接口功能 | n |

### uapi_radar_register_channel_switch_cb <a id="uapi_radar_register_channel_switch_cb"></a>

```c
errcode_t uapi_radar_register_channel_switch_cb(radar_channel_switch_cb_t cb, uint16_t *arr, uint8_t len)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_sub_6g.h"
```

**功能说明**

- 注册雷达信道切换回调函数，雷达底层判断需要切换信道时触发回调
- 回调函数运行于 radar driver 线程，不能阻塞或长时间等待，不能使用较大栈空间
- 支持配置信道切换参数，包括是否开启自适应切换、帧间隔门限、帧数占比等

**前置条件**

- 雷达模块已初始化完成
- 配置项 CONFIG_RADAR_PLT_SUB_6G 已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| cb | [radar_channel_switch_cb_t](#typedef_radar_channel_switch_cb_t) | 雷达信道切换回调函数 | 非空函数指针 |
| arr | uint16_t* | 雷达底层信道切换参数存储数组地址 | 非空指针，参数依次为: 是否开启底层信道自适应切换、极异常帧间隔门限(ms)、均值异常帧间隔门限(ms)、超过极异常帧间隔门限的帧数占比(扩大10倍) |
| len | uint8_t | 雷达信道切换参数存储数组长度 | 不超过 8 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 回调注册成功 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_PLT_SUB_6G | 特性宏 | 支持雷达 Sub_6G 接口功能 | n |

### uapi_radar_set_mwo_mode_para <a id="uapi_radar_set_mwo_mode_para"></a>

```c
errcode_t uapi_radar_set_mwo_mode_para(uint16_t *arr, uint8_t len, bool write_to_flash)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service_sub_6g.h"
```

**功能说明**

- 设置雷达微波模式检测参数，包括STA/SOFTAP模式下的子帧剔除梯度门限、MWO模式识别参数、超时时间等
- 支持选择是否将参数写入flash持久化保存
- 重复调用以最后一次设置为准

**前置条件**

- 雷达模块已初始化完成
- 配置项 CONFIG_RADAR_PLT_SUB_6G 已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| arr | uint16_t* | 参数设置数组地址 | 非空指针，参数依次为: STA (Station) 模式子帧剔除梯度门限(100~1000)、STA模式MWO识别梯度门限(100~1000)、SOFTAP模式子帧剔除梯度门限(100~1000)、SOFTAP模式MWO识别梯度门限(100~1000)、MWO识别子帧窗长(5~32)、MWO识别子帧选择门限(1~32)、MWO识别超时时间(min, 0表示关闭)、MWO模式距离门限扩大系数(扩大10倍) |
| len | uint8_t | 参数设置数组长度 | 不超过 8 |
| write_to_flash | bool | 是否写入flash | true: 写入flash持久化保存; false: 仅运行时生效 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 参数设置成功 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_PLT_SUB_6G | 特性宏 | 支持雷达 Sub_6G 接口功能 | n |

## Type definitions

### radar_result_cb_t <a id="typedef_radar_result_cb_t"></a>

```c
typedef void (*radar_result_cb_t)(radar_result_t *result);
```

**使用说明**

用于[uapi_radar_register_result_cb](#uapi_radar_register_result_cb)接口入参，雷达检测结果变化时触发回调。运行于 radar feature 线程，不能阻塞或长时间等待，不能使用较大栈空间。

### radar_current_frame_result_cb_t <a id="typedef_radar_current_frame_result_cb_t"></a>

```c
typedef void (*radar_current_frame_result_cb_t)(radar_current_frame_result_t *result);
```

**使用说明**

用于[uapi_radar_register_current_frame_result_cb](#uapi_radar_register_current_frame_result_cb)接口入参，每一个雷达帧计算完成后触发回调。运行于 radar feature 线程，不能阻塞或长时间等待，不能使用较大栈空间。

### radar_raw_data_cb_t <a id="typedef_radar_raw_data_cb_t"></a>

```c
typedef void (*radar_raw_data_cb_t)(radar_raw_data_t *raw_data);
```

**使用说明**

用于[uapi_radar_register_raw_data_cb](#uapi_radar_register_raw_data_cb)接口入参，雷达回波接收完成时触发回调。运行于 radar driver 线程，不能阻塞或长时间等待，不能使用较大栈空间。

### radar_debug_info_cb_t <a id="typedef_radar_debug_info_cb_t"></a>

```c
typedef void (*radar_debug_info_cb_t)(int16_t *arr, uint8_t len);
```

**使用说明**

用于[uapi_radar_register_debug_info_cb](#uapi_radar_register_debug_info_cb)接口入参，雷达维测数据收集完成时触发回调。运行于 radar feature 线程，不能阻塞或长时间等待，不能使用较大栈空间。维测信息依次为: 是否写入flash、LNA (Low Noise Amplifier)*10+VGA (Variable Gain Amplifier)、原始回波峰值、平均MO1底噪、平均MO2底噪、平均DP底噪、平均帧间隔、帧间隔超过Xms的帧数、bitmap数量超过门限的帧数、bitmap比例超过门限的帧数、实际参与统计的帧数、帧间隔最大值、帧间隔最大值下标、算法参数MO1门限、MO2门限、DP门限。

### radar_channel_switch_cb_t <a id="typedef_radar_channel_switch_cb_t"></a>

```c
typedef void (*radar_channel_switch_cb_t)(void);
```

**使用说明**

用于[uapi_radar_register_channel_switch_cb](#uapi_radar_register_channel_switch_cb)接口入参，雷达底层判断需要切换信道时触发回调。运行于 radar driver 线程，不能阻塞或长时间等待，不能使用较大栈空间。

## Enumerations

### radar_set_sts_t <a id="enum_radar_set_sts_t"></a>

```c
typedef enum {
    RADAR_STATUS_STOP = 0,
    RADAR_STATUS_START,
    RADAR_STATUS_RESET,
    RADAR_STATUS_RESUME,
} radar_set_sts_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| RADAR_STATUS_STOP | 0 | 雷达状态配置停止 |
| RADAR_STATUS_START | 1 | 雷达状态配置启动 |
| RADAR_STATUS_RESET | 2 | 雷达状态配置复位 |
| RADAR_STATUS_RESUME | 3 | 雷达状态配置恢复 |

### radar_get_sts_t <a id="enum_radar_get_sts_t"></a>

```c
typedef enum {
    RADAR_STATUS_IDLE = 0,
    RADAR_STATUS_RUNNING,
} radar_get_sts_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| RADAR_STATUS_IDLE | 0 | 雷达状态未工作 |
| RADAR_STATUS_RUNNING | 1 | 雷达状态工作 |

### radar_get_hardware_sts_t <a id="enum_radar_get_hardware_sts_t"></a>

```c
typedef enum {
    RADAR_STATUS_HW_FAULT = 0,
    RADAR_STATUS_HW_NORMAL,
} radar_get_hardware_sts_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| RADAR_STATUS_HW_FAULT | 0 | 雷达硬件状态故障 |
| RADAR_STATUS_HW_NORMAL | 1 | 雷达硬件状态正常 |

### radar_dbg_type_t <a id="enum_radar_dbg_type_t"></a>

```c
typedef enum {
    RADAR_DBG_NO_DATA_RPT = 0,
    RADAR_DBG_PC_DATA_RPT,
    RADAR_DBG_ADC_DATA_RPT,
    RADAR_DBG_RES_RPT,
    RADAR_DBG_UART0_RES_RPT,
    RADAR_DBG_RPT_BUTT,
} radar_dbg_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| RADAR_DBG_NO_DATA_RPT | 0 | 无数据上报 |
| RADAR_DBG_PC_DATA_RPT | 1 | 通过uart1上报脉压后的数据 |
| RADAR_DBG_ADC_DATA_RPT | 2 | 通过uart1上报adc数据 |
| RADAR_DBG_RES_RPT | 3 | 通过uart1上报result |
| RADAR_DBG_UART0_RES_RPT | 4 | 通过uart0上报result |
| RADAR_DBG_RPT_BUTT | 5 | 维测数据类型上限 |

### radar_height_type_t <a id="enum_radar_height_type_t"></a>

```c
typedef enum {
    RADAR_HEIGHT_1M,
    RADAR_HEIGHT_2M,
    RADAR_HEIGHT_3M,
    RADAR_HEIGHT_BUTT,
} radar_height_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| RADAR_HEIGHT_1M | 0 | 0到1.5米 |
| RADAR_HEIGHT_2M | 1 | 1.5米到2.5米 |
| RADAR_HEIGHT_3M | 2 | 2.5米以上 |
| RADAR_HEIGHT_BUTT | 3 | 架高类型上限 |

### radar_scenario_type_t <a id="enum_radar_scenario_type_t"></a>

```c
typedef enum {
    RADAR_SCENARIO_TYPE_HOME,
    RADAR_SCENARIO_TYPE_HALL,
    RADAR_SCENARIO_TYPE_BUTT,
} radar_scenario_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| RADAR_SCENARIO_TYPE_HOME | 0 | 家居场景 |
| RADAR_SCENARIO_TYPE_HALL | 1 | 空旷场景(面积大于50平方米或层高高于3.5米) |
| RADAR_SCENARIO_TYPE_BUTT | 2 | 场景类型上限 |

### radar_material_type_t <a id="enum_radar_material_type_t"></a>

```c
typedef enum {
    RADAR_MATERIAL_PLATSIC,
    RADAR_MATERIAL_PCB,
    RADAR_MATERIAL_SINGLE,
    RADAR_MATERIAL_BUTT,
} radar_material_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| RADAR_MATERIAL_PLATSIC | 0 | 塑料或无遮挡物 |
| RADAR_MATERIAL_PCB | 1 | PCB (Printed Circuit Board) 或金属 |
| RADAR_MATERIAL_SINGLE | 2 | 单模组 |
| RADAR_MATERIAL_BUTT | 3 | 遮挡材料类型上限 |

### errcode_radar_client_t <a id="enum_errcode_radar_client_t"></a>

```c
typedef enum {
    ERRCODE_RC_SUCCESS = 0,
    ERRCODE_RC_MALLOC_FAILED = 0x8000A100,
    ERRCODE_RC_QUEUE_WRITE_FAILED,
    ERRCODE_RC_POWER_ON_FAILED,
    ERRCODE_RC_POWERED_ON,
    ERRCODE_RC_POWERED_OFF,
    ERRCODE_RC_RADAR_ENABLED,
    ERRCODE_RC_RADAR_DISABLED,
    ERRCODE_RC_PTR_NULL,
    ERRCODE_RC_PARA_INVALID,
    ERRCODE_RC_STATUS_ERROR = 0x8000A11E,
    ERRCODE_RC_CALL_BACK,
    ERRCODE_RC_CHANNEL_IDX,
    ERRCODE_RC_GPIO_PINMUX,
    ERRCODE_RC_ANT_CHANNEL_NUM,
    ERRCODE_RC_ANT_CODE,
    ERRCODE_RC_TX_POWER,
    ERRCODE_RC_AGC_LNA,
    ERRCODE_RC_AGC_VGA,
    ERRCODE_RC_ANT_SW_CTRL,
    ERRCODE_RC_SUBFRAME_PERIOD,
    ERRCODE_RC_ANT_SWITCH_INTERVAL,
    ERRCODE_RC_START_BIN,
    ERRCODE_RC_END_BIN,
    ERRCODE_RC_TX_WAVE_SPREAD_FACTOR,
    ERRCODE_RC_TX_WAVE_ACC_RSHIFT_BIT,
    ERRCODE_RC_TX_WAVE_ACC_NUM,
    ERRCODE_RC_TX_WAVE_CORR_DIV,
    ERRCODE_RC_TX_WAVE_TX_WAVE_LEN,
    ERRCODE_RC_TX_WAVE_TX_WAVE_BITS,
    ERRCODE_RC_RADAR_VERSION_NOT_INIT,
    ERRCODE_RC_START_UP = 0x8000A183,
    ERRCODE_RC_CLOSE,
    ERRCODE_RC_FRAME_BUFFER_FULL,
    ERRCODE_RC_RAINGING_DURATION,
    ERRCODE_RC_TX_AHEAD_READ_ADC,
    ERRCODE_RC_AGC_CALI_FAILED,
    ERRCODE_RC_PC_PEAK_OVERFLOWED,
    ERRCODE_RC_EEPROM_READ_FAILED,
    ERRCODE_RC_IMAGE_CHECK_FAILED,
    ERRCODE_RC_MAX = 0x8000A1FF,
} errcode_radar_client_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| ERRCODE_RC_SUCCESS | 0 | 执行成功 |
| ERRCODE_RC_MALLOC_FAILED | 0x8000A100 | 申请动态内存失败 |
| ERRCODE_RC_QUEUE_WRITE_FAILED | 0x8000A101 | 消息队列写入失败 |
| ERRCODE_RC_POWER_ON_FAILED | 0x8000A102 | 上电加载失败 |
| ERRCODE_RC_POWERED_ON | 0x8000A103 | 已经上电加载 |
| ERRCODE_RC_POWERED_OFF | 0x8000A104 | 已经下电 |
| ERRCODE_RC_RADAR_ENABLED | 0x8000A105 | 雷达已经使能 |
| ERRCODE_RC_RADAR_DISABLED | 0x8000A106 | 雷达已经关闭 |
| ERRCODE_RC_PTR_NULL | 0x8000A107 | 雷达指针为空 |
| ERRCODE_RC_PARA_INVALID | 0x8000A108 | 雷达参数不合规 |
| ERRCODE_RC_STATUS_ERROR | 0x8000A11E | 启动参数异常 |
| ERRCODE_RC_CALL_BACK | 0x8000A11F | 回调注册异常 |
| ERRCODE_RC_CHANNEL_IDX | 0x8000A120 | 信道索引 |
| ERRCODE_RC_GPIO_PINMUX | 0x8000A121 | 开关GPIO (General Purpose Input/Output) PINMUX (Pin Multiplexing) 配置 |
| ERRCODE_RC_ANT_CHANNEL_NUM | 0x8000A122 | 天线对有效值可控 |
| ERRCODE_RC_ANT_CODE | 0x8000A123 | 天线码字可配 |
| ERRCODE_RC_TX_POWER | 0x8000A124 | 发射功率 |
| ERRCODE_RC_AGC_LNA | 0x8000A125 | AGC (Automatic Gain Control) 增益配置LNA |
| ERRCODE_RC_AGC_VGA | 0x8000A126 | AGC增益配置VGA |
| ERRCODE_RC_ANT_SW_CTRL | 0x8000A127 | 射频开关控制管脚 |
| ERRCODE_RC_SUBFRAME_PERIOD | 0x8000A128 | 子帧间隔 |
| ERRCODE_RC_ANT_SWITCH_INTERVAL | 0x8000A129 | 雷达天线对切换间隔 |
| ERRCODE_RC_START_BIN | 0x8000A12A | 雷达原始数据参数：起始bin索引 |
| ERRCODE_RC_END_BIN | 0x8000A12B | 雷达原始数据参数：结束bin索引 |
| ERRCODE_RC_TX_WAVE_SPREAD_FACTOR | 0x8000A12C | 雷达波形参数：波形序列扩频因子 |
| ERRCODE_RC_TX_WAVE_ACC_RSHIFT_BIT | 0x8000A12D | 雷达波形参数：累加位宽保护右移位数 |
| ERRCODE_RC_TX_WAVE_ACC_NUM | 0x8000A12E | 雷达波形参数：累加次数 |
| ERRCODE_RC_TX_WAVE_CORR_DIV | 0x8000A12F | 雷达波形参数：相关后除数因子 |
| ERRCODE_RC_TX_WAVE_TX_WAVE_LEN | 0x8000A130 | 雷达波形参数：发射波形长度 |
| ERRCODE_RC_TX_WAVE_TX_WAVE_BITS | 0x8000A131 | 雷达波形参数：波形扩频前的三元码序列 |
| ERRCODE_RC_RADAR_VERSION_NOT_INIT | 0x8000A132 | 雷达版本未初始化 |
| ERRCODE_RC_START_UP | 0x8000A183 | 启动异常 |
| ERRCODE_RC_CLOSE | 0x8000A184 | 关闭异常 |
| ERRCODE_RC_FRAME_BUFFER_FULL | 0x8000A185 | 上报数据缓冲满，bufferfull，丢帧 |
| ERRCODE_RC_RAINGING_DURATION | 0x8000A186 | 共存指向间隔异常 |
| ERRCODE_RC_TX_AHEAD_READ_ADC | 0x8000A187 | 雷达TX (Transmit) 发送在读取ADC (Analog-to-Digital Converter)数据前 |
| ERRCODE_RC_AGC_CALI_FAILED | 0x8000A188 | AGC校准异常 |
| ERRCODE_RC_PC_PEAK_OVERFLOWED | 0x8000A189 | 雷达脉压峰值溢出 |
| ERRCODE_RC_EEPROM_READ_FAILED | 0x8000A18A | 雷达从eeprom中读取校准结果时发生错误 |
| ERRCODE_RC_IMAGE_CHECK_FAILED | 0x8000A18B | 雷达固件镜像校验错误 |
| ERRCODE_RC_MAX | 0x8000A1FF | RC错误码最大值 |

## Structures

### radar_result_t <a id="struct_radar_result_t"></a>

```c
typedef struct {
    uint32_t lower_boundary;
    uint32_t upper_boundary;
    uint8_t is_human_presence;
    uint8_t reserved_0;
    uint8_t reserved_1;
    uint8_t reserved_2;
} radar_result_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| lower_boundary | uint32_t | 雷达结果靠近检测下边界 |
| upper_boundary | uint32_t | 雷达结果靠近检测上边界 |
| is_human_presence | uint8_t | 雷达结果有无人体存在 |
| reserved_0 | uint8_t | 保留字段 |
| reserved_1 | uint8_t | 保留字段 |
| reserved_2 | uint8_t | 保留字段 |

### radar_current_frame_result_t <a id="struct_radar_current_frame_result_t"></a>

```c
typedef struct {
    uint32_t gear;
    int32_t rng;
    int32_t vel;
    float snr;
} radar_current_frame_result_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| gear | uint32_t | 雷达当前帧结果，所处档位信息 |
| rng | int32_t | 雷达当前帧结果，距离测量值信息 |
| vel | int32_t | 雷达当前帧结果，速度测量值信息 |
| snr | float | 雷达当前帧结果，信噪比信息 |

### radar_raw_data_t <a id="struct_radar_raw_data_t"></a>

```c
typedef struct {
    int32_t data[RADAR_RAW_DATA_NUM];
} radar_raw_data_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| data | int32_t[40] | 雷达原始数据，IQ交替存储 |

### radar_dbg_para_t <a id="struct_radar_dbg_para_t"></a>

```c
typedef struct {
    uint8_t times;
    uint8_t loop;
    uint8_t ant;
    uint8_t wave;
    uint8_t dbg_type;
    uint16_t period;
} radar_dbg_para_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| times | uint8_t | 子帧发送次数, 默认值0(一直发送), 范围0~20 |
| loop | uint8_t | 单个子帧雷达波形循环发送次数, 默认值为8 |
| ant | uint8_t | 接收通路选择, 默认值为0 |
| wave | uint8_t | 雷达发射波形类型选择, 默认值为2 |
| dbg_type | uint8_t | 维测信息输出选择, 默认值为0, 范围0~4, 参考[radar_dbg_type_t](#enum_radar_dbg_type_t) |
| period | uint16_t | 雷达子帧间隔, 单位us, 默认值为5000, 范围3000~100000 |

### radar_sel_para_t <a id="struct_radar_sel_para_t"></a>

```c
typedef struct {
    uint8_t height;
    uint8_t scenario;
    uint8_t material;
    uint8_t fusion_track;
    uint8_t fusion_ai;
} radar_sel_para_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| height | uint8_t | 模组安装架高信息: 1/2/3米, 参考[radar_height_type_t](#enum_radar_height_type_t) |
| scenario | uint8_t | 场景: 家居/空旷, 参考[radar_scenario_type_t](#enum_radar_scenario_type_t) |
| material | uint8_t | 模组视距方向遮挡材料: 塑料/金属, 参考[radar_material_type_t](#enum_radar_material_type_t) |
| fusion_track | uint8_t | 是否融合距离跟踪结果 |
| fusion_ai | uint8_t | 是否融合AI结果 |

### radar_alg_para_t <a id="struct_radar_alg_para_t"></a>

```c
typedef struct {
    uint8_t d_th_1m;
    uint8_t d_th_2m;
    uint8_t p_th;
    uint8_t t_th_1m;
    uint8_t t_th_2m;
    uint8_t b_th_ratio;
    uint8_t b_th_cnt;
    uint8_t a_th;
    uint8_t pt_cld_para_1;
    uint8_t pt_cld_para_2;
    uint8_t pt_cld_para_3;
    uint8_t pt_cld_para_4;
    uint8_t rd_pwr_para_1;
    uint8_t rd_pwr_para_2;
    uint8_t rd_pwr_para_3;
} radar_alg_para_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| d_th_1m | uint8_t | 靠近1米档门限, 范围0-99 |
| d_th_2m | uint8_t | 靠近2米档门限, 范围0-99 |
| p_th | uint8_t | 存在6米档门限, 范围0-99 |
| t_th_1m | uint8_t | 距离跟踪1米档门限, 单位:分米, 范围0-30 |
| t_th_2m | uint8_t | 距离跟踪2米档门限, 单位:分米, 范围0-30 |
| b_th_ratio | uint8_t | 抗频谱对称干扰比例门限, 范围0-99 |
| b_th_cnt | uint8_t | 抗频谱对称干扰数量门限, 范围0-99 |
| a_th | uint8_t | AI人体识别相似度门限, 范围0-99 |
| pt_cld_para_1 | uint8_t | 0~1米范围点云门限, 范围0-20 |
| pt_cld_para_2 | uint8_t | 1~2米范围点云门限, 范围0-20 |
| pt_cld_para_3 | uint8_t | 2~5米范围点云门限, 范围0-20 |
| pt_cld_para_4 | uint8_t | 5米以上范围点云门限, 范围0-20 |
| rd_pwr_para_1 | uint8_t | 0~1米范围点云门限, 范围1-100 |
| rd_pwr_para_2 | uint8_t | 1~2米范围点云门限, 范围1-100 |
| rd_pwr_para_3 | uint8_t | 2~6米范围点云门限, 范围1-100 |




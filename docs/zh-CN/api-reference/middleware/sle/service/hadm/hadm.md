# SLE HADM

HADM (High Accuracy Distance Measurement) 高精度测距 API。利用 SLE (SparkLink Low Energy) 物理层信号测量技术估算设备间的距离，无需额外硬件。管理测距请求/响应、距离结果获取和测量参数配置。

适用定位标签、室内导航、接近检测等需精确距离信息的场景。

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_hadm_manager.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [sle_read_local_channel_sounding_caps](#sle_read_local_channel_sounding_caps) | 读取本端channel sounding能力 |
| [sle_read_remote_channel_sounding_caps](#sle_read_remote_channel_sounding_caps) | 读取对端channel sounding能力 |
| [sle_set_channel_sounding_param_ex](#sle_set_channel_sounding_param_ex) | 设置channel sounding参数 |
| [sle_set_channel_sounding_enable](#sle_set_channel_sounding_enable) | 设置channel sounding使能 |
| [sle_set_channel_sounding_disable](#sle_set_channel_sounding_disable) | 设置channel sounding关闭 |
| [sle_hadm_register_callbacks](#sle_hadm_register_callbacks) | 注册SLE hadm管理回调函数 |

## Functions

### sle_read_local_channel_sounding_caps <a id="sle_read_local_channel_sounding_caps"></a>

```c
errcode_t sle_read_local_channel_sounding_caps(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_hadm_manager.h"
```

**功能说明**

- 读取本端channel sounding能力集
- 读取结果通过`sle_read_local_channel_sounding_callback`回调异步返回
- 调用后需等待回调获取能力集数据，不可在回调中阻塞或长时间等待

**前置条件**

- SLE协议栈已初始化完成
- 已通过[sle_hadm_register_callbacks](#sle_hadm_register_callbacks)注册回调函数，且read_local_cs_caps_cb已赋值

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_FEATURE_GLE_HADM | 特性宏 | 支持SLE HADM channel sounding功能 | n |

### sle_read_remote_channel_sounding_caps <a id="sle_read_remote_channel_sounding_caps"></a>

```c
errcode_t sle_read_remote_channel_sounding_caps(uint16_t conn_id)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_hadm_manager.h"
```

**功能说明**

- 读取对端设备channel sounding能力集
- 读取结果通过`sle_read_remote_channel_sounding_callback`回调异步返回
- 调用后需等待回调获取能力集数据，不可在回调中阻塞或长时间等待

**前置条件**

- SLE协议栈已初始化完成
- 与对端设备已建立SLE连接，conn_id为有效连接ID
- 已通过[sle_hadm_register_callbacks](#sle_hadm_register_callbacks)注册回调函数，且read_remote_cs_caps_cb已赋值

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| conn_id | uint16_t | 连接ID | SLE有效连接ID |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_FEATURE_GLE_HADM | 特性宏 | 支持SLE HADM channel sounding功能 | n |

### sle_set_channel_sounding_param_ex <a id="sle_set_channel_sounding_param_ex"></a>

```c
errcode_t sle_set_channel_sounding_param_ex(uint16_t conn_id, sle_set_channel_sounding_param_ex_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_hadm_manager.h"
```

**功能说明**

- 设置channel sounding参数，包括频率间隔、锚点数量、刷新率、测距周期等
- 参数设置后影响后续channel sounding的测距行为
- param指针由调用者管理，接口内部不释放

**前置条件**

- SLE协议栈已初始化完成
- 与对端设备已建立SLE连接，conn_id为有效连接ID
- param指针不为NULL，且指向内存空间已申请成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| conn_id | uint16_t | 连接ID | SLE有效连接ID |
| param | [sle_set_channel_sounding_param_ex_t](#struct_sle_set_channel_sounding_param_ex_t) * | channel sounding参数结构体指针 | 不为NULL |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 参数设置成功 |
| Other | 其他错误码，参考errcode_t | 参数设置失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_FEATURE_GLE_HADM | 特性宏 | 支持SLE HADM channel sounding功能 | n |

### sle_set_channel_sounding_enable <a id="sle_set_channel_sounding_enable"></a>

```c
errcode_t sle_set_channel_sounding_enable(uint16_t conn_id)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_hadm_manager.h"
```

**功能说明**

- 使能channel sounding功能
- 使能结果通过`sle_channel_sounding_state_changed_callback`回调异步返回
- 调用后需等待回调确认使能状态，不可在回调中阻塞或长时间等待

**前置条件**

- SLE协议栈已初始化完成
- 与对端设备已建立SLE连接，conn_id为有效连接ID
- 已通过[sle_hadm_register_callbacks](#sle_hadm_register_callbacks)注册回调函数，且cs_state_changed_cb已赋值
- 已通过[sle_set_channel_sounding_param_ex](#sle_set_channel_sounding_param_ex)完成参数设置

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| conn_id | uint16_t | 连接ID | SLE有效连接ID |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 使能请求成功发起 |
| Other | 其他错误码，参考errcode_t | 使能请求失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_FEATURE_GLE_HADM | 特性宏 | 支持SLE HADM channel sounding功能 | n |

### sle_set_channel_sounding_disable <a id="sle_set_channel_sounding_disable"></a>

```c
errcode_t sle_set_channel_sounding_disable(uint16_t conn_id)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_hadm_manager.h"
```

**功能说明**

- 关闭channel sounding功能
- 关闭结果通过`sle_channel_sounding_state_changed_callback`回调异步返回
- 调用后需等待回调确认关闭状态，不可在回调中阻塞或长时间等待

**前置条件**

- SLE协议栈已初始化完成
- 与对端设备已建立SLE连接，conn_id为有效连接ID
- 已通过[sle_hadm_register_callbacks](#sle_hadm_register_callbacks)注册回调函数，且cs_state_changed_cb已赋值
- channel sounding已处于使能状态

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| conn_id | uint16_t | 连接ID | SLE有效连接ID |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 关闭请求成功发起 |
| Other | 其他错误码，参考errcode_t | 关闭请求失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_FEATURE_GLE_HADM | 特性宏 | 支持SLE HADM channel sounding功能 | n |

### sle_hadm_register_callbacks <a id="sle_hadm_register_callbacks"></a>

```c
errcode_t sle_hadm_register_callbacks(sle_hadm_callbacks_t *func)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_hadm_manager.h"
```

**功能说明**

- 注册SLE hadm管理回调函数，包括本端/对端能力读取回调、状态变化回调、IQ上报回调
- 回调函数运行于SLE service线程，不可阻塞或长时间等待
- 回调中指针由SLE service申请和释放，回调中不应释放

**前置条件**

- SLE协议栈已初始化完成
- func指针不为NULL，且指向内存空间已申请成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func | [sle_hadm_callbacks_t](#struct_sle_hadm_callbacks_t) * | 回调函数结构体指针 | 不为NULL |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 回调注册成功 |
| Other | 其他错误码，参考errcode_t | 回调注册失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_FEATURE_GLE_HADM | 特性宏 | 支持SLE HADM channel sounding功能 | n |

## Enumerations

### sle_channel_sounding_state_t <a id="enum_sle_channel_sounding_state_t"></a>

```c
typedef enum {
    SLE_CHANNEL_SOUNDING_DISABLE,
    SLE_CHANNEL_SOUNDING_ENABLE,
    SLE_CHANNEL_SOUNDING_MAX,
} sle_channel_sounding_state_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SLE_CHANNEL_SOUNDING_DISABLE | 0 | channel sounding关闭 |
| SLE_CHANNEL_SOUNDING_ENABLE | 1 | channel sounding使能 |
| SLE_CHANNEL_SOUNDING_MAX | 2 | 枚举边界值 |

## Structures

### sle_set_channel_sounding_param_ex_t <a id="struct_sle_set_channel_sounding_param_ex_t"></a>

```c
typedef struct {
    uint8_t  is_cs_param_chg; /*!< 是否需要更改测距连接参数 */
    uint8_t  freq_space;      /*!< 频率间隔 */
    uint8_t  con_anchor_num;  /*!< 需要连接的锚点数量 */
    uint8_t  refresh_rate;    /*!< 刷新率 */
    uint16_t acb_interval;    /*!< 根据参数计算得到的acb链路周期 */
    uint16_t cs_interval;     /*!< 根据参数计算得到的测距周期 */
    uint16_t posalg_freq;     /*!< 算法频率 */
    uint8_t  glp_mode;        /*!< 2.4GHz 模式 */
} sle_set_channel_sounding_param_ex_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| is_cs_param_chg | uint8_t | 是否需要更改测距连接参数 |
| freq_space | uint8_t | 频率间隔 |
| con_anchor_num | uint8_t | 需要连接的锚点数量 |
| refresh_rate | uint8_t | 刷新率 |
| acb_interval | uint16_t | 根据参数计算得到的acb链路周期 |
| cs_interval | uint16_t | 根据参数计算得到的测距周期 |
| posalg_freq | uint16_t | 算法频率 |
| glp_mode | uint8_t | 2.4GHz 模式 |

### sle_channel_sounding_iq_report_t <a id="struct_sle_channel_sounding_iq_report_t"></a>

```c
typedef struct {
    uint8_t samp_cnt;
    uint8_t report_idx;
    uint16_t es_sn;
    uint32_t timestamp_sn; // 对齐master的时间
    uint8_t rssi[SLE_CS_IQ_REPORT_COUNT];
    uint8_t freq[SLE_CS_IQ_REPORT_COUNT];
    uint16_t i_data[SLE_CS_IQ_REPORT_COUNT];
    uint16_t q_data[SLE_CS_IQ_REPORT_COUNT];
    uint32_t tof_result;
} sle_channel_sounding_iq_report_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| samp_cnt | uint8_t | 采样计数 |
| report_idx | uint8_t | 上报索引 |
| es_sn | uint16_t | 序列号 |
| timestamp_sn | uint32_t | 对齐master的时间戳 |
| rssi | uint8_t[20] | RSSI (Received Signal Strength Indicator) 数据数组 |
| freq | uint8_t[20] | 频率数据数组 |
| i_data | uint16_t[20] | I路数据数组 |
| q_data | uint16_t[20] | Q路数据数组 |
| tof_result | uint32_t | 飞行时间测量结果 |

### sle_channel_sounding_caps_t <a id="struct_sle_channel_sounding_caps_t"></a>

```c
typedef struct {
    uint8_t caps[SLE_CS_CAPS_LEN]; /*!< channel sounding 能力集 */
} sle_channel_sounding_caps_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| caps | uint8_t[0x15] | channel sounding能力集 |

### sle_hadm_callbacks_t <a id="struct_sle_hadm_callbacks_t"></a>

```c
typedef struct {
    sle_read_local_channel_sounding_callback read_local_cs_caps_cb;   /*!< 读取本端channel sounding回调函数 */
    sle_read_remote_channel_sounding_callback read_remote_cs_caps_cb; /*!< 读取对端channel sounding回调函数 */
    sle_channel_sounding_state_changed_callback cs_state_changed_cb;  /*!< channel sounding状态改变回调函数 */
    sle_channel_sounding_iq_report_callback cs_iq_report_cb;          /*!< channel sounding iq上报回调函数 */
} sle_hadm_callbacks_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| read_local_cs_caps_cb | `sle_read_local_channel_sounding_callback` | 读取本端channel sounding回调函数 |
| read_remote_cs_caps_cb | `sle_read_remote_channel_sounding_callback` | 读取对端channel sounding回调函数 |
| cs_state_changed_cb | `sle_channel_sounding_state_changed_callback` | channel sounding状态改变回调函数 |
| cs_iq_report_cb | `sle_channel_sounding_iq_report_callback` | channel sounding iq上报回调函数 |

## Type definitions

### sle_read_local_channel_sounding_callback <a id="typedef_sle_read_local_channel_sounding_callback"></a>

```c
typedef void (*sle_read_local_channel_sounding_callback)(sle_channel_sounding_caps_t *caps,
    errcode_t status);
```

**使用说明**

作为[sle_hadm_callbacks_t](#struct_sle_hadm_callbacks_t)的read_local_cs_caps_cb成员，在[sle_read_local_channel_sounding_caps](#sle_read_local_channel_sounding_caps)调用后异步返回本端能力集读取结果

### sle_read_remote_channel_sounding_callback <a id="typedef_sle_read_remote_channel_sounding_callback"></a>

```c
typedef void (*sle_read_remote_channel_sounding_callback)(uint16_t conn_id, sle_channel_sounding_caps_t *caps,
    errcode_t status);
```

**使用说明**

作为[sle_hadm_callbacks_t](#struct_sle_hadm_callbacks_t)的read_remote_cs_caps_cb成员，在[sle_read_remote_channel_sounding_caps](#sle_read_remote_channel_sounding_caps)调用后异步返回对端能力集读取结果

### sle_channel_sounding_state_changed_callback <a id="typedef_sle_channel_sounding_state_changed_callback"></a>

```c
typedef void (*sle_channel_sounding_state_changed_callback)(errcode_t status);
```

**使用说明**

作为[sle_hadm_callbacks_t](#struct_sle_hadm_callbacks_t)的cs_state_changed_cb成员，在[sle_set_channel_sounding_enable](#sle_set_channel_sounding_enable)或[sle_set_channel_sounding_disable](#sle_set_channel_sounding_disable)调用后异步返回状态变化结果

### sle_channel_sounding_iq_report_callback <a id="typedef_sle_channel_sounding_iq_report_callback"></a>

```c
typedef void (*sle_channel_sounding_iq_report_callback)(uint16_t conn_id, sle_channel_sounding_iq_report_t *report);
```

**使用说明**

作为[sle_hadm_callbacks_t](#struct_sle_hadm_callbacks_t)的cs_iq_report_cb成员，在channel sounding使能后异步上报IQ数据



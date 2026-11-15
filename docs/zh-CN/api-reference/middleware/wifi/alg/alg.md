# Wi-Fi ALG

ALG (Application Layer Gateway) 是 Wi-Fi 中间件提供的无线链路算法配置接口集合，用于对发送速率、发射功率控制 (TPC)、RTS (Request To Send) 、CCA (Clear Channel Assessment) 门限以及抗干扰等链路层参数进行设置与查询，从而根据应用场景优化 Wi-Fi 的传输性能与抗干扰能力。

**头文件清单**
```c
#include "include/middleware/services/wifi/wifi_alg.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [wifi_set_fixed_tx_rate](#wifi_set_fixed_tx_rate) | 设置TX (Transmit) 方向发送报文的速率模式 |
| [wifi_get_negotiated_rate](#wifi_get_negotiated_rate) | 获取指定用户当前的最优速率 |
| [wifi_set_tpc_mode](#wifi_set_tpc_mode) | 设置TPC (Transmit Power Control) 模式 |
| [wifi_set_rts_mode](#wifi_set_rts_mode) | 设置RTS模式 |
| [wifi_set_cca_threshold](#wifi_set_cca_threshold) | 设置CCA门限 |
| [wifi_set_intrf_mode](#wifi_set_intrf_mode) | 设置抗干扰模式 |
| [wifi_get_chan_intrf_info](#wifi_get_chan_intrf_info) | 获取信道干扰信息 |

## Functions

### wifi_set_fixed_tx_rate <a id="wifi_set_fixed_tx_rate"></a>

```c
errcode_t wifi_set_fixed_tx_rate(unsigned char auto_rate, alg_param_stru *alg_param)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_alg.h"
```

**功能说明**

- 设置TX方向发送报文的速率模式，支持固定速率或自动速率模式
- 通过auto_rate参数选择使用固定速率还是自动速率模式
- 通过alg_param指定固定速率等级值

**前置条件**

- WiFi模块已通过wifi_init()初始化完成，返回初始化成功状态
- alg_param指针不为NULL，且指向内存空间已申请成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| auto_rate | unsigned char | 速率模式选择 | 0: 固定速率模式; 1: 自动速率模式 |
| alg_param | [alg_param_stru](#struct_alg_param_stru) * | 速率等级参数 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 速率设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_get_negotiated_rate <a id="wifi_get_negotiated_rate"></a>

```c
errcode_t wifi_get_negotiated_rate(const uint8_t *mac, int32_t mac_len, uint32_t *tx_best_rate)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_alg.h"
```

**功能说明**

- 获取指定用户当前协商的最优速率
- 通过MAC (Media Access Control) 地址标识目标用户
- 获取的速率值通过tx_best_rate出参返回

**前置条件**

- WiFi模块已通过wifi_init()初始化完成，返回初始化成功状态
- mac指针不为NULL，且指向合法的MAC地址
- tx_best_rate指针不为NULL，且指向内存空间已申请成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| mac | const uint8_t * | 目标用户的MAC地址 | 非NULL |
| mac_len | int32_t | MAC地址长度 | - |
| tx_best_rate | uint32_t * | 获取的用户最佳速率 | 非NULL，出参 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| tx_best_rate | uint32_t * | 用户当前协商的最优速率值 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 速率获取成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_set_tpc_mode <a id="wifi_set_tpc_mode"></a>

```c
errcode_t wifi_set_tpc_mode(uint32_t tpc_value)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_alg.h"
```

**功能说明**

- 设置TPC（发射功率控制）模式
- 支持关闭动态调整、功率提升、自动调整三种模式
- 通过tpc_value参数指定TPC工作模式

**前置条件**

- WiFi模块已通过wifi_init()初始化完成，返回初始化成功状态

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| tpc_value | uint32_t | TPC模式设置值 | 0: 关闭动态调整; 1: 功率提升模式; 2: 自动调整模式 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | TPC模式设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_set_rts_mode <a id="wifi_set_rts_mode"></a>

```c
errcode_t wifi_set_rts_mode(uint8_t mode, uint16_t pkt_length)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_alg.h"
```

**功能说明**

- 设置RTS/CTS握手模式
- 支持自动模式、阈值模式、关闭模式三种模式
- 阈值模式下，报文长度超过pkt_length时发送RTS，未超过则不发送

**前置条件**

- WiFi模块已通过wifi_init()初始化完成，返回初始化成功状态

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| mode | uint8_t | RTS模式设置 | 0: 自动模式; 1: 阈值模式（超过配置报文长度发送RTS，未超过不发）; 2: 关闭模式（不发RTS） |
| pkt_length | uint16_t | RTS报文长度门限 | mode为1时有效 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | RTS模式设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_set_cca_threshold <a id="wifi_set_cca_threshold"></a>

```c
errcode_t wifi_set_cca_threshold(uint8_t mode, int8_t threshold)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_alg.h"
```

**功能说明**

- 设置CCA（空闲信道评估）门限值
- 支持为20M带宽或40M带宽分别设置CCA门限
- 通过mode参数指定带宽，通过threshold参数指定门限值

**前置条件**

- WiFi模块已通过wifi_init()初始化完成，返回初始化成功状态

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| mode | uint8_t | 带宽模式选择 | 0: 20M; 1: 40M |
| threshold | int8_t | CCA门限值 | - |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | CCA门限设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_set_intrf_mode <a id="wifi_set_intrf_mode"></a>

```c
errcode_t wifi_set_intrf_mode(const char *ifname, uint8_t enable, uint16_t flag)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_alg.h"
```

**功能说明**

- 设置抗干扰模式
- 通过enable参数使能或去使能抗干扰模式
- 通过flag参数配置抗干扰措施使能bit位
- 通过ifname指定设备名

**前置条件**

- WiFi模块已通过wifi_init()初始化完成，返回初始化成功状态
- ifname指针不为NULL，且指向合法的设备名

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| ifname | const char * | 设备名 | 非NULL |
| enable | uint8_t | 是否使能抗干扰模式 | 0: 去使能; 1: 使能 |
| flag | uint16_t | 抗干扰措施使能bit配置 | - |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 抗干扰模式设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| _PRE_WLAN_FEATURE_INTRF_MODE | 特性宏 | 支持抗干扰模式接口功能 | y |

### wifi_get_chan_intrf_info <a id="wifi_get_chan_intrf_info"></a>

```c
errcode_t wifi_get_chan_intrf_info(wifi_channel_info_t *info, uint8_t *info_len)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_alg.h"
```

**功能说明**

- 获取WiFi信道干扰信息
- 通过info出参返回信道干扰信息列表
- info_len同时作为入参和出参，入参时指定信道列表最大数量，出参时返回实际信道数量

**前置条件**

- WiFi模块已通过wifi_init()初始化完成，返回初始化成功状态
- info指针不为NULL，且指向内存空间已申请成功，长度不小于info_len指定值
- info_len指针不为NULL，且指向内存空间已申请成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| info_len | uint8_t * | 信道列表最大数量 | 非NULL，入参时指定最大数量，出参时返回实际数量 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| info | [wifi_channel_info_t](#struct_wifi_channel_info_t) * | 干扰信息信道列表 |
| info_len | uint8_t * | 信道列表的实际数量 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 信道干扰信息获取成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

## Type definitions

### errcode_t <a id="typedef_errcode_t"></a>

```c
typedef uint32_t errcode_t;
```

**使用说明**

作为alg模块所有接口的返回值类型使用

## Enumerations

无

## Structures

### alg_param_stru <a id="struct_alg_param_stru"></a>

```c
typedef struct {
    uint32_t rate_value;    /*!< 固定速率值。 */
} alg_param_stru;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| rate_value | uint32_t | 固定速率值 |

### wifi_channel_info_t <a id="struct_wifi_channel_info_t"></a>

```c
typedef struct {
    int16_t noise;
    uint16_t busy_ratio;
    uint16_t CCI;
    uint8_t channel;
    uint8_t resv;
} wifi_channel_info_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| noise | int16_t | 噪声值 |
| busy_ratio | uint16_t | 信道忙碌比率 |
| CCI | uint16_t | 同频干扰值 |
| channel | uint8_t | 信道号 |
| resv | uint8_t | 保留字段 |



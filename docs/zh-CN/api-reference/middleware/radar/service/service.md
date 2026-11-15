# Radar Service

雷达感知服务公共 API。WS63E 支持人体活动雷达检测，通过发射和接收反射信号感知目标物体的移动和存在。雷达分为窄带（高分辨率近距离）和宽带（远距离探测）两类子模式。

提供雷达版本信息查询接口，根据芯片型号动态加载对应的子模块（Sub-6G / SLP (SparkLink Positioning)）。

**头文件清单**

```c
#include "include/middleware/services/radar/radar_errcode.h"
#include "include/middleware/services/radar/radar_service.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_radar_get_version](#uapi_radar_get_version) | 获取雷达宽窄带软件版本号 |

## Functions

### uapi_radar_get_version <a id="uapi_radar_get_version"></a>

```c
errcode_radar_client_t uapi_radar_get_version(radar_version_info_t *radar_version)
```

**头文件清单**

```c
#include "include/middleware/services/radar/radar_service.h"
```

**功能说明**

- 获取雷达宽窄带软件版本号，包含窄带系统与宽带系统的版本信息
- 调用后通过出参返回 [radar_version_info_t](#struct_radar_version_info_t) 结构体，包含 major、minor、patch 三段版本号
- 适用于雷达功能初始化后查询当前固件版本，确认版本一致性或进行版本校验

**前置条件**

- 模块初始化状态：雷达服务已通过 `CONFIG_RADAR_SERVICE` 宏使能并完成初始化
- 依赖资源状态：雷达芯片已上电加载完成，固件版本信息已就绪

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| radar_version | [radar_version_info_t](#struct_radar_version_info_t) * | 指向版本信息结构体的指针，用于接收雷达宽窄带软件版本号 | 不为NULL，且指向的内存空间已分配 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| radar_version | [radar_version_info_t](#struct_radar_version_info_t) * | 填充雷达宽窄带软件版本号，narrow_band 为窄带版本号，wide_band 为宽带版本号 |

**返回值**

- 返回类型：errcode_radar_client_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_RC_SUCCESS(0) | 执行成功 | 获取版本号成功 |
| Other | 其他错误码，参考[errcode_radar_client_t](#enum_errcode_radar_client_t) | 执行失败 |

**参考案例**

- [radar_slp_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/slp_sample/radar_slp_sample.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_RADAR_SERVICE | 特性宏 | 支持雷达服务功能 | n |

## Structures

### radar_version_t <a id="struct_radar_version_t"></a>

```c
typedef struct {
    uint16_t major; /*!< 主版本号 */
    uint16_t minor; /*!< 次版本号 */
    uint16_t patch; /*!< 修订版本号 */
} radar_version_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| major | uint16_t | 主版本号 |
| minor | uint16_t | 次版本号 |
| patch | uint16_t | 修订版本号 |

### radar_version_info_t <a id="struct_radar_version_info_t"></a>

```c
typedef struct {
    radar_version_t narrow_band; /*!< 窄带软件版本号 */
    radar_version_t wide_band;   /*!< 宽带软件版本号 */
} radar_version_info_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| narrow_band | [radar_version_t](#struct_radar_version_t) | 窄带软件版本号 |
| wide_band | [radar_version_t](#struct_radar_version_t) | 宽带软件版本号 |

## Enumerations

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
    ERRCODE_RC_END_BIN,                                    /*!< 雷达原始数据参数：起始end索引 */
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
| ERRCODE_RC_AGC_LNA | 0x8000A125 | AGC (Automatic Gain Control) 增益配置LNA (Low Noise Amplifier) |
| ERRCODE_RC_AGC_VGA | 0x8000A126 | AGC增益配置VGA (Variable Gain Amplifier) |
| ERRCODE_RC_ANT_SW_CTRL | 0x8000A127 | 射频开关控制管脚 |
| ERRCODE_RC_SUBFRAME_PERIOD | 0x8000A128 | 子帧间隔 |
| ERRCODE_RC_ANT_SWITCH_INTERVAL | 0x8000A129 | 雷达天线对切换间隔 |
| ERRCODE_RC_START_BIN | 0x8000A12A | 雷达原始数据参数：起始bin索引 |
| ERRCODE_RC_END_BIN | 0x8000A12B | 雷达原始数据参数：起始end索引 |
| ERRCODE_RC_TX_WAVE_SPREAD_FACTOR | 0x8000A12C | 雷达波形参数：波形序列扩频因子 |
| ERRCODE_RC_TX_WAVE_ACC_RSHIFT_BIT | 0x8000A12D | 雷达波形参数：累加位宽保护右移位数 |
| ERRCODE_RC_TX_WAVE_ACC_NUM | 0x8000A12E | 雷达波形参数：累加次数 |
| ERRCODE_RC_TX_WAVE_CORR_DIV | 0x8000A12F | 雷达波形参数：相关后除数因子 |
| ERRCODE_RC_TX_WAVE_TX_WAVE_LEN | 0x8000A130 | 雷达波形参数：发射波形长度 |
| ERRCODE_RC_TX_WAVE_TX_WAVE_BITS | 0x8000A131 | 雷达波形参数：波形扩频前的三元码序列 |
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
| ERRCODE_RC_EEPROM_READ_FAILED | 0x8000A18A | 雷达从eeprom中读取校准结果时发生错误 |
| ERRCODE_RC_IMAGE_CHECK_FAILED | 0x8000A18B | 雷达固件镜像校验错误 |
| ERRCODE_RC_MAX | 0x8000A1FF | RC错误码最大值 |



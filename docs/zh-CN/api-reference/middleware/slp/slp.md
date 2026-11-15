# SLP

SLP (SparkLink Positioning) 提供基于星闪的低功耗高精度定位（测距与测角）能力，支持单向/单边双向/双边双向测距、多节点组网与空中鼠标等使用模式。本模块为 SLP 客户端接口，涵盖上电、配置本机特性、启动测距、负载收发与结果回调注册等流程。

**头文件清单**

```c
#include "include/middleware/services/slp/slp.h"
#include "include/middleware/services/slp/slp_errcode.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [SlpTask](#slptask) | SLP线程主函数 |
| [SlpPowerOnCommand](#slppoweroncommand) | SLP上电，完成SLP加载 |
| [SlpStartRangingCommand](#slpstartrangingcommand) | SLP启动测距 |
| [SlpPowerOffCommand](#slppoweroffcommand) | SLP下电 |
| [SlpSleepCommand](#slpsleepcommand) | SLP睡眠 |
| [SlpSetLocalAttCommand](#slpsetlocalattcommand) | 设置本机特性 |
| [SlpRecvPayload](#slprecvpayload) | SLP接收负载 |
| [SlpRegisterSendPayloadCallback](#slpregistersendpayloadcallback) | 注册SLP发送负载回调 |
| [SlpRegisterReportCallbacks](#slpregisterreportcallbacks) | 注册SLP上报结果回调函数 |

## Functions

### SlpTask <a id="slptask"></a>

```c
void SlpTask(void* param)
```

**头文件清单**

```c
#include "include/middleware/services/slp/slp.h"
```

**功能说明**

- SLP线程主函数，作为SLP服务的运行入口
- 接收可能传入的参数，用于SLP线程初始化与运行
- 在SLP上下文中执行，为其他SLP接口提供运行基础

**前置条件**

- SLP模块已初始化，相关系统资源已就绪
- 调用方需在合适的线程上下文中启动该函数

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| param | void* | 可能传入的参数 | - |

### SlpPowerOnCommand <a id="slppoweroncommand"></a>

```c
ErrcodeSlpClient SlpPowerOnCommand(void)
```

**头文件清单**

```c
#include "include/middleware/services/slp/slp.h"
```

**功能说明**

- SLP上电，完成SLP加载
- 执行SLP芯片上电初始化流程，加载固件
- 调用后方可进行后续测距、配置等操作

**前置条件**

- SLP硬件已正确连接
- 尚未调用SlpPowerOffCommand下电，或上次已正常下电

**返回值**

- 返回类型：ErrcodeSlpClient

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SLPC_SUCCESS(0x0) | 执行成功 | SLP上电加载成功 |
| Other | 其他错误码，参考[ErrcodeSlpClient](#enum_errcodeslpclient) | 执行失败 |

### SlpStartRangingCommand <a id="slpstartrangingcommand"></a>

```c
ErrcodeSlpClient SlpStartRangingCommand(const SlpStartRangingParam *param)
```

**头文件清单**

```c
#include "include/middleware/services/slp/slp.h"
```

**功能说明**

- SLP启动测距
- 根据传入参数配置帧参数、安全参数、测距参数，启动SLP测距流程
- 测距结果通过已注册的回调函数上报

**前置条件**

- SLP模块已通过SlpPowerOnCommand上电成功
- 已通过SlpRegisterReportCallbacks注册测距结果回调函数
- 已通过SlpSetLocalAttCommand设置本机特性

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| param | const [SlpStartRangingParam](#struct_slpstartrangingparam)* | 启动测距参数 | 指针不为NULL |

**返回值**

- 返回类型：ErrcodeSlpClient

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SLPC_SUCCESS(0x0) | 执行成功 | 测距启动成功 |
| Other | 其他错误码，参考[ErrcodeSlpClient](#enum_errcodeslpclient) | 执行失败 |

### SlpPowerOffCommand <a id="slppoweroffcommand"></a>

```c
ErrcodeSlpClient SlpPowerOffCommand(void)
```

**头文件清单**

```c
#include "include/middleware/services/slp/slp.h"
```

**功能说明**

- SLP下电
- 执行SLP芯片下电流程，释放硬件资源
- 下电后需重新调用SlpPowerOnCommand方可再次使用SLP功能

**前置条件**

- SLP模块已通过SlpPowerOnCommand上电成功
- 建议先停止测距操作

**返回值**

- 返回类型：ErrcodeSlpClient

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SLPC_SUCCESS(0x0) | 执行成功 | SLP下电成功 |
| Other | 其他错误码，参考[ErrcodeSlpClient](#enum_errcodeslpclient) | 执行失败 |

### SlpSleepCommand <a id="slpsleepcommand"></a>

```c
ErrcodeSlpClient SlpSleepCommand(void)
```

**头文件清单**

```c
#include "include/middleware/services/slp/slp.h"
```

**功能说明**

- SLP睡眠
- 使SLP芯片进入低功耗睡眠状态
- 睡眠后可再次唤醒继续使用SLP功能

**前置条件**

- SLP模块已通过SlpPowerOnCommand上电成功
- 建议先停止测距操作

**返回值**

- 返回类型：ErrcodeSlpClient

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SLPC_SUCCESS(0x0) | 执行成功 | SLP睡眠成功 |
| Other | 其他错误码，参考[ErrcodeSlpClient](#enum_errcodeslpclient) | 执行失败 |

### SlpSetLocalAttCommand <a id="slpsetlocalattcommand"></a>

```c
ErrcodeSlpClient SlpSetLocalAttCommand(const SlpLocalAtt *param)
```

**头文件清单**

```c
#include "include/middleware/services/slp/slp.h"
```

**功能说明**

- 设置本机特性
- 配置本机地址与屏幕参数（光标速度、屏幕分辨率）
- 需在启动测距前调用

**前置条件**

- SLP模块已通过SlpPowerOnCommand上电成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| param | const [SlpLocalAtt](#struct_slplocalatt)* | 本机特性参数 | 指针不为NULL |

**返回值**

- 返回类型：ErrcodeSlpClient

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SLPC_SUCCESS(0x0) | 执行成功 | 设置本机特性成功 |
| Other | 其他错误码，参考[ErrcodeSlpClient](#enum_errcodeslpclient) | 执行失败 |

### SlpRecvPayload <a id="slprecvpayload"></a>

```c
ErrcodeSlpClient SlpRecvPayload(const SlpPayloadInfo *info)
```

**头文件清单**

```c
#include "include/middleware/services/slp/slp.h"
```

**功能说明**

- SLP接收负载
- 接收对端发送的负载数据
- 负载信息通过SlpPayloadInfo结构体传递

**前置条件**

- SLP模块已通过SlpPowerOnCommand上电成功
- 已通过SlpRegisterSendPayloadCallback注册发送负载回调

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| info | const [SlpPayloadInfo](#struct_slppayloadinfo)* | 负载信息 | 指针不为NULL |

**返回值**

- 返回类型：ErrcodeSlpClient

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SLPC_SUCCESS(0x0) | 执行成功 | 接收负载成功 |
| Other | 其他错误码，参考[ErrcodeSlpClient](#enum_errcodeslpclient) | 执行失败 |

### SlpRegisterSendPayloadCallback <a id="slpregistersendpayloadcallback"></a>

```c
ErrcodeSlpClient SlpRegisterSendPayloadCallback(SlpSendPayloadCallback cbk)
```

**头文件清单**

```c
#include "include/middleware/services/slp/slp.h"
```

**功能说明**

- 注册SLP发送负载回调
- 当SLP需要发送负载时，通过注册的回调函数执行
- 回调函数运行于SLP线程上下文，不能阻塞或长时间等待
- 回调中指针由SLP申请内存并释放，回调中不应释放

**前置条件**

- SLP模块已通过SlpPowerOnCommand上电成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| cbk | [SlpSendPayloadCallback](#slpsendpayloadcallback) | 发送负载回调函数 | 不为NULL |

**返回值**

- 返回类型：ErrcodeSlpClient

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SLPC_SUCCESS(0x0) | 执行成功 | 注册回调成功 |
| Other | 其他错误码，参考[ErrcodeSlpClient](#enum_errcodeslpclient) | 执行失败 |

### SlpRegisterReportCallbacks <a id="slpregisterreportcallbacks"></a>

```c
ErrcodeSlpClient SlpRegisterReportCallbacks(SlpReportCallbacks *func)
```

**头文件清单**

```c
#include "include/middleware/services/slp/slp.h"
```

**功能说明**

- 注册SLP上报结果回调函数
- 注册光标数据上报回调与测距结果上报回调
- 回调函数运行于SLP线程上下文，不能阻塞或长时间等待
- 回调中指针由SLP申请内存并释放，回调中不应释放

**前置条件**

- SLP模块已通过SlpPowerOnCommand上电成功

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func | [SlpReportCallbacks](#struct_slpreportcallbacks)* | 回调函数结构体指针 | 指针不为NULL |

**返回值**

- 返回类型：ErrcodeSlpClient

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SLPC_SUCCESS(0x0) | 执行成功 | 注册回调成功 |
| Other | 其他错误码，参考[ErrcodeSlpClient](#enum_errcodeslpclient) | 执行失败 |

## Type definitions

### SlpReportRangingCallback <a id="slpreportrangingcallback"></a>

```c
typedef void (*SlpReportRangingCallback)(SlpRangingRpt *rangingRpt);
```

**使用说明**

测距结果上报回调函数类型，用于SlpRegisterReportCallbacks注册的回调结构体中rptRangingCbk成员。

### SlpReportCursorCallback <a id="slpreportcursorcallback"></a>

```c
typedef void (*SlpReportCursorCallback)(uint16_t x, uint16_t y);
```

**使用说明**

光标数据上报回调函数类型，用于SlpRegisterReportCallbacks注册的回调结构体中rptCursorCbk成员。

### SlpSendPayloadCallback <a id="slpsendpayloadcallback"></a>

```c
typedef void (*SlpSendPayloadCallback)(const SlpPayloadInfo *info);
```

**使用说明**

发送负载回调函数类型，用于SlpRegisterSendPayloadCallback的入参。

## Enumerations

### SlpSecMode <a id="enum_slpsecmode"></a>

```c
typedef enum {
    SLP_SEC_MODE_LPLS = 0,          /*!< slp低功耗低安全模式 */
    SLP_SEC_MODE_HPHS               /*!< slp高功耗高安全模式 */
} SlpSecMode;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SLP_SEC_MODE_LPLS | 0 | slp低功耗低安全模式 |
| SLP_SEC_MODE_HPHS | 1 | slp高功耗高安全模式 |

### SlpEncryptMode <a id="enum_slpencryptmode"></a>

```c
typedef enum {
    SLP_ENCRYPT_MODE_AES128 = 0,       /*!< slp aes-128 加密模式 */
    SLP_ENCRYPT_MODE_SM4               /*!< slp sm4加密模式 */
} SlpEncryptMode;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SLP_ENCRYPT_MODE_AES128 | 0 | slp aes-128加密模式 |
| SLP_ENCRYPT_MODE_SM4 | 1 | slp sm4加密模式 |

### SlpSessionKeyMode <a id="enum_slpsessionkeymode"></a>

```c
typedef enum {
    SLP_WB_SHARED_KEY = 0,       /*!< slp 宽带共享密钥 */
    SLP_NB_SESSION_KEY           /*!< slp 窄带会话密钥 */
} SlpSessionKeyMode;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SLP_WB_SHARED_KEY | 0 | slp宽带共享密钥 |
| SLP_NB_SESSION_KEY | 1 | slp窄带会话密钥 |

### SlpChannel <a id="enum_slpchannel"></a>

```c
typedef enum {
    SLP_CH_5 = 0,        /*!< slp信道5，中心频率6489.6MHz */
    SLP_CH_6,            /*!< slp信道6，中心频率6988.8MHz */
    SLP_CH_8,            /*!< slp信道8，中心频率7488MHz */
    SLP_CH_9             /*!< slp信道9，中心频率7987.2MHz */
} SlpChannel;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SLP_CH_5 | 0 | slp信道5，中心频率6489.6MHz |
| SLP_CH_6 | 1 | slp信道6，中心频率6988.8MHz |
| SLP_CH_8 | 2 | slp信道8，中心频率7488MHz |
| SLP_CH_9 | 3 | slp信道9，中心频率7987.2MHz |

### SlpUsageMode <a id="enum_slpusagemode"></a>

```c
typedef enum {
    SLP_USAGE_RANGING_AOX = 0,        /*!< slp测距测角使用模式 */
    SLP_USAGE_AIR_MOUSE,              /*!< slp空中鼠标使用模式 */
} SlpUsageMode;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SLP_USAGE_RANGING_AOX | 0 | slp测距测角使用模式 |
| SLP_USAGE_AIR_MOUSE | 1 | slp空中鼠标使用模式 |

### SlpRangingMode <a id="enum_slprangingmode"></a>

```c
typedef enum {
    SLP_RANGING_ONLY = 0,           /*!< slp测距 */
    SLP_RANGING_AOA,                /*!< slp测距和aoa测角 */
} SlpRangingMode;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SLP_RANGING_ONLY | 0 | slp测距 |
| SLP_RANGING_AOA | 1 | slp测距和aoa测角 |

### SlpMultiNodeMode <a id="enum_slpmultinodemode"></a>

```c
typedef enum {
    SLP_ONE_TO_ONE = 0,         /*!< slp 1对1 */
    SLP_ONE_TO_MANY,            /*!< slp 1对多 */
    SLP_MANY_TO_MANY            /*!< slp 多对多 */
} SlpMultiNodeMode;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SLP_ONE_TO_ONE | 0 | slp 1对1 |
| SLP_ONE_TO_MANY | 1 | slp 1对多 |
| SLP_MANY_TO_MANY | 2 | slp 多对多 |

### SlpRangingRoundUsage <a id="enum_slprangingroundusage"></a>

```c
typedef enum {
    SLP_OWR = 0,            /*!< slp 单向测距 */
    SLP_SS_TWR,             /*!< slp 单边双向测距 */
    SLP_DS_TWR              /*!< slp 双边双向测距 */
} SlpRangingRoundUsage;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SLP_OWR | 0 | slp单向测距 |
| SLP_SS_TWR | 1 | slp单边双向测距 |
| SLP_DS_TWR | 2 | slp双边双向测距 |

### SlpMrSource <a id="enum_slpmrsource"></a>

```c
typedef enum {
    SLP_MR_RECV = 0,        /*!< slp测量值接收方，表示计算距离的一方 */
    SLP_MR_SEND             /*!< slp测量值发送方 */
} SlpMrSource;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SLP_MR_RECV | 0 | slp测量值接收方，表示计算距离的一方 |
| SLP_MR_SEND | 1 | slp测量值发送方 |

### SlpAoxDirection <a id="enum_slpaoxdirection"></a>

```c
typedef enum {
    SLP_POLL_AOX = 0,              /*!< slp测距发起方发测角帧 */
    SLP_RESPONSE_AOX,              /*!< slp测距响应方发测角帧 */
    SLP_DOUBLE_SIDE_AOX            /*!< slp双边测角 */
} SlpAoxDirection;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SLP_POLL_AOX | 0 | slp测距发起方发测角帧 |
| SLP_RESPONSE_AOX | 1 | slp测距响应方发测角帧 |
| SLP_DOUBLE_SIDE_AOX | 2 | slp双边测角 |

### SlpNbSyncMode <a id="enum_slpnbsyncmode"></a>

```c
typedef enum {
    SLP_NB_SINGLE_SIDE_SYNC = 1,        /*!< 窄带单向同步 */
    SLP_NB_DOUBLE_SIDE_SYNC,            /*!< 窄带双向同步 */
} SlpNbSyncMode;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SLP_NB_SINGLE_SIDE_SYNC | 1 | 窄带单向同步 |
| SLP_NB_DOUBLE_SIDE_SYNC | 2 | 窄带双向同步 |

### SlpScreenResolution <a id="enum_slpscreenresolution"></a>

```c
typedef enum {
    SLP_SCREEN_720P = 0,              /*!< slp屏幕分辨率720p */
    SLP_SCREEN_1080P,                 /*!< slp屏幕分辨率1080p */
    SLP_SCREEN_4K,                    /*!< slp屏幕分辨率4K */
    SLP_SCREEN_8K                     /*!< slp屏幕分辨率8k */
} SlpScreenResolution;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SLP_SCREEN_720P | 0 | slp屏幕分辨率720p |
| SLP_SCREEN_1080P | 1 | slp屏幕分辨率1080p |
| SLP_SCREEN_4K | 2 | slp屏幕分辨率4K |
| SLP_SCREEN_8K | 3 | slp屏幕分辨率8k |

### SlpCursorSpeed <a id="enum_slpcursorspeed"></a>

```c
typedef enum {
    SLP_CURSOR_SPEED_LOW = 0,          /*!< slp光标低速移动 */
    SLP_CURSOR_SPEED_MEDIUM,           /*!< slp光标中速移动 */
    SLP_CURSOR_SPEED_HIGH,             /*!< slp光标高速移动 */
} SlpCursorSpeed;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| SLP_CURSOR_SPEED_LOW | 0 | slp光标低速移动 |
| SLP_CURSOR_SPEED_MEDIUM | 1 | slp光标中速移动 |
| SLP_CURSOR_SPEED_HIGH | 2 | slp光标高速移动 |

### ErrcodeSlp <a id="enum_errcodeslp"></a>

```c
typedef enum {
    ERRCODE_SLP_SUCCESS = 0,                               /*!< 执行成功错误码 */
    ERRCODE_SLP_EXCEED_MAX_COUNTER = ERRCODE_SLP_BASE,     /*!< 超过芯片最大计数值错误码 */
    RRCODE_SLP_MAX = ERRCODE_SLP_END                       /*!< SLP 错误码最大值 */
} ErrcodeSlp;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| ERRCODE_SLP_SUCCESS | 0 | 执行成功 |
| ERRCODE_SLP_EXCEED_MAX_COUNTER | 0x80009400 | 超过芯片最大计数值 |
| RRCODE_SLP_MAX | 0x800095FF | SLP错误码最大值 |

### ErrcodeSlpClient <a id="enum_errcodeslpclient"></a>

```c
typedef enum {
    ERRCODE_SLPC_SUCCESS = 0,                               /*!< 执行成功错误码 */
    ERRCODE_SLPC_MALLOC_FAILED = ERRCODE_SLPC_BASE,         /*!< 申请动态内存失败错误码 */
    ERRCODE_SLPC_MEMCPY_FAILED,                             /*!< 内存拷贝失败错误码 */
    ERRCODE_SLPC_SEND_PAYLOAD_FAILED,                       /*!< 发送负载失败错误码 */
    ERRCODE_SLPC_RECV_PAYLOAD_FAILED,                       /*!< 接收负载失败错误码 */
    RRCODE_SLPC_MAX = ERRCODE_SLPC_END                       /*!< SLPC 错误码最大值 */
} ErrcodeSlpClient;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| ERRCODE_SLPC_SUCCESS | 0 | 执行成功 |
| ERRCODE_SLPC_MALLOC_FAILED | 0x80009600 | 申请动态内存失败 |
| ERRCODE_SLPC_MEMCPY_FAILED | 0x80009601 | 内存拷贝失败 |
| ERRCODE_SLPC_SEND_PAYLOAD_FAILED | 0x80009602 | 发送负载失败 |
| ERRCODE_SLPC_RECV_PAYLOAD_FAILED | 0x80009603 | 接收负载失败 |
| RRCODE_SLPC_MAX | 0x800097FF | SLPC错误码最大值 |

## Structures

### SlpDeviceAddr <a id="struct_slpdeviceaddr"></a>

```c
typedef struct {
    uint8_t addr[SLP_ADDR_LEN];     /*!< 地址 */
} SlpDeviceAddr;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| addr | uint8_t[6] | 设备地址，长度为SLP_ADDR_LEN(6)字节 |

### SlpSecurityParam <a id="struct_slpsecurityparam"></a>

```c
typedef struct {
    uint8_t secMode;            /*!< 安全模式，参考 SlpSecMode */
    uint8_t encryptMode;        /*!< 加密模式，参考 SlpEncryptMode */
    uint8_t sessionKeyMode;     /*!< 会话密钥模式，参考 SlpSessionKeyMode */
    uint8_t sessionKeyIdx;      /*!< 会话密钥索引值，会话密钥模式是宽带共享密钥时有效，取值范围0~15 */
} SlpSecurityParam;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| secMode | uint8_t | 安全模式，参考[SlpSecMode](#enum_slpsecmode) |
| encryptMode | uint8_t | 加密模式，参考[SlpEncryptMode](#enum_slpencryptmode) |
| sessionKeyMode | uint8_t | 会话密钥模式，参考[SlpSessionKeyMode](#enum_slpsessionkeymode) |
| sessionKeyIdx | uint8_t | 会话密钥索引值，会话密钥模式为宽带共享密钥时有效，取值范围0~15 |

### SlpFrameParam <a id="struct_slpframeparam"></a>

```c
typedef struct {
    uint8_t codeLen;            /*!< 前导码长度 */
    uint8_t syncCodeIdx;        /*!< sync段的码字索引值 */
    uint8_t syncSymNum;         /*!< sync段的符号数量 */
    uint8_t ctsSegNum;          /*!< cts域中segment段的数量 */
    uint8_t ctsSegSymNum;       /*!< 一个segment段中的符号数量 */
    uint8_t gapBaseSymNum;      /*!< gap段基准的符号数量 */
    uint8_t ctsCpLen;           /*!< cts段中cp长度 */
} SlpFrameParam;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| codeLen | uint8_t | 前导码长度，0-31(16), 1-63(8), 2-91(4), 3-127(4) |
| syncCodeIdx | uint8_t | sync段的码字索引值，codeLen31/91: 0~7, codeLen63: 0~5, codeLen127: 0~15 |
| syncSymNum | uint8_t | sync段的符号数量，0-16, 1-32, 2-64, 3-96, 4-128, 5-256 |
| ctsSegNum | uint8_t | cts域中segment段的数量，0-1, 1-2, 2-4, 3-8, 4-16, 5-32 |
| ctsSegSymNum | uint8_t | 一个segment段中的符号数量，0-16, 1-32 |
| gapBaseSymNum | uint8_t | gap段基准的符号数量，0-12, 1-25, 2-50, 3-125, 4-250, 5-500, 6-1000 |
| ctsCpLen | uint8_t | cts段中cp长度，0-0, 1-64, 2-96, 3-128 |

### SlpRangingParam <a id="struct_slprangingparam"></a>

```c
typedef struct {
    uint8_t chIdx;                          /*!< 通道号，参考 SlpChannel */
    uint8_t usageMode;                      /*!< slp使用模式，参考 SlpUsageMode */
    uint8_t slpRangingMode;                 /*!< slp测距模式，参考 SlpRangingMode */
    uint8_t multiNodeMode;                  /*!< slp多节点模式，参考 SlpMultiNodeMode */
    uint8_t rangingRoundUsage;              /*!< slp测距轮用法，参考 SlpRangingRoundUsage */
    uint8_t mrSource;                       /*!< slp测量值请求源，参考 SlpMrSource */
    uint8_t rangingFreq;                    /*!< 测距频率，取值：1~20，单位：Hz */
    uint8_t validRoundNum;                  /*!< 有效测距轮数，取值范围：0~255，其中0表示无限轮 */
    uint8_t aoxDirection;                   /*!< 测角方向，参考 SlpAoxDirection */
    uint8_t nbSyncMode;                     /*!< 窄带同步模式，参考 SlpNbSyncMode */
    uint8_t deviceNum;                      /*!< 参与测距的设备个数，取值：2 */
    uint32_t sessionId;                     /*!< 会话标识符，随机值，每次启动测距，值不同 */
    SlpDeviceAddr deviceAddr[SLP_MAX_DEVICE_NUM]; /*!< 设备地址列表，第1个是测距发起者的地址 */
} SlpRangingParam;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| chIdx | uint8_t | 通道号，参考[SlpChannel](#enum_slpchannel) |
| usageMode | uint8_t | slp使用模式，参考[SlpUsageMode](#enum_slpusagemode) |
| slpRangingMode | uint8_t | slp测距模式，参考[SlpRangingMode](#enum_slprangingmode) |
| multiNodeMode | uint8_t | slp多节点模式，参考[SlpMultiNodeMode](#enum_slpmultinodemode) |
| rangingRoundUsage | uint8_t | slp测距轮用法，参考[SlpRangingRoundUsage](#enum_slprangingroundusage) |
| mrSource | uint8_t | slp测量值请求源，参考[SlpMrSource](#enum_slpmrsource) |
| rangingFreq | uint8_t | 测距频率，取值1~20，单位Hz |
| validRoundNum | uint8_t | 有效测距轮数，取值0~255，0表示无限轮 |
| aoxDirection | uint8_t | 测角方向，参考[SlpAoxDirection](#enum_slpaoxdirection) |
| nbSyncMode | uint8_t | 窄带同步模式，参考[SlpNbSyncMode](#enum_slpnbsyncmode) |
| deviceNum | uint8_t | 参与测距的设备个数，取值2 |
| sessionId | uint32_t | 会话标识符，随机值，每次启动测距值不同 |
| deviceAddr | [SlpDeviceAddr](#struct_slpdeviceaddr)[16] | 设备地址列表，第1个是测距发起者的地址，最大SLP_MAX_DEVICE_NUM(16) |

### SlpStartRangingParam <a id="struct_slpstartrangingparam"></a>

```c
typedef struct {
    SlpFrameParam frameParam;           /*!< slp帧参数 */
    SlpSecurityParam secParam;          /*!< slp安全参数 */
    SlpRangingParam rangingParam;       /*!< slp测距参数 */
} SlpStartRangingParam;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| frameParam | [SlpFrameParam](#struct_slpframeparam) | slp帧参数 |
| secParam | [SlpSecurityParam](#struct_slpsecurityparam) | slp安全参数 |
| rangingParam | [SlpRangingParam](#struct_slprangingparam) | slp测距参数 |

### SlpScreenParam <a id="struct_slpscreenparam"></a>

```c
typedef struct {
    uint8_t cursorSpeed;               /*!< slp光标移动速度，参考 SlpCursorSpeed */
    uint8_t resolution;                /*!< slp屏幕分辨率，参考 SlpScreenResolution */
} SlpScreenParam;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| cursorSpeed | uint8_t | slp光标移动速度，参考[SlpCursorSpeed](#enum_slpcursorspeed) |
| resolution | uint8_t | slp屏幕分辨率，参考[SlpScreenResolution](#enum_slpscreenresolution) |

### SlpLocalAtt <a id="struct_slplocalatt"></a>

```c
typedef struct {
    SlpDeviceAddr localAddr;        /*!< 本机地址 */
    SlpScreenParam screenParam;     /*!< 屏幕参数 */
} SlpLocalAtt;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| localAddr | [SlpDeviceAddr](#struct_slpdeviceaddr) | 本机地址 |
| screenParam | [SlpScreenParam](#struct_slpscreenparam) | 屏幕参数 |

### SlpRangingRpt <a id="struct_slprangingrpt"></a>

```c
typedef struct {
    uint32_t distance;                  /*!< 距离，单位：毫米 */
    uint8_t aoxAziFom;                  /*!< 方位角置信度 */
    uint8_t aoxElevFom;                 /*!< 俯仰角置信度 */
    int16_t aoxAzi;                     /*!< 方位角，单位：度 */
    int16_t aoxElev;                    /*!< 俯仰角，单位：度 */
    SlpDeviceAddr providerAddr;         /*!< 测量值提供方的地址 */
    SlpDeviceAddr requestorAddr;        /*!< 测量值请求方的地址 */
} SlpRangingRpt;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| distance | uint32_t | 距离，单位毫米 |
| aoxAziFom | uint8_t | 方位角置信度 |
| aoxElevFom | uint8_t | 俯仰角置信度 |
| aoxAzi | int16_t | 方位角，单位度 |
| aoxElev | int16_t | 俯仰角，单位度 |
| providerAddr | [SlpDeviceAddr](#struct_slpdeviceaddr) | 测量值提供方的地址 |
| requestorAddr | [SlpDeviceAddr](#struct_slpdeviceaddr) | 测量值请求方的地址 |

### SlpPayloadInfo <a id="struct_slppayloadinfo"></a>

```c
typedef struct {
    uint8_t *payload;                  /*!< slp负载 */
    uint16_t payloadLen;               /*!< slp负载长度，取值范围[0, 255] */
} SlpPayloadInfo;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| payload | uint8_t* | slp负载数据指针 |
| payloadLen | uint16_t | slp负载长度，取值范围0~255 |

### SlpReportCallbacks <a id="struct_slpreportcallbacks"></a>

```c
typedef struct {
    SlpReportCursorCallback rptCursorCbk;               /*!< sle上报光标回调函数 */
    SlpReportRangingCallback rptRangingCbk;             /*!< sle上报测距结果回调函数 */
} SlpReportCallbacks;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| rptCursorCbk | [SlpReportCursorCallback](#slpreportcursorcallback) | 上报光标回调函数 |
| rptRangingCbk | [SlpReportRangingCallback](#slpreportrangingcallback) | 上报测距结果回调函数 |



# WiFi Hotspot

WiFi SoftAP（热点）模式 API。管理 SoftAP 启停、配置（SSID/密码/信道/加密方式/最大接入数）、获取已连接 Station 列表和断开指定 Station。

WS63 支持 STA (Station) + SoftAP 共存模式，同时作为客户端连接路由器并作为热点供其他设备接入。最大支持 6 路 Station 接入。

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_hotspot.h"
#include "include/middleware/services/wifi/wifi_hotspot_config.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [wifi_softap_enable](#wifi_softap_enable) | 开启SoftAP接口 |
| [wifi_softap_disable](#wifi_softap_disable) | 关闭SoftAP接口 |
| [wifi_is_softap_enabled](#wifi_is_softap_enabled) | 获取SoftAP使能状态 |
| [wifi_set_softap_config_advance](#wifi_set_softap_config_advance) | 设置SoftAP扩展配置 |
| [wifi_get_softap_config](#wifi_get_softap_config) | 获取SoftAP配置 |
| [wifi_get_softap_config_advance](#wifi_get_softap_config_advance) | 获取SoftAP扩展配置 |
| [wifi_softap_get_sta_list](#wifi_softap_get_sta_list) | 获取SoftAP已连接的STA信息列表 |
| [wifi_softap_deauth_sta](#wifi_softap_deauth_sta) | 断开SoftAP指定STA的连接 |

## Functions

### wifi_softap_enable <a id="wifi_softap_enable"></a>

```c
errcode_t wifi_softap_enable(const softap_config_stru *config)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_hotspot.h"
```

**功能说明**

- 根据传入的SoftAP配置参数开启SoftAP接口，使设备进入热点模式
- 开启前校验配置参数合法性（SSID (Service Set Identifier) 长度、安全类型、密钥等），校验不通过返回失败
- 开启时会根据配置中的安全类型自动选择加密方式（如SAE/WPA3使用AES (Advanced Encryption Standard)，其他使用TKIP/AES混合）
- SoftAP已使能时调用会返回失败

**前置条件**

- WiFi已通过wifi_init()初始化完成，且初始化状态正常
- SoftAP当前未使能（即wifi_is_softap_enabled()返回0）
- 入参config指向的内存空间已申请成功，且配置参数合法

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| config | const [softap_config_stru](#struct_softap_config_stru) * | SoftAP配置参数 | 非NULL；ssid长度1~32字节；security_type参考[wifi_security_enum](#enum_wifi_security_enum)；channel_num取值0~14；wifi_psk_type取值0或1 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | SoftAP成功开启 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [radar_softap_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/softap_sample/radar_softap_sample.c)

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_OWE | 特性宏 | 支持OWE安全类型特性 | n |

### wifi_softap_disable <a id="wifi_softap_disable"></a>

```c
errcode_t wifi_softap_disable(void)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_hotspot.h"
```

**功能说明**

- 关闭SoftAP接口，停止热点模式
- 关闭时若DHCP (Dynamic Host Configuration Protocol) Server已启用，会自动关闭DHCP Server
- 关闭后清空SoftAP接口名称缓存

**前置条件**

- WiFi已通过wifi_init()初始化完成
- SoftAP当前已使能（即wifi_is_softap_enabled()返回1）

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | SoftAP成功关闭 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [radar_softap_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/softap_sample/radar_softap_sample.c)

### wifi_is_softap_enabled <a id="wifi_is_softap_enabled"></a>

```c
int32_t wifi_is_softap_enabled(void)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_hotspot.h"
```

**功能说明**

- 查询SoftAP当前是否已使能
- WiFi未初始化时返回0

**前置条件**

- WiFi已通过wifi_init()初始化完成

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 1 | SoftAP已使能 | SoftAP已成功开启 |
| 0 | SoftAP未使能 | SoftAP未开启或WiFi未初始化 |

**参考案例**

- [hilink_softap_adapter.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/hilink_softap_adapter.c)

### wifi_set_softap_config_advance <a id="wifi_set_softap_config_advance"></a>

```c
errcode_t wifi_set_softap_config_advance(const softap_config_advance_stru *config)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_hotspot.h"
```

**功能说明**

- 设置SoftAP的扩展配置参数，包括beacon间隔、DTIM (Delivery Traffic Indication Message) 周期、GTK重密钥周期、SSID隐藏标志、GI及协议模式
- SoftAP已使能时调用会返回失败
- 各参数为0时表示不配置该项，使用默认值

**前置条件**

- WiFi已通过wifi_init()初始化完成
- SoftAP当前未使能
- 入参config指向的内存空间已申请成功，且各配置参数在合法范围内

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| config | const [softap_config_advance_stru](#struct_softap_config_advance_stru) * | SoftAP扩展配置参数 | 非NULL；beacon_interval取值25~1000或0(未配置)；dtim_period取值1~30或0(未配置)；group_rekey取值30~86400或0(未配置)；hidden_ssid_flag取值1(不隐藏)或2(隐藏)或0(未配置)；protocol_mode参考[protocol_mode_enum](#enum_protocol_mode_enum)或0(未配置) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 扩展配置设置成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [radar_softap_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/softap_sample/radar_softap_sample.c)

### wifi_get_softap_config <a id="wifi_get_softap_config"></a>

```c
errcode_t wifi_get_softap_config(softap_config_stru *result)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_hotspot.h"
```

**功能说明**

- 获取SoftAP当前的基础配置信息，包括SSID、预共享密钥、安全类型、信道号、PSK (Pre-Shared Key) 类型
- 返回的是SoftAP已保存的配置副本

**前置条件**

- WiFi已通过wifi_init()初始化完成
- 入参result指向的内存空间已申请成功，大小不小于softap_config_stru结构体

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| result | [softap_config_stru](#struct_softap_config_stru) * | SoftAP配置信息输出 | 非NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| result | [softap_config_stru](#struct_softap_config_stru) | SoftAP当前基础配置信息，包含SSID、预共享密钥、安全类型、信道号、PSK类型 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 成功获取配置信息 |

### wifi_get_softap_config_advance <a id="wifi_get_softap_config_advance"></a>

```c
errcode_t wifi_get_softap_config_advance(softap_config_advance_stru *result)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_hotspot.h"
```

**功能说明**

- 获取SoftAP当前的扩展配置信息，包括beacon间隔、DTIM周期、GTK重密钥周期、SSID隐藏标志、GI及协议模式
- 返回的是SoftAP已保存的扩展配置副本

**前置条件**

- WiFi已通过wifi_init()初始化完成
- 入参result指向的内存空间已申请成功，大小不小于softap_config_advance_stru结构体

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| result | [softap_config_advance_stru](#struct_softap_config_advance_stru) * | SoftAP扩展配置信息输出 | 非NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| result | [softap_config_advance_stru](#struct_softap_config_advance_stru) | SoftAP当前扩展配置信息，包含beacon间隔、DTIM周期、GTK重密钥周期、SSID隐藏标志、GI及协议模式 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 成功获取扩展配置信息 |

### wifi_softap_get_sta_list <a id="wifi_softap_get_sta_list"></a>

```c
errcode_t wifi_softap_get_sta_list(wifi_sta_info_stru *result, uint32_t *size)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_hotspot.h"
```

**功能说明**

- 获取SoftAP下所有已连接STA的信息列表，包括MAC (Media Access Control) 地址、RSSI (Received Signal Strength Indicator) 信号强度、最佳发送速率
- 当实际STA数量小于*size时，*size会被更新为实际STA数量
- 返回的STA信息按顺序填充到result数组中

**前置条件**

- WiFi已通过wifi_init()初始化完成
- SoftAP当前已使能
- 入参result指向的内存空间已申请成功，数组长度不小于*size
- 入参size非NULL且*size大于0

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| result | [wifi_sta_info_stru](#struct_wifi_sta_info_stru) * | STA信息输出数组 | 非NULL；数组长度不小于*size |
| size | uint32_t * | STA数量，输入时为期望获取的最大数量，输出时为实际STA数量 | 非NULL；*size > 0 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| result | [wifi_sta_info_stru](#struct_wifi_sta_info_stru) | 已连接STA的信息列表，包含MAC地址、RSSI、最佳发送速率 |
| size | uint32_t * | 实际已连接的STA数量，当实际数量小于输入值时会被更新 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 成功获取STA信息列表 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

### wifi_softap_deauth_sta <a id="wifi_softap_deauth_sta"></a>

```c
errcode_t wifi_softap_deauth_sta(const uint8_t *mac, int32_t mac_len)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_hotspot.h"
```

**功能说明**

- 根据MAC地址断开SoftAP上指定STA的连接，发送deauth帧
- MAC地址长度必须为6，否则返回失败

**前置条件**

- WiFi已通过wifi_init()初始化完成
- SoftAP当前已使能
- 入参mac指向的内存空间已申请成功，长度为6字节

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| mac | const uint8_t * | 待断开STA的MAC地址 | 非NULL；长度必须为6 |
| mac_len | int32_t | MAC地址长度 | 必须为6 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 成功断开指定STA |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

## Type definitions

### errcode_t <a id="typedef_errcode_t"></a>

```c
typedef uint32_t errcode_t;
```

**使用说明**

作为wifi_softap_enable、wifi_softap_disable、wifi_set_softap_config_advance、wifi_get_softap_config、wifi_get_softap_config_advance、wifi_softap_get_sta_list、wifi_softap_deauth_sta接口的返回值类型

## Enumerations

### wifi_security_enum <a id="enum_wifi_security_enum"></a>

```c
typedef enum wifi_security_enum {
    WIFI_SEC_TYPE_INVALID = -1,         /*!< 无效安全类型 */
    WIFI_SEC_TYPE_OPEN,                 /*!< Open */
    WIFI_SEC_TYPE_WEP,                  /*!< WEP-SHARED */
    WIFI_SEC_TYPE_WPA2PSK,              /*!< WPA2-Personal */
    WIFI_SEC_TYPE_WPA2_WPA_PSK_MIX,     /*!< WPA-Personal和WPA2-Personal混合 */
    WIFI_SEC_TYPE_WPAPSK,               /*!< WPA-Personal */
    WIFI_SEC_TYPE_WPA,                  /*!< WPA-Enterprise */
    WIFI_SEC_TYPE_WPA2,                 /*!< WPA2-Enterprise */
    WIFI_SEC_TYPE_SAE,                  /*!< SAE(WPA3个人级) */
    WIFI_SEC_TYPE_WPA3_WPA2_PSK_MIX,    /*!< WPA2-Personal和WPA3-Personal混合 */
    WIFI_SEC_TYPE_WPA3,                 /*!< WPA3-Enterprise */
    WIFI_SEC_TYPE_OWE,                  /*!< OWE */
    WIFI_SEC_TYPE_WAPI_PSK,             /*!< WAPI个人级 */
    WIFI_SEC_TYPE_WAPI_CERT,            /*!< WAPI企业级 */
    WIFI_SEC_TYPE_WPA3_WPA2_MIX,        /*!< WPA2-Enterprise和WPA3-Enterprise混合 */
    WIFI_SEC_TYPE_WEP_OPEN,             /*!< WEP-OPEN */
    WIFI_SEC_TYPE_UNKNOWN               /*!< 其它认证类型 */
} wifi_security_enum;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| WIFI_SEC_TYPE_INVALID | -1 | 无效安全类型 |
| WIFI_SEC_TYPE_OPEN | 0 | Open |
| WIFI_SEC_TYPE_WEP | 1 | WEP-SHARED |
| WIFI_SEC_TYPE_WPA2PSK | 2 | WPA2-Personal |
| WIFI_SEC_TYPE_WPA2_WPA_PSK_MIX | 3 | WPA-Personal和WPA2-Personal混合 |
| WIFI_SEC_TYPE_WPAPSK | 4 | WPA-Personal |
| WIFI_SEC_TYPE_WPA | 5 | WPA-Enterprise |
| WIFI_SEC_TYPE_WPA2 | 6 | WPA2-Enterprise |
| WIFI_SEC_TYPE_SAE | 7 | SAE (Simultaneous Authentication of Equals)|
| WIFI_SEC_TYPE_WPA3_WPA2_PSK_MIX | 8 | WPA2-Personal和WPA3-Personal混合 |
| WIFI_SEC_TYPE_WPA3 | 9 | WPA3-Enterprise |
| WIFI_SEC_TYPE_OWE | 10 | OWE |
| WIFI_SEC_TYPE_WAPI_PSK | 11 | WAPI (WLAN Authentication and Privacy Infrastructure) 个人级 |
| WIFI_SEC_TYPE_WAPI_CERT | 12 | WAPI企业级 |
| WIFI_SEC_TYPE_WPA3_WPA2_MIX | 13 | WPA2-Enterprise和WPA3-Enterprise混合 |
| WIFI_SEC_TYPE_WEP_OPEN | 14 | WEP-OPEN |
| WIFI_SEC_TYPE_UNKNOWN | 15 | 其它认证类型 |

### protocol_mode_enum <a id="enum_protocol_mode_enum"></a>

```c
typedef enum {
    WIFI_MODE_UNDEFINE,        /*!< 未配置 */
    WIFI_MODE_11B,             /*!< 11b */
    WIFI_MODE_11B_G,           /*!< 11b/g */
    WIFI_MODE_11B_G_N,         /*!< 11b/g/n */
    WIFI_MODE_11B_G_N_AX,      /*!< 11b/g/n/ax */
} protocol_mode_enum;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| WIFI_MODE_UNDEFINE | 0 | 未配置 |
| WIFI_MODE_11B | 1 | 11b |
| WIFI_MODE_11B_G | 2 | 11b/g |
| WIFI_MODE_11B_G_N | 3 | 11b/g/n |
| WIFI_MODE_11B_G_N_AX | 4 | 11b/g/n/ax |

## Structures

### softap_config_stru <a id="struct_softap_config_stru"></a>

```c
typedef struct {
    int8_t ssid[WIFI_MAX_SSID_LEN];          /*!< SSID */
    int8_t pre_shared_key[WIFI_MAX_KEY_LEN]; /*!< 预共享秘钥 */
    int8_t reserved[2];
    wifi_security_enum security_type;        /*!< 安全类型 */
    int32_t channel_num;                     /*!< 信道号 */
    int32_t wifi_psk_type;                   /*!< PSK的类型 */
} softap_config_stru;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| ssid | int8_t[WIFI_MAX_SSID_LEN] | SSID，最大长度33字节（含'\0'），有效长度1~32 |
| pre_shared_key | int8_t[WIFI_MAX_KEY_LEN] | 预共享密钥，最大长度65字节（含'\0'），有效长度1~64 |
| reserved | int8_t[2] | 保留字段 |
| security_type | [wifi_security_enum](#enum_wifi_security_enum) | 安全类型 |
| channel_num | int32_t | 信道号，取值范围0~14，0表示自动选择 |
| wifi_psk_type | int32_t | PSK类型，取值0（不用提前计算PSK）或1（WEP (Wired Equivalent Privacy) 十六进制密钥） |

### softap_config_advance_stru <a id="struct_softap_config_advance_stru"></a>

```c
typedef struct {
    uint32_t beacon_interval;         /*!< beaconInterval, 范围25ms~1000ms, 默认100ms, 0表示未配置 */
    uint32_t dtim_period;             /*!< dtimPeriod, 范围1~30, 默认2, 0表示未配置 */
    uint32_t group_rekey;             /*!< groupRekey, 范围30s~86400s, 默认86400s, 0表示未配置 */
    uint32_t hidden_ssid_flag;        /*!< hiddenSsidFlag, 不隐藏：1, 隐藏：2, 默认不隐藏, 0表示未配置 */
    uint32_t gi;                      /*!< Gi, 默认auto_GI, 0表示未配置, 非零有效值需同时配置有效协议模式, 当前暂不支持 */
    protocol_mode_enum protocol_mode; /*!< 协议模式, 默认按芯片最大协议能力配置, 0表示未配置 */
} softap_config_advance_stru;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| beacon_interval | uint32_t | Beacon帧发送间隔，取值范围25~1000ms，默认100ms，0表示未配置 |
| dtim_period | uint32_t | DTIM周期，取值范围1~30，默认2，0表示未配置 |
| group_rekey | uint32_t | GTK重密钥周期，取值范围30~86400秒，默认86400秒，0表示未配置 |
| hidden_ssid_flag | uint32_t | SSID隐藏标志，1表示不隐藏，2表示隐藏，默认不隐藏，0表示未配置 |
| gi | uint32_t | 保护间隔，默认auto_GI，0表示未配置；配置非零有效值时需同时配置有效的协议模式；当前暂不支持 |
| protocol_mode | [protocol_mode_enum](#enum_protocol_mode_enum) | 协议模式，默认按芯片最大协议能力配置，0表示未配置 |

### wifi_sta_info_stru <a id="struct_wifi_sta_info_stru"></a>

```c
typedef struct {
    uint8_t mac_addr[WIFI_MAC_LEN];     /*!< MAC地址 */
    int8_t rssi;                        /*!< RSSI */
    int8_t rsv;                         /*!< 保留字段 */
    uint32_t best_rate;                 /*!< softap上一次接收相连的station报文最佳发送速率值(kbps) */
} wifi_sta_info_stru;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| mac_addr | uint8_t[WIFI_MAC_LEN] | STA的MAC地址，长度6字节 |
| rssi | int8_t | 接收信号强度指示(RSSI) |
| rsv | int8_t | 保留字段 |
| best_rate | uint32_t | 最佳发送速率(kbps) |





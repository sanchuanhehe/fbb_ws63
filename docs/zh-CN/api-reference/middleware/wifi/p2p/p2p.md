# WiFi P2P

WiFi P2P (Peer-to-Peer) 实现设备间直连通信，无需 AP (Access Point) 中转。管理层发现（设备扫描/监听）、连接建立（GO/GC 角色协商）、设备信息交换和连接断开。

P2P 角色：GO（Group Owner，类似 SoftAP）和 GC（Group Client，类似 STA (Station)）。适用文件传输、设备配对等需直连的场景。

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_p2p.h"
#include "include/middleware/services/wifi/wifi_p2p_config.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [wifi_p2p_enable](#wifi_p2p_enable) | 开启P2P接口 |
| [wifi_p2p_disable](#wifi_p2p_disable) | 关闭P2P接口 |
| [wifi_p2p_is_enabled](#wifi_p2p_is_enabled) | 获取P2P使能状态 |
| [wifi_p2p_find](#wifi_p2p_find) | 触发P2P设备扫描搜索 |
| [wifi_p2p_stop_find](#wifi_p2p_stop_find) | 停止P2P设备扫描 |
| [wifi_p2p_connect_cancel](#wifi_p2p_connect_cancel) | 停止P2P设备连接 |
| [wifi_p2p_listen](#wifi_p2p_listen) | 设置P2P设备监听时间 |
| [wifi_p2p_get_peers_info](#wifi_p2p_get_peers_info) | 获取搜索到的P2P设备信息 |
| [wifi_p2p_connect](#wifi_p2p_connect) | P2P主动连接 |
| [wifi_p2p_connect_accept](#wifi_p2p_connect_accept) | P2P接受或拒绝对端连接 |
| [wifi_p2p_disconnect](#wifi_p2p_disconnect) | P2P断连 |
| [wifi_p2p_go_get_gc_info](#wifi_p2p_go_get_gc_info) | GO获取已连接的GC信息 |
| [wifi_p2p_set_device_config](#wifi_p2p_set_device_config) | 设置P2P设备信息 |
| [wifi_p2p_get_device_config](#wifi_p2p_get_device_config) | 获取P2P设备信息 |
| [wifi_p2p_get_connect_info](#wifi_p2p_get_connect_info) | 获取P2P连接状态信息 |

## Functions

### wifi_p2p_enable <a id="wifi_p2p_enable"></a>

```c
errcode_t wifi_p2p_enable(void)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_p2p.h"
```

**功能说明**

- 开启P2P接口，初始化P2P设备并创建P2P网络接口
- P2P与SoftAP互斥，SoftAP已开启时无法使能P2P
- P2P使能后默认设置为手动连接选择模式

**前置条件**

- WiFi模块已初始化完成（wifi_is_wifi_inited()返回非0）
- SoftAP未开启（SoftAP与P2P互斥）

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | P2P接口开启成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_P2P_SUPPORT | 特性宏 | 支持P2P接口功能 | y |

### wifi_p2p_disable <a id="wifi_p2p_disable"></a>

```c
errcode_t wifi_p2p_disable(void)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_p2p.h"
```

**功能说明**

- 关闭P2P接口，停止P2P设备并释放网络资源
- GC模式下自动关闭DHCP (Dynamic Host Configuration Protocol)，GO模式下自动关闭DHCPS
- 关闭后清空P2P接口名称

**前置条件**

- WiFi模块已初始化完成
- P2P已开启

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | P2P接口关闭成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_P2P_SUPPORT | 特性宏 | 支持P2P接口功能 | y |

### wifi_p2p_is_enabled <a id="wifi_p2p_is_enabled"></a>

```c
int32_t wifi_p2p_is_enabled(void)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_p2p.h"
```

**功能说明**

- 获取P2P使能状态
- 用于查询P2P是否已初始化完成
- 返回值直接反映P2P全局使能标志

**前置条件**

- WiFi模块已初始化完成

**返回值**

- 返回类型：int32_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 1 | P2P已初始化 | P2P已通过wifi_p2p_enable()成功开启 |
| 0 | P2P未初始化 | P2P未开启或已关闭 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_P2P_SUPPORT | 特性宏 | 支持P2P接口功能 | y |

### wifi_p2p_find <a id="wifi_p2p_find"></a>

```c
errcode_t wifi_p2p_find(int32_t sec)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_p2p.h"
```

**功能说明**

- 触发P2P设备扫描搜索，在指定时间内扫描周围P2P设备
- 当sec为0时，使用默认扫描时间120秒
- 扫描时间范围5~120秒，超出范围返回失败

**前置条件**

- WiFi模块已初始化完成
- P2P已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| sec | int32_t | 扫描时间，单位秒 | 0：使用默认120秒；5~120：指定扫描时间 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | P2P扫描启动成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_P2P_SUPPORT | 特性宏 | 支持P2P接口功能 | y |

### wifi_p2p_stop_find <a id="wifi_p2p_stop_find"></a>

```c
errcode_t wifi_p2p_stop_find(void)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_p2p.h"
```

**功能说明**

- 停止P2P设备扫描
- 终止当前正在进行的P2P设备搜索过程
- 停止扫描后仍可获取已扫描到的设备信息

**前置条件**

- WiFi模块已初始化完成
- P2P已开启

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | P2P扫描停止成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_P2P_SUPPORT | 特性宏 | 支持P2P接口功能 | y |

### wifi_p2p_connect_cancel <a id="wifi_p2p_connect_cancel"></a>

```c
errcode_t wifi_p2p_connect_cancel(void)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_p2p.h"
```

**功能说明**

- 停止P2P设备连接，取消正在进行的P2P连接协商过程
- 取消连接不影响已建立的P2P组
- 连接取消后设备恢复到P2P_DEVICE_ONLY状态

**前置条件**

- WiFi模块已初始化完成
- P2P已开启

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | P2P连接取消成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_P2P_SUPPORT | 特性宏 | 支持P2P接口功能 | y |

### wifi_p2p_listen <a id="wifi_p2p_listen"></a>

```c
errcode_t wifi_p2p_listen(uint32_t period, uint32_t interval)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_p2p.h"
```

**功能说明**

- 设置P2P设备监听时间，使设备在指定周期内处于监听状态以响应探测请求
- period表示监听持续时间，interval表示一个周期的总时间
- period不能大于interval，两者不能同时为0且不能单独为0，取值范围0~65535

**前置条件**

- WiFi模块已初始化完成
- P2P已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| period | uint32_t | 监听时间，单位毫秒 | 0~65535，不能大于interval |
| interval | uint32_t | 一个周期的总时间，单位毫秒 | 0~65535，不能小于period |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| WIFI_SUCCESS(0x00) | 执行成功 | P2P监听设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_P2P_SUPPORT | 特性宏 | 支持P2P接口功能 | y |

### wifi_p2p_get_peers_info <a id="wifi_p2p_get_peers_info"></a>

```c
errcode_t wifi_p2p_get_peers_info(p2p_device_stru *dev_list, uint32_t *dev_num)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_p2p.h"
```

**功能说明**

- 获取搜索到的P2P设备信息列表
- 返回支持WPS (Wi-Fi Protected Setup) 连接方式的设备，不支持WPS的设备不进行显示
- dev_num为输入输出参数，输入时表示最大反馈设备数，输出时表示实际反馈设备数

**前置条件**

- WiFi模块已初始化完成
- P2P已开启
- 已执行过wifi_p2p_find()扫描

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| dev_list | [p2p_device_stru](#struct_p2p_device_stru)* | 搜索到的P2P设备列表，出参载体 | 非NULL，指向已申请的内存空间 |
| dev_num | uint32_t* | 最大反馈与实际反馈的P2P设备数目 | 非NULL，*dev_num不超过32 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| dev_list | [p2p_device_stru](#struct_p2p_device_stru)* | 实际搜索到的P2P设备信息列表 |
| dev_num | uint32_t* | 实际反馈的P2P设备数目 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 获取P2P设备信息成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_P2P_SUPPORT | 特性宏 | 支持P2P接口功能 | y |

### wifi_p2p_connect <a id="wifi_p2p_connect"></a>

```c
errcode_t wifi_p2p_connect(const p2p_config_stru *p2p_config)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_p2p.h"
```

**功能说明**

- P2P主动连接，根据当前P2P模式发起连接或邀请
- GC/GO模式下发起邀请，DEVICE_ONLY模式下发起连接协商
- 连接时自动判断对端是否为GO设备，决定加入组或新建组

**前置条件**

- WiFi模块已初始化完成
- P2P已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| p2p_config | const [p2p_config_stru](#struct_p2p_config_stru)* | 待连接的P2P设备网络信息 | 非NULL，go_intent范围0~15，persistent范围0~1 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | P2P连接发起成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_P2P_SUPPORT | 特性宏 | 支持P2P接口功能 | y |

### wifi_p2p_connect_accept <a id="wifi_p2p_connect_accept"></a>

```c
errcode_t wifi_p2p_connect_accept(const p2p_config_stru *p2p_config, int assoc)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_p2p.h"
```

**功能说明**

- P2P接受或拒绝对端连接请求
- assoc为1表示接受连接，0表示拒绝连接
- 调用前需校验WPS连接方式与当前配置是否匹配

**前置条件**

- WiFi模块已初始化完成
- P2P已开启
- P2P当前处于DEVICE_ONLY模式

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| p2p_config | const [p2p_config_stru](#struct_p2p_config_stru)* | 待连接的P2P设备网络信息 | 非NULL，go_intent范围0~15，persistent范围0~1 |
| assoc | int | 接受或拒绝连接标志 | 0：拒绝连接；1：接受连接 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | P2P连接接受/拒绝操作成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_P2P_SUPPORT | 特性宏 | 支持P2P接口功能 | y |

### wifi_p2p_disconnect <a id="wifi_p2p_disconnect"></a>

```c
errcode_t wifi_p2p_disconnect(void)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_p2p.h"
```

**功能说明**

- P2P断开连接，取消正在进行的连接协商并移除P2P组
- 同时执行取消连接和移除组操作
- 断连后设备恢复到P2P_DEVICE_ONLY状态

**前置条件**

- WiFi模块已初始化完成
- P2P已开启

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | P2P断连成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_P2P_SUPPORT | 特性宏 | 支持P2P接口功能 | y |

### wifi_p2p_go_get_gc_info <a id="wifi_p2p_go_get_gc_info"></a>

```c
errcode_t wifi_p2p_go_get_gc_info(p2p_client_info_stru *client_list, uint32_t *client_num)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_p2p.h"
```

**功能说明**

- P2P GO获取已连接的GC (Group Client) 信息列表
- client_num为输入输出参数，输入时表示最大反馈GC数，输出时表示实际反馈GC数
- 最大支持查询4个GC信息

**前置条件**

- WiFi模块已初始化完成
- P2P已开启，且当前设备为GO模式

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| client_list | [p2p_client_info_stru](#struct_p2p_client_info_stru)* | 反馈的GC信息列表 | 非NULL，指向已申请的内存空间 |
| client_num | uint32_t* | 最大反馈与实际反馈的GC数目 | 非NULL，*client_num不超过4 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| client_list | [p2p_client_info_stru](#struct_p2p_client_info_stru)* | 实际已连接的GC信息列表 |
| client_num | uint32_t* | 实际反馈的GC数目 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 获取GC信息成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_P2P_SUPPORT | 特性宏 | 支持P2P接口功能 | y |

### wifi_p2p_set_device_config <a id="wifi_p2p_set_device_config"></a>

```c
errcode_t wifi_p2p_set_device_config(const p2p_device_config_stru *p2p_dev_set_info)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_p2p.h"
```

**功能说明**

- 设置P2P设备信息，包括设备名称、WPS连接方式、监听信道和工作信道
- WPS连接方式仅支持PBC和PIN_DISPLAY两种模式
- GO Intent使用默认值7

**前置条件**

- WiFi模块已初始化完成
- P2P已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| p2p_dev_set_info | const [p2p_device_config_stru](#struct_p2p_device_config_stru)* | P2P设置信息 | 非NULL，wps_method仅支持[WPS_PBC](#enum_wps_method_enum)(0)或[WPS_PIN_DISPLAY](#enum_wps_method_enum)(1) |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | P2P设备信息设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_P2P_SUPPORT | 特性宏 | 支持P2P接口功能 | y |

### wifi_p2p_get_device_config <a id="wifi_p2p_get_device_config"></a>

```c
errcode_t wifi_p2p_get_device_config(p2p_device_config_stru *p2p_dev_set_info)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_p2p.h"
```

**功能说明**

- 获取P2P设备信息，包括设备名称、WPS连接方式、监听信道和工作信道
- 返回当前P2P用户配置信息

**前置条件**

- WiFi模块已初始化完成
- P2P已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| p2p_dev_set_info | [p2p_device_config_stru](#struct_p2p_device_config_stru)* | P2P设置信息，出参载体 | 非NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| p2p_dev_set_info | [p2p_device_config_stru](#struct_p2p_device_config_stru)* | 当前P2P设备配置信息 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 获取P2P设备信息成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_P2P_SUPPORT | 特性宏 | 支持P2P接口功能 | y |

### wifi_p2p_get_connect_info <a id="wifi_p2p_get_connect_info"></a>

```c
errcode_t wifi_p2p_get_connect_info(p2p_status_info_stru *status)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_p2p.h"
```

**功能说明**

- 获取P2P连接状态信息，包括工作模式、关联状态、工作信道、Group SSID (Service Set Identifier) 和Group BSSID (Basic Service Set Identifier)
- DEVICE_ONLY模式下仅返回mode信息
- CONNECTED状态下operation_channel、group_ssid、group_bssid有效

**前置条件**

- WiFi模块已初始化完成
- P2P已开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| status | [p2p_status_info_stru](#struct_p2p_status_info_stru)* | 待反馈的P2P连接状态信息 | 非NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| status | [p2p_status_info_stru](#struct_p2p_status_info_stru)* | P2P连接状态信息，包括模式、关联状态、信道、Group SSID和Group BSSID |
**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 获取P2P连接状态成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_P2P_SUPPORT | 特性宏 | 支持P2P接口功能 | y |

## Enumerations

### wps_method_enum <a id="enum_wps_method_enum"></a>

```c
typedef enum {
    WPS_PBC,           /*!< PBC方式连接。  */
    WPS_PIN_DISPLAY,   /*!< PIN DISPLAY方式连接,92不支持。  */
    WPS_PIN_KEYPAD,    /*!< PIN KEYPAD方式连接,92不支持。  */
    WPS_PIN_LABEL,     /*!< PIN LABEL方式连接,92不支持。  */
    WPS_OTHER,         /*!< 其他方式连接,92不支持。  */
} wps_method_enum;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| WPS_PBC | 0 | PBC方式连接 |
| WPS_PIN_DISPLAY | 1 | PIN DISPLAY方式连接,92不支持 |
| WPS_PIN_KEYPAD | 2 | PIN KEYPAD方式连接,92不支持 |
| WPS_PIN_LABEL | 3 | PIN LABEL方式连接,92不支持 |
| WPS_OTHER | 4 | 其他方式连接,92不支持 |

### p2p_mode_enum <a id="enum_p2p_mode_enum"></a>

```c
typedef enum {
    P2P_MODE_GC,             /*!< P2P GC模式。  */
    P2P_MODE_GO,             /*!< P2P GO模式。  */
    P2P_MODE_DEVICE_ONLY,    /*!< P2P未协商GO。  */
    P2P_MODE_BUTT,
} p2p_mode_enum;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| P2P_MODE_GC | 0 | P2P GC模式 |
| P2P_MODE_GO | 1 | P2P GO模式 |
| P2P_MODE_DEVICE_ONLY | 2 | P2P未协商GO |
| P2P_MODE_BUTT | 3 | 枚举边界值 |

### p2p_conn_state_enum <a id="enum_p2p_conn_state_enum"></a>

```c
typedef enum {
    P2P_DISCONNECTED,   /*!< 断连。  */
    P2P_CONNECTED,      /*!< 已连接。  */
    P2P_CONNECTING,     /*!< 连接中。  */
    P2P_CONN_STATUS_BUTT,
} p2p_conn_state_enum;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| P2P_DISCONNECTED | 0 | 断连 |
| P2P_CONNECTED | 1 | 已连接 |
| P2P_CONNECTING | 2 | 连接中 |
| P2P_CONN_STATUS_BUTT | 3 | 枚举边界值 |

## Structures

### wfd_info_stru <a id="struct_wfd_info_stru"></a>

```c
typedef struct wfd_info_stru {
    int16_t device_info;    /*!< WFD设备类型。  */
    int16_t ctrl_port;      /*!< 控制端口号。  */
    int16_t max_throughput; /*!< 最大吞吐量。  */
    int8_t reserved[2];
} wfd_info_stru;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| device_info | int16_t | WFD设备类型 |
| ctrl_port | int16_t | 控制端口号 |
| max_throughput | int16_t | 最大吞吐量 |
| reserved | int8_t[2] | 保留字段 |

### p2p_device_stru <a id="struct_p2p_device_stru"></a>

```c
typedef struct p2p_device_stru {
    int8_t name[WPS_DEV_NAME_LEN];  /*!< 设备device名称。  */
    uint8_t bssid[WIFI_MAC_LEN];    /*!< 设备device MAC地址。  */
    uint8_t wps_method;             /*!< 支持的WPS连接方式，
                                                  - bit0表示PBC
                                                  - bit1表示PIN LABEL
                                                  - bit2表示PIN DISPLAY
                                                  - bit3表示PIN KEYPAD
                                                  - bit4表示其他方式。  */
    uint8_t is_go           : 1;    /*!< 扫描出来的设备是否是p2p go。  */
    uint8_t enable_add_group : 1;   /*!< 扫描出的go对应的group，是否允许添加gc。  */
    uint8_t reserved       : 6;
    int8_t reserved1[3];
    wfd_info_stru wfd_info;         /*!< 暂不使用。  */
} p2p_device_stru;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| name | int8_t[33] | 设备名称，最大长度33字节 |
| bssid | uint8_t[6] | 设备MAC (Media Access Control) 地址 |
| wps_method | uint8_t | 支持的WPS连接方式位图：bit0-PBC，bit1-PIN LABEL，bit2-PIN DISPLAY，bit3-PIN KEYPAD，bit4-其他方式 |
| is_go | uint8_t:1 | 位域，扫描到的设备是否为P2P GO |
| enable_add_group | uint8_t:1 | 位域，GO对应的group是否允许添加GC |
| reserved | uint8_t:6 | 位域，保留 |
| reserved1 | int8_t[3] | 保留字段 |
| wfd_info | [wfd_info_stru](#struct_wfd_info_stru) | WFD信息，暂不使用 |

### p2p_config_stru <a id="struct_p2p_config_stru"></a>

```c
typedef struct p2p_config_stru {
    uint8_t bssid[WIFI_MAC_LEN];           /*!< peer的device MAC地址。  */
    int8_t pin[WIFI_WPS_PIN_MAX_LEN_NUM];  /*!< WPS的Pin码。  */
    uint8_t wps_method;                    /*!< WPS的连接方式。  */
    uint8_t go_intent;                     /*!< go intent，有效范围0~15。  */
    uint8_t persistent;                    /*!< 1:按照永久网络存储;0:不按照永久网络存储。  */
    int8_t reserved[2];
} p2p_config_stru;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| bssid | uint8_t[6] | 对端设备的MAC地址 |
| pin | int8_t[9] | WPS的PIN码，最大长度9字节（含'\0'） |
| wps_method | uint8_t | WPS连接方式，参考[wps_method_enum](#enum_wps_method_enum) |
| go_intent | uint8_t | GO意图值，有效范围0~15 |
| persistent | uint8_t | 永久网络存储标志：1-按永久网络存储，0-不按永久网络存储 |
| reserved | int8_t[2] | 保留字段 |

### p2p_status_info_stru <a id="struct_p2p_status_info_stru"></a>

```c
typedef struct p2p_status_info_stru {
    int32_t operation_channel;            /*!< GO/GC连接之后工作信道的中心频点，在wpa_state为CONNECTED时有效。  */
    uint8_t mode;                         /*!< P2P模式, 和P2pMode对应。  */
    uint8_t wpa_state;                    /*!< p2p关联状态，和P2pConnState对应.。  */
    int8_t group_ssid[WPS_DEV_NAME_LEN];  /*!< Group SSID，并且P2P的WPS关联成功（当前阶段组协商成功）时有效。  */
    uint8_t group_bssid[WIFI_MAC_LEN];    /*!< Group BSSID==go bssid，在wpa_state为CONNECTED，
                                                        并且P2P的WPS关联成功（当前阶段组协商成功）时有效。  */
    int8_t reserved[3];
} p2p_status_info_stru;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| operation_channel | int32_t | GO/GC连接后工作信道的中心频点，wpa_state为CONNECTED时有效 |
| mode | uint8_t | P2P模式，参考[p2p_mode_enum](#enum_p2p_mode_enum) |
| wpa_state | uint8_t | P2P关联状态，参考[p2p_conn_state_enum](#enum_p2p_conn_state_enum) |
| group_ssid | int8_t[33] | Group SSID，wpa_state为CONNECTED且P2P WPS关联成功时有效 |
| group_bssid | uint8_t[6] | Group BSSID（等于GO BSSID），wpa_state为CONNECTED且P2P WPS关联成功时有效 |
| reserved | int8_t[3] | 保留字段 |

### p2p_client_info_stru <a id="struct_p2p_client_info_stru"></a>

```c
typedef struct p2p_client_info_stru {
    uint8_t gc_bssid[WIFI_MAC_LEN];           /*!< 与go相连的client mac地址。  */
    uint8_t gc_device_bssid[WIFI_MAC_LEN];    /*!< 与go相连的client dev地址。  */
    int8_t gc_device_name[WPS_DEV_NAME_LEN];  /*!< client 设备名称。  */
    int8_t reserved[3];
} p2p_client_info_stru;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| gc_bssid | uint8_t[6] | 与GO相连的Client MAC地址 |
| gc_device_bssid | uint8_t[6] | 与GO相连的Client设备地址 |
| gc_device_name | int8_t[33] | Client设备名称，最大长度33字节 |
| reserved | int8_t[3] | 保留字段 |

### p2p_device_config_stru <a id="struct_p2p_device_config_stru"></a>

```c
typedef struct {
    int8_t dev_name[WPS_DEV_NAME_LEN];      /*!< 设备名称。  */
    uint8_t wps_method;                     /*!< 支持的WPS连接方式，可选项为WpsMethod,仅支持配置0/1/2/3。  */
    int32_t listen_channel;                 /*!< 回复probe response的信道。  */
    int32_t oper_channel;                   /*!< 建议的工作信道：0表示随机信道；非0有效值表示建议的信道。  */
} p2p_device_config_stru;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| dev_name | int8_t[33] | 设备名称，最大长度33字节 |
| wps_method | uint8_t | WPS连接方式，参考[wps_method_enum](#enum_wps_method_enum)，仅支持配置0/1/2/3 |
| listen_channel | int32_t | 回复probe response的信道 |
| oper_channel | int32_t | 建议的工作信道：0表示随机信道，非0有效值表示建议的信道 |



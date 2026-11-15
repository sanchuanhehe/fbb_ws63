# WiFi Device

WiFi STA (Station) 模式核心 API。管理 WiFi 初始化/去初始化、STA 启停、AP (Access Point) 扫描（标准/高级/信道扫描）、AP 连接/断开、重连策略、省电管理、漫游阈值、信道配置和 MAC (Media Access Control) 地址管理等。

支持 WPA/WPA2/WPA3 安全协议和 Fast Connect 快速连接。适用所有 WiFi 终端设备场景。

**头文件清单**
```c
#include "include/middleware/services/wifi/wifi_device.h"
#include "include/middleware/services/wifi/wifi_device_config.h"
#include "include/middleware/services/wifi/wifi_linked_info.h"
#include "include/middleware/services/wifi/wifi_scan_info.h"
#include "include/middleware/services/wifi/wifi_event.h"
#include "include/middleware/services/wifi/station_info.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [wifi_init](#wifi_init) | WiFi初始化 |
| [wifi_deinit](#wifi_deinit) | WiFi去初始化 |
| [wifi_is_wifi_inited](#wifi_is_wifi_inited) | 获取WiFi初始化状态 |
| [wifi_sta_enable](#wifi_sta_enable) | 开启STA |
| [wifi_sta_disable](#wifi_sta_disable) | 关闭STA |
| [wifi_is_sta_enabled](#wifi_is_sta_enabled) | 获取STA使能状态 |
| [wifi_get_dev](#wifi_get_dev) | 获取WiFi设备使能状态 |
| [wifi_sta_set_protocol_mode](#wifi_sta_set_protocol_mode) | 设置STA协议模式 |
| [wifi_sta_get_protocol_mode](#wifi_sta_get_protocol_mode) | 获取STA协议模式 |
| [wifi_sta_scan](#wifi_sta_scan) | STA全信道基础扫描 |
| [wifi_sta_scan_advance](#wifi_sta_scan_advance) | STA带特定参数的扫描 |
| [wifi_sta_set_scan_policy](#wifi_sta_set_scan_policy) | 设置STA扫描策略参数 |
| [wifi_raw_scan](#wifi_raw_scan) | STA带特定参数的原始扫描 |
| [wifi_sta_scan_stop](#wifi_sta_scan_stop) | 强制停止STA扫描 |
| [wifi_ap_get_scan_info](#wifi_ap_get_scan_info) | 获取softAP扫描结果 |
| [wifi_sta_get_scan_info](#wifi_sta_get_scan_info) | 获取STA扫描结果 |
| [wifi_sta_scan_result_clear](#wifi_sta_scan_result_clear) | 清空STA扫描结果 |
| [wifi_set_channel](#wifi_set_channel) | 切换信道 |
| [wifi_get_channel](#wifi_get_channel) | 获取信道 |
| [wifi_sta_wnm_bss_query](#wifi_sta_wnm_bss_query) | 发送BSS query报文 |
| [wifi_sta_wnm_notify](#wifi_sta_wnm_notify) | 发送WNM (Wireless Network Management) notify报文 |
| [wifi_sta_connect](#wifi_sta_connect) | STA连接网络 |
| [wifi_sta_get_last_conn_security_type](#wifi_sta_get_last_conn_security_type) | 获取上一次连接AP的加密方式 |
| [wifi_get_last_conn_vendor_ie](#wifi_get_last_conn_vendor_ie) | 获取上一次连接AP的vendor IE (Information Element) |
| [wifi_sta_disconnect](#wifi_sta_disconnect) | STA断开连接 |
| [wifi_sta_get_ap_info](#wifi_sta_get_ap_info) | 获取STA连接的网络状态 |
| [wifi_sta_set_reconnect_policy](#wifi_sta_set_reconnect_policy) | 设置STA重连策略 |
| [wifi_sta_set_pmf_mode](#wifi_sta_set_pmf_mode) | 配置STA的PMF (Protected Management Frames) 模式 |
| [wifi_sta_get_pmf_mode](#wifi_sta_get_pmf_mode) | 获取STA的PMF设置 |
| [wifi_sta_get_connect_status_code](#wifi_sta_get_connect_status_code) | 查询Authentication与Association帧status code |
| [wifi_set_mgmt_frame_rx_cb](#wifi_set_mgmt_frame_rx_cb) | 注册管理帧收包回调 |
| [wifi_set_promis_mode](#wifi_set_promis_mode) | 设置混杂模式 |
| [wifi_set_promis_rx_pkt_cb](#wifi_set_promis_rx_pkt_cb) | 注册混杂模式收包回调 |
| [wifi_sta_fast_connect](#wifi_sta_fast_connect) | STA快速连接网络 |
| [wifi_register_event_cb](#wifi_register_event_cb) | 注册WiFi事件回调 |
| [wifi_unregister_event_cb](#wifi_unregister_event_cb) | 去注册WiFi事件回调 |
| [wifi_set_app_ie](#wifi_set_app_ie) | 在管理帧中添加用户IE字段 |
| [wifi_del_app_ie](#wifi_del_app_ie) | 在管理帧中删除用户IE字段 |
| [wifi_set_wow_pattern](#wifi_set_wow_pattern) | 设置WoW (Wake on Wireless)Pattern模式 |
| [wifi_set_wow_sleep_mode](#wifi_set_wow_sleep_mode) | WoW休眠使能配置 |
| [wifi_csi_start](#wifi_csi_start) | 开启CSI (Channel State Information) 上报 |
| [wifi_csi_stop](#wifi_csi_stop) | 关闭CSI上报 |
| [wifi_set_csi_config](#wifi_set_csi_config) | 配置CSI参数 |
| [wifi_register_csi_report_cb](#wifi_register_csi_report_cb) | 注册CSI数据上报回调 |
| [wifi_send_custom_pkt](#wifi_send_custom_pkt) | 发送用户定制报文 |
| [wifi_set_pkt_retry_policy](#wifi_set_pkt_retry_policy) | 设置数据帧和管理帧最大软件重传次数 |
| [wifi_reset_mac_phy](#wifi_reset_mac_phy) | 复位MAC和PHY (Physical Layer) 接口 |
| [wifi_set_linkloss_config](#wifi_set_linkloss_config) | 设置Linkloss参数 |
| [wifi_set_base_mac_addr](#wifi_set_base_mac_addr) | 设置基础MAC地址 |
| [wifi_get_base_mac_addr](#wifi_get_base_mac_addr) | 获取基础MAC地址 |
| [wifi_softap_set_mac_addr](#wifi_softap_set_mac_addr) | 设置softAP MAC地址 |
| [wifi_softap_get_mac_addr](#wifi_softap_get_mac_addr) | 获取softAP MAC地址 |
| [wifi_set_mac_derivation_ptr](#wifi_set_mac_derivation_ptr) | 设置MAC派生策略回调 |
| [wifi_set_low_current_boot_mode](#wifi_set_low_current_boot_mode) | 设置低启动电流模式 |
| [wifi_get_country_code](#wifi_get_country_code) | 获取国家码 |
| [wifi_set_country_code](#wifi_set_country_code) | 设置国家码 |
| [wifi_sta_set_pm](#wifi_sta_set_pm) | 设置低功耗模式 |
| [wifi_set_sdp_mode](#wifi_set_sdp_mode) | 设置SDP (Service Discovery Protocol) 模式 |
| [wifi_set_sdp_subscribe](#wifi_set_sdp_subscribe) | 设置SDP订阅 |
| [wifi_sta_wps_pbc](#wifi_sta_wps_pbc) | WPS (Wi-Fi Protected Setup) PBC连接 |
| [wifi_sta_wps_pin](#wifi_sta_wps_pin) | WPS PIN连接 |
| [wifi_sta_get_wps_pin](#wifi_sta_get_wps_pin) | 获取WPS PIN码 |
| [wifi_set_psd_mode](#wifi_set_psd_mode) | 设置PSD (Power Spectral Density) 模式 |
| [wifi_set_psd_cb](#wifi_set_psd_cb) | 设置PSD回调接口 |
| [wifi_sta_config_conn_paras](#wifi_sta_config_conn_paras) | 配置STA连接参数 |
| [wifi_sta_get_rssi_of_data_frame](#wifi_sta_get_rssi_of_data_frame) | 获取最后一帧数据帧信号强度 |

## Functions

### wifi_init <a id="wifi_init"></a>

```c
errcode_t wifi_init(void)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- WiFi模块初始化，完成WiFi子系统资源分配与底层驱动加载
- 必须在所有WiFi功能接口调用前执行
- 重复调用返回错误码

**前置条件**

- WiFi硬件已上电就绪
- 系统内核及OSAL (Operating System Abstraction Layer) 模块已初始化完成

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | WiFi初始化成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_deinit <a id="wifi_deinit"></a>

```c
errcode_t wifi_deinit(void)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- WiFi去初始化，释放WiFi子系统资源
- 需在WiFi模块不再使用时调用
- 调用前需关闭STA/AP等已使能的接口

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA/AP接口已关闭

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | WiFi去初始化成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_is_wifi_inited <a id="wifi_is_wifi_inited"></a>

```c
int32_t wifi_is_wifi_inited(void)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 查询WiFi模块是否已初始化
- 用于其他模块判断WiFi初始化状态

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 1 | WiFi已初始化 | wifi_init()已成功调用 |
| 0 | WiFi未初始化 | wifi_init()未调用或未成功 |

**参考案例**

- [radar_slp_wireless_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/slp_wireless_sample/radar_slp_wireless_sample.c)

### wifi_sta_enable <a id="wifi_sta_enable"></a>

```c
errcode_t wifi_sta_enable(void)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 开启STA模式
- 使能后可进行扫描、连接等STA操作
- 需先调用wifi_init()完成WiFi初始化

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA未使能

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | STA使能成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [radar_slp_wireless_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/slp_wireless_sample/radar_slp_wireless_sample.c)

### wifi_sta_disable <a id="wifi_sta_disable"></a>

```c
errcode_t wifi_sta_disable(void)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 关闭STA模式
- 关闭后STA相关功能不可用
- 需先断开当前STA连接

**前置条件**

- STA已通过wifi_sta_enable()使能
- STA已断开与AP的连接

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | STA关闭成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [radar_softap_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/softap_sample/radar_softap_sample.c)

### wifi_is_sta_enabled <a id="wifi_is_sta_enabled"></a>

```c
int32_t wifi_is_sta_enabled(void)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 查询STA是否已使能
- 用于判断STA模式当前状态

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 1 | STA已使能 | wifi_sta_enable()已成功调用 |
| 0 | STA未使能 | wifi_sta_enable()未调用或未成功 |

**参考案例**

- [hilink_network_adapter.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/hilink_network_adapter.c)

### wifi_get_dev <a id="wifi_get_dev"></a>

```c
wifi_dev_t *wifi_get_dev(wifi_iftype_t iftype)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 根据接口类型获取WiFi设备结构体指针
- 返回NULL表示设备未使能
- 用于获取设备信息及操作句柄

**前置条件**

- WiFi已通过wifi_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| [in] iftype | [wifi_iftype_t](#enum_wifi_iftype_t) | WiFi接口类型 | WIFI_IFTYPE_STATION(2)、WIFI_IFTYPE_AP(3)、WIFI_IFTYPE_P2P_CLIENT、WIFI_IFTYPE_P2P_GO、WIFI_IFTYPE_P2P_DEVICE |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 非NULL | WiFi设备结构体指针 | 设备已使能 |
| NULL | 设备未使能 | 设备未初始化 |

### wifi_sta_set_protocol_mode <a id="wifi_sta_set_protocol_mode"></a>

```c
errcode_t wifi_sta_set_protocol_mode(protocol_mode_enum mode)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 设置STA的协议模式
- 协议模式决定STA支持的802.11协议标准
- 需在STA使能前设置

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| mode | [protocol_mode_enum](#enum_protocol_mode_enum) | 协议模式 | WIFI_MODE_UNDEFINE(0)、WIFI_MODE_11B(1)、WIFI_MODE_11B_G(2)、WIFI_MODE_11B_G_N(3)、WIFI_MODE_11B_G_N_AX(4) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_sta_get_protocol_mode <a id="wifi_sta_get_protocol_mode"></a>

```c
protocol_mode_enum wifi_sta_get_protocol_mode(void)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 获取STA当前协议模式
- 返回值对应[protocol_mode_enum](#enum_protocol_mode_enum)枚举成员

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已使能

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| mode | [protocol_mode_enum](#enum_protocol_mode_enum) | 当前STA协议模式 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| protocol_mode_enum | STA协议模式 | 成功获取 |

### wifi_sta_scan <a id="wifi_sta_scan"></a>

```c
errcode_t wifi_sta_scan(void)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 启动STA全信道基础扫描
- 扫描结果通过事件回调或wifi_sta_get_scan_info()获取
- 扫描完成触发WIFI_STATE_AVALIABLE事件

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已通过wifi_sta_enable()使能

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 扫描启动成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [radar_slp_wireless_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/slp_wireless_sample/radar_slp_wireless_sample.c)

### wifi_sta_scan_advance <a id="wifi_sta_scan_advance"></a>

```c
errcode_t wifi_sta_scan_advance(const wifi_scan_params_stru *scan_param)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 启动STA带特定参数的扫描
- 支持指定SSID (Service Set Identifier)、BSSID (Basic Service Set Identifier)、信道等过滤条件
- 扫描结果通过事件回调或wifi_sta_get_scan_info()获取

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已通过wifi_sta_enable()使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| scan_param | const [wifi_scan_params_stru](#struct_wifi_scan_params_stru) * | 扫描网络参数设置 | 非NULL，指向有效扫描参数结构体 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 扫描启动成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [hilink_network_adapter.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/hilink_network_adapter.c)

### wifi_sta_set_scan_policy <a id="wifi_sta_set_scan_policy"></a>

```c
errcode_t wifi_sta_set_scan_policy(wifi_if_type_enum iftype, wifi_scan_strategy_stru *scan_strategy)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 设置STA扫描策略参数
- 可配置每个信道停留时间、扫描slot数、probe req发送次数等

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已通过wifi_sta_enable()使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| iftype | [wifi_if_type_enum](#enum_wifi_if_type_enum) | 接口类型 | IFTYPE_STA(0)、IFTYPE_AP(1) |
| scan_strategy | [wifi_scan_strategy_stru](#struct_wifi_scan_strategy_stru) * | 扫描策略参数配置 | 非NULL，指向有效策略结构体 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [hilink_network_adapter.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/hilink_network_adapter.c)

### wifi_raw_scan <a id="wifi_raw_scan"></a>

```c
errcode_t wifi_raw_scan(wifi_scan_params_stru *scan_param, wifi_scan_no_save_cb cb)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 启动STA带特定参数的原始扫描，不经过wpa，回调由用户指定
- 扫描结果通过回调函数直接返回，不保存到扫描缓存
- 回调入参为wifi_scan_info_stru类型

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已通过wifi_sta_enable()使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| scan_param | [wifi_scan_params_stru](#struct_wifi_scan_params_stru) * | 扫描网络参数设置 | 非NULL |
| cb | [wifi_scan_no_save_cb](#typedef_wifi_scan_no_save_cb) | 扫描完成回调函数 | 非NULL |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 原始扫描启动成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_sta_scan_stop <a id="wifi_sta_scan_stop"></a>

```c
errcode_t wifi_sta_scan_stop(void)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 强制停止STA全信道扫描
- 扫描正在进行时调用此接口可中止扫描

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已通过wifi_sta_enable()使能
- 扫描正在进行中

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 扫描停止成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_ap_get_scan_info <a id="wifi_ap_get_scan_info"></a>

```c
errcode_t wifi_ap_get_scan_info(wifi_scan_info_stru *result, uint32_t *size)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 获取softAP扫描结果
- 扫描结果存入result数组，size返回扫描到的网络数目
- 需在扫描完成后调用

**前置条件**

- WiFi已通过wifi_init()初始化完成
- 扫描已完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| [out] result | [wifi_scan_info_stru](#struct_wifi_scan_info_stru) * | 扫描结果输出缓冲区 | 非NULL，指向有效内存空间 |
| [in/out] size | uint32_t * | 扫描到的网络数目 | 非NULL，输入时为缓冲区最大容量，输出时为实际数目 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| result | [wifi_scan_info_stru](#struct_wifi_scan_info_stru) | 扫描结果数组 |
| size | uint32_t | 扫描到的网络数目 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 获取成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_sta_get_scan_info <a id="wifi_sta_get_scan_info"></a>

```c
errcode_t wifi_sta_get_scan_info(wifi_scan_info_stru *result, uint32_t *size)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 获取STA扫描结果
- 扫描结果存入result数组，size返回扫描到的网络数目
- 需在扫描完成后调用

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已使能且扫描已完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| [out] result | [wifi_scan_info_stru](#struct_wifi_scan_info_stru) * | 扫描结果输出缓冲区 | 非NULL，指向有效内存空间 |
| [in/out] size | uint32_t * | 扫描到的网络数目 | 非NULL，输入时为缓冲区最大容量，输出时为实际数目 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| result | [wifi_scan_info_stru](#struct_wifi_scan_info_stru) | 扫描结果数组 |
| size | uint32_t | 扫描到的网络数目 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 获取成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [radar_slp_wireless_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/slp_wireless_sample/radar_slp_wireless_sample.c)

### wifi_sta_scan_result_clear <a id="wifi_sta_scan_result_clear"></a>

```c
errcode_t wifi_sta_scan_result_clear(void)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 清空STA扫描结果缓存
- 清空后无法再通过wifi_sta_get_scan_info()获取上次扫描结果

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已使能

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 清空成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_set_channel <a id="wifi_set_channel"></a>

```c
errcode_t wifi_set_channel(wifi_if_type_enum iftype, int32_t channel)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 设置WiFi工作信道
- 根据接口类型指定STA或AP的信道

**前置条件**

- WiFi已通过wifi_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| iftype | [wifi_if_type_enum](#enum_wifi_if_type_enum) | 接口类型 | IFTYPE_STA(0)、IFTYPE_AP(1) |
| channel | int32_t | 信道号 | 1~14(2.4GHz) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_get_channel <a id="wifi_get_channel"></a>

```c
errcode_t wifi_get_channel(wifi_if_type_enum iftype, int32_t *channel)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 获取WiFi当前工作信道
- 根据接口类型获取STA或AP的信道

**前置条件**

- WiFi已通过wifi_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| iftype | [wifi_if_type_enum](#enum_wifi_if_type_enum) | 接口类型 | IFTYPE_STA(0)、IFTYPE_AP(1) |
| channel | int32_t * | 信道号输出 | 非NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| channel | int32_t | 当前工作信道号 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 获取成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_sta_wnm_bss_query <a id="wifi_sta_wnm_bss_query"></a>

```c
errcode_t wifi_sta_wnm_bss_query(int32_t reason_code, int32_t candidate_list)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 发送BSS query报文
- 用于WNMBSS转换管理

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| reason_code | int32_t | 原因码 | 有效reason code值 |
| candidate_list | int32_t | 候选列表标记 | 0或1 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_WNM | 特性宏 | 支持WNM BSS转换管理功能 | 由构建目标决定 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 发送成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_sta_wnm_notify <a id="wifi_sta_wnm_notify"></a>

```c
errcode_t wifi_sta_wnm_notify(const char *param, uint32_t len)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 发送WNM notify报文
- 用于WNM通知交互

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| param | const char * | WNM notify参数 | 非NULL |
| len | uint32_t | 参数长度 | 大于0 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_WNM | 特性宏 | 支持WNM BSS转换管理功能 | 由构建目标决定 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 发送成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_sta_connect <a id="wifi_sta_connect"></a>

```c
errcode_t wifi_sta_connect(const wifi_sta_config_stru *config)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- STA发起连接网络请求
- 根据配置参数连接指定AP
- 连接结果通过事件回调通知

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已通过wifi_sta_enable()使能
- 已完成扫描获取目标AP信息

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| config | const [wifi_sta_config_stru](#struct_wifi_sta_config_stru) * | 连接网络参数设置 | 非NULL，指向有效连接配置结构体 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 连接请求发起成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [radar_slp_wireless_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/slp_wireless_sample/radar_slp_wireless_sample.c)

### wifi_sta_get_last_conn_security_type <a id="wifi_sta_get_last_conn_security_type"></a>

```c
errcode_t wifi_sta_get_last_conn_security_type(wifi_conn_sec_stru *param)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 获取上一次连接AP的加密方式
- 返回加密类型及pairwise加密信息

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| param | [wifi_conn_sec_stru](#struct_wifi_conn_sec_stru) * | 加密方式输出 | 非NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| param | [wifi_conn_sec_stru](#struct_wifi_conn_sec_stru) | 上一次连接AP的加密方式 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 获取成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_get_last_conn_vendor_ie <a id="wifi_get_last_conn_vendor_ie"></a>

```c
errcode_t wifi_get_last_conn_vendor_ie(uint8_t *mac, uint16_t mac_len, uint8_t *out_buffer, uint16_t out_buffer_len)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 获取上一次连接AP的vendor IE
- 根据AP的MAC地址查询上次连接的vendor IE信息

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| mac | uint8_t * | AP的MAC地址 | 非NULL，长度为mac_len |
| mac_len | uint16_t | MAC地址长度 | 有效长度值 |
| out_buffer | uint8_t * | 输出缓冲区 | 非NULL，空间不小于out_buffer_len |
| out_buffer_len | uint16_t | 输出缓冲区最大长度 | 大于0 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| out_buffer | uint8_t | vendor IE数据 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 获取成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_sta_disconnect <a id="wifi_sta_disconnect"></a>

```c
errcode_t wifi_sta_disconnect(void)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- STA断开与当前连接的网络
- 断连结果通过事件回调通知

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已使能且处于已连接状态

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 断连请求发起成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [hilink_network_adapter.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/hilink_network_adapter.c)

### wifi_sta_get_ap_info <a id="wifi_sta_get_ap_info"></a>

```c
errcode_t wifi_sta_get_ap_info(wifi_linked_info_stru *result)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 获取STA当前连接的网络状态信息
- 包括SSID、BSSID、RSSI (Received Signal Strength Indicator)、连接状态等

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| [out] result | [wifi_linked_info_stru](#struct_wifi_linked_info_stru) * | 连接状态输出 | 非NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| result | [wifi_linked_info_stru](#struct_wifi_linked_info_stru) | STA连接的AP信息 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 获取成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [radar_wireless_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/wireless_sample/radar_wireless_sample.c)

### wifi_sta_set_reconnect_policy <a id="wifi_sta_set_reconnect_policy"></a>

```c
errcode_t wifi_sta_set_reconnect_policy(int32_t enable, uint32_t seconds, uint32_t period, uint32_t max_try_count)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 设置STA重连网络策略
- 可配置重连使能、单次超时、重连间隔、最大重连次数
- 断连后根据策略自动发起重连

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| enable | int32_t | 使能重连网络 | 0:关闭，1:开启 |
| seconds | uint32_t | 单次重连超时时间(秒) | 2~65535 |
| period | uint32_t | 重连间隔周期(秒) | 1~65535 |
| max_try_count | uint32_t | 最大重连次数 | 1~65535 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [radar_slp_wireless_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/slp_wireless_sample/radar_slp_wireless_sample.c)

### wifi_sta_set_pmf_mode <a id="wifi_sta_set_pmf_mode"></a>

```c
errcode_t wifi_sta_set_pmf_mode(wifi_pmf_option_enum pmf)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 配置STA的PMF模式
- PMF用于管理帧保护，增强安全性

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pmf | [wifi_pmf_option_enum](#enum_wifi_pmf_option_enum) | PMF模式 | WIFI_MGMT_FRAME_PROTECTION_CLOSE(0)、WIFI_MGMT_FRAME_PROTECTION_OPTIONAL(1)、WIFI_MGMT_FRAME_PROTECTION_REQUIRED(2) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_sta_get_pmf_mode <a id="wifi_sta_get_pmf_mode"></a>

```c
wifi_pmf_option_enum wifi_sta_get_pmf_mode(void)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 获取STA当前PMF设置
- 返回PMF模式枚举值

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已使能

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| pmf_mode | [wifi_pmf_option_enum](#enum_wifi_pmf_option_enum) | 当前PMF管理帧保护模式 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| [wifi_pmf_option_enum](#enum_wifi_pmf_option_enum)枚举值 | PMF模式 | 成功获取 |
| WIFI_MGMT_FRAME_PROTECTION_BUTT(3) | 获取失败 | 执行失败 |

### wifi_sta_get_connect_status_code <a id="wifi_sta_get_connect_status_code"></a>

```c
int16_t wifi_sta_get_connect_status_code(void)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 查询Authentication与Association帧的status code
- 用于诊断连接失败原因

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已使能

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| status_code | int16_t | STA连接状态码 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| Status Codes value | 状态码值 | 成功获取 |
| -1 | 执行失败 | 获取失败 |

### wifi_set_mgmt_frame_rx_cb <a id="wifi_set_mgmt_frame_rx_cb"></a>

```c
errcode_t wifi_set_mgmt_frame_rx_cb(wifi_rx_mgmt_cb data_cb, uint8_t mode)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 注册管理帧收包回调函数
- 收到管理帧后通过回调通知上层
- 可配置上报管理帧模式

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| data_cb | [wifi_rx_mgmt_cb](#typedef_wifi_rx_mgmt_cb) | 管理帧上报回调函数 | 非NULL |
| mode | uint8_t | 上报管理帧模式 | 有效模式值 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 注册成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_set_promis_mode <a id="wifi_set_promis_mode"></a>

```c
errcode_t wifi_set_promis_mode(wifi_if_type_enum iftype, int32_t enable, const wifi_ptype_filter_stru *filter)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 设置混杂模式（Monitor/Promiscuous mode）
- 可配置接收帧类型过滤规则
- 开启后可接收非本地址的数据帧

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| iftype | [wifi_if_type_enum](#enum_wifi_if_type_enum) | 接口类型 | IFTYPE_STA(0)、IFTYPE_AP(1) |
| enable | int32_t | 开启/关闭 | 0:关闭，1:开启 |
| filter | const [wifi_ptype_filter_stru](#struct_wifi_ptype_filter_stru) * | 帧类型过滤列表 | 开启时非NULL |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_set_promis_rx_pkt_cb <a id="wifi_set_promis_rx_pkt_cb"></a>

```c
errcode_t wifi_set_promis_rx_pkt_cb(wifi_promis_cb data_cb)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 注册混杂模式的收包回调函数
- 混杂模式下收到数据帧后通过回调通知

**前置条件**

- WiFi已通过wifi_init()初始化完成
- 混杂模式已通过wifi_set_promis_mode()开启

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| data_cb | [wifi_promis_cb](#typedef_wifi_promis_cb) | 混杂模式回调函数 | 非NULL |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 注册成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_sta_fast_connect <a id="wifi_sta_fast_connect"></a>

```c
errcode_t wifi_sta_fast_connect(const wifi_fast_connect_stru *fast_request)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- STA快速连接网络
- 使用预计算的PSK (Pre-Shared Key) 和信道信息加速连接
- 适用于已知AP信息的重连场景

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已使能
- 已有目标AP的连接信息（含PSK）

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| fast_request | const [wifi_fast_connect_stru](#struct_wifi_fast_connect_stru) * | 快速连接网络参数 | 非NULL，指向有效快速连接参数结构体 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 快速连接请求发起成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_register_event_cb <a id="wifi_register_event_cb"></a>

```c
errcode_t wifi_register_event_cb(const wifi_event_stru *event)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 注册WiFi事件回调函数
- 包括连接状态变化、扫描状态变化、AP状态变化、STA加入/离开等事件
- 注册后相关事件通过回调通知上层

**前置条件**

- WiFi已通过wifi_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| event | const [wifi_event_stru](#struct_wifi_event_stru) * | 事件回调函数结构体 | 非NULL |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 注册成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**参考案例**

- [radar_slp_wireless_sample.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/radar/slp_wireless_sample/radar_slp_wireless_sample.c)

### wifi_unregister_event_cb <a id="wifi_unregister_event_cb"></a>

```c
errcode_t wifi_unregister_event_cb(const wifi_event_stru *event)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 去注册WiFi事件回调函数
- 去注册后不再接收对应WiFi事件通知

**前置条件**

- WiFi已通过wifi_init()初始化完成
- 已通过wifi_register_event_cb()注册回调

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| event | const [wifi_event_stru](#struct_wifi_event_stru) * | 待撤销的回调函数结构体 | 非NULL，与注册时一致 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 去注册成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_set_app_ie <a id="wifi_set_app_ie"></a>

```c
errcode_t wifi_set_app_ie(wifi_if_type_enum iftype, ie_index_enmu ie_index, uint8_t frame_type_bitmap, const uint8_t *ie, uint16_t ie_len)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 在管理帧中添加用户自定义IE
- 最多支持4个用户IE的插入
- 可指定插入IE的帧类型（beacon、probe request、probe response）

**前置条件**

- WiFi已通过wifi_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| iftype | [wifi_if_type_enum](#enum_wifi_if_type_enum) | 接口类型 | IFTYPE_STA(0)、IFTYPE_AP(1) |
| ie_index | [ie_index_enmu](#enum_ie_index_enmu) | IE索引 | IE_FIRST(0)、IE_SECOND(1)、IE_THIRD(2)、IE_FORTH(3) |
| frame_type_bitmap | uint8_t | 可插入IE的帧类型位图 | bit0:beacon，bit1:probe request，bit2:probe response |
| ie | const uint8_t * | 用户IE字段内容 | 非NULL |
| ie_len | uint16_t | 用户IE字段内容长度 | 大于0 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 添加成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_del_app_ie <a id="wifi_del_app_ie"></a>

```c
errcode_t wifi_del_app_ie(wifi_if_type_enum iftype, ie_index_enmu ie_index, uint8_t frame_type_bitmap)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 在管理帧中删除用户自定义IE
- 根据索引和帧类型位图删除指定IE

**前置条件**

- WiFi已通过wifi_init()初始化完成
- 已通过wifi_set_app_ie()添加过IE

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| iftype | [wifi_if_type_enum](#enum_wifi_if_type_enum) | 接口类型 | IFTYPE_STA(0)、IFTYPE_AP(1) |
| ie_index | [ie_index_enmu](#enum_ie_index_enmu) | IE索引 | IE_FIRST(0)、IE_SECOND(1)、IE_THIRD(2)、IE_FORTH(3) |
| frame_type_bitmap | uint8_t | 可删除IE的帧类型位图 | bit0:beacon，bit1:probe request，bit2:probe response |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 删除成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_set_wow_pattern <a id="wifi_set_wow_pattern"></a>

```c
errcode_t wifi_set_wow_pattern(int32_t type, uint8_t index, int8_t *pattern)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 设置WoWPattern模式
- 可配置TCP/UDP模式匹配pattern
- 用于低功耗唤醒场景

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| type | int32_t | Wow Pattern模式 | 有效pattern类型值 |
| index | uint8_t | 位置索引 | 有效索引值 |
| pattern | int8_t * | 16进制TCP/UDP pattern数据 | 非NULL |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| _PRE_WLAN_FEATURE_WOW_OFFLOAD | 特性宏 | 支持WoW Offload功能 | 由构建目标决定 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_set_wow_sleep_mode <a id="wifi_set_wow_sleep_mode"></a>

```c
errcode_t wifi_set_wow_sleep_mode(uint8_t en)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- WoW休眠使能配置
- 仅在STA模式下才能使能WoW休眠
- 0代表WoW休眠去使能，1代表WoW休眠使能

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| en | uint8_t | 使能/去使能WoW休眠 | 0:去使能，1:使能 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| _PRE_WLAN_FEATURE_WOW_OFFLOAD | 特性宏 | 支持WoW Offload功能 | 由构建目标决定 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_csi_start <a id="wifi_csi_start"></a>

```c
errcode_t wifi_csi_start(void)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 开启CSI上报
- 需先通过wifi_set_csi_config()配置CSI参数
- 需先通过wifi_register_csi_report_cb()注册回调

**前置条件**

- WiFi已通过wifi_init()初始化完成
- CSI已通过wifi_set_csi_config()配置完成
- CSI上报回调已通过wifi_register_csi_report_cb()注册

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | CSI上报开启成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_csi_stop <a id="wifi_csi_stop"></a>

```c
errcode_t wifi_csi_stop(void)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 关闭CSI上报
- 停止后不再接收CSI数据回调

**前置条件**

- WiFi已通过wifi_init()初始化完成
- CSI已通过wifi_csi_start()开启

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | CSI上报关闭成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_set_csi_config <a id="wifi_set_csi_config"></a>

```c
errcode_t wifi_set_csi_config(const int8_t *ifname, const csi_config_stru *config)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 配置CSI参数
- 包括用户ID、白名单、帧类型过滤、上报周期等
- 需在wifi_csi_start()之前调用

**前置条件**

- WiFi已通过wifi_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| ifname | const int8_t * | 使能CSI的接口名 | 非NULL，如"wlan0" |
| config | const [csi_config_stru](#struct_csi_config_stru) * | CSI配置参数 | 非NULL |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 配置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_register_csi_report_cb <a id="wifi_register_csi_report_cb"></a>

```c
errcode_t wifi_register_csi_report_cb(wifi_csi_data_cb data_cb)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 注册CSI数据上报回调函数
- CSI数据通过回调函数上报给上层
- 需在wifi_csi_start()之前调用

**前置条件**

- WiFi已通过wifi_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| data_cb | [wifi_csi_data_cb](#typedef_wifi_csi_data_cb) | CSI数据上报回调函数 | 非NULL |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 注册成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_send_custom_pkt <a id="wifi_send_custom_pkt"></a>

```c
errcode_t wifi_send_custom_pkt(const wifi_if_type_enum iftype, const uint8_t *data, uint32_t len)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 发送用户定制报文，报文须按照802.11协议格式封装

**前置条件**

- WiFi已通过wifi_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| iftype | [wifi_if_type_enum](#enum_wifi_if_type_enum) | 接口类型 | IFTYPE_STA/IFTYPE_AP/IFTYPE_P2P_CLIENT/IFTYPE_P2P_GO/IFTYPE_P2P_DEVICE |
| data | const uint8_t* | 待发送帧的内容 | 非NULL，须按802.11协议格式封装 |
| len | uint32_t | 待发送的帧长度 | 大于0 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 发送成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_set_pkt_retry_policy <a id="wifi_set_pkt_retry_policy"></a>

```c
errcode_t wifi_set_pkt_retry_policy(uint8_t type, uint8_t limit)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 设置数据帧和管理帧的最大软件重传次数

**前置条件**

- WiFi已通过wifi_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| type | uint8_t | 帧类型 | 数据帧/管理帧 |
| limit | uint8_t | 需要设置的软件最大重传次数 | 大于0 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_reset_mac_phy <a id="wifi_reset_mac_phy"></a>

```c
errcode_t wifi_reset_mac_phy(void)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 复位MAC和PHY接口，解决MAC、PHY挂死问题

**前置条件**

- WiFi已通过wifi_init()初始化完成

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 复位成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_set_linkloss_config <a id="wifi_set_linkloss_config"></a>

```c
errcode_t wifi_set_linkloss_config(linkloss_paras_stru *linkloss_paras)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 设置Linkloss的参数

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已通过wifi_sta_enable()使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| linkloss_paras | [linkloss_paras_stru](#struct_linkloss_paras_stru)* | 设置linkloss相关参数 | 非NULL |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_set_base_mac_addr <a id="wifi_set_base_mac_addr"></a>

```c
errcode_t wifi_set_base_mac_addr(const int8_t *mac_addr, uint8_t mac_len)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 设置基础MAC地址

**前置条件**

- WiFi已通过wifi_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| mac_addr | const int8_t* | MAC地址指针 | 非NULL |
| mac_len | uint8_t | MAC地址长度 | WIFI_MAC_LEN(6) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_get_base_mac_addr <a id="wifi_get_base_mac_addr"></a>

```c
errcode_t wifi_get_base_mac_addr(int8_t *mac_addr, uint8_t mac_len)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 获取基础MAC地址

**前置条件**

- WiFi已通过wifi_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| [out] mac_addr | int8_t* | MAC地址指针 | 非NULL，用于存储获取的MAC地址 |
| mac_len | uint8_t | MAC地址长度 | WIFI_MAC_LEN(6) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 获取成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_softap_set_mac_addr <a id="wifi_softap_set_mac_addr"></a>

```c
errcode_t wifi_softap_set_mac_addr(const int8_t *mac_addr, uint8_t mac_len)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 设置SoftAP的MAC地址

**前置条件**

- WiFi已通过wifi_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| mac_addr | const int8_t* | MAC地址指针 | 非NULL |
| mac_len | uint8_t | MAC地址长度 | WIFI_MAC_LEN(6) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_softap_get_mac_addr <a id="wifi_softap_get_mac_addr"></a>

```c
errcode_t wifi_softap_get_mac_addr(int8_t *mac_addr, uint8_t mac_len)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 获取SoftAP的MAC地址

**前置条件**

- WiFi已通过wifi_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| [out] mac_addr | int8_t* | MAC地址指针 | 非NULL，用于存储获取的MAC地址 |
| mac_len | uint8_t | MAC地址长度 | WIFI_MAC_LEN(6) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 获取成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_set_mac_derivation_ptr <a id="wifi_set_mac_derivation_ptr"></a>

```c
errcode_t wifi_set_mac_derivation_ptr(wifi_mac_derivation_ptr ptr)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 设置MAC派生策略

**前置条件**

- WiFi已通过wifi_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| ptr | [wifi_mac_derivation_ptr](#typedef_wifi_mac_derivation_ptr) | 派生方法指针 | 非NULL |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_set_low_current_boot_mode <a id="wifi_set_low_current_boot_mode"></a>

```c
errcode_t wifi_set_low_current_boot_mode(uint8_t flag)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 设置低启动电流模式

**前置条件**

- WiFi已通过wifi_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| flag | uint8_t | 模式设置 | 0:关闭 / 1:开启 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_get_country_code <a id="wifi_get_country_code"></a>

```c
errcode_t wifi_get_country_code(int8_t *country_code, uint8_t *len)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 获取国家码

**前置条件**

- WiFi已通过wifi_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| [out] country_code | int8_t* | 国家码 | 非NULL，用于存储获取的国家码 |
| [out] len | uint8_t* | 国家码数组长度 | 非NULL |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 获取成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_set_country_code <a id="wifi_set_country_code"></a>

```c
errcode_t wifi_set_country_code(const int8_t* country_code, uint8_t len)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 设置国家码

**前置条件**

- WiFi已通过wifi_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| country_code | const int8_t* | 国家码 | 非NULL |
| len | uint8_t | 国家码数组长度 | 大于0 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_sta_set_pm <a id="wifi_sta_set_pm"></a>

```c
errcode_t wifi_sta_set_pm(uint8_t ps_switch)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 设置低功耗模式

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已通过wifi_sta_enable()使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| ps_switch | uint8_t | 低功耗模式 | 0:关闭 / 1:开启 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_set_sdp_mode <a id="wifi_set_sdp_mode"></a>

```c
errcode_t wifi_set_sdp_mode(wifi_if_type_enum iftype, int32_t enable, int32_t ratio)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 设置SDP模式

**前置条件**

- WiFi已通过wifi_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| iftype | [wifi_if_type_enum](#enum_wifi_if_type_enum) | 接口类型 | IFTYPE_STA/IFTYPE_AP/IFTYPE_P2P_CLIENT/IFTYPE_P2P_GO/IFTYPE_P2P_DEVICE |
| enable | int32_t | 使能开关 | 0:关闭 / 1:开启 |
| ratio | int32_t | 比例 | 大于0 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_set_sdp_subscribe <a id="wifi_set_sdp_subscribe"></a>

```c
errcode_t wifi_set_sdp_subscribe(wifi_if_type_enum iftype, char *sdp_subscribe, int32_t local_handle)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 设置SDP订阅

**前置条件**

- WiFi已通过wifi_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| iftype | [wifi_if_type_enum](#enum_wifi_if_type_enum) | 接口类型 | IFTYPE_STA/IFTYPE_AP/IFTYPE_P2P_CLIENT/IFTYPE_P2P_GO/IFTYPE_P2P_DEVICE |
| sdp_subscribe | char* | SDP订阅 | 非NULL |
| local_handle | int32_t | 当前句柄 | 有效句柄值 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_sta_wps_pbc <a id="wifi_sta_wps_pbc"></a>

```c
errcode_t wifi_sta_wps_pbc(uint8_t *bssid)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- WPS PBC连接

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已通过wifi_sta_enable()使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bssid | uint8_t* | BSSID，可指定MAC地址或填NULL | NULL或有效的MAC地址 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 发起WPS PBC连接成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_sta_wps_pin <a id="wifi_sta_wps_pin"></a>

```c
errcode_t wifi_sta_wps_pin(int8_t *pin, uint8_t *bssid)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- WPS PIN连接

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已通过wifi_sta_enable()使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pin | int8_t* | WPS中PIN码，仅限十进制，长度为8 Bytes | 非NULL，长度为8 |
| bssid | uint8_t* | BSSID，可指定MAC地址或填NULL | NULL或有效的MAC地址 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 发起WPS PIN连接成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_sta_get_wps_pin <a id="wifi_sta_get_wps_pin"></a>

```c
errcode_t wifi_sta_get_wps_pin(int8_t* pin, uint32_t len)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 获取WPS PIN码

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已通过wifi_sta_enable()使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| [out] pin | int8_t* | 待获取PIN码 | 非NULL，用于存储PIN码 |
| len | uint32_t | PIN码的长度 | WIFI_WPS_PIN_MAX_LEN_NUM(9) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 获取PIN码成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_set_psd_mode <a id="wifi_set_psd_mode"></a>

```c
errcode_t wifi_set_psd_mode(ext_psd_option_param *psd_option)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 设置PSD模式

**前置条件**

- WiFi已通过wifi_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| psd_option | [ext_psd_option_param](#struct_ext_psd_option_param)* | PSD参数 | 非NULL |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_set_psd_cb <a id="wifi_set_psd_cb"></a>

```c
errcode_t wifi_set_psd_cb(wifi_psd_cb data_cb)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 设置PSD回调接口

**前置条件**

- WiFi已通过wifi_init()初始化完成

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| data_cb | [wifi_psd_cb](#typedef_wifi_psd_cb) | PSD回调函数 | 非NULL |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 设置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_sta_config_conn_paras <a id="wifi_sta_config_conn_paras"></a>

```c
errcode_t wifi_sta_config_conn_paras(wifi_sta_conn_paras *para)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 配置WiFi STA连接参数接口

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已通过wifi_sta_enable()使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| para | [wifi_sta_conn_paras](#struct_wifi_sta_conn_paras)* | WiFi STA连接参数 | 非NULL |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 配置成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_sta_get_rssi_of_data_frame <a id="wifi_sta_get_rssi_of_data_frame"></a>

```c
errcode_t wifi_sta_get_rssi_of_data_frame(int8_t *data_rssi)
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- 获取最后一帧数据帧的信号强度

**前置条件**

- WiFi已通过wifi_init()初始化完成
- STA已通过wifi_sta_enable()使能
- STA已连接到AP

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| [out] data_rssi | int8_t* | 最后一帧数据帧信号强度 | 非NULL，用于存储获取的RSSI值 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 获取成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

## Type definitions

### wifi_csi_data_cb <a id="typedef_wifi_csi_data_cb"></a>

```c
typedef void (*wifi_csi_data_cb)(uint8_t *csi_data, int32_t len);
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device_config.h"
```

**功能说明**

- 用户注册的回调函数，用于处理CSI上报的数据

**参数说明**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| csi_data | uint8_t* | 4字节扩展时间戳+758字节64位小端存储格式的CSI数据 | 非NULL |
| len | int32_t | 数据长度，固定为762字节 | 762 |

**返回值**

无（void）

### wifi_promis_cb <a id="typedef_wifi_promis_cb"></a>

```c
typedef int32_t (*wifi_promis_cb)(void* recv_buf, int32_t frame_len, int8_t rssi);
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device_config.h"
```

**功能说明**

- 混杂模式收包回调接口定义

**参数说明**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| recv_buf | void* | 帧数据 | 非NULL |
| frame_len | int32_t | 帧长度 | 大于0 |
| rssi | int8_t | 信号强度 | - |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 处理成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_rx_mgmt_cb <a id="typedef_wifi_rx_mgmt_cb"></a>

```c
typedef int32_t (*wifi_rx_mgmt_cb)(void* recv_buf, int32_t frame_len, int8_t rssi);
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device_config.h"
```

**功能说明**

- 管理帧收包回调接口定义

**参数说明**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| recv_buf | void* | 帧数据 | 非NULL |
| frame_len | int32_t | 帧长度 | 大于0 |
| rssi | int8_t | 信号强度 | - |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 处理成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_psd_cb <a id="typedef_wifi_psd_cb"></a>

```c
typedef int32_t (*wifi_psd_cb)(void *recv_buf, uint32_t data_len);
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device_config.h"
```

**功能说明**

- PSD数据上报回调接口定义

**参数说明**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| recv_buf | void* | PSD数据 | 非NULL |
| data_len | uint32_t | PSD数据长度 | 大于0 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 处理成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### wifi_scan_no_save_cb <a id="typedef_wifi_scan_no_save_cb"></a>

```c
typedef void (*wifi_scan_no_save_cb)(wifi_scan_info_stru *scan_result);
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_scan_info.h"
```

**功能说明**

- 定制化扫描回调函数

**参数说明**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| scan_result | [wifi_scan_info_stru](#struct_wifi_scan_info_stru)* | 扫描结果 | 非NULL |

**返回值**

无（void）

### wifi_mac_derivation_ptr <a id="typedef_wifi_mac_derivation_ptr"></a>

```c
typedef unsigned int(*wifi_mac_derivation_ptr)(unsigned char *origin_mac, unsigned char num, unsigned char type, unsigned char *output_mac, unsigned char out_put_num);
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device.h"
```

**功能说明**

- MAC派生方法指针定义

**参数说明**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| origin_mac | unsigned char* | 输入MAC地址 | 非NULL |
| num | unsigned char | 输入MAC地址长度 | WIFI_MAC_LEN(6) |
| type | unsigned char | 派生类型 | 2:STA / 3:SoftAP / 7~10:P2P (Peer-to-Peer) |
| output_mac | unsigned char* | 输出MAC地址 | 非NULL |
| out_put_num | unsigned char | 输出MAC地址长度 | WIFI_MAC_LEN(6) |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 派生成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

### errcode_t <a id="typedef_errcode_t"></a>

```c
typedef uint32_t errcode_t;
```

**头文件清单**

```c
#include "include/errcode.h"
```

**功能说明**

- 错误码定义，ERRCODE_SUCC为0x00，ERRCODE_FAIL为0xFFFFFFFF

## Enumerations

### wifi_pmf_option_enum <a id="enum_wifi_pmf_option_enum"></a>

```c
typedef enum {
    WIFI_MGMT_FRAME_PROTECTION_CLOSE,
    WIFI_MGMT_FRAME_PROTECTION_OPTIONAL,
    WIFI_MGMT_FRAME_PROTECTION_REQUIRED,
    WIFI_MGMT_FRAME_PROTECTION_BUTT
} wifi_pmf_option_enum;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device_config.h"
```

**功能说明**

- PMF管理帧保护模式类型

**枚举值说明**

| 枚举值 | 数值 | 说明 |
| ---- | ---- | ---- |
| WIFI_MGMT_FRAME_PROTECTION_CLOSE | 0 | 管理帧保护模式：关闭 |
| WIFI_MGMT_FRAME_PROTECTION_OPTIONAL | 1 | 管理帧保护模式：可选 |
| WIFI_MGMT_FRAME_PROTECTION_REQUIRED | 2 | 管理帧保护模式：必须 |
| WIFI_MGMT_FRAME_PROTECTION_BUTT | 3 | 枚举边界值，不可使用 |

### wifi_if_type_enum <a id="enum_wifi_if_type_enum"></a>

```c
typedef enum {
    IFTYPE_STA,
    IFTYPE_AP,
    IFTYPE_P2P_CLIENT,
    IFTYPE_P2P_GO,
    IFTYPE_P2P_DEVICE,
    IFTYPES_BUTT
} wifi_if_type_enum;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device_config.h"
```

**功能说明**

- WiFi接口类型

**枚举值说明**

| 枚举值 | 数值 | 说明 |
| ---- | ---- | ---- |
| IFTYPE_STA | 0 | STATION |
| IFTYPE_AP | 1 | HOTSPOT |
| IFTYPE_P2P_CLIENT | 2 | P2P CLIENT |
| IFTYPE_P2P_GO | 3 | P2P GO (Group Owner) |
| IFTYPE_P2P_DEVICE | 4 | P2P DEVICE |
| IFTYPES_BUTT | 5 | 枚举边界值，不可使用 |

### ie_index_enmu <a id="enum_ie_index_enmu"></a>

```c
typedef enum ie_index_enmu {
    IE_FIRST,
    IE_SECOND,
    IE_THIRD,
    IE_FORTH,
    IE_BUTT
} ie_index_enmu;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device_config.h"
```

**功能说明**

- IE索引，有四个索引可供选择

**枚举值说明**

| 枚举值 | 数值 | 说明 |
| ---- | ---- | ---- |
| IE_FIRST | 0 | 索引1 |
| IE_SECOND | 1 | 索引2 |
| IE_THIRD | 2 | 索引3 |
| IE_FORTH | 3 | 索引4 |
| IE_BUTT | 4 | 枚举边界值，不可使用 |

### wifi_security_enum <a id="enum_wifi_security_enum"></a>

```c
typedef enum wifi_security_enum {
    WIFI_SEC_TYPE_INVALID = -1,
    WIFI_SEC_TYPE_OPEN,
    WIFI_SEC_TYPE_WEP,
    WIFI_SEC_TYPE_WPA2PSK,
    WIFI_SEC_TYPE_WPA2_WPA_PSK_MIX,
    WIFI_SEC_TYPE_WPAPSK,
    WIFI_SEC_TYPE_WPA,
    WIFI_SEC_TYPE_WPA2,
    WIFI_SEC_TYPE_SAE,
    WIFI_SEC_TYPE_WPA3_WPA2_PSK_MIX,
    WIFI_SEC_TYPE_WPA3,
    WIFI_SEC_TYPE_OWE,
    WIFI_SEC_TYPE_WAPI_PSK,
    WIFI_SEC_TYPE_WAPI_CERT,
    WIFI_SEC_TYPE_WPA3_WPA2_MIX,
    WIFI_SEC_TYPE_WEP_OPEN,
    WIFI_SEC_TYPE_UNKNOWN
} wifi_security_enum;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device_config.h"
```

**功能说明**

- WiFi安全类型枚举

**枚举值说明**

| 枚举值 | 数值 | 说明 |
| ---- | ---- | ---- |
| WIFI_SEC_TYPE_INVALID | -1 | 无效安全类型 |
| WIFI_SEC_TYPE_OPEN | 0 | Open |
| WIFI_SEC_TYPE_WEP | 1 | WEP (Wired Equivalent Privacy)|
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

### wifi_disconn_state_enum <a id="enum_wifi_disconn_state_enum"></a>

```c
typedef enum {
    WIFI_DISCONN_STATE_AUTH_TIMEOUT = 1,
    WIFI_DISCONN_STATE_AUTH_RCV_DEAUTH,
    WIFI_DISCONN_STATE_AUTH_RCV_RSP_ERR,
    WIFI_DISCONN_STATE_AUTH_SAE_COMMIT_TIMEOUT,
    WIFI_DISCONN_STATE_AUTH_SAE_COMMIT_RCV_DEAUTH,
    WIFI_DISCONN_STATE_AUTH_SAE_COMMIT_CHECK_ERR,
    WIFI_DISCONN_STATE_AUTH_SAE_CONFIRM_TIMEOUT,
    WIFI_DISCONN_STATE_AUTH_SAE_CONFIRM_RCV_DEAUTH,
    WIFI_DISCONN_STATE_AUTH_SAE_CONFIRM_CHECK_ERR,
    WIFI_DISCONN_STATE_ASSOC_TIMEOUT,
    WIFI_DISCONN_STATE_ASSOC_RCV_DEAUTH,
    WIFI_DISCONN_STATE_ASSOC_RCV_DISASSOC,
    WIFI_DISCONN_STATE_ASSOC_RCV_RSP_ERR,
    WIFI_DISCONN_STATE_EAPOL_KEY1_TIMEOUT,
    WIFI_DISCONN_STATE_EAPOL_KEY1_RCV_DEAUTH,
    WIFI_DISCONN_STATE_EAPOL_KEY1_RCV_DISASSOC,
    WIFI_DISCONN_STATE_EAPOL_KEY1_RCV_ERR,
    WIFI_DISCONN_STATE_EAPOL_KEY3_TIMEOUT,
    WIFI_DISCONN_STATE_EAPOL_KEY3_RCV_DEAUTH,
    WIFI_DISCONN_STATE_EAPOL_KEY3_RCV_DISASSOC,
    WIFI_DISCONN_STATE_EAPOL_KEY3_RCV_ERR,
    WIFI_DISCONN_STATE_CONNECTED_RCV_DEAUTH,
    WIFI_DISCONN_STATE_CONNECTED_RCV_DISASSOC,
    WIFI_DISCONN_STATE_LINKLOSS,
    WIFI_DISCONN_STATE_APP_ACTIVE_DISCONN,
    WIFI_DISCONN_STATE_CANNOT_FIND_AP,
    WIFI_DISCONN_STATE_UNKNOWN
} wifi_disconn_state_enum;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device_config.h"
```

**功能说明**

- WiFi断连状态枚举

**枚举值说明**

| 枚举值 | 数值 | 说明 |
| ---- | ---- | ---- |
| WIFI_DISCONN_STATE_AUTH_TIMEOUT | 1 | 非SAE auth阶段超时 |
| WIFI_DISCONN_STATE_AUTH_RCV_DEAUTH | 2 | 非SAE auth阶段收到deauth |
| WIFI_DISCONN_STATE_AUTH_RCV_RSP_ERR | 3 | 非SAE auth阶段收到错误回复 |
| WIFI_DISCONN_STATE_AUTH_SAE_COMMIT_TIMEOUT | 4 | SAE auth阶段接收commit超时 |
| WIFI_DISCONN_STATE_AUTH_SAE_COMMIT_RCV_DEAUTH | 5 | SAE auth commit阶段收到deauth |
| WIFI_DISCONN_STATE_AUTH_SAE_COMMIT_CHECK_ERR | 6 | 校验SAE commit失败 |
| WIFI_DISCONN_STATE_AUTH_SAE_CONFIRM_TIMEOUT | 7 | SAE auth阶段接收confirm超时 |
| WIFI_DISCONN_STATE_AUTH_SAE_CONFIRM_RCV_DEAUTH | 8 | SAE auth confirm阶段收到deauth |
| WIFI_DISCONN_STATE_AUTH_SAE_CONFIRM_CHECK_ERR | 9 | 校验SAE confirm失败 |
| WIFI_DISCONN_STATE_ASSOC_TIMEOUT | 10 | assoc超时 |
| WIFI_DISCONN_STATE_ASSOC_RCV_DEAUTH | 11 | assoc阶段收到deauth |
| WIFI_DISCONN_STATE_ASSOC_RCV_DISASSOC | 12 | assoc阶段收到disassoc |
| WIFI_DISCONN_STATE_ASSOC_RCV_RSP_ERR | 13 | assoc阶段收到错误回复 |
| WIFI_DISCONN_STATE_EAPOL_KEY1_TIMEOUT | 14 | 接收eapol1超时 |
| WIFI_DISCONN_STATE_EAPOL_KEY1_RCV_DEAUTH | 15 | 等待eapol1阶段收到deauth |
| WIFI_DISCONN_STATE_EAPOL_KEY1_RCV_DISASSOC | 16 | 等待eapol1阶段收到disassoc |
| WIFI_DISCONN_STATE_EAPOL_KEY1_RCV_ERR | 17 | eapol1校验失败 |
| WIFI_DISCONN_STATE_EAPOL_KEY3_TIMEOUT | 18 | 接收eapol3超时 |
| WIFI_DISCONN_STATE_EAPOL_KEY3_RCV_DEAUTH | 19 | 等待eapol3阶段收到deauth |
| WIFI_DISCONN_STATE_EAPOL_KEY3_RCV_DISASSOC | 20 | 等待eapol3阶段收到disassoc |
| WIFI_DISCONN_STATE_EAPOL_KEY3_RCV_ERR | 21 | eapol3校验失败 |
| WIFI_DISCONN_STATE_CONNECTED_RCV_DEAUTH | 22 | 连接成功后收到deauth |
| WIFI_DISCONN_STATE_CONNECTED_RCV_DISASSOC | 23 | 连接成功后收到disassoc |
| WIFI_DISCONN_STATE_LINKLOSS | 24 | 驱动主动断连 |
| WIFI_DISCONN_STATE_APP_ACTIVE_DISCONN | 25 | 应用层主动断连 |
| WIFI_DISCONN_STATE_CANNOT_FIND_AP | 26 | 扫描不到AP |
| WIFI_DISCONN_STATE_UNKNOWN | 27 | 其它断连状态 |

### wifi_iftype_t <a id="enum_wifi_iftype_t"></a>

```c
typedef enum {
    WIFI_IFTYPE_UNSPECIFIED,
    WIFI_IFTYPE_ADHOC,
    WIFI_IFTYPE_STATION = 2,
    WIFI_IFTYPE_AP = 3,
    WIFI_IFTYPE_AP_VLAN,
    WIFI_IFTYPE_WDS,
    WIFI_IFTYPE_MONITOR,
    WIFI_IFTYPE_MESH_POINT = 7,
    WIFI_IFTYPE_P2P_CLIENT,
    WIFI_IFTYPE_P2P_GO,
    WIFI_IFTYPE_P2P_DEVICE,
    WIFI_IFTYPES_BUTT
} wifi_iftype_t;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device_config.h"
```

**功能说明**

- WiFi接口类型

**枚举值说明**

| 枚举值 | 数值 | 说明 |
| ---- | ---- | ---- |
| WIFI_IFTYPE_UNSPECIFIED | 0 | UNSPECIFIED |
| WIFI_IFTYPE_ADHOC | 1 | ADHOC |
| WIFI_IFTYPE_STATION | 2 | STATION |
| WIFI_IFTYPE_AP | 3 | HOTSPOT |
| WIFI_IFTYPE_AP_VLAN | 4 | HOTSPOT VLAN |
| WIFI_IFTYPE_WDS | 5 | WDS |
| WIFI_IFTYPE_MONITOR | 6 | MONITOR |
| WIFI_IFTYPE_MESH_POINT | 7 | MESH (Mesh Network) POINT |
| WIFI_IFTYPE_P2P_CLIENT | 8 | P2P CLIENT |
| WIFI_IFTYPE_P2P_GO | 9 | P2P GO |
| WIFI_IFTYPE_P2P_DEVICE | 10 | P2P DEVICE |
| WIFI_IFTYPES_BUTT | 11 | 枚举边界值，不可使用 |

### wifi_conn_state_enum <a id="enum_wifi_conn_state_enum"></a>

```c
typedef enum {
    WIFI_DISCONNECTED,
    WIFI_CONNECTED,
    WIFI_CONNECTING,
    WIFI_CONN_STATUS_BUTT,
} wifi_conn_state_enum;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_linked_info.h"
```

**功能说明**

- WiFi的连接状态

**枚举值说明**

| 枚举值 | 数值 | 说明 |
| ---- | ---- | ---- |
| WIFI_DISCONNECTED | 0 | 断连 |
| WIFI_CONNECTED | 1 | 已连接 |
| WIFI_CONNECTING | 2 | 连接中 |
| WIFI_CONN_STATUS_BUTT | 3 | 枚举边界值，不可使用 |

### wifi_event_state_enum <a id="enum_wifi_event_state_enum"></a>

```c
typedef enum {
    WIFI_STATE_NOT_AVALIABLE = 0,
    WIFI_STATE_AVALIABLE
} wifi_event_state_enum;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_event.h"
```

**功能说明**

- WiFi事件状态

**枚举值说明**

| 枚举值 | 数值 | 说明 |
| ---- | ---- | ---- |
| WIFI_STATE_NOT_AVALIABLE | 0 | 不可用状态 |
| WIFI_STATE_AVALIABLE | 1 | 可用状态 |

### wifi_scan_type_enum <a id="enum_wifi_scan_type_enum"></a>

```c
typedef enum {
    WIFI_BASIC_SCAN,
    WIFI_CHANNEL_SCAN,
    WIFI_SSID_SCAN,
    WIFI_SSID_PREFIX_SCAN,
    WIFI_BSSID_SCAN,
    WIFI_SSID_SCAN_WITH_CHANNEL,
    STA_SCAN_BUTT
} wifi_scan_type_enum;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device_config.h"
```

**功能说明**

- WiFi扫描的类型

**枚举值说明**

| 枚举值 | 数值 | 说明 |
| ---- | ---- | ---- |
| WIFI_BASIC_SCAN | 0 | 普通扫描 |
| WIFI_CHANNEL_SCAN | 1 | 基于指定信道的扫描 |
| WIFI_SSID_SCAN | 2 | 基于指定SSID的扫描 |
| WIFI_SSID_PREFIX_SCAN | 3 | 基于指定前缀SSID的扫描 |
| WIFI_BSSID_SCAN | 4 | 基于指定BSSID的扫描 |
| WIFI_SSID_SCAN_WITH_CHANNEL | 5 | 基于指定SSID与信道的扫描 |
| STA_SCAN_BUTT | 6 | 枚举边界值，不可使用 |

### protocol_mode_enum <a id="enum_protocol_mode_enum"></a>

```c
typedef enum {
    WIFI_MODE_UNDEFINE,
    WIFI_MODE_11B,
    WIFI_MODE_11B_G,
    WIFI_MODE_11B_G_N,
    WIFI_MODE_11B_G_N_AX,
} protocol_mode_enum;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device_config.h"
```

**功能说明**

- SoftAP和Station接口的协议模式

**枚举值说明**

| 枚举值 | 数值 | 说明 |
| ---- | ---- | ---- |
| WIFI_MODE_UNDEFINE | 0 | 未配置 |
| WIFI_MODE_11B | 1 | 11b |
| WIFI_MODE_11B_G | 2 | 11b/g |
| WIFI_MODE_11B_G_N | 3 | 11b/g/n |
| WIFI_MODE_11B_G_N_AX | 4 | 11b/g/n/ax |

### ip_type_stru_enum <a id="enum_ip_type_stru_enum"></a>

```c
typedef enum {
    STATIC_IP,
    DHCP,
    UNKNOWN
} ip_type_stru_enum;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device_config.h"
```

**功能说明**

- IP (Internet Protocol) 的分配类型

**枚举值说明**

| 枚举值 | 数值 | 说明 |
| ---- | ---- | ---- |
| STATIC_IP | 0 | 静态IP地址 |
| DHCP (Dynamic Host Configuration Protocol) | 1 | 由DHCP动态分配的IP地址 |
| UNKNOWN | 2 | 未知的IP地址类型 |

### wifi_sta_connect_paras_enum <a id="enum_wifi_sta_connect_paras_enum"></a>

```c
typedef enum {
    CONFIG_AUTH_MAX_RETRY,
    CONFIG_AUTH_RECV_TIMEOUT,
    CONFIG_ASSOC_MAX_RETRY,
    CONFIG_ASSOC_RECV_TIMEOUT,
    CONFIG_EAPOL1_RECV_TIMEOUT,
    CONFIG_EAPOL2_MAX_RETRY,
    CONFIG_EAPOL3_RECV_TIMEOUT,
    CONFIG_CONNECT_PARA_MAX,
} wifi_sta_connect_paras_enum;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device_config.h"
```

**功能说明**

- WiFi STA各连接阶段参数配置枚举类型

**枚举值说明**

| 枚举值 | 数值 | 说明 |
| ---- | ---- | ---- |
| CONFIG_AUTH_MAX_RETRY | 0 | AUTH最大重试次数 |
| CONFIG_AUTH_RECV_TIMEOUT | 1 | AUTH接收超时时间 |
| CONFIG_ASSOC_MAX_RETRY | 2 | ASSOC最大重试次数 |
| CONFIG_ASSOC_RECV_TIMEOUT | 3 | ASSOC接收超时时间 |
| CONFIG_EAPOL1_RECV_TIMEOUT | 4 | EAPOL1接收超时时间 |
| CONFIG_EAPOL2_MAX_RETRY | 5 | EAPOL2最大重试次数 |
| CONFIG_EAPOL3_RECV_TIMEOUT | 6 | EAPOL3接收超时时间 |
| CONFIG_CONNECT_PARA_MAX | 7 | 枚举边界值，不可使用 |

### wifi_wpa_psk_type_enum <a id="enum_wifi_wpa_psk_type_enum"></a>

```c
typedef enum {
    WIFI_WPA_PSK_NOT_USE,
} wifi_wpa_psk_type_enum;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device_config.h"
```

**功能说明**

- PSK的类型

**枚举值说明**

| 枚举值 | 数值 | 说明 |
| ---- | ---- | ---- |
| WIFI_WPA_PSK_NOT_USE | 0 | 不用提前计算PSK，本情况下不使用wifi_fast_connect_stru中的psk |

## Structures

### wifi_dev_t <a id="struct_wifi_dev_t"></a>

```c
typedef struct {
    uint32_t iftype;
    void *priv;
    uint32_t network_id;
    uint32_t ifname_len;
    char ifname[WIFI_IFNAME_MAX_SIZE + 1];
    char reserve[1];
} wifi_dev_t;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device_config.h"
```

**功能说明**

- WIFI_DEV的相关参数

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| iftype | uint32_t | IFTYPE，值参考[wifi_iftype_t](#enum_wifi_iftype_t) |
| priv | void* | priv |
| network_id | uint32_t | network_id |
| ifname_len | uint32_t | ifname_len |
| ifname | char[WIFI_IFNAME_MAX_SIZE + 1] | ifname |
| reserve | char[1] | 保留 |

### ip_config_stru <a id="struct_ip_config_stru"></a>

```c
typedef struct {
    uint32_t ip_address;
    uint32_t gateway;
    uint32_t dns_servers[WIFI_MAX_DNS_NUM];
    uint32_t netmask;
} ip_config_stru;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device_config.h"
```

**功能说明**

- IPV4的配置

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| ip_address | uint32_t | WiFi device的IP地址 |
| gateway | uint32_t | WiFi device的Gateway |
| dns_servers | uint32_t[WIFI_MAX_DNS_NUM] | WiFi device的DNS (Domain Name System) 服务器地址 |
| netmask | uint32_t | WiFi device的子网掩码 |

### ipv6_config_stru <a id="struct_ipv6_config_stru"></a>

```c
typedef struct {
    uint8_t ipv6_address[WIFI_IPV6_ADDR_LEN];
    uint8_t ipv6_dns_servers[WIFI_MAX_DNS_NUM][WIFI_IPV6_DNS_LEN];
} ipv6_config_stru;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device_config.h"
```

**功能说明**

- IPV6 (Internet Protocol Version 6) 的配置

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| ipv6_address | uint8_t[WIFI_IPV6_ADDR_LEN] | IPV6地址 |
| ipv6_dns_servers | uint8_t[WIFI_MAX_DNS_NUM][WIFI_IPV6_DNS_LEN] | DNS服务器地址 |

### wifi_sta_config_stru <a id="struct_wifi_sta_config_stru"></a>

```c
typedef struct wifi_sta_config_stru {
    int8_t ssid[WIFI_MAX_SSID_LEN];
    uint8_t bssid[WIFI_MAC_LEN];
    int8_t pre_shared_key[WIFI_MAX_KEY_LEN];
    wifi_security_enum security_type;
    int8_t wifi_psk_type;
    int8_t strengthen_verify;
    uint8_t channel;
    int8_t rsv;
    ip_type_stru_enum ip_type;
    ip_config_stru static_ip;
    ipv6_config_stru static_ipv6;
} wifi_sta_config_stru;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device_config.h"
```

**功能说明**

- 连接到指定WiFi device的WiFi STA配置

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| ssid | int8_t[WIFI_MAX_SSID_LEN] | SSID |
| bssid | uint8_t[WIFI_MAC_LEN] | BSSID |
| pre_shared_key | int8_t[WIFI_MAX_KEY_LEN] | 预共享密钥 |
| security_type | [wifi_security_enum](#enum_wifi_security_enum) | 安全类型 |
| wifi_psk_type | int8_t | PSK的类型 |
| strengthen_verify | int8_t | 加强校验，非零值表示open模式下需要校验密码长度是否为0，默认为0不校验 |
| channel | uint8_t | 关联的指定信道号，可选参数 |
| rsv | int8_t | 保留1字节 |
| ip_type | [ip_type_stru_enum](#enum_ip_type_stru_enum) | IP的分配类型 |
| static_ip | [ip_config_stru](#struct_ip_config_stru) | 静态IPV4地址 |
| static_ipv6 | [ipv6_config_stru](#struct_ipv6_config_stru) | 静态IPV6地址 |

### wifi_scan_params_stru <a id="struct_wifi_scan_params_stru"></a>

```c
typedef struct {
    int8_t ssid[WIFI_MAX_SSID_LEN];
    int8_t ssid_len;
    int8_t bssid[WIFI_MAC_LEN];
    int32_t channel_num;
    wifi_scan_type_enum scan_type;
} wifi_scan_params_stru;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device_config.h"
```

**功能说明**

- 扫描参数的设置

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| ssid | int8_t[WIFI_MAX_SSID_LEN] | SSID |
| ssid_len | int8_t | SSID的长度 |
| bssid | int8_t[WIFI_MAC_LEN] | BSSID |
| channel_num | int32_t | 信道号 |
| scan_type | [wifi_scan_type_enum](#enum_wifi_scan_type_enum) | WiFi的扫描类型 |

### wifi_scan_strategy_stru <a id="struct_wifi_scan_strategy_stru"></a>

```c
typedef struct {
    uint8_t scan_time;
    uint8_t scan_cnt;
    uint8_t single_probe_send_times;
    uint8_t reserved;
} wifi_scan_strategy_stru;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device_config.h"
```

**功能说明**

- 扫描策略设置

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| scan_time | uint8_t | 每个信道停留时间，单位ms，范围20~120ms，默认20ms，扫描总时间不能超过4.5s |
| scan_cnt | uint8_t | 扫描slot数，一个slot 20ms |
| single_probe_send_times | uint8_t | 单个probe req报文的发送次数，范围1~3，默认1 |
| reserved | uint8_t | 保留 |

### csi_config_stru <a id="struct_csi_config_stru"></a>

```c
typedef struct {
    uint8_t user_index;
    uint8_t enable;
    uint8_t match_ta_ra_select;
    uint8_t resv;
    uint8_t mac_addr[WIFI_MAC_LEN];
    uint8_t frame_filter_bitmap;
    uint8_t sub_type_filter_enable;
    uint8_t sub_type_filter;
    uint8_t ppdu_filter_bitmap;
    uint16_t period;
} csi_config_stru;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device_config.h"
```

**功能说明**

- CSI的配置

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| user_index | uint8_t | 用户ID，取值范围0~3，最多4个用户 |
| enable | uint8_t | CSI白名单用户开关 |
| match_ta_ra_select | uint8_t | CSI白名单地址过滤类型，0:RA / 1:TA |
| resv | uint8_t | 保留1字节对齐 |
| mac_addr | uint8_t[WIFI_MAC_LEN] | MAC地址 |
| frame_filter_bitmap | uint8_t | 帧类型过滤具体参数 |
| sub_type_filter_enable | uint8_t | 帧子类型过滤开关 |
| sub_type_filter | uint8_t | 帧子类型过滤具体参数 |
| ppdu_filter_bitmap | uint8_t | PPDU format过滤具体参数 |
| period | uint16_t | CSI上报时间间隔 |

### wifi_fast_connect_stru <a id="struct_wifi_fast_connect_stru"></a>

```c
typedef struct wifi_fast_connect_stru {
    wifi_sta_config_stru config;
    uint8_t psk[WIFI_PSK_LEN];
    wifi_wpa_psk_type_enum psk_flag;
    uint8_t channel_num;
} wifi_fast_connect_stru;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device_config.h"
```

**功能说明**

- 快速连接的参数设置

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| config | [wifi_sta_config_stru](#struct_wifi_sta_config_stru) | 连接到指定WiFi device的WiFi STA配置 |
| psk | uint8_t[WIFI_PSK_LEN] | PSK |
| psk_flag | [wifi_wpa_psk_type_enum](#enum_wifi_wpa_psk_type_enum) | PSK的标志，不需指定时置0 |
| channel_num | uint8_t | 信道号 |

### wifi_ptype_filter_stru <a id="struct_wifi_ptype_filter_stru"></a>

```c
typedef struct {
    int8_t mdata_en  : 1;
    int8_t udata_en  : 1;
    int8_t mmngt_en  : 1;
    int8_t umngt_en  : 1;
    int8_t custom_en : 1;
    int8_t resvd     : 3;
} wifi_ptype_filter_stru;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device_config.h"
```

**功能说明**

- 混杂模式报文接收过滤设置

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| mdata_en | int8_t:1 | 使能接收组播(广播)数据包 |
| udata_en | int8_t:1 | 使能接收单播数据包 |
| mmngt_en | int8_t:1 | 使能接收组播(广播)管理包 |
| umngt_en | int8_t:1 | 使能接收单播管理包 |
| custom_en | int8_t:1 | 使能接收beacon/probe request包 |
| resvd | int8_t:3 | 保留字段 |

### linkloss_paras_stru <a id="struct_linkloss_paras_stru"></a>

```c
typedef struct {
    uint16_t linkloss_threshold;
    uint8_t send_probe_request_ratio;
    uint8_t resv[1];
} linkloss_paras_stru;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device_config.h"
```

**功能说明**

- 设置linkloss相关参数

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| linkloss_threshold | uint16_t | 取值范围[50, 500]，设置为n时，表示linkloss阈值基础时间为(100*n)ms |
| send_probe_request_ratio | uint8_t | 取值范围[1, 10]，设置为n时，表示linkloss计数达到阈值的(n/10)时，开始发送探测帧保活 |
| resv | uint8_t[1] | 保留 |

### wifi_conn_sec_stru <a id="struct_wifi_conn_sec_stru"></a>

```c
typedef struct {
    wifi_security_enum sec_type;
    int32_t pairwise;
} wifi_conn_sec_stru;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device_config.h"
```

**功能说明**

- WiFi连接加密方式

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| sec_type | [wifi_security_enum](#enum_wifi_security_enum) | 安全类型 |
| pairwise | int32_t | 加密方式，AES/TKIP/MIX |

### wifi_sta_conn_paras <a id="struct_wifi_sta_conn_paras"></a>

```c
typedef struct {
    uint8_t bitmap;
    uint8_t resv;
    uint16_t conn_paras[CONFIG_CONNECT_PARA_MAX];
} wifi_sta_conn_paras;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device_config.h"
```

**功能说明**

- STA连接参数配置结构体

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| bitmap | uint8_t | 参数配置比特位图：bit0-auth max retry / bit1-auth recv timeout / bit2-assoc max retry / bit3-assoc recv timeout / bit4-eapol1 recv timeout / bit5-eapol2 max retry / bit6-eapol3 recv timeout / bit7-resv |
| resv | uint8_t | 保留字段 |
| conn_paras | uint16_t[CONFIG_CONNECT_PARA_MAX] | 参数值，超时单位：毫秒 |

### ext_psd_option_param <a id="struct_ext_psd_option_param"></a>

```c
typedef struct {
    uint8_t enable;
    uint8_t resv;
    uint16_t duration;
    uint32_t cycle;
} ext_psd_option_param;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_device_config.h"
```

**功能说明**

- 设置PSD状态

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| enable | uint8_t | PSD使能标记，0:关 / 1:开 |
| resv | uint8_t | 保留 |
| duration | uint16_t | 采样时长，取值1~65535，单位min |
| cycle | uint32_t | 采样间隔，单位ms，取值100~1000 |

### wifi_linked_info_stru <a id="struct_wifi_linked_info_stru"></a>

```c
typedef struct {
    int8_t ssid[WIFI_MAX_SSID_LEN];
    uint8_t bssid[WIFI_MAC_LEN];
    int8_t wpa_state;
    int32_t rssi;
    int16_t disconn_state;
    int16_t channel_num;
    int32_t snr;
    wifi_conn_state_enum conn_state;
} wifi_linked_info_stru;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_linked_info.h"
```

**功能说明**

- 有关连接到此STA的AP信息

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| ssid | int8_t[WIFI_MAX_SSID_LEN] | SSID |
| bssid | uint8_t[WIFI_MAC_LEN] | BSSID |
| wpa_state | int8_t | 断连时WPA (Wi-Fi Protected Access) 状态 |
| rssi | int32_t | RSSI |
| disconn_state | int16_t | STA断连时的状态 |
| channel_num | int16_t | AP的WiFi信道信息 |
| snr | int32_t | AP的WiFi信噪比信息 |
| conn_state | [wifi_conn_state_enum](#enum_wifi_conn_state_enum) | WiFi的连接状态 |

### wifi_scan_info_stru <a id="struct_wifi_scan_info_stru"></a>

```c
typedef struct {
    char ssid[WIFI_MAX_SSID_LEN];
    uint8_t bssid[WIFI_MAC_LEN];
    int8_t pairwise;
    wifi_security_enum security_type;
    int32_t rssi;
    int32_t band;
    int32_t channel_num;
} wifi_scan_info_stru;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_scan_info.h"
```

**功能说明**

- WiFi扫描结果信息

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| ssid | char[WIFI_MAX_SSID_LEN] | SSID |
| bssid | uint8_t[WIFI_MAC_LEN] | BSSID |
| pairwise | int8_t | 加密类型 |
| security_type | [wifi_security_enum](#enum_wifi_security_enum) | 安全类型 |
| rssi | int32_t | 信号强度 |
| band | int32_t | 频带 |
| channel_num | int32_t | 信道号 |

### wifi_event_stru <a id="struct_wifi_event_stru"></a>

```c
typedef struct {
    void (*wifi_event_connection_changed)(int32_t state, const wifi_linked_info_stru *info, int32_t reason_code);
    void (*wifi_event_scan_state_changed)(int32_t state, int32_t size);
    void (*wifi_event_softap_state_changed)(int32_t state);
    void (*wifi_event_softap_sta_join)(const wifi_sta_info_stru *info);
    void (*wifi_event_softap_sta_leave)(const wifi_sta_info_stru *info);
    void (*wifi_event_p2p_receive_connect)(const uint8_t *bssid, int8_t wps_method);
    void (*wifi_event_p2p_go_neg_result)(int32_t state, int32_t mode);
    void (*wifi_event_p2p_go_start)(int32_t state);
    void (*wifi_event_p2p_invitation_result)(int32_t state);
    void (*wifi_event_p2p_gc_connection_changed)(int32_t state, const p2p_status_info_stru *status);
    void (*wifi_event_p2p_go_connection_changed)(int32_t state, const p2p_client_info_stru *client);
    void (*wifi_event_wps_result)(int32_t state, wifi_if_type_enum wifi_if_type);
} wifi_event_stru;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/wifi_event.h"
```

**功能说明**

- 指向用于STA和Hotspot连接、断开连接或扫描的WiFi事件回调的指针

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| wifi_event_connection_changed | void(*)(int32_t, const wifi_linked_info_stru*, int32_t) | 连接状态改变回调 |
| wifi_event_scan_state_changed | void(*)(int32_t, int32_t) | 扫描状态改变回调 |
| wifi_event_softap_state_changed | void(*)(int32_t) | Hotspot状态改变回调 |
| wifi_event_softap_sta_join | void(*)(const wifi_sta_info_stru*) | Station连接回调 |
| wifi_event_softap_sta_leave | void(*)(const wifi_sta_info_stru*) | Station断连回调 |
| wifi_event_p2p_receive_connect | void(*)(const uint8_t*, int8_t) | P2P接收连接请求回调 |
| wifi_event_p2p_go_neg_result | void(*)(int32_t, int32_t) | P2P GO协商结果回调 |
| wifi_event_p2p_go_start | void(*)(int32_t) | P2P GO建立结果回调 |
| wifi_event_p2p_invitation_result | void(*)(int32_t) | P2P邀请结果回调 |
| wifi_event_p2p_gc_connection_changed | void(*)(int32_t, const p2p_status_info_stru*) | P2P GC (Group Client) 关联结果回调 |
| wifi_event_p2p_go_connection_changed | void(*)(int32_t, const p2p_client_info_stru*) | P2P GO关联结果回调 |
| wifi_event_wps_result | void(*)(int32_t, wifi_if_type_enum) | WPS关联结果回调 |

### wifi_sta_info_stru <a id="struct_wifi_sta_info_stru"></a>

```c
typedef struct {
    uint8_t mac_addr[WIFI_MAC_LEN];
    int8_t rssi;
    int8_t rsv;
    uint32_t best_rate;
} wifi_sta_info_stru;
```

**头文件清单**

```c
#include "include/middleware/services/wifi/station_info.h"
```

**功能说明**

- 返回与AP相连的STA信息

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| mac_addr | uint8_t[WIFI_MAC_LEN] | MAC地址 |
| rssi | int8_t | RSSI |
| rsv | int8_t | 保留字段 |
| best_rate | uint32_t | SoftAP上一次接收相连的Station报文最佳发送速率值(kbps) |





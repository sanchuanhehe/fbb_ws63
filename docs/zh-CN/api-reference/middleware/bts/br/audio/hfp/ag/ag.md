# HFP AG

HFP (Hands-Free Profile)AG (Audio Gateway) 端 API。WS63 作为音频网关连接蓝牙耳机/车载系统——管理通话状态控制（来电/接听/挂断/拒接）、音频连接建立和释放、AT 命令交互。

适用车载蓝牙、蓝牙通话适配器等场景。

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_ag.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [hfp_ag_connect](#hfp_ag_connect) | 建立与HF (Hands-Free) 的SLC(service level connection) |
| [hfp_ag_disconnect](#hfp_ag_disconnect) | 释放与HF的SLC |
| [hfp_ag_audio_connect](#hfp_ag_audio_connect) | 建立与HF的音频连接 |
| [hfp_ag_audio_disconnect](#hfp_ag_audio_disconnect) | 断开与HF的音频连接 |
| [hfp_ag_get_device_connect_state](#hfp_ag_get_device_connect_state) | 通过设备地址查询设备HFP连接状态 |
| [hfp_ag_get_sco_connect_state](#hfp_ag_get_sco_connect_state) | 通过设备地址查询设备HFP SCO (Synchronous Connection-Oriented) 连接状态 |
| [hfp_ag_send_answer_call_event](#hfp_ag_send_answer_call_event) | 接听呼入电话 |
| [hfp_ag_send_finish_call_event](#hfp_ag_send_finish_call_event) | 结束通话 |
| [hfp_ag_send_originate_call_event](#hfp_ag_send_originate_call_event) | 呼出电话 |
| [hfp_ag_send_network_incoming_event](#hfp_ag_send_network_incoming_event) | AG端发送来电事件 |
| [hfp_ag_send_network_event](#hfp_ag_send_network_event) | AG端发送其他网络事件 |
| [hfp_ag_set_volume](#hfp_ag_set_volume) | HF端发起音量调整 |
| [hfp_ag_send_current_calls](#hfp_ag_send_current_calls) | 发送通话列表 |
| [hfp_ag_send_subscriber_number](#hfp_ag_send_subscriber_number) | 发送订阅号码信息 |
| [hfp_ag_send_battery_charge](#hfp_ag_send_battery_charge) | AG端发送电量信息 |
| [hfp_ag_send_cmee_error](#hfp_ag_send_cmee_error) | AG端发送扩展错误码 |
| [hfp_ag_send_network_operator](#hfp_ag_send_network_operator) | AG端发送运营商信息 |
| [hfp_ag_send_indicator_val](#hfp_ag_send_indicator_val) | AG端发送指示器信息 |
| [hfp_ag_register_callbacks](#hfp_ag_register_callbacks) | 注册上层应用的回调函数 |

## Functions

### hfp_ag_connect <a id="hfp_ag_connect"></a>

```c
int hfp_ag_connect(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_ag.h"
```

**功能说明**

- 建立与对端HF设备的SLC服务层连接
- 调用后，BTS (Bluetooth Subsystem) 发起HFP AG与HF的连接流程，连接结果通过[hfp_ag_conn_state_changed_callback](#typedef_hfp_ag_conn_state_changed_callback)回调通知上层
- 需在蓝牙协议栈已初始化且BR/EDR链路已建立后调用

**前置条件**

- 蓝牙协议栈已初始化完成
- 对端设备已建立BR/EDR ACL (Asynchronous Connection-Less) 链路

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息指针 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 连接请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_ag_disconnect <a id="hfp_ag_disconnect"></a>

```c
int hfp_ag_disconnect(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_ag.h"
```

**功能说明**

- 释放与对端HF设备的SLC服务层连接
- 调用后，BTS发起HFP AG与HF的断连流程，断连结果通过[hfp_ag_conn_state_changed_callback](#typedef_hfp_ag_conn_state_changed_callback)回调通知上层
- 需在HFP AG已与HF建立SLC连接后调用

**前置条件**

- 蓝牙协议栈已初始化完成
- 对端设备已建立HFP SLC连接

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息指针 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 断连请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_ag_audio_connect <a id="hfp_ag_audio_connect"></a>

```c
int hfp_ag_audio_connect(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_ag.h"
```

**功能说明**

- 建立与对端HF设备的SCO音频连接
- 调用后，BTS发起SCO音频链路建立流程，连接结果通过[hfp_ag_sco_conn_state_changed_callback](#typedef_hfp_ag_sco_conn_state_changed_callback)回调通知上层
- 需在HFP AG已与HF建立SLC连接后调用

**前置条件**

- 蓝牙协议栈已初始化完成
- 对端设备已建立HFP SLC连接

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息指针 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 音频连接请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_ag_audio_disconnect <a id="hfp_ag_audio_disconnect"></a>

```c
int hfp_ag_audio_disconnect(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_ag.h"
```

**功能说明**

- 断开与对端HF设备的SCO音频连接
- 调用后，BTS发起SCO音频链路断连流程，断连结果通过[hfp_ag_sco_conn_state_changed_callback](#typedef_hfp_ag_sco_conn_state_changed_callback)回调通知上层
- 需在HFP AG已与HF建立SCO音频连接后调用

**前置条件**

- 蓝牙协议栈已初始化完成
- 对端设备已建立SCO音频连接

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息指针 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 音频断连请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_ag_get_device_connect_state <a id="hfp_ag_get_device_connect_state"></a>

```c
int hfp_ag_get_device_connect_state(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_ag.h"
```

**功能说明**

- 通过对端蓝牙设备地址查询该设备的HFP AG连接状态
- 返回值为[profile_connect_state_t](#enum_profile_connect_state_t)枚举，包含正在连接、已连接、正在断连、已断连四种状态
- 调用该接口不会触发连接状态变更，仅做查询操作

**前置条件**

- 蓝牙协议栈已初始化完成
- 入参bd_addr不为NULL，且指向有效的蓝牙设备地址

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息指针 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| PROFILE_STATE_CONNECTING(0x01) | 正在连接 | 正在建立HFP连接 |
| PROFILE_STATE_CONNECTED(0x02) | 已连接 | HFP连接已建立 |
| PROFILE_STATE_DISCONNECTING(0x03) | 正在断连 | 正在断开HFP连接 |
| PROFILE_STATE_DISCONNECTED(0x04) | 已断连 | HFP连接已断开 |

### hfp_ag_get_sco_connect_state <a id="hfp_ag_get_sco_connect_state"></a>

```c
int hfp_ag_get_sco_connect_state(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_ag.h"
```

**功能说明**

- 通过对端蓝牙设备地址查询该设备的HFP SCO音频连接状态
- 返回值为[profile_connect_state_t](#enum_profile_connect_state_t)枚举，包含正在连接、已连接、正在断连、已断连四种状态
- 调用该接口不会触发连接状态变更，仅做查询操作

**前置条件**

- 蓝牙协议栈已初始化完成
- 入参bd_addr不为NULL，且指向有效的蓝牙设备地址

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息指针 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| PROFILE_STATE_CONNECTING(0x01) | 正在连接 | 正在建立SCO连接 |
| PROFILE_STATE_CONNECTED(0x02) | 已连接 | SCO连接已建立 |
| PROFILE_STATE_DISCONNECTING(0x03) | 正在断连 | 正在断开SCO连接 |
| PROFILE_STATE_DISCONNECTED(0x04) | 已断连 | SCO连接已断开 |

### hfp_ag_send_answer_call_event <a id="hfp_ag_send_answer_call_event"></a>

```c
int hfp_ag_send_answer_call_event(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_ag.h"
```

**功能说明**

- AG端接听呼入电话
- 调用后，AG端向HF端发送接听通话事件通知
- 需在有呼入电话时调用

**前置条件**

- 蓝牙协议栈已初始化完成
- 对端设备已建立HFP SLC连接
- 当前存在呼入电话

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息指针 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 接听事件成功发送 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_ag_send_finish_call_event <a id="hfp_ag_send_finish_call_event"></a>

```c
int hfp_ag_send_finish_call_event(const bd_addr_t *bd_addr, hfp_ag_action_t type)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_ag.h"
```

**功能说明**

- AG端结束通话
- 调用后，AG端根据挂断类型向HF端发送结束通话事件通知
- 支持释放所有通话、释放来电或呼出通话、释放最后一次通话、释放保持通话等挂断类型

**前置条件**

- 蓝牙协议栈已初始化完成
- 对端设备已建立HFP SLC连接
- 当前存在活跃通话

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息指针 | 不为NULL |
| type | [hfp_ag_action_t](#enum_hfp_ag_action_t) | 挂断类型 | HFP_AG_CANCELED_ALLCALL(0x01): 释放所有通话或GSM服务不可用<br>HFP_AG_CANCELED_CALLSETUP(0x02): 释放来电或呼出通话<br>HFP_AG_CANCELED_LASTCALL(0x03): 释放最后一次通话<br>HFP_AG_CANCELED_CALLHELD(0x04): 释放保持中的通话 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 结束通话事件成功发送 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_ag_send_originate_call_event <a id="hfp_ag_send_originate_call_event"></a>

```c
int hfp_ag_send_originate_call_event(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_ag.h"
```

**功能说明**

- AG端发起呼出电话
- 调用后，AG端向HF端发送呼出通话事件通知
- 需在HFP SLC连接已建立后调用

**前置条件**

- 蓝牙协议栈已初始化完成
- 对端设备已建立HFP SLC连接

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息指针 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 呼出事件成功发送 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_ag_send_network_incoming_event <a id="hfp_ag_send_network_incoming_event"></a>

```c
int hfp_ag_send_network_incoming_event(const bd_addr_t *bd_addr, hfp_phone_info_t *phone_info)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_ag.h"
```

**功能说明**

- AG端发送来电事件通知
- 调用后，AG端向HF端发送来电信息，包含来电号码、号码类型、服务类型等
- 需在HFP SLC连接已建立后调用

**前置条件**

- 蓝牙协议栈已初始化完成
- 对端设备已建立HFP SLC连接
- 入参phone_info不为NULL，且指向有效的来电信息

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息指针 | 不为NULL |
| phone_info | [hfp_phone_info_t](#struct_hfp_phone_info_t) * | 来电信息指针 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 来电事件成功发送 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_ag_send_network_event <a id="hfp_ag_send_network_event"></a>

```c
int hfp_ag_send_network_event(const bd_addr_t *bd_addr, hfp_ag_network_event_t event, uint8_t param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_ag.h"
```

**功能说明**

- AG端发送其他网络事件通知
- 调用后，AG端向HF端发送网络事件，包含远端忙、响铃远端、来电、远端接听、服务不可用、服务可用、信号强度、漫游重置、漫游活跃等事件类型
- 需在HFP SLC连接已建立后调用

**前置条件**

- 蓝牙协议栈已初始化完成
- 对端设备已建立HFP SLC连接

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息指针 | 不为NULL |
| event | [hfp_ag_network_event_t](#enum_hfp_ag_network_event_t) | 网络事件类型 | HFP_AG_NETWORK_EVENT_RMT_IS_BUSY(0x01): 远端忙<br>HFP_AG_NETWORK_EVENT_ALERTING_RM(0x02): 响铃远端<br>HFP_AG_NETWORK_EVENT_INCOMING_CALL(0x03): 来电<br>HFP_AG_NETWORK_EVENT_RMT_ANSWER_CALL(0x04): 远端接听<br>HFP_AG_NETWORK_EVENT_SVC_UNAVAILABLE(0x05): 服务不可用<br>HFP_AG_NETWORK_EVENT_SVC_AVAILABLE(0x06): 服务可用<br>HFP_AG_NETWORK_EVENT_SIGNAL_STRENGTH(0x07): 信号强度<br>HFP_AG_NETWORK_EVENT_ROAMING_RESET(0x08): 漫游重置<br>HFP_AG_NETWORK_EVENT_ROAMING_ACTIVE(0x09): 漫游活跃 |
| param | uint8_t | 事件参数 | 根据事件类型不同，参数含义不同 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 网络事件成功发送 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_ag_set_volume <a id="hfp_ag_set_volume"></a>

```c
int hfp_ag_set_volume(const bd_addr_t *bd_addr, hfp_volume_type_t type, unsigned char volume)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_ag.h"
```

**功能说明**

- AG端发起HF端音量调整
- 调用后，AG端向HF端发送音量增益设置，支持麦克风和扬声器两种音量类型
- 音量增益范围为0-15

**前置条件**

- 蓝牙协议栈已初始化完成
- 对端设备已建立HFP SLC连接

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息指针 | 不为NULL |
| type | [hfp_volume_type_t](#enum_hfp_volume_type_t) | 调整HF音量类型 | HFP_VOLUME_MIC(0x00): 麦克风增益<br>HFP_VOLUME_SPEAKER(0x01): 扬声器增益 |
| volume | unsigned char | 音量增益值 | 0-15 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 音量设置成功发送 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_ag_send_current_calls <a id="hfp_ag_send_current_calls"></a>

```c
int hfp_ag_send_current_calls(const bd_addr_t *bd_addr, hfp_clcc_info_t *info, hfp_ag_last_t complete)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_ag.h"
```

**功能说明**

- AG端发送当前通话列表信息
- 调用后，AG端向HF端发送当前通话列表，每条通话记录包含通话索引、方向、状态、模式、多方通话标记、号码类型、号码长度和号码
- 通过complete参数标识是否为最后一条通话记录

**前置条件**

- 蓝牙协议栈已初始化完成
- 对端设备已建立HFP SLC连接
- 入参info不为NULL，且指向有效的通话信息

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息指针 | 不为NULL |
| info | [hfp_clcc_info_t](#struct_hfp_clcc_info_t) * | 通话信息指针 | 不为NULL |
| complete | [hfp_ag_last_t](#enum_hfp_ag_last_t) | 是否最后一条通话记录 | HFP_AG_LAST_NO(0): 不是最后一条<br>HFP_AG_LAST_YES(1): 是最后一条 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 通话列表成功发送 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_ag_send_subscriber_number <a id="hfp_ag_send_subscriber_number"></a>

```c
int hfp_ag_send_subscriber_number(const bd_addr_t *bd_addr, hfp_phone_info_t *info, hfp_ag_last_t complete)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_ag.h"
```

**功能说明**

- AG端发送订阅号码信息
- 调用后，AG端向HF端发送本机订阅号码信息，包含号码类型、服务类型、号码长度和号码
- 通过complete参数标识是否为最后一条订阅号码信息

**前置条件**

- 蓝牙协议栈已初始化完成
- 对端设备已建立HFP SLC连接
- 入参info不为NULL，且指向有效的订阅号码信息

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息指针 | 不为NULL |
| info | [hfp_phone_info_t](#struct_hfp_phone_info_t) * | 订阅号码信息指针 | 不为NULL |
| complete | [hfp_ag_last_t](#enum_hfp_ag_last_t) | 是否最后一条订阅号码 | HFP_AG_LAST_NO(0): 不是最后一条<br>HFP_AG_LAST_YES(1): 是最后一条 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 订阅号码信息成功发送 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_ag_send_battery_charge <a id="hfp_ag_send_battery_charge"></a>

```c
int hfp_ag_send_battery_charge(const bd_addr_t *bd_addr, uint8_t value)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_ag.h"
```

**功能说明**

- AG端发送电量信息
- 调用后，AG端向HF端发送当前电量值
- 电量值范围为0-5

**前置条件**

- 蓝牙协议栈已初始化完成
- 对端设备已建立HFP SLC连接

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息指针 | 不为NULL |
| value | uint8_t | 电量值 | 0-5 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 电量信息成功发送 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_ag_send_cmee_error <a id="hfp_ag_send_cmee_error"></a>

```c
int hfp_ag_send_cmee_error(const bd_addr_t *bd_addr, hfp_ag_cmee_error_t error_code)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_ag.h"
```

**功能说明**

- AG端发送扩展音频网关错误码(CMEE Error)
- 调用后，AG端向HF端发送扩展错误码，包含AG失败、无连接、操作不允许、操作不支持、SIM (Subscriber Identity Module) 卡异常等错误类型
- 需在HFP SLC连接已建立后调用

**前置条件**

- 蓝牙协议栈已初始化完成
- 对端设备已建立HFP SLC连接

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息指针 | 不为NULL |
| error_code | [hfp_ag_cmee_error_t](#enum_hfp_ag_cmee_error_t) | 扩展的音频网关错误码 | HFP_CMEERR_AGFAILURE(0): AG失败<br>HFP_CMEERR_NOCONN2PHONE(1): 无手机连接<br>HFP_CMEERR_OPERATION_NOTALLOWED(3): 操作不允许<br>HFP_CMEERR_OPERATION_NOTSUPPORTED(4): 操作不支持<br>HFP_CMEERR_PHSIMPIN_REQUIRED(5): 需要PH-SIM卡PIN码<br>HFP_CMEERR_SIMNOT_INSERTED(10): 没有插入SIM卡<br>HFP_CMEERR_SIMPIN_REQUIRED(11): 需要SIM卡PIN码<br>HFP_CMEERR_SIMPUK_REQUIRED(12): 需要SIM卡PUK码<br>HFP_CMEERR_SIM_FAILURE(13): SIM卡失败<br>HFP_CMEERR_SIM_BUSY(14): SIM卡忙<br>HFP_CMEERR_INCORRECT_PASSWORD(16): 不正确的密码<br>HFP_CMEERR_SIMPIN2_REQUIRED(17): 需要SIM卡PIN2码<br>HFP_CMEERR_SIMPUK2_REQUIRED(18): 需要SIM卡PUK2码<br>HFP_CMEERR_MEMORY_FULL(20): 内存满<br>HFP_CMEERR_INVALID_INDEX(21): 无效索引<br>HFP_CMEERR_MEMORY_FAILURE(23): 内存申请失败<br>HFP_CMEERR_TEXTSTRING_TOOLONG(24): 文本字符串过长<br>HFP_CMEERR_INVALID_CHAR_INTEXTSTRING(25): 文本字符串中的无效字符<br>HFP_CMEERR_DIAL_STRING_TOOLONG(26): 拨号过长<br>HFP_CMEERR_INVALID_CHAR_INDIALSTRING(27): 拨号中的无效字符<br>HFP_CMEERR_NETWORK_NOSERVICE(30): 没有网络服务<br>HFP_CMEERR_NETWORK_TIMEOUT(31): 网络超时<br>HFP_CMEERR_EMERGENCYCALL_ONLY(32): 仅支持紧急呼叫 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 扩展错误码成功发送 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_ag_send_network_operator <a id="hfp_ag_send_network_operator"></a>

```c
int hfp_ag_send_network_operator(const bd_addr_t *bd_addr, hfp_cops_info_t *info)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_ag.h"
```

**功能说明**

- AG端发送网络运营商信息
- 调用后，AG端向HF端发送当前网络运营商信息，包含运营商模式、格式和运营商名称
- 需在HFP SLC连接已建立后调用

**前置条件**

- 蓝牙协议栈已初始化完成
- 对端设备已建立HFP SLC连接
- 入参info不为NULL，且指向有效的运营商信息

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息指针 | 不为NULL |
| info | [hfp_cops_info_t](#struct_hfp_cops_info_t) * | 网络运营商信息指针 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 运营商信息成功发送 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_ag_send_indicator_val <a id="hfp_ag_send_indicator_val"></a>

```c
int hfp_ag_send_indicator_val(const bd_addr_t *bd_addr, hfp_cind_info_t *info)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_ag.h"
```

**功能说明**

- AG端发送指示器值信息
- 调用后，AG端向HF端发送当前指示器值，包含服务、呼叫、呼叫建立、呼叫保持、信号强度、漫游、电量等指示器
- 需在HFP SLC连接已建立后调用

**前置条件**

- 蓝牙协议栈已初始化完成
- 对端设备已建立HFP SLC连接
- 入参info不为NULL，且指向有效的指示器值信息

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息指针 | 不为NULL |
| info | [hfp_cind_info_t](#struct_hfp_cind_info_t) * | 指示器值信息指针 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 指示器信息成功发送 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_ag_register_callbacks <a id="hfp_ag_register_callbacks"></a>

```c
int hfp_ag_register_callbacks(hfp_ag_callbacks_t *func)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_ag.h"
```

**功能说明**

- 注册HFP AG上层应用的回调函数
- 注册后，BTS通过回调函数向上层反馈HFP AG连接状态、SCO连接状态、编码格式变化、音量变化、拨号请求、接听请求、挂断请求、DTMF (Dual-Tone Multi-Frequency) 请求、用户编号请求、通话列表请求、指示器请求、运营商请求等事件
- 需在使用其他HFP AG接口之前调用

**前置条件**

- 蓝牙协议栈已初始化完成
- 入参func不为NULL，且指向有效的回调函数定义

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func | [hfp_ag_callbacks_t](#struct_hfp_ag_callbacks_t) * | 指向回调函数接口定义的指针 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 回调注册成功 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

## Type definitions

### hfp_ag_conn_state_changed_callback <a id="typedef_hfp_ag_conn_state_changed_callback"></a>

```c
typedef void (*hfp_ag_conn_state_changed_callback)(const bd_addr_t *bd_addr, profile_connect_state_t state);
```

**使用说明**

HFP AG连接状态变更回调函数类型，在[hfp_ag_callbacks_t](#struct_hfp_ag_callbacks_t)中作为conn_state_changed_cb成员使用

### hfp_ag_sco_conn_state_changed_callback <a id="typedef_hfp_ag_sco_conn_state_changed_callback"></a>

```c
typedef void (*hfp_ag_sco_conn_state_changed_callback)(const bd_addr_t *bd_addr, hfp_sco_connect_state_t state);
```

**使用说明**

SCO音频连接状态变更回调函数类型，在[hfp_ag_callbacks_t](#struct_hfp_ag_callbacks_t)中作为sco_conn_state_changed_cb成员使用

### hfp_ag_codec_changed_callback <a id="typedef_hfp_ag_codec_changed_callback"></a>

```c
typedef void (*hfp_ag_codec_changed_callback)(const bd_addr_t *bd_addr, hfp_ag_codec_id_t codec);
```

**使用说明**

编码格式变化回调函数类型，在[hfp_ag_callbacks_t](#struct_hfp_ag_callbacks_t)中作为codec_changed_cb成员使用

### hfp_ag_volume_changed_callback <a id="typedef_hfp_ag_volume_changed_callback"></a>

```c
typedef void (*hfp_ag_volume_changed_callback)(const bd_addr_t *bd_addr, hfp_volume_type_t type, unsigned char volume);
```

**使用说明**

HF端音量变化回调函数类型，在[hfp_ag_callbacks_t](#struct_hfp_ag_callbacks_t)中作为volume_changed_cb成员使用

### hfp_ag_dial_req_callback <a id="typedef_hfp_ag_dial_req_callback"></a>

```c
typedef void (*hfp_ag_dial_req_callback)(const bd_addr_t *bd_addr, unsigned char *number, unsigned int len);
```

**使用说明**

HF端拨号请求回调函数类型，在[hfp_ag_callbacks_t](#struct_hfp_ag_callbacks_t)中作为dial_req_cb成员使用

### hfp_ag_mem_dial_req_callback <a id="typedef_hfp_ag_mem_dial_req_callback"></a>

```c
typedef void (*hfp_ag_mem_dial_req_callback)(const bd_addr_t *bd_addr, unsigned int index);
```

**使用说明**

HF端记忆拨号请求回调函数类型，在[hfp_ag_callbacks_t](#struct_hfp_ag_callbacks_t)中作为mem_dial_req_cb成员使用

### hfp_ag_lastnum_dial_req_callback <a id="typedef_hfp_ag_lastnum_dial_req_callback"></a>

```c
typedef void (*hfp_ag_lastnum_dial_req_callback)(const bd_addr_t *bd_addr);
```

**使用说明**

HF端最后一次通话拨号请求回调函数类型，在[hfp_ag_callbacks_t](#struct_hfp_ag_callbacks_t)中作为lastnum_dial_req_cb成员使用

### hfp_ag_answer_call_req_callback <a id="typedef_hfp_ag_answer_call_req_callback"></a>

```c
typedef void (*hfp_ag_answer_call_req_callback)(const bd_addr_t *bd_addr, hfp_ag_type_t type);
```

**使用说明**

HF端接听请求回调函数类型，在[hfp_ag_callbacks_t](#struct_hfp_ag_callbacks_t)中作为answer_call_req_cb成员使用

### hfp_ag_cancel_call_req_callback <a id="typedef_hfp_ag_cancel_call_req_callback"></a>

```c
typedef void (*hfp_ag_cancel_call_req_callback)(const bd_addr_t *bd_addr, hfp_ag_type_t type);
```

**使用说明**

HF端挂断请求回调函数类型，在[hfp_ag_callbacks_t](#struct_hfp_ag_callbacks_t)中作为cancel_call_req_cb成员使用

### hfp_ag_dtmf_req_callback <a id="typedef_hfp_ag_dtmf_req_callback"></a>

```c
typedef void (*hfp_ag_dtmf_req_callback)(const bd_addr_t *bd_addr, unsigned int code);
```

**使用说明**

HF端DTMF请求回调函数类型，在[hfp_ag_callbacks_t](#struct_hfp_ag_callbacks_t)中作为dtmf_req_cb成员使用

### hfp_ag_subscriber_number_req_callback <a id="typedef_hfp_ag_subscriber_number_req_callback"></a>

```c
typedef void (*hfp_ag_subscriber_number_req_callback)(const bd_addr_t *bd_addr);
```

**使用说明**

HF端用户编号信息请求回调函数类型，在[hfp_ag_callbacks_t](#struct_hfp_ag_callbacks_t)中作为subscriber_number_req_callback成员使用

### hfp_ag_current_calls_req_callback <a id="typedef_hfp_ag_current_calls_req_callback"></a>

```c
typedef void (*hfp_ag_current_calls_req_callback)(const bd_addr_t *bd_addr);
```

**使用说明**

HF端通话列表请求回调函数类型，在[hfp_ag_callbacks_t](#struct_hfp_ag_callbacks_t)中作为current_calls_req_callback成员使用

### hfp_ag_current_indicator_req_callback <a id="typedef_hfp_ag_current_indicator_req_callback"></a>

```c
typedef void (*hfp_ag_current_indicator_req_callback)(const bd_addr_t *bd_addr);
```

**使用说明**

HF端当前指示器值请求回调函数类型，在[hfp_ag_callbacks_t](#struct_hfp_ag_callbacks_t)中作为current_indicator_req_callback成员使用

### hfp_ag_network_operator_format_req_callback <a id="typedef_hfp_ag_network_operator_format_req_callback"></a>

```c
typedef void (*hfp_ag_network_operator_format_req_callback)(const bd_addr_t *bd_addr);
```

**使用说明**

HF端网络运营商格式请求回调函数类型，在[hfp_ag_callbacks_t](#struct_hfp_ag_callbacks_t)中作为network_operator_format_req_callback成员使用

### hfp_ag_network_operator_req_callback <a id="typedef_hfp_ag_network_operator_req_callback"></a>

```c
typedef void (*hfp_ag_network_operator_req_callback)(const bd_addr_t *bd_addr);
```

**使用说明**

HF端网络运营商请求回调函数类型，在[hfp_ag_callbacks_t](#struct_hfp_ag_callbacks_t)中作为network_operator_req_callback成员使用
## Enumerations

### hfp_ag_codec_id_t <a id="enum_hfp_ag_codec_id_t"></a>

```c
typedef enum {
    HFP_AG_CODEC_ID_CVSD = 0x01,
    HFP_AG_CODEC_ID_MSBC,
} hfp_ag_codec_id_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| HFP_AG_CODEC_ID_CVSD | 0x01 | HFP AG编码格式为CVSD (Continuous Variable Slope Delta) |
| HFP_AG_CODEC_ID_MSBC | 0x02 | HFP AG编码格式为MSBC (Wide Band Speech Codec) |

### hfp_ag_service_indi_t <a id="enum_hfp_ag_service_indi_t"></a>

```c
typedef enum {
    HFP_AG_SERVICE_UNAVAILABLE = 0,
    HFP_AG_SERVICE_AVAILABLE = 1,
} hfp_ag_service_indi_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| HFP_AG_SERVICE_UNAVAILABLE | 0 | 服务不可用 |
| HFP_AG_SERVICE_AVAILABLE | 1 | 服务可用 |

### hfp_ag_call_indi_t <a id="enum_hfp_ag_call_indi_t"></a>

```c
typedef enum {
    HFP_AG_CALL_NO_ACTIVE = 0,
    HFP_AG_CALL_ACTIVE = 1,
} hfp_ag_call_indi_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| HFP_AG_CALL_NO_ACTIVE | 0 | 无活跃通话 |
| HFP_AG_CALL_ACTIVE | 1 | 有活跃通话 |

### hfp_ag_callsetup_indi_t <a id="enum_hfp_ag_callsetup_indi_t"></a>

```c
typedef enum {
    HFP_AG_CALLSETUP_NO = 0,
    HFP_AG_CALLSETUP_INCOMING = 1,
    HFP_AG_CALLSETUP_OUTGOING = 2,
    HFP_AG_CALLSETUP_OUTALERT = 3,
} hfp_ag_callsetup_indi_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| HFP_AG_CALLSETUP_NO | 0 | 无呼叫建立 |
| HFP_AG_CALLSETUP_INCOMING | 1 | 来电呼叫建立 |
| HFP_AG_CALLSETUP_OUTGOING | 2 | 呼出呼叫建立 |
| HFP_AG_CALLSETUP_OUTALERT | 3 | 呼出响铃 |

### hfp_ag_callheld_indi_t <a id="enum_hfp_ag_callheld_indi_t"></a>

```c
typedef enum {
    HFP_AG_CALLHELD_NO = 0,
    HFP_AG_CALLHELD_ACTIVE_HOLD = 1,
    HFP_AG_CALLHELD_ONHOLD = 2,
} hfp_ag_callheld_indi_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| HFP_AG_CALLHELD_NO | 0 | 无呼叫保持 |
| HFP_AG_CALLHELD_ACTIVE_HOLD | 1 | 活跃通话保持 |
| HFP_AG_CALLHELD_ONHOLD | 2 | 通话被保持 |

### hfp_ag_roam_indi_t <a id="enum_hfp_ag_roam_indi_t"></a>

```c
typedef enum {
    HFP_AG_ROAM_NO_ACTIVE = 0,
    HFP_AG_ROAM_ACTIVE = 1,
} hfp_ag_roam_indi_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| HFP_AG_ROAM_NO_ACTIVE | 0 | 漫游未激活 |
| HFP_AG_ROAM_ACTIVE | 1 | 漫游已激活 |

### hfp_ag_call_state_t <a id="enum_hfp_ag_call_state_t"></a>

```c
typedef enum {
    HFP_AG_CALL_STATE_ACTIVE = 0,
    HFP_AG_CALL_STATE_HELD,
    HFP_AG_CALL_STATE_DIALING,
    HFP_AG_CALL_STATE_ALERTING,
    HFP_AG_CALL_STATE_INCOMING,
    HFP_AG_CALL_STATE_WAITING,
} hfp_ag_call_state_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| HFP_AG_CALL_STATE_ACTIVE | 0 | 正在通话 |
| HFP_AG_CALL_STATE_HELD | 1 | 保持通话 |
| HFP_AG_CALL_STATE_DIALING | 2 | 正在拨号 |
| HFP_AG_CALL_STATE_ALERTING | 3 | 正在响铃 |
| HFP_AG_CALL_STATE_INCOMING | 4 | 呼叫接入 |
| HFP_AG_CALL_STATE_WAITING | 5 | 呼叫等待 |

### hfp_ag_network_event_t <a id="enum_hfp_ag_network_event_t"></a>

```c
typedef enum {
    HFP_AG_NETWORK_EVENT_RMT_IS_BUSY = 0x01,
    HFP_AG_NETWORK_EVENT_ALERTING_RM = 0x02,
    HFP_AG_NETWORK_EVENT_INCOMING_CALL = 0x03,
    HFP_AG_NETWORK_EVENT_RMT_ANSWER_CALL = 0x04,
    HFP_AG_NETWORK_EVENT_SVC_UNAVAILABLE = 0x05,
    HFP_AG_NETWORK_EVENT_SVC_AVAILABLE = 0x06,
    HFP_AG_NETWORK_EVENT_SIGNAL_STRENGTH = 0x07,
    HFP_AG_NETWORK_EVENT_ROAMING_RESET = 0x08,
    HFP_AG_NETWORK_EVENT_ROAMING_ACTIVE = 0x09,
} hfp_ag_network_event_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| HFP_AG_NETWORK_EVENT_RMT_IS_BUSY | 0x01 | 远端忙事件 |
| HFP_AG_NETWORK_EVENT_ALERTING_RM | 0x02 | 响铃远端事件 |
| HFP_AG_NETWORK_EVENT_INCOMING_CALL | 0x03 | 来电事件 |
| HFP_AG_NETWORK_EVENT_RMT_ANSWER_CALL | 0x04 | 远端接听事件 |
| HFP_AG_NETWORK_EVENT_SVC_UNAVAILABLE | 0x05 | 服务不可用事件 |
| HFP_AG_NETWORK_EVENT_SVC_AVAILABLE | 0x06 | 服务可用事件 |
| HFP_AG_NETWORK_EVENT_SIGNAL_STRENGTH | 0x07 | 信号强度事件 |
| HFP_AG_NETWORK_EVENT_ROAMING_RESET | 0x08 | 漫游重置事件 |
| HFP_AG_NETWORK_EVENT_ROAMING_ACTIVE | 0x09 | 漫游活跃事件 |

### hfp_ag_type_t <a id="enum_hfp_ag_type_t"></a>

```c
typedef enum {
    HFP_AG_TYPE_ALL_CALLS = 0x01,
    HFP_AG_TYPE_INCOMING_CALL = 0x02,
    HFP_AG_TYPE_HELDINCOMING_CALL = 0x03,
    HFP_AG_TYPE_OUTGOING_CALL = 0x04,
    HFP_AG_TYPE_ONGOING_CALL = 0x05,
} hfp_ag_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| HFP_AG_TYPE_ALL_CALLS | 0x01 | 所有存在的通话 |
| HFP_AG_TYPE_INCOMING_CALL | 0x02 | 来电 |
| HFP_AG_TYPE_HELDINCOMING_CALL | 0x03 | 保持状态的来电 |
| HFP_AG_TYPE_OUTGOING_CALL | 0x04 | 呼叫中的通话 |
| HFP_AG_TYPE_ONGOING_CALL | 0x05 | 进行中的通话 |

### hfp_ag_action_t <a id="enum_hfp_ag_action_t"></a>

```c
typedef enum {
    HFP_AG_CANCELED_ALLCALL   = 0x01,
    HFP_AG_CANCELED_CALLSETUP = 0x02,
    HFP_AG_CANCELED_LASTCALL  = 0x03,
    HFP_AG_CANCELED_CALLHELD  = 0x04,
} hfp_ag_action_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| HFP_AG_CANCELED_ALLCALL | 0x01 | AG释放所有通话或者GSM服务不可用 |
| HFP_AG_CANCELED_CALLSETUP | 0x02 | AG或者GSM释放来电中的通话或者呼叫中的通话 |
| HFP_AG_CANCELED_LASTCALL | 0x03 | AG或者GSM释放最后一次通话 |
| HFP_AG_CANCELED_CALLHELD | 0x04 | GSM释放保持中的通话 |

### hfp_ag_last_t <a id="enum_hfp_ag_last_t"></a>

```c
typedef enum {
    HFP_AG_LAST_NO = 0,
    HFP_AG_LAST_YES = 1,
} hfp_ag_last_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| HFP_AG_LAST_NO | 0 | 不是最后一条 |
| HFP_AG_LAST_YES | 1 | 是最后一条 |

### hfp_ag_cmee_error_t <a id="enum_hfp_ag_cmee_error_t"></a>

```c
typedef enum {
    HFP_CMEERR_AGFAILURE                  = 0,
    HFP_CMEERR_NOCONN2PHONE               = 1,
    HFP_CMEERR_OPERATION_NOTALLOWED       = 3,
    HFP_CMEERR_OPERATION_NOTSUPPORTED     = 4,
    HFP_CMEERR_PHSIMPIN_REQUIRED          = 5,
    HFP_CMEERR_SIMNOT_INSERTED            = 10,
    HFP_CMEERR_SIMPIN_REQUIRED            = 11,
    HFP_CMEERR_SIMPUK_REQUIRED            = 12,
    HFP_CMEERR_SIM_FAILURE                = 13,
    HFP_CMEERR_SIM_BUSY                   = 14,
    HFP_CMEERR_INCORRECT_PASSWORD         = 16,
    HFP_CMEERR_SIMPIN2_REQUIRED           = 17,
    HFP_CMEERR_SIMPUK2_REQUIRED           = 18,
    HFP_CMEERR_MEMORY_FULL                = 20,
    HFP_CMEERR_INVALID_INDEX              = 21,
    HFP_CMEERR_MEMORY_FAILURE             = 23,
    HFP_CMEERR_TEXTSTRING_TOOLONG         = 24,
    HFP_CMEERR_INVALID_CHAR_INTEXTSTRING  = 25,
    HFP_CMEERR_DIAL_STRING_TOOLONG        = 26,
    HFP_CMEERR_INVALID_CHAR_INDIALSTRING  = 27,
    HFP_CMEERR_NETWORK_NOSERVICE          = 30,
    HFP_CMEERR_NETWORK_TIMEOUT            = 31,
    HFP_CMEERR_EMERGENCYCALL_ONLY         = 32,
} hfp_ag_cmee_error_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| HFP_CMEERR_AGFAILURE | 0 | AG失败 |
| HFP_CMEERR_NOCONN2PHONE | 1 | 没有连接到手机 |
| HFP_CMEERR_OPERATION_NOTALLOWED | 3 | 操作不允许 |
| HFP_CMEERR_OPERATION_NOTSUPPORTED | 4 | 操作不支持 |
| HFP_CMEERR_PHSIMPIN_REQUIRED | 5 | 需要PH-SIM卡PIN码 |
| HFP_CMEERR_SIMNOT_INSERTED | 10 | 没有插入SIM卡 |
| HFP_CMEERR_SIMPIN_REQUIRED | 11 | 需要SIM卡PIN码 |
| HFP_CMEERR_SIMPUK_REQUIRED | 12 | 需要SIM卡PUK码 |
| HFP_CMEERR_SIM_FAILURE | 13 | SIM卡失败 |
| HFP_CMEERR_SIM_BUSY | 14 | SIM卡忙 |
| HFP_CMEERR_INCORRECT_PASSWORD | 16 | 不正确的密码 |
| HFP_CMEERR_SIMPIN2_REQUIRED | 17 | 需要SIM卡PIN2码 |
| HFP_CMEERR_SIMPUK2_REQUIRED | 18 | 需要SIM卡PUK2码 |
| HFP_CMEERR_MEMORY_FULL | 20 | 内存满 |
| HFP_CMEERR_INVALID_INDEX | 21 | 无效索引 |
| HFP_CMEERR_MEMORY_FAILURE | 23 | 内存申请失败 |
| HFP_CMEERR_TEXTSTRING_TOOLONG | 24 | 文本字符串过长 |
| HFP_CMEERR_INVALID_CHAR_INTEXTSTRING | 25 | 文本字符串中的无效字符 |
| HFP_CMEERR_DIAL_STRING_TOOLONG | 26 | 拨号过长 |
| HFP_CMEERR_INVALID_CHAR_INDIALSTRING | 27 | 拨号中的无效字符 |
| HFP_CMEERR_NETWORK_NOSERVICE | 30 | 没有网络服务 |
| HFP_CMEERR_NETWORK_TIMEOUT | 31 | 网络超时 |
| HFP_CMEERR_EMERGENCYCALL_ONLY | 32 | 网络不允许，仅支持紧急呼叫 |

### hfp_ag_clcc_mode_t <a id="enum_hfp_ag_clcc_mode_t"></a>

```c
typedef enum {
    HFP_AG_CLCC_VOICE = 0,
    HFP_AG_CLCC_DATA = 1,
    HFP_AG_CLCC_FAX = 2,
} hfp_ag_clcc_mode_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| HFP_AG_CLCC_VOICE | 0 | 语音模式 |
| HFP_AG_CLCC_DATA | 1 | 数据模式 |
| HFP_AG_CLCC_FAX | 2 | 传真模式 |

### hfp_ag_clcc_mpty_t <a id="enum_hfp_ag_clcc_mpty_t"></a>

```c
typedef enum {
    HFP_AG_CLCC_NOT_MULTI = 0,
    HFP_AG_CLCC_MULTI = 1,
} hfp_ag_clcc_mpty_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| HFP_AG_CLCC_NOT_MULTI | 0 | 非多方通话 |
| HFP_AG_CLCC_MULTI | 1 | 多方通话 |

### hfp_ag_clcc_dir_t <a id="enum_hfp_ag_clcc_dir_t"></a>

```c
typedef enum {
    HFP_AG_CLCC_OUTGOING = 0,
    HFP_AG_CLCC_INCOMING = 1,
} hfp_ag_clcc_dir_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| HFP_AG_CLCC_OUTGOING | 0 | 呼出方向 |
| HFP_AG_CLCC_INCOMING | 1 | 呼入方向 |

### hfp_ag_cnum_service_t <a id="enum_hfp_ag_cnum_service_t"></a>

```c
typedef enum {
    HFP_AG_CNUM_ASYNC = 0,
    HFP_AG_CNUM_SYNC = 1,
    HFP_AG_CNUM_PAD = 2,
    HFP_AG_CNUM_PACKET = 3,
    HFP_AG_CNUM_VOICE = 4,
    HFP_AG_CNUM_FAX = 5,
} hfp_ag_cnum_service_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| HFP_AG_CNUM_ASYNC | 0 | 异步服务 |
| HFP_AG_CNUM_SYNC | 1 | 同步服务 |
| HFP_AG_CNUM_PAD | 2 | PAD服务 |
| HFP_AG_CNUM_PACKET | 3 | 分组服务 |
| HFP_AG_CNUM_VOICE | 4 | 语音服务 |
| HFP_AG_CNUM_FAX | 5 | 传真服务 |

### hfp_ag_cops_format_t <a id="enum_hfp_ag_cops_format_t"></a>

```c
typedef enum {
    HFP_AG_COPS_LONG_ALPHANUM = 0,
    HFP_AG_COPS_SHORT_ALPHANUM = 1,
    HFP_AG_COPS_NUM = 2,
} hfp_ag_cops_format_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| HFP_AG_COPS_LONG_ALPHANUM | 0 | 长格式的字符串数值，最大长度为16 |
| HFP_AG_COPS_SHORT_ALPHANUM | 1 | 短格式的字符串数值，最大长度为8 |
| HFP_AG_COPS_NUM | 2 | 数值格式 |

### hfp_ag_cops_mode_t <a id="enum_hfp_ag_cops_mode_t"></a>

```c
typedef enum {
    HFP_AG_COPS_AUTO = 0,
    HFP_AG_COPS_MANU = 1,
    HFP_AG_COPS_OFF = 2,
    HFP_AG_COPS_DISABLE = 3,
    HFP_AG_COPS_MANU_AUTO = 4,
} hfp_ag_cops_mode_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| HFP_AG_COPS_AUTO | 0 | 自动 |
| HFP_AG_COPS_MANU | 1 | 手动 |
| HFP_AG_COPS_OFF | 2 | 断开网络 |
| HFP_AG_COPS_DISABLE | 3 | 仅在读命令中配置 |
| HFP_AG_COPS_MANU_AUTO | 4 | 自动/手动自动切换 |

### hfp_sco_connect_state_t <a id="enum_hfp_sco_connect_state_t"></a>

```c
typedef enum {
    HFP_SCO_STATE_CONNECTING = 0x01,
    HFP_SCO_STATE_CONNECTED = 0x02,
    HFP_SCO_STATE_DISCONNECTING = 0x03,
    HFP_SCO_STATE_DISCONNECTED = 0x04
} hfp_sco_connect_state_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| HFP_SCO_STATE_CONNECTING | 0x01 | sco正在连接 |
| HFP_SCO_STATE_CONNECTED | 0x02 | sco已连接 |
| HFP_SCO_STATE_DISCONNECTING | 0x03 | sco正在断连 |
| HFP_SCO_STATE_DISCONNECTED | 0x04 | sco已断连 |

### hfp_volume_type_t <a id="enum_hfp_volume_type_t"></a>

```c
typedef enum {
    HFP_VOLUME_MIC = 0x00,
    HFP_VOLUME_SPEAKER
} hfp_volume_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| HFP_VOLUME_MIC | 0x00 | 调整microphone增益 |
| HFP_VOLUME_SPEAKER | 0x01 | 调整speaker增益 |

### profile_connect_state_t <a id="enum_profile_connect_state_t"></a>

```c
typedef enum {
    PROFILE_STATE_CONNECTING = 0x01,
    PROFILE_STATE_CONNECTED = 0x02,
    PROFILE_STATE_DISCONNECTING = 0x03,
    PROFILE_STATE_DISCONNECTED = 0x04
} profile_connect_state_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| PROFILE_STATE_CONNECTING | 0x01 | 正在连接 |
| PROFILE_STATE_CONNECTED | 0x02 | 已连接 |
| PROFILE_STATE_DISCONNECTING | 0x03 | 正在断连 |
| PROFILE_STATE_DISCONNECTED | 0x04 | 已断连 |

### errcode_bt_t <a id="enum_errcode_bt_t"></a>

```c
typedef enum {
    ERRCODE_BT_SUCCESS = 0x00,
    ERRCODE_BT_FAIL = ERRCODE_BT_COMMON_BASE,
    ERRCODE_BT_NOT_READY,
    ERRCODE_BT_MALLOC_FAIL,
    ERRCODE_BT_MEMCPY_FAIL,
    ERRCODE_BT_BUSY,
    ERRCODE_BT_DONE,
    ERRCODE_BT_UNSUPPORTED,
    ERRCODE_BT_PARAM_ERR,
    ERRCODE_BT_STATE_ERR,
    ERRCODE_BT_UNHANDLED,
    ERRCODE_BT_AUTH_FAIL,
    ERRCODE_BT_RMT_DEV_DOWN,
    ERRCODE_BT_AUTH_REJECTED,
    ERRCODE_BT_MAX = ERRCODE_BT_COMMON_END
} errcode_bt_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| ERRCODE_BT_SUCCESS | 0x00 | 执行成功 |
| ERRCODE_BT_FAIL | 0x80006000 | 执行失败 |
| ERRCODE_BT_NOT_READY | 0x80006001 | 执行状态未就绪 |
| ERRCODE_BT_MALLOC_FAIL | 0x80006002 | 内存不足 |
| ERRCODE_BT_MEMCPY_FAIL | 0x80006003 | 内存拷贝失败 |
| ERRCODE_BT_BUSY | 0x80006004 | 繁忙无法响应 |
| ERRCODE_BT_DONE | 0x80006005 | 执行完成 |
| ERRCODE_BT_UNSUPPORTED | 0x80006006 | 不支持 |
| ERRCODE_BT_PARAM_ERR | 0x80006007 | 无效参数 |
| ERRCODE_BT_STATE_ERR | 0x80006008 | 状态错误 |
| ERRCODE_BT_UNHANDLED | 0x80006009 | 未处理 |
| ERRCODE_BT_AUTH_FAIL | 0x8000600A | 鉴权失败 |
| ERRCODE_BT_RMT_DEV_DOWN | 0x8000600B | 远端设备关闭 |
| ERRCODE_BT_AUTH_REJECTED | 0x8000600C | 鉴权被拒 |
| ERRCODE_BT_MAX | 0x800067FF | 蓝牙错误码最大值 |

## Structures

### hfp_phone_info_t <a id="struct_hfp_phone_info_t"></a>

```c
typedef struct {
    uint8_t type;
    uint8_t service;
    uint8_t num_len;
    int8_t  number[HFP_AG_PHONENUM_MAX_DIGITS];
    uint8_t name_len;
    int8_t  alpha_str[1];
} hfp_phone_info_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| type | uint8_t | 电话号码类型，值范围是128-175 |
| service | uint8_t | 服务类型，协议支持4或5，见 [hfp_ag_cnum_service_t](#enum_hfp_ag_cnum_service_t) |
| num_len | uint8_t | 电话号码长度 |
| number | int8_t[32] | 订阅号码，最大长度是32 |
| name_len | uint8_t | 子地址长度 |
| alpha_str | int8_t[1] | 子地址，协议默认是0 |

### hfp_clcc_info_t <a id="struct_hfp_clcc_info_t"></a>

```c
typedef struct {
    uint8_t idx;
    uint8_t dir;
    uint8_t status;
    uint8_t mode;
    uint8_t mpty;
    uint8_t type;
    uint8_t num_len;
    int8_t  number[1];
} hfp_clcc_info_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| idx | uint8_t | 通话记录索引，从1开始 |
| dir | uint8_t | 方向，见 [hfp_ag_clcc_dir_t](#enum_hfp_ag_clcc_dir_t) |
| status | uint8_t | 状态，见 [hfp_ag_call_state_t](#enum_hfp_ag_call_state_t) |
| mode | uint8_t | 模式，见 [hfp_ag_clcc_mode_t](#enum_hfp_ag_clcc_mode_t) |
| mpty | uint8_t | 多方通话标记，见 [hfp_ag_clcc_mpty_t](#enum_hfp_ag_clcc_mpty_t) |
| type | uint8_t | 电话号码类型，值范围是128-175 |
| num_len | uint8_t | 电话号码长度 |
| number | int8_t[1] | 电话号码 |

### hfp_cops_info_t <a id="struct_hfp_cops_info_t"></a>

```c
typedef struct {
    uint8_t mode;
    uint8_t format;
    uint8_t operator_len;
    int8_t  operator_name[1];
} hfp_cops_info_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| mode | uint8_t | 当前模式，见 [hfp_ag_cops_mode_t](#enum_hfp_ag_cops_mode_t) |
| format | uint8_t | 字符串格式，协议仅支持长格式，见 [hfp_ag_cops_format_t](#enum_hfp_ag_cops_format_t) |
| operator_len | uint8_t | 运营商名字长度 |
| operator_name | int8_t[1] | 运营商名字 |

### hfp_cind_info_t <a id="struct_hfp_cind_info_t"></a>

```c
typedef struct {
    uint8_t     service;
    uint8_t     call;
    uint8_t     callsetup;
    uint8_t     callheld;
    uint8_t     signal;
    uint8_t     roam;
    uint8_t     battchg;
} hfp_cind_info_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| service | uint8_t | 服务，见 [hfp_ag_service_indi_t](#enum_hfp_ag_service_indi_t) |
| call | uint8_t | 呼叫，见 [hfp_ag_call_indi_t](#enum_hfp_ag_call_indi_t) |
| callsetup | uint8_t | 呼叫建立，见 [hfp_ag_callsetup_indi_t](#enum_hfp_ag_callsetup_indi_t) |
| callheld | uint8_t | 呼叫保持，见 [hfp_ag_callheld_indi_t](#enum_hfp_ag_callheld_indi_t) |
| signal | uint8_t | 信号强度，值范围0-5 |
| roam | uint8_t | 漫游，见 [hfp_ag_roam_indi_t](#enum_hfp_ag_roam_indi_t) |
| battchg | uint8_t | 电量，值范围0-5 |

### hfp_ag_callbacks_t <a id="struct_hfp_ag_callbacks_t"></a>

```c
typedef struct {
    hfp_ag_conn_state_changed_callback                  conn_state_changed_cb;
    hfp_ag_sco_conn_state_changed_callback              sco_conn_state_changed_cb;
    hfp_ag_codec_changed_callback                       codec_changed_cb;
    hfp_ag_volume_changed_callback                      volume_changed_cb;
    hfp_ag_dial_req_callback                            dial_req_cb;
    hfp_ag_mem_dial_req_callback                        mem_dial_req_cb;
    hfp_ag_lastnum_dial_req_callback                    lastnum_dial_req_cb;
    hfp_ag_answer_call_req_callback                     answer_call_req_cb;
    hfp_ag_cancel_call_req_callback                     cancel_call_req_cb;
    hfp_ag_dtmf_req_callback                            dtmf_req_cb;
    hfp_ag_subscriber_number_req_callback               subscriber_number_req_callback;
    hfp_ag_current_calls_req_callback                   current_calls_req_callback;
    hfp_ag_current_indicator_req_callback               current_indicator_req_callback;
    hfp_ag_network_operator_format_req_callback         network_operator_format_req_callback;
    hfp_ag_network_operator_req_callback                network_operator_req_callback;
} hfp_ag_callbacks_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| conn_state_changed_cb | [hfp_ag_conn_state_changed_callback](#typedef_hfp_ag_conn_state_changed_callback) | HFP AG连接状态变更回调 |
| sco_conn_state_changed_cb | [hfp_ag_sco_conn_state_changed_callback](#typedef_hfp_ag_sco_conn_state_changed_callback) | SCO音频连接状态变更回调 |
| codec_changed_cb | [hfp_ag_codec_changed_callback](#typedef_hfp_ag_codec_changed_callback) | 编码格式变化回调 |
| volume_changed_cb | [hfp_ag_volume_changed_callback](#typedef_hfp_ag_volume_changed_callback) | HF端音量变化回调 |
| dial_req_cb | [hfp_ag_dial_req_callback](#typedef_hfp_ag_dial_req_callback) | HF端拨号请求回调 |
| mem_dial_req_cb | [hfp_ag_mem_dial_req_callback](#typedef_hfp_ag_mem_dial_req_callback) | HF端记忆拨号请求回调 |
| lastnum_dial_req_cb | [hfp_ag_lastnum_dial_req_callback](#typedef_hfp_ag_lastnum_dial_req_callback) | HF端最后一次通话拨号请求回调 |
| answer_call_req_cb | [hfp_ag_answer_call_req_callback](#typedef_hfp_ag_answer_call_req_callback) | HF端接听请求回调 |
| cancel_call_req_cb | [hfp_ag_cancel_call_req_callback](#typedef_hfp_ag_cancel_call_req_callback) | HF端挂断请求回调 |
| dtmf_req_cb | [hfp_ag_dtmf_req_callback](#typedef_hfp_ag_dtmf_req_callback) | HF端DTMF请求回调 |
| subscriber_number_req_callback | [hfp_ag_subscriber_number_req_callback](#typedef_hfp_ag_subscriber_number_req_callback) | HF端用户编号信息请求回调 |
| current_calls_req_callback | [hfp_ag_current_calls_req_callback](#typedef_hfp_ag_current_calls_req_callback) | HF端通话列表请求回调 |
| current_indicator_req_callback | [hfp_ag_current_indicator_req_callback](#typedef_hfp_ag_current_indicator_req_callback) | HF端当前指示器值请求回调 |
| network_operator_format_req_callback | [hfp_ag_network_operator_format_req_callback](#typedef_hfp_ag_network_operator_format_req_callback) | HF端网络运营商格式请求回调 |
| network_operator_req_callback | [hfp_ag_network_operator_req_callback](#typedef_hfp_ag_network_operator_req_callback) | HF端网络运营商请求回调 |

### bd_addr_t <a id="struct_bd_addr_t"></a>

```c
typedef struct {
    uint8_t addr[BD_ADDR_LEN];
    uint8_t type;
} bd_addr_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| addr | uint8_t[6] | 蓝牙地址 |
| type | uint8_t | 蓝牙地址类型，见 [bt_addr_type](../../a2dp/a2dp.md#enum_bt_addr_type) |



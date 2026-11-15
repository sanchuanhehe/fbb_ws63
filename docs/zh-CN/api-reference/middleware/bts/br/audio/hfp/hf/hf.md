# HFP HF

HFP (Hands-Free Profile) HF（Hands-Free Profile，Hands-Free，免提）实现蓝牙免提配置文件的免提端角色，本设备作为免提设备与对端音频网关（AG）协同工作。本模块提供与 AG 建立服务级连接（SLC）和音频连接（SCO）、电话呼叫状态管理、音量调节以及编解码协商等接口。

**头文件清单**
```c
#include "include/middleware/services/bts/br/bts_hfp_hf.h"
#include "include/middleware/services/bts/common/bts_def.h"
#include "include/middleware/services/bts/br/bts_br_gap.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [hfp_hf_connect](#hfp_hf_connect) | 建立与AG (Audio Gateway) 的SLC (service level connection) |
| [hfp_hf_disconnect](#hfp_hf_disconnect) | 释放与AG的SLC |
| [hfp_hf_connect_sco](#hfp_hf_connect_sco) | 建立与AG的音频连接 |
| [hfp_hf_disconnect_sco](#hfp_hf_disconnect_sco) | 断开与AG的音频连接 |
| [hfp_hf_get_device_connect_state](#hfp_hf_get_device_connect_state) | 查询设备HFP连接状态 |
| [hfp_hf_get_sco_connect_state](#hfp_hf_get_sco_connect_state) | 查询设备HFP SCO (Synchronous Connection-Oriented) 连接状态 |
| [hfp_hf_send_dtmf_tone](#hfp_hf_send_dtmf_tone) | 指示AG发送DTMF (Dual-Tone Multi-Frequency)tone code |
| [hfp_hf_open_voice_recognition](#hfp_hf_open_voice_recognition) | 指示AG打开语音识别功能 |
| [hfp_hf_close_voice_recognition](#hfp_hf_close_voice_recognition) | 指示AG关闭语音识别功能 |
| [hfp_hf_accept_incoming_call](#hfp_hf_accept_incoming_call) | 接听呼入电话 |
| [hfp_hf_hold_active_call](#hfp_hf_hold_active_call) | 将当前激活状态的通话保持 |
| [hfp_hf_reject_incoming_call](#hfp_hf_reject_incoming_call) | 拒接呼入电话 |
| [hfp_hf_finish_call](#hfp_hf_finish_call) | 结束通话 |
| [hfp_hf_start_dial](#hfp_hf_start_dial) | 发起主叫 |
| [hfp_hf_set_volume](#hfp_hf_set_volume) | HF端发起音量调整 |
| [hfp_hf_register_callbacks](#hfp_hf_register_callbacks) | 注册上层应用的回调 |

## Functions

### hfp_hf_connect <a id="hfp_hf_connect"></a>

```c
int hfp_hf_connect(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_hf.h"
```

**功能说明**

- 建立与AG的SLC
- 该接口运行于bts线程，不能阻塞或长时间等待
- <devices>由bts申请内存，也由bts释放，回调中不应释放

**前置条件**

- 蓝牙协议栈已通过enable_bt_stack()初始化完成
- bd_addr不为NULL，且指向有效的远端蓝牙设备地址

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_hf_disconnect <a id="hfp_hf_disconnect"></a>

```c
int hfp_hf_disconnect(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_hf.h"
```

**功能说明**

- 释放与AG的SLC
- 该接口运行于bts线程，不能阻塞或长时间等待
- <devices>由bts申请内存，也由bts释放，回调中不应释放

**前置条件**

- 蓝牙协议栈已通过enable_bt_stack()初始化完成
- bd_addr不为NULL，且指向有效的远端蓝牙设备地址

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_hf_connect_sco <a id="hfp_hf_connect_sco"></a>

```c
int hfp_hf_connect_sco(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_hf.h"
```

**功能说明**

- 建立与AG的音频连接
- 该接口运行于bts线程，不能阻塞或长时间等待
- <devices>由bts申请内存，也由bts释放，回调中不应释放

**前置条件**

- 蓝牙协议栈已通过enable_bt_stack()初始化完成
- 与AG的HFP SLC已建立
- bd_addr不为NULL，且指向有效的远端蓝牙设备地址

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_hf_disconnect_sco <a id="hfp_hf_disconnect_sco"></a>

```c
int hfp_hf_disconnect_sco(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_hf.h"
```

**功能说明**

- 断开与AG的音频连接
- 该接口运行于bts线程，不能阻塞或长时间等待
- <devices>由bts申请内存，也由bts释放，回调中不应释放

**前置条件**

- 蓝牙协议栈已通过enable_bt_stack()初始化完成
- 与AG的SCO音频连接已建立
- bd_addr不为NULL，且指向有效的远端蓝牙设备地址

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_hf_get_device_connect_state <a id="hfp_hf_get_device_connect_state"></a>

```c
int hfp_hf_get_device_connect_state(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_hf.h"
```

**功能说明**

- 通过设备地址查询设备HFP连接状态
- 该接口运行于bts线程，不能阻塞或长时间等待
- <devices>由bts申请内存，也由bts释放，回调中不应释放

**前置条件**

- 蓝牙协议栈已通过enable_bt_stack()初始化完成
- bd_addr不为NULL，且指向有效的远端蓝牙设备地址

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息 | 不为NULL |

**返回值**

- 返回类型：int（[profile_connect_state_t](#enum_profile_connect_state_t)）

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| PROFILE_STATE_CONNECTING(0x01) | 正在连接 | 正在建立连接 |
| PROFILE_STATE_CONNECTED(0x02) | 已连接 | 连接已建立 |
| PROFILE_STATE_DISCONNECTING(0x03) | 正在断连 | 正在断开连接 |
| PROFILE_STATE_DISCONNECTED(0x04) | 已断连 | 连接已断开 |

### hfp_hf_get_sco_connect_state <a id="hfp_hf_get_sco_connect_state"></a>

```c
int hfp_hf_get_sco_connect_state(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_hf.h"
```

**功能说明**

- 通过设备地址查询设备HFP SCO连接状态
- 该接口运行于bts线程，不能阻塞或长时间等待
- <devices>由bts申请内存，也由bts释放，回调中不应释放

**前置条件**

- 蓝牙协议栈已通过enable_bt_stack()初始化完成
- bd_addr不为NULL，且指向有效的远端蓝牙设备地址

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息 | 不为NULL |

**返回值**

- 返回类型：int（[hfp_sco_connect_state_t](#enum_hfp_sco_connect_state_t)）

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| HFP_SCO_STATE_CONNECTING(0x01) | SCO正在连接 | 正在建立SCO连接 |
| HFP_SCO_STATE_CONNECTED(0x02) | SCO已连接 | SCO连接已建立 |
| HFP_SCO_STATE_DISCONNECTING(0x03) | SCO正在断连 | 正在断开SCO连接 |
| HFP_SCO_STATE_DISCONNECTED(0x04) | SCO已断连 | SCO连接已断开 |

### hfp_hf_send_dtmf_tone <a id="hfp_hf_send_dtmf_tone"></a>

```c
int hfp_hf_send_dtmf_tone(const bd_addr_t *bd_addr, unsigned char code)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_hf.h"
```

**功能说明**

- 指示AG发送DTMF tone code
- 该接口运行于bts线程，不能阻塞或长时间等待
- <devices>由bts申请内存，也由bts释放，回调中不应释放

**前置条件**

- 蓝牙协议栈已通过enable_bt_stack()初始化完成
- 与AG的HFP SLC已建立
- bd_addr不为NULL，且指向有效的远端蓝牙设备地址

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息 | 不为NULL |
| code | unsigned char | DTMF tone对应的字符 | ASCII (American Standard Code for Information Interchange) 字符 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_hf_open_voice_recognition <a id="hfp_hf_open_voice_recognition"></a>

```c
int hfp_hf_open_voice_recognition(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_hf.h"
```

**功能说明**

- 指示AG打开语音识别功能
- 该接口运行于bts线程，不能阻塞或长时间等待
- <devices>由bts申请内存，也由bts释放，回调中不应释放

**前置条件**

- 蓝牙协议栈已通过enable_bt_stack()初始化完成
- 与AG的HFP SLC已建立
- bd_addr不为NULL，且指向有效的远端蓝牙设备地址

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_hf_close_voice_recognition <a id="hfp_hf_close_voice_recognition"></a>

```c
int hfp_hf_close_voice_recognition(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_hf.h"
```

**功能说明**

- 指示AG关闭语音识别功能
- 该接口运行于bts线程，不能阻塞或长时间等待
- <devices>由bts申请内存，也由bts释放，回调中不应释放

**前置条件**

- 蓝牙协议栈已通过enable_bt_stack()初始化完成
- 与AG的HFP SLC已建立
- bd_addr不为NULL，且指向有效的远端蓝牙设备地址

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_hf_accept_incoming_call <a id="hfp_hf_accept_incoming_call"></a>

```c
int hfp_hf_accept_incoming_call(const bd_addr_t *bd_addr, hfp_hf_accept_call_action_type_t action_type)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_hf.h"
```

**功能说明**

- 接听呼入电话
- 该接口运行于bts线程，不能阻塞或长时间等待
- <devices>由bts申请内存，也由bts释放，回调中不应释放

**前置条件**

- 蓝牙协议栈已通过enable_bt_stack()初始化完成
- 与AG的HFP SLC已建立
- bd_addr不为NULL，且指向有效的远端蓝牙设备地址

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息 | 不为NULL |
| action_type | [hfp_hf_accept_call_action_type_t](#enum_hfp_hf_accept_call_action_type_t) | 接受呼叫时对其他呼叫的操作策略 | HFP_HF_ACCEPT_CALL_ACTION_NONE(0x00) / HFP_HF_ACCEPT_CALL_ACTION_HOLD(0x01) / HFP_HF_ACCEPT_CALL_ACTION_FINISH(0x02) |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_hf_hold_active_call <a id="hfp_hf_hold_active_call"></a>

```c
int hfp_hf_hold_active_call(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_hf.h"
```

**功能说明**

- 将当前通话状态是激活状态的通话保持
- 该接口运行于bts线程，不能阻塞或长时间等待
- <devices>由bts申请内存，也由bts释放，回调中不应释放

**前置条件**

- 蓝牙协议栈已通过enable_bt_stack()初始化完成
- 与AG的HFP SLC已建立
- bd_addr不为NULL，且指向有效的远端蓝牙设备地址

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_hf_reject_incoming_call <a id="hfp_hf_reject_incoming_call"></a>

```c
int hfp_hf_reject_incoming_call(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_hf.h"
```

**功能说明**

- 拒接呼入电话
- 该接口运行于bts线程，不能阻塞或长时间等待
- <devices>由bts申请内存，也由bts释放，回调中不应释放

**前置条件**

- 蓝牙协议栈已通过enable_bt_stack()初始化完成
- 与AG的HFP SLC已建立
- bd_addr不为NULL，且指向有效的远端蓝牙设备地址

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_hf_finish_call <a id="hfp_hf_finish_call"></a>

```c
int hfp_hf_finish_call(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_hf.h"
```

**功能说明**

- 结束通话
- 该接口用于结束正在进行和正在呼出的电话，但不能用于拒接呼入电话，拒接呼入电话请使用hfp_hf_reject_incoming_call接口
- 该接口运行于bts线程，不能阻塞或长时间等待
- <devices>由bts申请内存，也由bts释放，回调中不应释放

**前置条件**

- 蓝牙协议栈已通过enable_bt_stack()初始化完成
- 与AG的HFP SLC已建立
- bd_addr不为NULL，且指向有效的远端蓝牙设备地址

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_hf_start_dial <a id="hfp_hf_start_dial"></a>

```c
int hfp_hf_start_dial(const bd_addr_t *bd_addr, const unsigned char *number, unsigned char len)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_hf.h"
```

**功能说明**

- 发起主叫，发送"ATDdd...dd;"指令，例如拨号123456，则指令为"ATD123456;"
- 该接口运行于bts线程，不能阻塞或长时间等待
- <devices>由bts申请内存，也由bts释放，回调中不应释放

**前置条件**

- 蓝牙协议栈已通过enable_bt_stack()初始化完成
- 与AG的HFP SLC已建立
- bd_addr不为NULL，且指向有效的远端蓝牙设备地址
- number不为NULL，且指向有效的电话号码字符串

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息 | 不为NULL |
| number | const unsigned char * | 电话号码 | 不为NULL |
| len | unsigned char | 电话号码长度 | 大于0 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_hf_set_volume <a id="hfp_hf_set_volume"></a>

```c
int hfp_hf_set_volume(const bd_addr_t *bd_addr, hfp_volume_type_t type, unsigned char volume)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_hf.h"
```

**功能说明**

- HF端发起音量调整
- 该接口运行于bts线程，不能阻塞或长时间等待
- <devices>由bts申请内存，也由bts释放，回调中不应释放

**前置条件**

- 蓝牙协议栈已通过enable_bt_stack()初始化完成
- 与AG的HFP SLC已建立
- bd_addr不为NULL，且指向有效的远端蓝牙设备地址

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息 | 不为NULL |
| type | [hfp_volume_type_t](#enum_hfp_volume_type_t) | 调整HF音量类型 | HFP_VOLUME_MIC(0x00) / HFP_VOLUME_SPEAKER(0x01) |
| volume | unsigned char | 音量增益 | 0-15 |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### hfp_hf_register_callbacks <a id="hfp_hf_register_callbacks"></a>

```c
int hfp_hf_register_callbacks(hfp_hf_callbacks_t *func)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_hfp_hf.h"
```

**功能说明**

- 注册上层应用的回调函数
- 该接口运行于bts线程，不能阻塞或长时间等待
- <devices>由bts申请内存，也由bts释放，回调中不应释放

**前置条件**

- 蓝牙协议栈已通过enable_bt_stack()初始化完成
- func不为NULL，且指向有效的回调函数结构体

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func | [hfp_hf_callbacks_t](#struct_hfp_hf_callbacks_t) * | 指向回调函数接口定义的指针 | 不为NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

## Type definitions

### bt_uuid_t <a id="typedef_bt_uuid_t"></a>

```c
typedef struct {
    uint8_t uuid_len;
    uint8_t uuid[BT_UUID_MAX_LEN];
} bt_uuid_t;
```

**使用说明**

在[hfp_hf_calls_info_t](#struct_hfp_hf_calls_info_t)结构体中作为uuid成员的类型，标识电话服务UUID (Universally Unique Identifier)

### hfp_hf_conn_state_changed_callback <a id="typedef_hfp_hf_conn_state_changed_callback"></a>

```c
typedef void (*hfp_hf_conn_state_changed_callback)(const bd_addr_t *bd_addr, profile_connect_state_t state);
```

**使用说明**

在[hfp_hf_callbacks_t](#struct_hfp_hf_callbacks_t)结构体中作为conn_state_changed_cb成员的类型，HFP连接状态变化时由BTS (Bluetooth Subsystem) 回调

### hfp_hf_sco_conn_state_changed_callback <a id="typedef_hfp_hf_sco_conn_state_changed_callback"></a>

```c
typedef void (*hfp_hf_sco_conn_state_changed_callback)(const bd_addr_t *bd_addr, hfp_sco_connect_state_t state);
```

**使用说明**

在[hfp_hf_callbacks_t](#struct_hfp_hf_callbacks_t)结构体中作为sco_conn_state_changed_cb成员的类型，SCO连接状态变化时由BTS回调

### hfp_hf_battery_level_changed_callback <a id="typedef_hfp_hf_battery_level_changed_callback"></a>

```c
typedef void (*hfp_hf_battery_level_changed_callback)(const bd_addr_t *bd_addr, int battery_level);
```

**使用说明**

在[hfp_hf_callbacks_t](#struct_hfp_hf_callbacks_t)结构体中作为battery_level_changed_cb成员的类型，AG电量变化时由BTS回调

### hfp_hf_signal_strength_changed_callback <a id="typedef_hfp_hf_signal_strength_changed_callback"></a>

```c
typedef void (*hfp_hf_signal_strength_changed_callback)(const bd_addr_t *bd_addr, int signal_strength);
```

**使用说明**

在[hfp_hf_callbacks_t](#struct_hfp_hf_callbacks_t)结构体中作为signal_strength_changed_cb成员的类型，AG信号强度变化时由BTS回调

### hfp_hf_registration_status_changed_callback <a id="typedef_hfp_hf_registration_status_changed_callback"></a>

```c
typedef void (*hfp_hf_registration_status_changed_callback)(const bd_addr_t *bd_addr, int status);
```

**使用说明**

在[hfp_hf_callbacks_t](#struct_hfp_hf_callbacks_t)结构体中作为registration_status_changed_cb成员的类型，AG注册状态变化时由BTS回调

### hfp_hf_roaming_status_changed_callback <a id="typedef_hfp_hf_roaming_status_changed_callback"></a>

```c
typedef void (*hfp_hf_roaming_status_changed_callback)(const bd_addr_t *bd_addr, int status);
```

**使用说明**

在[hfp_hf_callbacks_t](#struct_hfp_hf_callbacks_t)结构体中作为roaming_status_changed_cb成员的类型，AG漫游状态变化时由BTS回调

### hfp_hf_operator_selection_callback <a id="typedef_hfp_hf_operator_selection_callback"></a>

```c
typedef void (*hfp_hf_operator_selection_callback)(const bd_addr_t *bd_addr, unsigned char *name, unsigned int len);
```

**使用说明**

在[hfp_hf_callbacks_t](#struct_hfp_hf_callbacks_t)结构体中作为operator_selection_cb成员的类型，查询AG运营商信息收到回复时由BTS回调

### hfp_hf_subscriber_number_callback <a id="typedef_hfp_hf_subscriber_number_callback"></a>

```c
typedef void (*hfp_hf_subscriber_number_callback)(const bd_addr_t *bd_addr, unsigned char *number, unsigned int len);
```

**使用说明**

在[hfp_hf_callbacks_t](#struct_hfp_hf_callbacks_t)结构体中作为subscriber_number_cb成员的类型，查询AG用户号码收到回复时由BTS回调

### hfp_hf_voice_recognition_status_changed_callback <a id="typedef_hfp_hf_voice_recognition_status_changed_callback"></a>

```c
typedef void (*hfp_hf_voice_recognition_status_changed_callback)(const bd_addr_t *bd_addr, int status);
```

**使用说明**

在[hfp_hf_callbacks_t](#struct_hfp_hf_callbacks_t)结构体中作为voice_recognition_status_changed_cb成员的类型，语音识别状态变化时由BTS回调

### hfp_hf_in_band_ring_tone_changed_callback <a id="typedef_hfp_hf_in_band_ring_tone_changed_callback"></a>

```c
typedef void (*hfp_hf_in_band_ring_tone_changed_callback)(const bd_addr_t *bd_addr, int status);
```

**使用说明**

在[hfp_hf_callbacks_t](#struct_hfp_hf_callbacks_t)结构体中作为in_band_ring_tone_changed_cb成员的类型，带内铃声状态变化时由BTS回调

### hfp_hf_volume_changed_callback <a id="typedef_hfp_hf_volume_changed_callback"></a>

```c
typedef void (*hfp_hf_volume_changed_callback)(const bd_addr_t *bd_addr, hfp_volume_type_t type, unsigned char volume);
```

**使用说明**

在[hfp_hf_callbacks_t](#struct_hfp_hf_callbacks_t)结构体中作为volume_changed_cb成员的类型，AG端调整HF音量时由BTS回调

### hfp_hf_call_changed_callback <a id="typedef_hfp_hf_call_changed_callback"></a>

```c
typedef void (*hfp_hf_call_changed_callback)(const bd_addr_t *bd_addr, const hfp_hf_calls_info_t *call);
```

**使用说明**

在[hfp_hf_callbacks_t](#struct_hfp_hf_callbacks_t)结构体中作为call_changed_cb成员的类型，通话状态变化时由BTS回调

### hfp_hf_dial_error_code_callback <a id="typedef_hfp_hf_dial_error_code_callback"></a>

```c
typedef void (*hfp_hf_dial_error_code_callback)(const bd_addr_t *bd_addr, int code);
```

**使用说明**

在[hfp_hf_callbacks_t](#struct_hfp_hf_callbacks_t)结构体中作为dial_error_code_cb成员的类型，拨号失败时由BTS回调

### hfp_hf_codec_changed_callback <a id="typedef_hfp_hf_codec_changed_callback"></a>

```c
typedef void (*hfp_hf_codec_changed_callback)(const bd_addr_t *bd_addr, int codec);
```

**使用说明**

在[hfp_hf_callbacks_t](#struct_hfp_hf_callbacks_t)结构体中作为codec_changed_cb成员的类型，codec编码格式变化时由BTS回调

## Enumerations

### hfp_hf_accept_call_action_type_t <a id="enum_hfp_hf_accept_call_action_type_t"></a>

```c
typedef enum {
    HFP_HF_ACCEPT_CALL_ACTION_NONE = 0x00,   /*!< 接受呼叫后无操作 */
    HFP_HF_ACCEPT_CALL_ACTION_HOLD = 0x01,   /*!< 接受呼叫保持当前通话 */
    HFP_HF_ACCEPT_CALL_ACTION_FINISH = 0x02  /*!< 接受呼叫后结束当前通话 */
} hfp_hf_accept_call_action_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| HFP_HF_ACCEPT_CALL_ACTION_NONE | 0x00 | 接受呼叫后无操作 |
| HFP_HF_ACCEPT_CALL_ACTION_HOLD | 0x01 | 接受呼叫保持当前通话 |
| HFP_HF_ACCEPT_CALL_ACTION_FINISH | 0x02 | 接受呼叫后结束当前通话 |

### hfp_hf_call_state_t <a id="enum_hfp_hf_call_state_t"></a>

```c
typedef enum {
    HFP_HF_CALL_STATE_ACTIVE = 0,           /*!< HFP HF正在通话 */
    HFP_HF_CALL_STATE_HELD,                 /*!< HFP HF呼叫保持通话 */
    HFP_HF_CALL_STATE_DIALING,              /*!< HFP HF呼叫正在拨号 */
    HFP_HF_CALL_STATE_ALERTING,             /*!< HFP HF呼叫正在响铃 */
    HFP_HF_CALL_STATE_INCOMING,             /*!< HFP HF呼叫接入 */
    HFP_HF_CALL_STATE_WAITING,              /*!< HFP HF呼叫等待 */
    HFP_HF_CALL_STATE_RESPONSE_HELD,        /*!< HFP HF呼叫响应保持 */
    HFP_HF_CALL_STATE_FINISHED              /*!< HFP HF呼叫结束 */
} hfp_hf_call_state_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| HFP_HF_CALL_STATE_ACTIVE | 0 | 正在通话 |
| HFP_HF_CALL_STATE_HELD | 1 | 呼叫保持通话 |
| HFP_HF_CALL_STATE_DIALING | 2 | 呼叫正在拨号 |
| HFP_HF_CALL_STATE_ALERTING | 3 | 呼叫正在响铃 |
| HFP_HF_CALL_STATE_INCOMING | 4 | 呼叫接入 |
| HFP_HF_CALL_STATE_WAITING | 5 | 呼叫等待 |
| HFP_HF_CALL_STATE_RESPONSE_HELD | 6 | 呼叫响应保持 |
| HFP_HF_CALL_STATE_FINISHED | 7 | 呼叫结束 |

### hfp_hf_codec_id_t <a id="enum_hfp_hf_codec_id_t"></a>

```c
typedef enum {
    HFP_HF_CODEC_ID_CVSD = 0x01, /*!< HFP HF编码格式为CVSD */
    HFP_HF_CODEC_ID_MSBC,        /*!< HFP HF编码格式为MSBC */
} hfp_hf_codec_id_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| HFP_HF_CODEC_ID_CVSD | 0x01 | 编码格式为CVSD (Continuous Variable Slope Delta) |
| HFP_HF_CODEC_ID_MSBC | 0x02 | 编码格式为MSBC (Wide Band Speech Codec) |

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
| ERRCODE_BT_NOT_READY | 0x80006001 | 状态未就绪 |
| ERRCODE_BT_MALLOC_FAIL | 0x80006002 | 内存不足 |
| ERRCODE_BT_MEMCPY_FAIL | 0x80006003 | 内存拷贝错误 |
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
| HFP_SCO_STATE_CONNECTING | 0x01 | SCO正在连接 |
| HFP_SCO_STATE_CONNECTED | 0x02 | SCO已连接 |
| HFP_SCO_STATE_DISCONNECTING | 0x03 | SCO正在断连 |
| HFP_SCO_STATE_DISCONNECTED | 0x04 | SCO已断连 |

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

### bt_addr_type <a id="enum_bt_addr_type"></a>

```c
typedef enum {
    BT_ADDRESS_TYPE_PUBLIC_DEVICE_ADDRESS = 0x00,
    BT_ADDRESS_TYPE_RANDOM_DEVICE_ADDRESS = 0x01,
} bt_addr_type;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| BT_ADDRESS_TYPE_PUBLIC_DEVICE_ADDRESS | 0x00 | 蓝牙公有地址 |
| BT_ADDRESS_TYPE_RANDOM_DEVICE_ADDRESS | 0x01 | 蓝牙随机地址 |

## Structures

### bd_addr_t <a id="struct_bd_addr_t"></a>

```c
typedef struct {
    uint8_t addr[BD_ADDR_LEN];     /*!< 蓝牙地址 */
    uint8_t type;                  /*!< 蓝牙地址类型，参考bt_addr_type */
} bd_addr_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| addr | uint8_t[6] | 蓝牙设备地址，长度为BD_ADDR_LEN(6) |
| type | uint8_t | 蓝牙地址类型，参考[bt_addr_type](#enum_bt_addr_type) |

### hfp_hf_calls_info_t <a id="struct_hfp_hf_calls_info_t"></a>

```c
typedef struct {
    bd_addr_t bd_addr;        /*!< 远端设备地址 */
    int id;                   /*!< 编号 */
    int state;                /*!< 电话呼叫状态 */
    char *number;             /*!< 电话号码 */
    unsigned char number_len; /*!< 电话号码位数 */
    bt_uuid_t uuid;           /*!< 电话服务uuid */
    bool multi_party;         /*!< 多方电话 */
    bool outgoing;            /*!< 呼入/呼出 */
    bool in_band_ring;        /*!< inband-ring模式 */
    long creation_time;       /*!< 呼叫开始时间 */
} hfp_hf_calls_info_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| bd_addr | [bd_addr_t](#struct_bd_addr_t) | 远端设备地址 |
| id | int | 呼叫编号 |
| state | int | 电话呼叫状态，参考[hfp_hf_call_state_t](#enum_hfp_hf_call_state_t) |
| number | char * | 电话号码 |
| number_len | unsigned char | 电话号码位数 |
| uuid | [bt_uuid_t](#typedef_bt_uuid_t) | 电话服务UUID |
| multi_party | bool | 是否为多方电话 |
| outgoing | bool | 呼入/呼出标识 |
| in_band_ring | bool | inband-ring模式标识 |
| creation_time | long | 呼叫开始时间 |

### hfp_hf_callbacks_t <a id="struct_hfp_hf_callbacks_t"></a>

```c
typedef struct {
    hfp_hf_conn_state_changed_callback conn_state_changed_cb;
    hfp_hf_sco_conn_state_changed_callback sco_conn_state_changed_cb;
    hfp_hf_battery_level_changed_callback battery_level_changed_cb;
    hfp_hf_signal_strength_changed_callback signal_strength_changed_cb;
    hfp_hf_registration_status_changed_callback registration_status_changed_cb;
    hfp_hf_roaming_status_changed_callback roaming_status_changed_cb;
    hfp_hf_operator_selection_callback operator_selection_cb;
    hfp_hf_subscriber_number_callback subscriber_number_cb;
    hfp_hf_voice_recognition_status_changed_callback voice_recognition_status_changed_cb;
    hfp_hf_in_band_ring_tone_changed_callback in_band_ring_tone_changed_cb;
    hfp_hf_volume_changed_callback volume_changed_cb;
    hfp_hf_call_changed_callback call_changed_cb;
    hfp_hf_dial_error_code_callback dial_error_code_cb;
    hfp_hf_codec_changed_callback codec_changed_cb;
} hfp_hf_callbacks_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| conn_state_changed_cb | [hfp_hf_conn_state_changed_callback](#typedef_hfp_hf_conn_state_changed_callback) | HFP连接状态变化回调 |
| sco_conn_state_changed_cb | [hfp_hf_sco_conn_state_changed_callback](#typedef_hfp_hf_sco_conn_state_changed_callback) | SCO连接状态变化回调 |
| battery_level_changed_cb | [hfp_hf_battery_level_changed_callback](#typedef_hfp_hf_battery_level_changed_callback) | AG电量变化回调 |
| signal_strength_changed_cb | [hfp_hf_signal_strength_changed_callback](#typedef_hfp_hf_signal_strength_changed_callback) | AG信号强度变化回调 |
| registration_status_changed_cb | [hfp_hf_registration_status_changed_callback](#typedef_hfp_hf_registration_status_changed_callback) | AG注册状态变化回调 |
| roaming_status_changed_cb | [hfp_hf_roaming_status_changed_callback](#typedef_hfp_hf_roaming_status_changed_callback) | AG漫游状态变化回调 |
| operator_selection_cb | [hfp_hf_operator_selection_callback](#typedef_hfp_hf_operator_selection_callback) | AG运营商信息回调 |
| subscriber_number_cb | [hfp_hf_subscriber_number_callback](#typedef_hfp_hf_subscriber_number_callback) | AG用户号码回调 |
| voice_recognition_status_changed_cb | [hfp_hf_voice_recognition_status_changed_callback](#typedef_hfp_hf_voice_recognition_status_changed_callback) | 语音识别状态变化回调 |
| in_band_ring_tone_changed_cb | [hfp_hf_in_band_ring_tone_changed_callback](#typedef_hfp_hf_in_band_ring_tone_changed_callback) | 带内铃声状态变化回调 |
| volume_changed_cb | [hfp_hf_volume_changed_callback](#typedef_hfp_hf_volume_changed_callback) | AG端调整HF音量回调 |
| call_changed_cb | [hfp_hf_call_changed_callback](#typedef_hfp_hf_call_changed_callback) | 通话状态变化回调 |
| dial_error_code_cb | [hfp_hf_dial_error_code_callback](#typedef_hfp_hf_dial_error_code_callback) | 拨号失败回调 |
| codec_changed_cb | [hfp_hf_codec_changed_callback](#typedef_hfp_hf_codec_changed_callback) | codec编码格式变化回调 |



# AVRCP TG

AVRCP (Audio/Video Remote Control Profile) TG (Audio/Video Remote Control Profile Target) 提供经典蓝牙 AVRCP 目标端能力。管理 AVRCP TG 连接/断开，以及向 CT (Controller) 主动通知播放状态、当前曲目、播放位置、Now Playing 内容、可用播放器、Addressed player、UID 和音量等事件变更。

作为 AVRCP 的目标端（TG），响应 CT 的控制命令并异步上报媒体状态变化，用于在被控播放设备上同步媒体播放、曲目切换和音量调节等状态。

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_avrcp_target.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [avrcp_tg_get_device_connect_state](#avrcp_tg_get_device_connect_state) | 通过设备地址查询设备avrcp连接状态 |
| [avrcp_tg_connect](#avrcp_tg_connect) | 与对端设备建立avrcp连接 |
| [avrcp_tg_disconnect](#avrcp_tg_disconnect) | 断开与CT的avrcp连接 |
| [avrcp_tg_notify_playback_status_changed](#avrcp_tg_notify_playback_status_changed) | 通知CT端播放状态发生改变 |
| [avrcp_tg_notify_track_changed](#avrcp_tg_notify_track_changed) | 通知CT端track发生改变 |
| [avrcp_tg_notify_track_reached_end](#avrcp_tg_notify_track_reached_end) | 通知CT端track已播放结束 |
| [avrcp_tg_notify_track_reached_start](#avrcp_tg_notify_track_reached_start) | 通知CT端track已播放开始 |
| [avrcp_tg_notify_playback_pos_changed](#avrcp_tg_notify_playback_pos_changed) | 通知CT端播放位置发生改变 |
| [avrcp_tg_notify_now_playing_content_changed](#avrcp_tg_notify_now_playing_content_changed) | 通知CT端NowPlaying folder发生改变 |
| [avrcp_tg_notify_available_players_changed](#avrcp_tg_notify_available_players_changed) | 通知CT端有新的播放器可用 |
| [avrcp_tg_notify_addressed_players_changed](#avrcp_tg_notify_addressed_players_changed) | 通知CT端Addressed player发生改变 |
| [avrcp_tg_notify_uid_changed](#avrcp_tg_notify_uid_changed) | 通知CT端UID发生改变 |
| [avrcp_tg_notify_volume_changed](#avrcp_tg_notify_volume_changed) | TG端通知CT端音量发生改变 |
| [avrcp_tg_register_callbacks](#avrcp_tg_register_callbacks) | 注册上层应用的回调 |

## Functions

### avrcp_tg_get_device_connect_state <a id="avrcp_tg_get_device_connect_state"></a>

```c
int avrcp_tg_get_device_connect_state(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_avrcp_target.h"
```

**功能说明**

- 通过设备地址查询设备avrcp连接状态
- 返回值为profile_connect_state_t枚举值，表示正在连接/已连接/正在断开/已断开
- 该回调函数运行于bts线程，不能阻塞或长时间等待

**前置条件**

- avrcp tg模块已初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息 | 非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| PROFILE_STATE_CONNECTING(0x01) | 正在连接 | 正在建立连接 |
| PROFILE_STATE_CONNECTED(0x02) | 已连接 | 连接已建立 |
| PROFILE_STATE_DISCONNECTING(0x03) | 正在断连 | 正在断开连接 |
| PROFILE_STATE_DISCONNECTED(0x04) | 已断连 | 连接已断开 |

### avrcp_tg_connect <a id="avrcp_tg_connect"></a>

```c
int avrcp_tg_connect(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_avrcp_target.h"
```

**功能说明**

- 与对端设备建立avrcp连接
- 该回调函数运行于bts线程，不能阻塞或长时间等待
- 连接结果通过 avrcp_tg_conn_state_changed_callback 回调通知上层

**前置条件**

- avrcp tg模块已初始化
- 已通过 avrcp_tg_register_callbacks() 注册回调函数

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息 | 非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 连接请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### avrcp_tg_disconnect <a id="avrcp_tg_disconnect"></a>

```c
int avrcp_tg_disconnect(const bd_addr_t *bd_addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_avrcp_target.h"
```

**功能说明**

- 断开与CT的avrcp连接
- 该回调函数运行于bts线程，不能阻塞或长时间等待
- 断连结果通过 avrcp_tg_conn_state_changed_callback 回调通知上层

**前置条件**

- avrcp tg模块已初始化
- 已通过 avrcp_tg_register_callbacks() 注册回调函数
- 与对端设备的avrcp连接已建立

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息 | 非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 断连请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### avrcp_tg_notify_playback_status_changed <a id="avrcp_tg_notify_playback_status_changed"></a>

```c
void avrcp_tg_notify_playback_status_changed(unsigned char play_status)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_avrcp_target.h"
```

**功能说明**

- 通知CT端播放状态发生改变
- 对应CT端注册的AVRCP_NOTIFY_EVENT_PLAYBACK_STATUS_CHANGED事件
- 该回调函数运行于bts上下文

**前置条件**

- avrcp tg与CT已建立连接
- CT端已注册AVRCP_NOTIFY_EVENT_PLAYBACK_STATUS_CHANGED事件

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| play_status | unsigned char | 播放状态 | AVRCP_PLAY_STATUS_STOPPED(0x00), AVRCP_PLAY_STATUS_PLAYING(0x01), AVRCP_PLAY_STATUS_PAUSED(0x02), AVRCP_PLAY_STATUS_FWD_SEEK(0x03), AVRCP_PLAY_STATUS_REV_SEEK(0x04), AVRCP_PLAY_STATUS_ERROR(0xFF) |

### avrcp_tg_notify_track_changed <a id="avrcp_tg_notify_track_changed"></a>

```c
void avrcp_tg_notify_track_changed(unsigned long long identifier)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_avrcp_target.h"
```

**功能说明**

- 通知CT端track发生改变
- 对应CT端注册的AVRCP_NOTIFY_EVENT_TRACK_CHANGED事件
- identifier为TG端当前element的唯一标识符

**前置条件**

- avrcp tg与CT已建立连接
- CT端已注册AVRCP_NOTIFY_EVENT_TRACK_CHANGED事件

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| identifier | unsigned long long | TG端当前element的唯一标识符 | - |

### avrcp_tg_notify_track_reached_end <a id="avrcp_tg_notify_track_reached_end"></a>

```c
void avrcp_tg_notify_track_reached_end(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_avrcp_target.h"
```

**功能说明**

- 通知CT端track已播放结束
- 对应CT端注册的AVRCP_NOTIFY_EVENT_TRACK_REACHED_END事件
- 该回调函数运行于bts上下文

**前置条件**

- avrcp tg与CT已建立连接
- CT端已注册AVRCP_NOTIFY_EVENT_TRACK_REACHED_END事件

### avrcp_tg_notify_track_reached_start <a id="avrcp_tg_notify_track_reached_start"></a>

```c
void avrcp_tg_notify_track_reached_start(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_avrcp_target.h"
```

**功能说明**

- 通知CT端track已播放开始
- 对应CT端注册的AVRCP_NOTIFY_EVENT_TRACK_REACHED_START事件
- 该回调函数运行于bts上下文

**前置条件**

- avrcp tg与CT已建立连接
- CT端已注册AVRCP_NOTIFY_EVENT_TRACK_REACHED_START事件

### avrcp_tg_notify_playback_pos_changed <a id="avrcp_tg_notify_playback_pos_changed"></a>

```c
void avrcp_tg_notify_playback_pos_changed(unsigned int playback_pos)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_avrcp_target.h"
```

**功能说明**

- 通知CT端播放位置发生改变
- 对应CT端注册的AVRCP_NOTIFY_EVENT_PLAYBACK_POS_CHANGED事件
- 若当前未选择track，则CT端INTERIM响应中返回0xFFFFFFFF

**前置条件**

- avrcp tg与CT已建立连接
- CT端已注册AVRCP_NOTIFY_EVENT_PLAYBACK_POS_CHANGED事件

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| playback_pos | unsigned int | 当前播放位置，单位毫秒 | 0~0xFFFFFFFF |

### avrcp_tg_notify_now_playing_content_changed <a id="avrcp_tg_notify_now_playing_content_changed"></a>

```c
void avrcp_tg_notify_now_playing_content_changed(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_avrcp_target.h"
```

**功能说明**

- 通知CT端NowPlaying folder发生改变
- 对应CT端注册的AVRCP_NOTIFY_EVENT_NOW_PLAYING_CONTENT_CHANGED事件
- 该回调函数运行于bts上下文

**前置条件**

- avrcp tg与CT已建立连接
- CT端已注册AVRCP_NOTIFY_EVENT_NOW_PLAYING_CONTENT_CHANGED事件

### avrcp_tg_notify_available_players_changed <a id="avrcp_tg_notify_available_players_changed"></a>

```c
void avrcp_tg_notify_available_players_changed(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_avrcp_target.h"
```

**功能说明**

- 通知CT端有新的播放器可用
- 对应CT端注册的AVRCP_NOTIFY_EVENT_AVAILABLE_PLAYERS_CHANGED事件
- 该回调函数运行于bts上下文

**前置条件**

- avrcp tg与CT已建立连接
- CT端已注册AVRCP_NOTIFY_EVENT_AVAILABLE_PLAYERS_CHANGED事件

### avrcp_tg_notify_addressed_players_changed <a id="avrcp_tg_notify_addressed_players_changed"></a>

```c
void avrcp_tg_notify_addressed_players_changed(unsigned short player_id, unsigned short uid_counter)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_avrcp_target.h"
```

**功能说明**

- 通知CT端Addressed player发生改变
- 对应CT端注册的AVRCP_NOTIFY_EVENT_ADDRESSED_PLAYER_CHANGED事件
- 该回调函数运行于bts上下文

**前置条件**

- avrcp tg与CT已建立连接
- CT端已注册AVRCP_NOTIFY_EVENT_ADDRESSED_PLAYER_CHANGED事件

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| player_id | unsigned short | 当前播放器的id | - |
| uid_counter | unsigned short | 当前浏览的播放器的UID计数器 | - |

### avrcp_tg_notify_uid_changed <a id="avrcp_tg_notify_uid_changed"></a>

```c
void avrcp_tg_notify_uid_changed(unsigned short uid_counter)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_avrcp_target.h"
```

**功能说明**

- 通知CT端UID发生改变
- 对应CT端注册的AVRCP_NOTIFY_EVENT_UIDS_CHANGED事件
- 该回调函数运行于bts上下文

**前置条件**

- avrcp tg与CT已建立连接
- CT端已注册AVRCP_NOTIFY_EVENT_UIDS_CHANGED事件

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| uid_counter | unsigned short | 当前浏览的播放器的UID计数器 | - |

### avrcp_tg_notify_volume_changed <a id="avrcp_tg_notify_volume_changed"></a>

```c
void avrcp_tg_notify_volume_changed(unsigned char volume)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_avrcp_target.h"
```

**功能说明**

- TG端通知CT端音量发生改变
- 对应CT端注册的AVRCP_NOTIFY_EVENT_VOLUME_CHANGED事件
- 该回调函数运行于bts上下文

**前置条件**

- avrcp tg与CT已建立连接
- CT端已注册AVRCP_NOTIFY_EVENT_VOLUME_CHANGED事件

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| volume | unsigned char | TG端的当前绝对音量 | 0~0x7F |

### avrcp_tg_register_callbacks <a id="avrcp_tg_register_callbacks"></a>

```c
int avrcp_tg_register_callbacks(avrcp_tg_callbacks_t *func)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_avrcp_target.h"
```

**功能说明**

- 注册上层应用的回调
- 注册后BTS (Bluetooth Subsystem) 在avrcp tg状态更新时通过回调反馈结果给上层
- 回调函数运行于bts线程，不能阻塞或长时间等待

**前置条件**

- avrcp tg模块已初始化

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func | [avrcp_tg_callbacks_t](#struct_avrcp_tg_callbacks_t) * | 指向回调函数接口定义的指针 | 非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 注册回调成功 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

## Type definitions

### avrcp_tg_conn_state_changed_callback <a id="typedef_avrcp_tg_conn_state_changed_callback"></a>

```c
typedef void (*avrcp_tg_conn_state_changed_callback)(const bd_addr_t *bd_addr, profile_connect_state_t state);
```

**使用说明**

在 [avrcp_tg_callbacks_t](#struct_avrcp_tg_callbacks_t) 结构体中作为成员 conn_state_changed_cb 使用，由上层实现并通过 [avrcp_tg_register_callbacks](#avrcp_tg_register_callbacks) 注册，用于avrcp tg连接状态改变通知。

**回调说明**

- BTS  每次完成avrcp tg状态更新后调用该回调反馈结果给上层
- 该回调运行于bts线程，不能阻塞或长时间等待
- bd_addr指向的内存由bts申请并释放，回调中不应释放

**回调参数**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息 | 非NULL |
| state | [profile_connect_state_t](#enum_profile_connect_state_t) | avrcp tg连接状态 | PROFILE_STATE_CONNECTING(0x01), PROFILE_STATE_CONNECTED(0x02), PROFILE_STATE_DISCONNECTING(0x03), PROFILE_STATE_DISCONNECTED(0x04) |

## Enumerations

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

### avrcp_play_status_t <a id="enum_avrcp_play_status_t"></a>

```c
typedef enum {
    AVRCP_PLAY_STATUS_STOPPED = 0x00,
    AVRCP_PLAY_STATUS_PLAYING = 0x01,
    AVRCP_PLAY_STATUS_PAUSED = 0x02,
    AVRCP_PLAY_STATUS_FWD_SEEK = 0x03,
    AVRCP_PLAY_STATUS_REV_SEEK = 0x04,
    AVRCP_PLAY_STATUS_ERROR = 0xFF
} avrcp_play_status_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| AVRCP_PLAY_STATUS_STOPPED | 0x00 | 停止状态 |
| AVRCP_PLAY_STATUS_PLAYING | 0x01 | 播放状态 |
| AVRCP_PLAY_STATUS_PAUSED | 0x02 | 暂停状态 |
| AVRCP_PLAY_STATUS_FWD_SEEK | 0x03 | 快进/下一首 |
| AVRCP_PLAY_STATUS_REV_SEEK | 0x04 | 快退/上一首 |
| AVRCP_PLAY_STATUS_ERROR | 0xFF | 未知状态 |

## Structures

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
| addr | uint8_t[6] | 蓝牙设备地址 |
| type | uint8_t | 蓝牙地址类型，参考 bt_addr_type |

### avrcp_tg_callbacks_t <a id="struct_avrcp_tg_callbacks_t"></a>

```c
typedef struct {
    avrcp_tg_conn_state_changed_callback conn_state_changed_cb;
} avrcp_tg_callbacks_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| conn_state_changed_cb | [avrcp_tg_conn_state_changed_callback](#typedef_avrcp_tg_conn_state_changed_callback) | avrcp tg连接状态改变回调函数 |



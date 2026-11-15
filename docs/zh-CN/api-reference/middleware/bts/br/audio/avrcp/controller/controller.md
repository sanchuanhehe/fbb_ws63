# AVRCP CT

AVRCP (Audio/Video Remote Control Profile) CT (Audio/Video Remote Control Profile Controller) 提供经典蓝牙 AVRCP 控制端能力。管理 AVRCP 连接/断开、按键 pass through（press/release button）、获取 TG (Audio/Video Remote Control Profile Target) 端支持的 company/event/element 属性、播放状态查询、事件 notification 注册与绝对音量设置。

作为 AVRCP 的控制端（CT），向对端 TG发送媒体控制命令并接收其异步通知，用于实现播放/暂停、上下首、音量调节及媒体信息（标题、专辑、艺人等）展示等功能。

**头文件清单**

- include/middleware/services/bts/br/bts_avrcp_controller.h

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [avrcp_ct_get_device_connect_state](#avrcp_ct_get_device_connect_state) | 通过设备地址查询AVRCP连接状态 |
| [avrcp_ct_connect](#avrcp_ct_connect) | 与对端设备建立AVRCP连接 |
| [avrcp_ct_disconnect](#avrcp_ct_disconnect) | 断开与TG的AVRCP连接 |
| [avrcp_ct_press_button](#avrcp_ct_press_button) | 通知TG端CT已按下button |
| [avrcp_ct_release_button](#avrcp_ct_release_button) | 通知TG端CT已释放button |
| [avrcp_ct_get_supported_companies](#avrcp_ct_get_supported_companies) | 获取TG端支持的公司列表 |
| [avrcp_ct_get_supported_events](#avrcp_ct_get_supported_events) | 获取TG端支持的事件列表 |
| [avrcp_ct_get_element_attributes](#avrcp_ct_get_element_attributes) | 获取TG端支持的属性列表 |
| [avrcp_ct_get_play_status](#avrcp_ct_get_play_status) | 获取TG端媒体的播放状态信息 |
| [avrcp_ct_register_notification](#avrcp_ct_register_notification) | 注册TG端事件通知 |
| [avrcp_ct_set_absolute_volume](#avrcp_ct_set_absolute_volume) | 设置TG端的绝对音量 |
| [avrcp_ct_register_callbacks](#avrcp_ct_register_callbacks) | 注册AVRCP CT回调函数 |

## Functions

### avrcp_ct_get_device_connect_state <a id="avrcp_ct_get_device_connect_state"></a>

```c
int avrcp_ct_get_device_connect_state(const bd_addr_t *bd_addr)
```

**头文件清单**

- include/middleware/services/bts/br/bts_avrcp_controller.h

**功能说明**

- 通过设备地址查询设备AVRCP连接状态
- 返回值为profile_connect_state_t枚举值，表示正在连接/已连接/正在断连/已断连
- 该接口运行于BTS (Bluetooth Subsystem) 线程，不可阻塞或长时间等待

**前置条件**

- 已通过avrcp_ct_register_callbacks注册回调函数
- 入参bd_addr不为NULL，指向有效的蓝牙设备地址

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

### avrcp_ct_connect <a id="avrcp_ct_connect"></a>

```c
int avrcp_ct_connect(const bd_addr_t *bd_addr)
```

**头文件清单**

- include/middleware/services/bts/br/bts_avrcp_controller.h

**功能说明**

- 与对端设备建立AVRCP连接
- 连接结果通过avrcp_ct_conn_state_changed_callback回调通知
- 该接口为异步操作，调用后立即返回，连接状态变更通过回调获取

**前置条件**

- 已通过avrcp_ct_register_callbacks注册回调函数
- 入参bd_addr不为NULL，指向有效的蓝牙设备地址
- BR/EDR链路已建立

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

### avrcp_ct_disconnect <a id="avrcp_ct_disconnect"></a>

```c
int avrcp_ct_disconnect(const bd_addr_t *bd_addr)
```

**头文件清单**

- include/middleware/services/bts/br/bts_avrcp_controller.h

**功能说明**

- 断开与TG的AVRCP连接
- 断开结果通过avrcp_ct_conn_state_changed_callback回调通知
- 该接口为异步操作，调用后立即返回，断开状态变更通过回调获取

**前置条件**

- 已通过avrcp_ct_register_callbacks注册回调函数
- 入参bd_addr不为NULL，指向有效的蓝牙设备地址
- 与对端设备的AVRCP连接已建立

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息 | 非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 断开请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### avrcp_ct_press_button <a id="avrcp_ct_press_button"></a>

```c
int avrcp_ct_press_button(const bd_addr_t *bd_addr, int key_operation)
```

**头文件清单**

- include/middleware/services/bts/br/bts_avrcp_controller.h

**功能说明**

- 通知TG端CT已按下button，发送AVRCP按键按下操作
- 按键响应通过avrcp_ct_press_button_response_callback回调通知
- 支持播放、暂停、快进、快退、音量等标准AVRCP按键操作

**前置条件**

- 已通过avrcp_ct_register_callbacks注册回调函数
- 入参bd_addr不为NULL，指向有效的蓝牙设备地址
- 与对端设备的AVRCP连接已建立

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息 | 非NULL |
| key_operation | int | 按键操作 | 取值参考[avrcp_key_operation_t](#enum_avrcp_key_operation_t) |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 按键操作成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### avrcp_ct_release_button <a id="avrcp_ct_release_button"></a>

```c
int avrcp_ct_release_button(const bd_addr_t *bd_addr, int key_operation)
```

**头文件清单**

- include/middleware/services/bts/br/bts_avrcp_controller.h

**功能说明**

- 通知TG端CT已释放button，发送AVRCP按键释放操作
- 按键释放响应通过avrcp_ct_release_button_response_callback回调通知
- 通常与avrcp_ct_press_button配对使用

**前置条件**

- 已通过avrcp_ct_register_callbacks注册回调函数
- 入参bd_addr不为NULL，指向有效的蓝牙设备地址
- 与对端设备的AVRCP连接已建立
- 已通过avrcp_ct_press_button发送对应的按键按下操作

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息 | 非NULL |
| key_operation | int | 按键操作 | 取值参考[avrcp_key_operation_t](#enum_avrcp_key_operation_t) |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 释放操作成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### avrcp_ct_get_supported_companies <a id="avrcp_ct_get_supported_companies"></a>

```c
int avrcp_ct_get_supported_companies(const bd_addr_t *bd_addr)
```

**头文件清单**

- include/middleware/services/bts/br/bts_avrcp_controller.h

**功能说明**

- 获取TG端支持的公司列表
- 响应通过avrcp_ct_supported_companies_callback回调通知
- 返回TG支持的company ID数量及ID列表

**前置条件**

- 已通过avrcp_ct_register_callbacks注册回调函数
- 入参bd_addr不为NULL，指向有效的蓝牙设备地址
- 与对端设备的AVRCP连接已建立

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息 | 非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### avrcp_ct_get_supported_events <a id="avrcp_ct_get_supported_events"></a>

```c
int avrcp_ct_get_supported_events(const bd_addr_t *bd_addr)
```

**头文件清单**

- include/middleware/services/bts/br/bts_avrcp_controller.h

**功能说明**

- 获取TG端支持的事件列表
- 响应通过avrcp_ct_supported_event_callback回调通知
- 协议规定0x01和0x02事件为强制支持

**前置条件**

- 已通过avrcp_ct_register_callbacks注册回调函数
- 入参bd_addr不为NULL，指向有效的蓝牙设备地址
- 与对端设备的AVRCP连接已建立

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息 | 非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### avrcp_ct_get_element_attributes <a id="avrcp_ct_get_element_attributes"></a>

```c
int avrcp_ct_get_element_attributes(const bd_addr_t *bd_addr, const avrcp_ct_get_element_attr_para_t *param)
```

**头文件清单**

- include/middleware/services/bts/br/bts_avrcp_controller.h

**功能说明**

- 获取TG端支持的属性列表，比如歌曲标题、专辑、艺术风格、年份和作曲家等
- 响应通过avrcp_ct_get_element_attribute_callback回调通知
- 通过attr_id_list指定待查询的属性ID列表

**前置条件**

- 已通过avrcp_ct_register_callbacks注册回调函数
- 入参bd_addr不为NULL，指向有效的蓝牙设备地址
- 入参param不为NULL，param->attr_id_list不为NULL
- 与对端设备的AVRCP连接已建立

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息 | 非NULL |
| param | const [avrcp_ct_get_element_attr_para_t](#struct_avrcp_ct_get_element_attr_para_t) * | 属性ID数量及列表 | 非NULL，attr_id_list非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### avrcp_ct_get_play_status <a id="avrcp_ct_get_play_status"></a>

```c
int avrcp_ct_get_play_status(const bd_addr_t *bd_addr)
```

**头文件清单**

- include/middleware/services/bts/br/bts_avrcp_controller.h

**功能说明**

- 获取TG端媒体的播放状态信息
- 响应通过avrcp_ct_get_play_status_callback回调通知
- 返回歌曲播放时长、已播放位置及播放状态

**前置条件**

- 已通过avrcp_ct_register_callbacks注册回调函数
- 入参bd_addr不为NULL，指向有效的蓝牙设备地址
- 与对端设备的AVRCP连接已建立

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息 | 非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### avrcp_ct_register_notification <a id="avrcp_ct_register_notification"></a>

```c
int avrcp_ct_register_notification(const bd_addr_t *bd_addr, unsigned char event_id, unsigned int interval)
```

**头文件清单**

- include/middleware/services/bts/br/bts_avrcp_controller.h

**功能说明**

- 将TG端支持的事件注册成功后，如果该事件改变会自动通知到CT端
- 响应通过avrcp_ct_notification_callback回调通知
- 需先通过avrcp_ct_get_supported_events获取TG支持的事件列表

**前置条件**

- 已通过avrcp_ct_register_callbacks注册回调函数
- 入参bd_addr不为NULL，指向有效的蓝牙设备地址
- 与对端设备的AVRCP连接已建立
- event_id为TG端支持的事件ID

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息 | 非NULL |
| event_id | unsigned char | 事件ID | 取值参考[avrcp_notify_event_t](#enum_avrcp_notify_event_t) |
| interval | unsigned int | TG端状态变更时向CT端回响应的时间间隔，单位秒 | - |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 注册请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### avrcp_ct_set_absolute_volume <a id="avrcp_ct_set_absolute_volume"></a>

```c
int avrcp_ct_set_absolute_volume(const bd_addr_t *bd_addr, unsigned char volume)
```

**头文件清单**

- include/middleware/services/bts/br/bts_avrcp_controller.h

**功能说明**

- 设置TG端的绝对音量
- 有效取值范围[0x00, 0x7F]，表示0%~100%
- 参考Audio/Video Remote Control 1.6.2 Section 6.13.1 Absolute Volume

**前置条件**

- 已通过avrcp_ct_register_callbacks注册回调函数
- 入参bd_addr不为NULL，指向有效的蓝牙设备地址
- 与对端设备的AVRCP连接已建立

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| bd_addr | const [bd_addr_t](#struct_bd_addr_t) * | 对端蓝牙设备的地址信息 | 非NULL |
| volume | unsigned char | 绝对音量百分比 | [0x00, 0x7F]，参考[avrcp_absolute_volume_t](#enum_avrcp_absolute_volume_t) |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 设置请求成功发起 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

### avrcp_ct_register_callbacks <a id="avrcp_ct_register_callbacks"></a>

```c
int avrcp_ct_register_callbacks(avrcp_ct_callbacks_t *func)
```

**头文件清单**

- include/middleware/services/bts/br/bts_avrcp_controller.h

**功能说明**

- 注册AVRCP CT回调函数
- 回调函数定义包含连接状态变更、按键响应、属性获取、播放状态、通知等全部回调
- 注册回调后方可接收其他接口的异步响应

**前置条件**

- 入参func不为NULL，指向有效的回调函数结构体
- func各回调成员函数指针已正确赋值

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func | [avrcp_ct_callbacks_t](#struct_avrcp_ct_callbacks_t) * | 指向回调函数接口定义的指针 | 非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 注册成功 |
| Other | 其他错误码，参考[errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

## Associations

### avrcp_ct_notification_value_cb_t <a id="union_avrcp_ct_notification_value_cb_t"></a>

```c
typedef union {
    avrcp_play_status_t play_status;
    unsigned long long identifier;
    unsigned int playback_position;
    avrcp_battery_status_t battery_status;
    avrcp_ct_system_status_t system_status;
    avrcp_ct_addressed_player_changed_cb_t addressed_player;
    unsigned short uid_counter;
    unsigned char volume;
} avrcp_ct_notification_value_cb_t;
```

| 成员名称 | 类型 | 描述 | 接口使用逻辑 |
| ------- | ---- | ---- | ----------- |
| play_status | [avrcp_play_status_t](#enum_avrcp_play_status_t) | 播放状态 | avrcp_ct_notification_callback回调出参载体 |
| identifier | unsigned long long | 标识符 | avrcp_ct_notification_callback回调出参载体 |
| playback_position | unsigned int | 当前歌曲已播放时长，单位ms，无效值0xFFFFFFFF | avrcp_ct_notification_callback回调出参载体 |
| battery_status | [avrcp_battery_status_t](#enum_avrcp_battery_status_t) | 电池状态 | avrcp_ct_notification_callback回调出参载体 |
| system_status | [avrcp_ct_system_status_t](#enum_avrcp_ct_system_status_t) | 系统状态 | avrcp_ct_notification_callback回调出参载体 |
| addressed_player | [avrcp_ct_addressed_player_changed_cb_t](#struct_avrcp_ct_addressed_player_changed_cb_t) | TG返回的播放器地址变更的状态信息 | avrcp_ct_notification_callback回调出参载体 |
| uid_counter | unsigned short | uid计数 | avrcp_ct_notification_callback回调出参载体 |
| volume | unsigned char | 当前播放器的音量 | avrcp_ct_notification_callback回调出参载体 |

## Enumerations

### avrcp_key_operation_t <a id="enum_avrcp_key_operation_t"></a>

```c
typedef enum {
    AVRCP_KEY_VOLUME_UP = 0x41,
    AVRCP_KEY_VOLUME_DOWN = 0x42,
    AVRCP_KEY_MUTE = 0x43,
    AVRCP_KEY_PLAY = 0x44,
    AVRCP_KEY_STOP = 0x45,
    AVRCP_KEY_PAUSE = 0x46,
    AVRCP_KEY_RECORD = 0x47,
    AVRCP_KEY_REWIND = 0x48,
    AVRCP_KEY_FAST_FORWARD = 0x49,
    AVRCP_KEY_FORWARD = 0x4B,
    AVRCP_KEY_BACKWARD = 0x4C,
    AVRCP_KEY_INVALID = 0x7F,
} avrcp_key_operation_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| AVRCP_KEY_VOLUME_UP | 0x41 | 音量调大操作 |
| AVRCP_KEY_VOLUME_DOWN | 0x42 | 音量调小操作 |
| AVRCP_KEY_MUTE | 0x43 | 静音操作 |
| AVRCP_KEY_PLAY | 0x44 | play操作 |
| AVRCP_KEY_STOP | 0x45 | stop操作 |
| AVRCP_KEY_PAUSE | 0x46 | pause操作 |
| AVRCP_KEY_RECORD | 0x47 | record操作 |
| AVRCP_KEY_REWIND | 0x48 | 将媒体设置为从起始位置开始 |
| AVRCP_KEY_FAST_FORWARD | 0x49 | 设置媒体快进 |
| AVRCP_KEY_FORWARD | 0x4B | 下一首 |
| AVRCP_KEY_BACKWARD | 0x4C | 上一首 |
| AVRCP_KEY_INVALID | 0x7F | 未知操作 |

### avrcp_ct_element_attr_id_t <a id="enum_avrcp_ct_element_attr_id_t"></a>

```c
typedef enum {
    AVRCP_CT_ELEMENT_ATTR_TITLE = 0x01,
    AVRCP_CT_ELEMENT_ATTR_ARTIST_NAME = 0x02,
    AVRCP_CT_ELEMENT_ATTR_ALBUM_NAME = 0x03,
    AVRCP_CT_ELEMENT_ATTR_TRACK_NUMBER = 0x04,
    AVRCP_CT_ELEMENT_ATTR_TOTAL_NUMBER_OF_TRACKS = 0x05,
    AVRCP_CT_ELEMENT_ATTR_TRACK_GENRE = 0x06,
    AVRCP_CT_ELEMENT_ATTR_TRACK_PLAYING_TIME = 0x07,
} avrcp_ct_element_attr_id_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| AVRCP_CT_ELEMENT_ATTR_TITLE | 0x01 | 标题，歌曲名或者内容描述 |
| AVRCP_CT_ELEMENT_ATTR_ARTIST_NAME | 0x02 | 艺人、表演者或团体 |
| AVRCP_CT_ELEMENT_ATTR_ALBUM_NAME | 0x03 | 唱片，专辑 |
| AVRCP_CT_ELEMENT_ATTR_TRACK_NUMBER | 0x04 | 音频文件的原始编号 |
| AVRCP_CT_ELEMENT_ATTR_TOTAL_NUMBER_OF_TRACKS | 0x05 | tracks/elements的总个数 |
| AVRCP_CT_ELEMENT_ATTR_TRACK_GENRE | 0x06 | 风格，流派 |
| AVRCP_CT_ELEMENT_ATTR_TRACK_PLAYING_TIME | 0x07 | 音频文件的播放时间 |

### avrcp_notify_event_t <a id="enum_avrcp_notify_event_t"></a>

```c
typedef enum {
    AVRCP_NOTIFY_EVENT_PLAYBACK_STATUS_CHANGED = 0x01,
    AVRCP_NOTIFY_EVENT_TRACK_CHANGED = 0x02,
    AVRCP_NOTIFY_EVENT_TRACK_REACHED_END = 0x03,
    AVRCP_NOTIFY_EVENT_TRACK_REACHED_START = 0x04,
    AVRCP_NOTIFY_EVENT_PLAYBACK_POS_CHANGED = 0x05,
    AVRCP_NOTIFY_EVENT_BATT_STATUS_CHANGED = 0x06,
    AVRCP_NOTIFY_EVENT_SYSTEM_STATUS_CHANGED = 0x07,
    AVRCP_NOTIFY_EVENT_PLAYER_APPLICATION_SETTING_CHANGED = 0x08,
    AVRCP_NOTIFY_EVENT_NOW_PLAYING_CONTENT_CHANGED = 0x09,
    AVRCP_NOTIFY_EVENT_AVAILABLE_PLAYERS_CHANGED = 0x0A,
    AVRCP_NOTIFY_EVENT_ADDRESSED_PLAYER_CHANGED = 0x0B,
    AVRCP_NOTIFY_EVENT_UIDS_CHANGED = 0x0C,
    AVRCP_NOTIFY_EVENT_VOLUME_CHANGED = 0x0D,
    AVRCP_NOTIFY_EVENT_RESERVED = 0x0E,
} avrcp_notify_event_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| AVRCP_NOTIFY_EVENT_PLAYBACK_STATUS_CHANGED | 0x01 | 播放状态变更事件 |
| AVRCP_NOTIFY_EVENT_TRACK_CHANGED | 0x02 | track状态变更事件 |
| AVRCP_NOTIFY_EVENT_TRACK_REACHED_END | 0x03 | track结束事件 |
| AVRCP_NOTIFY_EVENT_TRACK_REACHED_START | 0x04 | track开始事件 |
| AVRCP_NOTIFY_EVENT_PLAYBACK_POS_CHANGED | 0x05 | 播放位置改变事件 |
| AVRCP_NOTIFY_EVENT_BATT_STATUS_CHANGED | 0x06 | 电池状态变更事件 |
| AVRCP_NOTIFY_EVENT_SYSTEM_STATUS_CHANGED | 0x07 | 系统状态变更事件 |
| AVRCP_NOTIFY_EVENT_PLAYER_APPLICATION_SETTING_CHANGED | 0x08 | 播放器配置变更事件 |
| AVRCP_NOTIFY_EVENT_NOW_PLAYING_CONTENT_CHANGED | 0x09 | 播放内容变更事件 |
| AVRCP_NOTIFY_EVENT_AVAILABLE_PLAYERS_CHANGED | 0x0A | 可使用的播放器变更事件 |
| AVRCP_NOTIFY_EVENT_ADDRESSED_PLAYER_CHANGED | 0x0B | 播放地址变更事件 |
| AVRCP_NOTIFY_EVENT_UIDS_CHANGED | 0x0C | UID变更事件 |
| AVRCP_NOTIFY_EVENT_VOLUME_CHANGED | 0x0D | TG端音量变更事件 |
| AVRCP_NOTIFY_EVENT_RESERVED | 0x0E | 保留 |

### avrcp_absolute_volume_t <a id="enum_avrcp_absolute_volume_t"></a>

```c
typedef enum {
    AVRCP_ABSOLUTE_VOLUME_PERCENTAGE_0 = 0x00,
    AVRCP_ABSOLUTE_VOLUME_PERCENTAGE_100 = 0x7F,
    AVRCP_ABSOLUTE_VOLUME_INVALID = 0x80,
} avrcp_absolute_volume_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| AVRCP_ABSOLUTE_VOLUME_PERCENTAGE_0 | 0x00 | 绝对音量0% |
| AVRCP_ABSOLUTE_VOLUME_PERCENTAGE_100 | 0x7F | 绝对音量100% |
| AVRCP_ABSOLUTE_VOLUME_INVALID | 0x80 | 无效值 |

### avrcp_play_status_t <a id="enum_avrcp_play_status_t"></a>

```c
typedef enum {
    AVRCP_PLAY_STATUS_STOPPED = 0x00,
    AVRCP_PLAY_STATUS_PLAYING = 0x01,
    AVRCP_PLAY_STATUS_PAUSED = 0x02,
    AVRCP_PLAY_STATUS_FWD_SEEK = 0x03,
    AVRCP_PLAY_STATUS_REV_SEEK = 0x04,
    AVRCP_PLAY_STATUS_ERROR = 0xFF,
} avrcp_play_status_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| AVRCP_PLAY_STATUS_STOPPED | 0x00 | AVRCP媒体stopped状态 |
| AVRCP_PLAY_STATUS_PLAYING | 0x01 | AVRCP媒体playing状态 |
| AVRCP_PLAY_STATUS_PAUSED | 0x02 | AVRCP媒体Paused状态 |
| AVRCP_PLAY_STATUS_FWD_SEEK | 0x03 | AVRCP媒体切换下一首 |
| AVRCP_PLAY_STATUS_REV_SEEK | 0x04 | AVRCP媒体切换上一首 |
| AVRCP_PLAY_STATUS_ERROR | 0xFF | AVRCP媒体未知状态 |

### avrcp_battery_status_t <a id="enum_avrcp_battery_status_t"></a>

```c
typedef enum {
    AVRCP_BATTERY_STATUS_NORMAL = 0x00,
    AVRCP_BATTERY_STATUS_WARNING = 0x01,
    AVRCP_BATTERY_STATUS_CRITICAL = 0x02,
    AVRCP_BATTERY_STATUS_EXTERNAL = 0x03,
    AVRCP_BATTERY_STATUS_FULL_CHARGE = 0x04,
} avrcp_battery_status_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| AVRCP_BATTERY_STATUS_NORMAL | 0x00 | 电池的电量处于正常状态 |
| AVRCP_BATTERY_STATUS_WARNING | 0x01 | 电池的电量处于告警状态 |
| AVRCP_BATTERY_STATUS_CRITICAL | 0x02 | 电池的电量处于红色告警状态，不能在使用 |
| AVRCP_BATTERY_STATUS_EXTERNAL | 0x03 | 插接外部电源 |
| AVRCP_BATTERY_STATUS_FULL_CHARGE | 0x04 | 电池已充满电 |

### avrcp_ct_system_status_t <a id="enum_avrcp_ct_system_status_t"></a>

```c
typedef enum {
    AVRCP_CT_SYSTEM_STATUS_POWER_ON = 0x00,
    AVRCP_CT_SYSTEM_STATUS_POWER_OFF = 0x01,
    AVRCP_CT_SYSTEM_STATUS_UNPLUGGED = 0x02,
} avrcp_ct_system_status_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| AVRCP_CT_SYSTEM_STATUS_POWER_ON | 0x00 | AVRCP开机事件 |
| AVRCP_CT_SYSTEM_STATUS_POWER_OFF | 0x01 | AVRCP关机事件 |
| AVRCP_CT_SYSTEM_STATUS_UNPLUGGED | 0x02 | AVRCP未充电状态 |

### profile_connect_state_t <a id="enum_profile_connect_state_t"></a>

```c
typedef enum {
    PROFILE_STATE_CONNECTING = 0x01,
    PROFILE_STATE_CONNECTED = 0x02,
    PROFILE_STATE_DISCONNECTING = 0x03,
    PROFILE_STATE_DISCONNECTED = 0x04,
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
    ERRCODE_BT_FAIL = 0x80006000,
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
    ERRCODE_BT_MAX = 0x800067FF,
} errcode_bt_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| ERRCODE_BT_SUCCESS | 0x00 | 执行成功错误码 |
| ERRCODE_BT_FAIL | 0x80006000 | 执行失败错误码 |
| ERRCODE_BT_NOT_READY | 0x80006001 | 执行状态未就绪错误码 |
| ERRCODE_BT_MALLOC_FAIL | 0x80006002 | 内存不足错误码 |
| ERRCODE_BT_MEMCPY_FAIL | 0x80006003 | 内存拷贝错误错误码 |
| ERRCODE_BT_BUSY | 0x80006004 | 繁忙无法响应错误码 |
| ERRCODE_BT_DONE | 0x80006005 | 执行完成错误码 |
| ERRCODE_BT_UNSUPPORTED | 0x80006006 | 不支持错误码 |
| ERRCODE_BT_PARAM_ERR | 0x80006007 | 无效参数错误码 |
| ERRCODE_BT_STATE_ERR | 0x80006008 | 状态错误 |
| ERRCODE_BT_UNHANDLED | 0x80006009 | 未处理错误码 |
| ERRCODE_BT_AUTH_FAIL | 0x8000600A | 鉴权失败错误码 |
| ERRCODE_BT_RMT_DEV_DOWN | 0x8000600B | 远端设备关闭错误码 |
| ERRCODE_BT_AUTH_REJECTED | 0x8000600C | 鉴权被拒错误码 |
| ERRCODE_BT_MAX | 0x800067FF | 蓝牙错误码最大值 |

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
| addr | uint8_t[6] | 蓝牙地址 |
| type | uint8_t | 蓝牙地址类型，参考bt_addr_type |

### avrcp_ct_get_element_attr_para_t <a id="struct_avrcp_ct_get_element_attr_para_t"></a>

```c
typedef struct {
    unsigned char attr_num;
    unsigned int *attr_id_list;
} avrcp_ct_get_element_attr_para_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| attr_num | unsigned char | attribute数量 |
| attr_id_list | unsigned int * | attribute ID列表，协议规定每个ID占4字节，取值参考[avrcp_ct_element_attr_id_t](#enum_avrcp_ct_element_attr_id_t) |

### avrcp_ct_support_company_para_t <a id="struct_avrcp_ct_support_company_para_t"></a>

```c
typedef struct {
    unsigned char company_num;
    unsigned int *company_id;
} avrcp_ct_support_company_para_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| company_num | unsigned char | TG支持的company ID的数量 |
| company_id | unsigned int * | TG支持的company ID列表，协议规定每个ID占3字节，接口这里每个ID分配4字节 |

### avrcp_ct_support_event_para_t <a id="struct_avrcp_ct_support_event_para_t"></a>

```c
typedef struct {
    unsigned char event_num;
    unsigned char *event_id;
} avrcp_ct_support_event_para_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| event_num | unsigned char | TG支持的event的数量 |
| event_id | unsigned char * | TG支持的event ID列表，协议规定每个eventId占1个字节，取值参考[avrcp_notify_event_t](#enum_avrcp_notify_event_t) |

### avrcp_ct_element_attr_value_cb_t <a id="struct_avrcp_ct_element_attr_value_cb_t"></a>

```c
typedef struct {
    unsigned int attr_id;
    unsigned short character_set_id;
    unsigned short value_len;
    unsigned char *value;
} avrcp_ct_element_attr_value_cb_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| attr_id | unsigned int | 属性ID |
| character_set_id | unsigned short | 字符集ID |
| value_len | unsigned short | 属性名称的长度，0-65535，缺省默认0值 |
| value | unsigned char * | 属性名称 |

### avrcp_ct_element_attr_para_cb_t <a id="struct_avrcp_ct_element_attr_para_cb_t"></a>

```c
typedef struct {
    unsigned char attr_num;
    avrcp_ct_element_attr_value_cb_t *attr_value;
} avrcp_ct_element_attr_para_cb_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| attr_num | unsigned char | TG返回的attribute数量，1-255 |
| attr_value | [avrcp_ct_element_attr_value_cb_t](#struct_avrcp_ct_element_attr_value_cb_t) * | 属性信息，根据attr_num大小动态分配空间 |

### avrcp_ct_addressed_player_changed_cb_t <a id="struct_avrcp_ct_addressed_player_changed_cb_t"></a>

```c
typedef struct {
    unsigned short player_id;
    unsigned short uid_counter;
} avrcp_ct_addressed_player_changed_cb_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| player_id | unsigned short | 播放器的id |
| uid_counter | unsigned short | UID计数 |

### avrcp_ct_play_status_cb_t <a id="struct_avrcp_ct_play_status_cb_t"></a>

```c
typedef struct {
    unsigned int song_length;
    unsigned int song_position;
    avrcp_play_status_t play_status;
} avrcp_ct_play_status_cb_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| song_length | unsigned int | 歌曲的播放时间，单位ms |
| song_position | unsigned int | 歌曲已经播放的时间，单位ms |
| play_status | [avrcp_play_status_t](#enum_avrcp_play_status_t) | 歌曲的播放状态 |

### avrcp_ct_callbacks_t <a id="struct_avrcp_ct_callbacks_t"></a>

```c
typedef struct {
    avrcp_ct_conn_state_changed_callback conn_state_changed_cb;
    avrcp_ct_press_button_response_callback press_button_cb;
    avrcp_ct_release_button_response_callback release_button_cb;
    avrcp_ct_supported_companies_callback supported_companies_cb;
    avrcp_ct_supported_event_callback supported_event_cb;
    avrcp_ct_get_element_attribute_callback element_attr_cb;
    avrcp_ct_get_play_status_callback play_status_cb;
    avrcp_ct_notification_callback notification_cb;
} avrcp_ct_callbacks_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| conn_state_changed_cb | avrcp_ct_conn_state_changed_callback | AVRCP连接状态更新回调 |
| press_button_cb | avrcp_ct_press_button_response_callback | 收到TG的press button响应回调 |
| release_button_cb | avrcp_ct_release_button_response_callback | 收到TG的release button响应回调 |
| supported_companies_cb | avrcp_ct_supported_companies_callback | 收到TG支持的company列表回调 |
| supported_event_cb | avrcp_ct_supported_event_callback | 收到TG支持的event列表回调 |
| element_attr_cb | avrcp_ct_get_element_attribute_callback | 收到TG的GetElementAttributes响应回调 |
| play_status_cb | avrcp_ct_get_play_status_callback | 收到TG的GetPlayStatus响应回调 |
| notification_cb | avrcp_ct_notification_callback | 收到TG的Notification回调 |



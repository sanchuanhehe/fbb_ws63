# SLE Connection

SLE (SparkLink Low Energy) 连接管理模块负责设备发现与连接的完整生命周期管理，覆盖广播/扫描、连接建立、配对绑定、连接参数协商和 PHY (Physical Layer)/MCS (Modulation and Coding Scheme) 配置。

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_connection_manager.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [sle_connect_remote_device](#sle_connect_remote_device) | 向远端设备发起SLE连接请求 |
| [sle_create_connection_cancel](#sle_create_connection_cancel) | 取消创建连接 |
| [sle_disconnect_remote_device](#sle_disconnect_remote_device) | 向远端设备发起断开连接请求 |
| [sle_disconnect_all_remote_device](#sle_disconnect_all_remote_device) | 断开所有SLE连接 |
| [sle_update_connect_param](#sle_update_connect_param) | 发送连接参数更新请求 |
| [sle_pair_remote_device](#sle_pair_remote_device) | 向远端设备发起配对请求 |
| [sle_remove_paired_remote_device](#sle_remove_paired_remote_device) | 删除与指定设备的配对 |
| [sle_remove_all_pairs](#sle_remove_all_pairs) | 删除所有配对信息 |
| [sle_get_paired_devices_num](#sle_get_paired_devices_num) | 获取已配对设备数量 |
| [sle_get_paired_devices](#sle_get_paired_devices) | 获取已配对设备地址列表 |
| [sle_get_bonded_devices](#sle_get_bonded_devices) | 获取已绑定设备地址列表 |
| [sle_get_pair_state](#sle_get_pair_state) | 获取指定设备的配对状态 |
| [sle_read_remote_device_rssi](#sle_read_remote_device_rssi) | 读取对端设备RSSI (Received Signal Strength Indicator) 值 |
| [sle_set_acb_evt_param](#sle_set_acb_evt_param) | 设置ACB (Announce Control Block) 链路重传参数 |
| [sle_set_phy_param](#sle_set_phy_param) | 设置SLE PHY参数 |
| [sle_set_mcs](#sle_set_mcs) | 设置调制与编码策略 |
| [sle_set_data_len](#sle_set_data_len) | 设置连接链路最大传输payload字节数 |
| [sle_default_connection_param_set](#sle_default_connection_param_set) | 设置SLE默认连接参数 |
| [sle_connection_register_callbacks](#sle_connection_register_callbacks) | 注册SLE连接管理回调函数 |
| [sle_customize_max_pwr](#sle_customize_max_pwr) | 配置BLE (Bluetooth Low Energy) 和SLE最大功率 |
| [enable_sle](#enable_sle) | 使能SLE协议栈 |
| [disable_sle](#disable_sle) | 关闭SLE协议栈 |
| [sle_get_stack_state](#sle_get_stack_state) | 查询SLE协议栈状态 |
| [sle_set_local_addr](#sle_set_local_addr) | 设置本地设备地址 |
| [sle_get_local_addr](#sle_get_local_addr) | 获取本地设备地址 |
| [sle_set_local_name](#sle_set_local_name) | 设置本地设备名称 |
| [sle_get_local_name](#sle_get_local_name) | 获取本地设备名称 |
| [sle_set_announce_data](#sle_set_announce_data) | 设置设备公开数据 |
| [sle_remove_announce](#sle_remove_announce) | 删除指定设备公开 |
| [sle_set_announce_param](#sle_set_announce_param) | 设置设备公开参数 |
| [sle_start_announce](#sle_start_announce) | 开始设备公开 |
| [sle_stop_announce](#sle_stop_announce) | 停止设备公开 |
| [sle_set_seek_param](#sle_set_seek_param) | 设置设备扫描参数 |
| [sle_start_seek](#sle_start_seek) | 开始设备扫描 |
| [sle_stop_seek](#sle_stop_seek) | 停止设备扫描 |
| [sle_announce_seek_register_callbacks](#sle_announce_seek_register_callbacks) | 注册SLE设备发现回调函数 |
| [sle_transmission_signal_capability_req](#sle_transmission_signal_capability_req) | 发送连接管理能力查询请求 |
| [sle_transmission_register_callbacks](#sle_transmission_register_callbacks) | 注册SLE传输管理回调函数 |

## Functions

### sle_connect_remote_device <a id="sle_connect_remote_device"></a>

```c
errcode_t sle_connect_remote_device(const sle_addr_t *addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_connection_manager.h"
```

**功能说明**

- 向指定远端设备发起SLE连接请求
- 连接状态改变结果通过sle_connect_state_changed_callback回调返回
- 调用后异步执行，连接结果不通过返回值直接获取

**前置条件**

- SLE协议栈已通过enable_sle使能，且sle_enable_callback回调返回成功状态
- 已通过sle_connection_register_callbacks注册连接状态回调

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | const [sle_addr_t](#struct_sle_addr_t) * | 远端设备地址指针 | 不为NULL，指向有效sle_addr_t结构体 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 连接请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_client/src/sle_speed_client.c)

### sle_create_connection_cancel <a id="sle_create_connection_cancel"></a>

```c
errcode_t sle_create_connection_cancel(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_connection_manager.h"
```

**功能说明**

- 取消正在创建的SLE连接
- 调用后异步执行，取消结果通过sle_connect_state_changed_callback回调返回

**前置条件**

- SLE协议栈已通过enable_sle使能
- 已通过sle_connect_remote_device发起到远端设备的连接请求，且连接尚未建立完成
- 已通过sle_connection_register_callbacks注册连接状态回调

**入参**

无

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 取消连接请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

### sle_disconnect_remote_device <a id="sle_disconnect_remote_device"></a>

```c
errcode_t sle_disconnect_remote_device(const sle_addr_t *addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_connection_manager.h"
```

**功能说明**

- 向指定远端设备发起断开连接请求
- 断开连接状态改变结果通过sle_connect_state_changed_callback回调返回
- 调用后异步执行，断开结果不通过返回值直接获取

**前置条件**

- 与远端设备已建立SLE连接，连接状态为SLE_ACB_STATE_CONNECTED
- 已通过sle_connection_register_callbacks注册连接状态回调

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | const [sle_addr_t](#struct_sle_addr_t) * | 远端设备地址指针 | 不为NULL，指向有效sle_addr_t结构体 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 断连请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_chba_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle_chba/src/sle_chba_server.c)

### sle_disconnect_all_remote_device <a id="sle_disconnect_all_remote_device"></a>

```c
errcode_t sle_disconnect_all_remote_device(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_connection_manager.h"
```

**功能说明**

- 断开所有已建立的SLE连接
- 断开连接状态改变结果通过sle_connect_state_changed_callback回调返回
- 调用后异步执行，每个连接的断开结果分别回调通知

**前置条件**

- SLE协议栈已使能，且至少存在一个已建立的SLE连接
- 已通过sle_connection_register_callbacks注册连接状态回调

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 断开所有连接请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

### sle_update_connect_param <a id="sle_update_connect_param"></a>

```c
errcode_t sle_update_connect_param(sle_connection_param_update_t *params)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_connection_manager.h"
```

**功能说明**

- 向远端设备发送连接参数更新请求
- 更新结果通过sle_connect_param_update_callback回调返回
- 参数更新请求前的回调通过sle_connect_param_update_req_callback通知

**前置条件**

- 与远端设备已建立SLE连接
- 已通过sle_connection_register_callbacks注册连接参数更新回调

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| params | [sle_connection_param_update_t](#struct_sle_connection_param_update_t) * | 连接参数更新结构体指针 | 不为NULL，conn_id为有效连接ID，interval_min <= interval_max |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 参数更新请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_server/src/sle_speed_server.c)

### sle_pair_remote_device <a id="sle_pair_remote_device"></a>

```c
errcode_t sle_pair_remote_device(const sle_addr_t *addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_connection_manager.h"
```

**功能说明**

- 向指定远端设备发起配对请求
- 配对完成结果通过sle_pair_complete_callback回调返回
- 认证完成结果通过sle_auth_complete_callback回调返回

**前置条件**

- 与远端设备已建立SLE连接
- 已通过sle_connection_register_callbacks注册配对完成回调

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | const [sle_addr_t](#struct_sle_addr_t) * | 远端设备地址指针 | 不为NULL，指向有效sle_addr_t结构体 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 配对请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_client/src/sle_speed_client.c)

### sle_remove_paired_remote_device <a id="sle_remove_paired_remote_device"></a>

```c
errcode_t sle_remove_paired_remote_device(const sle_addr_t *addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_connection_manager.h"
```

**功能说明**

- 删除与指定远端设备的配对信息
- 删除结果通过sle_pair_remove_callback回调返回
- 调用后异步执行，删除结果不通过返回值直接获取

**前置条件**

- 与指定设备已存在配对记录
- 已通过sle_connection_register_callbacks注册配对删除回调

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | const [sle_addr_t](#struct_sle_addr_t) * | 远端设备地址指针 | 不为NULL，指向有效sle_addr_t结构体 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 删除配对请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_client/src/sle_speed_client.c)

### sle_remove_all_pairs <a id="sle_remove_all_pairs"></a>

```c
errcode_t sle_remove_all_pairs(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_connection_manager.h"
```

**功能说明**

- 删除所有已配对设备的配对信息
- 删除结果通过sle_pair_remove_callback回调返回
- 调用后异步执行，每个配对的删除结果分别回调通知

**前置条件**

- SLE协议栈已使能，且存在已配对设备记录
- 已通过sle_connection_register_callbacks注册配对删除回调

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 删除所有配对请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [oh_sle_srv_cm.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/oh_sle_srv_cm.c)

### sle_get_paired_devices_num <a id="sle_get_paired_devices_num"></a>

```c
errcode_t sle_get_paired_devices_num(uint16_t *number)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_connection_manager.h"
```

**功能说明**

- 获取已配对设备的数量
- 通过出参number返回已配对设备数量
- 调用后同步执行，结果通过出参直接获取

**前置条件**

- SLE协议栈已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| number | uint16_t * | 已配对设备数量出参指针 | 不为NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| number | uint16_t * | 已配对设备数量 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 成功获取已配对设备数量 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [oh_sle_srv_cm.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/oh_sle_srv_cm.c)

### sle_get_paired_devices <a id="sle_get_paired_devices"></a>

```c
errcode_t sle_get_paired_devices(sle_addr_t *addr, uint16_t *number)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_connection_manager.h"
```

**功能说明**

- 获取已配对设备的地址列表
- 通过出参addr返回设备地址链表，number为入参时指定缓冲区大小，出参时返回实际设备数量
- 调用后同步执行，结果通过出参直接获取

**前置条件**

- SLE协议栈已使能
- 已通过sle_get_paired_devices_num获取配对设备数量，并据此分配足够的缓冲区

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | [sle_addr_t](#struct_sle_addr_t) * | 设备地址链表缓冲区指针 | 不为NULL，缓冲区大小不小于number个sle_addr_t |
| number | uint16_t * | 设备数量，入参为缓冲区大小，出参为实际数量 | 不为NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| addr | [sle_addr_t](#struct_sle_addr_t) * | 已配对设备地址列表 |
| number | uint16_t * | 实际已配对设备数量 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 成功获取已配对设备地址列表 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [oh_sle_srv_cm.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/oh_sle_srv_cm.c)

### sle_get_bonded_devices <a id="sle_get_bonded_devices"></a>

```c
errcode_t sle_get_bonded_devices(sle_addr_t *addr, uint16_t *number)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_connection_manager.h"
```

**功能说明**

- 获取已绑定设备的地址列表
- 通过出参addr返回设备地址链表，number为入参时指定缓冲区大小，出参时返回实际设备数量
- 调用后同步执行，结果通过出参直接获取

**前置条件**

- SLE协议栈已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | [sle_addr_t](#struct_sle_addr_t) * | 设备地址链表缓冲区指针 | 不为NULL |
| number | uint16_t * | 设备数量，入参为缓冲区大小，出参为实际数量 | 不为NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| addr | [sle_addr_t](#struct_sle_addr_t) * | 已绑定设备地址列表 |
| number | uint16_t * | 实际已绑定设备数量 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 成功获取已绑定设备地址列表 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

### sle_get_pair_state <a id="sle_get_pair_state"></a>

```c
errcode_t sle_get_pair_state(const sle_addr_t *addr, uint8_t *state)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_connection_manager.h"
```

**功能说明**

- 获取指定设备的配对状态
- 通过出参state返回配对状态，取值参考sle_pair_state_t
- 调用后同步执行，结果通过出参直接获取

**前置条件**

- SLE协议栈已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | const [sle_addr_t](#struct_sle_addr_t) * | 远端设备地址指针 | 不为NULL，指向有效sle_addr_t结构体 |
| state | uint8_t * | 配对状态出参指针 | 不为NULL，取值参考[sle_pair_state_t](#enum_sle_pair_state_t) |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| state | uint8_t * | 配对状态，参考[sle_pair_state_t](#enum_sle_pair_state_t) |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 成功获取配对状态 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [oh_sle_srv_cm.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/oh_sle_srv_cm.c)

### sle_read_remote_device_rssi <a id="sle_read_remote_device_rssi"></a>

```c
errcode_t sle_read_remote_device_rssi(uint16_t conn_id)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_connection_manager.h"
```

**功能说明**

- 读取对端设备的RSSI值
- RSSI值通过sle_read_rssi_callback回调返回
- 调用后异步执行，RSSI值不通过返回值直接获取

**前置条件**

- 与远端设备已建立SLE连接
- 已通过sle_connection_register_callbacks注册RSSI读取回调

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| conn_id | uint16_t | 连接ID | 有效连接ID |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 读取RSSI请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_client/src/sle_speed_client.c)

### sle_set_acb_evt_param <a id="sle_set_acb_evt_param"></a>

```c
errcode_t sle_set_acb_evt_param(uint16_t conn_id, uint16_t evt_intv, uint8_t evt_num)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_connection_manager.h"
```

**功能说明**

- 设置ACB链路重传参数
- 设置重传间隔和重传次数
- 调用后同步生效

**前置条件**

- 与远端设备已建立SLE连接

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| conn_id | uint16_t | 连接ID | 有效连接ID |
| evt_intv | uint16_t | 重传间隔 | - |
| evt_num | uint8_t | 重传次数 | - |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 参数设置成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

### sle_set_phy_param <a id="sle_set_phy_param"></a>

```c
errcode_t sle_set_phy_param(uint16_t conn_id, sle_set_phy_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_connection_manager.h"
```

**功能说明**

- 设置SLE PHY参数，包括无线帧类型、PHY类型、导频密度、反馈类型等
- 设置结果通过sle_set_phy_callback回调返回
- 调用后异步执行，设置结果不通过返回值直接获取

**前置条件**

- 与远端设备已建立SLE连接
- 已通过sle_connection_register_callbacks注册PHY设置回调

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| conn_id | uint16_t | 连接ID | 有效连接ID |
| param | [sle_set_phy_t](#struct_sle_set_phy_t) * | PHY参数结构体指针 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | PHY参数设置请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_server/src/sle_speed_server.c)

### sle_set_mcs <a id="sle_set_mcs"></a>

```c
errcode_t sle_set_mcs(uint16_t conn_id, uint8_t mcs)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_connection_manager.h"
```

**功能说明**

- 设置调制与编码策略（MCS）
- MCS索引值参考sle_mcs_t枚举定义
- 调用后同步生效

**前置条件**

- 与远端设备已建立SLE连接

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| conn_id | uint16_t | 连接ID | 有效连接ID |
| mcs | uint8_t | 调制与编码策略索引值 | 参考[sle_mcs_t](#enum_sle_mcs_t) |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | MCS设置成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_server/src/sle_speed_server.c)

### sle_set_data_len <a id="sle_set_data_len"></a>

```c
errcode_t sle_set_data_len(uint16_t conn_id, uint16_t tx_octets)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_connection_manager.h"
```

**功能说明**

- 设置连接链路上所偏好的最大传输payload字节数
- 设置后影响链路数据传输的payload大小
- 调用后同步生效

**前置条件**

- 与远端设备已建立SLE连接

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| conn_id | uint16_t | 连接ID | 有效连接ID |
| tx_octets | uint16_t | 最大传输payload字节数 | - |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 数据长度设置成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_server/src/sle_speed_server.c)

### sle_default_connection_param_set <a id="sle_default_connection_param_set"></a>

```c
errcode_t sle_default_connection_param_set(sle_default_connect_param_t *set_param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_connection_manager.h"
```

**功能说明**

- 设置SLE默认连接参数，包括过滤策略、扫描间隔、连接间隔、超时时间等
- 在发起连接前调用，设置连接建立时的默认参数
- 调用后同步生效

**前置条件**

- SLE协议栈已使能
- 在发起连接请求之前调用

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| set_param | [sle_default_connect_param_t](#struct_sle_default_connect_param_t) * | 默认连接参数结构体指针 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 默认连接参数设置成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_client/src/sle_speed_client.c)

### sle_connection_register_callbacks <a id="sle_connection_register_callbacks"></a>

```c
errcode_t sle_connection_register_callbacks(sle_connection_callbacks_t *func)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_connection_manager.h"
```

**功能说明**

- 注册SLE连接管理回调函数
- 包含连接状态改变、连接参数更新、认证完成、配对完成、RSSI读取、低时延、PHY设置、配对删除等回调
- 必须在发起连接操作之前注册，否则无法接收回调通知

**前置条件**

- SLE协议栈已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func | [sle_connection_callbacks_t](#struct_sle_connection_callbacks_t) * | 回调函数结构体指针 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 回调注册成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_client/src/sle_speed_client.c)

### sle_customize_max_pwr <a id="sle_customize_max_pwr"></a>

```c
errcode_t sle_customize_max_pwr(int8_t ble_pwr, int8_t sle_pwr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_connection_manager.h"
```

**功能说明**

- 配置BLE和SLE的最大功率
- 分别设置BLE和SLE的发射功率上限
- 调用后同步生效

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| ble_pwr | int8_t | BLE最大功率 | 单位dBm |
| sle_pwr | int8_t | SLE最大功率 | 单位dBm |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 功率配置成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

### enable_sle <a id="enable_sle"></a>

```c
errcode_t enable_sle(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_device_discovery.h"
```

**功能说明**

- 使能SLE协议栈
- 使能结果通过sle_enable_callback回调返回
- 调用后异步执行，使能结果不通过返回值直接获取

**前置条件**

- 已通过sle_announce_seek_register_callbacks注册SLE使能回调

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 使能请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_client/src/sle_speed_client.c)

### disable_sle <a id="disable_sle"></a>

```c
errcode_t disable_sle(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_device_discovery.h"
```

**功能说明**

- 关闭SLE协议栈
- 关闭结果通过sle_disable_callback回调返回
- 调用后异步执行，关闭结果不通过返回值直接获取

**前置条件**

- SLE协议栈已使能
- 已通过sle_announce_seek_register_callbacks注册SLE去使能回调

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 关闭请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [oh_sle_srv_dd.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/oh_sle_srv_dd.c)

### sle_get_stack_state <a id="sle_get_stack_state"></a>

```c
errcode_t sle_get_stack_state(uint8_t *stack_state)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_device_discovery.h"
```

**功能说明**

- 查询SLE协议栈当前状态
- 查询结果通过出参stack_state返回，取值为[sle_stack_state_t](#enum_sle_stack_state_t)枚举

**前置条件**

- 无

**入参**

无

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| stack_state | uint8_t * | 协议栈状态，取值为[sle_stack_state_t](#enum_sle_stack_state_t)枚举，指针不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 状态查询成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

### sle_set_local_addr <a id="sle_set_local_addr"></a>

```c
errcode_t sle_set_local_addr(sle_addr_t *addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_device_discovery.h"
```

**功能说明**

- 设置本地设备地址
- 地址类型和地址值通过sle_addr_t结构体指定
- 调用后同步生效

**前置条件**

- SLE协议栈已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | [sle_addr_t](#struct_sle_addr_t) * | 本地设备地址指针 | 不为NULL，type取值参考[sle_addr_type_t](#enum_sle_addr_type_t) |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 地址设置成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_client/src/sle_speed_client.c)

### sle_get_local_addr <a id="sle_get_local_addr"></a>

```c
errcode_t sle_get_local_addr(sle_addr_t *addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_device_discovery.h"
```

**功能说明**

- 获取本地设备地址
- 通过出参addr返回设备地址
- 调用后同步执行，结果通过出参直接获取

**前置条件**

- SLE协议栈已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | [sle_addr_t](#struct_sle_addr_t) * | 本地设备地址出参指针 | 不为NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| addr | [sle_addr_t](#struct_sle_addr_t) * | 本地设备地址 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 成功获取本地地址 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_chba_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle_chba/src/sle_chba_server.c)

### sle_set_local_name <a id="sle_set_local_name"></a>

```c
errcode_t sle_set_local_name(const uint8_t *name, uint8_t len)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_device_discovery.h"
```

**功能说明**

- 设置本地设备名称
- 名称长度包括结束符\0
- 调用后同步生效

**前置条件**

- SLE协议栈已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| name | const uint8_t * | 本地设备名称指针 | 不为NULL，长度包括结束符\0 |
| len | uint8_t | 本地设备名称长度 | 包括结束符\0，不超过SLE_NAME_MAX_LEN(31) |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 名称设置成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [oh_sle_srv_dd.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/oh_sle_srv_dd.c)

### sle_get_local_name <a id="sle_get_local_name"></a>

```c
errcode_t sle_get_local_name(uint8_t *name, uint8_t *len)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_device_discovery.h"
```

**功能说明**

- 获取本地设备名称
- len作为入参时为用户分配的内存大小，作为出参时为本地设备名称长度
- 调用后同步执行，结果通过出参直接获取

**前置条件**

- SLE协议栈已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| name | uint8_t * | 本地设备名称缓冲区指针 | 不为NULL，缓冲区大小不小于SLE_NAME_MAX_LEN(31) |
| len | uint8_t * | 入参为缓冲区大小，出参为名称长度 | 不为NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| name | uint8_t * | 本地设备名称 |
| len | uint8_t * | 本地设备名称实际长度 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 成功获取本地名称 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [oh_sle_srv_dd.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/oh_sle_srv_dd.c)

### sle_set_announce_data <a id="sle_set_announce_data"></a>

```c
errcode_t sle_set_announce_data(uint8_t announce_id, const sle_announce_data_t *data)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_device_discovery.h"
```

**功能说明**

- 设置设备公开数据
- 包含设备公开数据和扫描响应数据
- 需在开始设备公开前设置

**前置条件**

- SLE协议栈已使能
- 已通过sle_set_announce_param设置设备公开参数

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| announce_id | uint8_t | 设备公开ID | - |
| data | const [sle_announce_data_t](#struct_sle_announce_data_t) * | 设备公开数据指针 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 数据设置成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_server_adv.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_server/src/sle_speed_server_adv.c)

### sle_remove_announce <a id="sle_remove_announce"></a>

```c
errcode_t sle_remove_announce(uint8_t announce_id)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_device_discovery.h"
```

**功能说明**

- 删除指定设备公开
- 删除结果通过sle_announce_remove_callback回调返回
- 调用后异步执行，删除结果不通过返回值直接获取

**前置条件**

- 指定announce_id的设备公开已存在

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| announce_id | uint8_t | 设备公开ID | - |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 删除请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

### sle_set_announce_param <a id="sle_set_announce_param"></a>

```c
errcode_t sle_set_announce_param(uint8_t announce_id, const sle_announce_param_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_device_discovery.h"
```

**功能说明**

- 设置设备公开参数，包括公开类型、G/T角色、发现等级、公开周期、信道、发射功率等
- 需在开始设备公开前设置
- 调用后同步生效

**前置条件**

- SLE协议栈已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| announce_id | uint8_t | 设备公开ID | - |
| param | const [sle_announce_param_t](#struct_sle_announce_param_t) * | 设备公开参数指针 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 参数设置成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_server_adv.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_server/src/sle_speed_server_adv.c)

### sle_start_announce <a id="sle_start_announce"></a>

```c
errcode_t sle_start_announce(uint8_t announce_id)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_device_discovery.h"
```

**功能说明**

- 开始设备公开
- 开始结果通过sle_announce_enable_callback回调返回
- 调用前需已设置设备公开参数和公开数据

**前置条件**

- SLE协议栈已使能
- 已通过sle_set_announce_param设置设备公开参数
- 已通过sle_set_announce_data设置设备公开数据
- 已通过sle_announce_seek_register_callbacks注册设备公开使能回调

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| announce_id | uint8_t | 设备公开ID | - |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 开始公开请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_server/src/sle_speed_server.c)

### sle_stop_announce <a id="sle_stop_announce"></a>

```c
errcode_t sle_stop_announce(uint8_t announce_id)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_device_discovery.h"
```

**功能说明**

- 停止设备公开
- 停止结果通过sle_announce_disable_callback回调返回
- 调用后异步执行，停止结果不通过返回值直接获取

**前置条件**

- 指定announce_id的设备公开已开始

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| announce_id | uint8_t | 设备公开ID | - |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 停止公开请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [oh_sle_srv_dd.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/wifi/ohos_connect/hilink_adapt/adapter/oh_sle_srv_dd.c)

### sle_set_seek_param <a id="sle_set_seek_param"></a>

```c
errcode_t sle_set_seek_param(sle_seek_param_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_device_discovery.h"
```

**功能说明**

- 设置设备扫描参数，包括本端地址类型、重复过滤、扫描过滤策略、PHY类型、扫描类型、扫描间隔和窗口
- 需在开始扫描前设置
- 调用后同步生效

**前置条件**

- SLE协议栈已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| param | [sle_seek_param_t](#struct_sle_seek_param_t) * | 设备扫描参数指针 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 扫描参数设置成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_client/src/sle_speed_client.c)

### sle_start_seek <a id="sle_start_seek"></a>

```c
errcode_t sle_start_seek(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_device_discovery.h"
```

**功能说明**

- 开始设备扫描
- 开始结果通过sle_start_seek_callback回调返回
- 扫描结果通过sle_seek_result_callback回调上报

**前置条件**

- SLE协议栈已使能
- 已通过sle_set_seek_param设置扫描参数
- 已通过sle_announce_seek_register_callbacks注册扫描使能和扫描结果回调

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 开始扫描请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_client/src/sle_speed_client.c)

### sle_stop_seek <a id="sle_stop_seek"></a>

```c
errcode_t sle_stop_seek(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_device_discovery.h"
```

**功能说明**

- 停止设备扫描
- 停止结果通过sle_seek_disable_callback回调返回
- 调用后异步执行，停止结果不通过返回值直接获取

**前置条件**

- 设备扫描已开始

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 停止扫描请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_client/src/sle_speed_client.c)

### sle_announce_seek_register_callbacks <a id="sle_announce_seek_register_callbacks"></a>

```c
errcode_t sle_announce_seek_register_callbacks(sle_announce_seek_callbacks_t *func)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_device_discovery.h"
```

**功能说明**

- 注册SLE设备发现回调函数
- 包含SLE使能/去使能、设备公开使能/关闭/停止/删除、扫描使能/关闭/结果等回调
- 必须在调用enable_sle之前注册，否则无法接收回调通知

**前置条件**

- SLE协议栈尚未使能或已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func | [sle_announce_seek_callbacks_t](#struct_sle_announce_seek_callbacks_t) * | 回调函数结构体指针 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 回调注册成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_client.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_client/src/sle_speed_client.c)

### sle_transmission_signal_capability_req <a id="sle_transmission_signal_capability_req"></a>

```c
errcode_t sle_transmission_signal_capability_req(uint16_t conn_id, sle_transmission_signal_capability_bit_t *param)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_transmition_manager.h"
```

**功能说明**

- 发送连接管理能力查询请求
- 查询能力包括中继能力、传输模式、测量能力、接入能力、MTU (Maximum Transmission Unit)、MPS (Maximum Packet Size) 等
- 调用后异步执行，查询结果通过回调返回

**前置条件**

- 与远端设备已建立SLE连接

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| conn_id | uint16_t | 连接ID | 有效连接ID |
| param | [sle_transmission_signal_capability_bit_t](#struct_sle_transmission_signal_capability_bit_t) * | 能力查询信息指针 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 能力查询请求成功发起 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

### sle_transmission_register_callbacks <a id="sle_transmission_register_callbacks"></a>

```c
errcode_t sle_transmission_register_callbacks(sle_transmission_callbacks_t *func)
```

**头文件清单**

```c
#include "include/middleware/services/bts/sle/sle_transmition_manager.h"
```

**功能说明**

- 注册SLE传输管理回调函数
- 包含传输数据繁忙回调
- 必须在数据传输操作之前注册，否则无法接收回调通知

**前置条件**

- SLE协议栈已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func | [sle_transmission_callbacks_t](#struct_sle_transmission_callbacks_t) * | 回调函数结构体指针 | 不为NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x00) | 执行成功 | 回调注册成功 |
| Other | 其他错误码，参考[errcode_t](#typedef_errcode_t) | 执行失败 |

**参考案例**

- [sle_speed_server.c](https://gitcode.com/HiSpark/fbb_ws63/tree/master/src/application/samples/bt/sle/sle_speed_server/src/sle_speed_server.c)


## Type definitions

### errcode_t <a id="typedef_errcode_t"></a>

```c
typedef uint32_t errcode_t;
```

**定义于**

- include/errcode.h

**说明**

- 通用错误码类型，uint32_t
- ERRCODE_SUCC(0x00)表示成功，其他值表示失败

## Enumerations

### sle_stack_state_t <a id="enum_sle_stack_state_t"></a>

```c
typedef enum {
    SLE_STATE_DISABLED,   /* SLE协议栈已禁能 */
    SLE_STATE_DISABLING,  /* SLE协议栈禁能中 */
    SLE_STATE_ENABLED,    /* SLE协议栈已使能 */
    SLE_STATE_ENABLING,   /* SLE协议栈使能中 */
} sle_stack_state_t;
```

SLE协议栈状态

| 枚举值 | 值 | 说明 |
| ------ | -- | ---- |
| SLE_STATE_DISABLED | 0 | SLE协议栈已禁能 |
| SLE_STATE_DISABLING | 1 | SLE协议栈禁能中 |
| SLE_STATE_ENABLED | 2 | SLE协议栈已使能 |
| SLE_STATE_ENABLING | 3 | SLE协议栈使能中 |

### sle_pair_state_t <a id="enum_sle_pair_state_t"></a>

```c
typedef enum {
    SLE_PAIR_NONE    = 0x01,  /* 未配对状态 */
    SLE_PAIR_PAIRING = 0x02,  /* 正在配对 */
    SLE_PAIR_PAIRED  = 0x03,  /* 已完成配对 */
} sle_pair_state_t;
```

星闪配对状态

| 枚举值 | 值 | 说明 |
| ------ | -- | ---- |
| SLE_PAIR_NONE | 0x01 | 未配对状态 |
| SLE_PAIR_PAIRING | 0x02 | 正在配对 |
| SLE_PAIR_PAIRED | 0x03 | 已完成配对 |

**定义于**

- include/middleware/services/bts/sle/sle_connection_manager.h

### sle_disc_reason_t <a id="enum_sle_disc_reason_t"></a>

```c
typedef enum {
    SLE_DISCONNECT_BY_REMOTE = 0x10,    /*!< @if Eng disconnect by remote
                                             @else   远端断链 @endif */
    SLE_DISCONNECT_BY_LOCAL  = 0x11,    /*!< @if Eng disconnect by local
                                             @else   本端断链 @endif */
} sle_disc_reason_t;
```

星闪断链原因

| 枚举值 | 值 | 说明 |
| ------ | -- | ---- |
| SLE_DISCONNECT_BY_REMOTE | 0x10 | 远端断链 |
| SLE_DISCONNECT_BY_LOCAL | 0x11 | 本端断链 |

**定义于**

- include/middleware/services/bts/sle/sle_connection_manager.h

### sle_acb_state_t <a id="enum_sle_acb_state_t"></a>

```c
typedef enum {
    SLE_ACB_STATE_NONE         = 0x00,  /* SLE ACB未连接状态 */
    SLE_ACB_STATE_CONNECTED    = 0x01,  /* SLE ACB已连接 */
    SLE_ACB_STATE_DISCONNECTED = 0x02,  /* SLE ACB已断接 */
} sle_acb_state_t;
```

SLE ACB连接状态

| 枚举值 | 值 | 说明 |
| ------ | -- | ---- |
| SLE_ACB_STATE_NONE | 0x00 | SLE ACB未连接状态 |
| SLE_ACB_STATE_CONNECTED | 0x01 | SLE ACB已连接 |
| SLE_ACB_STATE_DISCONNECTED | 0x02 | SLE ACB已断接 |

**定义于**

- include/middleware/services/bts/sle/sle_connection_manager.h

### sle_crypto_algo_t <a id="enum_sle_crypto_algo_t"></a>

```c
typedef enum {
    SLE_CRYTO_ALGO_AC1 = 0x01,  /* AC1加密算法类型 */
    SLE_CRYTO_ALGO_AC2 = 0x02,  /* AC2加密算法类型 */
    SLE_CRYTO_ALGO_EA1 = 0x03,  /* EA1加密算法类型 */
    SLE_CRYTO_ALGO_EA2 = 0x04,  /* EA2加密算法类型 */
} sle_crypto_algo_t;
```

星闪加密算法类型

| 枚举值 | 值 | 说明 |
| ------ | -- | ---- |
| SLE_CRYTO_ALGO_AC1 | 0x01 | AC1加密算法类型 |
| SLE_CRYTO_ALGO_AC2 | 0x02 | AC2加密算法类型 |
| SLE_CRYTO_ALGO_EA1 | 0x03 | EA1 (Encryption Algorithm 1) 加密算法类型 |
| SLE_CRYTO_ALGO_EA2 | 0x04 | EA2 (Encryption Algorithm 2) 加密算法类型 |

**定义于**

- include/middleware/services/bts/sle/sle_connection_manager.h

### sle_key_deriv_algo_t <a id="enum_sle_key_deriv_algo_t"></a>

```c
typedef enum {
    SLE_KEY_DERIV_ALGO_HA1     = 0x01,   /*!< @if Eng key derivation algorithm ac1
                                              @else   HA1秘钥分发算法类型 @endif */
    SLE_KEY_DERIV_ALGO_HA2     = 0x02,   /*!< @if Eng key derivation algorithm ac2
                                              @else   HA2秘钥分发算法类型 @endif */
} sle_key_deriv_algo_t;
```

星闪秘钥分发算法类型

| 枚举值 | 值 | 说明 |
| ------ | -- | ---- |
| SLE_KEY_DERIV_ALGO_HA1 | 0x01 | HA1秘钥分发算法类型 |
| SLE_KEY_DERIV_ALGO_HA2 | 0x02 | HA2秘钥分发算法类型 |

**定义于**

- include/middleware/services/bts/sle/sle_connection_manager.h

### sle_integr_chk_ind_t <a id="enum_sle_integr_chk_ind_t"></a>

```c
typedef enum {
    SLE_ENCRYPTION_ENABLE_INTEGRITY_CHK_ENABLE      = 0x00,  /* 加密和完整性保护同时启动 */
    SLE_ENCRYPTION_DISABLE_INTEGRITY_CHK_ENABLE     = 0x01,  /* 不启动加密，启动完整性保护 */
    SLE_ENCRYPTION_ENABLE_INTEGRITY_CHK_DISABLE     = 0x02,  /* 启动加密，不启动完整性保护 */
    SLE_ENCRYPTION_DISABLE_INTEGRITY_CHK_DISABLE    = 0x03,  /* 不启动加密，不启动完整性保护 */
} sle_integr_chk_ind_t;
```

星闪完整性校验指示类型

| 枚举值 | 值 | 说明 |
| ------ | -- | ---- |
| SLE_ENCRYPTION_ENABLE_INTEGRITY_CHK_ENABLE | 0x00 | 加密和完整性保护同时启动 |
| SLE_ENCRYPTION_DISABLE_INTEGRITY_CHK_ENABLE | 0x01 | 不启动加密，启动完整性保护 |
| SLE_ENCRYPTION_ENABLE_INTEGRITY_CHK_DISABLE | 0x02 | 启动加密，不启动完整性保护 |
| SLE_ENCRYPTION_DISABLE_INTEGRITY_CHK_DISABLE | 0x03 | 不启动加密，不启动完整性保护 |

**定义于**

- include/middleware/services/bts/sle/sle_connection_manager.h

### sle_bond_ind_t <a id="enum_sle_bond_ind_t"></a>

```c
typedef enum {
    SLE_PAIR_NO_BOND      = 0x00,   /*!< @if Eng Sle pair does not need bonding.
                                         @else   星闪配对不需要绑定。 @endif */
    SLE_PAIR_NEED_BOND    = 0x01,   /*!< @if Eng Sle pair needs bonding
                                         @else   星闪配对需要绑定。 @endif */
} sle_bond_ind_t;
```

星闪配对绑定指示

| 枚举值 | 值 | 说明 |
| ------ | -- | ---- |
| SLE_PAIR_NO_BOND | 0x00 | 星闪配对不需要绑定 |
| SLE_PAIR_NEED_BOND | 0x01 | 星闪配对需要绑定 |

**定义于**

- include/middleware/services/bts/sle/sle_connection_manager.h

### sle_radio_frame_t <a id="enum_sle_radio_frame_t"></a>

```c
typedef enum {
    SLE_RADIO_FRAME_1    = 0,   /* 无线帧类型1 */
    SLE_RADIO_FRAME_2    = 1,   /* 无线帧类型2 */
    SLE_RADIO_FRAME_3_M0 = 2,   /* 无线帧类型3，m序列0 */
    SLE_RADIO_FRAME_3_M1 = 3,   /* 无线帧类型3，m序列1 */
    SLE_RADIO_FRAME_3_M2 = 4,   /* 无线帧类型3，m序列2 */
    SLE_RADIO_FRAME_3_M3 = 5,   /* 无线帧类型3，m序列3 */
    SLE_RADIO_FRAME_3_M4 = 6,   /* 无线帧类型3，m序列4 */
    SLE_RADIO_FRAME_3_M5 = 7,   /* 无线帧类型3，m序列5 */
    SLE_RADIO_FRAME_4_M0 = 8,   /* 无线帧类型4，m序列0 */
    SLE_RADIO_FRAME_4_M1 = 9,   /* 无线帧类型4，m序列1 */
    SLE_RADIO_FRAME_4_M2 = 10,  /* 无线帧类型4，m序列2 */
    SLE_RADIO_FRAME_4_M3 = 11,  /* 无线帧类型4，m序列3 */
    SLE_RADIO_FRAME_4_M4 = 12,  /* 无线帧类型4，m序列4 */
    SLE_RADIO_FRAME_4_M5 = 13,  /* 无线帧类型4，m序列5 */
} sle_radio_frame_t;
```

星闪无线帧类型

| 枚举值 | 值 | 说明 |
| ------ | -- | ---- |
| SLE_RADIO_FRAME_1 | 0 | 无线帧类型1 |
| SLE_RADIO_FRAME_2 | 1 | 无线帧类型2 |
| SLE_RADIO_FRAME_3_M0 | 2 | 无线帧类型3，m序列0 |
| SLE_RADIO_FRAME_3_M1 | 3 | 无线帧类型3，m序列1 |
| SLE_RADIO_FRAME_3_M2 | 4 | 无线帧类型3，m序列2 |
| SLE_RADIO_FRAME_3_M3 | 5 | 无线帧类型3，m序列3 |
| SLE_RADIO_FRAME_3_M4 | 6 | 无线帧类型3，m序列4 |
| SLE_RADIO_FRAME_3_M5 | 7 | 无线帧类型3，m序列5 |
| SLE_RADIO_FRAME_4_M0 | 8 | 无线帧类型4，m序列0 |
| SLE_RADIO_FRAME_4_M1 | 9 | 无线帧类型4，m序列1 |
| SLE_RADIO_FRAME_4_M2 | 10 | 无线帧类型4，m序列2 |
| SLE_RADIO_FRAME_4_M3 | 11 | 无线帧类型4，m序列3 |
| SLE_RADIO_FRAME_4_M4 | 12 | 无线帧类型4，m序列4 |
| SLE_RADIO_FRAME_4_M5 | 13 | 无线帧类型4，m序列5 |

**定义于**

- include/middleware/services/bts/sle/sle_connection_manager.h

### sle_phy_tx_rx_t <a id="enum_sle_phy_tx_rx_t"></a>

```c
typedef enum {
    SLE_PHY_1M = 0x0,          /*!< @if Eng 1M PHY
                                         @else 1M PHY @endif */
    SLE_PHY_2M = 0x1,          /*!< @if Eng 2M PHY
                                         @else 2M PHY @endif */
    SLE_PHY_4M = 0x2,          /*!< @if Eng 4M PHY
                                         @else 4M PHY @endif */
    SLE_PHY_SUPPORT_NUM,
} sle_phy_tx_rx_t;
```

星闪发送/接收PHY类型

| 枚举值 | 值 | 说明 |
| ------ | -- | ---- |
| SLE_PHY_1M | 0x0 | 1M PHY |
| SLE_PHY_2M | 0x1 | 2M PHY |
| SLE_PHY_4M | 0x2 | 4M PHY |

**定义于**

- include/middleware/services/bts/sle/sle_connection_manager.h

### sle_phy_tx_rx_pilot_density_t <a id="enum_sle_phy_tx_rx_pilot_density_t"></a>

```c
typedef enum sle_phy_tx_rx_pilot_density_t {
    SLE_PHY_PILOT_DENSITY_4_TO_1 = 0x0,  /* 导频密度为4:1 */
    SLE_PHY_PILOT_DENSITY_8_TO_1 = 0x1,  /* 导频密度为8:1 */
    SLE_PHY_PILOT_DENSITY_16_TO_1 = 0x2,  /* 导频密度为16:1 */
    SLE_PHY_PILOT_DENSITY_NO = 0x3,  /* 无导频 */
} sle_phy_tx_rx_pilot_density_t;
```

星闪发送/接收导频密度指示

| 枚举值 | 值 | 说明 |
| ------ | -- | ---- |
| SLE_PHY_PILOT_DENSITY_4_TO_1 | 0x0 | 导频密度为4:1 |
| SLE_PHY_PILOT_DENSITY_8_TO_1 | 0x1 | 导频密度为8:1 |
| SLE_PHY_PILOT_DENSITY_16_TO_1 | 0x2 | 导频密度为16:1 |
| SLE_PHY_PILOT_DENSITY_NO | 0x3 | 无导频 |

**定义于**

- include/middleware/services/bts/sle/sle_connection_manager.h

### sle_save_smp_keys_mode_switch_t <a id="enum_sle_save_smp_keys_mode_switch_t"></a>

```c
typedef enum {
    SLE_SAVE_SMP_KEYS_AUTO = 0x00,          /*!< @if Eng Pair information unavailable
                                                        @else   秘钥自动保存 @endif */
    SLE_SAVE_SMP_KEYS_MANU = 0x01,            /*!< @if Eng Pair information available
                                                        @else   秘钥用户手动保存 @endif */
} sle_save_smp_keys_mode_switch_t;
```

配对秘钥保存模式开关

| 枚举值 | 值 | 说明 |
| ------ | -- | ---- |
| SLE_SAVE_SMP_KEYS_AUTO | 0x00 | 秘钥自动保存 |
| SLE_SAVE_SMP_KEYS_MANU | 0x01 | 秘钥用户手动保存 |

**定义于**

- include/middleware/services/bts/sle/sle_connection_manager.h

### sle_announce_level_t <a id="enum_sle_announce_level_t"></a>

```c
typedef enum {
    SLE_ANNOUNCE_LEVEL_NONE     = 0,  /* 不可见发现，预留 */
    SLE_ANNOUNCE_LEVEL_NORMAL   = 1,  /* 一般可发现 */
    SLE_ANNOUNCE_LEVEL_PRIORITY = 2,  /* 优先可发现，预留 */
    SLE_ANNOUNCE_LEVEL_PAIRED   = 3,  /* 被曾配对过的设备发现，预留 */
    SLE_ANNOUNCE_LEVEL_SPECIAL  = 4,  /* 被指定设备发现 */
} sle_announce_level_t;
```

被发现方可发现等级

| 枚举值 | 值 | 说明 |
| ------ | -- | ---- |
| SLE_ANNOUNCE_LEVEL_NONE | 0 | 不可见发现，预留 |
| SLE_ANNOUNCE_LEVEL_NORMAL | 1 | 一般可发现 |
| SLE_ANNOUNCE_LEVEL_PRIORITY | 2 | 优先可发现，预留 |
| SLE_ANNOUNCE_LEVEL_PAIRED | 3 | 被曾配对过的设备发现，预留 |
| SLE_ANNOUNCE_LEVEL_SPECIAL | 4 | 被指定设备发现 |

**定义于**

- include/middleware/services/bts/sle/sle_device_discovery.h

### sle_announce_gt_role_t <a id="enum_sle_announce_gt_role_t"></a>

```c
typedef enum {
    SLE_ANNOUNCE_ROLE_T_CAN_NEGO = 0,  /* 期望做T可协商 */
    SLE_ANNOUNCE_ROLE_G_CAN_NEGO = 1,  /* 期望做G可协商 */
    SLE_ANNOUNCE_ROLE_T_NO_NEGO  = 2,  /* 期望做T不可协商 */
    SLE_ANNOUNCE_ROLE_G_NO_NEGO  = 3,  /* 期望做G不可协商 */
} sle_announce_gt_role_t;
```

G/T角色协商指示

| 枚举值 | 值 | 说明 |
| ------ | -- | ---- |
| SLE_ANNOUNCE_ROLE_T_CAN_NEGO | 0 | 期望做T可协商 |
| SLE_ANNOUNCE_ROLE_G_CAN_NEGO | 1 | 期望做G可协商 |
| SLE_ANNOUNCE_ROLE_T_NO_NEGO | 2 | 期望做T不可协商 |
| SLE_ANNOUNCE_ROLE_G_NO_NEGO | 3 | 期望做G不可协商 |

**定义于**

- include/middleware/services/bts/sle/sle_device_discovery.h

### sle_announce_mode_t <a id="enum_sle_announce_mode_t"></a>

```c
typedef enum {
    SLE_ANNOUNCE_MODE_NONCONN_NONSCAN      = 0x00,  /* 不可连接不可扫描 */
    SLE_ANNOUNCE_MODE_CONNECTABLE_NONSCAN  = 0x01,  /* 可连接不可扫描 */
    SLE_ANNOUNCE_MODE_NONCONN_SCANABLE     = 0x02,  /* 不可连接可扫描 */
    SLE_ANNOUNCE_MODE_CONNECTABLE_SCANABLE = 0x03,  /* 可连接可扫描 */
    SLE_ANNOUNCE_MODE_CONNECTABLE_DIRECTED = 0x07,  /* 可连接可扫描定向 */
} sle_announce_mode_t;
```

设备公开类型

| 枚举值 | 值 | 说明 |
| ------ | -- | ---- |
| SLE_ANNOUNCE_MODE_NONCONN_NONSCAN | 0x00 | 不可连接不可扫描 |
| SLE_ANNOUNCE_MODE_CONNECTABLE_NONSCAN | 0x01 | 可连接不可扫描 |
| SLE_ANNOUNCE_MODE_NONCONN_SCANABLE | 0x02 | 不可连接可扫描 |
| SLE_ANNOUNCE_MODE_CONNECTABLE_SCANABLE | 0x03 | 可连接可扫描 |
| SLE_ANNOUNCE_MODE_CONNECTABLE_DIRECTED | 0x07 | 可连接可扫描定向 |

**定义于**

- include/middleware/services/bts/sle/sle_device_discovery.h

### sle_seek_phy_t <a id="enum_sle_seek_phy_t"></a>

```c
typedef enum {
    SLE_SEEK_PHY_1M = 0x1,  /* 1M PHY */
    SLE_SEEK_PHY_2M = 0x2,  /* 2M PHY */
    SLE_SEEK_PHY_4M = 0x4,  /* 4M PHY */
} sle_seek_phy_t;
```

设备发现PHY类型

| 枚举值 | 值 | 说明 |
| ------ | -- | ---- |
| SLE_SEEK_PHY_1M | 0x1 | 1M PHY |
| SLE_SEEK_PHY_2M | 0x2 | 2M PHY |
| SLE_SEEK_PHY_4M | 0x4 | 4M PHY |

**定义于**

- include/middleware/services/bts/sle/sle_device_discovery.h

### sle_seek_type_t <a id="enum_sle_seek_type_t"></a>

```c
typedef enum {
    SLE_SEEK_PASSIVE = 0x00,  /* 被动扫描 */
    SLE_SEEK_ACTIVE  = 0x01,  /* 主动扫描 */
} sle_seek_type_t;
```

设备发现类型

| 枚举值 | 值 | 说明 |
| ------ | -- | ---- |
| SLE_SEEK_PASSIVE | 0x00 | 被动扫描 |
| SLE_SEEK_ACTIVE | 0x01 | 主动扫描 |

**定义于**

- include/middleware/services/bts/sle/sle_device_discovery.h

### sle_seek_filter_t <a id="enum_sle_seek_filter_t"></a>

```c
typedef enum {
    SLE_SEEK_FILTER_ALLOW_ALL   = 0x00, /*!< @if Eng allow all
                                                       @else   允许来自任何人的设备发现数据包 @endif */
    SLE_SEEK_FILTER_ALLOW_WLST  = 0x01, /*!< @if Eng allow only white list, reserve
                                                       @else   允许来自白名单设备的设备发现数据包，预留 @endif */
} sle_seek_filter_t;
```

设备发现过滤类型

| 枚举值 | 值 | 说明 |
| ------ | -- | ---- |
| SLE_SEEK_FILTER_ALLOW_ALL | 0x00 | 允许来自任何人的设备发现数据包 |
| SLE_SEEK_FILTER_ALLOW_WLST | 0x01 | 允许来自白名单设备的设备发现数据包，预留 |

**定义于**

- include/middleware/services/bts/sle/sle_device_discovery.h

### sle_mcs_t <a id="enum_sle_mcs_t"></a>

```c
typedef enum {
    SLE_MCS_00 = 0,   /* MCS0: BPSK1/4 */
    SLE_MCS_01 = 1,   /* MCS1: BPSK3/8 */
    SLE_MCS_02 = 2,   /* MCS2: QPSK1/4 */
    SLE_MCS_03 = 3,   /* MCS3: QPSK3/8 */
    SLE_MCS_04 = 4,   /* MCS4: QPSK1/2 */
    SLE_MCS_05 = 5,   /* MCS5: QPSK5/8 */
    SLE_MCS_06 = 6,   /* MCS6: QPSK3/4 */
    SLE_MCS_07 = 7,   /* MCS7: QPSK7/8 */
    SLE_MCS_08 = 8,   /* MCS8: QPSK 1 */
    SLE_MCS_09 = 9,   /* MCS9: 8PSK5/8 */
    SLE_MCS_10 = 10,  /* MCS10: 8PSK3/4 */
    SLE_MCS_11 = 11,  /* MCS11: 8PSK7/8 */
    SLE_MCS_12 = 12,  /* MCS12: 8PSK 1 */
} sle_mcs_t;
```

调制与编码策略

| 枚举值 | 值 | 说明 |
| ------ | -- | ---- |
| SLE_MCS_00 | 0 | MCS0: BPSK1/4 |
| SLE_MCS_01 | 1 | MCS1: BPSK3/8 |
| SLE_MCS_02 | 2 | MCS2: QPSK1/4 |
| SLE_MCS_03 | 3 | MCS3: QPSK3/8 |
| SLE_MCS_04 | 4 | MCS4: QPSK1/2 |
| SLE_MCS_05 | 5 | MCS5: QPSK5/8 |
| SLE_MCS_06 | 6 | MCS6: QPSK3/4 |
| SLE_MCS_07 | 7 | MCS7: QPSK7/8 |
| SLE_MCS_08 | 8 | MCS8: QPSK (Quadrature Phase Shift Keying) 1 |
| SLE_MCS_09 | 9 | MCS9: 8PSK5/8 |
| SLE_MCS_10 | 10 | MCS10: 8PSK3/4 |
| SLE_MCS_11 | 11 | MCS11: 8PSK7/8 |
| SLE_MCS_12 | 12 | MCS12: 8PSK 1 |

**定义于**

- include/middleware/services/bts/sle/sle_device_discovery.h

### sle_filter_policy_t <a id="enum_sle_filter_policy_t"></a>

```c
typedef enum {
    SLE_ANNOUNCE_FLT_ANY_SEEK_ANY_CONNECT          = 0,  /* 接受所有seek_req/conn_req */
    SLE_ANNOUNCE_FLT_WHITE_SEEK_ANY_CONNECT         = 1,  /* 只接受符合过滤器的seek_req */
    SLE_ANNOUNCE_FLT_ANY_SEEK_WHITE_CONNECT         = 2,  /* 只接受符合过滤器的conn_req */
    SLE_ANNOUNCE_FLT_WHITE_SEEK_WHITE_CONNECT       = 3,  /* 接受符合过滤器的seek_req/conn_req */
} sle_filter_policy_t;
```

过滤策略

| 枚举值 | 值 | 说明 |
| ------ | -- | ---- |
| SLE_ANNOUNCE_FLT_ANY_SEEK_ANY_CONNECT | 0 | 接受所有seek_req/conn_req |
| SLE_ANNOUNCE_FLT_WHITE_SEEK_ANY_CONNECT | 1 | 只接受符合过滤器的seek_req |
| SLE_ANNOUNCE_FLT_ANY_SEEK_WHITE_CONNECT | 2 | 只接受符合过滤器的conn_req |
| SLE_ANNOUNCE_FLT_WHITE_SEEK_WHITE_CONNECT | 3 | 接受符合过滤器的seek_req/conn_req |

**定义于**

- include/middleware/services/bts/sle/sle_device_discovery.h

### sle_addr_type_t <a id="enum_sle_addr_type_t"></a>

```c
typedef enum {
    SLE_ADDRESS_TYPE_PUBLIC = 0,      /*!< @if Eng public address
                                           @else   公有地址 @endif */
    SLE_ADDRESS_TYPE_RANDOM = 6,      /*!< @if Eng random address
                                           @else   随机地址 @endif */
} sle_addr_type_t;
```

SLE地址类型

| 枚举值 | 值 | 说明 |
| ------ | -- | ---- |
| SLE_ADDRESS_TYPE_PUBLIC | 0 | 公有地址 |
| SLE_ADDRESS_TYPE_RANDOM | 6 | 随机地址 |

**定义于**

- include/middleware/services/bts/sle/sle_common.h

### sle_low_latency_rate_t <a id="enum_sle_low_latency_rate_t"></a>

```c
typedef enum sle_low_latency_rate_t {
    SLE_LOW_LATENCY_125HZ = 0,  /* 125HZ调度 */
    SLE_LOW_LATENCY_500HZ = 1,  /* 500HZ调度 */
    SLE_LOW_LATENCY_1K = 2,     /* 1000Hz调度 */
    SLE_LOW_LATENCY_2K = 3,     /* 2000HZ调度 */
    SLE_LOW_LATENCY_3K = 4,     /* 3000HZ调度 */
    SLE_LOW_LATENCY_4K = 5,     /* 4000Hz调度 */
    SLE_LOW_LATENCY_5K = 6,     /* 5000Hz调度 */
    SLE_LOW_LATENCY_6K = 7,     /* 6000HZ调度 */
    SLE_LOW_LATENCY_7K = 8,     /* 7000HZ调度 */
    SLE_LOW_LATENCY_8K = 9,     /* 8000HZ调度 */
} sle_low_latency_rate_t;
```

低时延调度速率

| 枚举值 | 值 | 说明 |
| ------ | -- | ---- |
| SLE_LOW_LATENCY_125HZ | 0 | 125HZ调度 |
| SLE_LOW_LATENCY_500HZ | 1 | 500HZ调度 |
| SLE_LOW_LATENCY_1K | 2 | 1000Hz调度 |
| SLE_LOW_LATENCY_2K | 3 | 2000HZ调度 |
| SLE_LOW_LATENCY_3K | 4 | 3000HZ调度 |
| SLE_LOW_LATENCY_4K | 5 | 4000Hz调度 |
| SLE_LOW_LATENCY_5K | 6 | 5000Hz调度 |
| SLE_LOW_LATENCY_6K | 7 | 6000HZ调度 |
| SLE_LOW_LATENCY_7K | 8 | 7000HZ调度 |
| SLE_LOW_LATENCY_8K | 9 | 8000HZ调度 |

**定义于**

- include/middleware/services/bts/sle/sle_low_latency.h

### sle_link_qos_state_t <a id="enum_sle_link_qos_state_t"></a>

```c
typedef enum {
    SLE_QOS_IDLE     = 0x00,  /* 空闲状态 */
    SLE_QOS_FLOWCTRL = 0x01,  /* 流控状态 */
    SLE_QOS_BUSY     = 0x02,  /* 繁忙状态 */
} sle_link_qos_state_t;
```

星闪链路忙闲状态

| 枚举值 | 值 | 说明 |
| ------ | -- | ---- |
| SLE_QOS_IDLE | 0x00 | 空闲状态 |
| SLE_QOS_FLOWCTRL | 0x01 | 流控状态 |
| SLE_QOS_BUSY | 0x02 | 繁忙状态 |

**定义于**

- include/middleware/services/bts/sle/sle_transmition_manager.h

## Structures

### sle_addr_t <a id="struct_sle_addr_t"></a>

```c
typedef struct {
    uint8_t type;                         /*!< @if Eng SLE device address type { @ref sle_addr_type_t }
                                               @else   SLE设备地址类型 { @ref sle_addr_type_t } @endif */
    unsigned char addr[SLE_ADDR_LEN];     /*!< @if Eng SLE device address
                                               @else   SLE设备地址 @endif */
} sle_addr_t;
```

设备地址

| 成员 | 类型 | 说明 |
| ------ | ---- | ---- |
| type | uint8_t | SLE设备地址类型，参考[sle_addr_type_t](#enum_sle_addr_type_t) |
| addr | unsigned char[SLE_ADDR_LEN(6)] | SLE设备地址 |

**定义于**

- include/middleware/services/bts/sle/sle_common.h

### sle_connection_param_update_req_t <a id="struct_sle_connection_param_update_req_t"></a>

```c
typedef struct sle_connection_param_update_req_t {
    uint16_t interval_min;        /* 链路调度最小间隔，单位slot */
    uint16_t interval_max;        /* 链路调度最大间隔，单位slot */
    uint16_t max_latency;         /* 延迟周期，单位slot */
    uint16_t supervision_timeout; /* 超时时间，单位10ms */
} sle_connection_param_update_req_t;
```

星闪逻辑链路更新参数请求

| 成员 | 类型 | 说明 |
| ------ | ---- | ---- |
| interval_min | uint16_t | 链路调度最小间隔，单位slot |
| interval_max | uint16_t | 链路调度最大间隔，单位slot |
| max_latency | uint16_t | 延迟周期，单位slot |
| supervision_timeout | uint16_t | 超时时间，单位10ms |

**定义于**

- include/middleware/services/bts/sle/sle_connection_manager.h

### sle_connection_param_update_t <a id="struct_sle_connection_param_update_t"></a>

```c
typedef struct sle_connection_param_update_t {
    uint16_t conn_id;             /* 连接ID */
    uint16_t interval_min;        /* 链路调度最小间隔，单位slot */
    uint16_t interval_max;        /* 链路调度最大间隔，单位slot */
    uint16_t max_latency;         /* 延迟周期，单位slot */
    uint16_t supervision_timeout; /* 超时时间，单位10ms */
} sle_connection_param_update_t;
```

星闪逻辑链路更新参数

| 成员 | 类型 | 说明 |
| ------ | ---- | ---- |
| conn_id | uint16_t | 连接ID |
| interval_min | uint16_t | 链路调度最小间隔，单位slot |
| interval_max | uint16_t | 链路调度最大间隔，单位slot |
| max_latency | uint16_t | 延迟周期，单位slot |
| supervision_timeout | uint16_t | 超时时间，单位10ms |

**定义于**

- include/middleware/services/bts/sle/sle_connection_manager.h

### sle_connection_param_update_evt_t <a id="struct_sle_connection_param_update_evt_t"></a>

```c
typedef struct sle_connection_param_update_evt_t {
    uint16_t interval;   /* 链路调度间隔，单位slot */
    uint16_t latency;    /* 延迟周期，单位slot */
    uint16_t supervision; /* 超时时间，单位10ms */
} sle_connection_param_update_evt_t;
```

星闪逻辑链路更新事件参数

| 成员 | 类型 | 说明 |
| ------ | ---- | ---- |
| interval | uint16_t | 链路调度间隔，单位slot |
| latency | uint16_t | 延迟周期，单位slot |
| supervision | uint16_t | 超时时间，单位10ms |

**定义于**

- include/middleware/services/bts/sle/sle_connection_manager.h

### sle_auth_info_evt_t <a id="struct_sle_auth_info_evt_t"></a>

```c
typedef struct sle_auth_info_evt_t {
    uint8_t link_key[SLE_LINK_KEY_LEN(16)];  /* 链路密钥 */
    uint8_t crypto_algo;                      /* 加密算法类型，参考sle_crypto_algo_t */
    uint8_t key_deriv_algo;                   /* 秘钥分发算法类型，参考sle_key_deriv_algo_t */
    uint8_t integr_chk_ind;                   /* 完整性校验指示，参考sle_integr_chk_ind_t */
    uint8_t is_bond;                          /* 配对绑定指示，参考sle_bond_ind_t */
} sle_auth_info_evt_t;
```

星闪认证结果

| 成员 | 类型 | 说明 |
| ------ | ---- | ---- |
| link_key | uint8_t[SLE_LINK_KEY_LEN(16)] | 链路密钥 |
| crypto_algo | uint8_t | 加密算法类型，参考[sle_crypto_algo_t](#enum_sle_crypto_algo_t) |
| key_deriv_algo | uint8_t | 秘钥分发算法类型，参考[sle_key_deriv_algo_t](#enum_sle_key_deriv_algo_t) |
| integr_chk_ind | uint8_t | 完整性校验指示，参考[sle_integr_chk_ind_t](#enum_sle_integr_chk_ind_t) |
| is_bond | uint8_t | 配对绑定指示，参考[sle_bond_ind_t](#enum_sle_bond_ind_t) |

**定义于**

- include/middleware/services/bts/sle/sle_connection_manager.h

### sle_set_phy_t <a id="struct_sle_set_phy_t"></a>

```c
typedef struct {
    uint8_t tx_format;          /* 发送无线帧类型，参考sle_radio_frame_t */
    uint8_t rx_format;          /* 接收无线帧类型，参考sle_radio_frame_t */
    uint8_t tx_phy;             /* 发送PHY，参考sle_phy_tx_rx_t */
    uint8_t rx_phy;             /* 接收PHY，参考sle_phy_tx_rx_t */
    uint8_t tx_pilot_density;   /* 发送导频密度指示，参考sle_phy_tx_rx_pilot_density_t */
    uint8_t rx_pilot_density;   /* 接收导频密度指示，参考sle_phy_tx_rx_pilot_density_t */
    uint8_t g_feedback;         /* 先发链路反馈类型指示，取值范围0-63 */
    uint8_t t_feedback;         /* 后发链路反馈类型指示，取值范围0-7 */
} sle_set_phy_t;
```

星闪PHY参数

| 成员 | 类型 | 说明 |
| ------ | ---- | ---- |
| tx_format | uint8_t | 发送无线帧类型，参考[sle_radio_frame_t](#enum_sle_radio_frame_t) |
| rx_format | uint8_t | 接收无线帧类型，参考[sle_radio_frame_t](#enum_sle_radio_frame_t) |
| tx_phy | uint8_t | 发送PHY，参考[sle_phy_tx_rx_t](#enum_sle_phy_tx_rx_t) |
| rx_phy | uint8_t | 接收PHY，参考[sle_phy_tx_rx_t](#enum_sle_phy_tx_rx_t) |
| tx_pilot_density | uint8_t | 发送导频密度指示，参考[sle_phy_tx_rx_pilot_density_t](#enum_sle_phy_tx_rx_pilot_density_t) |
| rx_pilot_density | uint8_t | 接收导频密度指示，参考[sle_phy_tx_rx_pilot_density_t](#enum_sle_phy_tx_rx_pilot_density_t) |
| g_feedback | uint8_t | 先发链路反馈类型指示，取值范围0-63 |
| t_feedback | uint8_t | 后发链路反馈类型指示，取值范围0-7 |

**定义于**

- include/middleware/services/bts/sle/sle_connection_manager.h

### sle_default_connect_param_t <a id="struct_sle_default_connect_param_t"></a>

```c
typedef struct {
    uint8_t  enable_filter_policy;  /* 链路是否打开过滤功能 */
    uint8_t  initiate_phys;         /* 链路扫描通信带宽：1:1M, 2:2M */
    uint8_t  gt_negotiate;          /* 链路建立时是否进行G和T交互 */
    uint16_t scan_interval;          /* 链路建立时扫描对端设备的interval */
    uint16_t scan_window;            /* 链路建立时扫描对端设备的windows */
    uint16_t min_interval;           /* 链路调度最小interval */
    uint16_t max_interval;           /* 链路调度最大interval */
    uint16_t timeout;                /* 链路超时时间 */
} sle_default_connect_param_t;
```

星闪默认连接参数

| 成员 | 类型 | 说明 |
| ------ | ---- | ---- |
| enable_filter_policy | uint8_t | 链路是否打开过滤功能 |
| initiate_phys | uint8_t | 链路扫描通信带宽：1:1M, 2:2M |
| gt_negotiate | uint8_t | 链路建立时是否进行G和T交互 |
| scan_interval | uint16_t | 链路建立时扫描对端设备的interval |
| scan_window | uint16_t | 链路建立时扫描对端设备的windows |
| min_interval | uint16_t | 链路调度最小interval |
| max_interval | uint16_t | 链路调度最大interval |
| timeout | uint16_t | 链路超时时间 |

**定义于**

- include/middleware/services/bts/sle/sle_connection_manager.h

### sle_connection_callbacks_t <a id="struct_sle_connection_callbacks_t"></a>

```c
typedef struct sle_connection_callbacks_t {
    sle_connect_state_changed_callback connect_state_changed_cb;  /* 连接状态改变回调函数 */
    sle_connect_param_update_req_callback connect_param_update_req_cb;  /* 连接参数更新请求回调函数 */
    sle_connect_param_update_callback connect_param_update_cb;  /* 连接参数更新回调函数 */
    sle_auth_complete_callback auth_complete_cb;  /* 认证完成回调函数 */
    sle_pair_complete_callback pair_complete_cb;  /* 配对完成回调函数 */
    sle_read_rssi_callback read_rssi_cb;  /* 读取RSSI回调函数 */
    sle_low_latency_callback low_latency_cb;  /* 设置low latency回调函数 */
    sle_set_phy_callback set_phy_cb;  /* 设置PHY回调函数 */
    sle_pair_remove_callback pair_remove_cb;  /* 取消配对完成回调函数 */
} sle_connection_callbacks_t;
```

SLE连接管理回调函数接口定义

| 成员 | 类型 | 说明 |
| ------ | ---- | ---- |
| connect_state_changed_cb | sle_connect_state_changed_callback | 连接状态改变回调函数 |
| connect_param_update_req_cb | sle_connect_param_update_req_callback | 连接参数更新请求回调函数 |
| connect_param_update_cb | sle_connect_param_update_callback | 连接参数更新回调函数 |
| auth_complete_cb | sle_auth_complete_callback | 认证完成回调函数 |
| pair_complete_cb | sle_pair_complete_callback | 配对完成回调函数 |
| read_rssi_cb | sle_read_rssi_callback | 读取RSSI回调函数 |
| low_latency_cb | sle_low_latency_callback | 设置low latency回调函数 |
| set_phy_cb | sle_set_phy_callback | 设置PHY回调函数 |
| pair_remove_cb | sle_pair_remove_callback | 取消配对完成回调函数 |

**定义于**

- include/middleware/services/bts/sle/sle_connection_manager.h

### sle_conn_param_t <a id="struct_sle_conn_param_t"></a>

```c
typedef struct sle_conn_param_t {
    uint16_t interval_min;          /* 连接间隔最小取值，取值范围[0x001E,0x3E80]，时间=N*0.25ms，时间范围[7.5ms,4s] */
    uint16_t interval_max;          /* 连接间隔最大取值，取值范围[0x001E,0x3E80]，时间=N*0.25ms，时间范围[7.5ms,4s] */
    uint16_t max_latency;           /* 最大休眠连接间隔，取值范围[0x0000,0x01F3]，默认0x0000 */
    uint16_t supervision_timeout;   /* 最大超时时间，取值范围[0x000A,0x0C80]，时间=N*10ms，时间范围[100ms,32s] */
    uint16_t min_ce_length;         /* 推荐的连接事件最小取值，取值范围[0x0000,0xFFFF]，时间=N*0.125ms */
    uint16_t max_ce_length;         /* 推荐的连接事件最大取值，取值范围[0x0000,0xFFFF]，时间=N*0.125ms */
} sle_conn_param_t;
```

连接参数（做G时有效）

| 成员 | 类型 | 说明 |
| ------ | ---- | ---- |
| interval_min | uint16_t | 连接间隔最小取值，取值范围[0x001E,0x3E80]，时间=N*0.25ms，时间范围[7.5ms,4s] |
| interval_max | uint16_t | 连接间隔最大取值，取值范围[0x001E,0x3E80]，时间=N*0.25ms，时间范围[7.5ms,4s] |
| max_latency | uint16_t | 最大休眠连接间隔，取值范围[0x0000,0x01F3]，默认0x0000 |
| supervision_timeout | uint16_t | 最大超时时间，取值范围[0x000A,0x0C80]，时间=N*10ms，时间范围[100ms,32s] |
| min_ce_length | uint16_t | 推荐的连接事件最小取值，取值范围[0x0000,0xFFFF]，时间=N*0.125ms |
| max_ce_length | uint16_t | 推荐的连接事件最大取值，取值范围[0x0000,0xFFFF]，时间=N*0.125ms |

**定义于**

- include/middleware/services/bts/sle/sle_device_discovery.h

### sle_announce_param_t <a id="struct_sle_announce_param_t"></a>

```c
typedef struct {
    uint8_t  announce_handle;               /* 设备公开句柄，取值范围[0, 0xFF] */
    uint8_t  announce_mode;                 /* 设备公开类型，参考sle_announce_mode_t */
    uint8_t  announce_gt_role;              /* G/T角色协商指示，参考sle_announce_gt_role_t */
    uint8_t  announce_level;                /* 发现等级，参考sle_announce_level_t */
    uint32_t announce_interval_min;         /* 最小设备公开周期，0x000020~0xffffff，单位125us */
    uint32_t announce_interval_max;         /* 最大设备公开周期，0x000020~0xffffff，单位125us */
    uint8_t  announce_channel_map;          /* 设备公开信道，0:76, 1:77, 2:78 */
    int8_t   announce_tx_power;             /* 广播发射功率，单位dBm，取值范围[-127, 20]，0x7F:不设置特定发送功率 */
    sle_addr_t own_addr;                    /* 本端地址 */
    sle_addr_t peer_addr;                   /* 对端地址 */
    uint16_t conn_interval_min;             /* 连接间隔最小取值，取值范围[0x001E,0x3E80] */
    uint16_t conn_interval_max;             /* 连接间隔最大取值，取值范围[0x001E,0x3E80] */
    uint16_t conn_max_latency;              /* 最大休眠连接间隔，取值范围[0x0000,0x01F3] */
    uint16_t conn_supervision_timeout;      /* 最大超时时间，取值范围[0x000A,0x0C80] */
    void * ext_param;                       /* 扩展设备公开参数，缺省时置空 */
} sle_announce_param_t;
```

设备公开参数

| 成员 | 类型 | 说明 |
| ------ | ---- | ---- |
| announce_handle | uint8_t | 设备公开句柄，取值范围[0, 0xFF] |
| announce_mode | uint8_t | 设备公开类型，参考[sle_announce_mode_t](#enum_sle_announce_mode_t) |
| announce_gt_role | uint8_t | G/T角色协商指示，参考[sle_announce_gt_role_t](#enum_sle_announce_gt_role_t) |
| announce_level | uint8_t | 发现等级，参考[sle_announce_level_t](#enum_sle_announce_level_t) |
| announce_interval_min | uint32_t | 最小设备公开周期，0x000020~0xffffff，单位125us |
| announce_interval_max | uint32_t | 最大设备公开周期，0x000020~0xffffff，单位125us |
| announce_channel_map | uint8_t | 设备公开信道，0:76, 1:77, 2:78 |
| announce_tx_power | int8_t | 广播发射功率，单位dBm，取值范围[-127, 20]，0x7F:不设置特定发送功率 |
| own_addr | [sle_addr_t](#struct_sle_addr_t) | 本端地址 |
| peer_addr | [sle_addr_t](#struct_sle_addr_t) | 对端地址 |
| conn_interval_min | uint16_t | 连接间隔最小取值，取值范围[0x001E,0x3E80]，announce_gt_role为SLE_ANNOUNCE_ROLE_T_NO_NEGO时无需配置 |
| conn_interval_max | uint16_t | 连接间隔最大取值，取值范围[0x001E,0x3E80]，announce_gt_role为SLE_ANNOUNCE_ROLE_T_NO_NEGO时无需配置 |
| conn_max_latency | uint16_t | 最大休眠连接间隔，取值范围[0x0000,0x01F3]，announce_gt_role为SLE_ANNOUNCE_ROLE_T_NO_NEGO时无需配置 |
| conn_supervision_timeout | uint16_t | 最大超时时间，取值范围[0x000A,0x0C80]，announce_gt_role为SLE_ANNOUNCE_ROLE_T_NO_NEGO时无需配置 |
| ext_param | void * | 扩展设备公开参数，缺省时置空 |

**定义于**

- include/middleware/services/bts/sle/sle_device_discovery.h

### sle_announce_data_t <a id="struct_sle_announce_data_t"></a>

```c
typedef struct sle_announce_data_t {
    uint16_t announce_data_len;  /* 设备公开数据长度 */
    uint16_t seek_rsp_data_len;  /* 扫描响应数据长度 */
    uint8_t *announce_data;      /* 设备公开数据 */
    uint8_t *seek_rsp_data;      /* 扫描响应数据 */
} sle_announce_data_t;
```

设备公开数据

| 成员 | 类型 | 说明 |
| ------ | ---- | ---- |
| announce_data_len | uint16_t | 设备公开数据长度 |
| seek_rsp_data_len | uint16_t | 扫描响应数据长度 |
| announce_data | uint8_t * | 设备公开数据 |
| seek_rsp_data | uint8_t * | 扫描响应数据 |

**定义于**

- include/middleware/services/bts/sle/sle_device_discovery.h

### sle_announce_enable_t <a id="struct_sle_announce_enable_t"></a>

```c
typedef struct sle_announce_enable_t {
    uint8_t enable;               /* 0x0:关闭设备公开, 0x1:使能设备公开 */
    uint8_t announce_handle;      /* 设备公开句柄 */
    uint16_t duration;            /* 0x0:设备公开时间无限制, 0x1~0xFFFF:设备公开时间=N*10ms */
    uint8_t max_announce_events;  /* 0x0:设备公开事件个数无限制, 0x1~0xFF:设备公开事件个数限制 */
} sle_announce_enable_t;
```

设备公开使能参数

| 成员 | 类型 | 说明 |
| ------ | ---- | ---- |
| enable | uint8_t | 0x0:关闭设备公开, 0x1:使能设备公开 |
| announce_handle | uint8_t | 设备公开句柄 |
| duration | uint16_t | 0x0:设备公开时间无限制, 0x1~0xFFFF:设备公开时间=N*10ms |
| max_announce_events | uint8_t | 0x0:设备公开事件个数无限制, 0x1~0xFF:设备公开事件个数限制 |

**定义于**

- include/middleware/services/bts/sle/sle_device_discovery.h

### sle_seek_param_t <a id="struct_sle_seek_param_t"></a>

```c
typedef struct sle_seek_param_t {
    uint8_t own_addr_type;                        /* 本端地址类型 */
    uint8_t filter_duplicates;                    /* 重复过滤开关，0:关闭，1:开启 */
    uint8_t seek_filter_policy;                   /* 扫描设备使用的过滤类型，参考sle_seek_filter_t */
    uint8_t seek_phys;                            /* 扫描设备所使用的PHY，参考sle_seek_phy_t */
    uint8_t seek_type[SLE_SEEK_PHY_NUM_MAX(3)];   /* 扫描类型，参考sle_seek_type_t */
    uint16_t seek_interval[SLE_SEEK_PHY_NUM_MAX(3)]; /* 扫描间隔，取值范围[0x0014, 0xFFFF]，time=N*0.125ms */
    uint16_t seek_window[SLE_SEEK_PHY_NUM_MAX(3)];   /* 扫描窗口，取值范围[0x0014, 0xFFFF]，time=N*0.125ms */
} sle_seek_param_t;
```

设备发现扫描参数

| 成员 | 类型 | 说明 |
| ------ | ---- | ---- |
| own_addr_type | uint8_t | 本端地址类型 |
| filter_duplicates | uint8_t | 重复过滤开关，0:关闭，1:开启 |
| seek_filter_policy | uint8_t | 扫描设备使用的过滤类型，参考[sle_seek_filter_t](#enum_sle_seek_filter_t) |
| seek_phys | uint8_t | 扫描设备所使用的PHY，参考[sle_seek_phy_t](#enum_sle_seek_phy_t) |
| seek_type | uint8_t[SLE_SEEK_PHY_NUM_MAX(3)] | 扫描类型，参考[sle_seek_type_t](#enum_sle_seek_type_t) |
| seek_interval | uint16_t[SLE_SEEK_PHY_NUM_MAX(3)] | 扫描间隔，取值范围[0x0014, 0xFFFF]，time=N*0.125ms |
| seek_window | uint16_t[SLE_SEEK_PHY_NUM_MAX(3)] | 扫描窗口，取值范围[0x0014, 0xFFFF]，time=N*0.125ms |

**定义于**

- include/middleware/services/bts/sle/sle_device_discovery.h

### sle_seek_result_info_t <a id="struct_sle_seek_result_info_t"></a>

```c
typedef struct sle_seek_result_info_t {
    uint8_t event_type;               /* 上报事件类型 */
    sle_addr_t addr;                  /* 地址 */
    sle_addr_t direct_addr;           /* 定向发现地址 */
    uint8_t rssi;                     /* 信号强度指示，取值范围[-127dBm, 20dBm]，0x7F表示不提供信号强度指示 */
    uint8_t data_status;              /* 数据状态 */
    uint8_t data_length;              /* 数据长度 */
    uint8_t *data;                    /* 数据 */
} sle_seek_result_info_t;
```

扫描结果报告设备信息

| 成员 | 类型 | 说明 |
| ------ | ---- | ---- |
| event_type | uint8_t | 上报事件类型 |
| addr | [sle_addr_t](#struct_sle_addr_t) | 地址 |
| direct_addr | [sle_addr_t](#struct_sle_addr_t) | 定向发现地址 |
| rssi | uint8_t | 信号强度指示，取值范围[-127dBm, 20dBm]，0x7F表示不提供信号强度指示 |
| data_status | uint8_t | 数据状态 |
| data_length | uint8_t | 数据长度 |
| data | uint8_t * | 数据 |

**定义于**

- include/middleware/services/bts/sle/sle_device_discovery.h

### sle_announce_seek_callbacks_t <a id="struct_sle_announce_seek_callbacks_t"></a>

```c
typedef struct sle_announce_seek_callbacks_t {
    sle_enable_callback sle_enable_cb;  /* SLE协议栈使能回调函数 */
    sle_disable_callback sle_disable_cb;  /* SLE协议栈去使能回调函数 */
    sle_announce_enable_callback announce_enable_cb;  /* 设备公开使能回调函数 */
    sle_announce_disable_callback announce_disable_cb;  /* 设备公开关闭回调函数 */
    sle_announce_terminal_callback announce_terminal_cb;  /* 设备公开停止回调函数 */
    sle_announce_remove_callback announce_remove_cb;  /* 设备公开删除回调函数 */
    sle_start_seek_callback seek_enable_cb;  /* 扫描使能回调函数 */
    sle_seek_disable_callback seek_disable_cb;  /* 扫描关闭回调函数 */
    sle_seek_result_callback seek_result_cb;  /* 扫描结果回调函数 */
    sle_dfr_callback sle_dfr_cb;  /* dfr回调函数 */
} sle_announce_seek_callbacks_t;
```

SLE设备公开回调函数接口定义

| 成员 | 类型 | 说明 |
| ------ | ---- | ---- |
| sle_enable_cb | sle_enable_callback | SLE协议栈使能回调函数 |
| sle_disable_cb | sle_disable_callback | SLE协议栈去使能回调函数 |
| announce_enable_cb | sle_announce_enable_callback | 设备公开使能回调函数 |
| announce_disable_cb | sle_announce_disable_callback | 设备公开关闭回调函数 |
| announce_terminal_cb | sle_announce_terminal_callback | 设备公开停止回调函数 |
| announce_remove_cb | sle_announce_remove_callback | 设备公开删除回调函数 |
| seek_enable_cb | sle_start_seek_callback | 扫描使能回调函数 |
| seek_disable_cb | sle_seek_disable_callback | 扫描关闭回调函数 |
| seek_result_cb | sle_seek_result_callback | 扫描结果回调函数 |
| sle_dfr_cb | sle_dfr_callback | dfr回调函数 |

**定义于**

- include/middleware/services/bts/sle/sle_device_discovery.h

### sle_transmission_callbacks_t <a id="struct_sle_transmission_callbacks_t"></a>

```c
typedef struct {
    sle_trans_data_busy_callback send_data_cb;             /*!< @if Eng Trans data busy callback.
                                                                @else   传输数据繁忙回调函数。 @endif */
} sle_transmission_callbacks_t;
```

SLE传输管理回调函数接口定义

| 成员 | 类型 | 说明 |
| ------ | ---- | ---- |
| send_data_cb | sle_trans_data_busy_callback | 传输数据繁忙回调函数 |

**定义于**

- include/middleware/services/bts/sle/sle_transmition_manager.h

### sle_transmission_signal_capability_bit_t <a id="struct_sle_transmission_signal_capability_bit_t"></a>

```c
typedef struct sle_transmission_signal_capability_bit_t {
    uint32_t relay_capability : 1;          /* 中继能力 */
    uint32_t trans_mode : 1;                /* 传输模式 */
    uint32_t measurement_capability : 1;    /* 测量能力 */
    uint32_t access_slb : 1;                /* slb接入 */
    uint32_t access_sle : 1;                /* sle接入 */
    uint32_t mtu : 1;                       /* 最大支持MTU */
    uint32_t mps : 1;                       /* 最大支持MPS */
    uint32_t reverse : 25;                  /* 保留比特位 */
} sle_transmission_signal_capability_bit_t;
```

连接管理查询能力比特位参数

| 成员 | 类型 | 说明 |
| ------ | ---- | ---- |
| relay_capability | uint32_t:1 | 中继能力 |
| trans_mode | uint32_t:1 | 传输模式 |
| measurement_capability | uint32_t:1 | 测量能力 |
| access_slb | uint32_t:1 | slb接入 |
| access_sle | uint32_t:1 | sle接入 |
| mtu | uint32_t:1 | 最大支持MTU |
| mps | uint32_t:1 | 最大支持MPS |
| reverse | uint32_t:25 | 保留比特位 |

**定义于**

- include/middleware/services/bts/sle/sle_transmition_manager.h

### sle_set_acb_low_latency_t <a id="struct_sle_set_acb_low_latency_t"></a>

```c
typedef struct {
    uint16_t conn_id; /*!< @if Eng connection ID.
                           @else 连接ID @endif */
    uint8_t  enable;  /*!< @if Eng low latency enable, { @ref sle_low_latency_rate_t }.
                           @else 低时延使能状态, { @ref sle_low_latency_rate_t } @endif */
    uint8_t  rate;    /*!< @if Eng low latency rate, { @ref sle_low_latency_rate_t }.
                           @else 低时延调度速率, { @ref sle_low_latency_rate_t }  @endif */
} sle_set_acb_low_latency_t;
```

星闪层低时延配置接口

| 成员 | 类型 | 说明 |
| ------ | ---- | ---- |
| conn_id | uint16_t | 连接ID |
| enable | uint8_t | 低时延使能状态，参考[sle_low_latency_status_t](../service/low_latency/low_latency.md#enum_sle_low_latency_status_t) |
| rate | uint8_t | 低时延调度速率，参考[sle_low_latency_rate_t](#enum_sle_low_latency_rate_t) |

**定义于**

- include/middleware/services/bts/sle/sle_low_latency.h





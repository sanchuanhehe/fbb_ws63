# BR GAP

经典蓝牙（BR/EDR）GAP (Generic Access Profile) 层 API。管理蓝牙协议栈使能/去使能、设备地址获取、连接管理和回调注册。

负责设备发现和链路建立的基础功能，与 BLE (Bluetooth Low Energy) GAP 区分——BR/EDR 用于高带宽场景（音频、串口透传），BLE 用于低功耗场景。

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_br_gap.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [enable_bt_stack](#enable_bt_stack) | 使能BT (Bluetooth) 协议栈 |
| [disable_bt_stack](#disable_bt_stack) | 去使能BT协议栈 |
| [bluetooth_set_local_addr](#bluetooth_set_local_addr) | 设置本地设备蓝牙地址 |
| [bluetooth_get_local_addr](#bluetooth_get_local_addr) | 获取本地设备蓝牙地址 |
| [bluetooth_get_local_name](#bluetooth_get_local_name) | 获取本地设备蓝牙名称 |
| [bluetooth_set_local_name](#bluetooth_set_local_name) | 设置本地设备蓝牙名称 |
| [bluetooth_factory_reset](#bluetooth_factory_reset) | 恢复蓝牙出厂设置 |
| [gap_br_get_scan_mode](#gap_br_get_scan_mode) | 获取蓝牙广播模式 |
| [gap_br_set_bt_scan_mode](#gap_br_set_bt_scan_mode) | 设置蓝牙广播模式 |
| [gap_get_paired_devices_num](#gap_get_paired_devices_num) | 获取配对设备数量 |
| [gap_get_paired_devices_list](#gap_get_paired_devices_list) | 获取配对设备列表 |
| [gap_get_pair_state](#gap_get_pair_state) | 获取设备配对状态 |
| [gap_remove_pair](#gap_remove_pair) | 删除指定配对设备 |
| [gap_remove_all_pairs](#gap_remove_all_pairs) | 删除所有配对设备 |
| [bt_is_acl_connected](#bt_is_acl_connected) | 检查ACL (Asynchronous Connection-Less) 链路是否连接 |
| [gap_disconnect_remote_device](#gap_disconnect_remote_device) | 断开远端设备连接 |
| [gap_connect_remote_device](#gap_connect_remote_device) | 与远端设备建立ACL连接 |
| [gap_br_start_discovery](#gap_br_start_discovery) | 启动蓝牙扫描 |
| [gap_br_cancel_discovery](#gap_br_cancel_discovery) | 停止蓝牙扫描 |
| [is_bt_discovering](#is_bt_discovering) | 检查是否正在扫描 |
| [gap_read_remote_rssi_value](#gap_read_remote_rssi_value) | 获取远端设备RSSI (Received Signal Strength Indicator) 信号强度 |
| [gap_get_device_name](#gap_get_device_name) | 获取远端设备名称 |
| [gap_get_device_class](#gap_get_device_class) | 获取远端设备CoD |
| [gap_register_callbacks](#gap_register_callbacks) | 注册GAP回调函数 |

## Functions

### enable_bt_stack <a id="enable_bt_stack"></a>

```c
errcode_t enable_bt_stack(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_br_gap.h"
```

**功能说明**

- 使能BT协议栈，初始化蓝牙底层资源与协议栈运行环境
- 调用该接口后协议栈进入初始化状态，状态变化通过 [gap_state_changed_callback](#typedef_gap_state_changed_callback) 回调上报
- 需在调用其他BR (Basic Rate) GAP接口之前完成使能

**前置条件**

- 蓝牙硬件资源已就绪
- 未重复调用使能接口

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 执行成功 | 协议栈使能成功 |
| 非0 | 执行失败 | 协议栈使能失败 |

### disable_bt_stack <a id="disable_bt_stack"></a>

```c
errcode_t disable_bt_stack(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_br_gap.h"
```

**功能说明**

- 去使能BT协议栈，释放蓝牙底层资源与协议栈运行环境
- 调用该接口后协议栈进入去初始化状态，状态变化通过 [gap_state_changed_callback](#typedef_gap_state_changed_callback) 回调上报
- 所有BR/EDR连接将在去使能过程中断开

**前置条件**

- BT协议栈已通过 enable_bt_stack() 使能

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 执行成功 | 协议栈去使能成功 |
| 非0 | 执行失败 | 协议栈去使能失败 |

### bluetooth_set_local_addr <a id="bluetooth_set_local_addr"></a>

```c
errcode_t bluetooth_set_local_addr(unsigned char *mac, unsigned int len)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_br_gap.h"
```

**功能说明**

- 设置本地设备的蓝牙地址
- 地址设置后，本地设备以新地址进行广播与连接
- 地址变更通过 [gap_local_device_addr_changed_callback](#typedef_gap_local_device_addr_changed_callback) 回调上报

**前置条件**

- BT协议栈已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| mac | unsigned char * | 蓝牙设备地址 | 非NULL，长度为6字节 |
| len | unsigned int | 地址长度 | 6 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 执行成功 | 设置本地设备地址成功 |
| 非0 | 执行失败 | 设置本地设备地址失败 |

### bluetooth_get_local_addr <a id="bluetooth_get_local_addr"></a>

```c
errcode_t bluetooth_get_local_addr(unsigned char *mac, unsigned int len)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_br_gap.h"
```

**功能说明**

- 获取本地设备的蓝牙地址
- 获取到的地址写入mac指向的缓冲区
- 返回当前本地设备使用的蓝牙地址

**前置条件**

- BT协议栈已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| mac | unsigned char * | 蓝牙设备地址输出缓冲区 | 非NULL，长度不小于6字节 |
| len | unsigned int | 地址长度 | 6 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| mac | unsigned char * | 本地设备蓝牙地址 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 执行成功 | 获取本地设备地址成功 |
| 非0 | 执行失败 | 获取本地设备地址失败 |

### bluetooth_get_local_name <a id="bluetooth_get_local_name"></a>

```c
errcode_t bluetooth_get_local_name(unsigned char *local_name, unsigned char *length)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_br_gap.h"
```

**功能说明**

- 获取本地设备的蓝牙名称
- 获取到的名称写入local_name指向的缓冲区，实际长度写入length
- 名称采用UTF-8编码

**前置条件**

- BT协议栈已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| local_name | unsigned char * | 蓝牙设备名称输出缓冲区 | 非NULL |
| length | unsigned char * | 获取到的名称长度输出 | 非NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| local_name | unsigned char * | 本地设备蓝牙名称 |
| length | unsigned char * | 名称实际长度 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 执行成功 | 获取本地设备名称成功 |
| 非0 | 执行失败 | 获取本地设备名称失败 |

### bluetooth_set_local_name <a id="bluetooth_set_local_name"></a>

```c
errcode_t bluetooth_set_local_name(const unsigned char *local_name, unsigned char length)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_br_gap.h"
```

**功能说明**

- 设置本地设备的蓝牙名称
- 名称设置后生效，名称变更通过 [gap_local_device_name_changed_callback](#typedef_gap_local_device_name_changed_callback) 回调上报
- 名称采用UTF-8编码，长度包括结束符\0

**前置条件**

- BT协议栈已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| local_name | const unsigned char * | 蓝牙设备名称 | 非NULL，UTF-8编码 |
| length | unsigned char | 名称长度，包括结束符\0 | 1~32 |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 执行成功 | 设置本地设备名称成功 |
| 非0 | 执行失败 | 设置本地设备名称失败 |

### bluetooth_factory_reset <a id="bluetooth_factory_reset"></a>

```c
bool bluetooth_factory_reset(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_br_gap.h"
```

**功能说明**

- 恢复蓝牙出厂设置，清除所有配对信息与用户配置
- 调用后蓝牙恢复至初始状态，所有已配对设备信息将被清除
- 需重新使能协议栈后方可继续使用

**前置条件**

- BT协议栈已使能

**返回值**

- 返回类型：bool

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| true | 恢复出厂设置成功 | 恢复操作成功 |
| false | 恢复出厂设置失败 | 恢复操作失败 |

### gap_br_get_scan_mode <a id="gap_br_get_scan_mode"></a>

```c
int gap_br_get_scan_mode(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_br_gap.h"
```

**功能说明**

- 获取当前蓝牙广播模式
- 返回值为 [gap_scan_mode_t](#enum_gap_scan_mode_t) 枚举类型对应的扫描模式
- 用于查询当前设备的可发现性和可连接性状态

**前置条件**

- BT协议栈已使能

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| GAP_SCAN_MODE_NONE(0x00) | 未知扫描类型 | 未设置扫描模式 |
| GAP_SCAN_MODE_CONNECTABLE(0x01) | 可连接扫描类型 | 设备可连接 |
| GAP_SCAN_MODE_GENERAL_DISCOVERABLE(0x02) | 通用广播扫描类型 | 设备通用可发现 |
| GAP_SCAN_MODE_LIMITED_DISCOVERABLE(0x03) | 受限广播扫描类型 | 设备受限可发现 |
| GAP_SCAN_MODE_CONNECTABLE_GENERAL_DISCOVERABLE(0x04) | 可连接的通用广播扫描类型 | 设备可连接且通用可发现 |
| GAP_SCAN_MODE_CONNECTABLE_LIMITED_DISCOVERABLE(0x05) | 可连接的受限广播扫描类型 | 设备可连接且受限可发现 |

### gap_br_set_bt_scan_mode <a id="gap_br_set_bt_scan_mode"></a>

```c
bool gap_br_set_bt_scan_mode(int mode, int duration)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_br_gap.h"
```

**功能说明**

- 设置蓝牙广播模式，控制设备的可发现性与可连接性
- 模式变更通过 [gap_scan_mode_changed_callback](#typedef_gap_scan_mode_changed_callback) 回调上报
- duration参数控制广播时长

**前置条件**

- BT协议栈已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| mode | int | 广播模式，参考 [gap_scan_mode_t](#enum_gap_scan_mode_t) | GAP_SCAN_MODE_NONE(0x00) ~ GAP_SCAN_MODE_CONNECTABLE_LIMITED_DISCOVERABLE(0x05) |
| duration | int | 广播时长 | 大于0 |

**返回值**

- 返回类型：bool

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| true | 设置蓝牙广播模式成功 | 设置成功 |
| false | 设置蓝牙广播模式失败 | 设置失败 |

### gap_get_paired_devices_num <a id="gap_get_paired_devices_num"></a>

```c
bool gap_get_paired_devices_num(unsigned int *number)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_br_gap.h"
```

**功能说明**

- 获取已配对设备的数量
- 获取到的数量写入number指向的变量
- 用于在调用 gap_get_paired_devices_list 前确定配对设备数量以分配缓冲区

**前置条件**

- BT协议栈已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| number | unsigned int * | 配对设备数量输出 | 非NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| number | unsigned int * | 已配对设备数量 |

**返回值**

- 返回类型：bool

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| true | 获取配对设备数量成功 | 获取成功 |
| false | 获取配对设备数量失败 | 获取失败 |

### gap_get_paired_devices_list <a id="gap_get_paired_devices_list"></a>

```c
bool gap_get_paired_devices_list(gap_paired_device_info_t *dev_info, int *number)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_br_gap.h"
```

**功能说明**

- 获取已配对设备的列表信息
- 获取到的设备信息写入dev_info指向的缓冲区，实际数量写入number
- 调用前建议先通过 gap_get_paired_devices_num 获取设备数量以分配足够缓冲区

**前置条件**

- BT协议栈已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| dev_info | [gap_paired_device_info_t](#struct_gap_paired_device_info_t) * | 配对设备信息输出缓冲区 | 非NULL |
| number | int * | 配对设备数量输出 | 非NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| dev_info | gap_paired_device_info_t * | 已配对设备信息列表 |
| number | int * | 实际配对设备数量 |

**返回值**

- 返回类型：bool

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| true | 获取配对设备列表成功 | 获取成功 |
| false | 获取配对设备列表失败 | 获取失败 |

### gap_get_pair_state <a id="gap_get_pair_state"></a>

```c
int gap_get_pair_state(const bd_addr_t *addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_br_gap.h"
```

**功能说明**

- 获取指定设备的配对状态
- 返回值为 [gap_pair_state_t](#enum_gap_pair_state_t) 枚举类型对应的配对状态
- 配对状态变化通过 [gap_pair_status_changed_callback](#typedef_gap_pair_status_changed_callback) 回调上报

**前置条件**

- BT协议栈已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | const [bd_addr_t](#struct_bd_addr_t) * | 待查询的设备地址 | 非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| GAP_PAIR_NONE(0x01) | 未配对 | 设备未配对 |
| GAP_PAIR_PAIRING(0x02) | 配对中 | 正在配对 |
| GAP_PAIR_PAIRED(0x03) | 已配对 | 配对已完成 |

### gap_remove_pair <a id="gap_remove_pair"></a>

```c
bool gap_remove_pair(const bd_addr_t *addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_br_gap.h"
```

**功能说明**

- 删除指定地址的配对设备，从配对列表中移除
- 删除后该设备的配对信息与绑定密钥将被清除
- 配对状态变化通过 [gap_pair_status_changed_callback](#typedef_gap_pair_status_changed_callback) 回调上报

**前置条件**

- BT协议栈已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | const [bd_addr_t](#struct_bd_addr_t) * | 待删除的设备地址 | 非NULL |

**返回值**

- 返回类型：bool

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| true | 删除配对设备成功 | 删除成功 |
| false | 删除配对设备失败 | 删除失败 |

### gap_remove_all_pairs <a id="gap_remove_all_pairs"></a>

```c
bool gap_remove_all_pairs(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_br_gap.h"
```

**功能说明**

- 删除所有配对设备，清空配对列表
- 所有配对信息与绑定密钥将被清除
- 配对状态变化通过 [gap_pair_status_changed_callback](#typedef_gap_pair_status_changed_callback) 回调上报

**前置条件**

- BT协议栈已使能

**返回值**

- 返回类型：bool

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| true | 删除所有配对设备成功 | 删除成功 |
| false | 删除所有配对设备失败 | 删除失败 |

### bt_is_acl_connected <a id="bt_is_acl_connected"></a>

```c
bool bt_is_acl_connected(const bd_addr_t *addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_br_gap.h"
```

**功能说明**

- 检查与指定设备的ACL链路是否已建立连接
- ACL连接状态变化通过 [gap_acl_state_changed_callback](#typedef_gap_acl_state_changed_callback) 回调上报
- 用于在发起Profile连接前确认底层ACL链路是否就绪

**前置条件**

- BT协议栈已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | const [bd_addr_t](#struct_bd_addr_t) * | 待检查的设备地址 | 非NULL |

**返回值**

- 返回类型：bool

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| true | ACL已连接 | ACL链路已建立 |
| false | ACL未连接 | ACL链路未建立 |

### gap_disconnect_remote_device <a id="gap_disconnect_remote_device"></a>

```c
errcode_t gap_disconnect_remote_device(const bd_addr_t *addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_br_gap.h"
```

**功能说明**

- 断开与指定远端设备的所有连接，包括所有Profile连接
- 断开连接后ACL链路将被释放，连接状态变化通过 [gap_acl_state_changed_callback](#typedef_gap_acl_state_changed_callback) 回调上报
- 所有关联Profile的连接状态也将变更

**前置条件**

- BT协议栈已使能
- 与目标设备已建立ACL连接

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | const [bd_addr_t](#struct_bd_addr_t) * | 待断开连接的设备地址 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 执行成功 | 断开连接成功 |
| 非0 | 执行失败 | 断开连接失败 |

### gap_connect_remote_device <a id="gap_connect_remote_device"></a>

```c
errcode_t gap_connect_remote_device(const bd_addr_t *addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_br_gap.h"
```

**功能说明**

- 与指定远端设备建立ACL连接
- ACL连接状态变化通过 [gap_acl_state_changed_callback](#typedef_gap_acl_state_changed_callback) 回调上报
- ACL连接建立后方可进行Profile连接

**前置条件**

- BT协议栈已使能
- 与目标设备未建立ACL连接

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | const [bd_addr_t](#struct_bd_addr_t) * | 待连接的设备地址 | 非NULL |

**返回值**

- 返回类型：errcode_t

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| 0 | 执行成功 | 连接请求成功发起 |
| 非0 | 执行失败 | 连接请求失败 |

### gap_br_start_discovery <a id="gap_br_start_discovery"></a>

```c
bool gap_br_start_discovery(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_br_gap.h"
```

**功能说明**

- 启动BR/EDR蓝牙扫描，发现周围蓝牙设备
- 扫描状态变化通过 [gap_discovery_state_changed_callback](#typedef_gap_discovery_state_changed_callback) 回调上报
- 发现的设备通过 [gap_discovery_result_callback](#typedef_gap_discovery_result_callback) 回调上报

**前置条件**

- BT协议栈已使能
- 当前未处于扫描状态

**返回值**

- 返回类型：bool

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| true | 启动扫描成功 | 扫描启动成功 |
| false | 启动扫描失败 | 扫描启动失败 |

### gap_br_cancel_discovery <a id="gap_br_cancel_discovery"></a>

```c
bool gap_br_cancel_discovery(void)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_br_gap.h"
```

**功能说明**

- 停止BR/EDR蓝牙扫描
- 扫描状态变化通过 [gap_discovery_state_changed_callback](#typedef_gap_discovery_state_changed_callback) 回调上报
- 停止后不再上报新的发现结果

**前置条件**

- BT协议栈已使能
- 当前处于扫描状态

**返回值**

- 返回类型：bool

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| true | 停止扫描成功 | 停止成功 |
| false | 停止扫描失败 | 停止失败 |

### is_bt_discovering <a id="is_bt_discovering"></a>

```c
bool is_bt_discovering(int transport)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_br_gap.h"
```

**功能说明**

- 检查蓝牙是否正在扫描
- 通过transport参数指定查询BR/EDR或BLE的扫描状态
- 用于判断当前是否需要停止扫描

**前置条件**

- BT协议栈已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| transport | int | 数据传输类型，参考 [bt_transport_type_t](#enum_bt_transport_type_t) | BT_TRANSPORT_BR_EDR(0x01) / BT_TRANSPORT_LE(0x02) |

**返回值**

- 返回类型：bool

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| true | 扫描中 | 正在扫描 |
| false | 扫描停止 | 未在扫描 |

### gap_read_remote_rssi_value <a id="gap_read_remote_rssi_value"></a>

```c
bool gap_read_remote_rssi_value(const bd_addr_t *addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_br_gap.h"
```

**功能说明**

- 读取远端设备的RSSI信号强度
- RSSI值通过 [gap_read_remote_rssi_event_callback](#typedef_gap_read_remote_rssi_event_callback) 回调上报
- 需与目标设备已建立ACL连接方可读取

**前置条件**

- BT协议栈已使能
- 与目标设备已建立ACL连接

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | const [bd_addr_t](#struct_bd_addr_t) * | 待查询的设备地址 | 非NULL |

**返回值**

- 返回类型：bool

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| true | 获取RSSI信号强度成功 | 读取请求成功发起 |
| false | 获取RSSI信号强度失败 | 读取请求失败 |

### gap_get_device_name <a id="gap_get_device_name"></a>

```c
bool gap_get_device_name(const bd_addr_t *addr, unsigned char *remote_name, unsigned char *length)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_br_gap.h"
```

**功能说明**

- 获取远端设备的蓝牙名称
- 名称通过remote_name输出，实际长度通过length输出
- 名称采用UTF-8编码

**前置条件**

- BT协议栈已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | const [bd_addr_t](#struct_bd_addr_t) * | 远端设备地址 | 非NULL |
| remote_name | unsigned char * | 远端设备名称输出缓冲区 | 非NULL |
| length | unsigned char * | 名称实际长度输出 | 非NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| remote_name | unsigned char * | 远端设备名称 |
| length | unsigned char * | 名称实际长度 |

**返回值**

- 返回类型：bool

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| true | 获取远端设备名称成功 | 获取成功 |
| false | 获取远端设备名称失败 | 获取失败 |

### gap_get_device_class <a id="gap_get_device_class"></a>

```c
int gap_get_device_class(const bd_addr_t *addr)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_br_gap.h"
```

**功能说明**

- 获取远端设备的Class of Device(CoD)
- CoD用于标识远端设备的类型与支持的服务
- CoD由Major Service Class、Major Device Class、Minor Device Class组合构成

**前置条件**

- BT协议栈已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| addr | const [bd_addr_t](#struct_bd_addr_t) * | 远端设备地址 | 非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| CoD值(>=0) | 设备类型 | 查询成功返回设备类型 |
| -1 | 获取失败 | 查询失败 |

### gap_register_callbacks <a id="gap_register_callbacks"></a>

```c
int gap_register_callbacks(gap_call_backs_t *func)
```

**头文件清单**

```c
#include "include/middleware/services/bts/br/bts_br_gap.h"
```

**功能说明**

- 注册GAP回调函数，上层应用通过注册回调接收蓝牙状态变化通知
- 回调函数结构体 [gap_call_backs_t](#struct_gap_call_backs_t) 包含所有GAP事件的回调指针
- 注册的回调函数将在对应事件发生时被协议栈调用

**前置条件**

- BT协议栈已使能

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| func | [gap_call_backs_t](#struct_gap_call_backs_t) * | 指向回调函数结构体的指针 | 非NULL |

**返回值**

- 返回类型：int

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_BT_SUCCESS(0x00) | 执行成功 | 注册成功 |
| Other | 其他错误码，参考 [errcode_bt_t](#enum_errcode_bt_t) | 执行失败 |

## Type definitions

### gap_state_changed_callback <a id="typedef_gap_state_changed_callback"></a>

```c
typedef void (*gap_state_changed_callback)(const int transport, const int status);
```

**使用说明**

蓝牙开关状态变化回调，在 [gap_register_callbacks](#gap_register_callbacks) 注册的 [gap_call_backs_t](#struct_gap_call_backs_t) 中使用；transport参数参考 [bt_transport_type_t](#enum_bt_transport_type_t)，status参数参考 [bt_stack_state_t](#enum_bt_stack_state_t)

### gap_acl_state_changed_callback <a id="typedef_gap_acl_state_changed_callback"></a>

```c
typedef void (*gap_acl_state_changed_callback)(const bd_addr_t *bd_addr, gap_acl_state_t state, unsigned int reason);
```

**使用说明**

ACL连接状态变化回调，在 [gap_register_callbacks](#gap_register_callbacks) 注册的 [gap_call_backs_t](#struct_gap_call_backs_t) 中使用；state参数参考 [gap_acl_state_t](#enum_gap_acl_state_t)，reason参数参考标准HCI (Host Controller Interface) 错误码

### gap_discovery_state_changed_callback <a id="typedef_gap_discovery_state_changed_callback"></a>

```c
typedef void (*gap_discovery_state_changed_callback)(int status);
```

**使用说明**

蓝牙发现状态变化回调，在 [gap_register_callbacks](#gap_register_callbacks) 注册的 [gap_call_backs_t](#struct_gap_call_backs_t) 中使用；status参数参考 [gap_discovery_state_t](#enum_gap_discovery_state_t)

### gap_discovery_result_callback <a id="typedef_gap_discovery_result_callback"></a>

```c
typedef void (*gap_discovery_result_callback)(const bd_addr_t *bd_addr);
```

**使用说明**

蓝牙发现结果回调，在 [gap_register_callbacks](#gap_register_callbacks) 注册的 [gap_call_backs_t](#struct_gap_call_backs_t) 中使用；bd_addr为所发现的远端设备地址

### gap_pair_requested_callback <a id="typedef_gap_pair_requested_callback"></a>

```c
typedef void (*gap_pair_requested_callback)(const bd_addr_t *bd_addr);
```

**使用说明**

蓝牙配对请求回调，在 [gap_register_callbacks](#gap_register_callbacks) 注册的 [gap_call_backs_t](#struct_gap_call_backs_t) 中使用；bd_addr为远端设备地址

### gap_pair_confirmed_callback <a id="typedef_gap_pair_confirmed_callback"></a>

```c
typedef void (*gap_pair_confirmed_callback)(const bd_addr_t *bd_addr, int req_type, int number);
```

**使用说明**

蓝牙配对确认回调，在 [gap_register_callbacks](#gap_register_callbacks) 注册的 [gap_call_backs_t](#struct_gap_call_backs_t) 中使用；req_type参考 [gap_pair_confirm_type_t](#enum_gap_pair_confirm_type_t)，number为配对密码

### gap_scan_mode_changed_callback <a id="typedef_gap_scan_mode_changed_callback"></a>

```c
typedef void (*gap_scan_mode_changed_callback)(int mode);
```

**使用说明**

蓝牙广播模式变化回调，在 [gap_register_callbacks](#gap_register_callbacks) 注册的 [gap_call_backs_t](#struct_gap_call_backs_t) 中使用；mode为广播模式

### gap_local_device_name_changed_callback <a id="typedef_gap_local_device_name_changed_callback"></a>

```c
typedef void (*gap_local_device_name_changed_callback)(const unsigned char *device_name, unsigned char length);
```

**使用说明**

本地设备名称变化回调，在 [gap_register_callbacks](#gap_register_callbacks) 注册的 [gap_call_backs_t](#struct_gap_call_backs_t) 中使用；device_name为本地设备名称，length为名称长度

### gap_local_device_addr_changed_callback <a id="typedef_gap_local_device_addr_changed_callback"></a>

```c
typedef void (*gap_local_device_addr_changed_callback)(const bd_addr_t *bd_addr);
```

**使用说明**

本地设备地址变化回调，在 [gap_register_callbacks](#gap_register_callbacks) 注册的 [gap_call_backs_t](#struct_gap_call_backs_t) 中使用；bd_addr为本地设备地址

### gap_pair_status_changed_callback <a id="typedef_gap_pair_status_changed_callback"></a>

```c
typedef void (*gap_pair_status_changed_callback)(const bd_addr_t *bd_addr, int status);
```

**使用说明**

蓝牙配对状态变化回调，在 [gap_register_callbacks](#gap_register_callbacks) 注册的 [gap_call_backs_t](#struct_gap_call_backs_t) 中使用；status参考 [gap_pair_state_t](#enum_gap_pair_state_t)

### gap_remote_uuid_changed_callback <a id="typedef_gap_remote_uuid_changed_callback"></a>

```c
typedef void (*gap_remote_uuid_changed_callback)(const bd_addr_t *bd_addr, bt_uuid_t uuid);
```

**使用说明**

远端设备UUID (Universally Unique Identifier) 变化回调，在 [gap_register_callbacks](#gap_register_callbacks) 注册的 [gap_call_backs_t](#struct_gap_call_backs_t) 中使用；uuid为远端设备UUID

### gap_remote_name_changed_callback <a id="typedef_gap_remote_name_changed_callback"></a>

```c
typedef void (*gap_remote_name_changed_callback)(
    const bd_addr_t *bd_addr, const unsigned char *device_name, unsigned char length);
```

**使用说明**

远端设备名称变化回调，在 [gap_register_callbacks](#gap_register_callbacks) 注册的 [gap_call_backs_t](#struct_gap_call_backs_t) 中使用；device_name为远端设备名称，length为名称长度

### gap_remote_alias_changed_callback <a id="typedef_gap_remote_alias_changed_callback"></a>

```c
typedef void (*gap_remote_alias_changed_callback)(const bd_addr_t *bd_addr, const unsigned char *alias,
    unsigned char length);
```

**使用说明**

远端设备昵称变化回调，在 [gap_register_callbacks](#gap_register_callbacks) 注册的 [gap_call_backs_t](#struct_gap_call_backs_t) 中使用；alias为远端设备昵称，length为昵称长度

### gap_remote_cod_changed_callback <a id="typedef_gap_remote_cod_changed_callback"></a>

```c
typedef void (*gap_remote_cod_changed_callback)(const bd_addr_t *bd_addr, int cod);
```

**使用说明**

远端设备类型变化回调，在 [gap_register_callbacks](#gap_register_callbacks) 注册的 [gap_call_backs_t](#struct_gap_call_backs_t) 中使用；cod为设备类型

### gap_remote_battery_level_changed_callback <a id="typedef_gap_remote_battery_level_changed_callback"></a>

```c
typedef void (*gap_remote_battery_level_changed_callback)(const bd_addr_t *bd_addr, int battery_level);
```

**使用说明**

远端设备电量变化回调，在 [gap_register_callbacks](#gap_register_callbacks) 注册的 [gap_call_backs_t](#struct_gap_call_backs_t) 中使用；battery_level为远端设备电量

### gap_read_remote_rssi_event_callback <a id="typedef_gap_read_remote_rssi_event_callback"></a>

```c
typedef void (*gap_read_remote_rssi_event_callback)(const bd_addr_t *bd_addr, int rssi, int status);
```

**使用说明**

远端设备信号强度变化回调，在 [gap_register_callbacks](#gap_register_callbacks) 注册的 [gap_call_backs_t](#struct_gap_call_backs_t) 中使用；rssi为信号强度，status为BT状态

### gap_is_accept_conn_on_safe_mode_callback <a id="typedef_gap_is_accept_conn_on_safe_mode_callback"></a>

```c
typedef void (*gap_is_accept_conn_on_safe_mode_callback)(const bd_addr_t *bd_addr, bool *res);
```

**使用说明**

询问上层应用是否接受连接回调，在 [gap_register_callbacks](#gap_register_callbacks) 注册的 [gap_call_backs_t](#struct_gap_call_backs_t) 中使用；bd_addr为远端设备地址，res为是否接受连接的决策结果

## Enumerations

### bt_stack_state_t <a id="enum_bt_stack_state_t"></a>

```c
typedef enum {
    BT_STACK_STATE_TURNING_ON = 0x0,
    BT_STACK_STATE_TURN_ON,
    BT_STACK_STATE_TURNING_OFF,
    BT_STACK_STATE_TURN_OFF
} bt_stack_state_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| BT_STACK_STATE_TURNING_ON | 0x0 | 蓝牙协议栈正在初始化中 |
| BT_STACK_STATE_TURN_ON | 0x1 | 蓝牙协议栈正在运行中 |
| BT_STACK_STATE_TURNING_OFF | 0x2 | 蓝牙协议栈正在去初始化中 |
| BT_STACK_STATE_TURN_OFF | 0x3 | 蓝牙协议栈已关闭 |

### bt_connect_strategy_type_t <a id="enum_bt_connect_strategy_type_t"></a>

```c
typedef enum {
    BT_CONNECTION_UNKNOWN = 0x00,
    BT_CONNECTION_ALLOWED,
    BT_CONNECTION_FORBIDDEN
} bt_connect_strategy_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| BT_CONNECTION_UNKNOWN | 0x00 | 未知连接 |
| BT_CONNECTION_ALLOWED | 0x01 | 允许连接 |
| BT_CONNECTION_FORBIDDEN | 0x02 | 禁止连接 |

### a2dp_playing_state_t <a id="enum_a2dp_playing_state_t"></a>

```c
typedef enum {
    A2DP_NOT_PLAYING = 0x00,
    A2DP_IS_PLAYING
} a2dp_playing_state_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| A2DP_NOT_PLAYING | 0x00 | a2dp未播放 |
| A2DP_IS_PLAYING | 0x01 | a2dp正在播放 |

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

### bt_transport_type_t <a id="enum_bt_transport_type_t"></a>

```c
typedef enum {
    BT_TRANSPORT_INVALID = 0x00,
    BT_TRANSPORT_BR_EDR = 0x01,
    BT_TRANSPORT_LE = 0x02
} bt_transport_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| BT_TRANSPORT_INVALID | 0x00 | 未知传输类型 |
| BT_TRANSPORT_BR_EDR | 0x01 | 传输类型为BR/EDR |
| BT_TRANSPORT_LE | 0x02 | 传输类型为BLE |

### gap_acl_state_t <a id="enum_gap_acl_state_t"></a>

```c
typedef enum {
    GAP_ACL_STATE_CONNECTED,
    GAP_ACL_STATE_DISCONNECTED,
    GAP_ACL_STATE_LE_CONNECTED,
    GAP_ACL_STATE_LE_DISCONNECTED
} gap_acl_state_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GAP_ACL_STATE_CONNECTED | 0 | GAP ACL已连接 |
| GAP_ACL_STATE_DISCONNECTED | 1 | GAP ACL已断接 |
| GAP_ACL_STATE_LE_CONNECTED | 2 | BLE GAP ACL已连接 |
| GAP_ACL_STATE_LE_DISCONNECTED | 3 | BLE GAP ACL已断接 |

### gap_discovery_state_t <a id="enum_gap_discovery_state_t"></a>

```c
typedef enum {
    GAP_DISCOVERY_STARTED = 0x01,
    GAP_DISCOVERYING = 0x02,
    GAP_DISCOVERY_STOPED = 0x03,
} gap_discovery_state_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GAP_DISCOVERY_STARTED | 0x01 | 已开始扫描 |
| GAP_DISCOVERYING | 0x02 | 正在扫描 |
| GAP_DISCOVERY_STOPED | 0x03 | 扫描已停止 |

### gap_bondable_mode_t <a id="enum_gap_bondable_mode_t"></a>

```c
typedef enum {
    GAP_BONDABLE_MODE_OFF = 0x00,
    GAP_BONDABLE_MODE_ON = 0x01
} gap_bondable_mode_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GAP_BONDABLE_MODE_OFF | 0x00 | 关闭蓝牙绑定模式 |
| GAP_BONDABLE_MODE_ON | 0x01 | 打开蓝牙绑定模式 |

### gap_access_permission_type_t <a id="enum_gap_access_permission_type_t"></a>

```c
typedef enum {
    GAP_ACCESS_PERMISSION_UNKNOWN,
    GAP_ACCESS_PERMISSION_ALLOWED,
    GAP_ACCESS_PERMISSION_FORBIDDEN,
} gap_access_permission_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GAP_ACCESS_PERMISSION_UNKNOWN | 0 | 未知 |
| GAP_ACCESS_PERMISSION_ALLOWED | 1 | 允许访问 |
| GAP_ACCESS_PERMISSION_FORBIDDEN | 2 | 禁止访问 |

### gap_scan_mode_t <a id="enum_gap_scan_mode_t"></a>

```c
typedef enum {
    GAP_SCAN_MODE_NONE = 0x00,
    GAP_SCAN_MODE_CONNECTABLE,
    GAP_SCAN_MODE_GENERAL_DISCOVERABLE,
    GAP_SCAN_MODE_LIMITED_DISCOVERABLE,
    GAP_SCAN_MODE_CONNECTABLE_GENERAL_DISCOVERABLE,
    GAP_SCAN_MODE_CONNECTABLE_LIMITED_DISCOVERABLE
} gap_scan_mode_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GAP_SCAN_MODE_NONE | 0x00 | 未知扫描类型 |
| GAP_SCAN_MODE_CONNECTABLE | 0x01 | 可连接扫描类型 |
| GAP_SCAN_MODE_GENERAL_DISCOVERABLE | 0x02 | 通用广播扫描类型 |
| GAP_SCAN_MODE_LIMITED_DISCOVERABLE | 0x03 | 受限广播扫描类型 |
| GAP_SCAN_MODE_CONNECTABLE_GENERAL_DISCOVERABLE | 0x04 | 可连接的通用广播扫描类型 |
| GAP_SCAN_MODE_CONNECTABLE_LIMITED_DISCOVERABLE | 0x05 | 可连接的受限广播扫描类型 |

### bt_profile_type_t <a id="enum_bt_profile_type_t"></a>

```c
typedef enum {
    BT_PROFILE_HEADSET = 0x0,
    BT_PROFILE_A2DP,
    BT_PROFILE_OPP,
    BT_PROFILE_HID,
    BT_PROFILE_PANU,
    BT_PROFILE_NAP,
    BT_PROFILE_HFP
} bt_profile_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| BT_PROFILE_HEADSET | 0x0 | Headset服务类型 |
| BT_PROFILE_A2DP | 0x1 | A2DP (Advanced Audio Distribution Profile) 服务类型 |
| BT_PROFILE_OPP | 0x2 | OPP服务类型 |
| BT_PROFILE_HID | 0x3 | HID (Human Interface Device) 服务类型 |
| BT_PROFILE_PANU | 0x4 | PANU服务类型 |
| BT_PROFILE_NAP | 0x5 | NAP服务类型 |
| BT_PROFILE_HFP | 0x6 | HFP (Hands-Free Profile) 服务类型 |

### gap_pair_state_t <a id="enum_gap_pair_state_t"></a>

```c
typedef enum {
    GAP_PAIR_NONE = 0x01,
    GAP_PAIR_PAIRING = 0x02,
    GAP_PAIR_PAIRED = 0x03
} gap_pair_state_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GAP_PAIR_NONE | 0x01 | 未配对状态 |
| GAP_PAIR_PAIRING | 0x02 | 正在配对 |
| GAP_PAIR_PAIRED | 0x03 | 已完成配对 |

### gap_pair_confirm_type_t <a id="enum_gap_pair_confirm_type_t"></a>

```c
typedef enum {
    GAP_PAIR_CONFIRM_TYPE_PIN_CODE = 0x01,
    GAP_PAIR_CONFIRM_TYPE_PASSKEY_DISPLAY = 0x02,
    GAP_PAIR_CONFIRM_TYPE_PASSKEY_INPUT = 0x03,
    GAP_PAIR_CONFIRM_TYPE_NUMERIC = 0x04,
    GAP_PAIR_CONFIRM_TYPE_CONSENT = 0x05,
} gap_pair_confirm_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| GAP_PAIR_CONFIRM_TYPE_PIN_CODE | 0x01 | 通过pin code配对 |
| GAP_PAIR_CONFIRM_TYPE_PASSKEY_DISPLAY | 0x02 | 展示PASSKEY |
| GAP_PAIR_CONFIRM_TYPE_PASSKEY_INPUT | 0x03 | 输入PASSKEY |
| GAP_PAIR_CONFIRM_TYPE_NUMERIC | 0x04 | 配对数值 |
| GAP_PAIR_CONFIRM_TYPE_CONSENT | 0x05 | 请求用户同意 |

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

## Structures

### gap_paired_device_info_t <a id="struct_gap_paired_device_info_t"></a>

```c
typedef struct {
    bd_addr_t addr;
    unsigned char device_name[BD_NAME_LEN];
} gap_paired_device_info_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| addr | [bd_addr_t](#struct_bd_addr_t) | 蓝牙地址 |
| device_name | unsigned char[BD_NAME_LEN] | 蓝牙设备名称，UTF-8编码，BD_NAME_LEN=32 |

### gap_call_backs_t <a id="struct_gap_call_backs_t"></a>

```c
typedef struct {
    gap_state_changed_callback state_change_callback;
    gap_acl_state_changed_callback acl_state_changed_callbak;
    gap_scan_mode_changed_callback scan_mode_changed_callback;
    gap_pair_status_changed_callback pair_status_changed_callback;
    gap_local_device_name_changed_callback device_name_changed_callback;
    gap_local_device_addr_changed_callback device_addr_changed_callback;
    gap_pair_requested_callback pair_requested_callback;
    gap_pair_confirmed_callback pair_confiremed_callback;
    gap_discovery_state_changed_callback discovery_state_changed_callback;
    gap_discovery_result_callback discovery_result_callback;
    gap_remote_uuid_changed_callback remote_uuid_changed_callback;
    gap_remote_name_changed_callback remote_name_changed_callback;
    gap_remote_alias_changed_callback remote_alias_changed_callback;
    gap_remote_cod_changed_callback remote_cod_changed_callback;
    gap_remote_battery_level_changed_callback remote_battery_level_changed_callback;
    gap_read_remote_rssi_event_callback read_remote_rssi_event_callback;
    gap_is_accept_conn_on_safe_mode_callback is_accept_conn_on_safe_mode_callback;
} gap_call_backs_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| state_change_callback | [gap_state_changed_callback](#typedef_gap_state_changed_callback) | 蓝牙开关状态变化回调 |
| acl_state_changed_callbak | [gap_acl_state_changed_callback](#typedef_gap_acl_state_changed_callback) | ACL连接状态变化回调 |
| scan_mode_changed_callback | [gap_scan_mode_changed_callback](#typedef_gap_scan_mode_changed_callback) | 广播模式变化回调 |
| pair_status_changed_callback | [gap_pair_status_changed_callback](#typedef_gap_pair_status_changed_callback) | 配对状态变化回调 |
| device_name_changed_callback | [gap_local_device_name_changed_callback](#typedef_gap_local_device_name_changed_callback) | 本地设备名称变化回调 |
| device_addr_changed_callback | [gap_local_device_addr_changed_callback](#typedef_gap_local_device_addr_changed_callback) | 本地设备地址变化回调 |
| pair_requested_callback | [gap_pair_requested_callback](#typedef_gap_pair_requested_callback) | 配对请求回调 |
| pair_confiremed_callback | [gap_pair_confirmed_callback](#typedef_gap_pair_confirmed_callback) | 配对确认回调 |
| discovery_state_changed_callback | [gap_discovery_state_changed_callback](#typedef_gap_discovery_state_changed_callback) | 发现状态变化回调 |
| discovery_result_callback | [gap_discovery_result_callback](#typedef_gap_discovery_result_callback) | 发现结果回调 |
| remote_uuid_changed_callback | [gap_remote_uuid_changed_callback](#typedef_gap_remote_uuid_changed_callback) | 远端设备UUID变化回调 |
| remote_name_changed_callback | [gap_remote_name_changed_callback](#typedef_gap_remote_name_changed_callback) | 远端设备名称变化回调 |
| remote_alias_changed_callback | [gap_remote_alias_changed_callback](#typedef_gap_remote_alias_changed_callback) | 远端设备昵称变化回调 |
| remote_cod_changed_callback | [gap_remote_cod_changed_callback](#typedef_gap_remote_cod_changed_callback) | 远端设备类型变化回调 |
| remote_battery_level_changed_callback | [gap_remote_battery_level_changed_callback](#typedef_gap_remote_battery_level_changed_callback) | 远端设备电量变化回调 |
| read_remote_rssi_event_callback | [gap_read_remote_rssi_event_callback](#typedef_gap_read_remote_rssi_event_callback) | 远端设备信号强度变化回调 |
| is_accept_conn_on_safe_mode_callback | [gap_is_accept_conn_on_safe_mode_callback](#typedef_gap_is_accept_conn_on_safe_mode_callback) | 询问是否接受连接回调 |

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
| addr | uint8_t[BD_ADDR_LEN] | 蓝牙地址，BD_ADDR_LEN=6 |
| type | uint8_t | 蓝牙地址类型，参考 [bt_addr_type](../pan/pan.md#enum_bt_addr_type) |



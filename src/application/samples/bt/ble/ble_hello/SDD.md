# BLE Hello 三功能样例 SDD

## 1. 状态

- 实现：完成。
- Server/Client clean build：通过。
- COM6 Server、COM8 Client 烧录：通过，JSON `success: true`。
- 冷启动与 Central 重启：通过。
- Peripheral 重启：通过；COM6/COM8 双端日志均显示默认态、`device_status_ok` 读取和最终写确认成功。
- 验证日期：2026-07-20。

## 2. 架构

| 模块 | 职责 |
| --- | --- |
| `Kconfig` | 定义互斥的 Server/Client 角色配置，由 BLE 外层 choice 通过 `osource` 引入 |
| `CMakeLists.txt` | 根据角色配置接入公共入口及对应角色源码 |
| `ble_hello.c` | `app_run()` 入口，创建优先级 26、栈 `0x2000` 的角色任务 |
| `ble_hello_server.c` | GAP/GATTS 回调、GATT 表、配对、通知、读写响应 |
| `ble_hello_server_adv.c` | 组装 AD、30 ms 广播、断连重广播及状态字节 |
| `ble_hello_client.c` | AD 解析、连接配对、MTU、发现、CCCD、Notify/Read/Write 状态机 |

Kconfig 角色：

- `CONFIG_SAMPLE_SUPPORT_BLE_HELLO_SERVER_SAMPLE`
- `CONFIG_SAMPLE_SUPPORT_BLE_HELLO_CLIENT_SAMPLE`

两项位于 BLE choice 中，与现有 BLE 样例互斥。入口、角色源文件、头文件均通过 CMake 实际接入。

## 3. GAP 设计

### 3.1 广播

| 参数 | 值 |
| --- | --- |
| 名称 | `ble_hello_server` |
| 类型 | 可连接非定向 |
| 间隔 | `0x30` = 48 × 0.625 ms = 30 ms |
| 信道 | `0x07`，37/38/39 |
| 时长 | 0，持续广播 |

AD 结构：

| Type | 内容 |
| --- | --- |
| `0x01` Flags | `0x06` |
| `0x03` Complete 16-bit UUID | `0x3333` |
| `0x09` Complete Local Name | `ble_hello_server` |
| `0x16` Service Data 16-bit | UUID `0x3333` + 状态字节 |

状态字节 `00` 表示 RAM 为 `device_status_ok`，`01` 表示保留的已写值。写入 Data 后更新状态，断连重广播时重新调用 `gap_ble_set_adv_data()`。

### 3.2 扫描、连接和安全

Client 使用 1M PHY、主动扫描，间隔和窗口均为 `0x30`。扫描结果严格按 AD Length/Type/Value 解析，必须同时找到精确 Complete Local Name 和 UUID `0x3333` 的 Service Data。

匹配后复制地址、停止扫描并调用 `gap_ble_connect_remote_device()`。两端通过 `gap_ble_set_sec_param()` 配置 bondable、NoInputNoOutput、Mode 1 Level 2；未配对时 Client 调用 `gap_ble_pair_remote_device()`。配对后交换 MTU 247。

## 4. GATT 设计

| 属性 | UUID | Properties | Permissions | 初始值 |
| --- | --- | --- | --- | --- |
| Primary Service | `0x3333` | - | - | - |
| Data | `0x3434` | READ、WRITE | READ、WRITE、AUTHORIZATION_NEED | `device_status_ok` |
| Hello Notify | `0x3435` | NOTIFY | READ | `hello world` |
| Hello CCCD | `0x2902` | - | READ、WRITE | `00 00` |

Data 缓冲区固定 32 字节。长度 1–31 合法；0 或不小于 32 返回 `GATT_STATUS_INVALID_ATTRIBUTE_VALUE_LENGTH`。未知 handle 返回 `GATT_STATUS_INVALID_HANDLE`。CCCD 只接受 `00 00` 或 `01 00`。

Server 使用：

1. `gatts_register_server()`
2. `gatts_add_service_sync()`
3. `gatts_add_characteristic_sync()`
4. `gatts_add_descriptor_sync()`
5. `gatts_start_service()`
6. `gatts_send_response()`
7. `gatts_notify_indicate()`

Client 发现 Service 和两条 Characteristic，再对 Hello Characteristic 调用 `gattc_discovery_descriptor()` 并按 UUID 保存实际 CCCD handle，不使用 `value_handle + 1`。

## 5. 状态机

```text
SCAN
  -> CONNECT / PAIR
  -> MTU 247
  -> DISCOVER SERVICE + DATA + HELLO + CCCD
  -> [广播状态为默认] WRITE device_status_ok（控制器缓存同步）
  -> WRITE CCCD 01 00
  -> NOTIFY hello world
  -> READ Data
  -> WRITE new_config_value
  -> COMPLETE
```

广播状态为 retained 时跳过缓存同步写，直接使能 CCCD。所有阶段由 GAP/GATTC/GATTS 回调推进，回调内不阻塞、不延时、不轮询。

## 6. WS63 平台约束

实机验证确认：`fbb monitor --reset` 按 `src/build/config/target_config/ws63/ws63.json` 发送 `AT+RST`，只重启应用核；BTH 核可保留属性值和旧广播数据。即使应用执行 BLE disable/enable，也不能把该行为等同于整芯片冷启动。

因此：

- Central 应用重启可使用 `--reset`；
- Peripheral 默认值恢复必须用硬件 RESET、断电重启或 `AT+REBOOT`；
- `AT+REBOOT` 在 SDK 中调用 `hal_reboot_chip()`；
- 默认态连接先用公开 `gattc_write_req()` 同步 Data 属性，再执行 Notify/Read，保证读取值与 Server RAM 一致。

## 7. 恢复和错误处理

| 场景 | 处理 |
| --- | --- |
| AD 越界、名称或状态字段不匹配 | 丢弃报告 |
| 连接请求失败 | 清 connecting 并重扫 |
| 配对失败/配对中断 | 删除陈旧配对并断连/重扫 |
| MTU/发现失败 | 记录状态，不推进后续阶段 |
| CCCD 未发现 | 不写 CCCD，记录错误 |
| Server 断连 | 清连接/CCCD并按当前 RAM 状态重广播 |
| Client 断连 | 清句柄与阶段标志并重扫描 |

远端数据日志全部使用 `%.*s` 和显式长度。

## 8. 内部接口

| 接口 | 说明 |
| --- | --- |
| `ble_hello_server_init()` | 注册 Server 回调并启用 BLE |
| `ble_hello_server_start_adv()` | 生成并启动广播，可用于断连恢复 |
| `ble_hello_server_set_adv_default_state()` | 更新下次广播的状态字节 |
| `ble_hello_server_send_notification()` | CCCD 开启后发送 Hello Notify |
| `ble_hello_client_init()` | 注册 Client 回调、启用 BLE并开始扫描 |

以上均为样例内部接口。

## 9. 验证矩阵

| 项目 | 证据 | 结果 |
| --- | --- | --- |
| Server clean build | 退出码 0；只启用 Server；Server/Adv 对象生成；fwpkg 更新 | 通过 |
| Client clean build | 退出码 0；只启用 Client；Client 对象生成；fwpkg 更新 | 通过 |
| 烧录 | COM6、COM8 最后一行 JSON `success: true`，各 7 分区 | 通过 |
| 整芯片冷启动 | Client：`state=device_status_ok`、Hello、Read 默认值、Write CFM；`matched` | 通过 |
| 通知 | Server CCCD enabled 后发送；Client 收到 `hello world` | 通过 |
| Read/Write | 冷启动读默认值，最终双方确认 `new_config_value` | 通过 |
| Central 重启 | 双端 JSON `matched`；Client `state=retained` 并读 `new_config_value` | 通过 |
| Peripheral 重启 | Server 重建服务、读 `device_status_ok`、最终写 `new_config_value`；Client 默认态日志匹配 | 通过 |
| 文档 | 三篇 basics 与源码一致；MkDocs build 成功 | 通过 |

COM6 在人工断电时会短暂消失，正在运行的 `fbb monitor` 不自动重连，因此冷启动以 COM8 完整业务日志为主要证据；Central 重启已取得双端完整 JSON。

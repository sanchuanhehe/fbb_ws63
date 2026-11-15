# BLE Hello 三功能样例

本样例用两块 WS63 开发板连续演示 BLE 广播/扫描/连接、`hello world` Notify，以及 Characteristic Read/Write。Server 与 Client 通过互斥 Kconfig 分别构建。

角色配置定义在本目录的 `Kconfig` 中；BLE 外层 Kconfig 仅通过 `osource` 将其纳入既有样例 choice，外层 CMake 也不依赖具体角色配置名。

## 数据模型

| 属性 | UUID | 属性/权限 | 初始值 |
| --- | --- | --- | --- |
| Primary Service | `0x3333` | Primary | - |
| Data Characteristic | `0x3434` | READ、WRITE；READ、WRITE、授权回调 | `device_status_ok` |
| Hello Characteristic | `0x3435` | NOTIFY | `hello world` |
| Hello CCCD | `0x2902` | READ、WRITE | `00 00` |

Data 缓冲区为 32 字节，只接受 1–31 字节写入。广播名称固定为 `ble_hello_server`；16-bit Service Data 的最后一字节表示 Server 当前状态：`00` 为默认值，`01` 为保留的已写值。

## 编译和烧录

在 SDK 根目录执行。以下示例使用 COM6 作为 Server、COM8 作为 Client。

Server：

```powershell
fbb config --target ws63-liteos-app set CONFIG_SAMPLE_ENABLE=y
fbb config --target ws63-liteos-app set CONFIG_ENABLE_BT_SAMPLE=y
fbb config --target ws63-liteos-app set CONFIG_SAMPLE_SUPPORT_BLE_SAMPLE=y
fbb config --target ws63-liteos-app set CONFIG_SAMPLE_SUPPORT_BLE_HELLO_SERVER_SAMPLE=y
fbb build ws63-liteos-app --clean
fbb flash ws63-liteos-app --port COM6 --json-summary
```

Server 烧录后立即构建 Client，避免同名 fwpkg 被覆盖：

```powershell
fbb config --target ws63-liteos-app set CONFIG_SAMPLE_SUPPORT_BLE_HELLO_CLIENT_SAMPLE=y
fbb build ws63-liteos-app --clean
fbb flash ws63-liteos-app --port COM8 --json-summary
```

烧录以最后一行 JSON 的 `"success": true` 为准。

## 运行判据

```powershell
fbb monitor --port COM6 --until "property updated: new_config_value" --timeout 60 --json-summary
fbb monitor --port COM8 --until "write cfm: success" --timeout 60 --json-summary
```

首次整芯片冷启动的 Client 关键日志：

```text
[ble hello client] found ble_hello_server, state=device_status_ok, connecting
[ble hello client] connected, conn_id=...
[ble hello client] pair complete, status=0x0
[ble hello client] MTU changed: 247, status=0x0
[ble hello client] data characteristic discovered, value=...
[ble hello client] notify characteristic discovered, value=...
[ble hello client] hello CCCD discovered, handle=...
[ble hello client] syncing data attribute: device_status_ok
[ble hello client] cache sync write success
[ble hello client] enabling hello CCCD
[ble hello client] Received: hello world
[ble hello client] read result: device_status_ok
[ble hello client] write request sent: new_config_value
[ble hello client] write cfm: success
```

Server 关键日志：

```text
[ble hello server] advertising started: ble_hello_server
[ble hello server] connected, conn_id=...
[ble hello server] hello CCCD enabled
[ble hello server] notification sent: hello world
[ble hello server] read response sent: value=device_status_ok
[ble hello server] property updated: new_config_value
[ble hello server] write response sent: success
```

## 重启验证

- 只用 `fbb monitor --reset` 重启 Client：Server 断连后重广播，Client 应扫描到 `state=retained` 并读取 `new_config_value`。
- 重启 Server：必须按硬件 RESET、断电重启或发送 `AT+REBOOT`。Client 应扫描到 `state=device_status_ok` 并读取默认值。
- `fbb monitor --reset` 实际发送 `AT+RST`，只重启应用核；WS63 BTH 核的 GATT/广播缓存可能保留，因此不能用它证明 Peripheral 整芯片冷启动。

## 内部接口

- `ble_hello_server_init()`
- `ble_hello_server_start_adv()`
- `ble_hello_server_set_adv_default_state()`
- `ble_hello_server_send_notification()`
- `ble_hello_client_init()`

这些接口仅属于样例，不新增 SDK 公共 API。教学 UUID 仅用于对照；产品应使用合规分配或自定义 128-bit UUID。

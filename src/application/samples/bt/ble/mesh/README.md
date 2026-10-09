# BLE Mesh 双板配网案例

> 使用技术：BLE Mesh、Provisioner、Device、PB-ADV/PB-GATT、AppKey、Generic OnOff

本案例使用两块 WS63 开发板验证 BLE Mesh 基础配网流程：一块板烧录 `mesh_device`，作为未配网节点；另一块板烧录 `provisioner_adv`，作为配网器扫描并完成配网。

## 案例目标

| 开发板 | Sample | 角色 | 功能 |
|--------|--------|------|------|
| 板 A | `mesh_device` | Device | 发送未配网 Beacon，等待 Provisioner 配网，配网后作为 OnOff/LED 节点 |
| 板 B | `provisioner_adv` | Provisioner | 扫描未配网节点，分配节点地址，完成配网并添加 AppKey |

## 准备工作

- 准备两块 WS63 开发板和两根 USB 数据线。
- 记录两个串口，例如板 A 为 `COM4`，板 B 为 `COM5`。
- 使用目标 `ws63-liteos-app-mesh`。
- 本案例需要通过串口 AT 命令启动 Mesh 和指定待配网设备，因此目标编译宏需要包含 `BTH_BLE_MESH_AT_ENABLE`。

## BLE Mesh AT 编译宏

`BTH_BLE_MESH_AT_ENABLE` 不是 menuconfig 选项，需要在 target 配置中打开。修改文件：

```text
src/build/config/target_config/ws63/config.py
```

在 `ws63-liteos-app-mesh` target 的 `defines` 列表中加入 `BTH_BLE_MESH_AT_ENABLE`，建议放在 `BLE_MESH_SUPPORT` 附近：

```python
'ws63-liteos-app-mesh': {
    ...
    'defines': [
        ...
        'BGLE_TASK_EXIST',
        'BLE_MESH_SUPPORT',
        'BTH_BLE_MESH_AT_ENABLE',
        'BTH_SLE_MESH_AT_ENABLE',
        'BTH_TASK_EXIST',
        ...
    ],
}
```

## 配置并烧录 Device

在工程根目录打开 menuconfig：

```bash
fbb menuconfig ws63-liteos-app-mesh
```

按下面路径选择 Device sample，保存并退出：

```text
Top
  → Application
    → Enable Sample
      → Enable the Sample of BT
        → Sample
          → Mesh
            → BLE mesh Sample
              → Support Ble Mesh device sample.
```

然后编译并烧录到板 A：

```bash
fbb build --clean ws63-liteos-app-mesh
fbb flash ws63-liteos-app-mesh --port COM4 --json-summary
```

烧录完成后打开板 A 串口，先确认 AT 通路：

```text
AT
```

返回 `OK` 后按验证目的二选一。

### 方案一：临时启动，两块板都不写 tuple

板 A 执行：

```text
AT+BMCFGLOCAL=ble_mesh_enable,3
```

板 B 烧录 Provisioner 后也执行临时启动：

```text
AT
AT+BMCFGLOCAL=ble_mesh_enable,3
```

等板 B 日志打印扫描地址，再执行：

```text
AT+BMPROVNET=1,<扫描地址去掉冒号>
```

### 方案二：两块板都写本地 tuple，不再临时启动

板 A 执行：

```text
AT+BMPROV=uuid_set,uuid,00112233445566778899AABBCCDDEEFF
AT+RST
```

方案二的目标流程是板 A、板 B 都写各自的本地 tuple，然后复位；复位后两块板都应自动从 NV 恢复，不再把 `AT+BMCFGLOCAL=ble_mesh_enable,3` 作为启动步骤。

板 B Provisioner 也应该有自己的本地 tuple。目标命令形式如下：

```text
AT+BMPROV=uuid_set,uuid,<板B自己的32位hex uuid>
AT+RST
```

板 B 自动启动并扫描到板 A 后，只需要指定目标地址：

```text
AT
AT+BMPROVNET=1,<扫描地址去掉冒号>
```

预期日志包含：

```text
ble device sample init start
mesh_initialized success
set unprov beacon flag: 1
ble device mesh_sample_init end
```

如果首次启动出现 `nv recover tuple info failed`，说明 NV 中还没有对应 tuple。临时验证走方案一；固定设备参数走方案二。

## 配置并烧录 Provisioner

重新打开 menuconfig：

```bash
fbb menuconfig ws63-liteos-app-mesh
```

在同一位置切换为 Provisioner sample，保存并退出：

```text
Top
  → Application
    → Enable Sample
      → Enable the Sample of BT
        → Sample
          → Mesh
            → BLE mesh Sample
              → Support Ble Mesh prov sample.
```

然后编译并烧录到板 B：

```bash
fbb build --clean ws63-liteos-app-mesh
fbb flash ws63-liteos-app-mesh --port COM5 --json-summary
```

打开板 B 串口后观察 Provisioner 启动日志：

```text
AT
```

预期日志包含：

```text
ble mesh provisioner init start
mesh_initialized success
ble onoff client mesh_sample_init end
```

如果当前样例因为空 NV 只打印 `nv recover tuple info failed`，没有继续到 `mesh_initialized success`，再手动拉起一次：

```text
AT+BMCFGLOCAL=ble_mesh_enable,3
```

## 发起配网

Provisioner 会扫描未配网 Device。若日志中出现类似下面的信息，说明已经扫到 Device，但还没有指定目标地址：

```text
Not as expected, scan addr: 81:d6:f0:a5:24:00
```

在板 B 串口指定目标设备地址，地址去掉冒号：

```text
AT+BMPROVNET=1,81d6f0a52400
```

配网成功时，板 B 预期日志包含：

```text
recv_unprovisioned_beacon dev uuid
node_added uuid:... addr:0x0002 num_elem:1
provisioner_complete: net_idx=0x0000, addr=0x0002
link_close
```

板 A 预期日志包含：

```text
provisioning complete for net_idx 0x0000 addr 0x0002
```

## 结果判定

| 检查项 | 成功现象 |
|--------|----------|
| Device 启动 | 串口打印 `mesh_initialized success`，并设置未配网 Beacon |
| Provisioner 启动 | 串口打印 `ble onoff client mesh_sample_init end` |
| 扫描发现 | Provisioner 能打印 Device 的 UUID 或扫描地址 |
| 配网完成 | Provisioner 打印 `node_added` 和 `provisioner_complete` |
| 节点地址 | Device 被分配到 `0x0002` 或后续递增地址 |

## 常见问题

| 现象 | 原因 | 处理方法 |
|------|------|----------|
| `AT+BMCFGLOCAL`、`AT+BMPROVNET` 或 `AT+BMSTAT` 返回 `ERROR` | BLE Mesh AT 命令表未注册，根因通常是目标编译宏未包含 `BTH_BLE_MESH_AT_ENABLE` | 确认 `ws63-liteos-app-mesh` 目标启用了 BLE Mesh AT 注册宏后重新编译 |
| Device 上电后没有自动发 Beacon | 首次启动 NV 中没有 tuple | 正式验证执行 `AT+BMPROV=uuid_set,uuid,<32位hex uuid>` 写入 UUID 后复位；临时调试可执行 `AT+BMCFGLOCAL=ble_mesh_enable,3` |
| Provisioner 扫到地址但不配网 | 没有指定期望配网地址 | 执行 `AT+BMPROVNET=1,<addr>` |

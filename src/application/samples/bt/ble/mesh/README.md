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

如果没有这个宏，`main.c` 不会调用 `mesh_at_register()` 注册 BLE Mesh AT 命令表，`AT+BMCFGLOCAL`、`AT+BMPROVNET`、`AT+BMSTAT` 会返回 `ERROR`。

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

返回 `OK` 后手动启动 BLE Mesh Device。推荐使用 `3`，同时打开 PB-ADV 和 PB-GATT：

```text
AT+BMCFGLOCAL=ble_mesh_enable,3
```

预期日志包含：

```text
ble device sample init start
mesh_initialized success
set unprov beacon flag: 1
ble device mesh_sample_init end
```

如果首次启动出现 `nv recover tuple info failed`，属于 NV 中没有历史配网 tuple 的情况。`AT+BMCFGLOCAL=ble_mesh_enable,3` 不是“开启 NV”，而是手动调用 sample 的 `ble_mesh_sample_enable()` 继续启动 Mesh 协议栈；该路径读取 NV 失败后只打印错误，并继续使用本机 BLE 地址生成 UUID，随后调用 `ble_mesh_enable()`。

首次空 NV 时有两种处理方式：

| 流程 | 命令 | 作用 | 是否写 tuple 到 NV |
|---|---|---|---|
| A：临时启动 | `AT+BMCFGLOCAL=ble_mesh_enable,3` | 当前这次继续启动 BLE Mesh，进入可配网状态 | 不写 |
| B：写 tuple 后启动 | `AT+BMPROV=uuid_set,uuid,<uuid>`、`AT+BMPROV=auth_set,pid,<pid>,auth,<auth>`，然后执行 `AT+BMCFGLOCAL=ble_mesh_enable,3` 或重启 | 先把 UUID/auth/地址 tuple 写入 NV，再启动或下次上电自动启动 | 写 |

流程 A 适合首次快速验证；重启后如果仍没有 tuple，自动初始化还会再次读 NV 失败。流程 B 适合固定设备参数，写入后不要随意重新设置，否则 Provisioner 或手机侧需要重新按新的 UUID/地址扫描设备。

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

打开板 B 串口后启动 Provisioner：

```text
AT
AT+BMCFGLOCAL=ble_mesh_enable,3
```

预期日志包含：

```text
ble mesh provisioner init start
mesh_initialized success
ble onoff client mesh_sample_init end
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
| Device 上电后没有自动发 Beacon | 首次启动 NV 中没有 tuple | 手动执行 `AT+BMCFGLOCAL=ble_mesh_enable,3` |
| Provisioner 扫到地址但不配网 | 没有指定期望配网地址 | 执行 `AT+BMPROVNET=1,<addr>` |

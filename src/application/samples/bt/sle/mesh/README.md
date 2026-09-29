# SLE Mesh 一键组网灯控案例

> 使用技术：SLE Mesh、MA、MF、NetId、组播、属性上报、灯控

本案例使用两块 WS63 开发板验证 SLE Mesh 一键组网：一块板烧录 MA 角色，作为管理和控制侧；另一块板烧录 MF 角色，作为转发和被控侧。组网成功后，MA 周期性发送灯控属性，MF 接收后打印 `light value 0/1`。

## 案例目标

| 开发板 | Sample | 角色 | 功能 |
|--------|--------|------|------|
| 板 A | `one_touch_start` | MA | Mesh Administration，管理/控制侧，创建网络并发送灯控属性 |
| 板 B | `one_touch_start` | MF | Mesh relay Forwarding，转发/被控侧，加入同一网络并响应灯控 |

## 准备工作

- 准备两块 WS63 开发板和两根 USB 数据线。
- 记录两个串口，例如板 A 为 `COM4`，板 B 为 `COM5`。
- 使用目标 `ws63-liteos-app-mesh`。
- MA 和 MF 需要使用相同的 `NetId`。当前 sample 中默认 `NetId` 为 `2`。

SLE Mesh one touch start 不需要像 BLE Mesh 一样通过 AT 手动启动。sample 启动后会直接调用 `sle_enter_net_sample_init()`；如果没有从 NV 恢复到网络参数，会使用源码中配置的默认 MA/MF 地址、角色和 `NetId` 调用 `sle_mesh_start()`。

## 配置并烧录 MA

在工程根目录打开 menuconfig：

```bash
fbb menuconfig ws63-liteos-app-mesh
```

按下面路径选择 MA 角色并打开灯控，保存并退出：

```text
Top
  → Application
    → Enable Sample
      → Enable the Sample of BT
        → Sample
          → Mesh
            → Support Sle Mesh one touch start sample.
              → Support SLE Mesh Demo
              → Support SLE Mesh One Touch Start Ma
              → Support SLE Mesh Light Ctrl
```

确认没有同时选择 `Support SLE Mesh One Touch Start Mf`。然后编译并烧录到板 A：

```bash
fbb build --clean ws63-liteos-app-mesh
fbb flash ws63-liteos-app-mesh --port COM4 --json-summary
```

MA 启动后预期日志包含：

```text
[Demo] this device run as Mesh MA.
```

打开灯控后，MA 会周期性发送组播属性：

```text
send multicast msg, group id(0xffff) property(0301) change to 1
send multicast msg, group id(0xffff) property(0301) change to 0
```

## 配置并烧录 MF

重新打开 menuconfig：

```bash
fbb menuconfig ws63-liteos-app-mesh
```

切换为 MF 角色，保存并退出：

```text
Top
  → Application
    → Enable Sample
      → Enable the Sample of BT
        → Sample
          → Mesh
            → Support Sle Mesh one touch start sample.
              → Support SLE Mesh Demo
              → Support SLE Mesh One Touch Start Mf
```

确认没有同时选择 `Support SLE Mesh One Touch Start Ma`；MF 侧基础验证不需要打开 `Support SLE Mesh Light Ctrl`。然后编译并烧录到板 B：

```bash
fbb build --clean ws63-liteos-app-mesh
fbb flash ws63-liteos-app-mesh --port COM5 --json-summary
```

MF 启动后预期日志包含：

```text
[Demo] this device run as Mesh MF.
```

组网成功并收到 MA 灯控属性后，MF 预期日志包含：

```text
group_instance_app_cbk
property(0301) change to 1
[Demo] light value 1
property(0301) change to 0
[Demo] light value 0
```

## 结果判定

| 检查项 | 成功现象 |
|--------|----------|
| MA 启动 | 打印 `this device run as Mesh MA` |
| MF 启动 | 打印 `this device run as Mesh MF` |
| Mesh 组网 | 两侧出现心跳或邻居消息日志 |
| 灯控发送 | MA 周期性打印 `send multicast msg` |
| 灯控接收 | MF 周期性打印 `light value 1/0` |

## 配置说明

| 配置项 | 说明 |
|--------|------|
| `CONFIG_SAMPLE_SUPPORT_SLE_MESH_ONE_TOUCH_START` | 打开 SLE Mesh one touch start sample |
| `CONFIG_SLE_MESH_DEMO_SUPPORT` | 打开 demo 逻辑 |
| `CONFIG_SLE_MESH_DEV_MA_ROLE` | 编译 MA 角色固件 |
| `CONFIG_SLE_MESH_DEV_MF_ROLE` | 编译 MF 角色固件 |
| `CONFIG_SLE_MESH_SUPPORT_LIGHT_CTRL` | 打开 MA 侧灯控发送逻辑 |

## 常见问题

| 现象 | 原因 | 处理方法 |
|------|------|----------|
| 两块板都打印同一角色 | MA/MF 配置没有切换干净 | 重新确认 `CONFIG_SLE_MESH_DEV_MA_ROLE` 和 `CONFIG_SLE_MESH_DEV_MF_ROLE` 互斥 |
| MF 没有 `light value` | MA 未打开灯控或两侧未加入同一网络 | 确认 MA 打开 `CONFIG_SLE_MESH_SUPPORT_LIGHT_CTRL`，并确认两侧 `NetId` 一致 |
| 只有启动日志，没有后续消息 | 另一块板未上电或固件角色不匹配 | 同时打开两侧串口，确认 MA/MF 都已运行 |

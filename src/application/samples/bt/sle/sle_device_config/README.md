# 参数配置与持久化

本样例使用两块 WS63 演示通过 SSAP 属性读写配置参数并保存到 NV：Client 读取和写入配置，Server 校验数据并保存合法值、拒绝非法值，复位后再从 NV 恢复。示例字段只用于验证配置链路，不驱动实际业务。

> 建议先完成 [sle_hello](../sle_hello/) 的连接和属性读写实验。本样例不需要传感器、Wi-Fi 或云端账号。

## 功能与角色

| 角色 | 功能 | 成功判据 |
|------|------|----------|
| Server | 以 `config_server` 广播，校验参数并写入 NV ID `0x20A1` | 输出 `config saved`；非法值输出 `status=0xf` |
| Client | 扫描、连接、配对、发现属性，自动读取和写入 Server | 输出 `valid config accepted`，随后回读值一致 |

Client 自动执行以下流程：

```text
读取 Server 当前参数
  → 写入合法配置 500 ms / 75.0 ℃ / mode 1
  → 收到成功 Write Confirmation
  → 写后回读
  → 写入非法配置 interval=50 ms
```

非法配置的失败状态在本次实测中只由 Server 端日志稳定呈现，不能把 Client 没有继续输出日志误判为测试失败或成功。真正的 NV 持久化还需要复位 Server 后观察加载日志。

## 公共配置结构体

Client 和 Server 共用 `sle_device_config_protocol.h` 中的 `sle_device_config_t`：

```c
typedef struct {
    uint16_t magic;
    uint16_t report_interval_ms;
    int16_t alarm_threshold_decicelsius;
    uint8_t mode;
    uint8_t version;
} sle_device_config_t;
```

当前结构体共 8 字节，各字段约束如下：

| 字段 | 类型 | 约束和说明 |
|------|------|------------|
| `magic` | `uint16_t` | 固定为 `0x5343` |
| `report_interval_ms` | `uint16_t` | 100～60000 ms |
| `alarm_threshold_decicelsius` | `int16_t` | -200～1000，单位 0.1 ℃ |
| `mode` | `uint8_t` | 0 为正常模式，1 为低功耗模式 |
| `version` | `uint8_t` | 当前为 1 |

本样例的两端都是相同工具链构建的 WS63，直接传输 `sle_device_config_t`。迁移到不同处理器或编译器时，应改为显式的逐字节编码和解码，不能依赖结构体布局与本机字节序。

## Kconfig UI 配置

在 SDK 根目录打开 `ws63-liteos-app` 的 Kconfig UI，依次进入：

```text
Application
  → Enable Sample.
    → Enable the Sample of BT.
      → Sample
        → Support SLE Sample.
          → SLE Sample
```

构建 Server 时选择：

```text
Support SLE Device Config Server Sample.
```

构建 Client 时改选：

```text
Support SLE Device Config Client Sample.
```

两个选项位于同一个 `choice`，一次构建只能选择一个角色。`CONFIG_SUPPORT_SLE_PERIPHERAL` 或 `CONFIG_SUPPORT_SLE_CENTRAL` 会由角色自动派生，不需要手工选择。

## 构建与烧录

先选择 Server，保存 Kconfig UI 后构建并烧录：

```powershell
fbb build --clean ws63-liteos-app
fbb flash ws63-liteos-app --port <server_port> --baud 2000000 --json-summary
```

再通过 Kconfig UI 切换为 Client，重新构建并烧录：

```powershell
fbb build --clean ws63-liteos-app
fbb flash ws63-liteos-app --port <client_port> --baud 2000000 --json-summary
```

必须在切换为 Client 前完成 Server 烧录，因为两个角色共用同一个构建输出。端口以实际枚举结果为准。

## 运行结果

先启动 Server，再启动 Client。Client 输出：

```text
[sle device config client] write valid config: interval=500, threshold=750, mode=1
[sle device config client] valid config accepted, handle=0x11
[sle device config client] read config: interval=500, threshold=750, mode=1
[sle device config client] persisted config verified
[sle device config client] write invalid config: interval=50
```

Server 输出：

```text
[sle device config server] config saved: interval=500, threshold=750, mode=1
[sle device config server] rejected: interval=50, threshold=750, mode=1
[sle device config server] write response status=0xf
```

复位 Server 后输出：

```text
[sle device config server] config loaded from NV: interval=500, threshold=750, mode=1
```

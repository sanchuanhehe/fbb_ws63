# 分片传输

本样例使用两块 WS63 演示 SLE 应用层分片传输：Server 将 1024 字节测试数据拆成 6 个 Notification，Client 请求传输、按序重组，并校验总长度和校验和。

> 建议先完成 [sle_hello](../sle_hello/) 的连接、属性读写和通知实验。

## 功能

```text
Client 扫描并连接 fragment_server
  → 配对、MTU 交换和属性发现
  → Write 0x01 请求传输
  → Server 发送 6 个分片
  → Client 重组 1024 字节并校验
```

## 公共分片结构体

Client 和 Server 共用 `sle_fragmentation_protocol.h` 中的 `sle_fragmentation_packet_t`：

```c
typedef struct {
    uint16_t magic;
    uint16_t transfer_id;
    uint16_t index;
    uint16_t total;
    uint16_t payload_len;
    uint16_t reserved;
    uint32_t checksum;
    uint8_t payload[SLE_FRAGMENTATION_PAYLOAD_SIZE];
} sle_fragmentation_packet_t;
```

协议头为 16 字节，字段约束如下：

| 字段 | 类型 | 约束和说明 |
|------|------|------------|
| `magic` | `uint16_t` | 固定为 `0x5346` |
| `transfer_id` | `uint16_t` | 固定为 1 |
| `index` | `uint16_t` | 从 0 开始递增 |
| `total` | `uint16_t` | 固定为 6 |
| `payload_len` | `uint16_t` | 当前负载长度，不超过 180 |
| `reserved` | `uint16_t` | 当前为 0 |
| `checksum` | `uint32_t` | 完整数据的累加和 |
| `payload` | `uint8_t[180]` | 当前分片负载 |

测试数据的第 `i` 个字节为 `i & 0xff`：

```text
1024 = 180 × 5 + 124
checksum = 130560
```

## 配置与烧录

打开`ws63-liteos-app`的Kconfig UI，进入：

```text
Application
  → Enable Sample.
    → Enable the Sample of BT.
      → Sample
        → Support SLE Sample.
          → SLE Sample
```

选择 Server：

```text
Support SLE Fragmentation Server Sample.
```

保存配置，然后构建并烧录：

```powershell
fbb build --clean ws63-liteos-app
fbb flash ws63-liteos-app --port <server_port> --baud 2000000 --json-summary
```

在同一个`SLE Sample`菜单中改选 Client：

```text
Support SLE Fragmentation Client Sample.
```

保存配置，然后构建并烧录：

```powershell
fbb build --clean ws63-liteos-app
fbb flash ws63-liteos-app --port <client_port> --baud 2000000 --json-summary
```

## 运行结果

先启动 Server，再启动 Client。Server 输出：

```text
[sle fragmentation server] transfer start: bytes=1024, fragments=6, checksum=130560
[sle fragmentation server] fragment sent: 1/6, payload=180
...
[sle fragmentation server] fragment sent: 6/6, payload=124
[sle fragmentation server] transfer complete
```

Client 输出：

```text
[sle fragmentation client] fragment received: 1/6, total_bytes=180
...
[sle fragmentation client] fragment received: 6/6, total_bytes=1024
[sle fragmentation client] reassembly complete: bytes=1024, checksum=130560
[sle fragmentation client] test passed
```

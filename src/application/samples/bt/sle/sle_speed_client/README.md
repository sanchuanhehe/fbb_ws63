# SLE 高吞吐传输 Client

本样例与 `sle_speed_server` 配合，自动扫描固定地址的 Server，完成连接、配对、MTU 交换和服务发现，并统计持续 Notification 的应用层吞吐量。

## 功能

```text
Client 扫描并连接 Server
  → 配对、交换 MTU 和发现服务
  → 发起属性操作并触发 Server 发送
  → 连续接收 1200 字节 Notification
  → 每 1000 包计算一次耗时和吞吐量
```

## 配置与烧录

打开 `ws63-liteos-app` 的 Kconfig UI，进入：

```text
Application
  → Enable Sample.
    → Enable the Sample of BT.
      → Sample
        → Support SLE Sample.
          → SLE Sample
```

选择：

```text
Support SLE Throughput Client Sample.
```

保存配置，然后构建并烧录：

```powershell
fbb build --clean ws63-liteos-app
fbb flash ws63-liteos-app --port <client_port> --baud 2000000 --json-summary
```

## 运行结果

先启动 Server，再启动 Client。Client 输出：

```text
[Connected]
[ssap client] exchange mtu, mtu size: 1500, version: 1.
g_count_after_get_us = 9745118, g_count_before_get_us = 7063025, data_len = 1200
time = 2.68 s
speed = 3579294.40 bps
```

吞吐量数值会随板间距离、射频环境和系统负载变化。

## 关键参数

| 参数 | 当前值 | 说明 |
|------|--------|------|
| `RECV_PKT_CNT` | 1000 | 每 1000 包输出一次统计结果 |
| `SPEED_DEFAULT_CONN_INTERVAL` | `0x14` | 高吞吐连接间隔配置 |
| `RSSI_AVG_COUNT` | 10 | 每 10 包发起一次 RSSI 读取 |

# SLE 高吞吐传输 Server

本样例与 `sle_speed_client` 配合，使用 SLE Notification 持续发送 1200 字节测试数据。Server 配置 1500 字节 MTU 和链路数据长度、4M PHY、MCS10，并根据链路流控状态提交数据。

## 功能

```text
Server 广播固定地址
  → Client 连接、配对和交换 MTU
  → Client 发起 Read Request
  → Server 创建发送任务并配置高速链路
  → Server 持续发送 1200 字节 Notification
  → Client 每 1000 包统计一次吞吐量
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
Support SLE Throughput Server Sample.
```

保存配置，然后构建并烧录：

```powershell
fbb build --clean ws63-liteos-app
fbb flash ws63-liteos-app --port <server_port> --baud 2000000 --json-summary
```

## 运行结果

Server 初始化成功后输出：

```text
[speed server] init ok
sle enable end.
```

配套 Client 完成连接和 MTU 交换后，会触发 Server 的连续发送任务。

## 关键参数

| 参数 | 当前值 | 说明 |
|------|--------|------|
| `PKT_DATA_LEN` | 1200 字节 | 单个 Notification 的应用数据长度 |
| `DEFAULT_SLE_SPEED_DATA_LEN` | 1500 字节 | 链路数据长度请求值 |
| `DEFAULT_SLE_SPEED_MTU_SIZE` | 1500 字节 | SSAP MTU 请求值 |
| `DEFAULT_SLE_SPEED_MCS` | 10 | 高吞吐调制编码档位 |
| PHY | 4M | 适合近距离、低干扰环境 |

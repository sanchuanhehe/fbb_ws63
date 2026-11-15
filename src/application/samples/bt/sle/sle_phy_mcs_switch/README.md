# SLE PHY/MCS 动态切换

## 功能简介

本案例使用两块 WS63 开发板演示基于 RSSI 的链路自适应：

- Server 广播 `sle_phy_mcs_server`，连接后周期读取对端 RSSI。
- Server 对 4 次 RSSI 采样求平均，并要求连续 2 个窗口满足条件才切换。
- PHY 更新成功回调到达后，再调用 `sle_set_mcs()` 设置对应 MCS。
- Client 扫描、连接 Server，并输出对端发起的 PHY 更新结果。
- 断连后双方自动恢复广播或扫描。

## 档位与迟滞

| 档位 | PHY | MCS | 进入条件 |
|---|---:|---:|---|
| robust | 1M | 0 | 初始档；balanced 下 RSSI ≤ -78 dBm |
| balanced | 2M | 4 | robust 下 RSSI ≥ -70 dBm；fast 下 RSSI ≤ -62 dBm |
| fast | 4M | 10 | balanced 下 RSSI ≥ -50 dBm |

升档和降档使用不同阈值，形成迟滞区，避免 RSSI 在边界附近波动时频繁切换。上述阈值只用于演示，产品应根据天线、结构、发射功率和目标环境重新标定。

## 关键 API

- `sle_read_remote_device_rssi()`：异步读取对端 RSSI。
- `sle_set_phy_param()`：异步更新收发 PHY。
- `set_phy_cb`：确认 PHY 最终更新结果。
- `sle_set_mcs()`：在 PHY 更新成功后设置匹配的 MCS。

## 构建

Server：

```shell
fbb config set CONFIG_ENABLE_BT_SAMPLE=y --target ws63-liteos-app
fbb config set CONFIG_SAMPLE_SUPPORT_SLE_SAMPLE=y --target ws63-liteos-app
fbb config set CONFIG_SAMPLE_SUPPORT_SLE_PHY_MCS_SWITCH_SERVER_SAMPLE=y --target ws63-liteos-app
fbb build ws63-liteos-app --clean
```

Client：

```shell
fbb config set CONFIG_SAMPLE_SUPPORT_SLE_PHY_MCS_SWITCH_CLIENT_SAMPLE=y --target ws63-liteos-app
fbb build ws63-liteos-app --clean
```

## 验证日志

近距离条件下，Server 从稳健档逐级升到高速档：

```text
[sle phy mcs server] switch complete: profile=robust, phy=1M, mcs=0, status=0x0
[sle phy mcs server] RSSI window: average=-39 dBm, current=robust, selected=balanced
[sle phy mcs server] switch complete: profile=balanced, phy=2M, mcs=4, status=0x0
[sle phy mcs server] RSSI window: average=-41 dBm, current=balanced, selected=fast
[sle phy mcs server] switch complete: profile=fast, phy=4M, mcs=10, status=0x0
```

Client 同步观察到 PHY 更新：

```text
[sle phy mcs client] PHY changed: conn_id=0x00, status=0x0, tx_phy=1M, rx_phy=1M
[sle phy mcs client] PHY changed: conn_id=0x00, status=0x0, tx_phy=2M, rx_phy=2M
[sle phy mcs client] PHY changed: conn_id=0x00, status=0x0, tx_phy=4M, rx_phy=4M
```

增加距离、遮挡或可控衰减后，可验证 fast→balanced→robust 的降档过程。

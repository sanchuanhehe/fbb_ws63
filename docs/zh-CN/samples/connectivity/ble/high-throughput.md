# 高吞吐传输

> BLE (Bluetooth Low Energy) GAP (Generic Access Profile) PHY/Data Length、GATT (Generic Attribute Profile) Notify

> 前置阅读：[Hello Notify](basics/hello-notify.md)、[连接参数调优](conn-param-tuning.md)

## 学习目标

- 理解影响 BLE 吞吐量的关键因素：PHY (Physical Layer) 速率、Connection Interval、Data Length、MTU (Maximum Transmission Unit)
- 掌握 2M PHY + 251 字节 Data Length 的高吞吐配置
- 理解 BLE 与 SLE (SparkLink Low Energy) 在高吞吐场景下的差距
- 能够配置并验证 BLE 的最大有效吞吐量

## 基本概念

### BLE 吞吐量的四个决定因素

| 因素 | 最大值 | 影响 |
|------|:---:|------|
| PHY | 2M | 物理层速率上限 |
| Connection Interval | 7.5ms（最短） | 调度频率，间隔越短时隙越多 |
| Data Length (DLE (Data Length Extension)) | 251 字节 | 单包有效载荷 |
| MTU | 517 字节 | GATT 层单次传输上限 |

### BLE vs SLE 高吞吐对比

| 对比项 | BLE | SLE |
|--------|:---:|:---:|
| PHY 上限 | 2Mbps | 4Mbps |
| 有效吞吐 | ~800Kbps | ~3Mbps |
| 单包载荷 | 251B | 更大 |
| 适用场景 | 中小数据、手机兼容 | 大数据、SLE 专有 |

### Data Length Extension (DLE)

BLE 4.2+ 支持 DLE，单包从 27 字节扩展到 251 字节——协议开销占比大幅降低。

## 涉及 API

| API | 谁调用 | 用途 |
|-----|--------|------|
| `gap_ble_set_phy()` | 双方 | 切换到 2M PHY |
| `gap_ble_set_data_length()` | 双方 | 设置单包最大数据长度 |
| `gap_ble_connect_param_update()` | Peripheral | 请求缩短连接间隔 |
| `gatts_notify_indicate()` | Peripheral | 高速发送数据包 |
| `gattc_exchange_mtu_req()` | Central | 请求更大的 MTU |

## 案例说明

### 案例简介

Peripheral 使用 2M PHY + 7.5ms 连接间隔 + 251 字节 DLE + 517 字节 MTU，向 Central 高速发送数据。

| 模式 | PHY | Interval | Data Length | 吞吐 |
|------|:---:|:---:|:---:|:---:|
| 普通 | 1M | 30ms | 27B | ~50Kbps |
| 高吞吐 | 2M | 7.5ms | 251B | ~800Kbps |

## 关键配置

| 参数 | 推荐值 | 说明 |
|------|:---:|------|
| PHY | 2M | 必需，1M 吞吐砍半 |
| Connection Interval | 7.5ms | 最短合法值 |
| Data Length | 251B | DLE 最大值 |
| MTU | 517B | GATT 最大值 |

## 代码详解

```c
/* 高吞吐配置三步曲 */
gap_ble_set_phy(conn_id, GAP_BLE_PHY_2M);
gap_ble_set_data_length(conn_id, 251, 500);  // tx=251, rx_time=500μs
gap_ble_connect_param_update(conn_id, 6, 6, 0, 500);  // 6×1.25ms=7.5ms

/* 高速发送循环 */
while (sending) {
    int ret = gatts_notify_indicate(conn_id, attr_handle,
                                     data_len, data, false);
    if (ret == ERRCODE_BUSY) {
        osal_task_yield();  // 流控满了，让出 CPU 等待
    }
}

/* Central 端吞吐统计：500 包窗口，total_bytes / elapsed_ms */
```

---


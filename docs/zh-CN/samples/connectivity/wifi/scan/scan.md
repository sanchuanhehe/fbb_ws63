# 扫描

> WiFi Scan API

## 学习目标

- 理解主动扫描和被动扫描的区别
- 掌握扫描参数配置和结果解析
- 能够在 WS63 上扫描周边 WiFi 热点

## 规格与功能

WS63 扫描周边 WiFi，串口打印每个热点的 SSID (Service Set Identifier)、RSSI (Received Signal Strength Indicator)、加密方式、信道。

| 规格项 | 说明 |
|--------|------|
| 扫描模式 | 主动扫描 |
| 扫描信道 | 全信道（1~13） |
| 结果内容 | SSID / BSSID (Basic Service Set Identifier) / RSSI / 信道 / 加密方式 |

## 基本概念

### 主动扫描 vs 被动扫描

| | 主动扫描 | 被动扫描 |
|:---|:---|:---|
| 方式 | STA (Station) 发送 Probe Request | STA 听 Beacon |
| 速度 | 快（主动问） | 慢（等 AP (Access Point) 自己说） |
| 功耗 | 较高 | 较低 |
| 结果 | 更完整（SSID 即使隐藏也能扫到？不能，但可探测存在） | 仅广播 SSID 的 AP |

主动扫描是最常用的模式。

### 扫描结果含义

| 字段 | 含义 | 示例 |
|------|------|------|
| SSID | 热点名称 | "MyRouter" |
| BSSID | AP 的 MAC (Media Access Control) 地址 | AA:BB:CC:DD:EE:FF |
| RSSI | 信号强度 | -45 dBm（越近 0 越强） |
| Channel | 信道 | 6（2.4GHz） |
| Security | 加密方式 | WPA2-PSK / Open |

## 涉及 API

| API | 用途 |
|-----|------|
| `wifi_sta_scan()` | 启动扫描 |
| `wifi_scan_stop()` | 停止扫描 |
| `wifi_event_register()` | 注册事件回调（扫描结果） |

## 案例操作指导


编译烧录后串口打印周边 WiFi 列表：

```text
[WiFi Scan] starting...
[WiFi Scan] SSID: MyRouter       RSSI: -45  Ch: 6   WPA2
[WiFi Scan] SSID: NeighborWiFi   RSSI: -72  Ch: 11  WPA2
[WiFi Scan] SSID: FreeWiFi       RSSI: -80  Ch: 1   Open
[WiFi Scan] done, 3 APs found
```

## 关键配置

- 扫描模式：主动扫描
- 扫描信道：全信道（1~13）
- 扫描结果按 RSSI 降序展示（信号最强的排最前）

## 代码详解

### 启动扫描

```c
wifi_scan_config_t scan_config = {
    .scan_type = WIFI_SCAN_ACTIVE,  // 主动扫描
    .channels  = 0,                 // 0 = 全信道
    .timeout   = 5000,              // 5 秒超时
};
wifi_sta_scan(&scan_config);
```

### 扫描结果回调

```c
static void wifi_sta_get_scan_info(wifi_scan_result_t *result)
{
    printf("[WiFi Scan] SSID: %-20s RSSI: %-4d Ch: %-3d %s\n",
           result->ssid,
           result->rssi,
           result->channel,
           result->security == WIFI_SEC_OPEN ? "Open" : "WPA2");
}

static void wifi_scan_done_cb(void)
{
    printf("[WiFi Scan] done, %d APs found\n", g_ap_count);
}
```

### 扫描结果筛选

```c
// 按 RSSI 过滤弱信号
if (result->rssi < -80) {
    return;  // 忽略信号太弱的 AP
}

// 按 SSID 过滤特定热点
if (strstr(result->ssid, "Target") == NULL) {
    return;  // 不是目标热点
}
```

---


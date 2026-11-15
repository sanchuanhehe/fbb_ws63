# 省电模式

> WiFi Power Save API

> 前置阅读：[STA (Station) 连接](../sta/sta-connect.md)

## 学习目标

- 理解 WiFi 省电模式的原理——DTIM (Delivery Traffic Indication Message) 间隔控制唤醒频率
- 掌握省电模式的启用/禁用
- 理解省电模式对功耗和延迟的 trade-off

## 规格与功能

STA 连接路由器后启用省电模式，降低功耗。对于电池供电的 IoT 设备，省电模式是必备功能。

## 基本概念

### DTIM 和工作原理

路由器（AP (Access Point)）每 100ms 发送一个 Beacon，其中每 N 个 Beacon（DTIM 间隔）携带广播/组播数据指示。STA 在 DTIM Beacon 时唤醒接收数据，其他 Beacon 时休眠。

```text
DTIM = 1:  B B B B B B B B B B   每个都醒（~100ms 延迟，功耗高）
DTIM = 3:  B . . B . . B . . B   每 3 个醒一次（~300ms 延迟，功耗 1/3）
DTIM = 10: B . . . . . . . . B   每 10 个醒一次（~1s 延迟，功耗 1/10）
```

> DTIM 间隔由**路由器决定**，STA 无法修改。但可以让 STA 在每个 DTIM 时不醒来——省电模式就是这个开关。

### 功耗 vs 延迟

| 模式 | 唤醒频率 | 延迟 | 功耗 | 适用 |
|------|:---:|:---:|:---:|------|
| Active | 始终在线 | < 1ms | ~50mA | 实时通信 |
| Power Save | DTIM 间隔 | 100ms~1s | ~5mA | 传感器定时上报 |

## 涉及 API

| API | 用途 |
|-----|------|
| `wifi_sta_set_pm(enabled)` | 启用/禁用省电模式 |
| `wifi_get_power_save_mode()` | 查询当前状态 |

## 案例操作指导


STA 连接成功后启用省电模式，观察延迟变化。

## 关键配置

- 省电模式：通过单一 API 开关控制
- 启用时机：STA 连接成功后
- 禁用时机：需要实时通信时（如 OTA (Over-The-Air) 升级）

## 代码详解

### 启用省电模式

```c
// STA 连接成功后 → 启用省电
static void wifi_event_cb(wifi_event_t event, void *data)
{
    if (event == WIFI_EVENT_STA_DHCP_SUCCESS) {
        wifi_sta_set_pm(true);  // 启用省电
        printf("[WiFi] power save enabled\n");
    }
}
```

### 动态切换

```c
// 常规上报期间 → 省电
wifi_sta_set_pm(true);

// OTA 升级期间 → 关闭省电（需要低延迟）
wifi_sta_set_pm(false);

// 升级完成 → 恢复省电
wifi_sta_set_pm(true);
```

---


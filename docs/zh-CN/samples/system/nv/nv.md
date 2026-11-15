# NV 持久化存储

> 使用技术：NV (Non-Volatile) 存储 API

## 学习目标

- 理解 NV 存储的用途——保存配网信息、配对密钥、配置参数、校准数据等掉电不丢失的数据
- 掌握 `uapi_nv_write()` / `uapi_nv_read()` 的基本操作
- 理解 NV 区域划分和容量限制
- 能够在 WS63 上实现配置参数的持久化存储

## 基本概念

### NV 存储的定位

Flash 中划出专用区域用于 Key-Value 存储。

| 对比项 | NV | LittleFS |
|--------|:---:|:---:|
| 数据模型 | Key-Value | 文件 |
| 适合数据 | 少量结构化数据（几十B~几KB） | 大量文件型数据（日志/配置） |
| 写入方式 | 按 ID 读写 | 按文件路径读写 |

### NV 区域划分

| 区域 | 说明 | 访问 |
|------|------|:---:|
| Factory | 出厂校准数据（如 TCXO (Temperature Compensated Crystal Oscillator) 值） | 只读 |
| Keep | 系统配置（MAC (Media Access Control) 地址等） | 系统保留 |
| User | 应用数据 | 可读写 |

### 写入寿命

Flash 写入次数有限（通常 10 万次）。频繁变化的参数（如运行时间计数器）不应直接写 NV——推荐定时批量写入。

## 涉及 API

| API | 用途 |
|-----|------|
| `uapi_nv_write(nv_id, data, len)` | 写入 NV 数据 |
| `uapi_nv_read(nv_id, data, &len)` | 读取 NV 数据 |
| `uapi_nv_delete(nv_id)` | 删除 NV 数据 |

## 案例说明

### 案例简介

上电时从 NV 读取已保存的配置（上报间隔/告警阈值），收到新配置后写入 NV——断电重上电后自动恢复上次配置。

## 关键配置

| 参数 | 推荐值 | 说明 |
|------|:---:|------|
| NV ID 范围 | 0x1000~0x1FFF | 应用层使用，避免与系统预留冲突 |
| 写入时机 | 配置变化后立即写入 | 避免高频写入（磨损 Flash） |

## 代码详解

```c
#define NV_ID_REPORT_INTERVAL  0x1000
#define NV_ID_ALARM_THRESHOLD  0x1001

/* 写入配置 */
uint32_t interval = 60;  // 上报间隔 60 秒
uapi_nv_write(NV_ID_REPORT_INTERVAL, &interval, sizeof(interval));

float threshold = 40.5;  // 告警阈值
uapi_nv_write(NV_ID_ALARM_THRESHOLD, &threshold, sizeof(threshold));

/* 读取配置 */
uint32_t saved_interval;
uint16_t len = sizeof(saved_interval);
uapi_nv_read(NV_ID_REPORT_INTERVAL, &saved_interval, &len);
printf("interval: %u s\n", saved_interval);
```

---


---
hide:
  - toc
---

# 星闪低功耗

SLE (SparkLink Low Energy) 是星闪低功耗接入技术的协议栈 API，提供高速、低延迟的短距无线通信能力，支持 1M/2M/4M 带宽，最高空口速率 12 Mbps。

## 连接管理

- [Connection](connection/connection.md) — 设备发现、连接建立与参数配置

## 数据交互

- [SSAP (SLE Service Access Protocol) Client](ssap/client/client.md) — 主动读写、订阅通知
- [SSAP Server](ssap/server/server.md) — 注册服务、响应读写

## 服务

- [Factory](service/factory/factory.md) — 工厂模式，创建和管理 SLE 实例
- [GLP (Generic Layer Protocol)](service/glp/glp.md) — 通用低功耗服务
- [HADM (High Accuracy Distance Measurement)](service/hadm/hadm.md) — 高精度测距
- [Low Latency](service/low_latency/low_latency.md) — 低延迟传输（125Hz~8KHz）
- [OTA (Over-The-Air)](service/ota/ota.md) — 星闪 OTA 升级

## 组网

- [CHBA (Converged Host Bus Adapter)](chba/chba.md) — 全屋智能 CHBA  IP (Internet Protocol) 组网

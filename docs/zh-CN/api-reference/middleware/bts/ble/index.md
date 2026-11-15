---
hide:
  - toc
---

# 低功耗蓝牙

BLE (Bluetooth Low Energy) 是 WS63 的低功耗蓝牙协议栈 API，基于 GATT (Generic Attribute Profile) 架构实现设备发现、连接管理和数据传输。

## 通用访问

- [GAP (Generic Access Profile)](gap/gap.md) — 设备发现、连接、配对、广播

## 数据交互

- [GATT Client](gatt/client/client.md) — 主动读写、订阅通知
- [GATT Server](gatt/server/server.md) — 注册服务、响应读写

## 工厂模式

- [Factory](factory/factory.md) — 创建和管理 BLE 协议栈实例

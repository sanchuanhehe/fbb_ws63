---
hide:
  - toc
---

# Wi-Fi

WS63 Wi-Fi 协议栈 API，支持 802.11 b/g/n/ax，覆盖 STA (Station) 站点、SoftAP 热点、P2P (Peer-to-Peer) 直连等模式。

## 核心功能

- [Hotspot](hotspot/hotspot.md) — STA 连接、SoftAP 创建、扫描
- [Device](device/device.md) — 设备管理，初始化、MAC (Media Access Control) 地址、省电模式
- [P2P](p2p/p2p.md) — Wi-Fi Direct 直连

## 底层与算法

- [Platform](platform/platform.md) — 平台层，底层硬件配置
- [ALG (Application Layer Gateway)](alg/alg.md) — 算法，自动调频、抗干扰、温度保护

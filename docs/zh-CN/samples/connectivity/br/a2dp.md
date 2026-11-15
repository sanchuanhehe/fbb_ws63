# A2DP

> BR/EDR A2DP (Advanced Audio Distribution Profile) Source、AVRCP (Audio/Video Remote Control Profile) Target、I2S (Inter-IC Sound)

## 学习目标

- 理解 A2DP 工作原理——Source 编码音频 → Sink 解码播放
- 掌握 WS63 作为 A2DP Source 连接蓝牙音箱/耳机的完整流程
- 理解编码协商过程（SBC/AAC）和音频参数
- 能够在 WS63 上实现音频推流到蓝牙音箱

## 基本概念

### A2DP 角色模型

| 角色 | 说明 | 设备 |
|------|------|------|
| Source (SRC) | 编码并发送音频 | WS63 |
| Sink (SNK) | 接收并解码播放 | 蓝牙音箱/耳机 |

### 编码协商

Source 和 Sink 连接后自动交换支持的编码格式列表 → 按优先级选择双方都支持的格式。SBC (Sub-Band Coding) 为必选、AAC (Advanced Audio Coding) 为可选。

### 音频数据流

```text
I2S RX → PCM Buffer → SBC Encoder → A2DP TX → Speaker
```

## 涉及 API

| API | 谁调用 | 用途 |
|-----|--------|------|
| `enable_bt_stack()` | Source | 使能经典蓝牙协议栈 |
| `bluetooth_set_local_name()` | Source | 设置蓝牙设备名 |
| `a2dp_src_connect()` | Source | 连接 A2DP Sink |
| `a2dp_src_start_playing()` | Source | 开始播放 |
| `a2dp_src_suspend_playing()` | Source | 暂停 |
| `a2dp_src_register_callbacks()` | Source | 注册回调 |
| `avrcp_tg_connect()` | Source | 接收遥控指令 |

## 案例说明

### 案例简介

WS63 通过 I2S 接收音频 PCM → SBC 编码 → A2DP 推送到蓝牙音箱，同时响应 AVRCP 遥控。

## 关键配置

| 参数 | 推荐值 | 说明 |
|------|:---:|------|
| Codec | SBC | 必选，兼容所有音箱 |
| 采样率 | 44100Hz / 48000Hz | CD 音质 |
| 位深 | 16-bit | SBC 标准 |
| SBC Bitpool | 53 | 高质量（范围 2~53） |

## 代码详解

```c
/* BT Stack 初始化 */
enable_bt_stack();
bluetooth_set_local_name("WS63-Audio", 9);

/* A2DP 连接 */
a2dp_src_connect(&bd_addr);
/* 等 connect_state_changed_cb → 已连接 */
/* 等 configuration_changed_cb → codec 协商完成 */

/* 开始播放 */
a2dp_src_start_playing(&bd_addr);
/* 音频帧由 I2S 驱动 → SBC 编码 → A2DP 自动发送 */

/* AVRCP 遥控 */
avrcp_tg_connect(&bd_addr);
/* avrcp_cmd_cb 中处理 播放/暂停/切歌/音量 */
```

---


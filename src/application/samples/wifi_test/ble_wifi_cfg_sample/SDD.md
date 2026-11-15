# BLE 配网软件方案设计文档 (SDD)

**文档版本：** V1.1  
**适用范围：** WS63 系列芯片（WS63 / WS63E）  
**目标平台：** Huawei LiteOS (v208.5.0)，RISC-V rv32imc  
**配网方式：** BLE GATT（手机 → WS63 单向写入 WiFi 凭证）  
**实现状态：** ✅ 已实现（代码路径 `src/application/samples/wifi_test/ble_wifi_cfg_sample/`）

---

## 1. 概述

### 1.1 背景

WS63 是面向 IoT 场景的嵌入式 Wi-Fi SoC，设备端通常没有键盘/屏幕等输入外设。因此需要借助手机等外部设备，通过近场通信将目标路由器的 SSID 和密码"告诉"WS63，使其完成 Wi-Fi 入网。

### 1.2 方案选型依据

| 维度 | BLE | SLE (星闪) | SoftAP |
|------|-----|------------|--------|
| 手机通用性 | 所有智能手机 | 仅星闪手机 | 所有智能手机 |
| 标准成熟度 | BLE 4.2/5.0 全覆盖 | 星闪 1.0 生态初期 | WiFi 标准 |
| SDK 完整度 | ✅ 完整 GATT 服务实现 | Demo 级 | 仅有 AP 示例 |
| 用户体验 | App 一键配网 | App 一键配网 | 需手动切换热点 |
| **结论** | **✅ 推荐** | 待生态成熟 | 体验差 |

**选型结论：采用 BLE GATT 配网方案**，手机端使用通用 BLE 调试工具（nRF Connect / BLE 调试助手）作为配网 App。

### 1.3 参考资料

| 文档 | 路径 |
|------|------|
| BLE WiFi Config Server 源码 | `src/application/samples/bt/ble/ble_wifi_cfg_server/` |
| BLE 配网完整流程示例 | `src/application/samples/wifi_test/ble_wifi_cfg_sample/` |
| WS63 错误码定义 | `src/include/errcode.h` |
| Wi-Fi STA API | `src/middleware/services/wifi_service/` |
| 软件开发指南 | `docs/zh-CN/guides/sdk-development/index.md` |

---

## 2. 系统架构

### 2.1 总体架构

```
┌────────────────────┐                           ┌────────────────────┐
│      手机端         │                           │    WS63 设备端      │
│  (BLE 调试助手)     │                           │                    │
│                    │     BLE GATT (2.4GHz)     │  ┌──────────────┐  │
│  ┌──────────────┐  │ ◄──────────────────────► │  │ BLE GATT     │  │
│  │ BLE Scanner  │  │                           │  │ Server       │  │
│  │ → 扫描广播    │  │  1. 扫描/连接              │  │ (ble_wifi_   │  │
│  │ → GATT 连接  │  │  2. 发现服务               │  │  cfg_server) │  │
│  │ → Write 凭证 │  │  3. 写入 WiFi 凭证          │  └──────┬───────┘  │
│  │ ← Notify 结果│  │  4. 读取连接结果            │         │          │
│  └──────────────┘  │                           │         ▼          │
│                    │                           │  ┌──────────────┐  │
│  用户操作:         │                           │  │ WiFi Manager │  │
│  1. 打开 App      │                           │  │ → STA Scan   │  │
│  2. 输入 SSID+PWD │                           │  │ → STA Connect│  │
│  3. 点击"发送"   │                           │  │ → DHCP       │  │
│  4. 查看结果     │                           │  └──────┬───────┘  │
│                    │                           │         │          │
└────────────────────┘                           │         ▼          │
                                                 │  ┌──────────────┐  │
                                                 │  │   路由器      │  │
                                                 │  │  (目标 AP)   │  │
                                                 │  └──────────────┘  │
                                                 └────────────────────┘
```

### 2.2 软件模块分层

```
┌─────────────────────────────────────────────┐
│          Application Layer                  │
│  ble_wifi_cfg_sample.c (配网主流程)          │
├─────────────────────────────────────────────┤
│          BLE Service Layer                  │
│  ble_wifi_cfg_server.c (GATT 服务实现)       │
│  ble_wifi_cfg_adv.c   (广播配置)            │
├─────────────────────────────────────────────┤
│          BLE Protocol Stack                 │
│  BTS GATT / GAP / L2CAP                    │
├─────────────────────────────────────────────┤
│          Wi-Fi Service Layer                │
│  wifi_sta_scan() / wifi_sta_connect()       │
│  wifi_register_event_cb()                   │
├─────────────────────────────────────────────┤
│          OS Abstraction Layer (OSAL)        │
│  osal_kthread_create / osal_msleep / ...    │
├─────────────────────────────────────────────┤
│          LiteOS Kernel                      │
│  Task Scheduler / Memory / IPC              │
└─────────────────────────────────────────────┘
```

---

## 3. BLE GATT 服务定义

### 3.1 服务与特征值

| 属性 | UUID | 权限 | 说明 |
|------|------|------|------|
| **Primary Service** | `0xFD5C` | — | BLE WiFi 配置服务 |
| **Control Point** | `0xFD5D` | Write No Response, Notify | 控制通道（配网流程控制） |
| **WiFi Information** | `0xFD5E` | Write No Response, Indicate | WiFi 凭证写入（SSID + 密码） |
| **AP Report** | `0xFD5F` | Write No Response, Notify | 扫描结果上报 / 连接状态回传 |

### 3.2 广播参数

| 参数 | 值 |
|------|-----|
| 广播类型 | Connectable Undirected (ADV_IND) |
| 广播间隔 | 32 ms (min: `0x20 × 0.625ms`, max: `0x60 × 0.625ms`) |
| 广播时长 | 持续广播（`0x0000` = forever） |
| 广播通道 | 37 / 38 / 39 (全部) |
| 设备名称 | `ble_wifi_config` (15 bytes) |
| Manufacturer Data | Company ID `0x027D` + 17 bytes 自定义数据 |

### 3.3 广播数据包解析

**ADV Data (23 bytes):**

| 偏移 | 字节 | 内容 | 说明 |
|------|------|------|------|
| 0 | `02 01 02` | Flags | LE General Discoverable, BR/EDR Not Supported |
| 3 | `13 FF` | Manufacturer Specific Data Header | length=19, type=0xFF |
| 5 | `7D 02` | Company ID (Little-Endian) | `0x027D` (HiSilicon) |
| 7 | `0E 70 80 00 ... A3` | 17 bytes | 自定义数据（设备识别用） |
| 23 | — | — | 总计 23 bytes (`HW_ADV_DATA_LEN = 0x17`) |

**Scan Response Data:**

| 偏移 | 内容 | 说明 |
|------|------|------|
| 0–2 | TX Power Level | `0x02 0x0A 0x00` |
| 3–20 | Device Name | `0x10 0x09` + `ble_wifi_config` (15 bytes) |

### 3.4 GATT Characteristic 详细定义

#### Characteristic 1: Control Point (`0xFD5D`)

| 属性 | 值 |
|------|-----|
| UUID | `0xFD5D` |
| Properties | Write No Response (`0x04`) + Notify (`0x10`) |
| CCCD | `0x2902`, Permissions: Read + Write |
| 说明 | 配网流程控制指令，当前版本保留用于扩展 |

#### Characteristic 2: WiFi Information (`0xFD5E`)

| 属性 | 值 |
|------|-----|
| UUID | `0xFD5E` |
| Properties | Write No Response (`0x04`) + Indicate (`0x20`) |
| CCCD | `0x2902`, Permissions: Read + Write |
| 数据格式 | SSID (32 bytes) + Password (32 bytes) |
| 说明 | **手机向设备写入 WiFi 凭证的核心特征值** |

#### Characteristic 3: AP Report (`0xFD5F`)

| 属性 | 值 |
|------|-----|
| UUID | `0xFD5F` |
| Properties | Write No Response (`0x04`) + Notify (`0x10`) |
| CCCD | `0x2902`, Permissions: Read + Write |
| 说明 | 设备向手机上报 AP 扫描列表 / 配网连接结果 |

---

## 4. 数据协议

### 4.1 WiFi 凭证写入格式（手机 → WS63）

**写入目标：** Characteristic `0xFD5E`  
**数据长度：** 64 bytes (固定)  
**编码格式：** ASCII 字符串，不足部分以 `0x00` 填充

| 偏移 | 长度 | 字段 | 说明 |
|------|------|------|------|
| 0 | 32 | SSID | WiFi 热点名称（ASCII），不足 32 字节尾部补 `\0` |
| 32 | 32 | Password | WiFi 热点密码（ASCII），不足 32 字节尾部补 `\0` |

**示例：** SSID = `"MyWiFi"` (6 bytes)，Password = `"12345678"` (8 bytes)

```
Offset: 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
--------------------------------------------------------------
0000:   4D 79 57 69 46 69 00 00 00 00 00 00 00 00 00 00   MyWiFi..........
0010:   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
0020:   31 32 33 34 35 36 37 38 00 00 00 00 00 00 00 00   12345678........
0030:   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
```

### 4.2 AP 扫描列表上报（WS63 → 手机）

**Notification 来源：** Characteristic `0xFD5F`  
**触发条件：** 手机向 `0xFD5F` 写入任意字节触发 AP 扫描

| 偏移 | 长度 | 字段 | 说明 |
|------|------|------|------|
| 0 | 1 | Report Type | `0x02` = AP 列表 |
| 1 | 1 | AP Count | 扫描到的 AP 数量 (max 10) |
| 2 | 34 × N | AP Entries | 每条 34 字节: SSID (33) + RSSI (1) |

### 4.3 配网结果上报（WS63 → 手机）

**Notification 来源：** Characteristic `0xFD5F`  
**触发条件：** 配网流程结束后自动上报

| 偏移 | 长度 | 字段 | 说明 |
|------|------|------|------|
| 0 | 1 | Report Type | `0x01` = WiFi 连接状态 |
| 1 | 1 | Result Code | 见下表 |

### 4.4 错误码定义

| 错误码 | 枚举 | 说明 |
|--------|------|------|
| `0x00` | `WIFI_ERRCODE_NONE` | 配网成功，已获取 IP |
| `0x01` | `WIFI_ERRCODE_SSID_NOT_FOUND` | 未找到指定 SSID 的热点 |
| `0x02` | `WIFI_ERRCODE_PWD_ERROR` | 密码错误（含 4-Way Handshake 超时 / MIC 校验失败） |
| `0x03` | `WIFI_ERRCODE_DHCP_FAILED` | DHCP 获取 IP 地址失败 |
| `0x04` | `WIFI_ERRCODE_BEACON_LOST` | 连接后信标丢失（信号弱/超出范围） |
| `0x05` | `WIFI_ERRCODE_OTHERS` | 其他未知错误 |

---

## 5. 配网流程

### 5.1 完整时序

```
手机 (BLE Client)                          WS63 (BLE Server + STA)
     │                                              │
     │  ① BLE 扫描                                  │
     │─────────────────────────────────────────────►│  广播 ADV_IND
     │  ◄─── ble_wifi_config                        │  (持续广播)
     │                                              │
     │  ② GATT 连接                                 │
     │─────────────────────────────────────────────►│  gap_ble 连接回调
     │  ◄─── CONNECTED                              │
     │                                              │
     │  ③ MTU 交换 (~100 bytes)                     │
     │─────────────────────────────────────────────►│
     │  ◄─── MTU Response                           │
     │                                              │
     │  ④ 发现服务 (Discover All Services)           │
     │─────────────────────────────────────────────►│
     │  ◄─── Service 0xFD5C found                   │
     │                                              │
     │  ⑤ 发现 Characteristic                       │
     │─────────────────────────────────────────────►│
     │  ◄─── 0xFD5D, 0xFD5E, 0xFD5F                │
     │                                              │
     │  ⑥ 启用 CCCD (Notify/Indicate)                │
     │   Write 0x0001 to 0xFD5E CCCD                │
     │   Write 0x0001 to 0xFD5F CCCD                │
     │─────────────────────────────────────────────►│
     │  ◄─── OK                                     │
     │                                              │
     │  ═══ 可选: 请求 AP 扫描列表 ═══                │
     │  ⑦ Write 0xFD5F (任意 1 byte)                │
     │─────────────────────────────────────────────►│  → wifi_sta_scan()
     │                                              │    扫描周围 AP
     │  ◄─── Notify 0xFD5F                          │  ← 上报 AP 列表
     │       [0x02][count][APs...]                   │
     │                                              │
     │  ═══ 必选: 写入 WiFi 凭证 ═══                  │
     │  ⑧ Write 0xFD5E (64 bytes)                   │
     │      [SSID(32) + Password(32)]               │
     │─────────────────────────────────────────────►│  → wifi_sta_scan()
     │                                              │    → 匹配 SSID
     │                                              │    → wifi_sta_connect()
     │                                              │    → DHCP 获取 IP
     │                                              │
     │  ◄─── Indicate 0xFD5E (ACK)                  │
     │  ◄─── Notify 0xFD5F                          │  ← 上报连接结果
     │       [0x01][result_code]                     │
     │                                              │
     │  ⑨ 配网完成，断开 BLE                          │
     │─────────────────────────────────────────────►│
     │  ◄─── DISCONNECTED                           │
     │                                              │  WS63 已连接路由器
     │                                              │  正常业务工作
```

### 5.2 配网状态机（WS63 端）

实现为两层状态机：`prov_state_t`（高层配网状态，驱动 LED 和流程控制）和内层的 `bgwc_state_enum`（WiFi 子状态）。

**高层状态机（`prov_state_t`）：**

```
       ┌──────────────┐
       │ PROV_STATE_   │  上电
       │    INIT       │
       └──────┬───────┘
              │
              ▼
       ┌──────────────┐
       │ PROV_STATE_   │  检查 NV 是否有已保存凭证
       │   NV_CHECK    │
       └──┬────────┬──┘
          │        │
    NV有凭证    NV无凭证
          │        │
          ▼        ▼
  ┌───────────┐  ┌──────────────┐
  │PROV_STATE_ │  │ PROV_STATE_  │  BLE 广播，LED 快闪
  │NV_CONFIGURED│ │  BLE_ADV     │◄───────────────────────┐
  └─────┬─────┘  └──────┬───────┘                       │
        │               │ 手机写入凭证                    │
        │               ▼                               │
        │       ┌──────────────┐                        │
        │       │ PROV_STATE_  │  BLE 已连接              │
        │       │ BLE_CONNECTED│                        │
        │       └──────┬───────┘                        │
        │              │ 启动 WiFi 扫描                  │
        ▼              ▼                               │
  ┌──────────────────────────┐                        │
  │ PROV_STATE_WIFI_SCANNING │  LED 慢闪                │
  └────────────┬─────────────┘                        │
               │ 扫描完成回调                           │
               ▼                                      │
  ┌──────────────────────────┐                        │
  │ PROV_STATE_WIFI_CONNECTING│  LED 慢闪              │
  └────────────┬─────────────┘                        │
               │ 连接状态回调                           │
               ▼                                      │
  ┌──────────────────────────┐                        │
  │  PROV_STATE_WIFI_DHCP    │  LED 慢闪              │
  └────┬──────────┬──────────┘                        │
       │ DHCP成功  │ DHCP/连接失败                      │
       ▼           ▼                                   │
  ┌──────────┐  ┌──────────────┐                      │
  │PROV_STATE│  │ PROV_STATE_  │  LED 快闪 3 次        │
  │_SUCCESS  │  │   FAILED     │─── 重试 (最多3次) ────┘
  │LED 常亮  │  └──────┬───────┘
  └──────────┘         │ 超过最大重试次数
                       ▼
                ┌──────────────┐
                │PROV_STATE_   │  LED 灭，深度空闲
                │  TIMEOUT     │
                └──────────────┘
```

**WiFi 子状态机（`bgwc_state_enum`，保持不变）：**

```
CONFIG_DEMO_INIT → CONFIG_DEMO_WIFI_INIT → CONFIG_DEMO_WIFI_SCAN_DOING
→ CONFIG_DEMO_WIFI_SCAN_DONE → CONFIG_DEMO_WIFI_CONNECT_DOING
→ CONFIG_DEMO_WIFI_CONNECT_DONE → CONFIG_DEMO_WIFI_DHCP_DONE
```

---

## 6. WS63 软件设计

### 6.1 模块架构

```
ble_wifi_cfg_sample.c          ← 配网主任务 + 状态机
  ├── ble_wifi_prov_nv.c       ← NV 持久化（WiFi 凭证存储）
  ├── ble_wifi_prov_led.c      ← LED 状态指示（独立任务驱动 GPIO）
  ├── ble_wifi_cfg_server.c    ← BLE GATT 服务（复用，未修改）
  └── ble_wifi_cfg_adv.c       ← BLE 广播（复用，未修改）
```

### 6.2 任务划分

| 任务名 | 优先级 | 栈大小 | 职责 |
|--------|--------|--------|------|
| `bgle_wifi_cfg_task` | 26 | 0x1000 | 配网主线程：状态机驱动 + BLE 初始化 + WiFi 连接 + 重试管理 |
| `prov_led` | 30 | 0x400 | LED 状态指示（低优先级，仅外观） |
| BLE GATT Server | (协议栈内部) | — | 广播 / GATT 服务 / 回调分发 |
| WiFi Host Task | 25 | 0x2000 | WiFi 协议栈主线程 |

### 6.3 关键接口

#### NV 存储模块（新增）

```c
// 保存 WiFi 凭证到 NV（配网成功后调用）
errcode_t ble_wifi_prov_nv_save(const char *ssid, const char *password);

// 从 NV 加载 WiFi 凭证（启动时调用）
errcode_t ble_wifi_prov_nv_load(char *ssid, uint16_t ssid_max,
                                char *password, uint16_t pwd_max);

// 清除 NV 中的凭证（恢复出厂设置）
errcode_t ble_wifi_prov_nv_clear(void);

// 检查是否有已保存的配网信息
bool ble_wifi_prov_nv_is_configured(void);
```

NV Key 分配（用户区域 `0x5000`-`0xFFFF`）：

| Key ID | 内容 | 说明 |
|--------|------|------|
| `0x5001` | WiFi SSID | ASCII 字符串，max 33 字节 |
| `0x5002` | WiFi 密码 | ASCII 字符串，max 65 字节 |
| `0x5003` | 配网标记 | 1 字节（0=未配网，1=已配网） |

#### LED 指示模块（新增）

```c
// 初始化 LED（创建独立指示灯任务）
void ble_wifi_prov_led_init(uint8_t pin);

// 设置 LED 状态（线程安全，可从任意任务调用）
void ble_wifi_prov_led_set_state(prov_led_state_t state);
```

LED 状态对照表：

| 状态枚举 | LED 行为 | 对应配网阶段 |
|----------|----------|-------------|
| `PROV_LED_OFF` | 熄灭 | 空闲 / NV 已配置 |
| `PROV_LED_FAST_BLINK` | 200ms 周期闪烁 | BLE 广播中 |
| `PROV_LED_SLOW_BLINK` | 800ms 周期闪烁 | WiFi 扫描/连接/DHCP 中 |
| `PROV_LED_ON` | 常亮 | 配网成功 |
| `PROV_LED_ERROR_FLASH` | 150ms × 6 次后熄灭 | 配网失败 |

#### BLE 侧（复用，未修改）

```c
errcode_t ble_wifi_cfg_server_init(void);
uint8_t ble_wifi_cfg_start_adv(void);
errcode_t ble_wifi_cfg_server_send_report_by_uuid(const uint8_t *data, uint32_t len);
errcode_t ble_wifi_cfg_server_send_report_by_handle(uint16_t attr_handle,
                                                     const uint8_t *data, uint8_t len);
```

#### WiFi 侧（复用，未修改）

```c
int wifi_sta_enable(void);
int wifi_register_event_cb(wifi_event_stru *cb);
int wifi_sta_scan(void);
int wifi_sta_get_scan_info(wifi_scan_info_stru *result, uint32_t *num);
int wifi_sta_connect(wifi_sta_config_stru *config);
```

### 6.4 Kconfig 配置

```
Application → Enable Sample → Enable the Sample of WIFI
  → Sample → Support BLE WIFI CFG Sample
    → BLE WiFi Provisioning Configuration  ← 新增
```

| 配置项 | 类型 | 默认值 | 说明 |
|--------|------|--------|------|
| `CONFIG_BLE_PROV_NV_ENABLE` | bool | y | NV 持久化存储，配网后保存凭证 |
| `CONFIG_BLE_PROV_AUTO_RECONNECT` | bool | y | 启动时自动用 NV 凭证连 WiFi |
| `CONFIG_BLE_PROV_LED_ENABLE` | bool | n | LED 状态指示灯 |
| `CONFIG_BLE_PROV_LED_PIN` | int | 7 | LED 连接的 GPIO Pin 号 |
| `CONFIG_BLE_PROV_TIMEOUT_SEC` | int | 60 | 配网超时（秒），范围 10~300 |
| `CONFIG_BLE_PROV_MAX_RETRIES` | int | 3 | 最大重试次数，范围 1~10 |

### 6.5 BLE 回调处理流程

```
ble_wifi_cfg_server_enable_cbk()     ──→ ble_wifi_gatts_register_server()
                                      ──→ wifi_cfg_server_service() (添加 Service + Characteristic)

ble_wifi_cfg_server_receive_write_req_cbk()
  ├─ handle == g_chara_cfg_hdl       ──→ set_wifi_cfg_info() (保存凭证)
  └─ handle == g_chara_wifi_list_hdl ──→ bgwc_wifi_list_resp_send() (触发 AP 扫描)

ble_wifi_cfg_server_connect_change_cbk()
  └─ conn_state == CONNECTED         ──→ 记录 g_conn_handle

bgwc_scan_state_changed()            ──→ 打包 AP 列表 → ble_wifi_cfg_server_send_report_by_uuid()

bgwc_connection_changed()            ──→ 解析错误码 → ble_wifi_cfg_server_send_report_by_uuid()
```

---

## 7. 手机端操作指南

### 7.1 工具选择

推荐使用 **BLE 调试助手**（华为应用市场可下载）或 **nRF Connect**（Google Play / 官网）。

### 7.2 操作步骤

#### 步骤 1：扫描设备

1. 打开 BLE 调试助手
2. 点击"扫描"开始搜索 BLE 设备
3. 在设备列表中找到名称为 `ble_wifi_config` 的设备
4. 点击设备名称，查看广播详情确认 Manufacturer Data 中 Company ID 为 `0x027D`

#### 步骤 2：连接设备

1. 点击 `ble_wifi_config` 设备发起连接
2. 等待连接成功（通常 1–2 秒）
3. 连接后应用会自动发现 Service 和 Characteristic

#### 步骤 3：启用通知

1. 展开 Service `0xFD5C`
2. 找到 Characteristic `0xFD5E`，向其 CCCD (`0x2902`) 写入 `01 00`（启用 Indicate）
3. 找到 Characteristic `0xFD5F`，向其 CCCD (`0x2902`) 写入 `01 00`（启用 Notify）

#### 步骤 4（可选）：获取 AP 列表

1. 向 Characteristic `0xFD5F` 写入任意 1 字节（如 `01`）
2. 稍等片刻，设备会通过 Notify 推送 `0xFD5F` 数据
3. 解析返回的 AP 列表（格式见 §4.2）

#### 步骤 5：写入 WiFi 凭证

1. 计算 HEX 数据：SSID (32 bytes) + Password (32 bytes)
   - 例如 SSID=`"MyWiFi"` → `4D 79 57 69 46 69` + 26 字节 `00`
   - 例如 PWD=`"12345678"` → `31 32 33 34 35 36 37 38` + 24 字节 `00`
2. 拼接为 64 字节 HEX
3. 向 Characteristic `0xFD5E` Write 这 64 字节

#### 步骤 6：查看配网结果

1. 设备会自动开始扫描并连接
2. 稍等片刻（扫描 3–5 秒 + 连接 5–10 秒 + DHCP 1–5 秒）
3. `0xFD5F` 会收到 Notify：`01 XX`
   - `01 00` = 配网成功
   - `01 01` = 未找到 Wi-Fi
   - `01 02` = 密码错误
   - `01 03` = DHCP 失败
   - `01 04` = 信号丢失
   - `01 05` = 其他错误

### 7.3 HEX 数据生成速查表

| WiFi SSID | 前 32 字节 HEX |
|-----------|----------------|
| `MyWiFi` | `4D 79 57 69 46 69` + 26×`00` |
| `HUAWEI-123` | `48 55 41 57 45 49 2D 31 32 33` + 22×`00` |
| `TP-LINK_5G` | `54 50 2D 4C 49 4E 4B 5F 35 47` + 22×`00` |

| WiFi 密码 | 后 32 字节 HEX |
|-----------|----------------|
| `12345678` | `31 32 33 34 35 36 37 38` + 24×`00` |
| `abc12345` | `61 62 63 31 32 33 34 35` + 24×`00` |
| 无密码 | 32×`00` |

---

## 8. 测试用例

### 8.1 功能测试

#### TC-01：正常配网流程

| 项目 | 内容 |
|------|------|
| **测试目的** | 验证完整的 BLE 配网流程 |
| **前置条件** | 路由器 SSID `TestWiFi`，密码 `pass1234` 正常工作；WS63 烧录 BLE WiFi CFG 固件 |
| **测试步骤** | 1. WS63 上电，确认 BLE 广播正常（`ble_wifi_config` 可见）<br>2. 手机端 BLE 调试助手连接设备<br>3. 发现 Service `0xFD5C`<br>4. 启用 CCCD<br>5. 向 `0xFD5E` 写入 64 字节凭证数据<br>6. 等待 Notify 返回 |
| **预期结果** | 1. 串口打印 `STA DHCP Succ`<br>2. 收到 Notify `01 00`（配网成功）<br>3. WS63 可通过 `ping` 验证网络连通 |
| **优先级** | P0 |

#### TC-02：SSID 不存在

| 项目 | 内容 |
|------|------|
| **测试目的** | 验证设备对不存在的 SSID 的错误处理 |
| **前置条件** | 同 TC-01 |
| **测试步骤** | 1. 向 `0xFD5E` 写入一个不存在的 SSID（如 `NoSuchWiFi___`）<br>2. 等待结果 |
| **预期结果** | 1. 串口打印 `Do not find AP, try again!`<br>2. 收到 Notify `01 01`（SSID 未找到）<br>3. 设备保持广播状态，等待重新配网 |
| **优先级** | P0 |

#### TC-03：密码错误

| 项目 | 内容 |
|------|------|
| **测试目的** | 验证设备对错误密码的检测 |
| **前置条件** | 路由器 SSID `TestWiFi` 密码为 `pass1234` |
| **测试步骤** | 1. 向 `0xFD5E` 写入正确 SSID + 错误密码 `wrongpass`<br>2. 等待结果 |
| **预期结果** | 1. 收到 Notify `01 02`（密码错误）<br>2. 设备保持广播状态，等待重新配网 |
| **优先级** | P0 |

#### TC-04：AP 列表获取

| 项目 | 内容 |
|------|------|
| **测试目的** | 验证设备能扫描并上报周围 WiFi 列表 |
| **前置条件** | 周围至少存在 2 个 WiFi 热点 |
| **测试步骤** | 1. 连接设备并启用 Notify<br>2. 向 `0xFD5F` 写入 `01`（请求 AP 列表）<br>3. 等待 Notify |
| **预期结果** | 收到 Notify 数据：`02` + count + AP 条目列表，每条包含 SSID 和 RSSI，数量 ≤ 10 |
| **优先级** | P1 |

#### TC-05：重复配网

| 项目 | 内容 |
|------|------|
| **测试目的** | 验证设备支持在配网失败后重新配网 |
| **前置条件** | 同 TC-01 |
| **测试步骤** | 1. 先发送错误密码（TC-03），确认失败<br>2. 不重启设备，直接再次写入正确凭证<br>3. 等待结果 |
| **预期结果** | 第二次配网成功，收到 Notify `01 00` |
| **优先级** | P1 |

#### TC-06：长 SSID / 长密码

| 项目 | 内容 |
|------|------|
| **测试目的** | 验证设备对边界长度的 SSID 和密码的处理 |
| **前置条件** | 路由器 SSID 设置为 32 字节（刚好满）、密码 63 字节 |
| **测试步骤** | 1. 写入 64 字节：满 32 字节 SSID + 满 32 字节密码<br>2. 等待结果 |
| **预期结果** | 配网成功，收到 Notify `01 00` |
| **优先级** | P1 |

#### TC-07：中文 SSID

| 项目 | 内容 |
|------|------|
| **测试目的** | 验证设备对 UTF-8 编码中文 SSID 的支持 |
| **前置条件** | 路由器 SSID 设置为中文（如 `我的WiFi`） |
| **测试步骤** | 1. 将 UTF-8 编码的中文 SSID（如 `E6 88 91 E7 9A 84 57 69 46 69`）写入前 32 字节<br>2. 等待结果 |
| **预期结果** | 配网成功，收到 Notify `01 00` |
| **优先级** | P2 |

#### TC-08：无密码开放网络

| 项目 | 内容 |
|------|------|
| **测试目的** | 验证设备连接无密码的开放 WiFi |
| **前置条件** | 路由器设置为开放网络（无密码） |
| **测试步骤** | 1. 写入 SSID + 全零密码（32 字节 `00`）<br>2. 等待结果 |
| **预期结果** | 配网成功，收到 Notify `01 00` |
| **优先级** | P1 |

### 8.2 异常测试

#### TC-09：WiFi 中途关闭

| 项目 | 内容 |
|------|------|
| **测试目的** | 验证设备在配网过程中路由器关闭的处理 |
| **前置条件** | 路由器开启 |
| **测试步骤** | 1. 写入正确凭证<br>2. 在扫描或连接过程中关闭路由器<br>3. 等待结果 |
| **预期结果** | 收到 Notify `01 01`（SSID 未找到）或 `01 04`（信标丢失），设备恢复广播 |
| **优先级** | P1 |

#### TC-10：BLE 连接断开

| 项目 | 内容 |
|------|------|
| **测试目的** | 验证手机 BLE 断连后设备的状态恢复 |
| **前置条件** | 手机已连接 WS63 |
| **测试步骤** | 1. 写入凭证后立即断开 BLE 连接（手机走远）<br>2. 通过串口观察设备状态 |
| **预期结果** | 1. 设备继续完成 WiFi 配网流程（不受 BLE 断连影响）<br>2. 若配网成功，设备正常连接路由器<br>3. 若配网失败，设备恢复 BLE 广播 |
| **优先级** | P1 |

#### TC-11：DHCP Server 不可用

| 项目 | 内容 |
|------|------|
| **测试目的** | 验证 DHCP 失败的场景 |
| **前置条件** | 路由器开启但关闭 DHCP 服务 |
| **测试步骤** | 1. 写入正确凭证<br>2. 等待结果 |
| **预期结果** | 收到 Notify `01 03`（DHCP 失败），设备恢复广播 |
| **优先级** | P1 |

#### TC-12：超大 MTU 数据

| 项目 | 内容 |
|------|------|
| **测试目的** | 验证设备对异常长度数据的处理 |
| **前置条件** | 手机已连接 |
| **测试步骤** | 1. 向 `0xFD5E` 写入超过 64 字节的数据<br>2. 或向 `0xFD5E` 写入不足 64 字节的数据 |
| **预期结果** | 设备不应崩溃；数据被截断或忽略；设备恢复广播状态 |
| **优先级** | P2 |

### 8.3 稳定性测试

#### TC-13：连续配网 100 次

| 项目 | 内容 |
|------|------|
| **测试目的** | 验证连续多次配网的稳定性 |
| **前置条件** | 同 TC-01 |
| **测试步骤** | 1. 自动化脚本循环执行：连接 BLE → 写入凭证 → 等待结果 → 断开 → 重启设备<br>2. 重复 100 次 |
| **预期结果** | 100 次全部成功，无内存泄漏（通过串口 `mem: used` 检查内存使用趋势） |
| **优先级** | P2 |

#### TC-14：长时间广播

| 项目 | 内容 |
|------|------|
| **测试目的** | 验证设备长时间广播不异常 |
| **前置条件** | WS63 上电，无手机连接 |
| **测试步骤** | 1. 设备保持广播状态 24 小时<br>2. 24 小时后连接手机执行一次配网 |
| **预期结果** | 24 小时后配网正常，工作正常 |
| **优先级** | P2 |

### 8.4 增强特性测试（V1.1 新增）

#### TC-15：NV 持久化与自动重连

| 项目 | 内容 |
|------|------|
| **测试目的** | 验证配网成功后凭证持久化，重启后自动重连 |
| **前置条件** | 启用 `CONFIG_BLE_PROV_NV_ENABLE` + `CONFIG_BLE_PROV_AUTO_RECONNECT` |
| **测试步骤** | 1. 通过 BLE 完成一次配网（TC-01）<br>2. 确认串口打印 `Credentials saved to NV`<br>3. 设备断电重启<br>4. 通过串口观察启动流程 |
| **预期结果** | 1. 串口打印 `NV configured, loading credentials...`<br>2. 串口打印 `loaded ssid=TestWiFi`<br>3. 跳过 BLE 广播，直接 WiFi 扫描连接<br>4. DHCP 成功获取 IP，LED 常亮<br>5. 手机端 BLE 扫描看不到 `ble_wifi_config` 广播 |
| **优先级** | P0 |

#### TC-16：NV 凭证被清除后重新配网

| 项目 | 内容 |
|------|------|
| **测试目的** | 验证 NV 清除后回退到 BLE 配网模式 |
| **前置条件** | 设备已有 NV 凭证（TC-15 后状态） |
| **测试步骤** | 1. 调用 `ble_wifi_prov_nv_clear()` 清除凭证（或通过 AT 命令）<br>2. 设备断电重启<br>3. 观察启动行为 |
| **预期结果** | 1. NV 检查返回 false<br>2. 启动 BLE 广播（`ble_wifi_config` 可见）<br>3. 可正常走 BLE 配网流程 |
| **优先级** | P1 |

#### TC-17：配网超时

| 项目 | 内容 |
|------|------|
| **测试目的** | 验证配网超时机制 |
| **前置条件** | `CONFIG_BLE_PROV_TIMEOUT_SEC=30`（缩短测试周期） |
| **测试步骤** | 1. WS63 上电，确认 BLE 广播<br>2. 手机不做任何连接，等待 30 秒<br>3. 通过串口和 LED 观察 |
| **预期结果** | 1. 30 秒后串口打印 `provisioning timeout, stop BLE`<br>2. BLE 广播停止（手机扫描不到设备）<br>3. LED 快闪 3 次后熄灭<br>4. 设备进入低功耗空闲 |
| **优先级** | P1 |

#### TC-18：失败重试次数验证

| 项目 | 内容 |
|------|------|
| **测试目的** | 验证最大重试次数限制 |
| **前置条件** | `CONFIG_BLE_PROV_MAX_RETRIES=2` |
| **测试步骤** | 1. 连续 2 次发送错误密码<br>2. 每次等待失败结果后再次发送<br>3. 观察第 3 次行为 |
| **预期结果** | 1. 前 2 次失败后 LED 快闪 3 次，恢复 BLE 广播<br>2. 第 2 次失败后（超过最大重试）串口打印 `All retries exhausted`<br>3. BLE 广播停止，LED 熄灭，设备进入深度空闲 |
| **优先级** | P1 |

#### TC-19：LED 状态全路径验证

| 项目 | 内容 |
|------|------|
| **测试目的** | 验证 LED 在所有配网阶段的指示是否正确 |
| **前置条件** | `CONFIG_BLE_PROV_LED_ENABLE=y`，`CONFIG_BLE_PROV_LED_PIN=7` |
| **测试步骤** | 1. 上电：观察 LED 初始状态<br>2. BLE 广播中：观察闪烁频率<br>3. 写入凭证后：观察连接阶段闪烁<br>4. 配网成功：观察最终状态<br>5. 断电重启（NV 有效）：观察跳过配网时的状态 |
| **预期结果** | 1. 初始灭 → 广播时快闪（~200ms 周期）<br>2. 连接中慢闪（~800ms 周期）<br>3. 成功时常亮<br>4. NV 路径下熄灭（因无需配网）→ DHCP 成功后常亮 |
| **优先级** | P1 |

#### TC-20：NV 快速连接失败后回退 BLE

| 项目 | 内容 |
|------|------|
| **测试目的** | 验证 NV 中保存的 WiFi 失效后自动回退 BLE 配网 |
| **前置条件** | NV 中已有凭证，但路由器已关闭或 SSID 已变更 |
| **测试步骤** | 1. 设备上电（NV 有效但 WiFi 不可达）<br>2. 观察重试行为 |
| **预期结果** | 1. 首次尝试 NV 快速连接失败<br>2. 串口打印 `NV failed, switching to BLE provisioning`<br>3. BLE 广播自动启动<br>4. 手机可正常连接并重新配网 |
| **优先级** | P1 |

### 8.5 测试环境

| 项目 | 要求 |
|------|------|
| WS63 硬件 | HiHope_NearLink_DK3863E_V03 或兼容开发板 |
| 固件 | `ws63-liteos-app`，启用 `CONFIG_SAMPLE_SUPPORT_BLE_WIFI_CFG_SAMPLE` |
| 手机 | Android 8.0+ / iOS 12.0+ |
| 测试工具 | BLE 调试助手 或 nRF Connect |
| 路由器 | 2.4GHz WiFi 路由器，支持 WPA2-PSK |
| 串口工具 | HiSpark Studio 监视器 / SecureCRT / PuTTY |

### 8.6 测试用例矩阵

| 编号 | 用例名称 | 类别 | 优先级 |
|------|---------|------|--------|
| TC-01 | 正常配网流程 | 功能 | P0 |
| TC-02 | SSID 不存在 | 功能 | P0 |
| TC-03 | 密码错误 | 功能 | P0 |
| TC-15 | NV 持久化与自动重连 | 增强 | P0 |
| TC-04 | AP 列表获取 | 功能 | P1 |
| TC-05 | 重复配网 | 功能 | P1 |
| TC-06 | 长 SSID / 长密码 | 功能 | P1 |
| TC-08 | 无密码开放网络 | 功能 | P1 |
| TC-09 | WiFi 中途关闭 | 异常 | P1 |
| TC-10 | BLE 连接断开 | 异常 | P1 |
| TC-11 | DHCP Server 不可用 | 异常 | P1 |
| TC-16 | NV 凭证清除后重新配网 | 增强 | P1 |
| TC-17 | 配网超时 | 增强 | P1 |
| TC-18 | 失败重试次数验证 | 增强 | P1 |
| TC-19 | LED 状态全路径验证 | 增强 | P1 |
| TC-20 | NV 快速连接失败回退 BLE | 增强 | P1 |
| TC-07 | 中文 SSID | 功能 | P2 |
| TC-12 | 超大 MTU 数据 | 异常 | P2 |
| TC-13 | 连续配网 100 次 | 稳定性 | P2 |
| TC-14 | 长时间广播 | 稳定性 | P2 |

---

## 附录 A: 编译与烧录

### A.1 启用 BLE 配网

通过 HiSpark Studio → Kconfig 启用：

```
Application → Enable Sample → Enable the Sample of WIFI
  → Sample → Support BLE WIFI CFG Sample
```

或命令行：

```bash
cd src
python build.py menuconfig
```

### A.2 编译

```bash
python build.py -c ws63-liteos-app
```

### A.3 烧录

HiSpark Studio → 程序加载 → 选择编译产物 `ws63-liteos-app_all.fwpkg` → 串口烧录

### A.4 验证广播

串口监视器（921600 baud）复位设备后应看到：

```
[wifi_cfg_server] init ok
Ble Adv State:0
```

---

## 附录 B: 已实现的产品化特性

以下功能已在当前代码（V1.1）中实现，通过 Kconfig 开关控制：

| 特性 | 状态 | Kconfig 开关 | 说明 |
|------|------|-------------|------|
| **NV 存储** | ✅ 已实现 | `CONFIG_BLE_PROV_NV_ENABLE` | 配网成功后自动保存 SSID/密码到 NV；下次上电跳过 BLE 直接连接 |
| **配网超时** | ✅ 已实现 | `CONFIG_BLE_PROV_TIMEOUT_SEC` | 超时后自动停止 BLE 广播，进入低功耗空闲 |
| **失败重试** | ✅ 已实现 | `CONFIG_BLE_PROV_MAX_RETRIES` | 失败后自动重新广播，等待手机再次配网，最多重试 N 次 |
| **LED 指示** | ✅ 已实现 | `CONFIG_BLE_PROV_LED_ENABLE` | 快闪=广播中，慢闪=连接中，常亮=成功，3闪灭=失败 |
| **BLE 清理** | ✅ 已实现 | — | 配网成功/失败后自动调用 `gap_ble_stop_adv()` 停止广播 |
| **AP 列表 bug 修复** | ✅ 已实现 | — | 增加状态守卫，纯 AP 列表请求不再死循环 |

后续可增强方向：
1. **安全增强**：增加 BLE 配对绑定（Passkey / Just Works），防止恶意配网
2. **自定义 App**：开发专用配网 App，封装 HEX 编解码，提供用户友好的 UI 流程
3. **WiFi 信息管理**：支持存储多组 WiFi 凭证，按信号强度择优连接
4. **按键触发**：硬件按键触发清除 NV 凭证，进入配网模式

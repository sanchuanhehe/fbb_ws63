# BLE HID Keyboard — 软件方案设计文档 (SDD)

**文档版本：** V1.0
**适用范围：** WS63 系列芯片（WS63 / WS63E）
**目标平台：** Huawei LiteOS (v208.5.0)，RISC-V rv32imc
**Profile：** BLE HID over GATT (HoG)，Keyboard 设备
**状态：** 设计阶段

---

## 1. 概述

### 1.1 背景

WS63 作为 BLE HID 键盘外设：手机通过自定义 BLE GATT 特征值发送文字，WS63 将文字转成标准 HID 键盘扫描码，通过 HID Report 特征值发送给 PC。PC 端通过自带蓝牙连接 WS63，无需任何驱动或配对码，即识别为标准蓝牙键盘。

### 1.2 应用场景

```
┌──────────┐  Custom GATT   ┌──────────────┐  HID over GATT   ┌──────────┐
│  手机     │ ◄────────────► │    WS63      │ ◄──────────────► │    PC    │
│          │  Write text    │              │  Input Report    │          │
│ BLE App  │ ─────────────► │ HID Keyboard │ ───────────────► │ 蓝牙设置  │
│          │                │ Peripheral   │                  │  中连接   │
└──────────┘                └──────────────┘                  └──────────┘
```

WS63 同时维护 **两个 BLE 连接**：
- **手机** — 向自定义 Data RX 特征值写入 UTF-8 文字
- **PC** — 作为 HID Host 读取 WS63 发送的键盘输入报告

### 1.3 方案对比

| 方案 | 手机 | WS63 | PC | 结论 |
|------|------|------|-----|------|
| **BLE HID 键盘** | BLE GATT 写文字 | 转 HID 扫描码发送 | 蓝牙原生支持，无驱动 | ✅ 本设计 |
| BLE 串口透传 | BLE GATT 写数据 | UART 转发 | USB 串口 COM 口 | 不同场景 |
| USB HID | BLE GATT 写数据 | USB HID Device | USB 驱动 | WS63 无 USB PHY |

---

## 2. 系统架构

### 2.1 总体架构

```
┌───────────────────────────────────────────────────────────┐
│                      WS63 设备端                           │
│                                                           │
│  ┌─────────────────┐    ┌──────────────────────────────┐ │
│  │ Data RX Service │    │    HID Keyboard Service       │ │
│  │    (0xA001)     │    │        (0x1812)               │ │
│  │                 │    │                              │ │
│  │ 手机写文字 ────►│    │  ┌─────────────────────────┐ │ │
│  │                 │    │  │ Report Map (0x2A4B)     │ │ │
│  │◄── Status ─────│    │  │ (键盘报告描述符)         │ │ │
│  └────────┬────────┘    │  └─────────────────────────┘ │ │
│           │             │  ┌─────────────────────────┐ │ │
│           ▼             │  │ Input Report (0x2A4D)   │ │ │
│  ┌─────────────────┐    │  │ (8 字节键盘报告)         │ │ │
│  │  Text-to-HID    │    │  └───────────┬─────────────┘ │ │
│  │  Converter      │    └──────────────┼───────────────┘ │
│  │  UTF-8 →        │                   │                  │
│  │  HID scan codes │──────────────────┘                  │
│  └─────────────────┘                                     │
└───────────────────────────────────────────────────────────┘
```

### 2.2 双连接模型

WS63 BLE 协议栈需支持至少 2 路并发连接：

| 连接 | 角色 | 暴露服务 | 数据方向 |
|------|------|---------|---------|
| 手机 ↔ WS63 | Central → Peripheral | Data RX Service (0xA001) | 手机写入文字 |
| PC ↔ WS63 | Central → Peripheral | HID Service (0x1812) | WS63 发送键盘报告 |

### 2.3 软件模块分层

```
┌─────────────────────────────────────────────┐
│          Application Layer                  │
│  ble_hid_sample.c  (入口 + 状态机)          │
├─────────────────────────────────────────────┤
│          HID Service Layer                  │
│  ble_hid_kb.c       (HID GATT 服务)         │
│  ble_hid_kb_adv.c   (HID 广播配置)          │
├─────────────────────────────────────────────┤
│          Data RX Service Layer              │
│  ble_hid_data.c     (数据接收服务)           │
├─────────────────────────────────────────────┤
│          HID Engine                         │
│  hid_keycode.c      (文字 → 扫描码映射)      │
├─────────────────────────────────────────────┤
│          BLE Protocol Stack                 │
│  BTS GATT / GAP / L2CAP                    │
├─────────────────────────────────────────────┤
│          OSAL + LiteOS Kernel               │
└─────────────────────────────────────────────┘
```

---

## 3. BLE GATT 服务定义

### 3.1 HID Service（标准，0x1812）

HID over GATT 必选服务，遵循 [HID over GATT Profile v1.0](https://www.bluetooth.com/specifications/specs/hid-over-gatt-profile-1-0/)。

| 属性 | UUID | 权限 | 说明 |
|-----------|------|-------------|-------------|
| **Primary Service** | `0x1812` | — | Human Interface Device |
| **Protocol Mode** | `0x2A4E` | Read, Write No Response | 0=Boot 模式, 1=Report 模式 |
| **Report Map** | `0x2A4B` | Read | 键盘 Report 描述符 |
| **Report** | `0x2A4D` | Read, Write, Notify | 输入/输出报告 |
| **HID Information** | `0x2A4A` | Read | HID 版本、国家码等 |
| **HID Control Point** | `0x2A4C` | Write No Response | 挂起/唤醒 |
| **Boot Keyboard Input** | `0x2A22` | Read, Notify | Boot 协议键盘输入 |
| **Boot Keyboard Output** | `0x2A32` | Read, Write | Boot 协议键盘输出 |

### 3.2 Data RX Service（自定义）

手机写入文字数据的通道。

| 属性 | UUID | 权限 | 说明 |
|-----------|------|-------------|-------------|
| **Primary Service** | `0xA001` | — | BLE HID Data Bridge |
| **Data RX** | `0xA002` | Write, Write No Response | 手机 → WS63：要输出的文字 |
| **Status TX** | `0xA003` | Notify | WS63 → 手机：状态反馈 |

### 3.3 广播参数

| 参数 | 值 |
|-----------|-------|
| 类型 | Connectable Undirected |
| 设备名 | `ble_hid_kb` |
| Appearance | `0x03C1` (Keyboard) |
| 广播携带 Service UUID | `0x1812`（HID Service） |
| 广播间隔 | 20 ms (min) ~ 30 ms (max) |

---

## 4. HID 键盘报告描述符

### 4.1 输入报告格式（8 字节）

| 偏移 | 位 | 字段 | 说明 |
|------|------|-------|-------------|
| 0 | 7-0 | 修饰键 | 位掩码: LCTRL,LSHIFT,LALT,LGUI,RCTRL,RSHIFT,RALT,RGUI |
| 1 | 7-0 | 保留 | 固定 0x00 |
| 2 | 7-0 | 按键 1 | 第一个按下的键（0 = 无） |
| 3 | 7-0 | 按键 2 | 第二个 |
| 4 | 7-0 | 按键 3 | 第三个 |
| 5 | 7-0 | 按键 4 | 第四个 |
| 6 | 7-0 | 按键 5 | 第五个 |
| 7 | 7-0 | 按键 6 | 第六个 |

按键方法：先发包含键码的报告，立刻再发全零报告（释放所有键）。

### 4.2 USB HID 键码速查表

| 按键 | 键码 | 按键 | 键码 | 按键 | 键码 |
|-----|------|-----|------|-----|------|
| a/A | 0x04 | n/N | 0x11 | 1/! | 0x1E |
| b/B | 0x05 | o/O | 0x12 | 2/@ | 0x1F |
| c/C | 0x06 | p/P | 0x13 | 3/# | 0x20 |
| d/D | 0x07 | q/Q | 0x14 | 4/$ | 0x21 |
| e/E | 0x08 | r/R | 0x15 | 5/% | 0x22 |
| f/F | 0x09 | s/S | 0x16 | 6/^ | 0x23 |
| g/G | 0x0A | t/T | 0x17 | 7/& | 0x24 |
| h/H | 0x0B | u/U | 0x18 | 8/* | 0x25 |
| i/I | 0x0C | v/V | 0x19 | 9/( | 0x26 |
| j/J | 0x0D | w/W | 0x1A | 0/) | 0x27 |
| k/K | 0x0E | x/X | 0x1B | 空格 | 0x2C |
| l/L | 0x0F | y/Y | 0x1C | 回车 | 0x28 |
| m/M | 0x10 | z/Z | 0x1D | 退格 | 0x2A |

大写 = 修饰键 0x02（左 Shift）+ 对应字母键码。

---

## 5. 数据协议（手机 ↔ WS63）

### 5.1 Data RX 特征值（0xA002）

手机向此特征值写入 UTF-8 编码的文字。

**格式：** 原始 UTF-8 字节，无帧头帧尾。

**示例：** 手机写入 `Hello` → WS63 收到 `48 65 6C 6C 6F`

### 5.2 Status TX 特征值（0xA003）

WS63 通过 Notify 向手机上报状态。

| 偏移 | 长度 | 字段 |
|--------|--------|-------|
| 0 | 1 | 状态码 |
| 1 | 1 | 附加信息（可选） |

状态码：

| 码 | 含义 |
|---------|---------|
| `0x00` | OK — 文字已发送完毕 |
| `0x01` | 忙 — 上一段文字还在发送中 |
| `0x02` | 错误 — PC 未连接（HID Host 不在线） |
| `0x03` | 溢出 — 文字超过最大长度 |

### 5.3 特殊字符处理

| 字符 | Hex | 动作 |
|-----------|-----|--------|
| `\n` | 0x0A | 回车键 |
| `\t` | 0x09 | Tab 键 |
| `\b` | 0x08 | 退格键 |

---

## 6. WS63 软件设计

### 6.1 任务划分

| 任务 | 优先级 | 栈大小 | 职责 |
|------|----------|-------|------|
| `ble_hid_main_task` | 26 | 0x1200 | 入口：初始化 HID 服务 + Data RX 服务 |
| `hid_kb_task` | 25 | 0x1000 | 文字→HID 转换 + 逐字发送报告 |
| BLE 协议栈 | (内部) | — | GATT / GAP / L2CAP |

### 6.2 状态机

```
                    ┌──────────┐
                    │   IDLE   │
                    └────┬─────┘
                         │ BLE 初始化完成
                         ▼
                    ┌──────────┐
                    │ ADVERTISING│  广播 HID Service
                    └──┬───┬───┘
                       │   │
          PC 连上      │   │  手机连上
                       ▼   ▼
                    ┌──────────────┐
                    │  CONNECTED   │  一方或两方已连接
                    └──┬───┬───┬───┘
                       │   │   │
       手机写文字      │   │   │  PC 断开
                       │   │   └──► 重新广播 HID
                       ▼   │
                    ┌──────────────┐
                    │  SENDING     │  逐字转 HID 报告 → 发送
                    └──────┬───────┘
                           │ 全部发送完毕
                           ▼
                    ┌──────────────┐
                    │   IDLE       │  等待下一段文字
                    └──────────────┘
```

### 6.3 关键数据结构

```c
/* HID 键盘输入报告 (8 字节) */
typedef struct __attribute__((packed)) {
    uint8_t modifiers;   /* 修饰键位掩码 */
    uint8_t reserved;    /* 0x00 */
    uint8_t keys[6];     /* 最多 6 个同时按下的键码 */
} hid_kb_report_t;

/* 字符到 HID 键码映射 */
typedef struct {
    uint8_t  utf8_seq[4];   /* UTF-8 字节序列 */
    uint8_t  utf8_len;      /* UTF-8 序列长度 */
    uint8_t  hid_code;      /* USB HID usage ID */
    bool     needs_shift;   /* 是否需要 Shift */
} hid_keymap_entry_t;
```

### 6.4 关键 API

```c
/* HID 键盘服务 */
errcode_t ble_hid_kb_init(void);              /* 注册 HID GATT 服务 */
errcode_t ble_hid_kb_start_adv(void);         /* 启动 HID 广播 */
errcode_t ble_hid_kb_send_report(const hid_kb_report_t *report);
bool ble_hid_kb_is_pc_connected(void);

/* Data RX 服务 */
errcode_t ble_hid_data_init(void);            /* 注册 Data RX GATT 服务 */
errcode_t ble_hid_data_send_status(uint8_t code);

/* 文字转 HID 引擎 */
errcode_t hid_engine_send_text(const uint8_t *utf8_text, uint16_t len);

/* 键码查表 */
uint8_t hid_keycode_from_ascii(char c, bool *needs_shift);
```

### 6.5 打字流程

```
对输入文字的每个 UTF-8 字符:
  1. 在键码映射表中查找 HID 键码
  2. 如果需要大写 / Shift 字符:
     a. 发送报告: [0x02, 0x00, 键码, 0,0,0,0,0]  (Shift + 按键)
     b. 发送报告: [0x00, 0x00, 0,0,0,0,0,0]        (全部释放)
  3. 普通字符:
     a. 发送报告: [0x00, 0x00, 键码, 0,0,0,0,0]   (单独按键)
     b. 发送报告: [0x00, 0x00, 0,0,0,0,0,0]        (全部释放)
  4. 每个报告对之间延迟 ~5ms（HID 轮询速率）
  5. 连续相同字符中间插入一个空报告
```

### 6.6 Kconfig 配置

```
Application → Enable Sample → Enable the Sample of BT
  → Support BLE Sample → Support BLE HID Keyboard Sample
```

| 配置项 | 类型 | 默认值 | 说明 |
|--------|------|---------|-------------|
| `CONFIG_SAMPLE_SUPPORT_BLE_HID_KB` | bool | n | 启用 BLE HID Keyboard 示例 |
| `CONFIG_BLE_HID_DEVICE_NAME` | string | `ble_hid_kb` | 广播设备名 |
| `CONFIG_BLE_HID_MAX_TEXT_LEN` | int | 128 | 单次最大文字长度 |
| `CONFIG_BLE_HID_REPORT_DELAY_MS` | int | 5 | HID 报告间隔 |

### 6.7 代码文件清单

```
src/application/samples/bt/ble/ble_hid_kb/
├── CMakeLists.txt
├── Kconfig
├── README.md
├── SDD.md
├── inc/
│   ├── ble_hid_kb.h          (HID 键盘服务接口)
│   ├── ble_hid_kb_adv.h      (HID 广播配置)
│   ├── ble_hid_data.h        (Data RX 服务接口)
│   └── hid_keycode.h         (键码映射表 + 引擎)
└── src/
    ├── ble_hid_kb.c           (HID GATT 服务实现: Report Map, Report, ...)
    ├── ble_hid_kb_adv.c       (广播数据配置: Appearance=键盘)
    ├── ble_hid_data.c         (Data RX GATT 服务实现)
    ├── hid_keycode.c          (ASCII/UTF-8 → HID 键码映射)
    └── ble_hid_sample.c       (主入口: app_run, 任务创建)
```

---

## 7. 测试用例

### 7.1 功能测试

#### TC-H01：PC 识别 WS63 为键盘

| 项目 | 内容 |
|------|------|
| **测试目的** | 验证 PC 蓝牙设置中 WS63 显示为键盘设备 |
| **前置条件** | WS63 烧录 BLE HID KB 固件，PC 蓝牙开启 |
| **测试步骤** | 1. WS63 上电<br>2. PC：蓝牙设置 → 添加设备<br>3. 查找 `ble_hid_kb` |
| **预期结果** | PC 将 `ble_hid_kb` 列为 **键盘** 设备（不是"其他设备"） |
| **优先级** | P0 |

#### TC-H02：手机发文字 → PC 输出

| 项目 | 内容 |
|------|------|
| **测试目的** | 验证端到端文字传输 |
| **前置条件** | PC 和手机都已连接 WS63 |
| **测试步骤** | 1. 手机 BLE 调试助手 → 连接 → 向 0xA002 写入 `Hello`<br>2. PC 打开记事本，确保焦点<br>3. 等待输出 |
| **预期结果** | 记事本中出现 `Hello` |
| **优先级** | P0 |

#### TC-H03：大小写混合

| 项目 | 内容 |
|------|------|
| **测试目的** | 验证大小写正确处理 |
| **测试步骤** | 手机写入 `Hello World` |
| **预期结果** | PC 记事本显示 `Hello World`，大小写正确 |
| **优先级** | P1 |

#### TC-H04：特殊字符

| 项目 | 内容 |
|------|------|
| **测试目的** | 验证空格、回车、Tab、标点 |
| **测试步骤** | 手机写入 `Hi!\nTab:\ttest` |
| **预期结果** | PC 显示 `Hi!` 换行 `Tab:` Tab `test` |
| **优先级** | P1 |

#### TC-H05：长文字（>100 字符）

| 项目 | 内容 |
|------|------|
| **测试目的** | 验证长文字不截断 |
| **测试步骤** | 手机写入 500 字符 |
| **预期结果** | 全部输出，WS63 返回 OK |
| **优先级** | P1 |

### 7.2 异常测试

#### TC-H06：PC 未连接时手机发数据

| 项目 | 内容 |
|------|------|
| **测试目的** | 验证 HID Host 不在线时的处理 |
| **测试步骤** | PC 未配对，手机直接写文字 |
| **预期结果** | WS63 返回状态 `0x02`（HID 未连接） |
| **优先级** | P1 |

#### TC-H07：PC 中途断开

| 项目 | 内容 |
|------|------|
| **测试目的** | 验证发送中途 PC 断开的恢复 |
| **测试步骤** | 手机发长文字，中途 PC 断开蓝牙 |
| **预期结果** | WS63 停止发送，恢复广播，通知手机 |
| **优先级** | P1 |

### 7.3 测试环境

| 项目 | 要求 |
|------|-------------|
| WS63 硬件 | HiHope_NearLink_DK3863E_V03 |
| 固件 | `ws63-liteos-app`，`CONFIG_SAMPLE_SUPPORT_BLE_HID_KB=y` |
| 手机 | Android 8.0+，BLE 调试助手 |
| PC | Windows 10/11，内置蓝牙 |
| 串口工具 | HiSpark Studio 监视器 |

### 7.4 测试用例矩阵

| 编号 | 名称 | 类别 | 优先级 |
|------|------|----------|----------|
| TC-H01 | PC 识别 WS63 为键盘 | 功能 | P0 |
| TC-H02 | 手机发文字 → PC 输出 | 功能 | P0 |
| TC-H03 | 大小写混合 | 功能 | P1 |
| TC-H04 | 特殊字符 | 功能 | P1 |
| TC-H05 | 长文字 | 功能 | P1 |
| TC-H06 | PC 未连时发数据 | 异常 | P1 |
| TC-H07 | PC 中途断开 | 异常 | P1 |

---

## 附录 A: 参考资料

| 文档 | 来源 |
|----------|--------|
| HID over GATT Profile v1.0 | bluetooth.com |
| USB HID Usage Tables v1.22 | usb.org |
| BLE UART 透传（参考实现） | `vendor/DFRobot_Beetle_WS63/demo/beetle_ble_uart/` |
| BLE WiFi CFG Server（GATT 模式参考） | `src/application/samples/bt/ble/ble_wifi_cfg_server/` |

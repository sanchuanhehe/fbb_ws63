# BLE HID Button — 软件方案设计文档 (SDD)

**文档版本：** V1.1
**适用范围：** WS63 系列
**目标平台：** Huawei LiteOS (v208.5.0)，RISC-V rv32imc
**Profile：** BLE HID over GATT (HoG)，Boot Keyboard
**状态：** ✅ 已实现并验证

---

## 1. 概述

### 1.1 背景

WS63 作为 BLE HID 键盘外设。板载物理按键 GPIO 13 按下时发送标准 HID 键盘输入报告，松开时发送释放报告。接收端（PC/手机）通过蓝牙连接后识别为标准蓝牙键盘，无需驱动。

### 1.2 系统拓扑

```
┌──────────┐  GPIO 13     ┌──────────────┐  BLE HID Report  ┌──────────────┐
│ 物理按键  │ ───────────► │    WS63      │ ────────────────► │  PC / 手机    │
│          │  按下/松开    │ HID Keyboard │  [0,0,KEY,...]   │ 蓝牙键盘设备  │
└──────────┘              └──────────────┘                   └──────────────┘
```

### 1.3 设计约束

- `app_run()` 回调在调度器启动前执行，不能直接调用 `osal_msleep()` 等阻塞函数
- BLE 初始化必须延迟到调度器运行后，因此入口回调只创建任务，实际工作在任务中完成
- Kconfig `int` 类型不支持 `0x` 十六进制前缀，键码默认值使用十进制

---

## 2. 系统架构

### 2.1 软件模块

```
ble_hid_btn_sample.c             ← 入口（app_run）
  └→ hid_main_task               ← 主任务：延迟 3s → 初始化 HID + 广播 → 创建按键任务
       ├── ble_hid_btn.c          ← HID GATT 服务（0x1812 + 6 个特征值 + 报告描述符）
       ├── ble_hid_adv.c          ← BLE 广播（Appearance=Keyboard，Service UUID=0x1812）
       └→ hid_btn_task            ← 按键任务：50Hz 轮询 GPIO 13，消抖，长按重复
```

### 2.2 任务划分

| 任务 | 优先级 | 栈 | 职责 |
|------|--------|------|------|
| `hid_main` | 26 | 0x1000 | 延迟初始化：等待调度器就绪 → `ble_hid_btn_init()` → `ble_hid_adv_start()` → 创建 `hid_btn` |
| `hid_btn` | 30 | 0x400 | 轮询 GPIO 13（20ms 周期），消抖，按键/松开/长按检测 → 发送 HID 报告 |
| BLE 协议栈 | 内部 | — | GATT / GAP |

### 2.3 初始化时序

```
app_run(ble_hid_btn_sample_entry)     ← 调度器启动前调用
  └→ osal_kthread_create(hid_main_task)  ← 创建任务（不阻塞）
       └→ [调度器启动]
       └→ osal_msleep(3000)              ← 等待 BLE 协议栈就绪
       └→ ble_hid_btn_init()             ← 注册 GATT 回调 + enable_ble + 注册服务
       └→ ble_hid_adv_start()            ← 设置广播数据 + 启动广播
       └→ osal_kthread_create(hid_btn_task)  ← 启动按键检测
```

---

## 3. BLE GATT 服务定义

### 3.1 HID Service（0x1812）

实际注册了 6 个特征值，使用同步 API：

| 属性 | UUID | 权限 | API | 说明 |
|------|------|------|-----|------|
| **Primary Service** | `0x1812` | — | `gatts_add_service_sync` | HID |
| **Protocol Mode** | `0x2A4E` | Read, Write No Resp | `gatts_add_characteristic_sync` | 默认 0 (Boot) |
| **Report Map** | `0x2A4B` | Read | `gatts_add_characteristic_sync` | 63 字节键盘描述符 |
| **Boot KB Input** | `0x2A22` | Read, Notify | + `gatts_add_descriptor_sync` (CCCD) | 按键输入 → Notify |
| **Boot KB Output** | `0x2A32` | Read, Write, Write No Resp | `gatts_add_characteristic_sync` | LED 输出 |
| **HID Information** | `0x2A4A` | Read | `gatts_add_characteristic_sync` | bcdHID=0x0111, Country=0, Flags=0x03 |
| **HID Control Point** | `0x2A4C` | Write No Resp | `gatts_add_characteristic_sync` | 挂起/唤醒 |

`gatts_start_service` 在所有特征值添加完成后调用一次。

### 3.2 广播参数（实际 API）

使用 `bts_le_gap.h` 中的标��� API：

```c
// 广播数据设置
gap_ble_set_adv_data(BLE_ADV_ID, &cfg);  // cfg 含 adv_data + scan_rsp

// 广播参数
gap_ble_adv_params_t param;
param.min_interval     = 0x20;    // 20 ms
param.max_interval     = 0x30;    // 30 ms
param.duration         = 0;       // 持续广播
param.adv_type         = GAP_BLE_ADV_CONN_SCAN_UNDIR;
param.channel_map      = 0x07;    // 37/38/39
param.adv_filter_policy = GAP_BLE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;
param.peer_addr.type   = BT_ADDRESS_TYPE_PUBLIC_DEVICE_ADDRESS;

// 开始广播
gap_ble_start_adv(BLE_ADV_ID);
```

| 参数 | 值 |
|------|-----|
| 设备名 | `ble_hid_btn`（Kconfig 可配） |
| Appearance | `0x03C1` (Keyboard)，在 ADV 数据中 `04 19 C1 03` |
| Service UUID in ADV | `03 02 12 18`（HID 0x1812） |
| 广播类型 | Connectable Undirected |

---

## 4. HID 报告定义

### 4.1 输入报告格式（固定 8 字节）

```c
typedef struct __attribute__((packed)) {
    uint8_t modifiers;   // 固定 0
    uint8_t reserved;    // 固定 0x00
    uint8_t keys[6];     // keys[0] = 键码，其余 0
} hid_kb_report_t;
```

**按下：** `[0x00, 0x00, KEYCODE, 0x00, 0x00, 0x00, 0x00, 0x00]`
**松开：** `[0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]`

通过 `gatts_notify_indicate` 发送，使用 Boot KB Input 的 value handle。

### 4.2 键码配置（Kconfig int，十进制）

| 场景 | 十进制 | 十六进制 | 说明 |
|------|--------|----------|------|
| PPT 翻页 | 78 | 0x4E | Page Down（默认） |
| PPT 上一页 | 75 | 0x4B | Page Up |
| 空格 | 44 | 0x2C | 暂停/播放 |
| 回车 | 40 | 0x28 | 确认/快门 |
| 音量+ | 233 | 0xE9 | Volume Up |
| 音量- | 234 | 0xEA | Volume Down |

---

## 5. 按键检测逻辑

### 5.1 GPIO 配置

- GPIO 13：输入模式 + `PIN_PULL_TYPE_UP`（内部上拉）
- 按键对地短接 → 低电平 = 按下
- 包含 2 次消抖（需连续 2 次读到相同值才确认状态变化）

### 5.2 按键状态机

```
RELEASED ──[连续 2 次 LOW]──► PRESSED ──→ 发送按下报告 [0,0,KEY,0...]
                                   │
                                   ├──[按住 < 500ms]──→ 无额外动作
                                   │
                                   └──[按住 ≥ 500ms]──→ 发送释放报告
                                                        → 10ms 延迟
                                                        → 发送按下报告
                                                        → 标记 repeat_sent
                                                        → 后续不再重复

PRESSED ──[连续 2 次 HIGH]──► RELEASED ──→ 发送释放报告 [0,0,0,0...]
```

### 5.3 长按重复（`CONFIG_BLE_HID_BTN_LONGPRESS=y` 时启用）

- 按住 > 500ms → 发送一次"释放+重新按下"模拟按键重复
- 之后不再重复（避免 PC 收到连续字符流）
- 松开后才重置状态

---

## 6. Kconfig 配置

```
Application → Enable Sample → Enable the Sample of BT
  → Support BLE Sample → Support BLE HID Button Sample
    → BLE HID Button Configuration
```

| 配置项 | 类型 | 默认值 | 说明 |
|--------|------|--------|------|
| `CONFIG_SAMPLE_SUPPORT_BLE_HID_BTN_SAMPLE` | bool | n | 启用 BLE HID Button |
| `CONFIG_BLE_HID_BTN_PIN` | int | 13 | 按键 GPIO |
| `CONFIG_BLE_HID_BTN_KEYCODE` | int | 78 | HID 键码（十进制，默认 78=PageDown） |
| `CONFIG_BLE_HID_BTN_LONGPRESS` | bool | y | 长按重复 |
| `CONFIG_BLE_HID_DEVICE_NAME` | string | `ble_hid_btn` | 设备名 |

> 注意：Kconfig `int` 类型不支持 `0x` 前缀，键码必须使用十进制值。

---

## 7. 手机端验证方法

1. 打开 BLE 调试助手 → 扫描 → 找到 `ble_hid_btn` → **CONNECT**
2. 展开 Service `0x1812` → 找到 `0x2A22`（Boot Keyboard Input）
3. **打开"接收通知数据"开关**（等价于向 CCCD `0x2902` 写入 `01 00`）
4. 按下 GPIO 13 → 手机实时收到 Notify：`00 00 4E 00 00 00 00 00`
5. 松开 GPIO 13 → 手机实时收到 Notify：`00 00 00 00 00 00 00 00`
6. 不打开通知开关 → 点击"读取"获得的是当前静态快照

---

## 8. 测试用例

### 8.1 功能测试

| 编号 | 名称 | 步骤 | 预期 | 优先级 |
|------|------|------|------|--------|
| TC-B01 | 手机识别 HID 服务 | 手机 BLE 扫描 → 连接 `ble_hid_btn` → 展开 Service | 看到 `0x1812` HID Service + 6 个特征值 | P0 |
| TC-B02 | 按下发送键码 | 打开 Notify → 按下 GPIO 13 | 实时收到 `00 00 4E 00 00 00 00 00` | P0 |
| TC-B03 | 松开释放 | 按住 → 松开 | 收到全零释放报告 `00 00 00 00 00 00 00 00` | P0 |
| TC-B04 | PC 识别为键盘 | PC 蓝牙设置 → 添加设备 → 搜索 | 找到 `ble_hid_btn`，设备类型显示"键盘" | P0 |
| TC-B05 | 长按不重复 2 次以上 | 按住 1 秒以上 | 只触发一次重复（释放+按下），之后不再重复 | P1 |
| TC-B06 | 换键码 | Kconfig 改成 40 (Enter) → 重编烧录 | 按下收到 `00 00 28 00 00 00 00 00` | P1 |
| TC-B07 | 断连重连 | 手机断开 → 重新连接 | 功能恢复正常 | P1 |
| TC-B08 | 连续按压 100 次 | 快速按 100 次 | 全部正常输出，无遗漏 | P2 |

### 8.2 异常测试

| 编号 | 名称 | 步骤 | 预期 |
|------|------|------|------|
| TC-B09 | 未开启 Notify 时读取 | 不开启通知开关 → 点"读取" | 返回当前静态快照（按下时读到键码，松开时读到全零） |
| TC-B10 | 按键消抖 | 慢速按下（模拟抖动） | 不产生误触发，一次按下只发一次报告 |

### 8.3 串口验证关键字

| 阶段 | 串口输出 |
|------|---------|
| 入口 | `[ble_hid_btn_sample] entry` |
| 主任务启动 | `[ble_hid_btn_sample] main task start` |
| HID 服务注册完成 | `[ble_hid_btn] init done` |
| 广播启动 | `[ble_hid_adv] started ret=0` |
| 按键任务就绪 | `[ble_hid_btn_sample] ready` |
| 按键按下 | `[ble_hid_btn_sample] press key=0x4E` |
| 按键松开 | `[ble_hid_btn_sample] release` |

---

## 9. 代码文件清单

```
src/application/samples/bt/ble/ble_hid_btn/
├── CMakeLists.txt               ← SOURCES + PRIVATE_HEADER (PARENT_SCOPE)
├── Kconfig                      ← 5 个配置项
├── SDD.md                       ← 本文档
├── inc/
│   ├── ble_hid_btn.h             ← HID 服务接口 + hid_kb_report_t 结构体
│   └── ble_hid_adv.h             ← 广播接口
└── src/
    ├── ble_hid_btn_sample.c      ← 主入口：app_run → hid_main_task → hid_btn_task
    ├── ble_hid_btn.c             ← HID GATT 服务：6 个特征值 + 73 字节报告描述符
    └── ble_hid_adv.c             ← 广播：ADV 数据（HID UUID + Keyboard Appearance）
```

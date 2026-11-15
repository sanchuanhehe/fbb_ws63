# BLE HID Button 示例

## 功能说明

WS63 作为蓝牙键盘外设。按下板载物理按键（默认 GPIO 13），WS63 通过 BLE HID 协议发送标准键盘输入报告，接收端（PC / 手机）蓝牙连接后识别为标准键盘设备，无需额外驱动。

支持特性：
- 单键 HID 键盘，键码通过 Kconfig 可配（默认 Page Down = PPT 翻页）
- 长按自动重复（按住 > 500ms 模拟一次重复）
- 按键消抖（2 次连续采样确认）
- 断连自动重新广播

## 硬件连接

| GPIO | 连接 | 说明 |
|------|------|------|
| 13 | 按键 → GND | 按下为低电平，内部上拉 |
| 开发板 USB | PC / 手机 | 烧录 + 串口监控 |

## 启用与编译

### 1. Kconfig 配置

HiSpark Studio → Kconfig 或命令行 `menuconfig`：

```
Application → Enable Sample → Enable the Sample of BT → [*]
  → Support BLE Sample → [*]
    → Support BLE HID Button Sample → [*]
```

**注意：** `application/samples/bt/Kconfig` 和 `application/samples/bt/ble/Kconfig` 都是 Kconfig `choice` 互斥块。启用 HID Button 时必须显式禁用其他 BT sample（如 BLE WIFI CFG、SLE、CHBA）。

可选配置项（在 `BLE HID Button Configuration` 子菜单）：

| 配置项 | 默认值 | 说明 |
|--------|--------|------|
| `CONFIG_BLE_HID_BTN_PIN` | 13 | 按键 GPIO Pin |
| `CONFIG_BLE_HID_BTN_KEYCODE` | 78 | HID 键码（十进制，78=Page Down，44=Space，40=Enter，233=Vol+，234=Vol-） |
| `CONFIG_BLE_HID_BTN_LONGPRESS` | y | 长按自动重复 |
| `CONFIG_BLE_HID_DEVICE_NAME` | `ble_hid_btn` | BLE 广播设备名 |

### 2. 编译

```bash
cd src
python build.py -c ws63-liteos-app
```

或使用 fbb：

```bash
fbb build --clean ws63-liteos-app
```

### 3. 烧录

HiSpark Studio → 程序加载 → `output/ws63/fwpkg/ws63-liteos-app/ws63-liteos-app_all.fwpkg`

或：

```bash
fbb flash ws63-liteos-app --port COM<N> --json-summary
```

### 4. 验证广播

烧录后串口（115200 baud）应看到：

```
[ble_hid_btn_sample] entry
[ble_hid_btn_sample] main task start
[ble_hid_btn] init begin
[ble_hid_btn] registering callbacks
[ble_hid_btn] enabling BLE
[ble_hid_btn] reg server
[ble_hid_btn] server_id=1
[ble_hid_btn] building service
[ble_hid_btn] svc hdl=14
[ble_hid_btn] chara uuid=0x2A4E val_hdl=16 ret=0
[ble_hid_btn] chara uuid=0x2A4B val_hdl=18 ret=0
[ble_hid_btn] chara uuid=0x2A22 val_hdl=20 ret=0    ← Boot Keyboard Input
[ble_hid_btn] chara uuid=0x2A32 val_hdl=23 ret=0
[ble_hid_btn] chara uuid=0x2A4A val_hdl=25 ret=0
[ble_hid_btn] chara uuid=0x2A4C val_hdl=27 ret=0
[ble_hid_btn] init done
[ble_hid_adv] started ret=0
[ble_hid_btn_sample] ready
[ble_hid_btn_sample] task started, pin=13 keycode=0x4E
```

6 个特征值全部 `ret=0` 说明 HID 服务注册成功，广播已启动。

---

## 测试验证

### 手机端验证

1. 打开 **BLE 调试助手**（华为应用市场可下载）
2. 点击 **扫描** → 在设备列表找到 **`ble_hid_btn`**
3. 点击 **CONNECT** → 连接成功
4. 展开 Service **`0x1812`**（HID Service）
5. 找到 Characteristic **`0x2A22`**（Boot Keyboard Input）
6. **打开"接收通知数据"开关**

#### 测试按键：

| 操作 | 手机收到 Notify（HEX） | 含义 |
|------|----------------------|------|
| 按下 GPIO 13 | `00 00 4E 00 00 00 00 00` | Page Down 键按下 |
| 松开 GPIO 13 | `00 00 00 00 00 00 00 00` | 所有键释放 |

- 如果不打开"接收通知数据"开关，点"读取"只能拿到当前静态快照（按下时读到键码，松开时读到全零）
- 打开通知开关后，每次按键状态变化都会实时推送

### PC 端验证

1. PC 打开 **蓝牙设置** → 添加蓝牙设备
2. 搜索到 **`ble_hid_btn`** → 设备类型显示**"键盘"**
3. 点击配对 → 连接成功
4. 打开 **记事本** 或 **PPT** → 确保窗口有焦点
5. 按下 GPIO 13 → 触发 Page Down（默认键码）

### 改键码测试

在 Kconfig 中修改 `CONFIG_BLE_HID_BTN_KEYCODE`（十进制），重编烧录后验证：

| 键码值 | 场景 | 手机端验证 |
|--------|------|-----------|
| 78 | PPT 翻页（默认） | 收到 `00 00 4E 00 00 00 00 00` |
| 44 | 空格/暂停播放 | 收到 `00 00 2C 00 00 00 00 00` |
| 40 | 回车/快门 | 收到 `00 00 28 00 00 00 00 00` |
| 233 | 音量+ | 收到 `00 00 E9 00 00 00 00 00` |
| 234 | 音量- | 收到 `00 00 EA 00 00 00 00 00` |

---

## 串口日志速查

| 阶段 | 关键字 | 说明 |
|------|--------|------|
| 入口 | `[ble_hid_btn_sample] entry` | 启动回调执行 |
| 主任务 | `main task start` | 主任务开始（含 3s 延迟等 BLE 就绪） |
| 回调注册 | `registering callbacks` | GAP + GATT 回调注册完成 |
| BLE 使能 | `enabling BLE` | `enable_ble()` 调用 |
| 服务注册 | `building service` | 开始添加特征值 |
| 初始化完成 | `init done` | 6 个特征值全部 `ret=0` |
| 广播启动 | `[ble_hid_adv] started ret=0` | 广播已开始 |
| 按键就绪 | `ready` + `task started, pin=13` | 按键任务开始轮询 |
| 按键按下 | `press key=0x4E` | 按键按下，发送报告 |
| 按键松开 | `release` | 按键松开，发送释放报告 |
| 长按重复 | `repeat key=0x4E` | 长按触发重复 |
| PC 连接 | `conn: id=0 state=1 pair=0 disc=0` | HID Host 已连接 |
| PC 断开 | `PC disconnected, re-advertising...` | 自动恢复广播 |

---

## BLE GATT 协议摘要

| 属性 | UUID | 权限 | 说明 |
|------|------|------|------|
| HID Service | `0x1812` | — | Human Interface Device |
| Protocol Mode | `0x2A4E` | Read, Write No Response | 默认 0 (Boot) |
| Report Map | `0x2A4B` | Read | 63 字节键盘报告描述符 |
| **Boot Keyboard Input** | **`0x2A22`** | Read, **Notify** | **按键输入报告（8 字节）** |
| Boot Keyboard Output | `0x2A32` | Read, Write | LED 输出 |
| HID Information | `0x2A4A` | Read | HID 版本信息 |
| HID Control Point | `0x2A4C` | Write No Response | 挂起/唤醒 |

- `0x2A22` 是发送按键报告的特征值，需要启用 CCCD Notify 才能收到实时推送
- 输入报告固定 8 字节：`[modifiers(1), reserved(1), keys[6]]`
- 按下：`[00, 00, KEYCODE, 00, 00, 00, 00, 00]`
- 松开：`[00, 00, 00, 00, 00, 00, 00, 00]`

## 代码文件

| 文件 | 说明 |
|------|------|
| `ble_hid_btn_sample.c` | 主入口：app_run → hid_main_task（延迟初始化）→ hid_btn_task（按键检测） |
| `ble_hid_btn.c` | HID GATT 服务：6 个特征值 + 标准键盘报告描述符 |
| `ble_hid_adv.c` | 广播：ADV 数据含 HID UUID + Keyboard Appearance |

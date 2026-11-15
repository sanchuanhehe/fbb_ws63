# BLE WiFi 配网示例

## 功能说明

通过手机 BLE 连接 WS63 设备，将 WiFi 路由器的 SSID 和密码发送给设备，设备自动完成 WiFi 入网。支持以下特性（通过 Kconfig 配置）：

- **NV 持久化**：配网成功后凭证保存到 Flash，断电重启自动重连，无需重复配网
- **配网超时保护**：超时后自动停止 BLE 广播，进入低功耗状态
- **失败自动重试**：密码错误/信号丢失等失败后自动重新广播，等待再次配网
- **LED 状态指示**：快闪（广播中）→ 慢闪（连接中）→ 常亮（成功）→ 3闪灭（失败）

## 硬件环境

| 项目 | 说明 |
|------|------|
| 开发板 | HiHope_NearLink_DK3863E_V03 或兼容 WS63 开发板 |
| 手机 | Android 8.0+（安装 BLE 调试助手）或 iPhone（安装 LightBlue） |
| 路由器 | 2.4GHz WiFi，支持 WPA2-PSK |

## 启用与编译

### 1. 启用配置

通过 HiSpark Studio → Kconfig 或命令行 `menuconfig` 启用：

```
Application → Enable Sample → Enable the Sample of WIFI
  → Sample → Support BLE WIFI CFG Sample

Application → Enable Sample → Enable the Sample of BT
  → Support BLE Sample → Support BLE WIFI CFG Sample
```

**注意**：`application/samples/bt/Kconfig` 和 `application/samples/bt/ble/Kconfig` 都是 Kconfig `choice` 块，启用 BLE WiFi CFG 时必须显式禁用互斥选项（SLE / CHBA / 其他 BLE sample），否则 Kconfig 会选择默认项而非你想要的选项。

可选 Kconfig 配置项（在 `BLE WiFi Provisioning Configuration` 子菜单）：

| 配置项 | 类型 | 默认值 | 说明 |
|--------|------|--------|------|
| `CONFIG_BLE_PROV_NV_ENABLE` | bool | y | Flash 保存凭证，重启自动重连 |
| `CONFIG_BLE_PROV_AUTO_RECONNECT` | bool | y | 启动时优先用 NV 中的凭证连接 WiFi |
| `CONFIG_BLE_PROV_LED_ENABLE` | bool | n | 启用 LED 状态指示灯 |
| `CONFIG_BLE_PROV_LED_PIN` | int | 7 | LED 连接的 GPIO Pin |
| `CONFIG_BLE_PROV_TIMEOUT_SEC` | int | 60 | 配网超时（秒） |
| `CONFIG_BLE_PROV_MAX_RETRIES` | int | 3 | WiFi 连接失败后最大重试次数 |

### 2. 编译

```bash
cd src
python build.py -c ws63-liteos-app
```

### 3. 烧录

通过 HiSpark Studio → 程序加载 → 选择 `output/ws63/fwpkg/ws63-liteos-app/ws63-liteos-app_all.fwpkg`

### 4. 验证广播

串口（115200 baud）复位后应看到：

```
[wifi_cfg_server] init ok
Ble Adv State:0
```

---

## 手机端操作指南

### 1. 扫描并连接

1. 打开 **BLE 调试助手**（华为应用市场可下载）或 **nRF Connect**（Google Play）
2. 点击"扫描"搜索 BLE 设备
3. 找到设备名 **`ble_wifi_config`** 的设备
4. 点击 **CONNECT**

连接成功后 WS63 串口打印：

```
connect state change conn_id: 0, status: 0x1, pair_status:1, disc_reason 0x0
```

### 2. 进入写入界面

1. 连接后会自动发现 Service，找到 UUID **`0xFD5C`** 的 Service
2. 展开后找到 Characteristic **`0xFD5E`**（WiFi 凭证通道）
3. 点击 `0xFD5E` 进入数据发送界面

### 3. 计算 HEX 数据

WiFi 凭证格式：**SSID（32 字节）+ 密码（32 字节）= 共 64 字节**

SSID 和密码均为原始字节（SSID 支持 UTF-8 中文），不足 32 字节时尾部补 `0x00`。

**HEX 生成方法（Python / 在线工具 / ADB）：**

```python
import codecs

ssid = "你的WiFi名称"       # ← 改成你的 WiFi SSID
password = "你的WiFi密码"    # ← 改成你的 WiFi 密码

ssid_bytes = ssid.encode('utf-8')[:32].ljust(32, b'\x00')
pwd_bytes = password.encode('ascii')[:32].ljust(32, b'\x00')
hex_data = (ssid_bytes + pwd_bytes).hex().upper()
print(hex_data)  # 复制这 128 个字符
```

**示例：**

| SSID | 密码 | 前 32 字节（SSID） | 后 32 字节（Password） |
|------|------|---------------------|------------------------|
| `MyWiFi` | `12345678` | `4D7957694669` + 26×`00` | `3132333435363738` + 24×`00` |
| `我的WiFi` | `abc12345` | `E68891E79A8457694669` + 22×`00` | `6162633132333435` + 24×`00` |

### 4. 发送凭证

1. 确认 **"HEX"** 开关已打开（蓝色/高亮）
2. 发送模式选 **"单次发送"**
3. 在输入框中粘贴第 3 步生成的 **128 个 HEX 字符**
4. 点击 **"发送"**

> **注意**：BLE 调试助手会按 MTU（约 20 字节/包）自动拆分成 3-4 次 Write Without Response 发送。WS63 端会自动拼接碎片，串口会打印 4 次 `frag write: pos=... total=...` 确认拼接进度。

WS63 收到数据后串口打印：

```
[wifi_cfg_server]ReceiveWriteReqCallback--server_id:1 conn_id:0
data_len:20 data:
xx xx xx xx xx ...
status: 0x0
[BGLE_WIFI] frag write: pos=0 len=20 total=20
...（共约 4 次，累计到 total=64）
APP|wifi cfg flag:1, wifi list flag:0.
```

### 5. 等待配网结果

设备自动执行 **WiFi 扫描 → 连接 → DHCP**，约 15-30 秒。

**配网成功**串口输出：

```
[BGLE_WIFI] expected_ssid :你的WiFi名称
[BGLE_WIFI] bgwc_connection_changed enter.
STA DHCP start.
STA DHCP Succ, IP=192.168.x.x
result code:0.
[BGLE_WIFI] SUCCESS (attempt 1).
[PROV_NV] credentials saved ok
[BGLE_WIFI] Credentials saved to NV.
```

手机端 `0xFD5F` 会收到 Notify：**`01 00`**（成功）。

---

## 错误码与排查

手机端 `0xFD5F` 收到的 Notify 格式为 `01 XX`，`XX` 为错误码：

| 错误码 | 含义 | 串口关键字 | 排查建议 |
|--------|------|-----------|---------|
| `01 00` | 配网成功 | `STA DHCP Succ` | — |
| `01 01` | 未找到 SSID | `Do not find AP, try again!` | 确认路由器开启 2.4GHz，SSID 拼写正确 |
| `01 02` | 密码错误 | `STA ASSOC Fail, errcode=2.` | 确认密码正确（注意大小写） |
| `01 03` | DHCP 失败 | `STA DHCP Fail.` | 确认路由器 DHCP 服务正常 |
| `01 04` | 信标丢失 | `STA ASSOC Fail, errcode=4.` | 靠近路由器，信号太弱 |
| `01 05` | 其他错误 | — | 检查串口完整日志 |

配网失败后设备会自动重试（默认 3 次），串口打印：

```
[BGLE_WIFI] FAILED attempt 1/3, errcode=1.
```

每次失败后 LED 快闪 3 次，然后恢复 BLE 广播，等待手机重新发送凭证。

---

## 重启后自动重连

配网成功后，WS63 将凭证写入 NV（Flash）。断电重启后将**跳过 BLE 广播，直接用 NV 中的凭证连接 WiFi**：

```
[BGLE_WIFI] NV configured, loading credentials...
[BGLE_WIFI] loaded ssid=你的WiFi名称
...
STA DHCP Succ, IP=192.168.x.x
```

此时手机扫描不到 `ble_wifi_config`（因为没有开广播）。

**清除 NV 凭证**：如果需要重新配网（例如更换了路由器），可通过 AT 命令或调用 `ble_wifi_prov_nv_clear()` 清除已保存的凭证。

---

## BLE GATT 协议摘要

| 属性 | UUID | 权限 | 用途 |
|------|------|------|------|
| Service | `0xFD5C` | — | BLE WiFi 配置服务 |
| Characteristic | `0xFD5D` | Write, Notify | 控制通道 |
| Characteristic | `0xFD5E` | Write, Indicate | **写入 WiFi 凭证** |
| Characteristic | `0xFD5F` | Write, Notify | 结果上报 / AP 列表请求 |
| Descriptor (CCCD) | `0x2902` | Read, Write | 启用 Notify/Indicate |

WiFi 凭证写入 `0xFD5E` 的数据格式（固定 64 字节）：

| 偏移 | 长度 | 内容 |
|------|------|------|
| 0 | 32 | WiFi SSID（UTF-8，不足补 `0x00`） |
| 32 | 32 | WiFi 密码（ASCII，不足补 `0x00`） |

---

## 代码文件

| 文件 | 说明 |
|------|------|
| `ble_wifi_cfg_sample.c` | 配网主任务：状态机、WiFi 扫描连接、重试管理、NV 集成 |
| `ble_wifi_prov_nv.c/h` | NV 存储模块：凭证保存/读取/清除 |
| `ble_wifi_prov_led.c/h` | LED 指示模块：独立任务驱动 GPIO 状态灯 |
| `../bt/ble/ble_wifi_cfg_server/` | BLE GATT 服务端（复用，本示例不修改） |

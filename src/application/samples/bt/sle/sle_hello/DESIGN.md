# SLE Hello World 应用设计文档

## 1. 概述

本应用基于 HiSpark WS63 平台的 SLE（星闪低功耗）协议栈，实现一个简单的 "Hello World" 数据收发示例：

- **Server 端（Terminal/Peripheral）**：启动 SLE 广播，等待 Client 连接，连接成功后发送 `"hello world"` 字符串
- **Client 端（Grant/Central）**：扫描并发现 Server，建立连接，接收 Server 发送的 `"hello world"` 数据，并通过串口打印输出

## 2. 目录结构

```
src/application/samples/bt/sle/sle_hello/
├── 设计文档.md                  # 本设计文档
├── Kconfig                      # Kconfig 配置
├── CMakeLists.txt               # 顶层编译脚本
├── sle_hello_server/
│   ├── src/
│   │   ├── sle_hello_server.h   # Server 头文件（UUID定义、API声明）
│   │   ├── sle_hello_server.c   # Server 核心逻辑（SSAP服务注册、数据发送）
│   │   └── sle_hello_server_adv.c # Server 广播配置（广播参数、广播数据）
│   └── CMakeLists.txt
├── sle_hello_client/
│   ├── src/
│   │   ├── sle_hello_client.h   # Client 头文件（API声明）
│   │   └── sle_hello_client.c   # Client 核心逻辑（扫描、连接、服务发现、接收数据）
│   └── CMakeLists.txt
└── sle_hello.c                  # 应用入口（app_run、任务创建、串口初始化）
```

## 3. 整体架构

```
┌─────────────────────┐         ┌─────────────────────┐
│   SLE Hello Server  │  SLE RF │  SLE Hello Client   │
│   (WS63 Board A)    │ ◄──────► │   (WS63 Board B)    │
├─────────────────────┤         ├─────────────────────┤
│ 1. enable_sle()     │         │ 1. enable_sle()     │
│ 2. 注册 announce/   │         │ 2. 注册 seek/       │
│    conn/SSAPS 回调   │         │    conn/SSAPC 回调   │
│ 3. 注册 SSAP Server  │         │ 3. start_seek()     │
│    添加 Service +    │         │    扫描广播数据       │
│    Property          │         │ 4. 匹配"hello_server"│
│ 4. start_announce()  │         │    → connect        │
│    广播名称:          │         │ 5. pair + exchange  │
│    "hello_server"    │         │    info             │
│ 5. 等待连接...       │         │ 6. find_structure   │
│ 6. 连接成功 → 配对   │         │    发现 Property     │
│ 7. 配对完成 → 发送   │         │ 7. 收到 Notification │
│    notify("hello     │────────►│    → 串口打印输出     │
│    world")           │         │                     │
└─────────────────────┘         └─────────────────────┘
```

## 4. SSAP 服务定义

### 4.1 UUID 分配

| 名称 | UUID | 说明 |
|------|------|------|
| App UUID | `{0x12, 0x34}` | Server 应用标识 |
| Service UUID | `0x3333` (16-bit, 基于 standard base UUID) | Hello 服务 |
| Property UUID | `0x3434` (16-bit, 基于 standard base UUID) | 数据上报属性 |
| Descriptor UUID | `SSAP_DESCRIPTOR_USER_DESCRIPTION` | 用户描述描述符 |

### 4.2 Property 权限定义

```c
// Property 权限: READ | NOTIFY (Server → Client 单向通知)
#define SLE_HELLO_PROPERTY_PERMISSIONS  (SSAP_PERMISSION_READ)
#define SLE_HELLO_PROPERTY_OPERATE_INDICATION \
    (SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_NOTIFY)

// Descriptor 权限: READ
#define SLE_HELLO_DESCRIPTOR_PERMISSIONS  (SSAP_PERMISSION_READ)
#define SLE_HELLO_DESCRIPTOR_OPERATE_INDICATION \
    (SSAP_OPERATE_INDICATION_BIT_READ)
```

### 4.3 数据传输方式

Server 通过 **SSAP Notification** 向 Client 发送数据：
- 调用 `ssaps_notify_indicate()` 或 `ssaps_notify_indicate_by_uuid()`
- Client 在 `notification_cb` 回调中接收数据

## 5. Server 端详细设计

### 5.1 文件: `sle_hello_server.h`

```c
// Service UUID
#define SLE_HELLO_SERVICE_UUID      0x3333
// Property UUID  
#define SLE_HELLO_NTF_REPORT_UUID   0x3434

// 广播名称
#define SLE_HELLO_SERVER_NAME       "hello_server"

// Server 初始化
errcode_t sle_hello_server_init(void);

// 发送数据
errcode_t sle_hello_server_send_data(const uint8_t *data, uint16_t len);

// 查询连接状态
uint16_t sle_hello_server_is_connected(void);
```

### 5.2 文件: `sle_hello_server.c` — 初始化流程

```
sle_hello_server_init()
  ├── enable_sle()                          // 使能 SLE 协议栈
  ├── sle_hello_announce_register_cbks()    // 注册广播回调
  │     └── sle_announce_seek_register_callbacks(&seek_cbks)
  │           ├── announce_enable_cb        // 广播使能回调
  │           ├── announce_disable_cb       // 广播停止回调
  │           └── announce_terminal_cb      // 广播终止回调
  ├── sle_hello_conn_register_cbks()        // 注册连接回调
  │     └── sle_connection_register_callbacks(&conn_cbks)
  │           ├── connect_state_changed_cb  // 连接状态变更回调
  │           └── pair_complete_cb          // 配对完成回调
  ├── sle_hello_ssaps_register_cbks()       // 注册 SSAP Server 回调
  │     └── ssaps_register_callbacks(&ssaps_cbk)
  │           ├── add_service_cb / add_property_cb / add_descriptor_cb
  │           ├── start_service_cb / mtu_changed_cb
  │           └── read_request_cb / write_request_cb
  ├── sle_hello_server_add()                // 添加 SSAP 服务
  │     ├── ssaps_register_server()         // 注册 Server (App UUID)
  │     ├── ssaps_add_service_sync()        // 添加 Service (0x3333)
  │     ├── ssaps_add_property_sync()       // 添加 Property (0x3434, READ|NOTIFY)
  │     ├── ssaps_add_descriptor_sync()     // 添加 Descriptor
  │     └── ssaps_start_service()           // 启动服务
  └── sle_hello_server_adv_init()           // 初始化广播 (在 adv.c 中)
        ├── sle_set_announce_param()        // 设置广播参数
        ├── sle_set_announce_data()         // 设置广播/扫描响应数据
        └── sle_start_announce()            // 启动广播
```

### 5.3 广播参数配置

| 参数 | 值 | 说明 |
|------|-----|------|
| announce_mode | `SLE_ANNOUNCE_MODE_CONNECTABLE_SCANABLE` | 可连接可扫描 |
| announce_handle | `1` | 广播句柄 |
| announce_interval | `0xC8` (25ms) | 广播间隔 |
| conn_interval | `0x64` (12.5ms) | 连接间隔 |
| conn_supervision_timeout | `0x1F4` (5000ms) | 超时时间 |
| local_name | `"hello_server"` | 本地设备名（放在扫描响应数据中） |
| own_addr | `{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}` | 固定本地地址 |

### 5.4 连接状态变更回调逻辑

```
connect_state_changed_cbk(conn_id, addr, conn_state, pair_state, disc_reason)
  ├── SLE_ACB_STATE_CONNECTED:
  │     └── 保存 conn_id → g_sle_conn_hdl
  └── SLE_ACB_STATE_DISCONNECTED:
        ├── 清除 g_sle_conn_hdl
        └── 通知主任务重新启动广播
```

### 5.5 配对完成回调逻辑

```
pair_complete_cbk(conn_id, addr, status)
  ├── 保存配对状态
  ├── 调用 ssaps_set_info(mtu=520, version=1)
  └── 发送 "hello world":
        └── ssaps_notify_indicate(g_server_id, g_sle_conn_hdl, &param)
              其中 param.handle = g_property_handle
                  param.type = SSAP_PROPERTY_TYPE_VALUE
                  param.value = "hello world"
                  param.value_len = 11
```

### 5.6 数据发送函数

```c
errcode_t sle_hello_server_send_data(const uint8_t *data, uint16_t len)
{
    ssaps_ntf_ind_t param = {0};
    param.handle = g_property_handle;
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.value = (uint8_t *)data;
    param.value_len = len;
    return ssaps_notify_indicate(g_server_id, g_sle_conn_hdl, &param);
}
```

## 6. Client 端详细设计

### 6.1 文件: `sle_hello_client.h`

```c
// Client 初始化
void sle_hello_client_init(ssapc_notification_callback notification_cb,
                           ssapc_indication_callback indication_cb);

// 启动扫描
void sle_hello_client_start_scan(void);
```

### 6.2 文件: `sle_hello_client.c` — 初始化流程

```
sle_hello_client_init(notification_cb, indication_cb)
  ├── osal_msleep(5000)                           // 等待 SLE 核心就绪
  ├── sle_hello_seek_cbk_register()               // 注册扫描回调
  │     └── sle_announce_seek_register_callbacks(&seek_cbk)
  │           ├── sle_enable_cb                   // SLE 使能回调
  │           ├── seek_enable_cb / seek_disable_cb
  │           └── seek_result_cb                  // 扫描结果回调
  ├── sle_hello_connect_cbk_register()            // 注册连接回调
  │     └── sle_connection_register_callbacks(&conn_cbk)
  │           ├── connect_state_changed_cb
  │           └── pair_complete_cb
  ├── sle_hello_ssapc_cbk_register()              // 注册 SSAP Client 回调
  │     └── ssapc_register_callbacks(&ssapc_cbk)
  │           ├── exchange_info_cb
  │           ├── find_structure_cb
  │           ├── ssapc_find_property_cbk
  │           ├── find_structure_cmp_cb
  │           ├── notification_cb  ← 用户传入（接收 "hello world"）
  │           └── indication_cb    ← 用户传入
  └── enable_sle()                                 // 使能 SLE
```

### 6.3 扫描流程

```
sle_enable_cb(status)
  └── sle_hello_client_start_scan()
        ├── sle_set_seek_param(&param)
        │     ├── own_addr_type = 0
        │     ├── seek_phys = 1
        │     ├── seek_interval = 100 (1.25ms * 100 = 125ms)
        │     └── seek_window = 100
        └── sle_start_seek()

seek_result_cb(seek_result_data)
  ├── 检查 data 中是否包含 "hello_server"
  ├── 匹配成功 → 保存对方地址 → sle_stop_seek()
  └── seek_disable_cb → sle_connect_remote_device(&remote_addr)
```

### 6.4 连接与配对流程

```
connect_state_changed_cbk(conn_id, addr, conn_state, pair_state, disc_reason)
  ├── SLE_ACB_STATE_CONNECTED:
  │     ├── 保存 conn_id
  │     └── sle_pair_remote_device(&remote_addr)  // 发起配对
  └── SLE_ACB_STATE_DISCONNECTED:
        ├── 清除配对信息
        └── sle_hello_client_start_scan()          // 重新扫描

pair_complete_cbk(conn_id, addr, status)
  └── ssapc_exchange_info_req(mtu=520, version=1) // 交换 MTU 信息

exchange_info_cb(client_id, conn_id, param, status)
  └── ssapc_find_structure(type=PROPERTY, start=1, end=0xFFFF) // 发现服务

find_structure_cb(client_id, conn_id, service, status)
  └── 保存 service 信息 (start_hdl, end_hdl, uuid)

find_property_cbk(client_id, conn_id, property, status)
  └── 保存 property 的 handle → 后续 write 操作需要

find_structure_cmp_cb(...)
  └── 服务发现完成（此时已可接收 notification）
```

### 6.5 数据接收与串口打印

```c
// notification_cb — Server 通过 notify 发送数据时触发
void sle_hello_notification_cb(uint8_t client_id, uint16_t conn_id,
                                ssapc_handle_value_t *data, errcode_t status)
{
    osal_printk("[SLE Hello Client] Received data: %s\r\n", data->data);
    // 直接通过调试串口(osal_printk)打印
}

// indication_cb — Server 通过 indicate 发送数据时触发  
void sle_hello_indication_cb(uint8_t client_id, uint16_t conn_id,
                              ssapc_handle_value_t *data, errcode_t status)
{
    osal_printk("[SLE Hello Client] Received indication: %s\r\n", data->data);
}
```

## 7. 应用入口设计

### 7.1 文件: `sle_hello.c`

```c
// Server 任务
void *sle_hello_server_task(const char *arg) {
    sle_hello_server_init();   // 初始化 SLE Server + 启动广播
    // 不需要循环 — 所有操作由回调驱动
    // 连接成功后 pair_complete_cbk 中自动发送 "hello world"
    return NULL;
}

// Client 任务
void *sle_hello_client_task(const char *arg) {
    sle_hello_client_init(sle_hello_notification_cb, sle_hello_indication_cb);
    // 不需要循环 — 所有操作由回调驱动
    return NULL;
}

// 应用入口
static void sle_hello_entry(void) {
    osal_kthread_lock();
#if defined(CONFIG_SLE_HELLO_SERVER)
    task = osal_kthread_create(sle_hello_server_task, 0, "SLEHelloServer", 0x1000);
#elif defined(CONFIG_SLE_HELLO_CLIENT)
    task = osal_kthread_create(sle_hello_client_task, 0, "SLEHelloClient", 0x1000);
#endif
    osal_kthread_set_priority(task, 28);
    osal_kthread_unlock();
}

app_run(sle_hello_entry);
```

## 8. 状态机

### 8.1 Server 状态机

```
[初始化] → enable_sle() → [SLE已使能]
  → 注册回调 → 添加服务 → start_announce() → [广播中]
  → Client连接 → [已连接] → 配对 → [已配对]
  → 发送 notify("hello world") → [数据已发送]
  → Client断开 → [已断开] → start_announce() → [广播中] (循环)
```

### 8.2 Client 状态机

```
[初始化] → sleep(5s) → enable_sle() → [SLE已使能]
  → start_seek() → [扫描中]
  → 发现 "hello_server" → stop_seek() → connect → [已连接]
  → pair → [已配对] → exchange_info → [MTU已交换]
  → find_structure → [服务发现中] → find_property → [属性已发现]
  → 收到 notification("hello world") → osal_printk() → [数据已打印]
  → Server断开 → [已断开] → start_seek() → [扫描中] (循环)
```

## 9. Kconfig 配置

```kconfig
# 顶层 gate config
config SLE_HELLO_SUPPORT
    bool
    prompt "Enable SLE Hello World sample."
    default n
    help
        This option enables the SLE Hello World sample application.

# Server/Client 二选一
choice
    prompt "Select SLE Hello role"
    default CONFIG_SLE_HELLO_SERVER
    depends on SLE_HELLO_SUPPORT
    config CONFIG_SLE_HELLO_SERVER
        bool "SLE Hello Server"
    config CONFIG_SLE_HELLO_CLIENT
        bool "SLE Hello Client"
endchoice

# 衍生配置: 广播/扫描能力标记
config SUPPORT_SLE_PERIPHERAL
    bool
    default y if CONFIG_SLE_HELLO_SERVER

config SUPPORT_SLE_CENTRAL
    bool
    default y if CONFIG_SLE_HELLO_CLIENT
```

## 10. 编译配置

### 10.1 顶层 CMakeLists.txt

```cmake
if(DEFINED CONFIG_SLE_HELLO_SERVER)
    add_subdirectory(sle_hello_server)
elseif(DEFINED CONFIG_SLE_HELLO_CLIENT)
    add_subdirectory(sle_hello_client)
endif()
```

### 10.2 集成到现有 Kconfig 体系

需要在 `src/application/samples/bt/sle/Kconfig` 的 `choice` 块中增加一个选项：

```kconfig
config SAMPLE_SUPPORT_SLE_HELLO_SAMPLE
    bool "Support SLE Hello World sample."
```

并 `osource` 引入 `sle_hello/Kconfig`。

## 11. 关键 API 引用

| API | 所属模块 | 用途 |
|-----|---------|------|
| `enable_sle()` | sle_device_discovery | 使能/上电 SLE 协议栈 |
| `sle_announce_seek_register_callbacks()` | sle_device_discovery | 注册广播/扫描回调 |
| `sle_set_announce_param()` | sle_device_discovery | 设置广播参数 |
| `sle_set_announce_data()` | sle_device_discovery | 设置广播数据/扫描响应数据 |
| `sle_start_announce()` | sle_device_discovery | 启动广播 |
| `sle_set_seek_param()` | sle_device_discovery | 设置扫描参数 |
| `sle_start_seek()` / `sle_stop_seek()` | sle_device_discovery | 启动/停止扫描 |
| `sle_connection_register_callbacks()` | sle_connection_manager | 注册连接回调 |
| `sle_connect_remote_device()` | sle_connection_manager | 发起连接 |
| `sle_pair_remote_device()` | sle_connection_manager | 发起配对 |
| `sle_remove_paired_remote_device()` | sle_connection_manager | 移除配对设备 |
| `ssaps_register_server()` | sle_ssap_server | 注册 SSAP Server |
| `ssaps_add_service_sync()` | sle_ssap_server | 添加 Service |
| `ssaps_add_property_sync()` | sle_ssap_server | 添加 Property |
| `ssaps_add_descriptor_sync()` | sle_ssap_server | 添加 Descriptor |
| `ssaps_start_service()` | sle_ssap_server | 启动服务 |
| `ssaps_set_info()` | sle_ssap_server | 设置 MTU/版本信息 |
| `ssaps_notify_indicate()` | sle_ssap_server | Server 发送通知/指示数据 |
| `ssaps_register_callbacks()` | sle_ssap_server | 注册 SSAP Server 回调 |
| `ssapc_register_callbacks()` | sle_ssap_client | 注册 SSAP Client 回调 |
| `ssapc_exchange_info_req()` | sle_ssap_client | Client 发起 MTU 交换 |
| `ssapc_find_structure()` | sle_ssap_client | Client 发起服务发现 |
| `osal_printk()` | soc_osal | 调试日志打印（串口输出） |
| `osal_kthread_create()` | soc_osal | 创建 OS 线程 |
| `app_run()` | app_init | 注册应用入口函数 |

## 12. 与现有 sample 的差异说明

相比于 `sle_uart` sample（完整的 UART ↔ SLE 双向透传），本应用精简如下：

1. **不需要硬件 UART** — Client 使用 `osal_printk` 直接打印到调试串口，不依赖 UART 外设
2. **单向数据流** — 仅 Server → Client 的单向 notify，不需要 Client → Server 的 write
3. **固定发送内容** — 配对完成后自动发送固定字符串 `"hello world"`，不涉及动态数据传输
4. **无需消息队列** — Server 不需要 msg queue 处理断开重连，逻辑更简单
5. **不包含低延迟模式** — 不涉及 `sle_low_latency` 相关配置

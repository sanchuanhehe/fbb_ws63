# SLE UART Bridge 透传应用设计文档

## 1. 概述

本应用基于 HiSpark WS63 平台的 SLE 协议栈，将 UART 串口与 SLE 无线链路桥接，实现双向数据透传：

- **Server 端（Peripheral）**：启动 SLE 广播，等待 Client 连接。连接建立后，UART 收到的数据通过 SSAP Notification 推送给 Client；收到 Client 的 Write Request 数据则从 UART 吐出
- **Client 端（Central）**：扫描并发现 Server，建立连接。连接建立后，UART 收到的数据通过 SSAP Write Request 发送给 Server；收到 Server 的 Notification 数据则从 UART 吐出

> 在 hello 三部曲（连接、通知、读写）的基础上，组合 UART 驱动和消息队列，构建完整的双向无线串口线。

## 2. 目录结构

```
src/application/samples/bt/sle/sle_uart/
├── DESIGN.md                       # 本设计文档
├── README.md                       # 快速上手指南
├── Kconfig                         # 可配置参数定义
├── CMakeLists.txt                  # 顶层编译脚本
├── sle_uart.c                      # 应用入口（任务创建、UART初始化、主循环）
├── sle_uart_server/
│   ├── CMakeLists.txt
│   ├── src/
│   │   ├── sle_uart_server.h       # Server 头文件（UUID、权限、API）
│   │   ├── sle_uart_server.c       # Server 核心逻辑（服务注册、通知发送、连接管理）
│   │   ├── sle_uart_server_adv.h   # 广播头文件
│   │   └── sle_uart_server_adv.c   # 广播配置
└── sle_uart_client/
    ├── CMakeLists.txt
    ├── src/
    │   ├── sle_uart_client.h       # Client 头文件（API）
    │   └── sle_uart_client.c       # Client 核心逻辑（扫描、连接、写请求、服务发现）
```

## 3. 整体架构

```
┌──────────────────────────────────────┐       ┌──────────────────────────────────────┐
│          SLE UART Server             │       │          SLE UART Client              │
│          (WS63 Board A)              │       │          (WS63 Board B)               │
├──────────────────────────────────────┤       ├──────────────────────────────────────┤
│                                      │       │                                      │
│  PC_A ──UART──► UART RX ISR          │       │  PC_B ──UART──► UART RX ISR          │
│                  │                   │       │                  │                   │
│                  ▼                   │       │                  ▼                   │
│              消息队列                  │       │              消息队列                  │
│                  │                   │       │                  │                   │
│                  ▼                   │       │                  ▼                   │
│          ssaps_notify_indicate()     │       │          ssapc_write_req()           │
│                  │                   │       │                  │                   │
│                  ▼                   │  SLE  │                  ▼                   │
│  ╔══════════════════════════════╗    │  RF   │  ╔══════════════════════════════╗    │
│  ║          SLE 链路             ║◄───┼───────┼──►║          SLE 链路             ║   │
│  ╚══════════════════════════════╝    │       │  ╚══════════════════════════════╝    │
│                  ▲                   │       │                  ▲                   │
│    write_request_cb                  │       │    notification_cb                  │
│                  │                   │       │                  │                   │
│                  ▼                   │       │                  ▼                   │
│            uapi_uart_write()         │       │            uapi_uart_write()         │
│                  │                   │       │                  │                   │
│  PC_A ◄──UART──┘                    │       │  PC_B ◄──UART──┘                    │
│                                      │       │                                      │
└──────────────────────────────────────┘       └──────────────────────────────────────┘
```

**两条独立链路：**

| 链路 | 方向 | ISR（生产者） | 任务（消费者） | SLE API |
|------|------|:---:|:---:|---------|
| **A** | Server → Client | `sle_uart_server_rx_handler` | `sle_uart_server_task` | `ssaps_notify_indicate()` |
| **B** | Client → Server | `sle_uart_client_rx_handler` | `sle_uart_client_task` | `ssapc_write_req()` |

## 4. SSAP 服务定义

### 4.1 UUID 分配

| 名称 | UUID | 说明 |
|------|------|------|
| App UUID | `{0x12, 0x34}` | Server 应用标识 |
| Service UUID | `0x2222` (16-bit, 基于 standard base UUID) | 透传服务 |
| Property UUID | `0x2323` (16-bit, 基于 standard base UUID) | 数据收发属性 |
| Descriptor UUID | `SSAP_DESCRIPTOR_USER_DESCRIPTION` | 用户描述描述符 |

### 4.2 Property 权限定义

```c
// 数据 Property: READ | WRITE | NOTIFY（双向）
#define SLE_UART_SRV_PROPERTIES  (SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE)
#define SLE_UART_SRV_OPERATION   (SSAP_OPERATE_INDICATION_BIT_READ | \
                                  SSAP_OPERATE_INDICATION_BIT_WRITE | \
                                  SSAP_OPERATE_INDICATION_BIT_NOTIFY)
```

与 hello 不同：hello 只需 READ | NOTIFY（单向），透传需要 WRITE（Client→Server 方向需写权限）。

### 4.3 数据传输方式

| 方向 | 方式 | API | 说明 |
|------|------|-----|------|
| Server → Client | Notification | `ssaps_notify_indicate()` | 无需 Client 逐包确认，延迟低、吞吐高 |
| Client → Server | Write Request | `ssapc_write_req()` | Client 主动写入 Server 端 Property |

> 不使用 Indication：Indication 要求 Client 回复确认，Server 必须等待确认才能发下一包，吞吐量不足 Notification 的一半。

## 5. Server 端详细设计

### 5.1 文件: `sle_uart_server.h`

```c
#define SLE_UART_SERVER_SERVICE    0x2222   // Service UUID
#define SLE_UART_SERVER_NTF_REPORT  0x2323  // Property UUID

errcode_t sle_uart_server_init(ssaps_read_request_callback  read_cb,
                               ssaps_write_request_callback write_cb);
errcode_t sle_uart_server_send_notification(const uint8_t *data, uint16_t len);
uint16_t sle_uart_server_is_connected(void);
```

### 5.2 文件: `sle_uart_server.c` — 初始化流程

```
sle_uart_server_init(read_cb, write_cb)
  ├── g_read_cb = read_cb; g_write_cb = write_cb      // 保存用户回调
  ├── enable_sle()                                      // 使能 SLE 协议栈
  ├── sle_uart_server_announce_register_cbks()          // 注册广播回调
  │     └── sle_announce_seek_register_callbacks(&seek_cbks)
  ├── sle_uart_server_conn_register_cbks()              // 注册连接回调
  │     └── sle_connection_register_callbacks(&conn_cbks)
  │           ├── connect_state_changed_cb
  │           └── pair_complete_cb
  ├── sle_uart_server_ssaps_register_cbks()             // 注册 SSAP Server 回调
  │     └── ssaps_register_callbacks(&ssaps_cbk)
  │           ├── add_service_cb / add_property_cb / add_descriptor_cb
  │           ├── start_service_cb / mtu_changed_cb
  │           ├── read_request_cb  = g_read_cb          // 透传场景不处理读
  │           └── write_request_cb = g_write_cb         // 收到 Client 数据 → UART TX
  └── sle_uart_server_add()                              // 添加 SSAP 服务
        ├── ssaps_register_server({0x12, 0x34})
        ├── ssaps_add_service_sync(UUID=0x2222)
        ├── ssaps_add_property_sync(UUID=0x2323, READ|WRITE|NOTIFY)
        ├── ssaps_add_descriptor_sync(USER_DESCRIPTION)
        └── ssaps_start_service()
```

### 5.3 广播参数配置

| 参数 | 值 | 说明 |
|------|-----|------|
| announce_mode | `SLE_ANNOUNCE_MODE_CONNECTABLE_SCANABLE` | 可连接可扫描 |
| announce_handle | `1` | 广播句柄 |
| announce_interval | `0x64` (12.5ms) | 广播间隔 |
| conn_interval | `CONFIG_SLE_UART_CONN_INTERVAL × 1.25ms` | 通过 Kconfig 配置 |
| local_name | `"uart_server"` | 本地设备名 |
| own_addr | `{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}` | 固定本地地址 |

### 5.4 连接状态变更回调逻辑

```
connect_state_changed_cbk(conn_id, addr, conn_state, pair_state, disc_reason)
  ├── SLE_ACB_STATE_CONNECTED:
  │     ├── g_sle_conn_hdl = conn_id
  │     └── g_connected = true
  └── SLE_ACB_STATE_DISCONNECTED:
        ├── g_sle_conn_hdl = 0
        ├── g_connected = false
        ├── 清空消息队列残留数据（非阻塞读取丢弃）
        └── sle_start_announce(SLE_ADV_HANDLE_DEFAULT)  // 重新广播
```

### 5.5 配对完成回调逻辑

```
pair_complete_cbk(conn_id, addr, status)
  └── ssaps_set_info(mtu=CONFIG_SLE_UART_MTU_SIZE, version=1)
```

> 与 hello 不同：配对完成后不立即发送数据，而是等待 UART 数据到达后才触发通知。

### 5.6 通知发送函数

```c
errcode_t sle_uart_server_send_notification(const uint8_t *data, uint16_t len)
{
    ssaps_ntf_ind_t param = {0};
    uint8_t send_buf[len];

    param.handle = g_property_handle;
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.value = send_buf;
    param.value_len = len;
    memcpy_s(send_buf, len, data, len);
    return ssaps_notify_indicate(g_server_id, g_sle_conn_hdl, &param);
}
```

### 5.7 接收 Client 方向数据

```c
// write_request_cb — Client 通过 Write Request 发送数据时触发
// 该回调在任务上下文中执行，直接调用 uapi_uart_write 吐出
static void sle_uart_server_write_cbk(uint8_t server_id, uint16_t conn_id,
                                       ssaps_req_write_cb_t *write_cb_para,
                                       errcode_t status)
{
    if (write_cb_para->value == NULL || write_cb_para->length == 0) return;
    uapi_uart_write(CONFIG_UART_BUS_ID, write_cb_para->value,
                    write_cb_para->length, 0);
}
```

## 6. Client 端详细设计

### 6.1 文件: `sle_uart_client.h`

```c
void sle_uart_client_init(ssapc_notification_callback  notification_cb,
                          ssapc_write_result_callback   write_cfm_cb);
uint16_t sle_uart_client_is_connected(void);
```

### 6.2 文件: `sle_uart_client.c` — 初始化流程

```
sle_uart_client_init(notification_cb, write_cfm_cb)
  ├── enable_sle()                                       // 使能 SLE 协议栈
  ├── 注册扫描回调 (sle_announce_seek_register_callbacks)
  │     ├── sle_enable_cb → sle_uart_client_start_scan()
  │     └── seek_result_cb → 匹配 "uart_server" → stop_seek → connect
  ├── 注册连接回调 (sle_connection_register_callbacks)
  │     ├── connect_state_changed_cb
  │     └── pair_complete_cb → ssapc_exchange_info_req()
  └── 注册 SSAPC 回调 (ssapc_register_callbacks)
        ├── exchange_info_cb → ssapc_find_structure()
        ├── find_structure_cb → 保存 service 信息
        ├── find_property_cbk → 保存 handle（供后续 write_req 使用）
        ├── find_structure_cmp_cb → 服务发现完成
        ├── notification_cb  ← 用户传入（收到 Server 数据 → UART TX）
        └── write_result_cb  ← 用户传入（透传场景无需处理）
```

### 6.3 扫描流程

```
sle_enable_cb(status)
  └── sle_uart_client_start_scan()
        ├── sle_set_seek_param(own_addr_type=0, seek_phys=1,
        │                      seek_interval=100, seek_window=100)
        └── sle_start_seek()

seek_result_cb(data)
  ├── 解析 adv_report，检查是否包含 "uart_server"
  ├── 匹配成功 → 保存地址 → sle_stop_seek()
  └── seek_disable_cb → sle_connect_remote_device(&remote_addr)
```

### 6.4 连接与配对流程

```
connect_state_changed_cbk(conn_id, addr, conn_state, pair_state, disc_reason)
  ├── SLE_ACB_STATE_CONNECTED:
  │     ├── g_conn_id = conn_id
  │     └── sle_pair_remote_device(&remote_addr)
  └── SLE_ACB_STATE_DISCONNECTED:
        ├── g_conn_id = 0
        ├── sle_remove_paired_remote_device(addr)
        └── sle_uart_client_start_scan()                 // 重新扫描

pair_complete_cbk(conn_id, addr, status)
  └── ssapc_exchange_info_req(mtu=CONFIG_SLE_UART_MTU_SIZE, version=1)

exchange_info_cb(client_id, conn_id, param, status)
  └── ssapc_find_structure(type=PROPERTY, start=1, end=0xFFFF)

find_structure_cb → 保存 service handle
find_property_cbk → 保存 property handle（供 write_req 使用）
find_structure_cmp_cb → 打印 "=== bridge ready ==="
```

### 6.5 接收 Server 方向数据

```c
// notification_cb — Server 通过 Notification 推送数据时触发
void sle_uart_client_notification_cb(uint8_t client_id, uint16_t conn_id,
                                      ssapc_handle_value_t *data, errcode_t status)
{
    if (status != ERRCODE_SLE_SUCCESS || data == NULL || data->data_len == 0) return;
    uapi_uart_write(CONFIG_UART_BUS_ID, data->data, data->data_len, 0);
}
```

## 7. 应用入口设计

### 7.1 文件: `sle_uart.c` — 消息队列 + 任务架构

与 hello 不同，透传应用的消息发送不能直接在 ISR 中完成（SLE API 不允许在中断上下文中调用）。因此采用 **ISR → 消息队列 → 任务** 的架构：

```
ISR（快速执行，不阻塞）              任务（可调用 SLE API）
┌──────────────────────┐          ┌──────────────────────────┐
│ uart_rx_handler      │          │ sle_uart_task            │
│   → 校验输入          │          │   → msgq_read_copy(阻塞) │
│   → 检查连接状态      │  msgq    │   → SLE 发送             │
│   → msgq_write_copy ─┼────────► │                          │
└──────────────────────┘          └──────────────────────────┘
```

**Server 任务主循环:**

```
sle_uart_server_task()
  ├── osal_msg_queue_create("sle_uart_srv_msgq", depth=MSGQ_LEN, item=MSGQ_ITEM_SIZE)
  ├── uapi_pin_set_mode / uapi_uart_init()                  // UART 初始化
  ├── sle_uart_server_init(read_cb, write_cb)               // SLE Server 初始化
  ├── sle_uart_server_adv_init()                            // 启动广播
  ├── uapi_uart_register_rx_callback(rx_handler)            // 注册 UART RX 回调
  └── while(1):
        ├── msgq_read_copy(OSAL_WAIT_FOREVER)               // 阻塞等待数据
        └── sle_uart_server_send_notification(data, len)    // SLE 发送
```

**Client 任务主循环:**

```
sle_uart_client_task()
  ├── osal_msg_queue_create("sle_uart_cli_msgq", ...)
  ├── uapi_pin_set_mode / uapi_uart_init()
  ├── sle_uart_client_init(notification_cb, write_cfm_cb)   // SLE Client 初始化
  ├── uapi_uart_register_rx_callback(rx_handler)
  └── while(1):
        ├── msgq_read_copy(OSAL_WAIT_FOREVER)
        ├── g_write_param.data = rx_buf; g_write_param.data_len = rx_len
        └── ssapc_write_req(0, g_conn_id, &g_write_param)
```

**连接状态守护:**

```c
// UART RX 回调（ISR 上下文）— 必须检查连接状态
static void sle_uart_server_rx_handler(const void *buffer, uint16_t length, bool error)
{
    if (error || buffer == NULL || length == 0) return;
    if (!sle_uart_server_is_connected()) return;    // 未连接，丢弃
    osal_msg_queue_write_copy(g_sle_uart_server_msgq_id,
                               (void *)buffer, (uint32_t)length, 0);
}
```

**应用入口:**

```c
static void sle_uart_entry(void)
{
    osal_kthread_lock();
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_UART_SERVER_SAMPLE)
    task = osal_kthread_create(sle_uart_server_task, 0, "SLEUartServer", 0x1000);
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_UART_CLIENT_SAMPLE)
    task = osal_kthread_create(sle_uart_client_task, 0, "SLEUartClient", 0x1000);
#endif
    osal_kthread_set_priority(task, 28);
    osal_kthread_unlock();
}

app_run(sle_uart_entry);
```

### 7.2 消息队列参数

| 参数 | 默认值 | 说明 |
|------|--------|------|
| `SLE_UART_MSGQ_LEN` | 16 | 队列深度，决定突发数据缓冲能力 |
| `SLE_UART_MSGQ_ITEM_SIZE` | 520 | 每项最大字节数，对齐 MTU |
| 总缓冲 | 8320 字节 | 16 × 520 |

使用 `write_copy` / `read_copy` 而非指针传递——UART RX buffer 在回调返回后会被驱动回收，必须立即拷贝。

## 8. 状态机

### 8.1 Server 状态机

```
[初始化] → enable_sle() → [SLE已使能]
  → 注册回调 → 添加服务 → start_announce() → [广播中]
  → Client连接 → [已连接] → 配对 → ssaps_set_info() → [透传就绪]
  → UART 数据到达 → notify → [发送中] → 回到 [透传就绪]
  → Client 数据到达 → write_cb → uart_write → 回到 [透传就绪]
  → Client 断开 → 清空msgq → start_announce() → [广播中] (循环)
```

### 8.2 Client 状态机

```
[初始化] → enable_sle() → [SLE已使能]
  → start_seek() → [扫描中]
  → 发现 "uart_server" → stop_seek() → connect → [已连接]
  → pair → exchange_info → find_structure → find_property → [透传就绪]
  → UART 数据到达 → write_req → [发送中] → 回到 [透传就绪]
  → Server 数据到达 → notification_cb → uart_write → 回到 [透传就绪]
  → Server 断开 → 清空msgq → start_seek() → [扫描中] (循环)
```

## 9. Kconfig 配置

### 9.1 顶层 sample 选择

在 `src/application/samples/bt/sle/Kconfig` 的 `choice` 块中：

```kconfig
config SAMPLE_SUPPORT_SLE_UART_SERVER_SAMPLE
    bool "Support SLE UART Server Sample."

config SAMPLE_SUPPORT_SLE_UART_CLIENT_SAMPLE
    bool "Support SLE UART Client Sample."
```

并通过 `osource` 引入子 Kconfig：

```kconfig
if SAMPLE_SUPPORT_SLE_UART_SERVER_SAMPLE || SAMPLE_SUPPORT_SLE_UART_CLIENT_SAMPLE
osource "application/samples/bt/sle/sle_uart/Kconfig"
endif
```

### 9.2 衍生配置

```kconfig
config SUPPORT_SLE_PERIPHERAL
    bool
    default y if SAMPLE_SUPPORT_SLE_UART_SERVER_SAMPLE

config SUPPORT_SLE_CENTRAL
    bool
    default y if SAMPLE_SUPPORT_SLE_UART_CLIENT_SAMPLE
```

### 9.3 可调参数

| 符号 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `UART_BUS_ID` | int | 1 | 数据 UART 总线 |
| `UART_TXD_PIN` / `UART_RXD_PIN` | int | 15 / 16 | 收发引脚 |
| `SLE_UART_BAUDRATE` | int | 115200 | 波特率 |
| `SLE_UART_RX_BUF_SIZE` | int | 512 | UART RX 缓冲区 |
| `SLE_UART_MSGQ_LEN` | int | 16 | 消息队列深度 |
| `SLE_UART_MSGQ_ITEM_SIZE` | int | 520 | 队列每项大小 |
| `SLE_UART_MTU_SIZE` | int | 520 | SLE MTU |
| `SLE_UART_CONN_INTERVAL` | int | 12 | 连接间隔 (×1.25ms, 范围 6~32) |

## 10. 编译配置

### 10.1 顶层 CMakeLists.txt

```cmake
set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/sle_uart.c)

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_UART_SERVER_SAMPLE)
    add_subdirectory_if_exist(sle_uart_server)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_UART_CLIENT_SAMPLE)
    add_subdirectory_if_exist(sle_uart_client)
endif()
```

### 10.2 集成到现有体系

在 `src/application/samples/bt/sle/CMakeLists.txt` 中增加条件编译：

```cmake
if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_UART_SERVER_SAMPLE)
    add_subdirectory_if_exist(sle_uart)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_UART_CLIENT_SAMPLE)
    add_subdirectory_if_exist(sle_uart)
endif()
```

## 11. 关键 API 引用

| API | 模块 | 用途 |
|-----|------|------|
| `uapi_uart_init()` / `uapi_uart_deinit()` | uart | UART 外设初始化/去初始化 |
| `uapi_uart_register_rx_callback()` | uart | 注册 UART 接收中断回调 |
| `uapi_uart_write()` | uart | UART 发送数据 |
| `uapi_pin_set_mode()` | pinctrl | 设置引脚复用模式 |
| `enable_sle()` | sle_device_discovery | 使能 SLE 协议栈 |
| `sle_set_announce_param()` / `sle_start_announce()` | sle_device_discovery | Server 广播 |
| `sle_set_seek_param()` / `sle_start_seek()` | sle_device_discovery | Client 扫描 |
| `sle_connect_remote_device()` | sle_connection_manager | Client 发起连接 |
| `sle_pair_remote_device()` | sle_connection_manager | 发起配对 |
| `ssaps_register_server()` / `ssaps_add_service_sync()` / `ssaps_add_property_sync()` | sle_ssap_server | Server 服务注册 |
| `ssaps_notify_indicate()` | sle_ssap_server | Server 发送 Notification |
| `ssaps_set_info()` | sle_ssap_server | Server 设置 MTU |
| `ssapc_write_req()` | sle_ssap_client | Client 发送 Write Request |
| `ssapc_exchange_info_req()` | sle_ssap_client | Client 发起 MTU 交换 |
| `ssapc_find_structure()` | sle_ssap_client | Client 服务发现 |
| `osal_msg_queue_create()` / `msgq_write_copy()` / `msgq_read_copy()` | soc_osal | 消息队列（ISR 安全） |
| `osal_kthread_create()` | soc_osal | 创建任务线程 |
| `app_run()` | app_init | 注册应用入口 |

## 12. 与 hello sample 的差异说明

| 差异点 | hello | uart bridge |
|--------|-------|-------------|
| 硬件依赖 | 无（仅调试串口） | 需 UART1 外设 + USB-TTL |
| 数据方向 | 单向 (Server→Client) | 双向 (Server↔Client) |
| 数据内容 | 固定 "hello world" | 动态，由串口数据驱动 |
| SSAP 发送方式 | Notification only | Notification + Write Request |
| Property 权限 | READ | READ \| WRITE (需写权限) |
| 消息队列 | 无 | ISR→msgq→任务 架构 |
| 任务循环 | 无（回调驱动，一次性） | while(1) 阻塞读队列 |
| 连接状态守护 | 无 | ISR 中检查 is_connected() |
| 断连队列清理 | 无 | 断开时非阻塞清空队列 |
| UART 回调 | 无 | UART RX ISR → msgq_write_copy |

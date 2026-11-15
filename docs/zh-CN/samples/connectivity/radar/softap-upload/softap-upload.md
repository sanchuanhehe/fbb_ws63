# SoftAP 上报

> SLP (SparkLink Positioning) Radar + WiFi SoftAP + TCP (Transmission Control Protocol) Socket

> 前置阅读：[基础雷达](../basic/basic-radar.md)、[SoftAP](../../wifi/softap/softap.md)

## 学习目标

- 掌握雷达 + WiFi SoftAP 的组合使用
- 理解 SoftAP 直连场景的适用性
- 能够在 WS63 上实现雷达感知 → 对端直连 → 数据上报

## 规格与功能

雷达感知 → SoftAP 创建热点 → PC 连接热点 → 通过 TCP Socket 接收雷达数据。

| 规格项 | 说明 |
|--------|------|
| 雷达 | SLP Radar，同基础感知 |
| 热点 | SoftAP，SSID (Service Set Identifier)=`WS63-Radar` |
| 传输 | TCP Socket（Server 模式） |
| 对端 | PC 连接热点后自动接收数据 |

程序运行流程：雷达初始化 → SoftAP 启动 + DHCP (Dynamic Host Configuration Protocol) Server → TCP Server 监听 → PC 连接热点 → TCP accept → 雷达结果回调 → `send()` → PC 接收。

## 基本概念

### SoftAP 上报链路

```mermaid
flowchart LR
    Radar[雷达感知] --> CB[结果回调]
    CB --> Pack[打包数据帧]
    Pack --> TCP[TCP Server]
    TCP --> AP[WiFi SoftAP]
    AP --> PC[PC 直连]
```

### STA vs SoftAP

| | WiFi STA (Station) 上报 | WiFi SoftAP 上报 |
|:---|:---|:---|
| 需要路由器 | 是 | 否 |
| 适合 | 远程云端上报 | 本地调试 / 无路由器环境 |
| 对端 | 云端服务器（公网 IP (Internet Protocol)） | PC/手机（直连） |
| 传输 | UDP (User Datagram Protocol)| TCP（可靠） |

## 涉及 API

| API | 用途 |
|-----|------|
| `uapi_radar_register_result_cb()` | Radar 结果回调 |
| `wifi_softap_enable()` | 开启 SoftAP |
| `socket()` / `bind()` / `listen()` / `accept()` | TCP Server |
| `send()` | TCP 发送数据 |

## 案例操作指导


PC 连接 `WS63-Radar` 热点，运行数据接收程序，自动接收雷达数据。

## 关键配置

| 参数 | 值 | 说明 |
|------|-----|------|
| SoftAP SSID | `WS63-Radar` | 热点名称 |
| TCP 端口 | 8888 | 自定义 |
| 最大连接数 | 1 | 雷达调试通常一对一 |

## 代码详解

### SoftAP 启动后创建 TCP Server

```c
static void softap_ready_cb(void)
{
    radar_init();
    radar_start();

    // 创建 TCP Server
    g_tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port   = htons(8888),
        .sin_addr   = { .s_addr = INADDR_ANY },
    };
    bind(g_tcp_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(g_tcp_sock, 1);

    // 等待客户端连接
    g_client_sock = accept(g_tcp_sock, NULL, NULL);
    printf("[Radar] client connected\n");
}
```

### 雷达结果回调中推送

```c
static void radar_result_cb(radar_result_t *result)
{
    if (g_client_sock < 0) return;  // 客户端未连接

    radar_frame_t frame = {
        .target_count      = result->target_count,
        .is_human_presence = result->is_human_presence,
    };
    memcpy(frame.targets, result->targets,
           result->target_count * sizeof(radar_target_info_t));

    send(g_client_sock, &frame, sizeof(frame), 0);
}
```

### 对端断开处理

```c
if (send_ret <= 0) {
    // 客户端断开
    close(g_client_sock);
    g_client_sock = -1;
    // 继续 accept 等待新客户端
    g_client_sock = accept(g_tcp_sock, NULL, NULL);
}
```

---


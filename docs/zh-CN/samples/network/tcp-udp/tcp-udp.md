# TCP/UDP

> lwIP (Lightweight IP (Internet Protocol)) Socket API

> 前置阅读：[STA (Station) 连接](../../connectivity/wifi/sta/sta-connect.md)

## 学习目标

- 理解 lwIP Socket API 与标准 POSIX (Portable Operating System Interface) Socket 的一致性——在 WS63 上写 TCP/UDP 代码与 Linux 上几乎一样
- 掌握 TCP (Transmission Control Protocol) Client/Server 和 UDP (User Datagram Protocol) 通信的标准 Socket 调用链
- 理解 lwIP 在嵌入式环境下的资源限制（Socket 数量、缓冲区大小）
- 能够在 WS63 上实现 TCP/UDP 数据通信

## 基本概念

### lwIP 在 WS63 上的定位

lwIP 是轻量级 TCP/IP 协议栈。WS63 通过 WiFi STA 或 CHBA (Converged Host Bus Adapter) 获取 IP 后，应用层通过标准 Socket API 通信。

| 对比项 | WS63 lwIP | Linux |
|--------|:---:|:---:|
| Socket 数量 | 8~16（`CONFIG_LWIP_MAX_SOCKETS`） | 数千 |
| TCP 缓冲 | 几 KB（Kconfig 可调） | 默认 128KB+ |
| API 兼容 | 标准 BSD Socket | 标准 BSD Socket |

### TCP vs UDP 在 IoT 场景的选择

| 对比项 | TCP | UDP |
|--------|:---:|:---:|
| 可靠性 | 可靠有序 | 不可靠，可能丢包/乱序 |
| 连接 | 需建立连接 | 无连接 |
| 延迟 | 较高（握手+ACK (Acknowledgment)） | 低 |
| 典型场景 | 传感器配置、固件下载、MQTT (Message Queuing Telemetry Transport) | 实时数据上报、局域网发现、CoAP (Constrained Application Protocol) |

### 通信流程

**TCP Client**：`socket()` → `connect()` → `send()`/`recv()` → `close()`

**UDP**：`socket()` → `sendto()`/`recvfrom()` → `close()`

## 涉及 API

| API | 用途 |
|-----|------|
| `socket(AF_INET, SOCK_STREAM / SOCK_DGRAM, 0)` | 创建 TCP/UDP socket |
| `connect()` / `bind()` / `listen()` / `accept()` | TCP 连接建立 |
| `send()` / `recv()` | TCP 收发 |
| `sendto()` / `recvfrom()` | UDP 收发 |
| `close()` | 关闭 socket |

## 案例说明

### 案例简介

- TCP Client：WS63 连接 PC 上的 TCP Server → 发送 "hello" → 接收回复
- UDP：WS63 向 PC 发送 UDP 数据包 → PC 接收并显示

## 关键配置

| 参数 | 默认值 | 说明 |
|------|:---:|------|
| `CONFIG_LWIP_MAX_SOCKETS` | 8 | 最大 Socket 数量 |
| `CONFIG_LWIP_TCP_SND_BUF` | 4KB | TCP 发送缓冲 |
| `CONFIG_LWIP_TCP_RCV_BUF` | 4KB | TCP 接收缓冲 |

## 代码详解

### TCP Client 标准流程

```c
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int sock = socket(AF_INET, SOCK_STREAM, 0);

struct sockaddr_in server = {
    .sin_family = AF_INET,
    .sin_port = htons(8080)
};
inet_pton(AF_INET, "192.168.1.100", &server.sin_addr);

connect(sock, (struct sockaddr *)&server, sizeof(server));

char *msg = "hello";
send(sock, msg, strlen(msg), 0);

char buf[256];
int len = recv(sock, buf, sizeof(buf), 0);

close(sock);
```

### UDP 收发

```c
int sock = socket(AF_INET, SOCK_DGRAM, 0);

struct sockaddr_in dest = {
    .sin_family = AF_INET,
    .sin_port = htons(9090)
};
inet_pton(AF_INET, "192.168.1.100", &dest.sin_addr);

char *data = "sensor: 25.5C";
sendto(sock, data, strlen(data), 0,
       (struct sockaddr *)&dest, sizeof(dest));

char buf[256];
struct sockaddr_in from;
socklen_t from_len = sizeof(from);
int len = recvfrom(sock, buf, sizeof(buf), 0,
                   (struct sockaddr *)&from, &from_len);

close(sock);
```

### 常见错误处理

| 错误 | 原因 | 处理 |
|------|------|------|
| `connect()` 超时 | 服务器不可达 | 重试 3 次后报错 |
| `send()` 返回 -1 | 连接已断开 | 重连后重发 |
| `recv()` 返回 0 | 对端正常关闭 | 关闭 socket，清理资源 |

---


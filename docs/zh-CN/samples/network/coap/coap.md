# CoAP

> CoAP (Constrained Application Protocol)、UDP (User Datagram Protocol) Socket

> 前置阅读：[TCP/UDP](../tcp-udp/tcp-udp.md)

## 学习目标

- 理解 CoAP 的设计理念——为资源受限设备设计的 RESTful (Representational State Transfer) 协议，基于 UDP
- 掌握 CoAP GET/PUT/POST/DELETE 与 HTTP (HyperText Transfer Protocol) 的对应关系
- 掌握 CoAP 可观察（Observe）模式——Server 数据变化时主动通知 Client
- 能够在 WS63 上实现 CoAP 服务端或客户端

## 基本概念

### CoAP vs MQTT

| 对比项 | CoAP | MQTT (Message Queuing Telemetry Transport) |
|--------|:---:|:---:|
| 传输层 | UDP（无连接、更轻量） | TCP (Transmission Control Protocol)|
| 模型 | RESTful（URI (Uniform Resource Identifier) 资源） | 发布/订阅（Topic） |
| 需要 Broker | 不需要（直连） | 需要 |
| 安全 | DTLS (Datagram Transport Layer Security) | TLS (Transport Layer Security) |
| 适用场景 | 局域网低功耗设备 | 跨互联网云平台 |

### RESTful 模型

CoAP URI 标识资源，方法操作资源：

| 方法 | 对应 HTTP | 说明 |
|------|:---:|------|
| GET | GET | 读取资源值 |
| PUT | PUT | 更新资源值 |
| POST | POST | 创建新资源 |
| DELETE | DELETE | 删除资源 |

### 可观察（Observe）模式

Client 注册对某资源的观察 → Server 资源值变化时自动推送——类似 MQTT Subscribe 但无需 Broker。

## 涉及 API

| API | 用途 |
|-----|------|
| CoAP context 初始化 | 创建 CoAP context |
| `coap_add_resource()` | Server 注册资源 URI |
| `coap_register_observe()` | Server 注册可观察资源 |
| `coap_send_request()` | Client 发送 GET/PUT/POST |
| CoAP 响应回调 | Client 收到 Server 回复 |

> CoAP 通常通过第三方库（如 libcoap）实现，具体 API 以 SDK实际集成为准。

## 案例说明

### 案例简介

WS63 作为 CoAP Server 暴露温度资源 → CoAP Client 通过 GET 读取 → 注册 Observe 接收温度变化推送。

## 关键配置

| 参数 | 推荐值 | 说明 |
|------|:---:|------|
| CoAP 端口 | 5683 | 标准端口（5684 = DTLS 加密） |
| Observe 推送间隔 | 温度变化 > 0.5°C | 避免频繁推送 |
| 消息可靠性 | CON / NON | CON 需 ACK (Acknowledgment) 确认、NON 不需 |

## 代码详解

### CoAP Server 注册资源

```c
/* 温度资源 GET handler */
static void temp_get_handler(coap_context_t *ctx,
                              coap_resource_t *resource,
                              coap_session_t *session,
                              coap_pdu_t *request,
                              coap_binary_t *token,
                              coap_string_t *query,
                              coap_pdu_t *response) {
    float temp = read_temperature();
    coap_add_data(response, sizeof(temp), (uint8_t *)&temp);
}

/* 注册资源 */
coap_resource_t *res = coap_resource_init(
    coap_make_str_const("/sensors/temp"),
    COAP_RESOURCE_FLAGS_NOTIFYABLE);
coap_resource_set_get_observable(res, 1);
coap_register_handler(res, COAP_REQUEST_GET, temp_get_handler);
coap_add_resource(ctx, res);
```

### Observe 推送

```c
/* 温度变化时通知所有 Observer */
void notify_temp_change(coap_context_t *ctx, float new_temp) {
    coap_resource_notify_observers(
        ctx->resource,              // "/sensors/temp" 资源
        NULL, NULL,                 // 通知所有观察者
        sizeof(new_temp),
        (uint8_t *)&new_temp);
}
```

### Client GET 请求

```c
coap_pdu_t *pdu = coap_new_pdu(COAP_MESSAGE_CON, COAP_REQUEST_GET);
coap_add_option(pdu, COAP_OPTION_URI_PATH,
                coap_make_str_const("sensors/temp"));
coap_send(session, pdu);
/* 等待 Server 响应——包含温度值 */
```

---


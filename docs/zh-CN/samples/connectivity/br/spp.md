# SPP

> BR/EDR SPP (Serial Port Profile)

## 学习目标

- 理解 SPP 工作原理——基于 RFCOMM (Radio Frequency Communication) 的串口仿真协议
- 掌握 Server 端 `spp_server_create()` → `spp_server_accept()` 的完整流程
- 掌握 Client 端 `spp_connect()` 连接指定设备
- 能够在 WS63 上实现经典蓝牙串口透传，与手机蓝牙串口 APP 互通

## 基本概念

### SPP vs BLE 透传

| 对比项 | SPP | BLE (Bluetooth Low Energy) 透传 |
|--------|:---:|:---:|
| 底层协议 | RFCOMM（RS-232 仿真） | GATT (Generic Attribute Profile) Notify/Write |
| 吞吐量 | ~1Mbps (EDR (Enhanced Data Rate)) | ~50-800Kbps |
| 功耗 | 较高 | 低 |
| 兼容性 | 几乎所有手机 | BLE 4.2+ |

### RFCOMM 通道模型

SPP 在 L2CAP 之上封装 RFCOMM——应用层看到的是一个"socket"：`spp_write()` 写到对端，对端 `receive_data_cb` 收到数据。

## 涉及 API

| API | 谁调用 | 用途 |
|-----|--------|------|
| `enable_bt_stack()` | 双方 | 使能经典蓝牙协议栈 |
| `bluetooth_set_local_name()` | 双方 | 设置可见名称 |
| `spp_server_create(&socket_para, name, len)` | Server | 创建 SPP Server socket |
| `spp_server_accept(server_id)` | Server | 开始接受连接 |
| `spp_connect(&socket_para, &bd_addr)` | Client | 连接 Server |
| `spp_write(client_id, data, len)` | 双方 | 发送数据 |
| `spp_register_callbacks(&func)` | 双方 | 注册回调 |

## 案例说明

### 案例简介

WS63 作为 SPP Server，等待手机蓝牙串口 APP 连接 → 双向透传数据。

## 关键配置

| 参数 | 推荐值 | 说明 |
|------|:---:|------|
| Service UUID (Universally Unique Identifier) | `00001101-...-8000-00805F9B34FB` | SPP 标准 UUID |
| Service Name | `WS63-SPP` | 手机 APP 上显示 |
| Socket Type | `SPP_SOCKET_RFCOMM` | |
| 安全等级 | Level 1（调试）/ Level 2（产品） | |

## 代码详解

```c
/* BT Stack 初始化 */
enable_bt_stack();
bluetooth_set_local_name("WS63-SPP", 8);

/* Server 端创建 socket */
spp_create_socket_para_t para = {
    .uuid = "00001101-0000-1000-8000-00805F9B34FB",
    .socket_type = SPP_SOCKET_RFCOMM
};
int server_id = spp_server_create(&para, "WS63-SPP", 8);
spp_server_accept(server_id);

/* Client 端连接 */
gap_connect_remote_device(&bd_addr);  // 先建 ACL
spp_connect(&para, &bd_addr);         // 再连 SPP

/* 数据收发 */
spp_write(client_id, "hello", 5);
/* 对端 receive_data_cb 收到 "hello" */
```

---


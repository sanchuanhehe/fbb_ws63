/*
 * Copyright (c) 2025 YunQiHui Network Technology (Shenzhen) Co., Ltd.
 * Copyright (C) 2024 HiHope Open Source Organization .
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Website: http://www.siotmate.com/
 */

#include "lwip/netifapi.h"
#include "wifi_hotspot.h"
#include "wifi_hotspot_config.h"
#include "stdlib.h"
#include "uart.h"
#include "lwip/nettool/misc.h"
#include "soc_osal.h"
#include "app_init.h"
#include "cmsis_os2.h"
#include "wifi_device.h"
#include "wifi_event.h"
#include "lwip/sockets.h"
#include "lwip/ip4_addr.h"

#define WIFI_TASK_STACK_SIZE 0x2000        // 任务栈大小(8KB)
#define WIFI_SCAN_AP_LIMIT 64              // 最大扫描AP数量
#define WIFI_CONN_STATUS_MAX_GET_TIMES 5   // 连接状态检查次数
#define DHCP_BOUND_STATUS_MAX_GET_TIMES 20 // DHCP绑定状态检查次数
#define WIFI_STA_IP_MAX_GET_TIMES 5        // IP获取状态检查次数

#define CONFIG_WIFI_SSID "H"              // 目标WiFi SSID
#define CONFIG_WIFI_PWD "12345678"        // WiFi密码
#define CONFIG_SERVER_IP "192.168.212.62" // 目标服务器IP
#define CONFIG_SERVER_PORT 6789           // 目标服务器端口

static const char *SEND_DATA = "TCP Test!\r\n"; // 测试发送数据

static void wifi_scan_state_changed(td_s32 state, td_s32 size)
{
    UNUSED(state);
    UNUSED(size);
    printf("扫描完成!\r\n");
}

static void wifi_connection_changed(td_s32 state, const wifi_linked_info_stru *info, td_s32 reason_code)
{
    UNUSED(reason_code);

    if (state == WIFI_STATE_AVALIABLE)
        printf("[WiFi]:%s, [信号强度]:%d\r\n", info->ssid, info->rssi);
}

static errcode_t example_get_match_network(const char *expected_ssid,
                                           const char *key,
                                           wifi_sta_config_stru *expected_bss)
{
    uint32_t num = WIFI_SCAN_AP_LIMIT; // 扫描AP数量上限
    uint32_t bss_index = 0;

    // 分配并初始化扫描结果缓冲区
    uint32_t scan_len = sizeof(wifi_scan_info_stru) * WIFI_SCAN_AP_LIMIT;
    wifi_scan_info_stru *result = osal_kmalloc(scan_len, OSAL_GFP_ATOMIC);
    if (result == NULL) {
        return ERRCODE_MALLOC;
    }

    memset_s(result, scan_len, 0, scan_len);
    if (wifi_sta_get_scan_info(result, &num) != ERRCODE_SUCC) {
        osal_kfree(result);
        return ERRCODE_FAIL;
    }

    // 在扫描结果中查找目标网络
    for (bss_index = 0; bss_index < num; bss_index++) {
        if (strlen(expected_ssid) == strlen(result[bss_index].ssid)) {
            if (memcmp(expected_ssid, result[bss_index].ssid, strlen(expected_ssid)) == 0) {
                break;
            }
        }
    }

    // 未找到目标AP
    if (bss_index >= num) {
        osal_kfree(result);
        return ERRCODE_FAIL;
    }

    // 复制匹配网络的配置信息
    if (memcpy_s(expected_bss->ssid, WIFI_MAX_SSID_LEN, result[bss_index].ssid, WIFI_MAX_SSID_LEN) != EOK) {
        osal_kfree(result);
        return ERRCODE_MEMCPY;
    }
    if (memcpy_s(expected_bss->bssid, WIFI_MAC_LEN, result[bss_index].bssid, WIFI_MAC_LEN) != EOK) {
        osal_kfree(result);
        return ERRCODE_MEMCPY;
    }
    expected_bss->security_type = result[bss_index].security_type;
    if (memcpy_s(expected_bss->pre_shared_key, WIFI_MAX_KEY_LEN, key, strlen(key)) != EOK) {
        osal_kfree(result);
        return ERRCODE_MEMCPY;
    }
    expected_bss->ip_type = DHCP; // 使用DHCP自动获取IP
    osal_kfree(result);
    return ERRCODE_SUCC;
}

static errcode_t wifi_connect(void)
{
    char ifname[WIFI_IFNAME_MAX_SIZE + 1] = "wlan0"; // 网络接口名
    wifi_sta_config_stru expected_bss = {0};         // 连接配置
    const char expected_ssid[] = CONFIG_WIFI_SSID;   // 目标SSID
    const char key[] = CONFIG_WIFI_PWD;              // WiFi密码
    struct netif *netif_p = NULL;                    // 网络接口
    wifi_linked_info_stru wifi_status;               // WiFi连接状态
    uint8_t index = 0;                               // 连接状态检查计数

    // 启用STA模式
    if (wifi_sta_enable() != ERRCODE_SUCC) {
        printf("STA启用失败!\r\n");
        return ERRCODE_FAIL;
    }

    do {
        printf("开始扫描...\r\n");
        (void)osDelay(100); // 延迟100ms

        // 执行WiFi扫描
        if (wifi_sta_scan() != ERRCODE_SUCC) {
            printf("扫描失败，重试!\r\n");
            continue;
        }

        (void)osDelay(300); // 延迟300ms

        // 查找目标网络
        if (example_get_match_network(expected_ssid, key, &expected_bss) != ERRCODE_SUCC) {
            printf("未找到目标AP，重试!\r\n");
            continue;
        }

        printf("尝试连接...\r\n");
        // 启动连接
        if (wifi_sta_connect(&expected_bss) != ERRCODE_SUCC) {
            continue;
        }

        // 检查连接状态
        for (index = 0; index < WIFI_CONN_STATUS_MAX_GET_TIMES; index++) {
            (void)osDelay(50); // 延迟50ms
            memset_s(&wifi_status, sizeof(wifi_status), 0, sizeof(wifi_status));
            if (wifi_sta_get_ap_info(&wifi_status) != ERRCODE_SUCC) {
                continue;
            }
            if (wifi_status.conn_state == WIFI_CONNECTED) {
                break;
            }
        }
        if (wifi_status.conn_state == WIFI_CONNECTED) {
            break; // 连接成功退出循环
        }
    } while (1);

    // 获取网络接口
    netif_p = netifapi_netif_find(ifname);
    if (netif_p == NULL) {
        return ERRCODE_FAIL;
    }

    // 启动DHCP客户端
    if (netifapi_dhcp_start(netif_p) != ERR_OK) {
        printf("DHCP启动失败!\r\n");
        return ERRCODE_FAIL;
    }

    // 检查DHCP绑定状态
    for (uint8_t i = 0; i < DHCP_BOUND_STATUS_MAX_GET_TIMES; i++) {
        (void)osDelay(50); // 延迟50ms
        if (netifapi_dhcp_is_bound(netif_p) == ERR_OK) {
            printf("DHCP绑定成功!\r\n");
            break;
        }
    }

    // 获取并打印IP地址
    for (uint8_t i = 0; i < WIFI_STA_IP_MAX_GET_TIMES; i++) {
        osDelay(1); // 延迟1ms
        if (netif_p->ip_addr.u_addr.ip4.addr != 0) {
            printf("本地IP: %u.%u.%u.%u\r\n", (netif_p->ip_addr.u_addr.ip4.addr & 0x000000ff),
                     // >>8将地址右移8位，将原来的第二个字节移动到最低字节位置,>>16 将地址右移16位，将原来的第三个字节移动到最低字节位置
                   (netif_p->ip_addr.u_addr.ip4.addr >> 8) & 0xff, (netif_p->ip_addr.u_addr.ip4.addr >> 16) & 0xff,
                   (netif_p->ip_addr.u_addr.ip4.addr >> 24) & 0xff); // >>24将地址右移24位，将原来的第四个字节移动到最低字节位置
            printf("连接成功!\r\n");
            return ERRCODE_SUCC;
        }
    }
    printf("连接失败!\r\n");
    return ERRCODE_FAIL;
}

int wifi_tcp_sample_init(const char *argument)
{
    UNUSED(argument);
    int sock_fd;                  // 套接字描述符
    struct sockaddr_in send_addr; // 服务器地址
    char recv_buf[512];           // 接收缓冲区

    // 注册WiFi事件回调
    wifi_event_stru wifi_event_cb = {0};
    wifi_event_cb.wifi_event_scan_state_changed = wifi_scan_state_changed;
    wifi_event_cb.wifi_event_connection_changed = wifi_connection_changed;

    if (wifi_register_event_cb(&wifi_event_cb) != 0) {
        printf("事件回调注册失败\r\n");
        return -1;
    }
    printf("事件回调注册成功\r\n");

    // 等待WiFi初始化完成
    while (wifi_is_wifi_inited() == 0) {
        (void)osDelay(10); // 延迟10ms
    }

    // 连接WiFi
    if (wifi_connect() != ERRCODE_SUCC) {
        return -1;
    }

    // 创建TCP套接字
    printf("创建套接字...\r\n");
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("套接字创建失败!\r\n");
        return 0;
    }
    printf("套接字创建成功!\r\n");

    // 配置服务器地址
    send_addr.sin_family = AF_INET;
    send_addr.sin_port = htons(CONFIG_SERVER_PORT);
    send_addr.sin_addr.s_addr = inet_addr(CONFIG_SERVER_IP);

    // 连接服务器
    if (connect(sock_fd, (struct sockaddr *)&send_addr, sizeof(send_addr)) != 0) {
        printf("服务器连接失败\n");
        lwip_close(sock_fd);
        return 0;
    }
    printf("服务器连接成功\n");

    // 持续通信
    while (1) {
        memset(recv_buf, 0, sizeof(recv_buf));

        // 发送数据
        printf("发送数据...\r\n");
        send(sock_fd, SEND_DATA, strlen(SEND_DATA), 0);
        osDelay(100); // 延迟100ms

        // 接收响应
        recv(sock_fd, recv_buf, sizeof(recv_buf), 0);
        printf("接收:%s\n", recv_buf);
    }

    lwip_close(sock_fd);
    return 0;
}

static void wifi_tcp_sample(void)
{
    // 配置任务属性
    osThreadAttr_t attr = {.name = "wifi_tcp_task", .stack_size = WIFI_TASK_STACK_SIZE, .priority = osPriorityNormal};

    // 创建任务
    if (osThreadNew((osThreadFunc_t)wifi_tcp_sample_init, NULL, &attr) == NULL) {
        printf("任务创建失败\r\n");
    }
    printf("任务创建成功\r\n");
}

// 启动应用
app_run(wifi_tcp_sample);
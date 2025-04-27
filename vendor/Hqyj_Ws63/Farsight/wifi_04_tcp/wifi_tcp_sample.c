/*
 * Copyright (c) 2024 HiSilicon Technologies CO., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
#include "wifi/wifi_connect.h"

#define WIFI_TASK_STACK_SIZE 0x2000

#define DELAY_TIME_MS 100

static const char *SEND_DATA = "TCP Test!\r\n";

static void wifi_scan_state_changed(td_s32 state, td_s32 size)
{
    UNUSED(state);
    UNUSED(size);
    printf("Scan done!\r\n");
    return;
}

static void wifi_connection_changed(td_s32 state, const wifi_linked_info_stru *info, td_s32 reason_code)
{
    UNUSED(reason_code);

    if (state == WIFI_STATE_AVALIABLE)
        printf("[WiFi]:%s, [RSSI]:%d\r\n", info->ssid, info->rssi);
}

int sta_sample_init(const char *argument)
{
    argument = argument;
    int sock_fd;
    // 服务器的地址信息
    struct sockaddr_in send_addr;
    char recv_buf[512];
    wifi_event_stru wifi_event_cb = {0};

    wifi_event_cb.wifi_event_scan_state_changed = wifi_scan_state_changed;
    wifi_event_cb.wifi_event_connection_changed = wifi_connection_changed;
    /* 注册事件回调 */
    if (wifi_register_event_cb(&wifi_event_cb) != 0) {
        printf("wifi_event_cb register fail.\r\n");
        return -1;
    }
    printf("wifi_event_cb register succ.\r\n");

    wifi_connect();

    printf("create socket start! \r\n");
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) != ERRCODE_SUCC) {
        printf("create socket failed!\r\n");
        return 0;
    }
    printf("create socket succ!\r\n");

    /* 初始化预连接的服务端地址 */
    send_addr.sin_family = AF_INET;
    send_addr.sin_port = htons(CONFIG_SERVER_PORT);
    send_addr.sin_addr.s_addr = inet_addr(CONFIG_SERVER_IP);
    if (connect(sock_fd, (struct sockaddr *)&send_addr, sizeof(send_addr)) != 0) {
        printf("Failed to connect to the server\n");
        return 0;
    }
    printf("Connection to server successful\n");

    /* 初始化预连接的服务端地址 */
    send_addr.sin_family = AF_INET;
    send_addr.sin_port = htons(CONFIG_SERVER_PORT);
    send_addr.sin_addr.s_addr = inet_addr(CONFIG_SERVER_IP);

    while (1) {
        memset(recv_buf, 0, sizeof(recv_buf));
        /* 发送数据到服务远端 */
        printf("sendto...\r\n");
        send(sock_fd, SEND_DATA, strlen(SEND_DATA), 0);
        osDelay(DELAY_TIME_MS);

        /* 接收服务端返回的字符串 */
        recv(sock_fd, recv_buf, sizeof(recv_buf), 0);
        printf("recvfrom:%s\n", recv_buf);
    }
    lwip_close(sock_fd);
    return 0;
}

static void sta_sample(void)
{
    osThreadAttr_t attr;
    attr.name = "sta_sample_task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = WIFI_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;
    if (osThreadNew((osThreadFunc_t)sta_sample_init, NULL, &attr) == NULL) {
        printf("Create sta_sample_task fail.\r\n");
    }
    printf("Create sta_sample_task succ.\r\n");
}

/* Run the sample. */
app_run(sta_sample);
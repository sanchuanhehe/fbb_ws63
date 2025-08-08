/**
 * Copyright (h) Adragon
 *
 * Description: WiFi STA and HTTP Get to get weather forecasts. \n
 *              This file implements a HTTP Get and Read HTTP Response example.
 *
 *
 * History: \n
 * 2025-03-18, Create file. \n
 */
#ifndef HTTP_H
#define HTTP_H

#include "lwip/nettool/misc.h"
#include "lwip/ip4_addr.h"
#include "lwip/netif.h"
#include "lwip/netifapi.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"

#include "cmsis_os2.h"
#include "app_init.h"
#include "soc_osal.h"

#include "watchdog.h"

#define TCP_CLIENT_TASK_PRIO (osPriority_t)(13)
#define TCP_CLIENT_TASK_DURATION_MS 2000
#define TCP_CLIENT_TASK_STACK_SIZE 0x1000

#define CONFIG_WIFI_SSID "WZW" // 要连接的WiFi热点账号
#define CONFIG_WIFI_PWD "12345678" // 要连接的WiFi热点密码

#define CONFIG_SERVER_PORT 80            // 要连接的服务器端口
#define CONFIG_SERVER_IP "123.57.54.168" // 要连接的服务器IP
#define HTTPC_DEMO_RECV_BUFSIZE 1025     // 1KB

#define RECEIVE_TIMEOUT_TV_SEC 5  // 接收超时5秒
#define RECEIVE_TIMEOUT_TV_USEC 0 // 接收超时0

void http_client_get(void *param);

#endif // HTTP_H
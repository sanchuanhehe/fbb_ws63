/**
 * Copyright (h) Adragon
 *
 * Description: WiFi STA and HTTP Get to get weather forecasts. \n
 *              This file is a sample for connecting a 2.4GHz WiFi STA.
 *
 *
 * History: \n
 * 2025-03-18, Create file. \n
 */
#ifndef WIFISTA_H
#define WIFISTA_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lwip/netifapi.h"
#include "wifi_hotspot.h"
#include "wifi_hotspot_config.h"
#include "td_base.h"
#include "td_type.h"
#include "uart.h"

#include "cmsis_os2.h"
#include "app_init.h"
#include "soc_osal.h"

#define WIFI_IFNAME_MAX_SIZE 16
#define WIFI_MAX_SSID_LEN 33
#define WIFI_SCAN_AP_LIMIT 64
#define WIFI_MAC_LEN 6
#define WIFI_STA_SAMPLE_LOG "[WIFI_STA_SAMPLE]"
#define WIFI_NOT_AVALLIABLE 0
#define WIFI_AVALIABE 1
#define WIFI_GET_IP_MAX_COUNT 300

#define WIFI_TASK_PRIO (osPriority_t)(13)
#define WIFI_TASK_DURATION_MS 2000
#define WIFI_TASK_STACK_SIZE 0x1000

typedef enum
{
    WIFI_STA_SAMPLE_INIT = 0,     /* 0:初始态 */
    WIFI_STA_SAMPLE_SCANING,      /* 1:扫描中 */
    WIFI_STA_SAMPLE_SCAN_DONE,    /* 2:扫描完成 */
    WIFI_STA_SAMPLE_FOUND_TARGET, /* 3:匹配到目标AP */
    WIFI_STA_SAMPLE_CONNECTING,   /* 4:连接中 */
    WIFI_STA_SAMPLE_CONNECT_DONE, /* 5:关联成功 */
    WIFI_STA_SAMPLE_GET_IP,       /* 6:获取IP */
} wifi_state_enum;

int wifi_connect(const char *ssid, const char *psk);

#endif // WIFISTA_H
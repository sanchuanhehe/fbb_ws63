/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: alilink sample include wifi sta auto connect and then connect to ali cloud \n
 *
 * History: \n
 * 2024-09-19, Create file. \n
 */

#include "lwip/netifapi.h"
#include "wifi_hotspot.h"
#include "wifi_hotspot_config.h"
#include "td_base.h"
#include "td_type.h"
#include "stdlib.h"
#include "uart.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "soc_osal.h"

#define ALILINK_IFNAME_MAX_SIZE             16
#define ALILINK_MAX_SSID_LEN                33
#define ALILINK_SCAN_AP_LIMIT               64
#define ALILINK_MAC_LEN                     6
#define ALILINK_SAMPLE_LOG                  "[ALILINK_SAMPLE]"
#define ALILINK_NOT_AVALLIABLE              0
#define ALILINK_AVALLIABLE                  1
#define ALILINK_GET_IP_MAX_COUNT            300

#define ALILINK_TASK_PRIO                  (osPriority_t)(13)
#define ALILINK_TASK_DURATION_MS           2000
#define ALILINK_TASK_STACK_SIZE            0x2000

extern td_s32 alilink_task_start(td_void);
static td_void alilink_scan_state_changed(td_s32 state, td_s32 size);
static td_void alilink_connection_changed(td_s32 state, const wifi_linked_info_stru *info, td_s32 reason_code);

static wifi_event_stru g_alilink_wifi_event_cb = {
    .wifi_event_connection_changed      = alilink_connection_changed,
    .wifi_event_scan_state_changed      = alilink_scan_state_changed,
};

enum {
    ALILINK_SAMPLE_INIT = 0,       /* 0:初始态 */
    ALILINK_SAMPLE_SCANING,        /* 1:扫描中 */
    ALILINK_SAMPLE_SCAN_DONE,      /* 2:扫描完成 */
    ALILINK_SAMPLE_FOUND_TARGET,   /* 3:匹配到目标AP */
    ALILINK_SAMPLE_CONNECTING,     /* 4:连接中 */
    ALILINK_SAMPLE_CONNECT_DONE,   /* 5:关联成功 */
    ALILINK_SAMPLE_GET_IP,         /* 6:获取IP */
} wifi_state_enum;

static td_u8 g_alilink_wifi_state = ALILINK_SAMPLE_INIT;

/*****************************************************************************
  STA 扫描事件回调函数
*****************************************************************************/
static td_void alilink_scan_state_changed(td_s32 state, td_s32 size)
{
    UNUSED(state);
    UNUSED(size);
    PRINT("%s::Scan done!.\r\n", ALILINK_SAMPLE_LOG);
    g_alilink_wifi_state = ALILINK_SAMPLE_SCAN_DONE;
    return;
}

/*****************************************************************************
  STA 关联事件回调函数
*****************************************************************************/
static td_void alilink_connection_changed(td_s32 state, const wifi_linked_info_stru *info, td_s32 reason_code)
{
    UNUSED(info);
    UNUSED(reason_code);

    if (state == ALILINK_NOT_AVALLIABLE) {
        PRINT("%s::Connect fail!. try agin !\r\n", ALILINK_SAMPLE_LOG);
        g_alilink_wifi_state = ALILINK_SAMPLE_INIT;
    } else {
        PRINT("%s::Connect succ!.\r\n", ALILINK_SAMPLE_LOG);
        g_alilink_wifi_state = ALILINK_SAMPLE_CONNECT_DONE;
    }
}

/*****************************************************************************
  STA 匹配目标AP
*****************************************************************************/
static td_s32 alilink_get_match_network(wifi_sta_config_stru *expected_bss)
{
    td_s32  ret;
    td_u32  num = 64; /* 64:扫描到的Wi-Fi网络数量 */
    td_char expected_ssid[] = "my_softAP";
    td_char key[] = "my_password"; /* 待连接的网络接入密码 */
    td_bool find_ap = TD_FALSE;
    td_u8   bss_index;
    /* 获取扫描结果 */
    td_u32 scan_len = sizeof(wifi_scan_info_stru) * ALILINK_SCAN_AP_LIMIT;
    wifi_scan_info_stru *result = osal_kmalloc(scan_len, OSAL_GFP_ATOMIC);
    if (result == TD_NULL) {
        return -1;
    }
    (td_void)memset_s(result, scan_len, 0, scan_len);
    ret = wifi_sta_get_scan_info(result, &num);
    if (ret != 0) {
        osal_kfree(result);
        return -1;
    }
    /* 筛选扫描到的Wi-Fi网络，选择待连接的网络 */
    for (bss_index = 0; bss_index < num; bss_index ++) {
        if (strlen(expected_ssid) == strlen(result[bss_index].ssid)) {
            if (memcmp(expected_ssid, result[bss_index].ssid, strlen(expected_ssid)) == 0) {
                find_ap = TD_TRUE;
                break;
            }
        }
    }
    /* 未找到待连接AP,可以继续尝试扫描或者退出 */
    if (find_ap == TD_FALSE) {
        osal_kfree(result);
        return -1;
    }
    /* 找到网络后复制网络信息和接入密码 */
    if (memcpy_s(expected_bss->ssid, ALILINK_MAX_SSID_LEN, expected_ssid, strlen(expected_ssid)) != 0) {
        osal_kfree(result);
        return -1;
    }
    if (memcpy_s(expected_bss->bssid, ALILINK_MAC_LEN, result[bss_index].bssid, ALILINK_MAC_LEN) != 0) {
        osal_kfree(result);
        return -1;
    }
    expected_bss->security_type = result[bss_index].security_type;
    if (memcpy_s(expected_bss->pre_shared_key, sizeof(expected_bss->pre_shared_key), key, strlen(key)) != 0) {
        osal_kfree(result);
        return -1;
    }
    expected_bss->ip_type = 1; /* 1：IP类型为动态DHCP获取 */
    osal_kfree(result);
    return 0;
}

/*****************************************************************************
  STA DHCP状态查询
*****************************************************************************/
static td_bool alilink_check_dhcp_status(struct netif *netif_p, td_u32 *wait_count)
{
    if ((ip_addr_isany(&(netif_p->ip_addr)) == 0) && (*wait_count <= ALILINK_GET_IP_MAX_COUNT)) {
        /* DHCP成功 */
        PRINT("%s::STA DHCP success.\r\n", ALILINK_SAMPLE_LOG);
        return 0;
    }

    if (*wait_count > ALILINK_GET_IP_MAX_COUNT) {
        PRINT("%s::STA DHCP timeout, try again !.\r\n", ALILINK_SAMPLE_LOG);
        *wait_count = 0;
        g_alilink_wifi_state = ALILINK_SAMPLE_INIT;
    }
    return -1;
}

static td_s32 alilink_sta_function(td_void)
{
    td_char ifname[ALILINK_IFNAME_MAX_SIZE + 1] = "wlan0"; /* 创建的STA接口名 */
    wifi_sta_config_stru expected_bss = {0}; /* 连接请求信息 */
    struct netif *netif_p = TD_NULL;
    td_u32 wait_count = 0;

    /* 创建STA接口 */
    if (wifi_sta_enable() != 0) {
        return -1;
    }
    PRINT("%s::STA enable succ.\r\n", ALILINK_SAMPLE_LOG);

    do {
        (void)osDelay(1); /* 1: 等待10ms后判断状态 */
        if (g_alilink_wifi_state == ALILINK_SAMPLE_INIT) {
            PRINT("%s::Scan start!\r\n", ALILINK_SAMPLE_LOG);
            g_alilink_wifi_state = ALILINK_SAMPLE_SCANING;
            /* 启动STA扫描 */
            if (wifi_sta_scan() != 0) {
                g_alilink_wifi_state = ALILINK_SAMPLE_INIT;
                continue;
            }
        } else if (g_alilink_wifi_state == ALILINK_SAMPLE_SCAN_DONE) {
            /* 获取待连接的网络 */
            if (alilink_get_match_network(&expected_bss) != 0) {
                PRINT("%s::Do not find AP, try again !\r\n", ALILINK_SAMPLE_LOG);
                g_alilink_wifi_state = ALILINK_SAMPLE_INIT;
                continue;
            }
            g_alilink_wifi_state = ALILINK_SAMPLE_FOUND_TARGET;
        } else if (g_alilink_wifi_state == ALILINK_SAMPLE_FOUND_TARGET) {
            PRINT("%s::Connect start.\r\n", ALILINK_SAMPLE_LOG);
            g_alilink_wifi_state = ALILINK_SAMPLE_CONNECTING;
            /* 启动连接 */
            if (wifi_sta_connect(&expected_bss) != 0) {
                g_alilink_wifi_state = ALILINK_SAMPLE_INIT;
                continue;
            }
        } else if (g_alilink_wifi_state == ALILINK_SAMPLE_CONNECT_DONE) {
            PRINT("%s::DHCP start.\r\n", ALILINK_SAMPLE_LOG);
            g_alilink_wifi_state = ALILINK_SAMPLE_GET_IP;
            netif_p = netifapi_netif_find(ifname);
            if (netif_p == TD_NULL || netifapi_dhcp_start(netif_p) != 0) {
                PRINT("%s::find netif or start DHCP fail, try again !\r\n", ALILINK_SAMPLE_LOG);
                g_alilink_wifi_state = ALILINK_SAMPLE_INIT;
                continue;
            }
        } else if (g_alilink_wifi_state == ALILINK_SAMPLE_GET_IP) {
            if (alilink_check_dhcp_status(netif_p, &wait_count) == 0) {
                break;
            }
            wait_count++;
        }
    } while (1);

    return 0;
}

int alilink_sample_init(void *param)
{
    UNUSED(param);

    /* 注册事件回调 */
    if (wifi_register_event_cb(&g_alilink_wifi_event_cb) != 0) {
        PRINT("%s::g_alilink_wifi_event_cb register fail.\r\n", ALILINK_SAMPLE_LOG);
        return -1;
    }
    PRINT("%s::g_alilink_wifi_event_cb register succ.\r\n", ALILINK_SAMPLE_LOG);

    /* 等待wifi初始化完成 */
    while (wifi_is_wifi_inited() == 0) {
        (void)osDelay(10); /* 10: 等待100ms后判断状态 */
    }
    PRINT("%s::wifi init succ.\r\n", ALILINK_SAMPLE_LOG);

    if (alilink_sta_function() != 0) {
        PRINT("%s::alilink_sta_function fail.\r\n", ALILINK_SAMPLE_LOG);
        return -1;
    }

    alilink_task_start();
    return 0;
}

static void alilink_sample_entry(void)
{
    osThreadAttr_t attr;
    attr.name       = "alilink_sample_task";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = ALILINK_TASK_STACK_SIZE;
    attr.priority   = ALILINK_TASK_PRIO;
    if (osThreadNew((osThreadFunc_t)alilink_sample_init, NULL, &attr) == NULL) {
        PRINT("%s::Create alilink_sample_task fail.\r\n", ALILINK_SAMPLE_LOG);
    }
    PRINT("%s::Create alilink_sample_task succ.\r\n", ALILINK_SAMPLE_LOG);
}

/* Run the sta_sample_task. */
app_run(alilink_sample_entry);
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026.
 * Description: Self-contained Wi-Fi STA connection for the BLE MQTT gateway.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "securec.h"
#include "soc_osal.h"
#include "lwip/netifapi.h"
#include "wifi_hotspot.h"
#include "wifi_hotspot_config.h"
#include "ble_gateway_iotda.h"
#include "ble_gateway_wifi.h"

#define GATEWAY_WIFI_LOG "[ble mqtt gateway]"
#define GATEWAY_WIFI_IFNAME "wlan0"
#define GATEWAY_WIFI_SCAN_LIMIT 64U
#define GATEWAY_WIFI_SCAN_RETRY_MS 1000U
#define GATEWAY_WIFI_POLL_MS 100U
#define GATEWAY_WIFI_DHCP_TIMEOUT_COUNT 300U
#define GATEWAY_WIFI_TASK_STACK_SIZE 0x3000U
#define GATEWAY_WIFI_TASK_PRIO 24U
#define GATEWAY_WIFI_IP_TYPE_DHCP 1U

typedef enum {
    GATEWAY_WIFI_INIT = 0,
    GATEWAY_WIFI_SCANNING,
    GATEWAY_WIFI_SCAN_DONE,
    GATEWAY_WIFI_CONNECTING,
    GATEWAY_WIFI_CONNECTED,
    GATEWAY_WIFI_GOT_IP,
} gateway_wifi_state_t;

static volatile gateway_wifi_state_t g_gateway_wifi_state = GATEWAY_WIFI_INIT;
static bool g_gateway_wifi_task_started;

static void gateway_wifi_scan_changed(int32_t state, int32_t size)
{
    osal_printk("%s Wi-Fi scan complete: state=%d count=%d\r\n", GATEWAY_WIFI_LOG, state, size);
    g_gateway_wifi_state = GATEWAY_WIFI_SCAN_DONE;
}

static void gateway_wifi_connection_changed(int32_t state, const wifi_linked_info_stru *info, int32_t reason_code)
{
    (void)info;
    if (state == 0) {
        g_gateway_wifi_state = GATEWAY_WIFI_INIT;
        osal_printk("%s Wi-Fi disconnected, reason=%d, reconnecting\r\n", GATEWAY_WIFI_LOG, reason_code);
    } else {
        g_gateway_wifi_state = GATEWAY_WIFI_CONNECTED;
        osal_printk("%s Wi-Fi associated\r\n", GATEWAY_WIFI_LOG);
    }
}

static wifi_event_stru g_gateway_wifi_callbacks = {
    .wifi_event_connection_changed = gateway_wifi_connection_changed,
    .wifi_event_scan_state_changed = gateway_wifi_scan_changed,
};

static bool gateway_wifi_config_valid(void)
{
    size_t ssid_len = strlen(CONFIG_GATEWAY_WIFI_SSID);
    size_t password_len = strlen(CONFIG_GATEWAY_WIFI_PASSWORD);
    return ssid_len > 0U && ssid_len <= 32U && password_len >= 8U && password_len <= 64U;
}

static errcode_t gateway_wifi_start_scan(void)
{
    return wifi_sta_scan();
}

static bool gateway_wifi_find_target(wifi_sta_config_stru *target)
{
    uint32_t count = GATEWAY_WIFI_SCAN_LIMIT;
    size_t buffer_size = sizeof(wifi_scan_info_stru) * GATEWAY_WIFI_SCAN_LIMIT;
    wifi_scan_info_stru *results = osal_kmalloc(buffer_size, OSAL_GFP_KERNEL);
    bool found = false;

    if (results == NULL || memset_s(results, buffer_size, 0, buffer_size) != EOK ||
        wifi_sta_get_scan_info(results, &count) != 0) {
        if (results != NULL) {
            osal_kfree(results);
        }
        return false;
    }
    for (uint32_t index = 0; index < count; index++) {
        if (strcmp(CONFIG_GATEWAY_WIFI_SSID, results[index].ssid) != 0) {
            continue;
        }
        if (strcpy_s((char *)target->ssid, sizeof(target->ssid), CONFIG_GATEWAY_WIFI_SSID) != EOK ||
            memcpy_s(target->bssid, sizeof(target->bssid), results[index].bssid, sizeof(target->bssid)) != EOK ||
            strcpy_s((char *)target->pre_shared_key, sizeof(target->pre_shared_key),
                     CONFIG_GATEWAY_WIFI_PASSWORD) != EOK) {
            break;
        }
        target->security_type = results[index].security_type;
        target->ip_type = GATEWAY_WIFI_IP_TYPE_DHCP;
        found = true;
        break;
    }
    (void)memset_s(results, buffer_size, 0, buffer_size);
    osal_kfree(results);
    return found;
}

static bool gateway_wifi_initialize(void)
{
    if (!gateway_wifi_config_valid()) {
        osal_printk("%s Wi-Fi private Kconfig is incomplete\r\n", GATEWAY_WIFI_LOG);
        return false;
    }
    if (wifi_register_event_cb(&g_gateway_wifi_callbacks) != 0) {
        osal_printk("%s Wi-Fi callback registration failed\r\n", GATEWAY_WIFI_LOG);
        return false;
    }
    while (wifi_is_wifi_inited() == 0) {
        osal_msleep(GATEWAY_WIFI_POLL_MS);
    }
    if (wifi_sta_enable() != 0) {
        osal_printk("%s Wi-Fi STA enable failed\r\n", GATEWAY_WIFI_LOG);
        return false;
    }
    osal_printk("%s Wi-Fi STA enabled\r\n", GATEWAY_WIFI_LOG);
    return true;
}

static void gateway_wifi_handle_init(wifi_sta_config_stru *target, uint32_t *dhcp_wait_count)
{
    errcode_t ret;
    (void)memset_s(target, sizeof(*target), 0, sizeof(*target));
    *dhcp_wait_count = 0;
    g_gateway_wifi_state = GATEWAY_WIFI_SCANNING;
    ret = gateway_wifi_start_scan();
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s Wi-Fi scan start failed: 0x%x\r\n", GATEWAY_WIFI_LOG, ret);
        g_gateway_wifi_state = GATEWAY_WIFI_INIT;
        osal_msleep(GATEWAY_WIFI_SCAN_RETRY_MS);
    }
}

static void gateway_wifi_handle_scan_done(wifi_sta_config_stru *target)
{
    int32_t ret;
    if (!gateway_wifi_find_target(target)) {
        osal_printk("%s configured Wi-Fi was not found, rescanning\r\n", GATEWAY_WIFI_LOG);
        g_gateway_wifi_state = GATEWAY_WIFI_INIT;
        osal_msleep(GATEWAY_WIFI_SCAN_RETRY_MS);
        return;
    }
    g_gateway_wifi_state = GATEWAY_WIFI_CONNECTING;
    ret = wifi_sta_connect(target);
    (void)memset_s(target, sizeof(*target), 0, sizeof(*target));
    if (ret != 0) {
        osal_printk("%s Wi-Fi connect request failed: %d\r\n", GATEWAY_WIFI_LOG, ret);
        g_gateway_wifi_state = GATEWAY_WIFI_INIT;
    }
}

static void gateway_wifi_handle_connected(struct netif **netif)
{
    *netif = netifapi_netif_find(GATEWAY_WIFI_IFNAME);
    g_gateway_wifi_state = (*netif != NULL && netifapi_dhcp_start(*netif) == 0) ?
        GATEWAY_WIFI_GOT_IP : GATEWAY_WIFI_INIT;
}

static void gateway_wifi_handle_got_ip(struct netif *netif, uint32_t *dhcp_wait_count)
{
    if (netif != NULL && !ip_addr_isany(&netif->ip_addr)) {
        osal_printk("%s DHCP ready, starting IoTDA\r\n", GATEWAY_WIFI_LOG);
        ble_gateway_iotda_run();
        if (g_gateway_wifi_state == GATEWAY_WIFI_GOT_IP) {
            osal_msleep(GATEWAY_WIFI_SCAN_RETRY_MS);
        }
        return;
    }
    (*dhcp_wait_count)++;
    if (*dhcp_wait_count > GATEWAY_WIFI_DHCP_TIMEOUT_COUNT) {
        osal_printk("%s DHCP timeout, reconnecting\r\n", GATEWAY_WIFI_LOG);
        g_gateway_wifi_state = GATEWAY_WIFI_INIT;
    }
}

static int gateway_wifi_task(void *arg)
{
    wifi_sta_config_stru target = {0};
    struct netif *netif = NULL;
    uint32_t dhcp_wait_count = 0;
    (void)arg;

    if (!gateway_wifi_initialize()) {
        return (int)ERRCODE_FAIL;
    }

    while (true) {
        switch (g_gateway_wifi_state) {
            case GATEWAY_WIFI_INIT:
                gateway_wifi_handle_init(&target, &dhcp_wait_count);
                break;
            case GATEWAY_WIFI_SCAN_DONE:
                gateway_wifi_handle_scan_done(&target);
                break;
            case GATEWAY_WIFI_CONNECTED:
                gateway_wifi_handle_connected(&netif);
                break;
            case GATEWAY_WIFI_GOT_IP:
                gateway_wifi_handle_got_ip(netif, &dhcp_wait_count);
                break;
            default:
                break;
        }
        osal_msleep(GATEWAY_WIFI_POLL_MS);
    }
}

errcode_t ble_gateway_wifi_start(void)
{
    osal_task *task;
    if (g_gateway_wifi_task_started) {
        return ERRCODE_SUCC;
    }
    task = osal_kthread_create(gateway_wifi_task, NULL, "ble_gateway_wifi", GATEWAY_WIFI_TASK_STACK_SIZE);
    if (task == NULL) {
        return ERRCODE_FAIL;
    }
    osal_kthread_set_priority(task, GATEWAY_WIFI_TASK_PRIO);
    osal_kfree(task);
    g_gateway_wifi_task_started = true;
    return ERRCODE_SUCC;
}

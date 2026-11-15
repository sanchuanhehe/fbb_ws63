/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2026. All rights reserved.
 *
 * Description: BLE WiFi provisioning sample — productized version with
 *              NV persistence, LED indication, timeout protection, and
 *              error recovery.
 *
 * Features (Kconfig-configurable):
 *   - BLE GATT-based WiFi credential delivery (Service UUID 0xFD5C)
 *   - NV storage of credentials across reboots (CONFIG_BLE_PROV_NV_ENABLE)
 *   - LED status indication (CONFIG_BLE_PROV_LED_ENABLE)
 *   - Provisioning timeout (CONFIG_BLE_PROV_TIMEOUT_SEC)
 *   - Auto-reconnect on boot when NV credentials exist
 */

#include "lwip/netifapi.h"
#include "wifi_hotspot.h"
#include "wifi_hotspot_config.h"
#include "td_base.h"
#include "td_type.h"
#include "stdlib.h"
#include "uart.h"
#include "cmsis_os2.h"
#include "soc_osal.h"
#include "app_init.h"
#include "bts_le_gap.h"

#include "ble_wifi_cfg_adv.h"
#include "ble_wifi_cfg_server.h"

#ifdef CONFIG_BLE_PROV_NV_ENABLE
#include "ble_wifi_prov_nv.h"
#endif

#ifdef CONFIG_BLE_PROV_LED_ENABLE
#include "ble_wifi_prov_led.h"
#endif

#ifdef CONFIG_BLE_PROV_BTN_ENABLE
#include "ble_wifi_prov_btn.h"
#endif

/* ============================================================
 *  Constants
 * ============================================================ */

#define WIFI_IFNAME_MAX_SIZE 16
#define WIFI_MAX_KEY_LEN 65
#define WIFI_MAX_SSID_LEN 33
#define WIFI_SCAN_AP_LIMIT 64
#define WIFI_MAC_LEN 6
#define WIFI_GET_IP_MAX_TIMES 100
#define WIFI_MAX_CONFIG_INFO_LEN 64
#define WIFI_CONFIG_INFO_SSID_LEN 32
#define WIFI_CONFIG_INFO_KEY_LEN 32
#define BGLE_WIFI_CFG_LOG "[BGLE_WIFI]"
#define WIFI_AP_LIST_MAX_NUM 10
#define WIFI_AP_LIST_PREFIX_LEN 2

/* Kconfig defaults (overridden by generated autoconf.h) */
#ifndef CONFIG_BLE_PROV_TIMEOUT_SEC
#define CONFIG_BLE_PROV_TIMEOUT_SEC 60
#endif
#ifndef CONFIG_BLE_PROV_LED_PIN
#define CONFIG_BLE_PROV_LED_PIN 0
#endif

#define PROV_TIMEOUT_TICKS ((uint32_t)CONFIG_BLE_PROV_TIMEOUT_SEC * 100) /* 1 tick ≈ 10 ms */
#define PROV_TASK_DELAY_TICK 10                                          /* main loop polling interval */
#define WIFI_SCAN_RETRY_COUNT 5
#define WIFI_SCAN_RETRY_DELAY_TICKS 50
#define WIFI_STATUS_POLL_DELAY_TICKS 10
#define PROV_SYSTEM_READY_DELAY_TICKS 200
#define PROV_RETRY_DELAY_MS 2000
#define PROV_RESTART_DELAY_MS 1000

/* WLAN disconnect reason codes */
#define WLAN_REASON_UNSPECIFIED 1
#define WLAN_REASON_PREV_AUTH_NOT_VALID 2
#define WLAN_REASON_DEAUTH_LEAVING 3
#define WLAN_REASON_DISASSOC_DUE_TO_INACTIVITY 4
#define WLAN_REASON_CLASS2_FRAME_FROM_NONAUTH_STA 6
#define WLAN_REASON_MICHAEL_MIC_FAILURE 14
#define WLAN_REASON_4WAY_HANDSHAKE_TIMEOUT 15
#define WLAN_REASON_GROUP_KEY_UPDATE_TIMEOUT 16
#define WLAN_DISCONN_BY_AP_BIT 15

#define WLAN_STATUS_CHALLENGE_FAIL 15
#define MAC_JOIN_RSP_TIMEOUT 5200
#define MAC_AUTH_RSP2_TIMEOUT 5201
#define MAC_AUTH_RSP4_TIMEOUT 5202
#define MAC_ASOC_RSP_TIMEOUT 5203
#define WLAN_DISASOC_MISC_LINKLOSS 5206
#define WIFI_NETWORK_NOT_FOUND_ERROR 5300
#define MAC_STATUS_MAX 7000

/* ============================================================
 *  Enums
 * ============================================================ */

/** WiFi connection sub-state (original sample state machine). */
enum {
    CONFIG_DEMO_INIT = 0,
    CONFIG_DEMO_WIFI_INIT,
    CONFIG_DEMO_WIFI_SCAN_DOING,
    CONFIG_DEMO_WIFI_SCAN_DONE,
    CONFIG_DEMO_WIFI_CONNECT_DOING,
    CONFIG_DEMO_WIFI_CONNECT_DONE,
    CONFIG_DEMO_WIFI_DHCP_DONE,
} g_bgwc_state_enum;

/** High-level provisioning state (drives LED + flow control). */
typedef enum {
    PROV_STATE_INIT = 0,
    PROV_STATE_NV_CHECK,
    PROV_STATE_BLE_ADV,
    PROV_STATE_BLE_CONNECTED,
    PROV_STATE_WIFI_SCANNING,
    PROV_STATE_WIFI_CONNECTING,
    PROV_STATE_WIFI_DHCP,
    PROV_STATE_SUCCESS,
    PROV_STATE_FAILED,
    PROV_STATE_TIMEOUT,
    PROV_STATE_NV_CONFIGURED,
} prov_state_t;

enum { CFG_TYPE_WIFI_STATE = 1, CFG_TYPE_AP_LIST = 2 } g_bgwc_cfg_type;

typedef enum {
    WIFI_ERRCODE_NONE = 0,
    WIFI_ERRCODE_SSID_NOT_FOUND,
    WIFI_ERRCODE_PWD_ERROR,
    WIFI_ERRCODE_DHCP_FAILED,
    WIFI_ERRCODE_BEACON_LOST,
    WIFI_ERRCODE_OTHERS
} bgwc_wifi_errcode;

typedef struct {
    char ssid[WIFI_MAX_SSID_LEN];
    int8_t rssi;
} bgwc_wifi_bss;

/* ============================================================
 *  Global state
 * ============================================================ */

static td_char g_data[WIFI_MAX_CONFIG_INFO_LEN] = {0};
static td_u8 g_bgwc_state = CONFIG_DEMO_INIT;
static uint8_t g_wifi_cfg_info_flag = 0;
static uint8_t g_wifi_list_req_flag = 0;
static int8_t g_errcode = WIFI_ERRCODE_NONE;
static prov_state_t g_prov_state = PROV_STATE_INIT;

/* ---- helpers ------------------------------------------------- */

static int8_t get_wifi_errcode(void)
{
    return g_errcode;
}
uint8_t get_wifi_cfg_info_flag(void)
{
    return g_wifi_cfg_info_flag;
}
void set_wifi_cfg_info_flag(uint8_t flag)
{
    g_wifi_cfg_info_flag = flag;
}
uint8_t get_wifi_list_req_flag(void)
{
    return g_wifi_list_req_flag;
}
void set_wifi_list_req_flag(uint8_t flag)
{
    g_wifi_list_req_flag = flag;
}

static void prov_set_state(prov_state_t s)
{
    g_prov_state = s;
#ifdef CONFIG_BLE_PROV_LED_ENABLE
    switch (s) {
        case PROV_STATE_BLE_ADV:
        case PROV_STATE_BLE_CONNECTED:
            ble_wifi_prov_led_set_state(PROV_LED_FAST_BLINK);
            break;
        case PROV_STATE_WIFI_SCANNING:
        case PROV_STATE_WIFI_CONNECTING:
        case PROV_STATE_WIFI_DHCP:
            ble_wifi_prov_led_set_state(PROV_LED_SLOW_BLINK);
            break;
        case PROV_STATE_SUCCESS:
        case PROV_STATE_NV_CONFIGURED:
            ble_wifi_prov_led_set_state(PROV_LED_ON);
            break;
        case PROV_STATE_FAILED:
        case PROV_STATE_TIMEOUT:
            ble_wifi_prov_led_set_state(PROV_LED_ERROR_FLASH);
            break;
        default:
            break;
    }
#endif
}

/* Accumulated write offset for GATT fragmentation reassembly.
 * BLE 调试助手 sends long HEX as multiple Write-Without-Response
 * packets (offset always 0), so we accumulate manually. */
static uint16_t g_cfg_write_offset = 0;

void set_wifi_cfg_info(uint8_t *info, uint16_t info_len, uint16_t offset)
{
    set_wifi_cfg_info_flag(1);

    /* If the client supplies a non-zero offset (Prepared Write), use it.
     * Otherwise accumulate sequentially (Write Without Response mode). */
    uint16_t pos;
    if (offset > 0) {
        pos = offset;
    } else {
        pos = g_cfg_write_offset;
    }

    if (pos + info_len > WIFI_MAX_CONFIG_INFO_LEN) {
        PRINT("%s cfg overflow, reset. pos=%u len=%u\r\n", BGLE_WIFI_CFG_LOG, pos, info_len);
        pos = 0;
        g_cfg_write_offset = 0;
    }

    (void)memcpy_s(g_data + pos, WIFI_MAX_CONFIG_INFO_LEN - pos, info, info_len);
    g_cfg_write_offset = pos + info_len;

    PRINT("%s frag write: pos=%u len=%u total=%u\r\n", BGLE_WIFI_CFG_LOG, pos, info_len, g_cfg_write_offset);

    /* Auto-reset when a full 64-byte credential is received */
    if (g_cfg_write_offset >= WIFI_MAX_CONFIG_INFO_LEN) {
        g_cfg_write_offset = 0;
    }
}

int bgwc_wifi_list_resp_send(uint16_t handle)
{
    set_wifi_list_req_flag(1);
    uint8_t result = 0x01;
    errcode_t ret = ble_wifi_cfg_server_send_report_by_handle(handle, (const uint8_t *)&result, sizeof(uint8_t));
    if (ret != ERRCODE_BT_SUCCESS) {
        PRINT("bgwc_wifi_list_resp_send fail, ret:%x.\n", ret);
    }
    return ret;
}

/* ============================================================
 *  WiFi scanning & connection
 * ============================================================ */

static td_s32 example_get_match_network(wifi_sta_config_stru *expected_bss)
{
    td_s32 ret;
    td_u32 num = 64;
    td_char expected_ssid[WIFI_CONFIG_INFO_SSID_LEN] = {0};
    td_char key[WIFI_CONFIG_INFO_KEY_LEN] = {0};
    td_bool find_ap = TD_FALSE;
    td_u8 bss_index;
    td_u32 scan_len = sizeof(wifi_scan_info_stru) * WIFI_SCAN_AP_LIMIT;
    wifi_scan_info_stru *result = osal_kmalloc(scan_len, OSAL_GFP_ATOMIC);

    if (result == NULL) {
        return -1;
    }
    memset_s(result, scan_len, 0, scan_len);
    ret = wifi_sta_get_scan_info(result, &num);
    if (ret != 0) {
        osal_kfree(result);
        return -1;
    }

    memcpy_s(expected_ssid, WIFI_CONFIG_INFO_SSID_LEN, g_data, WIFI_CONFIG_INFO_SSID_LEN);
    memcpy_s(key, WIFI_CONFIG_INFO_SSID_LEN, g_data + WIFI_CONFIG_INFO_SSID_LEN, WIFI_CONFIG_INFO_KEY_LEN);
    PRINT("%s expected_ssid :%s\r\n", BGLE_WIFI_CFG_LOG, expected_ssid);

    for (bss_index = 0; bss_index < num; bss_index++) {
        if (strlen(expected_ssid) == strlen(result[bss_index].ssid)) {
            if (memcmp(expected_ssid, result[bss_index].ssid, strlen(expected_ssid)) == 0) {
                find_ap = TD_TRUE;
                break;
            }
        }
    }
    if (find_ap == TD_FALSE) {
        osal_kfree(result);
        return -1;
    }

    if (memcpy_s(expected_bss->ssid, WIFI_MAX_SSID_LEN, expected_ssid, strlen(expected_ssid)) != 0) {
        osal_kfree(result);
        return -1;
    }
    if (memcpy_s(expected_bss->bssid, WIFI_MAC_LEN, result[bss_index].bssid, WIFI_MAC_LEN) != 0) {
        osal_kfree(result);
        return -1;
    }
    expected_bss->security_type = result[bss_index].security_type;
    if (memcpy_s(expected_bss->pre_shared_key, WIFI_MAX_SSID_LEN, key, strlen(key)) != 0) {
        osal_kfree(result);
        return -1;
    }
    expected_bss->ip_type = 1;
    osal_kfree(result);
    return 0;
}

static td_bool __attribute__((unused)) example_check_connect_status(td_void)
{
    td_u8 index;
    wifi_linked_info_stru wifi_status;
    for (index = 0; index < WIFI_SCAN_RETRY_COUNT; index++) {
        (td_void) osDelay(WIFI_SCAN_RETRY_DELAY_TICKS);
        memset_s(&wifi_status, sizeof(wifi_linked_info_stru), 0, sizeof(wifi_linked_info_stru));
        if (wifi_sta_get_ap_info(&wifi_status) != 0) {
            continue;
        }
        if (wifi_status.conn_state == 1) {
            return 0;
        }
    }
    return -1;
}

static td_void bgwc_get_ap_list_info(wifi_scan_info_stru *scan_ret, uint32_t real_ap_number, uint8_t *report_data)
{
    bgwc_wifi_bss bss = {0};
    uint8_t *bss_data = NULL;
    uint32_t count = 0;

    report_data[0] = CFG_TYPE_AP_LIST;
    bss_data = report_data + WIFI_AP_LIST_PREFIX_LEN;
    for (uint32_t idx = 0; idx < real_ap_number; idx++) {
        if (strlen(scan_ret[idx].ssid) == 0) {
            continue;
        }
        if (memcpy_s(bss.ssid, sizeof(bss.ssid), scan_ret[idx].ssid, sizeof(scan_ret[idx].ssid) - 1) != EOK) {
            return;
        }
        bss.rssi = (int8_t)scan_ret[idx].rssi;
        if (memcpy_s(bss_data, sizeof(bgwc_wifi_bss), &bss, sizeof(bss)) != EOK) {
            return;
        }
        PRINT("%d:ssid[%s]\trssi[%d] \t\r\n", count, bss.ssid, bss.rssi);
        bss_data += sizeof(bgwc_wifi_bss);
        count++;
        if (count >= WIFI_AP_LIST_MAX_NUM) {
            break;
        }
    }
    report_data[1] = (uint8_t)count;
}

/* ============================================================
 *  WiFi event callbacks
 * ============================================================ */

static td_void bgwc_scan_state_changed(td_s32 state, td_s32 size)
{
    wifi_scan_info_stru *scan_ret = NULL;
    uint32_t real_ap_number = (uint32_t)size;
    uint8_t *report_data = NULL;
    uint32_t max_len;
    PRINT("%s scan_state_changed enter.\n", BGLE_WIFI_CFG_LOG);
    UNUSED(state);

    g_bgwc_state = CONFIG_DEMO_WIFI_SCAN_DONE;
    if ((get_wifi_list_req_flag() == 0) || (size <= 0)) {
        return;
    }

    scan_ret = (wifi_scan_info_stru *)malloc(sizeof(wifi_scan_info_stru) * real_ap_number);
    if (scan_ret == NULL) {
        return;
    }
    memset_s(scan_ret, sizeof(wifi_scan_info_stru) * real_ap_number, 0, sizeof(wifi_scan_info_stru) * real_ap_number);
    if (wifi_sta_get_scan_info(scan_ret, &real_ap_number) != ERRCODE_SUCC) {
        free(scan_ret);
        return;
    }

    max_len = sizeof(bgwc_wifi_bss) * WIFI_AP_LIST_MAX_NUM + WIFI_AP_LIST_PREFIX_LEN;
    report_data = (uint8_t *)malloc(max_len);
    if (report_data == NULL) {
        free(scan_ret);
        return;
    }
    memset_s(report_data, max_len, 0, max_len);
    bgwc_get_ap_list_info(scan_ret, real_ap_number, report_data);
    ble_wifi_cfg_server_send_report_by_uuid((const uint8_t *)report_data,
                                            sizeof(bgwc_wifi_bss) * report_data[1] + WIFI_AP_LIST_PREFIX_LEN);
    set_wifi_list_req_flag(0);
    free(scan_ret);
    free(report_data);
}

static void bgwc_wifi_reason_code(td_s32 reason_code, int8_t *err_code)
{
    int disconn_by_ap = (reason_code >> WLAN_DISCONN_BY_AP_BIT) & 1;
    reason_code = reason_code & ~(1 << WLAN_DISCONN_BY_AP_BIT);

    if (disconn_by_ap == 1) {
        switch (reason_code) {
            case 0:
                *err_code = WIFI_ERRCODE_NONE;
                break;
            case WLAN_REASON_PREV_AUTH_NOT_VALID:
            case WLAN_REASON_MICHAEL_MIC_FAILURE:
            case WLAN_REASON_4WAY_HANDSHAKE_TIMEOUT:
            case WLAN_REASON_GROUP_KEY_UPDATE_TIMEOUT:
                *err_code = WIFI_ERRCODE_PWD_ERROR;
                break;
            default:
                *err_code = WIFI_ERRCODE_OTHERS;
                break;
        }
        return;
    }
    if (reason_code >= MAC_STATUS_MAX) {
        reason_code -= MAC_STATUS_MAX;
    }

    if (reason_code == WLAN_DISASOC_MISC_LINKLOSS) {
        *err_code = WIFI_ERRCODE_BEACON_LOST;
    } else if (reason_code == WIFI_NETWORK_NOT_FOUND_ERROR) {
        *err_code = WIFI_ERRCODE_SSID_NOT_FOUND;
    } else {
        switch (reason_code) {
            case WLAN_STATUS_CHALLENGE_FAIL:
                *err_code = WIFI_ERRCODE_PWD_ERROR;
                break;
            default:
                *err_code = WIFI_ERRCODE_OTHERS;
                break;
        }
    }
}

static td_void bgwc_connection_changed(td_s32 state, const wifi_linked_info_stru *info, td_s32 reason_code)
{
    UNUSED(state);
    PRINT("%s connection_changed enter.\n", BGLE_WIFI_CFG_LOG);

    g_errcode = WIFI_ERRCODE_NONE;
    if (info->conn_state == WIFI_DISCONNECTED) {
        bgwc_wifi_reason_code(reason_code, &g_errcode);
    }
    g_bgwc_state = CONFIG_DEMO_WIFI_CONNECT_DONE;
}

static td_void bgwc_softap_state_changed(td_s32 state)
{
    UNUSED(state);
    PRINT("%s softap_state_changed enter.\n", BGLE_WIFI_CFG_LOG);
}

static wifi_event_stru ble_wifi_cfg_event_cb = {.wifi_event_scan_state_changed = bgwc_scan_state_changed,
                                                .wifi_event_connection_changed = bgwc_connection_changed,
                                                .wifi_event_softap_state_changed = bgwc_softap_state_changed};

/* ============================================================
 *  WiFi connect / disconnect
 * ============================================================ */

static int bgwc_wifi_connect(void)
{
    wifi_sta_config_stru expected_bss = {0};
    if (example_get_match_network(&expected_bss) != 0) {
        PRINT("Do not find AP, try again !\r\n");
        g_errcode = WIFI_ERRCODE_SSID_NOT_FOUND;
        return -1;
    }
    if (wifi_sta_connect(&expected_bss) != 0) {
        PRINT("STA connect fail.\r\n");
        g_errcode = WIFI_ERRCODE_OTHERS;
        return -1;
    }
    prov_set_state(PROV_STATE_WIFI_CONNECTING);
    return 0;
}

/* ============================================================
 *  BLE + WiFi initialisation
 * ============================================================ */

static void bgwc_ble_start(void)
{
    errcode_t ret = ERRCODE_SUCC;
    ret |= ble_wifi_cfg_server_init();
    PRINT("%s Ble Init State:%d.\r\n", BGLE_WIFI_CFG_LOG, ret);
    ret |= ble_wifi_cfg_start_adv();
    PRINT("%s Ble Adv State:%d.\r\n", BGLE_WIFI_CFG_LOG, ret);
}

static void bgwc_ble_stop(void)
{
    (void)gap_ble_stop_adv(BTH_GAP_BLE_ADV_HANDLE_DEFAULT);
    PRINT("%s BLE adv stopped.\r\n", BGLE_WIFI_CFG_LOG);
}

static int bgwc_wifi_start(void)
{
    g_bgwc_state = CONFIG_DEMO_WIFI_INIT;
    if (wifi_sta_enable() != 0) {
        PRINT("%s sta enbale fail !\r\n", BGLE_WIFI_CFG_LOG);
        return -1;
    }
    if (wifi_register_event_cb(&ble_wifi_cfg_event_cb) != 0) {
        PRINT("%s wifi_register_event_cb fail.\r\n", BGLE_WIFI_CFG_LOG);
        return -1;
    }
    return 0;
}

/* ============================================================
 *  NV-based quick-connect path (skips BLE when pre-configured)
 * ============================================================ */

#ifdef CONFIG_BLE_PROV_NV_ENABLE
static int bgwc_nv_quick_connect(void)
{
    char ssid[WIFI_MAX_SSID_LEN];
    char password[WIFI_MAX_KEY_LEN];

    if (!ble_wifi_prov_nv_is_configured()) {
        return -1; /* no saved config — fall through to BLE provisioning */
    }

    PRINT("%s NV configured, loading credentials...\r\n", BGLE_WIFI_CFG_LOG);

    if (ble_wifi_prov_nv_load(ssid, sizeof(ssid), password, sizeof(password)) != ERRCODE_SUCC) {
        PRINT("%s NV load fail, fall back to provisioning.\r\n", BGLE_WIFI_CFG_LOG);
        ble_wifi_prov_nv_clear();
        return -1;
    }

    /* Populate g_data with loaded credentials for the existing flow */
    if (memset_s(g_data, WIFI_MAX_CONFIG_INFO_LEN, 0, WIFI_MAX_CONFIG_INFO_LEN) != EOK) {
        return -1;
    }
    (void)memcpy_s(g_data, WIFI_CONFIG_INFO_SSID_LEN, ssid, strlen(ssid));
    (void)memcpy_s(g_data + WIFI_CONFIG_INFO_SSID_LEN, WIFI_CONFIG_INFO_KEY_LEN, password, strlen(password));
    set_wifi_cfg_info_flag(1);
    prov_set_state(PROV_STATE_NV_CONFIGURED);

    PRINT("%s loaded ssid=%s\r\n", BGLE_WIFI_CFG_LOG, ssid);
    return 0;
}
#endif

/* ============================================================
 *  Main provisioning task
 * ============================================================ */

/* ---- shared DHCP helper ---- */
static int8_t prov_dhcp_and_get_ip(void)
{
    td_char ifname[WIFI_IFNAME_MAX_SIZE + 1] = "wlan0";
    struct netif *netif_p = netifapi_netif_find(ifname);

    PRINT("STA DHCP start.\r\n");
    prov_set_state(PROV_STATE_WIFI_DHCP);

    if (netif_p == NULL) {
        PRINT("not find %s.\r\n", ifname);
        return (int8_t)WIFI_ERRCODE_DHCP_FAILED;
    }
    if (netifapi_dhcp_start(netif_p) != 0) {
        PRINT("STA DHCP Fail.\r\n");
        return (int8_t)WIFI_ERRCODE_DHCP_FAILED;
    }

    for (td_char i = 0; i < WIFI_GET_IP_MAX_TIMES; i++) {
        (td_void) osDelay(WIFI_STATUS_POLL_DELAY_TICKS);
        if (ip_addr_isany(&(netif_p->ip_addr)) == 0) {
            PRINT("STA DHCP Succ, IP=%s.\r\n", ipaddr_ntoa(&(netif_p->ip_addr)));
            return (int8_t)WIFI_ERRCODE_NONE;
        }
    }
    PRINT("STA DHCP timeout.\r\n");
    return (int8_t)WIFI_ERRCODE_DHCP_FAILED;
}

/* ---- shared WiFi scan + connect + DHCP (no BLE) ---- */
static int8_t prov_wifi_scan_connect_dhcp(void)
{
    g_bgwc_state = CONFIG_DEMO_WIFI_INIT;
    prov_set_state(PROV_STATE_WIFI_SCANNING);

    /* Scan */
    PRINT("wifi_sta_scan start.\r\n");
    if (wifi_sta_scan() != 0) {
        PRINT("wifi_sta_scan fail.\n");
        return (int8_t)WIFI_ERRCODE_SSID_NOT_FOUND;
    }

    /* Wait for scan done */
    while (g_bgwc_state != CONFIG_DEMO_WIFI_SCAN_DONE) {
        (td_void) osDelay(PROV_TASK_DELAY_TICK);
    }

    /* Connect */
    PRINT("wifi_sta_connect start.\r\n");
    if (bgwc_wifi_connect() != 0) {
        PRINT("bgwc_wifi_connect fail.\n");
        int8_t e = get_wifi_errcode();
        return (e != WIFI_ERRCODE_NONE) ? e : (int8_t)WIFI_ERRCODE_OTHERS;
    }
    prov_set_state(PROV_STATE_WIFI_CONNECTING);

    /* Wait for connection result */
    while (g_bgwc_state != CONFIG_DEMO_WIFI_CONNECT_DONE) {
        (td_void) osDelay(PROV_TASK_DELAY_TICK);
    }

    int8_t err = get_wifi_errcode();
    if (err != WIFI_ERRCODE_NONE) {
        PRINT("STA ASSOC Fail, errcode=%d.\r\n", err);
        return err;
    }

    return prov_dhcp_and_get_ip();
}

/**
 * Run one round of BLE provisioning: start BLE, advertise, wait for
 * phone to write credentials, then scan / connect / DHCP.
 * Returns the final errcode (0 = success).
 */
static int8_t prov_run_one_round(bool first_attempt)
{
    uint32_t prov_start_tick = (uint32_t)osKernelGetTickCount();

    if (first_attempt) {
        bgwc_ble_start();
        bgwc_wifi_start();
    }
    prov_set_state(PROV_STATE_BLE_ADV);

    /* ---- Phase 1: wait for phone to write credentials OR timeout ---- */
    while (1) {
        (td_void) osDelay(PROV_TASK_DELAY_TICK);

        if (((uint32_t)osKernelGetTickCount() - prov_start_tick) > PROV_TIMEOUT_TICKS) {
            PRINT("%s provisioning timeout (%d s).\r\n", BGLE_WIFI_CFG_LOG, CONFIG_BLE_PROV_TIMEOUT_SEC);
            return (int8_t)WIFI_ERRCODE_OTHERS;
        }

        if ((get_wifi_cfg_info_flag() || get_wifi_list_req_flag()) && g_bgwc_state == CONFIG_DEMO_WIFI_INIT) {
            PRINT("wifi cfg flag:%d, wifi list flag:%d.\n", get_wifi_cfg_info_flag(), get_wifi_list_req_flag());
            if (wifi_sta_scan() != 0) {
                PRINT("wifi_sta_scan fail.\n");
                g_bgwc_state = CONFIG_DEMO_WIFI_INIT;
            } else {
                prov_set_state(PROV_STATE_WIFI_SCANNING);
                break;
            }
        }
    }

    /* ---- Phase 2: wait for scan done, then connect ---- */
    while (1) {
        (td_void) osDelay(PROV_TASK_DELAY_TICK);

        if (get_wifi_cfg_info_flag() && g_bgwc_state == CONFIG_DEMO_WIFI_SCAN_DONE) {
            if (bgwc_wifi_connect() == 0) {
                g_bgwc_state = CONFIG_DEMO_WIFI_CONNECT_DOING;
            } else {
                PRINT("bgwc_wifi_connect fail.\n");
                g_bgwc_state = CONFIG_DEMO_WIFI_INIT;
                break;
            }
        }
        if (g_bgwc_state == CONFIG_DEMO_WIFI_CONNECT_DONE) {
            break;
        }
    }

    /* ---- Phase 3: check connection result ---- */
    int8_t err = get_wifi_errcode();
    if (err != WIFI_ERRCODE_NONE) {
        PRINT("STA ASSOC Fail, errcode=%d.\r\n", err);
        return err;
    }

    return prov_dhcp_and_get_ip();
}

static void prov_reset_attempt(void)
{
    osal_msleep(PROV_RETRY_DELAY_MS);
    set_wifi_cfg_info_flag(0);
    set_wifi_list_req_flag(0);
    g_errcode = WIFI_ERRCODE_NONE;
    g_bgwc_state = CONFIG_DEMO_WIFI_INIT;
}

static int8_t prov_run_attempt(int attempt, bool *use_ble)
{
    if ((attempt == 0) && !(*use_ble)) {
        PRINT("%s NV connect attempt...\r\n", BGLE_WIFI_CFG_LOG);
        (void)bgwc_wifi_start();
        return prov_wifi_scan_connect_dhcp();
    }

    bool start_ble = (attempt == 0);
    if (!(*use_ble)) {
        *use_ble = true;
        start_ble = true;
        PRINT("%s NV failed, switching to BLE provisioning.\r\n", BGLE_WIFI_CFG_LOG);
    }
    return prov_run_one_round(start_ble);
}

#ifdef CONFIG_BLE_PROV_NV_ENABLE
static void prov_save_credentials(void)
{
    char ssid_buf[WIFI_CONFIG_INFO_SSID_LEN];
    char pwd_buf[WIFI_CONFIG_INFO_KEY_LEN];
    (void)memcpy_s(ssid_buf, sizeof(ssid_buf), g_data, WIFI_CONFIG_INFO_SSID_LEN);
    (void)memcpy_s(pwd_buf, sizeof(pwd_buf), g_data + WIFI_CONFIG_INFO_SSID_LEN, WIFI_CONFIG_INFO_KEY_LEN);
    if (ble_wifi_prov_nv_save(ssid_buf, pwd_buf) == ERRCODE_SUCC) {
        PRINT("%s Credentials saved to NV.\r\n", BGLE_WIFI_CFG_LOG);
    }
}
#endif

static int prov_finish_success(bool use_ble, int attempt)
{
    PRINT("%s SUCCESS (attempt %d).\r\n", BGLE_WIFI_CFG_LOG, attempt + 1);
    if (use_ble) {
        bgwc_ble_stop();
    }
#ifdef CONFIG_BLE_PROV_NV_ENABLE
    if (use_ble) {
        prov_save_credentials();
    }
#endif
    prov_set_state(PROV_STATE_SUCCESS);
    return 0;
}

/**
 * Main provisioning task: NV quick-connect on first boot, then
 * retries BLE provisioning on failure until success or exhaustion.
 */
static int ble_wifi_cfg_example_task(const char *arg)
{
    uint8_t result[WIFI_AP_LIST_PREFIX_LEN] = {CFG_TYPE_WIFI_STATE, 0};
    int8_t last_errcode;
    bool use_ble = true; /* default: full BLE provisioning */
    UNUSED(arg);

    (td_void) osDelay(PROV_SYSTEM_READY_DELAY_TICKS);

    /* ---- Check NV for pre-saved credentials ---- */
#ifdef CONFIG_BLE_PROV_NV_ENABLE
    if (bgwc_nv_quick_connect() == 0) {
        use_ble = false; /* NV path: skip BLE, go straight to WiFi */
    }
#endif

    /* ---- Retry loop ---- */
#ifndef CONFIG_BLE_PROV_MAX_RETRIES
#define CONFIG_BLE_PROV_MAX_RETRIES 3
#endif

    for (int attempt = 0; attempt < CONFIG_BLE_PROV_MAX_RETRIES; attempt++) {
        if (attempt > 0) {
            prov_reset_attempt();
        }
        last_errcode = prov_run_attempt(attempt, &use_ble);

        result[1] = (uint8_t)last_errcode;
        if (use_ble) {
            ble_wifi_cfg_server_send_report_by_uuid((const uint8_t *)result, sizeof(result));
        }

        if (last_errcode == (int8_t)WIFI_ERRCODE_NONE) {
            return prov_finish_success(use_ble, attempt);
        }

        PRINT("%s FAILED attempt %d/%d, errcode=%d.\r\n", BGLE_WIFI_CFG_LOG, attempt + 1, CONFIG_BLE_PROV_MAX_RETRIES,
              last_errcode);
        prov_set_state(PROV_STATE_FAILED);
    }

    /* ---- All retries exhausted ---- */
    if (use_ble) {
        bgwc_ble_stop();
    }
    prov_set_state(PROV_STATE_TIMEOUT);
    PRINT("%s All retries exhausted, entering deep sleep.\r\n", BGLE_WIFI_CFG_LOG);
    while (1) {
        osal_msleep(PROV_RESTART_DELAY_MS);
    }
    return -1;
}

/* ============================================================
 *  Entry point (registered via linker section)
 * ============================================================ */

#define BGWC_TASK_PRIO (osPriority_t)(26)
#define BGWC_TASK_STACK_SIZE 0x1000

static void bgle_wifi_cfg_entry(void)
{
#ifdef CONFIG_BLE_PROV_LED_ENABLE
    ble_wifi_prov_led_init((uint8_t)CONFIG_BLE_PROV_LED_PIN);
#endif

#ifdef CONFIG_BLE_PROV_BTN_ENABLE
    ble_wifi_prov_btn_init((uint8_t)CONFIG_BLE_PROV_BTN_PIN);
#endif

    osal_kthread_lock();
    osal_task *g_wifi_cfg_task = osal_kthread_create((osal_kthread_handler)ble_wifi_cfg_example_task, 0,
                                                     "bgle_wifi_cfg_task", BGWC_TASK_STACK_SIZE);
    if (g_wifi_cfg_task != NULL) {
        osal_kthread_set_priority(g_wifi_cfg_task, BGWC_TASK_PRIO);
        osal_kfree(g_wifi_cfg_task);
    }
    osal_kthread_unlock();
}

/* Run the ble_wifi_cfg_entry. */
app_run(bgle_wifi_cfg_entry);

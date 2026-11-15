/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: BLE WiFi provisioning NV storage module implementation.
 *              Uses uapi_nv_write / uapi_nv_read / uapi_nv_delete_key from nv.h.
 *              NV init (uapi_nv_init) is already called in hw_init() of main.c.
 */

#include <string.h>
#include "nv.h"
#include "securec.h"
#include "osal_debug.h"
#include "ble_wifi_prov_nv.h"

#define PROV_NV_TAG "[PROV_NV]"

errcode_t ble_wifi_prov_nv_save(const char *ssid, const char *password)
{
    errcode_t ret;
    uint8_t configured_flag = 1;

    if (ssid == NULL || password == NULL) {
        osal_printk("%s save: invalid param\r\n", PROV_NV_TAG);
        return ERRCODE_INVALID_PARAM;
    }

    /* Save SSID */
    ret = uapi_nv_write(NV_KEY_WIFI_SSID, (const uint8_t *)ssid,
                        (uint16_t)(strlen(ssid) + 1));
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s save ssid fail: 0x%x\r\n", PROV_NV_TAG, ret);
        return ret;
    }

    /* Save password */
    ret = uapi_nv_write(NV_KEY_WIFI_PASSWORD, (const uint8_t *)password,
                        (uint16_t)(strlen(password) + 1));
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s save password fail: 0x%x\r\n", PROV_NV_TAG, ret);
        return ret;
    }

    /* Save configured flag */
    ret = uapi_nv_write(NV_KEY_WIFI_CONFIGURED, &configured_flag, sizeof(configured_flag));
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s save flag fail: 0x%x\r\n", PROV_NV_TAG, ret);
        return ret;
    }

    osal_printk("%s credentials saved ok\r\n", PROV_NV_TAG);
    return ERRCODE_SUCC;
}

errcode_t ble_wifi_prov_nv_load(char *ssid, uint16_t ssid_max,
                                char *password, uint16_t pwd_max)
{
    uint16_t len;

    if (ssid == NULL || password == NULL || ssid_max == 0 || pwd_max == 0) {
        return ERRCODE_INVALID_PARAM;
    }

    /* Read SSID */
    if (memset_s(ssid, ssid_max, 0, ssid_max) != EOK) {
        return ERRCODE_FAIL;
    }
    if (uapi_nv_read(NV_KEY_WIFI_SSID, ssid_max, &len, (uint8_t *)ssid) != ERRCODE_SUCC) {
        osal_printk("%s load ssid fail\r\n", PROV_NV_TAG);
        return ERRCODE_FAIL;
    }
    if (len == 0 || ssid[0] == '\0') {
        osal_printk("%s ssid is empty\r\n", PROV_NV_TAG);
        return ERRCODE_FAIL;
    }

    /* Read password */
    if (memset_s(password, pwd_max, 0, pwd_max) != EOK) {
        return ERRCODE_FAIL;
    }
    if (uapi_nv_read(NV_KEY_WIFI_PASSWORD, pwd_max, &len, (uint8_t *)password) != ERRCODE_SUCC) {
        osal_printk("%s load password fail\r\n", PROV_NV_TAG);
        return ERRCODE_FAIL;
    }

    osal_printk("%s credentials loaded: ssid=%s\r\n", PROV_NV_TAG, ssid);
    return ERRCODE_SUCC;
}

errcode_t ble_wifi_prov_nv_clear(void)
{
    uint8_t zero = 0;

    /* Write zero-length / zero-value to invalidate saved credentials.
     * There is no uapi_nv_delete_key; clearing the configured flag
     * to 0 effectively marks the device as unconfigured. */
    (void)uapi_nv_write(NV_KEY_WIFI_CONFIGURED, &zero, sizeof(zero));
    (void)uapi_nv_write(NV_KEY_WIFI_SSID, &zero, sizeof(zero));
    (void)uapi_nv_write(NV_KEY_WIFI_PASSWORD, &zero, sizeof(zero));
    osal_printk("%s credentials cleared\r\n", PROV_NV_TAG);
    return ERRCODE_SUCC;
}

bool ble_wifi_prov_nv_is_configured(void)
{
    uint16_t len = 0;
    uint8_t flag = 0;
    errcode_t ret;

    ret = uapi_nv_read(NV_KEY_WIFI_CONFIGURED, sizeof(flag), &len, &flag);
    return (ret == ERRCODE_SUCC && len == sizeof(flag) && flag == 1);
}

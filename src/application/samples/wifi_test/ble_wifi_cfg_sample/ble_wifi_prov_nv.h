/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: BLE WiFi provisioning NV storage module — persist WiFi credentials
 *              across reboots using the NV (Non-Volatile) key-value storage.
 */

#ifndef BLE_WIFI_PROV_NV_H
#define BLE_WIFI_PROV_NV_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*
 * NV Key IDs — allocated from user normal area [0x5000, 0xFFFF).
 * See src/middleware/chips/ws63/nv/nv_config/include/key_id.h
 */
#define NV_KEY_WIFI_SSID       0x5001  /**< WiFi SSID,  max 33 bytes (ASCII + '\0') */
#define NV_KEY_WIFI_PASSWORD   0x5002  /**< WiFi password, max 65 bytes (ASCII + '\0') */
#define NV_KEY_WIFI_CONFIGURED 0x5003  /**< Provisioned flag: 1 byte (0 = no, 1 = yes) */

/**
 * @brief Save WiFi credentials to NV storage.
 * @param ssid      Null-terminated SSID string, max 32 chars.
 * @param password  Null-terminated password string, max 64 chars.
 * @return ERRCODE_SUCC on success, ERRCODE_FAIL otherwise.
 */
errcode_t ble_wifi_prov_nv_save(const char *ssid, const char *password);

/**
 * @brief Load WiFi credentials from NV storage.
 * @param[out] ssid      Buffer to receive SSID, at least ssid_max_len bytes.
 * @param[in]  ssid_max  Size of the ssid output buffer.
 * @param[out] password  Buffer to receive password, at least pwd_max_len bytes.
 * @param[in]  pwd_max   Size of the password output buffer.
 * @return ERRCODE_SUCC on success, ERRCODE_FAIL if not found or read error.
 */
errcode_t ble_wifi_prov_nv_load(char *ssid, uint16_t ssid_max,
                                char *password, uint16_t pwd_max);

/**
 * @brief Clear all stored WiFi credentials (factory reset of provisioning data).
 * @return ERRCODE_SUCC on success.
 */
errcode_t ble_wifi_prov_nv_clear(void);

/**
 * @brief Check if a valid WiFi configuration exists in NV.
 * @return true if configuration exists, false otherwise.
 */
bool ble_wifi_prov_nv_is_configured(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* BLE_WIFI_PROV_NV_H */

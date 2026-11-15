 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: ble mesh hex.
 *
 * History:
 * 2025-8-2, Create file.
 */
#ifndef __BLE_MESH_HEX_H__
#define __BLE_MESH_HEX_H__
#include "bts_def.h"
#ifdef __cplusplus
extern "C" {
#endif

int32_t ble_mesh_hex2bin(const char *hex, uint8_t *bin, uint32_t bin_len);
uint32_t ble_mesh_bin2hex(const uint8_t *buf, uint32_t buflen, char *hex, uint32_t hexlen);

#ifdef __cplusplus
}
#endif

#endif /* __BLE_MESH_HEX_H__ */

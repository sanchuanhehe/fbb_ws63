/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description:
 *
 * History:
 * 2025-6-04, Create file.
 */
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "errcode.h"
#include "nv_config.h"
#include "nv.h"
#include "ble_mesh_nv_op.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

errcode_t ble_mesh_data_write(uint16_t key, const uint8_t *data, uint16_t len)
{
    return uapi_nv_write(key, data, len);
}

errcode_t ble_mesh_data_write_encrypt(uint16_t key, const uint8_t *data, uint16_t len)
{
#if defined(CONFIG_NV_SUPPORT_ENCRYPT) && (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
    nv_key_attr_t attr = {0};
    attr.encrypted = true;
    return uapi_nv_write_with_attr(key, data, len, &attr, NULL);
#else
    return uapi_nv_write(key, data, len);
#endif
}

errcode_t ble_mesh_data_read(uint16_t key, uint16_t len, uint16_t *real_len, uint8_t *data)
{
    return uapi_nv_read(key, len, real_len, data);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: DFD Model Implementation.
 *
 * History:
 * 2025-05-01, Create file.
 */

#ifndef _DFD_SRV_INTERNAL_H_
#define _DFD_SRV_INTERNAL_H_

#include "blob.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bt_mesh_dfd_start_params {
    uint16_t app_idx;
    uint16_t timeout_base;
    uint16_t bank_idx;
    uint16_t group;
    enum bt_mesh_blob_transfer_mode_t transfer_mode;
    uint8_t ttl;
    bool apply;
};

#ifdef __cplusplus
}
#endif

#endif /* _DFD_SRV_INTERNAL_H_ */

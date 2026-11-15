 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: DFU Model Implementation.
 *
 * History:
 * 2025-05-01, Create file.
 */

#ifndef _DFU_H__
#define _DFU_H__

#include <sys/types.h>

#include "blob.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DFU_UPDATE_INFO_STATUS_MSG_HEAD 4
#define DFU_UPDATE_START_MSG_HEAD       12

enum bt_mesh_dfu_phase_t {
    BT_MESH_DFU_PHASE_IDLE              = 0x00,
    BT_MESH_DFU_PHASE_TRANSFER_ERR      = 0x01,
    BT_MESH_DFU_PHASE_TRANSFER_ACTIVE   = 0x02,
    BT_MESH_DFU_PHASE_VERIFY            = 0x03,
    BT_MESH_DFU_PHASE_VERIFY_OK         = 0x04,
    BT_MESH_DFU_PHASE_VERIFY_FAIL       = 0x05,
    BT_MESH_DFU_PHASE_APPLYING          = 0x06,
    BT_MESH_DFU_PHASE_TRANSFER_CANCELED = 0x07,
    BT_MESH_DFU_PHASE_APPLY_SUCCESS     = 0x08,
    BT_MESH_DFU_PHASE_APPLY_FAIL        = 0x09,
    BT_MESH_DFU_PHASE_UNKNOWN           = 0x0a,
};

enum bt_mesh_dfu_status_t {
    BT_MESH_DFU_SUCCESS                     = 0x00,
    BT_MESH_DFU_ERR_RESOURCES               = 0x01,
    BT_MESH_DFU_ERR_WRONG_PHASE             = 0x02,
    BT_MESH_DFU_ERR_INTERNAL                = 0x03,
    BT_MESH_DFU_ERR_FW_IDX                  = 0x04,
    BT_MESH_DFU_ERR_METADATA                = 0x05,
    BT_MESH_DFU_ERR_TEMPORARILY_UNAVAILABLE = 0x06,
    BT_MESH_DFU_ERR_BLOB_BUSY               = 0x07,
};

enum bt_mesh_dfu_effect_t {
    BT_MESH_DFU_EFFECT_NONE               = 0x00,
    BT_MESH_DFU_EFFECT_COMP_CHANGE_NO_RPR = 0x01,
    BT_MESH_DFU_EFFECT_COMP_CHANGE        = 0x02,
    BT_MESH_DFU_EFFECT_UNPROV             = 0x03,
};

enum bt_mesh_dfu_iter_t {
    BT_MESH_DFU_ITER_STOP     = 0x00,
    BT_MESH_DFU_ITER_CONTINUE = 0x01,
};

struct bt_mesh_dfu_img_t {
    const void *fwid;
    uint32_t fwid_len;
    const char *uri;
};

struct bt_mesh_dfu_bank_t {
    uint32_t size;    // fw size
    uint32_t fwid_len;
    uint32_t metadata_len;
    uint8_t fwid[MYNEWT_VAL_BT_MESH_DFU_FWID_MAX_LEN];
    uint8_t metadata[MYNEWT_VAL_BT_MESH_DFU_METADATA_MAX_LEN];
};

#define DFU_UPDATE_INFO_STATUS_MSG_MINLEN \
    (DFU_UPDATE_INFO_STATUS_MSG_HEAD + MYNEWT_VAL_BT_MESH_DFU_FWID_MAX_LEN + MYNEWT_VAL_BT_MESH_DFU_URI_MAX_LEN)

#define DFU_UPDATE_START_MSG_MAXLEN (DFU_UPDATE_START_MSG_HEAD + MYNEWT_VAL_BT_MESH_DFU_METADATA_MAX_LEN)

#ifdef __cplusplus
}
#endif

#endif /* _DFU_H__ */

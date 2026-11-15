/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: DFD Model Implementation.
 *
 * History:
 * 2025-05-01, Create file.
 */

#ifndef _DFD_H_
#define _DFD_H_

#ifdef __cplusplus
extern "C" {
#endif

enum bt_mesh_dfd_status_t {
    BT_MESH_DFD_SUCCESS                             = 0x00,
    BT_MESH_DFD_ERR_INSUFFICIENT_RESOURCES          = 0x01,
    BT_MESH_DFD_ERR_WRONG_PHASE                     = 0x02,
    BT_MESH_DFD_ERR_INTERNAL                        = 0x03,
    BT_MESH_DFD_ERR_FW_NOT_FOUND                    = 0x04,
    BT_MESH_DFD_ERR_INVALID_APPKEY_INDEX            = 0x05,
    BT_MESH_DFD_ERR_RECEIVERS_LIST_EMPTY            = 0x06,
    BT_MESH_DFD_ERR_BUSY_WITH_DISTRIBUTION          = 0x07,
    BT_MESH_DFD_ERR_BUSY_WITH_UPLOAD                = 0x08,
    BT_MESH_DFD_ERR_URI_NOT_SUPPORTED               = 0x09,
    BT_MESH_DFD_ERR_URI_MALFORMED                   = 0x0A,
    BT_MESH_DFD_ERR_URI_UNREACHABLE                 = 0x0B,
    BT_MESH_DFD_ERR_NEW_FW_NOT_AVAILABLE            = 0x0C,
    BT_MESH_DFD_ERR_SUSPEND_FAILED                  = 0x0D,
};

enum bt_mesh_dfd_phase_t {
    BT_MESH_DFD_PHASE_IDLE                          = 0x00,
    BT_MESH_DFD_PHASE_TRANSFER_ACTIVE               = 0x01,
    BT_MESH_DFD_PHASE_TRANSFER_SUCCESS              = 0x02,
    BT_MESH_DFD_PHASE_APPLYING_UPDATE               = 0x03,
    BT_MESH_DFD_PHASE_COMPLETED                     = 0x04,
    BT_MESH_DFD_PHASE_FAILED                        = 0x05,
    BT_MESH_DFD_PHASE_CANCELING_UPDATE              = 0x06,
    BT_MESH_DFD_PHASE_TRANSFER_SUSPENDED            = 0x07,
};

enum bt_mesh_dfd_upload_phase_t {
    BT_MESH_DFD_UPLOAD_PHASE_IDLE                   = 0x00,
    BT_MESH_DFD_UPLOAD_PHASE_TRANSFER_ACTIVE        = 0x01,
    BT_MESH_DFD_UPLOAD_PHASE_TRANSFER_ERROR         = 0x02,
    BT_MESH_DFD_UPLOAD_PHASE_TRANSFER_SUCCESS       = 0x03,
};

enum bt_mesh_dfd_upload_type_t {
    BT_MESH_DFD_UPLOAD_TYPE_INBAND  = 0x00,
    BT_MESH_DFD_UPLOAD_TYPE_OOB     = 0x01,
};

#ifdef __cplusplus
}
#endif

#endif /* _DFD_H_ */

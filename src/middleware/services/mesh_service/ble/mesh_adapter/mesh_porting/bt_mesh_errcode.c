/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description:
 *
 * History:
 * 2025-6-04, Create file.
 */
#include <errno.h>
#include "net.h"
#include "foundation.h"
#include "ble_mesh_errcode.h"
#include "ble_mesh_errcode_inner.h"

errcode_t ble_mesh_errcode_convert(int err_no)
{
    if (err_no == 0) {
        return ERRCODE_BLE_MESH_SUCCESS;
    }
    errcode_t mesh_err = ERRCODE_BLE_MESH_FAIL;
    switch (err_no) {
        case -ENOBUFS:
            mesh_err = ERRCODE_BLE_MESH_ALLOC_BUFF_FAILED;
            break;
        case -EINVAL:
        case -EMSGSIZE:
        case -ENOENT:
            mesh_err = ERRCODE_BLE_MESH_PARAM_ERR;
            break;
        case -EAGAIN:
        case -ENOTSUP:
        case -EALREADY:
            mesh_err = ERRCODE_BLE_MESH_STATUS_ERR;
            break;
        case -ENOMEM:
        case STATUS_INSUFF_RESOURCES:
            mesh_err = ERRCODE_BLE_MESH_RESOURCE_INSUFFICIENT;
            break;
        case -EBUSY:
            mesh_err = ERRCODE_BLE_MESH_BUSY;
            break;
        case STATUS_INVALID_NETKEY:
            mesh_err = ERRCODE_BLE_MESH_NET_KEY_INVALID;
            break;
        case STATUS_IDX_ALREADY_STORED:
            mesh_err = ERRCODE_BLE_MESH_APP_KEY_ALREADY_STORED;
            break;
        case STATUS_CANNOT_SET:
            mesh_err = ERRCODE_BLE_MESH_APP_KEY_CANNOT_SET;
            break;
        default:
            return ERRCODE_BLE_MESH_FAIL;
    }
    return mesh_err;
}
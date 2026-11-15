/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Implement for Common
 *
 * History:
 * 2025-5-22, Create file.
 */

#include "os_time.h"
#include "vnd_common.h"

uint8_t bt_mesh_vnd_recv_tid_handle(bt_mesh_vnd_mod_tid_t *mesh_handle, uint8_t tid)
{
    uint8_t ret = MESH_VND_TID_OK;
    uint32_t now = ble_npl_time_get_ms();

    BT_DBG("vnd mesh_handle->recv_tid_num:%d, tid:%d", mesh_handle->recv_tid_num, tid);

    // 重复tid不处理
    if (mesh_handle->recv_tid_num == tid && (now - mesh_handle->rcv_lastTime) < BT_MESH_VND_TID_EFFECT_TIME) {
        BT_ERR("vnd It is a repeat message.");
        ret = MESH_VND_TID_REPEAT;
    }

    mesh_handle->recv_tid_num = tid;
    mesh_handle->rcv_lastTime = now;

    return ret;
}
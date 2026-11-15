/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: defintion for Common
 *
 * History:
 * 2025-5-22, Create file.
 */

#ifndef __VND_COMMONM_H__
#define __VND_COMMONM_H__

#include "model_def.h"
#include "mesh/mesh.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BT_MESH_VND_TID_EFFECT_TIME (K_SECONDS(6))

#define BT_MESH_VND_TID_RESET_VALUE (0xff)
#define BT_MESH_VND_RESP_DATA_MAX_LEN (380)

typedef struct {
    uint8_t recv_tid_num;
    uint32_t rcv_lastTime;
    uint8_t reoport_tid_num;
} bt_mesh_vnd_mod_tid_t;

enum {
    MESH_VND_TID_OK,
    MESH_VND_TID_REPEAT,
    MESH_VND_TID_ERR,
};

uint8_t bt_mesh_vnd_recv_tid_handle(bt_mesh_vnd_mod_tid_t *mesh_handle, uint8_t tid);

#ifdef __cplusplus
}
#endif

#endif
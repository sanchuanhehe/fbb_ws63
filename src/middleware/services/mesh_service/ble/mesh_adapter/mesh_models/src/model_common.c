/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Implementation of Generic Model Interfaces
 *
 * History:
 * 2024-6-21, Create file.
 */
#include "mesh/glue.h"
#include "model_common.h"

#define MSG_TIMEOUT K_SECONDS(5)
#define LIGHT_CTL_IS_LAST_MSG_TIME K_SECONDS(6)

bool bt_mesh_is_server_recv_last_msg(struct last_msg *last, uint8_t tid, uint16_t src, uint16_t dst, int64_t *now)
{
    *now = k_uptime_get();

    if (last->tid == tid && last->src == src && last->dst == dst &&
        (*now - last->timestamp <= LIGHT_CTL_IS_LAST_MSG_TIME)) {
        return true;
    }

    return false;
}

void bt_mesh_server_update_last_msg(struct last_msg *last, uint8_t tid, uint16_t src, uint16_t dst, int64_t *now)
{
    last->tid = tid;
    last->src = src;
    last->dst = dst;
    last->timestamp = *now;
    return;
}

int cli_wait(struct bt_mesh_gen_model_cli *cli, void *param, uint32_t op)
{
    int err;

    cli->op_param = param;
    cli->op_pending = op;

    err = k_sem_take(&cli->op_sync, MSG_TIMEOUT);

    cli->op_pending = 0;
    cli->op_param = NULL;

    return err;
}

int set_option_field(struct os_mbuf *buf, uint8_t *trans_time, uint8_t *delay, uint8_t gen_def_trans_time)
{
    if (buf->om_len == 0x00) { /* No optional fields are available */
        *trans_time = gen_def_trans_time;
        *delay = 0;
    } else if (buf->om_len == 0x02) { /* Optional fields are available */
        *trans_time = net_buf_simple_pull_u8(buf);
        if (((*trans_time) & 0x3F) == 0x3F) {
            return BLE_MESH_FAIL;
        }
        *delay = net_buf_simple_pull_u8(buf);
    } else {
        return BLE_MESH_FAIL;
    }
    return BLE_MESH_SUCC;
}
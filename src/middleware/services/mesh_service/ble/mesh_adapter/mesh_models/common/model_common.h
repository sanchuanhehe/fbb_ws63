/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: General Model Interface Declaration
 *
 * History:
 * 2024-6-21, Create file.
 */
#ifndef __MODEL_COMMON_H__
#define __MODEL_COMMON_H__

#include <stdbool.h>
#include <stdint.h>
#include "model/model_def.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BLE_MESH_SUCC
#define BLE_MESH_SUCC 0
#endif
#ifndef BLE_MESH_FAIL
#define BLE_MESH_FAIL (-1)
#endif

#define MODEL_OP(X) BLE_MESH_MODEL_OP_##X

#define BLE_MESH_CHECK_SEND_STATUS(func)                              \
    do {                                                              \
        int status = (func);                                          \
        if (status) {                                                 \
            BT_ERR("line %d, Send failed, err %d", __LINE__, status); \
        }                                                             \
    } while (0)

#define MODEL_CLI_WAIT(err, cli, param, op)                \
    do {                                                   \
        err = cli_wait(cli, param, op);                    \
        if (err) {                                         \
            BT_ERR("cli_wait msg timeout,err is %d", err); \
        }                                                  \
    } while (0)

bool bt_mesh_is_server_recv_last_msg(struct last_msg *last, uint8_t tid, uint16_t src, uint16_t dst, int64_t *now);
void bt_mesh_server_update_last_msg(struct last_msg *last, uint8_t tid, uint16_t src, uint16_t dst, int64_t *now);
int cli_wait(struct bt_mesh_gen_model_cli *cli, void *param, uint32_t op);
int set_option_field(struct os_mbuf *buf, uint8_t *trans_time, uint8_t *delay, uint8_t gen_def_trans_time);

#ifdef __cplusplus
}
#endif

#endif /* __MODEL_COMMON_H__ */

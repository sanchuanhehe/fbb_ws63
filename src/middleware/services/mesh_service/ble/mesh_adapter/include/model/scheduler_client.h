/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Scheduler Client Interface Statement
 *
 * History:
 * 2025-5-21, Create file.
 */
#ifndef __SCHEDULER_CLIENT_H__
#define __SCHEDULER_CLIENT_H__

#include "model_def.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const struct bt_mesh_model_op bt_mesh_scheduler_cli_op[];
extern const struct bt_mesh_model_cb bt_mesh_scheduler_cli_cb;

#define BT_MESH_MODEL_SCHEDULER_CLI(cli_data, pub) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_SCHEDULER_CLI, bt_mesh_scheduler_cli_op, pub, cli_data, &bt_mesh_scheduler_cli_cb)

struct bt_mesh_scheduler_status_t {
    uint16_t schedules;
};

struct bt_mesh_scheduler_act_t {
    uint64_t index : 4;
    uint64_t year : 7;
    uint64_t month : 12;
    uint64_t day : 5;
    uint64_t hour : 5;
    uint64_t minute : 6;
    uint64_t second : 6;
    uint64_t day_of_week : 7;
    uint64_t action : 4;
    uint64_t trans_time : 8;
    uint16_t scene_number;
};

int bt_mesh_scheduler_get(bt_mesh_client_common_param_t *common, void *status);
int bt_mesh_scheduler_act_get(bt_mesh_client_common_param_t *common, void *status, uint8_t index);
int bt_mesh_scheduler_act_set(bt_mesh_client_common_param_t *common, struct bt_mesh_scheduler_act_t *set,
    struct bt_mesh_scheduler_act_t *status);

#ifdef __cplusplus
}
#endif

#endif /* __SCHEDULER_CLIENT_H__ */

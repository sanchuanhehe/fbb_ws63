 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Light CTL Client Model Statement
 *
 * History:
 * 2024-6-21, Create file.
 */
#ifndef __LIGHTNESS_CLIENT_H__
#define __LIGHTNESS_CLIENT_H__

#include "model_def.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const struct bt_mesh_model_op light_lightness_cli_op[];
extern const struct bt_mesh_model_cb bt_mesh_light_lightness_cli_cb;

#define BT_MESH_MODEL_LIGHT_LIGHTNESS_CLI(cli_data, pub)                                          \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI, light_lightness_cli_op, pub, cli_data, \
        &bt_mesh_light_lightness_cli_cb)

struct light_lightness_actual_status {
    uint16_t actual;
    uint16_t target_actual;
    uint8_t remain_time;
    uint8_t op_en;
};

struct light_lightness_linear_status {
    uint16_t linear;
    uint16_t target_linear;
    uint8_t remain_time;
    uint8_t op_en;
};

struct light_lightness_range_status {
    uint16_t light_range_min;
    uint16_t light_range_max;
    uint8_t status_code;
};

struct light_lightness_set {
    uint16_t linear;
    uint16_t actual;
    uint16_t last;
    uint16_t def;
    uint16_t light_range_min;
    uint16_t light_range_max;
    uint8_t tid;
    uint8_t trans_time;
    uint8_t delay;
    uint8_t op_en;
};

int bt_mesh_lightness_get(bt_mesh_client_common_param_t *common, void *status);
int bt_mesh_lightness_set(bt_mesh_client_common_param_t *common, struct light_lightness_set *set, void *status);

#ifdef __cplusplus
}
#endif

#endif /* __LIGHTNESS_CLIENT_H__ */
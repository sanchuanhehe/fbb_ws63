 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Light Lightness Server Interface Statement
 *
 * History:
 * 2024-6-21, Create file.
 */
#ifndef __LIGHTNESS_SERVER_H__
#define __LIGHTNESS_SERVER_H__

#include "model_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Following 2 values are as per Mesh Model specification */
#define LIGHTNESS_MIN 0x0001
#define LIGHTNESS_MAX 0xFFFF

struct light_lightness_state {
    uint16_t linear;
    uint16_t target_linear;

    uint16_t actual;
    uint16_t target_actual;

    uint16_t last;
    uint16_t def;

    uint8_t status_code;
    uint16_t light_range_min;
    uint16_t light_range_max;
};

struct bt_mesh_light_lightness_srv {
    struct bt_mesh_model *model;
    struct light_lightness_state *light_state;

    struct last_msg last_msg;
    struct transition actual_transition;
    struct transition linear_transition;
    int32_t tt_delta_actual;
    int32_t tt_delta_linear;

    int (*set)(struct bt_mesh_model *model, struct light_lightness_state *state);
};
extern const struct bt_mesh_model_op light_lightness_srv_op[];
extern const struct bt_mesh_model_cb light_lightness_srv_cb;
#define BT_MESH_MODEL_LIGHT_LIGHTNESS_SRV(srv, pub) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV, light_lightness_srv_op, pub, srv, &light_lightness_srv_cb)

struct bt_mesh_light_lightness_setup_srv {
    struct bt_mesh_model *model;
    struct light_lightness_state *light_state;
    int (*set)(struct bt_mesh_model *model, struct light_lightness_state *state);
};
extern const struct bt_mesh_model_op light_lightness_setup_srv_op[];
extern const struct bt_mesh_model_cb light_lightness_setup_srv_cb;
#define BT_MESH_MODEL_LIGHT_LIGHTNESS_SETUP_SRV(srv, pub)                                                \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV, light_lightness_setup_srv_op, pub, srv, \
        &light_lightness_setup_srv_cb)

#ifdef __cplusplus
}
#endif

#endif /* __LIGHTNESS_SERVER_H__ */
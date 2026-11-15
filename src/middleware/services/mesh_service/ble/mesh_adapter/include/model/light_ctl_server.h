 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Light CTL Server Model Statement
 *
 * History:
 * 2024-6-21, Create file.
 */
#ifndef __LIGHT_CTL_SERVER_H
#define __LIGHT_CTL_SERVER_H

#include "mesh/access.h"

/* Following 2 values are as per Mesh Model specification */
#define TEMP_MIN 0x0320
#define TEMP_MAX 0x4E20

typedef struct {
    uint16_t lightness;
    uint16_t target_lightness;

    uint16_t temp;
    uint16_t target_temp;

    int16_t delta_uv;
    int16_t target_delta_uv;

    uint8_t status_code;
    uint16_t temp_range_min;
    uint16_t temp_range_max;

    uint16_t lightness_def;
    uint16_t temp_def;
    int16_t delta_uv_def;
} ble_mesh_light_ctl_state;

/* * User data of Light CTL Server Model */
typedef struct {
    struct bt_mesh_model *model;     /* !< Pointer to the Lighting CTL Server Model. Initialized internally. */
    ble_mesh_light_ctl_state *state; /* !< Parameters of the Light CTL state */
    struct last_msg last;            /* !< Parameters of the last received set message */
    struct transition transition;    /* !< Parameters of state transition */

    int32_t tt_delta_lightness;
    int32_t tt_delta_temp;
    int32_t tt_delta_duv;

    int (*set)(struct bt_mesh_model *model, ble_mesh_light_ctl_state *ctl_state);
} ble_mesh_light_ctl_srv_t;

/* * User data of Light CTL Setup Server Model */
typedef struct {
    struct bt_mesh_model *model;     /* !< Pointer to the Lighting CTL Setup Server Model. Initialized internally. */
    ble_mesh_light_ctl_state *state; /* !< Parameters of the Light CTL state */
} ble_mesh_light_ctl_setup_srv_t;

/* * User data of Light CTL Temperature Server Model */
typedef struct {
    struct bt_mesh_model *model; /* !< Pointer to the Lighting CTL Temperature Server Model. Initialized internally. */
    ble_mesh_light_ctl_state *state; /* !< Parameters of the Light CTL state */
    struct last_msg last;            /* !< Parameters of the last received set message */
    struct transition transition;    /* !< Parameters of state transition */
    int32_t tt_delta_temp;           /* !< Delta change value of temperature state transition */
    int32_t tt_delta_duv;            /* !< Delta change value of delta uv state transition */

    int (*set)(struct bt_mesh_model *model, ble_mesh_light_ctl_state *ctl_state);
} ble_mesh_light_ctl_temp_srv_t;

extern const struct bt_mesh_model_op bt_mesh_light_ctl_srv_op[];
extern const struct bt_mesh_model_cb bt_mesh_light_ctl_srv_cb;

extern const struct bt_mesh_model_op bt_mesh_light_ctl_setup_srv_op[];
extern const struct bt_mesh_model_cb bt_mesh_light_ctl_setup_srv_cb;

extern const struct bt_mesh_model_op bt_mesh_light_ctl_temp_srv_op[];
extern const struct bt_mesh_model_cb bt_mesh_light_ctl_temp_srv_cb;


#define BT_MESH_MODEL_LIGHT_CTL_SRV(srv, pub) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_LIGHT_CTL_SRV, bt_mesh_light_ctl_srv_op, pub, srv, &bt_mesh_light_ctl_srv_cb)

#define BT_MESH_MODEL_LIGHT_CTL_SETUP_SRV(srv, pub)                                                  \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV, bt_mesh_light_ctl_setup_srv_op, pub, srv, \
        &bt_mesh_light_ctl_setup_srv_cb)

#define BT_MESH_MODEL_LIGHT_CTL_TEMP_SRV(srv, pub)                                                 \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_LIGHT_CTL_TEMP_SRV, bt_mesh_light_ctl_temp_srv_op, pub, srv, \
        &bt_mesh_light_ctl_temp_srv_cb)

#endif

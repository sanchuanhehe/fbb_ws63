 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Light CTL Client Model Statement
 *
 * History:
 * 2024-6-21, Create file.
 */
#ifndef __LIGHT_CTL_CLIENT_H
#define __LIGHT_CTL_CLIENT_H

#include "model_def.h"

struct bt_mesh_light_ctl_set {
    bool op_en;               /* Indicate whether optional parameters included */
    uint16_t ctl_lightness;   /* The target value of the Light CTL Lightness state     */
    uint16_t ctl_temperature; /* The target value of the Light CTL Temperature state   */
    int16_t ctl_delta_uv;     /* The target value of the Light CTL Delta UV state      */
    uint8_t tid;              /* Transaction Identifier                        */
    uint8_t trans_time;       /* Time to complete state transition (O)  */
    uint8_t delay;            /* Message execution delay in 5-millisecond steps (C.1)        */
};

struct bt_mesh_light_ctl_status {
    bool op_en;                       /* Indicate whether optional parameters included        */
    uint16_t present_ctl_lightness;   /* The present value of the Light CTL Lightness state */
    uint16_t present_ctl_temperature; /* The present value of the Light CTL Temperature state  */
    uint16_t target_ctl_lightness;    /* The target value of the Light CTL Lightness state */
    uint16_t target_ctl_temperature;  /* The target value of the Light CTL Temperature state */
    uint8_t remain_time;              /* Time to complete state transition (C.1)              */
};

struct bt_mesh_light_ctl_temperature_set {
    bool op_en;               /* Indicate whether optional parameters included */
    uint16_t ctl_temperature; /* The target value of the Light CTL Temperature state */
    int16_t ctl_delta_uv;     /* The target value of the Light CTL Delta UV state    */
    uint8_t tid;              /* Transaction Identifier                        */
    uint8_t trans_time;       /* Time to complete state transition (O)  */
    uint8_t delay;            /* Message execution delay in 5-millisecond steps (C.1)   */
};

struct bt_mesh_light_ctl_temperature_status {
    bool op_en;                       /* Indicate whether optional parameters included          */
    uint16_t present_ctl_temperature; /* The present value of the Light CTL Temperature state */
    int16_t present_ctl_delta_uv;    /* The present value of the Light CTL Delta UV state  */
    uint16_t target_ctl_temperature;  /* The target value of the Light CTL Temperature state (O) */
    int16_t target_ctl_delta_uv;     /* The target value of the Light CTL Delta UV state (C.1) */
    uint8_t remain_time;              /* Time to complete state transition (C.1)                */
};

struct bt_mesh_light_ctl_temperature_range_set {
    uint16_t range_min; /* The value of the Temperature Range Min field of the Light CTL Temperature Range state */
    uint16_t range_max; /* The value of the Temperature Range Max field of the Light CTL Temperature Range state */
};

struct bt_mesh_light_ctl_temperature_range_status {
    uint8_t status_code; /* Status code for the requesting message                                    */
    uint16_t range_min;  /* The value of the Temperature Range Min field of the Light CTL Temperature Range state */
    uint16_t range_max;  /* The value of the Temperature Range Max field of the Light CTL Temperature Range state */
};

struct bt_mesh_light_ctl_default_set {
    uint16_t lightness;   /* The value of the Light Lightness Default state  */
    uint16_t temperature; /* The value of the Light CTL Temperature Default state */
    int16_t delta_uv;     /* The value of the Light CTL Delta UV Default state  */
};

struct bt_mesh_light_ctl_default_status {
    uint16_t lightness;   /* The value of the Light Lightness Default state   */
    uint16_t temperature; /* The value of the Light CTL Temperature Default state */
    int16_t delta_uv;     /* The value of the Light CTL Delta UV Default state */
};

/* Light Ctl Client Model Callback */
extern const struct bt_mesh_model_cb bt_mesh_light_ctl_cli_cb;

/* Light Ctl Client Model Context */
extern const struct bt_mesh_model_op bt_mesh_light_ctl_cli_op[];

#define BT_MESH_MODEL_LIGHT_CTL_CLI(cli_data, pub) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_LIGHT_CTL_CLI, bt_mesh_light_ctl_cli_op, pub, cli_data, &bt_mesh_light_ctl_cli_cb)

/*
 * @brief Set light ctl states.
 *
 * @param common        Message common information structure.
 * @param set           Pointer of light set message value.
 * @param ack_status    Pointer of light ctl response status value.
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_light_ctl_client_set_state(bt_mesh_client_common_param_t *common, void *set, void *ack_status);

/*
 * @brief Get light ctl states.
 *
 * @param common    Message common information structure.
 * @param get       Pointer of light ctl get message value.
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_light_ctl_client_get_state(bt_mesh_client_common_param_t *common, void *get);

#endif

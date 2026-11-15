 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Time Server Interface Statement
 *
 * History:
 * 2025-5-21, Create file.
 */
#ifndef __TIME_SERVER_H__
#define __TIME_SERVER_H__

#include "model_def.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TIMEZONE_STEP 15
#define MINUTE_PER_SEC 60
#define BASE_YEAR 1900
#define DELAY_YEAR 2000

struct bt_mesh_time_state_t {
    uint8_t  tai_seconds[TAI_SECONDS_LEN];
    uint8_t  subsecond;
    uint8_t  uncertainty;
    uint16_t time_authority : 1;
    uint16_t tai_utc_delta_curr : 15;
    uint8_t  timezone_offset_curr;
    uint8_t  timezone_offset_new;
    uint8_t  tai_zone_change[TAI_OF_ZONE_CHANGE_LEN];
    uint16_t tai_utc_delta_new : 15;
    uint16_t padding1 : 1;
    uint8_t  tai_delta_change[TAI_OF_DELTA_CHANGE_LEN];
    uint8_t  time_role;
};

/* * User data of Time Server Model */
struct bt_mesh_time_srv_t {
    struct bt_mesh_model *model;
    struct bt_mesh_time_state_t *state;
    int (*set)(struct bt_mesh_model *model, struct bt_mesh_time_state_t *time_state);
};

/* * User data of Time Setup Server Model */
struct bt_mesh_time_setup_srv_t {
    struct bt_mesh_model *model;
    struct bt_mesh_time_state_t *state;
    struct transition time_transition;
    bool send_publish;
    int (*set)(struct bt_mesh_model *model, struct bt_mesh_time_state_t *time_state);
};

struct bt_mesh_utc_time_t {
    uint64_t utc_sec;
    uint64_t delta;
};

extern const struct bt_mesh_model_op bt_mesh_time_srv_op[];
extern const struct bt_mesh_model_cb bt_mesh_time_srv_cb;

extern const struct bt_mesh_model_op bt_mesh_time_setup_srv_op[];
extern const struct bt_mesh_model_cb bt_mesh_time_setup_srv_cb;
extern struct bt_mesh_utc_time_t local_utc_time;

#define BT_MESH_MODEL_TIME_SRV(srv, pub) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_TIME_SRV, bt_mesh_time_srv_op, pub, srv, &bt_mesh_time_srv_cb)

#define BT_MESH_MODEL_TIME_SETUP_SRV(srv, pub) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_TIME_SETUP_SRV, bt_mesh_time_setup_srv_op, pub, srv, &bt_mesh_time_setup_srv_cb)

#ifdef __cplusplus
}
#endif

#endif
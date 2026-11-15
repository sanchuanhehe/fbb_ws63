 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Time Client Interface Statement
 *
 * History:
 * 2025-5-21, Create file.
 */
#ifndef __TIME_CLIENT_H__
#define __TIME_CLIENT_H__

#include "model_def.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const struct bt_mesh_model_op bt_mesh_time_cli_op[];
extern const struct bt_mesh_model_cb bt_mesh_time_cli_cb;

#define BT_MESH_MODEL_TIME_CLI(cli_data, pub) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_TIME_CLI, bt_mesh_time_cli_op, pub, cli_data, &bt_mesh_time_cli_cb)

/* *
If the TAI Seconds field is 0x0000000000 the Subsecond, Uncertainty, Time Authority, TAI-UTC Delta and
Time Zone Offset fields shall be omitted; otherwise these fields shall be present.
*/
struct bt_mesh_time_status_t {
    uint8_t  tai_seconds[TAI_SECONDS_LEN];
    uint8_t  subsecond;
    uint8_t  uncertainty;
    uint16_t time_authority : 1;
    uint16_t tai_utc_delta_curr : 15;
    uint8_t  timezone_offset_curr;
};

struct bt_mesh_time_set_t {
    uint8_t  tai_seconds[TAI_SECONDS_LEN];
    uint8_t  subsecond;
    uint8_t  uncertainty;
    uint16_t time_authority : 1;
    uint16_t tai_utc_delta_curr : 15;
    uint8_t  timezone_offset_curr;
};

struct bt_mesh_timezone_status_t {
    uint8_t  timezone_offset_curr;
    uint8_t  timezone_offset_new;
    uint8_t  tai_zone_change[TAI_OF_ZONE_CHANGE_LEN];
};

struct bt_mesh_timezone_set_t {
    uint8_t  timezone_offset_new;
    uint8_t  tai_zone_change[TAI_OF_ZONE_CHANGE_LEN];
};

struct bt_mesh_tai_utc_delta_status_t {
    uint16_t tai_utc_delta_curr : 15;
    uint16_t padding1 : 1;
    uint16_t tai_utc_delta_new : 15;
    uint16_t padding2 : 1;
    uint8_t  tai_delta_change[TAI_OF_DELTA_CHANGE_LEN];
};

struct bt_mesh_tai_utc_delta_set_t {
    uint16_t tai_utc_delta_new : 15;
    uint16_t padding : 1;
    uint8_t  tai_delta_change[TAI_OF_DELTA_CHANGE_LEN];
};

struct bt_mesh_time_role_status_t {
    uint8_t  time_role;
};

struct bt_mesh_time_role_set_t {
    uint8_t  time_role;
};

int bt_mesh_time_set(
    bt_mesh_client_common_param_t *common,
    struct bt_mesh_time_set_t *set,
    struct bt_mesh_time_status_t *status);
int bt_mesh_time_get(bt_mesh_client_common_param_t *common, void *status);

int bt_mesh_timezone_set(
    bt_mesh_client_common_param_t *common,
    struct bt_mesh_timezone_set_t *set,
    struct bt_mesh_timezone_status_t *status);
int bt_mesh_timezone_get(bt_mesh_client_common_param_t *common, void *status);

int bt_mesh_tai_utc_delta_set(
    bt_mesh_client_common_param_t *common,
    struct bt_mesh_tai_utc_delta_set_t *set,
    struct bt_mesh_tai_utc_delta_status_t *status);
int bt_mesh_tai_utc_delta_get(bt_mesh_client_common_param_t *common, void *status);

int bt_mesh_time_role_set(
    bt_mesh_client_common_param_t *common,
    struct bt_mesh_time_role_set_t *set,
    struct bt_mesh_time_role_status_t *status);
int bt_mesh_time_role_get(bt_mesh_client_common_param_t *common, void *status);

#ifdef __cplusplus
}
#endif

#endif /* __TIME_CLIENT_H__ */
 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Scheduler Server Interface Statement
 *
 * History:
 * 2025-5-21, Create file.
 */
#ifndef __SCHEDULER_SERVER_H__
#define __SCHEDULER_SERVER_H__

#include "model_def.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SCHEDULER_COUNT_MAX             16
#define SCHEDULE_MAX_INDEX              0x0F
#define SCHEDULE_ANY_YEAR               0x64
#define SCHEDULE_ANY_DAY                0x00
#define SCHEDULE_ANY_HOUR               0x18
#define SCHEDULE_HOUR_ONCE_A_DAY        0x19
#define SCHEDULE_ANY_MINUTE             0x3C
#define SCHEDULE_MINUTE_15              0x3D
#define SCHEDULE_MINUTE_20              0x3E
#define SCHEDULE_MINUTE_ONCE_A_HOUR     0x3F
#define SCHEDULE_ANY_SEC                0x3C
#define SCHEDULE_SEC_15                 0x3D
#define SCHEDULE_SEC_20                 0x3E
#define SCHEDULE_SEC_ONCE_A_MINUTE      0x3F

#define SCHEDULE_ACT_ON                 0x01
#define SCHEDULE_ACT_OFF                0x00
#define SCHEDULE_ACT_SCENE_RECALL       0x02
#define SCHEDULE_ACT_INACTIVE           0x0F

#define SCHEDULE_STATUS_LEN             2
#define SCHEDULE_ACT_STATUS_LEN         10

#define BT_MESH_MON_LEN                 40
#define BT_MESH_WEEK_LEN                15

struct schedule_register_t {
    bool     exist;
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

struct bt_mesh_scheduler_state_t {
    uint8_t schedule_count;
    struct schedule_register_t *schedules;
};

/* * User data of Scheduler Server Model */
struct bt_mesh_scheduler_srv_t {
    struct bt_mesh_model *model;
    struct bt_mesh_scheduler_state_t *state;
    int (*set)(struct bt_mesh_model *model, struct bt_mesh_scheduler_state_t *scheduler_state);
};

/* * User data of Scheduler Setup Server Model */
struct bt_mesh_scheduler_setup_srv_t {
    struct bt_mesh_model *model;
    struct bt_mesh_scheduler_state_t *state;
    struct transition scheduler_transition;
    int (*set)(struct bt_mesh_model *model, struct bt_mesh_scheduler_state_t *scheduler_state);
};

extern const struct bt_mesh_model_op bt_mesh_scheduler_srv_op[];
extern const struct bt_mesh_model_cb bt_mesh_scheduler_srv_cb;

extern const struct bt_mesh_model_op bt_mesh_scheduler_setup_srv_op[];
extern const struct bt_mesh_model_cb bt_mesh_scheduler_setup_srv_cb;

#define BT_MESH_MODEL_SCHEDULER_SRV(srv, pub) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_SCHEDULER_SRV, bt_mesh_scheduler_srv_op, pub, srv, &bt_mesh_scheduler_srv_cb)

#define BT_MESH_MODEL_SCHEDULER_SETUP_SRV(srv, pub)                                                  \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_SCHEDULER_SETUP_SRV, bt_mesh_scheduler_setup_srv_op, pub, srv, \
        &bt_mesh_scheduler_setup_srv_cb)

void convert_month_bit_mask(uint32_t num, char *mon_buff, uint8_t buff_len);
void convert_week_bit_mask(uint32_t num, char *week_buff, uint8_t buff_len);

#ifdef __cplusplus
}
#endif

#endif

 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Common defintion for Model
 *
 * History:
 * 2024-6-21, Create file.
 */
#ifndef __MODEL_DEF_H__
#define __MODEL_DEF_H__

#include <stdbool.h>
#include <stdint.h>
#include "atomic_adapter.h"
#include "mesh/glue.h"
#include "mesh/access.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STATE_OFF 0x00
#define STATE_ON 0x01
#define STATE_DEFAULT 0x01
#define STATE_RESTORE 0x02

#define BLE_MESH_SERVER_RSP_MAX_LEN 384
#define BLE_MESH_SERVER_TRANS_MIC_SIZE 4
#define MODEL_OPCODE_LEN 2
#define MODEL_VND_OPCODE_LEN 3

/* Refer 7.2 of Mesh Model Specification */
#define RANGE_SUCCESSFULLY_UPDATED 0x00
#define CANNOT_SET_RANGE_MIN 0x01
#define CANNOT_SET_RANGE_MAX 0x02

#define TAI_SECONDS_LEN             5
#define TAI_OF_ZONE_CHANGE_LEN      5
#define TAI_OF_DELTA_CHANGE_LEN     5
#define DELAY_TIME_EXTEND 5

enum {
    BLE_MESH_TRANS_TIMER_START,
    BLE_MESH_TRANS_FLAG_MAX,
};

struct bt_mesh_gen_model_cli {
    struct bt_mesh_model *model;

    struct k_sem op_sync;
    uint32_t op_pending;
    void *op_param;
};

/* * Client model sending message parameters */
typedef struct {
    uint32_t opcode;             /* Message opcode */
    struct bt_mesh_model *model; /* Pointer to the client model */
    struct bt_mesh_msg_ctx ctx;  /* Message context */
    const struct bt_mesh_send_cb *cb; /* User defined callback function */
    void *cb_data;                    /* User defined callback value */
} bt_mesh_client_common_param_t;

struct transition {
    bool just_started;
    uint8_t trans_time;  // trans_time
    uint8_t remain_time; // remain_time
    uint8_t delay;
    uint32_t quo_tt;
    uint32_t counter;
    uint32_t total_duration;
    int64_t start_timestamp;
    ATOMIC_DEFINE(flag, BLE_MESH_TRANS_FLAG_MAX);
    struct k_work_delayable timer;
};

struct last_msg {
    uint8_t tid;
    uint16_t src;
    uint16_t dst;
    int64_t timestamp;
};

#ifdef __cplusplus
}
#endif

#endif /* __MODEL_COMMON_H__ */

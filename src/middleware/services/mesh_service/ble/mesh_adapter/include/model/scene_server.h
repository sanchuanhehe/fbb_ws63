 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Scene Server Model Statement
 *
 * History:
 * 2024-6-21, Create file.
 */
#ifndef _SCENE_SERVER_H_
#define _SCENE_SERVER_H_

#include "model_def.h"

#ifdef __cplusplus
extern "C" {
#endif

#define INVALID_SCENE_NUMBER 0x0000
#define SCENE_NUMBER_LEN 0x02

#define SCENE_SUCCESS 0x00
#define SCENE_REG_FULL 0x01
#define SCENE_NOT_FOUND 0x02

#define SCENE_COUNT_MAX 16
#define SCENE_WHITE_LIGHT 1
#define SCENE_GREEN_LIGHT 2
#define WHITE_LIGHT 5000
#define GREEN_LIGHT 60000
#define LIGHT_CNT               (1)     // means instance count
#define SCENE_CNT_MAX            (16)

struct scene_register {
    uint16_t scene_number;
    uint8_t scene_type; /* Indicate the type of scene value */
    uint16_t scene_value[SCENE_COUNT_MAX]; /* *< A list of scenes stored within an element */
};

struct bt_mesh_scenes_state {
    const uint16_t scene_count;
    struct scene_register *scenes;
    uint16_t current_scene;
    uint16_t target_scene;
    uint8_t status_code;

    /* Indicate if scene transition is in progress */
    bool in_progress;
};

struct bt_mesh_scene_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_scenes_state *state;
    struct last_msg last;
    struct transition transition;
    int (*set)(struct bt_mesh_model *model, uint32_t opcode, const uint8_t *state);
};

struct bt_mesh_scene_setup_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_scenes_state *state;
    int (*set)(struct bt_mesh_model *model, uint32_t opcode, const uint8_t *state);
};


extern const struct bt_mesh_model_op bt_mesh_scene_srv_op[];
extern const struct bt_mesh_model_cb bt_mesh_scene_srv_cb;

#define BT_MESH_MODEL_SCENE_SRV(srv, pub) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_SCENE_SRV, bt_mesh_scene_srv_op, pub, srv, &bt_mesh_scene_srv_cb)


extern const struct bt_mesh_model_op bt_mesh_scene_setup_srv_op[];
extern const struct bt_mesh_model_cb bt_mesh_scene_setup_srv_cb;

#define BT_MESH_MODEL_SCENE_SET_SRV(srv, pub)                                                \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_SCENE_SETUP_SRV, bt_mesh_scene_setup_srv_op, pub, srv, \
        &bt_mesh_scene_setup_srv_cb)

typedef struct {
    uint16_t id;
    uint8_t tid;
    uint8_t transit_t;
    uint8_t delay; // unit 5ms
} bt_mesh_scene_recall_t;

typedef struct {
    uint16_t id;
    uint16_t scene_lightness;
    uint16_t scene_temp;
    int16_t scene_level;
    uint8_t scene_onoff;
    uint8_t rsv[4];
} bt_mesh_scene_data_t;

typedef struct {
    bt_mesh_scene_data_t data[LIGHT_CNT][SCENE_CNT_MAX];
} model_scene_t;

void scene_publish(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t opcode);
void scene_change_light_status(uint16_t status, uint16_t mod_id);

#ifdef __cplusplus
}
#endif

#endif /* SCENE_SERVER_H_ */

 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Scene Client Interface Statement
 *
 * History:
 * 2024-6-21, Create file.
 */
#ifndef _SCENE_CLIENT_H_
#define _SCENE_CLIENT_H_

#include <stdbool.h>

#include "model_def.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BT_MESH_MAX_SCENES_NUM 16

/* Time Scene Client Model Callback */
extern const struct bt_mesh_model_cb bt_mesh_scene_client_cb;

/* Scene Client Model Context */
extern const struct bt_mesh_model_op bt_mesh_scene_cli_op[];

#define BT_MESH_MODEL_SCENE_CLI(cli_data, pub) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_SCENE_CLI, bt_mesh_scene_cli_op, pub, cli_data, &bt_mesh_scene_client_cb)


struct bt_mesh_scene_model_cli {
    struct bt_mesh_model *model;

    struct k_sem op_sync;
    uint32_t op_pending;
    void *op_param;
};

/*
 * Scene Store is an acknowledged message used to
 * store the current state of an element as a Scene, which can be recalled later.
 */
struct bt_mesh_scene_store {
    uint16_t scene_number; /* The number of the scene to be stored */
};

/*
 * Scene Status is an unacknowledged message used to
 * report the current status of a currently active scene (see Section 5.1.3.2) of an element.
 */
struct bt_mesh_scene_status {
    bool op_en;             /* Indicate whether optional parameters included */
    uint8_t status_code;    /* Status code for the last operation            */
    uint16_t current_scene; /* Scene Number of a current scene               */
    uint16_t target_scene;  /* Scene Number of a target scene (O)     */
    uint8_t remain_time;    /* Time to complete state transition (C.1)       */
};

/*
 * Scene Register Status is an unacknowledged message that is used to
 * report the current status of the Scene Register (see Section 5.1.3.1) of an element.
 */
struct bt_mesh_scene_register_status {
    uint8_t status_code;    /* Status code for the previous operation              */
    uint16_t current_scene; /* Scene Number of a current scene                     */
    uint16_t scenes[BT_MESH_MAX_SCENES_NUM];    /* A list of scenes stored within an element */
};

/*
 * Scene Recall is an acknowledged message that is used to
 * recall the current state of an element from a previously stored scene.
 */
struct bt_mesh_scene_recall {
    bool op_en;                 /* Indicate whether optional parameters included                    */
    uint16_t scene_number;      /* The number of the scene to be recalled                           */
    uint8_t tid;                /* Transaction Identifier                                           */
    uint8_t transition_time;    /* Transition Time field identifies the time an element will take to
                                   transition from the present states to the target states(O)       */
    uint8_t delay;              /* Indicate message execution delay (C.1)                           */
};

/*
 * Scene Delete is an acknowledged message used to
 * delete a Scene from the Scene Register state (see Section 5.1.3.1) of an element.
 */
struct bt_mesh_scene_delete {
    uint16_t scene_number; /* The number of the scene to be deleted */
};


typedef union {
    struct {
        uint16_t scene_number;
    } scene_store;
    struct {
        bool op_en;
        uint16_t scene_number;
        uint8_t tid;
        uint8_t trans_time;
        uint8_t delay;
    } scene_recall;
    struct {
        uint16_t scene_number;
    } scene_delete;
} bt_mesh_time_scene_client_set_msg_t;


typedef union {
    struct bt_mesh_scene_status scene_status;
    struct bt_mesh_scene_register_status register_status;
} bt_mesh_scene_client_status_t;


/**
 * @brief This function is called to get scene states.
 *
 * @param[in]  common: Message common information structure
 * @param[in]  get:    Pointer of time scene get message value
 *
 * @return Zero-success, other-fail
 */
int bt_mesh_scene_client_get_state(bt_mesh_client_common_param_t *common, void *status);

/**
 * @brief This function is called to set scene states.
 *
 * @param[in]  common: Message common information structure
 * @param[in]  set:    Pointer of time scene set message value
 *
 * @return Zero-success, other-fail
 */
int bt_mesh_scene_client_set_state(bt_mesh_client_common_param_t *common, void *set, void *status);

#ifdef __cplusplus
}
#endif

#endif /* _TIME_SCENE_CLIENT_H_ */

/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Scene Server Model Function Implementation
 *
 * History:
 * 2024-6-21, Create file.
 */
#include "osal_timer.h"
#include "osal_task.h"
#include "mesh/mesh.h"
#include "model/model_def.h"
#include "model_common.h"
#include "model/model_opcode.h"
#include "mesh/health_srv.h"
#include "state_transition.h"
#include "state_binding.h"
#include "model/generic_server.h"

#if (MYNEWT_VAL(BLE_MESH_SCENE_SERVER))

#define GET_TRANSITION_STEP(val)    ((val) & 0x3f)

model_scene_t    model_sig_scene;
uint8_t transition_forced_by_recall_flag = 0;
bt_mesh_scene_data_t g_scene_save_light_status = {
    .scene_onoff = 0,
    .scene_level = -0x8000,
    .scene_lightness = 0
};
/* message handlers (Start) */

/* Scene Server & Scene Setup Server message handlers */
void send_scene_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, bool publish)
{
    struct bt_mesh_scene_srv *scene_srv = model->user_data;
    struct os_mbuf *msg = NULL;
    uint8_t length = 1 + 6;

    if (publish) {
        msg = model->pub->msg;
        if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
            return;
        }
    } else {
        msg = NET_BUF_SIMPLE(length + BLE_MESH_SERVER_TRANS_MIC_SIZE);
    }

    if (msg == NULL) {
        BT_ERR("msg is NULL.");
        return;
    }

    bt_mesh_model_msg_init(msg, MODEL_OP(SCENE_STATUS));

    if (ctx->recv_op == MODEL_OP(SCENE_GET)) {
        net_buf_simple_add_u8(msg, SCENE_SUCCESS);
    } else {
        net_buf_simple_add_u8(msg, scene_srv->state->status_code);
    }
    net_buf_simple_add_le16(msg, scene_srv->state->current_scene);
    BT_INFO("add current_scene(%d) to msg", scene_srv->state->current_scene);

    if (scene_srv->transition.counter) {
        calculate_rt(&scene_srv->transition);
        net_buf_simple_add_le16(msg, scene_srv->state->target_scene);
        net_buf_simple_add_u8(msg, scene_srv->transition.remain_time);
        BT_INFO("add target_scene(%d) and remain_time(%d)  to msg", scene_srv->state->target_scene,
            scene_srv->transition.remain_time);
    }
    if (publish == false) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
        os_mbuf_free_chain(msg);
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish_duration(model,
            scene_srv->transition.total_duration + K_MSEC(DELAY_TIME_EXTEND *
            scene_srv->transition.trans_time * scene_srv->transition.counter),
            0));
    }
    return;
}

static void send_scene_register_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint8_t status_code,
    bool publish)
{
    struct bt_mesh_scene_setup_srv *scene_setup_srv = model->user_data;
    struct scene_register *scene = NULL;
    struct os_mbuf *msg = NULL;
    uint16_t total_len = 9U;
    int i;

    if (publish) {
        msg = model->pub->msg;
        if (model->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
            return;
        }
    } else {
        msg = NET_BUF_SIMPLE(MIN(BT_MESH_TX_SDU_MAX, BLE_MESH_SERVER_RSP_MAX_LEN));
    }

    if (ctx == NULL && publish == false) {
        BT_ERR("Invalid parameter");
        return;
    }

    if (msg == NULL) {
        BT_ERR("msg is NULL.");
        return;
    }

    bt_mesh_model_msg_init(msg, MODEL_OP(SCENE_REGISTER_STATUS));
    net_buf_simple_add_u8(msg, status_code);
    net_buf_simple_add_le16(msg, scene_setup_srv->state->current_scene);
    BT_INFO("status_code is %d,current_scene is %d", status_code, scene_setup_srv->state->current_scene);
    for (i = 0; i < scene_setup_srv->state->scene_count; i++) {
        scene = &scene_setup_srv->state->scenes[i];
        if (scene->scene_number == INVALID_SCENE_NUMBER) {
            continue;
        }

        total_len += SCENE_NUMBER_LEN;
        if ((publish == false && total_len > MIN(BT_MESH_TX_SDU_MAX, BLE_MESH_SERVER_RSP_MAX_LEN)) ||
            (publish == true && total_len > msg->om_len + BLE_MESH_SERVER_TRANS_MIC_SIZE)) {
            /* Add this in case the message is too long */
            BT_WARN("Too large scene register status");
            break;
        }
        net_buf_simple_add_le16(msg, scene->scene_number);
        BT_INFO("add scene_number(%d) to msg", scene->scene_number);
    }
    ctx->send_rel = true;
    if (publish == false) {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_send(model, ctx, msg, NULL, NULL));
    } else {
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
    }
    os_mbuf_free_chain(msg);
    return;
}

static int scene_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_scene_srv *scene_srv = model->user_data;

    if (scene_srv == NULL || scene_srv->state == NULL) {
        BT_ERR("%s, Invalid model user data", __func__);
        return -EINVAL;
    }
    BT_INFO("recv scene_get opcode:0x%04x", ctx->recv_op);
    switch (ctx->recv_op) {
        case MODEL_OP(SCENE_GET):
            send_scene_status(model, ctx, false);
            return 0;
        case MODEL_OP(SCENE_REGISTER_GET):
            send_scene_register_status(model, ctx, SCENE_SUCCESS, false);
            return 0;
        default:
            BT_WARN("Unknown Scene Get opcode 0x%04x", ctx->recv_op);
            return 0;
    }
}

struct scene_data {
    uint8_t tid;
    int64_t now;
    uint8_t trans_time;
    uint8_t delay;
    uint16_t scene_number;
};

static int is_in_transition(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct scene_register *scene,
    struct scene_data *data)
{
    int i;
    struct bt_mesh_scene_srv *scene_srv = model->user_data;

    for (i = 0; i < scene_srv->state->scene_count; i++) {
        scene = &scene_srv->state->scenes[i];
        if (scene->scene_number == data->scene_number) {
            BT_INFO("set scene_number to %d", scene->scene_number);
            break;
        }
    }
    if (i == scene_srv->state->scene_count) {
        BT_WARN("Scene Number 0x%04x not exists", data->scene_number);
        scene_srv->state->status_code = SCENE_NOT_FOUND;
        if (ctx->recv_op == MODEL_OP(SCENE_RECALL)) {
            send_scene_status(model, ctx, false);
        }
        return -EINVAL;
    }
    scene_srv->state->status_code = SCENE_SUCCESS;

    /* Mesh Model Spec doesn't mention about this operation. */
    if (bt_mesh_is_server_recv_last_msg(&scene_srv->last, data->tid, ctx->addr, ctx->recv_dst, &data->now)) {
        if (ctx->recv_op == MODEL_OP(SCENE_RECALL)) {
            BT_WARN("recv the same msg!");
            send_scene_status(model, ctx, false);
        }
        /* In this condition, no event will be callback to application layer */
        return -EINVAL;
    }
    return 0;
}

static int set_scene_data(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct scene_register *scene,
    struct scene_data *data)
{
    struct bt_mesh_scene_srv *scene_srv = model->user_data;
    if (scene_srv == NULL) {
        BT_ERR("scene srv is NULL");
        return -EINVAL;
    }
    scene_srv->state->in_progress = false;
    /* *
     * When the scene transition is not in progress, the value of the Target
     * Scene state shall be set to 0x0000.
     */
    scene_srv->state->target_scene = INVALID_SCENE_NUMBER;
    scene_srv->state->status_code = SCENE_SUCCESS;

    /* *
     * If the target state is equal to the current state, the transition
     * shall not be started and is considered complete.
     */
    if (scene_srv->state->current_scene != data->scene_number) {
        BT_INFO("current_scene(%d) != scene_number(%d),calculate tt value", scene_srv->state->current_scene,
            data->scene_number);
        scene_srv->transition.trans_time = data->trans_time;
        scene_srv->transition.delay = data->delay;
        scene_tt_values(scene_srv, data->trans_time, data->delay);
    } else {
        BT_WARN("recall the same scene_number:%d", scene_srv->state->current_scene);
        if (ctx->recv_op == MODEL_OP(SCENE_RECALL)) {
            send_scene_status(model, ctx, false);
        }

        return -EINVAL;
    }

    /* For Instantaneous Transition */
    if (scene_srv->transition.counter == 0U) {
        scene_srv->state->current_scene = data->scene_number;
        BT_INFO("counter = 0,so set current_scene = scene_number(%d)", scene_srv->state->current_scene);
    } else {
        /* *
         * When a scene transition is in progress, the value of the Current
         * Scene state shall be set to 0x0000.
         */
        scene_srv->state->in_progress = true;
        scene_srv->state->current_scene = INVALID_SCENE_NUMBER;
        scene_srv->state->target_scene = data->scene_number;
        BT_INFO("scene in transition,so set current_scene = 0,target_scene = scene_number(%d)",
            scene_srv->state->target_scene);
    }
    return 0;
}

static inline int is_transition_need(uint8_t transit_t, uint8_t delay)
{
    return (GET_TRANSITION_STEP(transit_t) || delay);
}

void bt_mesh_onoff_status(struct bt_mesh_elem *elem, bt_mesh_scene_recall_t *p_recall,
    bt_mesh_scene_data_t *p, struct bt_mesh_msg_ctx *ctx)
{
    struct bt_mesh_model *onoff_model = bt_mesh_model_find(elem, BT_MESH_MODEL_ID_GEN_ONOFF_SRV);
    if (onoff_model == NULL) {
        BT_ERR("onoff model is NULL");
        return; // exist
    }
    struct bt_mesh_gen_onoff_srv *onoff_srv = onoff_model->user_data;
    if (onoff_srv != NULL) {
        if (p_recall->transit_t != 0) {
            onoff_srv->transition.trans_time = p_recall->transit_t;
            onoff_srv->transition.delay = p_recall->delay;
        }
        onoff_srv->target_onoff = p->scene_onoff;
        BT_INFO("model: %p, onoff: %d, delay: %d, tt: %d",
            onoff_model, onoff_srv->target_onoff, onoff_srv->transition.delay, onoff_srv->transition.trans_time);
        bt_mesh_gen_onoff_set_scene(onoff_model, ctx);
    }
    uint32_t remain_time = ble_npl_callout_remaining_ticks(&onoff_srv->transition.timer.work, ble_npl_time_get());
    osal_msleep(osal_jiffies_to_msecs(remain_time));     // 此处应该等onoff状态处理完成后再执行level
}

void bt_mesh_level_status(struct bt_mesh_elem *elem, bt_mesh_scene_recall_t *p_recall,
    bt_mesh_scene_data_t *p, struct bt_mesh_msg_ctx *ctx)
{
    struct bt_mesh_model *level_model = bt_mesh_model_find(elem, BT_MESH_MODEL_ID_GEN_LEVEL_SRV);
    if (level_model == NULL) {
        BT_ERR("level model is NULL");
        return; // exist
    }
    struct bt_mesh_gen_level_srv *level_srv = level_model->user_data;
    if (level_srv != NULL) {
        bt_mesh_gen_level_set_scene(level_model, ctx, p, p_recall);
        BT_INFO("level_model: %p, level: %d, delay: %d, tt: %d", level_model, level_srv->level_state.target_level,
            level_srv->transition.delay, level_srv->transition.trans_time);
    }
}

int bt_mesh_sig_scene_recall(struct bt_mesh_model *model, uint16_t scene_id,
    struct bt_mesh_msg_ctx *ctx, bt_mesh_scene_recall_t *par)
{
    int err = 0;
    uint8_t st = SCENE_SUCCESS;
    bt_mesh_scene_recall_t *p_recall = (bt_mesh_scene_recall_t *)par;
    struct bt_mesh_elem *elem = bt_mesh_model_elem(model);

    if (scene_id == INVALID_SCENE_NUMBER) {
        return -1;
    }
    transition_forced_by_recall_flag = 1;
    for (int i = 0; i < SCENE_CNT_MAX; i++) {
        bt_mesh_scene_data_t *p = &model_sig_scene.data[model->elem_idx][i];
        if (p_recall->id == p->id) {
            st = SCENE_SUCCESS;
            bt_mesh_onoff_status(elem, p_recall, p, ctx);
            bt_mesh_level_status(elem, p_recall, p, ctx);
            break;    // exist
        }
    }
    transition_forced_by_recall_flag = 0;
    return err;
}

static void data_to_scene_recall(bt_mesh_scene_recall_t *par, struct scene_data *data)
{
    par->delay = data->trans_time;
    par->id = data->scene_number;
    par->tid = data->tid;
    par->transit_t = data->trans_time;
}

void calculation_delay_tt(struct scene_data *data, struct os_mbuf *buf)
{
    switch (buf->om_len) {
        case 0x00: /* No optional fields are available */
            data->trans_time = g_gen_def_trans_time;
            break;
        case 0x02: /* Optional fields are available */
            data->trans_time = net_buf_simple_pull_u8(buf);
            if ((data->trans_time & 0x3F) == 0x3F) {
                return;
            }
            data->delay = net_buf_simple_pull_u8(buf);
            break;
        default:
            return;
    }
    BT_WARN("tid=%u, scene_number=%u, trans_time=%u, delay=%u", data->tid, data->scene_number, data->trans_time,
        data->delay);
}

static int scene_recall(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_scene_srv *scene_srv = model->user_data;
    struct scene_register *scene = NULL;
    struct scene_data data = { 0 };
    bt_mesh_scene_recall_t par = { 0 };
    uint8_t delay;
    uint8_t trans_time;
    int ret = 0;

    BT_WARN("recv BLE_MESH_MODEL_OP_SCENE_RECALL(0x0842 or 0x0843)");
    if (scene_srv == NULL || scene_srv->state == NULL) {
        BT_ERR("Invalid model user data");
        return -EINVAL;
    }

    data.scene_number = net_buf_simple_pull_le16(buf);
    if (data.scene_number == INVALID_SCENE_NUMBER) {
        BT_ERR("Invalid Scene Number 0x0000");
        return -EINVAL;
    }
    data.tid = net_buf_simple_pull_u8(buf);
    calculation_delay_tt(&data, buf);
    ret = is_in_transition(model, ctx, scene, &data);
    if (ret) {
        BT_ERR("is_in_transition err. ret: %d", ret);
        return ret;
    }

    bt_mesh_server_stop_transition(&scene_srv->transition);
    bt_mesh_server_update_last_msg(&scene_srv->last, data.tid, ctx->addr, ctx->recv_dst, &data.now);
    data_to_scene_recall(&par, &data);
    bt_mesh_sig_scene_recall(model, data.scene_number, ctx, &par);
    ret = set_scene_data(model, ctx, scene, &data);
    if (ret) {
        BT_ERR("current scene is equal to the target number. err ret: %d", ret);
        return ret;
    }
    scene_srv->transition.just_started = true;
    if (ctx->recv_op == MODEL_OP(SCENE_RECALL)) {
        send_scene_status(model, ctx, false);
    }
    send_scene_status(model, ctx, true);

    bt_mesh_server_start_transition(&scene_srv->transition);
    return 0;
}

static void get_new_scene(int index, struct bt_mesh_scene_setup_srv *scene_setup_srv, struct scene_register *scene)
{
    int remaining_index = index;
    scene_setup_srv->state->status_code = SCENE_REG_FULL;
    for (remaining_index = 0; remaining_index < scene_setup_srv->state->scene_count; remaining_index++) {
        scene = &scene_setup_srv->state->scenes[remaining_index];
        if (scene->scene_number != INVALID_SCENE_NUMBER) {
            scene_setup_srv->state->current_scene = scene->scene_number;
            BT_INFO("change the  currently active scene to new scene_number:%d", scene_setup_srv->state->current_scene);
            break;
        }
    }
    if (remaining_index == scene_setup_srv->state->scene_count) {
        BT_ERR("no scene is active ,set current_scene to INVALID_SCENE_NUMBER(0)");
        scene_setup_srv->state->current_scene = INVALID_SCENE_NUMBER;
    }
}

static void scene_action_store(struct bt_mesh_model *model, struct scene_register *scene, uint16_t scene_number)
{
    struct bt_mesh_scene_setup_srv *scene_srv = model->user_data;
    int i = 0;
    for (i = 0; i < scene_srv->state->scene_count; i++) {
        scene = &scene_srv->state->scenes[i];
        if (scene->scene_number == scene_number) {
            scene_srv->state->status_code = SCENE_SUCCESS;
            scene_srv->state->current_scene = scene_number;
            BT_INFO("find the scene_number,set current_scene to (%d)", scene_srv->state->current_scene);
            break;
        }
    }
    /* Try to find a unset entry if no matching Scene Number is found */
    if (i == scene_srv->state->scene_count) {
        BT_WARN("No matching Scene Number 0x%04x found", scene_number);
        for (i = 0; i < scene_srv->state->scene_count; i++) {
            scene = &scene_srv->state->scenes[i];
            if (scene->scene_number == INVALID_SCENE_NUMBER) {
                scene->scene_number = scene_number;
                scene_srv->state->status_code = SCENE_SUCCESS;
                scene_srv->state->current_scene = scene_number;
                BT_INFO("then add new scene to state(scene_number and current_scene to %d)", scene_number);
                break;
            }
        }
        if (i == scene_srv->state->scene_count) {
            BT_WARN("Scene Register is full!");
            scene_srv->state->status_code = SCENE_REG_FULL;
        }
    }

    if (scene_srv->state->in_progress == true) {
        scene_srv->state->target_scene = scene_number;
        BT_INFO("set target_scene to scene_number [%d]", scene_srv->state->target_scene);
    }
}
static void scene_action_delete(struct bt_mesh_model *model, struct scene_register *scene, uint16_t scene_number)
{
    int i = 0;
    struct bt_mesh_scene_setup_srv *scene_setup_srv = model->user_data;
    for (i = 0; i < scene_setup_srv->state->scene_count; i++) {
        scene = &scene_setup_srv->state->scenes[i];
        if (scene->scene_number == scene_number) {
            scene->scene_number = INVALID_SCENE_NUMBER;
            break;
        }
    }
    
    for (int cnt = 0; cnt < SCENE_CNT_MAX; cnt++) {
        bt_mesh_scene_data_t *p = &model_sig_scene.data[model->elem_idx][cnt];
        if (p->id == scene_number) {
            p->id = INVALID_SCENE_NUMBER;
            memset_s(p, sizeof(bt_mesh_scene_data_t), 0, sizeof(bt_mesh_scene_data_t));
            break;
        }
    }
    if (i == scene_setup_srv->state->scene_count) {
        BT_WARN("Scene Number 0x%04x not exists", scene_number);
    }
    scene_setup_srv->state->status_code = SCENE_SUCCESS;
    if (scene_setup_srv->state->current_scene == scene_number) {
        scene_setup_srv->state->current_scene = INVALID_SCENE_NUMBER;
        BT_INFO("set current_scene to INVALID_SCENE_NUMBER(0x%04x)", INVALID_SCENE_NUMBER);
    }

    if (scene_setup_srv->state->target_scene == scene_number && scene_setup_srv->state->in_progress == true) {
        scene_setup_srv->state->target_scene = INVALID_SCENE_NUMBER;
        BT_INFO("in_progress:set target_scene to INVALID_SCENE_NUMBER(0x%04x)", INVALID_SCENE_NUMBER);
        struct bt_mesh_model *scene_model = bt_mesh_model_find(bt_mesh_model_elem(model), BT_MESH_MODEL_ID_SCENE_SRV);
        if (scene_model == NULL) {
            BT_ERR("Scene Server not present in the element");
            return;
        }

        struct bt_mesh_scene_srv *scene_srv = scene_srv = scene_model->user_data;
        if (scene_srv == NULL || scene_srv->state == NULL || scene_setup_srv->state != scene_srv->state) {
            BT_ERR("Invalid Scene Server user data or different scene state");
            return;
        }

        scene_srv->state->in_progress = false;
        BT_INFO("scene has been delete, stop the transiton");
        bt_mesh_server_stop_transition(&scene_srv->transition);
    }
}

void scene_change_light_status(uint16_t status, uint16_t mod_id)
{
    if (mod_id == BT_MESH_MODEL_ID_GEN_ONOFF_SRV) {
        g_scene_save_light_status.scene_onoff = status;
    } else if (mod_id == BT_MESH_MODEL_ID_GEN_LEVEL_SRV) {
        g_scene_save_light_status.scene_level = status;
    }
}

/**
 * @brief       This function set and save scene parameters.
 * @param[in]   scene_id-
 * @param[in]   cb_par    - parameters output by callback function which handle the opcode received.
 * @return      error status code of set scene.
 * @note
 */
uint8_t bt_mesh_sig_scene_set_ll(struct bt_mesh_model *model, uint16_t scene_id, uint8_t elem_idx, uint16_t op)
{
    uint8_t st = SCENE_SUCCESS;
    if (scene_id == INVALID_SCENE_NUMBER) {
        return -1;
    }
    if ((op == MODEL_OP(SCENE_STORE)) || (op == MODEL_OP(SCENE_STORE_UNACK))) {
        bt_mesh_scene_data_t *scene_data = NULL;
        bt_mesh_scene_data_t *p = NULL;
        for (int i = 0; i < SCENE_CNT_MAX; i++) {
            p = &model_sig_scene.data[elem_idx][i];
            if (p->id == 0 && scene_data == NULL) {
                scene_data = p;
                continue;
            }
            if (scene_id == p->id) {
                scene_data = p;
                break;
            }
        }
        if (scene_data) {
            scene_data->id = scene_id;
            scene_data->scene_onoff = g_scene_save_light_status.scene_onoff;
            scene_data->scene_level = g_scene_save_light_status.scene_level;
            scene_data->scene_lightness = g_scene_save_light_status.scene_lightness;
            BT_INFO("p->id: %d, onoff: %d, level: %d, lightness: %d",
                scene_data->id,
                scene_data->scene_onoff,
                scene_data->scene_level,
                scene_data->scene_lightness);
        }
    }
    return st;
}

static int scene_action(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_scene_setup_srv *scene_srv = model->user_data;
    struct scene_register *scene = NULL;
    uint16_t scene_number = 0U;
    uint8_t elem_idx = scene_srv->model->elem_idx;

    if (scene_srv == NULL || scene_srv->state == NULL) {
        BT_ERR("invalid model user data");
        return -EINVAL;
    }

    scene_number = net_buf_simple_pull_le16(buf);
    if (scene_number == INVALID_SCENE_NUMBER) {
        BT_ERR("invalid scence number");
        return -EINVAL;
    }
    BT_INFO("scene_number is %d", scene_number);
    switch (ctx->recv_op) {
        case MODEL_OP(SCENE_STORE):
        case MODEL_OP(SCENE_STORE_UNACK): {
            BT_WARN("recv BLE_MESH_MODEL_OP_SCENE_STORE or UNACK");
            /* Try to find a matching Scene Number */
            scene_action_store(model, scene, scene_number);
            if (scene_srv && scene_srv->set) {
                scene_srv->set(scene_srv->model, ctx->recv_op, (const uint8_t *)scene_srv->state); // 回调app
            }
            // 保存当前灯控的(onoff/level/lightness/temp) 数据信息
            bt_mesh_sig_scene_set_ll(model, scene_number, elem_idx, BLE_MESH_MODEL_OP_SCENE_STORE);
            break;
        }
        case MODEL_OP(SCENE_DELETE):
        case MODEL_OP(SCENE_DELETE_UNACK): {
            BT_WARN("recv BLE_MESH_MODEL_OP_SCENE_DELETE or UNACK");
            scene_action_delete(model, scene, scene_number);
            if (scene_srv && scene_srv->set) {
                scene_srv->set(scene_srv->model, ctx->recv_op, (const uint8_t *)scene_srv->state); // 回调app
            }
            break;
        }
        default:
            BT_ERR("Unknown Scene setup action opcode 0x%04x", ctx->recv_op);
            return -EINVAL;
    }

    if (ctx->recv_op == MODEL_OP(SCENE_STORE) || ctx->recv_op == MODEL_OP(SCENE_DELETE)) {
        send_scene_register_status(model, ctx, scene_srv->state->status_code, false);
    }
    return 0;
}
/* message handlers (End) */

/* Mapping of message handlers for Scene Server (0x1203) */
const struct bt_mesh_model_op bt_mesh_scene_srv_op[] = {
    { MODEL_OP(SCENE_GET),          0, scene_get    },
    { MODEL_OP(SCENE_RECALL),       3, scene_recall },
    { MODEL_OP(SCENE_RECALL_UNACK), 3, scene_recall },
    { MODEL_OP(SCENE_REGISTER_GET), 0, scene_get    },
    BT_MESH_MODEL_OP_END,
};

/* Mapping of message handlers for Scene Setup Server (0x1204) */
const struct bt_mesh_model_op bt_mesh_scene_setup_srv_op[] = {
    { MODEL_OP(SCENE_STORE),        2, scene_action },
    { MODEL_OP(SCENE_STORE_UNACK),  2, scene_action },
    { MODEL_OP(SCENE_DELETE),       2, scene_action },
    { MODEL_OP(SCENE_DELETE_UNACK), 2, scene_action },
    BT_MESH_MODEL_OP_END,
};

static int check_scene_server_init(struct bt_mesh_scenes_state *scenes_state)
{
    int i;

    if (scenes_state->scene_count == 0U || scenes_state->scenes == NULL) {
        BT_ERR("Invalid Scene state");
        return -EINVAL;
    }

    for (i = 0; i < scenes_state->scene_count; i++) {
        if (scenes_state->scenes[i].scene_value == NULL) {
            BT_ERR("Invalid Scene value, index %d", i);
            return -EINVAL;
        }
    }

    return 0;
}

static int scene_srv_pub_update(struct bt_mesh_model *mod)
{
    struct bt_mesh_scene_srv *scene_srv = mod->user_data;
    struct os_mbuf *msg = mod->pub->msg;
    if (!scene_srv || !msg) {
        return -EINVAL;
    }
    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_SCENE_STATUS);
    net_buf_simple_add_u8(msg, scene_srv->state->status_code);
    net_buf_simple_add_le16(msg, scene_srv->state->current_scene);
    if (scene_srv->transition.counter) {
        calculate_rt(&scene_srv->transition);
        net_buf_simple_add_le16(msg, scene_srv->state->target_scene);
        net_buf_simple_add_u8(msg, scene_srv->transition.remain_time);
        BT_INFO("add target_scene(%d) and remain_time(%d)  to msg", scene_srv->state->target_scene,
            scene_srv->transition.remain_time);
    }
    return 0;
}

static int scene_setup_srv_pub_update(struct bt_mesh_model *mod)
{
    struct bt_mesh_scene_setup_srv *scene_setup_srv = mod->user_data;
    struct os_mbuf *msg = mod->pub->msg;
    struct scene_register *scene = NULL;
    uint16_t total_len = 9U;
    if (!scene_setup_srv || !msg) {
        return -EINVAL;
    }
    bt_mesh_model_msg_init(msg, BLE_MESH_MODEL_OP_SCENE_REGISTER_GET);
    net_buf_simple_add_u8(msg, scene_setup_srv->state->status_code);
    net_buf_simple_add_le16(msg, scene_setup_srv->state->current_scene);
    for (int i = 0; i < scene_setup_srv->state->scene_count; i++) {
        scene = &scene_setup_srv->state->scenes[i];
        if (scene->scene_number == INVALID_SCENE_NUMBER) {
            continue;
        }
        total_len += SCENE_NUMBER_LEN;
        if (total_len > MIN(BT_MESH_TX_SDU_MAX, BLE_MESH_SERVER_RSP_MAX_LEN)) {
            /* Add this in case the message is too long */
            BT_WARN("Too large scene register status");
            break;
        }
        net_buf_simple_add_le16(msg, scene->scene_number);
        BT_INFO("add scene_number(%d) to msg", scene->scene_number);
    }
    return 0;
}

static int bt_mesh_scene_server_deinit(struct bt_mesh_model *model)
{
    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    if (model->user_data == NULL) {
        BT_ERR("Invalid Time Scene Server user data, model id 0x%04x", model->id);
        return -EINVAL;
    }

    switch (model->id) {
        case BT_MESH_MODEL_ID_SCENE_SRV: {
            struct bt_mesh_scene_srv *scene_srv = model->user_data;
            k_work_cancel_delayable_delete(&(scene_srv->transition.timer));

            break;
        }
        case BT_MESH_MODEL_ID_SCENE_SETUP_SRV: {
            break;
        }
        default:
            BT_WARN("Unknown Time Scene Server, model id 0x%04x", model->id);
            return -EINVAL;
    }

    return 0;
}

static int bt_mesh_scene_server_init(struct bt_mesh_model *model)
{
    if (model->user_data == NULL) {
        BT_ERR("Invalid Time Scene Server user data, model id 0x%04x", model->id);
        return -EINVAL;
    }

    switch (model->id) {
        case BT_MESH_MODEL_ID_SCENE_SRV: {
            struct bt_mesh_scene_srv *scene_srv = model->user_data;
            if (scene_srv->state == NULL) {
                BT_ERR("Invalid Scene State");
                return -EINVAL;
            }
            if (check_scene_server_init(scene_srv->state)) {
                return -EINVAL;
            }
            model->pub->update = scene_srv_pub_update;
            scene_srv->model = model;
            k_work_init_delayable(&(scene_srv->transition.timer), scene_recall_work_handler);
            k_work_add_arg_delayable(&(scene_srv->transition.timer), scene_srv);
            break;
        }
        case BT_MESH_MODEL_ID_SCENE_SETUP_SRV: {
            struct bt_mesh_scene_setup_srv *scene_setup_srv = model->user_data;
            if (scene_setup_srv->state == NULL) {
                BT_ERR("Invalid Scene State");
                return -EINVAL;
            }
            if (check_scene_server_init(scene_setup_srv->state)) {
                return -EINVAL;
            }
            scene_setup_srv->model = model;
            model->pub->update = scene_setup_srv_pub_update;
            break;
        }
        default:
            BT_WARN("Unknown Time Scene Server, model id 0x%04x", model->id);
            return -EINVAL;
    }

    return 0;
}

static int bt_mesh_scene_srv_deinit(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("The Scene Server does not support publication");
        return -EINVAL;
    }

    if (!bt_mesh_model_in_primary(model)) {
        BT_WARN("The Scene Server is not on the primary element");
    }

    return bt_mesh_scene_server_deinit(model);
}

static int bt_mesh_scene_srv_init(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("The Scene Server does not support publication");
        return -EINVAL;
    }

    if (!bt_mesh_model_in_primary(model)) {
        BT_WARN("The Scene Server is not on the primary element");
    }

    struct bt_mesh_elem *element = bt_mesh_model_elem(model);
    if (bt_mesh_model_find(element, BT_MESH_MODEL_ID_SCENE_SETUP_SRV) == NULL) {
        BT_WARN("The Scene Setup Server does not exist");
    }
    return bt_mesh_scene_server_init(model);
}

static int bt_mesh_scene_setup_srv_deinit(struct bt_mesh_model *model)
{
    if (!bt_mesh_model_in_primary(model)) {
        BT_WARN("Scene Setup Server not on the Primary element");
    }
    return bt_mesh_scene_server_deinit(model);
}

static int bt_mesh_scene_setup_srv_init(struct bt_mesh_model *model)
{
    if (!bt_mesh_model_in_primary(model)) {
        BT_WARN("Scene Setup Server not on the Primary element");
    }
    return bt_mesh_scene_server_init(model);
}

const struct bt_mesh_model_cb bt_mesh_scene_srv_cb = {
    .init = bt_mesh_scene_srv_init,
    .deinit = bt_mesh_scene_srv_deinit,
};

const struct bt_mesh_model_cb bt_mesh_scene_setup_srv_cb = {
    .init = bt_mesh_scene_setup_srv_init,
    .deinit = bt_mesh_scene_setup_srv_deinit,
};

#endif /* CONFIG_BLE_MESH_TIME_SCENE_SERVER */

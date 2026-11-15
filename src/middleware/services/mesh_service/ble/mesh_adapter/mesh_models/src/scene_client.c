/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Scene Client Model Function Implementation
 *
 * History:
 * 2024-6-21, Create file.
 */
#include "mesh/glue.h"
#include "mesh/mesh.h"
#include "model/model_opcode.h"
#include "model_common.h"
#include "model/scene_client.h"

#if (MYNEWT_VAL(BLE_MESH_SCENE_CLI))

#define INVALID_SCENE_NUMBER 0x0000

/* Scene client messages length */
#define BLE_MESH_SCENE_STORE_MSG_LEN (2 + 2 + 4)        /* op + data + mic */
#define BLE_MESH_SCENE_RECALL_MSG_LEN (2 + 5 + 4)       /* op + data + mic */
#define BLE_MESH_SCENE_GET_MSG_LEN (2 + 0 + 4)          /* op + data + mic */
#define BLE_MESH_SCENE_REGISTER_GET_MSG_LEN (2 + 0 + 4) /* op + data + mic */
#define BLE_MESH_SCENE_DELETE_MSG_LEN (2 + 2 + 4)       /* op + data + mic */
#define SCENE_MSG_TIMEOUT K_SECONDS(3)
#define SCENE_STATUS_LEN_WITHOUT_OPT 3
#define SCENE_STATUS_LEN_WITH_OPT 6

static struct bt_mesh_scene_model_cli *scene_cli;

#define SCENE_CLI_WAIT(err, cli, param, op)                      \
    do {                                                         \
        err = scene_cli_wait(cli, param, op);                    \
        if (err) {                                               \
            BT_ERR("scene_cli_wait msg timeout,err is %d", err); \
        }                                                        \
    } while (0)

static int scene_cli_wait(struct bt_mesh_scene_model_cli *cli, void *param, uint32_t op)
{
    int err;

    BT_DBG("");

    cli->op_param = param;
    cli->op_pending = op;
    BT_INFO("op_pending is 0x%04x,then take the sem", cli->op_pending);

    err = k_sem_take(&cli->op_sync, SCENE_MSG_TIMEOUT);

    cli->op_pending = 0;
    cli->op_param = NULL;
    BT_INFO("after sem take,op_pending is 0x%04x", cli->op_pending);

    return err;
}

static void time_scene_status_handler(
    struct bt_mesh_model *model, struct os_mbuf *buf, struct bt_mesh_scene_model_cli *cli)
{
    BT_INFO("recv scene status:BLE_MESH_MODEL_OP_SCENE_STATUS(0x%04x)", MODEL_OP(SCENE_STATUS));
    struct bt_mesh_scene_status *status;
    if (buf->om_len != SCENE_STATUS_LEN_WITHOUT_OPT && buf->om_len != SCENE_STATUS_LEN_WITH_OPT) {
        BT_ERR("Invalid Scene Status length %d", buf->om_len);
        k_sem_give(&cli->op_sync);
        return;
    }
    status = cli->op_param;
    if (!status) {
        return;
    }
    if (cli->op_pending != BLE_MESH_MODEL_OP_SCENE_STATUS) {
        BT_WARN("Unexpected Scene Status message");
        return;
    }

    status->status_code = net_buf_simple_pull_u8(buf);
    status->current_scene = net_buf_simple_pull_le16(buf);
    BT_INFO("status_code is %d,current_scene is %d", status->status_code, status->current_scene);
    if (buf->om_len) {
        status->op_en = true;
        status->target_scene = net_buf_simple_pull_le16(buf);
        status->remain_time = net_buf_simple_pull_u8(buf);
        BT_INFO("target_scene is %d,remain_time is %d", status->target_scene, status->remain_time);
    }

    k_sem_give(&cli->op_sync);
}

static void time_scene_register_status_handler(
    struct bt_mesh_model *model, struct os_mbuf *buf, struct bt_mesh_scene_model_cli *cli)
{
    BT_INFO("recv scene status:OP_SCENE_REGISTER_STATUS(0x%04x)", MODEL_OP(SCENE_REGISTER_STATUS));
    struct bt_mesh_scene_register_status *status;
    status = cli->op_param;
    if (!status) {
        return;
    }
    if (cli->op_pending != BLE_MESH_MODEL_OP_SCENE_REGISTER_STATUS) {
        BT_WARN("Unexpected Scene Register Status message");
        return;
    }

    status->status_code = net_buf_simple_pull_u8(buf);
    status->current_scene = net_buf_simple_pull_le16(buf);
    BT_INFO("status_code is %d,current_scene is %d", status->status_code, status->current_scene);
    int count = 0;
    while (buf->om_len >= sizeof(uint16_t) && count < ARRAY_SIZE(status->scenes)) {
        uint16_t scene_number = net_buf_simple_pull_le16(buf);
        status->scenes[count] = scene_number;
        count++;
    }
    k_sem_give(&cli->op_sync);
}

static int time_scene_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    if (model == NULL) {
        BT_ERR("model is NULL");
        return -EINVAL;
    }
    if (buf == NULL) {
        BT_ERR("buf is NULL");
        return -EINVAL;
    }
    struct bt_mesh_scene_model_cli *cli = model->user_data;
    switch (ctx->recv_op) {
        case MODEL_OP(SCENE_STATUS): {
            time_scene_status_handler(model, buf, cli);
            break;
        }
        case MODEL_OP(SCENE_REGISTER_STATUS): {
            time_scene_register_status_handler(model, buf, cli);
            break;
        }
        default:
            if (ctx == NULL) {
                BT_ERR("ctx is NULL");
                return -EINVAL;
            }
            BT_ERR("InvalidScene Status opcode 0x%04x", ctx->recv_op);
            return -EINVAL;
    }

    return 0;
}

const struct bt_mesh_model_op bt_mesh_scene_cli_op[] = {
    {MODEL_OP(SCENE_STATUS),          3, time_scene_status},
    {MODEL_OP(SCENE_REGISTER_STATUS), 3, time_scene_status},
    BT_MESH_MODEL_OP_END,
};

static int set_scene_msg(uint32_t opcode, void *value, struct os_mbuf *msg, uint32_t *status_opcode)
{
    int err = 0;
    switch (opcode) {
        case MODEL_OP(SCENE_STORE):
            *status_opcode = MODEL_OP(SCENE_REGISTER_STATUS);
        case MODEL_OP(SCENE_STORE_UNACK): {
            struct bt_mesh_scene_store *set;
            set = (struct bt_mesh_scene_store *)value;
            net_buf_simple_add_le16(msg, set->scene_number);
            BT_INFO("SCENE_STORE:scene_number is %d", set->scene_number);
            break;
        }
        case MODEL_OP(SCENE_RECALL):
            *status_opcode = MODEL_OP(SCENE_STATUS);
        case MODEL_OP(SCENE_RECALL_UNACK): {
            struct bt_mesh_scene_recall *set;
            set = (struct bt_mesh_scene_recall *)value;
            net_buf_simple_add_le16(msg, set->scene_number);
            net_buf_simple_add_u8(msg, set->tid);
            BT_INFO("SCENE_RECALL:scene_number is %d,tid is %d", set->scene_number, set->tid);
            if (set->op_en) {
                net_buf_simple_add_u8(msg, set->transition_time);
                net_buf_simple_add_u8(msg, set->delay);
                BT_INFO("SCENE_RECALL:transition_time is %d,delay is %d", set->transition_time, set->delay);
            }
            break;
        }
        case MODEL_OP(SCENE_DELETE):
            *status_opcode = MODEL_OP(SCENE_REGISTER_STATUS);
        case MODEL_OP(SCENE_DELETE_UNACK): {
            struct bt_mesh_scene_delete *set;
            set = (struct bt_mesh_scene_delete *)value;
            net_buf_simple_add_le16(msg, set->scene_number);
            BT_INFO("SCENE_DELETE:scene_number is %d", set->scene_number);
            break;
        }
        default:
            BT_ERR("Invalid Scene Set opcode 0x%04x", opcode);
            err = -EINVAL;
            break;
    }
    return err;
}

static int time_scene_set_state(
    bt_mesh_client_common_param_t *common, void *value, uint16_t value_len, bool need_ack, void *status)
{
    struct os_mbuf *msg = NULL;
    int err = 0;
    uint32_t status_opcode = 0;

    msg = NET_BUF_SIMPLE(value_len);
    if (!msg) {
        BT_ERR("Out of memory");
        return -ENOMEM;
    }

    bt_mesh_model_msg_init(msg, common->opcode);

    err = set_scene_msg(common->opcode, value, msg, &status_opcode);
    if (err) {
        goto done;
    }

    err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err) {
        BT_ERR("model_send() failed (err %d)", err);
        goto done;
    }

    if (!need_ack || !status) {
        goto done;
    }

    SCENE_CLI_WAIT(err, common->model->user_data, status, status_opcode);
done:
    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_scene_client_get_state(bt_mesh_client_common_param_t *common, void *status)
{
    if (!common || !common->model || !common->model->user_data) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    struct os_mbuf *msg = NULL;
    int err = 0;
    uint32_t status_opcode = 0;
    switch (common->opcode) {
        case MODEL_OP(SCENE_GET):
            status_opcode = MODEL_OP(SCENE_STATUS);
            break;
        case MODEL_OP(SCENE_REGISTER_GET):
            status_opcode = MODEL_OP(SCENE_REGISTER_STATUS);
            break;
        default:
            BT_ERR("Invalid Time Scene Get opcode 0x%04x", common->opcode);
            return -EINVAL;
    }

    msg = NET_BUF_SIMPLE(BLE_MESH_SCENE_GET_MSG_LEN);
    if (!msg) {
        BT_ERR("Out of memory");
        return -ENOMEM;
    }

    bt_mesh_model_msg_init(msg, common->opcode);

    err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err) {
        BT_ERR("model_send() failed (err %d)", err);
        goto done;
    }

    SCENE_CLI_WAIT(err, common->model->user_data, status, status_opcode);
done:
    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_scene_client_set_state(bt_mesh_client_common_param_t *common, void *set, void *status)
{
    uint16_t length = 0U;
    bool need_ack = false;

    if (!common || !common->model || !common->model->user_data || !set) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }
    switch (common->opcode) {
        case MODEL_OP(SCENE_STORE):
            need_ack = true;
        case MODEL_OP(SCENE_STORE_UNACK): {
            struct bt_mesh_scene_store *value = (struct bt_mesh_scene_store *)set;
            if (value->scene_number == INVALID_SCENE_NUMBER) {
                BT_ERR("scene store scene number 0x0000 is prohibited");
                return -EINVAL;
            }
            length = BLE_MESH_SCENE_STORE_MSG_LEN;
            break;
        }
        case MODEL_OP(SCENE_RECALL):
            need_ack = true;
        case MODEL_OP(SCENE_RECALL_UNACK): {
            struct bt_mesh_scene_recall *value = (struct bt_mesh_scene_recall *)set;
            if (value->scene_number == INVALID_SCENE_NUMBER) {
                BT_ERR("scene recall scene number 0x0000 is prohibited");
                return -EINVAL;
            }
            if (value->op_en) {
                if ((value->transition_time & 0x3F) > 0x3E) {
                    BT_ERR("invalid scene recall transition time");
                    return -EINVAL;
                }
            }
            length = BLE_MESH_SCENE_RECALL_MSG_LEN;
            break;
        }
        case MODEL_OP(SCENE_DELETE):
            need_ack = true;
        case MODEL_OP(SCENE_DELETE_UNACK): {
            length = BLE_MESH_SCENE_DELETE_MSG_LEN;
            break;
        }
        default:
            BT_ERR("Invalid Time Scene Set opcode 0x%04x", common->opcode);
            return -EINVAL;
    }

    return time_scene_set_state(common, set, length, need_ack, status);
}
static int scene_cli_pub_update(struct bt_mesh_model *mod)
{
    BT_INFO("mod(0x%x),  mod->pub->update(0x%x)", mod, mod->pub->update);
    return 0;
}

static int scene_client_deinit(struct bt_mesh_model *model)
{
    struct bt_mesh_scene_model_cli *scene_cli_tmp;

    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    if (!model->user_data) {
        BT_ERR("No Sensor Client user_data provided");
        return -EINVAL;
    }

    scene_cli_tmp = model->user_data;

    k_sem_deinit(&scene_cli_tmp->op_sync);

    return 0;
}

static int scene_client_init(struct bt_mesh_model *model)
{
    BT_DBG("");

    if (!model->user_data) {
        BT_ERR("No Sensor Client user_data provided");
        return -EINVAL;
    }

    scene_cli = model->user_data;
    scene_cli->model = model;
    BT_INFO("model(0x%x),  model->pub->update(0x%x)", model, model->pub->update);

    if (model->pub->update == NULL) {
        model->pub->update = scene_cli_pub_update;
    }
    k_sem_init(&scene_cli->op_sync, 0, 1);
    return 0;
}

const struct bt_mesh_model_cb bt_mesh_scene_client_cb = {
    .init = scene_client_init,
    .deinit = scene_client_deinit,
};

struct bt_mesh_model *bt_mesh_get_scene_cli_model(void)
{
    if (scene_cli) {
        return (void *)scene_cli->model;
    }

    return NULL;
}

#endif /* CONFIG_BLE_MESH_TIME_SCENE_CLIENT */

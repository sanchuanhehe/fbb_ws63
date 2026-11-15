/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Implement for Cilent Model
 *
 * History:
 * 2025-5-22, Create file.
 */

#include "syscfg/syscfg.h"
#include "model_common.h"
#include "ble_mesh_vnd_attr.h"
 #include "access.h"
#include "vnd_client.h"

static struct bt_mesh_gen_model_cli *g_vnd_cli = NULL;
static uint8_t g_send_tid = 0;

static bt_mesh_vnd_mod_tid_t g_mesh_handle = {0};

void *bt_mesh_vnd_get_cli_model(void)
{
    if (g_vnd_cli) {
        return (void *)g_vnd_cli->model;
    }

    return NULL;
}

int bt_mesh_vnd_cli_get(bt_mesh_client_common_param_t *common, bt_mesh_vnd_op_t *set, bt_mesh_vnd_status_t *status)
{
    if (!common || !common->model || !common->model->user_data || !status) {
        BT_ERR("vnd cli Invalid parameter");
        return -EINVAL;
    }
    struct os_mbuf *msg = NET_BUF_SIMPLE(MODEL_VND_OPCODE_LEN + BT_MSEH_VND_MOD_GET_LEN);

    bt_mesh_model_msg_init(msg, BT_MESH_OP_VND_ATTR_GET);
    g_send_tid = g_send_tid == BT_MESH_VND_TID_RESET_VALUE ? 0 : (g_send_tid + 1);
    net_buf_simple_add_u8(msg, g_send_tid);
    net_buf_simple_add_le16(msg, set->attr_type);

    int err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != 0) {
        BT_ERR("vnd cli model_send() failed (err %d)", err);
        goto done;
    }

    MODEL_CLI_WAIT(err, common->model->user_data, (void *)status, BT_MESH_OP_VND_ATTR_GET_STATUS);
done:
    os_mbuf_free_chain(msg);
    return err;
}

static void bt_mesh_add_unix_time_to_msg(struct os_mbuf *msg, const uint8_t time[6])
{
    net_buf_simple_add_u8(msg, time[0]); /* 0 index */
    net_buf_simple_add_u8(msg, time[1]); /* 1 index */
    net_buf_simple_add_u8(msg, time[2]); /* 2 index */
    net_buf_simple_add_u8(msg, time[3]); /* 3 index */
    net_buf_simple_add_u8(msg, time[4]); /* 4 index */
    net_buf_simple_add_u8(msg, time[5]); /* 5 index */
}

int bt_mesh_vnd_cli_set(bt_mesh_client_common_param_t *common, bt_mesh_vnd_op_t *set, bt_mesh_vnd_status_t *status)
{
    struct os_mbuf *msg = NULL;

    if (!common || !common->model || !common->model->user_data || !set) {
        BT_ERR("vnd cli Invalid parameter");
        return -EINVAL;
    }

    bt_mesh_vnd_data_union_t *value = (bt_mesh_vnd_data_union_t *)set->data;
    g_send_tid = g_send_tid == BT_MESH_VND_TID_RESET_VALUE ? 0 : (g_send_tid + 1);
    switch (set->attr_type) {
        case MESH_VENDOR_ONOFF_TYPE:
            msg = NET_BUF_SIMPLE(MODEL_VND_OPCODE_LEN + BT_MSEH_VND_MOD_ONOFF_SET_LEN);
            bt_mesh_model_msg_init(msg, common->opcode);
            net_buf_simple_add_u8(msg, g_send_tid);
            net_buf_simple_add_le16(msg, set->attr_type);
            net_buf_simple_add_u8(msg, value->onoff);
            break;
        case MESH_VENDOR_UNIX_TIME_TYPE:
            msg = NET_BUF_SIMPLE(MODEL_VND_OPCODE_LEN + BT_MSEH_VND_MOD_UNIX_TIME_SET_LEN);
            bt_mesh_model_msg_init(msg, common->opcode);
            net_buf_simple_add_u8(msg, g_send_tid);
            net_buf_simple_add_le16(msg, set->attr_type);
            bt_mesh_add_unix_time_to_msg(msg, value->unix_time);
            break;
        case MESH_VENDOR_POWER_PERCENT_TYPE:
            msg = NET_BUF_SIMPLE(MODEL_VND_OPCODE_LEN + BT_MSEH_VND_MOD_POWER_PERCENT_SET_LEN);
            bt_mesh_model_msg_init(msg, common->opcode);
            net_buf_simple_add_u8(msg, g_send_tid);
            net_buf_simple_add_le16(msg, set->attr_type);
            net_buf_simple_add_u8(msg, value->power_percent);
            break;
        default:
            BT_ERR("vnd cli Invalid attr type");
            break;
    }

    int err = bt_mesh_model_send(common->model, &common->ctx, msg, common->cb, common->cb_data);
    if (err != 0) {
        BT_ERR("vnd cli model_send() failed (err %d)", err);
        goto done;
    }
    if (!status) {
        goto done;
    }

    MODEL_CLI_WAIT(err, common->model->user_data, (void *)status, BT_MESH_OP_VND_ATTR_SET_STATUS);
done:
    os_mbuf_free_chain(msg);
    return err;
}

int bt_mesh_vnd_attr_set_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *msg)
{
    uint8_t tid = 0;
    int ret = 0;
    uint16_t len = 0;
    struct bt_mesh_gen_model_cli *cli = model->user_data;
    bt_mesh_vnd_status_t *param = (bt_mesh_vnd_status_t *)cli->op_param;

    if (!param) {
        BT_ERR("Invaild cli wait param");
        return -1;
    }

    BT_DBG("vnd cli setstatus msg->om_len %d, recv_data %s", msg->om_len, bt_hex(msg->om_data, msg->om_len));

    tid = net_buf_simple_pull_u8(msg);
    if (tid == g_mesh_handle.recv_tid_num) {
        BT_DBG("vnd cli The indication report success. TID:%d", g_mesh_handle.recv_tid_num);
        g_mesh_handle.recv_tid_num = (g_mesh_handle.recv_tid_num != BT_MESH_VND_TID_RESET_VALUE)
                                         ? (g_mesh_handle.recv_tid_num + 1)
                                         : BT_MESH_VND_TID_RESET_VALUE;
    }

    net_buf_simple_pull_le16(msg);
    len = msg->om_len;
    if (len > 0) {
        param->op_en = 1;
        ret = memcpy_s(param->data, sizeof(param->data), net_buf_simple_pull_mem(msg, len), len);
        if (ret != 0) {
            BT_ERR("param data memcpy failed:%d", ret);
            k_sem_give(&cli->op_sync);
            return ret;
        }
    }

    k_sem_give(&cli->op_sync);

    return 0;
}

int bt_mesh_vnd_attr_notify(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *msg)
{
    uint8_t tid = 0;
    uint16_t attr_type = 0;
    int ret = 0;
    uint8_t *buff_in = NULL;
    uint8_t *buff_out = NULL;
    struct os_mbuf *rsp = NULL;

    BT_DBG("vnd cli data_len %d, recv_data %s", msg->om_len, bt_hex(msg->om_data, msg->om_len));

    // 1. 判断TID是否合法
    tid = net_buf_simple_pull_u8(msg);
    ret = bt_mesh_vnd_recv_tid_handle(&g_mesh_handle, tid);
    if (ret != MESH_VND_TID_OK) {
        return ret;
    }

    rsp = NET_BUF_SIMPLE(MODEL_VND_OPCODE_LEN);  // opcode长度
    bt_mesh_model_msg_init(rsp, BT_MESH_OP_VND_ATTR_NOTIFY_STATUS);
    net_buf_simple_add_u8(rsp, g_mesh_handle.recv_tid_num);

    attr_type = net_buf_simple_pull_le16(msg);

    bt_mesh_vnd_mod_property_t *ptr = bt_mesh_vnd_cli_property_array_get(attr_type);
    if (ptr && ptr->set_cb && (msg->om_len == ptr->data_len)) {
        buff_in = net_buf_simple_pull(msg, ptr->data_len);
        if (ptr->set_cb(buff_in, ptr->data_len)) {
            BT_ERR("vnd cli set property attr_type %d failed", attr_type);
            ret = -1;
            goto done;
        }
    }

    // response
    net_buf_simple_add_le16(rsp, attr_type);
    buff_out = net_buf_simple_add(rsp, ptr->data_len);
    if (ptr->get_cb) {
        ret = ptr->get_cb(buff_out, ptr->data_len);
        if (ret < 0) {
            BT_ERR("vnd cli set property att %d failed", attr_type);
            goto done;
        }
    }

    BT_DBG("vnd cli BtMeshVndAttrNotif att type:0x%04x succ", attr_type);

    bt_mesh_msg_send(ctx, rsp, bt_mesh_model_elem(model)->addr, NULL, NULL);

done:
    os_mbuf_free_chain(rsp);
    return ret;
}

int bt_mesh_vnd_attr_notify_unack(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *msg)
{
    uint8_t tid = 0;
    uint16_t attr_type = 0;
    int ret = 0;
    uint8_t *buff_in = NULL;

    BT_DBG("vnd cli data_len %d, recv_data %s", msg->om_len, bt_hex(msg->om_data, msg->om_len));

    // 1. 判断TID是否合法
    tid = net_buf_simple_pull_u8(msg);
    ret = bt_mesh_vnd_recv_tid_handle(&g_mesh_handle, tid);
    if (ret != MESH_VND_TID_OK) {
        return ret;
    }

    attr_type = net_buf_simple_pull_le16(msg);

    bt_mesh_vnd_mod_property_t *ptr = bt_mesh_vnd_cli_property_array_get(attr_type);
    if (ptr && ptr->set_cb && msg->om_len == ptr->data_len) {
        buff_in = net_buf_simple_pull(msg, ptr->data_len);
        if (ptr->set_cb(buff_in, ptr->data_len) != 0) {
            BT_ERR("vnd cli set property attr_type %d failed", attr_type);
            return -1;
        }
    }

    BT_DBG("vnd cli bt_mesh_vnd_attr_notify_unack att type:0x%04x succ", attr_type);

    return 0;
}

int bt_mesh_vnd_attr_get_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *msg)
{
    uint8_t tid = 0;
    int ret = 0;
    uint16_t len = 0;
    struct bt_mesh_gen_model_cli *cli = model->user_data;
    bt_mesh_vnd_status_t *param = (bt_mesh_vnd_status_t *)cli->op_param;

    if (!param) {
        BT_ERR("Invaild cli wait param");
        return -1;
    }

    BT_DBG("vnd cli getstatus data_len %d, recv_data %s", msg->om_len, bt_hex(msg->om_data, msg->om_len));

    tid = net_buf_simple_pull_u8(msg);
    if (tid == g_mesh_handle.recv_tid_num) {
        BT_DBG("vnd cli The indication report success. TID:%d", g_mesh_handle.recv_tid_num);
        g_mesh_handle.recv_tid_num = (g_mesh_handle.recv_tid_num != BT_MESH_VND_TID_RESET_VALUE)
                                         ? (g_mesh_handle.recv_tid_num + 1)
                                         : BT_MESH_VND_TID_RESET_VALUE;
    }

    net_buf_simple_pull_le16(msg);
    len = msg->om_len;
    if (len > 0) {
        param->op_en = 1;
        ret = memcpy_s(param->data, sizeof(param->data), net_buf_simple_pull_mem(msg, len), len);
        if (ret != 0) {
            BT_ERR("param data memcpy failed:%d", ret);
            k_sem_give(&cli->op_sync);
            return ret;
        }
    }

    k_sem_give(&cli->op_sync);

    return 0;
}

const struct bt_mesh_model_op g_bt_mesh_vnd_attr_cli_op[] = {
    {BT_MESH_OP_VND_ATTR_SET_STATUS, 0, bt_mesh_vnd_attr_set_status},
    {BT_MESH_OP_VND_ATTR_NOTIFY, 0, bt_mesh_vnd_attr_notify},
    {BT_MESH_OP_VND_ATTR_NOTIFY_UNACK, 0, bt_mesh_vnd_attr_notify_unack},
    {BT_MESH_OP_VND_ATTR_GET_STATUS, 0, bt_mesh_vnd_attr_get_status},
    BT_MESH_MODEL_OP_END,
};
static int vnd_cli_pub_update(struct bt_mesh_model *mod)
{
    BT_INFO("mod(0x%x),  mod->pub->update(0x%x)", mod, mod->pub->update);
    return 0;
}

static int bt_mesh_vnd_cli_deinit(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_model_cli *cli = (struct bt_mesh_gen_model_cli *)model->user_data;

    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }

    if (!cli) {
        BT_ERR("vnd cli No Generic OnOff Client context provided");
        return -EINVAL;
    }

    k_sem_deinit(&cli->op_sync);

    return 0;
}

static int bt_mesh_vnd_cli_init(struct bt_mesh_model *model)
{
    struct bt_mesh_gen_model_cli *cli = (struct bt_mesh_gen_model_cli *)model->user_data;

    if (!cli) {
        BT_ERR("vnd cli No Generic OnOff Client context provided");
        return -EINVAL;
    }
    cli->model = model;
    BT_INFO("model(0x%x),  model->pub->update(0x%x)", model, model->pub->update);

    if (model->pub->update == NULL) {
        model->pub->update = vnd_cli_pub_update;
    }
    k_sem_init(&cli->op_sync, 0, 1);

    if (g_vnd_cli == NULL) {
        g_vnd_cli = cli;
    }

    return 0;
}

const struct bt_mesh_model_cb bt_mesh_vnd_attr_cli_cb = {
    .init = bt_mesh_vnd_cli_init,
    .deinit = bt_mesh_vnd_cli_deinit,
};

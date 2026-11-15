/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Implement for Server Model
 *
 * History:
 * 2025-5-22, Create file.
 */

#include "syscfg/syscfg.h"
#include "model_common.h"
#include "model_def.h"
#include "ble_mesh_vnd_attr.h"
 #include "access.h"
#include "vnd_server.h"

static struct bt_mesh_model *g_srv = NULL;
static struct bt_mesh_model *g_model;
static struct bt_mesh_msg_ctx *g_ctx;

static bt_mesh_vnd_mod_tid_t g_mesh_handle = {0};

int bt_mesh_vnd_attr_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *msg)
{
    uint8_t tid = 0;
    uint16_t attr_type = 0;
    int ret = 0;
    struct os_mbuf *rsp = NULL;
    uint8_t *buff_out = NULL;

    g_model = model;
    g_ctx = ctx;

    BT_DBG("vnd srv data_len %d, recv_data %s", msg->om_len, bt_hex(msg->om_data, msg->om_len));

    // 1. 判断TID是否合法
    tid = net_buf_simple_pull_u8(msg);
    ret = bt_mesh_vnd_recv_tid_handle(&g_mesh_handle, tid);
    if (ret != MESH_VND_TID_OK) {
        return ret;
    }
    rsp = NET_BUF_SIMPLE(MODEL_VND_OPCODE_LEN);  // opcode长度
    bt_mesh_model_msg_init(rsp, BT_MESH_OP_VND_ATTR_GET_STATUS);
    net_buf_simple_add_u8(rsp, g_mesh_handle.recv_tid_num);

    attr_type = net_buf_simple_pull_le16(msg);

    net_buf_simple_add_le16(rsp, attr_type);

    bt_mesh_vnd_mod_property_t *ptr = bt_mesh_vnd_srv_property_array_get(attr_type);
    if (ptr && ptr->get_cb) {
        buff_out = net_buf_simple_add(rsp, ptr->data_len);
        ret = ptr->get_cb(buff_out, ptr->data_len);
        if (ret < 0) {
            BT_ERR("vnd srv get att type:%d  data failed", attr_type);
            goto done;
        }
    }
    BT_DBG("vnd srv send data_len %d, recv_data %s", rsp->om_len, bt_hex(rsp->om_data, rsp->om_len));

    bt_mesh_msg_send(ctx, rsp, bt_mesh_model_elem(model)->addr, NULL, NULL);

done:
    os_mbuf_free_chain(rsp);
    return ret;
}

int bt_mesh_vnd_attr_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *msg)
{
    uint8_t tid = 0;
    uint16_t attr_type = 0;
    int ret = 0;
    uint8_t *buff_in = NULL;
    uint8_t *buff_out = NULL;
    struct os_mbuf *rsp = NULL;

    BT_DBG("vnd srv data_len %d, recv_data %s", msg->om_len, bt_hex(msg->om_data, msg->om_len));

    g_model = model;
    g_ctx = ctx;

    // 1. 判断TID是否合法
    tid = net_buf_simple_pull_u8(msg);
    ret = bt_mesh_vnd_recv_tid_handle(&g_mesh_handle, tid);
    if (ret != MESH_VND_TID_OK) {
        return ret;
    }

    rsp = NET_BUF_SIMPLE(MODEL_VND_OPCODE_LEN);  // opcode长度
    bt_mesh_model_msg_init(rsp, BT_MESH_OP_VND_ATTR_SET_STATUS);
    net_buf_simple_add_u8(rsp, g_mesh_handle.recv_tid_num);

    attr_type = net_buf_simple_pull_le16(msg);

    bt_mesh_vnd_mod_property_t *ptr = bt_mesh_vnd_srv_property_array_get(attr_type);
    if (ptr && ptr->set_cb && (msg->om_len == ptr->data_len)) {
        buff_in = net_buf_simple_pull_mem(msg, ptr->data_len);
        if (ptr->set_cb(buff_in, ptr->data_len)) {
            BT_ERR("vnd srv set property attr_type %d failed", attr_type);
            ret = -1;
            goto done;
        }
    }

    net_buf_simple_add_le16(rsp, attr_type);
    buff_out = net_buf_simple_add(rsp, ptr->data_len);
    if (ptr->get_cb) {
        ret = ptr->get_cb(buff_out, ptr->data_len);
        if (ret < 0) {
            BT_ERR("vnd srv get att type:%d  data failed", attr_type);
            goto done;
        }
    }

    // 消息推送
    if (model->pub->addr != BT_MESH_ADDR_UNASSIGNED) {
        struct os_mbuf *pub_msg = model->pub->msg;
        net_buf_simple_add_le16(pub_msg, attr_type);
        net_buf_simple_add_mem(pub_msg, buff_out, ptr->data_len);
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
    }

    BT_DBG("vnd srv send data_len %d, recv_data %s", rsp->om_len, bt_hex(rsp->om_data, rsp->om_len));

    bt_mesh_msg_send(ctx, rsp, bt_mesh_model_elem(model)->addr, NULL, NULL);

done:
    os_mbuf_free_chain(rsp);
    return ret;
}

int bt_mesh_vnd_attr_set_unack(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *msg)
{
    uint8_t tid = 0;
    uint16_t attr_type = 0;
    int ret = 0;
    uint8_t *buff_in = NULL;

    BT_DBG("vnd srv data_len %d, recv_data %s", msg->om_len, bt_hex(msg->om_data, msg->om_len));

    g_model = model;
    g_ctx = ctx;

    // 1. 判断TID是否合法
    tid = net_buf_simple_pull_u8(msg);
    ret = bt_mesh_vnd_recv_tid_handle(&g_mesh_handle, tid);
    if (ret != MESH_VND_TID_OK) {
        return ret;
    }

    attr_type = net_buf_simple_pull_le16(msg);

    bt_mesh_vnd_mod_property_t *ptr = bt_mesh_vnd_srv_property_array_get(attr_type);
    if (ptr && ptr->set_cb && (msg->om_len == ptr->data_len)) {
        buff_in = net_buf_simple_pull_mem(msg, ptr->data_len);
        if (ptr->set_cb(buff_in, ptr->data_len) != 0) {
            BT_ERR("vnd srv set property attr_type %d failed", attr_type);
            return -1;
        }
    }

    // 消息推送
    if (model->pub->addr != BT_MESH_ADDR_UNASSIGNED) {
        struct os_mbuf *pub_msg = model->pub->msg;
        net_buf_simple_add_le16(pub_msg, attr_type);
        net_buf_simple_add_mem(pub_msg, buff_in, ptr->data_len);
        BLE_MESH_CHECK_SEND_STATUS(bt_mesh_model_publish(model));
    }

    return 0;
}

int bt_mesh_vnd_notify_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *msg)
{
    uint8_t tid = 0;

    BT_DBG("vnd srv gen vendor notify status len:%d\n", msg->om_len);

    g_model = model;
    g_ctx = ctx;

    tid = net_buf_simple_pull_u8(msg);
    if (tid == g_mesh_handle.reoport_tid_num) {
        BT_DBG("vnd srv The indication report success. TID:%d", g_mesh_handle.reoport_tid_num);
        g_mesh_handle.reoport_tid_num = (g_mesh_handle.reoport_tid_num != BT_MESH_VND_TID_RESET_VALUE)
                                            ? (g_mesh_handle.reoport_tid_num + 1)
                                            : BT_MESH_VND_TID_RESET_VALUE;
    }

    return 0;
}

// 主动数据上报
void mesh_vendor_data_report(void)
{
    int ret = 0;
    uint8_t id = 0;
    uint16_t attr_type = 0;
    uint8_t *buff_out = NULL;
    struct os_mbuf *rsp = NULL;
    bt_mesh_vnd_mod_property_t *ptr = NULL;

    for (id = 0; id < bt_mesh_vnd_srv_property_array_size_get(); id++) {
        ptr = bt_mesh_vnd_srv_property_array_get_by_id(id);
        if (!ptr || !ptr->get_cb) {
            BT_ERR("vnd srv property is NULL");
            return;
        }
        rsp = NET_BUF_SIMPLE(MODEL_VND_OPCODE_LEN);  // opcode长度
        bt_mesh_model_msg_init(rsp, BT_MESH_OP_VND_ATTR_NOTIFY);
        net_buf_simple_add_u8(rsp, g_mesh_handle.reoport_tid_num);
        g_mesh_handle.reoport_tid_num = g_mesh_handle.reoport_tid_num == BT_MESH_VND_TID_RESET_VALUE
                                            ? BT_MESH_VND_TID_RESET_VALUE
                                            : (g_mesh_handle.reoport_tid_num + 1);
        net_buf_simple_add_le16(rsp, attr_type);

        BT_DBG("vnd srv Att type is:0x%04x", attr_type);

        // response
        buff_out = net_buf_simple_add(rsp, ptr->data_len);
        ret = ptr->get_cb(buff_out, ptr->data_len);
        if (ret < 0) {
            BT_ERR("vnd srv get property id %d failed", id);
            os_mbuf_free_chain(rsp);
            return;
        }

        bt_mesh_msg_send(g_ctx, rsp, bt_mesh_model_elem(g_model)->addr, NULL, NULL);
        os_mbuf_free_chain(rsp);
    }

    return;
}

const struct bt_mesh_model_op g_bt_mesh_vnd_attr_srv_op[] = {
    {BT_MESH_OP_VND_ATTR_GET, 0, bt_mesh_vnd_attr_get},
    {BT_MESH_OP_VND_ATTR_SET, 0, bt_mesh_vnd_attr_set},
    {BT_MESH_OP_VND_ATTR_SET_UNACK, 0, bt_mesh_vnd_attr_set_unack},
    {BT_MESH_OP_VND_ATTR_NOTIFY_STATUS, 0, bt_mesh_vnd_notify_status},
    BT_MESH_MODEL_OP_END,
};

static int vnd_srv_pub_update(struct bt_mesh_model *mod)
{
    return 0;
}

static int bt_mesh_vnd_srv_deinit(struct bt_mesh_model *model)
{
    unused(model);
    g_srv = NULL;

    if ((model->pub != NULL) && (model->pub->msg != NULL)) {
        os_mbuf_free(model->pub->msg);
        model->pub->msg = NULL;
    }
    return 0;
}

static int bt_mesh_vnd_srv_init(struct bt_mesh_model *model)
{
    if (model->pub == NULL) {
        BT_ERR("vnd srv has no publication support");
        return -EINVAL;
    }
    if (!bt_mesh_model_in_primary(model)) {
        BT_ERR("vnd srv server only allowed in primary element");
        return -EINVAL;
    }
    if (g_srv != NULL) {
        BT_ERR("vnd srv server already register");
        return -EINVAL;
    }

    g_srv = model;

    model->pub->update = vnd_srv_pub_update;
    return 0;
}

const struct bt_mesh_model_cb bt_mesh_vnd_attr_srv_cb = {
    .init = bt_mesh_vnd_srv_init,
    .deinit = bt_mesh_vnd_srv_deinit
};

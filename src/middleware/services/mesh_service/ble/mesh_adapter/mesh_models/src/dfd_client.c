/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: DFD Model Implementation.
 *
 * History:
 * 2025-05-01, Create file.
 */

#include <string.h>
#include "syscfg/syscfg.h"
#include "mesh/mesh.h"
#include "mesh/access.h"
#include "model/model_opcode.h"
#include "model_common.h"
#include "model/blob_client.h"
#include "model/dfd_server.h"
#include "mesh_ota_common.h"
#include "model/dfd_client.h"

#define DFD_MSG_TIMEOUT K_SECONDS(5)

#define BLE_MESH_DFD_CLI(blob_cli) CONTAINER_OF(blob_cli, struct bt_mesh_dfd_client_t, blob)
#define BLE_MESH_DFD_MSG_CTX(cli)             \
    {                                         \
        .app_idx = (cli)->req.app_idx,        \
        .addr = (cli)->req.addr,              \
        .send_ttl = (cli)->req.ttl,           \
    }

#define DFD_CLI_WAIT(err, cli, param, op)                     \
    do {                                                      \
        err = dfd_cli_wait(cli, param, op);                   \
        if (err) {                                            \
            BT_ERR("dfd_cli_wait msg timeout, err: %d", err); \
        }                                                     \
    } while (0)

static int dfd_cli_wait(struct bt_mesh_dfd_client_t *cli, void *param, uint32_t op)
{
    int err;

    cli->req.params = param;
    cli->req.op = op;
    BT_DBG("op pending is 0x%04x, then take the sem", cli->req.op);

    err = k_sem_take(&cli->req.sem, DFD_MSG_TIMEOUT);

    cli->req.op = 0;
    cli->req.params = NULL;
    BT_DBG("after sem take(err=%d),op_pending is 0x%04x", err, cli->req.op);

    return err;
}

/* req senders */
#define RECEIVER_ENTRY_SIZE (2 + 1)
#define RECEIVERS_ADD_SDU_OVERHEAD (BT_MESH_MODEL_OP_LEN(MODEL_OP(DFD_RECEIVERS_ADD)) + BT_MESH_MIC_SHORT)
#define MAX_RECEIVERS_OF_SDU DIV_ROUND_UP(BT_MESH_TX_SDU_MAX - RECEIVERS_ADD_SDU_OVERHEAD, RECEIVER_ENTRY_SIZE)
int bt_mesh_dfd_client_receivers_add(struct bt_mesh_dfd_client_t *cli,
    const struct bt_mesh_dist_receiver_entry_t *receivers, const uint32_t n_receivers,
    struct bt_mesh_dist_receivers_status_t *result)
{
    int err = 0;
    const uint32_t rsp_opcode = MODEL_OP(DFD_RECEIVERS_STATUS);
    struct bt_mesh_msg_ctx ctx = BLE_MESH_DFD_MSG_CTX(cli);
    uint32_t n_now = 0;

    if (n_receivers > cli->caps.max_receivers_list_size) {
        BT_ERR("exceeded max_receivers_list_size of caps: %d, %d", n_receivers, cli->caps.max_receivers_list_size);
        return -EINVAL;
    }

    while (n_now < n_receivers) {
        const uint32_t m = MIN(n_receivers - n_now, MAX_RECEIVERS_OF_SDU);
        struct os_mbuf *msg = BT_MESH_MODEL_BUF(MODEL_OP(DFD_RECEIVERS_ADD), m * RECEIVER_ENTRY_SIZE);
        if (msg == NULL) {
            BT_ERR("msg mem failed, now break..");
            err = -ENOMEM;
            break;
        }
        bt_mesh_model_msg_init(msg, MODEL_OP(DFD_RECEIVERS_ADD));
        
        for (uint32_t i = 0; i < m; i++) {
            net_buf_simple_add_le16(msg, receivers[n_now + i].addr);
            net_buf_simple_add_u8(msg, receivers[n_now + i].img_idx);
        }
        n_now += m;
        err = bt_mesh_model_send(cli->mod, &ctx, msg, 0, cli);
        os_mbuf_free_chain(msg);
        if (err) {
            BT_WARN("failed: %d %d", n_now, m);
            break;
        }
        DFD_CLI_WAIT(err, cli->mod->user_data, result, rsp_opcode);
    }

    return err;
}
int bt_mesh_dfd_client_receivers_delete_all(struct bt_mesh_dfd_client_t *cli,
    struct bt_mesh_dist_receivers_status_t *result)
{
    int err = 0;
    const uint32_t rsp_opcode = MODEL_OP(DFD_RECEIVERS_STATUS);
    struct bt_mesh_msg_ctx ctx = BLE_MESH_DFD_MSG_CTX(cli);

    struct os_mbuf *msg = BT_MESH_MODEL_BUF(MODEL_OP(DFD_RECEIVERS_DELETE_ALL), 0);
    if (msg == NULL) {
        BT_ERR("msg mem failed, now break..");
        err = -ENOMEM;
        return err;
    }
    bt_mesh_model_msg_init(msg, MODEL_OP(DFD_RECEIVERS_DELETE_ALL));

    err = bt_mesh_model_send(cli->mod, &ctx, msg, 0, cli);
    os_mbuf_free_chain(msg);
    if (err) {
        return err;
    }
    DFD_CLI_WAIT(err, cli->mod->user_data, result, rsp_opcode);

    return err;
}

int bt_mesh_dfd_client_receivers_get(struct bt_mesh_dfd_client_t *cli,
    const uint16_t from_idx, const uint32_t n_receivers, struct bt_mesh_dist_receivers_list_t *result)
{
    int err = 0;
    const uint32_t rsp_opcode = MODEL_OP(DFD_RECEIVERS_LIST);
    struct bt_mesh_msg_ctx ctx = BLE_MESH_DFD_MSG_CTX(cli);

    struct os_mbuf *msg = BT_MESH_MODEL_BUF(MODEL_OP(DFD_RECEIVERS_GET), 2 + 2); // first_idx:2, n_count:2
    if (msg == NULL) {
        BT_ERR("msg mem failed, now break..");
        err = -ENOMEM;
        return err;
    }
    bt_mesh_model_msg_init(msg, MODEL_OP(DFD_RECEIVERS_GET));
    net_buf_simple_add_le16(msg, from_idx);
    net_buf_simple_add_le16(msg, n_receivers);

    err = bt_mesh_model_send(cli->mod, &ctx, msg, 0, cli);
    os_mbuf_free_chain(msg);
    if (err) {
        return err;
    }
    DFD_CLI_WAIT(err, cli->mod->user_data, result, rsp_opcode);

    return err;
}

int bt_mesh_dfd_client_caps_get(struct bt_mesh_dfd_client_t *cli, struct bt_mesh_dist_caps_status_t *result)
{
    int err = 0;
    const uint32_t rsp_opcode = MODEL_OP(DFD_CAPABILITIES_STATUS);
    struct bt_mesh_msg_ctx ctx = BLE_MESH_DFD_MSG_CTX(cli);

    struct os_mbuf *msg = BT_MESH_MODEL_BUF(MODEL_OP(DFD_CAPABILITIES_GET), 0);
    if (msg == NULL) {
        BT_ERR("msg mem failed, now break..");
        err = -ENOMEM;
        return err;
    }
    bt_mesh_model_msg_init(msg, MODEL_OP(DFD_CAPABILITIES_GET));

    err = bt_mesh_model_send(cli->mod, &ctx, msg, 0, cli);
    os_mbuf_free_chain(msg);
    if (err) {
        return err;
    }
    DFD_CLI_WAIT(err, cli->mod->user_data, result, rsp_opcode);

    return err;
}

int bt_mesh_dfd_client_distribute_get(struct bt_mesh_dfd_client_t *cli, struct bt_mesh_dist_status_t *result)
{
    int err = 0;
    const uint32_t rsp_opcode = MODEL_OP(DFD_STATUS);
    struct bt_mesh_msg_ctx ctx = BLE_MESH_DFD_MSG_CTX(cli);

    struct os_mbuf *msg = BT_MESH_MODEL_BUF(MODEL_OP(DFD_GET), 0);
    if (msg == NULL) {
        BT_ERR("msg mem failed, now break..");
        err = -ENOMEM;
        return err;
    }
    bt_mesh_model_msg_init(msg, MODEL_OP(DFD_GET));

    err = bt_mesh_model_send(cli->mod, &ctx, msg, 0, cli);
    os_mbuf_free_chain(msg);
    if (err) {
        return err;
    }
    DFD_CLI_WAIT(err, cli->mod->user_data, result, rsp_opcode);

    return err;
}

#define DIST_START_PAYLOAD_LEN (2 + 1 + 2 + 1 + 2 + 2) // without opcode
int bt_mesh_dfd_client_distribute_start(struct bt_mesh_dfd_client_t *cli,
    const struct bt_mesh_dist_start_params_t *start, struct bt_mesh_dist_status_t *result)
{
    int err = 0;
    const uint32_t rsp_opcode = MODEL_OP(DFD_STATUS);
    struct bt_mesh_msg_ctx ctx = BLE_MESH_DFD_MSG_CTX(cli);

    struct os_mbuf *msg = BT_MESH_MODEL_BUF(MODEL_OP(DFD_START), DIST_START_PAYLOAD_LEN);
    if (msg == NULL) {
        BT_ERR("msg mem failed, now break..");
        err = -ENOMEM;
        return err;
    }
    bt_mesh_model_msg_init(msg, MODEL_OP(DFD_START));
    net_buf_simple_add_le16(msg, start->app_idx);
    net_buf_simple_add_u8(msg, start->ttl);
    net_buf_simple_add_le16(msg, start->timeout_base);
    net_buf_simple_add_u8(msg, (start->apply<<2U) | start->transfer_mode);
    net_buf_simple_add_le16(msg, start->img_idx);
    net_buf_simple_add_le16(msg, start->group);

    err = bt_mesh_model_send(cli->mod, &ctx, msg, 0, cli);
    os_mbuf_free_chain(msg);
    if (err) {
        return err;
    }
    DFD_CLI_WAIT(err, cli->mod->user_data, result, rsp_opcode);

    return err;
}

int bt_mesh_dfd_client_distribute_apply(struct bt_mesh_dfd_client_t *cli, struct bt_mesh_dist_status_t *result)
{
    int err = 0;
    const uint32_t rsp_opcode = MODEL_OP(DFD_STATUS);
    struct bt_mesh_msg_ctx ctx = BLE_MESH_DFD_MSG_CTX(cli);

    struct os_mbuf *msg = BT_MESH_MODEL_BUF(MODEL_OP(DFD_APPLY), 0);
    if (msg == NULL) {
        BT_ERR("msg mem failed, now break..");
        err = -ENOMEM;
        return err;
    }
    bt_mesh_model_msg_init(msg, MODEL_OP(DFD_APPLY));

    err = bt_mesh_model_send(cli->mod, &ctx, msg, 0, cli);
    os_mbuf_free_chain(msg);
    if (err) {
        return err;
    }
    DFD_CLI_WAIT(err, cli->mod->user_data, result, rsp_opcode);

    return err;
}

int bt_mesh_dfd_client_distribute_suspend(struct bt_mesh_dfd_client_t *cli, struct bt_mesh_dist_status_t *result)
{
    int err = 0;
    const uint32_t rsp_opcode = MODEL_OP(DFD_STATUS);
    struct bt_mesh_msg_ctx ctx = BLE_MESH_DFD_MSG_CTX(cli);

    struct os_mbuf *msg = BT_MESH_MODEL_BUF(MODEL_OP(DFD_SUSPEND), 0);
    if (msg == NULL) {
        BT_ERR("msg mem failed, now break..");
        err = -ENOMEM;
        return err;
    }
    bt_mesh_model_msg_init(msg, MODEL_OP(DFD_SUSPEND));

    err = bt_mesh_model_send(cli->mod, &ctx, msg, 0, cli);
    os_mbuf_free_chain(msg);
    if (err) {
        return err;
    }
    DFD_CLI_WAIT(err, cli->mod->user_data, result, rsp_opcode);

    return err;
}

int bt_mesh_dfd_client_distribute_cancel(struct bt_mesh_dfd_client_t *cli, struct bt_mesh_dist_status_t *result)
{
    int err = 0;
    const uint32_t rsp_opcode = MODEL_OP(DFD_STATUS);
    struct bt_mesh_msg_ctx ctx = BLE_MESH_DFD_MSG_CTX(cli);

    struct os_mbuf *msg = BT_MESH_MODEL_BUF(MODEL_OP(DFD_CANCEL), 0);
    if (msg == NULL) {
        BT_ERR("msg mem failed, now break..");
        err = -ENOMEM;
        return err;
    }
    bt_mesh_model_msg_init(msg, MODEL_OP(DFD_CANCEL));

    err = bt_mesh_model_send(cli->mod, &ctx, msg, 0, cli);
    os_mbuf_free_chain(msg);
    if (err) {
        return err;
    }
    DFD_CLI_WAIT(err, cli->mod->user_data, result, rsp_opcode);

    return err;
}

int bt_mesh_dfd_client_upload_get(struct bt_mesh_dfd_client_t *cli, struct bt_mesh_dist_upload_status_t *result)
{
    int err = 0;
    const uint32_t rsp_opcode = MODEL_OP(DFD_UPLOAD_STATUS);
    struct bt_mesh_msg_ctx ctx = BLE_MESH_DFD_MSG_CTX(cli);

    struct os_mbuf *msg = BT_MESH_MODEL_BUF(MODEL_OP(DFD_UPLOAD_GET), 0);
    if (msg == NULL) {
        BT_ERR("msg mem failed, now break..");
        err = -ENOMEM;
        return err;
    }
    bt_mesh_model_msg_init(msg, MODEL_OP(DFD_UPLOAD_GET));

    err = bt_mesh_model_send(cli->mod, &ctx, msg, 0, cli);
    os_mbuf_free_chain(msg);
    if (err) {
        return err;
    }
    DFD_CLI_WAIT(err, cli->mod->user_data, result, rsp_opcode);

    return err;
}

static int start_upload_transfer(struct bt_mesh_dfd_client_t *cli, uint16_t srv_addr);
#define DIST_UPLOAD_START_PAYLOAD_BASE_LEN (1 + 2 + 8 + 4 + 1) // Table 5.30: w/o opcode+metadata+fwid
#define DIST_UPLOAD_BLOB_ID_B32 32
int bt_mesh_dfd_client_upload_start(struct bt_mesh_dfd_client_t *cli,
    const struct bt_mesh_dist_upload_start_params_t *start, struct bt_mesh_dist_upload_status_t *result)
{
    int err = 0;
    const uint32_t rsp_opcode = MODEL_OP(DFD_UPLOAD_STATUS);
    struct bt_mesh_msg_ctx ctx = BLE_MESH_DFD_MSG_CTX(cli);

    uint32_t payload_len = DIST_UPLOAD_START_PAYLOAD_BASE_LEN + \
        start->metadata_len + start->fwid_len;
    struct os_mbuf *msg = BT_MESH_MODEL_BUF(MODEL_OP(DFD_UPLOAD_START), payload_len);
    if (msg == NULL) {
        BT_ERR("msg mem failed, now break..");
        err = -ENOMEM;
        return err;
    }
    bt_mesh_model_msg_init(msg, MODEL_OP(DFD_UPLOAD_START));

    net_buf_simple_add_u8(msg, start->ttl);
    net_buf_simple_add_le16(msg, start->timeout_base);
    net_buf_simple_add_le32(msg, (uint32_t)start->blob_id);
    net_buf_simple_add_le32(msg, (uint32_t)(start->blob_id >> DIST_UPLOAD_BLOB_ID_B32));
    net_buf_simple_add_le32(msg, start->fw_size);
    net_buf_simple_add_u8(msg, start->metadata_len);
    memcpy_s(net_buf_simple_add(msg, start->metadata_len), start->metadata_len,
        start->metadata, start->metadata_len);
    memcpy_s(net_buf_simple_add(msg, start->fwid_len), start->fwid_len,
        start->fwid, start->fwid_len);

    err = bt_mesh_model_send(cli->mod, &ctx, msg, 0, cli);
    os_mbuf_free_chain(msg);
    if (err) {
        return err;
    }
    DFD_CLI_WAIT(err, cli->mod->user_data, result, rsp_opcode);

    if (err == 0) {
        cli->upload.blob.size = start->fw_size;
        err = start_upload_transfer(cli, cli->req.addr);
    }

    return err;
}

int bt_mesh_dfd_client_upload_cancel(struct bt_mesh_dfd_client_t *cli, struct bt_mesh_dist_upload_status_t *result)
{
    int err = 0;
    const uint32_t rsp_opcode = MODEL_OP(DFD_UPLOAD_STATUS);
    struct bt_mesh_msg_ctx ctx = BLE_MESH_DFD_MSG_CTX(cli);

    struct os_mbuf *msg = BT_MESH_MODEL_BUF(MODEL_OP(DFD_UPLOAD_CANCEL), 0);
    if (msg == NULL) {
        BT_ERR("msg mem failed, now break..");
        err = -ENOMEM;
        return err;
    }
    bt_mesh_model_msg_init(msg, MODEL_OP(DFD_UPLOAD_CANCEL));

    err = bt_mesh_model_send(cli->mod, &ctx, msg, 0, cli);
    os_mbuf_free_chain(msg);
    if (err) {
        return err;
    }
    DFD_CLI_WAIT(err, cli->mod->user_data, result, rsp_opcode);

    return err;
}

int bt_mesh_dfd_client_fw_get(struct bt_mesh_dfd_client_t *cli,
    const uint8_t *fwid, const uint32_t fwid_len, struct bt_mesh_dist_fw_status_t *result)
{
    int err = 0;
    const uint32_t rsp_opcode = MODEL_OP(DFD_FW_STATUS);
    struct bt_mesh_msg_ctx ctx = BLE_MESH_DFD_MSG_CTX(cli);

    struct os_mbuf *msg = BT_MESH_MODEL_BUF(MODEL_OP(DFD_FW_GET), fwid_len);
    if (msg == NULL) {
        BT_ERR("msg mem failed, now break..");
        err = -ENOMEM;
        return err;
    }
    bt_mesh_model_msg_init(msg, MODEL_OP(DFD_FW_GET));
    memcpy_s(net_buf_simple_add(msg, fwid_len), fwid_len, fwid, fwid_len);

    err = bt_mesh_model_send(cli->mod, &ctx, msg, 0, cli);
    os_mbuf_free_chain(msg);
    if (err) {
        return err;
    }
    DFD_CLI_WAIT(err, cli->mod->user_data, result, rsp_opcode);

    return err;
}

int bt_mesh_dfd_client_fw_get_by_index(struct bt_mesh_dfd_client_t *cli,
    const uint16_t idx, struct bt_mesh_dist_fw_status_t *result)
{
    int err = 0;
    const uint32_t rsp_opcode = MODEL_OP(DFD_FW_STATUS);
    struct bt_mesh_msg_ctx ctx = BLE_MESH_DFD_MSG_CTX(cli);

    struct os_mbuf *msg = BT_MESH_MODEL_BUF(MODEL_OP(DFD_FW_GET_BY_INDEX), 2);
    if (msg == NULL) {
        BT_ERR("msg mem failed, now break..");
        err = -ENOMEM;
        return err;
    }
    bt_mesh_model_msg_init(msg, MODEL_OP(DFD_FW_GET_BY_INDEX));
    net_buf_simple_add_le16(msg, idx);

    err = bt_mesh_model_send(cli->mod, &ctx, msg, 0, cli);
    os_mbuf_free_chain(msg);
    if (err) {
        return err;
    }
    DFD_CLI_WAIT(err, cli->mod->user_data, result, rsp_opcode);

    return err;
}

int bt_mesh_dfd_client_fw_delete(struct bt_mesh_dfd_client_t *cli,
    const uint8_t *fwid, const uint32_t fwid_len, struct bt_mesh_dist_fw_status_t *result)
{
    int err = 0;
    const uint32_t rsp_opcode = MODEL_OP(DFD_FW_STATUS);
    struct bt_mesh_msg_ctx ctx = BLE_MESH_DFD_MSG_CTX(cli);

    struct os_mbuf *msg = BT_MESH_MODEL_BUF(MODEL_OP(DFD_FW_DELETE), 0);
    if (msg == NULL) {
        BT_ERR("msg mem failed, now break..");
        err = -ENOMEM;
        return err;
    }
    bt_mesh_model_msg_init(msg, MODEL_OP(DFD_FW_DELETE));
    memcpy_s(net_buf_simple_add(msg, fwid_len), fwid_len, fwid, fwid_len);

    err = bt_mesh_model_send(cli->mod, &ctx, msg, 0, cli);
    os_mbuf_free_chain(msg);
    if (err) {
        return err;
    }
    DFD_CLI_WAIT(err, cli->mod->user_data, result, rsp_opcode);

    return err;
}

int bt_mesh_dfd_client_fw_delete_all(struct bt_mesh_dfd_client_t *cli, struct bt_mesh_dist_fw_status_t *result)
{
    int err = 0;
    const uint32_t rsp_opcode = MODEL_OP(DFD_FW_STATUS);
    struct bt_mesh_msg_ctx ctx = BLE_MESH_DFD_MSG_CTX(cli);

    struct os_mbuf *msg = BT_MESH_MODEL_BUF(MODEL_OP(DFD_FW_DELETE_ALL), 0);
    if (msg == NULL) {
        BT_ERR("msg mem failed, now break..");
        err = -ENOMEM;
        return err;
    }
    bt_mesh_model_msg_init(msg, MODEL_OP(DFD_FW_DELETE_ALL));

    err = bt_mesh_model_send(cli->mod, &ctx, msg, 0, cli);
    os_mbuf_free_chain(msg);
    if (err) {
        return err;
    }
    DFD_CLI_WAIT(err, cli->mod->user_data, result, rsp_opcode);

    return err;
}

static int start_upload_transfer(struct bt_mesh_dfd_client_t *cli, uint16_t srv_addr)
{
    // set blobsrv also as dfdsrv
    sys_slist_init(&cli->blob_inputs.targets);
    memset_s(&cli->blob_target, sizeof(struct bt_mesh_blob_target_t), 0,
        sizeof(struct bt_mesh_blob_target_t));
    memset_s(&cli->blob_pull, sizeof(struct bt_mesh_blob_target_pull_t), 0,
        sizeof(struct bt_mesh_blob_target_pull_t));
    cli->blob_target.pull = &cli->blob_pull;
    cli->blob_target.addr = srv_addr;
    sys_slist_append(&cli->blob_inputs.targets, &cli->blob_target.n);
    cli->blob.inputs = &cli->blob_inputs; // set

    // cli->op to set DFD_UPLOAD_STATUS
    cli->upload.state = DFD_CLIENT_STATE_DISTR_UPLOAD_START;

    int err = 0;

    if (cli->upload.flags & FLAG_RESUME) {
        cli->upload.flags ^= FLAG_RESUME;
        err = bt_mesh_blob_client_resume(&cli->blob);
        if (err) {
            BT_ERR("Resuming BLOB transfer failed: %d", err);
            return err;
        }
        return 0;
    }

    if (cli->upload.flags & FLAG_SKIP_CAPS_GET) {
        cli->upload.flags ^= FLAG_SKIP_CAPS_GET;
        err = bt_mesh_blob_client_send(&cli->blob, cli->blob.inputs, &cli->upload.blob, cli->upload.stream);
        if (err) {
            BT_ERR("Starting BLOB transfer failed: %d", err);
            return err;
        }
        return err;
    }

    err = bt_mesh_blob_client_capabilities_get(&cli->blob, cli->blob.inputs);
    if (err) {
        BT_ERR("Failed starting blob transfer: %d", err);
        return err;
    }

    return err;
}


/* rsp handlers */
static int bt_mesh_dfd_client_handle_receivers_status(
    struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf);
static int bt_mesh_dfd_client_handle_receivers_list(
    struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf);
static int bt_mesh_dfd_client_handle_caps_status(
    struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf);
static int bt_mesh_dfd_client_handle_distribute_status(
    struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf);
static int bt_mesh_dfd_client_handle_upload_status(
    struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf);
static int bt_mesh_dfd_client_handle_fw_status(
    struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf);

const struct bt_mesh_model_op _bt_mesh_dfd_client_op[] = { // `todo BT_MESH_LEN_EXACT BT_MESH_LEN_MIN ?
    {MODEL_OP(DFD_RECEIVERS_STATUS), BT_MESH_LEN_EXACT(3), bt_mesh_dfd_client_handle_receivers_status},
    {MODEL_OP(DFD_RECEIVERS_LIST), BT_MESH_LEN_MIN(4), bt_mesh_dfd_client_handle_receivers_list},
    {MODEL_OP(DFD_CAPABILITIES_STATUS), BT_MESH_LEN_MIN(17), bt_mesh_dfd_client_handle_caps_status},
    {MODEL_OP(DFD_STATUS), BT_MESH_LEN_MIN(2), bt_mesh_dfd_client_handle_distribute_status},
    {MODEL_OP(DFD_UPLOAD_STATUS), BT_MESH_LEN_MIN(2), bt_mesh_dfd_client_handle_upload_status},
    {MODEL_OP(DFD_FW_STATUS), BT_MESH_LEN_MIN(5), bt_mesh_dfd_client_handle_fw_status},
    BT_MESH_MODEL_OP_END
};

static int bt_mesh_dfd_cli_deinit(struct bt_mesh_model *mod)
{
    struct bt_mesh_dfd_client_t *cli = mod->user_data;
    if (cli != NULL) {
        k_sem_deinit(&cli->req.sem);
    }

    if ((mod->pub != NULL) && (mod->pub->msg != NULL)) {
        os_mbuf_free(mod->pub->msg);
        mod->pub->msg = NULL;
    }
    return 0;
}

static int bt_mesh_dfd_cli_init(struct bt_mesh_model *mod)
{
    int err = 0;
    struct bt_mesh_dfd_client_t *cli = mod->user_data;
    const struct bt_mesh_model *blob_cli = NULL;

    cli->mod = mod;

    blob_cli = bt_mesh_model_find(bt_mesh_model_elem(mod), BT_MESH_MODEL_ID_BLOB_CLI);
    if (blob_cli == NULL) {
        BT_ERR("blob client model find error.");
        return -ENOMEM;
    }

    err = bt_mesh_model_extend(mod, cli->blob.mod);
    if (err) {
        BT_ERR("model extend error %d", err);
        return err;
    }

    cli->caps.max_receivers_list_size = CONFIG_BT_MESH_DFD_SRV_TARGETS_MAX;

    k_sem_init(&cli->req.sem, 0, 1);

    return 0;
}

static void bt_mesh_dfd_cli_reset(struct bt_mesh_model *mod)
{
    struct bt_mesh_dfd_client_t *cli = mod->user_data;

    cli->req.addr = BT_MESH_ADDR_UNASSIGNED;
    cli->req.op = 0;
    cli->req.params = NULL;

    cli->upload.state = 0; // STATE_IDLE;
    cli->upload.flags = 0;
}

const struct bt_mesh_model_cb _bt_mesh_dfd_client_cb = {
    .init = bt_mesh_dfd_cli_init,
    .reset = bt_mesh_dfd_cli_reset,
    .deinit = bt_mesh_dfd_cli_deinit,
};

static void bt_mesh_dfd_failed(struct bt_mesh_dfd_client_t *cli, enum bt_mesh_dfd_status_t reason)
{
    BT_INFO("%u", reason);

    cli->upload.flags |= FLAG_FAILED;

    if (cli->cb && cli->cb->on_ended) {
        cli->cb->on_ended(cli, reason);
    }
}

static void bt_mesh_dfd_cli_blob_caps(
    struct bt_mesh_blob_client_t *blob_client, const struct bt_mesh_blob_client_capabilities_t *caps)
{
    struct bt_mesh_dfd_client_t *cli = BLE_MESH_DFD_CLI(blob_client);
    int err = 0;

    if (!caps) {
        bt_mesh_dfd_failed(cli, BT_MESH_DFD_ERR_INSUFFICIENT_RESOURCES);
        return;
    }

    cli->upload.blob.block_size_logarithm = caps->max_block_size_logarithm;
    cli->upload.blob.chunk_size = caps->max_chunk_size;

    if (!(cli->upload.blob.mode & BT_MESH_BLOB_TRANSFER_MODE_ALL)) {
        cli->upload.blob.mode =
            caps->modes == BT_MESH_BLOB_TRANSFER_MODE_ALL ? BT_MESH_BLOB_TRANSFER_MODE_PUSH : caps->modes;
    } else {
        cli->upload.blob.mode =
            caps->modes == BT_MESH_BLOB_TRANSFER_MODE_ALL ? cli->upload.blob.mode : caps->modes;
    }

    err = bt_mesh_blob_client_send(blob_client, blob_client->inputs, &cli->upload.blob, cli->upload.stream);
    if (err) {
        BT_ERR("Starting BLOB transfer failed: %d", err);
        bt_mesh_dfd_failed(cli, BT_MESH_DFD_ERR_BUSY_WITH_UPLOAD);
    }
}

static void bt_mesh_dfd_cli_blob_lost_target(
    struct bt_mesh_blob_client_t *blob_client, struct bt_mesh_blob_target_t *blobt, enum bt_mesh_blob_status_t reason)
{
    BT_INFO("blob_lost_target");
}

static void bt_mesh_dfd_cli_blob_suspended(struct bt_mesh_blob_client_t *blob_client)
{
    struct bt_mesh_dfd_client_t *cli = BLE_MESH_DFD_CLI(blob_client);

    BT_INFO("BLOB transfer suspended");

    ota_node_role_take(OTA_NODE_ROLE_NONE);

    if (cli->cb && cli->cb->on_suspended) {
        cli->cb->on_suspended(cli);
    }
}

static void bt_mesh_dfd_cli_blob_end(
    struct bt_mesh_blob_client_t *blob_client, const struct bt_mesh_blob_transfer_t *transfer, bool success)
{
    struct bt_mesh_dfd_client_t *cli = BLE_MESH_DFD_CLI(blob_client);

    ota_node_role_take(OTA_NODE_ROLE_NONE);

    if (success) {
        return;
    }

    bt_mesh_dfd_failed(cli, BT_MESH_DFD_ERR_INTERNAL);
}

static void bt_mesh_dfd_cli_blob_transfer_progress(struct bt_mesh_blob_client_t *cli,
    struct bt_mesh_blob_target_t *target, const struct bt_mesh_blob_transfer_info_t *info)
{
    uint8_t progress = 0;
    uint8_t total_blocks = 0;
    uint8_t blocks_not_rxed = 0;
    uint8_t blocks_not_rxed_size = 0;
    int i = 0;

    total_blocks = DIV_ROUND_UP(info->size, 1U << info->block_size_logarithm);

    blocks_not_rxed_size = DIV_ROUND_UP(total_blocks, BT_MESH_BLOCKS_NOTRXED_SIZE);

    for (i = 0; i < blocks_not_rxed_size; i++) {
        blocks_not_rxed +=
            info->missing_blocks[i % BT_MESH_BLOCKS_NOTRXED_SIZE] & (1 << (i % BT_MESH_BLOCKS_NOTRXED_SIZE));
    }
    if (total_blocks > 0) {
        progress = (total_blocks - blocks_not_rxed) / total_blocks;
    }

    BT_INFO("BLOB transfer progress received from target 0x%04x:\n phase: %d\n progress: %u%%",
        target->addr, info->phase, progress);
}

static void bt_mesh_dfd_cli_blob_transfer_progress_complete(struct bt_mesh_blob_client_t*cli)
{
    BT_INFO("Determine BLOB transfer progress procedure complete");
}

const struct bt_mesh_blob_client_cb _bt_mesh_dfd_cli_blob_cli_cb = {
    .on_caps = bt_mesh_dfd_cli_blob_caps,
    .on_lost_target = bt_mesh_dfd_cli_blob_lost_target,
    .on_suspended = bt_mesh_dfd_cli_blob_suspended,
    .on_end = bt_mesh_dfd_cli_blob_end,
    .on_transfer_progress = bt_mesh_dfd_cli_blob_transfer_progress,
    .on_transfer_progress_complete = bt_mesh_dfd_cli_blob_transfer_progress_complete,
};

/* rsp handlers */
static int bt_mesh_dfd_client_handle_receivers_status(
    struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_dfd_client_t *cli = NULL;
    struct bt_mesh_dist_receivers_status_t *status = NULL;

    if (!mod || !mod->user_data) {
        BT_WARN("Mod or user data not existed");
        return 0;
    }

    cli = mod->user_data;
    status = (struct bt_mesh_dist_receivers_status_t *)cli->req.params;

    if (!status || cli->req.op != MODEL_OP(DFD_RECEIVERS_STATUS) || cli->req.addr != ctx->addr) {
        BT_WARN("Unexpected rsp(expecting %u) from 0x%04x(expecting 0x%04x)", cli->req.op, ctx->addr, cli->req.addr);
        return 0;
    }

    status->status = net_buf_simple_pull_u8(buf);
    status->n_receivers = net_buf_simple_pull_le16(buf);
    BT_DBG("%u:%u", status->status, status->n_receivers);

    k_sem_give(&cli->req.sem);
    return 0;
}

#define RECEIVERS_ENTRY_BITMASK_ADDR_15 15
#define RECEIVERS_ENTRY_BITMASK_PHASE_4 4
#define RECEIVERS_ENTRY_BITMASK_STATUS_3 3
#define RECEIVERS_ENTRY_BITMASK_STATUS_4 4
#define RECEIVERS_ENTRY_BITMASK_PROG_6 6
static int bt_mesh_dfd_client_handle_receivers_list(
    struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_dfd_client_t *cli = NULL;
    struct bt_mesh_dist_receivers_list_t *list = NULL;
    uint16_t n_receivers;
    uint16_t first_idx;

    if (!mod || !mod->user_data) {
        BT_WARN("Mod or user data not existed");
        return 0;
    }

    cli = (struct bt_mesh_dfd_client_t *)mod->user_data;
    list = (struct bt_mesh_dist_receivers_list_t *)cli->req.params;

    if (!list || cli->req.op != MODEL_OP(DFD_RECEIVERS_LIST) || cli->req.addr != ctx->addr) {
        BT_WARN("Unexpected rsp(expecting %u) from 0x%04x(expecting 0x%04x)", cli->req.op, ctx->addr, cli->req.addr);
        return 0;
    }

    n_receivers = net_buf_simple_pull_le16(buf);
    first_idx = net_buf_simple_pull_le16(buf);
    BT_INFO("%u:%u", n_receivers, first_idx);

    list->n_receivers = 0;
    if (n_receivers > DFD_CLIENT_RECEIVERS_LIST_MAX_NODES) {
        BT_ERR("overloaded receivers %d", n_receivers);
        return 0;
    }
    list->n_receivers = n_receivers;
    list->first_idx = first_idx;

    if (list->n_receivers * 5 > buf->om_len) {  // 5: len
        BT_ERR("n_receivers %u, buf len %u error", list->n_receivers, buf->om_len);
        return -1;
    }
    for (int i = 0; i < list->n_receivers; i++) {
        struct bt_mesh_dist_receivers_list_entry_t *t = &list->nodes[i];
        uint32_t b32 = net_buf_simple_pull_le32(buf);
        t->addr = b32 & BIT_MASK(RECEIVERS_ENTRY_BITMASK_ADDR_15);
        t->update_phase = (b32>>15U) & BIT_MASK(RECEIVERS_ENTRY_BITMASK_PHASE_4);
        t->update_status = (b32>>19U) & BIT_MASK(RECEIVERS_ENTRY_BITMASK_STATUS_3);
        t->transfer_status = (b32>>22U) & BIT_MASK(RECEIVERS_ENTRY_BITMASK_STATUS_4);
        t->transfer_progress = (b32>>26U) & BIT_MASK(RECEIVERS_ENTRY_BITMASK_PROG_6);
        t->update_fw_img_idx = net_buf_simple_pull_u8(buf);
    }
    
    k_sem_give(&cli->req.sem);
    return 0;
}
static int bt_mesh_dfd_client_handle_caps_status(
    struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_dfd_client_t *cli = NULL;
    struct bt_mesh_dist_caps_status_t *status = NULL;

    if (!mod || !mod->user_data) {
        BT_WARN("Mod or user data not existed");
        return 0;
    }

    cli = mod->user_data;
    status = (struct bt_mesh_dist_caps_status_t *)cli->req.params;

    if (!status || cli->req.op != MODEL_OP(DFD_CAPABILITIES_STATUS) || cli->req.addr != ctx->addr) {
        BT_WARN("Unexpected op(expecting 0x%08x) from 0x%04x(expecting 0x%04x)", cli->req.op, ctx->addr, cli->req.addr);
        return 0;
    }

    status->max_receivers_list_size = net_buf_simple_pull_le16(buf);
    status->max_fw_image_list_size = net_buf_simple_pull_le16(buf);
    status->max_fw_image_size = net_buf_simple_pull_le32(buf);
    status->max_upload_space = net_buf_simple_pull_le32(buf);
    status->remain_upload_space = net_buf_simple_pull_le32(buf);
    status->oob_retrieval = net_buf_simple_pull_u8(buf);

    k_sem_give(&cli->req.sem);
    return 0;
}

#define DIST_STATUS_BITMASK_MODE_2 2
#define DIST_STATUS_BITMASK_APPLY_1 1
static int bt_mesh_dfd_client_handle_distribute_status(
    struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_dfd_client_t *cli = NULL;
    struct bt_mesh_dist_status_t *status = NULL;

    if (!mod || !mod->user_data) {
        BT_WARN("Mod or user data not existed");
        return 0;
    }

    cli = mod->user_data;
    status = (struct bt_mesh_dist_status_t *)cli->req.params;

    if (!status || cli->req.op != MODEL_OP(DFD_STATUS) || cli->req.addr != ctx->addr) {
        BT_WARN("Unexpected rsp(expecting %u) from 0x%04x(expecting 0x%04x)", cli->req.op, ctx->addr, cli->req.addr);
        return 0;
    }

    status->fw_image_idx = 0;
    status->status = net_buf_simple_pull_u8(buf);
    status->phase = net_buf_simple_pull_u8(buf);
    if (buf->om_len > 1) {
        status->group = net_buf_simple_pull_le16(buf);
    }
    if (buf->om_len > 1) {
        status->app_idx = net_buf_simple_pull_le16(buf);
    }
    if (buf->om_len > 0) {
        status->ttl = net_buf_simple_pull_u8(buf);
    }
    if (buf->om_len > 1) {
        status->timeout_base = net_buf_simple_pull_le16(buf);
    }
    if (buf->om_len > 0) { // Table 5.28: Firmware Distribution Status message structure
        uint8_t b8 = net_buf_simple_pull_u8(buf);
        status->transfer_mode = b8 & BIT_MASK(DIST_STATUS_BITMASK_MODE_2);
        status->apply = (b8>>2U) & BIT_MASK(DIST_STATUS_BITMASK_APPLY_1);
    }
    if (buf->om_len > 1) {
        status->fw_image_idx = net_buf_simple_pull_le16(buf);
    }

    k_sem_give(&cli->req.sem);
    return 0;
}

#define UPLOAD_STATUS_BITMASK_PROG_7 7
#define UPLOAD_STATUS_BITMASK_TYPE_1 1
static int bt_mesh_dfd_client_handle_upload_status(
    struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_dfd_client_t *cli = NULL;
    struct bt_mesh_dist_upload_status_t *status = NULL;

    if (!mod || !mod->user_data) {
        BT_WARN("Mod or user data not existed");
        return 0;
    }

    cli = mod->user_data;
    status = (struct bt_mesh_dist_upload_status_t *)cli->req.params;

    if (!status || cli->req.op != MODEL_OP(DFD_UPLOAD_STATUS) || cli->req.addr != ctx->addr) {
        BT_WARN("Unexpected rsp(expecting %u) from 0x%04x(expecting 0x%04x)", cli->req.op, ctx->addr, cli->req.addr);
        return 0;
    }

    status->status = net_buf_simple_pull_u8(buf);
    status->upload_phase = net_buf_simple_pull_u8(buf);
    status->fwid_len = 0;
    if (buf->om_len) { // Table 5.33: Firmware Distribution Upload Status message structure
        uint8_t b8 = net_buf_simple_pull_u8(buf);
        status->progress = b8 & BIT_MASK(UPLOAD_STATUS_BITMASK_PROG_7);
        status->upload_type = (b8>>7U) & BIT_MASK(UPLOAD_STATUS_BITMASK_TYPE_1);
        status->fwid_len = 0;
        if (status->upload_type == BT_MESH_DFD_UPLOAD_TYPE_INBAND && buf->om_len) {
            status->fwid_len = buf->om_len;
            status->fwid = net_buf_simple_pull_mem(buf, buf->om_len);
        }
    }

    k_sem_give(&cli->req.sem);
    return 0;
}
static int bt_mesh_dfd_client_handle_fw_status(
    struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_dfd_client_t *cli = NULL;
    struct bt_mesh_dist_fw_status_t *status = NULL;

    if (!mod || !mod->user_data) {
        BT_WARN("Mod or user data not existed");
        return 0;
    }

    cli = mod->user_data;
    status = (struct bt_mesh_dist_fw_status_t *)cli->req.params;

    if (!status || cli->req.op != MODEL_OP(DFD_FW_STATUS) || cli->req.addr != ctx->addr) {
        BT_WARN("Unexpected rsp(expecting %u) from 0x%04x(expecting 0x%04x)", cli->req.op, ctx->addr, cli->req.addr);
        return 0;
    }

    status->fwid_len = 0;
    status->status = net_buf_simple_pull_u8(buf);
    status->n_imgs = net_buf_simple_pull_le16(buf);
    status->img_idx = net_buf_simple_pull_le16(buf);
    if (buf->om_len) {
        status->fwid_len = buf->om_len;
        status->fwid = net_buf_simple_pull_mem(buf, buf->om_len);
    }

    k_sem_give(&cli->req.sem);
    return 0;
}
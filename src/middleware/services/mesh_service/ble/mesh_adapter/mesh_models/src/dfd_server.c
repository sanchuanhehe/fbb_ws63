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
#include "model/model_opcode.h"
#include "model_common.h"
#include "model/dfd.h"
#include "model/blob_server.h"
#include "model/blob_client.h"
#include "model/dfu_client.h"
#include "dfu_bank.h"
#include "dfd_srv_internal.h"
#include "mesh_ota_common.h"
#include "access.h"
#include "model/dfd_server.h"

#define DFD_RECV_STATUS_LEN             3
#define DFD_RECV_ADD_LEN                3
#define DFD_CAPABILITIES_STATUS_LEN     17
#define DFD_STATUS_LEN                  12
#define DFD_START_LEN                   16
#define DFD_FW_STATUS_LEN               7
#define DFD_TARGET_NODE_ENTRY_LEN       5
#define DFU_CLIENT_PROGRESS             2
#define DFU_BLOB_ID_DEFAULT             8

#define DFD_UPLOAD_STATUS_MSG_MAXLEN (5 + MYNEWT_VAL_BT_MESH_DFU_FWID_MAX_LEN)
#define DFD_UPLOAD_START_MSG_MAXLEN \
    (DFD_START_LEN + MYNEWT_VAL_BT_MESH_DFU_FWID_MAX_LEN + MYNEWT_VAL_BT_MESH_DFU_METADATA_MAX_LEN)
#define DFD_RECEIVERS_LIST_MSG_MAXLEN \
    (BT_MESH_TX_SDU_MAX - BT_MESH_MIC_SHORT - BT_MESH_MODEL_OP_LEN(MODEL_OP(DFD_RECEIVERS_LIST)))
#define DFD_RECEIVERS_ADD_MSG_MAXLEN (CONFIG_BT_MESH_DFD_SRV_TARGETS_MAX * 3)

BUILD_ASSERT((DFD_UPLOAD_STATUS_MSG_MAXLEN + BT_MESH_MODEL_OP_LEN(MODEL_OP(DFD_UPLOAD_STATUS)) + BT_MESH_MIC_SHORT) <=
                 BT_MESH_TX_SDU_MAX,
    "Send Firmware Distribution Upload Status message Len error");
BUILD_ASSERT((DFD_UPLOAD_START_MSG_MAXLEN + BT_MESH_MODEL_OP_LEN(MODEL_OP(DFD_UPLOAD_START)) + BT_MESH_MIC_SHORT) <=
                 BT_MESH_RX_SDU_MAX,
    "Recv Firmware Distribution Upload Status message Len error");

typedef struct bt_mesh_dfd_srv_oob_ctx {
    uint8_t uri[MYNEWT_VAL_BT_MESH_DFU_URI_MAX_LEN];
    uint8_t uri_len;
    uint8_t fwid[MYNEWT_VAL_BT_MESH_DFU_FWID_MAX_LEN];
    uint8_t fwid_len;
    const struct bt_mesh_dfu_bank_t *bank;
    uint8_t progress;
    bool started;
} bt_mesh_dfd_srv_oob_ctx_t;

static bt_mesh_dfd_srv_oob_ctx_t dfd_srv_oob_ctx;

static int bt_mesh_dfd_srv_start_oob_upload(struct bt_mesh_dfd_srv_t *srv, const struct bt_mesh_dfu_bank_t *bank,
    const char *uri, uint8_t uri_len, const uint8_t *fwid, uint16_t fwid_len)
{
    BT_INFO("Start OOB Upload");

    if (memcpy_s(dfd_srv_oob_ctx.uri, MYNEWT_VAL_BT_MESH_DFU_URI_MAX_LEN, uri, uri_len) != EOK) {
        return BT_MESH_DFD_ERR_INTERNAL;
    }
    dfd_srv_oob_ctx.uri_len = uri_len;
    if (memcpy_s(dfd_srv_oob_ctx.fwid, MYNEWT_VAL_BT_MESH_DFU_FWID_MAX_LEN, fwid, fwid_len) != EOK) {
        return BT_MESH_DFD_ERR_INTERNAL;
    }
    dfd_srv_oob_ctx.fwid_len = fwid_len;
    dfd_srv_oob_ctx.bank = bank;
    dfd_srv_oob_ctx.progress = 0;
    dfd_srv_oob_ctx.started = true;

    return BT_MESH_DFD_SUCCESS;
}

static void bt_mesh_dfd_srv_cancel_oob_upload(struct bt_mesh_dfd_srv_t *srv, const struct bt_mesh_dfu_bank_t *bank)
{
    BT_INFO("Cancel OOB Upload");

    dfd_srv_oob_ctx.started = false;
}

static uint8_t bt_mesh_dfd_srv_oob_progress_get(struct bt_mesh_dfd_srv_t *srv, const struct bt_mesh_dfu_bank_t *bank)
{
    uint8_t progress;

    if (dfd_srv_oob_ctx.started) {
        progress = dfd_srv_oob_ctx.progress;

        dfd_srv_oob_ctx.progress = MIN(dfd_srv_oob_ctx.progress + 25, 100); // 25,100:progress
    } else {
        progress = 0;
    }

    BT_INFO("OOB Progress Get (%sstarted: %d %%)", dfd_srv_oob_ctx.started ? "" : "not ", progress);
    return progress;
}

static int dist_fw_recv(
    struct bt_mesh_dfd_srv_t *srv, const struct bt_mesh_dfu_bank_t *bank, const struct bt_mesh_blob_stream_t **srteam)
{
    BT_INFO("dist_fw_recv");
    *srteam = bt_mesh_get_upload_blob_stream();
    return 0;
}

static void dist_fw_del(struct bt_mesh_dfd_srv_t *srv, const struct bt_mesh_dfu_bank_t *bank)
{}

static int dist_fw_send(
    struct bt_mesh_dfd_srv_t *srv, const struct bt_mesh_dfu_bank_t *bank, const struct bt_mesh_blob_stream_t **srteam)
{
    BT_INFO("dist_fw_send");
    *srteam = bt_mesh_get_dist_blob_stream();
    return 0;
}

static void dist_phase_changed(struct bt_mesh_dfd_srv_t *srv, enum bt_mesh_dfd_phase_t phase)
{
    static enum bt_mesh_dfd_phase_t prev_phase;

    if (phase == BT_MESH_DFD_PHASE_COMPLETED || phase == BT_MESH_DFD_PHASE_FAILED) {
        if (phase == BT_MESH_DFD_PHASE_FAILED) {
            if (prev_phase != BT_MESH_DFD_PHASE_APPLYING_UPDATE) {
                return;
            }
        }
    }

    prev_phase = phase;
}

static const struct bt_mesh_dfd_srv_cb_t dfd_srv_cb = {
    .recv = dist_fw_recv,
    .del = dist_fw_del,
    .send = dist_fw_send,
    .phase = dist_phase_changed,
    .start_oob_upload = bt_mesh_dfd_srv_start_oob_upload,
    .cancel_oob_upload = bt_mesh_dfd_srv_cancel_oob_upload,
    .oob_progress_get = bt_mesh_dfd_srv_oob_progress_get,
};

struct bt_mesh_dfd_srv_t g_dfd_srv = BT_MESH_DFD_SRV_INIT(&dfd_srv_cb);

void bt_mesh_upload_status_rsp_oob(
    struct bt_mesh_dfd_srv_t *srv, struct bt_mesh_msg_ctx *ctx, enum bt_mesh_dfd_status_t status);

static void bt_mesh_dfd_recv_status_rsp(
    struct bt_mesh_dfd_srv_t *srv, struct bt_mesh_msg_ctx *ctx, enum bt_mesh_dfd_status_t status)
{
    int err = 0;
    struct os_mbuf *buf = BT_MESH_MODEL_BUF(MODEL_OP(DFD_RECEIVERS_STATUS), DFD_RECV_STATUS_LEN);

    bt_mesh_model_msg_init(buf, MODEL_OP(DFD_RECEIVERS_STATUS));
    net_buf_simple_add_u8(buf, status);
    net_buf_simple_add_le16(buf, srv->target_cnt);

    err = bt_mesh_model_send(srv->mod, ctx, buf, NULL, NULL);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("dfd recv status model_send() failed (err %d)", err);
    }
    os_mbuf_free_chain(buf);
}

static int bt_mesh_dfd_handle_recv_get(struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_dfd_srv_t *srv = mod->user_data;
    uint16_t first_idx = 0;
    uint16_t max_cnt = 0;
    uint8_t progress = 0;
    int i = 0;
    int err = 0;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    first_idx = net_buf_simple_pull_le16(buf);
    max_cnt = net_buf_simple_pull_le16(buf);
    if (max_cnt == 0 || first_idx >= srv->target_cnt) {
        BT_ERR("max_cnt %u or first_idx %u error", max_cnt, first_idx);
        return -EINVAL;
    }

    struct os_mbuf *rsp = BT_MESH_MODEL_BUF(MODEL_OP(DFD_RECEIVERS_LIST), DFD_RECEIVERS_LIST_MSG_MAXLEN);

    bt_mesh_model_msg_init(rsp, MODEL_OP(DFD_RECEIVERS_LIST));

    max_cnt = MIN(max_cnt, srv->target_cnt - first_idx);
    progress = bt_mesh_dfu_client_progress(&srv->dfu) / DFU_CLIENT_PROGRESS;

    net_buf_simple_add_le16(rsp, max_cnt);
    net_buf_simple_add_le16(rsp, first_idx);

    for (i = 0; i < max_cnt && net_buf_simple_tailroom(rsp) >= DFD_TARGET_NODE_ENTRY_LEN + BT_MESH_MIC_SHORT; i++) {
        const struct bt_mesh_dfu_target_t *t = &srv->targets[i + first_idx];
        net_buf_simple_add_le32(rsp,
            ((t->blob.addr & BIT_MASK(15)) | ((t->phase & BIT_MASK(4)) << 15U) |  // 15:Address 4:Retrieved Update Phase
                ((t->status & BIT_MASK(3)) << 19U) |       // 3:Update Status
                ((t->blob.status & BIT_MASK(4)) << 22U) |  // 4:Transfer Status
                ((progress & BIT_MASK(6)) << 26U)));       // 6:Transfer Progress
        net_buf_simple_add_u8(rsp, t->img_idx);
    }

    err = bt_mesh_model_send(srv->mod, ctx, rsp, NULL, NULL);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("dfd recv list model_send() failed (err %d)", err);
    }
    os_mbuf_free_chain(rsp);

    return 0;
}

static struct bt_mesh_dfu_target_t *bt_mesh_dfd_target_get(struct bt_mesh_dfd_srv_t *srv, uint16_t addr)
{
    if (!srv || srv->target_cnt > CONFIG_BT_MESH_DFD_SRV_TARGETS_MAX) {
        return NULL;
    }

    int i = 0;
    for (i = 0; i < srv->target_cnt; i++) {
        if (addr == srv->targets[i].blob.addr) {
            return &srv->targets[i];
        }
    }

    return NULL;
}

enum bt_mesh_dfd_status_t bt_mesh_dfd_srv_recv_add(struct bt_mesh_dfd_srv_t *srv, uint16_t addr, uint8_t img_idx)
{
    struct bt_mesh_dfu_target_t *t;
    struct bt_mesh_blob_target_pull_t *p;

    if (!BT_MESH_ADDR_IS_UNICAST(addr)) {
        return BT_MESH_DFD_SUCCESS;
    }

    if (bt_mesh_has_addr(addr)) {
        srv->dist_self = true;
        return BT_MESH_DFD_SUCCESS;
    }

    t = bt_mesh_dfd_target_get(srv, addr);
    if (t) {
        t->img_idx = img_idx;
        return BT_MESH_DFD_SUCCESS;
    }

    if (srv->target_cnt == ARRAY_SIZE(srv->targets)) {
        return BT_MESH_DFD_ERR_INSUFFICIENT_RESOURCES;
    }

    t = &srv->targets[srv->target_cnt];
#if MYNEWT_VAL(BT_MESH_OTA_PULL)
    p = &srv->pull_ctxs[srv->target_cnt];
    memset_s(p, sizeof(*p), 0, sizeof(*p));
#endif
    srv->target_cnt++;

    memset_s(t, sizeof(*t), 0, sizeof(*t));
    t->blob.addr = addr;
#if MYNEWT_VAL(BT_MESH_OTA_PULL)
    t->blob.pull = p;
#endif
    t->img_idx = img_idx;
    t->phase = BT_MESH_DFU_PHASE_UNKNOWN;

    BT_INFO("dfu recv add, blob addr: 0x%04x img: %u", t->blob.addr, t->img_idx);

    return BT_MESH_DFD_SUCCESS;
}

static int bt_mesh_dfd_handle_recv_add(struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    uint8_t img_idx = 0;
    uint16_t addr = 0;
    enum bt_mesh_dfd_status_t status = BT_MESH_DFD_SUCCESS;
    struct bt_mesh_dfd_srv_t *srv = mod->user_data;

    if (buf->om_len % DFD_RECV_ADD_LEN) {
        BT_ERR("om_len %u failed", buf->om_len);
        return -EINVAL;
    }

    if (bt_mesh_dfu_client_is_busy(&srv->dfu)) {
        bt_mesh_dfd_recv_status_rsp(srv, ctx, BT_MESH_DFD_ERR_BUSY_WITH_DISTRIBUTION);
        return 0;
    }

    if (srv->phase != BT_MESH_DFD_PHASE_IDLE && srv->phase != BT_MESH_DFD_PHASE_COMPLETED &&
        srv->phase != BT_MESH_DFD_PHASE_FAILED) {
        bt_mesh_dfd_recv_status_rsp(srv, ctx, BT_MESH_DFD_ERR_BUSY_WITH_DISTRIBUTION);
        return 0;
    }

    while (buf->om_len >= DFD_RECV_ADD_LEN && status == BT_MESH_DFD_SUCCESS) {
        addr = net_buf_simple_pull_le16(buf);
        img_idx = net_buf_simple_pull_u8(buf);
        status = bt_mesh_dfd_srv_recv_add(srv, addr, img_idx);
    }
    bt_mesh_dfd_recv_status_rsp(srv, ctx, status);

    return 0;
}

static int bt_mesh_dfd_handle_recv_del_all(struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_dfd_srv_t *srv = mod->user_data;
    enum bt_mesh_dfd_status_t status = BT_MESH_DFD_SUCCESS;
    
    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    status = bt_mesh_dfd_srv_recv_del_all(srv);
    bt_mesh_dfd_recv_status_rsp(srv, ctx, status);

    return 0;
}

enum bt_mesh_dfu_iter_t bt_mesh_dfd_bank_space_cb(const struct bt_mesh_dfu_bank_t *bank, void *user_data)
{
    uint32_t *total = user_data;

    *total += bank->size;

    return BT_MESH_DFU_ITER_CONTINUE;
}

static int bt_mesh_dfd_handle_capabilities_get(
    struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    uint32_t size = 0;
    int err = 0;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    struct os_mbuf *rsp = BT_MESH_MODEL_BUF(MODEL_OP(DFD_CAPABILITIES_STATUS), DFD_CAPABILITIES_STATUS_LEN);
    bt_mesh_model_msg_init(rsp, MODEL_OP(DFD_CAPABILITIES_STATUS));
    net_buf_simple_add_le16(rsp, CONFIG_BT_MESH_DFD_SRV_TARGETS_MAX);
    net_buf_simple_add_le16(rsp, MYNEWT_VAL_BT_MESH_DFU_BANK_CNT);
    net_buf_simple_add_le32(rsp, CONFIG_BT_MESH_DFD_SRV_BANK_MAX_SIZE);
    net_buf_simple_add_le32(rsp, CONFIG_BT_MESH_DFD_SRV_BANK_SPACE);

    (void)bt_mesh_dfu_bank_foreach(bt_mesh_dfd_bank_space_cb, &size);
    size = MIN(size, CONFIG_BT_MESH_DFD_SRV_BANK_SPACE);
    net_buf_simple_add_le32(rsp, CONFIG_BT_MESH_DFD_SRV_BANK_SPACE - size);
    net_buf_simple_add_u8(rsp, 0);

    err = bt_mesh_model_send(mod, ctx, rsp, NULL, NULL);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("dfd capabilities status model_send() failed (err %d)", err);
    }
    os_mbuf_free_chain(rsp);

    return 0;
}

static void bt_mesh_dfd_status_rsp(
    struct bt_mesh_dfd_srv_t *srv, struct bt_mesh_msg_ctx *ctx, enum bt_mesh_dfd_status_t status)
{
    uint8_t val = 0;
    int err = 0;
    struct os_mbuf *rsp = BT_MESH_MODEL_BUF(MODEL_OP(DFD_STATUS), DFD_STATUS_LEN);

    bt_mesh_model_msg_init(rsp, MODEL_OP(DFD_STATUS));
    net_buf_simple_add_u8(rsp, status);
    net_buf_simple_add_u8(rsp, srv->phase);

    if (srv->phase != BT_MESH_DFD_PHASE_IDLE && srv->dfu.transfer.bank) {
        net_buf_simple_add_le16(rsp, srv->inputs.group);
        net_buf_simple_add_le16(rsp, srv->inputs.app_idx);
        net_buf_simple_add_u8(rsp, srv->inputs.ttl);
        net_buf_simple_add_le16(rsp, srv->inputs.timeout_base);
        val = (srv->dfu.transfer.blob.mode & BIT_MASK(2)) |  // 2:Distribution Transfer Mode
              ((srv->apply & BIT_MASK(1)) << 2);  // 1:Update Policy 2:position
        net_buf_simple_add_u8(rsp, val);
        net_buf_simple_add_le16(rsp, srv->bank_idx);
    }

    err = bt_mesh_model_send(srv->mod, ctx, rsp, NULL, NULL);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("dfd status model_send() failed (err %d)", err);
    }
    os_mbuf_free_chain(rsp);
}

static int bt_mesh_dfd_handle_get(struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_dfd_srv_t *srv = mod->user_data;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    bt_mesh_dfd_status_rsp(srv, ctx, BT_MESH_DFD_SUCCESS);

    return 0;
}

static int bt_mesh_dfd_handle_start(struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_dfd_srv_t *srv = mod->user_data;
    struct bt_mesh_dfd_start_params params = {0};
    enum bt_mesh_dfd_status_t status = BT_MESH_DFD_SUCCESS;
    uint8_t val = 0;

    params.app_idx = net_buf_simple_pull_le16(buf);
    params.ttl = net_buf_simple_pull_u8(buf);
    params.timeout_base = net_buf_simple_pull_le16(buf);
    val = net_buf_simple_pull_u8(buf);
    params.transfer_mode = val & BIT_MASK(2); // 2:Distribution Transfer Mode
    params.apply = (val >> 2U) & BIT_MASK(1); // 1:Update Policy 2U:position
    params.bank_idx = net_buf_simple_pull_le16(buf);

    if (buf->om_len == DFD_START_LEN) {
        bt_mesh_dfd_status_rsp(srv, ctx, BT_MESH_DFD_ERR_INTERNAL);
        return 0;
    }

    if (buf->om_len != sizeof(params.group)) {
        return -EINVAL;
    }
    params.group = net_buf_simple_pull_le16(buf);

    if (!ota_node_role_take(OTA_NODE_ROLE_DISTRIBUTOR)) {
        BT_ERR("start: take role failed");
        return BT_MESH_DFD_ERR_INTERNAL;
    }

    status = bt_mesh_dfd_srv_start(srv, &params, ctx);
    if (status != BT_MESH_DFD_SUCCESS) {
        ota_node_role_take(OTA_NODE_ROLE_NONE);
    }

    return 0;
}

static int bt_mesh_dfd_handle_suspend(struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_dfd_srv_t *srv = mod->user_data;
    enum bt_mesh_dfd_status_t status = BT_MESH_DFD_SUCCESS;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    status = bt_mesh_dfd_srv_suspend(srv);
    bt_mesh_dfd_status_rsp(srv, ctx, status);

    return 0;
}

static int bt_mesh_dfd_handle_cancel(struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_dfd_srv_t *srv = mod->user_data;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    bt_mesh_dfd_srv_cancel(srv, ctx);

    return 0;
}

static int bt_mesh_dfd_handle_apply(struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_dfd_srv_t *srv = mod->user_data;
    enum bt_mesh_dfd_status_t status = BT_MESH_DFD_SUCCESS;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    status = bt_mesh_dfd_srv_apply(srv);
    bt_mesh_dfd_status_rsp(srv, ctx, status);

    return 0;
}

static void bt_mesh_dfd_upload_status_rsp_with_pro(
    struct bt_mesh_dfd_srv_t *srv, struct bt_mesh_msg_ctx *ctx, enum bt_mesh_dfd_status_t status, uint8_t progress)
{
    int err = 0;
    uint8_t remaining_progress = progress;
    struct os_mbuf *rsp = BT_MESH_MODEL_BUF(MODEL_OP(DFD_UPLOAD_STATUS), DFD_UPLOAD_STATUS_MSG_MAXLEN);

    BT_INFO("upload phase %u", srv->upload.phase);
    if (srv->upload.phase == BT_MESH_DFD_UPLOAD_PHASE_TRANSFER_ERROR) {
        remaining_progress = 0;
    }

    bt_mesh_model_msg_init(rsp, MODEL_OP(DFD_UPLOAD_STATUS));
    net_buf_simple_add_u8(rsp, status);
    net_buf_simple_add_u8(rsp, srv->upload.phase);

    if (srv->upload.phase != BT_MESH_DFD_UPLOAD_PHASE_IDLE && srv->upload.bank) {
        net_buf_simple_add_u8(rsp, remaining_progress);
        net_buf_simple_add_mem(rsp, srv->upload.bank->fwid, srv->upload.bank->fwid_len);
    }

    err = bt_mesh_model_send(srv->mod, ctx, rsp, NULL, NULL);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("dfd upload status model_send() failed (err %d)", err);
    }
    os_mbuf_free_chain(rsp);
}

static void bt_mesh_dfd_upload_status_rsp(
    struct bt_mesh_dfd_srv_t *srv, struct bt_mesh_msg_ctx *ctx, enum bt_mesh_dfd_status_t status)
{
    uint8_t progress = 0;
    enum bt_mesh_dfd_status_t remaining_status = status;
    if (srv->upload.is_oob) {
        remaining_status = BT_MESH_DFD_SUCCESS;
        bt_mesh_upload_status_rsp_oob(srv, ctx, remaining_status);
    } else {
        progress = bt_mesh_blob_server_progress(&srv->upload.blob);
        bt_mesh_dfd_upload_status_rsp_with_pro(srv, ctx, remaining_status, progress);
    }
}

static int bt_mesh_dfd_handle_upload_get(struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_dfd_srv_t *srv = mod->user_data;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    bt_mesh_dfd_upload_status_rsp(srv, ctx, BT_MESH_DFD_SUCCESS);

    return 0;
}

static int bt_mesh_dfd_set_upload_fwid(
    struct bt_mesh_dfd_srv_t *srv, struct bt_mesh_msg_ctx *ctx, const uint8_t *fwid, uint32_t fwid_len)
{
    uint8_t progress = 100;
    int err = 0;

    err = bt_mesh_dfu_bank_fwid_set(srv->upload.bank, fwid, fwid_len);
    switch (err) {
        case -EFBIG:
        case -EALREADY:
            bt_mesh_dfu_bank_release(srv->upload.bank);
            srv->upload.bank = NULL;
            bt_mesh_dfd_upload_status_rsp(srv, ctx, BT_MESH_DFD_ERR_INTERNAL);
            break;
        case -EEXIST:
            srv->upload.phase = BT_MESH_DFD_UPLOAD_PHASE_TRANSFER_SUCCESS;
            bt_mesh_dfu_bank_release(srv->upload.bank);

            err = bt_mesh_dfu_bank_get(fwid, fwid_len, &srv->upload.bank);
            if (!err) {
                bt_mesh_dfd_upload_status_rsp_with_pro(srv, ctx, BT_MESH_DFD_SUCCESS, progress);
            } else {
                srv->upload.bank = NULL;
                bt_mesh_dfd_upload_status_rsp(srv, ctx, BT_MESH_DFD_ERR_INTERNAL);
            }
            break;
        case 0:
            srv->upload.phase = BT_MESH_DFD_UPLOAD_PHASE_TRANSFER_ACTIVE;
            break;
        case -EINVAL:
        default:
            break;
    }

    return err;
}

static bool bt_mesh_dfd_upload_is_busy(const struct bt_mesh_dfd_srv_t *srv)
{
    return bt_mesh_blob_server_is_busy(&srv->upload.blob) ||
           srv->upload.phase == BT_MESH_DFD_UPLOAD_PHASE_TRANSFER_ACTIVE;
}

static bool bt_mesh_dfd_upload_start_task_check(
    struct bt_mesh_dfd_srv_t *srv, struct bt_mesh_msg_ctx *ctx, struct bt_mesh_dfd_upload_start_t *upload_start)
{
    if (upload_start->size > CONFIG_BT_MESH_DFD_SRV_BANK_MAX_SIZE) {
        bt_mesh_dfd_upload_status_rsp(srv, ctx, BT_MESH_DFD_ERR_INSUFFICIENT_RESOURCES);
        return false;
    }

    if (bt_mesh_dfd_upload_is_busy(srv)) {
        if (!srv->upload.bank) {
            BT_WARN("dfd upload is busy");
            bt_mesh_dfd_upload_status_rsp(srv, ctx, BT_MESH_DFD_ERR_INTERNAL);
            return false;
        }

        if (srv->upload.bank->fwid_len == upload_start->fwid_len &&
            !memcmp(srv->upload.bank->fwid, upload_start->fwid, upload_start->fwid_len) &&
            srv->upload.bank->metadata_len == upload_start->meta_len &&
            !memcmp(srv->upload.bank->metadata, upload_start->meta, upload_start->meta_len) &&
            srv->upload.blob.state.transfer.id == upload_start->blob_id &&
            srv->upload.blob.state.ttl == upload_start->ttl &&
            srv->upload.blob.state.timeout_base == upload_start->timeout_base) {
            BT_INFO("repeat upload start");
            bt_mesh_dfd_upload_status_rsp(srv, ctx, BT_MESH_DFD_SUCCESS);
            return false;
        }

        BT_WARN("not support multiple tasks");
        bt_mesh_dfd_upload_status_rsp(srv, ctx, BT_MESH_DFD_ERR_BUSY_WITH_UPLOAD);
        return false;
    }

    return true;
}

static int bt_mesh_dfd_upload_start_param_get(struct os_mbuf *buf, struct bt_mesh_dfd_upload_start_t *upload_start)
{
    upload_start->ttl = net_buf_simple_pull_u8(buf);
    upload_start->timeout_base = net_buf_simple_pull_le16(buf);
    upload_start->blob_id = net_buf_simple_pull_le32(buf);
    upload_start->blob_id |= ((uint64_t)net_buf_simple_pull_le32(buf) << 32); // 32:position
    upload_start->size = net_buf_simple_pull_le32(buf);
    upload_start->meta_len = net_buf_simple_pull_u8(buf);
    if (buf->om_len < upload_start->meta_len) {
        BT_ERR("buf len(%u) < meta len(%u) error", buf->om_len, upload_start->meta_len);
        return -EINVAL;
    }

    upload_start->meta = net_buf_simple_pull_mem(buf, upload_start->meta_len);
    upload_start->fwid_len = buf->om_len;
    if (buf->om_len == 0) {
        BT_ERR("fwid len error");
        return -EINVAL;
    }
    upload_start->fwid = net_buf_simple_pull_mem(buf, upload_start->fwid_len);

    BT_INFO("Upload Start: size: %d, fwid: %s, metadata: %s",
        upload_start->size,
        bt_hex(upload_start->fwid, upload_start->fwid_len),
        bt_hex(upload_start->meta, upload_start->meta_len));

    return 0;
}

static bool bt_mesh_dfd_upload_start_blob_proc(
    struct bt_mesh_dfd_srv_t *srv, struct bt_mesh_dfd_upload_start_t *upload_start, struct bt_mesh_msg_ctx *ctx)
{
    int err = 0;

    srv->stream = NULL;
    err = srv->cb->recv(srv, srv->upload.bank, &srv->stream);
    if (err || !srv->stream) {
        BT_ERR("dfd server upload error %d, stream: %p", err, srv->stream);
        bt_mesh_dfu_bank_release(srv->upload.bank);
        bt_mesh_dfd_upload_status_rsp(srv, ctx, BT_MESH_DFD_ERR_INTERNAL);
        return false;
    }

    if (!ota_node_role_take(OTA_NODE_ROLE_DISTRIBUTOR)) {
        BT_ERR("take role failed");
        bt_mesh_dfu_bank_release(srv->upload.bank);
        bt_mesh_dfd_upload_status_rsp(srv, ctx, BT_MESH_DFD_ERR_INTERNAL);
        return false;
    }
    err = bt_mesh_blob_server_recv(
        &srv->upload.blob, upload_start->blob_id, srv->stream, upload_start->ttl, upload_start->timeout_base);
    if (err) {
        BT_ERR("blob server upload error %d", err);
        ota_node_role_take(OTA_NODE_ROLE_NONE);
        bt_mesh_dfu_bank_release(srv->upload.bank);
        bt_mesh_dfd_upload_status_rsp(srv, ctx, BT_MESH_DFD_ERR_INTERNAL);
        return false;
    }

    return true;
}

static int bt_mesh_dfd_handle_upload_start(struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_dfd_srv_t *srv = mod->user_data;
    struct bt_mesh_dfd_upload_start_t upload_start = {0};
    int err = 0;
    bool res = false;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    err = bt_mesh_dfd_upload_start_param_get(buf, &upload_start);
    if (err) {
        BT_ERR("upload start param get error");
        return err;
    }
    res = bt_mesh_dfd_upload_start_task_check(srv, ctx, &upload_start);
    if (res != true) {
        return 0;
    }
    if (srv->upload.bank) {
        bt_mesh_dfu_bank_release(srv->upload.bank);
    }
    srv->upload.bank = bt_mesh_dfu_bank_reserve();
    if (!srv->upload.bank) {
        bt_mesh_dfd_upload_status_rsp(srv, ctx, BT_MESH_DFD_ERR_INSUFFICIENT_RESOURCES);
        return 0;
    }
    err = bt_mesh_dfd_set_upload_fwid(srv, ctx, upload_start.fwid, upload_start.fwid_len);
    if (err) {
        BT_WARN("dfd set upload fwid error %d", err);
        return err;
    }
    err = bt_mesh_dfu_bank_info_set(srv->upload.bank, upload_start.size, upload_start.meta, upload_start.meta_len);
    switch (err) {
        case -EFBIG:
            bt_mesh_dfd_upload_status_rsp(srv, ctx, BT_MESH_DFD_ERR_INTERNAL);
            break;
        case 0:
            break;
        default:
            BT_WARN("dfu solt info set error %d", err);
            return err;
    }
    res = bt_mesh_dfd_upload_start_blob_proc(srv, &upload_start, ctx);
    if (res != true) {
        return 0;
    }
    srv->upload.phase = BT_MESH_DFD_UPLOAD_PHASE_TRANSFER_ACTIVE;
    bt_mesh_dfd_upload_status_rsp(srv, ctx, BT_MESH_DFD_SUCCESS);

    return 0;
}

void bt_mesh_upload_status_rsp_oob(
    struct bt_mesh_dfd_srv_t *srv, struct bt_mesh_msg_ctx *ctx, enum bt_mesh_dfd_status_t status)
{
    uint8_t progress = 0;
    int err = 0;

    if (srv->upload.is_oob) {
        progress = srv->cb->oob_progress_get(srv, srv->upload.bank);
    }

    if (progress == 100) { // 100:success
        srv->upload.phase = BT_MESH_DFD_UPLOAD_PHASE_TRANSFER_SUCCESS;
    }

    struct os_mbuf *rsp = BT_MESH_MODEL_BUF(MODEL_OP(DFD_UPLOAD_STATUS), DFD_UPLOAD_STATUS_MSG_MAXLEN);

    bt_mesh_model_msg_init(rsp, MODEL_OP(DFD_UPLOAD_STATUS));
    net_buf_simple_add_u8(rsp, status);
    net_buf_simple_add_u8(rsp, srv->upload.phase);

    if (srv->upload.phase != BT_MESH_DFD_UPLOAD_PHASE_IDLE && srv->upload.bank) {
        if (srv->upload.is_oob) {
            net_buf_simple_add_u8(rsp, progress | BIT(7)); // 7:Upload Progress
            net_buf_simple_add_mem(rsp, srv->upload.oob.current_fwid, srv->upload.oob.current_fwid_len);
        } else {
            net_buf_simple_add_u8(rsp, progress);
            net_buf_simple_add_mem(rsp, srv->upload.bank->fwid, srv->upload.bank->fwid_len);
        }
    }

    err = bt_mesh_model_send(srv->mod, ctx, rsp, NULL, NULL);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("dfd upload status model_send() failed (err %d)", err);
    }
    os_mbuf_free_chain(rsp);
}

static int bt_mesh_dfd_upload_start_oob_param_get(
    struct bt_mesh_dfd_upload_start_oob_t *upload_start_oob, struct os_mbuf *buf)
{
    upload_start_oob->uri_len = net_buf_simple_pull_u8(buf);
    if (upload_start_oob->uri_len > buf->om_len) {
        BT_ERR("uri len(%u) > buf len(%u) error", upload_start_oob->uri_len, buf->om_len);
        return -EINVAL;
    }
    upload_start_oob->uri = net_buf_simple_pull_mem(buf, upload_start_oob->uri_len);

    upload_start_oob->fwid_len = buf->om_len;
    if (buf->om_len == 0) {
        BT_ERR("fwid len error");
        return -EINVAL;
    }
    upload_start_oob->fwid = net_buf_simple_pull_mem(buf, upload_start_oob->fwid_len);

    return 0;
}

static int bt_mesh_dfd_upload_set(struct bt_mesh_dfd_srv_t *srv, struct bt_mesh_msg_ctx *ctx,
    struct bt_mesh_dfd_upload_start_oob_t *upload_start_oob, struct bt_mesh_dfu_bank_t *bank)
{
    if (srv->upload.bank) {
        bt_mesh_dfu_bank_release(srv->upload.bank);
    }

    srv->upload.bank = bank;
    srv->upload.is_oob = true;
    if (memcpy_s(srv->upload.oob.uri,
        MYNEWT_VAL_BT_MESH_DFU_URI_MAX_LEN,
        upload_start_oob->uri,
        upload_start_oob->uri_len) != EOK) {
        return -EINVAL;
    }
    srv->upload.oob.uri_len = upload_start_oob->uri_len;
    if (memcpy_s(srv->upload.oob.current_fwid,
        MYNEWT_VAL_BT_MESH_DFU_FWID_MAX_LEN,
        upload_start_oob->fwid,
        upload_start_oob->fwid_len) != EOK) {
        return -EINVAL;
    }
    srv->upload.oob.current_fwid_len = upload_start_oob->fwid_len;
    memcpy_s(&srv->upload.oob.ctx, sizeof(struct bt_mesh_msg_ctx), ctx, sizeof(struct bt_mesh_msg_ctx));

    return 0;
}

static void bt_mesh_upload_start_oob_proc(struct bt_mesh_dfd_srv_t *srv, struct bt_mesh_msg_ctx *ctx)
{
    int status = srv->cb->start_oob_upload(srv, srv->upload.bank, (char *)srv->upload.oob.uri, srv->upload.oob.uri_len,
        srv->upload.oob.current_fwid, srv->upload.oob.current_fwid_len);
    if (status != BT_MESH_DFD_SUCCESS) {
        bt_mesh_upload_status_rsp_oob(srv, ctx, status);
        bt_mesh_dfu_bank_release(srv->upload.bank);
    } else {
        srv->upload.is_pending_oob_check = true;
        srv->upload.phase = BT_MESH_DFD_UPLOAD_PHASE_TRANSFER_ACTIVE;
        bt_mesh_upload_status_rsp_oob(srv, ctx, status);
    }
}

static int bt_mesh_dfd_handle_upload_start_oob(
    struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_dfd_srv_t *srv = mod->user_data;
    struct bt_mesh_dfd_upload_start_oob_t upload_start_oob = {0};
    int err = 0;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    err = bt_mesh_dfd_upload_start_oob_param_get(&upload_start_oob, buf);
    if (err) {
        BT_ERR("upload start param get error");
        return err;
    }

    if (bt_mesh_dfd_upload_is_busy(srv)) {
        if (srv->upload.is_oob && upload_start_oob.uri_len == srv->upload.oob.uri_len &&
            upload_start_oob.fwid_len == srv->upload.oob.current_fwid_len &&
            !memcmp(upload_start_oob.uri, srv->upload.oob.uri, upload_start_oob.uri_len) &&
            !memcmp(upload_start_oob.fwid, srv->upload.oob.current_fwid, upload_start_oob.fwid_len)) {
            bt_mesh_upload_status_rsp_oob(srv, ctx, BT_MESH_DFD_SUCCESS);
            return 0;
        }
        bt_mesh_upload_status_rsp_oob(srv, ctx, BT_MESH_DFD_ERR_BUSY_WITH_UPLOAD);
        return 0;
    } else if (srv->upload.is_oob && srv->upload.is_pending_oob_check) {
        return 0;
    }

    if (upload_start_oob.uri_len > MYNEWT_VAL_BT_MESH_DFU_URI_MAX_LEN ||
        upload_start_oob.fwid_len > MYNEWT_VAL_BT_MESH_DFU_FWID_MAX_LEN) {
        bt_mesh_upload_status_rsp_oob(srv, ctx, BT_MESH_DFD_ERR_INTERNAL);
        return 0;
    }

    struct bt_mesh_dfu_bank_t *bank = bt_mesh_dfu_bank_reserve();
    if (bank == NULL) {
        bt_mesh_upload_status_rsp_oob(srv, ctx, BT_MESH_DFD_ERR_INSUFFICIENT_RESOURCES);
        return 0;
    }

    err = bt_mesh_dfd_upload_set(srv, ctx, &upload_start_oob, bank);
    if (err) {
        BT_ERR("upload set error");
        return err;
    }

    bt_mesh_upload_start_oob_proc(srv, ctx);

    return 0;
}

static int bt_mesh_dfd_handle_upload_cancel(struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_dfd_srv_t *srv = mod->user_data;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    srv->upload.phase = BT_MESH_DFD_UPLOAD_PHASE_IDLE;

    if (srv->upload.is_oob) {
        srv->cb->cancel_oob_upload(srv, srv->upload.bank);
    } else {
        (void)bt_mesh_blob_server_cancel(&srv->upload.blob);
    }
    bt_mesh_dfd_upload_status_rsp(srv, ctx, BT_MESH_DFD_SUCCESS);

    return 0;
}

static void bt_mesh_dfd_fw_status_rsp(struct bt_mesh_dfd_srv_t *srv, struct bt_mesh_msg_ctx *ctx,
    enum bt_mesh_dfd_status_t status, uint16_t idx, const uint8_t *fwid, uint32_t fwid_len)
{
    struct os_mbuf *rsp =
        BT_MESH_MODEL_BUF(MODEL_OP(DFD_FW_STATUS), DFD_FW_STATUS_LEN + MYNEWT_VAL_BT_MESH_DFU_FWID_MAX_LEN);
    int bank_cnt = 0;
    int err = 0;

    bt_mesh_model_msg_init(rsp, MODEL_OP(DFD_FW_STATUS));
    net_buf_simple_add_u8(rsp, status);
    bank_cnt = bt_mesh_dfu_bank_count();
    net_buf_simple_add_le16(rsp, bank_cnt);
    net_buf_simple_add_le16(rsp, idx);
    if (fwid) {
        net_buf_simple_add_mem(rsp, fwid, fwid_len);
    }

    err = bt_mesh_model_send(srv->mod, ctx, rsp, NULL, NULL);
    if (err != BLE_MESH_SUCC) {
        BT_ERR("dfd fw status model_send() failed (err %d)", err);
    }
    os_mbuf_free_chain(rsp);
}

static int bt_mesh_dfd_handle_fw_get(struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_dfd_srv_t *srv = mod->user_data;
    struct bt_mesh_dfu_bank_t *bank = NULL;
    const uint8_t *fwid = NULL;
    uint32_t fwid_len = 0;
    int idx = 0;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    fwid_len = buf->om_len;
    fwid = net_buf_simple_pull_mem(buf, fwid_len);

    idx = bt_mesh_dfu_bank_get(fwid, fwid_len, &bank);
    if (idx >= 0) {
        bt_mesh_dfd_fw_status_rsp(srv, ctx, BT_MESH_DFD_SUCCESS, idx, fwid, fwid_len);
    } else {
        bt_mesh_dfd_fw_status_rsp(srv, ctx, BT_MESH_DFD_ERR_FW_NOT_FOUND, 0xffff, fwid, fwid_len);
    }

    return 0;
}

static int bt_mesh_dfd_handle_fw_get_by_index(
    struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_dfd_srv_t *srv = mod->user_data;
    const struct bt_mesh_dfu_bank_t *bank = NULL;
    uint16_t idx = 0;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    idx = net_buf_simple_pull_le16(buf);

    bank = bt_mesh_dfu_bank_get_by_idx(idx);
    if (bank) {
        bt_mesh_dfd_fw_status_rsp(srv, ctx, BT_MESH_DFD_SUCCESS, idx, bank->fwid, bank->fwid_len);
    } else {
        bt_mesh_dfd_fw_status_rsp(srv, ctx, BT_MESH_DFD_ERR_FW_NOT_FOUND, idx, NULL, 0);
    }

    return 0;
}

static int bt_mesh_dfd_handle_fw_del(struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_dfd_srv_t *srv = mod->user_data;
    const uint8_t *fwid = NULL;
    uint32_t fwid_len = 0;
    enum bt_mesh_dfd_status_t status = BT_MESH_DFD_SUCCESS;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    fwid_len = buf->om_len;
    fwid = net_buf_simple_pull_mem(buf, fwid_len);

    status = bt_mesh_dfd_srv_fw_del(srv, &fwid_len, &fwid);
    bt_mesh_dfd_fw_status_rsp(srv, ctx, status, 0xffff, fwid, fwid_len);

    return 0;
}

static enum bt_mesh_dfu_iter_t bt_mesh_dfd_bank_del_cb(const struct bt_mesh_dfu_bank_t *bank, void *user_data)
{
    struct bt_mesh_dfd_srv_t *srv = user_data;

    if (srv->cb && srv->cb->del) {
        srv->cb->del(srv, bank);
    }

    return BT_MESH_DFU_ITER_CONTINUE;
}

enum bt_mesh_dfd_status_t bt_mesh_dfd_srv_fw_del_all(struct bt_mesh_dfd_srv_t *srv)
{
    if (srv->phase != BT_MESH_DFD_PHASE_IDLE) {
        return BT_MESH_DFD_ERR_BUSY_WITH_DISTRIBUTION;
    }

    bt_mesh_dfu_bank_foreach(bt_mesh_dfd_bank_del_cb, srv);
    bt_mesh_dfu_bank_del_all();

    return BT_MESH_DFD_SUCCESS;
}

static int bt_mesh_dfd_handle_fw_del_all(struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_dfd_srv_t *srv = mod->user_data;
    enum bt_mesh_dfd_status_t status = BT_MESH_DFD_SUCCESS;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    status = bt_mesh_dfd_srv_fw_del_all(srv);
    bt_mesh_dfd_fw_status_rsp(srv, ctx, status, 0xffff, NULL, 0);

    return 0;
}

const struct bt_mesh_model_op bt_mesh_dfd_srv_op[] = {
    {MODEL_OP(DFD_RECEIVERS_GET), BT_MESH_LEN_EXACT(4), bt_mesh_dfd_handle_recv_get},
    {MODEL_OP(DFD_RECEIVERS_ADD), BT_MESH_LEN_MIN(3), bt_mesh_dfd_handle_recv_add},
    {MODEL_OP(DFD_RECEIVERS_DELETE_ALL), BT_MESH_LEN_EXACT(0), bt_mesh_dfd_handle_recv_del_all},
    {MODEL_OP(DFD_CAPABILITIES_GET), BT_MESH_LEN_EXACT(0), bt_mesh_dfd_handle_capabilities_get},
    {MODEL_OP(DFD_GET), BT_MESH_LEN_EXACT(0), bt_mesh_dfd_handle_get},
    {MODEL_OP(DFD_START), BT_MESH_LEN_MIN(10), bt_mesh_dfd_handle_start},
    {MODEL_OP(DFD_SUSPEND), BT_MESH_LEN_EXACT(0), bt_mesh_dfd_handle_suspend},
    {MODEL_OP(DFD_CANCEL), BT_MESH_LEN_EXACT(0), bt_mesh_dfd_handle_cancel},
    {MODEL_OP(DFD_APPLY), BT_MESH_LEN_EXACT(0), bt_mesh_dfd_handle_apply},
    {MODEL_OP(DFD_UPLOAD_GET), BT_MESH_LEN_EXACT(0), bt_mesh_dfd_handle_upload_get},
    {MODEL_OP(DFD_UPLOAD_START), BT_MESH_LEN_MIN(16), bt_mesh_dfd_handle_upload_start},
    {MODEL_OP(DFD_UPLOAD_START_OOB), BT_MESH_LEN_MIN(2), bt_mesh_dfd_handle_upload_start_oob},
    {MODEL_OP(DFD_UPLOAD_CANCEL), BT_MESH_LEN_EXACT(0), bt_mesh_dfd_handle_upload_cancel},
    {MODEL_OP(DFD_FW_GET), BT_MESH_LEN_MIN(0), bt_mesh_dfd_handle_fw_get},
    {MODEL_OP(DFD_FW_GET_BY_INDEX), BT_MESH_LEN_EXACT(2), bt_mesh_dfd_handle_fw_get_by_index},
    {MODEL_OP(DFD_FW_DELETE), BT_MESH_LEN_MIN(0), bt_mesh_dfd_handle_fw_del},
    {MODEL_OP(DFD_FW_DELETE_ALL), BT_MESH_LEN_EXACT(0), bt_mesh_dfd_handle_fw_del_all},
    BT_MESH_MODEL_OP_END
};

static void bt_mesh_dfd_phase_set(struct bt_mesh_dfd_srv_t *srv, enum bt_mesh_dfd_phase_t phase)
{
    srv->phase = phase;

    if (srv->cb && srv->cb->phase) {
        srv->cb->phase(srv, srv->phase);
    }
}

static void bt_mesh_dfu_suspended(struct bt_mesh_dfu_client_t *cli)
{
    struct bt_mesh_dfd_srv_t *srv = CONTAINER_OF(cli, struct bt_mesh_dfd_srv_t, dfu);

    bt_mesh_dfd_phase_set(srv, BT_MESH_DFD_PHASE_TRANSFER_SUSPENDED);
}

static void bt_mesh_dfu_ended(struct bt_mesh_dfu_client_t *cli, enum bt_mesh_dfu_status_t reason)
{
    struct bt_mesh_dfd_srv_t *srv = CONTAINER_OF(cli, struct bt_mesh_dfd_srv_t, dfu);
    int err = 0;

    BT_INFO("reason: %u, phase: %u, apply: %u", reason, srv->phase, srv->apply);

    if (srv->phase == BT_MESH_DFD_PHASE_IDLE) {
        return;
    }

    if (srv->phase == BT_MESH_DFD_PHASE_CANCELING_UPDATE) {
        bt_mesh_dfd_phase_set(srv, BT_MESH_DFD_PHASE_IDLE);
        return;
    }

    if (reason != BT_MESH_DFU_SUCCESS) {
        bt_mesh_dfd_phase_set(srv, BT_MESH_DFD_PHASE_FAILED);
        return;
    }

    if (!srv->apply) {
        bt_mesh_dfd_phase_set(srv, BT_MESH_DFD_PHASE_TRANSFER_SUCCESS);
        return;
    }

    bt_mesh_dfd_phase_set(srv, BT_MESH_DFD_PHASE_APPLYING_UPDATE);

    err = bt_mesh_dfu_client_apply(cli);
    if (err) {
        BT_ERR("dfu apply failed: %d", err);
        bt_mesh_dfd_phase_set(srv, BT_MESH_DFD_PHASE_FAILED);
    }
}

static void bt_mesh_dfu_applied(struct bt_mesh_dfu_client_t *cli)
{
    struct bt_mesh_dfd_srv_t *srv = CONTAINER_OF(cli, struct bt_mesh_dfd_srv_t, dfu);
    int err = 0;

    if (srv->phase == BT_MESH_DFD_PHASE_CANCELING_UPDATE) {
        bt_mesh_dfd_phase_set(srv, BT_MESH_DFD_PHASE_FAILED);
        return;
    }

    if (srv->phase != BT_MESH_DFD_PHASE_APPLYING_UPDATE) {
        return;
    }

    err = bt_mesh_dfu_client_confirm(cli);
    if (err) {
        BT_ERR("dfu confirm failed: %d", err);
        bt_mesh_dfd_phase_set(srv, BT_MESH_DFD_PHASE_FAILED);
    }
}

static void bt_mesh_dfu_confirmed(struct bt_mesh_dfu_client_t *cli)
{
    struct bt_mesh_dfd_srv_t *srv = CONTAINER_OF(cli, struct bt_mesh_dfd_srv_t, dfu);
    int err = 0;

    if (srv->phase != BT_MESH_DFD_PHASE_APPLYING_UPDATE && srv->phase != BT_MESH_DFD_PHASE_CANCELING_UPDATE) {
        return;
    }

    bt_mesh_dfd_phase_set(srv, BT_MESH_DFD_PHASE_COMPLETED);
    sys_slist_init(&srv->inputs.targets);
    srv->target_cnt = 0;

    if (srv->dist_self == true) {
        srv->dist_self = false;
        bt_mesh_ota_result_print();
        err = bt_mesh_upagrade_apply();
        if (err) {
            BT_ERR("dist apply error");
        }
    }
}

const struct bt_mesh_dfu_client_cb bt_mesh_dfd_srv_dfu_cb = {
    .on_suspended = bt_mesh_dfu_suspended,
    .on_ended = bt_mesh_dfu_ended,
    .on_applied = bt_mesh_dfu_applied,
    .on_confirmed = bt_mesh_dfu_confirmed,
};

static int bt_mesh_dfd_blob_upload_start(
    struct bt_mesh_blob_server_t *b, struct bt_mesh_msg_ctx *ctx, struct bt_mesh_blob_transfer_t *transfer)
{
    BT_INFO("blob server upload start");
    return 0;
}

static void bt_mesh_dfd_blob_upload_end(struct bt_mesh_blob_server_t *b, uint64_t id, bool success)
{
    struct bt_mesh_dfd_srv_t *srv = CONTAINER_OF(b, struct bt_mesh_dfd_srv_t, upload.blob);
    ota_node_role_take(OTA_NODE_ROLE_NONE);

    if (srv->upload.phase != BT_MESH_DFD_UPLOAD_PHASE_TRANSFER_ACTIVE) {
        return;
    }

    if (success && (bt_mesh_dfu_bank_commit(srv->upload.bank) == 0)) {
        srv->upload.phase = BT_MESH_DFD_UPLOAD_PHASE_TRANSFER_SUCCESS;
        return;
    }

    srv->upload.phase = BT_MESH_DFD_UPLOAD_PHASE_TRANSFER_ERROR;
}

static void bt_mesh_dfd_blob_upload_timeout(struct bt_mesh_blob_server_t *b)
{
    BT_INFO("blob server upload timeout");
    ota_node_role_take(OTA_NODE_ROLE_NONE);

    bt_mesh_dfd_blob_upload_end(b, b->state.transfer.id, false);
}

const struct bt_mesh_blob_server_cb_t bt_mesh_dfd_srv_blob_cb = {
    .on_start = bt_mesh_dfd_blob_upload_start,
    .on_end = bt_mesh_dfd_blob_upload_end,
    .on_suspended = bt_mesh_dfd_blob_upload_timeout,
};

static int bt_mesh_dfd_srv_deinit(struct bt_mesh_model *mod)
{
    if ((mod->pub != NULL) && (mod->pub->msg != NULL)) {
        os_mbuf_free(mod->pub->msg);
        mod->pub->msg = NULL;
    }

    return 0;
}

static int bt_mesh_dfd_srv_init(struct bt_mesh_model *mod)
{
    int err = 0;
    struct bt_mesh_dfd_srv_t *srv = mod->user_data;
    const struct bt_mesh_model *blob_srv = NULL;
    const struct bt_mesh_model *dfu_cli = NULL;

    srv->mod = mod;

    blob_srv = bt_mesh_model_find(bt_mesh_model_elem(mod), BT_MESH_MODEL_ID_BLOB_SRV);
    if (blob_srv == NULL) {
        BT_ERR("blob server model find error.");
        return -EINVAL;
    }

    dfu_cli = bt_mesh_model_find(bt_mesh_model_elem(mod), BT_MESH_MODEL_ID_DFU_CLI);
    if (dfu_cli == NULL) {
        BT_ERR("dfu client model find error.");
        return -EINVAL;
    }

    err = bt_mesh_model_extend(mod, srv->upload.blob.mod);
    if (err) {
        BT_ERR("model extend error %d", err);
        return err;
    }

    return 0;
}

static void bt_mesh_dfd_srv_reset(struct bt_mesh_model *mod)
{
    struct bt_mesh_dfd_srv_t *srv = mod->user_data;

    bt_mesh_dfd_phase_set(srv, BT_MESH_DFD_PHASE_IDLE);
    srv->upload.phase = BT_MESH_DFD_UPLOAD_PHASE_IDLE;

    sys_slist_init(&srv->inputs.targets);
    srv->target_cnt = 0;

    bt_mesh_dfu_bank_foreach(bt_mesh_dfd_bank_del_cb, srv);
    bt_mesh_dfu_bank_del_all();
}

const struct bt_mesh_model_cb bt_mesh_dfd_srv_cb = {
    .init = bt_mesh_dfd_srv_init,
    .reset = bt_mesh_dfd_srv_reset,
    .deinit = bt_mesh_dfd_srv_deinit,
};

enum bt_mesh_dfd_status_t bt_mesh_dfd_srv_recv_del_all(struct bt_mesh_dfd_srv_t *srv)
{
    if (bt_mesh_dfu_client_is_busy(&srv->dfu)) {
        return BT_MESH_DFD_ERR_BUSY_WITH_DISTRIBUTION;
    }

    if (srv->phase != BT_MESH_DFD_PHASE_IDLE && srv->phase != BT_MESH_DFD_PHASE_COMPLETED &&
        srv->phase != BT_MESH_DFD_PHASE_FAILED) {
        return BT_MESH_DFD_ERR_BUSY_WITH_DISTRIBUTION;
    }

    sys_slist_init(&srv->inputs.targets);
    srv->target_cnt = 0;

    return BT_MESH_DFD_SUCCESS;
}

static void bt_mesh_dfd_srv_ctx_set(struct bt_mesh_dfd_srv_t *srv, struct bt_mesh_dfd_start_params *params)
{
    int i = 0;

    sys_slist_init(&srv->inputs.targets);
    for (i = 0; i < srv->target_cnt; i++) {
        uint16_t addr = srv->targets[i].blob.addr;

        memset_s(&srv->targets[i].blob, sizeof(struct bt_mesh_blob_target_t), 0,
            sizeof(struct bt_mesh_blob_target_t));
#if MYNEWT_VAL(BT_MESH_OTA_PULL)
        memset_s(&srv->pull_ctxs[i], sizeof(struct bt_mesh_blob_target_pull_t), 0,
            sizeof(struct bt_mesh_blob_target_pull_t));
        srv->targets[i].blob.pull = &srv->pull_ctxs[i];
#endif
        srv->targets[i].blob.addr = addr;
        sys_slist_append(&srv->inputs.targets, &srv->targets[i].blob.n);
    }

    srv->bank_idx = params->bank_idx;
    srv->inputs.app_idx = params->app_idx;
    srv->inputs.timeout_base = params->timeout_base;
    srv->inputs.group = params->group;
    srv->inputs.ttl = params->ttl;
    srv->apply = params->apply;

    BT_INFO("dfd start: bank: %d, appidx: %d, tb: %d, addr: %04X, ttl: %d, apply: %d",
        params->bank_idx, params->app_idx, params->timeout_base,
        params->group, params->ttl, params->apply);
}

static bool bt_mesh_dfd_is_busy(const struct bt_mesh_dfd_srv_t *srv)
{
    return srv->phase == BT_MESH_DFD_PHASE_TRANSFER_ACTIVE || srv->phase == BT_MESH_DFD_PHASE_TRANSFER_SUCCESS ||
           srv->phase == BT_MESH_DFD_PHASE_APPLYING_UPDATE;
}

static bool bt_mesh_dfd_srv_check(
    struct bt_mesh_dfd_srv_t *srv, struct bt_mesh_dfd_start_params *params, enum bt_mesh_dfd_status_t *status)
{
    if (srv->inputs.app_idx == params->app_idx && srv->inputs.timeout_base == params->timeout_base &&
        srv->inputs.group == params->group && srv->inputs.ttl == params->ttl && srv->bank_idx == params->bank_idx &&
        srv->dfu.transfer.blob.mode == params->transfer_mode && srv->apply == params->apply) {
        if (bt_mesh_dfd_is_busy(srv)) {
            BT_INFO("dfd is busy, phase=%u", srv->phase);
            *status = BT_MESH_DFD_SUCCESS;
            return false;
        }
        if (srv->phase == BT_MESH_DFD_PHASE_TRANSFER_SUSPENDED) {
            bt_mesh_dfu_client_resume(&srv->dfu);
            bt_mesh_dfd_phase_set(srv, BT_MESH_DFD_PHASE_TRANSFER_ACTIVE);
            *status = BT_MESH_DFD_SUCCESS;
            return false;
        }
    } else if (bt_mesh_dfd_is_busy(srv) || srv->phase == BT_MESH_DFD_PHASE_TRANSFER_SUSPENDED) {
        BT_WARN("dfd is busy or dfd phase suspend");
        *status = BT_MESH_DFD_ERR_BUSY_WITH_DISTRIBUTION;
        return false;
    }

    if (srv->phase == BT_MESH_DFD_PHASE_CANCELING_UPDATE) {
        BT_WARN("dfd phase cancel");
        *status = BT_MESH_DFD_ERR_BUSY_WITH_DISTRIBUTION;
        return false;
    }
    return true;
}

enum bt_mesh_dfd_status_t bt_mesh_dfd_srv_start(
    struct bt_mesh_dfd_srv_t *srv, struct bt_mesh_dfd_start_params *params, struct bt_mesh_msg_ctx *ctx)
{
    int err = 0;
    struct bt_mesh_dfu_client_transfer_t transfer = {0};
    enum bt_mesh_dfd_status_t status = BT_MESH_DFD_SUCCESS;
    bool ret = false;

    if (srv->target_cnt == 0) {
        status = BT_MESH_DFD_ERR_RECEIVERS_LIST_EMPTY;
        goto rsp;
    }
    if (!bt_mesh_app_key_exists(params->app_idx)) {
        status = BT_MESH_DFD_ERR_INVALID_APPKEY_INDEX;
        goto rsp;
    }

    transfer.mode = params->transfer_mode;
    transfer.bank = bt_mesh_dfu_bank_get_by_idx(params->bank_idx);
    if (!transfer.bank) {
        status = BT_MESH_DFD_ERR_FW_NOT_FOUND;
        goto rsp;
    }

    ret = bt_mesh_dfd_srv_check(srv, params, &status);
    if (!ret) {
        goto rsp;
    }

    srv->stream = NULL;
    err = srv->cb->send(srv, transfer.bank, &srv->stream);
    if (err || !srv->stream) {
        status = BT_MESH_DFD_ERR_INTERNAL;
        goto rsp;
    }
    bt_mesh_dfd_srv_ctx_set(srv, params);
    transfer.blob_params = NULL;

    bt_mesh_dfd_phase_set(srv, BT_MESH_DFD_PHASE_TRANSFER_ACTIVE);
    srv->dfu.transfer.bank = transfer.bank;
    srv->dfu.transfer.blob.mode = transfer.mode;
    bt_mesh_dfd_status_rsp(srv, ctx, BT_MESH_DFD_SUCCESS);

    transfer.blob_id = DFU_BLOB_ID_DEFAULT;
    err = bt_mesh_dfu_client_send(&srv->dfu, &srv->inputs, srv->stream, &transfer);
    if (err) {
        bt_mesh_dfd_phase_set(srv, BT_MESH_DFD_PHASE_IDLE);
        BT_ERR("dfu_client_send error %d", err);
        return BT_MESH_DFD_ERR_INTERNAL;
    }

    return BT_MESH_DFD_SUCCESS;
rsp:
    bt_mesh_dfd_status_rsp(srv, ctx, status);
    return status;
}

enum bt_mesh_dfd_status_t bt_mesh_dfd_srv_suspend(struct bt_mesh_dfd_srv_t *srv)
{
    int err = 0;

    if (srv->phase == BT_MESH_DFD_PHASE_TRANSFER_SUSPENDED) {
        return BT_MESH_DFD_SUCCESS;
    }

    if (srv->phase != BT_MESH_DFD_PHASE_TRANSFER_ACTIVE) {
        return BT_MESH_DFD_ERR_WRONG_PHASE;
    }

    err = bt_mesh_dfu_client_suspend(&srv->dfu);
    if (err) {
        return BT_MESH_DFD_ERR_SUSPEND_FAILED;
    }

    srv->phase = BT_MESH_DFD_PHASE_TRANSFER_SUSPENDED;
    return BT_MESH_DFD_SUCCESS;
}

enum bt_mesh_dfd_status_t bt_mesh_dfd_srv_cancel(struct bt_mesh_dfd_srv_t *srv, struct bt_mesh_msg_ctx *ctx)
{
    enum bt_mesh_dfd_phase_t phase = BT_MESH_DFD_PHASE_IDLE;
    int err = 0;

    if (srv->phase == BT_MESH_DFD_PHASE_CANCELING_UPDATE || srv->phase == BT_MESH_DFD_PHASE_IDLE) {
        if (ctx != NULL) {
            bt_mesh_dfd_status_rsp(srv, ctx, BT_MESH_DFD_SUCCESS);
        }
        return BT_MESH_DFD_SUCCESS;
    }

    if (srv->phase == BT_MESH_DFD_PHASE_COMPLETED || srv->phase == BT_MESH_DFD_PHASE_FAILED) {
        bt_mesh_dfd_phase_set(srv, BT_MESH_DFD_PHASE_IDLE);
        if (ctx != NULL) {
            bt_mesh_dfd_status_rsp(srv, ctx, BT_MESH_DFD_SUCCESS);
        }
        return BT_MESH_DFD_SUCCESS;
    }

    phase = srv->phase;
    bt_mesh_dfd_phase_set(srv, BT_MESH_DFD_PHASE_CANCELING_UPDATE);
    err = bt_mesh_dfu_client_cancel(&srv->dfu, NULL);
    if (err) {
        if (ctx != NULL) {
            bt_mesh_dfd_status_rsp(srv, ctx, BT_MESH_DFD_ERR_INTERNAL);
        }
        return BT_MESH_DFD_ERR_INTERNAL;
    }

    if (ctx != NULL) {
        bt_mesh_dfd_status_rsp(srv, ctx, BT_MESH_DFD_SUCCESS);
    }

    if (phase == BT_MESH_DFD_PHASE_APPLYING_UPDATE) {
        bt_mesh_dfd_phase_set(srv, BT_MESH_DFD_PHASE_IDLE);
        if (ctx != NULL) {
            bt_mesh_dfd_status_rsp(srv, ctx, BT_MESH_DFD_SUCCESS);
        }
    }

    return BT_MESH_DFD_SUCCESS;
}

enum bt_mesh_dfd_status_t bt_mesh_dfd_srv_apply(struct bt_mesh_dfd_srv_t *srv)
{
    int err = 0;

    if (srv->phase == BT_MESH_DFD_PHASE_APPLYING_UPDATE || srv->phase == BT_MESH_DFD_PHASE_COMPLETED) {
        return BT_MESH_DFD_SUCCESS;
    }

    if (srv->phase != BT_MESH_DFD_PHASE_TRANSFER_SUCCESS) {
        return BT_MESH_DFD_ERR_WRONG_PHASE;
    }

    err = bt_mesh_dfu_client_apply(&srv->dfu);
    if (err) {
        return BT_MESH_DFD_ERR_INTERNAL;
    }

    bt_mesh_dfd_phase_set(srv, BT_MESH_DFD_PHASE_APPLYING_UPDATE);
    return BT_MESH_DFD_SUCCESS;
}

static int bt_mesh_dfd_bank_del(struct bt_mesh_dfd_srv_t *srv, const struct bt_mesh_dfu_bank_t *bank)
{
    if (srv->cb && srv->cb->del) {
        srv->cb->del(srv, bank);
    }

    return bt_mesh_dfu_bank_del(bank);
}

enum bt_mesh_dfd_status_t bt_mesh_dfd_srv_fw_del(
    struct bt_mesh_dfd_srv_t *srv, uint32_t *fwid_len, const uint8_t **fwid)
{
    struct bt_mesh_dfu_bank_t *bank = NULL;
    int idx = 0, err = 0;

    if (srv->phase != BT_MESH_DFD_PHASE_IDLE) {
        *fwid = NULL;
        *fwid_len = 0;
        return BT_MESH_DFD_ERR_BUSY_WITH_DISTRIBUTION;
    }

    idx = bt_mesh_dfu_bank_get(*fwid, *fwid_len, &bank);
    if (idx < 0) {
        return BT_MESH_DFD_SUCCESS;
    }

    err = bt_mesh_dfd_bank_del(srv, bank);
    if (err) {
        *fwid = NULL;
        *fwid_len = 0;
        return BT_MESH_DFD_ERR_INTERNAL;
    } else {
        return BT_MESH_DFD_SUCCESS;
    }
}
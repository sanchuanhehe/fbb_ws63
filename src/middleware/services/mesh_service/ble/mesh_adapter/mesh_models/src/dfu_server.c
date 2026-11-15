/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: DFU Model Implementation.
 *
 * History:
 * 2025-05-01, Create file.
 */

#include "syscfg/syscfg.h"
#include "os/os_mbuf.h"
#include "mesh/glue.h"
#include "mesh/mesh.h"
#include "model/model_opcode.h"
#include "model/model_def.h"
#include "model_common.h"
#include "blob.h"
#include "access.h"
#include "ble_mesh_nv_op.h"
#include "mesh_ota_common.h"
#include "ble_mesh_ota.h"
#include "ble_mesh_ota_inner.h"
#include "model/dfu_server.h"

#define UPDATE_IDX_NONE 0xff
#define DFU_SERVER_STATUS_CHECK_OK 1

BUILD_ASSERT((DFU_UPDATE_START_MSG_MAXLEN + BT_MESH_MODEL_OP_LEN(MODEL_OP(DFU_UPDATE_START)) +
          BT_MESH_MIC_SHORT) <= BT_MESH_RX_SDU_MAX,
         "The Firmware Update Start message does not fit into the maximum incoming SDU size.");

BUILD_ASSERT((DFU_UPDATE_INFO_STATUS_MSG_MINLEN +
          BT_MESH_MODEL_OP_LEN(MODEL_OP(DFU_UPDATE_INFO_STATUS)) + BT_MESH_MIC_SHORT) <= BT_MESH_TX_SDU_MAX,
         "The Firmware Update Info Status message does not fit into the maximum outgoing SDU "
         "size.");

static bool dfu_verify_fail = false;
static bool dfu_apply_stop = false;
static uint32_t target_fw_ver_curr = 0x01000001;  // 01:type 000001:version
static uint32_t target_fw_ver_new;
static struct bt_mesh_dfu_img_t dfu_imgs[] = {{
    .fwid = &target_fw_ver_curr,
    .fwid_len = sizeof(target_fw_ver_curr),
}};

static int target_metadata_check(struct bt_mesh_dfu_srv_t *srv, const struct bt_mesh_dfu_img_t *img,
    struct os_mbuf *metadata_raw, enum bt_mesh_dfu_effect_t *effect)
{
    BT_INFO("target_metadata_check");
    if (metadata_raw->om_len > 0) {
        return ble_mesh_ota_metadata_check(metadata_raw->om_data, metadata_raw->om_len) ? 0 : -1;
    }
    
    return 0;
}

static void target_dfu_transfer_end(struct bt_mesh_dfu_srv_t *srv, const struct bt_mesh_dfu_img_t *img, bool success)
{
    BT_INFO("target_dfu_transfer_end");
    if (!success) {
        return;
    }

    if (dfu_verify_fail) {
        bt_mesh_dfu_srv_rejected(srv);
        dfu_verify_fail = false;
    } else {
        bt_mesh_dfu_srv_verified(srv);
    }
}

static int target_dfu_start(struct bt_mesh_dfu_srv_t *srv, const struct bt_mesh_dfu_img_t *img,
    struct os_mbuf *metadata, const struct bt_mesh_blob_stream_t **srteam)
{
    BT_INFO("target_dfu_start");

    *srteam = bt_mesh_get_target_blob_stream();

    return 0;
}

static int target_dfu_apply(struct bt_mesh_dfu_srv_t *srv, const struct bt_mesh_dfu_img_t *img)
{
    if (srv && (srv->update.phase == BT_MESH_DFU_PHASE_APPLYING) && (dfu_apply_stop == true)) {
        dfu_apply_stop = false;
        return 0;
    }

    ble_mesh_ota_upgrade_status_report(OTA_UPGRADE_END);
    bt_mesh_dfu_srv_applied(srv);  // 升级前准备，需要添加

    return bt_mesh_upagrade_apply();
}

static int target_dfu_recover(
    struct bt_mesh_dfu_srv_t *srv, const struct bt_mesh_dfu_img_t *img, const struct bt_mesh_blob_stream_t **srteam)
{
    BT_INFO("target_dfu_recover");

    *srteam = bt_mesh_get_target_blob_stream();

    return 0;
}

static const struct bt_mesh_dfu_srv_cb dfu_srv_cb = {
    .check = target_metadata_check,
    .start = target_dfu_start,
    .end = target_dfu_transfer_end,
    .apply = target_dfu_apply,
    .recover = target_dfu_recover,
};

struct bt_mesh_dfu_srv_t g_dfu_srv = BT_MESH_DFU_SRV_INIT(&dfu_srv_cb, dfu_imgs, ARRAY_SIZE(dfu_imgs));

static inline uint16_t dfu_metadata_checksum(struct os_mbuf *buf)
{
    struct net_buf_simple_state state;
    uint8_t meta_checksum[2] = {0, 0};

    net_buf_simple_save(buf, &state);

    while (buf->om_len) {
        uint8_t byte = net_buf_simple_pull_u8(buf);
        meta_checksum[0] += byte;
        meta_checksum[1] += meta_checksum[0];
    }

    net_buf_simple_restore(buf, &state);

    return (meta_checksum[0] << 8U) | meta_checksum[1];
}


static void dfu_server_store_state(struct bt_mesh_dfu_srv_t *srv)
{
    if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
        bt_mesh_model_data_store(srv->mod, false, NULL, &srv->update, sizeof(srv->update));
    }
}

static void dfu_server_erase_state(struct bt_mesh_dfu_srv_t *srv)
{
    if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
        bt_mesh_model_data_store(srv->mod, false, NULL, NULL, BT_MESH_DFU_DATE_SIZE_0);
    }
}

static void transfer_failed(struct bt_mesh_dfu_srv_t *srv)
{
    if (bt_mesh_dfu_srv_is_busy(srv) && srv->update.idx < srv->img_count) {
        dfu_server_erase_state(srv);

        if (srv->cb->end) {
            srv->cb->end(srv, &srv->imgs[srv->update.idx], false);
        }
    }
}

static enum bt_mesh_dfu_status_t metadata_check(
    struct bt_mesh_dfu_srv_t *srv, uint8_t idx, struct os_mbuf *buf, enum bt_mesh_dfu_effect_t *effect)
{
    *effect = BT_MESH_DFU_EFFECT_NONE;

    if (idx >= srv->img_count) {
        return BT_MESH_DFU_ERR_FW_IDX;
    }

    if (!srv->cb->check) {
        return BT_MESH_DFU_SUCCESS;
    }

    if (srv->cb->check(srv, &srv->imgs[idx], buf, effect)) {
        *effect = BT_MESH_DFU_EFFECT_NONE;
        return BT_MESH_DFU_ERR_METADATA;
    }

    return BT_MESH_DFU_SUCCESS;
}

static void apply_send_end(int err, void *cb_params)
{
    struct bt_mesh_dfu_srv_t *srv = cb_params;
    int check = 0;

    if (err) {
        srv->update.phase = BT_MESH_DFU_PHASE_VERIFY_OK; // apply end失败后，重新把状态重置为OK，等待下一次重发
        BT_WARN("Apply response failed, wait for retry (err %d)", err);
        return;
    }

    if (!srv->cb->apply || srv->update.idx == UPDATE_IDX_NONE) {
        srv->update.phase = BT_MESH_DFU_PHASE_IDLE;
        dfu_server_store_state(srv);
        BT_INFO("Prerequisites for apply callback are wrong");
        return;
    }

    dfu_server_store_state(srv);

    check = srv->cb->apply(srv, &srv->imgs[srv->update.idx]);
    if (check) {
        srv->update.phase = BT_MESH_DFU_PHASE_IDLE;
        dfu_server_store_state(srv);
        BT_INFO("Application apply callback failed (err %d)", check);
    }
}

static void apply_send_start(uint16_t duration, int err, void *cb_params)
{
    if (err) {
        apply_send_end(err, cb_params);
    }
}

static void dfu_server_img_verify(struct bt_mesh_dfu_srv_t *srv)
{
    srv->update.phase = BT_MESH_DFU_PHASE_VERIFY;
    if (srv->update.idx >= srv->img_count) {
        bt_mesh_dfu_srv_rejected(srv);
        return;
    }

    if (!srv->cb->end) {
        bt_mesh_dfu_srv_verified(srv);
        return;
    }

    srv->cb->end(srv, &srv->imgs[srv->update.idx], true);
    if (srv->update.phase == BT_MESH_DFU_PHASE_VERIFY) {
        dfu_server_store_state(srv);
    }
}

static int bt_mesh_dfu_handle_info_get(struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx,
                                       struct os_mbuf *buf)
{
    struct bt_mesh_dfu_srv_t *srv = mod->user_data;
    uint8_t idx = 0;
    uint8_t limit = 0;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    if (srv->update.phase == BT_MESH_DFU_PHASE_APPLYING) {
        BT_INFO("Still applying, not responding");
        return -EBUSY;
    }

    idx = net_buf_simple_pull_u8(buf);
    limit = net_buf_simple_pull_u8(buf);

    BT_INFO("from %u (limit: %u)", idx, limit);

    struct os_mbuf *rsp = BT_MESH_MODEL_BUF(MODEL_OP(DFU_UPDATE_INFO_STATUS), BT_MESH_TX_SDU_MAX);
    bt_mesh_model_msg_init(rsp, MODEL_OP(DFU_UPDATE_INFO_STATUS));
    net_buf_simple_add_u8(rsp, srv->img_count);
    net_buf_simple_add_u8(rsp, idx);

    for (; idx < srv->img_count && limit > 0; ++idx) {
        uint32_t entry_len;

        if (!srv->imgs[idx].fwid) {
            continue;
        }

        entry_len = BT_MESH_DFU_ENTRY_HEAD_LEN + srv->imgs[idx].fwid_len;
        if (srv->imgs[idx].uri) {
            entry_len += strlen(srv->imgs[idx].uri);
        }

        if (net_buf_simple_tailroom(rsp) + BT_MESH_MIC_SHORT < (int64_t)entry_len) {
            break;
        }

        net_buf_simple_add_u8(rsp, srv->imgs[idx].fwid_len);
        net_buf_simple_add_mem(rsp, srv->imgs[idx].fwid, srv->imgs[idx].fwid_len);

        if (srv->imgs[idx].uri) {
            uint32_t len = strlen(srv->imgs[idx].uri);

            net_buf_simple_add_u8(rsp, len);
            net_buf_simple_add_mem(rsp, srv->imgs[idx].uri, len);
        } else {
            net_buf_simple_add_u8(rsp, 0);
        }

        limit--;
    }

    if (srv->update.phase != BT_MESH_DFU_PHASE_IDLE) {
        ctx->send_ttl = srv->update.ttl;
    }

    bt_mesh_model_send(mod, ctx, rsp, NULL, NULL);
    os_mbuf_free_chain(rsp);

    return 0;
}

static int bt_mesh_dfu_handle_metadata_check(struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx,
                                             struct os_mbuf *buf)
{
    struct bt_mesh_dfu_srv_t *srv = mod->user_data;
    enum bt_mesh_dfu_status_t status;
    enum bt_mesh_dfu_effect_t effect;
    uint8_t idx = 0;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    struct os_mbuf *rsp = BT_MESH_MODEL_BUF(MODEL_OP(DFU_UPDATE_METADATA_STATUS), BT_MESH_DFU_UPDATE_METADATA_LEN);
    bt_mesh_model_msg_init(rsp, MODEL_OP(DFU_UPDATE_METADATA_STATUS));

    idx = net_buf_simple_pull_u8(buf);
    status = metadata_check(srv, idx, buf, &effect);

    BT_INFO("%u", idx);

    net_buf_simple_add_u8(rsp, (status & BIT_MASK(BT_MESH_DFU_STATUS_MASK)) | (effect << BT_MESH_DFU_STATUS_MASK));
    net_buf_simple_add_u8(rsp, idx);

    if (srv->update.phase != BT_MESH_DFU_PHASE_IDLE) {
        ctx->send_ttl = srv->update.ttl;
    }

    bt_mesh_model_send(mod, ctx, rsp, NULL, NULL);
    os_mbuf_free_chain(rsp);

    return 0;
}

static void update_status_rsp(struct bt_mesh_dfu_srv_t *srv,
                              struct bt_mesh_msg_ctx *ctx,
                              enum bt_mesh_dfu_status_t status,
                              const struct bt_mesh_send_cb *send_cb)
{
    struct os_mbuf *buf = BT_MESH_MODEL_BUF(MODEL_OP(DFU_UPDATE_STATUS), BT_MESH_DFU_UPDATE_STATUS_LEN);
    bt_mesh_model_msg_init(buf, MODEL_OP(DFU_UPDATE_STATUS));

    net_buf_simple_add_u8(buf, ((status & BIT_MASK(BT_MESH_DFU_STATUS_MASK)) |
                          (srv->update.phase << BT_MESH_DFU_UPDATE_PHASE)));

    if (srv->update.phase != BT_MESH_DFU_PHASE_IDLE) {
        net_buf_simple_add_u8(buf, srv->update.ttl);
        net_buf_simple_add_u8(buf, srv->update.effect);
        net_buf_simple_add_le16(buf, srv->update.timeout_base);
        net_buf_simple_add_le32(buf, (uint32_t)srv->blob.state.transfer.id);
        net_buf_simple_add_le32(buf, (uint32_t)(srv->blob.state.transfer.id >> DFU_SRV_UINT64_ADD_OFFSET));
        net_buf_simple_add_u8(buf, srv->update.idx);

        ctx->send_ttl = srv->update.ttl;
    }

    bt_mesh_model_send(srv->mod, ctx, buf, send_cb, srv);
    os_mbuf_free_chain(buf);
}

static int bt_mesh_dfu_handle_get(struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx,
                                  struct os_mbuf *buf)
{
    struct bt_mesh_dfu_srv_t *srv = mod->user_data;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    update_status_rsp(srv, ctx, BT_MESH_DFU_SUCCESS, NULL);

    return 0;
}

static inline bool is_active_update(struct bt_mesh_dfu_srv_t *srv, uint8_t idx, uint16_t timeout_base,
                                    const uint64_t *blob_id, uint8_t ttl, uint16_t meta_checksum)
{
    return (srv->update.idx != idx || srv->blob.state.transfer.id != *blob_id ||
            srv->update.ttl != ttl || srv->update.timeout_base != timeout_base ||
            srv->update.meta != meta_checksum);
}

static void bt_mesh_dfu_get_handle_param(struct bt_mesh_dfu_handle_param_t *dfu_param, struct os_mbuf *buf)
{
    dfu_param->ttl = net_buf_simple_pull_u8(buf);
    dfu_param->timeout_base = net_buf_simple_pull_le16(buf);
    dfu_param->blob_id = net_buf_simple_pull_le32(buf);
    dfu_param->blob_id |= ((uint64_t)net_buf_simple_pull_le32(buf) << DFU_SRV_UINT64_ADD_OFFSET);
    dfu_param->idx = net_buf_simple_pull_u8(buf);
    dfu_param->meta_checksum = dfu_metadata_checksum(buf);

    BT_INFO("BLOB ID = %llu %u ttl: %u extra time: %u",
        dfu_param->blob_id,
        dfu_param->idx,
        dfu_param->ttl,
        dfu_param->timeout_base);
}

static int bt_mesh_dfu_check_normal_status(struct bt_mesh_dfu_handle_param_t *dfu_param, struct os_mbuf *buf,
    struct bt_mesh_dfu_srv_t *srv, struct bt_mesh_msg_ctx *ctx)
{
    enum bt_mesh_dfu_status_t status;

    if ((!buf->om_len || dfu_param->meta_checksum == srv->update.meta) &&
        srv->update.phase == BT_MESH_DFU_PHASE_TRANSFER_ERR && srv->update.ttl == dfu_param->ttl &&
        srv->update.timeout_base == dfu_param->timeout_base && srv->update.idx == dfu_param->idx &&
        srv->blob.state.transfer.id == dfu_param->blob_id) {
        srv->update.phase = BT_MESH_DFU_PHASE_TRANSFER_ACTIVE;
        status = BT_MESH_DFU_SUCCESS;
        dfu_server_store_state(srv);

        BT_INFO("Resuming transfer");
        update_status_rsp(srv, ctx, status, NULL);

        return DFU_SERVER_STATUS_CHECK_OK;
    }

    if (bt_mesh_dfu_srv_is_busy(srv)) {
        if (is_active_update(srv, dfu_param->idx, dfu_param->timeout_base,
                             &(dfu_param->blob_id), dfu_param->ttl, dfu_param->meta_checksum)) {
            status = BT_MESH_DFU_ERR_WRONG_PHASE;
        } else {
            status = BT_MESH_DFU_SUCCESS;
            srv->update.ttl = dfu_param->ttl;
            srv->blob.state.transfer.id = dfu_param->blob_id;
            srv->update.timeout_base = dfu_param->timeout_base;
        }

        BT_INFO("Busy. Phase: %u", srv->update.phase);
        update_status_rsp(srv, ctx, status, NULL);

        return DFU_SERVER_STATUS_CHECK_OK;
    }

    return 0;
}

static int bt_mesh_dfu_check_cb(struct bt_mesh_dfu_handle_param_t *dfu_param, struct os_mbuf *buf,
                                struct bt_mesh_dfu_srv_t *srv, struct bt_mesh_msg_ctx *ctx,
                                const struct bt_mesh_blob_stream_t **stream)
{
    int err = 0;
    enum bt_mesh_dfu_status_t status;

    srv->update.ttl = dfu_param->ttl;
    srv->update.timeout_base = dfu_param->timeout_base;
    srv->update.meta = dfu_param->meta_checksum;

    err = srv->cb->start(srv, &srv->imgs[dfu_param->idx], buf, stream); // 回调检查
    if ((err == -EALREADY) || (!err && bt_mesh_has_addr(ctx->addr))) { // 判断消息来源
        status = BT_MESH_DFU_SUCCESS;
        srv->update.idx = dfu_param->idx;
        srv->blob.state.transfer.id = dfu_param->blob_id;
        srv->update.phase = BT_MESH_DFU_PHASE_VERIFY;
        update_status_rsp(srv, ctx, status, NULL);
        dfu_server_img_verify(srv);
        return DFU_SERVER_STATUS_CHECK_OK;
    }

    if (err == -ENOMEM) {
        status = BT_MESH_DFU_ERR_RESOURCES;
        update_status_rsp(srv, ctx, status, NULL);
        return DFU_SERVER_STATUS_CHECK_OK;
    }

    if (err == -EBUSY) {
        status = BT_MESH_DFU_ERR_TEMPORARILY_UNAVAILABLE;
        update_status_rsp(srv, ctx, status, NULL);
        return DFU_SERVER_STATUS_CHECK_OK;
    }

    if (err || !(*stream) || !((*stream)->on_write)) {
        status = BT_MESH_DFU_ERR_INTERNAL;
        update_status_rsp(srv, ctx, status, NULL);
        return DFU_SERVER_STATUS_CHECK_OK;
    }
    return 0;
}

static int bt_mesh_dfu_handle_start(struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx,
                                    struct os_mbuf *buf)
{
    struct bt_mesh_dfu_srv_t *srv = mod->user_data;
    const struct bt_mesh_blob_stream_t *stream;
    struct bt_mesh_dfu_handle_param_t dfu_param;
    enum bt_mesh_dfu_status_t status;
    struct net_buf_simple_state buf_state;
    int ret = 0;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    bt_mesh_dfu_get_handle_param(&dfu_param, buf);

    ret = bt_mesh_dfu_check_normal_status(&dfu_param, buf, srv, ctx);
    if (ret) {
        return 0;
    }

    net_buf_simple_save(buf, &buf_state);
    status = metadata_check(srv, dfu_param.idx, buf, (enum bt_mesh_dfu_effect_t *)&srv->update.effect); // 是否可选
    net_buf_simple_restore(buf, &buf_state);
    if (status != BT_MESH_DFU_SUCCESS) {
        update_status_rsp(srv, ctx, status, NULL);
        return 0;
    }

    ret = bt_mesh_dfu_check_cb(&dfu_param, buf, srv, ctx, &stream);
    if (ret) {
        return 0;
    }

    ota_node_role_take(OTA_NODE_ROLE_NONE);
    if (!ota_node_role_take(OTA_NODE_ROLE_TARGET)) {
        BT_ERR("take role failed");
        status = BT_MESH_DFU_ERR_BLOB_BUSY;
        update_status_rsp(srv, ctx, status, NULL);
        return 0;
    }

    ret = bt_mesh_blob_server_recv(&srv->blob, dfu_param.blob_id, stream, dfu_param.ttl, dfu_param.timeout_base);
    if (ret) {
        status = BT_MESH_DFU_ERR_BLOB_BUSY;
        ota_node_role_take(OTA_NODE_ROLE_NONE);
        update_status_rsp(srv, ctx, status, NULL);
        return 0;
    }

    ble_mesh_ota_upgrade_status_report(OTA_UPGRADE_START);
    srv->update.idx = dfu_param.idx;
    srv->update.phase = BT_MESH_DFU_PHASE_TRANSFER_ACTIVE;
    status = BT_MESH_DFU_SUCCESS;
    dfu_server_store_state(srv);
    update_status_rsp(srv, ctx, status, NULL);

    return 0;
}

static int bt_mesh_dfu_handle_cancel(struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx,
                                     struct os_mbuf *buf)
{
    struct bt_mesh_dfu_srv_t *srv = mod->user_data;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    if (srv->update.idx == UPDATE_IDX_NONE) {
        goto rsp;
    }

    BT_INFO("");

    bt_mesh_blob_server_cancel(&srv->blob);
    srv->update.phase = BT_MESH_DFU_PHASE_IDLE;
    transfer_failed(srv);

rsp:
    update_status_rsp(srv, ctx, BT_MESH_DFU_SUCCESS, NULL);

    return 0;
}

static int bt_mesh_dfu_handle_apply(struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx,
                                    struct os_mbuf *buf)
{
    struct bt_mesh_dfu_srv_t *srv = mod->user_data;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    static const struct bt_mesh_send_cb send_cb = {
        .start = apply_send_start,
        .end = apply_send_end,
    };

    if (srv->update.phase == BT_MESH_DFU_PHASE_APPLYING) {
        update_status_rsp(srv, ctx, BT_MESH_DFU_SUCCESS, NULL);
        return 0;
    }

    if (srv->update.phase != BT_MESH_DFU_PHASE_VERIFY_OK) {
        BT_WARN("Apply: Invalid phase %u", srv->update.phase);
        update_status_rsp(srv, ctx, BT_MESH_DFU_ERR_WRONG_PHASE, NULL);
        return 0;
    }

    srv->update.phase = BT_MESH_DFU_PHASE_APPLYING;
    update_status_rsp(srv, ctx, BT_MESH_DFU_SUCCESS, &send_cb);

    return 0;
}

const struct bt_mesh_model_op _bt_mesh_dfu_srv_op[] = {
    { MODEL_OP(DFU_UPDATE_INFO_GET), BT_MESH_LEN_EXACT(2), bt_mesh_dfu_handle_info_get },
    { MODEL_OP(DFU_UPDATE_METADATA_CHECK), BT_MESH_LEN_MIN(1), bt_mesh_dfu_handle_metadata_check },
    { MODEL_OP(DFU_UPDATE_GET), BT_MESH_LEN_EXACT(0), bt_mesh_dfu_handle_get },
    { MODEL_OP(DFU_UPDATE_START), BT_MESH_LEN_MIN(12), bt_mesh_dfu_handle_start },
    { MODEL_OP(DFU_UPDATE_CANCEL), BT_MESH_LEN_EXACT(0), bt_mesh_dfu_handle_cancel },
    { MODEL_OP(DFU_UPDATE_APPLY), BT_MESH_LEN_EXACT(0), bt_mesh_dfu_handle_apply },
    BT_MESH_MODEL_OP_END,
};

static int dfu_srv_deinit(struct bt_mesh_model *mod)
{
    if ((mod->pub != NULL) && (mod->pub->msg != NULL)) {
        os_mbuf_free(mod->pub->msg);
        mod->pub->msg = NULL;
    }
    return 0;
}

static int dfu_srv_init(struct bt_mesh_model *mod)
{
    int err = 0;
    struct bt_mesh_dfu_srv_t *srv = mod->user_data;

    srv->mod = mod;
    srv->update.idx = UPDATE_IDX_NONE;

    if (!srv->cb || !srv->cb->start || !srv->imgs || srv->img_count == 0 || srv->img_count == UPDATE_IDX_NONE) {
        BT_ERR("Invalid DFU Server initialization");
        return -EINVAL;
    }

    const struct bt_mesh_model *blob_srv =
        bt_mesh_model_find(bt_mesh_model_elem(mod), BT_MESH_MODEL_ID_BLOB_SRV);

    if (blob_srv == NULL) {
        BT_ERR("Missing BLOB Srv.");
        return -EINVAL;
    }

    err = bt_mesh_model_extend(mod, srv->blob.mod);
    if (err) {
        return err;
    }

    return 0;
}

int dfu_srv_settings_set(struct bt_mesh_model *model, const char *name, char *val) {return -1;}

static void dfu_srv_reset(struct bt_mesh_model *mod)
{
    struct bt_mesh_dfu_srv_t *srv = mod->user_data;

    srv->update.phase = BT_MESH_DFU_PHASE_IDLE;
    dfu_server_erase_state(srv);
}

const struct bt_mesh_model_cb _bt_mesh_dfu_srv_cb = {
    .init = dfu_srv_init,
    .settings_set = dfu_srv_settings_set,
    .reset = dfu_srv_reset,
    .deinit = dfu_srv_deinit,
};

static void blob_suspended(struct bt_mesh_blob_server_t *blob_server)
{
    struct bt_mesh_dfu_srv_t *srv = CONTAINER_OF(blob_server, struct bt_mesh_dfu_srv_t, blob);

    ota_node_role_take(OTA_NODE_ROLE_NONE);
    srv->update.phase = BT_MESH_DFU_PHASE_TRANSFER_ERR;
    dfu_server_store_state(srv);
}

static void blob_end(struct bt_mesh_blob_server_t *blob_server, uint64_t id, bool success)
{
    struct bt_mesh_dfu_srv_t *srv = CONTAINER_OF(blob_server, struct bt_mesh_dfu_srv_t, blob);

    BT_INFO("success: %u", success);
    ota_node_role_take(OTA_NODE_ROLE_NONE);

    if (!success) {
        srv->update.phase = BT_MESH_DFU_PHASE_TRANSFER_ERR;
        transfer_failed(srv);
        return;
    }

    dfu_server_img_verify(srv);
}

static int blob_recover(struct bt_mesh_blob_server_t *blob_server, struct bt_mesh_blob_transfer_t *transfer,
                        const struct bt_mesh_blob_stream_t **stream)
{
    struct bt_mesh_dfu_srv_t *srv = CONTAINER_OF(blob_server, struct bt_mesh_dfu_srv_t, blob);

    if (!srv->cb->recover || srv->update.phase != BT_MESH_DFU_PHASE_TRANSFER_ERR ||
        srv->update.idx >= srv->img_count) {
        return -ENOTSUP;
    }

    return srv->cb->recover(srv, &srv->imgs[srv->update.idx], stream);
}

const struct bt_mesh_blob_server_cb_t _bt_mesh_dfu_srv_blob_cb = {
    .on_suspended = blob_suspended,
    .on_end = blob_end,
    .on_recover = blob_recover,
};

void bt_mesh_dfu_srv_verified(struct bt_mesh_dfu_srv_t *srv)
{
    if (srv->update.phase != BT_MESH_DFU_PHASE_VERIFY) {
        BT_WARN("Wrong state");
        return;
    }

    srv->update.phase = BT_MESH_DFU_PHASE_VERIFY_OK;
    dfu_server_store_state(srv);
}

void bt_mesh_dfu_srv_rejected(struct bt_mesh_dfu_srv_t *srv)
{
    if (srv->update.phase != BT_MESH_DFU_PHASE_VERIFY) {
        BT_WARN("Wrong state");
        return;
    }

    srv->update.phase = BT_MESH_DFU_PHASE_VERIFY_FAIL;
    dfu_server_store_state(srv);
}

void bt_mesh_dfu_srv_cancel(struct bt_mesh_dfu_srv_t *srv)
{
    if (srv->update.phase == BT_MESH_DFU_PHASE_IDLE) {
        BT_WARN("Wrong state");
        return;
    }

    (void)bt_mesh_blob_server_cancel(&srv->blob);
}

void bt_mesh_dfu_srv_applied(struct bt_mesh_dfu_srv_t *srv)
{
    if (srv->update.phase != BT_MESH_DFU_PHASE_APPLYING) {
        BT_WARN("Wrong state");
        return;
    }

    srv->update.phase = BT_MESH_DFU_PHASE_IDLE;
    dfu_server_store_state(srv);
}

bool bt_mesh_dfu_srv_is_busy(const struct bt_mesh_dfu_srv_t *srv)
{
    return srv->update.phase != BT_MESH_DFU_PHASE_IDLE &&
           srv->update.phase != BT_MESH_DFU_PHASE_TRANSFER_ERR &&
           srv->update.phase != BT_MESH_DFU_PHASE_VERIFY_FAIL;
}

uint8_t bt_mesh_dfu_srv_progress(const struct bt_mesh_dfu_srv_t *srv)
{
    if (!bt_mesh_dfu_srv_is_busy(srv)) {
        return 0U;
    }

    if (srv->update.phase == BT_MESH_DFU_PHASE_TRANSFER_ACTIVE) {
        return bt_mesh_blob_server_progress(&srv->blob);
    }

    return 100U; // 放到680
}

#ifdef CONFIG_BLE_MESH_PTS_MODE
void target_dfu_verify_fail_set()
{
    dfu_verify_fail = true;
}

void target_dfu_apply_stop_set()
{
    dfu_apply_stop = true;
}
#endif

int bt_mesh_set_target_fwid(const uint8_t *fwid, uint8_t fwid_len)
{
    if (fwid_len < sizeof(uint32_t)) {
        return -EINVAL;
    }
    target_fw_ver_curr = *(uint32_t *)fwid;
    BT_INFO("fw ver curr:0x%08x", target_fw_ver_curr);
    return 0;
}

uint8_t bt_mesh_get_dfu_status(void)
{
    return g_dfu_srv.update.phase;
}

/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: DFU Model Implementation.
 *
 * History:
 * 2025-05-01, Create file.
 */

#include <string.h>
#include "syscfg/syscfg.h"
#include "os/os_mbuf.h"
#include "mesh/glue.h"
#include "mesh/mesh.h"
#include "model/model_opcode.h"
#include "model/model_def.h"
#include "access.h"
#include "model_common.h"
#include "blob.h"
#include "mesh_ota_common.h"
#include "os_time.h"
#include "model/dfu_client.h"

#define DFU_CLIENT_STATUS_CHECK_OK 1

#define TARGETS_FOR_EACH(cli, target)                                  \
    SYS_SLIST_FOR_EACH_CONTAINER(                                      \
        (sys_slist_t *)&((cli)->blob.inputs)->targets, target, blob.n)

#define BLE_MESH_DFU_MSG_CTX(cli, dst)                         \
    {                                                          \
        .app_idx = (cli)->blob.inputs->app_idx, .addr = (dst), \
        .send_ttl = (cli)->blob.inputs->ttl,                   \
    }

#define BLE_MESH_DFU_CLI(blob_cli) CONTAINER_OF(blob_cli, struct bt_mesh_dfu_client_t, blob)

BUILD_ASSERT((DFU_UPDATE_START_MSG_MAXLEN + BT_MESH_MODEL_OP_LEN(MODEL_OP(DFU_UPDATE_START)) +
          BT_MESH_MIC_SHORT) <= BT_MESH_TX_SDU_MAX, "DFU_UPDATE_START message not fit into the BT_MESH_TX_SDU_MAX.");

BUILD_ASSERT((DFU_UPDATE_INFO_STATUS_MSG_MINLEN +
          BT_MESH_MODEL_OP_LEN(MODEL_OP(DFU_UPDATE_INFO_STATUS)) + BT_MESH_MIC_SHORT) <=
          BT_MESH_RX_SDU_MAX, "DFU_UPDATE_INFO_STATUS message not fit into the BT_MESH_TX_SDU_MAX.");

enum req_t {
    REQ_NONE     = 0x00,
    REQ_METADATA = 0x01,
    REQ_IMG      = 0x02,
    REQ_STATUS   = 0x03,
};

enum {
    FLAG_FAILED = BIT(0),
    FLAG_CANCELLED = BIT(1),
    FLAG_SKIP_CAPS_GET = BIT(2),
    FLAG_RESUME = BIT(3),
    FLAG_COMPLETED = BIT(4),
};

enum {
    STATE_IDLE      = 0x00,
    STATE_TRANSFER  = 0x01,
    STATE_REFRESH   = 0x02,
    STATE_VERIFIED  = 0x03,
    STATE_APPLY     = 0x04,
    STATE_APPLIED   = 0x05,
    STATE_CONFIRM   = 0x06,
    STATE_CANCEL    = 0x07,
    STATE_SUSPENDED = 0x08,
};

static int32_t dfu_cli_timeout = (BT_MESH_DFU_CLI_TIMEOUT * MSEC_PER_SEC);

static struct bt_mesh_dfu_target_t *target_get(struct bt_mesh_dfu_client_t *cli, uint16_t addr)
{
    struct bt_mesh_dfu_target_t *target;

    TARGETS_FOR_EACH(cli, target) {
        if (addr == target->blob.addr) {
            return target;
        }
    }

    return NULL;
}

static void target_failed(struct bt_mesh_dfu_client_t *cli, struct bt_mesh_dfu_target_t *target,
                          enum bt_mesh_dfu_status_t status)
{
    target->status = status;

    BT_ERR("Target 0x%04x failed: %u", target->blob.addr, status);

    if (target->blob.status == BT_MESH_BLOB_SUCCESS) {
        target->blob.status = BT_MESH_BLOB_ERR_INTERNAL;
    }

    if (cli->cb && cli->cb->on_lost_target) {
        cli->cb->on_lost_target(cli, target);
    }
}

static void dfu_complete(struct bt_mesh_dfu_client_t *cli)
{
    if (cli->cb && cli->cb->on_ended) {
        cli->cb->on_ended(cli, BT_MESH_DFU_SUCCESS);
    }
}

static void dfu_applied(struct bt_mesh_dfu_client_t *cli)
{
    cli->transfer.state = STATE_APPLIED;
    if (cli->cb && cli->cb->on_applied) {
        cli->cb->on_applied(cli);
    }
}

static void dfu_failed(struct bt_mesh_dfu_client_t *cli, enum bt_mesh_dfu_status_t reason)
{
    BT_INFO("%u", reason);

    cli->transfer.flags |= FLAG_FAILED;

    if (cli->cb && cli->cb->on_ended) {
        cli->cb->on_ended(cli, reason);
    }
}

static int dfu_request_param_setup(struct bt_mesh_dfu_client_t *cli, enum req_t type, uint16_t addr, uint8_t *params)
{
    if (cli->req.type != REQ_NONE) {
        return -EBUSY;
    }

    cli->req.addr = addr;
    cli->req.params = params;
    cli->req.type = type;

    return 0;
}

static int dfu_request_param_wait(struct bt_mesh_dfu_client_t *cli, int64_t timeout)
{
    int err = 0;

    err = k_sem_take(&cli->req.sem, timeout);
    cli->req.type = REQ_NONE;

    return err;
}

static bool targets_active(struct bt_mesh_dfu_client_t *cli)
{
    struct bt_mesh_dfu_target_t *target;

    TARGETS_FOR_EACH(cli, target) {
        if (target->status == BT_MESH_DFU_SUCCESS) {
            return true;
        }
    }

    return false;
}

static void dfu_client_refresh(struct bt_mesh_dfu_client_t *cli);

static void blob_caps(struct bt_mesh_blob_client_t *blob_client,
                      const struct bt_mesh_blob_client_capabilities_t *caps)
{
    struct bt_mesh_dfu_client_t *cli = BLE_MESH_DFU_CLI(blob_client);
    int err = 0;

    if (!caps) {
        dfu_failed(cli, BT_MESH_DFU_ERR_RESOURCES);
        return;
    }

    cli->transfer.blob.block_size_logarithm = caps->max_block_size_logarithm;
    cli->transfer.blob.chunk_size = caps->max_chunk_size;

    if (!(cli->transfer.blob.mode & BT_MESH_BLOB_TRANSFER_MODE_ALL)) {
        cli->transfer.blob.mode =
            caps->modes == BT_MESH_BLOB_TRANSFER_MODE_ALL ? BT_MESH_BLOB_TRANSFER_MODE_PUSH : caps->modes;
    } else {
        cli->transfer.blob.mode =
            caps->modes == BT_MESH_BLOB_TRANSFER_MODE_ALL ? cli->transfer.blob.mode : caps->modes;
    }

    err = bt_mesh_blob_client_send(blob_client, blob_client->inputs, &cli->transfer.blob, cli->transfer.stream);
    if (err) {
        BT_ERR("Starting BLOB transfer failed: %d", err);
        dfu_failed(cli, BT_MESH_DFU_ERR_BLOB_BUSY);
    }
}

static void blob_lost_target(struct bt_mesh_blob_client_t *blob_client, struct bt_mesh_blob_target_t *blobt,
                             enum bt_mesh_blob_status_t reason)
{
    struct bt_mesh_dfu_target_t *target = CONTAINER_OF(blobt, struct bt_mesh_dfu_target_t, blob);
    struct bt_mesh_dfu_client_t *cli = BLE_MESH_DFU_CLI(blob_client);

    if ((cli->transfer.state == STATE_CONFIRM || cli->transfer.state == STATE_APPLY) &&
        target->effect == BT_MESH_DFU_EFFECT_UNPROV) {
        target->blob.status = BT_MESH_BLOB_SUCCESS;
        return;
    }

    target_failed(cli, target, BT_MESH_DFU_ERR_INTERNAL);
}

static void blob_suspended(struct bt_mesh_blob_client_t *blob_client)
{
    struct bt_mesh_dfu_client_t *cli = BLE_MESH_DFU_CLI(blob_client);

    BT_INFO("BLOB transfer suspended");

    cli->transfer.state = STATE_SUSPENDED;
    ota_node_role_take(OTA_NODE_ROLE_NONE);

    if (cli->cb && cli->cb->on_suspended) {
        cli->cb->on_suspended(cli);
    }
}

static void blob_end(struct bt_mesh_blob_client_t *blob_client,
                     const struct bt_mesh_blob_transfer_t *transfer, bool success)
{
    struct bt_mesh_dfu_client_t *cli = BLE_MESH_DFU_CLI(blob_client);

    cli->req.img_cb = NULL;
    ota_node_role_take(OTA_NODE_ROLE_NONE);

    if (success) {
        dfu_client_refresh(cli);
        return;
    }

    if (cli->transfer.state == STATE_CANCEL) {
        return;
    }

    if (cli->transfer.state != STATE_TRANSFER) {
        BT_ERR("Blob failed in invalid state %u", cli->transfer.state);
        return;
    }

    dfu_failed(cli, BT_MESH_DFU_ERR_INTERNAL);
}

static void blob_transfer_progress(struct bt_mesh_blob_client_t*cli, struct bt_mesh_blob_target_t *target,
                                   const struct bt_mesh_blob_transfer_info_t *info)
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
    progress = (total_blocks - blocks_not_rxed) / total_blocks;

    BT_INFO("BLOB transfer progress received from target 0x%04x:\n phase: %d\n progress: %u%%",
        target->addr, info->phase, progress);
}

static void blob_transfer_progress_complete(struct bt_mesh_blob_client_t*cli)
{
    BT_INFO("Determine BLOB transfer progress procedure complete");
}

const struct bt_mesh_blob_client_cb _bt_mesh_dfu_client_blob_handlers = {
    .on_caps = blob_caps,
    .on_lost_target = blob_lost_target,
    .on_suspended = blob_suspended,
    .on_end = blob_end,
    .on_transfer_progress = blob_transfer_progress,
    .on_transfer_progress_complete = blob_transfer_progress_complete,
};

static void trinasmit_start(uint16_t dur, int err, void *cb_data);
static void trinasmit_end(int err, void *cb_data);

static const struct bt_mesh_send_cb send_cb = {
    .start = trinasmit_start,
    .end = trinasmit_end,
};

static void trinasmit_start(uint16_t dur, int err, void *cb_data)
{
    if (err) {
        trinasmit_end(err, cb_data);
    }
}

static void trinasmit_end(int err, void *cb_data)
{
    struct bt_mesh_dfu_client_t *cli = cb_data;

    blob_client_broadcast_tx_completed(&cli->blob);
}

static int target_info_get(struct bt_mesh_dfu_client_t *cli, struct bt_mesh_msg_ctx *ctx,
                           uint8_t idx, uint8_t max_count, const struct bt_mesh_send_cb *cb)
{
    int err = 0;
    struct os_mbuf *buf = BT_MESH_MODEL_BUF(MODEL_OP(DFU_UPDATE_INFO_GET), BT_MESH_DFU_UPDATE_INFO_GET_HEAD);
    bt_mesh_model_msg_init(buf, MODEL_OP(DFU_UPDATE_INFO_GET));
    net_buf_simple_add_u8(buf, idx);
    net_buf_simple_add_u8(buf, max_count);

    bt_mesh_ota_stat(MODEL_OP(DFU_UPDATE_INFO_GET));
    err = bt_mesh_model_send(cli->mod, ctx, buf, cb, cli);
    if (err) {
        BT_ERR("Send err: %d", err);
        if (cb) {
            cb->end(err, cli);
        }
        os_mbuf_free_chain(buf);
        return err;
    }
    os_mbuf_free_chain(buf);
    return 0;
}

static void send_info_get(struct bt_mesh_blob_client_t *blob_client, uint16_t dst)
{
    struct bt_mesh_dfu_client_t *cli = BLE_MESH_DFU_CLI(blob_client);
    struct bt_mesh_msg_ctx ctx = BLE_MESH_DFU_MSG_CTX(cli, dst);

    cli->req.img_cnt = 0xff; // 协议查询

    target_info_get(cli, &ctx, 0, cli->req.img_cnt, &send_cb);
}

static void send_update_start(struct bt_mesh_blob_client_t *blob_client, uint16_t dst)
{
    struct bt_mesh_dfu_client_t *cli = BLE_MESH_DFU_CLI(blob_client);
    struct bt_mesh_msg_ctx ctx = BLE_MESH_DFU_MSG_CTX(cli, dst);
    struct bt_mesh_dfu_target_t *target;
    uint16_t timeout_base = 0;

    if (blob_client->tx.ctx.b_force_unicast) {
        target = target_get(cli, dst);
    } else {
        target = SYS_SLIST_PEEK_HEAD_CONTAINER(
            (sys_slist_t *)&((cli)->blob.inputs)->targets, target, blob.n);
    }
    if (target == NULL) {
        BT_ERR("target from dfu client is NULL");
        return;
    }

    struct os_mbuf *buf = BT_MESH_MODEL_BUF(MODEL_OP(DFU_UPDATE_START), DFU_UPDATE_START_MSG_MAXLEN);
    bt_mesh_model_msg_init(buf, MODEL_OP(DFU_UPDATE_START));

    net_buf_simple_add_u8(buf, cli->blob.inputs->ttl);
#ifdef CONFIG_BLE_MESH_PTS_MODE
    net_buf_simple_add_le16(buf, cli->blob.inputs->timeout_base);
#else
    timeout_base = ((CLIENT_TIMEOUT_MSEC(&cli->blob) / 1000) / 10) - 1;  // 1000:ms 10:step
    net_buf_simple_add_le16(buf, timeout_base + 2);                      // 2:extend
#endif
    net_buf_simple_add_le32(buf, (uint32_t)cli->transfer.blob.id);
    net_buf_simple_add_le32(buf, (uint32_t)(cli->transfer.blob.id >> BT_MESHDFU_UINT64_ADD_OFFSET));
    net_buf_simple_add_u8(buf, target->img_idx);
    net_buf_simple_add_mem(buf, cli->transfer.bank->metadata, cli->transfer.bank->metadata_len);

    BT_INFO("blob id = %llu", cli->transfer.blob.id);

    bt_mesh_ota_stat(MODEL_OP(DFU_UPDATE_START));
    (void)bt_mesh_model_send(cli->mod, &ctx, buf, &send_cb, cli);
    os_mbuf_free_chain(buf);
}

static void send_update_get(struct bt_mesh_blob_client_t *blob_client, uint16_t dst)
{
    struct bt_mesh_dfu_client_t *cli = BLE_MESH_DFU_CLI(blob_client);
    struct bt_mesh_msg_ctx ctx = BLE_MESH_DFU_MSG_CTX(cli, dst);

    struct os_mbuf *buf = BT_MESH_MODEL_BUF(MODEL_OP(DFU_UPDATE_GET), BT_MESHDFU_UPDATE_BUF_DEFINE);
    bt_mesh_model_msg_init(buf, MODEL_OP(DFU_UPDATE_GET));

    bt_mesh_ota_stat(MODEL_OP(DFU_UPDATE_GET));
    (void)bt_mesh_model_send(cli->mod, &ctx, buf, &send_cb, cli);
    os_mbuf_free_chain(buf);
}

static void send_update_cancel(struct bt_mesh_blob_client_t *blob_client, uint16_t dst)
{
    struct bt_mesh_dfu_client_t *cli = BLE_MESH_DFU_CLI(blob_client);
    struct bt_mesh_msg_ctx ctx = BLE_MESH_DFU_MSG_CTX(cli, dst);

    struct os_mbuf *buf = BT_MESH_MODEL_BUF(MODEL_OP(DFU_UPDATE_CANCEL), BT_MESHDFU_UPDATE_BUF_DEFINE);
    bt_mesh_model_msg_init(buf, MODEL_OP(DFU_UPDATE_CANCEL));

    (void)bt_mesh_model_send(cli->mod, &ctx, buf, &send_cb, cli);
    os_mbuf_free_chain(buf);
}

static void send_update_apply(struct bt_mesh_blob_client_t *blob_client, uint16_t dst)
{
    struct bt_mesh_dfu_client_t *cli = BLE_MESH_DFU_CLI(blob_client);
    struct bt_mesh_msg_ctx ctx = BLE_MESH_DFU_MSG_CTX(cli, dst);

    struct os_mbuf *buf = BT_MESH_MODEL_BUF(MODEL_OP(DFU_UPDATE_APPLY), BT_MESHDFU_UPDATE_BUF_DEFINE);
    bt_mesh_model_msg_init(buf, MODEL_OP(DFU_UPDATE_APPLY));

    bt_mesh_ota_stat(MODEL_OP(DFU_UPDATE_APPLY));
    (void)bt_mesh_model_send(cli->mod, &ctx, buf, &send_cb, cli);
    os_mbuf_free_chain(buf);
}

static void dfu_client_transfer(struct bt_mesh_blob_client_t *blob_client);
static void dfu_client_apply(struct bt_mesh_dfu_client_t *cli);
static void dfu_client_status_applied(struct bt_mesh_blob_client_t *blob_client);
static void dfu_client_status_confirmed(struct bt_mesh_blob_client_t *blob_client);
static void dfu_client_status_cancelled(struct bt_mesh_blob_client_t *blob_client);

static void initiate(struct bt_mesh_dfu_client_t *cli)
{
    struct blob_client_broadcast_context_t tsransmit = {
        .send = send_update_start,
        .next = dfu_client_transfer,
        .b_acked = true,
    };
    struct bt_mesh_dfu_target_t *target;
    int img_idx = -1;

    TARGETS_FOR_EACH(cli, target) {
        if (img_idx == -1) {
            img_idx = target->img_idx;
        } else if (target->img_idx != img_idx) {
            tsransmit.b_force_unicast = true;
            break;
        }
    }

    cli->op = MODEL_OP(DFU_UPDATE_STATUS);
    cli->transfer.state = STATE_TRANSFER;

    blob_client_broadcast(&cli->blob, &tsransmit);
}

static void skip_targets_from_broadcast(struct bt_mesh_dfu_client_t *cli, bool skip) // 维测是否生效，能否打印
{
    struct bt_mesh_dfu_target_t *target;

    TARGETS_FOR_EACH(cli, target) {
        if (bt_mesh_has_addr(target->blob.addr) || target->phase == BT_MESH_DFU_PHASE_VERIFY) {
            target->blob.b_skip = skip;
            break;
        }
    }
}

static bool transfer_skip(struct bt_mesh_dfu_client_t *cli)
{
    struct bt_mesh_dfu_target_t *target;

    TARGETS_FOR_EACH(cli, target) {
        if (!bt_mesh_has_addr(target->blob.addr) || !target->blob.b_skip) {
            return false;
        }
    }

    return true;
}

static void dfu_client_transfer(struct bt_mesh_blob_client_t *blob_client)
{
    struct bt_mesh_dfu_client_t *cli = BLE_MESH_DFU_CLI(blob_client);
    int err = 0;

    if (!targets_active(cli)) {
        dfu_failed(cli, BT_MESH_DFU_ERR_INTERNAL);
        return;
    }

    skip_targets_from_broadcast(cli, true);

    if (transfer_skip(cli)) {
        dfu_client_refresh(cli);
        return;
    }

    if (cli->transfer.flags & FLAG_RESUME) {
        cli->transfer.flags ^= FLAG_RESUME;
        err = bt_mesh_blob_client_resume(blob_client);
        if (err) {
            BT_ERR("Resuming BLOB transfer failed: %d", err);
            dfu_failed(cli, BT_MESH_DFU_ERR_BLOB_BUSY);
        }
        return;
    }

    if (cli->transfer.flags & FLAG_SKIP_CAPS_GET) {
        cli->transfer.flags ^= FLAG_SKIP_CAPS_GET;
        err = bt_mesh_blob_client_send(blob_client, blob_client->inputs, &cli->transfer.blob, cli->transfer.stream);
        if (err) {
            BT_ERR("Starting BLOB transfer failed: %d", err);
            dfu_failed(cli, BT_MESH_DFU_ERR_BLOB_BUSY);
        }
        return;
    }

    err = bt_mesh_blob_client_capabilities_get(&cli->blob, cli->blob.inputs);
    if (err) {
        BT_ERR("Failed starting blob transfer: %d", err);
        dfu_failed(cli, BT_MESH_DFU_ERR_BLOB_BUSY);
    }

    return;
}

static void dfu_client_status_refreshed(struct bt_mesh_blob_client_t *blob_client)
{
    struct bt_mesh_dfu_client_t *cli = BLE_MESH_DFU_CLI(blob_client);

    if (!targets_active(cli)) {
        dfu_failed(cli, BT_MESH_DFU_ERR_INTERNAL);
        return;
    }

    cli->transfer.state = STATE_VERIFIED;
    dfu_complete(cli);
}

static void dfu_client_refresh(struct bt_mesh_dfu_client_t *cli)
{
    const struct blob_client_broadcast_context_t tsransmit = {
        .send = send_update_get,
        .next = dfu_client_status_refreshed,
        .b_acked = true
    };

    cli->transfer.state = STATE_REFRESH;
    cli->op = MODEL_OP(DFU_UPDATE_STATUS);

    skip_targets_from_broadcast(cli, false);

    blob_client_broadcast(&cli->blob, &tsransmit);
}

static void dfu_client_apply(struct bt_mesh_dfu_client_t *cli)
{
    const struct blob_client_broadcast_context_t tsransmit = {
        .send = send_update_apply,
        .next = dfu_client_status_applied,
        .b_acked = true
    };

    cli->transfer.state = STATE_APPLY;
    cli->op = MODEL_OP(DFU_UPDATE_STATUS);
    blob_client_broadcast(&cli->blob, &tsransmit);
}

static void dfu_client_status_applied(struct bt_mesh_blob_client_t *blob_client)
{
    struct bt_mesh_dfu_client_t *cli = BLE_MESH_DFU_CLI(blob_client);

    if (!targets_active(cli)) {
        dfu_failed(cli, BT_MESH_DFU_ERR_INTERNAL);
        return;
    }
    dfu_applied(cli);
}

static enum bt_mesh_dfu_iter_t target_img_cb(struct bt_mesh_dfu_client_t *cli, struct bt_mesh_msg_ctx *ctx,
                                             uint8_t idx, uint8_t cnt, const struct bt_mesh_dfu_img_t *img,
                                             uint8_t *cb_data)
{
    struct bt_mesh_dfu_target_t *target;
    static uint8_t succ_count = 0;

    if ((img->fwid_len != cli->transfer.bank->fwid_len) ||
        memcmp(cli->transfer.bank->fwid, img->fwid, img->fwid_len)) {
        BT_INFO("len %u, cli fwid 0x%02x", cli->transfer.bank->fwid_len, cli->transfer.bank->fwid[0]);

        return BT_MESH_DFU_ITER_CONTINUE;
    }

    target = target_get(cli, ctx->addr);
    if (target) {
        BT_INFO("SUCCESS: 0x%04x applied dfu (as image %u)", ctx->addr, idx);
        target->phase = BT_MESH_DFU_PHASE_APPLY_SUCCESS;
        succ_count++;
        if (cli->blob.tx.n_pending_blocks == 1) {
            bt_mesh_ota_result(ble_npl_time_get_ms() / MSEC_PER_SEC, succ_count, OTA_END);
            succ_count = 0;
        }
        blob_client_broadcast_responded(&cli->blob, &target->blob);
    } else {
        BT_WARN("Target 0x%04x not found", ctx->addr);
    }

    return BT_MESH_DFU_ITER_STOP;
}

static void dfu_client_confirm(struct bt_mesh_dfu_client_t *cli)
{
    const struct blob_client_broadcast_context_t tsransmit = {
        .send = send_info_get,
        .next = dfu_client_status_confirmed,
        .b_acked = true,
        .b_optional = true,
    };

    cli->op = MODEL_OP(DFU_UPDATE_INFO_STATUS);
    cli->req.img_cb = target_img_cb;
    cli->req.ttl = cli->blob.inputs->ttl;

    blob_client_broadcast(&cli->blob, &tsransmit);
}

static void dfu_client_status_confirmed(struct bt_mesh_blob_client_t *blob_client)
{
    struct bt_mesh_dfu_client_t *cli = BLE_MESH_DFU_CLI(blob_client);
    struct bt_mesh_dfu_target_t *target;
    bool success = false;

    cli->req.img_cb = NULL;

    TARGETS_FOR_EACH(cli, target) {
        if (target->status != BT_MESH_DFU_SUCCESS) {
            continue;
        }

        if (target->effect == BT_MESH_DFU_EFFECT_UNPROV) {
            if (!target->blob.b_acked) {
                success = true;
                continue;
            }

            BT_INFO("Target 0x%04x still provisioned", target->blob.addr);
            target->phase = BT_MESH_DFU_PHASE_APPLY_FAIL;
            target_failed(cli, target, BT_MESH_DFU_ERR_INTERNAL);
        } else if (!target->blob.b_acked) {
            BT_INFO("Target 0x%04x failed to respond", target->blob.addr);
            target->phase = BT_MESH_DFU_PHASE_APPLY_FAIL;
            target_failed(cli, target, BT_MESH_DFU_ERR_INTERNAL);
        } else if (target->status == BT_MESH_DFU_SUCCESS) {
            success = true;
        }
    }

    if (success) {
        cli->transfer.state = STATE_IDLE;
        cli->transfer.flags = FLAG_COMPLETED;

        if (cli->cb && cli->cb->on_confirmed) {
            cli->cb->on_confirmed(cli);
        }
    } else {
        dfu_failed(cli, BT_MESH_DFU_ERR_INTERNAL);
    }
}

static void dfu_client_cancel(struct bt_mesh_dfu_client_t *cli)
{
    const struct blob_client_broadcast_context_t tsransmit = {
        .send = send_update_cancel,
        .next = dfu_client_status_cancelled,
        .b_acked = true
    };

    cli->op = MODEL_OP(DFU_UPDATE_STATUS);

    blob_client_broadcast(&cli->blob, &tsransmit);
}

static void dfu_client_status_cancelled(struct bt_mesh_blob_client_t *blob_client)
{
    struct bt_mesh_dfu_client_t *cli = BLE_MESH_DFU_CLI(blob_client);

    cli->transfer.flags |= FLAG_CANCELLED;
    dfu_failed(cli, BT_MESH_DFU_ERR_INTERNAL);
}

static int bt_mesh_dfu_handle_check_cli(struct bt_mesh_dfu_client_t *cli, struct os_mbuf *buf,
    enum bt_mesh_dfu_status_t status, enum bt_mesh_dfu_phase_t phase, struct bt_mesh_msg_ctx *ctx)
{
    if (cli->req.type == REQ_STATUS && cli->req.addr == ctx->addr) {
        if (cli->req.params) {
            struct bt_mesh_dfu_target_status_t *rsp = (struct bt_mesh_dfu_target_status_t *)cli->req.params;

            rsp->status = status;
            rsp->phase = phase;
            if (buf->om_len == BT_MESH_DFU_MSG_LEN) {
                rsp->ttl = net_buf_simple_pull_u8(buf);
                rsp->effect = net_buf_simple_pull_u8(buf) & BIT_MASK(BT_MESH_DFU_EFFECT_LEN);
                rsp->timeout_base = net_buf_simple_pull_le16(buf);
                rsp->blob_id = net_buf_simple_pull_le32(buf);
                rsp->blob_id |= ((uint64_t)net_buf_simple_pull_le32(buf) << BT_MESHDFU_UINT64_ADD_OFFSET);
                rsp->img_idx = net_buf_simple_pull_u8(buf);
            } else if (buf->om_len) {
                return -EINVAL;
            }

            rsp->ttl = 0U;
            rsp->effect = BT_MESH_DFU_EFFECT_NONE;
            rsp->timeout_base = 0U;
            rsp->blob_id = 0U;
            rsp->img_idx = 0U;
        }
        k_sem_give(&cli->req.sem);
    }

    if (cli->op != MODEL_OP(DFU_UPDATE_STATUS)) {
        return 0;
    }

    return DFU_CLIENT_STATUS_CHECK_OK;
}

static int bt_mesh_dfu_handle_check_status(struct bt_mesh_dfu_client_t *cli, struct bt_mesh_dfu_target_t *target,
    enum bt_mesh_dfu_status_t status, enum bt_mesh_dfu_phase_t phase)
{
    if (cli->transfer.state == STATE_APPLY && phase == BT_MESH_DFU_PHASE_IDLE &&
        status == BT_MESH_DFU_ERR_WRONG_PHASE) {
        BT_DBG("Response received with Idle phase");
        blob_client_broadcast_responded(&cli->blob, &target->blob);
        return DFU_CLIENT_STATUS_CHECK_OK;
    }

    if (status != BT_MESH_DFU_SUCCESS) {
        target_failed(cli, target, status);
        blob_client_broadcast_responded(&cli->blob, &target->blob);
        return DFU_CLIENT_STATUS_CHECK_OK;
    }

    return 0;
}

static int bt_mesh_dfu_handle_get_blobid(
    struct bt_mesh_dfu_target_t *target, struct os_mbuf *buf, struct bt_mesh_dfu_client_t *cli)
{
    if (buf->om_len == BT_MESH_DFU_MSG_LEN) {
        net_buf_simple_pull_u8(buf);
        target->effect = net_buf_simple_pull_u8(buf) & BIT_MASK(BT_MESH_DFU_EFFECT_LEN);
        net_buf_simple_pull_le16(buf);

        uint64_t blob_id = 0;
        blob_id = net_buf_simple_pull_le32(buf);
        blob_id |= ((uint64_t)net_buf_simple_pull_le32(buf) << BT_MESHDFU_UINT64_ADD_OFFSET);
        if (blob_id != cli->transfer.blob.id) {
            BT_WARN("Invalid BLOB ID");
            target_failed(cli, target, BT_MESH_DFU_ERR_BLOB_BUSY);
            blob_client_broadcast_responded(&cli->blob, &target->blob);
            return 0;
        }

        target->img_idx = net_buf_simple_pull_u8(buf);
    } else if (buf->om_len) {
        return -EINVAL;
    }

    return DFU_CLIENT_STATUS_CHECK_OK;
}

static int bt_mesh_dfu_handle_check_responded(
    struct bt_mesh_dfu_target_t *target, struct bt_mesh_dfu_client_t *cli, enum bt_mesh_dfu_phase_t phase)
{
    if (cli->transfer.state == STATE_REFRESH) {
        if (phase == BT_MESH_DFU_PHASE_VERIFY) {
            BT_INFO("STATE_REFRESH Still pending...");
            return DFU_CLIENT_STATUS_CHECK_OK;
        } else if (phase == BT_MESH_DFU_PHASE_VERIFY_FAIL) {
            BT_WARN("Verification failed on target 0x%04x", target->blob.addr);
            target_failed(cli, target, BT_MESH_DFU_ERR_WRONG_PHASE);
        }
    } else if (cli->transfer.state == STATE_APPLY) {
        if (phase != BT_MESH_DFU_PHASE_APPLYING &&
            (target->effect == BT_MESH_DFU_EFFECT_UNPROV || phase != BT_MESH_DFU_PHASE_IDLE)) {
            BT_WARN("Target 0x%04x in phase %u after apply", target->blob.addr, phase);
            target_failed(cli, target, BT_MESH_DFU_ERR_WRONG_PHASE);
            blob_client_broadcast_responded(&cli->blob, &target->blob);
            return DFU_CLIENT_STATUS_CHECK_OK;
        }
        if (phase != BT_MESH_DFU_PHASE_IDLE) {
            return DFU_CLIENT_STATUS_CHECK_OK;
        }
        return 0;
    } else if (cli->transfer.state == STATE_CONFIRM) {
        if (phase == BT_MESH_DFU_PHASE_APPLYING) {
            BT_INFO("Still pending...");
            return DFU_CLIENT_STATUS_CHECK_OK;
        }

        if (phase != BT_MESH_DFU_PHASE_IDLE) {
            BT_WARN("Target 0x%04x in phase %u after apply", target->blob.addr, phase);
            target->phase = BT_MESH_DFU_PHASE_APPLY_FAIL;
            target_failed(cli, target, BT_MESH_DFU_ERR_WRONG_PHASE);
            blob_client_broadcast_responded(&cli->blob, &target->blob);
            return DFU_CLIENT_STATUS_CHECK_OK;
        }
    } else if (cli->transfer.state == STATE_CANCEL) {
        target->phase = BT_MESH_DFU_PHASE_TRANSFER_CANCELED;
    }

    return 0;
}

static int bt_mesh_dfu_handle_status(struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_dfu_client_t *cli = mod->user_data;
    enum bt_mesh_dfu_status_t status;
    enum bt_mesh_dfu_phase_t phase;
    struct bt_mesh_dfu_target_t *target;
    uint8_t byte = 0;
    int ret = 0;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    bt_mesh_ota_stat(MODEL_OP(DFU_UPDATE_STATUS));
    byte = net_buf_simple_pull_u8(buf);
    status = byte & BIT_MASK(BT_MESH_DFU_STATUS_BIT_MASK);
    phase = byte >> BT_MESH_DFU_STATUS_PHASE;

    ret = bt_mesh_dfu_handle_check_cli(cli, buf, status, phase, ctx);
    if (ret != DFU_CLIENT_STATUS_CHECK_OK) {
        return ret;
    }

    target = target_get(cli, ctx->addr);
    if (!target) {
        BT_WARN("Unknown target 0x%04x", ctx->addr);
        return -ENOENT;
    }

    if (status != BT_MESH_DFU_SUCCESS) {
        BT_INFO("target addr:%u status: %u phase: %u, cur state: %u", ctx->addr, status, phase, cli->transfer.state);
    }

    target->phase = phase;

    ret = bt_mesh_dfu_handle_check_status(cli, target, status, phase);
    if (ret) {
        return 0;
    }

    ret = bt_mesh_dfu_handle_get_blobid(target, buf, cli);
    if (ret != DFU_CLIENT_STATUS_CHECK_OK) {
        return ret;
    }

    ret = bt_mesh_dfu_handle_check_responded(target, cli, phase);
    if (ret) {
        return 0;
    }

    blob_client_broadcast_responded(&cli->blob, &target->blob);

    return 0;
}

static int bt_mesh_dfu_handle_get_img(struct os_mbuf *buf, struct bt_mesh_dfu_client_t *cli,
    struct bt_mesh_dfu_index_t *index, enum bt_mesh_dfu_iter_t it, struct bt_mesh_msg_ctx *ctx)
{
    enum bt_mesh_dfu_iter_t remaining_it = it;
    while (buf->om_len && cli->req.img_cb && index->idx < cli->req.img_cnt) {
        char uri_buf[MYNEWT_VAL_BT_MESH_DFU_URI_MAX_LEN + 1];
        struct bt_mesh_dfu_img_t img;
        uint32_t uri_len;

        img.fwid_len = net_buf_simple_pull_u8(buf);
        if (buf->om_len < img.fwid_len + 1) {
            BT_WARN("Invalid format: fwid");
            return -EINVAL;
        }

        img.fwid = net_buf_simple_pull_mem(buf, img.fwid_len);

        uri_len = net_buf_simple_pull_u8(buf);
        if (buf->om_len < uri_len) {
            BT_WARN("Invalid format: uri");
            return -EINVAL;
        }

        BT_DBG("\tImage %u\n\r\tfwid: %s", index->idx, bt_hex(img.fwid, img.fwid_len));

        if (uri_len) {
            uint32_t uri_buf_len = MIN(MYNEWT_VAL_BT_MESH_DFU_URI_MAX_LEN, uri_len);
            memcpy_s(uri_buf, uri_buf_len, net_buf_simple_pull_mem(buf, uri_len), uri_buf_len);
            uri_buf[uri_buf_len] = '\0';
            img.uri = uri_buf;
        } else {
            img.uri = NULL;
        }

        remaining_it = cli->req.img_cb(cli, ctx, index->idx, index->img_cnt, &img, cli->req.params);
        if (remaining_it != BT_MESH_DFU_ITER_CONTINUE) {
            if (cli->req.type == REQ_IMG) {
                k_sem_give(&cli->req.sem);
            }

            return 0;
        }

        index->idx++;
    }

    return DFU_CLIENT_STATUS_CHECK_OK;
}

static int bt_mesh_dfu_handle_info_status(struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_dfu_client_t *cli = mod->user_data;
    struct bt_mesh_dfu_target_t *target;
    struct bt_mesh_dfu_index_t index;
    enum bt_mesh_dfu_iter_t it = BT_MESH_DFU_ITER_CONTINUE;
    int ret = 0;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    bt_mesh_ota_stat(MODEL_OP(DFU_UPDATE_INFO_STATUS));
    if (!cli->req.img_cb || (cli->req.type == REQ_IMG && cli->req.addr != ctx->addr)) {
        BT_WARN("Unexpected info status from 0x%04x", ctx->addr);
        return 0;
    }

    index.img_cnt = net_buf_simple_pull_u8(buf);
    if (index.img_cnt < cli->req.img_cnt) {
        cli->req.img_cnt = index.img_cnt;
    }

    index.idx = net_buf_simple_pull_u8(buf);
    if (index.idx >= index.img_cnt) {
        BT_WARN("Invalid idx %u", index.idx);
        return -ENOENT;
    }

    BT_DBG("Image list from 0x%04x from index %u", ctx->addr, index.idx);

    ret = bt_mesh_dfu_handle_get_img(buf, cli, &index, it, ctx); // 名字修改
    if (ret != DFU_CLIENT_STATUS_CHECK_OK) {
        return ret;
    }

    if (index.idx < cli->req.img_cnt) {
        BT_INFO("Fetching more images (%u/%u)", index.idx, cli->req.img_cnt);
        ctx->send_ttl = cli->req.ttl;
        target_info_get(
            cli, ctx, index.idx, cli->req.img_cnt - index.idx, (cli->req.type == REQ_IMG) ? NULL : &send_cb);
        return 0;
    }

    if (cli->req.type == REQ_IMG) {
        k_sem_give(&cli->req.sem);
        return 0;
    }

    target = target_get(cli, ctx->addr);
    if (target) {
        BT_WARN("Target 0x%04x failed to apply image: %s", ctx->addr,
            bt_hex(cli->transfer.bank->fwid, cli->transfer.bank->fwid_len));
        target->phase = BT_MESH_DFU_PHASE_APPLY_FAIL;
        target_failed(cli, target, BT_MESH_DFU_ERR_INTERNAL);
        blob_client_broadcast_responded(&cli->blob, &target->blob);
    }

    return 0;
}

static int bt_mesh_dfu_handle_metadata_status(
    struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_dfu_client_t *cli = mod->user_data;
    struct bt_mesh_dfu_metadata_status_t *rsp = (struct bt_mesh_dfu_metadata_status_t *)cli->req.params;
    uint8_t hdr = 0;
    uint8_t idx = 0;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    hdr = net_buf_simple_pull_u8(buf);
    idx = net_buf_simple_pull_u8(buf);
    if (cli->req.type != REQ_METADATA || ctx->addr != cli->req.addr || idx != rsp->idx) {
        BT_WARN("Unexpected metadata status from 0x%04x img %u", ctx->addr, idx);
        if (cli->req.type != REQ_METADATA) {
            BT_WARN("Expected %u", cli->req.type);
        } else {
            BT_WARN("Expected 0x%04x img %u", cli->req.addr, idx);
        }

        return 0;
    }

    rsp->status = hdr & BIT_MASK(BT_MESH_DFU_STATUS_BIT_MASK);
    rsp->effect = (hdr >> BT_MESH_DFU_STATUS_BIT_MASK);
    k_sem_give(&cli->req.sem);

    return 0;
}

const struct bt_mesh_model_op _bt_mesh_dfu_client_op[] = {
    {MODEL_OP(DFU_UPDATE_STATUS), BT_MESH_LEN_MIN(1), bt_mesh_dfu_handle_status},
    {MODEL_OP(DFU_UPDATE_INFO_STATUS), BT_MESH_LEN_MIN(2), bt_mesh_dfu_handle_info_status},
    {MODEL_OP(DFU_UPDATE_METADATA_STATUS), BT_MESH_LEN_EXACT(2), bt_mesh_dfu_handle_metadata_status},
    BT_MESH_MODEL_OP_END,
};

static int dfu_cli_deinit(struct bt_mesh_model *mod)
{
    struct bt_mesh_dfu_client_t *cli = mod->user_data;
    if (cli != NULL) {
        k_sem_deinit(&cli->req.sem);
    }

    if ((mod->pub != NULL) && (mod->pub->msg != NULL)) {
        os_mbuf_free(mod->pub->msg);
        mod->pub->msg = NULL;
    }
    return 0;
}

static int dfu_cli_init(struct bt_mesh_model *mod)
{
    int err = 0;
    struct bt_mesh_dfu_client_t *cli = mod->user_data;
    cli->mod = mod;

    const struct bt_mesh_model *blob_cli =
        bt_mesh_model_find(bt_mesh_model_elem(mod), BT_MESH_MODEL_ID_BLOB_CLI);

    if (blob_cli == NULL) {
        BT_ERR("Missing BLOB Cli.");
        return -EINVAL;
    }

    err = bt_mesh_model_extend(mod, cli->blob.mod);
    if (err) {
        return err;
    }

    k_sem_init(&cli->req.sem, 0, 1);

    return 0;
}

static void dfu_cli_reset(struct bt_mesh_model *mod)
{
    struct bt_mesh_dfu_client_t *cli = mod->user_data;

    cli->req.type = REQ_NONE;
    cli->req.addr = BT_MESH_ADDR_UNASSIGNED;
    cli->req.img_cnt = 0;
    cli->req.img_cb = NULL;
    cli->transfer.state = STATE_IDLE;
    cli->transfer.flags = 0;
}

const struct bt_mesh_model_cb _bt_mesh_dfu_client_cb = {
    .init = dfu_cli_init,
    .reset = dfu_cli_reset,
    .deinit = dfu_cli_deinit,
};

int bt_mesh_dfu_client_send(struct bt_mesh_dfu_client_t *cli,
                            const struct bt_mesh_blob_client_inputs_t *inputs,
                            const struct bt_mesh_blob_stream_t *stream,
                            const struct bt_mesh_dfu_client_transfer_t *transfer)
{
    struct bt_mesh_dfu_target_t *target;

    if (bt_mesh_dfu_client_is_busy(cli)) {
        return -EBUSY;
    }

    cli->transfer.blob.mode = transfer->mode;
    cli->transfer.blob.size = transfer->bank->size;

    if (transfer->blob_id == 0) {
        int err = bt_rand(&cli->transfer.blob.id, sizeof(cli->transfer.blob.id));
        if (err) {
            return err;
        }
    } else {
        cli->transfer.blob.id = transfer->blob_id;
    }

    cli->transfer.stream = stream;
    cli->blob.inputs = inputs;
    cli->transfer.bank = transfer->bank;
    cli->transfer.flags = 0U;

    if (transfer->blob_params) {
        cli->transfer.flags |= FLAG_SKIP_CAPS_GET;
        cli->transfer.blob.block_size_logarithm = transfer->blob_params->block_size_logarithm;
        cli->transfer.blob.chunk_size = transfer->blob_params->chunk_size;
    }

    TARGETS_FOR_EACH(cli, target) {
        target->status = BT_MESH_DFU_SUCCESS;
        target->phase = BT_MESH_DFU_PHASE_UNKNOWN;
    }

    initiate(cli);
    return 0;
}

int bt_mesh_dfu_client_suspend(struct bt_mesh_dfu_client_t *cli)
{
    int err;

    err = bt_mesh_blob_client_suspend(&cli->blob);
    if (!err) {
        cli->transfer.state = STATE_SUSPENDED;
    }

    return err;
}

int bt_mesh_dfu_client_resume(struct bt_mesh_dfu_client_t *cli)
{
    struct bt_mesh_dfu_target_t *target;

    if (cli->transfer.state != STATE_SUSPENDED) {
        return -EINVAL;
    }

    cli->transfer.flags = FLAG_RESUME;

    TARGETS_FOR_EACH(cli, target) {
        if (!!target->blob.b_timedout) {
            target->status = BT_MESH_DFU_SUCCESS;
            target->phase = BT_MESH_DFU_PHASE_UNKNOWN;
        }
    }

    initiate(cli);
    return 0;
}

int bt_mesh_dfu_client_cancel(struct bt_mesh_dfu_client_t *cli, struct bt_mesh_msg_ctx *ctx)
{
    if (ctx) {
        int err = 0;

        err = dfu_request_param_setup(cli, REQ_STATUS, ctx->addr, NULL);
        if (err) {
            return err;
        }

        struct os_mbuf *buf = BT_MESH_MODEL_BUF(MODEL_OP(DFU_UPDATE_CANCEL), BT_MESHDFU_UPDATE_BUF_DEFINE);
        bt_mesh_model_msg_init(buf, MODEL_OP(DFU_UPDATE_CANCEL));

        err = bt_mesh_model_send(cli->mod, ctx, buf, NULL, NULL);
        os_mbuf_free_chain(buf);
        if (err) {
            cli->req.type = REQ_NONE;
            return err;
        }

        return dfu_request_param_wait(cli, K_MSEC(dfu_cli_timeout));
    }

    if (cli->transfer.state == STATE_IDLE) {
        return -EALREADY;
    }

    cli->transfer.state = STATE_CANCEL;
    blob_client_broadcast_abort(&cli->blob);
    dfu_client_cancel(cli);
    return 0;
}

int bt_mesh_dfu_client_apply(struct bt_mesh_dfu_client_t *cli)
{
    if (cli->transfer.state != STATE_VERIFIED) {
        return -EBUSY;
    }

    dfu_client_apply(cli);

    return 0;
}

int bt_mesh_dfu_client_confirm(struct bt_mesh_dfu_client_t *cli)
{
    if (cli->transfer.state != STATE_APPLIED) {
        return -EBUSY;
    }

    cli->transfer.state = STATE_CONFIRM;
    dfu_client_confirm(cli);

    return 0;
}

uint8_t bt_mesh_dfu_client_progress(struct bt_mesh_dfu_client_t *cli)
{
    if (cli->transfer.state == STATE_TRANSFER) {
        return bt_mesh_blob_client_transfer_progress_active_get(&cli->blob);
    }

    if (cli->transfer.state == STATE_IDLE) {
        if (cli->transfer.flags & FLAG_COMPLETED) {
            return 100U;
        }
        return 0U;
    }

    return 100U;
}

bool bt_mesh_dfu_client_is_busy(struct bt_mesh_dfu_client_t *cli)
{
    return (cli->transfer.state == STATE_TRANSFER || cli->transfer.state == STATE_REFRESH ||
            cli->transfer.state == STATE_APPLY || cli->transfer.state == STATE_CONFIRM) &&
            !(cli->transfer.flags & FLAG_FAILED);
}

int bt_mesh_dfu_client_imgs_get(struct bt_mesh_dfu_client_t *cli, struct bt_mesh_msg_ctx *ctx,
                                bt_mesh_dfu_img_cb_t cb, uint8_t *cb_data, uint8_t max_count)
{
    int err = 0;

    if (cli->req.img_cb) {
        return -EBUSY;
    }

    err = dfu_request_param_setup(cli, REQ_IMG, ctx->addr, NULL);
    if (err) {
        return err;
    }

    cli->req.img_cb = cb;
    cli->req.params = cb_data;
    cli->req.ttl = ctx->send_ttl;
    cli->req.img_cnt = max_count;

    err = target_info_get(cli, ctx, 0, cli->req.img_cnt,  NULL);
    if (err) {
        cli->req.img_cb = NULL;
        cli->req.type = REQ_NONE;
        return err;
    }

    err = dfu_request_param_wait(cli, K_MSEC(dfu_cli_timeout));

    cli->req.img_cb = NULL;

    return err;
}

int bt_mesh_dfu_client_metadata_check(struct bt_mesh_dfu_client_t *cli, struct bt_mesh_msg_ctx *ctx, uint8_t img_idx,
                                      const struct bt_mesh_dfu_bank_t *bank, struct bt_mesh_dfu_metadata_status_t *rsp)
{
    int err = 0;

    err = dfu_request_param_setup(cli, REQ_METADATA, ctx->addr, (uint8_t *)rsp);
    if (err) {
        return err;
    }

    struct os_mbuf *buf = BT_MESH_MODEL_BUF(MODEL_OP(DFU_UPDATE_METADATA_CHECK),
                                            1 + MYNEWT_VAL_BT_MESH_DFU_METADATA_MAX_LEN);
    bt_mesh_model_msg_init(buf, MODEL_OP(DFU_UPDATE_METADATA_CHECK));

    net_buf_simple_add_u8(buf, img_idx);

    if (bank->metadata_len) {
        net_buf_simple_add_mem(buf, bank->metadata, bank->metadata_len);
    }

    rsp->idx = img_idx;

    err = bt_mesh_model_send(cli->mod, ctx, buf, NULL, NULL);
    os_mbuf_free_chain(buf);
    if (err) {
        cli->req.type = REQ_NONE;
        return err;
    }

    return dfu_request_param_wait(cli, K_MSEC(dfu_cli_timeout));
}

int bt_mesh_dfu_client_status_get(struct bt_mesh_dfu_client_t *cli, struct bt_mesh_msg_ctx *ctx,
                                  struct bt_mesh_dfu_target_status_t *rsp)
{
    int err = 0;

    err = dfu_request_param_setup(cli, REQ_STATUS, ctx->addr, (uint8_t *)rsp);
    if (err) {
        return err;
    }

    struct os_mbuf *buf = BT_MESH_MODEL_BUF(MODEL_OP(DFU_UPDATE_GET), BT_MESHDFU_UPDATE_BUF_DEFINE);
    bt_mesh_model_msg_init(buf, MODEL_OP(DFU_UPDATE_GET));

    err = bt_mesh_model_send(cli->mod, ctx, buf, NULL, NULL);
    os_mbuf_free_chain(buf);
    if (err) {
        cli->req.type = REQ_NONE;
        return err;
    }

    return dfu_request_param_wait(cli, K_MSEC(dfu_cli_timeout));
}

int32_t bt_mesh_dfu_client_timeout_get(void)
{
    return dfu_cli_timeout;
}

void bt_mesh_dfu_client_timeout_set(int32_t t)
{
    dfu_cli_timeout = t;
}

/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 *
 * Description: BLOB Model Implementation.
 *
 * History:
 * 2025-05-01, Create file.
 */

#include <stdio.h>
#include "syscfg/syscfg.h"
#include "os/os_mbuf.h"
#include "os_time.h"
#include "mesh/glue.h"
#include "mesh/mesh.h"
#include "access.h"
#include "model/model_opcode.h"
#include "model/model_def.h"
#include "model_common.h"
#include "mesh_ota_common.h"
#include "blob.h"
#include "blob_common.h"
#include "ble_mesh_ota.h"
#include "model/blob_server.h"
#include "model/blob_client.h"

/* The Maximum BLOB Poll Interval - T_MBPI */
#define BLOB_POLL_TIME_MAX_SECS 30

#define BLOCK_REPORT_TIME_MSEC ((BLOB_POLL_TIME_MAX_SECS * 2 + 7) * 1000)

#define IS_SENDING_CHUNKS_BY_PULL(client) \
    ((client)->state == BT_MESH_BLOB_CLIENT_STATE_BLOCK_SEND && \
    (client)->transfer->mode == BT_MESH_BLOB_TRANSFER_MODE_PULL)

#define IS_UNICAST_MODE(client) ((client)->inputs->group == BT_MESH_ADDR_UNASSIGNED || (client)->tx.ctx.b_force_unicast)

#ifndef CONFIG_BLE_MESH_PTS_MODE
BUILD_ASSERT((BLOB_TRANSFER_STATUS_MSG_MAXLEN + BT_MESH_MODEL_OP_LEN(MODEL_OP(BLOB_TRANSFER_STATUS)) +
                 BT_MESH_MIC_SHORT) <= BT_MESH_RX_SDU_MAX,
    "BLOB_TRANSFER_STATUS message not fit into the BT_MESH_RX_SDU_MAX.");
#endif

BUILD_ASSERT((BLOB_BLOCK_REPORT_STATUS_MSG_MAXLEN + BT_MESH_MODEL_OP_LEN(MODEL_OP(BLOB_BLOCK_REPORT)) +
                 BT_MESH_MIC_SHORT) <= BT_MESH_RX_SDU_MAX,
    "BLOB_BLOCK_REPORT message not fit into the BT_MESH_RX_SDU_MAX.");

BUILD_ASSERT((BLOB_BLOCK_STATUS_MSG_MAXLEN + BT_MESH_MODEL_OP_LEN(MODEL_OP(BLOB_BLOCK_STATUS)) + BT_MESH_MIC_SHORT) <=
                 BT_MESH_RX_SDU_MAX,
    "BLOB_BLOCK_STATUS message not fit into the BT_MESH_RX_SDU_MAX.");

/* BLOB MSG PLAYLOAD LENGTH */
#define BLOB_INFO_GET_LEN               0
#define BLOB_TRANSFER_START_LEN         16
#define BLOB_TRANSFER_GET_LEN           0
#define BLOB_TRANSFER_CANCEL_LEN        8
#define BLOB_BLOCK_START_LEN            4
#define BLOB_CHUNK_LEN                  BT_MESH_RX_SDU_MAX
#define BLOB_BLOCK_GET_LEN              0

/* limits */
#define BLOB_BLOCK_SIZE_LOG_LIMIT_MIN   0x06
#define BLOB_BLOCK_SIZE_LOG_LIMIT_MAX   0x20
#define BLOB_MTU_SIZE_LIMIT_MIN         0x14
#define BLOB_ID_LEN 8

/* 目标节点列表轮询 */
#define FOR_EACH_TARGET(client, target) \
    SYS_SLIST_FOR_EACH_CONTAINER((sys_slist_t *)&(client)->inputs->targets, target, n)

struct block_status_t {
    enum bt_mesh_blob_status_t status;
    enum bt_mesh_blob_chunks_missing_how missing_how;
    struct bt_mesh_blob_block_t block;
};

static struct bt_mesh_blob_target_t *target_next(
    struct bt_mesh_blob_client_t *client, struct bt_mesh_blob_target_t **current);
static void client_cancel_transfer(struct bt_mesh_blob_client_t *client);

static void client_reset_state(struct bt_mesh_blob_client_t *client)
{
    k_work_cancel_delayable(&client->tx.timeout_retry);
    client->state = BT_MESH_BLOB_CLIENT_STATE_NONE;
    client->tx.b_sending = 0;
    client->transfer = NULL;
    client->tx.cli_timestamp = 0LL;
    client->tx.ctx.b_inited = 0;
}

static void client_start_retry_timer(struct bt_mesh_blob_client_t *client)
{
    int64_t next_timeout;

    if (IS_SENDING_CHUNKS_BY_PULL(client)) {
        struct bt_mesh_blob_target_t *target = NULL;
        int64_t next_timeout_ms = client->tx.cli_timestamp;

        if (client->inputs == NULL) {
            BT_ERR("inputs is null");
            return;
        }
        FOR_EACH_TARGET(client, target) {
            if (target->status == BT_MESH_BLOB_SUCCESS && !target->b_proc_completed &&
                target->pull->block_report_timestamp < next_timeout_ms) {
                next_timeout_ms = target->pull->block_report_timestamp;
            }
        }

        /* pull: cli_timestamp and block_report_timestamp.
         */
        next_timeout_ms -= k_uptime_get();
        next_timeout = next_timeout_ms <= 0 ? K_NO_WAIT : K_MSEC(next_timeout_ms);
    } else {
        next_timeout = K_MSEC(CLIENT_TIMEOUT_MSEC(client) / MYNEWT_VAL(BLE_MESH_BLOB_CLI_BLOCK_RETRIES));
    }

    (void)k_work_reschedule(&client->tx.timeout_retry, next_timeout);
}

static struct bt_mesh_blob_target_t *client_get_target_by_addr(struct bt_mesh_blob_client_t *client, uint16_t addr)
{
    struct bt_mesh_blob_target_t *target = NULL;

    if (client->inputs == NULL) {
        BT_ERR("inputs is null");
        return NULL;
    }
    FOR_EACH_TARGET(client, target) {
        if (addr == target->addr) {
            return target;
        }
    }

    BT_ERR("target not found: 0x%04x", addr);
    return NULL;
}

static uint32_t targets_reset(struct bt_mesh_blob_client_t *client)
{
    uint32_t count = 0;
    struct bt_mesh_blob_target_t *target = NULL;

    if (client->inputs == NULL) {
        BT_ERR("inputs is null");
        return count;
    }
    FOR_EACH_TARGET(client, target) {
        if (target->status == BT_MESH_BLOB_SUCCESS) {
            count++;
            target->b_acked = 0U;
        }
    }

    return count;
}

static void target_drop(
    struct bt_mesh_blob_client_t *client, struct bt_mesh_blob_target_t *target, enum bt_mesh_blob_status_t status)
{
    BT_WARN("Dropping target: 0x%04x, stattus: %u", target->addr, status);

    target->status = status;
    if (client->cb && client->cb->on_lost_target) {
        client->cb->on_lost_target(client, target, status);
    }
}

static bool targets_active(struct bt_mesh_blob_client_t *client)
{
    struct bt_mesh_blob_target_t *target = NULL;

    if (client->inputs == NULL) {
        BT_ERR("inputs is null");
        return false;
    }
    FOR_EACH_TARGET(client, target) {
        if (target->status == BT_MESH_BLOB_SUCCESS) {
            return true;
        }
    }

    return false;
}

static bool targets_timedout(struct bt_mesh_blob_client_t *client)
{
    struct bt_mesh_blob_target_t *target = NULL;

    if (client->inputs == NULL) {
        BT_ERR("inputs is null");
        return true;
    }
    FOR_EACH_TARGET(client, target) {
        if (target->b_timedout) {
            return true;
        }
    }

    return false;
}

static int stream_open(struct bt_mesh_blob_client_t *client)
{
    if (!client->stream->on_open) {
        return 0;
    }

    return client->stream->on_open(client->stream, client->transfer, BT_MESH_BLOB_STREAM_READ);
}

static void stream_close(struct bt_mesh_blob_client_t *client)
{
    if (!client->stream->on_close) {
        return;
    }

    client->stream->on_close(client->stream, client->transfer);
}

static uint16_t next_missing_chunk(struct bt_mesh_blob_client_t *client, const uint8_t *missing_chunks, uint16_t idx)
{
    uint16_t remaining_idx = idx;
    for (; remaining_idx < client->block.chunks_count; remaining_idx++) {
        if (missing_chunks_get(missing_chunks, remaining_idx)) {
            break;
        }
    }

    return remaining_idx;
}

/* Used in pull mode */
static void update_missing_chunks(struct bt_mesh_blob_client_t *client)
{
    struct bt_mesh_blob_target_t *target;

    memset_s(client->block.missing_chunks, sizeof(client->block.missing_chunks),
        0, sizeof(client->block.missing_chunks));
    if (client->inputs == NULL) {
        BT_ERR("inputs is null");
        return;
    }
    FOR_EACH_TARGET(client, target) {
        if (target->b_proc_completed || target->b_timedout) {
            continue;
        }

        for (uint32_t idx = 0; idx < client->block.chunks_count; idx++) {
            bool missing = missing_chunks_get(client->block.missing_chunks, idx) ||
                           missing_chunks_get(target->pull->missing_chunks, idx);
            missing_chunks_set(client->block.missing_chunks, idx, missing);
        }
    }
}

static inline uint32_t chunk_size(
    const struct bt_mesh_blob_transfer_t *transfer, const struct bt_mesh_blob_block_t *block, uint16_t chunk_idx)
{
    if ((chunk_idx == block->chunks_count - 1) && (block->size % transfer->chunk_size)) {
        return block->size % transfer->chunk_size;
    }

    return transfer->chunk_size;
}

/** chunk分片序号解码: 参看 MshMBTv1.0: 5.3.8 and Table 4.9 */
#define CHK_IDX_SIZEOF 2
#define CHK_IDX_SHIFT12 12
#define CHK_IDX_SHIFT6 6
static int chunk_index_decode(struct os_mbuf *msg)
{
    uint16_t idx;
    uint8_t byte;

    if (msg->om_len == 0) {
        return -EINVAL;
    }

    byte = net_buf_simple_pull_u8(msg);
    /* utf-8 decoding */
    if ((byte & 0xf0) == 0xe0) { /* 0x800 - 0xffff */
        if (msg->om_len < CHK_IDX_SIZEOF) {
            return -EINVAL;
        }

        idx = (byte & 0x0f) << CHK_IDX_SHIFT12;
        idx |= (net_buf_simple_pull_u8(msg) & 0x3f) << CHK_IDX_SHIFT6;
        idx |= (net_buf_simple_pull_u8(msg) & 0x3f);
    } else if ((byte & 0xe0) == 0xc0) { /* 0x80 - 0x7ff */
        if (msg->om_len < 1) {
            return -EINVAL;
        }

        idx = (byte & 0x1f) << CHK_IDX_SHIFT6;
        idx |= (net_buf_simple_pull_u8(msg) & 0x3f);
    } else { /* 0x00 - 0x7f */
        idx = byte & 0x7f;
    }

    return idx;
}

static void block_set(struct bt_mesh_blob_client_t *client, uint16_t block_idx)
{
    client->block.index = block_idx;
    client->block.offset = block_idx * (1UL << client->transfer->block_size_logarithm);
    client->block.size = block_size_by_index(client->transfer->size, client->transfer->block_size_logarithm, block_idx);
    client->block.chunks_count = DIV_ROUND_UP(client->block.size, client->transfer->chunk_size);

    if (client->transfer->mode == BT_MESH_BLOB_TRANSFER_MODE_PUSH) {
        missing_chunks_set_all(&client->block);
    } else {
        struct bt_mesh_blob_target_t *target;

        /* pull mode */
        memset_s(client->block.missing_chunks, sizeof(client->block.missing_chunks), 0,
            sizeof(client->block.missing_chunks));

        if (client->inputs == NULL) {
            BT_ERR("inputs is null");
            return;
        }
        FOR_EACH_TARGET(client, target) {
            uint32_t size_missing_chunks = sizeof(target->pull->missing_chunks);
            memset_s(target->pull->missing_chunks, size_missing_chunks, 0, size_missing_chunks);
        }
    }

    BT_INFO("%u size: %u chunks: %u", block_idx, client->block.size, client->block.chunks_count);
}

static void suspend(struct bt_mesh_blob_client_t *client)
{
    client->state = BT_MESH_BLOB_CLIENT_STATE_SUSPENDED;

    if (client->cb && client->cb->on_suspended) {
        client->cb->on_suspended(client);
    }
}

static void end(struct bt_mesh_blob_client_t *client, bool success)
{
    const struct bt_mesh_blob_transfer_t *transfer = client->transfer;

    BT_INFO("%u", success);

    stream_close(client);
    client_reset_state(client);
    if (client->cb && client->cb->on_end) {
        client->cb->on_end(client, transfer, success);
    }
}

static enum bt_mesh_blob_status_t caps_adjust(
    struct bt_mesh_blob_client_t *client, const struct bt_mesh_blob_client_capabilities_t *in)
{
    if (!(in->modes & client->caps.modes)) {
        /* PULL not supported currently, by setting MYNEWT_VAL_BLE_MESH_BLOB_TRANSFER_MODE in syscfg.h */
        return BT_MESH_BLOB_ERR_UNSUPPORTED_MODE;
    }

    if ((in->min_block_size_logarithm > client->caps.max_block_size_logarithm) ||
        (in->max_block_size_logarithm < client->caps.min_block_size_logarithm)) {
        BT_WARN("wrong caps of INVALID_BLOCK_SIZE");
        return BT_MESH_BLOB_ERR_INVALID_BLOCK_SIZE;
    }

    client->caps.min_block_size_logarithm = MAX(client->caps.min_block_size_logarithm, in->min_block_size_logarithm);
    client->caps.max_block_size_logarithm = MIN(client->caps.max_block_size_logarithm, in->max_block_size_logarithm);
    client->caps.max_chunks = MIN(client->caps.max_chunks, in->max_chunks);
    client->caps.mtu_size = MIN(client->caps.mtu_size, in->mtu_size);
    client->caps.max_chunk_size = MIN(client->caps.max_chunk_size, in->max_chunk_size);
    client->caps.modes &= in->modes;
    client->caps.max_size = MIN(client->caps.max_size, in->max_size);

    return BT_MESH_BLOB_SUCCESS;
}

/*******************************************************************************
 * 发送状态机实现
 *
 ******************************************************************************/

static struct bt_mesh_blob_target_t *target_next(
    struct bt_mesh_blob_client_t *client, struct bt_mesh_blob_target_t **current)
{
    if (*current) {
        *current = SYS_SLIST_PEEK_NEXT_CONTAINER(*current, n);
    } else {
        *current = SYS_SLIST_PEEK_HEAD_CONTAINER((sys_slist_t *)&client->inputs->targets, *current, n);
    }

    while (*current) {
        if ((*current)->b_acked || (*current)->b_proc_completed || (*current)->status != BT_MESH_BLOB_SUCCESS ||
            (*current)->b_timedout || (*current)->b_skip) {
            goto next;
        }

        if (IS_SENDING_CHUNKS_BY_PULL(client) &&
            (k_uptime_get() < (*current)->pull->block_report_timestamp ||
                !missing_chunks_get((*current)->pull->missing_chunks, client->chunk_idx))) {
            goto next;
        } else {
            break;
        }

    next:
        *current = SYS_SLIST_PEEK_NEXT_CONTAINER(*current, n);
    }

    return *current;
}

static void target_send(struct bt_mesh_blob_client_t *client)
{
    client->tx.b_sending = 1U;

    if (client->state != BT_MESH_BLOB_CLIENT_STATE_BLOCK_SEND && client->state != BT_MESH_BLOB_CLIENT_STATE_CAPS_GET &&
        client->tx.n_retries < MYNEWT_VAL(BLE_MESH_BLOB_CLI_BLOCK_RETRIES)) {
        client->tx.ctx.b_force_unicast = true;
    } else {
        client->tx.ctx.b_force_unicast = false;
    }

    if (IS_UNICAST_MODE(client)) {
        client->tx.ctx.send(client, client->tx.target->addr);
    } else {
        client->tx.ctx.send(client, client->inputs->group);
    }
}

static void broadcast_complete(struct bt_mesh_blob_client_t *client)
{
    BT_DBG("%s", client->tx.b_cancelled ? "cancelling" : "continuing");

    client->tx.ctx.b_inited = 0;
    k_work_cancel_delayable(&client->tx.timeout_retry);

    if (client->tx.b_cancelled) {
        client_cancel_transfer(client);
    } else {
        if (client->tx.ctx.next == NULL) {
            BT_ERR("No next callback for client");
            return;
        }
        client->tx.ctx.next(client);
    }
}

static void tx_complete(struct ble_npl_event *work)
{
    struct bt_mesh_blob_client_t *client = ble_npl_event_get_arg(work);
    if (client == NULL) {
        BT_ERR("tx_complete! bt_mesh_blob_client_t is NULL.");
        return;
    }

    BT_DBG("ms_delay:%u, init:%u, sending:%u, cancelled:%u,  unicast:%u, acked:%u, pending_blocks: %u",
        K_MSEC(client->tx.ctx.ms_delay_between_send), client->tx.ctx.b_inited, client->tx.b_sending,
        client->tx.b_cancelled, IS_UNICAST_MODE(client), client->tx.ctx.b_acked, client->tx.n_pending_blocks);

    if (!client->tx.ctx.b_inited || !client->tx.b_sending) {
        return;
    }

    client->tx.b_sending = 0U;

    if (client->tx.b_cancelled) {
        broadcast_complete(client);
        return;
    }

    if (client->tx.ctx.send_complete) {
        client->tx.ctx.send_complete(client, client->tx.target->addr);
    }

    if (IS_UNICAST_MODE(client) && target_next(client, &client->tx.target)) {
        BT_DBG("unicast and having next target => target_send()");
        target_send(client);
        return;
    }

    if (client->tx.ctx.b_acked && client->tx.n_pending_blocks) {
        client_start_retry_timer(client);
        return;
    }

    broadcast_complete(client);
}

static void drop_remaining_targets(struct bt_mesh_blob_client_t *client)
{
    struct bt_mesh_blob_target_t *target;

    BT_INFO("");

    client->tx.n_pending_blocks = 0;

    if (client->inputs == NULL) {
        BT_ERR("inputs is null");
        return;
    }
    FOR_EACH_TARGET(client, target) {
        if (!target->b_acked && !target->b_timedout && !target->b_proc_completed && !target->b_skip) {
            target->b_timedout = 1U;
            target_drop(client, target, BT_MESH_BLOB_ERR_INTERNAL);
        }
    }

    /* 更新缺失chunk分片：将踢出的目标节点中缺失分片排除掉 */
    if (IS_SENDING_CHUNKS_BY_PULL(client)) {
        update_missing_chunks(client);
    }
}

static void retry_timeout(struct ble_npl_event *work)
{
    struct bt_mesh_blob_client_t *client = ble_npl_event_get_arg(work);
    if (client == NULL) {
        BT_ERR("timeout! bt_mesh_blob_client_t is NULL.");
        return;
    }

    if (IS_SENDING_CHUNKS_BY_PULL(client)) {
        if (k_uptime_get() >= client->tx.cli_timestamp) {
            BT_INFO("Transfer/pull timed out.");

            if (!client->tx.ctx.b_optional) {
                drop_remaining_targets(client);
            }
        }

        broadcast_complete(client);
        return;
    }

    BT_INFO("%u", client->tx.n_retries);

    client->tx.n_retries--;
    client->tx.target = NULL;

    if (client->tx.b_sending) {
        BT_ERR("Transfer still in progress, cannot retry");
        return;
    }
    if (!client->tx.ctx.b_inited) {
        BT_ERR("Transfer context not initialized");
        return;
    }

    if (!client->tx.n_retries) {
        BT_INFO("Transfer timed out.");

        if (!client->tx.ctx.b_optional) {
            drop_remaining_targets(client);
        }

        broadcast_complete(client);
        return;
    }

    if (!client->tx.ctx.b_acked || !target_next(client, &client->tx.target) || client->tx.b_cancelled) {
        broadcast_complete(client);
        return;
    }

    target_send(client);
}

void blob_client_broadcast(struct bt_mesh_blob_client_t *client, const struct blob_client_broadcast_context_t *ctx)
{
    if (client->tx.ctx.b_inited || client->tx.b_sending) {
        BT_ERR("BLOB cli busy");
        return;
    }

    client->tx.b_cancelled = 0U;
    client->tx.n_retries = MYNEWT_VAL(BLE_MESH_BLOB_CLI_BLOCK_RETRIES);
    client->tx.ctx = *ctx;
    client->tx.ctx.b_inited = 1U;

    client->tx.n_pending_blocks = targets_reset(client);

    if (client->state != BT_MESH_BLOB_CLIENT_STATE_BLOCK_SEND) {
        BT_INFO("%u targets", client->tx.n_pending_blocks);
    }

    client->tx.target = NULL;
    if (!target_next(client, &client->tx.target)) {
        BT_INFO("No active targets");
        broadcast_complete(client);
        return;
    }
    target_send(client);
}

void blob_client_broadcast_tx_completed(struct bt_mesh_blob_client_t *client)
{
    k_work_schedule(&client->tx.timeout_complete, K_MSEC(client->tx.ctx.ms_delay_between_send));
}

void blob_client_broadcast_responded(struct bt_mesh_blob_client_t *client, struct bt_mesh_blob_target_t *target)
{
    if (!target || target->b_acked) {
        return;
    }

    BT_DBG("0x%04x, pending: %d", target->addr, client->tx.n_pending_blocks);

    target->b_acked = 1U;

    if (!--client->tx.n_pending_blocks && !client->tx.b_sending) {
        broadcast_complete(client);
    }
}

void blob_client_broadcast_abort(struct bt_mesh_blob_client_t *client)
{
    if (!client->tx.ctx.b_inited) {
        return;
    }

    if ((client)->state >= BT_MESH_BLOB_CLIENT_STATE_START) {
        stream_close(client);
    }

    client_reset_state(client);
}

static void send_start(uint16_t duration, int err, void *cb_data);
static void send_end(int err, void *user_data);

static int transmit(struct bt_mesh_blob_client_t *client, uint16_t addr, struct os_mbuf *buf)
{
    static const struct bt_mesh_send_cb send_cb = {
        .start = send_start,
        .end = send_end,
    };
    struct bt_mesh_msg_ctx ctx = {
        .app_idx = client->inputs->app_idx,
        .addr = addr,
        .send_ttl = client->inputs->ttl,
    };
    int err;

    err = bt_mesh_model_send(client->mod, &ctx, buf, &send_cb, client);
    BT_DBG("addr: 0x%04x b_force_unicast:%u OpCode: 0x%0x 0x%0x err:%d",
        ctx.addr, client->tx.ctx.b_force_unicast, buf->om_data[0], buf->om_data[0], err);
    if (err) {
        BT_ERR("Send err: %d", err);
        send_end(err, client);
        return err;
    }

    return 0;
}

static void send_start(uint16_t duration, int err, void *cb_data)
{
    struct bt_mesh_blob_client_t *client = cb_data;
    int64_t xtime = k_uptime_get();

    if (err) {
        BT_ERR("TX Start failed: %d", err);
        send_end(err, cb_data);
    }
}

static void send_end(int err, void *user_data)
{
    struct bt_mesh_blob_client_t *client = user_data;

    int64_t xtime = k_uptime_get();

    if (!client->tx.ctx.b_inited) {
        return;
    }

    blob_client_broadcast_tx_completed(client);
}

/*******************************************************************************
 * TX：发送
 ******************************************************************************/

static void ctx_send_info_get(struct bt_mesh_blob_client_t *client, uint16_t dst)
{
    struct os_mbuf *msg = BT_MESH_MODEL_BUF(MODEL_OP(BLOB_INFO_GET), BLOB_INFO_GET_LEN);
    bt_mesh_model_msg_init(msg, MODEL_OP(BLOB_INFO_GET));

    bt_mesh_ota_stat(MODEL_OP(BLOB_INFO_GET));
    transmit(client, dst, msg);
    os_mbuf_free_chain(msg);
}

#define TRANSFER_MODE_SHIFT6 6
#define TRANSFER_ID_SHIFT32 32
static void ctx_send_transfer_start(struct bt_mesh_blob_client_t *client, uint16_t dst)
{
    struct os_mbuf *msg = BT_MESH_MODEL_BUF(MODEL_OP(BLOB_TRANSFER_START), BLOB_TRANSFER_START_LEN);
    bt_mesh_model_msg_init(msg, MODEL_OP(BLOB_TRANSFER_START));
    net_buf_simple_add_u8(msg, client->transfer->mode << TRANSFER_MODE_SHIFT6);
    net_buf_simple_add_le32(msg, (uint32_t)client->transfer->id);
    net_buf_simple_add_le32(msg, (uint32_t)(client->transfer->id >> TRANSFER_ID_SHIFT32));
    net_buf_simple_add_le32(msg, client->transfer->size);
    net_buf_simple_add_u8(msg, client->transfer->block_size_logarithm);
    net_buf_simple_add_le16(msg, BT_MESH_TX_SDU_MAX);

    bt_mesh_ota_stat(MODEL_OP(BLOB_TRANSFER_START));
    transmit(client, dst, msg);
    os_mbuf_free_chain(msg);
}

static void ctx_send_transfer_get(struct bt_mesh_blob_client_t *client, uint16_t dst)
{
    struct os_mbuf *msg = BT_MESH_MODEL_BUF(MODEL_OP(BLOB_TRANSFER_GET), BLOB_TRANSFER_GET_LEN);
    bt_mesh_model_msg_init(msg, MODEL_OP(BLOB_TRANSFER_GET));

    bt_mesh_ota_stat(MODEL_OP(BLOB_TRANSFER_GET));
    transmit(client, dst, msg);
    os_mbuf_free_chain(msg);
}

static void ctx_send_transfer_cancel(struct bt_mesh_blob_client_t *client, uint16_t dst)
{
    struct os_mbuf *msg = BT_MESH_MODEL_BUF(MODEL_OP(BLOB_TRANSFER_CANCEL), BLOB_TRANSFER_CANCEL_LEN);
    bt_mesh_model_msg_init(msg, MODEL_OP(BLOB_TRANSFER_CANCEL));
    net_buf_simple_add_le32(msg, (uint32_t)client->transfer->id);
    net_buf_simple_add_le32(msg, (uint32_t)(client->transfer->id >> TRANSFER_ID_SHIFT32));

    transmit(client, dst, msg);
    os_mbuf_free_chain(msg);
}

static void ctx_send_block_start(struct bt_mesh_blob_client_t *client, uint16_t dst)
{
    struct os_mbuf *msg = BT_MESH_MODEL_BUF(MODEL_OP(BLOB_BLOCK_START), BLOB_BLOCK_START_LEN);
    bt_mesh_model_msg_init(msg, MODEL_OP(BLOB_BLOCK_START));
    net_buf_simple_add_le16(msg, client->block.index);
    net_buf_simple_add_le16(msg, client->transfer->chunk_size);

    bt_mesh_ota_stat(MODEL_OP(BLOB_BLOCK_START));
    transmit(client, dst, msg);
    os_mbuf_free_chain(msg);
}

static void ctx_send_chunk(struct bt_mesh_blob_client_t *client, uint16_t dst)
{
    struct bt_mesh_blob_chunk_t chunk;
    int err;

    chunk.size = chunk_size(client->transfer, &client->block, client->chunk_idx);
    chunk.offset = client->transfer->chunk_size * client->chunk_idx;

    struct os_mbuf *msg = BT_MESH_MODEL_BUF(MODEL_OP(BLOB_CHUNK), 2 + chunk.size); // 2: idx
    bt_mesh_model_msg_init(msg, MODEL_OP(BLOB_CHUNK));
    net_buf_simple_add_le16(msg, client->chunk_idx);
    chunk.data = net_buf_simple_add(msg, chunk.size);
    err = client->stream->on_read(client->stream, client->transfer, &client->block, &chunk);
    if (err || client->state == BT_MESH_BLOB_CLIENT_STATE_NONE) {
        bt_mesh_blob_client_cancel(client);
    } else {
        bt_mesh_ota_stat(MODEL_OP(BLOB_CHUNK));
        transmit(client, dst, msg);
    }
    os_mbuf_free_chain(msg);
}

static void ctx_send_block_get(struct bt_mesh_blob_client_t *client, uint16_t dst)
{
    struct os_mbuf *msg = BT_MESH_MODEL_BUF(MODEL_OP(BLOB_BLOCK_GET), BLOB_BLOCK_GET_LEN);
    bt_mesh_model_msg_init(msg, MODEL_OP(BLOB_BLOCK_GET));
    int err = 0;

    bt_mesh_ota_stat(MODEL_OP(BLOB_BLOCK_GET));
    transmit(client, dst, msg);
    os_mbuf_free_chain(msg);
}

/**
 * impl of STATE MACHINE
 **/
static void on_caps_collected(struct bt_mesh_blob_client_t *client);
static void on_block_start(struct bt_mesh_blob_client_t *client);
static void on_chunk_send(struct bt_mesh_blob_client_t *client);
static void on_block_check(struct bt_mesh_blob_client_t *client);
static void on_block_check_end(struct bt_mesh_blob_client_t *client);
static void on_block_report_wait(struct bt_mesh_blob_client_t *client);
static void on_chunk_send_end(struct bt_mesh_blob_client_t *client);
static void on_confirm_transfer(struct bt_mesh_blob_client_t *client);
static void on_transfer_complete(struct bt_mesh_blob_client_t *client);

static void caps_get(struct bt_mesh_blob_client_t *client)
{
    const struct blob_client_broadcast_context_t ctx = {
        .send = ctx_send_info_get,
        .next = on_caps_collected,
        .b_acked = true,
    };

    client->state = BT_MESH_BLOB_CLIENT_STATE_CAPS_GET;
    blob_client_broadcast(client, &ctx);
}

static void on_caps_collected(struct bt_mesh_blob_client_t *client)
{
    struct bt_mesh_blob_target_t *target;
    bool success = false;

    client->state = BT_MESH_BLOB_CLIENT_STATE_NONE;

    client_reset_state(client);

    if (client->inputs == NULL) {
        BT_ERR("inputs is null");
        return;
    }
    FOR_EACH_TARGET(client, target) {
        if (target->status == BT_MESH_BLOB_SUCCESS) {
            success = true;
            break;
        }
    }

    BT_DBG("max block %d, max chunk %d, max chunks %d",
        client->caps.max_block_size_logarithm, client->caps.max_chunk_size, client->caps.max_chunks);
 
    if (client->caps.max_block_size_logarithm < BLOB_BLOCK_SIZE_LOG_LIMIT_MIN) {
        BT_WARN("over adjusted: reset to BLOB_BLOCK_SIZE_LOG_LIMIT_MIN");
        client->caps.max_block_size_logarithm = BLOB_BLOCK_SIZE_LOG_LIMIT_MIN;
    }

    if (client->cb && client->cb->on_caps) {
        client->cb->on_caps(client, success ? &client->caps : NULL);
    }
}

#define PTS_CANCEL_HOLD_TIME 10
static int transfer_start(struct bt_mesh_blob_client_t *client)
{
    const struct blob_client_broadcast_context_t ctx = {
        .send = ctx_send_transfer_start,
        .next = atomic_test_bit(bt_mesh.flags, BT_MESH_MBT_TEST) ? client_cancel_transfer : on_block_start,
        .b_acked = true,
    };
    int err;

    err = stream_open(client);
    if (err) {
        return -EIO;
    }

    if (atomic_test_and_clear_bit(bt_mesh.flags, BT_MESH_MBT_TEST)) {
        BT_WARN("pts: holding...");
        k_sleep(K_MSEC(PTS_CANCEL_HOLD_TIME));
    }

    client->state = BT_MESH_BLOB_CLIENT_STATE_START;

    blob_client_broadcast(client, &ctx);
    return 0;
}

static void on_block_start(struct bt_mesh_blob_client_t *client)
{
    const struct blob_client_broadcast_context_t ctx = {
        .send = ctx_send_block_start,
        .next = on_chunk_send,
        .b_acked = true,
    };
    struct bt_mesh_blob_target_t *target;

    if (!targets_active(client)) {
        if (targets_timedout(client)) {
            suspend(client);
            return;
        }

        end(client, false);
        return;
    }

    BT_INFO("%u (%u chunks, %u/%u)", client->block.index, client->block.chunks_count,
        client->block.index + 1, client->block_count);

    if (client->block.index == 0) {
        bt_mesh_ota_result(ble_npl_time_get_ms() / MSEC_PER_SEC, 0, OTA_START);  // 1000:ms
    }

    client->chunk_idx = 0;
    client->state = BT_MESH_BLOB_CLIENT_STATE_BLOCK_START;
   
    client->tx.cli_timestamp = 0LL;

    if (client->inputs == NULL) {
        BT_ERR("inputs is null");
        return;
    }
    FOR_EACH_TARGET(client, target) {
        target->b_proc_completed = 0U;

        if (client->transfer->mode == BT_MESH_BLOB_TRANSFER_MODE_PULL) {
            target->pull->block_report_timestamp = 0LL;
        }
    }

    if (client->stream->on_block_start) {
        client->stream->on_block_start(client->stream, client->transfer, &client->block);
        if (client->state == BT_MESH_BLOB_CLIENT_STATE_NONE) {
            return;
        }
    }
    blob_client_broadcast(client, &ctx);
}

static void chunk_tx_complete(struct bt_mesh_blob_client_t *client, uint16_t dst)
{
    if (client->transfer->mode != BT_MESH_BLOB_TRANSFER_MODE_PULL) {
        return;
    }

    uint16_t chunk_idx = next_missing_chunk(client, client->tx.target->pull->missing_chunks, client->chunk_idx + 1);
    if (chunk_idx < client->block.chunks_count) {
        /* Will send more chunks to this target in this iteration. */
        return;
    }

    struct bt_mesh_blob_target_t *target;
    int64_t timestamp = k_uptime_get() + BLOCK_REPORT_TIME_MSEC;

    if (!IS_UNICAST_MODE(client)) {
        /* group: reset timestamp for all targets after all chunks are sent */
        if (client->inputs == NULL) {
            BT_ERR("inputs is null");
            return;
        }
        FOR_EACH_TARGET(client, target) {
            target->pull->block_report_timestamp = timestamp;
        }
        return;
    }

    client->tx.target->pull->block_report_timestamp = timestamp;
}

static void on_chunk_send(struct bt_mesh_blob_client_t *client)
{
    struct blob_client_broadcast_context_t ctx = {
        .send = ctx_send_chunk,
        .next = on_chunk_send_end,
        .b_acked = false,
        .ms_delay_between_send = client->chunk_interval_ms,
    };

    if (client->transfer->mode == BT_MESH_BLOB_TRANSFER_MODE_PULL) {
        ctx.send_complete = chunk_tx_complete;
    }

    if (!targets_active(client)) {
        if (targets_timedout(client)) {
            suspend(client);
            return;
        }

        end(client, false);
        return;
    }

    int64_t xtime = k_uptime_get();
    BT_INFO("ms:%llu idx:%u/%u@%u size: %u", xtime,
        client->chunk_idx + 1,
        client->block.chunks_count, client->block.index,
        chunk_size(client->transfer, &client->block, client->chunk_idx));

    client->state = BT_MESH_BLOB_CLIENT_STATE_BLOCK_SEND;
    blob_client_broadcast(client, &ctx);
}

static void on_chunk_send_end(struct bt_mesh_blob_client_t *client)
{
    if (client->transfer->mode == BT_MESH_BLOB_TRANSFER_MODE_PUSH) {
        missing_chunks_set(client->block.missing_chunks, client->chunk_idx, false);
    }

    client->chunk_idx = next_missing_chunk(client, client->block.missing_chunks, client->chunk_idx + 1);
    if (client->chunk_idx < client->block.chunks_count) {
        on_chunk_send(client);
        return;
    }

    if (client->transfer->mode == BT_MESH_BLOB_TRANSFER_MODE_PUSH) {
        on_block_check(client);
    } else {
        on_block_report_wait(client);
    }
}

/* (block_check - block_check_end) only for Push mode.
 */
static void on_block_check(struct bt_mesh_blob_client_t *client)
{
    const struct blob_client_broadcast_context_t ctx = {
        .send = ctx_send_block_get,
        .next = on_block_check_end,
        .b_acked = true,
    };

    client->state = BT_MESH_BLOB_CLIENT_STATE_BLOCK_CHECK;

    BT_INFO("");

    blob_client_broadcast(client, &ctx);
}

static void on_block_report_wait(struct bt_mesh_blob_client_t *client)
{
    const struct blob_client_broadcast_context_t ctx = {
        .next = on_block_check_end,
        .b_acked = false,
    };

    /* Check if all servers already confirmed */
    if (next_missing_chunk(client, client->block.missing_chunks, 0) >= client->block.chunks_count) {
        on_block_check_end(client);
        return;
    }

    BT_INFO("Waiting for partial block report...");
    client->tx.ctx = ctx;

    /* Start Client Timeout Timer for the first time. */
    if (!client->tx.cli_timestamp) {
        client->tx.cli_timestamp = k_uptime_get() + CLIENT_TIMEOUT_MSEC(client);
    }

    client_start_retry_timer(client);
}

static void on_block_check_end(struct bt_mesh_blob_client_t *client)
{
    if (!targets_active(client)) {
        if (targets_timedout(client)) {
            suspend(client);
            return;
        }

        end(client, false);
        return;
    }

    client->chunk_idx = next_missing_chunk(client, client->block.missing_chunks, 0);
    if (client->chunk_idx < client->block.chunks_count) {
        on_chunk_send(client);
        return;
    }

    BT_INFO("No more missing for this block %u", client->block.index);

    if (client->stream->on_block_end) {
        client->stream->on_block_end(client->stream, client->transfer, &client->block);
        if (client->state == BT_MESH_BLOB_CLIENT_STATE_NONE) {
            return;
        }
    }

    if (client->block.index == client->block_count - 1) {
        struct bt_mesh_blob_target_t *target;

        if (client->inputs == NULL) {
            BT_ERR("inputs is null");
            return;
        }
        FOR_EACH_TARGET(client, target) {
            target->b_proc_completed = 0U;
        }

        on_confirm_transfer(client);
        return;
    }

    block_set(client, client->block.index + 1);
    on_block_start(client);
}

static void on_confirm_transfer(struct bt_mesh_blob_client_t *client)
{
    const struct blob_client_broadcast_context_t ctx = {
        .send = ctx_send_transfer_get,
        .next = on_transfer_complete,
        .b_acked = true,
    };

    BT_INFO("");

    client->state = BT_MESH_BLOB_CLIENT_STATE_TRANSFER_CHECK;

    blob_client_broadcast(client, &ctx);
}

static void on_progress_checked(struct bt_mesh_blob_client_t *client)
{
    BT_INFO("");

    client->state = BT_MESH_BLOB_CLIENT_STATE_NONE;

    if (client->cb && client->cb->on_transfer_progress_complete) {
        client->cb->on_transfer_progress_complete(client);
    }
}

static void check_transfer(struct bt_mesh_blob_client_t *client)
{
    const struct blob_client_broadcast_context_t ctx = {
        .send = ctx_send_transfer_get,
        .next = on_progress_checked,
        .b_acked = true,
    };

    BT_INFO("");

    client->state = BT_MESH_BLOB_CLIENT_STATE_TRANSFER_PROGRESS_GET;

    blob_client_broadcast(client, &ctx);
}

static void client_cancel_transfer(struct bt_mesh_blob_client_t *client)
{
    const struct blob_client_broadcast_context_t ctx = {
        .send = ctx_send_transfer_cancel,
        .next = on_transfer_complete,
        .b_acked = true,
    };

    BT_INFO("");

    client->state = BT_MESH_BLOB_CLIENT_STATE_CANCEL;

    blob_client_broadcast(client, &ctx);
}

static void on_transfer_complete(struct bt_mesh_blob_client_t *client)
{
    bool success = targets_active(client) && client->state == BT_MESH_BLOB_CLIENT_STATE_TRANSFER_CHECK;

    end(client, success);
}

/*******************************************************************************
 * RX
 ******************************************************************************/
static void recv_block_status(
    struct bt_mesh_blob_client_t *client, struct bt_mesh_blob_target_t *target, struct block_status_t *block);

static void buf2caps(struct bt_mesh_blob_client_capabilities_t *caps, struct os_mbuf *buf)
{
    caps->min_block_size_logarithm = net_buf_simple_pull_u8(buf);
    caps->max_block_size_logarithm = net_buf_simple_pull_u8(buf);
    caps->max_chunks = net_buf_simple_pull_le16(buf);
    caps->max_chunk_size = net_buf_simple_pull_le16(buf);
    caps->max_size = net_buf_simple_pull_le32(buf);
    caps->mtu_size = net_buf_simple_pull_le16(buf);
    caps->modes = net_buf_simple_pull_u8(buf);
}

#define CHUNK_SIZE_MINSIZE 8
static int ble_mesh_blob_handle_info_status(struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_blob_client_t *client = mod->user_data;
    struct bt_mesh_blob_client_capabilities_t caps;
    enum bt_mesh_blob_status_t status;
    struct bt_mesh_blob_target_t *target;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    bt_mesh_ota_stat(MODEL_OP(BLOB_INFO_STATUS));
    if (client->state != BT_MESH_BLOB_CLIENT_STATE_CAPS_GET) {
        BT_WARN("wrong state %d from 0x%04x", client->state, ctx->addr);
        return -EBUSY;
    }

    buf2caps(&caps, buf);
    BT_DBG("\n0x%04x\n\tblock size: %u - %u\n\tchunks: %u\n\tchunk size: %u\n"
           "\tblob size: %u\n\tmtu size: %u\n\tmodes: %x",
        ctx->addr, caps.min_block_size_logarithm, caps.max_block_size_logarithm,
        caps.max_chunks, caps.max_chunk_size, caps.max_size, caps.mtu_size, caps.modes);
    if (caps.max_block_size_logarithm < caps.min_block_size_logarithm || caps.max_chunks == 0 ||
        caps.min_block_size_logarithm < BLOB_BLOCK_SIZE_LOG_LIMIT_MIN ||
        caps.max_block_size_logarithm > BLOB_BLOCK_SIZE_LOG_LIMIT_MAX ||
        caps.max_chunk_size < CHUNK_SIZE_MINSIZE ||
        caps.max_size == 0 || caps.mtu_size < BLOB_MTU_SIZE_LIMIT_MIN) {
            BT_WARN("got wrong caps from 0x%04x", ctx->addr);
            return -EINVAL;
    }

    target = client_get_target_by_addr(client, ctx->addr);
    if (!target) {
        BT_WARN("Unknown target 0x%04x", ctx->addr);
        return -ENOENT;
    }

    status = caps_adjust(client, &caps);
    if (status != BT_MESH_BLOB_SUCCESS) {
        target_drop(client, target, status);
    }

    blob_client_broadcast_responded(client, target);
    return 0;
}

#define INFO_STATUS_MASK4 4
#define INFO_STATUS_SHIFT6 6
#define INFO_STATUS_SHIFT32 32
#define INFO_STATUS_MINLEN 7
static void buf2transfer(struct bt_mesh_blob_transfer_info_t *info, struct os_mbuf *buf)
{
    uint8_t status_and_mode;

    status_and_mode = net_buf_simple_pull_u8(buf);
    info->status = status_and_mode & BIT_MASK(INFO_STATUS_MASK4);
    info->mode = status_and_mode >> INFO_STATUS_SHIFT6;
    info->phase = net_buf_simple_pull_u8(buf);

    if (buf->om_len >= BLOB_ID_LEN) {
        info->id = net_buf_simple_pull_le32(buf);
        info->id |= ((uint64_t)net_buf_simple_pull_le32(buf) << INFO_STATUS_SHIFT32);
    }

    if (buf->om_len >= INFO_STATUS_MINLEN) {
        info->size = net_buf_simple_pull_le32(buf);
        info->block_size_logarithm = net_buf_simple_pull_u8(buf);
        info->mtu_size = net_buf_simple_pull_le16(buf);
        info->missing_blocks = net_buf_simple_pull(buf, buf->om_len);
    }
}

static int ble_mesh_transfer_status_state_check(struct bt_mesh_blob_client_t *client)
{
    if (client->state != BT_MESH_BLOB_CLIENT_STATE_CANCEL && client->state != BT_MESH_BLOB_CLIENT_STATE_START &&
        client->state != BT_MESH_BLOB_CLIENT_STATE_TRANSFER_CHECK &&
        client->state != BT_MESH_BLOB_CLIENT_STATE_TRANSFER_PROGRESS_GET) {
        BT_WARN("wrong state: %d", client->state);
        if (BT_MESH_ADDR_IS_UNICAST(client->req.addr)) {
            k_sem_give(&client->req.sem);
        }
        return -EBUSY;
    }
    return 0;
}

static int ble_mesh_transfer_status_proc(enum bt_mesh_blob_transfer_phase_t *expected_phase,
    struct bt_mesh_blob_client_t *client, struct bt_mesh_blob_target_t *target,
    struct bt_mesh_blob_transfer_info_t *info)
{
    if (client->state == BT_MESH_BLOB_CLIENT_STATE_TRANSFER_CHECK) {
        *expected_phase = BT_MESH_BLOB_TRANSFER_PHASE_COMPLETE;
    } else if (client->state == BT_MESH_BLOB_CLIENT_STATE_START) {
        *expected_phase = BT_MESH_BLOB_TRANSFER_PHASE_WAITING_FOR_NEXT_BLOCK;
    }  else if (client->state == BT_MESH_BLOB_CLIENT_STATE_CANCEL) {
        *expected_phase = BT_MESH_BLOB_TRANSFER_PHASE_INACTIVE;
    } else if (client->state == BT_MESH_BLOB_CLIENT_STATE_TRANSFER_PROGRESS_GET) {
        blob_client_broadcast_responded(client, target);
        if (client->cb && client->cb->on_transfer_progress) {
            client->cb->on_transfer_progress(client, target, info);
        }
        return 0;
    } else {
        BT_WARN("wrong state: %d", client->state);
        if (BT_MESH_ADDR_IS_UNICAST(client->req.addr)) {
            k_sem_give(&client->req.sem);
        }
        return -EBUSY;
    }
    return 1;
}

static int ble_mesh_blob_handle_transfer_status(
    struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_blob_client_t *client = mod->user_data;
    enum bt_mesh_blob_transfer_phase_t expected_phase;
    struct bt_mesh_blob_target_t *target;
    struct bt_mesh_blob_transfer_info_t info = {0};

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    bt_mesh_ota_stat(MODEL_OP(BLOB_TRANSFER_STATUS));
    buf2transfer(&info, buf);
    if (info.status != BT_MESH_BLOB_SUCCESS) {
        BT_INFO("status: %u, mode: %u, phase: %u, id: %s", info.status, info.mode, info.phase,
            bt_hex(&info.id, sizeof(info.id)));
    }

    int err = ble_mesh_transfer_status_state_check(client);
    if (err) {
        return err;
    }

    target = client_get_target_by_addr(client, ctx->addr);
    if (target == NULL) {
        BT_WARN("no target found by addr: 0x%04x", ctx->addr);
        return -ENOENT;
    }

    err = ble_mesh_transfer_status_proc(&expected_phase, client, target, &info);
    if (err == 0 || err == -EBUSY) {
        return err;
    }

    if (info.status != BT_MESH_BLOB_SUCCESS) {
        target_drop(client, target, info.status);
    } else if (info.phase != expected_phase) {
        BT_WARN("wrong phase: %u != %u (expected)", expected_phase, info.phase);
        return -EINVAL;
    } else if (info.phase != BT_MESH_BLOB_TRANSFER_PHASE_INACTIVE && info.id != client->transfer->id) {
        target_drop(client, target, BT_MESH_BLOB_ERR_WRONG_BLOB_ID);
    }

    blob_client_broadcast_responded(client, target);
    return 0;
}

static void buf2blks(struct block_status_t *status, struct os_mbuf *buf, uint32_t cli_block_size)
{
    uint8_t status_and_format;
    uint16_t chunk_size;

    status_and_format = net_buf_simple_pull_u8(buf);
    status->status = status_and_format & BIT_MASK(INFO_STATUS_MASK4);
    status->missing_how = status_and_format >> INFO_STATUS_SHIFT6;
    status->block.index = net_buf_simple_pull_le16(buf);
    chunk_size = net_buf_simple_pull_le16(buf);
    status->block.chunks_count = DIV_ROUND_UP(cli_block_size, chunk_size);
}

static int ble_mesh_missing_encode(
    struct block_status_t *status, struct bt_mesh_blob_client_t *client, struct os_mbuf *buf)
{
    int idx = 0;

    if (!buf->om_len) {
        status->missing_how = BT_MESH_BLOB_CHUNKS_MISSING_NONE;
    }

    while (buf->om_len) {
        idx = chunk_index_decode(buf);
        if (idx < 0 || idx >= status->block.chunks_count) {
            BT_ERR("wrong mising_how encoding, exit..");
            if (BT_MESH_ADDR_IS_UNICAST(client->req.addr)) {
                k_sem_give(&client->req.sem);
            }
            return -EINVAL;
        }

        BT_INFO("Missing %d", idx);

        missing_chunks_set(status->block.missing_chunks, idx, true);
    }
    return 0;
}

static int ble_mesh_blob_handle_block_status(
    struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_blob_client_t *client = mod->user_data;
    struct bt_mesh_blob_target_t *target;
    struct block_status_t status = {0};
    uint32_t len;
    int err = 0;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    bt_mesh_ota_stat(MODEL_OP(BLOB_BLOCK_STATUS));
    target = client_get_target_by_addr(client, ctx->addr);
    if (!target) {
        return -ENOENT;
    }

    buf2blks(&status, buf, client->block.size);
    if (status.status != BT_MESH_BLOB_SUCCESS) {
        BT_INFO(
            "%04x-status:%u block:%u encoding:%u", ctx->addr, status.status, status.block.index, status.missing_how);
    }

    switch (status.missing_how) {
        case BT_MESH_BLOB_CHUNKS_MISSING_NONE:
            break;
        case BT_MESH_BLOB_CHUNKS_MISSING_ALL:
            missing_chunks_set_all(&status.block);
            break;
        case BT_MESH_BLOB_CHUNKS_MISSING_ENCODED:
            /** check MshMBTv1.0: 5.3.8 */
            err = ble_mesh_missing_encode(&status, client, buf);
            if (err) {
                return err;
            }
            break;
        case BT_MESH_BLOB_CHUNKS_MISSING_SOME:
            if (buf->om_len > sizeof(status.block.missing_chunks)) {
                return -EINVAL;
            }

            len = buf->om_len;
            memcpy_s(status.block.missing_chunks, sizeof(status.block.missing_chunks),
                net_buf_simple_pull_mem(buf, len), len);

            BT_INFO("missing chunks: %s", bt_hex(status.block.missing_chunks, len));
            break;
    }

    recv_block_status(client, target, &status);
    return 0;
}

static int ble_mesh_blob_handle_block_report(
    struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_blob_client_t *client = mod->user_data;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    struct block_status_t status = {
        .status = BT_MESH_BLOB_SUCCESS,
        .block.index = client->block.index,
        .missing_how = (buf->om_len ? BT_MESH_BLOB_CHUNKS_MISSING_ENCODED : BT_MESH_BLOB_CHUNKS_MISSING_NONE),
    };
    struct bt_mesh_blob_target_t *target;

    if (!client->transfer) {
        return -EINVAL;
    }

    if (client->transfer->mode == BT_MESH_BLOB_TRANSFER_MODE_PUSH) {
        BT_WARN("unexpected encoded block report in push mode");
        return -EINVAL;
    }

    BT_INFO("PULL mode not supported yet");

    target = client_get_target_by_addr(client, ctx->addr);
    if (!target) {
        return -ENOENT;
    }

    while (buf->om_len) {
        int idx;
        idx = chunk_index_decode(buf);
        if (idx < 0) {
            return idx;
        }
        missing_chunks_set(status.block.missing_chunks, idx, true);
    }

    if (next_missing_chunk(client, target->pull->missing_chunks, 0) >= client->block.chunks_count) {
        BT_INFO("chunks all already confirmed");
        return 0;
    }

    client->tx.cli_timestamp = k_uptime_get() + CLIENT_TIMEOUT_MSEC(client);
    recv_block_status(client, target, &status);
    return 0;
}

static void recv_block_status(struct bt_mesh_blob_client_t *client,
    struct bt_mesh_blob_target_t *target, struct block_status_t *block)
{
    if (client->state != BT_MESH_BLOB_CLIENT_STATE_BLOCK_SEND &&
        client->state != BT_MESH_BLOB_CLIENT_STATE_BLOCK_START &&
        client->state != BT_MESH_BLOB_CLIENT_STATE_BLOCK_CHECK) {
        BT_WARN("wrong state %u to 0x%04x", client->state, target->addr);
        return;
    }

    if (block->status != BT_MESH_BLOB_SUCCESS) {
        BT_WARN("target 0x%04x: block: %u status: %u", target->addr, block->block.index, block->status);
        target_drop(client, target, block->status);
        blob_client_broadcast_responded(client, target);
        return;
    }

    if (client->block.index != block->block.index) {
        BT_INFO("wrong block index %u, expected %u)", block->block.index, client->block.index);
        return;
    }

    if (block->missing_how == BT_MESH_BLOB_CHUNKS_MISSING_ALL) {
        missing_chunks_set_all(&client->block);
    } else  if (block->missing_how == BT_MESH_BLOB_CHUNKS_MISSING_NONE) {
        if (client->transfer->mode == BT_MESH_BLOB_TRANSFER_MODE_PULL) {
            memset_s(target->pull->missing_chunks, sizeof(target->pull->missing_chunks),
                0, sizeof(target->pull->missing_chunks));
            update_missing_chunks(client);
        }
        target->b_proc_completed = 1U;

        BT_DBG("target 0x%04x: none missing chunks now..", target->addr);
    } else if (client->transfer->mode == BT_MESH_BLOB_TRANSFER_MODE_PULL) {
        memcpy_s(target->pull->missing_chunks, sizeof(target->pull->missing_chunks),
            block->block.missing_chunks, sizeof(block->block.missing_chunks));

        BT_DBG("target 0x%04x: missing: %s", target->addr,
            bt_hex(target->pull->missing_chunks, client->block.chunks_count));

        update_missing_chunks(client);

        target->pull->block_report_timestamp = 0LL;
    } else {
        for (uint32_t i = 0; i < ARRAY_SIZE(block->block.missing_chunks); ++i) {
            client->block.missing_chunks[i] |= block->block.missing_chunks[i];
        }
    }

    if (IS_SENDING_CHUNKS_BY_PULL(client)) {
        if (!client->tx.b_sending) {
            broadcast_complete(client);
        }

        return;
    }

    blob_client_broadcast_responded(client, target);
}

const struct bt_mesh_model_op _bt_mesh_blob_client_op[] = {
    {MODEL_OP(BLOB_INFO_STATUS), 13, ble_mesh_blob_handle_info_status},
    {MODEL_OP(BLOB_TRANSFER_STATUS), 2, ble_mesh_blob_handle_transfer_status},
    {MODEL_OP(BLOB_BLOCK_STATUS), 5, ble_mesh_blob_handle_block_status},
    {MODEL_OP(BLOB_BLOCK_REPORT), 0, ble_mesh_blob_handle_block_report},
    BT_MESH_MODEL_OP_END,
};

static int blob_client_deinit(struct bt_mesh_model *mod);
static int blob_client_init(struct bt_mesh_model *mod);
static void blob_client_reset(struct bt_mesh_model *mod);
const struct bt_mesh_model_cb _bt_mesh_blob_client_cb = {
    .init = blob_client_init,
    .reset = blob_client_reset,
    .deinit = blob_client_deinit,
};

int bt_mesh_blob_client_capabilities_get(
    struct bt_mesh_blob_client_t *client, const struct bt_mesh_blob_client_inputs_t *inputs)
{
    if (!client || !inputs) {
        return -EINVAL;
    }
    if (bt_mesh_blob_client_is_busy(client)) {
        return -EBUSY;
    }

    client->caps.max_size = 0xffffffff;
    client->caps.mtu_size = 0xffff;
    client->caps.modes = MYNEWT_VAL(BLE_MESH_BLOB_TRANSFER_MODE); // only PUSH currently
    client->caps.min_block_size_logarithm = BLOB_BLOCK_SIZE_LOG_LIMIT_MIN; // 6B
    client->caps.max_block_size_logarithm = BLOB_BLOCK_SIZE_LOG_LIMIT_MAX; // 24G
    client->caps.max_chunks = MYNEWT_VAL(BLE_MESH_BLOB_CHUNK_COUNT_MAX);
    client->caps.max_chunk_size = BLOB_TX_CHUNK_SIZE;

    client->inputs = inputs;

    if (!targets_reset(client)) {
        BT_ERR("wrong target, exit..");
        return -ENODEV;
    }

    caps_get(client);

    return 0;
}

int bt_mesh_blob_client_send(struct bt_mesh_blob_client_t *client, const struct bt_mesh_blob_client_inputs_t *inputs,
    const struct bt_mesh_blob_transfer_t *transfer, const struct bt_mesh_blob_stream_t *stream)
{
    if (!client || !inputs || !transfer || !stream) {
        return -EINVAL;
    }
    if (bt_mesh_blob_client_is_busy(client)) {
        BT_ERR("busy, exit..");
        return -EBUSY;
    }

    if (transfer->block_size_logarithm < BLOB_BLOCK_SIZE_LOG_LIMIT_MIN ||
        transfer->block_size_logarithm > BLOB_BLOCK_SIZE_LOG_LIMIT_MAX ||
        transfer->chunk_size < CHUNK_SIZE_MINSIZE || transfer->chunk_size > BLOB_TX_CHUNK_SIZE ||
        !(transfer->mode & BT_MESH_BLOB_TRANSFER_MODE_ALL)) {
            BT_ERR("wrong transfer params, exit..");
            return -EINVAL;
    }

    client->inputs = inputs;
    client->transfer = transfer;
    client->stream = stream;

    if (client->transfer->block_size_logarithm == BLOB_BLOCK_SIZE_LOG_LIMIT_MAX) {
        client->block_count = 1;
    } else {
        client->block_count = DIV_ROUND_UP(client->transfer->size, (1U << client->transfer->block_size_logarithm));
    }

    block_set(client, 0);

    if (client->block.chunks_count > MYNEWT_VAL(BLE_MESH_BLOB_CHUNK_COUNT_MAX)) {
        BT_ERR("%d chunks received more than MYNEWT_VAL(BLE_MESH_BLOB_CHUNK_COUNT_MAX)=%d",
            client->block.chunks_count, MYNEWT_VAL(BLE_MESH_BLOB_CHUNK_COUNT_MAX));
        return -EINVAL;
    }

    if (!targets_reset(client)) {
        BT_ERR("wrong targets: failed targets_reset()");
        return -ENODEV;
    }

    BT_INFO("\n\tblock_size_logarithm: %u\n\tchunk_size: %u\n"
           "\tblob size: %u\n\tmode: %x",
        client->transfer->block_size_logarithm,
        client->transfer->chunk_size,
        client->transfer->size,
        client->transfer->mode);

    return transfer_start(client);
}

void bt_mesh_blob_client_cancel(struct bt_mesh_blob_client_t *client)
{
    if (!bt_mesh_blob_client_is_busy(client)) {
        BT_WARN("blob laready cancelled..");
        return;
    }

    BT_INFO("state=%u", client->state);

    if (client->state == BT_MESH_BLOB_CLIENT_STATE_CAPS_GET || client->state == BT_MESH_BLOB_CLIENT_STATE_SUSPENDED) {
        client_reset_state(client);
        return;
    }

    client->tx.b_cancelled = 1U;
    client->state = BT_MESH_BLOB_CLIENT_STATE_CANCEL;
}

int bt_mesh_blob_client_resume(struct bt_mesh_blob_client_t *client)
{
    struct bt_mesh_blob_target_t *target;

    if (client->state != BT_MESH_BLOB_CLIENT_STATE_SUSPENDED) {
        BT_WARN("No resume: not suspended");
        return -EINVAL;
    }

    if (client->inputs == NULL) {
        BT_ERR("inputs is null");
        return -EINVAL;
    }
    /* Restore timedout targets. */
    FOR_EACH_TARGET(client, target) {
        if (!!target->b_timedout) {
            target->status = BT_MESH_BLOB_SUCCESS;
            target->b_timedout = 0U;
        }
    }

    if (!targets_reset(client)) {
        BT_ERR("No valid target..");
        return -ENODEV;
    }

    block_set(client, 0);
    return transfer_start(client);
}


int bt_mesh_blob_client_suspend(struct bt_mesh_blob_client_t *client)
{
    if (client->state == BT_MESH_BLOB_CLIENT_STATE_SUSPENDED) {
        return 0;
    }

    if (client->state != BT_MESH_BLOB_CLIENT_STATE_BLOCK_SEND &&
        client->state != BT_MESH_BLOB_CLIENT_STATE_BLOCK_START &&
        client->state != BT_MESH_BLOB_CLIENT_STATE_BLOCK_CHECK) {
        BT_WARN("BLOB transfer not started: %d", client->state);
        return -EINVAL;
    }

    client->state = BT_MESH_BLOB_CLIENT_STATE_SUSPENDED;
    (void)k_work_cancel_delayable(&client->tx.timeout_retry);
    client->tx.b_sending = 0;
    client->tx.ctx.b_inited = 0;
    client->tx.cli_timestamp = 0LL;
    return 0;
}

void bt_mesh_blob_client_set_chunk_interval_ms(struct bt_mesh_blob_client_t *client, uint32_t interval_ms)
{
    client->chunk_interval_ms = interval_ms;
}

bool bt_mesh_blob_client_is_busy(struct bt_mesh_blob_client_t *client)
{
    return client->state != BT_MESH_BLOB_CLIENT_STATE_NONE;
}

uint8_t bt_mesh_blob_client_transfer_progress_active_get(struct bt_mesh_blob_client_t *client)
{
    if (!client || client->state < BT_MESH_BLOB_CLIENT_STATE_START) {
        return 0;
    }

    return (client->block.index * 100U) / client->block_count;
}

int bt_mesh_blob_client_transfer_progress_get(
    struct bt_mesh_blob_client_t *client, const struct bt_mesh_blob_client_inputs_t *inputs)
{
    if (bt_mesh_blob_client_is_busy(client)) {
        return -EBUSY;
    }

    client->inputs = inputs;

    check_transfer(client);

    return 0;
}

static int blob_client_deinit(struct bt_mesh_model *mod)
{
    struct bt_mesh_blob_client_t *client = mod->user_data;
    if ((mod->pub != NULL) && (mod->pub->msg != NULL)) {
        os_mbuf_free(mod->pub->msg);
        mod->pub->msg = NULL;
    }

    if (!client) {
        BT_ERR("No valid mod->user_data..");
        return -EINVAL;
    }
    k_work_cancel_delayable_delete(&client->tx.timeout_retry);
    k_work_cancel_delayable_delete(&client->tx.timeout_complete);
    k_sem_deinit(&client->req.sem);

    return 0;
}

static int blob_client_init(struct bt_mesh_model *mod)
{
    struct bt_mesh_blob_client_t *client = mod->user_data;

    client->mod = mod;

    client->tx.cli_timestamp = 0LL;
    bt_mesh_blob_client_set_chunk_interval_ms(client, MYNEWT_VAL(BLE_MESH_TX_BLOB_CHUNK_SEND_INTERVAL));
    k_work_init_delayable(&client->tx.timeout_retry, retry_timeout);
    k_work_add_arg_delayable(&client->tx.timeout_retry, client);
    k_work_init_delayable(&client->tx.timeout_complete, tx_complete);
    k_work_add_arg_delayable(&client->tx.timeout_complete, client);
    bt_mesh_mbt_test(false);
    k_sem_init(&client->req.sem, 0, 1);
    return 0;
}

static void blob_client_reset(struct bt_mesh_model *mod)
{
    struct bt_mesh_blob_client_t *client = mod->user_data;

    bt_mesh_mbt_test(false);
    client_reset_state(client);
}
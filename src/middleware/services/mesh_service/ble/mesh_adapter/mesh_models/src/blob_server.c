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
#include "mesh/mesh.h"
#include "model/model_opcode.h"
#include "atomic_adapter.h"
#include "lpn.h"
#include "state_transition.h"
#include "state_binding.h"
#include "model_common.h"
#include "blob.h"
#include "blob_common.h"
#include "ble_mesh_ota.h"
#include "access.h"
#include "model/blob_server.h"

#define MTU_SIZE_MAX (BT_MESH_RX_SDU_MAX - BT_MESH_MIC_SHORT)

#define BLOB_INFO_STATUS_LEN 15

/* The Receive BLOB Timeout Timer */
#define SERVER_TIMEOUT_SECS(serv) (10 * (1 + (serv)->state.timeout_base))
/* The initial timer value used in Pull mode - T_BPI */
#define REPORT_TIMER_TIMEOUT K_SECONDS(MYNEWT_VAL(BLE_MESH_BLOB_REPORT_TIMEOUT))

bt_mesh_blob_caps_t blob_caps = {MYNEWT_VAL(BLE_MESH_BLOB_SIZE_MAX), BLOB_BLOCK_SIZE_LOG_MAX, BLOB_BLOCK_SIZE_LOG_MIN};

BUILD_ASSERT(
    BLOB_BLOCK_SIZE_LOG_MIN <= BLOB_BLOCK_SIZE_LOG_MAX, "BLOB_BLOCK_SIZE_LOG_MIN not fit into BLOB_BLOCK_SIZE_LOG_MAX");

#ifndef CONFIG_BLE_MESH_PTS_MODE
BUILD_ASSERT((BLOB_TRANSFER_STATUS_MSG_MAXLEN + BT_MESH_MODEL_OP_LEN(MODEL_OP(BLOB_TRANSFER_STATUS)) +
                 BT_MESH_MIC_SHORT) <= BT_MESH_TX_SDU_MAX,
    "BLOB_TRANSFER_STATUS message not fit into the BT_MESH_TX_SDU_MAX.");
#endif

BUILD_ASSERT((BLOB_BLOCK_REPORT_STATUS_MSG_MAXLEN + BT_MESH_MODEL_OP_LEN(MODEL_OP(BLOB_BLOCK_REPORT)) +
                 BT_MESH_MIC_SHORT) <= BT_MESH_TX_SDU_MAX,
    "BLOB_BLOCK_REPORT message does not fit into the BT_MESH_TX_SDU_MAX.");

BUILD_ASSERT((BLOB_BLOCK_STATUS_MSG_MAXLEN + BT_MESH_MODEL_OP_LEN(MODEL_OP(BLOB_BLOCK_STATUS)) + BT_MESH_MIC_SHORT) <=
                 BT_MESH_TX_SDU_MAX,
    "BLOB_BLOCK_STATUS message does not fit into the BT_MESH_TX_SDU_MAX.");

static void cancel(struct bt_mesh_blob_server_t *serv);
static void suspend(struct bt_mesh_blob_server_t *serv);

static inline uint32_t block_count_get(const struct bt_mesh_blob_server_t *serv)
{
    return DIV_ROUND_UP(serv->state.transfer.size, (1U << serv->state.transfer.block_size_logarithm));
}

#define CHUNK_HEAD_LEN2 2
static inline uint32_t max_chunk_size(const struct bt_mesh_blob_server_t *serv)
{
    return MIN((serv->state.mtu_size - CHUNK_HEAD_LEN2 -
        BT_MESH_MODEL_OP_LEN(MODEL_OP(BLOB_CHUNK))), BLOB_RX_CHUNK_SIZE);
}

#define CHUNK_HEAD_LEN6 6
#define CHUNK_BYTE_BITS 8
static inline uint32_t max_chunks_count(const struct bt_mesh_blob_server_t *serv)
{
    /* MTU: because of using bitmap to store the missing chunks */
    return MIN(CHUNK_BYTE_BITS * (serv->state.mtu_size - CHUNK_HEAD_LEN6), MYNEWT_VAL(BLE_MESH_BLOB_CHUNK_COUNT_MAX));
}

#define POPCOUNT(x) __builtin_popcount(x)
static inline uint32_t count_missing_chunks(const struct bt_mesh_blob_block_t *block)
{
    int i;
    uint32_t count = 0;

    for (i = 0; i < ARRAY_SIZE(block->missing_chunks); ++i) {
        count += POPCOUNT(block->missing_chunks[i]);
    }

    return count;
}

static void store_state(const struct bt_mesh_blob_server_t *serv)
{
    if (!IS_ENABLED(CONFIG_BT_SETTINGS)) {
        return;
    }

    /* bit count -> byte count: */
    uint32_t block_len = DIV_ROUND_UP(block_count_get(serv), 8);

    bt_mesh_model_data_store(
        serv->mod, false, NULL, &serv->state, offsetof(struct bt_mesh_blob_server_state_t, blocks) + block_len);
}

static void erase_state(struct bt_mesh_blob_server_t *serv)
{
    if (!IS_ENABLED(CONFIG_BT_SETTINGS)) {
        return;
    }

    bt_mesh_model_data_store(serv->mod, false, NULL, NULL, 0);
}

static int stream_open(struct bt_mesh_blob_server_t *serv)
{
    if (!serv->stream->on_open) {
        return 0;
    }

    return serv->stream->on_open(serv->stream, &serv->state.transfer, BT_MESH_BLOB_STREAM_WRITE);
}

static void stream_close(struct bt_mesh_blob_server_t *serv)
{
    if (!serv->stream->on_close) {
        return;
    }

    serv->stream->on_close(serv->stream, &serv->state.transfer);
}

static void reset_timer(struct bt_mesh_blob_server_t *serv)
{
    uint32_t timeout_secs = serv->state.transfer.mode == BT_MESH_BLOB_TRANSFER_MODE_PULL
                                ? MAX(SERVER_TIMEOUT_SECS(serv), MYNEWT_VAL(BLE_MESH_BLOB_REPORT_TIMEOUT) + 1)
                                : SERVER_TIMEOUT_SECS(serv);
    k_work_reschedule(&serv->rx_timeout, K_SECONDS(timeout_secs));
}

#define CHUNK_IDX_SHIFT6 6
#define CHUNK_IDX_SHIFT12 12
static void buf_chunk_index_add(struct os_mbuf *buf, uint16_t chunk)
{
    /* utf-8 encoded: MshMBT_v1.0 Table 4.9 */
    if (chunk < 0x80) {
        net_buf_simple_add_u8(buf, chunk);
    } else if (chunk < 0x8000) {
        net_buf_simple_add_u8(buf, 0xc0 | (chunk >> CHUNK_IDX_SHIFT6));
        net_buf_simple_add_u8(buf, 0x80 | (chunk & BIT_MASK(CHUNK_IDX_SHIFT6)));
    } else {
        net_buf_simple_add_u8(buf, 0xe0 | (chunk >> CHUNK_IDX_SHIFT12));
        net_buf_simple_add_u8(buf, 0x80 | ((chunk >> CHUNK_IDX_SHIFT6) & BIT_MASK(CHUNK_IDX_SHIFT6)));
        net_buf_simple_add_u8(buf, 0x80 | (chunk & BIT_MASK(CHUNK_IDX_SHIFT6)));
    }
}

static int pull_req_max(const struct bt_mesh_blob_server_t *serv)
{
    int count = MYNEWT_VAL(BLE_MESH_BLOB_SRV_PULL_REQ_COUNT);

    /* lpn: todo */

    return MIN(count, count_missing_chunks(&serv->block));
}

static void report_sent(int err, void *cb_data)
{
    struct bt_mesh_blob_server_t *serv = cb_data;

    BT_INFO("");

    if (IS_ENABLED(CONFIG_BT_MESH_LOW_POWER) && bt_mesh_lpn_established()) {
        bt_mesh_lpn_poll();
    }

    if (k_work_delayable_is_pending(&serv->rx_timeout)) {
        k_work_reschedule(&serv->pull.report, REPORT_TIMER_TIMEOUT);
    }
}

static void block_report(struct bt_mesh_blob_server_t *serv)
{
    static const struct bt_mesh_send_cb report_cb = {.end = report_sent};
    struct bt_mesh_msg_ctx ctx = {
        .app_idx = serv->state.app_idx,
        .send_ttl = serv->state.ttl,
        .addr = serv->state.cli,
    };
    int count;
    int i;

    BT_INFO("rx blob timeout timer: %i", k_work_delayable_is_pending(&serv->rx_timeout));

    struct os_mbuf *msg = BT_MESH_MODEL_BUF(MODEL_OP(BLOB_BLOCK_REPORT), BLOB_BLOCK_REPORT_STATUS_MSG_MAXLEN);
    bt_mesh_model_msg_init(msg, MODEL_OP(BLOB_BLOCK_REPORT));

    count = pull_req_max(serv);

    for (i = 0; i < serv->block.chunks_count && count; ++i) {
        if (missing_chunks_get(serv->block.missing_chunks, i)) {
            buf_chunk_index_add(msg, i);
            count--;
        }
    }

    (void)bt_mesh_model_send(serv->mod, &ctx, msg, &report_cb, serv);
    os_mbuf_free_chain(msg);
}

static void phase_set(struct bt_mesh_blob_server_t *serv, enum bt_mesh_blob_transfer_phase_t phase)
{
    BT_INFO("%u -> %u", serv->phase, phase);
    serv->phase = phase;
}

static void cancel(struct bt_mesh_blob_server_t *serv)
{
    phase_set(serv, BT_MESH_BLOB_TRANSFER_PHASE_INACTIVE);
    serv->state.transfer.mode = BT_MESH_BLOB_TRANSFER_MODE_NONE;
    serv->state.ttl = BT_MESH_TTL_DEFAULT;
    serv->block.index = 0xffff;
    memset_s(serv->block.missing_chunks, sizeof(serv->block.missing_chunks), 0, sizeof(serv->block.missing_chunks));
    serv->state.transfer.chunk_size = 0xffff;
    k_work_cancel_delayable(&serv->rx_timeout);
    k_work_cancel_delayable(&serv->pull.report);
    stream_close(serv);
    erase_state(serv);

    if (serv->cb && serv->cb->on_end) {
        serv->cb->on_end(serv, serv->state.transfer.id, false);
    }
}

static void suspend(struct bt_mesh_blob_server_t *serv)
{
    BT_INFO("0ta");
    k_work_cancel_delayable(&serv->rx_timeout);
    k_work_cancel_delayable(&serv->pull.report);
    phase_set(serv, BT_MESH_BLOB_TRANSFER_PHASE_SUSPENDED);
    if (serv->cb && serv->cb->on_suspended) {
        serv->cb->on_suspended(serv);
    }
}

static void resume(struct bt_mesh_blob_server_t *serv)
{
    BT_INFO("resuming");

    phase_set(serv, BT_MESH_BLOB_TRANSFER_PHASE_WAITING_FOR_NEXT_BLOCK);
    reset_timer(serv);
}

static void end(struct bt_mesh_blob_server_t *serv)
{
    phase_set(serv, BT_MESH_BLOB_TRANSFER_PHASE_COMPLETE);
    ble_mesh_ota_upgrade_status_report(TRANSFER_END);
    k_work_cancel_delayable(&serv->rx_timeout);
    k_work_cancel_delayable(&serv->pull.report);
    stream_close(serv);
    erase_state(serv);

    if (serv->cb && serv->cb->on_end) {
        serv->cb->on_end(serv, serv->state.transfer.id, true);
    }
}

static bool all_blocks_received(struct bt_mesh_blob_server_t *serv)
{
    for (int i = 0; i < ARRAY_SIZE(serv->state.blocks); ++i) {
        if (serv->state.blocks[i]) {
            return false;
        }
    }

    return true;
}

static bool pull_mode_transfer_complete(struct bt_mesh_blob_server_t *serv)
{
    return serv->state.transfer.mode == BT_MESH_BLOB_TRANSFER_MODE_PULL &&
           serv->phase == BT_MESH_BLOB_TRANSFER_PHASE_WAITING_FOR_NEXT_CHUNK && all_blocks_received(serv);
}

static void timeout(struct ble_npl_event *work)
{
    struct bt_mesh_blob_server_t *serv = CONTAINER_OF(work, struct bt_mesh_blob_server_t, rx_timeout.work);

    BT_INFO("current phase:%u", serv->phase);

    if (serv->phase == BT_MESH_BLOB_TRANSFER_PHASE_WAITING_FOR_START) {
        cancel(serv);
    } else if (pull_mode_transfer_complete(serv)) {
        end(serv);
    } else {
        suspend(serv);
    }
}

static void report_timeout(struct ble_npl_event *work)
{
    struct bt_mesh_blob_server_t *serv = CONTAINER_OF(work, struct bt_mesh_blob_server_t, pull.report.work);

    BT_INFO("");

    if (serv->phase != BT_MESH_BLOB_TRANSFER_PHASE_WAITING_FOR_NEXT_BLOCK &&
        serv->phase != BT_MESH_BLOB_TRANSFER_PHASE_WAITING_FOR_NEXT_CHUNK) {
        return;
    }

    block_report(serv);
}

/*******************************************************************************
 * Message handler
 ******************************************************************************/
#define TRANSFER_STATUS_BITMASK4 4
#define TRANSFER_STATUS_SHIFT6 6
#define TRANSFER_ID_SHIFT32 32
#define BLOCK_COUNT_BYTELEN8 8

static uint32_t bt_mesh_blob_miss_chunk_set(struct bt_mesh_blob_server_t *server)
{
    uint32_t missing_chunks;

    if (server->phase == BT_MESH_BLOB_TRANSFER_PHASE_INACTIVE ||
        server->phase == BT_MESH_BLOB_TRANSFER_PHASE_WAITING_FOR_START) {
        missing_chunks = server->block.chunks_count;
    } else if (server->phase == BT_MESH_BLOB_TRANSFER_PHASE_COMPLETE) {
        missing_chunks = 0U;
    } else {
        missing_chunks = count_missing_chunks(&server->block);
    }
    return missing_chunks;
}

static void block_status_response(
    struct bt_mesh_blob_server_t *server, struct bt_mesh_msg_ctx *ctx, enum bt_mesh_blob_status_t status)
{
    int i;
    uint32_t missing_chunks;
    enum bt_mesh_blob_chunks_missing_how missing_how;

    struct os_mbuf *msg = BT_MESH_MODEL_BUF(MODEL_OP(BLOB_BLOCK_STATUS), BLOB_BLOCK_STATUS_MSG_MAXLEN);
    bt_mesh_model_msg_init(msg, MODEL_OP(BLOB_BLOCK_STATUS));

    missing_chunks = bt_mesh_blob_miss_chunk_set(server);

    if (server->state.transfer.mode == BT_MESH_BLOB_TRANSFER_MODE_PULL) {
        missing_how = BT_MESH_BLOB_CHUNKS_MISSING_ENCODED;
    } else if (missing_chunks == server->block.chunks_count) {
        missing_how = BT_MESH_BLOB_CHUNKS_MISSING_ALL;
    } else if (missing_chunks == 0) {
        missing_how = BT_MESH_BLOB_CHUNKS_MISSING_NONE;
    } else {
        missing_how = BT_MESH_BLOB_CHUNKS_MISSING_SOME;
    }

    BT_DBG("status: %u, missing_chunks: %u/%u, missing_how=%u",
        status, missing_chunks, server->block.chunks_count, missing_how);

    net_buf_simple_add_u8(msg, (status & BIT_MASK(TRANSFER_STATUS_BITMASK4)) | (missing_how << TRANSFER_STATUS_SHIFT6));
    if (status == BT_MESH_BLOB_ERR_WRONG_PHASE && (server->phase == BT_MESH_BLOB_TRANSFER_PHASE_INACTIVE ||
                                                      server->phase == BT_MESH_BLOB_TRANSFER_PHASE_SUSPENDED ||
                                                      server->phase == BT_MESH_BLOB_TRANSFER_PHASE_COMPLETE)) {
        net_buf_simple_add_le16(msg, 0xFFFF);
        net_buf_simple_add_le16(msg, 0xFFFF);
    } else {
        net_buf_simple_add_le16(msg, server->block.index);
        net_buf_simple_add_le16(msg, server->state.transfer.chunk_size);
    }

    if (missing_how == BT_MESH_BLOB_CHUNKS_MISSING_SOME) {
        net_buf_simple_add_mem(msg, server->block.missing_chunks,
            DIV_ROUND_UP(server->block.chunks_count, BLOCK_COUNT_BYTELEN8));

        BT_DBG("bitmap: %s", bt_hex(server->block.missing_chunks,
            DIV_ROUND_UP(server->block.chunks_count, BLOCK_COUNT_BYTELEN8)));
    } else if (missing_how == BT_MESH_BLOB_CHUNKS_MISSING_ENCODED) {
        int count = pull_req_max(server);

        for (i = 0; (i < server->block.chunks_count) && count; ++i) {
            if (missing_chunks_get(server->block.missing_chunks, i)) {
                BT_INFO("missing %u", i);
                buf_chunk_index_add(msg, i);
                count--;
            }
        }
    }

    if (server->phase != BT_MESH_BLOB_TRANSFER_PHASE_INACTIVE) {
        ctx->send_ttl = server->state.ttl;
    }

    (void)bt_mesh_model_send(server->mod, ctx, msg, NULL, NULL);
    os_mbuf_free_chain(msg);
}

static void transfer_status_response(
    struct bt_mesh_blob_server_t *serv, struct bt_mesh_msg_ctx *ctx, enum bt_mesh_blob_status_t status)
{
    struct os_mbuf *msg = BT_MESH_MODEL_BUF(MODEL_OP(BLOB_TRANSFER_STATUS), BLOB_TRANSFER_STATUS_MSG_MAXLEN);
    bt_mesh_model_msg_init(msg, MODEL_OP(BLOB_TRANSFER_STATUS));

    net_buf_simple_add_u8(msg, ((status & BIT_MASK(TRANSFER_STATUS_BITMASK4)) |
        (serv->state.transfer.mode << TRANSFER_STATUS_SHIFT6)));
    net_buf_simple_add_u8(msg, serv->phase);

    if (serv->phase == BT_MESH_BLOB_TRANSFER_PHASE_INACTIVE) {
        goto send;
    }

    net_buf_simple_add_le32(msg, (uint32_t)serv->state.transfer.id);
    net_buf_simple_add_le32(msg, (uint32_t)(serv->state.transfer.id >> TRANSFER_ID_SHIFT32));

    if (serv->phase == BT_MESH_BLOB_TRANSFER_PHASE_WAITING_FOR_START) {
        goto send;
    }

    net_buf_simple_add_le32(msg, serv->state.transfer.size);
    net_buf_simple_add_u8(msg, serv->state.transfer.block_size_logarithm);
    net_buf_simple_add_le16(msg, serv->state.mtu_size);
    net_buf_simple_add_mem(msg, serv->state.blocks, DIV_ROUND_UP(block_count_get(serv), BLOCK_COUNT_BYTELEN8));

send:
    ctx->send_ttl = serv->state.ttl;
    (void)bt_mesh_model_send(serv->mod, ctx, msg, NULL, NULL);
    os_mbuf_free_chain(msg);
}

static int ble_mesh_blob_handle_transfer_get(
    struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_blob_server_t *serv = mod->user_data;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    BT_INFO("");
    if (pull_mode_transfer_complete(serv)) {
        /* pull? */
        end(serv);
    }

    transfer_status_response(serv, ctx, BT_MESH_BLOB_SUCCESS);

    return 0;
}

struct handle_transfer_start_info {
    enum bt_mesh_blob_status_t status;
    enum bt_mesh_blob_transfer_mode_t mode;
    uint64_t id;
    uint32_t size;
    uint8_t block_size_logarithm;
    uint32_t block_count;
    uint16_t mtu_size;
};

static void handle_transfer_start_setup(struct bt_mesh_blob_server_t *serv,
    struct handle_transfer_start_info *info, struct os_mbuf *buf)
{
    if (!info || !buf) {
        return;
    }

    info->status = BT_MESH_BLOB_SUCCESS; // default as SUCCESS
    info->block_count = 0;
    info->mode = (net_buf_simple_pull_u8(buf) >> TRANSFER_STATUS_SHIFT6);
    info->id = net_buf_simple_pull_le32(buf);
    info->id |= ((uint64_t)net_buf_simple_pull_le32(buf) << TRANSFER_ID_SHIFT32);
    info->size = net_buf_simple_pull_le32(buf);
    info->block_size_logarithm = net_buf_simple_pull_u8(buf);
    info->mtu_size = net_buf_simple_pull_le16(buf);

    if (serv->phase == BT_MESH_BLOB_TRANSFER_PHASE_INACTIVE) {
        info->status = BT_MESH_BLOB_ERR_WRONG_PHASE;
        BT_WARN("Uninitialized, phase %u", serv->phase);
        return;
    }

    if (serv->state.transfer.id != info->id) {
        info->status = BT_MESH_BLOB_ERR_WRONG_BLOB_ID;

        BT_WARN("Invalid ID: %s", bt_hex(&info->id, sizeof(uint64_t)));
        BT_WARN("Expected ID: %s", bt_hex(&serv->state.transfer.id, sizeof(uint64_t)));
        return;
    }

    if (serv->phase != BT_MESH_BLOB_TRANSFER_PHASE_WAITING_FOR_START) {
        if (serv->state.transfer.mode != info->mode || serv->state.transfer.size != info->size ||
            serv->state.transfer.block_size_logarithm != info->block_size_logarithm ||
            serv->state.mtu_size > info->mtu_size) {
            info->status = BT_MESH_BLOB_ERR_WRONG_PHASE;
            BT_WARN("Busy or wrng params -> WR0NG_PHASE");
            return;
        }
    }

    if (info->size > blob_caps.max_blob_size || info->size == 0) {
        BT_WARN("blob size too large: %u>%u", info->size, blob_caps.max_blob_size);
        info->status = BT_MESH_BLOB_ERR_BLOB_TOO_LARGE;
        return;
    }

    if (((1U << info->block_size_logarithm) < (1U << blob_caps.min_block_size)) ||
        ((1U << info->block_size_logarithm) > (1U << blob_caps.max_block_size))) {
        BT_WARN("Invalid block size: %u", info->block_size_logarithm);
        info->status = BT_MESH_BLOB_ERR_INVALID_BLOCK_SIZE;
        return;
    }

    return;
}

static int handle_transfer_start_srv(struct bt_mesh_blob_server_t *serv, const struct handle_transfer_start_info *info)
{
    uint32_t block_count = 0;
    serv->state.mtu_size = MIN(info->mtu_size, MTU_SIZE_MAX);
    serv->state.transfer.id = info->id;
    serv->state.transfer.size = info->size;
    serv->state.transfer.mode = info->mode;
    serv->state.transfer.block_size_logarithm = info->block_size_logarithm;
    serv->state.transfer.chunk_size = 0xffff;
    serv->block.index = 0xffff;

    block_count = block_count_get(serv);
    if (block_count > 0 && block_count < BT_MESH_BLOB_BLOCKS_MAX) {
        memset_s(serv->state.blocks, sizeof(serv->state.blocks), 0, sizeof(serv->state.blocks));
        for (int i = 0; i < block_count; i++) {
            atomic_set_bit(serv->state.blocks, i);
        }
    }

    return block_count;
}

/* return true => goto rsp */
static bool handle_transfer_start_act_on_info(struct bt_mesh_blob_server_t *serv, struct bt_mesh_msg_ctx *ctx,
    struct handle_transfer_start_info *info)
{
    if (info->status != BT_MESH_BLOB_SUCCESS) {
        return true;
    }

    if (serv->phase != BT_MESH_BLOB_TRANSFER_PHASE_WAITING_FOR_START) {
        if (serv->phase == BT_MESH_BLOB_TRANSFER_PHASE_SUSPENDED) {
            resume(serv);
            store_state(serv);
            info->status = BT_MESH_BLOB_SUCCESS;
            return true;
        }
    }

    serv->state.cli = ctx->addr;
    serv->state.app_idx = ctx->app_idx;
    info->block_count = handle_transfer_start_srv(serv, info);

    if (info->block_count > BT_MESH_BLOB_BLOCKS_MAX || info->block_count < 0) {
        BT_WARN("Invalid block count (%d)", info->block_count);
        info->status = BT_MESH_BLOB_ERR_INVALID_PARAM;
        cancel(serv);
        return true;
    }

    return false;
}

static int ble_mesh_blob_handle_transfer_start(
    struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_blob_server_t *serv = mod->user_data;
    struct handle_transfer_start_info info = {0};
    int err;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    handle_transfer_start_setup(serv, &info, buf);
    BT_INFO("\n\tsize: %u block size: %u\n\tmtu_size: %u\n\tmode: %u",
        info.size, (1U << info.block_size_logarithm), info.mtu_size, info.mode);

    if (info.mode != BT_MESH_BLOB_TRANSFER_MODE_PULL && info.mode != BT_MESH_BLOB_TRANSFER_MODE_PUSH) {
        BT_WARN("Invalid mode 0x%x", info.mode);
        return -EINVAL;
    } else if (info.mode != BT_MESH_BLOB_TRANSFER_MODE_PUSH) {
        BT_WARN("UnSupported mode 0x%x", info.mode);
        info.status = BT_MESH_BLOB_ERR_UNSUPPORTED_MODE;
        phase_set(serv, BT_MESH_BLOB_TRANSFER_PHASE_WAITING_FOR_START); // pts blobs 30
        goto rsp;
    }

    if (handle_transfer_start_act_on_info(serv, ctx, &info)) {
        goto rsp;
    }

    err = stream_open(serv);
    if (err) {
        BT_ERR("Couldn't open stream (err: %d)", err);
        info.status = BT_MESH_BLOB_ERR_INTERNAL;
        cancel(serv);
        goto rsp;
    }

    if (serv->cb && serv->cb->on_start) {
        err = serv->cb->on_start(serv, ctx, &serv->state.transfer);
        if (err) {
            BT_ERR("Couldn't start transfer (err: %d)", err);
            info.status = BT_MESH_BLOB_ERR_INTERNAL;
            cancel(serv);
            goto rsp;
        }
    }

    reset_timer(serv);
    phase_set(serv, BT_MESH_BLOB_TRANSFER_PHASE_WAITING_FOR_NEXT_BLOCK);
    store_state(serv);
    info.status = BT_MESH_BLOB_SUCCESS;

rsp:
    transfer_status_response(serv, ctx, info.status);

    return 0;
}

static int ble_mesh_blob_handle_transfer_cancel(
    struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    uint64_t id;
    enum bt_mesh_blob_status_t status = BT_MESH_BLOB_SUCCESS;
    struct bt_mesh_blob_server_t *serv = mod->user_data;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    id = net_buf_simple_pull_le32(buf);
    id |= ((uint64_t)net_buf_simple_pull_le32(buf) << TRANSFER_ID_SHIFT32);

    BT_INFO("id=%u, phase=%u", (uint32_t)id, serv->phase);

    if (serv->state.transfer.id != id) {
        status = BT_MESH_BLOB_ERR_WRONG_BLOB_ID;
        goto rsp;
    }

    if (serv->phase == BT_MESH_BLOB_TRANSFER_PHASE_INACTIVE) {
        goto rsp;
    }

    cancel(serv);

rsp:
    transfer_status_response(serv, ctx, status);

    return 0;
}

/* return true => goto rsp */
static bool handle_block_start_act_on_status(struct bt_mesh_blob_server_t *serv,
    enum bt_mesh_blob_status_t *status, uint16_t block_number, uint16_t chunk_size)
{
    if (serv->phase == BT_MESH_BLOB_TRANSFER_PHASE_WAITING_FOR_START ||
        serv->phase == BT_MESH_BLOB_TRANSFER_PHASE_INACTIVE) {
        *status = BT_MESH_BLOB_ERR_WRONG_PHASE;
        return true;
    }

    reset_timer(serv);

    if (serv->phase == BT_MESH_BLOB_TRANSFER_PHASE_WAITING_FOR_NEXT_CHUNK) {
        if (block_number != serv->block.index || chunk_size != serv->state.transfer.chunk_size) {
            *status = BT_MESH_BLOB_ERR_WRONG_PHASE;
        } else {
            *status = BT_MESH_BLOB_SUCCESS;
        }

        return true;
    }

    if (block_number >= block_count_get(serv)) {
        *status = BT_MESH_BLOB_ERR_INVALID_BLOCK_NUM;
        return true;
    }

    if (!chunk_size || chunk_size > max_chunk_size(serv) ||
        (DIV_ROUND_UP((1 << serv->state.transfer.block_size_logarithm), chunk_size) >
            (int32_t)max_chunks_count(serv))) {
        BT_WARN("Invalid chunk size: (chunk size: %u, max: %u, block log: %u, count: %u)",
            chunk_size,
            max_chunk_size(serv),
            serv->state.transfer.block_size_logarithm,
            max_chunks_count(serv));
        *status = BT_MESH_BLOB_ERR_INVALID_CHUNK_SIZE;
        return true;
    }

    serv->block.size = block_size_by_index(serv->state.transfer.size,
        serv->state.transfer.block_size_logarithm, block_number);
    serv->block.index = block_number;
    serv->block.chunks_count = DIV_ROUND_UP(serv->block.size, chunk_size);
    serv->state.transfer.chunk_size = chunk_size;
    serv->block.offset = block_number * (1UL << serv->state.transfer.block_size_logarithm);

    return false;
}

static int ble_mesh_blob_handle_block_start(struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    int err;
    enum bt_mesh_blob_status_t status;
    uint16_t block_number, chunk_size;
    struct bt_mesh_blob_server_t *serv = mod->user_data;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    block_number = net_buf_simple_pull_le16(buf);
    chunk_size = net_buf_simple_pull_le16(buf);
    if (handle_block_start_act_on_status(serv, &status, block_number, chunk_size)) {
        goto rsp;
    }

    if (serv->phase == BT_MESH_BLOB_TRANSFER_PHASE_COMPLETE) {
        memset_s(serv->block.missing_chunks, sizeof(serv->block.missing_chunks), 0, sizeof(serv->block.missing_chunks));
        status = BT_MESH_BLOB_SUCCESS;
        goto rsp;
    }

    if (serv->phase == BT_MESH_BLOB_TRANSFER_PHASE_SUSPENDED && serv->cb && serv->cb->on_resume) {
        serv->cb->on_resume(serv);
    }

    phase_set(serv, BT_MESH_BLOB_TRANSFER_PHASE_WAITING_FOR_NEXT_CHUNK);
    missing_chunks_set_all(&serv->block);

    BT_INFO("%u: (%u/%u)\n\tsize: %u\n\tchunk size: %u\n\tchunk count: %u",
        serv->block.index,
        serv->block.index + 1,
        block_count_get(serv),
        serv->block.size,
        chunk_size,
        serv->block.chunks_count);

    if (serv->stream->on_block_start) {
        err = serv->stream->on_block_start(serv->stream, &serv->state.transfer, &serv->block);
        if (err) {
            cancel(serv);
            status = BT_MESH_BLOB_ERR_INTERNAL;
            goto rsp;
        }
    }

    if (serv->state.transfer.mode == BT_MESH_BLOB_TRANSFER_MODE_PULL) {
        /* Wait for the client to send the first chunk */
        BT_INFO("pull -> NOT OK");
        k_work_reschedule(&serv->pull.report, REPORT_TIMER_TIMEOUT);
    }

    status = BT_MESH_BLOB_SUCCESS;

rsp:
    block_status_response(serv, ctx, status);

    return 0;
}

static int ble_mesh_blob_handle_block_get(struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    enum bt_mesh_blob_status_t status;
    struct bt_mesh_blob_server_t *serv = mod->user_data;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    switch (serv->phase) {
        case BT_MESH_BLOB_TRANSFER_PHASE_WAITING_FOR_START:
        case BT_MESH_BLOB_TRANSFER_PHASE_INACTIVE:
            status = BT_MESH_BLOB_ERR_WRONG_PHASE;
            break;
        case BT_MESH_BLOB_TRANSFER_PHASE_WAITING_FOR_NEXT_BLOCK:
        case BT_MESH_BLOB_TRANSFER_PHASE_WAITING_FOR_NEXT_CHUNK:
        case BT_MESH_BLOB_TRANSFER_PHASE_COMPLETE:
            status = BT_MESH_BLOB_SUCCESS;
            break;
        case BT_MESH_BLOB_TRANSFER_PHASE_SUSPENDED:
            status = BT_MESH_BLOB_ERR_INFO_UNAVAILABLE;
            break;
        default:
            status = BT_MESH_BLOB_ERR_INTERNAL;
            break;
    }

    BT_INFO("status:%u phase=%u", status, serv->phase);

    if (serv->phase != BT_MESH_BLOB_TRANSFER_PHASE_COMPLETE) {
        reset_timer(serv);
    }
    block_status_response(serv, ctx, status);

    return 0;
}

static int ble_mesh_blob_handle_info_get(struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    struct bt_mesh_blob_server_t *serv = mod->user_data;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    BT_INFO("");
    struct os_mbuf *rsp = BT_MESH_MODEL_BUF(MODEL_OP(BLOB_INFO_STATUS), BLOB_INFO_STATUS_LEN);
    bt_mesh_model_msg_init(rsp, MODEL_OP(BLOB_INFO_STATUS));
    net_buf_simple_add_u8(rsp, blob_caps.min_block_size);
    net_buf_simple_add_u8(rsp, blob_caps.max_block_size);
    net_buf_simple_add_le16(rsp, MYNEWT_VAL(BLE_MESH_BLOB_CHUNK_COUNT_MAX));
    net_buf_simple_add_le16(rsp, MYNEWT_VAL(BLE_MESH_RX_BLOB_CHUNK_SIZE));
    net_buf_simple_add_le32(rsp, blob_caps.max_blob_size);
    net_buf_simple_add_le16(rsp, MTU_SIZE_MAX);
    net_buf_simple_add_u8(rsp, BT_MESH_BLOB_TRANSFER_MODE_PUSH);

    if (serv->phase != BT_MESH_BLOB_TRANSFER_PHASE_INACTIVE) {
        ctx->send_ttl = serv->state.ttl;
    }

    (void)bt_mesh_model_send(serv->mod, ctx, rsp, NULL, NULL);
    os_mbuf_free_chain(rsp);
    return 0;
}

/* return 0 => ok */
static int handle_chunk_setup(struct bt_mesh_blob_server_t *serv, struct os_mbuf *buf,
    struct bt_mesh_blob_chunk_t *chunk, uint16_t *ind)
{
    uint32_t expected_size = 0;

    *ind = net_buf_simple_pull_le16(buf);
    chunk->size = buf->om_len;
    chunk->data = net_buf_simple_pull_mem(buf, chunk->size);
    chunk->offset = *ind * serv->state.transfer.chunk_size;

    if (*ind >= serv->block.chunks_count || serv->phase != BT_MESH_BLOB_TRANSFER_PHASE_WAITING_FOR_NEXT_CHUNK) {
        BT_ERR("wrong phase or index: phase=%u (0x3?), ind=%u/%u", serv->phase, *ind, serv->block.chunks_count);
        return -EINVAL;
    }

    if (*ind + 1 == serv->block.chunks_count) {
        expected_size = serv->block.size % serv->state.transfer.chunk_size;
    }

    if (expected_size == 0) {
        expected_size = serv->state.transfer.chunk_size;
    }

    if (chunk->size != expected_size) {
        BT_ERR("wrong chunk size: expected %u != chunk->size %u ; ind=%u/%u",
            expected_size, chunk->size, *ind, serv->block.chunks_count);
        return -EINVAL;
    }

    return 0; // ok
}

static int ble_mesh_block_proc(struct bt_mesh_blob_server_t *serv)
{
    if (serv->state.transfer.mode == BT_MESH_BLOB_TRANSFER_MODE_PULL) {
        block_report(serv);
    }

    if (serv->stream->on_block_end) {
        serv->stream->on_block_end(serv->stream, &serv->state.transfer, &serv->block);
    }

    atomic_clear_bit(serv->state.blocks, serv->block.index);

    if (!all_blocks_received(serv)) {
        phase_set(serv, BT_MESH_BLOB_TRANSFER_PHASE_WAITING_FOR_NEXT_BLOCK);
        store_state(serv);
        ble_mesh_ota_upgrade_status_report(TRANSFER_END);
        return 0;
    }

    if (serv->state.transfer.mode == BT_MESH_BLOB_TRANSFER_MODE_PULL) {
        return 0;
    }

    end(serv);
    return 0;
}

static int ble_mesh_blob_handle_chunk(struct bt_mesh_model *mod, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf)
{
    int err;
    uint16_t ind;
    struct bt_mesh_blob_chunk_t chunk;
    struct bt_mesh_blob_server_t *serv = mod->user_data;

    if (bt_mesh_has_addr(ctx->addr)) {
        BT_DBG("Dropping locally originated packet");
        return 0;
    }

    err = handle_chunk_setup(serv, buf, &chunk, &ind);
    if (err) {
        return err;
    }
    BT_INFO("%u/%u@%u (%u bytes)", ind + 1, serv->block.chunks_count, serv->block.index, chunk.size);

    reset_timer(serv);
    if (serv->state.transfer.mode == BT_MESH_BLOB_TRANSFER_MODE_PULL) {
        k_work_reschedule(&serv->pull.report, REPORT_TIMER_TIMEOUT);
    }

    if (!missing_chunks_get(serv->block.missing_chunks, ind)) {
        BT_INFO("dup chunk: %u", ind);
        return -EALREADY;
    }

    if (ind == 0) {
        ble_mesh_ota_upgrade_status_report(TRANSFER_START);
    }

    err = serv->stream->on_write(serv->stream, &serv->state.transfer, &serv->block, &chunk);
    if (err) {
        BT_ERR("stream on_write failed: %d", err);
        return err;
    }

    missing_chunks_set(serv->block.missing_chunks, ind, false);
    if (count_missing_chunks(&serv->block)) {
        return 0;
    }

    return ble_mesh_block_proc(serv);
}

const struct bt_mesh_model_op _bt_mesh_blob_server_op[] = {
    {MODEL_OP(BLOB_INFO_GET), BT_MESH_LEN_EXACT(0), ble_mesh_blob_handle_info_get},
    {MODEL_OP(BLOB_TRANSFER_GET), BT_MESH_LEN_EXACT(0), ble_mesh_blob_handle_transfer_get},
    {MODEL_OP(BLOB_TRANSFER_START), BT_MESH_LEN_EXACT(16), ble_mesh_blob_handle_transfer_start},
    {MODEL_OP(BLOB_TRANSFER_CANCEL), BT_MESH_LEN_EXACT(8), ble_mesh_blob_handle_transfer_cancel},
    {MODEL_OP(BLOB_BLOCK_START), BT_MESH_LEN_EXACT(4), ble_mesh_blob_handle_block_start},
    {MODEL_OP(BLOB_BLOCK_GET), BT_MESH_LEN_EXACT(0), ble_mesh_blob_handle_block_get},
    {MODEL_OP(BLOB_CHUNK), BT_MESH_LEN_MIN(2), ble_mesh_blob_handle_chunk},
    BT_MESH_MODEL_OP_END,
};

static int blob_server_start(struct bt_mesh_model *mod)
{
    int err = -ENOTSUP;
    struct bt_mesh_blob_server_t *serv = NULL;

    if (!mod || !mod->user_data) {
        return 0;
    }
    serv = mod->user_data;

    if (serv->phase == BT_MESH_BLOB_TRANSFER_PHASE_INACTIVE) {
        return 0;
    }

    if (serv->cb && serv->cb->on_recover) {
        serv->stream = NULL;
        err = serv->cb->on_recover(serv, &serv->state.transfer, &serv->stream);
        if (!err && serv->stream) {
            err = stream_open(serv);
        }
    }

    if (err || !serv->stream) {
        BT_WARN("discarding this transfer.");
        serv->state.transfer.mode = BT_MESH_BLOB_TRANSFER_MODE_NONE;
        serv->state.ttl = BT_MESH_TTL_DEFAULT;
        phase_set(serv, BT_MESH_BLOB_TRANSFER_PHASE_INACTIVE);
        erase_state(serv);
    }

    return 0;
}

static int blob_server_settings_set(struct bt_mesh_model *mod, const char *name, char *val)
{
    return -EINVAL;
}

static void blob_server_reset(struct bt_mesh_model *mod)
{
    if (!mod || !mod->user_data) {
        return;
    }

    struct bt_mesh_blob_server_t *serv = mod->user_data;

    serv->state.transfer.mode = BT_MESH_BLOB_TRANSFER_MODE_NONE;
    phase_set(serv, BT_MESH_BLOB_TRANSFER_PHASE_INACTIVE);

    k_work_cancel_delayable(&serv->rx_timeout);
    k_work_cancel_delayable(&serv->pull.report);

    erase_state(serv);
}

int bt_mesh_blob_server_recv(struct bt_mesh_blob_server_t *serv, uint64_t id,
    const struct bt_mesh_blob_stream_t *stream, uint8_t ttl, uint16_t timeout_base)
{
    if (!serv || !stream || !stream->on_write) {
        return -EINVAL;
    }

    if (bt_mesh_blob_server_is_busy(serv)) {
        return -EBUSY;
    }

    serv->block.index = 0xffff;
    serv->state.transfer.chunk_size = 0xffff;
    serv->state.transfer.id = id;
    serv->state.ttl = ttl;
    serv->state.timeout_base = timeout_base;
    serv->stream = stream;

    phase_set(serv, BT_MESH_BLOB_TRANSFER_PHASE_WAITING_FOR_START);
    store_state(serv);

    return 0;
}

int bt_mesh_blob_server_cancel(struct bt_mesh_blob_server_t *serv)
{
    if (!bt_mesh_blob_server_is_busy(serv)) {
        return -EALREADY;
    }

    cancel(serv);

    return 0;
}

uint8_t bt_mesh_blob_server_progress(const struct bt_mesh_blob_server_t *serv)
{
    uint32_t i, total, got;

    if (serv->phase == BT_MESH_BLOB_TRANSFER_PHASE_WAITING_FOR_START ||
        serv->phase == BT_MESH_BLOB_TRANSFER_PHASE_INACTIVE
        ) {
        return 0;
    }

    i = got = 0;
    total = block_count_get(serv);
    while (i++ < total) {
        if (!atomic_test_bit(serv->state.blocks, i)) {
            got++;
        }
    }

    return (100U * got) / total;
}

bool bt_mesh_blob_server_is_busy(const struct bt_mesh_blob_server_t *serv)
{
    return serv->phase != BT_MESH_BLOB_TRANSFER_PHASE_INACTIVE &&
           serv->phase != BT_MESH_BLOB_TRANSFER_PHASE_COMPLETE &&
           serv->phase != BT_MESH_BLOB_TRANSFER_PHASE_SUSPENDED ;
}

void bt_mesh_blob_caps_set(bt_mesh_blob_caps_t *blob_capas)
{
    BT_INFO("curr max_blob_size %u, max_block_size %u, min_block_size %u",
        blob_caps.max_blob_size, blob_caps.max_block_size, blob_caps.min_block_size);
    blob_caps.max_blob_size = blob_capas->max_blob_size;
    blob_caps.max_block_size = blob_capas->max_block_size;
    blob_caps.min_block_size = blob_capas->min_block_size;
    BT_INFO("set max_blob_size %u, max_block_size %u, min_block_size %u",
        blob_caps.max_blob_size, blob_caps.max_block_size, blob_caps.min_block_size);
}

static int blob_server_deinit(struct bt_mesh_model *mod)
{
    struct bt_mesh_blob_server_t *serv = mod->user_data;

    if ((mod->pub != NULL) && (mod->pub->msg != NULL)) {
        os_mbuf_free(mod->pub->msg);
        mod->pub->msg = NULL;
    }

    if (!serv) {
        BT_ERR("No valid mod->user_data..");
        return -EINVAL;
    }

    k_work_cancel_delayable_delete(&serv->pull.report);
    k_work_cancel_delayable_delete(&serv->rx_timeout);

    return 0;
}

static int blob_server_init(struct bt_mesh_model *mod)
{
    struct bt_mesh_blob_server_t *serv = mod->user_data;

    serv->mod = mod;
    serv->state.ttl = BT_MESH_TTL_DEFAULT;
    serv->state.transfer.chunk_size = 0xffff;
    serv->block.index = 0xffff;
    k_work_init_delayable(&serv->pull.report, report_timeout);
    k_work_init_delayable(&serv->rx_timeout, timeout);

    return 0;
}

const struct bt_mesh_model_cb _bt_mesh_blob_server_cb = {
    .init = blob_server_init,
    .settings_set = blob_server_settings_set,
    .start = blob_server_start,
    .reset = blob_server_reset,
    .deinit = blob_server_deinit,
};
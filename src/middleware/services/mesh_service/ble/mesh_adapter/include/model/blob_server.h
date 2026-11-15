/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 *
 * Description: BLOB Model Implementation.
 *
 * History:
 * 2025-05-01, Create file.
 */

#ifndef _BLOB_SERVER_H_
#define _BLOB_SERVER_H_

#include "blob.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bt_mesh_blob_server_t;

#if IS_ENABLED(BLE_MESH_BLOB_SERVER)
#define BT_MESH_BLOB_BLOCKS_MAX (DIV_ROUND_UP(MYNEWT_VAL(BLE_MESH_BLOB_SIZE_MAX), \
    MYNEWT_VAL(BLE_MESH_BLOB_BLOCK_SIZE_MIN)))
#else
#define BT_MESH_BLOB_BLOCKS_MAX 1
#endif

#define BT_MESH_MODEL_BLOB_SRV(_server) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_BLOB_SRV, _bt_mesh_blob_server_op, NULL, _server, &_bt_mesh_blob_server_cb)

/*  回调函数接口：由server使用者实现. 返回值0为成功，负数值为失败原因 */
struct bt_mesh_blob_server_cb_t {
    int (*on_start)(struct bt_mesh_blob_server_t *server, struct bt_mesh_msg_ctx *ctx,
        struct bt_mesh_blob_transfer_t *transfer);

    void (*on_end)(struct bt_mesh_blob_server_t *server, uint64_t id, bool success);

    void (*on_suspended)(struct bt_mesh_blob_server_t *server);

    void (*on_resume)(struct bt_mesh_blob_server_t *server);

    int (*on_recover)(struct bt_mesh_blob_server_t *server, struct bt_mesh_blob_transfer_t *transfer,
        const struct bt_mesh_blob_stream_t **stream);
};

/** BLOB Server 模型 */
struct bt_mesh_blob_server_t {
    const struct bt_mesh_blob_server_cb_t *cb;
    const struct bt_mesh_blob_stream_t *stream;
    struct k_work_delayable rx_timeout;
    struct bt_mesh_blob_block_t block;
    struct bt_mesh_model *mod;
    enum bt_mesh_blob_transfer_phase_t phase;

    /* Pull */
    struct {
        uint16_t chunk_idx;
        struct k_work_delayable report;
    } pull;

    struct bt_mesh_blob_server_state_t {
        struct bt_mesh_blob_transfer_t transfer;
        uint16_t cli;
        uint8_t ttl;
        uint16_t timeout_base;
        uint16_t app_idx;
        uint16_t mtu_size;

        /* MYNEWT_VAL(BLE_MESH_BLOB_SIZE_MAX)/MYNEWT_VAL(BLE_MESH_BLOB_BLOCK_SIZE_MIN)/sizeof(atomic_t32) */
        ATOMIC_DEFINE(blocks, BT_MESH_BLOB_BLOCKS_MAX); /* 位图表示缺失block块 */
    } state;
};

typedef struct {
    uint32_t max_blob_size;
    uint32_t max_block_size;
    uint32_t min_block_size;
} bt_mesh_blob_caps_t;

/** 使用接口：对应标准中blob服务端模型流程*/
int bt_mesh_blob_server_recv(struct bt_mesh_blob_server_t *server, uint64_t id,
    const struct bt_mesh_blob_stream_t *stream, uint8_t ttl, uint16_t timeout_base);

int bt_mesh_blob_server_cancel(struct bt_mesh_blob_server_t *server);

bool bt_mesh_blob_server_is_busy(const struct bt_mesh_blob_server_t *server);

uint8_t bt_mesh_blob_server_progress(const struct bt_mesh_blob_server_t *server);

void bt_mesh_blob_caps_set(bt_mesh_blob_caps_t *blob_capas);

extern const struct bt_mesh_model_op _bt_mesh_blob_server_op[];
extern const struct bt_mesh_model_cb _bt_mesh_blob_server_cb;

#ifdef __cplusplus
}
#endif

#endif /* _BLOB_SERVER_H_ */

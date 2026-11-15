/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 *
 * Description: BLOB Model Implementation.
 *
 * History:
 * 2025-05-01, Create file.
 */

#ifndef __BLOB_H_
#define __BLOB_H_

#include "model_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 标准 6.2.4.2. */
#define CLIENT_TIMEOUT_MSEC(client) \
    (10 * MSEC_PER_SEC * ((client)->inputs->timeout_base + 2) + 100 * (client)->inputs->ttl)

enum bt_mesh_blob_transfer_mode_t {
    BT_MESH_BLOB_TRANSFER_MODE_NONE    = 0x0,
    BT_MESH_BLOB_TRANSFER_MODE_PUSH    = 0x1,
    BT_MESH_BLOB_TRANSFER_MODE_PULL    = 0x2,
    BT_MESH_BLOB_TRANSFER_MODE_ALL     = 0x3,
};

/** Transfer phase. */
enum bt_mesh_blob_transfer_phase_t {
    BT_MESH_BLOB_TRANSFER_PHASE_INACTIVE                   = 0x0,
    BT_MESH_BLOB_TRANSFER_PHASE_WAITING_FOR_START          = 0x1,
    BT_MESH_BLOB_TRANSFER_PHASE_WAITING_FOR_NEXT_BLOCK     = 0x2,
    BT_MESH_BLOB_TRANSFER_PHASE_WAITING_FOR_NEXT_CHUNK     = 0x3,
    BT_MESH_BLOB_TRANSFER_PHASE_COMPLETE                   = 0x4,
    BT_MESH_BLOB_TRANSFER_PHASE_SUSPENDED                  = 0x5,
};

/** BLOB model status. */
enum bt_mesh_blob_status_t {
    BT_MESH_BLOB_SUCCESS                     = 0x0,
    BT_MESH_BLOB_ERR_INVALID_BLOCK_NUM       = 0x1,
    BT_MESH_BLOB_ERR_INVALID_BLOCK_SIZE      = 0x2,
    BT_MESH_BLOB_ERR_INVALID_CHUNK_SIZE      = 0x3,
    BT_MESH_BLOB_ERR_WRONG_PHASE             = 0x4,
    BT_MESH_BLOB_ERR_INVALID_PARAM           = 0x5,
    BT_MESH_BLOB_ERR_WRONG_BLOB_ID           = 0x6,
    BT_MESH_BLOB_ERR_BLOB_TOO_LARGE          = 0x7,
    BT_MESH_BLOB_ERR_UNSUPPORTED_MODE        = 0x8,
    BT_MESH_BLOB_ERR_INTERNAL                = 0x9,
    BT_MESH_BLOB_ERR_INFO_UNAVAILABLE        = 0xA,
};

/** BLOB block. */
#define DIV_ROUND_UP(_N, _D) (((_N) + (_D) - 1) / (_D))
struct bt_mesh_blob_block_t {
    uint32_t size;         /* 按 bytes */
    uint64_t offset;       /* 按 bytes，blob起始偏移  */
    uint16_t index;        /* 在blob中的分块序号 */
    uint16_t chunks_count; /* 在block中的chunk片数 */
    uint8_t missing_chunks[DIV_ROUND_UP(MYNEWT_VAL(BLE_MESH_BLOB_CHUNK_COUNT_MAX), 8)];
};

/** BLOB chunk. */
struct bt_mesh_blob_chunk_t {
    uint64_t offset; /* 按 bytes，block起始偏移 */
    uint32_t size;
    uint8_t *data;
};

/** BLOB transfer. */
struct bt_mesh_blob_transfer_t {
    uint64_t id; /* blob id. */
    uint32_t size;
    enum bt_mesh_blob_transfer_mode_t mode;
    uint8_t block_size_logarithm; /* 按对数取值 */
    uint16_t chunk_size;    /* chunk分片单元大小 */
};

/** BLOB stream mode. */
enum bt_mesh_blob_stream_mode {
    BT_MESH_BLOB_STREAM_READ,
    BT_MESH_BLOB_STREAM_WRITE,
};

/** BLOB 读写流：server实现写出on_write；client实现读入on_read；返回值0为成功，负数值为失败原因 */
struct bt_mesh_blob_stream_t {
    int (*on_open)(const struct bt_mesh_blob_stream_t *stream, const struct bt_mesh_blob_transfer_t *transfer,
        enum bt_mesh_blob_stream_mode mode);

    void (*on_close)(const struct bt_mesh_blob_stream_t *stream, const struct bt_mesh_blob_transfer_t *transfer);

    int (*on_block_start)(const struct bt_mesh_blob_stream_t *stream, const struct bt_mesh_blob_transfer_t *transfer,
        const struct bt_mesh_blob_block_t *block);

    void (*on_block_end)(const struct bt_mesh_blob_stream_t *stream, const struct bt_mesh_blob_transfer_t *transfer,
        const struct bt_mesh_blob_block_t *block);

    int (*on_write)(const struct bt_mesh_blob_stream_t *stream, const struct bt_mesh_blob_transfer_t *transfer,
        const struct bt_mesh_blob_block_t *block, const struct bt_mesh_blob_chunk_t *chunk);

    int (*on_read)(const struct bt_mesh_blob_stream_t *stream, const struct bt_mesh_blob_transfer_t *transfer,
        const struct bt_mesh_blob_block_t *block, const struct bt_mesh_blob_chunk_t *chunk);
};

#ifdef __cplusplus
}
#endif
#endif /* __BLOB_H_ */

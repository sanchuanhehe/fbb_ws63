/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 *
 * Description: BLOB Model Implementation.
 *
 * History:
 * 2025-05-01, Create file.
 */

#ifndef _BLOB_COMMON_H_
#define _BLOB_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#define BLOB_BLOCK_NOT_SET 0xffff

/* service data unit: opcode(1) +2 +MIC(4) */
#define BLOB_CHUNK_SDU_OVERHEAD (BT_MESH_MODEL_OP_LEN(MODEL_OP(BLOB_CHUNK)) + 2 + BT_MESH_MIC_SHORT) // 2: chunk param

#define BLOB_CHUNK_SIZE_MAX(sdu_max) ((sdu_max)-BLOB_CHUNK_SDU_OVERHEAD)
#define BLOB_CHUNK_SDU_LEN(chunk_size) (BLOB_CHUNK_SDU_OVERHEAD + (chunk_size))

#if MYNEWT_VAL(BLE_MESH_ALIGN_CHUNK_SIZE_TO_MAX_SEGMENT) || \
    MYNEWT_VAL(BLE_MESH_RX_BLOB_CHUNK_SIZE) > BLOB_CHUNK_SIZE_MAX(BT_MESH_RX_SDU_MAX)
#define BLOB_RX_CHUNK_SIZE BLOB_CHUNK_SIZE_MAX(BT_MESH_RX_SDU_MAX)
#else
#define BLOB_RX_CHUNK_SIZE MYNEWT_VAL(BLE_MESH_RX_BLOB_CHUNK_SIZE)
#endif

#if MYNEWT_VAL(BLE_MESH_ALIGN_CHUNK_SIZE_TO_MAX_SEGMENT) || \
    MYNEWT_VAL(BLE_MESH_TX_BLOB_CHUNK_SIZE) > BLOB_CHUNK_SIZE_MAX(BT_MESH_TX_SDU_MAX)
#define BLOB_TX_CHUNK_SIZE BLOB_CHUNK_SIZE_MAX(BT_MESH_TX_SDU_MAX)
#else
#define BLOB_TX_CHUNK_SIZE MYNEWT_VAL(BLE_MESH_TX_BLOB_CHUNK_SIZE)
#endif

#define BLOB_BLOCK_STATUS_MSG_HEAD_LEN 5
#if IS_ENABLED(BLE_MESH_BLOB_SERVER)
#define BLOB_BLOCK_REPORT_STATUS_MSG_MAXLEN \
    (MAX(sizeof(((struct bt_mesh_blob_block_t *)0)->missing_chunks), \
        MYNEWT_VAL(BLE_MESH_BLOB_SRV_PULL_REQ_COUNT) * 3))
#define BLOB_BLOCK_STATUS_MSG_MAXLEN \
    (BLOB_BLOCK_STATUS_MSG_HEAD_LEN + MAX(sizeof(((struct bt_mesh_blob_block_t *)0)->missing_chunks), \
        MYNEWT_VAL(BLE_MESH_BLOB_SRV_PULL_REQ_COUNT) * 3))
#else
#define BLOB_BLOCK_REPORT_STATUS_MSG_MAXLEN sizeof(((struct bt_mesh_blob_server_t *)0)->block.missing)
#define BLOB_BLOCK_STATUS_MSG_MAXLEN (BLOB_BLOCK_STATUS_MSG_HEAD_LEN + \
    sizeof(((struct bt_mesh_blob_server_t *)0)->block.missing))
#endif

#define BLOB_TRANSFER_STATUS_MSG_HEAD_LEN 17
#define BLOB_TRANSFER_STATUS_MSG_MAXLEN (BLOB_TRANSFER_STATUS_MSG_HEAD_LEN + \
    sizeof(((struct bt_mesh_blob_server_t *)0)->state.blocks))

enum bt_mesh_blob_chunks_missing_how {
    BT_MESH_BLOB_CHUNKS_MISSING_ALL     = 0x0,
    BT_MESH_BLOB_CHUNKS_MISSING_NONE    = 0x1,
    BT_MESH_BLOB_CHUNKS_MISSING_SOME    = 0x2,
    BT_MESH_BLOB_CHUNKS_MISSING_ENCODED = 0x3,
};

#define CHUNK_BITMAP_N 8
#define WRITE_BIT(var, bit, set) ((var) = (set) ? ((var) | BIT(bit)) : ((var) & ~BIT(bit)))

static inline void missing_chunks_set(uint8_t *missing_chunks, int idx, bool missing)
{
    WRITE_BIT(missing_chunks[idx / CHUNK_BITMAP_N], idx % CHUNK_BITMAP_N, missing);
}

static inline void missing_chunks_set_none(struct bt_mesh_blob_block_t *block)
{
    memset_s(block->missing_chunks, sizeof(block->missing_chunks), 0, sizeof(block->missing_chunks));
}

static inline bool missing_chunks_get(const uint8_t *missing_chunks, int idx)
{
    return !!(missing_chunks[idx / CHUNK_BITMAP_N] & BIT(idx % CHUNK_BITMAP_N));
}

static inline void missing_chunks_set_all(struct bt_mesh_blob_block_t *block)
{
    uint32_t bytes = block->chunks_count / CHUNK_BITMAP_N;

    memset_s(block->missing_chunks, sizeof(block->missing_chunks), 0xff, bytes);
    if (block->chunks_count % CHUNK_BITMAP_N) {
        block->missing_chunks[bytes] = BIT_MASK(block->chunks_count % CHUNK_BITMAP_N);
    }
}

static inline uint32_t block_size_by_index(uint32_t transfer_size, uint8_t block_size_log, uint32_t idx)
{
    if (((idx + 1U) << block_size_log) <= transfer_size) {
        return (1U << block_size_log);
    }

    return transfer_size & BIT_MASK(block_size_log);
}

/**---具体实现--
 * 编译时计算对数值
 */
#define DEBRACKET_S(...) __VA_ARGS__
#define H_UTIL_LISTIFY_0(F, sep, ...)
#define H_UTIL_LISTIFY_1(F, sep, ...) F(0, __VA_ARGS__)
#define H_UTIL_LISTIFY_2(F, sep, ...) H_UTIL_LISTIFY_1(F, sep, __VA_ARGS__) DEBRACKET_S sep F(1, __VA_ARGS__)
#define H_UTIL_LISTIFY_3(F, sep, ...) H_UTIL_LISTIFY_2(F, sep, __VA_ARGS__) DEBRACKET_S sep F(2, __VA_ARGS__)
#define H_UTIL_LISTIFY_4(F, sep, ...) H_UTIL_LISTIFY_3(F, sep, __VA_ARGS__) DEBRACKET_S sep F(3, __VA_ARGS__)
#define H_UTIL_LISTIFY_5(F, sep, ...) H_UTIL_LISTIFY_4(F, sep, __VA_ARGS__) DEBRACKET_S sep F(4, __VA_ARGS__)
#define H_UTIL_LISTIFY_6(F, sep, ...) H_UTIL_LISTIFY_5(F, sep, __VA_ARGS__) DEBRACKET_S sep F(5, __VA_ARGS__)
#define H_UTIL_LISTIFY_7(F, sep, ...) H_UTIL_LISTIFY_6(F, sep, __VA_ARGS__) DEBRACKET_S sep F(6, __VA_ARGS__)
#define H_UTIL_LISTIFY_8(F, sep, ...) H_UTIL_LISTIFY_7(F, sep, __VA_ARGS__) DEBRACKET_S sep F(7, __VA_ARGS__)
#define H_UTIL_LISTIFY_9(F, sep, ...) H_UTIL_LISTIFY_8(F, sep, __VA_ARGS__) DEBRACKET_S sep F(8, __VA_ARGS__)
#define H_UTIL_LISTIFY_10(F, sep, ...) H_UTIL_LISTIFY_9(F, sep, __VA_ARGS__) DEBRACKET_S sep F(9, __VA_ARGS__)
#define H_UTIL_LISTIFY_11(F, sep, ...) H_UTIL_LISTIFY_10(F, sep, __VA_ARGS__) DEBRACKET_S sep F(10, __VA_ARGS__)
#define H_UTIL_LISTIFY_12(F, sep, ...) H_UTIL_LISTIFY_11(F, sep, __VA_ARGS__) DEBRACKET_S sep F(11, __VA_ARGS__)
#define H_UTIL_LISTIFY_13(F, sep, ...) H_UTIL_LISTIFY_12(F, sep, __VA_ARGS__) DEBRACKET_S sep F(12, __VA_ARGS__)
#define H_UTIL_LISTIFY_14(F, sep, ...) H_UTIL_LISTIFY_13(F, sep, __VA_ARGS__) DEBRACKET_S sep F(13, __VA_ARGS__)
#define H_UTIL_LISTIFY_15(F, sep, ...) H_UTIL_LISTIFY_14(F, sep, __VA_ARGS__) DEBRACKET_S sep F(14, __VA_ARGS__)
#define H_UTIL_LISTIFY_16(F, sep, ...) H_UTIL_LISTIFY_15(F, sep, __VA_ARGS__) DEBRACKET_S sep F(15, __VA_ARGS__)
#define H_UTIL_LISTIFY_17(F, sep, ...) H_UTIL_LISTIFY_16(F, sep, __VA_ARGS__) DEBRACKET_S sep F(16, __VA_ARGS__)
#define H_UTIL_LISTIFY_18(F, sep, ...) H_UTIL_LISTIFY_17(F, sep, __VA_ARGS__) DEBRACKET_S sep F(17, __VA_ARGS__)
#define H_UTIL_LISTIFY_19(F, sep, ...) H_UTIL_LISTIFY_18(F, sep, __VA_ARGS__) DEBRACKET_S sep F(18, __VA_ARGS__)
#define H_UTIL_LISTIFY_20(F, sep, ...) H_UTIL_LISTIFY_19(F, sep, __VA_ARGS__) DEBRACKET_S sep F(19, __VA_ARGS__)
#define UTIL_PRIMITIVE_CAT(a, ...) a##__VA_ARGS__
#define UTIL_CAT(a, ...) UTIL_PRIMITIVE_CAT(a, __VA_ARGS__)
#define LISTIFY(LEN, F, sep, ...) UTIL_CAT(H_UTIL_LISTIFY_, LEN)(F, sep, __VA_ARGS__)
#define BLOB_LOG_2_CEIL(l, x) ((x) <= (1U << (l))) ? (l) :
#define BLOB_LOG_2_FLOOR(l, x) ((x) < (1U << ((l) + 1))) ? (l) :
#define BLOB_BLOCK_SIZE_LOG_CEIL(x) (LISTIFY(20, BLOB_LOG_2_CEIL, (), x) 20)
#define BLOB_BLOCK_SIZE_LOG_FLOOR(x) (LISTIFY(20, BLOB_LOG_2_FLOOR, (), x) 20)
/* Log2 of the minimum/maximum block size */
#define BLOB_BLOCK_SIZE_LOG_MIN BLOB_BLOCK_SIZE_LOG_CEIL(MYNEWT_VAL(BLE_MESH_BLOB_BLOCK_SIZE_MIN))
#define BLOB_BLOCK_SIZE_LOG_MAX BLOB_BLOCK_SIZE_LOG_FLOOR(MYNEWT_VAL(BLE_MESH_BLOB_BLOCK_SIZE_MAX))

#ifdef __cplusplus
}
#endif

#endif /* _BLOB_COMMON_H_ */
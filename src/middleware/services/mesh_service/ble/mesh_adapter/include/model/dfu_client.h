 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: DFU Model Implementation.
 *
 * History:
 * 2025-05-01, Create file.
 */

#ifndef _DFU_CLIENT_H__
#define _DFU_CLIENT_H__

#include "model_def.h"
#include "model/blob_client.h"
#include "dfu.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bt_mesh_dfu_client_t;

#define BT_MESH_DFU_CLI_TIMEOUT          10
#define BT_MESH_DFU_EFFECT_LEN           5
#define BT_MESH_BLOCKS_NOTRXED_SIZE      8
#define BT_MESH_DFU_UPDATE_INFO_GET_HEAD 2
#define BT_MESH_DFU_MSG_LEN              13
#define BT_MESH_DFU_STATUS_BIT_MASK      3
#define BT_MESH_DFU_STATUS_PHASE         5
#define BT_MESHDFU_UPDATE_BUF_DEFINE     0
#define BT_MESHDFU_UINT64_ADD_OFFSET     32

#define BT_MESH_DFU_CLI_INIT(_handlers)                       \
    {                                                         \
        .cb = (_handlers),                                      \
        .blob = { .cb = &_bt_mesh_dfu_client_blob_handlers }, \
    }

#define BT_MESH_MODEL_DFU_CLI(_cli)                                          \
    BT_MESH_MODEL_BLOB_CLI(&(_cli)->blob),                                   \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_DFU_CLI, _bt_mesh_dfu_client_op, NULL, \
             (_cli), &_bt_mesh_dfu_client_cb)


struct bt_mesh_dfu_target_t {
    struct bt_mesh_blob_target_t blob;
    uint8_t img_idx;
    uint8_t effect;
    uint8_t status;
    uint8_t phase;
};

struct bt_mesh_dfu_index_t {
    uint8_t img_cnt;
    uint8_t idx;
};

struct bt_mesh_dfu_metadata_status_t {
    uint8_t idx;
    enum bt_mesh_dfu_status_t status;
    enum bt_mesh_dfu_effect_t effect;
};

struct bt_mesh_dfu_target_status_t {
    enum bt_mesh_dfu_status_t status;
    enum bt_mesh_dfu_phase_t phase;
    enum bt_mesh_dfu_effect_t effect;
    uint64_t blob_id;
    uint8_t img_idx;
    uint8_t ttl;
    uint16_t timeout_base;
};

typedef enum bt_mesh_dfu_iter_t (*bt_mesh_dfu_img_cb_t)(
    struct bt_mesh_dfu_client_t *cli, struct bt_mesh_msg_ctx *ctx, uint8_t idx,
    uint8_t total, const struct bt_mesh_dfu_img_t *img, uint8_t *cb_data);

struct bt_mesh_dfu_client_cb {
    void (*on_suspended)(struct bt_mesh_dfu_client_t *cli);
    void (*on_ended)(struct bt_mesh_dfu_client_t *cli, enum bt_mesh_dfu_status_t reason);
    void (*on_applied)(struct bt_mesh_dfu_client_t *cli);
    void (*on_confirmed)(struct bt_mesh_dfu_client_t *cli);
    void (*on_lost_target)(struct bt_mesh_dfu_client_t *cli, struct bt_mesh_dfu_target_t *target);
};

struct bt_mesh_dfu_client_t {
    const struct bt_mesh_dfu_client_cb *cb;
    struct bt_mesh_blob_client_t blob;
    uint32_t op;
    struct bt_mesh_model *mod;
    struct {
        const struct bt_mesh_dfu_bank_t *bank;
        const struct bt_mesh_blob_stream_t *stream;
        struct bt_mesh_blob_transfer_t blob;
        uint8_t state;
        uint8_t flags;
    } transfer;
    struct {
        uint8_t ttl;
        uint8_t type;
        uint8_t img_cnt;
        uint16_t addr;
        struct k_sem sem;
        uint8_t *params;
        bt_mesh_dfu_img_cb_t img_cb;
    } req;
};

struct bt_mesh_dfu_client_transfer_blob_params_t {
    uint8_t block_size_logarithm;
    uint16_t chunk_size;
};

struct bt_mesh_dfu_client_transfer_t {
    uint64_t blob_id;
    const struct bt_mesh_dfu_bank_t *bank;
    enum bt_mesh_blob_transfer_mode_t mode;
    const struct bt_mesh_dfu_client_transfer_blob_params_t *blob_params;
};

int bt_mesh_dfu_client_send(struct bt_mesh_dfu_client_t *cli,
                            const struct bt_mesh_blob_client_inputs_t *inputs,
                            const struct bt_mesh_blob_stream_t *stream,
                            const struct bt_mesh_dfu_client_transfer_t *transfer);
int bt_mesh_dfu_client_suspend(struct bt_mesh_dfu_client_t *cli);
int bt_mesh_dfu_client_resume(struct bt_mesh_dfu_client_t *cli);
int bt_mesh_dfu_client_cancel(struct bt_mesh_dfu_client_t *cli, struct bt_mesh_msg_ctx *ctx);
int bt_mesh_dfu_client_apply(struct bt_mesh_dfu_client_t *cli);
int bt_mesh_dfu_client_confirm(struct bt_mesh_dfu_client_t *cli);
int bt_mesh_dfu_client_imgs_get(struct bt_mesh_dfu_client_t *cli, struct bt_mesh_msg_ctx *ctx,
                                bt_mesh_dfu_img_cb_t cb, uint8_t *cb_data, uint8_t max_count);
int bt_mesh_dfu_client_metadata_check(struct bt_mesh_dfu_client_t *cli, struct bt_mesh_msg_ctx *ctx, uint8_t img_idx,
                                      const struct bt_mesh_dfu_bank_t *bank, struct bt_mesh_dfu_metadata_status_t *rsp);
int bt_mesh_dfu_client_status_get(struct bt_mesh_dfu_client_t *cli, struct bt_mesh_msg_ctx *ctx,
                                  struct bt_mesh_dfu_target_status_t *rsp);
int32_t bt_mesh_dfu_client_timeout_get(void);
uint8_t bt_mesh_dfu_client_progress(struct bt_mesh_dfu_client_t *cli);
bool bt_mesh_dfu_client_is_busy(struct bt_mesh_dfu_client_t *cli);
void bt_mesh_dfu_client_timeout_set(int32_t timeout);
extern const struct bt_mesh_blob_client_cb _bt_mesh_dfu_client_blob_handlers;
extern const struct bt_mesh_model_cb _bt_mesh_dfu_client_cb;
extern const struct bt_mesh_model_op _bt_mesh_dfu_client_op[];

#ifdef __cplusplus
}
#endif

#endif /* _DFU_CLIENT_H__ */
/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: DFD Model Implementation.
 *
 * History:
 * 2025-05-01, Create file.
 */

#ifndef _DFD_SRV_H_
#define _DFD_SRV_H_

#include "model_def.h"
#include "dfd.h"
#include "dfd_srv_internal.h"
#include "blob_server.h"
#include "blob_client.h"
#include "dfu_client.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_BT_MESH_DFD_SRV_BANK_MAX_SIZE
#define CONFIG_BT_MESH_DFD_SRV_BANK_MAX_SIZE (1024 * 1024 * 2)
#endif

#ifndef CONFIG_BT_MESH_DFD_SRV_BANK_SPACE
#define CONFIG_BT_MESH_DFD_SRV_BANK_SPACE (1024 * 1024 * 2)
#endif

struct bt_mesh_dfd_srv_t;

struct bt_mesh_dfd_srv_cb_t {
    int (*recv)(struct bt_mesh_dfd_srv_t *srv, const struct bt_mesh_dfu_bank_t *bank,
        const struct bt_mesh_blob_stream_t **stream);

    void (*del)(struct bt_mesh_dfd_srv_t *srv, const struct bt_mesh_dfu_bank_t *bank);

    int (*send)(struct bt_mesh_dfd_srv_t *srv, const struct bt_mesh_dfu_bank_t *bank,
        const struct bt_mesh_blob_stream_t **stream);

    void (*phase)(struct bt_mesh_dfd_srv_t *srv, enum bt_mesh_dfd_phase_t phase);

    int (*start_oob_upload)(struct bt_mesh_dfd_srv_t *srv, const struct bt_mesh_dfu_bank_t *bank, const char *uri,
        uint8_t uri_len, const uint8_t *fwid, uint16_t fwid_len);

    void (*cancel_oob_upload)(struct bt_mesh_dfd_srv_t *srv, const struct bt_mesh_dfu_bank_t *bank);

    uint8_t (*oob_progress_get)(struct bt_mesh_dfd_srv_t *srv, const struct bt_mesh_dfu_bank_t *bank);
};

struct bt_mesh_dfd_srv_t {
    const struct bt_mesh_dfd_srv_cb_t *cb;
    struct bt_mesh_model *mod;
    struct bt_mesh_dfu_client_t dfu;
    struct bt_mesh_dfu_target_t targets[CONFIG_BT_MESH_DFD_SRV_TARGETS_MAX];
#if MYNEWT_VAL(BT_MESH_OTA_PULL)
    struct bt_mesh_blob_target_pull_t pull_ctxs[CONFIG_BT_MESH_DFD_SRV_TARGETS_MAX];
#endif
    const struct bt_mesh_blob_stream_t *stream;
    uint16_t target_cnt;
    uint16_t bank_idx;
    bool dist_self;
    bool apply;
    enum bt_mesh_dfd_phase_t phase;
    struct bt_mesh_blob_client_inputs_t inputs;

    struct {
        enum bt_mesh_dfd_upload_phase_t phase;
        struct bt_mesh_dfu_bank_t *bank;
        const struct flash_area *area;
        struct bt_mesh_blob_server_t blob;

        bool is_oob;
        bool is_pending_oob_check;
        struct {
            uint8_t uri_len;
            uint8_t uri[MYNEWT_VAL_BT_MESH_DFU_URI_MAX_LEN];
            uint16_t current_fwid_len;
            uint8_t current_fwid[MYNEWT_VAL_BT_MESH_DFU_FWID_MAX_LEN];
            struct bt_mesh_msg_ctx ctx;
        } oob;
    } upload;
};

struct bt_mesh_dfd_upload_start_t {
    uint8_t ttl;
    uint16_t timeout_base;
    uint64_t blob_id;
    uint32_t size;
    uint8_t meta_len;
    uint8_t *meta;
    uint32_t fwid_len;
    uint8_t *fwid;
};

struct bt_mesh_dfd_upload_start_oob_t {
    uint8_t uri_len;
    uint8_t *uri;
    uint32_t fwid_len;
    uint8_t *fwid;
};

#define BT_MESH_DFD_SRV_INIT(_cb)                                          \
    {                                                                      \
        .cb = (_cb), .dfu = BT_MESH_DFU_CLI_INIT(&bt_mesh_dfd_srv_dfu_cb), \
        .upload = {                                                        \
            .blob = {.cb = &bt_mesh_dfd_srv_blob_cb},                      \
        },                                                                 \
    }

#define BT_MESH_MODEL_DFD_SRV()                                                   \
    BT_MESH_MODEL_DFU_CLI(&g_dfd_srv.dfu), BT_MESH_MODEL_BLOB_SRV(&g_dfd_srv.upload.blob), \
        BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_DFD_SRV, bt_mesh_dfd_srv_op, NULL, &g_dfd_srv, &bt_mesh_dfd_srv_cb)

extern const struct bt_mesh_model_op bt_mesh_dfd_srv_op[];
extern const struct bt_mesh_model_cb bt_mesh_dfd_srv_cb;
extern const struct bt_mesh_dfu_client_cb bt_mesh_dfd_srv_dfu_cb;
extern const struct bt_mesh_blob_server_cb_t bt_mesh_dfd_srv_blob_cb;
extern struct bt_mesh_dfd_srv_t g_dfd_srv;
/**
@brief 添加目标节点
@param [in] srv: dfd sever模型实例
@return bt_mesh_dfd_status_t
*/
enum bt_mesh_dfd_status_t bt_mesh_dfd_srv_recv_add(struct bt_mesh_dfd_srv_t *srv, uint16_t addr, uint8_t img_idx);

/**
@brief 删除目标节点
@param [in] srv: dfd sever模型实例
@return bt_mesh_dfd_status_t
*/
enum bt_mesh_dfd_status_t bt_mesh_dfd_srv_recv_del_all(struct bt_mesh_dfd_srv_t *srv);

/**
@brief 分发开始
@param [in] srv: dfd sever模型实例
@return bt_mesh_dfd_status_t
*/
enum bt_mesh_dfd_status_t bt_mesh_dfd_srv_start(
    struct bt_mesh_dfd_srv_t *srv, struct bt_mesh_dfd_start_params *params, struct bt_mesh_msg_ctx *ctx);

/**
@brief 分发中断
@param [in] srv: dfd sever模型实例
@return bt_mesh_dfd_status_t
*/
enum bt_mesh_dfd_status_t bt_mesh_dfd_srv_suspend(struct bt_mesh_dfd_srv_t *srv);

/**
@brief 分发取消
@param [in] srv: dfd sever模型实例
@return bt_mesh_dfd_status_t
*/
enum bt_mesh_dfd_status_t bt_mesh_dfd_srv_cancel(struct bt_mesh_dfd_srv_t *srv, struct bt_mesh_msg_ctx *ctx);

/**
@brief 升级生效
@param [in] srv: dfd sever模型实例
@return bt_mesh_dfd_status_t
*/
enum bt_mesh_dfd_status_t bt_mesh_dfd_srv_apply(struct bt_mesh_dfd_srv_t *srv);

/**
@brief 删除固件ID
@param [in] srv: dfd sever模型实例
@return bt_mesh_dfd_status_t
*/
enum bt_mesh_dfd_status_t bt_mesh_dfd_srv_fw_del(struct bt_mesh_dfd_srv_t *srv, size_t *fwid_len, const uint8_t **fwid);

/**
@brief 删除所有固件ID
@param [in] srv: dfd sever模型实例
@return bt_mesh_dfd_status_t
*/
enum bt_mesh_dfd_status_t bt_mesh_dfd_srv_fw_del_all(struct bt_mesh_dfd_srv_t *srv);

enum bt_mesh_dfu_iter_t bt_mesh_dfd_bank_space_cb(const struct bt_mesh_dfu_bank_t *bank, void *user_data);

#ifdef __cplusplus
}
#endif

#endif /* _DFD_SRV_H_ */

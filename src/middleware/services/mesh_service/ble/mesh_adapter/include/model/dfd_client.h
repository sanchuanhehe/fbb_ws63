/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: DFD Model Implementation.
 *
 * History:
 * 2025-06-01, Create file.
 */

#ifndef _DFD_CLIENT_H_
#define _DFD_CLIENT_H_

#include "model_def.h"
#include "dfd.h"
#include "dfu_client.h"
#include "blob_client.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BT_MESH_DFD_CLI_INIT(_cb)                                          \
    {                                                                      \
        .cb = (_cb),                                                     \
        .blob = {.cb = &_bt_mesh_dfd_cli_blob_cli_cb},                      \
    }

#define BT_MESH_MODEL_DFD_CLI(_cli)                                                   \
    BT_MESH_MODEL_BLOB_CLI(&(_cli)->blob),                                   \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_DFD_CLI, _bt_mesh_dfd_client_op, NULL, \
             (_cli), &_bt_mesh_dfd_client_cb)

enum bt_mesh_dfd_client_state_t {
    DFD_CLIENT_STATE_IDLE = 0,
    DFD_CLIENT_STATE_CAPABILITY_GET,
    DFD_CLIENT_STATE_FW_UPDATE_INFO_GET,
    DFD_CLIENT_STATE_UPDATE_METADATA_CHECK,
    DFD_CLIENT_STATE_SUBSCRIPTION_SET, // 4
    DFD_CLIENT_STATE_DISTR_RECEIVER_ADD,
    DFD_CLIENT_STATE_DISTR_UPLOAD_START,
    DFD_CLIENT_STATE_BLOB_TRANSFER_GET,
    DFD_CLIENT_STATE_BLOB_INFO_GET,  // 8
    DFD_CLIENT_STATE_BLOB_TRANSFER_START,
    DFD_CLIENT_STATE_BLOB_BLOCK_START,
    DFD_CLIENT_STATE_BLOB_BLOCK_START_CHECK_RESULT,
    DFD_CLIENT_STATE_BLOB_CHUNK_START, // 0x0c
    DFD_CLIENT_STATE_BLOB_BLOCK_GET,
    DFD_CLIENT_STATE_DISTR_START,
    DFD_CLIENT_STATE_DISTR_RECEIVER_GET,
    DFD_CLIENT_STATE_DISTR_PRE_APPLY, // 0x10
    DFD_CLIENT_STATE_DISTR_APPLY,
    DFD_CLIENT_STATE_DISTR_GET,
    DFD_CLIENT_STATE_DISTR_PRE_CANCEL,
    DFD_CLIENT_STATE_DISTR_CANCEL,
    DFD_CLIENT_STATE_MAX, // 0x15
};

enum {
    FLAG_FAILED = BIT(0),
    FLAG_CANCELLED = BIT(1),
    FLAG_SKIP_CAPS_GET = BIT(2),
    FLAG_RESUME = BIT(3),
    FLAG_COMPLETED = BIT(4),
};

struct bt_mesh_dfd_client_cb_t;

struct bt_mesh_dist_receiver_entry_t {
    uint16_t addr;
    uint8_t img_idx;
};

struct bt_mesh_dist_caps_status_t {
    uint16_t max_receivers_list_size;
    uint16_t max_fw_image_list_size;
    uint32_t max_fw_image_size;
    uint32_t max_upload_space;
    uint32_t remain_upload_space;
    uint8_t oob_retrieval;
};

struct bt_mesh_dist_start_params_t {
    uint16_t app_idx;
    uint8_t ttl;
    uint16_t timeout_base;
    enum bt_mesh_blob_transfer_mode_t transfer_mode;
    bool apply;
    // uint8_t rfu                      :5;
    uint16_t img_idx;
    uint16_t group; // group or unassigned
};

// Firmware Distribution Status
struct bt_mesh_dist_status_t {
    enum bt_mesh_dfd_status_t status;
    enum bt_mesh_dfd_phase_t phase;
    // --below is optional
    uint16_t group;  // group or unassigned
    uint16_t app_idx;
    uint8_t ttl;
    uint16_t timeout_base;
    enum bt_mesh_blob_transfer_mode_t transfer_mode;
    bool apply;
    // uint8_t rfu                      :5;
    uint16_t fw_image_idx;
};

struct bt_mesh_dist_upload_start_params_t {
    uint8_t ttl;
    uint16_t timeout_base;
    uint64_t blob_id;
    uint32_t fw_size;
    uint8_t metadata_len;
    uint8_t *metadata;
    uint32_t fwid_len;
    uint8_t *fwid;
};

struct bt_mesh_dist_upload_status_t {
    enum bt_mesh_dfd_status_t status;
    enum bt_mesh_dfd_upload_phase_t upload_phase;
    // below optional
    uint8_t progress;
    enum bt_mesh_dfd_upload_type_t upload_type;
    uint32_t fwid_len;
    uint8_t *fwid;
};

struct bt_mesh_dist_fw_status_t {
    enum bt_mesh_dfd_status_t status;
    uint16_t n_imgs;
    uint16_t img_idx; // 0xFFFF: not listed/existed
    uint32_t fwid_len;
    uint8_t *fwid;
};

struct bt_mesh_dist_receivers_status_t {
    uint16_t n_receivers;
    enum bt_mesh_dfd_status_t status;
};

struct bt_mesh_dist_receivers_list_entry_t {
    uint32_t addr : 15;
    uint32_t update_phase : 4;
    uint32_t update_status : 3;
    uint32_t transfer_status : 4;
    uint32_t transfer_progress : 6;  // unit: 2%
    uint8_t update_fw_img_idx;
};

#define DFD_CLIENT_RECEIVERS_LIST_MAX_NODES 64
struct bt_mesh_dist_receivers_list_t {
    uint16_t n_receivers;
    uint16_t first_idx;
    struct bt_mesh_dist_receivers_list_entry_t nodes[DFD_CLIENT_RECEIVERS_LIST_MAX_NODES];
};

struct bt_mesh_dfd_client_inputs_t {
    // dfd server
    uint16_t addr;
    uint16_t app_idx;
    uint8_t  ttl;

    sys_slist_t dist_receivers;
    uint16_t dist_app_idx;
    uint16_t dist_group;
    uint8_t  dist_ttl;
    uint16_t dist_timeout_base;
    enum bt_mesh_blob_transfer_mode_t dist_transfer_mode;
    uint8_t dist_apply;     // 0/1
    uint16_t dist_img_idx;
    uint8_t dist_fwid_len;
    uint8_t dist_fwid[64];

    uint8_t  upload_ttl;
    uint16_t upload_timeout_base;
    uint64_t upload_blob_id;
    uint32_t upload_fw_size;
    uint8_t upload_metadata_len;
    uint8_t upload_metadata[255];
    uint8_t upload_fwid_len;
    uint8_t upload_fwid[64];
    enum bt_mesh_blob_transfer_mode_t upload_transfer_mode;
};


struct bt_mesh_dfd_client_t {
    const struct bt_mesh_dfd_client_cb_t *cb;
    struct bt_mesh_blob_client_t blob;
    uint32_t op;
    struct bt_mesh_model *mod;
    enum bt_mesh_dfd_client_state_t state;
    enum bt_mesh_dfd_phase_t phase;
    struct bt_mesh_dfd_client_inputs_t inputs;
    struct bt_mesh_blob_client_inputs_t blob_inputs;
    struct bt_mesh_blob_target_t blob_target;
    struct bt_mesh_blob_target_pull_t blob_pull;
    struct bt_mesh_dist_caps_status_t caps;
    struct bt_mesh_dist_status_t dist_status;

    struct {
        struct bt_mesh_blob_client_inputs_t inputs;
        const struct bt_mesh_dfu_bank_t *bank;
        const struct bt_mesh_blob_stream_t *stream;
        struct bt_mesh_blob_transfer_t blob;
        uint8_t state;
        uint8_t flags;
    } upload;
    struct {
        uint16_t        app_idx;
        uint8_t         ttl;
        uint16_t        addr;       // dfd server
        struct k_sem    sem;        // sync the pending op
        uint32_t        op;         // pending op of rsp
        uint8_t         *params;    // params for op result
    } req;
};

// -->
struct bt_mesh_dfd_client_cb_t {
    void (*on_suspended)(struct bt_mesh_dfd_client_t *cli);
    void (*on_ended)(struct bt_mesh_dfd_client_t *cli, enum bt_mesh_dfu_status_t reason);
    void (*on_applied)(struct bt_mesh_dfd_client_t *cli);
    void (*on_confirmed)(struct bt_mesh_dfd_client_t *cli);
    void (*on_lost_target)(struct bt_mesh_dfd_client_t *cli, struct bt_mesh_dfu_target_t *target);
};

/* Table 7.4: Firmware Distribution Client elements, procedures, and messages */
int bt_mesh_dfd_client_receivers_add(struct bt_mesh_dfd_client_t *cli,
    const struct bt_mesh_dist_receiver_entry_t *receivers, const uint32_t n,
    struct bt_mesh_dist_receivers_status_t *result); // Status
int bt_mesh_dfd_client_receivers_delete_all(struct bt_mesh_dfd_client_t *cli,
    struct bt_mesh_dist_receivers_status_t *result); // Status
int bt_mesh_dfd_client_receivers_get(struct bt_mesh_dfd_client_t *cli,
    const uint16_t from_idx, const uint32_t n_receivers, struct bt_mesh_dist_receivers_list_t *result); // List

int bt_mesh_dfd_client_caps_get(struct bt_mesh_dfd_client_t *cli,
    struct bt_mesh_dist_caps_status_t *result); // Status

int bt_mesh_dfd_client_distribute_get(struct bt_mesh_dfd_client_t *cli,
    struct bt_mesh_dist_status_t *result); // Status
int bt_mesh_dfd_client_distribute_start(struct bt_mesh_dfd_client_t *cli,
    const struct bt_mesh_dist_start_params_t *start, struct bt_mesh_dist_status_t *result); // Status
int bt_mesh_dfd_client_distribute_apply(struct bt_mesh_dfd_client_t *cli,
    struct bt_mesh_dist_status_t *result); // Status
int bt_mesh_dfd_client_distribute_suspend(struct bt_mesh_dfd_client_t *cli,
    struct bt_mesh_dist_status_t *result); // Status
int bt_mesh_dfd_client_distribute_cancel(struct bt_mesh_dfd_client_t *cli,
    struct bt_mesh_dist_status_t *result); // Status

int bt_mesh_dfd_client_upload_get(struct bt_mesh_dfd_client_t *cli,
    struct bt_mesh_dist_upload_status_t *result); // Status
int bt_mesh_dfd_client_upload_start(struct bt_mesh_dfd_client_t *cli,
    const struct bt_mesh_dist_upload_start_params_t *start, struct bt_mesh_dist_upload_status_t *result); // Status
int bt_mesh_dfd_client_upload_cancel(struct bt_mesh_dfd_client_t *cli,
    struct bt_mesh_dist_upload_status_t *result); // Status

int bt_mesh_dfd_client_fw_get(struct bt_mesh_dfd_client_t *cli,
    const uint8_t *fwid, const uint32_t fwid_len, struct bt_mesh_dist_fw_status_t *result); // Status
int bt_mesh_dfd_client_fw_get_by_index(struct bt_mesh_dfd_client_t *cli,
    const uint16_t idx, struct bt_mesh_dist_fw_status_t *result); // Status
int bt_mesh_dfd_client_fw_delete(struct bt_mesh_dfd_client_t *cli,
    const uint8_t *fwid, const uint32_t fwid_len, struct bt_mesh_dist_fw_status_t *result); // Status
int bt_mesh_dfd_client_fw_delete_all(struct bt_mesh_dfd_client_t *cli,
    struct bt_mesh_dist_fw_status_t *result); // Status

// <--

extern const struct bt_mesh_blob_client_cb _bt_mesh_dfd_cli_blob_cli_cb;
extern const struct bt_mesh_model_cb _bt_mesh_dfd_client_cb;
extern const struct bt_mesh_model_op _bt_mesh_dfd_client_op[];

#ifdef __cplusplus
}
#endif

#endif /* _DFD_CLIENT_H_ */
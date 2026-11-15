/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 *
 * Description: BLOB Model Implementation.
 *
 * History:
 * 2025-05-01, Create file.
 */

#ifndef _BLOB_CLIENT_H_
#define _BLOB_CLIENT_H_

#include <sys/types.h>
#include "blob.h"
#include "mesh/slist.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bt_mesh_blob_client_t;

#define BT_MESH_MODEL_BLOB_CLI(cli_data) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_BLOB_CLI, _bt_mesh_blob_client_op, NULL, cli_data, &_bt_mesh_blob_client_cb)

struct bt_mesh_blob_target_pull_t {
    int64_t block_report_timestamp;

    uint8_t missing_chunks[DIV_ROUND_UP(MYNEWT_VAL(BLE_MESH_BLOB_CHUNK_COUNT_MAX), 8)];
};

/** BLOB Client （目标节点）. */
struct bt_mesh_blob_target_t {
    sys_snode_t n;
    uint16_t addr;
    struct bt_mesh_blob_target_pull_t *pull;
    uint8_t status; /* bt_mesh_blob_status */

    uint8_t b_acked : 1;
    uint8_t b_skip : 1;
    uint8_t b_proc_completed : 1;
    uint8_t b_timedout : 1;
};

/** BLOB 传输信息. **/
struct bt_mesh_blob_transfer_info_t {
    enum bt_mesh_blob_status_t status;
    enum bt_mesh_blob_transfer_mode_t mode;
    enum bt_mesh_blob_transfer_phase_t phase;
    uint64_t id;            /* blob id */
    uint32_t size;          /* blob 大小 */
    uint8_t block_size_logarithm; /* 按对数取值 */
    uint16_t mtu_size;
    const uint8_t *missing_blocks; /* 位图表示 */
};

/** BLOB client 接受的输入（目标节点）. */
struct bt_mesh_blob_client_inputs_t {
    sys_slist_t targets;
    uint8_t ttl;
    uint16_t app_idx;
    uint16_t group; /* 组地址或虚拟地址时广播发出；UNASSIGNED时按目标节点列表逐个发出 */
    uint16_t timeout_base; /* 按标准 timeout = 20 seconds + (10 seconds * timeout_base) + (100 ms * TTL) */
};

/** 目标传输能力*/
struct bt_mesh_blob_client_capabilities_t {
    uint16_t mtu_size;
    enum bt_mesh_blob_transfer_mode_t modes;
    uint32_t max_size;          /* blob 大小 */
    uint8_t min_block_size_logarithm; /* 按对数取值 */
    uint8_t max_block_size_logarithm; /* 按对数取值 */
    uint16_t max_chunks;        /* block块中最大chunk片数 */
    uint16_t max_chunk_size;
};

/** BLOB Client State */
enum bt_mesh_blob_client_state_t {
    BT_MESH_BLOB_CLIENT_STATE_NONE                     = 0x0,
    BT_MESH_BLOB_CLIENT_STATE_CAPS_GET                 = 0x1,
    BT_MESH_BLOB_CLIENT_STATE_START                    = 0x2,
    BT_MESH_BLOB_CLIENT_STATE_BLOCK_START              = 0x3,
    BT_MESH_BLOB_CLIENT_STATE_BLOCK_SEND               = 0x4,
    BT_MESH_BLOB_CLIENT_STATE_BLOCK_CHECK              = 0x5,
    BT_MESH_BLOB_CLIENT_STATE_TRANSFER_CHECK           = 0x6,
    BT_MESH_BLOB_CLIENT_STATE_CANCEL                   = 0x7,
    BT_MESH_BLOB_CLIENT_STATE_SUSPENDED                = 0x8,
    BT_MESH_BLOB_CLIENT_STATE_TRANSFER_PROGRESS_GET    = 0x9,
};

/*  回调函数接口：由client使用者实现. 返回值0为成功，负数值为失败原因 */
struct bt_mesh_blob_client_cb {
    void (*on_caps)(struct bt_mesh_blob_client_t *client, const struct bt_mesh_blob_client_capabilities_t *caps);

    void (*on_lost_target)(
        struct bt_mesh_blob_client_t *client, struct bt_mesh_blob_target_t *target, enum bt_mesh_blob_status_t reason);

    void (*on_suspended)(struct bt_mesh_blob_client_t *client);

    void (*on_end)(struct bt_mesh_blob_client_t *client, const struct bt_mesh_blob_transfer_t *transfer, bool success);

    void (*on_transfer_progress)(struct bt_mesh_blob_client_t *client, struct bt_mesh_blob_target_t *target,
        const struct bt_mesh_blob_transfer_info_t *info);

    void (*on_transfer_progress_complete)(struct bt_mesh_blob_client_t *client);
};

/** 状态机具体实现：send当前状态的消息发送，next进入下一个状态 */
struct blob_client_broadcast_context_t {
    /** 目标地址dst为单播时逐个调用，否则只调用一次 */
    void (*send)(struct bt_mesh_blob_client_t *cli, uint16_t dst);

    /** send完成后调用 */
    void (*send_complete)(struct bt_mesh_blob_client_t *cli, uint16_t dst);

    /** send_complete完成后调用      */
    void (*next)(struct bt_mesh_blob_client_t *cli);

    /** send的消息是否需要响应 */
    bool b_acked;

    /** 在组播时可以强制逐个按目标单播地址发送 */
    bool b_force_unicast;

    /**  可选：超时重发次数超过时，选择不剔除*/
    bool b_optional;

    /** blob_client_broadcast和broadcast_complete调用之间 */
    bool b_inited;
    
    /* send调用之间，加入延迟ms */
    uint32_t ms_delay_between_send;
};

/** BLOB client 模型 */
struct bt_mesh_blob_client_t {
    const struct bt_mesh_blob_client_cb *cb;
    struct bt_mesh_model *mod;

    const struct bt_mesh_blob_transfer_t *transfer;
    const struct bt_mesh_blob_stream_t *stream;
    const struct bt_mesh_blob_client_inputs_t *inputs;
    uint32_t chunk_interval_ms;
    uint16_t block_count;
    uint16_t chunk_idx;
    uint16_t mtu_size;
    enum bt_mesh_blob_client_state_t state;
    struct bt_mesh_blob_block_t block;
    struct bt_mesh_blob_client_capabilities_t caps;

    struct {
        struct bt_mesh_blob_target_t *target;
        struct blob_client_broadcast_context_t ctx;
        struct k_work_delayable timeout_retry;
        int64_t cli_timestamp; /* for pull */
        struct k_work_delayable timeout_complete;
        uint16_t n_pending_blocks;
        uint8_t n_retries;
        uint8_t b_cancelled : 1;
        uint8_t b_sending : 1;
    } tx;

    struct {
        uint16_t        app_idx;
        uint8_t         ttl;
        uint16_t        addr;       // blob server
        struct k_sem    sem;        // sync the pending op
        uint32_t        op;         // pending op of rsp
        uint8_t         *params;    // params for op result
    } req;
};

/** 使用接口：对应标准中blob客服端模型流程*/
int bt_mesh_blob_client_capabilities_get(
    struct bt_mesh_blob_client_t *cli, const struct bt_mesh_blob_client_inputs_t *inputs);

int bt_mesh_blob_client_send(struct bt_mesh_blob_client_t *cli, const struct bt_mesh_blob_client_inputs_t *inputs,
    const struct bt_mesh_blob_transfer_t *transfer, const struct bt_mesh_blob_stream_t *stream);

int bt_mesh_blob_client_suspend(struct bt_mesh_blob_client_t *cli);

int bt_mesh_blob_client_resume(struct bt_mesh_blob_client_t *cli);

void bt_mesh_blob_client_cancel(struct bt_mesh_blob_client_t *cli);

int bt_mesh_blob_client_transfer_progress_get(
    struct bt_mesh_blob_client_t *cli, const struct bt_mesh_blob_client_inputs_t *inputs);

uint8_t bt_mesh_blob_client_transfer_progress_active_get(struct bt_mesh_blob_client_t *cli);

bool bt_mesh_blob_client_is_busy(struct bt_mesh_blob_client_t *cli);

void bt_mesh_blob_client_set_chunk_interval_ms(struct bt_mesh_blob_client_t *cli, uint32_t interval_ms);

/** state machine */
void blob_client_broadcast(struct bt_mesh_blob_client_t *client, const struct blob_client_broadcast_context_t *ctx);

void blob_client_broadcast_abort(struct bt_mesh_blob_client_t *client);

void blob_client_broadcast_responded(struct bt_mesh_blob_client_t *client, struct bt_mesh_blob_target_t *target);

void blob_client_broadcast_tx_completed(struct bt_mesh_blob_client_t *client);

extern const struct bt_mesh_model_op _bt_mesh_blob_client_op[];
extern const struct bt_mesh_model_cb _bt_mesh_blob_client_cb;

#ifdef __cplusplus
}
#endif

#endif /* _BLOB_CLIENT_H_ */

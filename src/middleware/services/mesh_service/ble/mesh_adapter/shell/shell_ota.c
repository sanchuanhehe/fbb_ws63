/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: ble mesh operator interface.
 *
 * History:
 * 2025-6-10, Create file.
 */

#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include "syscfg/syscfg.h"
#include "mesh/mesh.h"
#include "access.h"
#include "blob_client.h"
#include "blob_server.h"
#include "at.h"
#include "model_opcode.h"
#include "ble_mesh_hex.h"
#include "shell.h"
#include "shell_utils.h"
#include "ble_mesh_errcode.h"
#include "mesh_ota_common.h"
#include "ble_mesh_ota.h"
#include "dfu_bank.h"

#if MYNEWT_VAL(BLE_MESH_DFD_CLIENT)
#include "dfd.h"
#include "dfd_client.h"
#include "dfd_server.h"
#include "dfd_srv_internal.h"
#endif /* MYNEWT_VAL(BLE_MESH_DFD_CLIENT) */

#if MYNEWT_VAL(BLE_MESH_DFU_CLIENT) || MYNEWT_VAL(BLE_MESH_DFU_SERVER)
#include "dfu.h"
#include "dfu_server.h"

#if MYNEWT_VAL(BLE_MESH_DFU_CLIENT)
#include "dfu_client.h"
#endif /* MYNEWT_VAL(BLE_MESH_DFU_CLIENT) */

#endif

#define DFU_TARGETS_MAX         32
#define DFU_PULL_MAX            32

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
    char                 *para5;
    char                 *para6;
    char                 *para7;
    char                 *para8;
    char                 *para9;
    char                 *para10;
    char                 *para11;
    char                 *para12;
    char                 *para13;
    char                 *para14;
    char                 *para15;
} ble_mesh_ota_t;

/** dfd context for the mesh shell */
struct bt_mesh_ota_shell {
    bool b_initialized;

    uint16_t app;
    uint16_t net;
    uint8_t ttl;
    uint16_t group;
    uint16_t timeout_base;
    uint16_t dst;
};
static struct bt_mesh_ota_shell bt_mesh_ota_shell_ctx = {0};
const struct bt_mesh_blob_stream_t *bt_mesh_shell_blob_stream;

static bool bt_mesh_shell_mdl_first_get(uint16_t id, struct bt_mesh_model **mod)
{
    const struct bt_mesh_comp *comp = (const struct bt_mesh_comp *)bt_mesh_comp_get();

    if (comp == NULL) {
        return false;
    }

    for (int i = 0; i < comp->elem_count; i++) {
        *mod = bt_mesh_model_find(&comp->elem[i], id);
        if (*mod) {
            return true;
        }
    }

    return false;
}

/* "app,0,net,0,ttl,7,group,xxx,timeout,xxx,policy,xxx,fwid,xxx" */
static int ota_init(int argc, const char *argv[])
{
    ota_node_role_t role = OTA_NODE_ROLE_NONE;
    uint16_t i_req_addr = 0;
    uint16_t i_req_app_idx = 0;
    uint8_t i_req_ttl = BT_MESH_TTL_DEFAULT;

    for (int i = 1; i < argc - 1; i = i + 2) {  // 2:step
        if (!strcmp(argv[i], "role")) {
            if (!strcmp(argv[i + 1], "I")) {
                role = OTA_NODE_ROLE_INITIATOR;
            }
            if (!strcmp(argv[i + 1], "D")) {
                role = OTA_NODE_ROLE_DISTRIBUTOR;
            }
            if (!strcmp(argv[i + 1], "T")) {
                role = OTA_NODE_ROLE_TARGET;
            }
        } else if (!strcmp(argv[i], "addr")) {
            SHELL_PARAM_UINT(i_req_addr, argv[i + 1]);
        } else if (!strcmp(argv[i], "app_idx")) {
            SHELL_PARAM_UINT(i_req_app_idx, argv[i + 1]);
        } else if (!strcmp(argv[i], "ttl")) {
            SHELL_PARAM_UINT(i_req_ttl, argv[i + 1]);
        } else {
            BT_INFO_SHELL("invalid param: %s", argv[i]);
        }
    }
    if (role == OTA_NODE_ROLE_INITIATOR && i_req_addr == 0) {
        BT_INFO_SHELL("invalid distributor server addr for role I");
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (role == OTA_NODE_ROLE_NONE) {
        BT_INFO_SHELL("role not set: give role free and set to default NONE");
        ota_node_role_take(OTA_NODE_ROLE_NONE);
        return ERRCODE_BLE_MESH_SUCCESS;
    }

    if (!ota_node_role_take(role)) {
        BT_INFO_SHELL("take role=%u, failed", role);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (role == OTA_NODE_ROLE_INITIATOR) {
        struct bt_mesh_model *mod = NULL;
        if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFD_CLI, &mod)) {
            BT_WARN("mod not found");
            return ERRCODE_BLE_MESH_FAIL;
        }
        struct bt_mesh_dfd_client_t *cli = mod->user_data;
        cli->req.addr = i_req_addr;
        cli->req.app_idx = i_req_app_idx;
        cli->req.ttl = i_req_ttl;
        cli->upload.stream = bt_mesh_shell_blob_stream;
    }

    BT_INFO_SHELL("role=%u is taken! remember to do [AT+OTA=init] with no role param to give free", role);
    return ERRCODE_BLE_MESH_SUCCESS;
}

static int ota_reset(int argc, const char *argv[])
{
    bt_mesh_ota_shell_ctx.b_initialized = false;
    return 0;
}

/* BLOB SHELL START */
#if MYNEWT_VAL(BLE_MESH_BLOB_CLIENT) || MYNEWT_VAL(BLE_MESH_BLOB_SERVER)
static uint8_t blob_rx_sum;
bool bt_mesh_shell_blob_valid;
static const char *blob_data = "B10b";

static int blob_io_open(const struct bt_mesh_blob_stream_t *io, const struct bt_mesh_blob_transfer_t *transfer,
    enum bt_mesh_blob_stream_mode mode)
{
    blob_rx_sum = 0;
    bt_mesh_shell_blob_valid = true;
    return ERRCODE_BLE_MESH_SUCCESS;
}

static int blob_chunk_wr(const struct bt_mesh_blob_stream_t *io, const struct bt_mesh_blob_transfer_t *transfer,
    const struct bt_mesh_blob_block_t *block, const struct bt_mesh_blob_chunk_t *chunk)
{
    int i;

    for (i = 0; i < chunk->size; ++i) {
        blob_rx_sum += chunk->data[i];
        if (chunk->data[i] != blob_data[(i + chunk->offset) % sizeof(blob_data)]) {
            bt_mesh_shell_blob_valid = false;
        }
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

static int blob_chunk_rd(const struct bt_mesh_blob_stream_t *io, const struct bt_mesh_blob_transfer_t *transfer,
    const struct bt_mesh_blob_block_t *block, const struct bt_mesh_blob_chunk_t *chunk)
{
    for (int i = 0; i < chunk->size; ++i) {
        chunk->data[i] = blob_data[(i + chunk->offset) % sizeof(blob_data)];
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

static const struct bt_mesh_blob_stream_t dummy_blob_stream = {
    .on_open = blob_io_open,
    .on_read = blob_chunk_rd,
    .on_write = blob_chunk_wr,
};
const struct bt_mesh_blob_stream_t *bt_mesh_shell_blob_stream = &dummy_blob_stream;

static int64_t blob_time;

static int blob_srv_start(
    struct bt_mesh_blob_server_t *srv, struct bt_mesh_msg_ctx *ctx, struct bt_mesh_blob_transfer_t *xfer)
{
    BT_INFO_SHELL("BLOB start");
    blob_time = k_uptime_get();
    return ERRCODE_BLE_MESH_SUCCESS;
}

static void blob_srv_end(struct bt_mesh_blob_server_t *srv, uint64_t id, bool success)
{
    if (success) {
        int64_t duration = k_uptime_delta(&blob_time);

        BT_INFO_SHELL(
            "BLOB completed in %u.%03u s", (uint32_t)(duration / MSEC_PER_SEC), (uint32_t)(duration % MSEC_PER_SEC));
    } else {
        BT_INFO_SHELL("BLOB cancelled");
    }
}

static const struct bt_mesh_blob_server_cb_t blob_srv_cb = {
    .on_start = blob_srv_start,
    .on_end = blob_srv_end,
};
static struct bt_mesh_blob_server_t bt_mesh_shell_blob_srv = {.cb = &blob_srv_cb};

// AT+OTA=BLOBS_rx, id, 12345, ttl, 3, timeout_base, 5
static int cmd_blobs_rx(int argc, const char *argv[])
{
    uint16_t timeout_base = 0U;
    uint32_t id = 0;
    uint8_t ttl = BT_MESH_TTL_MAX;
    int err = 0;
    static struct bt_mesh_model *mod_srv = NULL;

    if (!mod_srv && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_BLOB_SRV, &mod_srv)) {
        return -ENODEV;
    }

    for (int i = 1; i < argc - 1; i = i + 2) {  // 2:step
        if (!strcmp(argv[i], "id")) {
            SHELL_PARAM_UINT(id, argv[i + 1]);
        } else if (!strcmp(argv[i], "ttl")) {
            SHELL_PARAM_UINT(ttl, argv[i + 1]);
        } else if (!strcmp(argv[i], "timeout_base")) {
            SHELL_PARAM_UINT(timeout_base, argv[i + 1]);
        } else {
            BT_INFO_SHELL("invalid param: %s", argv[i]);
        }
    }

    if (id == 0) {
        BT_INFO_SHELL("blob id not set");
        return ERRCODE_BLE_MESH_FAIL;
    }

    BT_INFO_SHELL("Receive BLOB 0x%x", id);
    blob_rx_sum = 0;
    err = bt_mesh_blob_server_recv(
        (struct bt_mesh_blob_server_t *)mod_srv->user_data, id, bt_mesh_shell_blob_stream, ttl, timeout_base);
    if (err) {
        BT_INFO_SHELL("BLOB RX setup failed (%d)", err);
    }

    return err;
}

static int cmd_blob_caps_set(int argc, const char *argv[])
{
    bt_mesh_blob_caps_t blob_capas = {0};
    uint32_t max_blob_size = 0;
    uint32_t max_block_size = 0;
    uint32_t min_block_size = 0;

    if (argc < 7) {  // 7:argc size
        BT_INFO_SHELL("argc %u error", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (strcmp(argv[1], "max_blob_size") != 0 || strcmp(argv[3], "max_block_size") != 0 ||  // 1,3:index
        strcmp(argv[5], "min_block_size") != 0) {                                           // 5:index
        BT_INFO_SHELL("param error");
        return ERRCODE_BLE_MESH_FAIL;
    }

    blob_capas.max_blob_size = strtoul(argv[2], NULL, 10);  // 2:index 10:十进制
    SHELL_PARAM_UINT(blob_capas.max_block_size, argv[4]);   // 4:index
    SHELL_PARAM_UINT(blob_capas.min_block_size, argv[6]);   // 6:index

    bt_mesh_blob_caps_set(&blob_capas);

    return ERRCODE_BLE_MESH_SUCCESS;
}

// blobc
static struct bt_mesh_model* blob_cli_mod_get(void);
static struct {
    uint16_t group;
    struct bt_mesh_blob_client_inputs_t inputs;
    struct bt_mesh_blob_target_t targets[32];
    struct bt_mesh_blob_target_pull_t pull[32];
    uint8_t target_count;
    struct bt_mesh_blob_transfer_t xfer;
    bool do_tx_on_caps;
} blob_cli_xfer;

static void blob_cli_lost_target(
    struct bt_mesh_blob_client_t *cli, struct bt_mesh_blob_target_t *target, enum bt_mesh_blob_status_t reason)
{
    BT_INFO_SHELL("Mesh Blob: Lost target 0x%04x (reason: %u)", target->addr, reason);
}
static void blob_cli_caps(struct bt_mesh_blob_client_t *cli, const struct bt_mesh_blob_client_capabilities_t *caps)
{
    static const char *const modes[] = {
        "none",
        "push",
        "pull",
        "all",
    };

    if (!caps || caps->modes < 0 || caps->modes > BT_MESH_BLOB_TRANSFER_MODE_ALL) {
        BT_INFO_SHELL("None of the targets can be used for BLOB transfer");
        return;
    }

    BT_INFO_SHELL("Mesh BLOB: capabilities:");
    BT_INFO_SHELL("\tMax BLOB size: %u bytes", caps->max_size);
    BT_INFO_SHELL("\tBlock size: %u-%u (%u-%u bytes)", caps->min_block_size_logarithm, caps->max_block_size_logarithm,
        1 << caps->min_block_size_logarithm, 1 << caps->max_block_size_logarithm);
    BT_INFO_SHELL("\tMax chunks: %u", caps->max_chunks);
    BT_INFO_SHELL("\tChunk size: %u", caps->max_chunk_size);
    BT_INFO_SHELL("\tMTU size: %u", caps->mtu_size);
    BT_INFO_SHELL("\tModes: %s", modes[caps->modes]);
    if (blob_cli_xfer.do_tx_on_caps) {
        int err = 0;
        struct bt_mesh_model* mod_cli = blob_cli_mod_get();
        if (!mod_cli) {
            return;
        }
        blob_cli_xfer.xfer.block_size_logarithm = caps->max_block_size_logarithm;
        blob_cli_xfer.xfer.chunk_size = caps->max_chunk_size;
        err = bt_mesh_blob_client_send((struct bt_mesh_blob_client_t *)mod_cli->user_data,
            &blob_cli_xfer.inputs,
            &blob_cli_xfer.xfer,
            bt_mesh_shell_blob_stream);
        if (err) {
            BT_INFO_SHELL("BLOB transfer TX failed (err: %d)", err);
        }
    }
}

static void blob_cli_end(struct bt_mesh_blob_client_t *cli, const struct bt_mesh_blob_transfer_t *xfer, bool success)
{
    if (success) {
        BT_INFO_SHELL("Mesh BLOB transfer complete.");
    } else {
        BT_INFO_SHELL("Mesh BLOB transfer failed.");
    }
    bt_mesh_mbt_test(false);
}

#define XFER_PROG_BLOCK_BITS 8
static uint8_t get_progress(const struct bt_mesh_blob_transfer_info_t *info)
{
    uint8_t total_blocks;
    uint8_t blocks_not_rxed = 0;
    uint8_t blocks_not_rxed_size;
    int i;

    total_blocks = DIV_ROUND_UP(info->size, 1U << info->block_size_logarithm);

    blocks_not_rxed_size = DIV_ROUND_UP(total_blocks, XFER_PROG_BLOCK_BITS);

    for (i = 0; i < blocks_not_rxed_size; i++) {
        blocks_not_rxed += info->missing_blocks[i % XFER_PROG_BLOCK_BITS] & (1 << (i % XFER_PROG_BLOCK_BITS));
    }

    return (total_blocks - blocks_not_rxed) / total_blocks;
}
static void xfer_progress(struct bt_mesh_blob_client_t *cli,
    struct bt_mesh_blob_target_t *target, const struct bt_mesh_blob_transfer_info_t *info)
{
    uint8_t progress = get_progress(info);

    BT_INFO_SHELL("BLOB transfer progress received from target 0x%04x:\n"
                   "\tphase: %d\n"
                   "\tprogress: %u%%",
        target->addr,
        info->phase,
        progress);
}

static void xfer_progress_complete(struct bt_mesh_blob_client_t *cli)
{
    BT_INFO_SHELL("Determine BLOB transfer progress procedure complete");
}

static const struct bt_mesh_blob_client_cb blob_cli_handlers = {
    .on_lost_target = blob_cli_lost_target,
    .on_caps = blob_cli_caps,
    .on_end = blob_cli_end,
    .on_transfer_progress = xfer_progress,
    .on_transfer_progress_complete = xfer_progress_complete,
};

struct bt_mesh_blob_client_t bt_mesh_shell_blob_cli = {.cb = &blob_cli_handlers};

static struct bt_mesh_model* blob_cli_mod_get(void)
{
    static struct bt_mesh_model *mod_cli = NULL;
    if (mod_cli == NULL) {
        if (bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_BLOB_CLI, &mod_cli) && mod_cli) {
            mod_cli->user_data = &bt_mesh_shell_blob_cli;
            bt_mesh_shell_blob_cli.mod = mod_cli;
            mod_cli->cb->init(mod_cli);
        }
    }
    return mod_cli;
}

static void blob_cli_inputs_prepare(uint16_t group)
{
    int i;

    blob_cli_xfer.inputs.ttl = BT_MESH_TTL_DEFAULT;
    blob_cli_xfer.inputs.group = group;
    sys_slist_init(&blob_cli_xfer.inputs.targets);

    for (i = 0; i < blob_cli_xfer.target_count; ++i) {
        /* Reset target context. */
        uint16_t addr = blob_cli_xfer.targets[i].addr;

        memset_s(&blob_cli_xfer.targets[i], sizeof(struct bt_mesh_blob_target_t),
            0, sizeof(struct bt_mesh_blob_target_t));
        memset_s(&blob_cli_xfer.pull[i], sizeof(struct bt_mesh_blob_target_pull_t),
            0, sizeof(struct bt_mesh_blob_target_pull_t));
        blob_cli_xfer.targets[i].addr = addr;
        blob_cli_xfer.targets[i].pull = &blob_cli_xfer.pull[i];

        sys_slist_append(&blob_cli_xfer.inputs.targets, &blob_cli_xfer.targets[i].n);
    }
}

#define SHELL_BLOBC_BLOCK_SIZE_LOG 13
#define SHELL_BLOBC_chunk_size 128
static int cmd_blobc_tx_parse_args(size_t argc, const char *argv[], uint16_t *cancel_after_start)
{
    uint32_t id = 0;

    blob_cli_xfer.group = 0;
    blob_cli_xfer.do_tx_on_caps = true;
    blob_cli_xfer.xfer.id = 0;
    blob_cli_xfer.xfer.size = 0;
    blob_cli_xfer.xfer.block_size_logarithm = SHELL_BLOBC_BLOCK_SIZE_LOG;
    blob_cli_xfer.xfer.chunk_size = SHELL_BLOBC_chunk_size;
    blob_cli_xfer.inputs.timeout_base = 0;
    blob_cli_xfer.xfer.mode = BT_MESH_BLOB_TRANSFER_MODE_PUSH;
    for (uint32_t i = 1; i < argc - 1; i = i + 2) {  // 2:step
        if (!strcmp(argv[i], "group")) {
            SHELL_PARAM_UINT(blob_cli_xfer.group, argv[i + 1]);
        } else if (!strcmp(argv[i], "id")) {
            SHELL_PARAM_UINT(id, argv[i + 1]);
        } else if (!strcmp(argv[i], "size")) {
            SHELL_PARAM_UINT(blob_cli_xfer.xfer.size, argv[i + 1]);
        } else if (!strcmp(argv[i], "mode")) {
            SHELL_PARAM_UINT(blob_cli_xfer.xfer.mode, argv[i + 1]);
        } else if (!strcmp(argv[i], "block")) {
            SHELL_PARAM_UINT(blob_cli_xfer.xfer.block_size_logarithm, argv[i + 1]);
        } else if (!strcmp(argv[i], "chunk")) {
            SHELL_PARAM_UINT(blob_cli_xfer.xfer.chunk_size, argv[i + 1]);
        } else if (!strcmp(argv[i], "timeout_base")) {
            SHELL_PARAM_UINT(blob_cli_xfer.inputs.timeout_base, argv[i + 1]);
        } else if (!strcmp(argv[i], "cancel_after_start")) {
            SHELL_PARAM_UINT(*cancel_after_start, argv[i + 1]);
        } else {
            BT_INFO_SHELL("invalid param: %s", argv[i]);
        }
    }

    blob_cli_xfer.xfer.id = id;
    return ERRCODE_BLE_MESH_SUCCESS;
}

static int cmd_blobc_tx(int argc, const char *argv[])
{
    uint16_t cancel_after_start = 0, group = 0;
    int err = 0;

    struct bt_mesh_model *mod_cli = blob_cli_mod_get();
    if (!mod_cli) {
        return ERRCODE_BLE_MESH_MODEL_NOT_FOUND;
    }

    cmd_blobc_tx_parse_args(argc, argv, &cancel_after_start);
    group = blob_cli_xfer.group;

    if (blob_cli_xfer.xfer.id == 0 || blob_cli_xfer.xfer.size == 0 || blob_cli_xfer.target_count == 0) {
        BT_INFO_SHELL("Failed: not set blob id or size, or targets");
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (cancel_after_start) {
        bt_mesh_mbt_test(true);
    }
    blob_cli_inputs_prepare(group);
    BT_INFO_SHELL("Sending transfer 0x%x (mode: %s, %u bytes) to 0x%04x",
        (uint32_t)blob_cli_xfer.xfer.id,
        blob_cli_xfer.xfer.mode == BT_MESH_BLOB_TRANSFER_MODE_PUSH ? "push" : "pull",
        blob_cli_xfer.xfer.size,
        group);

    if (blob_cli_xfer.do_tx_on_caps) {
        err = bt_mesh_blob_client_capabilities_get(
            (struct bt_mesh_blob_client_t *)mod_cli->user_data, &blob_cli_xfer.inputs);
    } else {
        err = bt_mesh_blob_client_send((struct bt_mesh_blob_client_t *)mod_cli->user_data,
            &blob_cli_xfer.inputs,
            &blob_cli_xfer.xfer,
            bt_mesh_shell_blob_stream);
    }
    return err;
}

static int cmd_blobc_target(int argc, const char *argv[])
{
    struct bt_mesh_blob_target_t *t;
    int err = 0;

    if (blob_cli_xfer.target_count == ARRAY_SIZE(blob_cli_xfer.targets)) {
        BT_INFO_SHELL("No more room");
        return ERRCODE_BLE_MESH_SUCCESS;
    }

    if (argc != 3 || strcmp(argv[1], "addr")) { // 3: argc
        BT_INFO_SHELL("wrong params: expecting \"=addr,xx\"], argc=%d", argc);
        return ERRCODE_BLE_MESH_SUCCESS;
    }

    t = &blob_cli_xfer.targets[blob_cli_xfer.target_count];
    SHELL_PARAM_UINT(t->addr, argv[2]); // 2: addr

    if (err) {
        BT_INFO_SHELL("Unable to parse input string argument");
        return err;
    }

    BT_INFO_SHELL("Added target 0x%04x", t->addr);

    blob_cli_xfer.target_count++;
    return ERRCODE_BLE_MESH_SUCCESS;
}
static int cmd_blobc_caps(int argc, const char *argv[])
{
    uint16_t group;
    int err = 0;

    struct bt_mesh_model* mod_cli = blob_cli_mod_get();
    if (!mod_cli) {
        return ERRCODE_BLE_MESH_MODEL_NOT_FOUND;
    }

    BT_INFO_SHELL("Retrieving transfer capabilities...");

    group = BT_MESH_ADDR_UNASSIGNED;
    blob_cli_xfer.inputs.timeout_base = 0;
    for (int i = 1; i < argc - 1; i = i + 2) {  // 2:step
        if (!strcmp(argv[i], "group")) {
            SHELL_PARAM_UINT(group, argv[i + 1]);
        } else if (!strcmp(argv[i], "timeout_base")) {
            SHELL_PARAM_UINT(blob_cli_xfer.inputs.timeout_base, argv[i + 1]);
        } else {
            BT_INFO_SHELL("invalid param: %s", argv[i]);
        }
    }

    if (!blob_cli_xfer.target_count) {
        BT_INFO_SHELL("Failed: No targets");
        return ERRCODE_BLE_MESH_SUCCESS;
    }

    blob_cli_inputs_prepare(group);

    err = bt_mesh_blob_client_capabilities_get((struct bt_mesh_blob_client_t *)mod_cli->user_data,
        &blob_cli_xfer.inputs);
    if (err) {
        BT_INFO_SHELL("bt_mesh_blob_client_capabilities_get failed (err: %d)", err);
        return err;
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

static int cmd_blobc_tx_cancel(int argc, const char *argv[])
{
    struct bt_mesh_model* mod_cli = blob_cli_mod_get();
    if (!mod_cli) {
        return ERRCODE_BLE_MESH_MODEL_NOT_FOUND;
    }

    BT_INFO_SHELL("Cancelling transfer");
    bt_mesh_blob_client_cancel((struct bt_mesh_blob_client_t *)mod_cli->user_data);

    return ERRCODE_BLE_MESH_SUCCESS;
}

static int cmd_blobc_tx_get(int argc, const char *argv[])
{
    uint16_t group;
    int err;

    struct bt_mesh_model* mod_cli = blob_cli_mod_get();
    if (!mod_cli) {
        return ERRCODE_BLE_MESH_MODEL_NOT_FOUND;
    }

    group = BT_MESH_ADDR_UNASSIGNED;
    for (int i = 1; i < argc - 1; i = i + 2) {  // 2:step
        if (!strcmp(argv[i], "group")) {
            SHELL_PARAM_UINT(group, argv[i + 1]);
        } else {
            BT_INFO_SHELL("invalid param: %s", argv[i]);
        }
    }

    if (!blob_cli_xfer.target_count) {
        BT_INFO_SHELL("Failed: No targets");
        return ERRCODE_BLE_MESH_STATUS_ERR;
    }

    blob_cli_inputs_prepare(group);

    err = bt_mesh_blob_client_transfer_progress_get((struct bt_mesh_blob_client_t *)mod_cli->user_data,
        &blob_cli_xfer.inputs);
    if (err) {
        BT_INFO_SHELL("ERR %d", err);
        return err;
    }
    return ERRCODE_BLE_MESH_SUCCESS;
}

static int cmd_blobc_tx_suspend(int argc, const char *argv[])
{
    struct bt_mesh_model* mod_cli = blob_cli_mod_get();
    if (!mod_cli) {
        return ERRCODE_BLE_MESH_MODEL_NOT_FOUND;
    }

    BT_INFO_SHELL("Suspending transfer");
    bt_mesh_blob_client_suspend((struct bt_mesh_blob_client_t *)mod_cli->user_data);

    return ERRCODE_BLE_MESH_SUCCESS;
}

static int cmd_blobc_tx_resume(int argc, const char *argv[])
{
    struct bt_mesh_model* mod_cli = blob_cli_mod_get();
    if (!mod_cli) {
        return ERRCODE_BLE_MESH_MODEL_NOT_FOUND;
    }

    BT_INFO_SHELL("Resuming transfer");
    bt_mesh_blob_client_resume((struct bt_mesh_blob_client_t *)mod_cli->user_data);

    return ERRCODE_BLE_MESH_SUCCESS;
}

#endif  // #if MYNEWT_VAL(BLE_MESH_BLOB_CLIENT) || MYNEWT_VAL(BLE_MESH_BLOB_SERVER)

#if MYNEWT_VAL(BLE_MESH_DFD_CLIENT)
static void dfd_print_dist_status(const int err, const struct bt_mesh_dist_status_t *status)
{
    BT_INFO_SHELL("{ err=%d \"status\": %d, \"phase\": %d", err, status->status, status->phase);
    if ((err == 0) && (status->phase != BT_MESH_DFD_PHASE_IDLE)) {
        BT_INFO_SHELL(", \"group\": %d, \"app_idx\": %d, "
                      "\"ttl\": %d, \"timeout_base\": %d, \"transfer_mode\": %d, "
                      "\"apply\": %d, \"bank_idx\": %d",
            status->group,
            status->app_idx,
            status->ttl,
            status->timeout_base,
            status->transfer_mode,
            status->apply,
            status->fw_image_idx);
    }
    BT_INFO_SHELL(" }");
}

static void dfd_print_upload_status(const int err, const struct bt_mesh_dist_upload_status_t *status)
{
    BT_INFO_SHELL("{ err=%d \"status\": %d, \"phase\": %d", err, status->status, status->upload_phase);
    if ((err == 0) && (status->upload_phase != BT_MESH_DFD_UPLOAD_PHASE_IDLE)) {
        BT_INFO_SHELL(", \"progress\": %d, \"upload_type\": %d, "
                      "\"fwid_len\": %d, \"fwid\": todo",
            status->progress,
            status->upload_type,
            status->fwid_len);
    }
    BT_INFO_SHELL(" }");
}

static void dfd_print_fw_status(const int err, const struct bt_mesh_dist_fw_status_t *status)
{
    BT_INFO_SHELL("{ \"err = %d status\": %d, \"bank_cnt\": %d, \"idx\": %d",
        err, status->status, status->n_imgs, status->img_idx);
    if ((err == 0) && (status->fwid_len && status->fwid)) {
        BT_INFO_SHELL(", \"fwid\": \"");
        for (uint32_t i = 0; i < status->fwid_len; i++) {
            BT_INFO_SHELL("%02x", status->fwid[i]);
        }
        BT_INFO_SHELL("\"");
    }
    BT_INFO_SHELL(" }");
}

int cmd_dfd_receivers_add(int argc, const char *argv[])
{
    int err = 0, addr_num = 0;
    struct bt_mesh_model *mod = NULL;
    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFD_CLI, &mod)) {
        BT_WARN("mod found none with err=%d", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct bt_mesh_dfd_client_t *cli = mod->user_data;
    struct bt_mesh_dist_status_t d_status = {0};
    struct bt_mesh_dist_receivers_status_t r_status = {0};

    if (argc < 5 || (strcmp(argv[0], "receivers_add") != 0) || (strcmp(argv[1], "addr") != 0) || // 5: argc
        (strcmp(argv[3], "img_idx") != 0)) { // 3:argv index
        BT_INFO_SHELL("cmd error: receivers_add,addr,xx,img_idx,xx");
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint8_t img_idx;
    char *beg = (char *)argv[2]; // index
    uint8_t addr[5] = {0}; // 5:size
    struct bt_mesh_dist_receiver_entry_t target_addr[50] = {0}; // 50:addr num

    SHELL_PARAM_UINT(img_idx, argv[4]);  // 4:argv index

    int len = strlen(argv[2]); // 2:index
    if (len > 200 || len % 4 != 0) { // 200:max len 4:addr len
        BT_INFO_SHELL("addr error, len %u", len);
        return ERRCODE_BLE_MESH_FAIL;
    }
    while (len >= 4) { // 4:len
        if (memcpy_s(addr, sizeof(addr), beg, 4) != EOK) { // 4:len
            return ERRCODE_BLE_MESH_FAIL;
        }
        target_addr[addr_num].addr = strtol((char *)addr, NULL, 16); // 16:进制
        target_addr[addr_num].img_idx = img_idx;
        addr_num++;
        beg += 4; // 4:len
        len -= 4; // 4:len
    }

    err = bt_mesh_dfd_client_receivers_add(cli, target_addr, addr_num, &r_status);
    if (err || r_status.status != BT_MESH_DFD_SUCCESS) {
        BT_WARN("failed: err=%d, status=%u", err, r_status.status);
        return ERRCODE_BLE_MESH_FAIL;
    } else {
        BT_INFO_SHELL("n_receivers now %u", r_status.n_receivers);
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

int cmd_dfd_receivers_delete_all(int argc, const char *argv[])
{
    int err = 0;
    struct bt_mesh_model *mod = NULL;
    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFD_CLI, &mod)) {
        BT_WARN("mod found none with err=%d", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct bt_mesh_dfd_client_t *cli = mod->user_data;
    struct bt_mesh_dist_status_t d_status;
    struct bt_mesh_dist_receivers_status_t r_status;

    err = bt_mesh_dfd_client_receivers_delete_all(cli, &r_status);
    if (err || r_status.status != BT_MESH_DFD_SUCCESS) {
        BT_WARN("failed: err=%d, status=%u", err, r_status.status);
        return ERRCODE_BLE_MESH_FAIL;
    } else {
        BT_INFO_SHELL("n_receivers now %u", r_status.n_receivers);
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

int cmd_dfd_receivers_get(int argc, const char *argv[])
{
    int err = 0;
    struct bt_mesh_model *mod = NULL;
    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFD_CLI, &mod)) {
        BT_WARN("mod found none with err=%d", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint16_t from_idx = 0;
    uint32_t n_receivers = 0;
    if (argc < 5 || strcmp(argv[1], "first") != 0 || strcmp(argv[3], "cnt") != 0) { // 5,1,3:argc argv index
        BT_WARN("params wrong:argc=%u", argc);
        return ERRCODE_BLE_MESH_FAIL;
    }
    SHELL_PARAM_UINT(from_idx, argv[2]);    // 2:argv index
    SHELL_PARAM_UINT(n_receivers, argv[4]); // 4:argv index

    struct bt_mesh_dfd_client_t *cli = mod->user_data;
    struct bt_mesh_dist_receivers_list_t r_list;
    err = bt_mesh_dfd_client_receivers_get(cli, from_idx, n_receivers, &r_list);
    if (err) {
        BT_WARN("failed: err=%d", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    for (int i = 0; i < r_list.n_receivers; i++) {
        BT_INFO_SHELL("\t\t\"%d\": { \"addr\": %d, \"phase\": %d, "
                      "\"status\": %d, \"blob_status\": %d, \"blob_progress\": %d, "
                      "\"img_idx\": %d }%s",
            i + r_list.first_idx,
            r_list.nodes[i].addr,
            r_list.nodes[i].update_phase,
            r_list.nodes[i].update_status,
            r_list.nodes[i].transfer_status,
            r_list.nodes[i].transfer_progress,
            r_list.nodes[i].update_fw_img_idx,
            (i == r_list.n_receivers - 1) ? "" : ",");
    }
    BT_INFO_SHELL("\t}\n}");
    return ERRCODE_BLE_MESH_SUCCESS;
}

static int dfd_upload_params_get(int argc, const char *argv[], struct bt_mesh_dist_upload_start_params_t *params)
{
    params->ttl = BT_MESH_TTL_DEFAULT;
    params->timeout_base = 1;
    int err = bt_rand(&params->blob_id, sizeof(params->blob_id));
    if (err) {
        return err;
    }

    for (int i = 1; i < argc - 1; i = i + 2) {  // 2:step
        if (!strcmp(argv[i], "ttl")) {
            SHELL_PARAM_UINT(params->ttl, argv[i + 1]);
        } else if (!strcmp(argv[i], "timeout_base")) {
            SHELL_PARAM_UINT(params->timeout_base, argv[i + 1]);
        } else if (!strcmp(argv[i], "blob_id")) {
            SHELL_PARAM_UINT(params->blob_id, argv[i + 1]);
        } else if (!strcmp(argv[i], "fw_size")) {
            SHELL_PARAM_UINT(params->fw_size, argv[i + 1]);
        } else if (!strcmp(argv[i], "metadata")) {
            static uint8_t metadata[MYNEWT_VAL_BT_MESH_DFU_FWID_MAX_LEN];
            uint32_t hexlen = strlen(argv[i + 1]);
            uint32_t metadata_len = ble_mesh_hex2bin(argv[i + 1], metadata, sizeof(metadata));
            if (metadata_len < 0 || metadata_len != ((hexlen + 1) / 2)) {  // 2:half
                BT_INFO_SHELL("wrong params: metadata_len");
                return ERRCODE_BLE_MESH_FAIL;
            }
            params->metadata = metadata;
            params->metadata_len = metadata_len;
        } else if (!strcmp(argv[i], "fwid")) {
            uint32_t fwid_len = 0;
            static uint8_t fwid[MYNEWT_VAL_BT_MESH_DFU_FWID_MAX_LEN];
            uint32_t hexlen = strlen(argv[i + 1]);
            fwid_len = ble_mesh_hex2bin(argv[i + 1], fwid, sizeof(fwid));
            if (fwid_len < 0 || fwid_len != ((hexlen + 1) / 2)) {  // 2:half
                BT_INFO_SHELL("wrong params: fwid_len");
                return ERRCODE_BLE_MESH_FAIL;
            }
            params->fwid = fwid;
            params->fwid_len = fwid_len;
        } else {
            BT_INFO_SHELL("invalid param: %s", argv[i]);
        }
    }

    if (params->fw_size == 0 || params->fwid_len == 0) {
        BT_INFO_SHELL("err: fw not set");
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

int cmd_dfd_upload_start(int argc, const char *argv[])
{
    int err = 0;
    struct bt_mesh_model *mod = NULL;
    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFD_CLI, &mod)) {
        BT_WARN("mod found none with err=%d", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct bt_mesh_dfd_client_t *cli = mod->user_data;
    struct bt_mesh_dist_upload_start_params_t start = {0};
    struct bt_mesh_dist_upload_status_t status = {0};

    err = dfd_upload_params_get(argc, argv, &start);
    if (err) {
        BT_WARN("params wrong: err=%d", err);
        return ERRCODE_BLE_MESH_FAIL;
    }
    err = bt_mesh_dfd_client_upload_start(cli, &start, &status);
    if (err || status.status != BT_MESH_DFD_SUCCESS) {
        dfd_print_upload_status(err, &status);
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}
int cmd_dfd_upload_get(int argc, const char *argv[])
{
    int err = 0;
    struct bt_mesh_model *mod = NULL;
    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFD_CLI, &mod)) {
        BT_WARN("mod found none with err=%d", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct bt_mesh_dfd_client_t *cli = mod->user_data;
    struct bt_mesh_dist_upload_status_t status;

    err = bt_mesh_dfd_client_upload_get(cli, &status);
    dfd_print_upload_status(err, &status);

    return err;
}
int cmd_dfd_upload_cancel(int argc, const char *argv[])
{
    int err = 0;
    struct bt_mesh_model *mod = NULL;
    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFD_CLI, &mod)) {
        BT_WARN("mod found none with err=%d", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct bt_mesh_dfd_client_t *cli = mod->user_data;
    struct bt_mesh_dist_upload_status_t status;

    err = bt_mesh_dfd_client_upload_cancel(cli, &status);
    if (err || status.status != BT_MESH_DFD_SUCCESS) {
        dfd_print_upload_status(err, &status);
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

int cmd_dfd_capabilities_get(int argc, const char *argv[])
{
    int err = 0;
    struct bt_mesh_model *mod = NULL;
    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFD_CLI, &mod)) {
        BT_WARN("mod found none with err=%d", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct bt_mesh_dfd_client_t *cli = mod->user_data;
    struct bt_mesh_dist_caps_status_t *caps = &cli->caps;

    err = bt_mesh_dfd_client_caps_get(cli, caps);
    if (err) {
        BT_WARN("failed: err=%d", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    BT_INFO_SHELL("{ \"targets_max\": %d, \"bank_cnt\": %d, \"bank_max_size\": %d, "
                  "\"bank_space\": %d, \"remaining_space\": %d, \"oob_supported\": false }",
        caps->max_receivers_list_size,
        caps->max_fw_image_list_size,
        caps->max_fw_image_size,
        caps->max_upload_space,
        caps->remain_upload_space);

    return ERRCODE_BLE_MESH_SUCCESS;
}

int cmd_dfd_get(int argc, const char *argv[])
{
    int err = 0;
    struct bt_mesh_model *mod = NULL;
    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFD_CLI, &mod)) {
        BT_WARN("mod found none with err=%d", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct bt_mesh_dfd_client_t *cli = mod->user_data;
    struct bt_mesh_dist_status_t status;

    err = bt_mesh_dfd_client_distribute_get(cli, &status);
    dfd_print_dist_status(err, &status);

    return ERRCODE_BLE_MESH_SUCCESS;
}

static int dfd_start_params_get(uint32_t argc, const char *argv[], struct bt_mesh_dist_start_params_t *params)
{
    int err = 0;
    params->app_idx = 0;
    params->img_idx = 0;
    params->group = BT_MESH_ADDR_UNASSIGNED;
    params->apply = true;
    params->ttl = 10;  // 10:中继10跳
    params->timeout_base = 28;  // 28:超时时间301s
    params->transfer_mode = BT_MESH_BLOB_TRANSFER_MODE_PUSH;

    for (int i = 1; i < argc - 1; i = i + 2) {  // 2:step
        if (!strcmp(argv[i], "app_idx")) {
            SHELL_PARAM_UINT(params->app_idx, argv[i + 1]);
        } else if (!strcmp(argv[i], "img_idx")) {
            SHELL_PARAM_UINT(params->img_idx, argv[i + 1]);
        } else if (!strcmp(argv[i], "group")) {
            SHELL_PARAM_UINT(params->group, argv[i + 1]);
        } else if (!strcmp(argv[i], "apply")) {
            SHELL_PARAM_UINT(params->apply, argv[i + 1]);
        } else if (!strcmp(argv[i], "ttl")) {
            SHELL_PARAM_UINT(params->ttl, argv[i + 1]);
        } else if (!strcmp(argv[i], "timeout_base")) {
            SHELL_PARAM_UINT(params->timeout_base, argv[i + 1]);
        } else if (!strcmp(argv[i], "transfer_mode")) {
            SHELL_PARAM_UINT(params->transfer_mode, argv[i + 1]);
        } else {
            BT_INFO_SHELL("invalid param: %s", argv[i]);
            return ERRCODE_BLE_MESH_FAIL;
        }
    }

    BT_INFO_SHELL("%d %d %d %d %d %d %d",
        params->app_idx, params->img_idx, params->group, params->apply,
        params->ttl, params->timeout_base, params->transfer_mode);

    return err;
}

int cmd_dfd_start(int argc, const char *argv[])
{
    int err = 0;
    struct bt_mesh_model *mod = NULL;
    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFD_CLI, &mod)) {
        BT_WARN("mod found none with err=%d", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct bt_mesh_dfd_client_t *cli = mod->user_data;
    struct bt_mesh_dist_start_params_t start;
    struct bt_mesh_dist_status_t status;

    err = dfd_start_params_get(argc, argv, &start);
    if (err) {
        BT_WARN("params wrong: err=%d", err);
        return ERRCODE_BLE_MESH_FAIL;
    }
    err = bt_mesh_dfd_client_distribute_start(cli, &start, &status);
    if (err || status.status != BT_MESH_DFD_SUCCESS) {
        dfd_print_dist_status(err, &status);
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

int cmd_dfd_suspend(int argc, const char *argv[])
{
    int err = 0;
    struct bt_mesh_model *mod = NULL;
    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFD_CLI, &mod)) {
        BT_WARN("mod found none with err=%d", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct bt_mesh_dfd_client_t *cli = mod->user_data;
    struct bt_mesh_dist_status_t status;

    err = bt_mesh_dfd_client_distribute_suspend(cli, &status);
    if (err || status.status != BT_MESH_DFD_SUCCESS) {
        dfd_print_dist_status(err, &status);
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

int cmd_dfd_cancel(int argc, const char *argv[])
{
    int err = 0;
    struct bt_mesh_model *mod = NULL;
    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFD_CLI, &mod)) {
        BT_WARN("mod found none with err=%d", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct bt_mesh_dfd_client_t *cli = mod->user_data;
    struct bt_mesh_dist_status_t status;

    err = bt_mesh_dfd_client_distribute_cancel(cli, &status);
    if (err || status.status != BT_MESH_DFD_SUCCESS) {
        dfd_print_dist_status(err, &status);
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

int cmd_dfd_apply(int argc, const char *argv[])
{
    int err = 0;
    struct bt_mesh_model *mod = NULL;
    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFD_CLI, &mod)) {
        BT_WARN("mod found none with err=%d", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct bt_mesh_dfd_client_t *cli = mod->user_data;
    struct bt_mesh_dist_status_t status;

    err = bt_mesh_dfd_client_distribute_apply(cli, &status);
    if (err || status.status != BT_MESH_DFD_SUCCESS) {
        dfd_print_dist_status(err, &status);
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

int cmd_dfd_fw_get(int argc, const char *argv[])
{
    int err = 0;
    struct bt_mesh_model *mod = NULL;
    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFD_CLI, &mod)) {
        BT_WARN("mod found none with err=%d", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct bt_mesh_dfd_client_t *cli = mod->user_data;
    struct bt_mesh_dist_fw_status_t status;
    uint8_t fwid[MYNEWT_VAL_BT_MESH_DFU_FWID_MAX_LEN];
    uint32_t fwid_len;

    if (argc < 3 || strcmp(argv[1], "fwid") != 0) { // 3:argc size 1:argv index
        BT_INFO_SHELL("wrong params: fwid");
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint32_t hexlen = strlen(argv[2]); // 2:argv index
    fwid_len = ble_mesh_hex2bin(argv[2], fwid, sizeof(fwid)); // 2:argv index
    if (fwid_len < 0 || fwid_len != ((hexlen + 1) / 2)) {  // 2:half
        BT_INFO_SHELL("wrong params: fwid_len");
        return ERRCODE_BLE_MESH_FAIL;
    }

    err = bt_mesh_dfd_client_fw_get(cli, fwid, fwid_len, &status);
    if (err || status.status != BT_MESH_DFD_SUCCESS) {
        BT_INFO_SHELL("failed: err=%d status=%u", err, status.status);
        return ERRCODE_BLE_MESH_FAIL;
    }
    dfd_print_fw_status(err, &status);

    return ERRCODE_BLE_MESH_SUCCESS;
}

int cmd_dfd_fw_get_by_idx(int argc, const char *argv[])
{
    int err = 0;
    struct bt_mesh_model *mod = NULL;
    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFD_CLI, &mod)) {
        BT_WARN("mod found none with err=%d", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct bt_mesh_dfd_client_t *cli = mod->user_data;
    struct bt_mesh_dist_fw_status_t status;

    uint16_t idx = 0;
    if (argc < 3 || strcmp(argv[1], "idx") != 0) { // 3:argc size 1:argv index
        BT_INFO_SHELL("wrong params: idx");
        return ERRCODE_BLE_MESH_FAIL;
    }
    SHELL_PARAM_UINT(idx, argv[2]); // 2:argv index

    err = bt_mesh_dfd_client_fw_get_by_index(cli, idx, &status);
    if (err || status.status != BT_MESH_DFD_SUCCESS) {
        BT_INFO_SHELL("failed: err=%d status=%u", err, status.status);
        return ERRCODE_BLE_MESH_FAIL;
    }
    dfd_print_fw_status(err, &status);

    return ERRCODE_BLE_MESH_SUCCESS;
}

int cmd_dfd_fw_delete(int argc, const char *argv[])
{
    int err = 0;
    struct bt_mesh_model *mod = NULL;
    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFD_CLI, &mod)) {
        BT_WARN("mod found none with err=%d", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct bt_mesh_dfd_client_t *cli = mod->user_data;
    struct bt_mesh_dist_fw_status_t status;
    uint8_t fwid[MYNEWT_VAL_BT_MESH_DFU_FWID_MAX_LEN];
    uint32_t fwid_len;

    if (argc < 3 || strcmp(argv[1], "fwid") != 0) { // 3:argc size 1:argv index
        BT_INFO_SHELL("wrong params: fwid");
        return ERRCODE_BLE_MESH_FAIL;
    }

    uint32_t hexlen = strlen(argv[2]); // 2:argv index
    fwid_len = ble_mesh_hex2bin(argv[2], fwid, sizeof(fwid)); // 2:argv index
    if (fwid_len < 0 || fwid_len != ((hexlen + 1) / 2)) {  // 2:half
        BT_INFO_SHELL("wrong params: fwid_len");
        return ERRCODE_BLE_MESH_FAIL;
    }

    err = bt_mesh_dfd_client_fw_delete(cli, fwid, fwid_len, &status);
    if (err || status.status != BT_MESH_DFD_SUCCESS) {
        BT_INFO_SHELL("failed: err=%d status=%u", err, status.status);
        return ERRCODE_BLE_MESH_FAIL;
    }
    dfd_print_fw_status(err, &status);

    return ERRCODE_BLE_MESH_SUCCESS;
}

int cmd_dfd_fw_delete_all(int argc, const char *argv[])
{
    int err = 0;
    struct bt_mesh_model *mod = NULL;
    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFD_CLI, &mod)) {
        BT_WARN("mod found none with err=%d", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    struct bt_mesh_dfd_client_t *cli = mod->user_data;
    struct bt_mesh_dist_fw_status_t status;

    err = bt_mesh_dfd_client_fw_delete_all(cli, &status);
    if (err || status.status != BT_MESH_DFD_SUCCESS) {
        BT_INFO_SHELL("failed: err=%d status=%u", err, status.status);
        return ERRCODE_BLE_MESH_FAIL;
    }
    dfd_print_fw_status(err, &status);

    return ERRCODE_BLE_MESH_SUCCESS;
}

#endif // #if MYNEWT_VAL(BLE_MESH_DFD_CLIENT)

#if MYNEWT_VAL(BLE_MESH_DFU_CLIENT) || MYNEWT_VAL(BLE_MESH_DFU_SERVER)
static struct {
    struct bt_mesh_dfu_target_t targets[DFU_TARGETS_MAX];
    struct bt_mesh_blob_target_pull_t pull[DFU_PULL_MAX];
    uint32_t target_cnt;
    struct bt_mesh_blob_client_inputs_t inputs;
} dfu_tx;

static void dfu_tx_prepare(void)
{
    sys_slist_init(&dfu_tx.inputs.targets);

    for (uint32_t i = 0; i < dfu_tx.target_cnt; i++) {
        /* Reset target context. */
        uint16_t addr = dfu_tx.targets[i].blob.addr;

        memset_s(
            &dfu_tx.targets[i].blob, sizeof(struct bt_mesh_blob_target_t), 0, sizeof(struct bt_mesh_blob_target_t));
        memset_s(
            &dfu_tx.pull[i], sizeof(struct bt_mesh_blob_target_pull_t), 0, sizeof(struct bt_mesh_blob_target_pull_t));
        dfu_tx.targets[i].blob.addr = addr;
        dfu_tx.targets[i].blob.pull = &dfu_tx.pull[i];

        sys_slist_append(&dfu_tx.inputs.targets, &dfu_tx.targets[i].blob.n);
    }
}

static int cmd_dfu_bank_add(int argc, const char *argv[])
{
    ble_mesh_dfu_firmware_info_t add_bank = {0};
 
    if (argc < 7 || strcmp(argv[1], "size") != 0 || strcmp(argv[3], "fw_id") != 0 || // 7:argc size 1,3:argv index
        strcmp(argv[5], "metadata") != 0) { // 5:argv index
        BT_INFO_SHELL("wrong params: %d", argc);
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }
 
    SHELL_PARAM_UINT(add_bank.size, argv[2]); // 2:argv index
    BT_INFO_SHELL("Adding bank (size: %u)", add_bank.size);
    add_bank.fwid_len = ble_mesh_hex2bin(argv[4], add_bank.fwid, sizeof(add_bank.fwid)); // 4:argv index
    if (add_bank.fwid_len < 0) {
        BT_INFO_SHELL("wrong params: fwid_lenL:%d", add_bank.fwid_len);
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }
 
    BT_INFO_SHELL("fwid_len %u, fwid 0x%02x", add_bank.fwid_len, add_bank.fwid[0]);
    add_bank.metadata_len = ble_mesh_hex2bin(argv[6], add_bank.metadata, sizeof(add_bank.metadata)); // 6:argv index
    if (add_bank.metadata_len < 0) {
        BT_INFO_SHELL("wrong params: metadata_len:%d", add_bank.metadata_len);
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }
    int err = ble_mesh_ota_add_firmware(&add_bank);
    if (err) {
        BT_INFO_SHELL("wrong params: metadata_len");
        return err;
    }
 
    return ERRCODE_BLE_MESH_SUCCESS;
}

static int cmd_dfu_prepare(int argc, const char *argv[])
{
    uint32_t size = 0;
    int err = 0;

    if (argc < 3 || strcmp(argv[1], "size") != 0) { // 3:argc 1:argv index
        BT_INFO_SHELL("cmd err.");
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(size, argv[2]); // 2:argv index
    BT_INFO_SHELL("prepare bank (size: %u)", size);

    err = bt_mesh_dist_flash_proc(size);
    if (err) {
        BT_INFO_SHELL("dist flash error");
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

static int cmd_get_upg_flag(int argc, const char *argv[])
{
    int err = 0;
    uint8_t ota_upg = 0;
 
    if (argc < 1) { // 1:argc 1
        BT_INFO_SHELL("cmd err.");
        return ERRCODE_BLE_MESH_FAIL;
    }
 
    err = ble_mesh_ota_get_upgrade_flag(&ota_upg);
    if (err) {
        BT_INFO_SHELL("ota_get_upg_flag error");
        return ERRCODE_BLE_MESH_FAIL;
    }
    BT_INFO_SHELL("ota upg flag is %u", ota_upg);
 
    return ERRCODE_BLE_MESH_SUCCESS;
}
 
static int cmd_clear_upg_flag(int argc, const char *argv[])
{
    int err = 0;
 
    if (argc < 1) { // 1:argc 1
        BT_INFO_SHELL("cmd err.");
        return ERRCODE_BLE_MESH_FAIL;
    }
 
    err = ble_mesh_ota_clear_upgrade_flag();
    if (err) {
        BT_INFO_SHELL("ota_clear_upg_flag error");
        return ERRCODE_BLE_MESH_FAIL;
    }
    BT_INFO_SHELL("ota upg flag clear succ");
 
    return ERRCODE_BLE_MESH_SUCCESS;
}

static int cmd_dfu_bank_del(uint32_t argc, char *argv[])
{
    const struct bt_mesh_dfu_bank_t *bank;
    uint8_t idx;
    int err = ERRCODE_BLE_MESH_SUCCESS;

    if (argc < 3 || strcmp(argv[1], "idx") != 0) { // 3:argc size 1:argv index
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(idx, argv[2]); // 2:argv index

    bank = bt_mesh_dfu_bank_get_by_idx(idx);
    if (!bank) {
        BT_INFO_SHELL("No bank at %u", idx);
        return ERRCODE_BLE_MESH_FAIL;
    }

    err = bt_mesh_dfu_bank_del(bank);
    if (err) {
        BT_INFO_SHELL("Failed deleting bank %u (err: %d)", idx, err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    BT_INFO_SHELL("Slot %u deleted.", idx);
    return ERRCODE_BLE_MESH_SUCCESS;
}

static int cmd_dfu_bank_del_all(uint32_t argc, char *argv[])
{
    bt_mesh_dfu_bank_del_all();
    BT_INFO_SHELL("All banks deleted.");
    return ERRCODE_BLE_MESH_SUCCESS;
}

static void bank_info_print(const struct bt_mesh_dfu_bank_t *bank, const uint8_t *idx)
{
    char fwid[2 * MYNEWT_VAL_BT_MESH_DFU_FWID_MAX_LEN + 1]; // 2:len
    char metadata[2 * MYNEWT_VAL_BT_MESH_DFU_METADATA_MAX_LEN + 1]; // 2:len
    uint32_t len;

    len = ble_mesh_bin2hex(bank->fwid, bank->fwid_len, fwid, sizeof(fwid));
    fwid[len] = '\0';
    len = ble_mesh_bin2hex(bank->metadata, bank->metadata_len, metadata, sizeof(metadata));
    metadata[len] = '\0';

    if (idx != NULL) {
        BT_INFO_SHELL("Slot %u:", *idx);
    } else {
        BT_INFO_SHELL("Slot:");
    }
    BT_INFO_SHELL("\tSize:     %u bytes", bank->size);
    BT_INFO_SHELL("\tFWID:     %s", fwid);
    BT_INFO_SHELL("\tMetadata: %s", metadata);
}

static int cmd_dfu_bank_get(uint32_t argc, char *argv[])
{
    const struct bt_mesh_dfu_bank_t *bank;
    uint8_t idx;
    int err = ERRCODE_BLE_MESH_SUCCESS;

    if (argc < 3 || strcmp(argv[1], "idx") != 0) { // 3:argc size 1:argv index
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(idx, argv[2]); // 2:argv index

    bank = bt_mesh_dfu_bank_get_by_idx(idx);
    if (!bank) {
        BT_INFO_SHELL("No bank at %u", idx);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bank_info_print(bank, &idx);
    return ERRCODE_BLE_MESH_SUCCESS;
}

#if MYNEWT_VAL(BLE_MESH_DFU_CLIENT)
static int cmd_dfu_target(int argc, const char *argv[])
{
    uint8_t img_idx;
    uint16_t addr;
    int err = ERRCODE_BLE_MESH_SUCCESS;

    if (argc < 5 || strcmp(argv[1], "addr") != 0 || strcmp(argv[3], "img_idx")) { // 5:argc size 1,3:argv index
        return ERRCODE_BLE_MESH_FAIL;
    }
    SHELL_PARAM_UINT(addr, argv[2]); // 2:argv index
    SHELL_PARAM_UINT(img_idx, argv[4]); // 4:argv index

    if (dfu_tx.target_cnt == ARRAY_SIZE(dfu_tx.targets)) {
        BT_INFO_SHELL("No room.");
        return ERRCODE_BLE_MESH_FAIL;
    }

    for (uint32_t i = 0; i < dfu_tx.target_cnt; i++) {
        if (dfu_tx.targets[i].blob.addr == addr) {
            BT_INFO_SHELL("Target 0x%04x already exists", addr);
            return ERRCODE_BLE_MESH_FAIL;
        }
    }

    dfu_tx.targets[dfu_tx.target_cnt].blob.addr = addr;
    dfu_tx.targets[dfu_tx.target_cnt].img_idx = img_idx;
    sys_slist_append(&dfu_tx.inputs.targets, &dfu_tx.targets[dfu_tx.target_cnt].blob.n);
    dfu_tx.target_cnt++;

    BT_INFO_SHELL("Added target 0x%04x", addr);
    return ERRCODE_BLE_MESH_SUCCESS;
}

static int cmd_dfu_targets_reset(uint32_t argc, char *argv[])
{
    dfu_tx_prepare();
    return ERRCODE_BLE_MESH_SUCCESS;
}

static int cmd_dfu_target_state(int argc, const char *argv[])
{
    struct bt_mesh_dfu_target_status_t rsp;
    struct bt_mesh_msg_ctx ctx = {0};
    int err = ERRCODE_BLE_MESH_SUCCESS;
    struct bt_mesh_model *mod_cli = NULL;
    if (argc < 3 || strcmp(argv[1], "dst") != 0) { // 3:argc size 1:argv index
        BT_INFO_SHELL("The parameter is incorrect.");
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFU_CLI, &mod_cli)) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_RANGE_CHECK_IN(argv[1], argv[2], 0x0001, 0x7FFF); // 1,2:argv index
    SHELL_PARAM_UINT(ctx.addr, argv[2]); // 2:argv index

    err = bt_mesh_dfu_client_status_get((struct bt_mesh_dfu_client_t *)mod_cli->user_data, &ctx, &rsp);
    if (err) {
        BT_INFO_SHELL("Failed getting target status (err: %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_dfu_test(true);

    BT_INFO_SHELL("Target 0x%04x:", ctx.addr);
    BT_INFO_SHELL("\tStatus:     %u", rsp.status);
    BT_INFO_SHELL("\tPhase:      %u", rsp.phase);
    if (rsp.phase != BT_MESH_DFU_PHASE_IDLE) {
        BT_INFO_SHELL("\tEffect:       %u", rsp.effect);
        BT_INFO_SHELL("\tImg Idx:      %u", rsp.img_idx);
        BT_INFO_SHELL("\tTTL:          %u", rsp.ttl);
        BT_INFO_SHELL("\tTimeout base: %u", rsp.timeout_base);
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

static enum bt_mesh_dfu_iter_t dfu_img_cb(struct bt_mesh_dfu_client_t *cli, struct bt_mesh_msg_ctx *ctx, uint8_t idx,
    uint8_t total, const struct bt_mesh_dfu_img_t *img, uint8_t *cb_data)
{
    char fwid[2 * MYNEWT_VAL_BT_MESH_DFU_FWID_MAX_LEN + 1]; // 2:len
    uint32_t len;

    len = ble_mesh_bin2hex(img->fwid, img->fwid_len, fwid, sizeof(fwid));
    fwid[len] = '\0';

    BT_INFO_SHELL("Image %u:", idx);
    BT_INFO_SHELL("\tFWID: %s", fwid);
    if (img->uri) {
        BT_INFO_SHELL("\tURI:  %s", img->uri);
    }

    return BT_MESH_DFU_ITER_CONTINUE;
}

static int cmd_dfu_target_imgs(int argc, const char *argv[])
{
    struct bt_mesh_msg_ctx ctx = {0};
    uint8_t img_cnt = 0xff;
    int err = ERRCODE_BLE_MESH_SUCCESS;
    struct bt_mesh_model *mod_cli = NULL;
    if (argc < 5 || strcmp(argv[1], "dst") != 0 || strcmp(argv[3], "img_cnt") != 0) { // 5:argc size 1,3:argv index
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFU_CLI, &mod_cli)) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(ctx.addr, argv[2]); // 2:argv index
    SHELL_PARAM_UINT(img_cnt, argv[4]); // 4:argv index
    BT_INFO_SHELL("Requesting DFU images in 0x%04x", ctx.addr);

    err = bt_mesh_dfu_client_imgs_get((struct bt_mesh_dfu_client_t *)mod_cli->user_data,
        &ctx, dfu_img_cb, NULL, img_cnt);
    if (err) {
        BT_INFO_SHELL("Request failed (err: %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

static int cmd_dfu_target_check(int argc, const char *argv[])
{
    struct bt_mesh_dfu_metadata_status_t rsp;
    const struct bt_mesh_dfu_bank_t *bank;
    struct bt_mesh_msg_ctx ctx = {0};
    uint8_t bank_idx, img_idx;
    int err = ERRCODE_BLE_MESH_SUCCESS;
    struct bt_mesh_model *mod_cli = NULL;
    if (argc < 7 || strcmp(argv[1], "dst") != 0 || strcmp(argv[3], "bank_idx") != 0 || // 7:argc size 1,3:argv index
        strcmp(argv[5], "img_idx") != 0) {  // 5:argv index
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFU_CLI, &mod_cli)) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(ctx.addr, argv[2]); // 2:argv index
    SHELL_PARAM_UINT(bank_idx, argv[4]); // 4:argv index
    SHELL_PARAM_UINT(img_idx, argv[6]); // 6:argv index

    bank = bt_mesh_dfu_bank_get_by_idx(bank_idx);
    if (!bank) {
        BT_INFO_SHELL("No image in bank %u", bank_idx);
        return ERRCODE_BLE_MESH_FAIL;
    }

    err = bt_mesh_dfu_client_metadata_check((struct bt_mesh_dfu_client_t *)mod_cli->user_data,
        &ctx, img_idx, bank, &rsp);
    if (err) {
        BT_INFO_SHELL("Metadata check failed. err: %d", err);
        return ERRCODE_BLE_MESH_FAIL;
    }

    BT_INFO_SHELL("Slot %u check for 0x%04x image %u:", bank_idx, bt_mesh_ota_shell_ctx.dst, img_idx);
    BT_INFO_SHELL("\tStatus: %u", rsp.status);
    BT_INFO_SHELL("\tEffect: 0x%x", rsp.effect);

    return ERRCODE_BLE_MESH_SUCCESS;
}

static int cmd_dfu_send(int argc, const char *argv[])
{
    struct bt_mesh_dfu_client_transfer_blob_params_t blob_params;
    struct bt_mesh_dfu_client_transfer_t transfer = {0};
    uint8_t bank_idx;
    uint16_t group = BT_MESH_ADDR_UNASSIGNED;
    int err = ERRCODE_BLE_MESH_SUCCESS;
    struct bt_mesh_model *mod_cli = NULL;
    if ((argc < 3) || strcmp(argv[1], "bank_idx") != 0) { // 3:argc size 1:argv index
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFU_CLI, &mod_cli)) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(bank_idx, argv[2]); // 2:argv index
    if (argc > 4 && !strcmp(argv[3], "group")) { // 4:argc size 3:argv index
        SHELL_PARAM_UINT(group, argv[4]); // 4:argv index
    }
    if (argc > 6 && !strcmp(argv[5], "mode")) { // 6:argc size 5:argv index
        SHELL_PARAM_UINT(transfer.mode, argv[6]); // 6:argv index
    } else {
        transfer.mode = BT_MESH_BLOB_TRANSFER_MODE_PUSH;
    }
    if (argc > 10 && !strcmp(argv[7], "block_size_logarithm") && // 10:argc size 7:argv index
        !strcmp(argv[9], "chunk_size")) { // 9:argv index
        SHELL_PARAM_UINT(blob_params.block_size_logarithm, argv[8]); // 8:argv index
        SHELL_PARAM_UINT(blob_params.chunk_size, argv[10]); // 10:argv index
        transfer.blob_params = &blob_params;
    }

    if (!dfu_tx.target_cnt) {
        BT_INFO_SHELL("No targets.");
        return ERRCODE_BLE_MESH_FAIL;
    }

    transfer.bank = bt_mesh_dfu_bank_get_by_idx(bank_idx);
    if (!transfer.bank) {
        BT_INFO_SHELL("No image in bank %u", bank_idx);
        return ERRCODE_BLE_MESH_FAIL;
    }

    BT_INFO_SHELL("Starting DFU from bank %u (%u targets)", bank_idx, dfu_tx.target_cnt);

    dfu_tx.inputs.group = group;
    dfu_tx.inputs.app_idx = bt_mesh_ota_shell_ctx.app;
    dfu_tx.inputs.ttl = BT_MESH_TTL_DEFAULT;

    err = bt_mesh_dfu_client_send(
        (struct bt_mesh_dfu_client_t *)mod_cli->user_data, &dfu_tx.inputs, bt_mesh_shell_blob_stream, &transfer);
    if (err) {
        BT_INFO_SHELL("Failed (err: %d)", err);
        return ERRCODE_BLE_MESH_FAIL;
    }
    return ERRCODE_BLE_MESH_SUCCESS;
}

static int cmd_dfu_tx_cancel(int argc, const char *argv[])
{
    struct bt_mesh_msg_ctx ctx = {0};
    int err = ERRCODE_BLE_MESH_SUCCESS;
    struct bt_mesh_model *mod_cli = NULL;

    if ((argc < 3) || strcmp(argv[1], "dst") != 0) {  // 3:argc size 1:argv index
        return ERRCODE_BLE_MESH_FAIL;
    }

    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFU_CLI, &mod_cli)) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    SHELL_PARAM_UINT(ctx.addr, argv[2]); // 2:argv index

    err = bt_mesh_dfu_client_cancel((struct bt_mesh_dfu_client_t *)mod_cli->user_data, &ctx);  // 3:argc size
    if (err) {
        BT_INFO_SHELL("Failed (err: %d)", err);
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

static int cmd_dfu_apply(int argc, const char *argv[])
{
    int err = ERRCODE_BLE_MESH_SUCCESS;
    struct bt_mesh_model *mod_cli = NULL;
    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFU_CLI, &mod_cli)) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    BT_INFO_SHELL("Applying DFU");

    err = bt_mesh_dfu_client_apply((struct bt_mesh_dfu_client_t *)mod_cli->user_data);
    if (err) {
        BT_INFO_SHELL("Failed (err: %d)", err);
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

#ifdef CONFIG_BLE_MESH_PTS_MODE
static int cmd_dfu_verify_fail(int argc, const char *argv[])
{
    target_dfu_verify_fail_set();

    return ERRCODE_BLE_MESH_SUCCESS;
}

static int cmd_dfu_apply_stop(int argc, const char *argv[])
{
    target_dfu_apply_stop_set();

    return ERRCODE_BLE_MESH_SUCCESS;
}

static int cmd_dfu_trans_error(int argc, const char *argv[])
{
    static struct bt_mesh_model *mod_srv = NULL;

    if (!mod_srv && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFU_SRV, &mod_srv)) {
        return -ENODEV;
    }

    struct bt_mesh_dfu_srv_t *srv = mod_srv->user_data;

    srv->update.phase = BT_MESH_DFU_PHASE_TRANSFER_ERR;
    srv->blob.phase = BT_MESH_BLOB_TRANSFER_PHASE_SUSPENDED;

    return ERRCODE_BLE_MESH_SUCCESS;
}

static int cmd_dfd_trans_error(int argc, const char *argv[])
{
    static struct bt_mesh_model *mod_srv = NULL;

    if (!mod_srv && !bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFD_SRV, &mod_srv)) {
        return -ENODEV;
    }

    struct bt_mesh_dfd_srv_t *srv = mod_srv->user_data;

    srv->upload.phase = BT_MESH_DFD_UPLOAD_PHASE_TRANSFER_ERROR;
    srv->upload.blob.phase = BT_MESH_BLOB_TRANSFER_PHASE_SUSPENDED;
    BT_INFO_SHELL("upload phase %u", srv->upload.phase);
    BT_INFO_SHELL("upload blob phase %u", srv->upload.blob.phase);

    return ERRCODE_BLE_MESH_SUCCESS;
}
#endif

static int cmd_dfu_confirm(uint32_t argc, char *argv[])
{
    int err = ERRCODE_BLE_MESH_SUCCESS;
    struct bt_mesh_model *mod_cli = NULL;
    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFU_CLI, &mod_cli)) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    BT_INFO_SHELL("Confirming DFU");

    err = bt_mesh_dfu_client_confirm((struct bt_mesh_dfu_client_t *)mod_cli->user_data);
    if (err) {
        BT_INFO_SHELL("Failed (err: %d)", err);
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

static int cmd_dfu_suspend(uint32_t argc, char *argv[])
{
    int err = ERRCODE_BLE_MESH_SUCCESS;
    struct bt_mesh_model *mod_cli = NULL;
    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFU_CLI, &mod_cli)) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    BT_INFO_SHELL("Suspending DFU");

    err = bt_mesh_dfu_client_suspend((struct bt_mesh_dfu_client_t *)mod_cli->user_data);
    if (err) {
        BT_INFO_SHELL("Failed (err: %d)", err);
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

static int cmd_dfu_resume(uint32_t argc, char *argv[])
{
    int err = ERRCODE_BLE_MESH_SUCCESS;
    struct bt_mesh_model *mod_cli = NULL;
    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFU_CLI, &mod_cli)) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    BT_INFO_SHELL("Resuming DFU");

    err = bt_mesh_dfu_client_resume((struct bt_mesh_dfu_client_t *)mod_cli->user_data);
    if (err) {
        BT_INFO_SHELL("Failed (err: %d)", err);
    }

    return ERRCODE_BLE_MESH_SUCCESS;
}

static int cmd_dfu_tx_progress(uint32_t argc, char *argv[])
{
    struct bt_mesh_model *mod_cli = NULL;
    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFU_CLI, &mod_cli)) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    BT_INFO_SHELL("DFU progress:%u %%", bt_mesh_dfu_client_progress((struct bt_mesh_dfu_client_t *)mod_cli->user_data));
    return ERRCODE_BLE_MESH_SUCCESS;
}
#endif /* MYNEWT_VAL(BLE_MESH_DFU_CLIENT) */

#if MYNEWT_VAL(BLE_MESH_DFU_SERVER)
static int cmd_dfu_applied(uint32_t argc, char *argv[])
{
    struct bt_mesh_model *mod_srv = NULL;
    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFU_SRV, &mod_srv)) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_dfu_srv_applied((struct bt_mesh_dfu_srv_t *)mod_srv->user_data);
    return ERRCODE_BLE_MESH_SUCCESS;
}

static int cmd_dfu_rx_cancel(uint32_t argc, char *argv[])
{
    struct bt_mesh_model *mod_srv = NULL;
    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFU_SRV, &mod_srv)) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    bt_mesh_dfu_srv_cancel((struct bt_mesh_dfu_srv_t *)mod_srv->user_data);
    return ERRCODE_BLE_MESH_SUCCESS;
}

static int cmd_dfu_rx_progress(uint32_t argc, char *argv[])
{
    struct bt_mesh_model *mod_srv = NULL;
    if (!bt_mesh_shell_mdl_first_get(BT_MESH_MODEL_ID_DFU_SRV, &mod_srv)) {
        return ERRCODE_BLE_MESH_FAIL;
    }

    BT_INFO_SHELL("DFU progress: %u %%", bt_mesh_dfu_srv_progress((struct bt_mesh_dfu_srv_t *)mod_srv->user_data));
    return ERRCODE_BLE_MESH_SUCCESS;
}
#endif /* MYNEWT_VAL(BLE_MESH_DFU_SERVER) */

#endif /* MYNEWT_VAL(BLE_MESH_DFU_CLIENT) || MYNEWT_VAL(BLE_MESH_DFU_SERVER) */

static int cmd_ota_stat(int argc, const char *argv[])
{
    bt_mesh_ota_stat_print();
    return ERRCODE_BLE_MESH_SUCCESS;
}

static int cmd_ota_stat_clean(int argc, const char *argv[])
{
    bt_mesh_ota_stat_clean();
    return ERRCODE_BLE_MESH_SUCCESS;
}

static int cmd_ota_result(int argc, const char *argv[])
{
    bt_mesh_ota_result_print();
    return ERRCODE_BLE_MESH_SUCCESS;
}

static bool cmd_ota_upgrade_check_callback(void)
{
    BT_INFO("reboot check");
    return true;
}

static bool cmd_ota_metadata_check_callback(const uint8_t *metadata, uint8_t len)
{
    uint32_t target_fw_ver_new = 0;
    if (len < sizeof(uint32_t)) {
        return true;
    }
    target_fw_ver_new = *(uint32_t*)metadata;

    BT_INFO("metadata_check 0x%08x", target_fw_ver_new);
    if (target_fw_ver_new == 0x11) {
        return true;
    }
    return false;
}

static void cmd_ota_upgrade_status_callback(ble_mesh_ota_upgrade_status_t upgrade_status)
{
    if (upgrade_status == OTA_UPGRADE_START) {
        BT_INFO("Ota Upgrade Start");
    } else if (upgrade_status == TRANSFER_START) {
        BT_INFO("Ota Blob Block Transfer Start");
    } else if (upgrade_status == TRANSFER_END) {
        BT_INFO("Ota Blob Block Transfer End");
    } else if (upgrade_status == OTA_UPGRADE_END) {
        BT_INFO("Ota Upgrade End");
    } else {
        BT_ERR("Upgrade Status %d error", upgrade_status);
    }
}

static int cmd_ota_callback_register(int argc, const char *argv[])
{
    static ble_mesh_ota_register_callbacks_t cbks = {
        .upgrade_check_cb = cmd_ota_upgrade_check_callback,
        .metadata_check_cb = cmd_ota_metadata_check_callback,
        .ota_upgrade_status_cb = cmd_ota_upgrade_status_callback,
    };

    ble_mesh_ota_register_callbacks(&cbks);
    return ERRCODE_BLE_MESH_SUCCESS;
}

static int cmd_ota_firmware_upg_status(int argc, const char *argv[])
{
    ble_mesh_firmware_update_phase_t status = ble_mesh_ota_get_firmware_update_status();
    BT_INFO_SHELL("current status:%u", status);
    return ERRCODE_BLE_MESH_SUCCESS;
}

static int cmd_ota_firmware_id_set(int argc, const char *argv[])
{
    if (argc < 3 || strcmp(argv[1], "imgid") != 0) {  // 3:argc 1:argv index
        BT_INFO_SHELL("cmd err. para num:%d", argc);
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }
    uint8_t fwid[MYNEWT_VAL_BT_MESH_DFU_FWID_MAX_LEN] = {0};
    uint32_t hexlen = strlen(argv[2]); // 2:argv index
    if (hexlen > MYNEWT_VAL_BT_MESH_DFU_FWID_MAX_LEN * 2) {
        BT_INFO_SHELL("cmd err. para num:%d", hexlen);
        return ERRCODE_BLE_MESH_PARAM_ERR;
    }

    uint32_t fwid_len = ble_mesh_hex2bin(argv[2], fwid, sizeof(fwid)); // 2:argv index

    return ble_mesh_ota_set_target_fwid(fwid, fwid_len);
}

const at_para_parse_syntax_t g_ble_mesh_ota_params[] = {
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_ota_t, para1)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_ota_t, para2)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 200,
        .offset = offsetof(ble_mesh_ota_t, para3)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_ota_t, para4)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_ota_t, para5)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_ota_t, para6)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_ota_t, para7)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_ota_t, para8)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .offset = offsetof(ble_mesh_ota_t, para9)},
    {.type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 40,
        .last = true,
        .offset = offsetof(ble_mesh_ota_t, para10)},
};

at_cmd ota_cmd_tbl[] = {
    // "=role,I,addr,2" node takes role as initiator to distributor withof addr 2. "" node gives role free
    {"init", ota_init},
    {"reset", ota_reset},

    // receivers_add,addr,1,img_idx,0
    {"receivers_add", cmd_dfd_receivers_add},
    {"receivers_delete_all", cmd_dfd_receivers_delete_all},
    // receivers_get,first,0,cnt,2
    {"receivers_get", cmd_dfd_receivers_get},
    {"caps", cmd_dfd_capabilities_get},
    {"get", cmd_dfd_get},
    // start,app_idx,0,apply,0
    {"start", cmd_dfd_start},
    {"suspend", cmd_dfd_suspend},
    {"cancel", cmd_dfd_cancel},
    {"apply", cmd_dfd_apply},
    // fw_get,fwid,efbeadde
    {"fw_get", cmd_dfd_fw_get},
    // fw_get_by_idx,idx,1
    {"fw_get_by_idx", cmd_dfd_fw_get_by_idx},
    // fw_delete,fwid,efbeadde
    {"fw_delete", cmd_dfd_fw_delete},
    {"fw_delete_all", cmd_dfd_fw_delete_all},
    {"ota_stat", cmd_ota_stat},
    {"ota_stat_clean", cmd_ota_stat_clean},
    {"ota_result", cmd_ota_result},
    {"dfu_cli_add_bank", cmd_dfu_bank_add},
    {"dfu_cli_prepare", cmd_dfu_prepare},
    {"get_upg_flag", cmd_get_upg_flag},
    {"clear_upg_flag", cmd_clear_upg_flag},
    {"register_callback", cmd_ota_callback_register},
    {"get_upg_status", cmd_ota_firmware_upg_status},
    {"set_fw_id", cmd_ota_firmware_id_set},
#ifdef CONFIG_BLE_MESH_PTS_MODE
    {"upload_start", cmd_dfd_upload_start},    // NA
    {"upload_get", cmd_dfd_upload_get},        // NA
    {"upload_cancel", cmd_dfd_upload_cancel},  // NA
    {"dfd_srv_trans_error", cmd_dfd_trans_error},

#if MYNEWT_VAL(BLE_MESH_BLOB_SERVER)
    // prepare to receive a blob: "id,xx,ttl,xx,timeout_base,xx", id is mandatory.
    {"blobs_rx", cmd_blobs_rx},
    {"blob_caps_set", cmd_blob_caps_set},
#endif // #if MYNEWT_VAL(BLE_MESH_BLOB_SERVER)
#if MYNEWT_VAL(BLE_MESH_BLOB_CLIENT)
    // add a target: "addr,xx"
    {"blobc_target", cmd_blobc_target},
    // start to transfer a blob: "id,xx,size,xx,block,xx,chunk,xx,timeout_base,xx,group,xx,cancel_after_start,1"
    // id and size: mandatory; block: size in logarithm
    {"blobc_tx", cmd_blobc_tx},
    // "group,xx"
    {"blobc_get", cmd_blobc_tx_get},
    {"blobc_cancel", cmd_blobc_tx_cancel},
    {"blobc_suspend", cmd_blobc_tx_suspend},
    {"blobc_resume", cmd_blobc_tx_resume},
    {"blobc_caps", cmd_blobc_caps},
#endif // #if MYNEWT_VAL(BLE_MESH_BLOB_CLIENT)

#if MYNEWT_VAL(BLE_MESH_DFU_CLIENT)
    {"dfu_cli_info_get", cmd_dfu_target_imgs},
    {"dfu_cli_metadata_check", cmd_dfu_target_check},
    {"dfu_cli_update_start", cmd_dfu_send},
    {"dfu_cli_apply", cmd_dfu_apply},
    {"dfu_cli_update_get", cmd_dfu_target_state},
    {"dfu_cli_cancel", cmd_dfu_tx_cancel},
    {"dfu_target", cmd_dfu_target},
#endif /* MYNEWT_VAL(BLE_MESH_DFU_CLIENT) */

#if MYNEWT_VAL(BLE_MESH_DFU_SERVER)
    {"dfu_srv_verify_fail", cmd_dfu_verify_fail},
    {"dfu_srv_apply_stop", cmd_dfu_apply_stop},
    {"dfu_srv_trans_error", cmd_dfu_trans_error},
#endif /* MYNEWT_VAL(BLE_MESH_DFU_SERVER) */
#endif /* CONFIG_BLE_MESH_PTS_MODE */
};

/**
 * OTA cmd cmd_args
 *         cmd_args: param1_name, param1_value,  param2_name, param2_value,  ..
 **/
at_ret_t at_bt_mesh_ota_main(const ble_mesh_ota_t *args)
{
    // 判断是否要删除多余的逗号
    BT_DBG("[debug] at_bt_mesh_ota_main args[1]:%d,  args[2]:%s\r\n", args->para1, args->para2);
    char *argv[15] = {NULL};
    int argc = 0;
    // 进行参数处理
    ASSIGN_PARA(argv, args->para1, argc);
    ASSIGN_PARA(argv, args->para2, argc);
    ASSIGN_PARA(argv, args->para3, argc);
    ASSIGN_PARA(argv, args->para4, argc);
    ASSIGN_PARA(argv, args->para5, argc);
    ASSIGN_PARA(argv, args->para6, argc);
    ASSIGN_PARA(argv, args->para7, argc);
    ASSIGN_PARA(argv, args->para8, argc);
    ASSIGN_PARA(argv, args->para9, argc);
    ASSIGN_PARA(argv, args->para10, argc);
    ASSIGN_PARA(argv, args->para11, argc);
    ASSIGN_PARA(argv, args->para12, argc);
    ASSIGN_PARA(argv, args->para13, argc);
    ASSIGN_PARA(argv, args->para14, argc);
    ASSIGN_PARA(argv, args->para15, argc);

    if (argc < 1) {
        BT_INFO_SHELL("argc err %u", argc);
        return AT_RET_SYNTAX_ERROR;
    }
    int cmd_tbl_num = (sizeof(ota_cmd_tbl) / sizeof(at_cmd));
    for (int i = 0; i < cmd_tbl_num; i++) {
        if (strcmp(argv[0], ota_cmd_tbl[i].str) == 0) {
            BT_INFO_SHELL("cmd:%s", argv[0]);
            return ota_cmd_tbl[i].func(argc, (const char **)argv);
        }
    }

    BT_INFO_SHELL("at_bt_mesh_ota_main - invalid cmd:%s", argv[0]);
    return AT_RET_OK;
}


const at_cmd_entry_t at_bt_mesh_ota_parse_table[] = {
    {
        "OTA",  // OTA控制
        0,
        0,
        g_ble_mesh_ota_params,               // 参数类型
        NULL,                                // 无参数时调用的函数
        (at_set_func_t)at_bt_mesh_ota_main,  // 有参数时调用的函数
        NULL,
        NULL,
    },
};

#define AT_BLE_MESH_FUNC_NUM (sizeof(g_at_ble_mesh_func_tbl) / sizeof(g_at_ble_mesh_func_tbl[0]))
#define AT_BT_MESH_FUNC_NUM (sizeof(at_bt_mesh_ota_parse_table) / sizeof(at_bt_mesh_ota_parse_table[0]))

void mesh_at_ota_register(void)
{
    BT_INFO_SHELL("enter");
    int ret = uapi_at_cmd_table_register(at_bt_mesh_ota_parse_table, AT_BT_MESH_FUNC_NUM, 1024);
    BT_INFO_SHELL("uapi_at_cmd_table_register ret = %d", ret);
}
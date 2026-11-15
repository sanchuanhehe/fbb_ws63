/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Mesh model.
 *
 * History:
 * 2025-4-15, Create file.
 */
#include <string.h>
#include "syscfg/syscfg.h"
#include "mesh/mesh.h"
#include "model/model_opcode.h"
#include "model_common.h"
#include "ble_mesh_model_inner.h"
#include "ble_mesh_model.h"
#include "ble_mesh_model_op.h"
#include "dfd_server.h"
#include "dfd_client.h"
#include "dfu_server.h"
#include "blob.h"
#include "upg_debug.h"
#include "upg.h"
#include "partition.h"
#include "upg_porting.h"
#include "os_time.h"

#include "ble_mesh_nv_op.h"
#include "ble_mesh_ota_inner.h"
#include "mesh_ota_common.h"


static bt_mesh_ota_stat_t ota_stat = {0};
static bt_mesh_ota_result_t ota_result = {0};

static int upload_blob_stream_open(const struct bt_mesh_blob_stream_t *srteam,
    const struct bt_mesh_blob_transfer_t *transfer, enum bt_mesh_blob_stream_mode mode)
{
    errcode_t ret;
    uint32_t max_len;

    ret = uapi_partition_init();  // Flash初始化
    if (ret != ERRCODE_SUCC) {
        BT_ERR("uapi_partition_init error. ret = 0x%08x\r\n", ret);
        return -1;
    }

    max_len = uapi_upg_get_storage_size();  // Flash空间检查
    if (transfer->size > max_len) {
        BT_ERR("image(%u) > max(%u) is too big.\r\n", transfer->size, max_len);
        return -1;
    }

    upg_prepare_info_t prepare_info;
    prepare_info.package_len = transfer->size;
    ret = uapi_upg_prepare(&prepare_info);  // Flash准备
    if (ret != ERRCODE_SUCC) {
        BT_ERR("uapi_upg_prepare error = 0x%x\r\n", ret);
        return -1;
    }

    return 0;
}

static void upload_blob_stream_close(
    const struct bt_mesh_blob_stream_t *srteam, const struct bt_mesh_blob_transfer_t *transfer)
{
        return;
}

static int upload_blob_stream_chunk_wr(const struct bt_mesh_blob_stream_t *srteam,
    const struct bt_mesh_blob_transfer_t *transfer, const struct bt_mesh_blob_block_t *block,
    const struct bt_mesh_blob_chunk_t *chunk)
{
    int ret;
    
    ret = uapi_upg_write_package_sync(block->offset + chunk->offset, chunk->data, chunk->size);
    if (ret != ERRCODE_SUCC) {
        return -1;
    }

    return 0;
}

static int upload_blob_stream_block_start(const struct bt_mesh_blob_stream_t *srteam,
    const struct bt_mesh_blob_transfer_t *transfer, const struct bt_mesh_blob_block_t *block)
{
    return 0;
}

static void upload_blob_stream_block_end(const struct bt_mesh_blob_stream_t *srteam,
    const struct bt_mesh_blob_transfer_t *transfer, const struct bt_mesh_blob_block_t *block)
{
    return;
}

static const struct bt_mesh_blob_stream_t upload_blob_stream = {
    .on_open = upload_blob_stream_open,
    .on_close = upload_blob_stream_close,
    .on_block_start = upload_blob_stream_block_start,
    .on_block_end = upload_blob_stream_block_end,
    .on_write = upload_blob_stream_chunk_wr,
};

// distribute blob read from flash
static uint8_t dist_blob_tx_sum;
static int dist_blob_stream_open(const struct bt_mesh_blob_stream_t *srteam,
    const struct bt_mesh_blob_transfer_t *transfer, enum bt_mesh_blob_stream_mode mode)
{
    return 0;
}

static void dist_blob_stream_close(
    const struct bt_mesh_blob_stream_t *srteam, const struct bt_mesh_blob_transfer_t *transfer)
{
    BT_INFO("sum=%d", dist_blob_tx_sum);
}

static int dist_blob_stream_chunk_rd(const struct bt_mesh_blob_stream_t *srteam,
    const struct bt_mesh_blob_transfer_t *transfer, const struct bt_mesh_blob_block_t *block,
    const struct bt_mesh_blob_chunk_t *chunk)
{
    int ret;

    ret = uapi_upg_read_package(
        block->offset + chunk->offset, (uint8_t *)chunk->data, chunk->size);  // Flash读取，一次读取一个chunk
    if (ret != ERRCODE_SUCC) {
        BT_INFO("uapi_upg_read_package %d Failed.", ret);
        return -1;
    }

    return 0;
}

static int dist_blob_stream_block_start(const struct bt_mesh_blob_stream_t *srteam,
    const struct bt_mesh_blob_transfer_t *transfer, const struct bt_mesh_blob_block_t *block)
{
    return 0;
}

static void dist_blob_stream_block_end(const struct bt_mesh_blob_stream_t *srteam,
    const struct bt_mesh_blob_transfer_t *transfer, const struct bt_mesh_blob_block_t *block)
{
    BT_INFO("block end");
    bt_mesh_ota_result(ble_npl_time_get_ms() / MSEC_PER_SEC, 0, OTA_TRANS_END);
    return;
}

static const struct bt_mesh_blob_stream_t dist_blob_stream = {
    .on_open = dist_blob_stream_open,
    .on_close = dist_blob_stream_close,
    .on_block_start = dist_blob_stream_block_start,
    .on_block_end = dist_blob_stream_block_end,
    .on_read = dist_blob_stream_chunk_rd,
};

// target blob writes to flash
static uint8_t target_blob_rx_sum;
static int target_blob_stream_open(const struct bt_mesh_blob_stream_t *srteam,
    const struct bt_mesh_blob_transfer_t *transfer, enum bt_mesh_blob_stream_mode mode)
{
    errcode_t ret;
    uint32_t max_len;

    ret = uapi_partition_init();
    if (ret != ERRCODE_SUCC) {
        BT_INFO("uapi_partition_init error. ret = 0x%08x\r\n", ret);
        return -1;
    }

    max_len = uapi_upg_get_storage_size();
    if (transfer->size > max_len) {
        BT_INFO("image(%u) > max(%u) is too big.\r\n", transfer->size, max_len);
        return -1;
    }

    upg_prepare_info_t prepare_info;
    prepare_info.package_len = transfer->size;
    ret = uapi_upg_prepare(&prepare_info);
    if (ret != ERRCODE_SUCC) {
        return -1;
    }

    return 0;
}

static void target_blob_stream_close(
    const struct bt_mesh_blob_stream_t *srteam, const struct bt_mesh_blob_transfer_t *transfer)
{
    BT_INFO("sum=%d", target_blob_rx_sum);
}

static int target_blob_stream_chunk_wr(const struct bt_mesh_blob_stream_t *srteam,
    const struct bt_mesh_blob_transfer_t *transfer, const struct bt_mesh_blob_block_t *block,
    const struct bt_mesh_blob_chunk_t *chunk)
{
    int ret;
    
    ret = uapi_upg_write_package_sync(block->offset + chunk->offset, chunk->data, chunk->size);
    if (ret != ERRCODE_SUCC) {
        return -1;
    }

    return 0;
}

static int target_blob_stream_block_start(const struct bt_mesh_blob_stream_t *srteam,
    const struct bt_mesh_blob_transfer_t *transfer, const struct bt_mesh_blob_block_t *block)
{
    BT_INFO("blob stream start.");
    return 0;
}

static void target_blob_stream_block_end(const struct bt_mesh_blob_stream_t *srteam,
    const struct bt_mesh_blob_transfer_t *transfer, const struct bt_mesh_blob_block_t *block)
{
    return;
}

static const struct bt_mesh_blob_stream_t target_blob_stream = {
    .on_open = target_blob_stream_open,
    .on_close = target_blob_stream_close,
    .on_block_start = target_blob_stream_block_start,
    .on_block_end = target_blob_stream_block_end,
    .on_write = target_blob_stream_chunk_wr,
};

const struct bt_mesh_blob_stream_t *bt_mesh_get_target_blob_stream(void)
{
    return &target_blob_stream;
}

const struct bt_mesh_blob_stream_t *bt_mesh_get_upload_blob_stream(void)
{
    return &upload_blob_stream; // dist_fw_recv使用
}

const struct bt_mesh_blob_stream_t *bt_mesh_get_dist_blob_stream(void)
{
    return &dist_blob_stream; // dist_fw_send使用
}

/* BLOB<->FLASH: ends */

int bt_mesh_upagrade_apply(void)
{
    uint8_t upg_flag = 1;
    errcode_t ret = uapi_upg_request_upgrade(false);  // 请求升级
    if (ret != ERRCODE_SUCC) {
        BT_ERR("uapi_upg_request_upgrade error = 0x%x\r\n", ret);
        return -1;
    }

    ret = ble_mesh_data_write(BTH_BLE_MESH_OTA_FLAG_NV_ID, &upg_flag, sizeof(upg_flag));
    if (ret) {
        BT_ERR("Failed write ota upg: %d", ret);    // 标记记录失败不影响升级功能
    }

    if (!ble_mesh_ota_reboot_check()) {
        BT_WARN("user not reboot");
        return 0;
    }

    upg_reboot();  // 复位升级
    return 0;
}

static struct bt_mesh_model* ota_mdl_first_get(uint16_t id);
static bool role_take_i(void);
static bool role_take_d(void);
static bool role_take_t(void);

static ota_node_role_t ota_node_role = OTA_NODE_ROLE_NONE;
#define ROLE_TAKE_TIMEOUT 5
bool ota_node_role_take(ota_node_role_t role)
{
    int err = 0;
    static bool k_sem_inited = false;
    static struct k_sem k_sem_ota_node_role;
    if (!k_sem_inited) {
        k_sem_init(&k_sem_ota_node_role, 0, 1);
        k_sem_give(&k_sem_ota_node_role); // ?
        k_sem_inited = true;
        BT_INFO("sem inited");
    }

    if (ota_node_role == role) {
        BT_INFO("role not changed, ignore with ok");
        return true;
    }

    if (role == OTA_NODE_ROLE_NONE) {
        BT_INFO("take NONE: release sem now");
        //// give the role
        // do not set OTA_NODE_ROLE_NONE, just let it be!
        return true;
    }

    if (!(role == OTA_NODE_ROLE_TARGET || role == OTA_NODE_ROLE_DISTRIBUTOR || role == OTA_NODE_ROLE_INITIATOR)) {
            BT_ERR("%d->%d: role invalid", ota_node_role, role);
            return false;
    }
    //// take the role
    if (err) {
        BT_ERR("%d->%d: failed or busy, err=%d", ota_node_role, role, err);
        return false;
    }

    if ((role == OTA_NODE_ROLE_INITIATOR && role_take_i()) ||
        (role == OTA_NODE_ROLE_DISTRIBUTOR && role_take_d()) ||
        (role == OTA_NODE_ROLE_TARGET && role_take_t())) {
            BT_ERR("%d->%d taken ok", ota_node_role, role);
            ota_node_role = role;
            return true;
        }

    BT_ERR("take role=%u failed", role);
    //// give the role: failed and give
    return false;
}

static struct bt_mesh_model* ota_mdl_first_get(uint16_t id)
{
    struct bt_mesh_model *mod = NULL;
    const struct bt_mesh_comp *comp = (struct bt_mesh_comp *)ble_mesh_get_comp();

    if (comp == NULL) {
        return NULL;
    }

    for (int i = 0; i < comp->elem_count; i++) {
        mod = bt_mesh_model_find(&comp->elem[i], id);
        if (mod) {
            return mod;
        }
    }

    return NULL;
}

static bool role_take_i(void)
{
    struct bt_mesh_model *mod_blob_cli = NULL;
    struct bt_mesh_model *mod_dfd_cli = NULL;
    struct bt_mesh_model *mod_dfu_cli = NULL;
    struct bt_mesh_dfd_client_t *dfd_cli = NULL;

    mod_dfd_cli = ota_mdl_first_get(BT_MESH_MODEL_ID_DFD_CLI);
    mod_blob_cli = ota_mdl_first_get(BT_MESH_MODEL_ID_BLOB_CLI);
    mod_dfu_cli = ota_mdl_first_get(BT_MESH_MODEL_ID_DFU_CLI);
    if (mod_dfd_cli == NULL || mod_blob_cli == NULL || mod_dfu_cli == NULL) {
        BT_ERR("mod not all found: dfd_cli=%p blobcli=%p dfucli=%p", mod_dfd_cli, mod_blob_cli, mod_dfu_cli);
        return false;
    }
    dfd_cli = mod_dfd_cli->user_data;
    if (dfd_cli == NULL) {
        BT_ERR("dfd_cli user_data is none, not initialized well");
        return false;
    }
    mod_blob_cli->user_data = &dfd_cli->blob;
    dfd_cli->blob.mod = mod_blob_cli;

    return true;
}

static bool role_take_d(void)
{
    struct bt_mesh_model *mod_blob_cli = NULL;
    struct bt_mesh_model *mod_blob_srv = NULL;
    struct bt_mesh_model *mod_dfd_srv = NULL;
    struct bt_mesh_model *mod_dfu_cli = NULL;
    struct bt_mesh_dfd_srv_t *dfd_srv = NULL;

    mod_dfd_srv = ota_mdl_first_get(BT_MESH_MODEL_ID_DFD_SRV);
    mod_blob_cli = ota_mdl_first_get(BT_MESH_MODEL_ID_BLOB_CLI);
    mod_blob_srv = ota_mdl_first_get(BT_MESH_MODEL_ID_BLOB_SRV);
    mod_dfu_cli = ota_mdl_first_get(BT_MESH_MODEL_ID_DFU_CLI);
    if (mod_dfd_srv == NULL || mod_blob_cli == NULL || mod_dfu_cli == NULL || mod_blob_srv == NULL) {
        BT_ERR("mod not all found: dfd_srv=%p blobcli=%p blobsrv=%p dfucli=%p",
            mod_dfd_srv, mod_blob_cli, mod_blob_srv, mod_dfu_cli);
        return false;
    }
    dfd_srv = mod_dfd_srv->user_data;
    if (dfd_srv == NULL) {
        BT_ERR("dfd_srv user_data is none, not initialized well");
        return false;
    }
    mod_dfu_cli->user_data = &dfd_srv->dfu;
    mod_blob_cli->user_data = &dfd_srv->dfu.blob;
    mod_blob_srv->user_data = &dfd_srv->upload.blob;
    dfd_srv->dfu.mod = mod_dfu_cli;
    dfd_srv->dfu.blob.mod = mod_blob_cli;
    dfd_srv->upload.blob.mod = mod_blob_srv;

    return true;
}

static bool role_take_t(void)
{
    struct bt_mesh_model *mod_blob_srv = NULL;
    struct bt_mesh_model *mod_dfu_srv = NULL;
    struct bt_mesh_dfu_srv_t *dfu_srv = NULL;

    mod_dfu_srv = ota_mdl_first_get(BT_MESH_MODEL_ID_DFU_SRV);
    mod_blob_srv = ota_mdl_first_get(BT_MESH_MODEL_ID_BLOB_SRV);
    if (mod_dfu_srv == NULL || mod_blob_srv == NULL) {
        BT_ERR("mod not all found: dfsrv=%p blobsrv=%p", mod_dfu_srv, mod_blob_srv);
        return false;
    }
    dfu_srv = mod_dfu_srv->user_data;
    if (dfu_srv == NULL) {
        BT_ERR("dfu_srv user_data is none, not initialized well");
        return false;
    }
    mod_blob_srv->user_data = &dfu_srv->blob;
    dfu_srv->blob.mod = mod_blob_srv;

    return true;
}

int bt_mesh_dist_flash_proc(uint32_t size)
{
    errcode_t ret;
    upg_prepare_info_t prepare_info = {0};

    prepare_info.package_len = size;
    ret = uapi_upg_prepare(&prepare_info);  // Flash准备
    if (ret != ERRCODE_SUCC) {
        BT_ERR("uapi_upg_prepare error = 0x%x\r\n", ret);
        return -1;
    }
    return 0;
}

void bt_mesh_ota_stat(uint32_t opcode)
{
    switch (opcode) {
        case MODEL_OP(DFU_UPDATE_INFO_GET):
            ota_stat.update_info_get++;
            break;
        case MODEL_OP(DFU_UPDATE_START):
            ota_stat.update_start++;
            break;
        case MODEL_OP(DFU_UPDATE_GET):
            ota_stat.update_get++;
            break;
        case MODEL_OP(DFU_UPDATE_APPLY):
            ota_stat.update_apply++;
            break;
        case MODEL_OP(DFU_UPDATE_STATUS):
            ota_stat.update_status++;
            break;
        case MODEL_OP(DFU_UPDATE_INFO_STATUS):
            ota_stat.update_info_status++;
            break;
        case MODEL_OP(BLOB_INFO_GET):
            ota_stat.blob_info_get++;
            break;
        case MODEL_OP(BLOB_TRANSFER_START):
            ota_stat.blob_trans_start++;
            break;
        case MODEL_OP(BLOB_TRANSFER_GET):
            ota_stat.blob_trans_get++;
            break;
        case MODEL_OP(BLOB_BLOCK_START):
            ota_stat.blob_block_start++;
            break;
        case MODEL_OP(BLOB_CHUNK):
            ota_stat.blob_chunk_trans++;
            break;
        case MODEL_OP(BLOB_BLOCK_GET):
            ota_stat.blob_block_get++;
            break;
        case MODEL_OP(BLOB_INFO_STATUS):
            ota_stat.blob_info_status++;
            break;
        case MODEL_OP(BLOB_TRANSFER_STATUS):
            ota_stat.blob_trans_status++;
            break;
        case MODEL_OP(BLOB_BLOCK_STATUS):
            ota_stat.blob_block_status++;
            break;
        default:
            break;
    }
}

void bt_mesh_ota_stat_print(void)
{
    BT_INFO("-------------------------------------------------");
    BT_INFO("update_start: %u", ota_stat.update_start);
    BT_INFO("update_get: %u", ota_stat.update_get);
    BT_INFO("update_apply: %u", ota_stat.update_apply);
    BT_INFO("update_status: %u", ota_stat.update_status);
    BT_INFO("update_info_get: %u", ota_stat.update_info_get);
    BT_INFO("update_info_status: %u", ota_stat.update_info_status);
    BT_INFO("blob_info_get: %u", ota_stat.blob_info_get);
    BT_INFO("blob_info_status: %u", ota_stat.blob_info_status);
    BT_INFO("blob_trans_start: %u", ota_stat.blob_trans_start);
    BT_INFO("blob_trans_get: %u", ota_stat.blob_trans_get);
    BT_INFO("blob_trans_status: %u", ota_stat.blob_trans_status);
    BT_INFO("blob_block_start: %u", ota_stat.blob_block_start);
    BT_INFO("blob_block_get: %u", ota_stat.blob_block_get);
    BT_INFO("blob_block_status: %u", ota_stat.blob_block_status);
    BT_INFO("blob_chunk_trans: %u", ota_stat.blob_chunk_trans);
    BT_INFO("-------------------------------------------------");
}

void bt_mesh_ota_stat_clean(void)
{
    memset_s(&ota_stat, sizeof(ota_stat), 0, sizeof(ota_stat));
}

void bt_mesh_ota_result(time_t time, uint8_t count, bt_mesh_ota_phase_t ota_phase)
{
    if (ota_phase == OTA_START) {
        ota_result.start_time = time;
        ota_result.succ_count = count;
    } else if (ota_phase == OTA_TRANS_END) {
        ota_result.trans_end_time = time;
    } else if (ota_phase == OTA_END) {
        ota_result.end_time = time;
        ota_result.succ_count = count;
    }
}

void bt_mesh_ota_result_print(void)
{
    struct tm start = {0};
    localtime_r(&ota_result.start_time, &start);

    BT_INFO("-------------------------------------------------");
    BT_INFO("start [%d-%02d-%02d %02d:%02d:%02d] ",
        1900 + start.tm_year, // year start 1900
        1 + start.tm_mon,
        start.tm_mday,
        start.tm_hour,
        start.tm_min,
        start.tm_sec);

    struct tm trans = {0};
    localtime_r(&ota_result.trans_end_time, &trans);
    BT_INFO("trans end [%d-%02d-%02d %02d:%02d:%02d] ",
        1900 + trans.tm_year, // year start 1900
        1 + trans.tm_mon,
        trans.tm_mday,
        trans.tm_hour,
        trans.tm_min,
        trans.tm_sec);

    struct tm end = {0};
    localtime_r(&ota_result.end_time, &end);
    BT_INFO("end   [%d-%02d-%02d %02d:%02d:%02d] ",
        1900 + end.tm_year,  // year start 1900
        1 + end.tm_mon,
        end.tm_mday,
        end.tm_hour,
        end.tm_min,
        end.tm_sec);

    BT_INFO("success count %u", ota_result.succ_count);
    BT_INFO("total time %u min %u sec",
        (ota_result.end_time - ota_result.start_time) / 60,   // 60:min
        (ota_result.end_time - ota_result.start_time) % 60);  // 60:min
    BT_INFO("trans time %u min %u sec",
        (ota_result.trans_end_time - ota_result.start_time) / 60,   // 60:min
        (ota_result.trans_end_time - ota_result.start_time) % 60);  // 60:min
    BT_INFO("-------------------------------------------------");
}

void bt_mesh_mbt_test(bool enable)
{
#ifdef CONFIG_BLE_MESH_PTS_MODE
    atomic_set_bit_to(bt_mesh.flags, BT_MESH_MBT_TEST, enable);
#endif
}

void bt_mesh_dfu_test(bool enable)
{
#ifdef CONFIG_BLE_MESH_PTS_MODE
    atomic_set_bit_to(bt_mesh.flags, BT_MESH_DFU_TEST, enable);
#endif
}

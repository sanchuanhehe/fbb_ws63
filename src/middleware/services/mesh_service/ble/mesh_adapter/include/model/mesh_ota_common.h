/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: ble mesh operator interface.
 *
 * History:
 * 2024-6-21, Create file.
 */
#ifndef _BLE_MESH_OTA_COMMON_H_
#define _BLE_MESH_OTA_COMMON_H_
#include "mesh/access.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    OTA_NODE_ROLE_NONE,
    OTA_NODE_ROLE_TARGET, // default
    OTA_NODE_ROLE_DISTRIBUTOR, // by handle_xx()
    OTA_NODE_ROLE_INITIATOR, // by AT command "init" or upper api
} ota_node_role_t;

bool ota_node_role_take(ota_node_role_t role);

typedef struct {
    uint32_t update_start;
    uint32_t update_get;
    uint32_t update_apply;
    uint32_t update_status;
    uint32_t update_info_get;
    uint32_t update_info_status;
    uint32_t blob_info_get;
    uint32_t blob_info_status;
    uint32_t blob_trans_start;
    uint32_t blob_trans_get;
    uint32_t blob_trans_status;
    uint32_t blob_block_start;
    uint32_t blob_block_get;
    uint32_t blob_block_status;
    uint32_t blob_chunk_trans;
} bt_mesh_ota_stat_t;

typedef struct {
    time_t start_time;
    time_t trans_end_time;
    time_t end_time;
    uint8_t succ_count;
} bt_mesh_ota_result_t;

typedef enum {
    OTA_START,
    OTA_TRANS_END,
    OTA_END,
} bt_mesh_ota_phase_t;

const struct bt_mesh_blob_stream_t *bt_mesh_get_target_blob_stream(void);
const struct bt_mesh_blob_stream_t *bt_mesh_get_upload_blob_stream(void);
const struct bt_mesh_blob_stream_t *bt_mesh_get_dist_blob_stream(void);
int bt_mesh_upagrade_apply(void);

void bt_mesh_ota_stat(uint32_t opcode);
void bt_mesh_ota_stat_clean(void);
void bt_mesh_ota_stat_print(void);
void bt_mesh_ota_result(time_t time, uint8_t count, bt_mesh_ota_phase_t ota_phase);
void bt_mesh_ota_result_print(void);

void bt_mesh_mbt_test(bool enable);
void bt_mesh_dfu_test(bool enable);
int bt_mesh_dist_flash_proc(uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* __BLE_MESH_MODEL_OTA_H__ */
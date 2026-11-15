/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: DFU Model Implementation.
 *
 * History:
 * 2025-05-01, Create file.
 */

#ifndef __DFU_BANK_H__
#define __DFU_BANK_H__
#include "dfu.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum bt_mesh_dfu_iter_t (*bt_mesh_dfu_bank_cb_t)(const struct bt_mesh_dfu_bank_t *bank, void *user_data);
int bt_mesh_dfu_bank_count(void);
int bt_mesh_dfu_bank_info_set(
    struct bt_mesh_dfu_bank_t *dfu_bank, uint32_t size, const uint8_t *metadata, uint32_t metadata_len);
int bt_mesh_dfu_bank_fwid_set(struct bt_mesh_dfu_bank_t *dfu_bank, const uint8_t *fwid, uint32_t fwid_len);
int bt_mesh_dfu_bank_commit(struct bt_mesh_dfu_bank_t *dfu_bank);
int bt_mesh_dfu_bank_del(const struct bt_mesh_dfu_bank_t *bank);
int bt_mesh_dfu_bank_get(const uint8_t *fwid, uint32_t fwid_len, struct bt_mesh_dfu_bank_t **bank);
int bt_mesh_dfu_bank_img_idx_get(const struct bt_mesh_dfu_bank_t *bank);
void bt_mesh_dfu_bank_del_all(void);
void bt_mesh_dfu_bank_release(const struct bt_mesh_dfu_bank_t *dfu_bank);
struct bt_mesh_dfu_bank_t *bt_mesh_dfu_bank_reserve(void);
const struct bt_mesh_dfu_bank_t *bt_mesh_dfu_bank_get_by_idx(uint16_t img_idx);

uint32_t bt_mesh_dfu_bank_foreach(bt_mesh_dfu_bank_cb_t cb, void *user_data);

#ifdef __cplusplus
}
#endif

#endif /* __DFU_BANK_H__ */
/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: DFU Model Implementation.
 *
 * History:
 * 2025-05-01, Create file.
 */

#ifndef __DFU_METADATA_H__
#define __DFU_METADATA_H__

#include <stdint.h>

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** FW version. */
struct bt_mesh_dfu_metadata_fw_ver {
    uint8_t major;
    uint8_t minor;
    uint16_t revision;
    uint32_t build_num;
};

/** FW core type. */
enum bt_mesh_dfu_metadata_fw_core_type {
    BT_MESH_DFU_FW_CORE_TYPE_APP = BIT(0),
    BT_MESH_DFU_FW_CORE_TYPE_NETWORK = BIT(1),
    BT_MESH_DFU_FW_CORE_TYPE_APP_SPECIFIC_BLOB = BIT(2),
};

/** FW metadata. */
struct bt_mesh_dfu_metadata {
    struct bt_mesh_dfu_metadata_fw_ver fw_ver;
    uint32_t fw_size;
    enum bt_mesh_dfu_metadata_fw_core_type fw_core_type;
    uint32_t comp_hash;
    uint16_t elems;
    uint8_t *user_data;
    uint32_t user_data_len;
};

int bt_mesh_dfu_metadata_decode(struct os_mbuf *buf, struct bt_mesh_dfu_metadata *metadata);
int bt_mesh_dfu_metadata_encode(const struct bt_mesh_dfu_metadata *metadata, struct os_mbuf *buf);

#ifdef __cplusplus
}
#endif

#endif /* __DFU_METADATA_H__ */
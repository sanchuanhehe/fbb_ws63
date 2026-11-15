 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: DFU Model Implementation.
 *
 * History:
 * 2025-05-01, Create file.
 */

#include <string.h>
#include "os/os_mbuf.h"
#include "mesh/glue.h"
#include "mesh/mesh.h"
#include "model/model_opcode.h"
#include "crypto.h"
#include "access.h"
#include "model/dfu_metadata.h"

#define METADATE_DECODE_MSG_LEN    12
#define METADATE_CODE_TYPE_MSG_LEN 6
#define COMP_HASH_GET_MAC_LEN      16
#define CMAC_RAW_KEY               1
#define COM_DATA_GET_PAGE_0        0

int bt_mesh_dfu_metadata_decode(struct os_mbuf *buf, struct bt_mesh_dfu_metadata *metadata)
{
    if (buf->om_len < METADATE_DECODE_MSG_LEN) {
        return -EMSGSIZE;
    }

    metadata->fw_ver.major = net_buf_simple_pull_u8(buf);
    metadata->fw_ver.minor = net_buf_simple_pull_u8(buf);
    metadata->fw_ver.revision = net_buf_simple_pull_le16(buf);
    metadata->fw_ver.build_num = net_buf_simple_pull_le32(buf);
    metadata->fw_size = net_buf_simple_pull_le24(buf);
    metadata->fw_core_type = net_buf_simple_pull_u8(buf);

    if (metadata->fw_core_type & BT_MESH_DFU_FW_CORE_TYPE_APP) {
        if (buf->om_len < METADATE_CODE_TYPE_MSG_LEN) {
            return -EMSGSIZE;
        }

        metadata->comp_hash = net_buf_simple_pull_le32(buf);
        metadata->elems = net_buf_simple_pull_le16(buf);
    }

    metadata->user_data = buf->om_len > 0 ? buf->om_data : NULL;
    metadata->user_data_len = buf->om_len;

    return 0;
}


int bt_mesh_dfu_metadata_encode(const struct bt_mesh_dfu_metadata *metadata, struct os_mbuf *buf)
{
    size_t md_len_min = METADATE_DECODE_MSG_LEN + metadata->user_data_len;

    if (metadata->fw_core_type & BT_MESH_DFU_FW_CORE_TYPE_APP) {
        md_len_min += METADATE_CODE_TYPE_MSG_LEN;
    }

    if (net_buf_simple_tailroom(buf) < md_len_min) {
        return -EMSGSIZE;
    }

    net_buf_simple_add_u8(buf, metadata->fw_ver.major);
    net_buf_simple_add_u8(buf, metadata->fw_ver.minor);
    net_buf_simple_add_le16(buf, metadata->fw_ver.revision);
    net_buf_simple_add_le32(buf, metadata->fw_ver.build_num);
    net_buf_simple_add_le24(buf, metadata->fw_size);
    net_buf_simple_add_u8(buf, metadata->fw_core_type);
    net_buf_simple_add_le32(buf, metadata->comp_hash);
    net_buf_simple_add_le16(buf, metadata->elems);

    if (metadata->user_data_len > 0) {
        net_buf_simple_add_mem(buf, metadata->user_data, metadata->user_data_len);
    }

    return 0;
}
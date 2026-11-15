/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: defintion for Vendor opcode
 *
 * History:
 * 2025-5-22, Create file.
 */

#ifndef __VND_OPCODE_H__
#define __VND_OPCODE_H__

#include "mesh/mesh.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HW_CID
#define HW_CID 0x027D
#endif

#define HW_MESH_MODEL_VND_ID_SRV 0x0001
#define HW_MESH_MODEL_VND_ID_CLI 0x0002

#define BT_MESH_OP_VND_ATTR_GET BT_MESH_MODEL_OP_3(0xD0, HW_CID)
#define BT_MESH_OP_VND_ATTR_SET BT_MESH_MODEL_OP_3(0xD1, HW_CID)
#define BT_MESH_OP_VND_ATTR_SET_UNACK BT_MESH_MODEL_OP_3(0xD2, HW_CID)
#define BT_MESH_OP_VND_ATTR_SET_STATUS BT_MESH_MODEL_OP_3(0xD3, HW_CID)
#define BT_MESH_OP_VND_ATTR_NOTIFY BT_MESH_MODEL_OP_3(0xD4, HW_CID)
#define BT_MESH_OP_VND_ATTR_NOTIFY_STATUS BT_MESH_MODEL_OP_3(0xD5, HW_CID)
#define BT_MESH_OP_VND_ATTR_NOTIFY_UNACK BT_MESH_MODEL_OP_3(0xD6, HW_CID)
#define BT_MESH_OP_VND_ATTR_GET_STATUS BT_MESH_MODEL_OP_3(0xD7, HW_CID)

#ifdef __cplusplus
}
#endif

#endif
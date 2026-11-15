/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Mesh net buff adapter.
 *
 * History:
 * 2025-4-15, Create file.
 */
#include <stdint.h>

#include "mesh/glue.h"
#include "os/endian.h"

#include "ble_mesh_common.h"

#define MAX_VALUE_UINT8 0XFF
#define MAX_VALUE_UINT16 0XFFFF
#define MAX_VALUE_UINT32 0XFFFFFFFF

uint16_t ble_mesh_net_buf_pull_le16(ble_mesh_net_buff_t *net_buff)
{
    if (net_buff->off + sizeof(uint16_t) > net_buff->len) {
        return MAX_VALUE_UINT16;
    }
    uint16_t value = get_le16(net_buff->payload + net_buff->off);
    net_buff->off += sizeof(uint16_t);
    return value;
}

uint16_t ble_mesh_net_buf_pull_be16(ble_mesh_net_buff_t *net_buff)
{
    if ((net_buff->off + sizeof(uint16_t)) > net_buff->len) {
        return MAX_VALUE_UINT16;
    }
    uint16_t value = get_be16(net_buff->payload + net_buff->off);
    net_buff->off += sizeof(uint16_t);
    return value;
}

uint32_t ble_mesh_net_buf_pull_le24(ble_mesh_net_buff_t *net_buff)
{
    if ((net_buff->off + 3) > net_buff->len) { // 3 24 bit len
        return MAX_VALUE_UINT32;
    }
    uint32_t value = get_le24(net_buff->payload + net_buff->off);
    net_buff->off += 3;  // 3 24 bit len
    return value;
}

uint32_t ble_mesh_net_buf_pull_be32(ble_mesh_net_buff_t *net_buff)
{
    if ((net_buff->off + sizeof(uint32_t)) > net_buff->len) {
        return MAX_VALUE_UINT32;
    }
    uint32_t value = get_be32(net_buff->payload + net_buff->off);
    net_buff->off += sizeof(uint32_t);
    return value;
}

uint32_t ble_mesh_net_buf_pull_le32(ble_mesh_net_buff_t *net_buff)
{
    if ((net_buff->off + sizeof(uint32_t)) > net_buff->len) {
        return MAX_VALUE_UINT32;
    }
    uint32_t value = get_le32(net_buff->payload + net_buff->off);
    net_buff->off += sizeof(uint32_t);
    return value;
}

uint8_t ble_mesh_net_buf_pull_u8(ble_mesh_net_buff_t *net_buff)
{
    if ((net_buff->off + sizeof(uint8_t)) > net_buff->len) {
        return MAX_VALUE_UINT8;
    }
    uint8_t value = net_buff->payload[net_buff->off];
    net_buff->off += sizeof(uint8_t);
    return value;
}

uint32_t ble_mesh_net_buf_left_len(ble_mesh_net_buff_t *net_buff)
{
    return net_buff->len > net_buff->off ? net_buff->len - net_buff->off : 0;
}
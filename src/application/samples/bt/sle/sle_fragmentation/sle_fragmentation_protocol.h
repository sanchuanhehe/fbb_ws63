/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SLE fragmentation sample protocol.
 */

#ifndef SLE_FRAGMENTATION_PROTOCOL_H
#define SLE_FRAGMENTATION_PROTOCOL_H

#include <stdint.h>

#define SLE_FRAGMENTATION_MAGIC 0x5346
#define SLE_FRAGMENTATION_TRANSFER_ID 1
#define SLE_FRAGMENTATION_DATA_SIZE 1024
#define SLE_FRAGMENTATION_PAYLOAD_SIZE 180
#define SLE_FRAGMENTATION_TOTAL_FRAGMENTS \
    ((SLE_FRAGMENTATION_DATA_SIZE + SLE_FRAGMENTATION_PAYLOAD_SIZE - 1) / SLE_FRAGMENTATION_PAYLOAD_SIZE)

typedef struct {
    uint16_t magic;
    uint16_t transfer_id;
    uint16_t index;
    uint16_t total;
    uint16_t payload_len;
    uint16_t reserved;
    uint32_t checksum;
    uint8_t payload[SLE_FRAGMENTATION_PAYLOAD_SIZE];
} sle_fragmentation_packet_t;

#endif

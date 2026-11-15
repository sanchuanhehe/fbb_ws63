/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SLE Hello Server ADV Config. \n
 *
 * History: \n
 * 2024-05-18, Create file. \n
 */

#ifndef SLE_FRAGMENTATION_SERVER_ADV_H
#define SLE_FRAGMENTATION_SERVER_ADV_H

#include <stdint.h>

typedef struct sle_adv_common_value {
    uint8_t length;
    uint8_t type;
    uint8_t value;
} le_adv_common_t;

typedef enum sle_adv_channel {
    SLE_ADV_CHANNEL_MAP_77 = 0x01,
    SLE_ADV_CHANNEL_MAP_78 = 0x02,
    SLE_ADV_CHANNEL_MAP_79 = 0x04,
    SLE_ADV_CHANNEL_MAP_DEFAULT = 0x07
} sle_adv_channel_map_t;

typedef enum sle_adv_data {
    SLE_ADV_DATA_TYPE_DISCOVERY_LEVEL = 0x01,
    SLE_ADV_DATA_TYPE_ACCESS_MODE = 0x02,
    SLE_ADV_DATA_TYPE_SERVICE_DATA_16BIT_UUID = 0x03,
    SLE_ADV_DATA_TYPE_SERVICE_DATA_128BIT_UUID = 0x04,
    SLE_ADV_DATA_TYPE_COMPLETE_LIST_OF_16BIT_SERVICE_UUIDS = 0x05,
    SLE_ADV_DATA_TYPE_COMPLETE_LIST_OF_128BIT_SERVICE_UUIDS = 0x06,
    SLE_ADV_DATA_TYPE_INCOMPLETE_LIST_OF_16BIT_SERVICE_UUIDS = 0x07,
    SLE_ADV_DATA_TYPE_INCOMPLETE_LIST_OF_128BIT_SERVICE_UUIDS = 0x08,
    SLE_ADV_DATA_TYPE_SERVICE_STRUCTURE_HASH_VALUE = 0x09,
    SLE_ADV_DATA_TYPE_SHORTENED_LOCAL_NAME = 0x0A,
    SLE_ADV_DATA_TYPE_COMPLETE_LOCAL_NAME = 0x0B,
    SLE_ADV_DATA_TYPE_TX_POWER_LEVEL = 0x0C,
    SLE_ADV_DATA_TYPE_SLB_COMMUNICATION_DOMAIN = 0x0D,
    SLE_ADV_DATA_TYPE_SLB_MEDIA_ACCESS_LAYER_ID = 0x0E,
    SLE_ADV_DATA_TYPE_EXTENDED = 0xFE,
    SLE_ADV_DATA_TYPE_MANUFACTURER_SPECIFIC_DATA = 0xFF
} sle_adv_data_type;

errcode_t sle_fragmentation_announce_register_cbks(void);
errcode_t sle_fragmentation_server_adv_init(void);

#endif

/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SLE device configuration protocol.
 */

#ifndef SLE_DEVICE_CONFIG_PROTOCOL_H
#define SLE_DEVICE_CONFIG_PROTOCOL_H

#include <stdint.h>

#define SLE_DEVICE_CONFIG_MAGIC 0x5343
#define SLE_DEVICE_CONFIG_VERSION 1
#define SLE_DEVICE_CONFIG_INTERVAL_MIN_MS 100
#define SLE_DEVICE_CONFIG_INTERVAL_MAX_MS 60000
#define SLE_DEVICE_CONFIG_THRESHOLD_MIN (-200)
#define SLE_DEVICE_CONFIG_THRESHOLD_MAX 1000
#define SLE_DEVICE_CONFIG_MODE_MAX 1

typedef struct {
    uint16_t magic;
    uint16_t report_interval_ms;
    int16_t alarm_threshold_decicelsius;
    uint8_t mode;
    uint8_t version;
} sle_device_config_t;

#endif

/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026.
 * Description: Binary protocol shared by the BLE gateway roles.
 */

#ifndef BLE_GATEWAY_PROTOCOL_H
#define BLE_GATEWAY_PROTOCOL_H

#include <stdbool.h>
#include <stdint.h>

#define BLE_GATEWAY_PROTOCOL_VERSION 1U
#define BLE_GATEWAY_NODE_ID_DEFAULT 1U
#define BLE_GATEWAY_REPORT_SIZE 14U
#define BLE_GATEWAY_COMMAND_SIZE 6U
#define BLE_GATEWAY_COMMAND_SET_INTERVAL 1U
#define BLE_GATEWAY_MIN_INTERVAL_S 5U
#define BLE_GATEWAY_MAX_INTERVAL_S 3600U
#define BLE_GATEWAY_DEFAULT_INTERVAL_S 10U
#define BLE_GATEWAY_MIN_TEMPERATURE_TENTHS_C (-400)
#define BLE_GATEWAY_MAX_TEMPERATURE_TENTHS_C 850
#define BLE_GATEWAY_MAX_HUMIDITY_TENTHS_PERCENT 1000U
#define BLE_GATEWAY_MIN_PRESSURE_PA 30000U
#define BLE_GATEWAY_MAX_PRESSURE_PA 110000U

#define BLE_GATEWAY_VERSION_OFFSET 0U
#define BLE_GATEWAY_NODE_ID_OFFSET 1U
#define BLE_GATEWAY_SEQUENCE_OFFSET 2U
#define BLE_GATEWAY_TEMPERATURE_OFFSET 6U
#define BLE_GATEWAY_HUMIDITY_OFFSET 8U
#define BLE_GATEWAY_PRESSURE_OFFSET 10U
#define BLE_GATEWAY_COMMAND_TYPE_OFFSET 1U
#define BLE_GATEWAY_COMMAND_INTERVAL_OFFSET 2U
#define BLE_GATEWAY_BYTE_0_OFFSET 0U
#define BLE_GATEWAY_BYTE_1_OFFSET 1U
#define BLE_GATEWAY_BYTE_2_OFFSET 2U
#define BLE_GATEWAY_BYTE_3_OFFSET 3U
#define BLE_GATEWAY_BYTE_SHIFT 8U
#define BLE_GATEWAY_HALF_WORD_SHIFT 16U
#define BLE_GATEWAY_THREE_BYTE_SHIFT 24U

typedef struct {
    uint8_t version;
    uint8_t node_id;
    uint32_t sequence;
    int16_t temperature_tenths_celsius;
    uint16_t humidity_tenths_percent;
    uint32_t pressure_pa;
} ble_gateway_report_t;

static inline uint16_t ble_gateway_get_u16_le(const uint8_t *data)
{
    return (uint16_t)data[BLE_GATEWAY_BYTE_0_OFFSET] |
           ((uint16_t)data[BLE_GATEWAY_BYTE_1_OFFSET] << BLE_GATEWAY_BYTE_SHIFT);
}

static inline uint32_t ble_gateway_get_u32_le(const uint8_t *data)
{
    return (uint32_t)data[BLE_GATEWAY_BYTE_0_OFFSET] |
           ((uint32_t)data[BLE_GATEWAY_BYTE_1_OFFSET] << BLE_GATEWAY_BYTE_SHIFT) |
           ((uint32_t)data[BLE_GATEWAY_BYTE_2_OFFSET] << BLE_GATEWAY_HALF_WORD_SHIFT) |
           ((uint32_t)data[BLE_GATEWAY_BYTE_3_OFFSET] << BLE_GATEWAY_THREE_BYTE_SHIFT);
}

static inline void ble_gateway_put_u16_le(uint8_t *data, uint16_t value)
{
    data[BLE_GATEWAY_BYTE_0_OFFSET] = (uint8_t)value;
    data[BLE_GATEWAY_BYTE_1_OFFSET] = (uint8_t)(value >> BLE_GATEWAY_BYTE_SHIFT);
}

static inline void ble_gateway_put_u32_le(uint8_t *data, uint32_t value)
{
    data[BLE_GATEWAY_BYTE_0_OFFSET] = (uint8_t)value;
    data[BLE_GATEWAY_BYTE_1_OFFSET] = (uint8_t)(value >> BLE_GATEWAY_BYTE_SHIFT);
    data[BLE_GATEWAY_BYTE_2_OFFSET] = (uint8_t)(value >> BLE_GATEWAY_HALF_WORD_SHIFT);
    data[BLE_GATEWAY_BYTE_3_OFFSET] = (uint8_t)(value >> BLE_GATEWAY_THREE_BYTE_SHIFT);
}

static inline void ble_gateway_encode_report(uint8_t output[BLE_GATEWAY_REPORT_SIZE],
                                             const ble_gateway_report_t *report)
{
    output[BLE_GATEWAY_VERSION_OFFSET] = report->version;
    output[BLE_GATEWAY_NODE_ID_OFFSET] = report->node_id;
    ble_gateway_put_u32_le(&output[BLE_GATEWAY_SEQUENCE_OFFSET], report->sequence);
    ble_gateway_put_u16_le(&output[BLE_GATEWAY_TEMPERATURE_OFFSET],
                           (uint16_t)report->temperature_tenths_celsius);
    ble_gateway_put_u16_le(&output[BLE_GATEWAY_HUMIDITY_OFFSET], report->humidity_tenths_percent);
    ble_gateway_put_u32_le(&output[BLE_GATEWAY_PRESSURE_OFFSET], report->pressure_pa);
}

static inline void ble_gateway_unpack_report(const uint8_t *data, ble_gateway_report_t *report)
{
    report->version = data[BLE_GATEWAY_VERSION_OFFSET];
    report->node_id = data[BLE_GATEWAY_NODE_ID_OFFSET];
    report->sequence = ble_gateway_get_u32_le(&data[BLE_GATEWAY_SEQUENCE_OFFSET]);
    report->temperature_tenths_celsius =
        (int16_t)ble_gateway_get_u16_le(&data[BLE_GATEWAY_TEMPERATURE_OFFSET]);
    report->humidity_tenths_percent = ble_gateway_get_u16_le(&data[BLE_GATEWAY_HUMIDITY_OFFSET]);
    report->pressure_pa = ble_gateway_get_u32_le(&data[BLE_GATEWAY_PRESSURE_OFFSET]);
}

static inline bool ble_gateway_report_fields_valid(const ble_gateway_report_t *report)
{
    return report->node_id == BLE_GATEWAY_NODE_ID_DEFAULT && report->sequence != 0U &&
           report->temperature_tenths_celsius >= BLE_GATEWAY_MIN_TEMPERATURE_TENTHS_C &&
           report->temperature_tenths_celsius <= BLE_GATEWAY_MAX_TEMPERATURE_TENTHS_C &&
           report->humidity_tenths_percent <= BLE_GATEWAY_MAX_HUMIDITY_TENTHS_PERCENT &&
           report->pressure_pa >= BLE_GATEWAY_MIN_PRESSURE_PA && report->pressure_pa <= BLE_GATEWAY_MAX_PRESSURE_PA;
}

static inline bool ble_gateway_decode_report(const uint8_t *data, uint16_t length, ble_gateway_report_t *report)
{
    if (data == NULL || report == NULL || length != BLE_GATEWAY_REPORT_SIZE ||
        data[BLE_GATEWAY_VERSION_OFFSET] != BLE_GATEWAY_PROTOCOL_VERSION) {
        return false;
    }
    ble_gateway_unpack_report(data, report);
    return ble_gateway_report_fields_valid(report);
}

static inline void ble_gateway_encode_interval_command(uint8_t output[BLE_GATEWAY_COMMAND_SIZE], uint32_t interval_s)
{
    output[BLE_GATEWAY_VERSION_OFFSET] = BLE_GATEWAY_PROTOCOL_VERSION;
    output[BLE_GATEWAY_COMMAND_TYPE_OFFSET] = BLE_GATEWAY_COMMAND_SET_INTERVAL;
    ble_gateway_put_u32_le(&output[BLE_GATEWAY_COMMAND_INTERVAL_OFFSET], interval_s);
}

static inline bool ble_gateway_command_header_valid(const uint8_t *data, uint16_t length, const uint32_t *interval_s)
{
    return data != NULL && interval_s != NULL && length == BLE_GATEWAY_COMMAND_SIZE &&
           data[BLE_GATEWAY_VERSION_OFFSET] == BLE_GATEWAY_PROTOCOL_VERSION &&
           data[BLE_GATEWAY_COMMAND_TYPE_OFFSET] == BLE_GATEWAY_COMMAND_SET_INTERVAL;
}

static inline bool ble_gateway_store_interval(const uint8_t *data, uint32_t *interval_s)
{
    uint32_t value = ble_gateway_get_u32_le(&data[BLE_GATEWAY_COMMAND_INTERVAL_OFFSET]);
    if (value < BLE_GATEWAY_MIN_INTERVAL_S || value > BLE_GATEWAY_MAX_INTERVAL_S) {
        return false;
    }
    *interval_s = value;
    return true;
}

static inline bool ble_gateway_decode_interval_command(const uint8_t *data, uint16_t length, uint32_t *interval_s)
{
    if (!ble_gateway_command_header_valid(data, length, interval_s)) {
        return false;
    }
    return ble_gateway_store_interval(data, interval_s);
}

#endif

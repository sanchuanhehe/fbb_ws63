/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Implement for Cilent Model Attr
 *
 * History:
 * 2025-5-22, Create file.
 */

#include "syscfg/syscfg.h"
#include "model_common.h"
#include "model_def.h"
#include "ble_mesh_vnd_attr.h"

// 主动上报处理
static int mesh_onoff_notify(const uint8_t *data, uint16_t len)
{
    if (data == NULL) {
        BT_ERR("vnd attr data is NULL");
        return -1;
    }

    BT_INFO("vnd cli len %d, bytes %s", len, bt_hex(data, len));
    return 0;
}

static int mesh_unix_time_notify(const uint8_t *data, uint16_t len)
{
    if (data == NULL) {
        BT_ERR("vnd attr data is NULL");
        return -1;
    }

    BT_INFO("vnd cli len %d, bytes %s", len, bt_hex(data, len));
    return 0;
}

static int mesh_power_percent_notify(const uint8_t *data, uint16_t len)
{
    if (data == NULL) {
        BT_ERR("vnd attr data is NULL");
        return -1;
    }

    BT_INFO("vnd cli len %d, bytes %s", len, bt_hex(data, len));
    return 0;
}

static bt_mesh_vnd_mod_property_t bt_mesh_vnd_cli_property_array[] = {
    {mesh_onoff_notify, NULL, MESH_VENDOR_ONOFF_TYPE, 1},
    {mesh_unix_time_notify, NULL, MESH_VENDOR_UNIX_TIME_TYPE, 6},
    {mesh_power_percent_notify, NULL, MESH_VENDOR_POWER_PERCENT_TYPE, 1},
};

bt_mesh_vnd_mod_property_t *bt_mesh_vnd_cli_property_array_get(uint16_t attr_type)
{
    uint8_t id = 0;

    for (id = 0; id < sizeof(bt_mesh_vnd_cli_property_array) / sizeof(bt_mesh_vnd_cli_property_array[0]); id++) {
        if (attr_type == bt_mesh_vnd_cli_property_array[id].type) {
            return &bt_mesh_vnd_cli_property_array[id];
        }
    }
    BT_ERR("vnd attr_type:%04x cli find id failed", attr_type);

    return NULL;
}

/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Implement for Server Model Attr
 *
 * History:
 * 2025-5-22, Create file.
 */

#include "syscfg/syscfg.h"
#include "model_def.h"
#include "vnd_opcode.h"
#include "ble_mesh_vnd_attr.h"

static bt_mesh_vnd_data_t g_vnd_data;

static int mesh_onoff_set(const uint8_t *data, uint16_t len)
{
    if (data == NULL) {
        BT_ERR("vnd attr data is NULL");
        return -1;
    }

    g_vnd_data.mesh_onoff = *data;
    BT_INFO("vnd srv Mesh set data is OnOff:%d", g_vnd_data.mesh_onoff);

    return 0;
}

static int mesh_onoff_get(uint8_t *data, uint16_t buf_len)
{
    if (data == NULL) {
        BT_ERR("vnd attr data is NULL");
        return -1;
    }

    *data = g_vnd_data.mesh_onoff;
    BT_INFO("vnd srv Mesh get data is OnOff:%d", g_vnd_data.mesh_onoff);

    return sizeof(g_vnd_data.mesh_onoff);
}

static int mesh_unix_time_set(const uint8_t *data, uint16_t len)
{
    if (data == NULL) {
        BT_ERR("vnd attr data is NULL");
        return -1;
    }

    uint8_t i = 0;

    memcpy_s(&g_vnd_data.mesh_unix_time, sizeof(g_vnd_data.mesh_unix_time), data, sizeof(g_vnd_data.mesh_unix_time));
    for (i = 0; i < ARRAY_SIZE(g_vnd_data.mesh_unix_time); i++) {
        BT_INFO("vnd srv Mesh set data is unix_time:%d-0x%02x", i, g_vnd_data.mesh_unix_time[i]);
    }

    return 0;
}

static int mesh_unix_time_get(uint8_t *data, uint16_t buf_len)
{
    if (data == NULL) {
        BT_ERR("vnd attr data is NULL");
        return -1;
    }

    uint8_t i = 0;

    if (memcpy_s(data, sizeof(g_vnd_data.mesh_unix_time),
                 &g_vnd_data.mesh_unix_time,
                 sizeof(g_vnd_data.mesh_unix_time)) != EOK) {
    return -1;
    }
    for (i = 0; i < ARRAY_SIZE(g_vnd_data.mesh_unix_time); i++) {
        BT_INFO("vnd srv Mesh get data is unix_time:%d-0x%02x", i, g_vnd_data.mesh_unix_time[i]);
    }

    return sizeof(g_vnd_data.mesh_unix_time);
}

static int mesh_power_percent_set(const uint8_t *data, uint16_t len)
{
    if (data == NULL) {
        BT_ERR("vnd attr data is NULL");
        return -1;
    }

    g_vnd_data.mesh_power_percent = *data;
    BT_INFO("vnd srv Mesh set data is power_percent:%d", g_vnd_data.mesh_power_percent);

    return 0;
}

static int mesh_power_percent_get(uint8_t *data, uint16_t buf_len)
{
    if (data == NULL) {
        BT_ERR("vnd attr data is NULL");
        return -1;
    }

    *data = g_vnd_data.mesh_power_percent;
    BT_INFO("vnd srv Mesh get data is power_percent:%d", g_vnd_data.mesh_power_percent);

    return sizeof(g_vnd_data.mesh_power_percent);
}

static bt_mesh_vnd_mod_property_t bt_mesh_vnd_srv_property_array[] = {
    {mesh_onoff_set, mesh_onoff_get, MESH_VENDOR_ONOFF_TYPE, 1},
    {mesh_unix_time_set, mesh_unix_time_get, MESH_VENDOR_UNIX_TIME_TYPE, 6},
    {mesh_power_percent_set, mesh_power_percent_get, MESH_VENDOR_POWER_PERCENT_TYPE, 1},
};

uint8_t bt_mesh_vnd_srv_property_array_size_get(void)
{
    return sizeof(bt_mesh_vnd_srv_property_array) / sizeof(bt_mesh_vnd_srv_property_array[0]);
}

bt_mesh_vnd_mod_property_t *bt_mesh_vnd_srv_property_array_get_by_id(uint8_t id)
{
    return &bt_mesh_vnd_srv_property_array[id];
}

bt_mesh_vnd_mod_property_t *bt_mesh_vnd_srv_property_array_get(uint16_t attr_type)
{
    uint16_t id = 0;

    for (id = 0; id < bt_mesh_vnd_srv_property_array_size_get(); id++) {
        if (attr_type == bt_mesh_vnd_srv_property_array[id].type) {
            return &bt_mesh_vnd_srv_property_array[id];
        }
    }
    BT_ERR("vnd attr_type:%04x srv find id failed", attr_type);

    return NULL;
}

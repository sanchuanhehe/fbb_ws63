/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Defintion for Cilent Model
 *
 * History:
 * 2025-5-22, Create file.
 */

#ifndef __BEL_MESH_VND_ATTR_H__
#define __BEL_MESH_VND_ATTR_H__

#include "mesh/mesh.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief define property set handle return 0 if success, other is error
 *        sdk call the function that inform the server data to the device
 */
typedef int (*bt_mesh_vnd_property_set_cb)(const uint8_t *data, uint16_t len);

/**
 * @brief define property get handle. return the data length obtained, -1 is error, 0 is no data
 *        sdk call the function fetch user data and send to the server,
 *        the data should wrapped by user and skd just transmit
 */
typedef int (*bt_mesh_vnd_property_get_cb)(uint8_t *buf, uint16_t buf_len);

typedef struct {
    bt_mesh_vnd_property_set_cb set_cb;  // set callback
    bt_mesh_vnd_property_get_cb get_cb;  // get callback
    uint16_t type;                       // attr type
    uint16_t data_len;                   //
} bt_mesh_vnd_mod_property_t;

enum {
    MESH_RS_OK = 0,         // success
    MESH_RS_ERR = -1,       // normal error
    MESH_RS_ERR_PARA = -2,  // parameters error
};

// vnd attr type
enum {
    MESH_VENDOR_ONOFF_TYPE = 0xF000,
    MESH_VENDOR_UNIX_TIME_TYPE = 0xF001,
    MESH_VENDOR_POWER_PERCENT_TYPE = 0xF002,
};

typedef struct {
    uint8_t mesh_onoff;
    uint8_t mesh_unix_time[6];
    uint8_t mesh_power_percent;
} bt_mesh_vnd_data_t;

// cli
bt_mesh_vnd_mod_property_t *bt_mesh_vnd_cli_property_array_get(uint16_t attr_type);

// srv
uint8_t bt_mesh_vnd_srv_property_array_size_get(void);
bt_mesh_vnd_mod_property_t *bt_mesh_vnd_srv_property_array_get_by_id(uint8_t id);
bt_mesh_vnd_mod_property_t *bt_mesh_vnd_srv_property_array_get(uint16_t attr_type);

#ifdef __cplusplus
}
#endif

#endif
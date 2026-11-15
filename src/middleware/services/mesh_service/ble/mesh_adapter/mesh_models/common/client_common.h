/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Common Interfaces on the Client
 *
 * History:
 * 2024-6-21, Create file.
 */
#ifndef CLIENT_COMMON_H
#define CLIENT_COMMON_H

#include <string.h>
#include <stdint.h>
#include <ble_mesh_prov.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bt_mesh_model *bt_mesh_get_gen_onoff_cli_model(void);
struct bt_mesh_model *bt_mesh_get_gen_level_cli_model(void);
struct bt_mesh_model *bt_mesh_get_gen_dtt_cli_model(void);
struct bt_mesh_model *bt_mesh_get_gen_power_onoff_cli_model(void);
struct bt_mesh_model *bt_mesh_get_gen_power_level_cli_model(void);
struct bt_mesh_model *bt_mesh_get_gen_battery_cli_model(void);
struct bt_mesh_model *bt_mesh_get_gen_prop_cli_model(void);
struct bt_mesh_model *bt_mesh_get_light_lightness_cli_model(void);
struct bt_mesh_model *bt_mesh_get_light_ctl_cli_model(void);
struct bt_mesh_model *bt_mesh_get_sensor_cli_model(void);
struct bt_mesh_model *bt_mesh_get_scene_cli_model(void);
struct bt_mesh_model *bt_mesh_get_time_cli_model(void);
struct bt_mesh_model *bt_mesh_get_scheduler_cli_model(void);
struct bt_mesh_model *bt_mesh_get_gen_location_cli_model(void);

#ifdef __cplusplus
}
#endif

#endif /* _SERVER_COMMON_H_ */

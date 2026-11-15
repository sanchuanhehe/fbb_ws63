/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: shell config
 *
 * History:
 * 2024-6-21, Create file.
 */
#ifndef __SHELL_H__
#define __SHELL_H__

#include "ble_mesh_hex.h"

typedef int (*cmd_func)(int argc, const char *argv[]);

typedef struct {
    const char *str;
    cmd_func func;
} at_cmd;

/** Mesh Application. */
struct bt_mesh_app_key {
    uint16_t net_idx;
    uint16_t app_idx;
    bool updated;
    struct bt_mesh_app_cred {
        uint8_t id;
        uint8_t val[16];
    } keys[2];
};

static inline uint8_t bt_mesh_hb_log(uint32_t val)
{
    if (!val) {
        return 0x00;
    } else if (val == 0xffff) {
        return 0xff;
    } else {
        return 32 - __builtin_clz(val);  // 32:size
    }
}

void ble_mesh_shell_init(void);
void mesh_at_generics_register(void);
void mesh_at_lighting_register(void);
void mesh_at_sensors_register(void);
void mesh_at_time_scenes_register(void);
void mesh_at_cfg_register(void);
void mesh_at_prov_register(void);
void mesh_at_health_register(void);
void mesh_at_vnd_register(void);
void mesh_at_priv_beacon_register(void);
void mesh_at_register(void);
void mesh_at_rmt_prov_register(void);
void mesh_at_ota_register(void);
void mesh_at_message_register(void);

#endif

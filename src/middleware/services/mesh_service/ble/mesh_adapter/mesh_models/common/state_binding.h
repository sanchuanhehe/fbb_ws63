/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: State Binding Interface Declaration
 *
 * History:
 * 2024-6-21, Create file.
 */
#ifndef __STATE_BINDING_H__
#define __STATE_BINDING_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern struct bt_mesh_gen_onoff_srv *gen_onoff_srv;
extern struct bt_mesh_gen_level_srv *gen_level_srv;
extern uint8_t g_gen_def_trans_time;
extern struct bt_mesh_light_lightness_srv *light_lightness_srv;
extern ble_mesh_light_ctl_srv_t *light_ctl_svr_usr_data;
extern ble_mesh_light_ctl_setup_srv_t *light_ctl_setup_svr_usr_data;
extern ble_mesh_light_ctl_temp_srv_t *light_ctl_temp_svr_usr_data;

void gen_onoff_bind_state(uint8_t onoff, struct bt_mesh_model *onoff_model);
void gen_level_bind_state(int16_t gen_level, struct bt_mesh_model *gen_level_model);
void gen_power_level_bind_state(uint16_t gen_pwr_level, struct bt_mesh_model *gen_pwr_level_model);
void lightness_actual_bind_state(uint16_t lightness_actual, struct bt_mesh_model *lightness_model);
void lightness_linear_bind_state(uint16_t lightness_linear, struct bt_mesh_model *lightness_model);
void gen_onpowerup_bind_state(uint8_t onpowerup, struct bt_mesh_model *gen_power_onoff_model);

void light_ctl_light_update_state_binding(uint16_t lightness);
void light_ctl_temp_update_state_binding(uint16_t temp);

#ifdef __cplusplus
}
#endif

#endif /* __STATE_BINDING_H__ */

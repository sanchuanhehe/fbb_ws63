 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: State Transition Interface Statement
 *
 * History:
 * 2024-6-21, Create file.
 */
#ifndef __STATE_TRANSITION_H__
#define __STATE_TRANSITION_H__

#include "model_common.h"
#include "model/generic_server.h"
#include "model/lightness_server.h"
#include "model/light_ctl_server.h"
#include "model/scene_server.h"

#ifdef __cplusplus
extern "C" {
#endif

void calculate_rt(struct transition *transition);
void bt_mesh_server_stop_transition(struct transition *transition);
void bt_mesh_server_start_transition(struct transition *transition);
void onoff_tt_values(struct bt_mesh_gen_onoff_srv *srv);
void level_tt_values(struct bt_mesh_gen_level_srv *srv);
void power_level_tt_values(struct bt_mesh_gen_power_level_srv *srv);
void scene_tt_values(struct bt_mesh_scene_srv *srv, uint8_t trans_time, uint8_t delay);
void lightness_tt_values(struct bt_mesh_light_lightness_srv *srv);
void lightness_linear_tt_values(struct bt_mesh_light_lightness_srv *srv);
void generic_onoff_work_handler(struct ble_npl_event *work);
void generic_level_work_handler(struct ble_npl_event *work);
void generic_power_level_work_handler(struct ble_npl_event *work);
void scene_recall_work_handler(struct ble_npl_event *work);
void lightness_actual_work_handler(struct ble_npl_event *work);
void lightness_linear_work_handler(struct ble_npl_event *work);
void light_ctl_tt_values(ble_mesh_light_ctl_srv_t *srv, uint8_t tt, uint8_t delay);
void light_ctl_temp_tt_values(ble_mesh_light_ctl_temp_srv_t *srv, uint8_t tt, uint8_t delay);
void light_ctl_work_handler(struct ble_npl_event *work);
void light_ctl_temp_work_handler(struct ble_npl_event *work);
void time_work_handler(struct ble_npl_event *work);
void scheduler_act_work_handler(struct ble_npl_event *work);

#ifdef __cplusplus
}
#endif

#endif /* __STATE_TRANSITION_H__ */

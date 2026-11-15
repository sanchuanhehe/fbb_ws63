/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: ble mesh operator interface.
 *
 * History:
 * 2024-6-21, Create file.
 */
#ifndef __BLE_MESH_MODEL_OP_H__
#define __BLE_MESH_MODEL_OP_H__
#include "mesh/access.h"
#include "health_client.h"
#include "lightness_server.h"
#include "light_ctl_server.h"
#include "generic_server.h"
#include "scheduler_server.h"
#ifdef __cplusplus
extern "C" {
#endif

#define MODEL_ONOFF_PUB_MSG_LEN 4
#define MODEL_LEVEL_PUB_MSG_LEN 7
#define MODEL_DTT_PUB_MSG_LEN 4
#define MODEL_ONPOWERUP_PUB_MSG_LEN 4
#define MODEL_LIGHTNESS_PUB_MSG_LEN 4
#define MODEL_LIGHT_CTL_PUB_MSG_LEN 11
#define MODEL_LIGHT_CTL_TEMP_PUB_MSG_LEN 11
#define MODEL_DFT_TRS_TIME_PUB_MSG_LEN 1

void ble_mesh_health_current_status(struct bt_mesh_health_cli *cli, uint16_t addr, uint8_t test_id, uint16_t cid,
                                    uint8_t *faults, size_t fault_count);
// gen onoff server 操作
int ble_mesh_gen_onoff_set_state(struct bt_mesh_model *model, uint8_t state);
// light ctl server 操作
int set_by_lightness_setup_state(struct bt_mesh_model *model, struct light_lightness_state *state);
int set_by_lightness_state(struct bt_mesh_model *model, struct light_lightness_state *state);
int ble_mesh_light_ctl_set_cb(struct bt_mesh_model *model, ble_mesh_light_ctl_state *ctl_state);
int set_by_level_state(struct bt_mesh_model *model, struct generic_level_state *state);

int ble_mesh_current_fault_get(struct bt_mesh_model *model, uint8_t *test_id, uint16_t *company_id, uint8_t *faults,
                               uint8_t *fault_count);

int ble_mesh_registered_fault_get(struct bt_mesh_model *model, uint16_t company_id, uint8_t *test_id, uint8_t *faults,
                                  uint8_t *fault_count);

int ble_mesh_fault_clear(struct bt_mesh_model *model, uint16_t company_id);

int ble_mesh_fault_test(struct bt_mesh_model *model, uint8_t test_id, uint16_t company_id);

void ble_mesh_attention_on(struct bt_mesh_model *model);

void ble_mesh_attention_off(struct bt_mesh_model *model);

int ble_mesh_dtt_set_cb(struct bt_mesh_model *model, uint8_t state);
int ble_mesh_scene_set_cb(struct bt_mesh_model *model, uint32_t opcode, const uint8_t *state);
int ble_mesh_scheduler_set_cb(struct bt_mesh_model *model, struct bt_mesh_scheduler_state_t *scheduler_state);

#ifdef __cplusplus
}
#endif

#endif /* __BLE_MESH_MODEL_OP_H__ */

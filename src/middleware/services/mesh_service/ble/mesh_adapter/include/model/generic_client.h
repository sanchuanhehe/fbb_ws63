 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: generic client config.
 *
 * History:
 * 2024-6-21, Create file.
 */
#ifndef __GENERIC_CLIENT_H__
#define __GENERIC_CLIENT_H__

#include "model_def.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const struct bt_mesh_model_op gen_onoff_cli_op[];
extern const struct bt_mesh_model_cb bt_mesh_gen_onoff_cli_cb;

#define BT_MESH_MODEL_GEN_ONOFF_CLI(cli_data, pub) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_ONOFF_CLI, gen_onoff_cli_op, pub, cli_data, &bt_mesh_gen_onoff_cli_cb)

extern const struct bt_mesh_model_op gen_level_cli_op[];
extern const struct bt_mesh_model_cb bt_mesh_gen_level_cli_cb;

#define BT_MESH_MODEL_GEN_LEVEL_CLI(cli_data, pub) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_LEVEL_CLI, gen_level_cli_op, pub, cli_data, &bt_mesh_gen_level_cli_cb)

extern const struct bt_mesh_model_op gen_dtt_cli_op[];
extern const struct bt_mesh_model_cb bt_mesh_gen_dtt_cli_cb;

#define BT_MESH_MODEL_GEN_DEF_TRANS_TIME_CLI(cli_data, pub) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_CLI, gen_dtt_cli_op, pub, cli_data, &bt_mesh_gen_dtt_cli_cb)

extern const struct bt_mesh_model_op gen_power_onoff_cli_op[];
extern const struct bt_mesh_model_cb bt_mesh_gen_power_onoff_cli_cb;

#define BT_MESH_MODEL_GEN_POWER_ONOFF_CLI(cli_data, pub)                                          \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_POWER_ONOFF_CLI, gen_power_onoff_cli_op, pub, cli_data, \
        &bt_mesh_gen_power_onoff_cli_cb)

extern const struct bt_mesh_model_op gen_battery_cli_op[];
extern const struct bt_mesh_model_cb bt_mesh_gen_battery_cli_cb;

#define BT_MESH_MODEL_GEN_BATTERY_CLI(cli_data, pub) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_BATTERY_CLI, gen_battery_cli_op, pub, cli_data, &bt_mesh_gen_battery_cli_cb)

extern const struct bt_mesh_model_op gen_power_level_cli_op[];
extern const struct bt_mesh_model_cb bt_mesh_gen_power_level_cli_cb;

#define BT_MESH_MODEL_GEN_POWER_LEVEL_CLI(cli_data, pub)                                          \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_POWER_LEVEL_CLI, gen_power_level_cli_op, pub, cli_data, \
        &bt_mesh_gen_power_level_cli_cb)

extern const struct bt_mesh_model_op gen_location_cli_op[];
extern const struct bt_mesh_model_cb bt_mesh_gen_location_cli_cb;

#define BT_MESH_MODEL_GEN_LOCATION_CLI(cli_data, pub)                                          \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_LOCATION_CLI, gen_location_cli_op, pub, cli_data, \
        &bt_mesh_gen_location_cli_cb)

extern const struct bt_mesh_model_op gen_prop_cli_op[];
extern const struct bt_mesh_model_cb bt_mesh_gen_prop_cli_cb;

#define BT_MESH_MODEL_GEN_PROP_CLI(cli_data, pub) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_PROP_CLI, gen_prop_cli_op, pub, cli_data, &bt_mesh_gen_prop_cli_cb)

struct gen_user_properties_status {
    struct os_mbuf *user_prop_ids;
};

struct gen_user_property_status {
    uint16_t user_prop_id;
    uint16_t user_access;
    struct os_mbuf *user_prop_val;
};

struct gen_manu_properties_status {
    struct os_mbuf *manu_prop_ids;
};

struct gen_manu_property_status {
    uint16_t manu_prop_id;
    uint16_t manu_access;
    struct os_mbuf *manu_prop_val;
};

struct gen_user_property_param {
    int (*status)(struct bt_mesh_model *model, struct gen_user_property_status *user_prop);
};

struct gen_user_properties_param {
    int (*status)(struct bt_mesh_model *model, struct gen_user_properties_status *user_prop);
};

struct gen_manu_property_param {
    int (*status)(struct bt_mesh_model *model, struct gen_manu_property_status *user_prop);
};

struct gen_manu_properties_param {
    int (*status)(struct bt_mesh_model *model, struct gen_manu_properties_status *user_prop);
};

struct gen_onoff_status {
    uint8_t onoff;
    uint8_t target_onoff;
    uint8_t remain_time;
    uint8_t op_en;
};

struct gen_level_status {
    int16_t level;
    int16_t target_level;
    uint8_t remain_time;
    uint8_t op_en;
};

struct gen_battery_status {
    uint32_t time_to_discharge;
    uint32_t time_to_charge;
    uint8_t battery_level;
    uint8_t flags;
};

struct gen_location_status {
    uint32_t global_latitude;
    uint32_t global_longitude;
    uint16_t global_altitude;
};

struct gen_power_level_status {
    uint16_t power_level;
    uint16_t target_power_level;
    uint8_t remain_time;
    uint8_t op_en;
};

struct gen_power_level_range_status {
    uint16_t min_power_level;
    uint16_t max_power_level;
    uint8_t status_code;
};

struct gen_onoff_set {
    uint8_t onoff;
    uint8_t tid;
    uint8_t trans_time;
    uint8_t delay;
    uint8_t op_en;
};

struct gen_level_set {
    int16_t level;
    uint16_t move_level;
    int32_t delta_level;
    uint8_t tid;
    uint8_t trans_time;
    uint8_t delay;
    uint8_t op_en;
};

struct gen_power_level_set {
    uint16_t power_level;
    uint16_t power_def_level;
    uint16_t power_min_level;
    uint16_t power_max_level;
    uint8_t tid;
    uint8_t trans_time;
    uint8_t delay;
    uint8_t op_en;
};

struct gen_user_property_set {
    uint16_t user_property_id;           /* Property ID identifying a Generic User Property */
    struct os_mbuf *user_property_value; /* Raw value for the User Property    */
};

struct gen_manu_property_set {
    uint16_t manu_property_id; /* Property ID identifying a Generic Manufacturer Property */
    uint8_t manu_user_access;  /* Enumeration indicating user access                      */
};

int bt_mesh_gen_onoff_get(bt_mesh_client_common_param_t *common, struct gen_onoff_status *status);
int bt_mesh_gen_onoff_set(bt_mesh_client_common_param_t *common, struct gen_onoff_set *set,
    struct gen_onoff_status *status);

int bt_mesh_gen_level_get(bt_mesh_client_common_param_t *common, struct gen_level_status *status);
int bt_mesh_gen_level_set(bt_mesh_client_common_param_t *common, struct gen_level_set *set,
    struct gen_level_status *status);

int bt_mesh_gen_dtt_get(bt_mesh_client_common_param_t *common, uint8_t *status);
int bt_mesh_gen_dtt_set(bt_mesh_client_common_param_t *common, uint8_t *set, uint8_t *status);

int bt_mesh_gen_battery_get(bt_mesh_client_common_param_t *common, struct gen_battery_status *status);

int bt_mesh_gen_location_get(bt_mesh_client_common_param_t *common, struct gen_location_status *status);

int bt_mesh_gen_power_onoff_get(bt_mesh_client_common_param_t *common, uint8_t *status);
int bt_mesh_gen_power_onoff_set(bt_mesh_client_common_param_t *common, uint8_t *set, uint8_t *status);

int bt_mesh_gen_power_level_get(bt_mesh_client_common_param_t *common, void *status);
int bt_mesh_gen_power_level_set(bt_mesh_client_common_param_t *common, struct gen_power_level_set *set, void *status);

int bt_mesh_gen_properties_get(bt_mesh_client_common_param_t *common, void *param);
int bt_mesh_gen_property_get(bt_mesh_client_common_param_t *common, uint16_t prop_id, void *param);
int bt_mesh_gen_property_set(bt_mesh_client_common_param_t *common, void *set, void *param);

#ifdef __cplusplus
}
#endif

#endif /* __GENERIC_CLIENT_H__ */

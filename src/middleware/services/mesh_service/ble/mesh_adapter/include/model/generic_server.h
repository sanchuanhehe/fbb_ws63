 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: generic server config
 *
 * History:
 * 2024-6-21, Create file.
 */

#ifndef __GENERIC_SERVER_H__
#define __GENERIC_SERVER_H__

#include "model_def.h"
#include "model/scene_server.h"

#ifdef __cplusplus
extern "C" {
#endif

/* gen onoff definition server begin */
struct bt_mesh_gen_onoff_srv {
    struct bt_mesh_model *model;
    uint8_t onoff;
    uint8_t target_onoff;
    struct last_msg last_msg;
    struct transition transition;
    int32_t tt_delta;

    int (*set)(struct bt_mesh_model *model, uint8_t onoff);
};
extern const struct bt_mesh_model_op gen_onoff_srv_op[];
extern const struct bt_mesh_model_cb gen_onoff_srv_cb;
#define BT_MESH_MODEL_GEN_ONOFF_SRV(srv, pub) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_ONOFF_SRV, gen_onoff_srv_op, pub, srv, &gen_onoff_srv_cb)
/* gen onoff definition server end */

/* gen level definition server begin */
struct generic_level_state {
    int16_t current_level;
    int16_t target_level;
    int16_t last_level;
    int32_t delta_level; // last_delta
};
struct bt_mesh_gen_level_srv {
    struct bt_mesh_model *model;
    struct generic_level_state level_state;
    struct last_msg last_msg;
    struct transition transition;
    int32_t tt_delta;

    int (*set)(struct bt_mesh_model *model, struct generic_level_state *state);
};
extern const struct bt_mesh_model_op gen_level_srv_op[];
extern const struct bt_mesh_model_cb gen_level_srv_cb;
#define BT_MESH_MODEL_GEN_LEVEL_SRV(srv, pub) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_LEVEL_SRV, gen_level_srv_op, pub, srv, &gen_level_srv_cb)
/* gen level definition end */

/* gen def_trans_time server definition begin */
struct bt_mesh_gen_dtt_srv {
    struct bt_mesh_model *model;
    uint8_t dtt;

    int (*set)(struct bt_mesh_model *model, uint8_t dtt);
};
extern const struct bt_mesh_model_op gen_def_trans_time_srv_op[];
extern const struct bt_mesh_model_cb gen_dtt_srv_cb;
#define BT_MESH_MODEL_GEN_TRANS_TIME_SRV(srv, pub) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_SRV, gen_def_trans_time_srv_op, pub, srv, &gen_dtt_srv_cb)
/* gen def_trans_time server definition end */

/* gen battery server definition begin */
struct battery_state {
    uint8_t battery_level;
    uint32_t time_to_discharge;
    uint32_t time_to_charge;
    uint8_t flags;
};
struct bt_mesh_gen_battery_srv {
    struct bt_mesh_model *model;
    struct battery_state battery_state;
};
extern const struct bt_mesh_model_op gen_battery_srv_op[];
extern const struct bt_mesh_model_cb gen_battery_srv_cb;
#define BT_MESH_MODEL_GEN_BATTERY_SRV(srv, pub) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_BATTERY_SRV, gen_battery_srv_op, pub, srv, &gen_battery_srv_cb)
/* gen battery server definition end */

/* gen power level server definition begin */
struct generic_power_level_state {
    uint16_t current_level;
    uint16_t target_level;
    uint16_t last_level; // 存储通用电源实际状态的最后一个已知的非0值
    uint16_t default_level;
    uint16_t min_level;
    uint16_t max_level;
    uint8_t status_code;
};
struct bt_mesh_gen_power_level_srv {
    struct bt_mesh_model *model;
    struct generic_power_level_state *pwr_level_state;
    struct last_msg last_msg;
    struct transition transition;
    int32_t tt_delta;

    int (*set)(struct bt_mesh_model *model, struct generic_power_level_state *power_level);
};
extern const struct bt_mesh_model_op gen_power_level_srv_op[];
extern const struct bt_mesh_model_cb gen_power_level_srv_cb;
#define BT_MESH_MODEL_GEN_POWER_LEVEL_SRV(srv, pub) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_POWER_LEVEL_SRV, gen_power_level_srv_op, pub, srv, &gen_power_level_srv_cb)

struct bt_mesh_gen_power_level_setup_srv {
    struct bt_mesh_model *model;
    struct generic_power_level_state *pwr_level_state;
    int (*set)(struct bt_mesh_model *model, struct generic_power_level_state *power_level);
};
extern const struct bt_mesh_model_op gen_power_level_setup_srv_op[];
extern const struct bt_mesh_model_cb gen_power_level_setup_srv_cb;
#define BT_MESH_MODEL_GEN_POWER_LEVEL_SETUP_SRV(srv, pub)                                                \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_POWER_LEVEL_SETUP_SRV, gen_power_level_setup_srv_op, pub, srv, \
        &gen_power_level_setup_srv_cb)
/* gen power level server definition end */

/* gen power onoff server definition begin */
struct generic_power_onoff_state {
    uint8_t power_onoff;
};

struct bt_mesh_gen_power_onoff_srv {
    struct bt_mesh_model *model;
    struct generic_power_onoff_state *power_onoff_state;
};
extern const struct bt_mesh_model_op gen_power_onoff_srv_op[];
extern const struct bt_mesh_model_cb gen_power_onoff_srv_cb;
#define BT_MESH_MODEL_GEN_POWER_ONOFF_SRV(srv, pub) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_POWER_ONOFF_SRV, gen_power_onoff_srv_op, pub, srv, &gen_power_onoff_srv_cb)

struct bt_mesh_gen_power_onoff_setup_srv {
    struct bt_mesh_model *model;
    struct generic_power_onoff_state *power_onoff_state;

    int (*set)(struct bt_mesh_model *model, uint8_t power_onoff);
};
extern const struct bt_mesh_model_op gen_power_onoff_setup_srv_op[];
extern const struct bt_mesh_model_cb gen_power_onoff_setup_srv_cb;
#define BT_MESH_MODEL_GEN_POWER_ONOFF_SETUP_SRV(srv, pub)                                                \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_POWER_ONOFF_SETUP_SRV, gen_power_onoff_setup_srv_op, pub, srv, \
        &gen_power_onoff_setup_srv_cb)
/* gen power onoff server definition end */

struct generic_user_properties_state {
    struct os_mbuf *user_prop_ids;
};
struct generic_user_property_state {
    uint16_t user_prop_id;
    uint16_t user_access;
    struct os_mbuf *user_prop_val;
};
struct bt_mesh_gen_user_prop_srv {
    struct bt_mesh_model *model;
    int (*get_prop)(struct bt_mesh_model *model, struct generic_user_property_state *user_prop);
    int (*get_props)(struct bt_mesh_model *model, struct generic_user_properties_state *user_props);
    int (*set)(struct bt_mesh_model *model, uint16_t user_prop_id, struct os_mbuf *user_prop_val);
};
extern const struct bt_mesh_model_op gen_user_prop_srv_op[];
extern const struct bt_mesh_model_cb gen_user_prop_srv_cb;
#define BT_MESH_MODEL_GEN_USER_PROP_SRV(srv, pub) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_USER_PROP_SRV, gen_user_prop_srv_op, pub, srv, &gen_user_prop_srv_cb)

struct generic_manu_properties_state {
    struct os_mbuf *manu_prop_ids;
};
struct generic_manu_property_state {
    uint16_t manu_prop_id;
    uint16_t manu_access;
    struct os_mbuf *manu_prop_val;
};
struct bt_mesh_gen_manufacturer_prop_srv {
    struct bt_mesh_model *model;
    int (*get_prop)(struct bt_mesh_model *model, struct generic_manu_property_state *manu_prop);
    int (*get_props)(struct bt_mesh_model *model, struct generic_manu_properties_state *manu_props);
    int (*set)(struct bt_mesh_model *model, uint16_t manu_prop_id, uint16_t manu_access);
};
extern const struct bt_mesh_model_op gen_manu_prop_srv_op[];
extern const struct bt_mesh_model_cb gen_manu_prop_srv_cb;
#define BT_MESH_MODEL_GEN_MANUFACTURER_PROP_SRV(srv, pub) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_MANUFACTURER_PROP_SRV, gen_manu_prop_srv_op, pub, srv, &gen_manu_prop_srv_cb)

/* gen location server definition begin */
struct location_local_state {
    uint16_t local_north;
    uint16_t local_east;
    uint16_t local_altitude;
    uint8_t  floor_number;
    uint16_t uncertainty;
};

struct generic_location_state {
    uint32_t global_latitude;
    uint32_t global_longitude;
    uint16_t global_altitude;
};

struct bt_mesh_gen_location_srv {
    struct bt_mesh_model *model;
    struct generic_location_state *location_state;
    struct location_local_state *loc_local_state;
};
extern const struct bt_mesh_model_op gen_location_srv_op[];
extern const struct bt_mesh_model_cb gen_location_srv_cb;
#define BT_MESH_MODEL_GEN_LOCATION_SRV(srv, pub) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_LOCATION_SRV, gen_location_srv_op, pub, srv, &gen_location_srv_cb)

struct bt_mesh_gen_location_setup_srv {
    struct bt_mesh_model *model;
    struct generic_location_state *location_state;
    struct location_local_state *loc_local_state;

    int (*set)(struct bt_mesh_model *model, struct generic_location_state *location);
};
extern const struct bt_mesh_model_op gen_location_setup_srv_op[];
extern const struct bt_mesh_model_cb gen_location_setup_srv_cb;
#define BT_MESH_MODEL_GEN_LOCATION_SETUP_SRV(srv, pub) \
    BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_LOCATION_SETUPSRV, gen_location_setup_srv_op, pub, srv, \
        &gen_location_setup_srv_cb)
/* gen location server definition end */
void bt_mesh_gen_onoff_set_scene(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx);
void bt_mesh_gen_level_set_scene(struct bt_mesh_model *model,
    struct bt_mesh_msg_ctx *ctx, bt_mesh_scene_data_t *p, bt_mesh_scene_recall_t *p_recall);

#ifdef __cplusplus
}
#endif

#endif /* __GENERIC_SERVER_H__ */
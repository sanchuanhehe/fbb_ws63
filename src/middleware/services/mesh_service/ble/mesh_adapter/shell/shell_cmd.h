/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: at bt mesh config
 *
 * History:
 * 2024-6-21, Create file.
 */
#ifndef _AT_BT_MESH_H_
#define _AT_BT_MESH_H_

#if MYNEWT_VAL(BLE_MESH_CONSOLE_ONOFF_CLIENT)
typedef struct {
    uint32_t para_map;
    char *para1;
    char *para2;
    char *para3;
    char *para4;
    char *para5;
    char *para6;
    char *para7;
    char *para8;
    char *para9;
    char *para10;
    char *para11;
    char *para12;
    char *para13;
    char *para14;
    char *para15;
    char *para16;
    char *para17;
} ble_mesh_onoff_client_t;
#endif

#if MYNEWT_VAL(BLE_MESH_CONSOLE_LIGHT_CTL_CLIENT)
typedef struct {
    uint32_t para_map;
    char *para1;
    char *para2;
    char *para3;
    char *para4;
    char *para5;
    char *para6;
    char *para7;
    char *para8;
    char *para9;
    char *para10;
    char *para11;
    char *para12;
    char *para13;
    char *para14;
    char *para15;
    char *para16;
    char *para17;
    char *para18;
    char *para19;
    char *para20;
    char *para21;
} ble_mesh_light_ctl_client_t;
#endif

#if MYNEWT_VAL(BLE_MESH_CONSOLE_LEVEL_CLIENT)
typedef struct {
    uint32_t para_map;
    char *para1;
    char *para2;
    char *para3;
    char *para4;
    char *para5;
    char *para6;
    char *para7;
    char *para8;
    char *para9;
    char *para10;
    char *para11;
    char *para12;
    char *para13;
    char *para14;
    char *para15;
    char *para16;
    char *para17;
} ble_mesh_level_client_t;
#endif

#if MYNEWT_VAL(BLE_MESH_CONSOLE_POWER_LEVEL_CLIENT)
typedef struct {
    uint32_t para_map;
    char *para1;
    char *para2;
    char *para3;
    char *para4;
    char *para5;
    char *para6;
    char *para7;
    char *para8;
    char *para9;
    char *para10;
    char *para11;
    char *para12;
    char *para13;
    char *para14;
    char *para15;
    char *para16;
    char *para17;
} ble_mesh_power_level_client_t;
#endif

#if MYNEWT_VAL(BLE_MESH_CONSOLE_DEF_TRANS_TIME_CLIENT)
typedef struct {
    uint32_t para_map;
    char *para1;
    char *para2;
    char *para3;
    char *para4;
    char *para5;
    char *para6;
    char *para7;
    char *para8;
    char *para9;
    char *para10;
    char *para11;
} ble_mesh_def_trans_time_t;
#endif

#if MYNEWT_VAL(BLE_MESH_CONSOLE_ONPOWERUP_CLIENT)
typedef struct {
    uint32_t para_map;
    char *para1;
    char *para2;
    char *para3;
    char *para4;
    char *para5;
    char *para6;
    char *para7;
    char *para8;
    char *para9;
    char *para10;
    char *para11;
} ble_mesh_on_power_client_t;
#endif

#if MYNEWT_VAL(BLE_MESH_CONSOLE_BATTERY_CLIENT)
typedef struct {
    uint32_t para_map;
    char *para1;
    char *para2;
    char *para3;
    char *para4;
    char *para5;
    char *para6;
    char *para7;
    char *para8;
    char *para9;
} ble_mesh_battery_client_t;
#endif

#if MYNEWT_VAL(BLE_MESH_CONSOLE_LIGHTNESS_CLIENT)
typedef struct {
    uint32_t para_map;
    char *para1;
    char *para2;
    char *para3;
    char *para4;
    char *para5;
    char *para6;
    char *para7;
    char *para8;
    char *para9;
    char *para10;
    char *para11;
    char *para12;
    char *para13;
    char *para14;
    char *para15;
    char *para16;
    char *para17;
} ble_mesh_lightness_client_t;
#endif

#if MYNEWT_VAL(BLE_MESH_CONSOLE_LOCATION_CLIENT)
typedef struct {
    uint32_t para_map;
    char *para1;
    char *para2;
    char *para3;
    char *para4;
    char *para5;
    char *para6;
    char *para7;
    char *para8;
    char *para9;
} ble_mesh_location_client_t;
#endif

#if MYNEWT_VAL(BLE_MESH_CONSOLE_PROP_CLIENT)
typedef struct {
    uint32_t para_map;
    char *para1;
    char *para2;
    char *para3;
    char *para4;
    char *para5;
    char *para6;
    char *para7;
    char *para8;
    char *para9;
    char *para10;
    char *para11;
    char *para12;
    char *para13;
} ble_mesh_property_info_t;
#endif

#if MYNEWT_VAL(BLE_MESH_CONSOLE_CFG_CLIENT)
typedef struct {
    uint32_t para_map;
    char *para1;
    char *para2;
    char *para3;
    char *para4;
    char *para5;
    char *para6;
    char *para7;
    char *para8;
    char *para9;
    char *para10;
    char *para11;
    char *para12;
    char *para13;
    char *para14;
    char *para15;
    char *para16;
    char *para17;
    char *para18;
    char *para19;
    char *para20;
    char *para21;
    char *para22;
    char *para23;
} ble_mesh_config_client_t;
#endif

#if MYNEWT_VAL(BLE_MESH_SENSOR_CLI)
typedef struct {
    uint32_t para_map;
    char *para1;
    char *para2;
    char *para3;
    char *para4;
    char *para5;
    char *para6;
    char *para7;
    char *para8;
    char *para9;
    char *para10;
    char *para11;
    char *para12;
    char *para13;
    char *para14;
    char *para15;
    char *para16;
    char *para17;
    char *para18;
    char *para19;
    char *para20;
    char *para21;
    char *para22;
    char *para23;
    char *para24;
    char *para25;
} ble_mesh_sensor_t;
#endif

#if MYNEWT_VAL(BLE_MESH_SCENE_CLI)
typedef struct {
    uint32_t para_map;
    char *para1;
    char *para2;
    char *para3;
    char *para4;
    char *para5;
    char *para6;
    char *para7;
    char *para8;
    char *para9;
    char *para10;
    char *para11;
    char *para12;
    char *para13;
    char *para14;
    char *para15;
    char *para16;
    char *para17;
} ble_mesh_scene_client_t;
#endif

#if MYNEWT_VAL(BLE_MESH_TIME_CLI)
typedef struct {
    uint32_t para_map;
    char *para1;
    char *para2;
    char *para3;
    char *para4;
    char *para5;
    char *para6;
    char *para7;
    char *para8;
    char *para9;
    char *para10;
    char *para11;
    char *para12;
    char *para13;
    char *para14;
    char *para15;
    char *para16;
    char *para17;
    char *para18;
    char *para19;
    char *para20;
    char *para21;
} ble_mesh_time_client_t;
#endif

#if MYNEWT_VAL(BLE_MESH_SCHEDULER_CLI)
typedef struct {
    uint32_t para_map;
    char *para1;
    char *para2;
    char *para3;
    char *para4;
    char *para5;
    char *para6;
    char *para7;
    char *para8;
    char *para9;
    char *para10;
    char *para11;
    char *para12;
    char *para13;
    char *para14;
    char *para15;
    char *para16;
    char *para17;
    char *para18;
    char *para19;
    char *para20;
    char *para21;
} ble_mesh_scheduler_client_t;
#endif

#if MYNEWT_VAL(BLE_MESH_CFG_TEST)
typedef struct {
    uint32_t para_map;
    char *para1;
    char *para2;
    char *para3;
    char *para4;
    char *para5;
    char *para6;
    char *para7;
    char *para8;
    char *para9;
    char *para10;
    char *para11;
    char *para12;
    char *para13;
    char *para14;
    char *para15;
    char *para16;
    char *para17;
    char *para18;
    char *para19;
    char *para20;
    char *para21;
    char *para22;
    char *para23;
} ble_mesh_config_local_t;
#endif

#if MYNEWT_VAL(BLE_MESH_HEALTH_CLI)
typedef struct {
    uint32_t para_map;
    char *para1;
    char *para2;
    char *para3;
    char *para4;
    char *para5;
    char *para6;
    char *para7;
} ble_mesh_health_model_t;
#endif
typedef struct {
    uint32_t para_map;
    char *para1;
    char *para2;
    char *para3;
    char *para4;
    char *para5;
    char *para6;
    char *para7;
    char *para8;
    char *para9;
    char *para10;
    char *para11;
    char *para12;
    char *para13;
    char *para14;
    char *para15;
    char *para16;
} ble_at_vnd_op_t;

at_ret_t at_bt_mesh_display_state(void);  // 显示mesh节点状态
#if MYNEWT_VAL(BLE_MESH_CONSOLE_ONOFF_CLIENT)
at_ret_t at_bt_mesh_onoff_client(const ble_mesh_onoff_client_t *args);  // light 客户端控制
#endif
#if MYNEWT_VAL(BLE_MESH_CONSOLE_LIGHT_CTL_CLIENT)
at_ret_t at_bt_mesh_light_ctl_client(const ble_mesh_light_ctl_client_t *args);  // light 控制温度和色差
#endif
#if MYNEWT_VAL(BLE_MESH_CONSOLE_LEVEL_CLIENT)  // 设置状态级别
at_ret_t at_bt_mesh_set_level_client(const ble_mesh_level_client_t *args);
#endif
#if MYNEWT_VAL(BLE_MESH_CONSOLE_POWER_LEVEL_CLIENT)  // 设置电源等级
at_ret_t at_bt_mesh_power_level_client(const ble_mesh_power_level_client_t *args);
#endif
#if MYNEWT_VAL(BLE_MESH_CONSOLE_DEF_TRANS_TIME_CLIENT)  // 设置数据传输时间
at_ret_t at_bt_mesh_set_def_trans_timer(const ble_mesh_def_trans_time_t *args);
#endif
#if MYNEWT_VAL(BLE_MESH_CONSOLE_ONPOWERUP_CLIENT)  // 获取light 上电状态
at_ret_t at_bt_mesh_onpower_state(const ble_mesh_on_power_client_t *args);
#endif
#if MYNEWT_VAL(BLE_MESH_CONSOLE_LIGHTNESS_CLIENT)
at_ret_t at_bt_mesh_lightness_client(const ble_mesh_lightness_client_t *args);
#endif
#if MYNEWT_VAL(BLE_MESH_CONSOLE_PROP_CLIENT)
at_ret_t at_bt_mesh_property_info(const ble_mesh_property_info_t *args);
#endif
#if MYNEWT_VAL(BLE_MESH_CFG_TEST)
at_ret_t at_bt_mesh_config_local(
    const ble_mesh_config_local_t *args);  // 配置本端，AT+BM_CT ---- 参数类型如何构建待确定
#endif
#if MYNEWT_VAL(BLE_MESH_CONSOLE_CFG_CLIENT)
at_ret_t at_bt_mesh_config_client(
    const ble_mesh_config_client_t *args);  // 配置客户端接口，AT+BM_CC --- 参数类型如何构建待确定
#endif
#if MYNEWT_VAL(BLE_MESH_SENSOR_CLI)
at_ret_t at_bt_mesh_sensor_client(const ble_mesh_sensor_t *args);
#endif
#if MYNEWT_VAL(BLE_MESH_SCENE_CLI)
at_ret_t at_bt_mesh_scene_client(const ble_mesh_scene_client_t *args);
#endif
#if MYNEWT_VAL(BLE_MESH_HEALTH_CLI)
at_ret_t at_bt_mesh_health_model(const ble_mesh_health_model_t *args);  // health model 模型 -- -参数类型如何构建待确定
#endif

#if MYNEWT_VAL(BLE_MESH_BEACON_CONFIG)
int cmd_cfg_set_beacon(int argc, const char *argv[]);
int cmd_cfg_get_beacon(int argc, const char *argv[]);
int cmd_cfg_set_priv_beacon(int argc, const char *argv[]);
int cmd_cfg_get_priv_beacon(int argc, const char *argv[]);
int cmd_cfg_set_priv_beacon_interval(int argc, const char *argv[]);
int cmd_cfg_get_priv_beacon_interval(int argc, const char *argv[]);
int cmd_cfg_get_addr_and_net_index(const char *argv[], uint16_t *dst, uint16_t *net_index);
#endif

#endif /* _AT_BT_MESH_H_ */
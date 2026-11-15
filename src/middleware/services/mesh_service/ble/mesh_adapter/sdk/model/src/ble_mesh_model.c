/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Mesh model.
 *
 * History:
 * 2025-4-15, Create file.
 */

#include "generic_server.h"
#include "generic_client.h"
#include "model_def.h"
#include "model_opcode.h"

#include "mesh/access.h"
#include "mesh/main.h"
#include "mesh/cfg_srv.h"
#include "mesh/cfg_cli.h"
#include "health_client.h"
#include "mesh/health_srv.h"

#include "common_def.h"
#include "soc_osal.h"
#include "osal_task.h"
#include "osal_addr.h"
#include "osal_debug.h"
#include "bts_def.h"
#include "syscfg/syscfg.h"
#include "ble_mesh_common.h"
#include "lightness_server.h"
#include "lightness_client.h"
#include "priv_beacon_srv.h"
#include "priv_beacon_cli.h"
#include "light_ctl_server.h"
#include "light_ctl_client.h"
#include "time_server.h"
#include "time_client.h"
#include "scene_server.h"
#include "scene_client.h"
#include "scheduler_server.h"
#include "scheduler_client.h"
#include "ble_mesh_model_op.h"
#include "vnd_server.h"
#include "vnd_client.h"
#include "mesh_ota_common.h"
#include "dfu_server.h"
#include "dfd_server.h"
#include "dfd_client.h"
#include "remote_prov_server.h"
#include "remote_prov_client.h"
#include "ble_mesh_model.h"

#define LED_OFF 0
#define FAULT_ARR_SIZE 2

#define MODEL_ONOFF_PUB_MSG_LEN 4
#define MODEL_HEALTH_PUB_MSG_LEN 4

#define BLE_MESH_SUCC 0

#define COMP_DATA_PAGE_0 0x00
#define COMP_DATA_MAX_LEN 32

#define MODEL_TIME_PUB_MSG_LEN 10
#define MODEL_SCENE_PUB_MSG_LEN 6
#define MODEL_SCHEDULER_PUB_MSG_LEN 10
#define MODEL_VND_PUB_MSG_LEN 16

// 灯控相关
#define MODEL_LIGHTNESS_PUB_MSG_LEN 4

// health检测客户端相关操作
#if defined(CONFIG_BLE_MESH_MODEL_HEALTH_CLIENT) && (CONFIG_BLE_MESH_MODEL_HEALTH_CLIENT == 1)
static struct bt_mesh_health_cli health_cli = {
    .current_status = ble_mesh_health_current_status,
};

static struct bt_mesh_model_pub health_pub_cli;
#endif

// health检测服务端相关操作
#if defined(CONFIG_BLE_MESH_MODEL_HEALTH_SERVER) && (CONFIG_BLE_MESH_MODEL_HEALTH_SERVER == 1)
static const struct bt_mesh_health_srv_cb health_srv_cb = {
    .fault_get_cur = &ble_mesh_current_fault_get,
    .fault_get_reg = &ble_mesh_registered_fault_get,
    .fault_clear = &ble_mesh_fault_clear,
    .fault_test = &ble_mesh_fault_test,
    .attn_on = &ble_mesh_attention_on,
    .attn_off = &ble_mesh_attention_off,
};

static struct bt_mesh_health_srv health_srv = {
    .cb = &health_srv_cb,
};

static struct bt_mesh_model_pub health_pub_srv;
#endif

// gen onoff server 操作
#if defined CONFIG_BLE_MESH_ONOFF_SERVER && (CONFIG_BLE_MESH_ONOFF_SERVER == 1)
static struct bt_mesh_gen_onoff_srv gen_onoff_server = {
    .set = ble_mesh_gen_onoff_set_state,
};

static struct bt_mesh_model_pub gen_onoff_srv_pub;
#endif

// gen onoff client
#if defined(CONFIG_BLE_MESH_ONOFF_CLIENT) && (CONFIG_BLE_MESH_ONOFF_CLIENT == 1)
static struct bt_mesh_gen_model_cli gen_onoff_client;
static struct bt_mesh_model_pub gen_onoff_pub_cli;
#endif

#if defined(CONFIG_BLE_MESH_LIGHT_CTL_SERVER) && (CONFIG_BLE_MESH_LIGHT_CTL_SERVER == 1)
// 亮度相关模型
struct light_lightness_state g_lightness_state = {
    .linear = UINT16_MAX,
    .target_linear = UINT16_MAX,

    .actual = UINT16_MAX,
    .target_actual = UINT16_MAX,

    .last = UINT16_MAX,
    .def = 0,

    .status_code = RANGE_SUCCESSFULLY_UPDATED,
    .light_range_min = LIGHTNESS_MIN,
    .light_range_max = LIGHTNESS_MAX,
};

struct bt_mesh_light_lightness_srv lightness_server = {
    .light_state = &g_lightness_state,
    .set = set_by_lightness_state,
};

struct bt_mesh_light_lightness_setup_srv lightness_setup_server = {
    .light_state = &g_lightness_state,
    .set = set_by_lightness_setup_state,
};

ble_mesh_light_ctl_state light_ctl_state = {
    .status_code = CANNOT_SET_RANGE_MIN,
    .temp_range_min = TEMP_MIN,
    .temp_range_max = TEMP_MAX,
    .temp_def = TEMP_MIN,
    .temp = TEMP_MIN,
};

ble_mesh_light_ctl_srv_t light_ctl_srv = {
    .state = &light_ctl_state,
    .set = ble_mesh_light_ctl_set_cb,
};

ble_mesh_light_ctl_setup_srv_t light_ctl_setup_srv = {
    .state = &light_ctl_state,
};

static struct bt_mesh_model_pub light_lightness_srv_pub;
static struct bt_mesh_model_pub light_lightness_srv_setup_pub;
static struct bt_mesh_model_pub light_ctl_srv_pub;
static struct bt_mesh_model_pub light_ctl_setup_srv_pub;

/* 色温相关模型 */
static struct bt_mesh_gen_level_srv gen_level_server = {
    .set = set_by_level_state,
    .level_state.current_level = -0x8000,    // -0x8000: defult value
};
static struct bt_mesh_model_pub gen_level_srv_pub;

ble_mesh_light_ctl_temp_srv_t light_ctl_temp_srv = {
    .state = &light_ctl_state,
    .set = ble_mesh_light_ctl_set_cb,
};

static struct bt_mesh_model_pub light_ctl_temp_srv_pub;

/* 灯控状态变化 */
static struct bt_mesh_gen_dtt_srv gen_dtt_server = {
    .set = ble_mesh_dtt_set_cb,
};
static struct bt_mesh_model_pub gen_dtt_srv_pub;
#endif

// light ctl client
#if defined(CONFIG_BLE_MESH_LIGHT_CTL_CLIENT) && (CONFIG_BLE_MESH_LIGHT_CTL_CLIENT == 1)
static struct bt_mesh_gen_model_cli lightness_client;
static struct bt_mesh_model_pub lightness_pub_cli;

static struct bt_mesh_gen_model_cli light_ctl_client;
static struct bt_mesh_model_pub light_ctl_pub_cli;

static struct bt_mesh_gen_model_cli gen_level_client;
static struct bt_mesh_model_pub gen_level_pub_cli;
#endif

static struct bt_mesh_cfg_cli cfg_cli = {};

#if defined(CONFIG_BLE_MESH_PRIV_BEACON_CLI) && (CONFIG_BLE_MESH_PRIV_BEACON_CLI == 1)
static struct bt_mesh_priv_beacon_cli_t priv_beacon_cli = {};
#endif

#if defined(CONFIG_BLE_MESH_DFD_CTL_CLIENT) && (CONFIG_BLE_MESH_DFD_CTL_CLIENT == 1)
static const struct bt_mesh_dfd_client_cb_t dfd_cli_cb = {0};
static struct bt_mesh_dfd_client_t dfd_cli = BT_MESH_DFD_CLI_INIT(&dfd_cli_cb);
#endif

#if defined(CONFIG_BLE_MESH_LIGHT_CTL_CLIENT) && (CONFIG_BLE_MESH_LIGHT_CTL_CLIENT == 1)
static struct bt_mesh_gen_model_cli gen_dtt_client;
static struct bt_mesh_model_pub gen_dtt_pub_cli;
#endif

// time
#if defined(CONFIG_BLE_MESH_TIME_CLIENT) && (CONFIG_BLE_MESH_TIME_CLIENT == 1)
static struct bt_mesh_gen_model_cli time_client;
static struct bt_mesh_model_pub time_pub_cli;
#endif

#if defined(CONFIG_BLE_MESH_TIME_SERVER) && (CONFIG_BLE_MESH_TIME_SERVER == 1)
struct bt_mesh_time_state_t time_state = { 0 };

struct bt_mesh_time_srv_t time_srv = {
    .state = &time_state,
};

struct bt_mesh_time_setup_srv_t time_setup_srv = {
    .state = &time_state,
};

static struct bt_mesh_model_pub time_srv_pub;
static struct bt_mesh_model_pub time_setup_srv_pub;
#endif

// scene
#if defined(CONFIG_BLE_MESH_SCENE_CLIENT) && (CONFIG_BLE_MESH_SCENE_CLIENT == 1)
static struct bt_mesh_gen_model_cli scene_client;
static struct bt_mesh_model_pub scene_pub_cli;
#endif

#if defined(CONFIG_BLE_MESH_SCENE_SERVER) && (CONFIG_BLE_MESH_SCENE_SERVER == 1)
struct scene_register scenes_reg[SCENE_COUNT_MAX] = { 0 };

struct bt_mesh_scenes_state scene_state = {
    .scene_count = SCENE_COUNT_MAX,
    .scenes = scenes_reg,
    .current_scene = 0,
    .target_scene = 0,
    .status_code = 0,
    .in_progress = false,
};

struct bt_mesh_scene_srv scene_srv = {
    .state = &scene_state,
    .set = ble_mesh_scene_set_cb,
};

struct bt_mesh_scene_setup_srv scene_setup_srv = {
    .state = &scene_state,
};

static struct bt_mesh_model_pub scene_srv_pub;
static struct bt_mesh_model_pub scene_setup_srv_pub;
#endif

// scheduler
#if defined(CONFIG_BLE_MESH_SCHEDULER_CLIENT) && (CONFIG_BLE_MESH_SCHEDULER_CLIENT == 1)
static struct bt_mesh_gen_model_cli scheduler_client;
static struct bt_mesh_model_pub scheduler_pub_cli;
#endif

#if defined(CONFIG_BLE_MESH_SCHEDULER_SERVER) && (CONFIG_BLE_MESH_SCHEDULER_SERVER == 1)
struct schedule_register_t schedule_reg[SCHEDULER_COUNT_MAX] = { 0 };

struct bt_mesh_scheduler_state_t scheduler_state = {
    .schedule_count = SCHEDULER_COUNT_MAX,
    .schedules = schedule_reg,
};

struct bt_mesh_scheduler_srv_t scheduler_srv = {
    .state = &scheduler_state,
};

struct bt_mesh_scheduler_setup_srv_t scheduler_setup_srv = {
    .state = &scheduler_state,
    .set = ble_mesh_scheduler_set_cb,
};

static struct bt_mesh_model_pub scheduler_srv_pub;
static struct bt_mesh_model_pub scheduler_setup_srv_pub;
#endif

#if defined(CONFIG_BLE_MESH_MODEL_RMT_PROV_CLIENT) && (CONFIG_BLE_MESH_MODEL_RMT_PROV_CLIENT == 1)
struct bt_mesh_model_rmt_prov_cli rmt_prov_cli;
struct bt_mesh_model_pub rmt_prov_pub_cli;
#endif

#if defined(CONFIG_BLE_MESH_MODEL_RMT_PROV_SERVER) && (CONFIG_BLE_MESH_MODEL_RMT_PROV_SERVER == 1)
struct bt_mesh_rmt_prov_srv_t rmt_prov_srv = {};
#endif

static struct bt_mesh_model root_models[] = {
    BT_MESH_MODEL_CFG_SRV,
    BT_MESH_MODEL_CFG_CLI(&cfg_cli),

/* 通用开关服务端模型 */
#if defined(CONFIG_BLE_MESH_ONOFF_SERVER) && (CONFIG_BLE_MESH_ONOFF_SERVER == 1)
    BT_MESH_MODEL_GEN_ONOFF_SRV(&gen_onoff_server, &gen_onoff_srv_pub),
#endif

/* 通用开关客户端模型 */
#if defined(CONFIG_BLE_MESH_ONOFF_CLIENT) && (CONFIG_BLE_MESH_ONOFF_CLIENT == 1)
    BT_MESH_MODEL_GEN_ONOFF_CLI(&gen_onoff_client, &gen_onoff_pub_cli),
#endif

/* 灯控服务端模型 */
#if defined(CONFIG_BLE_MESH_LIGHT_CTL_SERVER) && (CONFIG_BLE_MESH_LIGHT_CTL_SERVER == 1)
    BT_MESH_MODEL_LIGHT_LIGHTNESS_SRV(&lightness_server, &light_lightness_srv_pub),
    BT_MESH_MODEL_LIGHT_LIGHTNESS_SETUP_SRV(&lightness_setup_server, &light_lightness_srv_setup_pub),
    BT_MESH_MODEL_LIGHT_CTL_SRV(&light_ctl_srv, &light_ctl_srv_pub),
    BT_MESH_MODEL_LIGHT_CTL_SETUP_SRV(&light_ctl_setup_srv, &light_ctl_setup_srv_pub),
    BT_MESH_MODEL_GEN_TRANS_TIME_SRV(&gen_dtt_server, &gen_dtt_srv_pub),
    BT_MESH_MODEL_GEN_LEVEL_SRV(&gen_level_server, &gen_level_srv_pub),
    BT_MESH_MODEL_LIGHT_CTL_TEMP_SRV(&light_ctl_temp_srv, &light_ctl_temp_srv_pub),
#endif

/* 私有Beacon服务端模型 */
#if defined(CONFIG_BLE_MESH_PRIV_BEACON_SRV) && (CONFIG_BLE_MESH_PRIV_BEACON_SRV == 1)
    BT_MESH_MODEL_PRIV_BEACON_SRV,
#endif

/* 私有Beacon客户端模型 */
#if defined(CONFIG_BLE_MESH_PRIV_BEACON_CLI) && (CONFIG_BLE_MESH_PRIV_BEACON_CLI == 1)
    BT_MESH_MODEL_PRIV_BEACON_CLI(&priv_beacon_cli),
#endif

/* 灯控客户端模型 */
#if defined(CONFIG_BLE_MESH_LIGHT_CTL_CLIENT) && (CONFIG_BLE_MESH_LIGHT_CTL_CLIENT == 1)
    BT_MESH_MODEL_LIGHT_LIGHTNESS_CLI(&lightness_client, &lightness_pub_cli),
    BT_MESH_MODEL_LIGHT_CTL_CLI(&light_ctl_client, &light_ctl_pub_cli),
    BT_MESH_MODEL_GEN_LEVEL_CLI(&gen_level_client, &gen_level_pub_cli),
    BT_MESH_MODEL_GEN_DEF_TRANS_TIME_CLI(&gen_dtt_client, &gen_dtt_pub_cli),
#endif

/* 健康检服务端模型 */
#if defined(CONFIG_BLE_MESH_MODEL_HEALTH_SERVER) && (CONFIG_BLE_MESH_MODEL_HEALTH_SERVER == 1)
    BT_MESH_MODEL_HEALTH_SRV(&health_srv, &health_pub_srv),
#endif

/* 健康检查客户端模型 */
#if defined(CONFIG_BLE_MESH_MODEL_HEALTH_CLIENT) && (CONFIG_BLE_MESH_MODEL_HEALTH_CLIENT == 1)
    BT_MESH_MODEL_HEALTH_CLI(&health_cli, &health_pub_cli),
#endif

/* 时间服务端模型 */
#if defined(CONFIG_BLE_MESH_TIME_SERVER) && (CONFIG_BLE_MESH_TIME_SERVER == 1)
    BT_MESH_MODEL_TIME_SRV(&time_srv, &time_srv_pub),
    BT_MESH_MODEL_TIME_SETUP_SRV(&time_setup_srv, &time_setup_srv_pub),
#endif

/* 时间客户端模型 */
#if defined(CONFIG_BLE_MESH_TIME_CLIENT) && (CONFIG_BLE_MESH_TIME_CLIENT == 1)
    BT_MESH_MODEL_TIME_CLI(&time_client, &time_pub_cli),
#endif

/* 场景服务端模型 */
#if defined(CONFIG_BLE_MESH_SCENE_SERVER) && (CONFIG_BLE_MESH_SCENE_SERVER == 1)
    BT_MESH_MODEL_SCENE_SRV(&scene_srv, &scene_srv_pub),
    BT_MESH_MODEL_SCENE_SET_SRV(&scene_setup_srv, &scene_setup_srv_pub),
#endif

/* 场景客户端模型 */
#if defined(CONFIG_BLE_MESH_SCENE_CLIENT) && (CONFIG_BLE_MESH_SCENE_CLIENT == 1)
    BT_MESH_MODEL_SCENE_CLI(&scene_client, &scene_pub_cli),
#endif

/* 调度器服务端模型 */
#if defined(CONFIG_BLE_MESH_SCHEDULER_SERVER) && (CONFIG_BLE_MESH_SCHEDULER_SERVER == 1)
    BT_MESH_MODEL_SCHEDULER_SRV(&scheduler_srv, &scheduler_srv_pub),
    BT_MESH_MODEL_SCHEDULER_SETUP_SRV(&scheduler_setup_srv, &scheduler_setup_srv_pub),
#endif

/* 调度器客户端模型 */
#if defined(CONFIG_BLE_MESH_SCHEDULER_CLIENT) && (CONFIG_BLE_MESH_SCHEDULER_CLIENT == 1)
    BT_MESH_MODEL_SCHEDULER_CLI(&scheduler_client, &scheduler_pub_cli),
#endif

/* 升级控制客户端模型 */
#if defined(CONFIG_BLE_MESH_DFU_CTL_SERVER) && (CONFIG_BLE_MESH_DFU_CTL_SERVER == 1)
    BT_MESH_MODEL_DFU_SRV(),
#endif

/* 固件分发服务模型 */
#if defined(CONFIG_BLE_MESH_DFD_CTL_SERVER) && (CONFIG_BLE_MESH_DFD_CTL_SERVER == 1)
    BT_MESH_MODEL_DFD_SRV(),
#endif

/* 固件分发客户端模型 */
#if defined(CONFIG_BLE_MESH_DFD_CTL_CLIENT) && (CONFIG_BLE_MESH_DFD_CTL_CLIENT == 1)
    BT_MESH_MODEL_DFD_CLI(&dfd_cli),
#endif

/* 远程配网服务端模型注册 */
#if defined(CONFIG_BLE_MESH_MODEL_RMT_PROV_SERVER) && (CONFIG_BLE_MESH_MODEL_RMT_PROV_SERVER == 1)
    BT_MESH_MODEL_RMT_PROV_SRV(&rmt_prov_srv),
#endif

/* 远程配网客户端模型注册 */
#if defined(CONFIG_BLE_MESH_MODEL_RMT_PROV_CLIENT) && (CONFIG_BLE_MESH_MODEL_RMT_PROV_CLIENT == 1)
    BT_MESH_MODEL_RMT_PROV_CLI(&rmt_prov_cli, &rmt_prov_pub_cli),
#endif
};

#if defined(CONFIG_BLE_MESH_LIGHT_CTL_SERVER) && (CONFIG_BLE_MESH_LIGHT_CTL_SERVER == 1)
struct bt_mesh_model temp_models[] = {
};
#endif

#if defined(CONFIG_BLE_MESH_VND_MODEL_CLIENT) && (CONFIG_BLE_MESH_VND_MODEL_CLIENT == 1)
static struct bt_mesh_gen_model_cli hw_vendor_client;
static struct bt_mesh_model_pub hw_vendor_cli_pub;
#endif

#if defined(CONFIG_BLE_MESH_VND_MODEL_SERVER) && (CONFIG_BLE_MESH_VND_MODEL_SERVER == 1)
static struct bt_mesh_model_pub hw_vendor_srv_pub;
#endif
/**
 * @brief 用户自定义模型
 *
 */
static struct bt_mesh_model vnd_models[] = {
#if defined(CONFIG_BLE_MESH_VND_MODEL_CLIENT) && (CONFIG_BLE_MESH_VND_MODEL_CLIENT == 1)
    HW_MESH_MODEL_VND_ATTR_CLI(&hw_vendor_client, &hw_vendor_cli_pub),
#endif
#if defined(CONFIG_BLE_MESH_VND_MODEL_SERVER) && (CONFIG_BLE_MESH_VND_MODEL_SERVER == 1)
    HW_MESH_MODEL_VND_ATTR_SRV(&hw_vendor_srv_pub),
#endif
};

static struct bt_mesh_elem elements[] = {
    BT_MESH_ELEM(0, root_models, vnd_models),
#if defined(CONFIG_BLE_MESH_LIGHT_CTL_SERVER) && (CONFIG_BLE_MESH_LIGHT_CTL_SERVER == 1)
    BT_MESH_ELEM(0, temp_models, BT_MESH_MODEL_NONE),
#endif
};

static struct bt_mesh_comp composition = {
    .cid = MYNEWT_VAL_BLE_MESH_COMP_CID,
    .pid = MYNEWT_VAL_BLE_MESH_COMP_PID,
    .vid = MYNEWT_VAL_BLE_MESH_COMP_VID,
    .elem = elements,
    .elem_count = ARRAY_SIZE(elements),
};

void ble_mesh_model_init(void)
{
#if defined(CONFIG_BLE_MESH_LIGHT_CTL_SERVER) && (CONFIG_BLE_MESH_LIGHT_CTL_SERVER == 1)
    light_lightness_srv_pub.msg = NET_BUF_SIMPLE(MODEL_LIGHTNESS_PUB_MSG_LEN);
    light_lightness_srv_setup_pub.msg = NET_BUF_SIMPLE(MODEL_LIGHTNESS_PUB_MSG_LEN);
    light_ctl_srv_pub.msg = NET_BUF_SIMPLE(MODEL_LIGHT_CTL_PUB_MSG_LEN);
    light_ctl_setup_srv_pub.msg = NET_BUF_SIMPLE(MODEL_LIGHT_CTL_PUB_MSG_LEN);
    light_ctl_temp_srv_pub.msg = NET_BUF_SIMPLE(MODEL_LIGHT_CTL_TEMP_PUB_MSG_LEN);
    gen_dtt_srv_pub.msg = NET_BUF_SIMPLE(MODEL_DFT_TRS_TIME_PUB_MSG_LEN);
#endif

#if defined(CONFIG_BLE_MESH_LIGHT_CTL_CLIENT) && (CONFIG_BLE_MESH_LIGHT_CTL_CLIENT == 1)
    lightness_pub_cli.msg = NET_BUF_SIMPLE(MODEL_LIGHTNESS_PUB_MSG_LEN);
    light_ctl_pub_cli.msg = NET_BUF_SIMPLE(MODEL_LIGHT_CTL_PUB_MSG_LEN);
    gen_level_pub_cli.msg = NET_BUF_SIMPLE(MODEL_LEVEL_PUB_MSG_LEN);
    gen_dtt_pub_cli.msg = NET_BUF_SIMPLE(MODEL_DFT_TRS_TIME_PUB_MSG_LEN);
#endif

#if defined(CONFIG_BLE_MESH_MODEL_HEALTH_CLIENT) && (CONFIG_BLE_MESH_MODEL_HEALTH_CLIENT == 1)
    health_pub_cli.msg = NET_BUF_SIMPLE(MODEL_HEALTH_PUB_MSG_LEN);
#endif

#if defined(CONFIG_BLE_MESH_MODEL_HEALTH_SERVER) && (CONFIG_BLE_MESH_MODEL_HEALTH_SERVER == 1)
    health_pub_srv.msg = BT_MESH_HEALTH_FAULT_MSG(0);
#endif

#if defined(CONFIG_BLE_MESH_TIME_CLIENT) && (CONFIG_BLE_MESH_TIME_CLIENT == 1)
    time_pub_cli.msg = NET_BUF_SIMPLE(MODEL_TIME_PUB_MSG_LEN);
#endif

#if defined(CONFIG_BLE_MESH_TIME_SERVER) && (CONFIG_BLE_MESH_TIME_SERVER == 1)
    time_srv_pub.msg = NET_BUF_SIMPLE(MODEL_TIME_PUB_MSG_LEN);
    time_setup_srv_pub.msg = NET_BUF_SIMPLE(MODEL_TIME_PUB_MSG_LEN);
#endif

#if defined(CONFIG_BLE_MESH_SCENE_CLIENT) && (CONFIG_BLE_MESH_SCENE_CLIENT == 1)
    scene_pub_cli.msg = NET_BUF_SIMPLE(MODEL_SCENE_PUB_MSG_LEN);
#endif

#if defined(CONFIG_BLE_MESH_SCENE_SERVER) && (CONFIG_BLE_MESH_SCENE_SERVER == 1)
    scene_srv_pub.msg = NET_BUF_SIMPLE(MODEL_SCENE_PUB_MSG_LEN);
    scene_setup_srv_pub.msg = NET_BUF_SIMPLE(MODEL_SCENE_PUB_MSG_LEN);
#endif

#if defined(CONFIG_BLE_MESH_SCHEDULER_CLIENT) && (CONFIG_BLE_MESH_SCHEDULER_CLIENT == 1)
    scheduler_pub_cli.msg = NET_BUF_SIMPLE(MODEL_SCHEDULER_PUB_MSG_LEN);
#endif

#if defined(CONFIG_BLE_MESH_SCHEDULER_SERVER) && (CONFIG_BLE_MESH_SCHEDULER_SERVER == 1)
    scheduler_srv_pub.msg = NET_BUF_SIMPLE(MODEL_SCHEDULER_PUB_MSG_LEN);
    scheduler_setup_srv_pub.msg = NET_BUF_SIMPLE(MODEL_SCHEDULER_PUB_MSG_LEN);
#endif

#if defined(CONFIG_BLE_MESH_VND_MODEL_SERVER) && (CONFIG_BLE_MESH_VND_MODEL_SERVER == 1)
    hw_vendor_srv_pub.msg = NET_BUF_SIMPLE(MODEL_VND_PUB_MSG_LEN);
#endif
}

const struct bt_mesh_comp *ble_mesh_get_comp(void)
{
    return &composition;
}

errcode_t ble_mesh_mod_get_composition(
    const ble_mesh_prov_net_info_t *net_info,
    ble_mesh_composition_callback func,
    uint8_t *status)
{
    ble_mesh_net_buff_t net_buff = {0};
    if (net_info == NULL || status == NULL) {
        BT_ERR("net_info or state is NULL")
        return -EINVAL;
    }
    struct os_mbuf *comp = NET_BUF_SIMPLE(COMP_DATA_MAX_LEN);
    if (!comp) {
        BT_ERR("alloc memery err");
        return -EINVAL;
    }
    int err = bt_mesh_cfg_comp_data_get(net_info->net_idx, net_info->addr, COMP_DATA_PAGE_0, status, comp);
    if (err != 0) {
        BT_INFO("Getting composition failed err %d\r\n", err);
        goto done;
    }
    if (*status != 0x00) {
        BT_INFO("Got non-success status 0x%02x\r\n", *status);
        goto done;
    }
    if (func != NULL) {
        net_buff.payload = comp->om_data;
        net_buff.len = comp->om_len;
        net_buff.off = 0;
        func(&net_buff);
    }
    err = ERRCODE_BT_SUCCESS;
done:
    os_mbuf_free_chain(comp);
    return err;
}

void ble_mesh_mod_update_vid(uint16_t vid)
{
    composition.vid = vid;
    return;
}
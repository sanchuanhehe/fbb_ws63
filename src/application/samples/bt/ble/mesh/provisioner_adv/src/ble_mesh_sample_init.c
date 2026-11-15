/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: BLE MESH Provisioner Adv Source.
 *
 * History:
 * 2024-12-23, Create file.
 */

#ifdef CONFIG_BLE_MESH_PROVISIONER_ADV
#include <stdbool.h>
#include <stdint.h>
#include "app_init.h"
#include "soc_osal.h"
#include "osal_task.h"
#include "osal_addr.h"

#include "bts_def.h"
#include "bts_le_gap.h"
#include "ble_mesh_common.h"
#include "ble_mesh_model.h"
#include "ble_mesh_prov.h"
#include "ble_mesh_manager.h"
#include "ble_mesh_nv_op.h"
#include "ble_mesh_sample.h"

#define CID_HIS 0x010F  // HiSilicon

#define BLE_ADDR_RANDOM (0x01)

static uint8_t dev_uuid[BLE_MESH_UUID_LEN] = {0xdd, 0xdd};
static uint8_t static_auth_value[BLE_MESH_STATIC_AUTH_LEN] = {0};  // 16 static auth value 长度

#define PROV_OWN_ADDR 0x0001

#define BLE_MESH_SUCC 0
static void prov_reset(void);
static void prov_complete(void);
static void link_close(ble_mesh_prov_bearer_t bearer);
static void provisioner_complete(uint16_t net_idx, uint16_t addr);
static void recv_rmt_unprovisioned_beacon(uint8_t uuid[BLE_MESH_UUID_LEN], uint8_t oob_info, uint32_t *uri_hash);
static void recv_unprovisioned_beacon(uint8_t uuid[BLE_MESH_UUID_LEN], uint8_t oob_info, uint32_t *uri_hash);
uint16_t g_prov_alloc_addr = 0x0002;
uint16_t g_net_idx = 0;
uint16_t g_net_addr = 0;
uint16_t g_app_idx = 0;
uint8_t g_app_key[BLE_MESH_APP_KEY_LEN];
ble_mesh_prov_net_info_t g_net_info = {0};

#define COMP_DATA_MIN_LEN 4
#define SIG_MODEL_DATA_LEN 2
#define VENDOR_MODEL_DATA_LEN 4

#define BLE_MESH_PID_LEN 4

typedef struct ble_mesh_sample_tuple {
    uint8_t dev_uuid[BLE_MESH_UUID_LEN];
    bd_addr_t addr;
    uint8_t pid[BLE_MESH_PID_LEN];
    uint8_t auth_info[BLE_MESH_STATIC_AUTH_LEN];
} ble_mesh_sample_tuple_t;

static ble_mesh_sample_tuple_t g_tuple = {0};

// log macro define
#define SAMPLE_INFO(fmt, args...) osal_printk("[info] %s: " fmt "\n", __func__, ##args)
#define SAMPLE_DBG(fmt, args...) osal_printk("[debug] %s: " fmt "\n", __func__, ##args)
#define SAMPLE_ERR(fmt, args...) osal_printk("[error] %s: " fmt "\n", __func__, ##args)

static void link_close(ble_mesh_prov_bearer_t bearer)
{
    SAMPLE_INFO("link_close\r\n");
}

static void prov_reset(void)
{
    SAMPLE_INFO("prov_reset\r\n");
    bt_mesh_scan_enable();
}

static void recv_unprovisioned_beacon(uint8_t uuid[BLE_MESH_UUID_LEN], uint8_t oob_info, uint32_t *uri_hash)
{
    int err;
    uint8_t attention_duration = 0;

    SAMPLE_INFO("recv_unprovisioned_beacon dev uuid:%s\r\n", bt_hex(uuid, BLE_MESH_UUID_LEN));
    err = ble_mesh_pb_adv_open(uuid, g_net_idx, g_prov_alloc_addr);
    if (err != BLE_MESH_SUCC) {
        SAMPLE_ERR("recv_unprovisioned_beacon failed (err %d)\r\n", err);
    }
}

static void provisioner_complete(uint16_t net_idx, uint16_t addr)
{
    g_net_addr = addr;
    g_net_idx = net_idx;
    SAMPLE_INFO("provisioner_complete: net_idx=0x%04x, addr=0x%04x\r\n", net_idx, addr);
}

static void node_added(uint16_t net_idx, uint8_t uuid[BLE_MESH_UUID_LEN], uint16_t addr, uint8_t num_elem)
{
    SAMPLE_INFO("node_added uuid:%s addr:0x%04x num_elem:%d\r\n", bt_hex(uuid, BLE_MESH_UUID_LEN), addr, num_elem);
    g_prov_alloc_addr += num_elem;
    g_net_addr = addr;
    g_net_idx = net_idx;
}

static void prov_complete(void)
{
    int err = 0;
    uint8_t status = 0;
    g_net_info.net_idx = g_net_idx, g_net_info.addr = g_net_addr, g_net_info.elem_addr = g_net_addr,
    g_net_info.key_net_idx = g_net_idx, g_net_info.key_app_idx = g_app_idx,
    memcpy_s(g_net_info.app_key, BLE_MESH_APP_KEY_LEN, g_app_key, BLE_MESH_APP_KEY_LEN);

    // 添加密钥
    err = ble_mesh_cfg_add_app_key(&g_net_info, &status);
    SAMPLE_INFO("add app key ret: %d\r\n", err);
}

/* Disable OOB security for SILabs Android app */
static ble_mesh_prov_t provision = {
    .unprovisioned_beacon_recv = recv_unprovisioned_beacon,
    .provisioned_complete = provisioner_complete,
    .rmt_unprovisioned_beacon_recv = recv_rmt_unprovisioned_beacon,
    .link_close = link_close,
    .reset = prov_reset,
    .node_added = node_added,
};

static void ble_mesh_get_dev_uuid(void)
{
    bd_addr_t addr;
    errcode_t ret = gap_ble_get_local_addr(&addr);
    if (ret != ERRCODE_BT_SUCCESS) {
        SAMPLE_ERR("gap_ble_get_local_addr faile 0x%x.\r\n", ret);
        return;
    }

    memcpy_s(dev_uuid, BD_ADDR_LEN, addr.addr, BD_ADDR_LEN);
}

/*   远程扫描启动接口
 *   uuid 当uuid有效时，扫描指定uuid；当uuid为NULL时，扫描所有未配网设备
 */
int ble_mesh_sample_rmt_prov_scan_start(
    uint16_t net_idx, uint16_t addr, const uint8_t uuid[BLE_MESH_UUID_LEN], uint8_t items_limit, uint8_t timeout)
{
    return ble_mesh_pb_start_rmt_scan(net_idx, addr, uuid, items_limit, timeout);
}

/*   远程扩展扫描启动接口
 *   uuid 当uuid有效时，扫描指定uuid；当uuid为NULL时，扫描server端信息
 *   ad_filter为rmt_prov_ex_scan_t格式
 */
int ble_mesh_sample_rmt_prov_ex_scan_start(uint16_t net_idx, uint16_t addr, const uint8_t *uuid,
                                           uint8_t timeout, ble_mesh_rmt_prov_ex_scan_t *ad_filter)
{
    return ble_mesh_pb_start_rmt_ex_scan(net_idx, addr, uuid, timeout, ad_filter);
}

/*   远程配网启动接口
 *   uuid 当uuid有效时，启动远程配网流程；当uuid为NULL时，启动node refresh程序
 *   refresh 0 秘钥刷新 1 节点地址刷新 2 节点组成刷新
 *   rmt_addr 配网服务器地址
 */
int ble_mesh_sample_rmt_prov_link_open(
    const uint8_t uuid[BLE_MESH_UUID_LEN], uint16_t net_idx, uint16_t rmt_addr, uint8_t refresh)
{
    return ble_mesh_pb_rmt_adv_open(uuid, net_idx, rmt_addr, g_prov_alloc_addr, refresh);
}

// 保存扫描到设备
static void recv_rmt_unprovisioned_beacon(uint8_t uuid[BLE_MESH_UUID_LEN], uint8_t oob_info, uint32_t *uri_hash)
{
    SAMPLE_INFO("recv_unprovisioned_beacon dev uuid, %s", bt_hex(uuid, BLE_MESH_UUID_LEN));
    SAMPLE_INFO("OOB Information: 0x%02x", oob_info);
    if (uri_hash) {
        SAMPLE_INFO("URI Hash: 0x%08x", *uri_hash);
    } else {
        SAMPLE_INFO("URI Hash: (null)");
    }
}

static void ble_mesh_sample_load_nv(void)
{
    SAMPLE_INFO("recover addr:0x%02x:*:*:%02x:%02x", g_tuple.addr.addr[0], g_tuple.addr.addr[4], g_tuple.addr.addr[5]);
    errcode_t ret = gap_ble_set_local_addr(&g_tuple.addr);
    if (ret != ERRCODE_BT_SUCCESS) {
        SAMPLE_ERR("set local addr failed, ret:0x%x.", ret);
        return;
    }
    return;
}


static uint32_t ble_mesh_get_auth_string(const bd_addr_t *addr, const char *pid, const char *auth)
{
    static char auth_value_string[128] = {0};  // 128 鉴权字符串最大长度
    static char sha256[32] = {0};              // 32 sha256加密后的长度

    int sec_ret = sprintf_s(auth_value_string,
        sizeof(auth_value_string),
        "%s,%02x%02x%02x%02x%02x%02x,%s",
        pid,
        addr->addr[0],
        addr->addr[1],
        addr->addr[2],  // 0 1 2 3 4 5 6 mac地址
        addr->addr[3],
        addr->addr[4],
        addr->addr[5],  // 0 1 2 3 4 5 6 mac地址
        auth);
    if (sec_ret == -1) {
        SAMPLE_ERR("sprintf auth fail.");
        return ERRCODE_BT_FAIL;
    }

    mbedtls_sha256((const unsigned char *)auth_value_string, strlen(auth_value_string), sha256, 0);
    memcpy_s(static_auth_value, sizeof(static_auth_value), sha256, sizeof(static_auth_value));
    provision.static_value_len = sizeof(static_auth_value);
    provision.static_auth_value = static_auth_value;
    return ERRCODE_BT_SUCCESS;
}

uint32_t ble_mesh_sample_set_auth_value(const char *pid, const char *auth_str)
{
    SAMPLE_INFO("ble device auth set pid:%s auth:%s end!\r\n", pid, auth_str);
    bd_addr_t addr = {0};
    errcode_t ret = gap_ble_get_local_addr(&addr);
    return ble_mesh_get_auth_string(&addr, pid, auth_str);
}

uint32_t ble_mesh_sample_set_static_oob_value(const uint8_t *static_val, uint8_t size)
{
    SAMPLE_INFO("ble provisioner static oob set:%s end!\r\n", bt_hex(static_val, size));
    return ble_mesh_set_static_oob(static_val, size);
}

uint32_t ble_mesh_sample_set_remote_pub_key(const uint8_t *key, uint8_t pub_key_len)
{
    SAMPLE_INFO("ble provisioner remote public key set:\n%s\n end!\r\n", bt_hex(key, pub_key_len)); // 64: key len
    return ble_mesh_set_remote_public_key(key);
}

uint32_t ble_mesh_sample_set_input_action(uint8_t action, uint8_t size)
{
    SAMPLE_INFO("ble provisioner input action set, action: 0x%02x, size: %d\r\n", action, size);
    return ble_mesh_set_input_action(action, size);
}

uint32_t ble_mesh_sample_set_output_action(uint8_t action, uint8_t size)
{
    SAMPLE_INFO("ble provisioner output action set, action: 0x%02x, size: %d\r\n", action, size);
    return ble_mesh_set_output_action(action, size);
}

errcode_t ble_mesh_sample_disable(void)
{
    errcode_t ret = ERRCODE_BT_SUCCESS;
    ret = ble_mesh_disable();
    if (ret != BLE_MESH_SUCC) {
        SAMPLE_INFO("Failed to disable mesh node (err %d)", ret);
        return ret;
    }
    ble_mesh_provisioner_disable();

    return ret;
}

errcode_t ble_mesh_sample_enable(ble_mesh_prov_bearer_t bearers)
{
    SAMPLE_INFO("ble mesh provisioner init start.\r\n");
    errcode_t ret = ERRCODE_BT_SUCCESS;
    uint16_t real_len = 0;
    static ble_mesh_sample_tuple_t g_tuple_tmp = {0};
    if (!ble_is_enable()) {
        SAMPLE_ERR("ble not enable.");
        return ERRCODE_BT_FAIL;
    }

    if (memcmp(&g_tuple_tmp, &g_tuple, sizeof(ble_mesh_sample_tuple_t)) == 0) {
        // NV 没有加载重新加载一次
        // 从nv读取uuid和4元组信息,如果NV恢复失败，暂不使能mesh协议栈
        ret = ble_mesh_data_read(BTH_BLE_MESH_TUPLE_NV_ID, sizeof(ble_mesh_sample_tuple_t), &real_len, &g_tuple);
        if (ret != ERRCODE_BT_SUCCESS) {
            SAMPLE_ERR("nv recover tuple info failed, ret:0x%x.", ret);
        } else {
            ble_mesh_sample_load_nv();
        }
    }
    
    ble_mesh_get_dev_uuid();

    ret = ble_mesh_enable(dev_uuid, &provision, bearers);
    if (ret != BLE_MESH_SUCC) {
        SAMPLE_INFO("Failed to enable mesh node (err %d)", ret);
        return ret;
    }
    ret = ble_mesh_provisioner_enable(PROV_OWN_ADDR, g_net_idx, 0, 0);
    if (ret != BLE_MESH_SUCC) {
        SAMPLE_INFO("Failed to create net (err %d)\r\n", ret);
    }
    bt_rand(g_app_key, sizeof(g_app_key));

    SAMPLE_INFO("ble onoff client mesh_sample_init end!\r\n");
    return ret;
}

uint32_t ble_mesh_sample_init(void)
{
    SAMPLE_INFO("ble_mesh_sample_init enter.");
    static uint8_t retry_times = 20;  // 20 ble enable retry times
    uint16_t real_len = 0;
    errcode_t ret = ERRCODE_BT_SUCCESS;
    if (!ble_is_enable()) {
        ret = enable_ble();
        if (ret != ERRCODE_BT_SUCCESS) {
            SAMPLE_ERR("ble enable failed, ret:0x%x.", ret);
            return ret;
        }
        /* 等待ble初始化完成 */
        while (!ble_is_enable() && retry_times > 0) {
            (void)osal_msleep(100); /* 1: 等待100 ms后判断状态 */
            SAMPLE_ERR("ble enable wait:%d.", retry_times--);
        }
    }

    if (!ble_is_enable()) {
        SAMPLE_ERR("ble not enable.");
        return ERRCODE_BT_FAIL;
    }
    // 从nv读取uuid和4元组信息,如果NV恢复失败，暂不使能mesh协议栈
    ret = ble_mesh_data_read(BTH_BLE_MESH_TUPLE_NV_ID, sizeof(ble_mesh_sample_tuple_t), &real_len, &g_tuple);
    if (ret != ERRCODE_BT_SUCCESS) {
        SAMPLE_ERR("nv recover tuple info failed, ret:0x%x.", ret);
        return ret;
    }

    ble_mesh_sample_load_nv();

    return ble_mesh_sample_enable(BLE_MESH_PROV_ADV | BLE_MESH_PROV_GATT);
}

static void ble_mesh_sample_entry(void)
{
    static uint8_t task_prio = 26;  // 26 任务优先级
    static uint16_t static_size = 0x2000;

    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle =
        osal_kthread_create((osal_kthread_handler)ble_mesh_sample_init, 0, "ble_mesh_sample_init", static_size);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, task_prio);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
    SAMPLE_INFO("ble_mesh_sample_entry succ.");
}

/* Run the ble_mesh_sample_entry. */
app_run(ble_mesh_sample_entry);
#endif  // CONFIG_BLE_MESH_PROVISIONER_ADV
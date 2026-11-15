/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Initializing the OnOff Client sample.
 *
 * History:
 * 2024-6-21, Create file.
 */
#include <stdint.h>
#include "soc_osal.h"
#include "osal_task.h"
#include "osal_addr.h"
#include "osal_debug.h"
#include "app_init.h"
#include "bts_def.h"
#include "bts_le_gap.h"
#include "ble_mesh_model.h"
#include "ble_mesh_prov.h"
#include "ble_mesh_manager.h"
#include "ble_mesh_nv_op.h"
#include "ble_mesh_hex.h"
#include "ble_mesh_sample.h"

#define BLE_ADDR_RANDOM (0x01)
#define LED_OFF 0
#define FAULT_ARR_SIZE 2

#define MODEL_ONOFF_PUB_MSG_LEN 4
#define MODEL_HEALTH_PUB_MSG_LEN 4

#define BLE_MESH_SUCC 0
#define MAX_AUTH_STRING_LEN 128
#define SHA_256_LEN 32
#define BLE_MESH_PID_LEN 4
#define SUPPORT_STATIC_AUTH_LEN 16

static uint8_t g_dev_uuid[BLE_MESH_UUID_LEN] = {0};
static uint8_t g_static_auth_value[BLE_MESH_STATIC_AUTH_LEN] = {0};  // 16 static auth value 长度
static const char g_uri_hash[] = "7226a27f";
static ble_mesh_prov_bearer_t g_bearers = BLE_MESH_PROV_ADV | BLE_MESH_PROV_GATT;
// log macro define
#define SAMPLE_INFO(fmt, args...) osal_printk("[info] %s: " fmt "\n", __func__, ##args)
#define SAMPLE_DBG(fmt, args...) osal_printk("[debug] %s: " fmt "\n", __func__, ##args)
#define SAMPLE_ERR(fmt, args...) osal_printk("[error] %s: " fmt "\n", __func__, ##args)

typedef struct ble_mesh_sample_tuple {
    uint8_t dev_uuid[BLE_MESH_UUID_LEN];
    bd_addr_t addr;
    uint8_t pid[BLE_MESH_PID_LEN];
    uint8_t auth_info[SUPPORT_STATIC_AUTH_LEN];
} ble_mesh_sample_tuple_t;

static ble_mesh_sample_tuple_t g_tuple = {0};

static void prov_complete(uint16_t net_idx, uint16_t addr)
{
    SAMPLE_INFO("provisioning complete for net_idx 0x%04x addr 0x%04x", net_idx, addr);
}

static void prov_reset(void)
{
    SAMPLE_INFO("prov_reset");
    errcode_t err = ble_mesh_prov_enable(g_bearers);
    if (err != BLE_MESH_SUCC) {
        SAMPLE_ERR("Failed to enable mesh node (err %d)", err);
        return;
    }
}

static struct ble_mesh_prov provision = {
    .static_value_len = 0,
    .static_auth_value = NULL,
    .provisioned_complete = prov_complete,
    .reset = prov_reset,
    .node_added = NULL,
    .uri = &g_uri_hash,
};

static uint32_t ble_mesh_auth_method_set_static_oob_value(const uint8_t *static_val, uint8_t size)
{
    if (!size || !static_val) {
        return ERRCODE_BT_FAIL;
    }
    if (memcpy_s(g_static_auth_value, sizeof(g_static_auth_value),
                 static_val, size > BLE_MESH_STATIC_AUTH_LEN ? BLE_MESH_STATIC_AUTH_LEN : size) != EOK) {
        SAMPLE_ERR("memcpy_s failed");
        return ERRCODE_BT_FAIL;
    }
    provision.static_value_len = size;
    provision.static_auth_value = g_static_auth_value;
    if (size < BLE_MESH_STATIC_AUTH_LEN) {
        if (memset_s(g_static_auth_value + size, BLE_MESH_STATIC_AUTH_LEN - size, 0, BLE_MESH_STATIC_AUTH_LEN - size) !=
            EOK) {
            SAMPLE_ERR("memcpy_s failed");
            return ERRCODE_BT_FAIL;
        }
    }

    return ERRCODE_BT_SUCCESS;
}

static void ble_mesh_get_dev_uuid(void)
{
    static uint8_t init_uuid[BLE_MESH_UUID_LEN] = {0};
    if (memcmp(init_uuid, g_dev_uuid, BLE_MESH_UUID_LEN) != 0) {
        return;
    }

    bd_addr_t addr;
    errcode_t ret = gap_ble_get_local_addr(&addr);
    if (ret != ERRCODE_BT_SUCCESS) {
        SAMPLE_ERR("gap_ble_get_local_addr faile 0x%x.", ret);
        return;
    }

    memcpy_s(g_dev_uuid, BD_ADDR_LEN, addr.addr, BD_ADDR_LEN);
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
    if (memcpy_s(g_static_auth_value, sizeof(g_static_auth_value), sha256, SUPPORT_STATIC_AUTH_LEN) != EOK) {
        SAMPLE_ERR("memcpy static value fail.");
    }

    ble_mesh_auth_method_set_static_oob_value(g_static_auth_value, SUPPORT_STATIC_AUTH_LEN);
    return ERRCODE_BT_SUCCESS;
}

static void ble_mesh_sample_set_uuid(uint8_t *uuid, uint8_t len)
{
    if (memcpy_s(g_dev_uuid, sizeof(g_dev_uuid), uuid, len) != EOK) {
        SAMPLE_ERR("ble device uuid memcpy failed, len:%d!", len);
    }
}

static void ble_mesh_sample_load_nv(void)
{
    SAMPLE_INFO("recover addr:0x%02x:*:*:%02x:%02x", g_tuple.addr.addr[0], g_tuple.addr.addr[4], g_tuple.addr.addr[5]);
    static char pid[BLE_MESH_PID_LEN * 2 + 1] = {0};                // 2 : one btye two hex chars, 1 end
    static char auth_info[SUPPORT_STATIC_AUTH_LEN * 2 + 1] = {0};  // 2 : one btye two hex chars, 1 end
    errcode_t ret = gap_ble_set_local_addr(&g_tuple.addr);
    if (ret != ERRCODE_BT_SUCCESS) {
        SAMPLE_ERR("set local addr failed, ret:0x%x.", ret);
        return;
    }

    ble_mesh_sample_set_uuid(g_tuple.dev_uuid, BLE_MESH_UUID_LEN);

    ble_mesh_bin2hex(g_tuple.pid, BLE_MESH_PID_LEN, pid, sizeof(pid));
    ble_mesh_bin2hex(g_tuple.auth_info, SUPPORT_STATIC_AUTH_LEN, auth_info, sizeof(auth_info));

    ble_mesh_get_auth_string(&g_tuple.addr, pid, auth_info);
    return;
}

uint32_t ble_mesh_sample_set_static_oob_value(const uint8_t *static_val, uint8_t size)
{
    SAMPLE_DBG("ble device static oob end!\r\n");
    return ble_mesh_auth_method_set_static_oob_value(static_val, size);
}

errcode_t ble_mesh_sample_disable(void)
{
    errcode_t ret = ERRCODE_BT_SUCCESS;
    ret = ble_mesh_disable();
    if (ret != BLE_MESH_SUCC) {
        SAMPLE_INFO("Failed to disable mesh node (err %d)", ret);
        return ret;
    }

    return ret;
}

errcode_t ble_mesh_sample_enable(ble_mesh_prov_bearer_t bearers)
{
    SAMPLE_INFO("ble device sample init start, bearers: 0x%x.\r\n", bearers);
    errcode_t ret = ERRCODE_BT_SUCCESS;
    uint16_t real_len = 0;
    static ble_mesh_sample_tuple_t g_tuple_tmp = {0};
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

    ret = ble_mesh_enable(g_dev_uuid, &provision, bearers);
    if (ret != BLE_MESH_SUCC) {
        SAMPLE_ERR("Failed to enable mesh node (err %d)", ret);
        return ret;
    }
    g_bearers = bearers;
    SAMPLE_INFO("ble device mesh_sample_init end!");
    return ERRCODE_BT_SUCCESS;
}

void ble_mesh_sample_set_uuid_value(uint8_t *uuid, uint8_t len)
{
    ble_mesh_sample_set_uuid(uuid, len);
    (void)memcpy_s(g_tuple.dev_uuid, BLE_MESH_UUID_LEN, g_dev_uuid, BLE_MESH_UUID_LEN);
    errcode_t ret = ble_mesh_data_write(BTH_BLE_MESH_TUPLE_NV_ID, &g_tuple, sizeof(g_tuple));
    if (ret != ERRCODE_BT_SUCCESS) {
        SAMPLE_ERR("ble device uuid write to nv failed, 0x%x!", ret);
    }
}

uint32_t ble_mesh_sample_set_auth_value(const char *pid, const char *auth_str)
{
    SAMPLE_INFO("ble device auth set pid:%s auth:%s end!", pid, auth_str);
    bd_addr_t addr = {0};
    errcode_t ret = gap_ble_get_local_addr(&addr);
    if (ret != ERRCODE_BT_SUCCESS) {
        SAMPLE_ERR("gap_ble_get_local_addr faile 0x%x.", ret);
        return ret;
    }

    ret = ble_mesh_get_auth_string(&addr, pid, auth_str);
    if (ret != ERRCODE_BT_SUCCESS) {
        SAMPLE_ERR("ble_mesh_get_auth_string failed, 0x:%x!", ret);
        return ret;
    }
    (void)memset_s(&g_tuple.auth_info, BLE_MESH_PID_LEN, 0x00, BLE_MESH_PID_LEN);
    ble_mesh_hex2bin(pid, g_tuple.pid, BLE_MESH_PID_LEN);

    (void)memset_s(&g_tuple.auth_info, sizeof(g_tuple.auth_info), 0x00, sizeof(g_tuple.auth_info));
    ble_mesh_hex2bin(auth_str, g_tuple.auth_info, sizeof(g_tuple.auth_info));

    g_tuple.addr.type = addr.type;
    (void)memcpy_s(g_tuple.addr.addr, BD_ADDR_LEN, addr.addr, BD_ADDR_LEN);
    ret = ble_mesh_data_write(BTH_BLE_MESH_TUPLE_NV_ID, &g_tuple, sizeof(g_tuple));
    if (ret != ERRCODE_BT_SUCCESS) {
        SAMPLE_ERR("ble device uuid write to nv failed, 0x%x!", ret);
    }
    return ret;
}

uint32_t ble_mesh_sample_init(void)
{
    SAMPLE_INFO("ble_mesh_sample_init enter.");
    static uint8_t retry_times = 20;  // ble enable retry times
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
            (void)osal_msleep(100); /* 1: 等待100ms后判断状态 */
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

    ret = ble_mesh_sample_enable(BLE_MESH_PROV_ADV);
    if (ret != ERRCODE_BT_SUCCESS) {
        SAMPLE_ERR("ble_mesh_sample_enable fail, ret:0x%x.", ret);
        return ret;
    }
    return 0;
}

static void ble_mesh_sample_entry(void)
{
    static uint8_t task_prio = 26;
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
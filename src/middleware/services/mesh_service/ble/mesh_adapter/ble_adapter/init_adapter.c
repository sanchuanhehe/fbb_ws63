/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: init adapter source
 *
 * History:
 * 2024-6-21, Create file.
 */
#include "trng.h"
#include "mesh/porting.h"
#include "adv.h"
#include "remote_prov_server.h"
#include "mesh_gatt_adapter.h"
#include "mesh_gap_adapter.h"
#include "init_adapter.h"

#define BLE_MESH_TASK_PRIO 11
#define BLE_NPL_TIME_FOREVER    INT32_MAX
#define TASK_DEFAULT_PRIORITY       1
#define TASK_DEFAULT_STACK          NULL
#define BLE_MESH_ADV_TASK_STACK_SIZE 0x1600
#define BLE_MESH_ADV_TASK_NAME "mesh_adv_task"

static osal_kthread_handler s_task_mesh_adv;

static struct bt_mesh_queue adv_queue;
static struct bt_mesh_dev bt_mesh_dev;

static future future_mesh;
extern void os_msys_init(void);
extern void os_mempool_module_init(void);
static bool adapter_initialized = false;

int ble_hs_hci_rand(void *dst, int length)
{
    int ret;
    BT_DBG("Creat random length: %d", length);
    if (dst == NULL || length == 0) {
        BT_ERR("Invalid parameter");
        return -EINVAL;
    }

    ret = uapi_drv_cipher_trng_get_random_bytes(dst, length);   // 改为平台生成随机数接口

    BT_DBG("Random %s", bt_hex(dst, length));

    return ret;
}

int bt_mesh_get_hci_version(void)
{
    return bt_mesh_dev.hci_version;
}

void mesh_deinitialized(void)
{
    bt_mesh_destroy_task();

    if (adv_queue.handle != 0xFFFFFFFF) {
        bt_mesh_queue_delete(adv_queue.handle);
        adv_queue.handle = 0xFFFFFFFF;
    }
    adapter_initialized = false;
}

void mesh_initialized(void)
{
    uint32_t ret;
    BT_INFO("mesh_initialized enter");

    ret = bt_mesh_queue_create(
        "adv_queue", MYNEWT_VAL(BLE_MESH_ADV_BUF_COUNT) + 1, &(adv_queue.handle), sizeof(bt_mesh_msg_t));
    if (ret != 0) {
        BT_ERR("Failed to create adv queue: %d", ret);
        return;
    }
    tsk_init_param param;
    param.task_entry = mesh_adv_thread;
    param.task_prio = BLE_MESH_TASK_PRIO;
    param.args = NULL;
    param.stack_size = BLE_MESH_ADV_TASK_STACK_SIZE;
    param.task_name = BLE_MESH_ADV_TASK_NAME;
    ret = bt_mesh_create_task(s_task_mesh_adv, &param);
    if (ret != 1) {
        BT_ERR("Failed to create task: %d", ret);
        bt_mesh_queue_delete(adv_queue.handle);
        return;
    }
    
    adapter_initialized = true;
    BT_INFO("mesh_initialized success");
}

int adapter_deinit(void)
{
    bt_mesh_atomic_mutex_free();
    os_msys_deinit();

    future_deinit(&future_mesh);
    if (!ble_mesh_gap_adapter_deinit()) {
        BT_ERR("[ERROR] err gap adapter deinit err.\r\n");
    }

    if (!ble_mesh_gatt_adapter_deinit()) {
        BT_ERR("[ERROR] err gatt adapter deinit err.\r\n");
    }

    if (ble_mesh_gatts_unregister() != 0) {
        BT_ERR("[ERROR] err ble_mesh_gatts_unregister err.\r\n");
        return -1;
    }
    return 0;
}

int adapter_init(void)
{
    bt_mesh_atomic_mutex_new();
    os_mempool_module_init();
    os_msys_init();

    future_init(&future_mesh);

    if (!ble_mesh_gap_adapter_init()) {
        BT_ERR("[ERROR] err gap adapter init failed.");
        goto init_failed;
    }

    if (!ble_mesh_gatt_adapter_init()) {
        BT_ERR("[ERROR] err gatt adapter init failed.");
        ble_mesh_gap_adapter_deinit();
        goto init_failed;
    }

    // 服务注册
    if (ble_mesh_gatts_register() != 0) {
        BT_ERR("[ERROR] err ble_mesh_gatts_register err.");
        ble_mesh_gap_adapter_deinit();
        ble_mesh_gatt_adapter_deinit();
        goto init_failed;
    }

    // open source仓中的服务注册到adapter
    bt_mesh_register_gatt();

    return 0;
init_failed:
    bt_mesh_atomic_mutex_free();
    os_msys_deinit();
    future_deinit(&future_mesh);
    return -1;
}

bool ble_get_adapter_init_status(void)
{
    return adapter_initialized;
}
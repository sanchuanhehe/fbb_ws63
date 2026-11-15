/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
 * Description: BLE Hello sample entry.
 */

#include "app_init.h"
#include "soc_osal.h"

#if defined(CONFIG_SAMPLE_SUPPORT_BLE_GATEWAY_SERVER_SAMPLE)
#include "ble_gateway_server.h"
#elif defined(CONFIG_SAMPLE_SUPPORT_BLE_GATEWAY_CLIENT_SAMPLE)
#include "ble_gateway_client.h"
#include "ble_gateway_iotda.h"
#endif

#define BLE_GATEWAY_TASK_PRIO 26
#define BLE_GATEWAY_TASK_STACK_SIZE 0x2000

static int ble_gateway_task(const char *arg)
{
    (void)arg;
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_GATEWAY_SERVER_SAMPLE)
    errcode_t ret = ble_gateway_server_init();
    if (ret == ERRCODE_SUCC) {
        ble_gateway_server_report_loop();
    }
    return (int)ret;
#elif defined(CONFIG_SAMPLE_SUPPORT_BLE_GATEWAY_CLIENT_SAMPLE)
    return (int)ble_gateway_client_init();
#else
    return 0;
#endif
}

static void ble_gateway_entry(void)
{
    osal_task *task_handle = NULL;

#if defined(CONFIG_SAMPLE_SUPPORT_BLE_GATEWAY_CLIENT_SAMPLE)
    ble_gateway_iotda_init();
#endif
    osal_kthread_lock();
    task_handle =
        osal_kthread_create((osal_kthread_handler)ble_gateway_task, NULL, "ble_gateway", BLE_GATEWAY_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, BLE_GATEWAY_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

app_run(ble_gateway_entry);

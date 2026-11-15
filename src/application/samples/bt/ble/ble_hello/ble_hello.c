/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026.
 * Description: BLE Hello sample entry.
 */

#include "app_init.h"
#include "soc_osal.h"

#if defined(CONFIG_SAMPLE_SUPPORT_BLE_HELLO_SERVER_SAMPLE)
#include "ble_hello_server.h"
#elif defined(CONFIG_SAMPLE_SUPPORT_BLE_HELLO_CLIENT_SAMPLE)
#include "ble_hello_client.h"
#endif

#define BLE_HELLO_TASK_PRIO 26
#define BLE_HELLO_TASK_STACK_SIZE 0x2000

static int ble_hello_task(const char *arg)
{
    (void)arg;
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_HELLO_SERVER_SAMPLE)
    return (int)ble_hello_server_init();
#elif defined(CONFIG_SAMPLE_SUPPORT_BLE_HELLO_CLIENT_SAMPLE)
    return (int)ble_hello_client_init();
#else
    return 0;
#endif
}

static void ble_hello_entry(void)
{
    osal_task *task_handle = NULL;

    osal_kthread_lock();
    task_handle =
        osal_kthread_create((osal_kthread_handler)ble_hello_task, NULL, "ble_hello", BLE_HELLO_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, BLE_HELLO_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

app_run(ble_hello_entry);

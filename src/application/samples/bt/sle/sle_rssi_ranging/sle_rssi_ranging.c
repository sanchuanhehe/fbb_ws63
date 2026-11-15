/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
 *
 * @if Eng
 * @brief SLE RSSI ranging sample entry. \n
 * @else
 * @brief SLE RSSI 测距案例入口。 \n
 * @endif
 */
#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RSSI_RANGING_SERVER_SAMPLE)
#include "sle_rssi_ranging_server.h"
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_RSSI_RANGING_CLIENT_SAMPLE)
#include "sle_rssi_ranging_client.h"
#endif

#define SLE_RSSI_RANGING_TASK_PRIO       28
#define SLE_RSSI_RANGING_TASK_STACK_SIZE 0x1000

/**
 * @if Eng
 * @brief Start the Server or Client selected by Kconfig.
 * @param [in] arg Reserved task argument.
 * @return Always returns NULL after role initialization.
 * @else
 * @brief 启动 Kconfig 选中的 Server 或 Client 角色。
 * @param [in] arg 预留的任务参数。
 * @return 角色初始化完成后固定返回 NULL。
 * @endif
 */
static void *sle_rssi_ranging_task(const char *arg)
{
    unused(arg);
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RSSI_RANGING_SERVER_SAMPLE)
    osal_printk("[sle rssi server] task start\r\n");
    (void)sle_rssi_ranging_server_init();
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_RSSI_RANGING_CLIENT_SAMPLE)
    osal_printk("[sle rssi client] task start\r\n");
    (void)sle_rssi_ranging_client_init();
#endif
    return NULL;
}

/**
 * @if Eng
 * @brief Create the sample entry task without blocking system initialization.
 * @else
 * @brief 创建案例入口任务，避免阻塞系统初始化流程。
 * @endif
 */
static void sle_rssi_ranging_entry(void)
{
    osal_task *task_handle = NULL;

    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)sle_rssi_ranging_task, 0,
        "SLERssiRange", SLE_RSSI_RANGING_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SLE_RSSI_RANGING_TASK_PRIO);
    }
    osal_kthread_unlock();
}

app_run(sle_rssi_ranging_entry);

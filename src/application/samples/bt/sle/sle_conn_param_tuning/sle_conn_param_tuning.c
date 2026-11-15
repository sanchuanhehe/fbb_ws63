/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
 *
 * @if Eng
 * @brief SLE connection parameter tuning sample entry. \n
 * @else
 * @brief SLE 连接参数调优案例入口。 \n
 * @endif
 */
#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_CONN_PARAM_TUNING_SERVER_SAMPLE)
#include "sle_conn_param_tuning_server.h"
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_CONN_PARAM_TUNING_CLIENT_SAMPLE)
#include "sle_conn_param_tuning_client.h"
#endif

#define SLE_CONN_PARAM_TASK_PRIO       28
#define SLE_CONN_PARAM_TASK_STACK_SIZE 0x1000

/**
 * @if Eng
 * @brief Start the Server or Client role selected by Kconfig.
 * @else
 * @brief 启动 Kconfig 选中的 Server 或 Client 角色。
 * @endif
 */
static void *sle_conn_param_tuning_task(const char *arg)
{
    unused(arg);
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_CONN_PARAM_TUNING_SERVER_SAMPLE)
    osal_printk("[sle conn param server] task start\r\n");
    (void)sle_conn_param_tuning_server_init();
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_CONN_PARAM_TUNING_CLIENT_SAMPLE)
    osal_printk("[sle conn param client] task start\r\n");
    (void)sle_conn_param_tuning_client_init();
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
static void sle_conn_param_tuning_entry(void)
{
    osal_task *task_handle = NULL;

    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)sle_conn_param_tuning_task, 0,
        "SLEConnParam", SLE_CONN_PARAM_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SLE_CONN_PARAM_TASK_PRIO);
    }
    osal_kthread_unlock();
}

app_run(sle_conn_param_tuning_entry);

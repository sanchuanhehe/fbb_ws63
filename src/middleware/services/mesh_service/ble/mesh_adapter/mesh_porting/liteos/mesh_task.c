/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: mesh common source \n
 *
 * History: \n
 * 2024-6-21, Create file. \n
 */
#include "mesh_common.h"
static osal_task *g_mesh_task_hdl = NULL;

uint32_t bt_mesh_create_task(osal_kthread_handler task_id, tsk_init_param *param)
{
    osal_kthread_handler func = (osal_kthread_handler)(param->task_entry);
    char *name = param->task_name;
    uint32_t stack_size = param->stack_size;
    void *data = param->args;

    osal_kthread_lock();
    g_mesh_task_hdl = osal_kthread_create(func, NULL, name, stack_size);
    if (g_mesh_task_hdl == NULL) {
        BT_ERR("osal_kthread_create err.\r\n");
        osal_kthread_unlock();
        return 0;
    }
    if (osal_kthread_set_priority(g_mesh_task_hdl, param->task_prio) != 0) {
        BT_INFO("osal_kthread_set_priority err.\r\n");
        osal_kthread_destroy(g_mesh_task_hdl, 1);
        g_mesh_task_hdl = NULL;
        osal_kthread_unlock();
        return 0;
    }
    osal_kthread_unlock();
    MESH_UNUSED(task_id);
    MESH_UNUSED(data);
    return 1;
}

uint32_t bt_mesh_destroy_task(void)
{
    if (g_mesh_task_hdl != NULL) {
        BT_INFO("osal_kthread_destroy mesh task.\r\n");
        osal_kthread_destroy(g_mesh_task_hdl, 1);
        g_mesh_task_hdl = NULL;
    }
    return 0;
}

uint32_t future_deinit(future *future_mesh)
{
    if ((future_mesh != NULL) && (future_mesh->sync.sem != NULL)) {
        osal_sem_destroy(&future_mesh->sync);
    }
    return 0;
}

uint32_t future_init(future *future_mesh)
{
    future_mesh->result = FUTURE_RESULT_INIT;
    return osal_sem_binary_sem_init(&future_mesh->sync, 0);
}

uint32_t future_wait(future *future_mesh, uint32_t timeout)
{
    return osal_sem_down_timeout(&future_mesh->sync, timeout);
}

uint32_t future_ready(future *future_mesh, uint16_t result)
{
    future_mesh->result = result;
    osal_sem_up(&future_mesh->sync);
    return 0;
}

uint32_t future_uninit(future *future_mesh)
{
    future_mesh->result = FUTURE_RESULT_INIT;
    osal_sem_destroy(&future_mesh->sync);
    return 0;
}
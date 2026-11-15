/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: mesh common config \n
 *
 * History: \n
 * 2024-6-21, Create file. \n
 */
#ifndef __MESH_TASK_H__
#define __MESH_TASK_H__

#include "osal_msgqueue.h"
#include "osal_addr.h"
#include "osal_task.h"
#include "osal_mutex.h"
#include "osal_semaphore.h"
#include "mesh/slist.h"
#include "atomic_adapter.h"

#define MESH_UNUSED(x) (void)(x)

#define BLE_MESH_TICK_TIME_FOREVER 0XFFFF
#define FUTURE_RESULT_SUCCESS (1)
#define FUTURE_RESULT_FAIL (2)

#define BLE_NPL_TIME_FOREVER    INT32_MAX
#define TASK_DEFAULT_PRIORITY       1
#define TASK_DEFAULT_STACK          NULL
#define TASK_DEFAULT_STACK_SIZE     400
#define MS_PER_SEC 1000

#define BLE_MESH_TIME_FOREVER (0xFFFFFFFF)
#define TIME_OUT_5000_MS 5000
#define TIME_OUT_500_MS 500
#define TIME_OUT_100_MS 100
#define TIME_OUT_1000_MS 1000
#define TICKS_1000 1000
#define TICKS_1000000 1000000

#define FUTURE_PEND_INIT (0XFFFF)
#define FUTURE_RESULT_INIT (0XFFFF)
#ifndef BLE_MESH_SUCCESS
#define BLE_MESH_SUCCESS 0
#endif
#ifndef BLE_MESH_FAIL
#define BLE_MESH_FAIL (-1)
#endif

typedef struct {
    osal_semaphore sync;
    uint16_t result;
} future;

typedef void (*task_func)(void *);

typedef struct {
    task_func task_entry;  /**< Task entrance function */
    uint16_t task_prio;    /**< Task priority */
    void *args;        /**< Task Parameter, of which the type is void * */
    uint32_t stack_size;   /**< Task stack size */
    char *task_name;       /**< Task name */
} tsk_init_param;

uint32_t bt_mesh_create_task(osal_kthread_handler task_id, tsk_init_param *param);
uint32_t bt_mesh_destroy_task(void);

uint32_t future_init(future *future_mesh);
uint32_t future_deinit(future *future_mesh);
uint32_t future_wait(future *future_mesh, uint32_t timeout);
uint32_t future_ready(future *future_mesh, uint16_t result);
uint32_t future_uninit(future *future_mesh);

static inline uint32_t bt_mesh_queue_create(const CHAR *queue_name, uint16_t len, uint32_t *handle, uint16_t item_size)
{
    return osal_msg_queue_create(queue_name, len, (unsigned long *)handle, 0, item_size);
}

static inline uint32_t bt_mesh_queue_delete(uint32_t handle)
{
    osal_msg_queue_delete(handle);
    return 0;
}

static inline uint32_t bt_mesh_queue_receive(uint32_t handle, void *item, uint32_t item_size, uint32_t tick_timeout)
{
    return osal_msg_queue_read_copy(handle, item, &item_size, tick_timeout);
}

#endif
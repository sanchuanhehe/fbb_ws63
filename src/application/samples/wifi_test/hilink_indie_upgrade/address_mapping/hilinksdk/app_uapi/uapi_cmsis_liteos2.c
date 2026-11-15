/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Common operations on the cmsis liteos2, including session creation and destruction. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"
#include "cmsis_os2.h"

/* Only applicable to the partially used interfaces */
uint32_t osKernelGetTickCount(void)
{
    app_call0(APP_CALL_OS_KERNEL_GET_TICK_COUNT, osKernelGetTickCount, uint32_t);
    return 0;
}

uint32_t osKernelGetTickFreq(void)
{
    app_call0(APP_CALL_OS_KERNEL_GET_TICK_FREQ, osKernelGetTickFreq, uint32_t);
    return 0;
}

osStatus_t osDelay(uint32_t ticks)
{
    app_call1(APP_CALL_OS_DELAY, osDelay, osStatus_t, uint32_t, ticks);
    return osError;
}

osThreadId_t osThreadNew(osThreadFunc_t func, void *argument, const osThreadAttr_t *attr)
{
    app_call3(APP_CALL_OS_THREAD_NEW, osThreadNew, osThreadId_t, osThreadFunc_t, func, void *, argument,
        const osThreadAttr_t *, attr);
    return NULL;
}

osStatus_t osThreadTerminate(osThreadId_t thread_id)
{
    app_call1(APP_CALL_OS_THREAD_TERMINATE, osThreadTerminate, osStatus_t, osThreadId_t, thread_id);
    return osError;
}

osThreadId_t osThreadGetId(void)
{
    app_call0(APP_CALL_OS_THREAD_GET_ID, osThreadGetId, osThreadId_t);
    return NULL;
}

osMutexId_t osMutexNew(const osMutexAttr_t *attr)
{
    app_call1(APP_CALL_OS_MUTEX_NEW, osMutexNew, osMutexId_t, const osMutexAttr_t *, attr);
    return NULL;
}

osStatus_t osMutexDelete(osMutexId_t mutex_id)
{
    app_call1(APP_CALL_OS_MUTEX_DELETE, osMutexDelete, osStatus_t, osMutexId_t, mutex_id);
    return osError;
}

osStatus_t osMutexAcquire(osMutexId_t mutex_id, uint32_t timeout)
{
    app_call2(APP_CALL_OS_MUTEX_ACQUIRE, osMutexAcquire, osStatus_t, osMutexId_t, mutex_id, uint32_t, timeout);
    return osError;
}

osStatus_t osMutexRelease(osMutexId_t mutex_id)
{
    app_call1(APP_CALL_OS_MUTEX_RELEASE, osMutexRelease, osStatus_t, osMutexId_t, mutex_id);
    return osError;
}

osSemaphoreId_t osSemaphoreNew(uint32_t max_count, uint32_t initial_count,
    const osSemaphoreAttr_t *attr)
{
    app_call3(APP_CALL_OS_SEMAPHORE_NEW, osSemaphoreNew, osSemaphoreId_t, uint32_t, max_count,
        uint32_t, initial_count, const osSemaphoreAttr_t *, attr);
    return NULL;
}

osStatus_t osSemaphoreAcquire(osSemaphoreId_t semaphore_id, uint32_t timeout)
{
    app_call2(APP_CALL_OS_SEMAPHORE_ACQUIRE, osSemaphoreAcquire, osStatus_t,
        osSemaphoreId_t, semaphore_id, uint32_t, timeout);
    return osError;
}

osStatus_t osSemaphoreRelease(osSemaphoreId_t semaphore_id)
{
    app_call1(APP_CALL_OS_SEMAPHORE_RELEASE, osSemaphoreRelease, osStatus_t, osSemaphoreId_t, semaphore_id);
    return osError;
}

osStatus_t osSemaphoreDelete(osSemaphoreId_t semaphore_id)
{
    app_call1(APP_CALL_OS_SEMAPHORE_DELETE, osSemaphoreDelete, osStatus_t, osSemaphoreId_t, semaphore_id);
    return osError;
}

osStatus_t osThreadSuspend(osThreadId_t thread_id)
{
    app_call1(APP_CALL_OS_THREAD_SUSPEND, osThreadSuspend, osStatus_t, osThreadId_t, thread_id);
    return osError;
}

osStatus_t osThreadResume(osThreadId_t thread_id)
{
    app_call1(APP_CALL_OS_THREAD_RESUME, osThreadResume, osStatus_t, osThreadId_t, thread_id);
    return osError;
}

void *malloc(size_t size)
{
    app_call1(APP_CALL_MALLOC, malloc, void *, size_t, size);
    return NULL;
}

void free(void *pt)
{
    app_call1_ret_void(APP_CALL_FREE, free, void *, pt);
}
/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Thread adaptation layer interface. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"
#include "hilink_thread_adapter.h"

HiLinkTaskId HILINK_CreateTask(HiLinkTaskParam *param)
{
    app_call1(APP_CALL_HILINK_CREATE_TASK, HILINK_CreateTask, HiLinkTaskId, HiLinkTaskParam *, param);
    return NULL;
}

int HILINK_ThreadSuspend(HiLinkTaskId handle)
{
    app_call1(APP_CALL_HILINK_THREAD_SUSPEND, HILINK_ThreadSuspend, int, HiLinkTaskId, handle);
    return 0;
}

int HILINK_ThreadResume(HiLinkTaskId handle)
{
    app_call1(APP_CALL_HILINK_THREAD_RESUME, HILINK_ThreadResume, int, HiLinkTaskId, handle);
    return 0;
}

void HILINK_DeleteTask(HiLinkTaskId handle)
{
    app_call1_ret_void(APP_CALL_HILINK_DELETE_TASK, HILINK_DeleteTask, HiLinkTaskId, handle);
}

HiLinkTaskId HILINK_GetCurrentTaskId(void)
{
    app_call0(APP_CALL_HILINK_GET_CURRENT_TASK_ID, HILINK_GetCurrentTaskId, HiLinkTaskId);
    return NULL;
}

HiLinkMutexId HILINK_MutexCreate(void)
{
    app_call0(APP_CALL_HILINK_MUTEX_CREATE, HILINK_MutexCreate, HiLinkMutexId);
    return NULL;
}

int HILINK_MutexLock(HiLinkMutexId mutex, unsigned int ms)
{
    app_call2(APP_CALL_HILINK_MUTEX_LOCK, HILINK_MutexLock, int, HiLinkMutexId, mutex, unsigned int, ms);
    return 0;
}

int HILINK_MutexUnlock(HiLinkMutexId mutex)
{
    app_call1(APP_CALL_HILINK_MUTEX_UNLOCK, HILINK_MutexUnlock, int, HiLinkMutexId, mutex);
    return 0;
}

void HILINK_MutexDestroy(HiLinkMutexId mutex)
{
    app_call1_ret_void(APP_CALL_HILINK_MUTEX_DESTROY, HILINK_MutexDestroy, HiLinkMutexId, mutex);
}

HiLinkSemId HILINK_SemCreate(unsigned int count)
{
    app_call1(APP_CALL_HILINK_SEM_CREATE, HILINK_SemCreate, HiLinkSemId, unsigned int, count);
    return NULL;
}

int HILINK_SemWait(HiLinkSemId handle, unsigned int ms)
{
    app_call2(APP_CALL_HILINK_SEM_WAIT, HILINK_SemWait, int, HiLinkSemId, handle, unsigned int, ms);
    return 0;
}

int HILINK_SemPost(HiLinkSemId handle)
{
    app_call1(APP_CALL_HILINK_SEM_POST, HILINK_SemPost, int, HiLinkSemId, handle);
    return 0;
}

void HILINK_SemDestroy(HiLinkSemId handle)
{
    app_call1_ret_void(APP_CALL_HILINK_SEM_DESTROY, HILINK_SemDestroy, HiLinkSemId, handle);
}

int HILINK_MilliSleep(unsigned int ms)
{
    app_call1(APP_CALL_HILINK_MILLI_SLEEP, HILINK_MilliSleep, int, unsigned int, ms);
    return 0;
}

void HILINK_SchedYield(void)
{
    app_call0_ret_void(APP_CALL_HILINK_SCHED_YIELD, HILINK_SchedYield);
}
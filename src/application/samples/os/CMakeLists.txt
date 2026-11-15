#===============================================================================
# @brief    CMake file for OSAL and RTOS samples.
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
#===============================================================================

if(DEFINED CONFIG_SAMPLE_SUPPORT_OSAL_EVENT_FLAG)
    add_subdirectory_if_exist(ipc/osal_event_flag)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_OSAL_MUTEX)
    add_subdirectory_if_exist(ipc/osal_mutex)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_OSAL_MSG_QUEUE)
    add_subdirectory_if_exist(ipc/osal_msg_queue)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_OSAL_SEMAPHORE)
    add_subdirectory_if_exist(ipc/osal_semaphore)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_OSAL_KMALLOC)
    add_subdirectory_if_exist(memory/osal_kmalloc)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_OSAL_TASK_CONCURRENCY)
    add_subdirectory_if_exist(task/osal_task_concurrency)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_OSAL_INTERRUPT)
    add_subdirectory_if_exist(interrupt/osal_interrupt)
endif()

set(SOURCES "${SOURCES}" PARENT_SCOPE)

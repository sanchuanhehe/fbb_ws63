/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: nimble npl os log
 *
 * History:
 * 2025-6-21, Create file.
 */

#ifndef _NIMBLE_NPL_OS_LOG_H_
#define _NIMBLE_NPL_OS_LOG_H_
#include <time.h>
#include <sys/time.h>
#include <osal_debug.h>
#include <log_printf.h>
#include <log_oam_logger.h>
#if defined(CONFIG_BLE_MESH_PRINT_HSO) && (CONFIG_BLE_MESH_PRINT_HSO == 1)
#include <log_def_mesh.h>
#endif
#include <diag_log.h>
#include <logcfg.h>

#if defined(CONFIG_BLE_MESH_PRINT_HSO) && (CONFIG_BLE_MESH_PRINT_HSO == 1)
#define BLE_MESH_DEBUG_LOG(fmt, args...) \
    oml_bth_log_alter(0, 0, LOG_LEVEL_INFO, fmt, var_args_cnt(unused, args...), ##args)
#define BLE_MESH_INFO_LOG(fmt, args...) \
    oml_bth_log_alter(0, 0, LOG_LEVEL_INFO, fmt, var_args_cnt(unused, args...), ##args)
#define BLE_MESH_WARN_LOG(fmt, args...) \
    oml_bth_log_alter(0, 0, LOG_LEVEL_WARNING, fmt, var_args_cnt(unused, args...), ##args)
#define BLE_MESH_ERROR_LOG(fmt, args...) \
    oml_bth_log_alter(0, 0, LOG_LEVEL_ERROR, fmt, var_args_cnt(unused, args...), ##args)
#else
static inline void print_sys_time(void)
{
#if defined(MYNEWT_VAL_BLE_MESH_LOG_PRINT_TIME) && (MYNEWT_VAL_BLE_MESH_LOG_PRINT_TIME == 1)
    struct timeval tv;
    struct tm *tm_ptr;
    struct tm result = {0};
    gettimeofday(&tv, NULL);
    tm_ptr = localtime_r(&tv.tv_sec, &result);
    osal_printk("[%d-%02d-%02d %02d:%02d:%02d.%ld] ",
        1900 + result.tm_year, // year start 1900
        1 + result.tm_mon, result.tm_mday, result.tm_hour, result.tm_min,  result.tm_sec,
        tv.tv_usec / 1000);  // 1000 usec to msec
#endif
}

#define BLE_MESH_DEBUG_LOG(fmt, args...)    do {  \
    print_sys_time();                             \
    osal_printk(fmt, ##args);                     \
} while (0)

#define BLE_MESH_INFO_LOG(fmt, args...)     do {  \
    print_sys_time();                             \
    osal_printk(fmt, ##args);                     \
} while (0)

#define BLE_MESH_WARN_LOG(fmt, args...)     do { \
    print_sys_time();                            \
    osal_printk(fmt, ##args);                    \
} while (0)

#define BLE_MESH_ERROR_LOG(fmt, args...)    do { \
    print_sys_time();                            \
    osal_printk(fmt, ##args);                    \
} while (0)

#endif
#define BLE_NPL_PRINT(fmt, args...)    do {     \
    print_sys_time();                           \
    osal_printk(fmt, ##args);                   \
} while (0)


#if BLE_MESH_LOG_LEVEL >= BLE_MESH_LOG_LEVEL_DBG
#define BLE_NPL_LOG_MODULE_DEBUG(fmt, args...)    BLE_MESH_DEBUG_LOG(fmt, ##args)
#define BLE_NPL_LOG_MODULE_INFO(fmt, args...)     BLE_MESH_INFO_LOG(fmt, ##args)
#define BLE_NPL_LOG_MODULE_WARN(fmt, args...)     BLE_MESH_WARN_LOG(fmt, ##args)
#define BLE_NPL_LOG_MODULE_ERROR(fmt, args...)    BLE_MESH_ERROR_LOG(fmt, ##args)
#elif BLE_MESH_LOG_LEVEL == BLE_MESH_LOG_LEVEL_INFO
#define BLE_NPL_LOG_MODULE_DEBUG(fmt, args...)
#define BLE_NPL_LOG_MODULE_INFO(fmt, args...)     BLE_MESH_INFO_LOG(fmt, ##args)
#define BLE_NPL_LOG_MODULE_WARN(fmt, args...)     BLE_MESH_WARN_LOG(fmt, ##args)
#define BLE_NPL_LOG_MODULE_ERROR(fmt, args...)    BLE_MESH_ERROR_LOG(fmt, ##args)
#elif BLE_MESH_LOG_LEVEL == BLE_MESH_LOG_LEVEL_WARNING
#define BLE_NPL_LOG_MODULE_DEBUG(fmt, args...)
#define BLE_NPL_LOG_MODULE_INFO(fmt, args...)
#define BLE_NPL_LOG_MODULE_WARN(fmt, args...)     BLE_MESH_WARN_LOG(fmt, ##args)
#define BLE_NPL_LOG_MODULE_ERROR(fmt, args...)    BLE_MESH_ERROR_LOG(fmt, ##args)
#elif BLE_MESH_LOG_LEVEL == BLE_MESH_LOG_LEVEL_ERROR
#define BLE_NPL_LOG_MODULE_DEBUG(fmt, args...)
#define BLE_NPL_LOG_MODULE_INFO(fmt, args...)
#define BLE_NPL_LOG_MODULE_WARN(fmt, args...)
#define BLE_NPL_LOG_MODULE_ERROR(fmt, args...)    BLE_MESH_ERROR_LOG(fmt, ##args)
#else // default log info
#define BLE_NPL_LOG_MODULE_DEBUG(fmt, args...)
#define BLE_NPL_LOG_MODULE_INFO(fmt, args...)
#define BLE_NPL_LOG_MODULE_WARN(fmt, args...)
#define BLE_NPL_LOG_MODULE_ERROR(fmt, args...)
#endif
#endif  /* _NIMBLE_NPL_OS_LOG_H_ */

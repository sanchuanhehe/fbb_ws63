#ifndef __HCC_COMM_H
#define __HCC_COMM_H

#include "osal_types.h"
#include "osal_list.h"
#include "soc_osal.h"
#include "securec.h"
#include "hcc_if.h"

#include "syschannel_common.h"
#if defined(_PRE_OS_VERSION_LINUX) && defined(_PRE_OS_VERSION) && (_PRE_OS_VERSION == _PRE_OS_VERSION_LINUX)
#include "oal_netbuf.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

osal_u32 syschannel_host_init(osal_void);
osal_void syschannel_host_exit(osal_void);
syschannel_handler *syschannel_host_get_handler(osal_void);
/* 定时器相关函数 */
osal_void syschannel_heartbeat_timer_create(osal_void);
osal_void syschannel_heartbeat_timer_destroy(osal_void);
osal_void syschannel_heartbeat_timer_restart(osal_void);
osal_u32 syschannel_heartbeat_timeout(osal_void);
osal_u32 uapi_syschannel_host_do_suspend(osal_bool need_dev_reset);
osal_u32 uapi_syschannel_host_suspend(osal_void); /* reset dev by default */
osal_u32 uapi_syschannel_host_resume(osal_u32 try_cnt, osal_u32 delay_ms);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides Watchdog driver \n
 *
 * History: \n
 * 2023-11-01, Create file. \n
 */

#ifndef WATCHDOG_PATCH_H
#define WATCHDOG_PATCH_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* watchdog porting */
void irq_wdt_handler(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
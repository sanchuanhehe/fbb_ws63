/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: nimble npl os config
 *
 * History:
 * 2024-6-21, Create file.
 */
#ifndef _NIMBLE_NPL_OS_H_
#define _NIMBLE_NPL_OS_H_

#include "os_types.h"

#define BLE_NPL_OS_ALIGNMENT    4
#define BLE_NPL_TIME_FOREVER    INT32_MAX

uint32_t ble_npl_hw_enter_critical(void);
void *ble_npl_event_get_arg(struct ble_npl_event *ev);
void ble_npl_callout_stop(struct ble_npl_callout *c);
int ble_npl_callout_delete(timer_t tmr);

#endif  /* _NIMBLE_NPL_OS_H_ */

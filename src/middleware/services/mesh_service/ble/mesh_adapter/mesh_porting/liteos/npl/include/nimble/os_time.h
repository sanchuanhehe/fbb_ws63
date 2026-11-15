/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: npl time
 *
 * History:
 * 2025-6-21, Create file.
 */
#ifndef _OS_TIME_H_
#define _OS_TIME_H_

#include "nimble/nimble_npl.h"

void ble_npl_time_mdelay(uint32_t ms);
uint32_t ble_npl_time_get_ms(void);
#endif /* _OS_TIME_H_ end */
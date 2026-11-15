/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: SLE CHBA OPTIONS.
 */
#ifndef SLE_CHBA_OPT_H
#define SLE_CHBA_OPT_H

#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "sle_chba_manager.h"

#ifdef CONFIG_SUPPORT_SLE_BASE_STATION
#define SLE_CHBA_DEFAULT_INTERVAL 16 // unit: 1.25ms
#else
#define SLE_CHBA_DEFAULT_INTERVAL 60 // unit: 1.25ms
#endif

#define SLE_CHBA_DEFAULT_PHY SLE_PHY_4M

#define SLE_CHBA_DEFAULT_MCS SLE_MCS_10

#define SLE_CHBA_LINK_TIMEOUT 500 // unit: 10ms
#endif
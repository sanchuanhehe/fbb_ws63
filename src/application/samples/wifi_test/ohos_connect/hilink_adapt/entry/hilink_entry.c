/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: HiLink SDK 配网的入口函数
 */

#include "cmsis_os2.h"
#include "nv.h"
#include "hilink_sal_defines.h"
#include "efuse_porting.h"
#include "wifi_device_config.h"
#include "hilink_sle_api.h"
#include "hilink_custom.h"
#include "hilink_entry.h"

/* 极简配网 */
int hilink_minimalist_netcfg_init(void)
{
#ifdef SUPPORT_MINIMALIST_NETCFG
    HILINK_EnableSle();      /* 打开星闪总开关 */
    HILINK_EnablePrescan();
    return hilink_ble_main();
#else
    return 0;
#endif
}

/* 蓝牙辅助配网 */
int hilink_ble_ancillay_netcfg_init(void)
{
#ifdef SUPPORT_BLE_ANCILLAY_NETCFG
    return hilink_ble_main();
#else
    return 0;
#endif
}

/* softAP配网 */
int hilink_softap_netcfg_init(void)
{
#ifdef SUPPORT_SOFTAP_NETCFG
    return hilink_wifi_main();
#else
    return 0;
#endif
}

/* 快速配网 */
int hilink_quick_netcfg_init(void)
{
#ifdef SUPPORT_QUICK_NETCFG
    return hilink_wifi_main();
#else
    return 0;
#endif
}

int hilink_entry(void *param)
{
    param = param;
#ifdef CONFIG_SUPPORT_HILINK_INDIE_UPGRADE
    hilink_indie_upgrade_main();
#endif

    uint8_t hilink_entry_mode = 0;
    uint16_t hilink_entry_mode_len = 0;
    uint32_t ret = ERRCODE_FAIL;
    ret = uapi_nv_read(NV_ID_HILINK_ENTRY_MODE, sizeof(hilink_entry_mode), &hilink_entry_mode_len,
        &hilink_entry_mode);
    if (ret != ERRCODE_SUCC) {
        HILINK_SAL_ERROR("read hilink entry mode from nv failed\r\n");
        return -1; /* -1:读取配网模式失败 */
    }

    SetNetCfgMode(hilink_entry_mode);
    HILINK_SAL_NOTICE("read hilink entry mode: %u\r\n", hilink_entry_mode);
    if (hilink_entry_mode == 0) { /* 0: 极简配网 */
        ret = hilink_minimalist_netcfg_init();
    } else if (hilink_entry_mode == 1) { /* 1: softAP配网 */
        hilink_softap_netcfg_init();
    }  else if (hilink_entry_mode == 2) { /* 2: ble辅助配网 */
        hilink_ble_ancillay_netcfg_init();
    }  else if (hilink_entry_mode == 3) { /* 3: 快速配网 */
        hilink_quick_netcfg_init();
    } else {
        HILINK_SAL_ERROR("hilink_entry_mode=%u\r\n", hilink_entry_mode);
        return -1;
    }

    return 0;
}
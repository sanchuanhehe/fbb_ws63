/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: hcc rom patch callback function.
 * Author: CompanyName
 * Create: 2023-05-18
 */

#include "hcc_rom_callback.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_HCC_COMM_HCC_ROM_CALLBACK_C

#undef THIS_MOD_ID
#define THIS_MOD_ID LOG_PFMODULE

hcc_rom_callback *g_hcc_rom_cb = TD_NULL;
td_void hcc_rom_cb_register(hcc_rom_callback *cb)
{
    g_hcc_rom_cb = cb;
}

td_void *hcc_get_rom_cb(hcc_cb_offset offset)
{
    if (g_hcc_rom_cb != TD_NULL) {
        return (td_void *)(*(((td_u32 *)(td_void *)g_hcc_rom_cb) + (td_u32)offset));
    } else {
        return TD_NULL;
    }
}

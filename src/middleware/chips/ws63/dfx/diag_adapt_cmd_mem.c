/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: diag mem adapt
 * This file should be changed only infrequently and with great care.
 */

#ifdef CONFIG_REG_WHITELIST
#include "diag_adapt_cmd_mem.h"
#include "soc_errno.h"
#endif
#include "diag_cmd_mem_read_write.h"

typedef struct diag_mem_config {
    uintptr_t start_addr;
    uintptr_t end_addr;
} diag_mem_config_t;

static bool diag_permit_check(uintptr_t start_addr, uintptr_t end_addr)
{
    const diag_mem_config_t mem_config[] = {
        { 0x44210060, 0x44210064 },
        { 0x442100D0, 0x442100D4 },
        { 0x44210138, 0x4421013C },
        { 0x44210064, 0x4421006C },
        { 0x44210098, 0x442100A0 },
        { 0x442100B0, 0x442100B8 },
        { 0x4400D868, 0x4400D880 },
        { 0x00100000, 0x0017FFFF },
        { 0x00180000, 0x001C7FFF },
        { 0x00200000, 0x00800000 },
        { 0x00A00000, 0x00A987FF }
    };

    const uintptr_t altx_tb_config[] = {
        0x44210000, 0x44210198, 0x4421019c, 0x442101a0, 0x442101a4, 0x442101a8, 0x442101ac, 0x442101b0, 0x442101b4,
        0x442101b8, 0x44210e20, 0x44212058, 0x4421205c, 0x44212064, 0x44212068, 0x44212080, 0x44212090, 0x44212098,
        0x44212404, 0x44212408, 0x4421240c, 0x44212410, 0x44212414, 0x44212428, 0x4421248c, 0x44212498, 0x4421249c,
        0x442124a0, 0x4421299c, 0x44212e64, 0x44212e88, 0x44212ea0, 0x44212eb0, 0x44212f68, 0x442130e8, 0x44213488,
        0x442134b8, 0x442134e8, 0x44213540
    };
    
    bool ret = false;
    uint32_t loop;

    for (loop = 0; loop < sizeof(mem_config) / sizeof(diag_mem_config_t); loop++) {
        if ((mem_config[loop].start_addr <= start_addr) && (mem_config[loop].end_addr >= end_addr)) {
            return true;
        }
    }

    for (loop = 0; loop < sizeof(altx_tb_config) / sizeof(uintptr_t); loop++) {
        if ((altx_tb_config[loop] == start_addr) && (end_addr == start_addr + sizeof(uint32_t))) {
            ret = true;
            break;
        }
    }
    return ret;
}

#ifdef CONFIG_REG_WHITELIST
osal_s32 reg_rw_check_addr(osal_u32 start_addr, osal_u32 bytes_cnt)
{
    bool ret = diag_permit_check(start_addr, start_addr + bytes_cnt);
    if (ret) {
        return EXT_SUCCESS;
    } else {
        return EXT_FAILURE;
    }
}
#endif

bool diag_cmd_permit_read(uintptr_t start_addr, uintptr_t end_addr)
{
    return diag_permit_check(start_addr, end_addr);
}

bool diag_cmd_permit_write(uintptr_t start_addr, uintptr_t end_addr)
{
    return diag_permit_check(start_addr, end_addr);
}

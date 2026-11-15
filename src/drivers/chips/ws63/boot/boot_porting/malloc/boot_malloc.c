/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: delay
 *
 * Create: 2024-02-5
 */
 
#include "boot_malloc.h"
#include "malloc.h"
#include "malloc_porting.h"
 
void boot_malloc_init(void)
{
    malloc_port_init();
}
 
void *boot_malloc(uint32_t size)
{
    return malloc(size);
}
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description:  LiteOs Heap and Stack info
 *
 * Create: 2025-07-2
 */
#ifndef MEMORY_INFO_H
#define MEMORY_INFO_H

#include <bits/alltypes.h>

void print_os_task_id_and_name(void);

/**
 * @brief  Print LiteOs stack info.
*/
void print_stack_waterline_riscv(void);

/**
 * @brief  Print LiteOs heap info.
*/
void print_heap_statistics_riscv(void);

void print_os_sys_task_heap(uint8_t taskid);

void print_os_all_sys_task_heap(void);

void print_os_dfx_info(void);
/**
 * @}
 */
#endif
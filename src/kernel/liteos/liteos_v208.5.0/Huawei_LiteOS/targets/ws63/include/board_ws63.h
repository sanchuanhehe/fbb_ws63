/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: board ws63 header. \n
 *
 * History: \n
 * 2024-1-23, Create file. \n
 */

#ifndef BOARD_WS63_H
#define BOARD_WS63_H

extern unsigned int __sram_text_begin__;
extern unsigned int __sram_text_end__;
extern unsigned int __text_begin__;
extern unsigned int __text_end__;

extern unsigned int __tcm_text_begin__;
extern unsigned int __tcm_text_end__;
extern LITE_OS_SEC_TEXT VOID OsExcInfoDisplayContext(const ExcInfo *exc);

VOID OsTaskWaterLineArrayGet(UINT32 *array, UINT32 *len);
VOID OsTaskCBArrayGet(UINT32 *array, UINT32 *len);
VOID OsExcInfoDisplayContextExt(const ExcInfo *exc);
VOID crashinfo_taskinfo_title_print(VOID);
VOID crashinfo_taskinfo_print(const LosTaskCB *allTaskArray, UINT32 *water_line_info_arr, UINT32 flash_save_offset,
    UINT32 task_num);
CHAR *OsCurTaskNameGetExt(VOID);
LITE_OS_SEC_TEXT VOID ArchBackTraceCustom(UINTPTR *array, UINTPTR arry_len);
#endif
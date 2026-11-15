/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: app call entry. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#ifndef APP_CALL_ENTRY_H
#define APP_CALL_ENTRY_H

#ifdef __cplusplus
extern "C" {
#endif

void *get_hilink_api_addr(unsigned int index, const char *prototype);
void hilink_func_map_init(void);

extern char hilink_info_addr;

#ifdef __cplusplus
}
#endif
#endif

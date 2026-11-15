/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Functions of all external interfaces of the application and HiLinkSDK. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#ifndef FUNC_CALL_H
#define FUNC_CALL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void *(*api_get)(unsigned int index, const char *prototype);

struct hilink_info_stru {
    void (*entry)(api_get *hilink_get, api_get app_get);
};

struct mapping_tbl {
    void *addr;
    unsigned int checksum;
};

#ifdef __cplusplus
}
#endif
#endif

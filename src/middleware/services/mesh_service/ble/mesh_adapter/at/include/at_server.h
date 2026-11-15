 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: AT server config. \n
 *
 * History: \n
 * 2024-6-21, Create file. \n
 */
#ifndef _AT_SERVER_
#define _AT_SERVER_
typedef struct {
    char *at_cmd;
    int at_len;
    char *at_desc;
    int (*get)(int argc, const char *argv[]);
    int (*op)(int argc, const char *argv[]);
    int (*other)(int argc, const char *argv[]);
} ext_at_cmd;


int at_register_cmd(ext_at_cmd *at, int at_size);

int at_init();

void at_quit();

#endif
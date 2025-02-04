/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: dfx get task info
 * This file should be changed only infrequently and with great care.
 */
#ifndef DIAG_CMD_GET_TASK_INFO_H
#define DIAG_CMD_GET_TASK_INFO_H

#include "errcode.h"
#include "diag.h"

errcode_t diag_cmd_get_task_info(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                 diag_option_t *option);
#endif
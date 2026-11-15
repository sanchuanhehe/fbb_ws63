/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description:  w33 firmware file arrays.
 *
 * History:
 * 2024-05-07, Create file.
 */
#include "w33_fw_file_array.h"

#define W33_FW_ATTR

#define W33_CFG_BIN  "w33_cfg.bin"
W33_FW_ATTR const char g_w33_cfg_file[] = {
#include W33_CFG_BIN
};

const tiot_file g_w33_fw_files[] = {
    { "cfg", sizeof(g_w33_cfg_file), g_w33_cfg_file },
};

const tiot_file_path g_w33_fw_file_path = {
    g_w33_fw_files, sizeof(g_w33_fw_files) / sizeof(tiot_file)
};

const tiot_file_path *w33_fw_file_path_get(void)
{
    return &g_w33_fw_file_path;
}

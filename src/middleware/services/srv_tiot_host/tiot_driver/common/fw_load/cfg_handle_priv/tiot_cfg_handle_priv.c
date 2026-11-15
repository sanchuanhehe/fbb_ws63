/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description:  \n
 *
 * History: \n
 * 2023-12-05, Create file. \n
 */

#include "tiot_misc.h"
#include "tiot_firmware.h"
#include "tiot_firmware_utils.h"
#include "tiot_board_log.h"
#include "tiot_cfg_handle_priv.h"

#define TIOT_FW_FILE_STA        0x43
#define TIOT_FW_FILE_ACK        0x06
#define TIOT_FW_FILE_NAK        0x15
#define TIOT_FW_PRIV_CMD_OK     'G'

#define TIOT_FW_PRIV_ACK_MAX    128
#define TIOT_FW_CUS_STR_LEN     12

#define READ_TIMEOUT_MS     1000

typedef enum {
    TIOT_FW_EXTCMD_ACK = 0x0,
    TIOT_FW_EXTCMD_ACK_EXT = 0x1,
    TIOT_FW_EXTCMD_CUSTOMIZE = 0x2,
    TIOT_FW_EXTCMD_NUM
} tiot_fw_ext_cmd_priv;

typedef int32_t (*priv_ext_cmd_handle_func)(tiot_fw *fw, const uint8_t *cmd, uint16_t len);
typedef int32_t (*pre_priv_ext_cmd_handle_func)(tiot_fw *fw, uint8_t *cmd, uint16_t *len);

static int32_t tiot_fw_ack_handle_priv(tiot_fw *fw, const uint8_t *cmd, uint16_t len);
static int32_t tiot_fw_ack_ext_handle_priv(tiot_fw *fw, const uint8_t *cmd, uint16_t len);
static int32_t tiot_fw_customize_ext_cmd_handle_priv(tiot_fw *fw, uint8_t *cmd, uint16_t *len);

static const priv_ext_cmd_handle_func tiot_fw_priv_ext_cmd_table[TIOT_FW_EXTCMD_NUM] = {
    tiot_fw_ack_handle_priv,
    tiot_fw_ack_ext_handle_priv,
    tiot_fw_ack_handle_priv,
};

static const pre_priv_ext_cmd_handle_func tiot_fw_pre_priv_ext_cmd_table[TIOT_FW_EXTCMD_NUM] = {
    NULL,
    NULL,
    tiot_fw_customize_ext_cmd_handle_priv,
};

static int32_t tiot_fw_customize_para_replace(uint8_t *cmd, uint16_t *len, uint32_t cus_data)
{
    char *tmp = NULL;
    size_t str_len;
    char cus_data_str[TIOT_FW_CUS_STR_LEN] = { 0 };

    if (*len == 0 || (cmd[*len - 1] != ' ')) {
        tiot_print_err("[TIoT:priv] customize cmd not space-terminated.\r\n");
        return ERRCODE_TIOT_FW_LOAD_CUST_ERR;
    }

    if (tiot_num_to_hex(cus_data, cus_data_str, TIOT_FW_CUS_STR_LEN) != ERRCODE_TIOT_SUCC) {
        tiot_print_err("[TIoT:priv] tiot_num_to_hex failed.\r\n");
        return ERRCODE_TIOT_FW_LOAD_CUST_ERR;
    }

    // cmd如下格式"WRITEM 4 0x3ff08 0x1e8480 "; tmp指向子串"0x1e8480"的起始地址.
    cmd[*len - 1] = '\0';
    tmp = strrchr((const char *)cmd, ' ');
    if (tmp == NULL) {
        tiot_print_err("[TIoT:priv] customize cmd string does not have space.\r\n");
        return ERRCODE_TIOT_FW_LOAD_CUST_ERR;
    }
    tmp++;

    // 把tmp指向的字符串替换为tiot驱动配置的参数
    int ret = strcpy_s((char *)tmp, strlen(tmp) + 1, cus_data_str);
    if (ret != EOK) {
        tiot_print_err("[TIoT:priv] customize para replace failed. \r\n");
        return ERRCODE_TIOT_FW_LOAD_CUST_ERR;
    }

    // cmd最后的'\0'替换为空格
    str_len = strlen((const char *)cmd);
    cmd[str_len] = ' ';

    // 更新cmd的长度
    *len = str_len + 1;
    return ERRCODE_TIOT_SUCC;
}

static int32_t tiot_fw_ack_handle_priv(tiot_fw *fw, const uint8_t *cmd, uint16_t len)
{
    int32_t ret = 0;
    uint8_t ack = 0;
    const uint8_t quit_cmd[] = "QUIT ";
    // QUIT命令特殊处理
    if ((len == (sizeof(quit_cmd) - 1)) && (memcmp(quit_cmd, cmd, len) == 0)) {
        return ERRCODE_TIOT_SUCC;
    }
    ret = tiot_firmware_read(fw, &ack, sizeof(uint8_t), READ_TIMEOUT_MS);
    if (ret != sizeof(uint8_t)) {
        return ret;
    }

    tiot_print_dbg("[TIoT:priv]ack 0x%x\r\n", ack);
    if ((ack == TIOT_FW_PRIV_CMD_OK) || (ack == TIOT_FW_FILE_STA) || (ack == TIOT_FW_FILE_ACK)) {
        return ERRCODE_TIOT_SUCC;
    } else if (ack == TIOT_FW_FILE_NAK) {
        return ERRCODE_TIOT_RETRY;
    }
    return ERRCODE_TIOT_FW_LOAD_INVALID_ACK;
}

static int32_t tiot_fw_ack_ext_handle_priv(tiot_fw *fw, const uint8_t *cmd, uint16_t len)
{
    tiot_unused(cmd);
    tiot_unused(len);
    uint32_t i;
    int32_t ret = 0;
    uint8_t ack_buff[TIOT_FW_PRIV_ACK_MAX] = { 0 };
    ret = tiot_firmware_read(fw, ack_buff, sizeof(ack_buff), READ_TIMEOUT_MS);
    if (ret <= 0) {
        return ret;
    }
    tiot_print_info("[TIoT:fw]ack ext: ");
    for (i = 0; i < (uint32_t)ret; i++) {
        tiot_print_info("0x%x ", ack_buff[i]);
    }
    tiot_print_info("\r\n");
    return ERRCODE_TIOT_SUCC;
}

static int32_t tiot_fw_customize_ext_cmd_handle_priv(tiot_fw *fw, uint8_t *cmd, uint16_t *len)
{
    int32_t ret;
    uint32_t cus_data;

    tiot_controller *ctrl = tiot_container_of(fw, tiot_controller, firmware);
    if (ctrl->dev_cus == 0) {
        tiot_print_info("[TIoT:priv] customize is NULL.\r\n");
        return ERRCODE_TIOT_SUCC;
    }

    ret = tiot_firmware_customize_value_get(fw, &cus_data);
    if (ret != ERRCODE_TIOT_SUCC) {
        return ret;
    }

    return tiot_fw_customize_para_replace(cmd, len, cus_data);
}

int32_t tiot_fw_ext_cmd_handle_priv(tiot_fw *fw, uint16_t ext_cmd, const uint8_t *cmd, uint16_t len)
{
    if (ext_cmd >= TIOT_FW_EXTCMD_NUM) {
        tiot_print_err("[TIoT:fw]priv ext cmd exceed max!\r\n");
        return ERRCODE_TIOT_FW_LOAD_INVALID_EXT_CMD;
    }
    return tiot_fw_priv_ext_cmd_table[ext_cmd](fw, cmd, len);
}

int32_t tiot_fw_pre_ext_cmd_handle_priv(tiot_fw *fw, uint16_t ext_cmd, uint8_t *cmd, uint16_t *len)
{
    if (ext_cmd >= TIOT_FW_EXTCMD_NUM) {
        tiot_print_err("[TIoT:fw]pre priv ext cmd exceed max!\r\n");
        return ERRCODE_TIOT_FW_LOAD_INVALID_PRE_EXT_CMD;
    }

    if (tiot_fw_pre_priv_ext_cmd_table[ext_cmd] == NULL) {
        return ERRCODE_TIOT_SUCC;
    }

    return tiot_fw_pre_priv_ext_cmd_table[ext_cmd](fw, cmd, len);
}
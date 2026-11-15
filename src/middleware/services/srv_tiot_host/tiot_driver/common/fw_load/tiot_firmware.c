/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: TIoT firmware load module. \n
 *
 * History: \n
 * 2023-03-30, Create file. \n
 */
#include "tiot_firmware.h"
#include <stdbool.h>
#include "tiot_firmware_utils.h"
#include "securec.h"
#include "tiot_board_log.h"

#define FW_LEN_PER_SEND   64

#define TIOT_FW_LOAD_FINISH    0
#define TIOT_FW_LOAD_NEXT      1

#define TIOT_FW_LOAD_RETRY_CNT 3

#define TIOT_FW_EXT_VERSION1 0x1

static inline uint32_t little_endian_to_u32(const uint8_t *data)
{
    uint32_t val;
    val = data[0x3];
    val <<= 0x8;
    val |= data[0x2];
    val <<= 0x8;
    val |= data[0x1];
    val <<= 0x8;
    val |= data[0x0];
    return val;
}

int32_t tiot_firmware_init(tiot_fw *fw, const tiot_fw_ops *ops)
{
    if ((fw == NULL) || (ops == NULL) || (ops->ext_cmd_handle == NULL)) {
        tiot_print_dbg("[TIoT][fw]param invalid!\n");
        return ERRCODE_TIOT_INVALID_PARAM;
    }
    fw->ops = ops;
    fw->fw_file = NULL;
    return ERRCODE_TIOT_SUCC;
}

static int32_t tiot_firmware_pre_ext_cmd_write(tiot_fw *fw, uint16_t ext_cmd, uint8_t *cmd, uint16_t *cmd_len)
{
    int32_t ret = ERRCODE_TIOT_SUCC;

    if (fw->ops->pre_ext_cmd_handle != NULL) {
        ret = fw->ops->pre_ext_cmd_handle(fw, ext_cmd, cmd, cmd_len);
    }
    return ret;
}

static int32_t firmware_send_cmd(tiot_fw *fw, uint16_t ext_cmd, uint16_t cmd_len)
{
    uint16_t send_len;
    uint16_t tmp_len;
    // 加载时uart等接口每次最多发送64字节数据， 避免资源受限的场景申请大块内存
    uint8_t cmd[FW_LEN_PER_SEND] = { 0 };
    int32_t ret = 0;
    uint16_t remain_len = cmd_len;

    // 一个完整的命令分多次发送，每次发送FW_LEN_PER_SEND字节
    do {
        // 1, Get cmd data from file, maximum of 64 bytes each time.
        send_len = (remain_len > FW_LEN_PER_SEND) ? FW_LEN_PER_SEND : remain_len;
        ret = tiot_file_read(fw->fw_file, (char *)cmd, send_len, fw->cur_offset);
        if (ret != send_len) {
            tiot_print_err("[TIoT:fw]cmd content read fail\r\n");
            return ret;
        }
        fw->cur_offset += send_len;

        // 2, Callback function before sending data.
        tmp_len = send_len;
        // 要确保需要处理的扩展命令长度都小于FW_LEN_PER_SEND字节（customize当前已保证），否则需要调整当前的分包发送逻辑
        ret = tiot_firmware_pre_ext_cmd_write(fw, ext_cmd, cmd, &tmp_len);
        if (ret != ERRCODE_TIOT_SUCC) {
            tiot_print_err("[TIoT:fw]pre ext cmd handle failed.\r\n");
            return ret;
        }

        // 3, send data
        ret = tiot_firmware_write(fw, cmd, tmp_len);
        if (ret != tmp_len) {
            tiot_print_err("[TIoT:fw]cmd content send fail\r\n");
            return ret;
        }

        remain_len -= send_len;
    } while (remain_len > 0);

    // 4, after the whole cmd data send, handle the cmd ack (注意： ext_cmd_handle只能使用最后一包cmd)
    ret = fw->ops->ext_cmd_handle(fw, ext_cmd, cmd, tmp_len);
    if (ret == (int32_t)ERRCODE_TIOT_RETRY) {
        // 如果命令要重试，需复位文件指针， 以及customize数据指针（当前不实现，customize对应的命令不会返回RETRY）
        fw->cur_offset -= cmd_len;
    }

    return ret;
}

static int32_t tiot_firmware_send_cmd_retry(tiot_fw *fw, uint16_t ext_cmd, uint16_t cmd_len)
{
    uint8_t retry_cnt;
    int32_t ret = ERRCODE_TIOT_FW_LOAD_EXT_CMD_FAIL;

    for (retry_cnt = 0; retry_cnt < TIOT_FW_LOAD_RETRY_CNT; retry_cnt++) {
        ret = firmware_send_cmd(fw, ext_cmd, cmd_len);
        if (ret != (int32_t)ERRCODE_TIOT_RETRY) {
            break;
        }
        tiot_print_warning("[TIoT:fw]single cmd retry %d\r\n", retry_cnt);
    }

    if (ret == (int32_t)ERRCODE_TIOT_FW_LOAD_QUIT) {
        tiot_print_info("[TIoT:fw]version matches, no need to upgrade firmware.\r\n");
    } else if (ret == (int32_t)ERRCODE_TIOT_RETRY) {
        ret = ERRCODE_TIOT_RETRY_FAIL;
        tiot_print_err("[TIoT:fw]cmd handle retry fail.\r\n");
    }

    return ret;
}

static int32_t tiot_firmware_customize_index_init(tiot_fw *fw)
{
    tiot_controller *ctrl = tiot_container_of(fw, tiot_controller, firmware);
    if (ctrl->dev_cus == 0) {
        tiot_print_info("[TIoT:fw]tiot firmware no customize. \r\n");
        return ERRCODE_TIOT_SUCC;
    }

    tiot_dev_customize *cus_ptr = (tiot_dev_customize *)ctrl->dev_cus;
    cus_ptr->curr_data = cus_ptr->start_data;

    return ERRCODE_TIOT_SUCC;
}

static int32_t tiot_firmware_load_cmds(tiot_fw *fw)
{
    int32_t ret;
    uint16_t cmd_len, ext_cmd;

    ret = tiot_firmware_customize_index_init(fw);
    if (ret != ERRCODE_TIOT_SUCC) {
        return ret;
    }

    do {
        ret = tiot_file_read(fw->fw_file, (char *)&cmd_len, sizeof(uint16_t), fw->cur_offset);
        // 处于文件末尾，读文件结束
        if (ret == 0) {
            break;
        }
        if (ret != sizeof(uint16_t)) {
            tiot_print_err("[TIoT:fw]cmd len read fail\r\n");
            break;
        }
        ext_cmd = cmd_len >> 12;   // 拓展命令高4位, 右移12位
        cmd_len = cmd_len & 0xFFF; // 长度为低12位

        // 更新offset
        fw->cur_offset += sizeof(uint16_t);

        ret = tiot_firmware_send_cmd_retry(fw, ext_cmd, cmd_len);
        if (ret != ERRCODE_TIOT_SUCC) {
            break;
        }
    } while (true);

    return ret == (int32_t)ERRCODE_TIOT_FW_LOAD_QUIT ? ERRCODE_TIOT_SUCC : ret;
}

static inline int32_t firmware_read_u32(tiot_fw *fw, uint32_t *val)
{
    uint8_t data[4];
    int32_t ret = tiot_file_read(fw->fw_file, (char *)data, sizeof(uint32_t), fw->cur_offset);
    if (ret != sizeof(uint32_t)) {
        return ret;
    }
    fw->cur_offset += sizeof(uint32_t);
    *val = little_endian_to_u32(data);
    return ERRCODE_TIOT_SUCC;
}

static int32_t firmware_load_ext_version1(tiot_fw *fw, tiot_fw_ext_version1 *ext)
{
    int32_t ret = firmware_read_u32(fw, &ext->file_len);
    if (ret != ERRCODE_TIOT_SUCC) {
        tiot_print_err("[TIoT:fw]read img file size fail.\n");
        return ret;
    }
    tiot_print_info("[TIoT:fw]img file size = %d\n", ext->file_len);
    return ERRCODE_TIOT_SUCC;
}

static int32_t tiot_firmware_load_file(tiot_fw *fw, const tiot_file_path *file_path, const char *file_name)
{
    uint32_t version;
    int32_t ret = -1;
    tiot_fw_ext_version1 ext = { 0 };

    /* 初始化时保证cfg_path不为null */
    fw->fw_file = tiot_file_open(file_path, file_name, OSAL_O_RDONLY, 0);
    if (fw->fw_file == NULL) {
        tiot_print_err("open file %s fail\n", file_name);
        return ERRCODE_TIOT_FILE_OPEN_FAIL;
    }
    /* 读取镜像格式版本号 */
    fw->cur_offset = 0;  /* 从偏移量为0开始读取镜像格式版本号 */
    ret = firmware_read_u32(fw, &version);
    if (ret != ERRCODE_TIOT_SUCC) {
        goto file_close;
    }
    tiot_print_info("[TIoT:fw]cfg img version: 0x%x\n", version);
    if (version == TIOT_FW_EXT_VERSION1) { /* 存在拓展区 */
        ret = firmware_load_ext_version1(fw, &ext);
        if (ret != ERRCODE_TIOT_SUCC) {
            goto file_close;
        }
#ifdef CONFIG_FILE_BY_ARRAY
        fw->fw_file->len = ext.file_len;
#else
        tiot_print_warning("[TIoT:fw]non file-array should not use ext version1.\n");
#endif
    }
    // 读取发送命令
    ret = tiot_firmware_load_cmds(fw);
file_close:
    tiot_file_close(fw->fw_file);
    fw->fw_file = NULL;
    return ret;
}

int32_t tiot_firmware_load(tiot_fw *fw, const tiot_file_path *file_path, const char *file_name)
{
    int32_t ret;
    if (fw == NULL || file_name == NULL) {
        tiot_print_dbg("[TIoT][fw]fw object or file_name is NULL.\n");
        return ERRCODE_TIOT_INVALID_PARAM;
    }
    if (fw->ops->pre_cmd_execute != NULL) {
        tiot_print_info("[TIoT][fw]Pre command execute\n");
        fw->ops->pre_cmd_execute(fw);
    }
    /* 执行cfg镜像内命令 */
    ret = tiot_firmware_load_file(fw, file_path, file_name);
    if (ret != ERRCODE_TIOT_SUCC) {
        tiot_print_err("[TIoT][fw]command execute err!\n");
        return ret;
    }
    if (fw->ops->after_cmd_execute != NULL) {
        tiot_print_info("[TIoT][fw]After command execute\n");
        fw->ops->after_cmd_execute(fw);
    }
    return ERRCODE_TIOT_SUCC;
}

int32_t tiot_firmware_customize_value_get(tiot_fw *fw, uint32_t *cus_data)
{
    tiot_dev_customize *cus_ptr;

    tiot_controller *ctrl = tiot_container_of(fw, tiot_controller, firmware);
    if (ctrl->dev_cus == 0) {
        tiot_print_err("[TIoT:fw]customize dev_cus is NULL.\r\n");
        return ERRCODE_TIOT_FW_LOAD_CUST_ERR;
    }

    cus_ptr = (tiot_dev_customize *)ctrl->dev_cus;
    if (cus_ptr->curr_data > cus_ptr->end_data) {
        tiot_print_err("[TIoT:fw] firmware customize get overflow.\r\n");
        return ERRCODE_TIOT_FW_LOAD_CUST_ERR;
    }

    *cus_data = *(uint32_t *)(cus_ptr->curr_data);
    cus_ptr->curr_data += cus_ptr->unit_data_len;
    return ERRCODE_TIOT_SUCC;
}
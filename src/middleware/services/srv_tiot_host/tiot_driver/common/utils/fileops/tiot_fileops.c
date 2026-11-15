/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: TIoT file ops. \n
 *
 * History: \n
 * 2023-04-13, Create file. \n
 */

#include "tiot_fileops.h"
#include "tiot_board_log.h"
#include "tiot_osal.h"
#include "securec.h"

#ifdef CONFIG_FILE_BY_ARRAY
tiot_file *tiot_file_open(const tiot_file_path *path, const char *file_name, int flags, int mode)
{
    unsigned int i;
    const tiot_file *file_list;
    /* 非只读文件 */
    tiot_unused(mode);
    if (flags != OSAL_O_RDONLY) {
        tiot_print_dbg("[TIoT][file]rw file\n");
        return NULL;
    }
    if (path == NULL) {
        tiot_print_dbg("[TIoT][file]array file path is NULL\n");
        return NULL;
    }

    file_list = path->file_list;  /* 文件数组生成保证不为NULL. */
    for (i = 0; i < path->file_num; i++) {
        if (strcmp(file_list->name, file_name) == 0) {
            return (tiot_file *)file_list;
        }
        file_list++;
    }
    return NULL;
}

int32_t tiot_file_read(tiot_file *filp, char *buf, unsigned long size, int offset)
{
    uint32_t remain_size;
    uint32_t copy_size;
    int ret;
    if (buf == NULL) {
        return ERRCODE_TIOT_INVALID_PARAM;
    }
    /* 偏移值需在文件数据范围内. */
    if ((offset < 0) || (offset > filp->len)) {
        tiot_print_err("[TIoT][file]exceed file range!\n");
        return ERRCODE_TIOT_INVALID_PARAM;
    }

    remain_size = filp->len - offset;
    copy_size = size > remain_size ? remain_size : size;
    if (copy_size == 0) {
        /* 处于文件末尾 */
        return 0;
    }
    ret = memcpy_s(buf, copy_size, &filp->data[offset], copy_size);
    if (ret != EOK) {
        return ERRCODE_TIOT_MEMCPY_FAIL;
    }
    return copy_size;
}
#else
tiot_file *tiot_file_open(const tiot_file_path *path, const char *file_name, int flags, int mode)
{
    (void)path;
    return (tiot_file *)osal_klib_fopen(file_name, flags, mode);
}

void tiot_file_close(tiot_file *filp)
{
    osal_klib_fclose((void *)filp);
}

int32_t tiot_file_read(tiot_file *filp, char *buf, unsigned long size, int offset)
{
    int32_t len;
    if (buf == NULL) {
        tiot_print_err("[TIoT][file]read buff is NULL!\n");
        return ERRCODE_TIOT_INVALID_PARAM;
    }
    if (osal_klib_fseek(offset, OSAL_SEEK_SET, (void *)filp) < 0) {
        tiot_print_err("[TIoT][file]fseek error!\n");
        return ERRCODE_TIOT_FILE_SEEK_FAIL;
    }
    len = osal_klib_fread(buf, size, (void *)filp);
    return len >= 0 ? len : (int32_t)ERRCODE_TIOT_FILE_READ_FAIL;
}
#endif


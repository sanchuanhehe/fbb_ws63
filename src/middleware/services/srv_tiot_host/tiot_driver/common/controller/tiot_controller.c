/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: TIoT device controller. \n
 *
 * History: \n
 * 2023-03-17, Create file. \n
 */

#include <stddef.h>
#include "tiot_device_info.h"
#include "tiot_controller.h"
#include "tiot_board_log.h"

#define CTRL_OPEN_COUNT_MAX  0xFFFFFFFFU

int32_t tiot_controller_init(tiot_controller *ctrl, tiot_controller_info *info)
{
    int32_t ret;
    tiot_board_info *board_info = info->board_info;
    const tiot_device_info *device_info = info->dev_info;

    if (ctrl == NULL) {
        return ERRCODE_TIOT_INVALID_PARAM;
    }
    if (ctrl->state == TIOT_CONTROLLER_INIT) {
        return ERRCODE_TIOT_CTRL_INITED;
    }

    /***** 软件初始化 *****/
    /* 初始化open状态互斥锁. */
    ret = osal_mutex_init(&ctrl->open_mutex);
    if (ret != OSAL_SUCCESS) {
        ret = ERRCODE_TIOT_MUTEX_INIT_FAIL;
        goto mutex_init_fail;
    }

    /* 注册：1) PM状态机，如为NULL则使用默认状态机.
             2）板级PM管脚信息，依据产品不同而不同. */
    ret = tiot_pm_init(&ctrl->pm, board_info->hw_info.pm_info,
                       device_info->pm_event_map, device_info->pm_event_map_size);
    if (ret != ERRCODE_TIOT_SUCC) {
        goto pm_init_fail;
    }

    /* 注册：1）固件加载回调，加载前 + 加载后，用于注册不同加载协议或设置波特率等，依据芯片不同而不同. */
    ret = tiot_firmware_init(&ctrl->firmware, &device_info->exec_cbs);
    if (ret != ERRCODE_TIOT_SUCC) {
        goto fw_init_fail;
    }

    /* 注册：1) 接收回调 */
    ret = tiot_xfer_init(&ctrl->transfer, board_info->hw_info.xmit_id, &device_info->xfer_info);
    if (ret != ERRCODE_TIOT_SUCC) {
        goto xfer_init_fail;
    }

    /* device info必须为静态分配变量. */
    ctrl->dev_info = device_info;
    ctrl->state = TIOT_CONTROLLER_INIT;
    ctrl->dev_cus = (uintptr_t)info->dev_cus;
    ctrl->is_host = info->is_host;

    return ERRCODE_TIOT_SUCC;

xfer_init_fail:
    tiot_xfer_deinit(&ctrl->transfer);
    memset_s((void *)&ctrl->transfer, sizeof(tiot_xfer_manager), 0, sizeof(tiot_xfer_manager));
fw_init_fail:
    memset_s((void *)&ctrl->firmware, sizeof(tiot_fw), 0, sizeof(tiot_fw));
pm_init_fail:
    memset_s((void *)&ctrl->pm, sizeof(tiot_pm), 0, sizeof(tiot_pm));
    osal_mutex_destroy(&ctrl->open_mutex);
mutex_init_fail:
    return ret;
}

int32_t tiot_controller_open(tiot_controller *ctrl, void *param)
{
    int32_t ret = ERRCODE_TIOT_SUCC;
    if (ctrl == NULL) {
        tiot_print_dbg("[TIoT]device is null.\n");
        return ERRCODE_TIOT_INVALID_PARAM;
    }
    osal_mutex_lock(&ctrl->open_mutex);
    if (ctrl->state == TIOT_CONTROLLER_UNINIT) {
        tiot_print_err("[TIoT]dev state wrong %d.\r\n", ctrl->state);
        ret = ERRCODE_TIOT_CTRL_UNINITED;
        goto out;
    }
    if (ctrl->open_count == CTRL_OPEN_COUNT_MAX) {
        tiot_print_err("[TIoT]reach max open count.\r\n");
        ret = ERRCODE_TIOT_CTRL_TOO_MANY_OPEN;
        goto out;
    }
    ctrl->open_count++;
    if (ctrl->state == TIOT_CONTROLLER_OPEN) {
        /* 重复调用open仅添加计数. */
        goto out;
    }
    ctrl->state = TIOT_CONTROLLER_OPENING; /* 中间状态，受mutex锁保护，不需要在open内判断. */
    ret = ctrl->ops.open(ctrl, param);
    if (ret != ERRCODE_TIOT_SUCC) {
        ctrl->open_count--;
        ctrl->state = TIOT_CONTROLLER_INIT;
        goto out;
    }
    ctrl->state = TIOT_CONTROLLER_OPEN;
out:
    osal_mutex_unlock(&ctrl->open_mutex);
    return ret;
}

bool tiot_controller_check_opened(tiot_controller *ctrl)
{
    bool ret = true;
    if (ctrl == NULL) {
        tiot_print_dbg("[TIoT]device is null.\n");
        return false;
    }
    osal_mutex_lock(&ctrl->open_mutex);
    if (ctrl->state != TIOT_CONTROLLER_OPEN) {
        tiot_print_dbg("[TIoT]device not opened.\n");
        ret = false;
    }
    osal_mutex_unlock(&ctrl->open_mutex);
    return ret;
}

void tiot_controller_close(tiot_controller *ctrl)
{
    if (ctrl == NULL) {
        tiot_print_dbg("[TIoT]device is null.\n");
        return;
    }
    osal_mutex_lock(&ctrl->open_mutex);
    if ((ctrl->state != TIOT_CONTROLLER_OPEN) && (ctrl->state != TIOT_CONTROLLER_OPENING)) {
        tiot_print_dbg("[TIoT]device is not opening or not opened.\n");
        osal_mutex_unlock(&ctrl->open_mutex);
        return;
    }
    /* 打开状态且open_count >=1时，减少open_count计数; 当open_count减少后变为0时，实际调用close操作. */
    ctrl->open_count--;
    if (ctrl->open_count == 0) {
        ctrl->ops.close(ctrl);
        ctrl->state = TIOT_CONTROLLER_INIT;
    }
    osal_mutex_unlock(&ctrl->open_mutex);
}

void tiot_controller_deinit(tiot_controller *ctrl)
{
    if ((ctrl == NULL) || (ctrl->state == TIOT_CONTROLLER_UNINIT)) {
        return;
    }

    tiot_xfer_deinit(&ctrl->transfer);
    tiot_pm_deinit(&ctrl->pm);
    osal_mutex_destroy(&ctrl->open_mutex);
    (void)memset_s((void *)ctrl, sizeof(tiot_controller), 0, sizeof(tiot_controller));
    ctrl->state = TIOT_CONTROLLER_UNINIT;
}

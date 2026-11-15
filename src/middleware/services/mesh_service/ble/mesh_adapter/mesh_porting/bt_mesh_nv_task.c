/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description:
 *
 * History:
 * 2025-6-04, Create file.
 */

#include "osal_task.h"
#include "osal_wait.h"
#include "soc_errno.h"
#include "mesh_common.h"
#include "ble_mesh_nv_op.h"

#define NV_WTITE_TASK_STACK_SIZE 0x400
#define NV_WTITE_TASK_PRIORITY_NUM 18
#define SERVICE_QUEUE_MAX_SIZE 32
#define OSAL_INVALID_MSG_NUM 0xFFFFFFFF
#define NV_ENCRYPT_YES 1
#define NV_ENCRYPT_NO 0

static osal_task *g_nv_write_task_hdl = NULL;

typedef struct {
    uint8_t  is_encrypt;
    uint16_t key;
    uint16_t len;
    uint8_t *data;
} nv_weite_data_t;

#define SERVICE_MSG_SIZE sizeof(nv_weite_data_t)

static const char *NV_WTITE_TASK_NAME = "nv_write_task";
unsigned long g_nv_write_queue_id = OSAL_INVALID_MSG_NUM;

static int bt_mesh_nv_write_task_body(void *data)
{
    MESH_UNUSED(data);
    nv_weite_data_t wd = {0};
    uint32_t msg_data_size = SERVICE_MSG_SIZE;
    int ret;

    while (true) {
        if (osal_kthread_should_stop() != 0) {
            break;
        }
        (void)memset_s(&wd, sizeof(nv_weite_data_t), 0, msg_data_size);
        ret = osal_msg_queue_read_copy(g_nv_write_queue_id, &wd, &msg_data_size, OSAL_WAIT_FOREVER);
        if (ret != ERRCODE_SUCC) {
            continue;
        }
        if (wd.is_encrypt == NV_ENCRYPT_YES) {
            ret = ble_mesh_data_write_encrypt(wd.key, wd.data, wd.len);
        } else {
            ret = ble_mesh_data_write(wd.key, wd.data, wd.len);
        }
        
        if (ret) {
            BT_ERR("Failed to write, ret: 0x%x, key: 0x%x, len: %d", ret, wd.key, wd.len);
        } else {
            BT_INFO("write %dB to 0x%x succ", wd.len, wd.key);
        }
        osal_kfree(wd.data);
    }

    return EXT_ERR_SUCCESS;
}

int32_t bt_mesh_nv_write_task_deinit(void)
{
    int32_t err = ERRCODE_SUCC;
    if (g_nv_write_task_hdl == NULL || g_nv_write_queue_id == OSAL_INVALID_MSG_NUM) {
        BT_ERR("NV task or queue not initialized.");
        return ERRCODE_FAIL;
    }

    if (g_nv_write_task_hdl != NULL) {
        osal_kthread_destroy(g_nv_write_task_hdl, 0);
        g_nv_write_task_hdl = NULL;
    }

    if (g_nv_write_queue_id != OSAL_INVALID_MSG_NUM) {
        osal_msg_queue_delete(g_nv_write_queue_id);
        g_nv_write_queue_id = OSAL_INVALID_MSG_NUM;
    }

    return ERRCODE_SUCC;
}

void bt_mesh_nv_write_task_init(void)
{
    if (g_nv_write_queue_id != OSAL_INVALID_MSG_NUM) {
        return;
    }

    BT_INFO("");
    int32_t ret = ERRCODE_SUCC;

    ret = (uint32_t)osal_msg_queue_create(
        "nv_write_msg", SERVICE_QUEUE_MAX_SIZE, &g_nv_write_queue_id, 0, SERVICE_MSG_SIZE);
    if (ret != ERRCODE_SUCC) {
        BT_ERR("queue init fail, ret: 0x%x", ret);
        return;
    }

    g_nv_write_task_hdl =
        osal_kthread_create(bt_mesh_nv_write_task_body, NULL, NV_WTITE_TASK_NAME, NV_WTITE_TASK_STACK_SIZE);
    if (g_nv_write_task_hdl == NULL) {
        BT_ERR("create task(%s) fail", NV_WTITE_TASK_NAME);
        osal_msg_queue_delete(g_nv_write_queue_id);
        return;
    }
    osal_kthread_set_priority(g_nv_write_task_hdl, NV_WTITE_TASK_PRIORITY_NUM);
}

static int32_t ble_mesh_nv_push_inner(uint16_t key, const uint8_t *data, uint16_t len, bool is_encrypt)
{
    nv_weite_data_t wd = {0};
    int32_t ret = ERRCODE_SUCC;

    wd.key = key;
    wd.len = len;
    wd.is_encrypt = is_encrypt ? NV_ENCRYPT_YES : NV_ENCRYPT_NO; // 1 encrypt, 0 not encrypt

    uint8_t *data_copy = (uint8_t *)osal_kmalloc(len, 0);
    if (data_copy == NULL) {
        BT_ERR("osal_kmalloc fail");
        return ERRCODE_FAIL;
    }
    if (memcpy_s(data_copy, len, data, len) != EOK) {
        BT_ERR("data memcpy_s failed");
        osal_kfree(data_copy);
        return ERRCODE_FAIL;
    }
    wd.data = data_copy;

    ret = osal_msg_queue_write_copy(g_nv_write_queue_id, &wd, sizeof(nv_weite_data_t), 0);
    if (ret != EOK) {
        BT_ERR("queue full");
        osal_kfree(data_copy);
        return ERRCODE_FAIL;
    }

    return ret;
}

int32_t ble_mesh_nv_push(uint16_t key, const uint8_t *data, uint16_t len)
{
    return ble_mesh_nv_push_inner(key, data, len, false);
}
// 加密写
int32_t ble_mesh_nv_push_encrypt(uint16_t key, const uint8_t *data, uint16_t len)
{
    return ble_mesh_nv_push_inner(key, data, len, true);
}
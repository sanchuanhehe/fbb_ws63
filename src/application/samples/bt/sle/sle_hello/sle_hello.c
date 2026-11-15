/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SLE Hello World Sample Entry. \n
 *
 * History: \n
 * 2024-05-18, Create file. \n
 */
#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_HELLO_SERVER_SAMPLE)
#include "sle_hello_server.h"
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_HELLO_CLIENT_SAMPLE)
#include "sle_hello_client.h"
#endif

#define SLE_HELLO_TASK_PRIO 28
#define SLE_HELLO_TASK_STACK_SIZE 0x1000

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_HELLO_CLIENT_SAMPLE)
static void sle_hello_notification_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data, errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);
    osal_printk("\r\n========================================\r\n");
    osal_printk("[SLE Hello Client] Received: %s\r\n", data->data);
    osal_printk("========================================\r\n\r\n");
}

static void sle_hello_indication_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data, errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);
    osal_printk("\r\n========================================\r\n");
    osal_printk("[SLE Hello Client] Received indication: %s\r\n", data->data);
    osal_printk("========================================\r\n\r\n");
}

static void sle_hello_read_cfm_cb(uint8_t client_id,
                                  uint16_t conn_id,
                                  ssapc_handle_value_t *read_data,
                                  errcode_t status)
{
    unused(client_id);
    unused(status);
    osal_printk("\r\n========================================\r\n");
    osal_printk("[SLE Hello Client] Read result: %s\r\n", read_data->data);
    osal_printk("========================================\r\n\r\n");

    /* 读完成后，继续发起写请求 */
    sle_hello_client_send_write_req(conn_id);
}

static void sle_hello_write_cfm_cb(uint8_t client_id,
                                   uint16_t conn_id,
                                   ssapc_write_result_t *write_result,
                                   errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    osal_printk("\r\n========================================\r\n");
    if (status == 0) {
        osal_printk("[SLE Hello Client] Write cfm: success, handle=0x%02x\r\n", write_result->handle);
    } else {
        osal_printk("[SLE Hello Client] Write cfm: failed, status=0x%x\r\n", status);
    }
    osal_printk("========================================\r\n\r\n");
}
#endif

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_HELLO_SERVER_SAMPLE)
static void *sle_hello_server_task(const char *arg)
{
    unused(arg);
    osal_printk("[SLE Hello Server] task start.\r\n");
    sle_hello_server_init();
    osal_printk("[SLE Hello Server] waiting for connection...\r\n");
    return NULL;
}
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_HELLO_CLIENT_SAMPLE)
static void *sle_hello_client_task(const char *arg)
{
    unused(arg);
    osal_printk("[SLE Hello Client] task start.\r\n");
    sle_hello_client_init(sle_hello_notification_cb, sle_hello_indication_cb, sle_hello_read_cfm_cb,
                          sle_hello_write_cfm_cb);
    osal_printk("[SLE Hello Client] waiting for data...\r\n");
    return NULL;
}
#endif

static void sle_hello_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_HELLO_SERVER_SAMPLE)
    task_handle = osal_kthread_create((osal_kthread_handler)sle_hello_server_task, 0, "SLEHelloServer",
                                      SLE_HELLO_TASK_STACK_SIZE);
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_HELLO_CLIENT_SAMPLE)
    task_handle = osal_kthread_create((osal_kthread_handler)sle_hello_client_task, 0, "SLEHelloClient",
                                      SLE_HELLO_TASK_STACK_SIZE);
#endif
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SLE_HELLO_TASK_PRIO);
    }
    osal_kthread_unlock();
}

/* Run the sle_hello_entry. */
app_run(sle_hello_entry);

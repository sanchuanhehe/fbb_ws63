/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SLE Device Config Sample Entry. \n
 *
 * History: \n
 * 2024-05-18, Create file. \n
 */
#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"
#include "securec.h"
#include "sle_device_config_protocol.h"

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_DEVICE_CONFIG_SERVER_SAMPLE)
#include "sle_device_config_server.h"
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_DEVICE_CONFIG_CLIENT_SAMPLE)
#include "sle_device_config_client.h"
#endif

#define SLE_DEVICE_CONFIG_TASK_PRIO 28
#define SLE_DEVICE_CONFIG_TASK_STACK_SIZE 0x1000
#define SLE_DEVICE_CONFIG_TEST_STEP_INIT 0
#define SLE_DEVICE_CONFIG_TEST_STEP_VALID_SENT 1
#define SLE_DEVICE_CONFIG_TEST_STEP_VALID_CONFIRMED 2
#define SLE_DEVICE_CONFIG_TEST_STEP_PERSISTENCE_READ 3
#define SLE_DEVICE_CONFIG_EXPECTED_INTERVAL_MS 500
#define SLE_DEVICE_CONFIG_EXPECTED_THRESHOLD 750
#define SLE_DEVICE_CONFIG_EXPECTED_MODE 1

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_DEVICE_CONFIG_CLIENT_SAMPLE)
static uint8_t g_config_test_step;

static void sle_device_config_notification_cb(uint8_t client_id,
                                              uint16_t conn_id,
                                              ssapc_handle_value_t *data,
                                              errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);
    osal_printk("[sle device config client] notification length=%u\r\n", data->data_len);
}

static void sle_device_config_indication_cb(uint8_t client_id,
                                            uint16_t conn_id,
                                            ssapc_handle_value_t *data,
                                            errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);
    osal_printk("[sle device config client] indication length=%u\r\n", data->data_len);
}

static void sle_device_config_read_cfm_cb(uint8_t client_id,
                                          uint16_t conn_id,
                                          ssapc_handle_value_t *read_data,
                                          errcode_t status)
{
    unused(client_id);
    if ((status != ERRCODE_SUCC) || (read_data->data_len != sizeof(sle_device_config_t))) {
        osal_printk("[sle device config client] read failed: status=0x%x, length=%u\r\n", status, read_data->data_len);
        return;
    }
    sle_device_config_t config = {0};
    if (memcpy_s(&config, sizeof(config), read_data->data, read_data->data_len) != EOK) {
        osal_printk("[sle device config client] read copy failed\r\n");
        return;
    }
    osal_printk("[sle device config client] read config: interval=%u, threshold=%d, mode=%u\r\n",
                config.report_interval_ms, config.alarm_threshold_decicelsius, config.mode);
    if (g_config_test_step == SLE_DEVICE_CONFIG_TEST_STEP_INIT) {
        g_config_test_step = SLE_DEVICE_CONFIG_TEST_STEP_VALID_SENT;
        sle_device_config_client_send_valid_config(conn_id);
    } else if (g_config_test_step == SLE_DEVICE_CONFIG_TEST_STEP_VALID_CONFIRMED) {
        g_config_test_step = SLE_DEVICE_CONFIG_TEST_STEP_PERSISTENCE_READ;
        if ((config.report_interval_ms == SLE_DEVICE_CONFIG_EXPECTED_INTERVAL_MS) &&
            (config.alarm_threshold_decicelsius == SLE_DEVICE_CONFIG_EXPECTED_THRESHOLD) &&
            (config.mode == SLE_DEVICE_CONFIG_EXPECTED_MODE)) {
            osal_printk("[sle device config client] persisted config verified\r\n");
        }
        sle_device_config_client_send_invalid_config(conn_id);
    }
}

static void sle_device_config_write_cfm_cb(uint8_t client_id,
                                           uint16_t conn_id,
                                           ssapc_write_result_t *write_result,
                                           errcode_t status)
{
    unused(client_id);
    if ((g_config_test_step == SLE_DEVICE_CONFIG_TEST_STEP_VALID_SENT) && (status == ERRCODE_SUCC)) {
        osal_printk("[sle device config client] valid config accepted, handle=0x%02x\r\n", write_result->handle);
        g_config_test_step = SLE_DEVICE_CONFIG_TEST_STEP_VALID_CONFIRMED;
        sle_device_config_client_read_config(conn_id);
    } else if ((g_config_test_step == SLE_DEVICE_CONFIG_TEST_STEP_PERSISTENCE_READ) && (status != ERRCODE_SUCC)) {
        osal_printk("[sle device config client] invalid config rejected, status=0x%x\r\n", status);
        osal_printk("[sle device config client] test passed\r\n");
    } else {
        osal_printk("[sle device config client] unexpected write result, step=%u, status=0x%x\r\n", g_config_test_step,
                    status);
    }
}
#endif

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_DEVICE_CONFIG_SERVER_SAMPLE)
static void *sle_device_config_server_task(const char *arg)
{
    unused(arg);
    osal_printk("[sle device config server] task start\r\n");
    sle_device_config_server_init();
    osal_printk("[sle device config server] waiting for connection\r\n");
    return NULL;
}
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_DEVICE_CONFIG_CLIENT_SAMPLE)
static void *sle_device_config_client_task(const char *arg)
{
    unused(arg);
    osal_printk("[sle device config client] task start\r\n");
    sle_device_config_client_init(sle_device_config_notification_cb, sle_device_config_indication_cb,
                                  sle_device_config_read_cfm_cb, sle_device_config_write_cfm_cb);
    osal_printk("[sle device config client] waiting for connection\r\n");
    return NULL;
}
#endif

static void sle_device_config_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_DEVICE_CONFIG_SERVER_SAMPLE)
    task_handle = osal_kthread_create((osal_kthread_handler)sle_device_config_server_task, 0, "SLEConfigServer",
                                      SLE_DEVICE_CONFIG_TASK_STACK_SIZE);
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_DEVICE_CONFIG_CLIENT_SAMPLE)
    task_handle = osal_kthread_create((osal_kthread_handler)sle_device_config_client_task, 0, "SLEConfigClient",
                                      SLE_DEVICE_CONFIG_TASK_STACK_SIZE);
#endif
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SLE_DEVICE_CONFIG_TASK_PRIO);
    }
    osal_kthread_unlock();
}

/* Run the sle_device_config_entry. */
app_run(sle_device_config_entry);

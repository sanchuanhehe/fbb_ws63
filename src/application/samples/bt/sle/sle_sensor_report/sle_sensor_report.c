/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2024. All rights reserved.
 *
 * @if Eng
 * @brief Provides the entry and display task for the SLE sensor report sample.
 * @else
 * @brief 提供 SLE 传感器上报案例入口及显示任务。
 * @endif
 *
 * History: \n
 * 2024-06-01, Create file. \n
 */

#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"
#include "sle_errcode.h"

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_SENSOR_REPORT_SERVER_SAMPLE)
#include "sle_sensor_report_server.h"
#endif

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_SENSOR_REPORT_CLIENT_SAMPLE)
#include "sle_ssap_client.h"
#include "sle_sensor_report_client.h"
#endif

#define SLE_SENSOR_REPORT_TASK_PRIO 28
#define SLE_SENSOR_REPORT_TASK_STACK_SIZE 0x1000
#define SENSOR_TEMP_SCALE 100

/* Client-local frame matching the server layout. / 客户端本地帧，布局与服务端一致。 */
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_SENSOR_REPORT_CLIENT_SAMPLE)
#define SENSOR_FRAME_TYPE_PERIODIC 0x01
#define SENSOR_FRAME_TYPE_ALARM 0x02

/**
 * @if Eng
 * @brief Defines a data type used by this sample.
 * @else
 * @brief 定义本案例使用的数据类型。
 * @endif
 */
typedef struct {
    uint8_t frame_type;
    uint8_t sensor_count;
    uint32_t timestamp;
    int16_t temperature;
    uint8_t humidity;
    uint16_t light;
} __attribute__((packed)) sensor_data_frame_t;
#endif

#define SENSOR_CLIENT_LOG "[sensor client]"

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_SENSOR_REPORT_CLIENT_SAMPLE)

/* Client data callbacks. / 客户端数据回调。 */

/**
 * @if Eng
 * @brief Handles a notification received from the SLE server.
 * @else
 * @brief 处理从 SLE 服务端收到的通知。
 * @endif
 */
static void sle_sensor_report_notification_cb(uint8_t client_id,
                                              uint16_t conn_id,
                                              ssapc_handle_value_t *data,
                                              errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);

    if (data == NULL || data->data == NULL || data->data_len != sizeof(sensor_data_frame_t)) {
        return;
    }

    sensor_data_frame_t *frame = (sensor_data_frame_t *)data->data;
    osal_printk("%s [T=%ums] temp=%d.%02dC, hum=%u%%, light=%ulux, type=0x%02x\r\n", SENSOR_CLIENT_LOG,
                frame->timestamp, frame->temperature / SENSOR_TEMP_SCALE,
                (frame->temperature >= 0) ? (frame->temperature % SENSOR_TEMP_SCALE)
                                          : (-frame->temperature % SENSOR_TEMP_SCALE),
                frame->humidity, frame->light, frame->frame_type);
}

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c sle_sensor_report_indication_cb.
 * @else
 * @brief 处理分发给 \c sle_sensor_report_indication_cb 的异步事件。
 * @endif
 */
static void sle_sensor_report_indication_cb(uint8_t client_id,
                                            uint16_t conn_id,
                                            ssapc_handle_value_t *data,
                                            errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);

    if (data == NULL || data->data == NULL) {
        return;
    }

    sensor_data_frame_t *frame = (sensor_data_frame_t *)data->data;
    osal_printk("%s ** ALARM ** temp=%d.%02dC exceeds threshold! type=0x%02x\r\n", SENSOR_CLIENT_LOG,
                frame->temperature / SENSOR_TEMP_SCALE,
                (frame->temperature >= 0) ? (frame->temperature % SENSOR_TEMP_SCALE)
                                          : (-frame->temperature % SENSOR_TEMP_SCALE),
                frame->frame_type);
}

/* Client task. / 客户端任务。 */

/**
 * @if Eng
 * @brief Runs the worker task implemented by \c sle_sensor_report_client_task.
 * @else
 * @brief 运行 \c sle_sensor_report_client_task 实现的工作任务。
 * @endif
 */
static void *sle_sensor_report_client_task(const char *arg)
{
    unused(arg);
    sle_sensor_report_client_init(sle_sensor_report_notification_cb, sle_sensor_report_indication_cb);
    return NULL;
}

#endif /* CONFIG_SAMPLE_SUPPORT_SLE_SENSOR_REPORT_CLIENT_SAMPLE */

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_SENSOR_REPORT_SERVER_SAMPLE)

/* Server task. / 服务端任务。 */

/**
 * @if Eng
 * @brief Runs the worker task implemented by \c sle_sensor_report_server_task.
 * @else
 * @brief 运行 \c sle_sensor_report_server_task 实现的工作任务。
 * @endif
 */
static void *sle_sensor_report_server_task(const char *arg)
{
    unused(arg);
    (void)sle_sensor_report_server_init();
    return NULL;
}

#endif /* CONFIG_SAMPLE_SUPPORT_SLE_SENSOR_REPORT_SERVER_SAMPLE */

/* Application entry. / 应用入口。 */

/**
 * @if Eng
 * @brief Creates and starts the sample task.
 * @else
 * @brief 创建并启动案例任务。
 * @endif
 */
static void sle_sensor_report_entry(void)
{
    osal_task *task_handle = NULL;

    osal_kthread_lock();

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_SENSOR_REPORT_SERVER_SAMPLE)
    task_handle = osal_kthread_create((osal_kthread_handler)sle_sensor_report_server_task, 0, "SensorReportServer",
                                      SLE_SENSOR_REPORT_TASK_STACK_SIZE);
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_SENSOR_REPORT_CLIENT_SAMPLE)
    task_handle = osal_kthread_create((osal_kthread_handler)sle_sensor_report_client_task, 0, "SensorReportClient",
                                      SLE_SENSOR_REPORT_TASK_STACK_SIZE);
#endif
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SLE_SENSOR_REPORT_TASK_PRIO);
    }

    osal_kthread_unlock();
}

app_run(sle_sensor_report_entry);

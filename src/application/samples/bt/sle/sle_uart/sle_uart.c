/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
 *
 * @if Eng
 * @brief Provides the entry and UART bridge tasks for the SLE UART sample.
 * @else
 * @brief 提供 SLE UART 案例入口及串口桥接任务。
 * @endif
 *
 * History: \n
 * 2024-05-18, Create file. \n
 */
#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"
#include "pinctrl.h"
#include "uart.h"

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_UART_SERVER_SAMPLE)
#include "sle_uart_server.h"
#include "sle_uart_server_adv.h"
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_UART_CLIENT_SAMPLE)
#include "sle_uart_client.h"
#include "sle_errcode.h"
#endif

#define SLE_UART_TASK_PRIO 28
#define SLE_UART_TASK_STACK_SIZE 0x1000
#define SLE_UART_DROP_LOG_INITIAL_COUNT 3U
#define SLE_UART_DROP_LOG_INTERVAL 100U

/* UART receive buffer shared with the driver. / 与串口驱动共享的接收缓冲区。 */
static uint8_t g_uart_rx_buffer[CONFIG_SLE_UART_RX_BUF_SIZE];

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_UART_SERVER_SAMPLE)
/* SLE server role. / SLE 服务端。 */

unsigned long g_sle_uart_server_msgq_id;

/**
 * @if Eng
 * @brief Handles data received by the UART driver.
 * @else
 * @brief 处理串口驱动接收到的数据。
 * @endif
 */
static void sle_uart_server_rx_handler(const void *buffer, uint16_t length, bool error)
{
    if (error || buffer == NULL || length == 0) {
        osal_printk("[sle uart server] rx cb, error=%d, buf=%p, len=%u\r\n", error, buffer, length);
        return;
    }

    if (!sle_uart_server_is_connected()) {
        osal_printk("[sle uart server] uart rx dropped: not connected\r\n");
        return;
    }

    if (osal_msg_queue_write_copy(g_sle_uart_server_msgq_id, (void *)buffer, (uint32_t)length, 0) != OSAL_SUCCESS) {
        static uint32_t drop_cnt = 0;
        drop_cnt++;
        if (drop_cnt <= SLE_UART_DROP_LOG_INITIAL_COUNT || drop_cnt % SLE_UART_DROP_LOG_INTERVAL == 0) {
            osal_printk("[sle uart server] msgq full, dropped %u packets\r\n", drop_cnt);
        }
    }
}

/**
 * @if Eng
 * @brief Handles an SSAP write request.
 * @else
 * @brief 处理 SSAP 写请求。
 * @endif
 */
static void sle_uart_server_write_cbk(uint8_t server_id,
                                      uint16_t conn_id,
                                      ssaps_req_write_cb_t *write_cb_para,
                                      errcode_t status)
{
    unused(server_id);
    unused(conn_id);
    unused(status);

    if (write_cb_para->value == NULL || write_cb_para->length == 0) {
        return;
    }

    osal_printk("[sle uart server] recv %d bytes from client: %.*s\r\n", write_cb_para->length, write_cb_para->length,
                write_cb_para->value);
    /* Forward client data to UART. / 将客户端数据转发到串口。 */
    int32_t wr_ret = uapi_uart_write(CONFIG_UART_BUS_ID, write_cb_para->value, write_cb_para->length, 0);
    if (wr_ret < 0) {
        osal_printk("[sle uart server] uart write fail: 0x%x\r\n", wr_ret);
    }
}

/**
 * @if Eng
 * @brief Handles an SSAP read request.
 * @else
 * @brief 处理 SSAP 读请求。
 * @endif
 */
static void sle_uart_server_read_cbk(uint8_t server_id,
                                     uint16_t conn_id,
                                     ssaps_req_read_cb_t *read_cb_para,
                                     errcode_t status)
{
    unused(server_id);
    unused(conn_id);
    unused(read_cb_para);
    unused(status);
    /* Read requests are unused for transparent transmission. / 透传场景不处理读请求。 */
}

/**
 * @if Eng
 * @brief Runs the worker task implemented by \c sle_uart_server_task.
 * @else
 * @brief 运行 \c sle_uart_server_task 实现的工作任务。
 * @endif
 */
static errcode_t sle_uart_initialize_uart(const char *role)
{
    uart_attr_t uart_attr = {.baud_rate = CONFIG_SLE_UART_BAUDRATE,
                             .data_bits = UART_DATA_BIT_8,
                             .stop_bits = UART_STOP_BIT_1,
                             .parity = UART_PARITY_NONE};
    uart_pin_config_t pin_config = {
        .tx_pin = CONFIG_UART_TXD_PIN, .rx_pin = CONFIG_UART_RXD_PIN, .cts_pin = PIN_NONE, .rts_pin = PIN_NONE};
    uart_buffer_config_t buffer_config = {.rx_buffer = g_uart_rx_buffer, .rx_buffer_size = CONFIG_SLE_UART_RX_BUF_SIZE};

    osal_printk("[sle uart %s] pin config: bus=%d, tx=%d mode=%d, rx=%d mode=%d\r\n", role, CONFIG_UART_BUS_ID,
                CONFIG_UART_TXD_PIN, CONFIG_UART_TXD_PIN_MODE, CONFIG_UART_RXD_PIN, CONFIG_UART_RXD_PIN_MODE);
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
    uapi_pin_set_ie(CONFIG_UART_RXD_PIN, PIN_IE_ENABLE);
#endif
    errcode_t ret = uapi_pin_set_mode(CONFIG_UART_TXD_PIN, CONFIG_UART_TXD_PIN_MODE);
    if (ret != ERRCODE_SUCC) {
        osal_printk("[sle uart %s] set tx pin mode fail: 0x%x\r\n", role, ret);
    }
    ret = uapi_pin_set_mode(CONFIG_UART_RXD_PIN, CONFIG_UART_RXD_PIN_MODE);
    if (ret != ERRCODE_SUCC) {
        osal_printk("[sle uart %s] set rx pin mode fail: 0x%x\r\n", role, ret);
    }

    uapi_uart_deinit(CONFIG_UART_BUS_ID);
    ret = uapi_uart_init(CONFIG_UART_BUS_ID, &pin_config, &uart_attr, NULL, &buffer_config);
    if (ret == ERRCODE_SUCC) {
        osal_printk("[sle uart %s] uart init ok, bus=%d, tx=%d, rx=%d, baud=%d\r\n", role, CONFIG_UART_BUS_ID,
                    CONFIG_UART_TXD_PIN, CONFIG_UART_RXD_PIN, CONFIG_SLE_UART_BAUDRATE);
    }
    return ret;
}

static void sle_uart_server_forward(void)
{
    uint8_t rx_buf[CONFIG_SLE_UART_MSGQ_ITEM_SIZE];
    uint32_t rx_len;

    while (1) {
        rx_len = CONFIG_SLE_UART_MSGQ_ITEM_SIZE;
        if (osal_msg_queue_read_copy(g_sle_uart_server_msgq_id, rx_buf, &rx_len, OSAL_WAIT_FOREVER) != OSAL_SUCCESS) {
            continue;
        }
        if (rx_len == 0) {
            continue;
        }
        osal_printk("[sle uart server] send notify %d bytes: %.*s\r\n", rx_len, rx_len, rx_buf);
        sle_uart_server_send_notification(rx_buf, (uint16_t)rx_len);
    }
}

static void *sle_uart_server_task(const char *arg)
{
    unused(arg);
    errcode_t ret;

    osal_printk("[sle uart server] task start.\r\n");

    /* 1. Create the message queue. / 1. 创建消息队列。 */
    if (osal_msg_queue_create("sle_uart_srv_msgq", CONFIG_SLE_UART_MSGQ_LEN, &g_sle_uart_server_msgq_id, 0,
                              CONFIG_SLE_UART_MSGQ_ITEM_SIZE) != OSAL_SUCCESS) {
        osal_printk("[sle uart server] msgq create failed!\r\n");
        return NULL;
    }
    ret = sle_uart_initialize_uart("server");
    if (ret != ERRCODE_SUCC) {
        osal_printk("[sle uart server] uart init fail: 0x%x\r\n", ret);
        return NULL;
    }

    /* 2. Configure UART pins. / 2. 配置串口引脚。 */
    /* 3. Initialize UART. / 3. 初始化串口。 */
    /* 4. Initialize the SLE server. / 4. 初始化 SLE 服务端。 */
    sle_uart_server_init(sle_uart_server_read_cbk, sle_uart_server_write_cbk);
    sle_uart_server_adv_init();
    osal_printk("[sle uart server] waiting for connection...\r\n");

    /* 5. Register the UART RX callback in ISR context. / 5. 注册在中断上下文执行的串口接收回调。 */
    ret = uapi_uart_register_rx_callback(CONFIG_UART_BUS_ID, UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE, 1,
                                         sle_uart_server_rx_handler);
    if (ret != ERRCODE_SUCC) {
        osal_printk("[sle uart server] register uart rx callback fail: 0x%x\r\n", ret);
        return NULL;
    } else {
        osal_printk("[sle uart server] uart rx callback registered ok\r\n");
    }

    /* 6. Forward queued UART data through SLE. / 6. 通过 SLE 转发队列中的串口数据。 */
    sle_uart_server_forward();
    return NULL;
}

#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_UART_CLIENT_SAMPLE)
/* SLE client role. / SLE 客户端。 */

unsigned long g_sle_uart_client_msgq_id;

/* Write parameter populated during discovery and reused for sending. / 服务发现阶段填充并由发送任务复用的写参数。 */
ssapc_write_param_t g_write_param = {0};

/**
 * @if Eng
 * @brief Handles data received by the UART driver.
 * @else
 * @brief 处理串口驱动接收到的数据。
 * @endif
 */
static void sle_uart_client_rx_handler(const void *buffer, uint16_t length, bool error)
{
    if (error || buffer == NULL || length == 0) {
        osal_printk("[sle uart client] rx cb, error=%d, buf=%p, len=%u\r\n", error, buffer, length);
        return;
    }

    if (!sle_uart_client_is_connected()) {
        osal_printk("[sle uart client] uart rx dropped: not connected\r\n");
        return;
    }

    if (osal_msg_queue_write_copy(g_sle_uart_client_msgq_id, (void *)buffer, (uint32_t)length, 0) != OSAL_SUCCESS) {
        static uint32_t drop_cnt = 0;
        drop_cnt++;
        if (drop_cnt <= SLE_UART_DROP_LOG_INITIAL_COUNT || drop_cnt % SLE_UART_DROP_LOG_INTERVAL == 0) {
            osal_printk("[sle uart client] msgq full, dropped %u packets\r\n", drop_cnt);
        }
    }
}

/**
 * @if Eng
 * @brief Handles a notification received from the SLE server.
 * @else
 * @brief 处理从 SLE 服务端收到的通知。
 * @endif
 */
static void sle_uart_client_notification_cb(uint8_t client_id,
                                            uint16_t conn_id,
                                            ssapc_handle_value_t *data,
                                            errcode_t status)
{
    unused(client_id);
    unused(conn_id);

    if (status != ERRCODE_SLE_SUCCESS || data == NULL || data->data_len == 0) {
        osal_printk("[sle uart client] notify cb, status=%x, data=%p, len=%u\r\n", status, data,
                    data ? data->data_len : 0);
        return;
    }
    osal_printk("[sle uart client] recv %d bytes from server: %.*s\r\n", data->data_len, data->data_len, data->data);
    uapi_uart_write(CONFIG_UART_BUS_ID, data->data, data->data_len, 0);
}

/**
 * @if Eng
 * @brief Handles completion of an SLE client write request.
 * @else
 * @brief 处理 SLE 客户端写请求完成事件。
 * @endif
 */
static void sle_uart_client_write_cfm_cb(uint8_t client_id,
                                         uint16_t conn_id,
                                         ssapc_write_result_t *write_result,
                                         errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(write_result);
    unused(status);
}

/**
 * @if Eng
 * @brief Runs the worker task implemented by \c sle_uart_client_task.
 * @else
 * @brief 运行 \c sle_uart_client_task 实现的工作任务。
 * @endif
 */
static errcode_t sle_uart_client_initialize_uart(void)
{
    uart_attr_t uart_attr = {.baud_rate = CONFIG_SLE_UART_BAUDRATE,
                             .data_bits = UART_DATA_BIT_8,
                             .stop_bits = UART_STOP_BIT_1,
                             .parity = UART_PARITY_NONE};
    uart_pin_config_t pin_config = {
        .tx_pin = CONFIG_UART_TXD_PIN, .rx_pin = CONFIG_UART_RXD_PIN, .cts_pin = PIN_NONE, .rts_pin = PIN_NONE};
    uart_buffer_config_t buffer_config = {.rx_buffer = g_uart_rx_buffer, .rx_buffer_size = CONFIG_SLE_UART_RX_BUF_SIZE};

    osal_printk("[sle uart %s] pin config: bus=%d, tx=%d mode=%d, rx=%d mode=%d\r\n", "client", CONFIG_UART_BUS_ID,
                CONFIG_UART_TXD_PIN, CONFIG_UART_TXD_PIN_MODE, CONFIG_UART_RXD_PIN, CONFIG_UART_RXD_PIN_MODE);
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
    uapi_pin_set_ie(CONFIG_UART_RXD_PIN, PIN_IE_ENABLE);
#endif
    errcode_t ret = uapi_pin_set_mode(CONFIG_UART_TXD_PIN, CONFIG_UART_TXD_PIN_MODE);
    if (ret != ERRCODE_SUCC) {
        osal_printk("[sle uart %s] set tx pin mode fail: 0x%x\r\n", "client", ret);
    }
    ret = uapi_pin_set_mode(CONFIG_UART_RXD_PIN, CONFIG_UART_RXD_PIN_MODE);
    if (ret != ERRCODE_SUCC) {
        osal_printk("[sle uart %s] set rx pin mode fail: 0x%x\r\n", "client", ret);
    }

    uapi_uart_deinit(CONFIG_UART_BUS_ID);
    ret = uapi_uart_init(CONFIG_UART_BUS_ID, &pin_config, &uart_attr, NULL, &buffer_config);
    if (ret == ERRCODE_SUCC) {
        osal_printk("[sle uart client] uart init ok, bus=%d, tx=%d, rx=%d, baud=%d\r\n", CONFIG_UART_BUS_ID,
                    CONFIG_UART_TXD_PIN, CONFIG_UART_RXD_PIN, CONFIG_SLE_UART_BAUDRATE);
    }
    return ret;
}

static void sle_uart_client_forward(void)
{
    uint8_t data[CONFIG_SLE_UART_MSGQ_ITEM_SIZE];
    uint32_t data_len;

    while (1) {
        data_len = CONFIG_SLE_UART_MSGQ_ITEM_SIZE;
        if (osal_msg_queue_read_copy(g_sle_uart_client_msgq_id, data, &data_len, OSAL_WAIT_FOREVER) != OSAL_SUCCESS) {
            continue;
        }
        if (data_len == 0) {
            continue;
        }
        g_write_param.data = data;
        g_write_param.data_len = data_len;
        osal_printk("[sle uart client] send %d bytes: %.*s\r\n", data_len, data_len, data);
        ssapc_write_req(0, g_conn_id, &g_write_param);
    }
}

static void *sle_uart_client_task(const char *arg)
{
    unused(arg);
    errcode_t ret;

    osal_printk("[sle uart client] task start.\r\n");

    /* 1. Create the message queue. / 1. 创建消息队列。 */
    if (osal_msg_queue_create("sle_uart_cli_msgq", CONFIG_SLE_UART_MSGQ_LEN, &g_sle_uart_client_msgq_id, 0,
                              CONFIG_SLE_UART_MSGQ_ITEM_SIZE) != OSAL_SUCCESS) {
        osal_printk("[sle uart client] msgq create failed!\r\n");
        return NULL;
    }
    ret = sle_uart_client_initialize_uart();
    if (ret != ERRCODE_SUCC) {
        osal_printk("[sle uart client] uart init fail: 0x%x\r\n", ret);
        return NULL;
    }

    /* 2. Configure UART pins. / 2. 配置串口引脚。 */
    /* 3. Initialize UART. / 3. 初始化串口。 */
    /* 4. Initialize scanning, connection, pairing, MTU, and discovery. / 4. 初始化扫描、连接、配对、MTU 和服务发现。 */
    sle_uart_client_init(sle_uart_client_notification_cb, sle_uart_client_write_cfm_cb);
    osal_printk("[sle uart client] scanning for uart_server...\r\n");

    /* 5. Register the UART RX callback in ISR context. / 5. 注册在中断上下文执行的串口接收回调。 */
    ret = uapi_uart_register_rx_callback(CONFIG_UART_BUS_ID, UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE, 1,
                                         sle_uart_client_rx_handler);
    if (ret != ERRCODE_SUCC) {
        osal_printk("[sle uart client] register uart rx callback fail: 0x%x\r\n", ret);
        return NULL;
    } else {
        osal_printk("[sle uart client] uart rx callback registered ok\r\n");
    }

    /* 6. Forward queued UART data with SLE write requests. / 6. 使用 SLE 写请求转发队列中的串口数据。 */
    sle_uart_client_forward();
    return NULL;
}
#endif

/**
 * @if Eng
 * @brief Creates and starts the sample task.
 * @else
 * @brief 创建并启动案例任务。
 * @endif
 */
static void sle_uart_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_UART_SERVER_SAMPLE)
    task_handle =
        osal_kthread_create((osal_kthread_handler)sle_uart_server_task, 0, "SLEUartServer", SLE_UART_TASK_STACK_SIZE);
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_UART_CLIENT_SAMPLE)
    task_handle =
        osal_kthread_create((osal_kthread_handler)sle_uart_client_task, 0, "SLEUartClient", SLE_UART_TASK_STACK_SIZE);
#endif
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SLE_UART_TASK_PRIO);
    }
    osal_kthread_unlock();
}

app_run(sle_uart_entry);

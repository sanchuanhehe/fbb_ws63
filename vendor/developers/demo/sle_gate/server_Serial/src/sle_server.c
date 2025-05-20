/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Copyright (c) kidwjb
 * Description: use with radar module.
 */
#include "securec.h"
#include "errcode.h"
#include "osal_addr.h"
#include "soc_osal.h"
#include "app_init.h"
#include "sle_common.h"
#include "sle_errcode.h"
#include "sle_ssap_server.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "sle_server_adv.h"
#include "sle_server.h"
#include "pinctrl.h"
#include "uart.h"
#include "watchdog.h"
#include <time.h>
#include <sys/time.h>

#define DELAY_1000MS 1000
#define CYCLES 10
#define OCTET_BIT_LEN 8
#define UUID_LEN_2 2
#define BT_INDEX_4 4
#define BT_INDEX_5 5
#define BT_INDEX_0 0

#define UART_BAUDRATE 115200
#define UART_DATA_BITS 3
#define UART_STOP_BITS 1
#define UART_PARITY_BIT 0
#define UART_TRANSFER_SIZE 512
#define UART_TASK_PRIO 25
#define UART_TASK_STACK_SIZE 0x2000
#define CONFIG_UART_TXD_PIN 15
#define CONFIG_UART_RXD_PIN 16
#define CONFIG_UART_BUS_ID 1
#define DELAY_3000MS 3000
#define DELAY_5000MS 5000
#define encode2byte_little(_ptr, data)                     \
    do {                                                   \
        *(uint8_t *)((_ptr) + 1) = (uint8_t)((data) >> 8); \
        *(uint8_t *)(_ptr) = (uint8_t)(data);              \
    } while (0)

enum {
    INDEX0,
    INDEX1,
    INDEX2,
    INDEX3,
    INDEX4,
    INDEX5,
    INDEX6,
    INDEX7,
    INDEX8,
    INDEX9,
    INDEX10,
    INDEX11,
    INDEX12,
    INDEX13,
    INDEX14,
    INDEX15,
};

/* sle server app uuid for test */
char g_sle_uuid_app_uuid[UUID_LEN_2] = {0x0, 0x0};
/* server notify property uuid for test */
char g_sle_property_value[OCTET_BIT_LEN] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
/* sle connect acb handle */
uint16_t g_sle_conn_hdl = 0;
/* sle server handle */
uint8_t g_server_id = 0;
/* sle service handle */
uint16_t g_service_handle = 0;
/* sle ntf property handle */
uint16_t g_property_handle = 0;
/* sle pair acb handle */
uint16_t g_sle_pair_hdl;

static uint8_t sle_uuid_base[] = {0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
                                  0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

uint8_t g_app_uart_rx_buff[UART_TRANSFER_SIZE] = {0};

uint8_t existence = 0;     // 1: 有人    0：无人
uint8_t kinetic = 0;       // 0 ~ 100
uint8_t heart_rate = 0;    // 60 ~ 120
uint8_t breathe_state = 0; // 1:正常 2：呼吸过高 3：呼吸过低 4：无呼吸
uint8_t breate_value = 0;  // 0~35

//*********************get_time**********************//
void get_current_time(void)
{
    while (1) {
        struct timeval current_time;
        if (gettimeofday(&current_time, NULL) == -1) {
            osal_printk("gettimeofday FAIL\r\n");
        }
        time_t rawtime = current_time.tv_sec; // Get the time in seconds
        // Convert to local time
        struct tm *local_time;
        local_time = localtime(&rawtime);
        if (local_time == NULL) {
            printf("localtime failed\r\n");
            return;
        }
        // Print local time in YYYY-MM-DD format
        printf("Local time: %04d-%02d-%02d %02d:%02d:%02d\r\n",
               local_time->tm_year + 1900, // tm_year is years since 1900
               local_time->tm_mon + 1,     // tm_mon is months since January (0-11)
               local_time->tm_mday,        // Day of the month (1-31)
               local_time->tm_hour,        // Hour (0-23)
               local_time->tm_min,         // Minutes (0-59)
               local_time->tm_sec);        // Seconds (0-59)
        osal_msleep(DELAY_5000MS);
    }
}

void time_convert(time_t *rawtime, uint8_t *send_time)
{
    uint64_t temp = 0;
    uint8_t i = 0;
    for (i = 0; i < 8; i++) {                           /*8: 一共个字节 */
        temp |= (uint64_t)send_time[i] << (56 - i * 8); /*56位，一次8位 */
    }
    *rawtime = (time_t)temp;
}
//---------------gettime-----------------------//

//******************uart*********************//
void uart_rx_cbk(const void *buffer, uint16_t length, bool error)
{
    unused(error);
    unused(length);
    uint8_t *data = (uint8_t *)buffer;
    if (data[INDEX0] == 0x53 && data[INDEX1] == 0x59) { /*53 59 帧头 */
        if (data[INDEX2] == 0x80)                       // 人体存在功能 0x80
        {
            if (data[INDEX3] == 0x01) { // 存在信息 0x01
                existence = data[INDEX6];
            }
            if (data[INDEX3] == 0x03) { // 体动参数 0x03
                kinetic = data[INDEX6];
            }
        }
        if (data[INDEX2] == 0x85) { // 心率监测 0x85
            if (data[INDEX3] == 0x02) { // 心率数值 0x02
                heart_rate = data[INDEX6];
            }
        }
        if (data[INDEX2] == 0x81) { // 呼吸检测 0x81
            if (data[INDEX3] == 0x01) { // 呼吸信息 0x01
                breathe_state = data[INDEX6];
            }
            if (data[INDEX3] == 0x02) { // 呼吸数值 0x02
                breate_value = data[INDEX6];
            }
        }
    }
}

static void My_uart_pin_init(void)
{
    uapi_pin_set_mode(CONFIG_UART_TXD_PIN, PIN_MODE_1); // 引脚模式设置为1
    uapi_pin_set_mode(CONFIG_UART_TXD_PIN, PIN_MODE_1);
}
static uart_buffer_config_t g_app_uart_buffer_config = {.rx_buffer = g_app_uart_rx_buff,
                                                        .rx_buffer_size = UART_TRANSFER_SIZE};
static void uart_config(void)
{
    uart_attr_t attr = {.baud_rate = UART_BAUDRATE,
                        .data_bits = UART_DATA_BITS,
                        .stop_bits = UART_STOP_BITS,
                        .parity = UART_PARITY_BIT};
    uart_pin_config_t pin_config = {
        .tx_pin = CONFIG_UART_TXD_PIN, .rx_pin = CONFIG_UART_RXD_PIN, .cts_pin = PIN_NONE, .rts_pin = PIN_NONE};
    uapi_uart_deinit(CONFIG_UART_BUS_ID);
    uapi_uart_init(CONFIG_UART_BUS_ID, &pin_config, &attr, NULL, &g_app_uart_buffer_config);
}
static void *uart_task(const char *arg)
{
    unused(arg);
    My_uart_pin_init();
    uart_config();
    uapi_uart_unregister_rx_callback(CONFIG_UART_BUS_ID);
    errcode_t ret =
        uapi_uart_register_rx_callback(CONFIG_UART_BUS_ID, UART_RX_CONDITION_FULL_OR_IDLE, 1, uart_rx_cbk); // 1 enable
    if (ret == ERRCODE_FAIL) {
        osal_printk("rx cbk register fail\r\n");
    }
    osal_printk("\r\n[R60ABD1]start !\r\n");
    uint8_t string_sense[100];
    int count = 0;
    while (1) {
        count = sprintf_s((char *)string_sense, sizeof(string_sense),
                          "existence: %d\r\n"
                          "kinetic: %d\r\n"
                          "heart_rate: %d\r\n"
                          "breathe_state: %d\r\n"
                          "breate_value: %d\r\n"
                          "\r\n",
                          existence, kinetic, heart_rate, breathe_state, breate_value);
        if (count == -1) {
            osal_printk("sprintf fail\r\n");
        } else {
            osal_printk("sprintf num %d\r\n", count);
        }
        sle_uuid_server_send_report_by_handle(string_sense, sizeof(string_sense));
        osal_msleep(DELAY_3000MS);
    }
    return NULL;
}
static void uart_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)uart_task, 0, "UartTask", UART_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, UART_TASK_PRIO);
    }
    osal_kthread_unlock();
}
//------------------uart---------------------//

static void sle_uuid_set_base(sle_uuid_t *out)
{
    (void)memcpy_s(out->uuid, SLE_UUID_LEN, sle_uuid_base, SLE_UUID_LEN);
    out->len = UUID_LEN_2;
}

static void sle_uuid_setu2(uint16_t u2, sle_uuid_t *out)
{
    sle_uuid_set_base(out);
    out->len = UUID_LEN_2;
    encode2byte_little(&out->uuid[INDEX14], u2);
}

static void ssaps_read_request_cbk(uint8_t server_id,
                                   uint16_t conn_id,
                                   ssaps_req_read_cb_t *read_cb_para,
                                   errcode_t status)
{
    osal_printk("[uuid server] ssaps read request cbk server_id:%x, conn_id:%x, handle:%x, status:%x\r\n", server_id,
                conn_id, read_cb_para->handle, status);
}

static void ssaps_write_request_cbk(uint8_t server_id,
                                    uint16_t conn_id,
                                    ssaps_req_write_cb_t *write_cb_para,
                                    errcode_t status)
{
    osal_printk("[uuid server] ssaps write request cbk server_id:%x, conn_id:%x, handle:%x, status:%x\r\n", server_id,
                conn_id, write_cb_para->handle, status);
    osal_printk("write request data: %s\r\n", write_cb_para->value);
    if (write_cb_para->value[INDEX0] == 0xAA && write_cb_para->value[INDEX1] == 0x33 &&
        write_cb_para->value[INDEX10] == 0xFF) {
        time_t rawtime;
        time_convert(&rawtime, &(write_cb_para->value[INDEX2]));
        struct timeval current_time;
        current_time.tv_sec = rawtime;
        current_time.tv_usec = 0;
        if (settimeofday(&current_time, NULL) == -1) {
            osal_printk("settimeofday FAIL\r\n");
            return;
        }
        osal_printk("settimeofday SUCC\r\n");
    }
}

static void ssaps_mtu_changed_cbk(uint8_t server_id, uint16_t conn_id, ssap_exchange_info_t *mtu_size, errcode_t status)
{
    osal_printk("[uuid server] ssaps write request cbk server_id:%x, conn_id:%x, mtu_size:%x, status:%x\r\n", server_id,
                conn_id, mtu_size->mtu_size, status);
}

static void ssaps_start_service_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
    osal_printk("[uuid server] start service cbk server_id:%x, handle:%x, status:%x\r\n", server_id, handle, status);
}

static void sle_ssaps_register_cbks(void)
{
    ssaps_callbacks_t ssaps_cbk = {0};
    ssaps_cbk.start_service_cb = ssaps_start_service_cbk;
    ssaps_cbk.mtu_changed_cb = ssaps_mtu_changed_cbk;
    ssaps_cbk.read_request_cb = ssaps_read_request_cbk;
    ssaps_cbk.write_request_cb = ssaps_write_request_cbk;
    ssaps_register_callbacks(&ssaps_cbk);
}

static errcode_t sle_uuid_server_service_add(void)
{
    errcode_t ret;
    sle_uuid_t service_uuid = {0};
    sle_uuid_setu2(SLE_UUID_SERVER_SERVICE, &service_uuid);
    ret = ssaps_add_service_sync(g_server_id, &service_uuid, 1, &g_service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uuid server] sle uuid add service fail, ret:%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_uuid_server_property_add(void)
{
    errcode_t ret;
    ssaps_property_info_t property = {0};
    ssaps_desc_info_t descriptor = {0};
    uint8_t ntf_value[] = {0x01, 0x0};

    property.permissions = SLE_UUID_TEST_PROPERTIES;
    sle_uuid_setu2(SLE_UUID_SERVER_NTF_REPORT, &property.uuid);
    property.value = osal_vmalloc(sizeof(g_sle_property_value));
    property.operate_indication = SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_NOTIFY;
    if (property.value == NULL) {
        osal_printk("[uuid server] sle property mem fail\r\n");
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(property.value, sizeof(g_sle_property_value), g_sle_property_value, sizeof(g_sle_property_value)) !=
        EOK) {
        osal_vfree(property.value);
        osal_printk("[uuid server] sle property mem cpy fail\r\n");
        return ERRCODE_SLE_FAIL;
    }
    ret = ssaps_add_property_sync(g_server_id, g_service_handle, &property, &g_property_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uuid server] sle uuid add property fail, ret:%x\r\n", ret);
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    descriptor.permissions = SLE_UUID_TEST_DESCRIPTOR;
    descriptor.operate_indication = SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE;
    descriptor.type = SSAP_DESCRIPTOR_USER_DESCRIPTION;
    descriptor.value_len = sizeof(ntf_value);
    descriptor.value = osal_vmalloc(sizeof(ntf_value));
    if (descriptor.value == NULL) {
        osal_printk("[uuid server] sle descriptor mem fail\r\n");
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(descriptor.value, sizeof(ntf_value), ntf_value, sizeof(ntf_value)) != EOK) {
        osal_printk("[uuid server] sle descriptor mem cpy fail\r\n");
        osal_vfree(property.value);
        osal_vfree(descriptor.value);
        return ERRCODE_SLE_FAIL;
    }
    ret = ssaps_add_descriptor_sync(g_server_id, g_service_handle, g_property_handle, &descriptor);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uuid server] sle uuid add descriptor fail, ret:%x\r\n", ret);
        osal_vfree(property.value);
        osal_vfree(descriptor.value);
        return ERRCODE_SLE_FAIL;
    }
    osal_vfree(property.value);
    osal_vfree(descriptor.value);
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_uuid_server_add(void)
{
    errcode_t ret;
    sle_uuid_t app_uuid = {0};

    osal_printk("[uuid server] sle uuid add service in\r\n");
    app_uuid.len = sizeof(g_sle_uuid_app_uuid);
    if (memcpy_s(app_uuid.uuid, app_uuid.len, g_sle_uuid_app_uuid, sizeof(g_sle_uuid_app_uuid)) != EOK) {
        return ERRCODE_SLE_FAIL;
    }
    ssaps_register_server(&app_uuid, &g_server_id);

    if (sle_uuid_server_service_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }

    if (sle_uuid_server_property_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }
    osal_printk("[uuid server] sle uuid add service, server_id:%x, service_handle:%x, property_handle:%x\r\n",
                g_server_id, g_service_handle, g_property_handle);
    ret = ssaps_start_service(g_server_id, g_service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uuid server] sle uuid add service fail, ret:%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }
    osal_printk("[uuid server] sle uuid add service out\r\n");
    return ERRCODE_SLE_SUCCESS;
}

/* device通过uuid向host发送数据：report */
errcode_t sle_uuid_server_send_report_by_uuid(const uint8_t *data, uint16_t len)
{
    ssaps_ntf_ind_by_uuid_t param = {0};
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.start_handle = g_service_handle;
    param.end_handle = g_property_handle;
    param.value_len = len;
    param.value = osal_vmalloc(len);
    if (param.value == NULL) {
        osal_printk("[uuid server] send report new fail\r\n");
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(param.value, param.value_len, data, len) != EOK) {
        osal_printk("[uuid server] send input report memcpy fail\r\n");
        osal_vfree(param.value);
        return ERRCODE_SLE_FAIL;
    }
    sle_uuid_setu2(SLE_UUID_SERVER_NTF_REPORT, &param.uuid);
    ssaps_notify_indicate_by_uuid(g_server_id, g_sle_conn_hdl, &param);
    osal_vfree(param.value);
    return ERRCODE_SLE_SUCCESS;
}

/* device通过handle向host发送数据：report */
errcode_t sle_uuid_server_send_report_by_handle(const uint8_t *data, uint8_t len)
{
    ssaps_ntf_ind_t param = {0};
    errcode_t ret;
    param.handle = g_property_handle;
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.value = osal_vmalloc(len);
    param.value_len = len;
    if (param.value == NULL) {
        osal_printk("[uuid server] send report new fail\r\n");
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(param.value, param.value_len, data, len) != EOK) {
        osal_printk("[uuid server] send input report memcpy fail\r\n");
        osal_vfree(param.value);
        return ERRCODE_SLE_FAIL;
    }
    osal_printk("memcpy param data: %s", param.value);
    ret = ssaps_notify_indicate(g_server_id, g_sle_conn_hdl, &param);
    if (ret == ERRCODE_FAIL) {
        osal_printk("notify fail\r\n");
    }
    osal_vfree(param.value);
    return ERRCODE_SLE_SUCCESS;
}

static void sle_connect_state_changed_cbk(uint16_t conn_id,
                                          const sle_addr_t *addr,
                                          sle_acb_state_t conn_state,
                                          sle_pair_state_t pair_state,
                                          sle_disc_reason_t disc_reason)
{
    osal_printk(
        "[uuid server] connect state changed conn_id:0x%02x, conn_state:0x%x, pair_state:0x%x, \
        disc_reason:0x%x\r\n",
        conn_id, conn_state, pair_state, disc_reason);
    osal_printk("[uuid server] connect state changed addr:%02x:**:**:**:%02x:%02x\r\n", addr->addr[BT_INDEX_0],
                addr->addr[BT_INDEX_4], addr->addr[BT_INDEX_5]);
    g_sle_conn_hdl = conn_id;
    if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
    }
}

static void sle_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    osal_printk("[uuid server] pair complete conn_id:%02x, status:%x\r\n", conn_id, status);
    osal_printk("[uuid server] pair complete addr:%02x:**:**:**:%02x:%02x\r\n", addr->addr[BT_INDEX_0],
                addr->addr[BT_INDEX_4], addr->addr[BT_INDEX_5]);
    g_sle_pair_hdl = conn_id + 1;
    ssap_exchange_info_t param = {0};
    param.mtu_size = 520; /*520 :MTU SIZE*/
    param.version = 1;    /*1: VERSION*/
    ssaps_set_info(g_server_id, &param);
}

static void sle_conn_register_cbks(void)
{
    sle_connection_callbacks_t conn_cbks = {0};
    conn_cbks.connect_state_changed_cb = sle_connect_state_changed_cbk;
    conn_cbks.pair_complete_cb = sle_pair_complete_cbk;
    sle_connection_register_callbacks(&conn_cbks);
}

/* 初始化uuid server */
errcode_t sle_uuid_server_init(void)
{
    uart_entry();
    enable_sle();
    sle_conn_register_cbks();
    sle_ssaps_register_cbks();
    sle_uuid_server_add();
    sle_uuid_server_adv_init();
    osal_printk("[uuid server] init ok\r\n");
    return ERRCODE_SLE_SUCCESS;
}

#define SLE_UUID_SERVER_TASK_PRIO 24
#define SLE_UUID_SERVER_STACK_SIZE 0x2000
#define GET_CURRENT_TIME_PIO 27
#define GET_CURRENT_TIME_STACK_SIZE 0x1000

static void sle_uuid_server_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)sle_uuid_server_init, 0, "sle_uuid_server",
                                      SLE_UUID_SERVER_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SLE_UUID_SERVER_TASK_PRIO);
        osal_kfree(task_handle);
    }

    task_handle =
        osal_kthread_create((osal_kthread_handler)get_current_time, 0, "get_time", GET_CURRENT_TIME_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, GET_CURRENT_TIME_PIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the app entry. */
app_run(sle_uuid_server_entry);
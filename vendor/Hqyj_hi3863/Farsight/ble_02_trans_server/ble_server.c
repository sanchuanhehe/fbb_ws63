/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 * Description: ble uuid server sample.
 */

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "osal_addr.h"
#include "bts_def.h"
#include "securec.h"
#include "errcode.h"
#include "test_suite_uart.h"
#include "bts_def.h"
#include "bts_le_gap.h"
#include "bts_gatt_stru.h"
#include "bts_gatt_server.h"
#include "soc_osal.h"
#include "app_init.h"
#include "ble_server_adv.h"
#include "ble_server.h"
#include "uart.h"
#include "pinctrl.h"
#include "common_def.h"
#include "platform_core.h"
/* 串口接收数据结构体 */
typedef struct {
    uint8_t *value;
    uint16_t value_len;
} msg_data_t;
/* 消息队列结构体 */
unsigned long g_msg_queue = 0;
unsigned int msg_rev_size = sizeof(msg_data_t);
/* ble connect state */
uint8_t g_connection_state = 0;
/* 任务相关*/
#define BLE_SERVER_TASK_PRIO 24
#define BLE_SERVER_STACK_SIZE 0x2000
/*串口接收缓冲区大小*/
#define UART_RX_MAX 512
uint8_t uart_rx_buffer[UART_RX_MAX];
/* 串口接收io*/
#define CONFIG_UART0_TXD_PIN 17
#define CONFIG_UART0_RXD_PIN 18
#define CONFIG_UART0_PIN_MODE 1

/* max transport unit, default is 512 */
static uint16_t g_uart_mtu = 512;
/* server app uuid for test */
char g_uuid_app_uuid[] = {0x0, 0x0};
static uint8_t g_ble_uart_server_addr[] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0x00 };
/* ble notification att handle */
uint16_t g_notification_characteristic_att_hdl = 0;

/* ble connect handle */
uint16_t g_conn_hdl = 0;

/* ble server id */
uint8_t g_server_id = 0;
/* octets of 16 bits */
#define UUID_LEN_2 2


/* 将uint16的uuid数字转化为bt_uuid_t */
void stream_data_to_uuid(uint16_t uuid_data, bt_uuid_t *out_uuid)
{
    char uuid[] = {(uint8_t)(uuid_data >> 8), (uint8_t)uuid_data};  //保存服务的高8位和低8位
    out_uuid->uuid_len = UUID_LEN_2;
    if (memcpy_s(out_uuid->uuid, out_uuid->uuid_len, uuid, UUID_LEN_2) != EOK) {    //将服务的uuid拷贝出来
        return;
    }
}

errcode_t compare_service_uuid(bt_uuid_t *uuid1, bt_uuid_t *uuid2)
{
    if (uuid1->uuid_len != uuid2->uuid_len) {
        return ERRCODE_BT_FAIL;
    }
    if (memcmp(uuid1->uuid, uuid2->uuid, uuid1->uuid_len) != 0) {
        return ERRCODE_BT_FAIL;
    }
    return ERRCODE_BT_SUCCESS;
}

/* 添加HID服务的所有特征和描述符  添加描述符：客户端特性配置 */
static void ble_server_add_characters_and_descriptors(uint32_t server_id, uint32_t srvc_handle)
{
    bt_uuid_t server_uuid = {0};
    uint8_t server_value[] = {0x12, 0x34};
    osal_printk("[ble_server_add_characters] Beginning add characteristic\r\n");
    stream_data_to_uuid(BLE_UUID_UUID_SERVER_REPORT, &server_uuid);
    gatts_add_chara_info_t character;
    character.chara_uuid = server_uuid;
    character.properties = GATT_CHARACTER_PROPERTY_BIT_READ  | GATT_CHARACTER_PROPERTY_BIT_NOTIFY |GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP;
    character.permissions =  GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_WRITE;
    character.value_len = sizeof(server_value);
    character.value = server_value;
    gatts_add_characteristic(server_id, srvc_handle, &character);

     bt_uuid_t ccc_uuid = {0};
    uint8_t ccc_data_val[] = {0x00, 0x00};
    osal_printk("[ble_server_add_descriptors] Beginning add descriptors\r\n");
    stream_data_to_uuid(BLE_UUID_CLIENT_CHARACTERISTIC_CONFIGURATION, &ccc_uuid);
    gatts_add_desc_info_t descriptor;
    descriptor.desc_uuid = ccc_uuid;
    descriptor.permissions = GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_WRITE ;
    descriptor.value_len = sizeof(ccc_data_val);
    descriptor.value = ccc_data_val;
    gatts_add_descriptor(server_id, srvc_handle, &descriptor);
    osal_vfree(ccc_uuid.uuid);
    
}

/* 服务添加回调 */
static void ble_server_service_add_cbk(uint8_t server_id, bt_uuid_t *uuid, uint16_t handle, errcode_t status)
{
    bt_uuid_t service_uuid = {0};
    osal_printk("ble_server_service_add_cbk] Add service cbk: server: %d, status: %d, srv_handle: %d, uuid_len: %d,uuid:",
        server_id, status, handle, uuid->uuid_len);
    for (int8_t i = 0; i < uuid->uuid_len ; i++) {
        osal_printk("%02x", (uint8_t)uuid->uuid[i]);
    }
    osal_printk("\n");
    stream_data_to_uuid(BLE_UUID_UUID_SERVER_SERVICE, &service_uuid);
    if (compare_service_uuid(uuid, &service_uuid) == ERRCODE_BT_SUCCESS) {
        ble_server_add_characters_and_descriptors(server_id, handle);
        osal_printk("[ble_server_service_add_cbk] Start service\r\n");
        gatts_start_service(server_id, handle);
    } else {
        osal_printk("[ble_server_service_add_cbk] Unknown service uuid\r\n");
        return;
    }
}

/* 特征添加回调 */
static void  ble_server_characteristic_add_cbk(uint8_t server_id, bt_uuid_t *uuid, uint16_t service_handle,
    gatts_add_character_result_t *result, errcode_t status)
{
    int8_t i = 0;
    osal_printk("[ble_server_characteristic_add_cbk] Add characteristic cbk: server: %d, status: %d, srv_hdl: %d "\
        "char_hdl: %x, char_val_hdl: %x, uuid_len: %d, uuid: ",
        server_id, status, service_handle, result->handle, result->value_handle, uuid->uuid_len);
    for (i = 0; i < uuid->uuid_len ; i++) {
        osal_printk("%02x", (uint8_t)uuid->uuid[i]);
    }
    osal_printk("\n");
    g_notification_characteristic_att_hdl = result->value_handle;
}

/* 描述符添加回调 */
static void  ble_server_descriptor_add_cbk(uint8_t server_id, bt_uuid_t *uuid, uint16_t service_handle,
    uint16_t handle, errcode_t status)
{
    int8_t i = 0;
    osal_printk("[ble_server_descriptor_add_cbk] Add descriptor cbk : server: %d, status: %d, srv_hdl: %d, desc_hdl: %x ,"\
        "uuid_len:%d, uuid: ", server_id, status, service_handle, handle, uuid->uuid_len);
    for (i = 0; i < uuid->uuid_len ; i++) {
        osal_printk("%02x", (uint8_t)uuid->uuid[i]);
    }
    osal_printk("\n");
}

/* 开始服务回调 */
static void ble_server_service_start_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
    osal_printk("[ble_server_service_start_cbk] Start service cbk : server: %d status: %d srv_hdl: %d\n", server_id, handle, status);
}
/* 写特征值请求回调*/
static void ble_server_receive_write_req_cbk(uint8_t server_id, uint16_t conn_id,gatts_req_write_cb_t *write_cb_para, errcode_t status)
{
    osal_printk("[ble_server_receive_write_req_cbk]server_id:%d conn_id:%d,status:%d ", server_id, conn_id,status);
    osal_printk("request_id:%d att_handle:%d\n", write_cb_para->request_id, write_cb_para->handle);
    osal_printk("data_len:%d data:\n", write_cb_para->length);
    for (uint16_t i = 0; i < write_cb_para->length; i++) {
        osal_printk("%c", write_cb_para->value[i]);
    }
    osal_printk("\n");
}
/* 读特征值请求回调*/
static void ble_server_receive_read_req_cbk(uint8_t server_id, uint16_t conn_id,gatts_req_read_cb_t *read_cb_para, errcode_t status)
{
    osal_printk("[ble_server_receive_read_req_cbk]server_id:%d conn_id:%d,status:%d\n", server_id, conn_id,status);
    osal_printk("request_id:%d att_handle:%d \n",read_cb_para->request_id, read_cb_para->handle);
}

static void ble_server_adv_enable_cbk(uint8_t adv_id, adv_status_t status)
{
    osal_printk("[ble_server_adv_enable_cbk] Adv enable adv_id: %d, status:%d\n", adv_id, status);
 
}

static void ble_server_adv_disable_cbk(uint8_t adv_id, adv_status_t status)
{
    osal_printk("[ble_server_adv_disable_cbk] adv_id: %d, status:%d\n",
        adv_id, status);
}


void ble_server_connect_change_cbk(uint16_t conn_id, bd_addr_t *addr, gap_ble_conn_state_t conn_state,
    gap_ble_pair_state_t pair_state, gap_ble_disc_reason_t disc_reason)
{
    osal_printk("[ble_server_connect_change_cbk] Connect state change conn_id: %d, status: %d, pair_status:%d, addr %x disc_reason %x\n",
        conn_id, conn_state, pair_state, addr[0], disc_reason);
    g_conn_hdl = conn_id;
    g_connection_state = (uint8_t)conn_state;

}

void ble_server_mtu_changed_cbk(uint8_t server_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status)
{
    osal_printk("[ble_server_mtu_changed_cbk] Mtu change change server_id: %d, conn_id: %d, mtu_size: %d, status:%d \n",
        server_id, conn_id, mtu_size, status);
}

void ble_server_enable_cbk(errcode_t status)
{
    osal_printk("[ble_server_enable_cbk] Ble server enable status: %d\n", status);
    if(gatts_set_mtu_size(g_server_id,g_uart_mtu) == ERRCODE_SUCC)
    {
         osal_printk("gatts_set_mtu_size 100\n");
    }
}


/* 2.创建服务 */
uint8_t ble_uuid_add_service(void)
{
    osal_printk("[ble_uuid_add_service] Ble uuid add service in\r\n");
    bt_uuid_t service_uuid = {0};
    stream_data_to_uuid(BLE_UUID_UUID_SERVER_SERVICE, &service_uuid);//将UUID转换到结构体中
    gatts_add_service(g_server_id, &service_uuid, true);               //添加服务
    osal_printk("[ble_uuid_add_service] Ble uuid add service out\r\n");
    return ERRCODE_BT_SUCCESS;
}

/* device向host发送数据：report */
errcode_t ble_server_send_report(uint8_t *data, uint16_t len)
{
        gatts_ntf_ind_t param = { 0 };
        uint16_t conn_id = g_conn_hdl;
        param.attr_handle = g_notification_characteristic_att_hdl;
        param.value_len = len;
        param.value = data;
        osal_printk("send input report indicate_handle:%d\n",  g_notification_characteristic_att_hdl);
        gatts_notify_indicate(g_server_id, conn_id, &param);
        return ERRCODE_BT_SUCCESS;
}

static errcode_t ble_server_register_callbacks(void)
{
    errcode_t ret;
    gap_ble_callbacks_t gap_cb = {0};
    gatts_callbacks_t service_cb = {0};

    gap_cb.start_adv_cb = ble_server_adv_enable_cbk;//开启广播回调函数
    gap_cb.stop_adv_cb = ble_server_adv_disable_cbk;//广播关闭回调函数
    gap_cb.conn_state_change_cb = ble_server_connect_change_cbk;//连接状态改变回调函数
    gap_cb.ble_enable_cb = ble_server_enable_cbk;//开启蓝牙回调函数
    ret = gap_ble_register_callbacks(&gap_cb);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("[ ble_server_register_callbacks ] Reg gap cbk failed\r\n");
        return ERRCODE_BT_FAIL;
    }

    service_cb.add_service_cb = ble_server_service_add_cbk;
    service_cb.add_characteristic_cb = ble_server_characteristic_add_cbk;
    service_cb.add_descriptor_cb = ble_server_descriptor_add_cbk;
    service_cb.start_service_cb = ble_server_service_start_cbk;
    service_cb.read_request_cb = ble_server_receive_read_req_cbk;
    service_cb.write_request_cb = ble_server_receive_write_req_cbk;
    service_cb.mtu_changed_cb = ble_server_mtu_changed_cbk;
    ret = gatts_register_callbacks(&service_cb);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("[ ble_server_register_callbacks ] Reg service cbk failed\r\n");
        return ERRCODE_BT_FAIL;
    }
    return ret;
}

/* 初始化uuid server service */
void ble_server_init(void)
{
    osal_printk("BLE SERVER ENTRY.\n");
    /* 注册GATT server用户回调函数 */
    ble_server_register_callbacks(); 
    osal_printk("BLE START ENABLE.\n");
    /* 打开蓝牙开关 */
    enable_ble();

    bt_uuid_t app_uuid = {0};
    bd_addr_t ble_addr = { 0 };
    app_uuid.uuid_len = sizeof(g_uuid_app_uuid);
    if (memcpy_s(app_uuid.uuid, app_uuid.uuid_len, g_uuid_app_uuid, sizeof(g_uuid_app_uuid)) != EOK) {
        return ;
    }
     ble_addr.type = BLE_PUBLIC_DEVICE_ADDRESS;
    if (memcpy_s(ble_addr.addr, BD_ADDR_LEN, g_ble_uart_server_addr, sizeof(g_ble_uart_server_addr)) != EOK) {
        osal_printk("add server app addr memcpy failed\n");
        return;
    }
    gap_ble_set_local_addr(&ble_addr);
    /* 1.创建一个server */
    gatts_register_server(&app_uuid, &g_server_id);
    /* 2.根据UUID创建service */
    ble_uuid_add_service();
    /* 3.设置广播参数 */
    ble_start_adv();
}

void ble_main_task(void *argument)
{
    argument=argument;
    ble_server_init();
    app_uart_init_config();
    while (1)
    {
        msg_data_t msg_data = { 0 };
        int msg_ret = osal_msg_queue_read_copy(g_msg_queue, &msg_data, &msg_rev_size, OSAL_WAIT_FOREVER);
        if (msg_ret != OSAL_SUCCESS) {
            osal_printk("msg queue read copy fail.");
            if (msg_data.value != NULL) {
                osal_vfree(msg_data.value);
            }
            continue;
        }
        if (msg_data.value != NULL) {
            ble_server_send_report(msg_data.value, msg_data.value_len);
            osal_vfree(msg_data.value);
        }
    }
}
static void ble_server_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    int ret = osal_msg_queue_create("ble_msg", msg_rev_size, &g_msg_queue, 0, msg_rev_size);
    if (ret != OSAL_SUCCESS) {
        osal_printk("create queue failure!,error:%x\n", ret);
    }

    task_handle= osal_kthread_create((osal_kthread_handler)ble_main_task, 0, "ble_main_task",
                                      BLE_SERVER_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, BLE_SERVER_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the app entry. */
app_run(ble_server_entry);

void ble_uart_server_read_handler(const void *buffer, uint16_t length, bool error)
{
    osal_printk("ble_uart_server_read_handler.\r\n");
    unused(error);
    if (  g_connection_state != 0) {
        msg_data_t msg_data = { 0 };
        void* buffer_cpy = osal_vmalloc(length);
        if (memcpy_s(buffer_cpy, length, buffer, length) != EOK) {
            osal_vfree(buffer_cpy);
            return;
        }
        msg_data.value = (uint8_t *)buffer_cpy;
        msg_data.value_len = length;
        osal_msg_queue_write_copy(g_msg_queue, (void *)&msg_data, msg_rev_size, 0);
    }
}
void app_uart_init_config(void)
{
    uart_buffer_config_t uart_buffer_config;
    uapi_pin_set_mode(CONFIG_UART0_TXD_PIN, CONFIG_UART0_PIN_MODE);
    uapi_pin_set_mode(CONFIG_UART0_RXD_PIN, CONFIG_UART0_PIN_MODE);
    uart_attr_t attr = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity = UART_PARITY_NONE
        };
    uart_buffer_config.rx_buffer_size = 512;
    uart_buffer_config.rx_buffer = uart_rx_buffer;
    uart_pin_config_t pin_config = {.tx_pin = S_MGPIO0, .rx_pin = S_MGPIO1, .cts_pin = PIN_NONE, .rts_pin = PIN_NONE};
    uapi_uart_deinit(UART_BUS_0); // 重点，UART初始化之前需要去初始化，否则会报0x80001044
    int res = uapi_uart_init(UART_BUS_0, &pin_config, &attr, NULL, &uart_buffer_config);
    if (res != 0) {
        printf("uart init failed res = %02x\r\n", res);
    }
     if (uapi_uart_register_rx_callback(UART_BUS_0, UART_RX_CONDITION_MASK_IDLE, 1, ble_uart_server_read_handler) == ERRCODE_SUCC) {
        osal_printk("uart%d int mode register receive callback succ!\r\n", UART_BUS_0);
    }
}

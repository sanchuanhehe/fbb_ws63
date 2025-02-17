/*
 * Copyright (c) 2024 HiSilicon Technologies CO., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "osal_list.h"
#include "bts_le_gap.h"
#include "bts_gatt_client.h"
#include "soc_osal.h"
#include "app_init.h"
#include "securec.h"
#include "uart.h"
#include "common_def.h"
#include "pinctrl.h"
#include "platform_core.h"
#include "ble_client.h"
/* 串口接收数据结构体 */
typedef struct {
    uint8_t *value;
    uint16_t value_len;
} msg_data_t;
/* 消息队列结构体 */
unsigned long g_msg_queue = 0;
unsigned int msg_rev_size = sizeof(msg_data_t);
/* 串口接收缓冲区大小 */
#define UART_RX_MAX 512
uint8_t uart_rx_buffer[UART_RX_MAX];
/* 串口接收io */
#define CONFIG_UART1_TXD_PIN 17
#define CONFIG_UART1_RXD_PIN 18
#define CONFIG_UART1_PIN_MODE 1
/* 任务相关 */
#define BLE_UUID_CLIENT_TASK_PRIO 24
#define BLE_UUID_CLIENT_STACK_SIZE 0x2000
/* octets of 16 bits uart */
#define UUID16_LEN 2
#define DEFAULT_SCAN_INTERVAL 0x48

/* client id, invalid client id is "0" */
uint8_t g_client_id = 0;
/* ble connect id */
uint8_t g_conn_id = 0;
/* ble connect state */
uint8_t g_connection_state = 0;
/* characteristic handle */
static uint16_t g_ble_chara_hanle_write_value = 0;
/* max transport unit, default is 512 */
static uint16_t g_uart_mtu = 512;
/* client app uuid for test */
static bt_uuid_t g_client_app_uuid = {UUID16_LEN, {0}};

errcode_t ble_gatt_client_discover_all_service(uint16_t conn_id)
{
    errcode_t ret = ERRCODE_BT_SUCCESS;
    osal_printk("[%s] \n", __FUNCTION__);
    bt_uuid_t service_uuid = {0}; /* uuid length is zero, discover all service */
    ret |= gattc_discovery_service(g_client_id, conn_id, &service_uuid);
    return ret;
}
/* 发现服务回调函数 */
static void ble_client_discover_service_cbk(uint8_t client_id,
                                            uint16_t conn_id,
                                            gattc_discovery_service_result_t *service,
                                            errcode_t status)
{
    gattc_discovery_character_param_t param = {0};
    osal_printk("[GATTClient]Discovery service----client:%d conn_id:%d\n", client_id, conn_id);
    osal_printk("            start handle:%d end handle:%d uuid_len:%d\n            uuid:", service->start_hdl,
                service->end_hdl, service->uuid.uuid_len);
    for (uint8_t i = 0; i < service->uuid.uuid_len; i++) {
        osal_printk("%02x", service->uuid.uuid[i]);
    }
    osal_printk("\n            status:%d\n", status);
    param.service_handle = service->start_hdl;
    param.uuid.uuid_len = 0; /* uuid length is zero, discover all character */
    gattc_discovery_character(g_client_id, conn_id, &param);
}
/* 发现特征回调函数 */
static void ble_client_discover_character_cbk(uint8_t client_id,
                                              uint16_t conn_id,
                                              gattc_discovery_character_result_t *character,
                                              errcode_t status)
{
    osal_printk("[GATTClient]Discovery character----client:%d conn_id:%d uuid_len:%d\n            uuid:", client_id,
                conn_id, character->uuid.uuid_len);
    for (uint8_t i = 0; i < character->uuid.uuid_len; i++) {
        osal_printk("%02x", character->uuid.uuid[i]);
    }
    osal_printk("\n            declare handle:%d value handle:%d properties:%x\n", character->declare_handle,
                character->value_handle, character->properties);
    osal_printk("            status:%d\n", status);
    g_ble_chara_hanle_write_value = character->value_handle;
    osal_printk(" g_client_id:%d value_handle:%d\n", g_client_id, character->value_handle);
    gattc_discovery_descriptor(g_client_id, conn_id, character->declare_handle);
}
/* 发现特征描述符回调函数 */
static void ble_client_discover_descriptor_cbk(uint8_t client_id,
                                               uint16_t conn_id,
                                               gattc_discovery_descriptor_result_t *descriptor,
                                               errcode_t status)
{
    osal_printk("[GATTClient]Discovery descriptor----client:%d conn_id:%d uuid len:%d\n            uuid:", client_id,
                conn_id, descriptor->uuid.uuid_len);
    for (uint8_t i = 0; i < descriptor->uuid.uuid_len; i++) {
        osal_printk("%02x", descriptor->uuid.uuid[i]);
    }
    osal_printk("\n            descriptor handle:%d\n", descriptor->descriptor_hdl);
    osal_printk("            status:%d\n", status);
}
/* 发现服务完成回调函数 */
static void ble_client_discover_service_compl_cbk(uint8_t client_id,
                                                  uint16_t conn_id,
                                                  bt_uuid_t *uuid,
                                                  errcode_t status)
{
    osal_printk("[GATTClient]Discovery service complete----client:%d conn_id:%d uuid len:%d\n            uuid:",
                client_id, conn_id, uuid->uuid_len);
    for (uint8_t i = 0; i < uuid->uuid_len; i++) {
        osal_printk("%02x", uuid->uuid[i]);
    }
    osal_printk("            status:%d\n", status);
}
/* 发现特征完成回调函数 */
static void ble_client_discover_character_compl_cbk(uint8_t client_id,
                                                    uint16_t conn_id,
                                                    gattc_discovery_character_param_t *param,
                                                    errcode_t status)
{
    osal_printk("[GATTClient]Discovery character complete----client:%d conn_id:%d uuid len:%d\n            uuid:",
                client_id, conn_id, param->uuid.uuid_len);
    for (uint8_t i = 0; i < param->uuid.uuid_len; i++) {
        osal_printk("%02x", param->uuid.uuid[i]);
    }
    osal_printk("\n            service handle:%d\n", param->service_handle);
    osal_printk("            status:%d\n", status);
}
/* 发现特征描述符完成回调函数 */
static void ble_client_discover_descriptor_compl_cbk(uint8_t client_id,
                                                     uint16_t conn_id,
                                                     uint16_t character_handle,
                                                     errcode_t status)
{
    osal_printk("[GATTClient]Discovery descriptor complete----client:%d conn_id:%d\n", client_id, conn_id);
    osal_printk("            charatcer handle:%d\n", character_handle);
    osal_printk("            status:%d\n", status);
}
/* 收到读响应回调函数 */
static void ble_client_read_cfm_cbk(uint8_t client_id,
                                    uint16_t conn_id,
                                    gattc_handle_value_t *read_result,
                                    gatt_status_t status)
{
    osal_printk("[GATTClient]Read result----client:%d conn_id:%d\n", client_id, conn_id);
    osal_printk("            handle:%d data_len:%d\ndata:", read_result->handle, read_result->data_len);
    for (uint16_t i = 0; i < read_result->data_len; i++) {
        osal_printk("%02x", read_result->data[i]);
    }
    osal_printk("\n            status:%d\n", status);
}
/* 按照uuid读取完成回调函数 */
static void ble_client_read_compl_cbk(uint8_t client_id,
                                      uint16_t conn_id,
                                      gattc_read_req_by_uuid_param_t *param,
                                      errcode_t status)
{
    osal_printk("[GATTClient]Read by uuid complete----client:%d conn_id:%d\n", client_id, conn_id);
    osal_printk("start handle:%d end handle:%d uuid len:%d\n            uuid:", param->start_hdl, param->end_hdl,
                param->uuid.uuid_len);
    for (uint16_t i = 0; i < param->uuid.uuid_len; i++) {
        osal_printk("%02x", param->uuid.uuid[i]);
    }
    osal_printk("\n            status:%d\n", status);
}
/* 写特征值回调函数 */
errcode_t ble_client_write_cmd(uint8_t *data, uint16_t len, uint16_t handle)
{
    gattc_handle_value_t uart_handle_value = {0};
    uart_handle_value.handle = handle;
    uart_handle_value.data_len = len;
    uart_handle_value.data = data;
    osal_printk(" ble_client_write_cmd len: %d, g_client_id: %d ", len, g_client_id);
    for (uint16_t i = 0; i < len; i++) {
        osal_printk("%c", data[i]);
    }
    osal_printk("\n");
    errcode_t ret = gattc_write_cmd(g_client_id, g_conn_id, &uart_handle_value);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk(" gattc_write_data failed\n");
        return ERRCODE_BT_FAIL;
    }
    return ERRCODE_BT_SUCCESS;
}
/* 收到写响应回调函数 */
static void ble_client_write_cfm_cbk(uint8_t client_id, uint16_t conn_id, uint16_t handle, gatt_status_t status)
{
    osal_printk("[GATTClient]Write result----client:%d conn_id:%d handle:%d\n", client_id, conn_id, handle);
    osal_printk("            status:%d\n", status);
}
/* mtu改变回调函数 */
static void ble_client_mtu_changed_cbk(uint8_t client_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status)
{
    osal_printk("[GATTClient]Mtu changed----client:%d conn_id:%d mtu size:%d\n", client_id, conn_id, mtu_size);
    osal_printk("            status:%d\n", status);
}
/* 收到通知回调函数 */
static void ble_client_notification_cbk(uint8_t client_id,
                                        uint16_t conn_id,
                                        gattc_handle_value_t *data,
                                        errcode_t status)
{
    osal_printk("[GATTClient]Receive notification----client:%d conn_id:%d status:%d\n", client_id, conn_id, status);
    osal_printk("handle:%d data_len:%d data:", data->handle, data->data_len);
    for (uint16_t i = 0; i < data->data_len; i++) {
        osal_printk("%c", data->data[i]);
    }
}
/* 收到指示回调函数 */
static void ble_client_indication_cbk(uint8_t client_id, uint16_t conn_id, gattc_handle_value_t *data, errcode_t status)
{
    osal_printk("[GATTClient]Receive indication----client:%d conn_id:%d\n", client_id, conn_id);
    osal_printk("            handle:%d data_len:%d\ndata:", data->handle, data->data_len);
    for (uint16_t i = 0; i < data->data_len; i++) {
        osal_printk("%02x", data->data[i]);
    }
    osal_printk("\n            status:%d\n", status);
}

/* 蓝牙协议栈使能回调 */
void ble_client_enable_cbk(errcode_t status)
{
    osal_printk("[ ble_client_enable_cbk ] Ble enable: %d\n", status);
}

/* 扫描结果回调函数 */
static void ble_client_scan_result_cbk(gap_scan_result_data_t *scan_result_data)
{
    uint8_t ble_mac[BD_ADDR_LEN] = {0x00, 0x90, 0x78, 0x56, 0x34, 0x12};
    if (memcmp(scan_result_data->addr.addr, ble_mac, BD_ADDR_LEN) == 0) {
        osal_printk("[%s]Find The Target Device.\n", __FUNCTION__);
        gap_ble_stop_scan();
        bd_addr_t client_addr = {0};
        client_addr.type = scan_result_data->addr.type;
        if (memcpy_s(client_addr.addr, BD_ADDR_LEN, scan_result_data->addr.addr, BD_ADDR_LEN) != EOK) {
            osal_printk(" add server app addr memcpy failed\r\n");
            return;
        }
        gap_ble_connect_remote_device(&client_addr);
    } else {
        osal_printk("\naddr:");
        for (uint8_t i = 0; i < BD_ADDR_LEN; i++) {
            osal_printk(" %02x:", scan_result_data->addr.addr[i]);
        }
    }
}
/* 连接状态改变回调函数 */
static void ble_client_conn_state_change_cbk(uint16_t conn_id,
                                             bd_addr_t *addr,
                                             gap_ble_conn_state_t conn_state,
                                             gap_ble_pair_state_t pair_state,
                                             gap_ble_disc_reason_t disc_reason)
{
    osal_printk("[%s] Connect state change conn_id: %d, status: %d, pair_status:%d, disc_reason %x\n", __FUNCTION__,
                conn_id, conn_state, pair_state, disc_reason);
    g_conn_id = conn_id;
    g_connection_state = conn_state;
    if (conn_state == GAP_BLE_STATE_CONNECTED) {
        gap_ble_pair_remote_device(addr);
        gattc_exchange_mtu_req(g_client_id, g_conn_id, g_uart_mtu);
    }
}
/* 配对完成回调函数 */
static void ble_client_pair_result_cbk(uint16_t conn_id, const bd_addr_t *addr, errcode_t status)
{
    unused(addr);
    osal_printk("[%s] Pair result conn_id: %d,status: %d \n", __FUNCTION__, conn_id, status);
    gattc_exchange_mtu_req(g_client_id, g_conn_id, g_uart_mtu);
    ble_gatt_client_discover_all_service(conn_id);
}
/* 连接参数更新回调函数 */
static void ble_client_conn_param_update_cbk(uint16_t conn_id,
                                             errcode_t status,
                                             const gap_ble_conn_param_update_t *param)
{
    osal_printk("[%s] conn_param_update conn_id: %d,status: %d \n", __FUNCTION__, conn_id, status);
    osal_printk("interval:%d latency:%d timeout:%d.\n", param->interval, param->latency, param->timeout);
}

errcode_t ble_gatt_client_callback_register(void)
{
    errcode_t ret = ERRCODE_BT_UNHANDLED;
    gap_ble_callbacks_t gap_cb = {0};
    gap_cb.ble_enable_cb = ble_client_enable_cbk;
    gap_cb.scan_result_cb = ble_client_scan_result_cbk;
    gap_cb.conn_state_change_cb = ble_client_conn_state_change_cbk;
    gap_cb.pair_result_cb = ble_client_pair_result_cbk;
    gap_cb.conn_param_update_cb = ble_client_conn_param_update_cbk;
    ret |= gap_ble_register_callbacks(&gap_cb);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("Reg gap cbk failed ret = %d\n", ret);
    }
    gattc_callbacks_t cb = {0};
    cb.discovery_svc_cb = ble_client_discover_service_cbk;
    cb.discovery_svc_cmp_cb = ble_client_discover_service_compl_cbk;
    cb.discovery_chara_cb = ble_client_discover_character_cbk;
    cb.discovery_chara_cmp_cb = ble_client_discover_character_compl_cbk;
    cb.discovery_desc_cb = ble_client_discover_descriptor_cbk;
    cb.discovery_desc_cmp_cb = ble_client_discover_descriptor_compl_cbk;
    cb.read_cb = ble_client_read_cfm_cbk;
    cb.read_cmp_cb = ble_client_read_compl_cbk;
    cb.write_cb = ble_client_write_cfm_cbk;
    cb.mtu_changed_cb = ble_client_mtu_changed_cbk;
    cb.notification_cb = ble_client_notification_cbk;
    cb.indication_cb = ble_client_indication_cbk;
    ret |= gattc_register_callbacks(&cb);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("Reg gatt cbk failed ret = %d\n", ret);
    }
    return ret;
}
/* 扫描设置并开启 */
errcode_t ble_cliant_start_scan(void)
{
    errcode_t ret = ERRCODE_BT_SUCCESS;
    gap_ble_scan_params_t ble_device_scan_params = {0};
    ble_device_scan_params.scan_interval = DEFAULT_SCAN_INTERVAL;
    ble_device_scan_params.scan_window = DEFAULT_SCAN_INTERVAL;
    ble_device_scan_params.scan_type = 0x00;
    ble_device_scan_params.scan_phy = GAP_BLE_PHY_2M;
    ble_device_scan_params.scan_filter_policy = 0x00;
    ret |= gap_ble_set_scan_parameters(&ble_device_scan_params);
    ret |= gap_ble_start_scan();
    return ret;
}

errcode_t ble_client_init(void)
{
    errcode_t ret = ERRCODE_BT_SUCCESS;
    osal_printk("BLE CLIENT ENTRY.\n");
    ret |= ble_gatt_client_callback_register();

    ret |= enable_ble();
    osal_printk("[ ble_client_init ] Enable_ble ret = %x\n", ret);
    ret |= gattc_register_client(&g_client_app_uuid, &g_client_id);
    osal_printk("[ ble_client_init ] Ble_gatt_client_init, ret:%x.\n", ret);
    ret |= ble_cliant_start_scan();
    osal_printk("[ ble_client_init ] Ble_client_start_scan, ret:%x.\n", ret);
    return ret;
}

void ble_main_task(void)
{
    ble_client_init();
    app_uart_init_config();

    while (1) {
        msg_data_t msg_data = {0};
        int msg_ret = osal_msg_queue_read_copy(g_msg_queue, &msg_data, &msg_rev_size, OSAL_WAIT_FOREVER);
        if (msg_ret != OSAL_SUCCESS) {
            osal_printk("msg queue read copy fail.");
            if (msg_data.value != NULL) {
                osal_vfree(msg_data.value);
            }
            continue;
        }

        if (msg_data.value != NULL) {
            uint16_t write_handle = g_ble_chara_hanle_write_value;
            ble_client_write_cmd(msg_data.value, msg_data.value_len, write_handle);
            osal_vfree(msg_data.value);
        }
    }
}

static void ble_client_entry(void)
{
    int ret = osal_msg_queue_create("ble_msg", msg_rev_size, &g_msg_queue, 0, msg_rev_size);
    if (ret != OSAL_SUCCESS) {
        osal_printk("create queue failure!,error:%x\n", ret);
    }
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle =
        osal_kthread_create((osal_kthread_handler)ble_main_task, 0, "ble_gatt_client", BLE_UUID_CLIENT_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, BLE_UUID_CLIENT_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the app entry. */
app_run(ble_client_entry);

/* 串口接收回调 */
void ble_uart_client_read_handler(const void *buffer, uint16_t length, bool error)
{
    osal_printk("ble_uart_client_read_handler.\r\n");
    unused(error);

    if (g_connection_state != 0) {
        msg_data_t msg_data = {0};
        void *buffer_cpy = osal_vmalloc(length);
        if (memcpy_s(buffer_cpy, length, buffer, length) != EOK) {
            osal_vfree(buffer_cpy);
            return;
        }
        msg_data.value = (uint8_t *)buffer_cpy;
        msg_data.value_len = length;
        osal_msg_queue_write_copy(g_msg_queue, (void *)&msg_data, msg_rev_size, 0);
    }
}

/* 串口初始化配置 */
void app_uart_init_config(void)
{
    uart_buffer_config_t uart_buffer_config;
    uapi_pin_set_mode(CONFIG_UART1_TXD_PIN, CONFIG_UART1_PIN_MODE);
    uapi_pin_set_mode(CONFIG_UART1_RXD_PIN, CONFIG_UART1_PIN_MODE);
    uart_attr_t attr = {
        .baud_rate = 115200, .data_bits = UART_DATA_BIT_8, .stop_bits = UART_STOP_BIT_1, .parity = UART_PARITY_NONE};
    uart_buffer_config.rx_buffer_size = 512;
    uart_buffer_config.rx_buffer = uart_rx_buffer;
    uart_pin_config_t pin_config = {.tx_pin = S_MGPIO0, .rx_pin = S_MGPIO1, .cts_pin = PIN_NONE, .rts_pin = PIN_NONE};
    uapi_uart_deinit(UART_BUS_0);
    int res = uapi_uart_init(UART_BUS_0, &pin_config, &attr, NULL, &uart_buffer_config);
    if (res != 0) {
        printf("uart init failed res = %02x\r\n", res);
    }
    if (uapi_uart_register_rx_callback(UART_BUS_0, UART_RX_CONDITION_MASK_IDLE, 1, ble_uart_client_read_handler) ==
        ERRCODE_SUCC) {
        osal_printk("uart%d int mode register receive callback succ!\r\n", UART_BUS_0);
    }
}

/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 *
 * Description: SLE private service register sample of client.
 */
#include "app_init.h"
#include "systick.h"
#include "los_memory.h"
#include "securec.h"
#include "soc_osal.h"
#include "common_def.h"
#include "sle_device_discovery.h"
#include "sle_connection_manager.h"
#include "sle_errcode.h"
#include "sle_ssap_client.h"

#include "pinctrl.h"
#include "uart.h"

/* sle define */
#define SLE_UART_TASK_PRIO              26
#define SLE_UART_STACK_SIZE             0x2000

#define SLE_MTU_SIZE_DEFAULT            1500
#define SLE_SEEK_INTERVAL_DEFAULT       100
#define SLE_SEEK_WINDOW_DEFAULT         100
#define UUID_16BIT_LEN                  2
#define UUID_128BIT_LEN                 16
#define UART_DEFAULT_CONN_INTERVAL      0x64
#define UART_DEFAULT_TIMEOUT_MULTIPLIER 0x1f4
#define UART_DEFAULT_SCAN_INTERVAL      400
#define UART_DEFAULT_SCAN_WINDOW        20
#define UART_DURATION_MS                2000

static sle_announce_seek_callbacks_t g_seek_cbk = {0};
static sle_connection_callbacks_t    g_connect_cbk = {0};
static ssapc_callbacks_t             g_ssapc_cbk = {0};
static sle_addr_t                    g_remote_addr = {0};
static uint16_t                      g_conn_id = 0;
static ssapc_find_service_result_t   g_find_service_result = {0};
static bool                          g_bis_conn = false;
static uint16_t                      g_sle_send_handle = 0;

/* uart define */
#define CONFIG_SLE_UART_BUS 0
#define CONFIG_UART_TXD_PIN 17
#define CONFIG_UART_RXD_PIN 18
#define SLE_UART_BAUDRATE   115200
#define SLE_UART_TRANSFER_SIZE   256

static uint8_t g_app_uart_rx_buff[SLE_UART_TRANSFER_SIZE] = {0};

static uart_buffer_config_t g_app_uart_buffer_config = {
    .rx_buffer = g_app_uart_rx_buff,
    .rx_buffer_size = SLE_UART_TRANSFER_SIZE
};

/* uart func */
static void uart_init_pin(void)
{
    uapi_pin_set_mode(CONFIG_UART_TXD_PIN, PIN_MODE_1);
    uapi_pin_set_mode(CONFIG_UART_RXD_PIN, PIN_MODE_1);
    return;
}

static void uart_init_config(void)
{
    uart_attr_t attr = {
        .baud_rate = SLE_UART_BAUDRATE,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity = UART_PARITY_NONE
    };

    uart_pin_config_t pin_config = {
        .tx_pin = CONFIG_UART_TXD_PIN,
        .rx_pin = CONFIG_UART_RXD_PIN,
        .cts_pin = PIN_NONE,
        .rts_pin = PIN_NONE
    };
    uapi_uart_deinit(CONFIG_SLE_UART_BUS);
    uapi_uart_init(CONFIG_SLE_UART_BUS, &pin_config, &attr, NULL, &g_app_uart_buffer_config);
    return;
}

static void sle_uart_read_int_handler(const void *buffer, uint16_t length, bool error)
{
    unused(error);
    uint8_t value[SLE_UART_TRANSFER_SIZE+1] = {0};
    (void)memcpy_s(value, SLE_UART_TRANSFER_SIZE, buffer, length);
    osal_printk("uart%d get %d length data: %s\r\n", CONFIG_SLE_UART_BUS, length, value);

    ssapc_write_param_t param = {0};
    param.handle = g_sle_send_handle;
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.data_len = length;
    param.data = value;
    (void)memcpy_s(param.data, param.data_len, buffer, length);
    if (true == g_bis_conn) {
        ssapc_write_req(0, g_conn_id, &param);
    } else {
        osal_printk("sle is not connected, please connect first\r\n");
    }
}

static void sle_uart_init(void)
{
    /* UART pinmux */
    uart_init_pin();

    /* UART init config */
    uart_init_config();

    errcode_t ret = uapi_uart_register_rx_callback(CONFIG_SLE_UART_BUS,
                                                    UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
                                                    1, sle_uart_read_int_handler);
    if (ret != ERRCODE_SUCC) {
        osal_printk("[Sle Client] uart%d register rx callback fail!\r\n", CONFIG_SLE_UART_BUS);
    }

    return;
}


/* sle func */
void sle_uart_connect_param_init(void)
{
    sle_default_connect_param_t param = {0};
    param.enable_filter_policy = 0;
    param.gt_negotiate = 0;
    param.initiate_phys = 1;
    param.max_interval = UART_DEFAULT_CONN_INTERVAL;
    param.min_interval = UART_DEFAULT_CONN_INTERVAL;
    param.scan_interval = UART_DEFAULT_SCAN_INTERVAL;
    param.scan_window = UART_DEFAULT_SCAN_WINDOW;
    param.timeout = UART_DEFAULT_TIMEOUT_MULTIPLIER;
    sle_default_connection_param_set(&param);
}

void sle_start_scan(void)
{
    sle_seek_param_t param = {0};
    param.own_addr_type = 0;
    param.filter_duplicates = 0;
    param.seek_filter_policy = 0;
    param.seek_phys = 1;
    param.seek_type[0] = 0;
    param.seek_interval[0] = SLE_SEEK_INTERVAL_DEFAULT;
    param.seek_window[0] = SLE_SEEK_WINDOW_DEFAULT;
    sle_set_seek_param(&param);
    sle_start_seek();
}

void sle_sample_sle_enable_cbk(errcode_t status)
{
    if (status == 0) {
        uint8_t local_addr[SLE_ADDR_LEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
        sle_addr_t local_address;
        local_address.type = 0;
        (void)memcpy_s(local_address.addr, SLE_ADDR_LEN, local_addr, SLE_ADDR_LEN);
        sle_set_local_addr(&local_address);
        sle_uart_connect_param_init();
        sle_start_scan();
    }
}

void sle_sample_seek_enable_cbk(errcode_t status)
{
    if (status == 0) {
        return;
    }
}

void sle_sample_seek_disable_cbk(errcode_t status)
{
    if (status == 0) {
        sle_connect_remote_device(&g_remote_addr);
    }
}

void sle_sample_seek_result_info_cbk(sle_seek_result_info_t *seek_result_data)
{
    if (seek_result_data != NULL) {
        uint8_t mac[SLE_ADDR_LEN] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
        if (memcmp(seek_result_data->addr.addr, mac, SLE_ADDR_LEN) == 0) {
            (void)memcpy_s(&g_remote_addr, sizeof(sle_addr_t), &seek_result_data->addr, sizeof(sle_addr_t));
            sle_stop_seek();
        }
    }
}

static void sle_uart_notification_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data,
    errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);
    uapi_uart_write(CONFIG_SLE_UART_BUS, (uint8_t *)(data->data), data->data_len, 0);
}

static void sle_uart_indication_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data,
    errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);
    osal_printk("\n sle uart receive indication from conn[%d] : %s\r\n", conn_id, data->data);
}

void sle_sample_seek_cbk_register(void)
{
    g_seek_cbk.sle_enable_cb = sle_sample_sle_enable_cbk;
    g_seek_cbk.seek_enable_cb = sle_sample_seek_enable_cbk;
    g_seek_cbk.seek_disable_cb = sle_sample_seek_disable_cbk;
    g_seek_cbk.seek_result_cb = sle_sample_seek_result_info_cbk;
}

void sle_sample_connect_state_changed_cbk(uint16_t conn_id, const sle_addr_t *addr,
    sle_acb_state_t conn_state, sle_pair_state_t pair_state, sle_disc_reason_t disc_reason)
{
    osal_printk("[ssap client] connect state changed conn_id:0x%02x, conn_state:0x%x, pair_state:0x%x, \
        disc_reason:0x%x\r\n", conn_id, conn_state, pair_state, disc_reason);
    osal_printk("[ssap client] conn state changed addr:0x%02x:**:**:0x%02x:0x%02x\n",
        addr->addr[0], addr->addr[4], addr->addr[5]); /* 0 4 5: addr index */
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        if (pair_state == SLE_PAIR_NONE) {
            sle_pair_remote_device(&g_remote_addr);
        }
        g_conn_id = conn_id;
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        sle_remove_paired_remote_device(addr);
        sle_start_scan();
        g_bis_conn = false;
    }
}

void sle_sample_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    osal_printk("[ssap client] pair complete conn_id:%d, addr:0x%02x:**:**:0x%02x:0x%02x\n", conn_id, addr->addr[0],
        addr->addr[4], addr->addr[5]); /* 0 4 5: addr index */
    if (status == ERRCODE_SLE_SUCCESS) {
        ssap_exchange_info_t info = {0};
        info.mtu_size = SLE_MTU_SIZE_DEFAULT;
        info.version = 1;
        ssapc_exchange_info_req(1, g_conn_id, &info);
        return;
    }
    osal_printk("[uart server] pair failed, remove pair and restart scan\r\n");
    sle_remove_paired_remote_device(addr);
    sle_start_scan();
}

void sle_sample_update_cbk(uint16_t conn_id, errcode_t status, const sle_connection_param_update_evt_t *param)
{
    unused(status);
    osal_printk("[ssap client] updat state changed conn_id:%d, interval = 0x%02x\n", conn_id, param->interval);
}

void sle_sample_update_req_cbk(uint16_t conn_id, errcode_t status, const sle_connection_param_update_req_t *param)
{
    unused(conn_id);
    unused(status);
    osal_printk("[ssap client] sle_sample_update_req_cbk interval_min = 0x%02x, interval_max = 0x%02x\n",
        param->interval_min, param->interval_max);
}

void sle_sample_connect_cbk_register(void)
{
    g_connect_cbk.connect_state_changed_cb = sle_sample_connect_state_changed_cbk;
    g_connect_cbk.pair_complete_cb = sle_sample_pair_complete_cbk;
    g_connect_cbk.connect_param_update_req_cb = sle_sample_update_req_cbk;
    g_connect_cbk.connect_param_update_cb = sle_sample_update_cbk;
}

void sle_sample_exchange_info_cbk(uint8_t client_id, uint16_t conn_id, ssap_exchange_info_t *param,
    errcode_t status)
{
    osal_printk("[ssap client] pair complete client id:%d status:%d\n", client_id, status);
    osal_printk("[ssap client] exchange mtu, mtu size: %d, version: %d.\n",
        param->mtu_size, param->version);

    ssapc_find_structure_param_t find_param = {0};
    find_param.type = SSAP_FIND_TYPE_PROPERTY;  /* find property */
    find_param.start_hdl = 1;
    find_param.end_hdl = 0xFFFF;
    ssapc_find_structure(0, conn_id, &find_param);
}

void sle_sample_find_structure_cbk(uint8_t client_id, uint16_t conn_id, ssapc_find_service_result_t *service,
    errcode_t status)
{
    osal_printk("[ssap client] sle_sample_find_structure_cbk client: %d conn_id:%d status: %d \n",
        client_id, conn_id, status);
    osal_printk("[ssap client] find structure start_hdl:[0x%02x], end_hdl:[0x%02x], uuid len:%d\r\n",
        service->start_hdl, service->end_hdl, service->uuid.len);
    if (service->uuid.len == UUID_16BIT_LEN) {
        osal_printk("[ssap client] structure uuid:[0x%02x][0x%02x]\r\n",
            service->uuid.uuid[14], service->uuid.uuid[15]); /* 14 15: uuid index */
    } else {
        for (uint8_t idx = 0; idx < UUID_128BIT_LEN; idx++) {
            osal_printk("[ssap client] structure uuid[%d]:[0x%02x]\r\n", idx, service->uuid.uuid[idx]);
        }
    }
    g_find_service_result.start_hdl = service->start_hdl;
    g_find_service_result.end_hdl = service->end_hdl;
    if (memcpy_s(&g_find_service_result.uuid, sizeof(sle_uuid_t), &service->uuid, sizeof(sle_uuid_t)) != EOK) {
        osal_printk("[ssap client] find structure mem cpy failed");
        return;
    }
}

void sle_sample_find_structure_cmp_cbk(uint8_t client_id, uint16_t conn_id,
    ssapc_find_structure_result_t *structure_result, errcode_t status)
{
    osal_printk("[ssap client] sle_sample_find_structure_cmp_cbk client id:%d status:%d type:%d conn_id:%d\r\n",
        client_id, status, structure_result->type, conn_id);
}

void sle_sample_find_property_cbk(uint8_t client_id, uint16_t conn_id,
    ssapc_find_property_result_t *property, errcode_t status)
{
    g_sle_send_handle = property->handle;
    g_bis_conn = true;
    osal_printk("[ssap client] sle_sample_find_property_cbk, client id: %d, conn id: %d, operate ind: %d, "
        "descriptors count: %d status:%d \r\n", client_id, conn_id, property->operate_indication,
        property->descriptors_count, status);
}

void sle_sample_write_cfm_cbk(uint8_t client_id, uint16_t conn_id, ssapc_write_result_t *write_result,
    errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(write_result);
    unused(status);
}

void sle_sample_read_cfm_cbk(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *read_data,
    errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(read_data);
    unused(status);
}

void sle_sample_ssapc_cbk_register(ssapc_notification_callback notification_cb,
    ssapc_notification_callback indication_cb)
{
    g_ssapc_cbk.exchange_info_cb = sle_sample_exchange_info_cbk;
    g_ssapc_cbk.find_structure_cb = sle_sample_find_structure_cbk;
    g_ssapc_cbk.find_structure_cmp_cb = sle_sample_find_structure_cmp_cbk;
    g_ssapc_cbk.ssapc_find_property_cbk = sle_sample_find_property_cbk;
    g_ssapc_cbk.write_cfm_cb = sle_sample_write_cfm_cbk;
    g_ssapc_cbk.read_cfm_cb = sle_sample_read_cfm_cbk;
    g_ssapc_cbk.notification_cb = notification_cb;
    g_ssapc_cbk.indication_cb = indication_cb;
}

static void sle_client_init(void)
{
    osal_msleep(UART_DURATION_MS);
    sle_sample_seek_cbk_register();
    sle_sample_connect_cbk_register();
    sle_sample_ssapc_cbk_register(sle_uart_notification_cb, sle_uart_indication_cb);
    sle_announce_seek_register_callbacks(&g_seek_cbk);
    sle_connection_register_callbacks(&g_connect_cbk);
    ssapc_register_callbacks(&g_ssapc_cbk);
    if (enable_sle() != ERRCODE_SUCC) {
        osal_printk("[Sle Client] sle enbale fail!\r\n");
    }
    return;
}

static int sle_init(void)
{
    sle_uart_init();
    sle_client_init();
    return 0;
}

static void sle_uart_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)sle_init, 0, "sle_uart_client", SLE_UART_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SLE_UART_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the sle_uart_entry */
app_run(sle_uart_entry);
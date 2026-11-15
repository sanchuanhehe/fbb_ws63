/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023. All rights reserved.
 * Description: sle uuid server sample.
 */
#include "app_init.h"
#include "watchdog.h"
#include "systick.h"
#include "los_memory.h"
#include "securec.h"
#include "errcode.h"
#include "osal_addr.h"
#include "soc_osal.h"
#include "common_def.h"
#include "nv.h"
#include "sle_common.h"
#include "sle_errcode.h"
#include "sle_ssap_server.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "sle_uart_server_adv.h"
#include "sle_uart_server.h"

#include "pinctrl.h"
#include "uart.h"

/* sle server app uuid for test */
static char     g_sle_uuid_app_uuid[UUID_LEN_2] = {0x0, 0x0};
/* server notify property uuid for test */
static char     g_sle_property_value[OCTET_BIT_LEN] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static uint16_t g_conn_id = 0;          /* sle connect acb handle */
static uint8_t  g_server_id = 0;        /* sle server handle */
static uint16_t g_service_handle = 0;   /* sle service handle */
static uint16_t g_property_handle = 0;  /* sle ntf property handle */
static bool     g_bis_conn = false;


static uint8_t sle_uuid_base[] = { 0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA, \
    0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static void encode2byte_little(uint8_t *_ptr, uint16_t data)
{
    *(uint8_t *)((_ptr) + 1) = (uint8_t)((data) >> 0x08);
    *(uint8_t *)(_ptr) = (uint8_t)(data);
}

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

static void sle_uart_server_read_int_handler(const void *buffer, uint16_t length, bool error)
{
    unused(error);
    uint8_t value[SLE_UART_TRANSFER_SIZE+1] = {0};
    (void)memcpy_s(value, SLE_UART_TRANSFER_SIZE, buffer, length);
    osal_printk("uart%d get %d length data: %s\r\n", CONFIG_SLE_UART_BUS, length, value);

    ssaps_ntf_ind_t param = {0};
    param.handle = g_property_handle;
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.value_len = length;
    param.value = value;
    (void)memcpy_s(param.value, param.value_len, buffer, length);
    if (true == g_bis_conn) {
        ssaps_notify_indicate(g_server_id, g_conn_id, &param);
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
                                                    1, sle_uart_server_read_int_handler);
    if (ret != ERRCODE_SUCC) {
        osal_printk("uart%d register rx callback fail!\r\n", CONFIG_SLE_UART_BUS);
    }

    return;
}


/* sle func */
static void sle_uuid_set_base(sle_uuid_t *out)
{
    (void)memcpy_s(out->uuid, SLE_UUID_LEN, sle_uuid_base, SLE_UUID_LEN);
    out->len = UUID_LEN_2;
}

static void sle_uuid_setu2(uint16_t u2, sle_uuid_t *out)
{
    sle_uuid_set_base(out);
    out->len = UUID_LEN_2;
    encode2byte_little(&out->uuid[BT_INDEX_14], u2);
}

static void ssaps_read_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_read_cb_t *read_cb_para,
    errcode_t status)
{
    osal_printk("[uart server] ssaps read request cbk server_id:0x%x, conn_id:0x%x, handle:0x%x, status:0x%x\r\n",
        server_id, conn_id, read_cb_para->handle, status);
}

static void ssaps_write_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_write_cb_t *write_cb_para,
    errcode_t status)
{
    unused(server_id);
    unused(conn_id);
    unused(status);
    uapi_uart_write(CONFIG_SLE_UART_BUS, (uint8_t *)(write_cb_para->value), write_cb_para->length, 0);
}

static void ssaps_mtu_changed_cbk(uint8_t server_id, uint16_t conn_id,  ssap_exchange_info_t *mtu_size,
    errcode_t status)
{
    osal_printk("[uart server] ssaps write request cbk server_id:%d, conn_id:%d, mtu_size:%d, status:%d\r\n",
        server_id, conn_id, mtu_size->mtu_size, status);
}

static void ssaps_start_service_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
    osal_printk("[uart server] start service cbk server_id:%d, handle:%d, status:%d\r\n",
        server_id, handle, status);
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
        osal_printk("[uart server] sle uuid add service fail, ret:0x%x\r\n", ret);
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

    sle_uuid_setu2(SLE_UUID_SERVER_NTF_REPORT, &property.uuid);
    property.permissions = SLE_UUID_TEST_PROPERTIES;
    property.operate_indication = SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_NOTIFY;
    property.value = osal_vmalloc(sizeof(g_sle_property_value));
    if (property.value == NULL) {
        osal_printk("[uart server] sle property mem fail\r\n");
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(property.value, sizeof(g_sle_property_value), g_sle_property_value,
        sizeof(g_sle_property_value)) != EOK) {
        osal_vfree(property.value);
        osal_printk("[uart server] sle property mem cpy fail\r\n");
        return ERRCODE_SLE_FAIL;
    }
    ret = ssaps_add_property_sync(g_server_id, g_service_handle, &property, &g_property_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uart server] sle uuid add property fail, ret:0x%x\r\n", ret);
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    descriptor.permissions = SLE_UUID_TEST_DESCRIPTOR;
    descriptor.operate_indication = SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE;
    descriptor.type = SSAP_DESCRIPTOR_USER_DESCRIPTION;
    descriptor.value = ntf_value;
    descriptor.value_len = sizeof(ntf_value);

    ret = ssaps_add_descriptor_sync(g_server_id, g_service_handle, g_property_handle, &descriptor);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uart server] sle uuid add descriptor fail, ret:0x%x\r\n", ret);
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    osal_vfree(property.value);
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_uuid_server_add(void)
{
    errcode_t ret;
    sle_uuid_t app_uuid = {0};

    osal_printk("[uart server] sle uuid add service in\r\n");
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
    osal_printk("[uart server] sle uuid add service, server_id:0x%x, service_handle:0x%x, property_handle:0x%x\r\n",
        g_server_id, g_service_handle, g_property_handle);
    ret = ssaps_start_service(g_server_id, g_service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uart server] sle uuid add service fail, ret:0x%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }
    osal_printk("[uart server] sle uuid add service out\r\n");
    return ERRCODE_SLE_SUCCESS;
}

static void sle_connect_state_changed_cbk(uint16_t conn_id, const sle_addr_t *addr,
    sle_acb_state_t conn_state, sle_pair_state_t pair_state, sle_disc_reason_t disc_reason)
{
    osal_printk("[uart server] connect state changed conn_id:0x%02x, conn_state:0x%x, pair_state:0x%x, \
        disc_reason:0x%x\r\n", conn_id, conn_state, pair_state, disc_reason);
    osal_printk("[uart server] connect state changed addr:0x%02x:**:**:**:0x%02x:0x%02x\r\n",
        addr->addr[BT_INDEX_0], addr->addr[BT_INDEX_4], addr->addr[BT_INDEX_5]);
    g_conn_id = conn_id;
    if (conn_state ==  SLE_ACB_STATE_CONNECTED) {
        sle_set_data_len(g_conn_id, DEFAULT_SLE_UART_DATA_LEN);
        g_bis_conn = true;
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        sle_remove_paired_remote_device(addr);
        sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
        g_bis_conn = false;
    }
}

static void sle_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    osal_printk("[uart server] pair complete conn_id:0x%02x, status:0x%x\r\n",
        conn_id, status);
    osal_printk("[uart server] pair complete addr:0x%02x:**:**:**:0x%02x:0x%02x\r\n",
        addr->addr[BT_INDEX_0], addr->addr[BT_INDEX_4], addr->addr[BT_INDEX_5]);
    if (status == ERRCODE_SLE_SUCCESS) {
        return;
    }
    osal_printk("[uart server] pair failed, remove pair and restart adv\r\n");
    sle_remove_paired_remote_device(addr);
    sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
}

void sle_sample_update_cbk(uint16_t conn_id, errcode_t status, const sle_connection_param_update_evt_t *param)
{
    unused(status);
    osal_printk("[ssap server] updat state changed conn_id:%d, interval = 0x%02x\n", conn_id, param->interval);
}

void sle_sample_update_req_cbk(uint16_t conn_id, errcode_t status, const sle_connection_param_update_req_t *param)
{
    unused(conn_id);
    unused(status);
    osal_printk("[ssap server] sle_sample_update_req_cbk interval_min:0x%02x, interval_max:0x%02x\n",
        param->interval_min, param->interval_max);
}

void sle_sample_rssi_cbk(uint16_t conn_id, int8_t rssi, errcode_t status)
{
    osal_printk("[ssap server] conn_id:%d, rssi = %d, status = 0x%x\n", conn_id, rssi, status);
}

static void sle_conn_register_cbks(void)
{
    sle_connection_callbacks_t conn_cbks = {0};
    conn_cbks.connect_state_changed_cb = sle_connect_state_changed_cbk;
    conn_cbks.pair_complete_cb = sle_pair_complete_cbk;
    conn_cbks.connect_param_update_req_cb = sle_sample_update_req_cbk;
    conn_cbks.connect_param_update_cb = sle_sample_update_cbk;
    conn_cbks.read_rssi_cb = sle_sample_rssi_cbk;
    sle_connection_register_callbacks(&conn_cbks);
}

void sle_ssaps_set_info(void)
{
    ssap_exchange_info_t info = {0};
    info.mtu_size = DEFAULT_SLE_UART_MTU_SIZE;
    info.version = 1;
    ssaps_set_info(g_server_id, &info);
}

void sle_set_local_addr_init(void)
{
    sle_addr_t addr = {0};
    uint8_t mac[SLE_ADDR_LEN] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
    addr.type = 0;
    memcpy_s(addr.addr, SLE_ADDR_LEN, mac, SLE_ADDR_LEN);
    sle_set_local_addr(&addr);
}

void sle_uart_server_set_nv(void)
{
    uint16_t nv_value_len = 0;
    uint8_t nv_value = 0;
    uapi_nv_read(0x20A0, sizeof(uint16_t), &nv_value_len, &nv_value);
    if (nv_value != 7) {     // 7:btc功率档位
        nv_value = 7;       // 7:btc功率档位
        uapi_nv_write(0x20A0, (uint8_t *)&(nv_value), sizeof(nv_value));
    }
    osal_printk("[uart server] The value of nv is set to %d.\r\n", nv_value);
}

void sle_enable_server_cbk(void)
{
    sle_uart_server_set_nv();
    sle_uuid_server_add();
    sle_ssaps_set_info();
    sle_set_local_addr_init();
    sle_uuid_server_adv_init();
    osal_printk("[uart server] init ok\r\n");
}

void sle_server_init(void)
{
    osal_msleep(UART_DURATION_MS);
    uapi_watchdog_disable();
    sle_announce_register_cbks();
    sle_conn_register_cbks();
    sle_ssaps_register_cbks();
    enable_sle();
    printf("sle enable end.\r\n");
    return;
}

static int sle_init(void)
{
    sle_uart_init();
    sle_server_init();
    return 0;
}

static void sle_uart_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle= osal_kthread_create((osal_kthread_handler)sle_init, 0, "sle_uart_server", UART_DEFAULT_KTHREAD_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, UART_DEFAULT_KTHREAD_PROI);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the sle_uart_entry. */
app_run(sle_uart_entry);


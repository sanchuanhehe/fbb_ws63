/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
 *
 * @if Eng
 * @brief Implements advertising configuration for the SLE UART server.
 * @else
 * @brief 实现 SLE UART 服务端的广播配置。
 * @endif
 *
 * History: \n
 * 2024-05-18, Create file. \n
 */
#include "securec.h"
#include "errcode.h"
#include "soc_osal.h"
#include "sle_common.h"
#include "sle_uart_server.h"
#include "sle_device_discovery.h"
#include "sle_errcode.h"
#include "sle_uart_server_adv.h"

#define SLE_ANNOUNCE_TX_POWER_DBM 18
#include "string.h"

/* Connection interval: 15 ms; advertising interval: 25 ms. / 连接间隔：15 ms；广播间隔：25 ms。 */
#define SLE_CONN_INTV_MIN_DEFAULT CONFIG_SLE_UART_CONN_INTERVAL
#define SLE_CONN_INTV_MAX_DEFAULT CONFIG_SLE_UART_CONN_INTERVAL
#define SLE_ADV_INTERVAL_MIN_DEFAULT 0xC8
#define SLE_ADV_INTERVAL_MAX_DEFAULT 0xC8
#define SLE_CONN_SUPERVISION_TIMEOUT_DEFAULT 0x1F4
#define SLE_CONN_MAX_LATENCY 0x1F3

#define SLE_ADV_TX_POWER 10
#define SLE_ADV_HANDLE_DEFAULT 1
#define SLE_ADV_DATA_LEN_MAX 251
#define NAME_MAX_LENGTH 16

static uint8_t g_sle_local_name[NAME_MAX_LENGTH] = "uart_server";

#define SLE_UART_SERVER_LOG "[sle uart server]"

/**
 * @if Eng
 * @brief Encodes the local device name into advertising data.
 * @else
 * @brief 将本地设备名称编码到广播数据中。
 * @endif
 */
static uint16_t sle_set_adv_local_name(uint8_t *adv_data, uint16_t max_len)
{
    errno_t ret;
    uint8_t index = 0;

    uint8_t *local_name = g_sle_local_name;
    uint8_t local_name_len = sizeof(g_sle_local_name) - 1;
    adv_data[index++] = local_name_len + 1;
    adv_data[index++] = SLE_ADV_DATA_TYPE_COMPLETE_LOCAL_NAME;
    ret = memcpy_s(&adv_data[index], max_len - index, local_name, local_name_len);
    if (ret != EOK) {
        osal_printk("%s memcpy fail\r\n", SLE_UART_SERVER_LOG);
        return 0;
    }
    return (uint16_t)index + local_name_len;
}

/**
 * @if Eng
 * @brief Builds the advertising data payload.
 * @else
 * @brief 构造广播数据载荷。
 * @endif
 */
static uint16_t sle_set_adv_data(uint8_t *adv_data)
{
    size_t len = 0;
    uint16_t idx = 0;
    errno_t ret = 0;

    len = sizeof(struct sle_adv_common_value);
    struct sle_adv_common_value adv_disc_level = {
        .length = len - 1,
        .type = SLE_ADV_DATA_TYPE_DISCOVERY_LEVEL,
        .value = SLE_ANNOUNCE_LEVEL_NORMAL,
    };
    ret = memcpy_s(&adv_data[idx], SLE_ADV_DATA_LEN_MAX - idx, &adv_disc_level, len);
    if (ret != EOK) {
        return 0;
    }
    idx += len;

    len = sizeof(struct sle_adv_common_value);
    struct sle_adv_common_value adv_access_mode = {
        .length = len - 1,
        .type = SLE_ADV_DATA_TYPE_ACCESS_MODE,
        .value = 0,
    };
    ret = memcpy_s(&adv_data[idx], SLE_ADV_DATA_LEN_MAX - idx, &adv_access_mode, len);
    if (ret != EOK) {
        return 0;
    }
    idx += len;

    return idx;
}

/**
 * @if Eng
 * @brief Builds the scan response payload.
 * @else
 * @brief 构造扫描响应数据载荷。
 * @endif
 */
static uint16_t sle_set_scan_response_data(uint8_t *scan_rsp_data)
{
    uint16_t idx = 0;
    errno_t ret;
    size_t scan_rsp_data_len = sizeof(struct sle_adv_common_value);

    struct sle_adv_common_value tx_power_level = {
        .length = scan_rsp_data_len - 1,
        .type = SLE_ADV_DATA_TYPE_TX_POWER_LEVEL,
        .value = SLE_ADV_TX_POWER,
    };
    ret = memcpy_s(scan_rsp_data, SLE_ADV_DATA_LEN_MAX, &tx_power_level, scan_rsp_data_len);
    if (ret != EOK) {
        return 0;
    }
    idx += scan_rsp_data_len;

    idx += sle_set_adv_local_name(&scan_rsp_data[idx], SLE_ADV_DATA_LEN_MAX - idx);
    return idx;
}

/**
 * @if Eng
 * @brief Configures the default SLE advertising parameters.
 * @else
 * @brief 配置默认 SLE 广播参数。
 * @endif
 */
static int sle_set_default_announce_param(void)
{
    errno_t ret;
    sle_announce_param_t param = {0};
    unsigned char local_addr[SLE_ADDR_LEN] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    param.announce_mode = SLE_ANNOUNCE_MODE_CONNECTABLE_SCANABLE;
    param.announce_handle = SLE_ADV_HANDLE_DEFAULT;
    param.announce_gt_role = SLE_ANNOUNCE_ROLE_T_CAN_NEGO;
    param.announce_level = SLE_ANNOUNCE_LEVEL_NORMAL;
    param.announce_channel_map = SLE_ADV_CHANNEL_MAP_DEFAULT;
    param.announce_interval_min = SLE_ADV_INTERVAL_MIN_DEFAULT;
    param.announce_interval_max = SLE_ADV_INTERVAL_MAX_DEFAULT;
    param.conn_interval_min = SLE_CONN_INTV_MIN_DEFAULT;
    param.conn_interval_max = SLE_CONN_INTV_MAX_DEFAULT;
    param.conn_max_latency = SLE_CONN_MAX_LATENCY;
    param.conn_supervision_timeout = SLE_CONN_SUPERVISION_TIMEOUT_DEFAULT;
    param.announce_tx_power = SLE_ANNOUNCE_TX_POWER_DBM;
    param.own_addr.type = 0;
    ret = memcpy_s(param.own_addr.addr, SLE_ADDR_LEN, local_addr, SLE_ADDR_LEN);
    if (ret != EOK) {
        return 0;
    }
    return sle_set_announce_param(param.announce_handle, &param);
}

/**
 * @if Eng
 * @brief Configures the default SLE advertising data.
 * @else
 * @brief 配置默认 SLE 广播数据。
 * @endif
 */
static int sle_set_default_announce_data(void)
{
    errcode_t ret;
    uint8_t announce_data_len = 0;
    uint8_t seek_data_len = 0;
    sle_announce_data_t data = {0};
    uint8_t adv_handle = SLE_ADV_HANDLE_DEFAULT;
    uint8_t announce_data[SLE_ADV_DATA_LEN_MAX] = {0};
    uint8_t seek_rsp_data[SLE_ADV_DATA_LEN_MAX] = {0};

    announce_data_len = sle_set_adv_data(announce_data);
    data.announce_data = announce_data;
    data.announce_data_len = announce_data_len;

    seek_data_len = sle_set_scan_response_data(seek_rsp_data);
    data.seek_rsp_data = seek_rsp_data;
    data.seek_rsp_data_len = seek_data_len;

    ret = sle_set_announce_data(adv_handle, &data);
    if (ret == ERRCODE_SLE_SUCCESS) {
        osal_printk("%s set announce data success.\r\n", SLE_UART_SERVER_LOG);
    } else {
        osal_printk("%s set adv data fail.\r\n", SLE_UART_SERVER_LOG);
    }
    return ERRCODE_SLE_SUCCESS;
}

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c sle_uart_server_announce_enable_cbk.
 * @else
 * @brief 处理分发给 \c sle_uart_server_announce_enable_cbk 的异步事件。
 * @endif
 */
static void sle_uart_server_announce_enable_cbk(uint32_t announce_id, errcode_t status)
{
    osal_printk("%s announce enable cbk id:%02x, status:%x\r\n", SLE_UART_SERVER_LOG, announce_id, status);
}

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c sle_uart_server_announce_disable_cbk.
 * @else
 * @brief 处理分发给 \c sle_uart_server_announce_disable_cbk 的异步事件。
 * @endif
 */
static void sle_uart_server_announce_disable_cbk(uint32_t announce_id, errcode_t status)
{
    osal_printk("%s announce disable cbk id:%02x, status:%x\r\n", SLE_UART_SERVER_LOG, announce_id, status);
}

/**
 * @if Eng
 * @brief Handles the asynchronous event delivered to \c sle_uart_server_announce_terminal_cbk.
 * @else
 * @brief 处理分发给 \c sle_uart_server_announce_terminal_cbk 的异步事件。
 * @endif
 */
static void sle_uart_server_announce_terminal_cbk(uint32_t announce_id)
{
    osal_printk("%s announce terminal cbk id:%02x\r\n", SLE_UART_SERVER_LOG, announce_id);
}

/**
 * @if Eng
 * @brief Registers the callbacks required by \c sle_uart_server_announce_register_cbks.
 * @else
 * @brief 注册 \c sle_uart_server_announce_register_cbks 所需的回调函数。
 * @endif
 */
errcode_t sle_uart_server_announce_register_cbks(void)
{
    errcode_t ret = 0;
    sle_announce_seek_callbacks_t seek_cbks = {0};
    seek_cbks.announce_enable_cb = sle_uart_server_announce_enable_cbk;
    seek_cbks.announce_disable_cb = sle_uart_server_announce_disable_cbk;
    seek_cbks.announce_terminal_cb = sle_uart_server_announce_terminal_cbk;
    ret = sle_announce_seek_register_callbacks(&seek_cbks);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s register announce callbacks fail:%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

/**
 * @if Eng
 * @brief Initializes the feature implemented by \c sle_uart_server_adv_init.
 * @else
 * @brief 初始化 \c sle_uart_server_adv_init 对应的功能。
 * @endif
 */
errcode_t sle_uart_server_adv_init(void)
{
    errcode_t ret;
    sle_set_default_announce_param();
    sle_set_default_announce_data();
    ret = sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_start_announce fail:%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ret;
    }
    osal_printk("%s start announce success.\r\n", SLE_UART_SERVER_LOG);
    return ERRCODE_SLE_SUCCESS;
}

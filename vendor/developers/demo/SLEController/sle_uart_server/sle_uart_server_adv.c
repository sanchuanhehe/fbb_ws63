/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: sle adv config for sle uart server. \n
 *
 * History: \n
 * 2023-07-17, Create file. \n
 *
 * Code Comments: Written by Lamonce.
 * Last Modified: June 09, 2025 \n
 *
 * Introduction:
 * This file implements the advertising functionality for SLE UART server communications.
 * It provides complete advertising configuration and management, including advertisement
 * parameters setup, advertisement data formatting, and scan response data configuration.
 * The module supports standard SLE advertising modes, handles advertisement state callbacks,
 * and manages the advertisement lifecycle. Key features include customizable advertisement
 * intervals, transmit power control, device name broadcasting, and service data advertising.
 *
 * 简介：
 * 本文件实现了 SLE UART 服务端通信的广播功能。它提供了完整的广播配置和管理，
 * 包括广播参数设置、广播数据格式化和扫描响应数据配置。该模块支持标准 SLE 广播模式，
 * 处理广播状态回调，并管理广播生命周期。主要功能包括可自定义的广播间隔、发射功率控制、
 * 设备名称广播和服务数据广播。整个模块设计符合星闪低功耗通信协议规范，
 * 确保设备能被客户端高效发现和连接。
 *
 * DISCLAIMER:
 * This code is provided for reference and learning purposes only.
 * No warranty of correctness, completeness, or suitability for any purpose is provided.
 * Before using in production environments, please review and test thoroughly.
 *
 * 免责声明:
 * 本文件中的代码及注释仅供学习和参考使用，不保证其在所有环境下的正确性和完整性。
 * 在实际项目中使用前，请根据具体需求进行适当的修改和测试。
 *
 */
#include "securec.h"              // 安全函数库，用于替代标准 C 库中不安全的函数
#include "errcode.h"              // 错误码定义
#include "osal_addr.h"            // 地址相关函数
#include "product.h"              // 产品相关函数
#include "sle_common.h"           // 公共函数定义
#include "sle_uart_server.h"      // Server 端头文件
#include "sle_device_discovery.h" // 设备发现相关头文件
#include "sle_errcode.h"          // 错误码定义
#include "osal_debug.h"           // 调试相关函数
#include "osal_task.h"            // 任务相关函数
#include "string.h"               // 字符串相关函数
#include "sle_uart_server_adv.h"  // Server 端广播相关头文件

/* sle device name */
#define NAME_MAX_LENGTH 16
/* 连接调度间隔12.5ms，单位125us */
#define SLE_CONN_INTV_MIN_DEFAULT 0x64
/* 连接调度间隔12.5ms，单位125us */
#define SLE_CONN_INTV_MAX_DEFAULT 0x64
/* 连接调度间隔25ms，单位125us */
#define SLE_ADV_INTERVAL_MIN_DEFAULT 0xC8
/* 连接调度间隔25ms，单位125us */
#define SLE_ADV_INTERVAL_MAX_DEFAULT 0xC8
/* 超时时间5000ms，单位10ms */
#define SLE_CONN_SUPERVISION_TIMEOUT_DEFAULT 0x1F4
/* 超时时间4990ms，单位10ms */
#define SLE_CONN_MAX_LATENCY 0x1F3
/* 广播发送功率 */
#define SLE_ADV_TX_POWER 10
/* 广播ID */
#define SLE_ADV_HANDLE_DEFAULT 1
/* 最大广播数据长度 */
#define SLE_ADV_DATA_LEN_MAX 251
/* 广播名称 */

// static uint8_t sle_local_name[NAME_MAX_LENGTH] = "NearLink"; // 广播名称
#define SLE_SERVER_INIT_DELAY_MS 1000                              // 广播初始化延时
#define sample_at_log_print(fmt, args...) osal_printk(fmt, ##args) // 日志打印宏
#define SLE_UART_SERVER_LOG "[sle uart server]"                    // 日志前缀

static uint8_t sle_local_name[NAME_MAX_LENGTH] = {0};

int set_SLE_local_name(const uint8_t *name)
{
    if (name == NULL || strlen((char*)name) >= NAME_MAX_LENGTH)
    {
        osal_printk("%s set_SLE_local_name failed, name is null or too long\n", SLE_UART_SERVER_LOG);
        return -1; // 错误处理
    }
    // 复制本地名称到全局变量
    strncpy((char *)sle_local_name, (char *)name, NAME_MAX_LENGTH - 1);
    sle_local_name[NAME_MAX_LENGTH - 1] = '\0'; // 确保字符串以 null 结尾
    return 0;                                   // 成功
}

// 设置广播设备名称，也是本地名称
static uint16_t sle_set_adv_local_name(uint8_t *adv_data, uint16_t max_len)
{
    errno_t ret;
    uint8_t index = 0;

    uint8_t *local_name = sle_local_name;                                                   // 赋值本地名称
    uint8_t local_name_len = sizeof(sle_local_name) - 1;                                    // 不包括结束符
    sample_at_log_print("%s local_name_len = %d\r\n", SLE_UART_SERVER_LOG, local_name_len); // 日志
    sample_at_log_print("%s local_name: ", SLE_UART_SERVER_LOG);
    for (uint8_t i = 0; i < local_name_len; i++)
    {
        sample_at_log_print("0x%02x ", local_name[i]);
    }
    sample_at_log_print("\r\n");
    adv_data[index++] = local_name_len + 1;                                        // 长度+1
    adv_data[index++] = SLE_ADV_DATA_TYPE_COMPLETE_LOCAL_NAME;                     // 数据类型
    ret = memcpy_s(&adv_data[index], max_len - index, local_name, local_name_len); // 拷贝本地名称
    if (ret != EOK)
    {
        sample_at_log_print("%s memcpy fail\r\n", SLE_UART_SERVER_LOG);
        return 0;
    }
    return (uint16_t)index + local_name_len;
}

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
    if (ret != EOK)
    {
        sample_at_log_print("%s adv_disc_level memcpy fail\r\n", SLE_UART_SERVER_LOG);
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
    if (ret != EOK)
    {
        sample_at_log_print("%s adv_access_mode memcpy fail\r\n", SLE_UART_SERVER_LOG);
        return 0;
    }
    idx += len;

    return idx;
}

// 设置扫描响应数据
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
    if (ret != EOK)
    {
        sample_at_log_print("%s sle scan response data memcpy fail\r\n", SLE_UART_SERVER_LOG);
        return 0;
    }
    idx += scan_rsp_data_len;

    /* set local name */
    idx += sle_set_adv_local_name(&scan_rsp_data[idx], SLE_ADV_DATA_LEN_MAX - idx);
    return idx;
}

// 设置广播参数
static int sle_set_default_announce_param(void)
{
    errno_t ret;
    sle_announce_param_t param = {0};
    uint8_t index;
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
    param.announce_tx_power = 18;
    param.own_addr.type = 0;
    ret = memcpy_s(param.own_addr.addr, SLE_ADDR_LEN, local_addr, SLE_ADDR_LEN);
    if (ret != EOK)
    {
        sample_at_log_print("%s sle_set_default_announce_param data memcpy fail\r\n", SLE_UART_SERVER_LOG);
        return 0;
    }
    sample_at_log_print("%s sle_uart_local addr: ", SLE_UART_SERVER_LOG);
    for (index = 0; index < SLE_ADDR_LEN; index++)
    {
        sample_at_log_print("0x%02x ", param.own_addr.addr[index]);
    }
    sample_at_log_print("\r\n");
    return sle_set_announce_param(param.announce_handle, &param);
}

// 设置默认广播数据
static int sle_set_default_announce_data(void)
{
    errcode_t ret;
    uint8_t announce_data_len = 0;
    uint8_t seek_data_len = 0;
    sle_announce_data_t data = {0};
    uint8_t adv_handle = SLE_ADV_HANDLE_DEFAULT;
    uint8_t announce_data[SLE_ADV_DATA_LEN_MAX] = {0};
    uint8_t seek_rsp_data[SLE_ADV_DATA_LEN_MAX] = {0};
    uint8_t data_index = 0;

    announce_data_len = sle_set_adv_data(announce_data);
    data.announce_data = announce_data;
    data.announce_data_len = announce_data_len;

    sample_at_log_print("%s data.announce_data_len = %d\r\n", SLE_UART_SERVER_LOG, data.announce_data_len);
    sample_at_log_print("%s data.announce_data: ", SLE_UART_SERVER_LOG);
    for (data_index = 0; data_index < data.announce_data_len; data_index++)
    {
        sample_at_log_print("0x%02x ", data.announce_data[data_index]);
    }
    sample_at_log_print("\r\n");

    seek_data_len = sle_set_scan_response_data(seek_rsp_data);
    data.seek_rsp_data = seek_rsp_data;
    data.seek_rsp_data_len = seek_data_len;

    sample_at_log_print("%s data.seek_rsp_data_len = %d\r\n", SLE_UART_SERVER_LOG, data.seek_rsp_data_len);
    sample_at_log_print("%s data.seek_rsp_data: ", SLE_UART_SERVER_LOG);
    for (data_index = 0; data_index < data.seek_rsp_data_len; data_index++)
    {
        sample_at_log_print("0x%02x ", data.seek_rsp_data[data_index]);
    }
    sample_at_log_print("\r\n");

    ret = sle_set_announce_data(adv_handle, &data);
    if (ret == ERRCODE_SLE_SUCCESS)
    {
        sample_at_log_print("%s set announce data success.\r\n", SLE_UART_SERVER_LOG);
    }
    else
    {
        sample_at_log_print("%s set adv param fail.\r\n", SLE_UART_SERVER_LOG);
    }
    return ERRCODE_SLE_SUCCESS;
}

// 广播启动回调
static void sle_announce_enable_cbk(uint32_t announce_id, errcode_t status)
{
    sample_at_log_print("%s sle announce enable callback id:%02x, state:%x\r\n", SLE_UART_SERVER_LOG, announce_id,
                        status);
}

// 广播停止回调
static void sle_announce_disable_cbk(uint32_t announce_id, errcode_t status)
{
    sample_at_log_print("%s sle announce disable callback id:%02x, state:%x\r\n", SLE_UART_SERVER_LOG, announce_id,
                        status);
}

// 广播终止回调
static void sle_announce_terminal_cbk(uint32_t announce_id)
{
    sample_at_log_print("%s sle announce terminal callback id:%02x\r\n", SLE_UART_SERVER_LOG, announce_id);
}

// 注册广播回调函数
errcode_t sle_uart_announce_register_cbks(void)
{
    errcode_t ret = 0;
    sle_announce_seek_callbacks_t seek_cbks = {0};
    seek_cbks.announce_enable_cb = sle_announce_enable_cbk;
    seek_cbks.announce_disable_cb = sle_announce_disable_cbk;
    seek_cbks.announce_terminal_cb = sle_announce_terminal_cbk;
    ret = sle_announce_seek_register_callbacks(&seek_cbks);
    if (ret != ERRCODE_SLE_SUCCESS)
    {
        sample_at_log_print("%s sle_uart_announce_register_cbks,register_callbacks fail :%x\r\n",
                            SLE_UART_SERVER_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

// 初始化广播
errcode_t sle_uart_server_adv_init(void)
{
    errcode_t ret;
    sle_set_default_announce_param();
    sle_set_default_announce_data();
    ret = sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
    sample_at_log_print("%s sle_uart_server_adv_init,sle_start_announce devise name :%s\r\n",
                        SLE_UART_SERVER_LOG, sle_local_name);
    if (ret != ERRCODE_SLE_SUCCESS)
    {
        sample_at_log_print("%s sle_uart_server_adv_init,sle_start_announce fail :%x\r\n", SLE_UART_SERVER_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SLE private service register sample of client.
 */
/**
 * Copyright (c) kidwjb
 *
 * Description: sle GATE demo. \n
 *              This file is a sle GATE demo. \n
 *
 * History: \n
 * 2025-04-08, Create file. \n
 */
#include "pinctrl.h"
#include "common_def.h"
#include "soc_osal.h"
#include "osal_wait.h"
#include "app_init.h"
#include "sle_device_discovery.h"
#include "sle_connection_manager.h"
#include "sle_ssap_client.h"
#include "cmsis_os2.h"
#include "MQTTClientPersistence.h"
#include "MQTTClient.h"
#include "wifi_connect.h"
#include "lwip/sntp.h"
#include <sys/time.h>

#define SLE_SEEK_INTERVAL_DEFAULT 100
#define SLE_SEEK_WINDOW_DEFAULT 100
#define SLE_MTU_SIZE_DEFAULT 300
#define UUID_16BIT_LEN 2
#define UUID_128BIT_LEN 16
#define MSG_QUEUE_NUMBER 10
#define SLE_CONNNECT_MAX 8
#define DELAY_100MS 100
#define DELAY_200MS 200
#define DELAY_500MS 500
#define DELAY_5000MS 5000

#define SERVER_IP_ADDR "***************.myhuaweicloud.com" // 接入地址 hostname
#define SERVER_IP_PORT 1883                                // port 单片机一般使用MQTT，端口1883
#define CLIENT_ID "sle_gate_0_0_2025033107"                // client_id

#define MQTT_CMDTOPIC_SUB "$oc/devices/sle_gate/sys/commands/set/#" // 平台下发命令  订阅主题

#define MQTT_DATATOPIC_PUB "$oc/devices/sle_gate/sys/properties/report"                 // 属性上报topic 发布主题
#define MQTT_CLIENT_RESPONSE "$oc/devices/sle_gate/sys/commands/response/request_id=%s" // 命令响应topic

#define DATA_SEVER_NAME "Switch"
#define DATA_ATTR_NAME "LED_state"
#define DATA_ADC_SERVER_NAME "Sensor"
#define DATA_ADC_ATTR_NAME "ADC_value"

#define MQTT_DATA_SEND \
    "{\"services\": [\
    {\"service_id\": \"%s\", \"properties\": {\"%s\": %s}},\
    {\"service_id\": \"%s\", \"properties\": {\"%s\": %d}}\
]}"

#define KEEP_ALIVE_INTERVAL 120
#define DELAY_TIME_MS 200
#define IOT

#ifdef IOT
char *g_username = "sle_gate";
char *g_password = "4d50d2ec*************a04312";
#endif

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

char g_send_buffer[512] = {0}; // 发布数据缓冲区
char g_response_id[100] = {0}; // 保存命令id缓冲区

char g_response_buf[] =
    "{\"result_code\": 0,\"response_name\": \"LED\",\"paras\": {\"result\": \"success\"}}"; // 响应json

volatile MQTTClient_deliveryToken deliveredToken;

MQTTClient client;

uint8_t g_cmdFlag = 0;

osMessageQueueId_t MsgQueue_ID; // 消息队列的ID

typedef struct {
    uint8_t data[100];
    uint8_t connect_id;
} notify_data_t; // 自定义消息

notify_data_t notify_data_put;
notify_data_t notify_data_get;

uint8_t revert = 0;
uint8_t voltage = 0;

sle_addr_t g_remote_addr = {0};
sle_announce_seek_callbacks_t sle_announce_seek_callbacks = {0};
sle_connection_callbacks_t sle_connection_callbacks = {0};
ssapc_callbacks_t ssapc_callbacks = {0};
uint16_t g_conn_id[SLE_CONNNECT_MAX] = {0};
uint8_t connect_num;
ssapc_find_service_result_t g_find_service_result = {0};
ssapc_find_property_result_t ssapc_find_property_result = {0};
static sle_addr_t g_sle_uart_remote_addr = {0};
uint8_t wifi_if_connect = 0;

uint8_t remote_addr_key[SLE_CONNNECT_MAX][2] = {0}; // 创建键值对

//*******************sntp********************//
void time_convert(time_t *rawtime, uint8_t *send_time)
{
    uint8_t i = 0;
    uint64_t temp = (uint64_t)*rawtime;
    for (i = 0; i < 8; i++) {                       /*8: 一共个字节*/
        send_time[i] = temp >> (56 - i * 8) & 0xFF; /* 56位，一次8位*/
    }
}

int sntp_task(void)
{

    while (wifi_if_connect == 0) {
        osal_msleep(DELAY_100MS);
    }
    int ret;
    int server_num = 1;                 /*Number of SNTP servers available*/
    char *sntp_server = "pool.ntp.org"; /*sntp_server : List of the available servers*/
    struct timeval time_local;          /*Output Local time of server, which will be received in NTP
             response from server*/
    uint8_t send_time_data[11];
    send_time_data[INDEX0] = 0xAA;
    send_time_data[INDEX1] = 0x33;
    send_time_data[INDEX10] = 0xFF;
    ssapc_write_param_t param = {0};
    param.handle =
        2; /*2:
              必须是让句柄等于2才能够发送，这个是property的句柄值，但是这个程序不知为何无法正常接收到property句柄，所以直接赋值2*/
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.data_len = sizeof(send_time_data);
    memset(&time_local, 0, sizeof(time_local));
    // Start SNTP
    ret = lwip_sntp_start(server_num, &sntp_server, &time_local);
    if (ret != 0) {
        osal_printk("SNTP start failed with code: %d\r\n", ret);
        return ret;
    }

    // Print the received time from server
    osal_printk("Received time from server = [%li] sec [%li] u sec\r\n", time_local.tv_sec, time_local.tv_usec);
    struct timeval adjusted_time;
    adjusted_time.tv_sec = time_local.tv_sec + 8 * 3600; /*8 3600 :用于时间转换*/
    adjusted_time.tv_usec = time_local.tv_usec;
    if (settimeofday(&adjusted_time, NULL) == -1) {
        osal_printk("settimeofday FAIL\r\n");
        return -1;
    }

    while (1) {
        struct timeval current_time;
        if (gettimeofday(&current_time, NULL) == -1) {
            osal_printk("gettimeofday FAIL\r\n");
        }

        // Convert to local time
        time_t rawtime = current_time.tv_sec; // Get the time in seconds
        time_convert(&rawtime, &(send_time_data[INDEX2]));
        param.data = send_time_data;
        uint8_t i = 0;
        for (i = 0; i < connect_num; i++) {
            ssapc_write_req(0, g_conn_id[i], &param);
        }

        struct tm *local_time;
        // Convert to local time
        local_time = localtime(&rawtime);

        if (local_time == NULL) {
            printf("localtime failed\r\n");
            return -1;
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
    return ret;
}

//-----------------sntp----------------------//

//****************mqtt********************//
// 处理连接丢失回调函数
void connect_lost(void *context, char *cause)
{
    unused(context);
    osal_printk("connection lost : %s\r\n", cause);
}
//-----------------------------------//

// 接收消息处理//
void parse_after_equal(const char *input, char *output)
{
    const char *equalsign = strchr(input, '=');
    if (equalsign != NULL) {
        // 计算等号后面的字符串长度
        strcpy(output, equalsign + 1);
    }
}

// 处理接收到消息的回调函数
int message_arrive(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    unused(context);
    unused(topicLen);
    ssapc_write_param_t param = {0};
    param.handle =
        2; /*2:
              必须是让句柄等于2才能够发送，这个是property的句柄值，但是这个程序不知为何无法正常接收到property句柄，所以直接赋值2*/
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    uint8_t data[50];
    uint8_t len = sizeof(data);
    osal_printk("[Message receive topic]: %s\r\n", topicName);
    osal_printk("[Message] : %s \r\n", (char *)message->payload);
    // LED 控制
    if (strstr((char *)message->payload, "true") != NULL) {
        osal_printk("\r\n broker message is TRUE !!! \r\n");
        sprintf_s((char *)data, sizeof(data), "LED State: true");
    } else {
        osal_printk("\r\n broker message is FAUSE !!! \r\n");
        sprintf_s((char *)data, sizeof(data), "LED State: false");
    }
    uint8_t i = 0;
    for (i = 0; i <= connect_num; i++) {
        if (remote_addr_key[i][INDEX0] == 0x03) {
            osal_printk("find KEY-> uuid : %d  con_id : %d", remote_addr_key[i][INDEX0], remote_addr_key[i][INDEX0]);
            param.data_len = len;
            param.data = data;
            ssapc_write_req(0, remote_addr_key[i][INDEX0], &param); /*0 :client id */
            break;
        }
    }
    // 解析命令id
    parse_after_equal(topicName, g_response_id);
    g_cmdFlag = 1;
    memset((char *)message->payload, 0, message->payloadlen);

    return 1; // 1 表示消息已被处理
}
//----------------------------------------------//

// 处理消息传递完成回调函数
void delivered(void *context, MQTTClient_deliveryToken dt)
{
    unused(context);
    osal_printk("Message with token value %d delivery confirmed\r\n", dt);

    deliveredToken = dt;
}

//------------------------------------------------//

// 订阅主题
int mqtt_subscribe(const char *topic)
{
    osal_printk("subscribe start!\r\n");
    MQTTClient_subscribe(client, topic, 1); /*1 : qos */
    return 0;
}
//--------------------------------------------//

// 发布主题

int mqtt_publish(const char *topic, char *msg)
{
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int ret = 0;
    pubmsg.payload = msg;
    pubmsg.payloadlen = (int)strlen(msg);
    pubmsg.qos = 1;
    pubmsg.retained = 0;
    osal_printk("[payload]: %s,[topic]:%s\r\n", msg, topic);
    ret = MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    if (ret != MQTTCLIENT_SUCCESS) {
        osal_printk("mqtt publish failed \r\n");
        return ret;
    }
    return ret;
}

//-------------------------------------------//

static errcode_t mqtt_connect(void)
{
    int ret;
    MQTTClient_connectOptions mqtt_connect_opt =
        MQTTClient_connectOptions_initializer; // MQTT 3.1.1 non-WebSockets,适用于嵌入式设备
    // 初始化mqtt客户端
    MQTTClient_init();

    // 创建mqtt客户端
    ret = MQTTClient_create(&client, SERVER_IP_ADDR, CLIENT_ID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (ret != MQTTCLIENT_SUCCESS) {
        osal_printk("Failed to create MQTT client,return code %d\r\n", ret);
        return ERRCODE_FAIL;
    }
    mqtt_connect_opt.keepAliveInterval = KEEP_ALIVE_INTERVAL;
    mqtt_connect_opt.cleansession = 1;

#ifdef IOT
    mqtt_connect_opt.username = g_username;
    mqtt_connect_opt.password = g_password;
#endif

    // 创建回调函数
    MQTTClient_setCallbacks(client, NULL, connect_lost, message_arrive, delivered);

    // 尝试连接
    if ((ret = MQTTClient_connect(client, &mqtt_connect_opt)) != MQTTCLIENT_SUCCESS) {
        osal_printk("[MQTT_Task] : Failed to connect ,return code %d\r\n", ret);
        MQTTClient_destroy(&client); // 连接失败时销毁客户端
        return ERRCODE_FAIL;
    }
    osal_printk("[MQTT_Task] : Connected to MQTT brocker\r\n");
    osDelay(DELAY_TIME_MS);

    // 订阅MQTT主题
    mqtt_subscribe(MQTT_CMDTOPIC_SUB);
    wifi_if_connect = 1;
    while (1) {
        // 相应平台命令部分
        osDelay(DELAY_TIME_MS); // 需要延时，否则会发布失败
        if (g_cmdFlag) {
            sprintf(g_send_buffer, MQTT_CLIENT_RESPONSE, g_response_id);
            // 设备响应命令
            mqtt_publish(g_send_buffer, g_response_buf);

            g_cmdFlag = 0;
            memset(g_response_id, 0, sizeof(g_response_id) / sizeof(g_response_id[INDEX0]));
        }

        // 属性上报部分
        osDelay(DELAY_TIME_MS);
        // 发送LED状态
        memset(g_send_buffer, 0,
               sizeof(g_send_buffer) / sizeof(g_send_buffer[INDEX0])); // 要把buffer清零，不然上面buffer已有值会有些影响
        sprintf(g_send_buffer, MQTT_DATA_SEND, DATA_SEVER_NAME, DATA_ATTR_NAME, revert ? "true" : "false",
                DATA_ADC_SERVER_NAME, DATA_ADC_ATTR_NAME, voltage);
        mqtt_publish(MQTT_DATATOPIC_PUB, g_send_buffer);
        osDelay(DELAY_TIME_MS);
    }
    return ERRCODE_SUCC;
}

void mqtt_task(void)
{
    wifi_connect();
    osal_msleep(DELAY_200MS);
    mqtt_connect();
}

//---------------mqtt---------------------//

//***************queue*******************//

void queue_notification_deal_task(void)
{
    osStatus_t status = 0;
    while (1) {
        status = osMessageQueueGet(MsgQueue_ID, &notify_data_get, 0, 0);
        if (status == osOK) {
            osal_printk("con_id:%d queue deal :\r\n %s\r\n", notify_data_get.connect_id, notify_data_get.data);
            if (notify_data_get.data[INDEX1] == 0x11) {
                revert = notify_data_get.data[INDEX2];
            } else if (notify_data_get.data[INDEX1] == 0x22) {
                voltage = notify_data_get.data[INDEX2];
            }
        }
        osal_msleep(DELAY_500MS); // 每500ms处理一次队列中的数据
    }
}

//---------------queue-------------------//

//**************notification*************//

void sle_notification_cbk(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data, errcode_t status)
{
    unused(client_id);
    unused(status);
    notify_data_put.connect_id = conn_id;
    memcpy_s(notify_data_put.data, sizeof(notify_data_put.data), data->data, data->data_len);
    osMessageQueuePut(MsgQueue_ID, &notify_data_put, 0, 0); // 将数据入队
}
//----------------notification--------------//

//***************seek******************//
void sle_scan_start(void)
{
    sle_seek_param_t sle_seek_param = {0};
    sle_seek_param.own_addr_type = 0;     // 本端地址类型
    sle_seek_param.filter_duplicates = 0; // 重复过滤开关，0：关闭，1：开启
    sle_seek_param.seek_filter_policy = SLE_SEEK_FILTER_ALLOW_ALL;
    sle_seek_param.seek_phys = SLE_SEEK_PHY_1M;
    sle_seek_param.seek_type[0] = SLE_SEEK_PASSIVE;
    sle_seek_param.seek_interval[0] = SLE_SEEK_INTERVAL_DEFAULT;
    sle_seek_param.seek_window[0] = SLE_SEEK_WINDOW_DEFAULT;
    sle_set_seek_param(&sle_seek_param);
    sle_start_seek();
}
void sle_enable_cbk(errcode_t status)
{
    if (status == 0) {
        osal_printk("sle enable: %d.\r\n", status);
        sle_scan_start();
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
        (void)memcpy_s(&g_remote_addr, sizeof(sle_addr_t), &seek_result_data->addr, sizeof(sle_addr_t));
        sle_stop_seek();
    }
    memcpy_s(&g_sle_uart_remote_addr, sizeof(sle_addr_t), &seek_result_data->addr, sizeof(sle_addr_t));
    remote_addr_key[connect_num][INDEX0] = (uint8_t)seek_result_data->addr.addr[INDEX5];
}

void sle_seek_cbk_config(void)
{
    sle_announce_seek_callbacks.sle_enable_cb = sle_enable_cbk;
    sle_announce_seek_callbacks.seek_enable_cb = sle_sample_seek_enable_cbk;
    sle_announce_seek_callbacks.seek_disable_cb = sle_sample_seek_disable_cbk;
    sle_announce_seek_callbacks.seek_result_cb = sle_sample_seek_result_info_cbk;
}
//-------------------seek----------------//

//*******************connect********************//
void connect_state_changed_cbk(uint16_t conn_id,
                               const sle_addr_t *addr,
                               sle_acb_state_t conn_state,
                               sle_pair_state_t pair_state,
                               sle_disc_reason_t disc_reason)
{
    osal_printk("[ssap client] conn state changed conn_id:%d, addr:%02x***%02x%02x\n", conn_id, addr->addr[INDEX0],
                addr->addr[INDEX4], addr->addr[INDEX5]); /* 0 4 5: addr index */
    // osal_printk("[ssap client] conn state changed disc_reason:0x%x\n", disc_reason);
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        if (pair_state == SLE_PAIR_NONE) {
            sle_pair_remote_device(&g_remote_addr);
        }
        osal_printk("\r\n**** sle server addr:");
        uint8_t i = 0;
        for (i = 0; i < INDEX6; i++) {
            osal_printk("%02x", g_sle_uart_remote_addr.addr[i]);
        }
        osal_printk("\r\n");
        g_conn_id[connect_num] = conn_id;
        remote_addr_key[connect_num][INDEX1] = conn_id;
        connect_num++;
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        connect_num--;
        sle_remove_paired_remote_device(addr);
        sle_scan_start();
        osal_printk("sle disconnected,disreason : %d\r\n", disc_reason);
    }
    sle_start_seek();
}
void pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    osal_printk("[ssap client] pair complete conn_id:%d, addr:%02x***%02x%02x\n", conn_id, addr->addr[INDEX0],
                addr->addr[INDEX4], addr->addr[INDEX5]); /* 0 4 5: addr index */
    if (status == 0) {
        ssap_exchange_info_t info = {0};
        info.mtu_size = SLE_MTU_SIZE_DEFAULT;
        info.version = 1;
        ssapc_exchange_info_req(1, g_conn_id[connect_num], &info);
    }
}
void sle_connection_cbk_config(void)
{
    sle_connection_callbacks.connect_state_changed_cb = connect_state_changed_cbk; // 连接状态
    sle_connection_callbacks.pair_complete_cb = pair_complete_cbk;
}

//----------------------------connect-------------------------//

//*************************ssap****************************//
void exchange_info_cbk(uint8_t client_id, uint16_t conn_id, ssap_exchange_info_t *param, errcode_t status)
{
    osal_printk("[ssap client] pair complete client id:%d status:%d\n", client_id, status);
    osal_printk("[ssap client] exchange mtu, mtu size: %d, version: %d.\n", param->mtu_size, param->version);
    ssapc_find_structure_param_t find_param = {0};
    find_param.type = SSAP_FIND_TYPE_PRIMARY_SERVICE;
    find_param.start_hdl = 1;
    find_param.end_hdl = 0xFFFF;
    ssapc_find_structure(0, conn_id, &find_param);
}
void sle_sample_find_structure_cbk(uint8_t client_id,
                                   uint16_t conn_id,
                                   ssapc_find_service_result_t *service,
                                   errcode_t status)
{
    osal_printk("[ssap client] find structure cbk client: %d conn_id:%d status: %d \n", client_id, conn_id, status);
    osal_printk("[ssap client] find structure start_hdl:[0x%02x], end_hdl:[0x%02x], uuid len:%d\r\n",
                service->start_hdl, service->end_hdl, service->uuid.len);
    if (service->uuid.len == UUID_16BIT_LEN) {
        osal_printk("[ssap client] structure uuid:[0x%02x][0x%02x]\r\n", service->uuid.uuid[INDEX14],
                    service->uuid.uuid[INDEX15]); /* 14 15: uuid index */
    } else {
        for (uint8_t idx = 0; idx < UUID_128BIT_LEN; idx++) {
            osal_printk("[ssap client] structure uuid[%d]:[0x%02x]\r\n", idx, service->uuid.uuid[idx]);
        }
    }
    g_find_service_result.start_hdl = service->start_hdl;
    g_find_service_result.end_hdl = service->end_hdl;
    memcpy_s(&g_find_service_result.uuid, sizeof(sle_uuid_t), &service->uuid, sizeof(sle_uuid_t));
}
void sle_sample_find_property_cbk(uint8_t client_id,
                                  uint16_t conn_id,
                                  ssapc_find_property_result_t *property,
                                  errcode_t status)
{
    ssapc_find_property_result.handle = property->handle;
    osal_printk(
        "[ssap client] find property cbk, client id: %d, conn id: %d, operate indication: %d, "
        "descriptors count: %d status:%d.\n",
        client_id, conn_id, property->operate_indication, property->descriptors_count, status);
    for (uint16_t idx = 0; idx < property->descriptors_count; idx++) {
        osal_printk("[ssap client] find property cbk, descriptors type [%d]: 0x%02x.\n", idx,
                    property->descriptors_type[idx]);
    }
    if (property->uuid.len == UUID_16BIT_LEN) {
        osal_printk("[ssap client] find property cbk, uuid: %02x %02x.\n", property->uuid.uuid[INDEX14],
                    property->uuid.uuid[INDEX15]); /* 14 15: uuid index */
    } else if (property->uuid.len == UUID_128BIT_LEN) {
        for (uint16_t idx = 0; idx < UUID_128BIT_LEN; idx++) {
            osal_printk("[ssap client] find property cbk, uuid [%d]: %02x.\n", idx, property->uuid.uuid[idx]);
        }
    }
}

void find_structure_cmp_cbk(uint8_t client_id,
                            uint16_t conn_id,
                            ssapc_find_structure_result_t *structure_result,
                            errcode_t status)
{
    unused(conn_id);
    osal_printk("[ssap client] find structure cmp cbk client id:%d status:%d type:%d uuid len:%d \r\n", client_id,
                status, structure_result->type, structure_result->uuid.len);
    if (structure_result->uuid.len == UUID_16BIT_LEN) {
        osal_printk("[ssap client] find structure cmp cbk structure uuid:[0x%02x][0x%02x]\r\n",
                    structure_result->uuid.uuid[INDEX14], structure_result->uuid.uuid[INDEX15]); /* 14 15: uuid index */
    } else {
        for (uint8_t idx = 0; idx < UUID_128BIT_LEN; idx++) {
            osal_printk("[ssap client] find structure cmp cbk structure uuid[%d]:[0x%02x]\r\n", idx,
                        structure_result->uuid.uuid[idx]);
        }
    }
}
void sle_sample_write_cfm_cbk(uint8_t client_id, uint16_t conn_id, ssapc_write_result_t *write_result, errcode_t status)
{
    osal_printk("[ssap client] write cfm cbk, client id: %d status:%d.\n", client_id, status);
    ssapc_read_req(0, conn_id, write_result->handle, write_result->type);
}
void sle_sample_read_cfm_cbk(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *read_data, errcode_t status)
{
    osal_printk("[ssap client] read cfm cbk client id: %d conn id: %d status: %d\n", client_id, conn_id, status);
    osal_printk("[ssap client] read cfm cbk handle: %d, type: %d , len: %d\n", read_data->handle, read_data->type,
                read_data->data_len);
    for (uint16_t idx = 0; idx < read_data->data_len; idx++) {
        osal_printk("[ssap client] read cfm cbk[%d] 0x%02x\r\n", idx, read_data->data[idx]);
    }
}
void ssap_cbk_config(void)
{
    ssapc_callbacks.exchange_info_cb = exchange_info_cbk;
    ssapc_callbacks.find_structure_cb = sle_sample_find_structure_cbk;
    ssapc_callbacks.ssapc_find_property_cbk = sle_sample_find_property_cbk;
    ssapc_callbacks.find_structure_cmp_cb = find_structure_cmp_cbk;
    ssapc_callbacks.notification_cb = sle_notification_cbk;
}
//-------------------------ssap----------------------------//
void *sle_init(void)
{
    sle_seek_cbk_config();
    sle_announce_seek_register_callbacks(&sle_announce_seek_callbacks);
    sle_connection_cbk_config();
    sle_connection_register_callbacks(&sle_connection_callbacks);
    ssap_cbk_config();
    ssapc_register_callbacks(&ssapc_callbacks);
    enable_sle();
    return NULL;
}

#define SLE_UUID_CLIENT_TASK_PRIO 24
#define SLE_UUID_CLIENT_STACK_SIZE 0x2000
#define MSGQUEUE_TASK_PRIO 27
#define MSGQUEUE_TASK_STACK_SIZE 0x1000
#define MQTT_TASK_PRIO 26
#define MQTT_TASK_STACK_SIZE 0x2000
#define SNTP_TASK_PRIO 28
#define SNTP_TASK_STACK_SIZE 0x1000

static void sle_uuid_client_entry(void)
{
    // 创建消息队列
    MsgQueue_ID = osMessageQueueNew(MSG_QUEUE_NUMBER, sizeof(notify_data_t),
                                    NULL); // 消息队列中的消息个数，消息队列中的消息大小，属性
    if (MsgQueue_ID != NULL) {
        osal_printk("ID = %d, Create MsgQueue_ID is OK!\n", MsgQueue_ID);
    }
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)sle_init, 0, "sle_gatt_client", SLE_UUID_CLIENT_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SLE_UUID_CLIENT_TASK_PRIO);
        osal_kfree(task_handle);
    }

    task_handle = osal_kthread_create((osal_kthread_handler)queue_notification_deal_task, 0, "queue_deal",
                                      MSGQUEUE_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, MSGQUEUE_TASK_PRIO);
        osal_kfree(task_handle);
    }

    task_handle = osal_kthread_create((osal_kthread_handler)mqtt_task, 0, "mqtt", MQTT_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, MQTT_TASK_PRIO);
        osal_kfree(task_handle);
    }

    task_handle = osal_kthread_create((osal_kthread_handler)sntp_task, 0, "sntp", SNTP_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SNTP_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}
/* Run the app entry. */
app_run(sle_uuid_client_entry);

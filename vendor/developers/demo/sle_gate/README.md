# 星闪网关

## 案例提供者

[kidwjb](https://gitee.com/kidwjb)

## 案例设计

本案例旨在帮助开发者建立一个简易的星闪网关，实现多个server通过sle与client通信，并且上报自己的数据给client，client实时上报数据到mqtt云端服务器，mqtt服务器可以下发命令，client接收到后给相应server命令控制LED灯亮灭

### 硬件参考资料

- [HiHope ws63开发板](https://gitee.com/hihopeorg_group/near-link/blob/master/NearLink_Pi_IOT/%E6%98%9F%E9%97%AA%E6%B4%BE%E7%89%A9%E8%81%94%E7%BD%91%E5%BC%80%E5%8F%91%E5%A5%97%E4%BB%B6%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E%E4%B9%A6_V1.1.pdf)

- [HiHope ws63开发板原理图](https://gitee.com/hihopeorg_group/near-link/blob/master/NearLink_DK_WS63E/NearLink_DK_WS63E%E5%BC%80%E5%8F%91%E6%9D%BF%E5%8E%9F%E7%90%86%E5%9B%BE.pdf)

### 软件参考资料

- [HiHope ws63开发板驱动开发指南](../../../docs/board/WS63V100%20%E8%AE%BE%E5%A4%87%E9%A9%B1%E5%8A%A8%20%E5%BC%80%E5%8F%91%E6%8C%87%E5%8D%97_02.pdf)
- [HiHope ws63开发板IO复用关系表](../../../docs/board/IO复用关系.md)
- [WS63V100 lwIP 开发指南_02.pdf](https://gitee.com/HiSpark/fbb_ws63/blob/master/docs/board/WS63V100%20lwIP%20%E5%BC%80%E5%8F%91%E6%8C%87%E5%8D%97_02.pdf)
- [WS63V100 软件开发指南_03.pdf](https://gitee.com/HiSpark/fbb_ws63/blob/master/docs/board/WS63V100%20%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91%E6%8C%87%E5%8D%97_03.pdf)

### 参考头文件

- sle_device_discovery.h
- sle_connection_manager.h
- sle_ssap_client.h
- sle_ssap_server.h

## 实验平台

`HiHope_NearLink_DK_WS63_V03` 开发板

## 实验目的

本实验旨在通过HiHope_NearLink_DK_WS63_V03开发板，帮助开发者建立一个星闪网关

## 实验原理

### sle1v多通信

- 创建sle客户端与服务端
- sle客户端上报数据到mqtt
- sle客户端选择相应服务端通信

### API 讲解

1. **ssapc_register_callbacks**
   - **功能**：注册SSAP客户端回调函数。
   - **参数**：
     - ssapc_callbacks_t *func
   - **返回值**： ERRCODE_SUCC 成功 ，Other 失败

2. **ssaps_notify_indicate_by_uuid**
   - **功能**：通过uuid向对端发送通知或指示。
   - **参数**：
     - uint8_t server_id
     -  uint16_t conn_id
     - ssaps_ntf_ind_t *param
   - **返回值**： ERRCODE_SUCC 成功 ，Other 失败

3. **MQTTClient_subscribe**
   - **功能**：订阅mqtt主题。
   - **参数**：
     - MQTTClient handle, const char* topic
     - int qos
   - **返回值**：失败返回其他错误码

4. **MQTTClient_publishMessage**

   ​	功能：MQTT publish函数

   - **参数**：
     - MQTTClient handle, const char* topicName
     - MQTTClient_message* msg

     - MQTTClient_deliveryToken* dt
      - **返回值**：失败返回其他错误码

5. **ssapc_write_req**

   - **功能**：客户端发起写请求。
   - **参数**：
     - uint8_t client_id
     - uint16_t conn_id
     - ssapc_write_param_t *param
   - **返回值**： ERRCODE_SUCC 成功 ，Other 失败

---

### 流程图说明（以客户端为例）

- **步骤 1**：注册sle客户端回填函数，使能sle
- **步骤 2**：开启wifi功能，连接mqtt服务器
- **步骤 3**：使用队列处理从客户端接收的数据信息
- **步骤 4**：接收mqtt下发指令并通过写请求发送指令给对应服务端

## 实验步骤

### 例程代码

sle_client.c

```c
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

#define SLE_SEEK_INTERVAL_DEFAULT   100
#define SLE_SEEK_WINDOW_DEFAULT     100
#define SLE_MTU_SIZE_DEFAULT        300
#define UUID_16BIT_LEN 2
#define UUID_128BIT_LEN 16
#define MSG_QUEUE_NUMBER 10
#define SLE_CONNNECT_MAX 8

#define SERVER_IP_ADDR "0857268db2.st1.iot*************aweicloud.com" //接入地址 hostname
#define SERVER_IP_PORT 1883 //port 单片机一般使用MQTT，端口1883
#define  CLIENT_ID "sle_**********33107" //client_id

#define MQTT_CMDTOPIC_SUB "$oc/devices/sle_gate/sys/commands/set/#" // 平台下发命令  订阅主题

#define MQTT_DATATOPIC_PUB "$oc/devices/sle_gate/sys/properties/report"                 // 属性上报topic 发布主题
#define MQTT_CLIENT_RESPONSE "$oc/devices/sle_gate/sys/commands/response/request_id=%s" // 命令响应topic

#define DATA_SEVER_NAME "Switch"
#define DATA_ATTR_NAME "LED_state"
#define DATA_ADC_SERVER_NAME "Sensor"
#define DATA_ADC_ATTR_NAME "ADC_value"

#define MQTT_DATA_SEND "{\"services\": [\
    {\"service_id\": \"%s\", \"properties\": {\"%s\": %s}},\
    {\"service_id\": \"%s\", \"properties\": {\"%s\": %d}}\
]}"

#define KEEP_ALIVE_INTERVAL 120
#define DELAY_TIME_MS 200
#define IOT

#ifdef IOT
char *g_username = "sle_gate";
char *g_password = "4d50d2ec1a5079f75efb3a****************e2cfb0be1647a04312";
#endif

char g_send_buffer[512] = {0};  //发布数据缓冲区
char g_response_id[100] = {0}; //保存命令id缓冲区

char g_response_buf[] = 
    "{\"result_code\": 0,\"response_name\": \"LED\",\"paras\": {\"result\": \"success\"}}"; // 响应json

volatile MQTTClient_deliveryToken deliveredToken;

MQTTClient client;

uint8_t g_cmdFlag = 0;

extern int MQTTClient_init(void);

osMessageQueueId_t MsgQueue_ID; // 消息队列的ID

typedef struct {
    uint8_t data[100];
    uint8_t connect_id;
}notify_data_t;//自定义消息

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
static sle_addr_t g_sle_uart_remote_addr = { 0 };

uint8_t remote_addr_key[SLE_CONNNECT_MAX][2] = {0}; //创建键值对

//****************mqtt********************//
//处理连接丢失回调函数
void connect_lost(void* context, char* cause)
{
    unused(context);
    osal_printk("connection lost : %s\r\n",cause);
}
//-----------------------------------//

//接收消息处理//
void parse_after_equal(const char *input,char *output)
{
    const char *equalsign = strchr(input,'=');
    if(equalsign != NULL)
    {
        //计算等号后面的字符串长度
        strcpy(output,equalsign +1);
    }
}

//处理接收到消息的回调函数
int message_arrive(void* context, char* topicName, int topicLen, MQTTClient_message* message)
{
    unused(context);
    unused (topicLen);
    ssapc_write_param_t param = {0};
    param.handle = 2;
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    uint8_t data[50];
    uint8_t len = sizeof(data);
    osal_printk("[Message receive topic]: %s\r\n",topicName);
    osal_printk("[Message] : %s \r\n",(char *)message->payload);
    //LED 控制
    if(strstr((char *)message->payload,"true") != NULL)
    {
        osal_printk("\r\n broker message is TRUE !!! \r\n");
        sprintf_s((char *)data,sizeof(data),"LED State: true");
    }
    else
    {
        osal_printk("\r\n broker message is FAUSE !!! \r\n");
        sprintf_s((char *)data,sizeof(data),"LED State: false");
    }
    uint8_t i = 0;
    for(i = 0;i <= connect_num;i++)
    {
        if(remote_addr_key[i][0] == 0x03)
        {
            osal_printk("find KEY-> uuid : %d  con_id : %d",remote_addr_key[i][0],remote_addr_key[i][1]);
            param.data_len = len;
            param.data = data;
            ssapc_write_req(0, remote_addr_key[i][1], &param);
            break;
        }
    }
    // param.data_len = len;
    // param.data = data;
    // ssapc_write_req(0, 0, &param);
    //解析命令id
    parse_after_equal(topicName,g_response_id);
    g_cmdFlag = 1;
    memset((char *)message->payload,0,message->payloadlen);

    return 1;//表示消息已被处理
}
//----------------------------------------------//

//处理消息传递完成回调函数
void delivered(void* context, MQTTClient_deliveryToken dt)
{
    unused(context);
    osal_printk("Message with token value %d delivery confirmed\r\n",dt);

    deliveredToken = dt;
}

//------------------------------------------------//

//订阅主题
int mqtt_subscribe(const char *topic)
{
    osal_printk("subscribe start!\r\n");
    MQTTClient_subscribe(client,topic,1);
    return 0;
}
//--------------------------------------------//

//发布主题

int mqtt_publish(const char *topic,char *msg)
{
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int ret = 0;
    pubmsg.payload = msg;
    pubmsg.payloadlen = (int)strlen(msg);
    pubmsg.qos = 1;
    pubmsg.retained = 0;
    osal_printk("[payload]: %s,[topic]:%s\r\n",msg,topic);
    ret = MQTTClient_publishMessage(client,topic,&pubmsg,&token);
    if(ret != MQTTCLIENT_SUCCESS)
    {
        osal_printk("mqtt publish failed \r\n");
        return ret;
    }
    return ret;
}

//-------------------------------------------//

static errcode_t mqtt_connect(void)
{
    int ret;
    MQTTClient_connectOptions mqtt_connect_opt = MQTTClient_connectOptions_initializer;// MQTT 3.1.1 non-WebSockets,适用于嵌入式设备
    //初始化mqtt客户端
    MQTTClient_init();

    //创建mqtt客户端
    ret = MQTTClient_create(&client,SERVER_IP_ADDR,CLIENT_ID,MQTTCLIENT_PERSISTENCE_NONE,NULL);
    if(ret != MQTTCLIENT_SUCCESS)
    {
        osal_printk("Failed to create MQTT client,return code %d\r\n",ret);
        return ERRCODE_FAIL;
    }
    mqtt_connect_opt.keepAliveInterval = KEEP_ALIVE_INTERVAL;
    mqtt_connect_opt.cleansession = 1;

    #ifdef IOT
    mqtt_connect_opt.username = g_username;
    mqtt_connect_opt.password = g_password;
    #endif

    //创建回调函数
    MQTTClient_setCallbacks(client,NULL,connect_lost,message_arrive,delivered);

    //尝试连接
    if((ret = MQTTClient_connect(client,&mqtt_connect_opt)) != MQTTCLIENT_SUCCESS)
    {
        osal_printk("[MQTT_Task] : Failed to connect ,return code %d\r\n",ret);
        MQTTClient_destroy(&client);//连接失败时销毁客户端
        return ERRCODE_FAIL;
    }
    osal_printk("[MQTT_Task] : Connected to MQTT brocker\r\n");
    osDelay(DELAY_TIME_MS);

    //订阅MQTT主题
    mqtt_subscribe(MQTT_CMDTOPIC_SUB);
    while (1)
    {
        //相应平台命令部分
        osDelay(DELAY_TIME_MS);//需要延时，否则会发布失败
        if(g_cmdFlag)
        {
            sprintf(g_send_buffer,MQTT_CLIENT_RESPONSE,g_response_id);
            //设备响应命令
            mqtt_publish(g_send_buffer,g_response_buf);

            g_cmdFlag = 0;
            memset(g_response_id,0,sizeof(g_response_id) / sizeof(g_response_id[0]));
        }

        //属性上报部分
        osDelay(DELAY_TIME_MS);
        //发送LED状态
        memset(g_send_buffer,0,sizeof(g_send_buffer) / sizeof(g_send_buffer[0]));//要把buffer清零，不然上面buffer已有值会有些影响
        sprintf(g_send_buffer,MQTT_DATA_SEND,DATA_SEVER_NAME,DATA_ATTR_NAME,revert ? "true" : "false",
        DATA_ADC_SERVER_NAME,DATA_ADC_ATTR_NAME,voltage);
        mqtt_publish(MQTT_DATATOPIC_PUB,g_send_buffer);
        osDelay(DELAY_TIME_MS);
    }
    return ERRCODE_SUCC;
}

void mqtt_task(void)
{
    wifi_connect();
    osal_msleep(200);
    mqtt_connect();
}


//---------------mqtt---------------------//

//***************queue*******************//

void queue_notification_deal_task(void)
{
    osStatus_t status = 0;
    while(1)
    {
        status = osMessageQueueGet(MsgQueue_ID, &notify_data_get, 0, 0);
        if(status == osOK)
        {
           osal_printk("con_id:%d queue deal :\r\n %s\r\n",notify_data_get.connect_id,notify_data_get.data);
           if(notify_data_get.data[1] == 0x11)
            {
                revert = notify_data_get.data[2];
            }
            else if(notify_data_get.data[1] == 0x22)
            {
                voltage = notify_data_get.data[2];
            }
        }
        osal_msleep(500);//每500ms处理一次队列中的数据
    }
}

//---------------queue-------------------//

//**************notification*************//

void sle_notification_cbk(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data,
    errcode_t status)
{
    unused(client_id);
    unused(status);
    notify_data_put.connect_id = conn_id;
    // osal_printk("recv:data_len:%d\r\n",data->data_len);
    memcpy_s(notify_data_put.data,sizeof(notify_data_put.data),data->data,data->data_len);
    osMessageQueuePut(MsgQueue_ID,&notify_data_put,0,0);//将数据入队
}
//----------------notification--------------//

//***************seek******************//
void sle_scan_start(void)
{
    sle_seek_param_t sle_seek_param = { 0 };
    sle_seek_param.own_addr_type = 0; //本端地址类型
    sle_seek_param.filter_duplicates = 0;//重复过滤开关，0：关闭，1：开启 
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
    if(status == 0)
    {
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
    remote_addr_key[connect_num][0] = (uint8_t)seek_result_data->addr.addr[5];
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
void connect_state_changed_cbk(uint16_t conn_id, const sle_addr_t *addr,
    sle_acb_state_t conn_state, sle_pair_state_t pair_state, sle_disc_reason_t disc_reason)
{
    osal_printk("[ssap client] conn state changed conn_id:%d, addr:%02x***%02x%02x\n", conn_id, addr->addr[0],
        addr->addr[4], addr->addr[5]); /* 0 4 5: addr index */
    //osal_printk("[ssap client] conn state changed disc_reason:0x%x\n", disc_reason);
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        if (pair_state == SLE_PAIR_NONE) {
            sle_pair_remote_device(&g_remote_addr);
        }
        osal_printk("\r\n**** sle server addr:");
        uint8_t i = 0;
        for(i = 0;i < 6;i++)
        {
            osal_printk("%02x",g_sle_uart_remote_addr.addr[i]);
        }
        osal_printk("\r\n");
        g_conn_id[connect_num] = conn_id;
        remote_addr_key[connect_num][1] = conn_id;
        connect_num++;
    }
    else if(conn_state == SLE_ACB_STATE_DISCONNECTED)
    {
        connect_num--;
        sle_remove_paired_remote_device(addr);
        sle_scan_start();
        osal_printk("sle disconnected,disreason : %d\r\n",disc_reason);
    }
    sle_start_seek();
}
void pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    osal_printk("[ssap client] pair complete conn_id:%d, addr:%02x***%02x%02x\n", conn_id, addr->addr[0],
        addr->addr[4], addr->addr[5]); /* 0 4 5: addr index */
    if (status == 0) {
        ssap_exchange_info_t info = {0};
        info.mtu_size = SLE_MTU_SIZE_DEFAULT;
        info.version = 1;
        ssapc_exchange_info_req(1,g_conn_id[connect_num], &info);
    }
}
void sle_connection_cbk_config(void)
{
    sle_connection_callbacks.connect_state_changed_cb = connect_state_changed_cbk;//连接状态
    sle_connection_callbacks.pair_complete_cb = pair_complete_cbk;
}

//----------------------------connect-------------------------//

//*************************ssap****************************//
void exchange_info_cbk(uint8_t client_id, uint16_t conn_id, ssap_exchange_info_t *param,
    errcode_t status)
{
    osal_printk("[ssap client] pair complete client id:%d status:%d\n", client_id, status);
    osal_printk("[ssap client] exchange mtu, mtu size: %d, version: %d.\n",
        param->mtu_size, param->version);
    ssapc_find_structure_param_t find_param = {0};
    find_param.type = SSAP_FIND_TYPE_PRIMARY_SERVICE;
    find_param.start_hdl = 1;
    find_param.end_hdl = 0xFFFF;
    ssapc_find_structure(0, conn_id, &find_param);
}
void sle_sample_find_structure_cbk(uint8_t client_id, uint16_t conn_id,
    ssapc_find_service_result_t *service, errcode_t status)
{
    osal_printk("[ssap client] find structure cbk client: %d conn_id:%d status: %d \n",
        client_id, conn_id, status);
    osal_printk("[ssap client] find structure start_hdl:[0x%02x], end_hdl:[0x%02x], uuid len:%d\r\n",
        service->start_hdl, service->end_hdl, service->uuid.len);
    if (service->uuid.len == UUID_16BIT_LEN) {
        osal_printk("[ssap client] structure uuid:[0x%02x][0x%02x]\r\n",
            service->uuid.uuid[14], service->uuid.uuid[15]); /* 14 15: uuid index */
    } 
    else {
        for (uint8_t idx = 0; idx < UUID_128BIT_LEN; idx++) {
            osal_printk("[ssap client] structure uuid[%d]:[0x%02x]\r\n", idx, service->uuid.uuid[idx]);
        }
    }
    g_find_service_result.start_hdl = service->start_hdl;
    g_find_service_result.end_hdl = service->end_hdl;
    memcpy_s(&g_find_service_result.uuid, sizeof(sle_uuid_t), &service->uuid, sizeof(sle_uuid_t));
}
void sle_sample_find_property_cbk(uint8_t client_id, uint16_t conn_id,
    ssapc_find_property_result_t *property, errcode_t status)
{
    ssapc_find_property_result.handle = property->handle;
    osal_printk("[ssap client] find property cbk, client id: %d, conn id: %d, operate indication: %d, "
        "descriptors count: %d status:%d.\n", client_id, conn_id, property->operate_indication,
        property->descriptors_count, status);
    for (uint16_t idx = 0; idx < property->descriptors_count; idx++) {
        osal_printk("[ssap client] find property cbk, descriptors type [%d]: 0x%02x.\n",
            idx, property->descriptors_type[idx]);
    }
    if (property->uuid.len == UUID_16BIT_LEN) {
        osal_printk("[ssap client] find property cbk, uuid: %02x %02x.\n",
            property->uuid.uuid[14], property->uuid.uuid[15]); /* 14 15: uuid index */
    } else if (property->uuid.len == UUID_128BIT_LEN) {
        for (uint16_t idx = 0; idx < UUID_128BIT_LEN; idx++) {
            osal_printk("[ssap client] find property cbk, uuid [%d]: %02x.\n",
                idx, property->uuid.uuid[idx]);
        }
    }
}

void find_structure_cmp_cbk(uint8_t client_id, uint16_t conn_id,
    ssapc_find_structure_result_t *structure_result, errcode_t status)
{
    unused(conn_id);
    osal_printk("[ssap client] find structure cmp cbk client id:%d status:%d type:%d uuid len:%d \r\n",
        client_id, status, structure_result->type, structure_result->uuid.len);
    if (structure_result->uuid.len == UUID_16BIT_LEN) {
        osal_printk("[ssap client] find structure cmp cbk structure uuid:[0x%02x][0x%02x]\r\n",
            structure_result->uuid.uuid[14], structure_result->uuid.uuid[15]); /* 14 15: uuid index */
    } else {
        for (uint8_t idx = 0; idx < UUID_128BIT_LEN; idx++) {
            osal_printk("[ssap client] find structure cmp cbk structure uuid[%d]:[0x%02x]\r\n", idx,
                structure_result->uuid.uuid[idx]);
        }
    }
}
void sle_sample_write_cfm_cbk(uint8_t client_id, uint16_t conn_id, ssapc_write_result_t *write_result,
    errcode_t status)
{
    osal_printk("[ssap client] write cfm cbk, client id: %d status:%d.\n", client_id, status);
    ssapc_read_req(0, conn_id, write_result->handle, write_result->type);
}
void sle_sample_read_cfm_cbk(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *read_data,
    errcode_t status)
{
    osal_printk("[ssap client] read cfm cbk client id: %d conn id: %d status: %d\n",
        client_id, conn_id, status);
    osal_printk("[ssap client] read cfm cbk handle: %d, type: %d , len: %d\n",
        read_data->handle, read_data->type, read_data->data_len);
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
    //ssapc_callbacks.write_cfm_cb = sle_sample_write_cfm_cbk;
    //ssapc_callbacks.read_cfm_cb = sle_sample_read_cfm_cbk;
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

static void sle_uuid_client_entry(void)
{
    // 创建消息队列
    MsgQueue_ID = osMessageQueueNew(MSG_QUEUE_NUMBER, sizeof(notify_data_t),
                                    NULL); // 消息队列中的消息个数，消息队列中的消息大小，属性
    if (MsgQueue_ID != NULL) {
        printf("ID = %d, Create MsgQueue_ID is OK!\n", MsgQueue_ID);
    }
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle= osal_kthread_create((osal_kthread_handler)sle_init, 0, "sle_gatt_client",
        SLE_UUID_CLIENT_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SLE_UUID_CLIENT_TASK_PRIO);
        osal_kfree(task_handle);
    }

    task_handle= osal_kthread_create((osal_kthread_handler)queue_notification_deal_task, 0, "queue_deal",
        MSGQUEUE_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, MSGQUEUE_TASK_PRIO);
        osal_kfree(task_handle);
    }

    task_handle= osal_kthread_create((osal_kthread_handler)mqtt_task, 0, "queue_deal",
        MQTT_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, MQTT_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}
/* Run the app entry. */
app_run(sle_uuid_client_entry);

```


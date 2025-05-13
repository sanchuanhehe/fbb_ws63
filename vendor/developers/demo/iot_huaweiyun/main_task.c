/*
 * Copyright (c) 2024 Beijing HuaQingYuanJian Education Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * Copyright (c) kidwjb
 *
 * Description: mqtt demo. \n
 *              This file implements a mqtt demo. \n
 *
 * History: \n
 * 2025-04-08, Create file. \n
 */
#include "soc_osal.h"
#include "app_init.h"
#include "cmsis_os2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClientPersistence.h"
#include "MQTTClient.h"
#include "errcode.h"
#include "wifi/wifi_connect.h"
#include "BSP/bsp.h"
#include "adc.h"
#include "adc_porting.h"

osThreadId_t mqtt_init_task_id;

#define SERVER_IP_ADDR "*************.myhuaweicloud.com" // 接入地址 hostname
#define SERVER_IP_PORT 1883                              // port 单片机一般使用MQTT，端口1883
#define CLIENT_ID "my_LED_0_0_2025030516"                // client_id

#define MQTT_CMDTOPIC_SUB "$oc/devices/my_LED/sys/commands/set/#" // 平台下发命令  订阅主题

#define MQTT_DATATOPIC_PUB "$oc/devices/my_LED/sys/properties/report"                 // 属性上报topic 发布主题
#define MQTT_CLIENT_RESPONSE "$oc/devices/my_LED/sys/commands/response/request_id=%s" // 命令响应topic

#define DATA_SEVER_NAME "Switch"
#define DATA_ATTR_NAME "LED_state"
#define DATA_ADC_SERVER_NAME "Sensor"
#define DATA_ADC_ATTR_NAME "ADC_value"

#define MQTT_DATA_SEND \
    "{\"services\": [\
    {\"service_id\": \"%s\", \"properties\": {\"%s\": %s}},\
    {\"service_id\": \"%s\", \"properties\": {\"%s\": %d}}\
]}"

#define CONFIG_ADC_CHANNEL 5

#define KEEP_ALIVE_INTERVAL 120
#define DELAY_TIME_MS 200
#define IOT

#ifdef IOT
char *g_username = "my_LED";
char *g_password = "513e*************93d16314";
#endif

char g_send_buffer[512] = {0}; // 发布数据缓冲区
char g_response_id[100] = {0}; // 保存命令id缓冲区

char g_response_buf[] =
    "{\"result_code\": 0,\"response_name\": \"LED\",\"paras\": {\"result\": \"success\"}}"; // 响应json

uint8_t g_cmdFlag;
volatile MQTTClient_deliveryToken deliveredToken;

MQTTClient client;
extern int MQTTClient_init(void);

uint16_t voltage = 0;

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
    osal_printk("[Message receive topic]: %s\r\n", topicName);
    osal_printk("[Message] : %s \r\n", (char *)message->payload);
    // LED 控制
    if (strstr((char *)message->payload, "true") != NULL) {
        osal_printk("\r\n broker message is TRUE !!! \r\n");
        my_io_setval(LED_PIN, GPIO_LEVEL_HIGH);
    } else {
        my_io_setval(LED_PIN, GPIO_LEVEL_LOW);
        osal_printk("\r\n broker message is FAUSE !!! \r\n");
    }
    // 解析命令id
    parse_after_equal(topicName, g_response_id);
    g_cmdFlag = 1;
    memset((char *)message->payload, 0, message->payloadlen);

    return 1; // 表示消息已被处理
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
    MQTTClient_subscribe(client, topic, 1);
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

    while (1) {
        adc_port_read(5, &voltage); /* 5: prot 5*/
        // 相应平台命令部分
        osDelay(DELAY_TIME_MS); // 需要延时，否则会发布失败
        if (g_cmdFlag) {
            sprintf(g_send_buffer, MQTT_CLIENT_RESPONSE, g_response_id);
            // 设备响应命令
            mqtt_publish(g_send_buffer, g_response_buf);
            g_cmdFlag = 0;
            memset(g_response_id, 0, sizeof(g_response_id) / sizeof(g_response_id[0]));
        }

        // 属性上报部分
        osDelay(DELAY_TIME_MS);
        // 发送LED状态
        memset(g_send_buffer, 0,
               sizeof(g_send_buffer) / sizeof(g_send_buffer[0])); // 要把buffer清零，不然上面buffer已有值会有些影响
        sprintf(g_send_buffer, MQTT_DATA_SEND, DATA_SEVER_NAME, DATA_ATTR_NAME,
                my_io_readval(LED_PIN) ? "true" : "false", DATA_ADC_SERVER_NAME, DATA_ADC_ATTR_NAME, voltage);
        mqtt_publish(MQTT_DATATOPIC_PUB, g_send_buffer);
        osDelay(DELAY_TIME_MS);
    }
    return ERRCODE_SUCC;
}

void mqtt_init_task(void)
{
    uapi_adc_init(ADC_CLOCK_NONE);
    osDelay(DELAY_TIME_MS);
    my_gpio_init(LED_PIN);
    wifi_connect();
    osDelay(DELAY_TIME_MS);
    mqtt_connect();
}

static void network_wifi_mqtt_example(void)
{
    printf("Enter HUAWEI IOT example()!");

    osThreadAttr_t options;
    options.name = "mqtt_init_task";
    options.attr_bits = 0;
    options.cb_mem = NULL;
    options.cb_size = 0;
    options.stack_mem = NULL;
    options.stack_size = 0x2000;
    options.priority = osPriorityNormal;

    mqtt_init_task_id = osThreadNew((osThreadFunc_t)mqtt_init_task, NULL, &options);
    if (mqtt_init_task_id != NULL) {
        printf("ID = %d, Create mqtt_init_task_id is OK!", mqtt_init_task_id);
    }
}
/* Run the sample. */
app_run(network_wifi_mqtt_example);
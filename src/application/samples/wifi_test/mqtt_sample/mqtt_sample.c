/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Application function mqtts publish \n
 */
#ifdef CONFIG_SAMPLE_SUPPORT_MQTT
#include "lwip/netifapi.h"
#include "td_base.h"
#include "td_type.h"
#include "uart.h"
#include "MQTTClient.h"
#include "MQTTClientPersistence.h"

#define MQTT_QOS                         1
#define MQTT_SAMPLE_LOG                  "[MQTT_SAMPLE]"
#define MQTT_MS_TO_SECOND                1000
#define MQTT_CLEANSESSION                1
#define MQTT_CLIENT_ID_PUB               "ExampleClientPub"
#define MQTT_TCP_TIMEOUT_MS              10000L
#define MQTT_KEEPALIVEINTERVAL           20

/* 请按照注释,提示或者默认值格式填充以下8个参数：客户端证书，客户端私钥，根CA证书，mqtt服务端uri
 * mqtt topic，mqtt username，mqtt passwd，mqtt publish msg */
static const td_u8 g_mqtt_client_crt[] = "\
-----BEGIN CERTIFICATE-----\r\n\
**********************[certificate body]**********************\r\n\
-----END CERTIFICATE-----\r\n\
";
static const td_u8 g_mqtt_client_key[] = "\
-----BEGIN PRIVATE KEY-----\r\n\
**********************[private key body]**********************\r\n\
-----END PRIVATE KEY-----\r\n\
";
static const td_u8 g_mqtt_ca_crt[] = "\
-----BEGIN CERTIFICATE-----\r\n\
**********************[CA body]**********************\r\n\
-----END CERTIFICATE-----\r\n\
";
static const td_char g_mqtt_uri[] = ""; /* example:"ssl://192.168.80.50:8883" */
static const td_char g_mqtt_topic[] = "\'topic\'";
static const td_char g_mqtt_username[] = "admin";
static const td_char g_mqtt_password[] = "admin";
static const td_char g_mqtt_publish_msg[] = "\'hello,world!\'";

static cert_string g_mqtt_client_crt_store = {g_mqtt_client_crt, sizeof(g_mqtt_client_crt)};
static cert_string g_mqtt_ca_crt_store = {g_mqtt_ca_crt, sizeof(g_mqtt_ca_crt)};
static key_string g_mqtt_client_key_store = {g_mqtt_client_key, sizeof(g_mqtt_client_key)};

extern int MQTTClient_init(void);
extern void MQTTClient_cleanup(void);

static td_void mqtt_publish_config_ssl_conn(MQTTClient_SSLOptions *ssl_opts, MQTTClient_connectOptions *conn_opts)
{
    ssl_opts->los_keyStore = &g_mqtt_client_crt_store;
    ssl_opts->los_trustStore = &g_mqtt_ca_crt_store;
    ssl_opts->los_privateKey = &g_mqtt_client_key_store;
    ssl_opts->sslVersion = MQTT_SSL_VERSION_TLS_1_2;
    conn_opts->keepAliveInterval = MQTT_KEEPALIVEINTERVAL;
    conn_opts->cleansession = MQTT_CLEANSESSION;
    conn_opts->ssl = ssl_opts;
    conn_opts->username = g_mqtt_username;
    conn_opts->password = g_mqtt_password;
}

static td_void mqtt_publish_config_publish_msg(MQTTClient_message *pubmsg)
{
    pubmsg->payload = (void *)g_mqtt_publish_msg;
    pubmsg->payloadlen = (int)strlen(g_mqtt_publish_msg);
    pubmsg->qos = MQTT_QOS;
    pubmsg->retained = 0;
}

td_void mqtt_publish_client(td_void)
{
    td_s32 ret;
    MQTTClient client;
    MQTTClient_deliveryToken token;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

    PRINT("%s::Client Start...\r\n", MQTT_SAMPLE_LOG);
    ret = MQTTClient_init();
    if (ret != MQTTCLIENT_SUCCESS) {
        PRINT("%s::Client init(mutex) failed!\r\n", MQTT_SAMPLE_LOG);
        goto client_deinit;
    }

    ret = MQTTClient_create(&client, g_mqtt_uri, MQTT_CLIENT_ID_PUB, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (ret != MQTTCLIENT_SUCCESS) {
        PRINT("%s::Client create failed!\r\n", MQTT_SAMPLE_LOG);
        goto client_deinit;
    }

    mqtt_publish_config_ssl_conn(&ssl_opts, &conn_opts);
    ret = MQTTClient_connect(client, &conn_opts);
    if (ret != MQTTCLIENT_SUCCESS) {
        PRINT("%s::Client connect failed!\r\n", MQTT_SAMPLE_LOG);
        goto client_destroy;
    }

    mqtt_publish_config_publish_msg(&pubmsg);
    ret = MQTTClient_publishMessage(client, g_mqtt_topic, &pubmsg, &token);
    if (ret != MQTTCLIENT_SUCCESS) {
        PRINT("%s::Client publish failed!\r\n", MQTT_SAMPLE_LOG);
        goto client_disconnect;
    }

    PRINT("%s::Client will wait at most %d seconds for publication of %s on topic %s with client id %s\r\n",
        MQTT_SAMPLE_LOG, MQTT_TCP_TIMEOUT_MS / MQTT_MS_TO_SECOND, g_mqtt_publish_msg, g_mqtt_topic, MQTT_CLIENT_ID_PUB);
    ret = MQTTClient_waitForCompletion(client, token, MQTT_TCP_TIMEOUT_MS);
    if (ret == MQTTCLIENT_SUCCESS) {
        PRINT("%s::Client published ok,message with delivery token %d delivered!\r\n", MQTT_SAMPLE_LOG, token);
    }

client_disconnect:
    MQTTClient_disconnect(client, MQTT_TCP_TIMEOUT_MS);

client_destroy:
    MQTTClient_destroy(&client);

client_deinit:
    MQTTClient_cleanup();
    PRINT("%s::Client Stop!\r\n", MQTT_SAMPLE_LOG);
    return;
}
#endif
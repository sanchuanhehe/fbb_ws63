---
title: MQTT 开发指南
---

# 前言<a name="ZH-CN_TOPIC_0000001856160653"></a>

**概述<a name="section4537382116410"></a>**

本文档主要介绍基于MQTT功能开发实现示例。

MQTT基于开源组件paho.mqtt.c-1.3.12实现，详细说明请参考官方说明：[https://www.eclipse.org/paho/files/mqttdoc/MQTTClient/html/index.html](https://www.eclipse.org/paho/files/mqttdoc/MQTTClient/html/index.html)

# API接口描述<a name="ZH-CN_TOPIC_0000001809362004"></a>




## 结构体说明<a name="ZH-CN_TOPIC_0000001809521856"></a>

paho.mqtt.c详细的结构体说明请参考官方说明文档：[https://www.eclipse.org/paho/files/mqttdoc/MQTTClient/html/annotated.html](https://www.eclipse.org/paho/files/mqttdoc/MQTTClient/html/annotated.html)

## API列表<a name="ZH-CN_TOPIC_0000001809362000"></a>

paho.mqtt.c详细的API说明请参考官方说明文档：[https://www.eclipse.org/paho/files/mqttdoc/MQTTClient/html/globals\_func.html](https://www.eclipse.org/paho/files/mqttdoc/MQTTClient/html/globals_func.html)

## 配置说明<a name="ZH-CN_TOPIC_0000001809521852"></a>

paho.mqtt.c详细配置说明请参考官方说明文档：[https://www.eclipse.org/paho/files/mqttdoc/MQTTClient/html/globals\_defs.html](https://www.eclipse.org/paho/files/mqttdoc/MQTTClient/html/globals_defs.html)

# 开发指南<a name="ZH-CN_TOPIC_0000001856240657"></a>





## 开发流程<a name="ZH-CN_TOPIC_0000001856240661"></a>

使用paho.mqtt.c的应用程序通常使用类似的结构：

-   创建一个客户端对象。
-   设置选项以连接到MQTT服务器。
-   如果正在使用多线程（异步模式）操作，请设置回调函数（请参见官方说明“[https://www.eclipse.org/paho/files/mqttdoc/MQTTClient/html/async.html](https://www.eclipse.org/paho/files/mqttdoc/MQTTClient/html/async.html)”）。
-   订阅客户需要接收的任何主题。
-   重复直到完成：
    -   发布客户端需要的所有消息。
    -   处理任何传入的消息。

-   断开客户端。
-   释放客户端正在使用的所有内存。

具体实现可以参考官方说明中的示例：

-   Synchronous publication example：[https://www.eclipse.org/paho/files/mqttdoc/MQTTClient/html/pubsync.html](https://www.eclipse.org/paho/files/mqttdoc/MQTTClient/html/pubsync.html)

-   Asynchronous publication example：[https://www.eclipse.org/paho/files/mqttdoc/MQTTClient/html/pubasync.html](https://www.eclipse.org/paho/files/mqttdoc/MQTTClient/html/pubasync.html)

-   Asynchronous subscription example：[https://www.eclipse.org/paho/files/mqttdoc/MQTTClient/html/subasync.html](https://www.eclipse.org/paho/files/mqttdoc/MQTTClient/html/subasync.html)

## 源码下载说明<a name="ZH-CN_TOPIC_0000002173234886"></a>

在SDK编译框架中，paho.mqtt.c源码位于open\_source/mqtt/paho.mqtt.c目录。SDK默认不包含paho.mqtt.c源码，如果产品需要使用：

1.  首先从官方网站下载“paho.mqtt.c v1.3.12”

    cd open\_source/mqtt

    git clone -b v1.3.12 https://github.com/eclipse-paho/paho.mqtt.c.git

2.  合入“open\_source/mqtt/mqtt\_v1.3.12.patch”文件。

    cd paho.mqtt.c

    patch -p1 < ../mqtt\_v1.3.12.patch

3.  添加MQTT组件。

    修改build/config/target\_config/ws63/config.py脚本，在'ws63-liteos-app'集合内的'ram\_componect'列表中，增加'mqtt'元素即可。

## 订阅示例代码<a name="ZH-CN_TOPIC_0000001856160649"></a>

```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MQTTClient.h"
#include "MQTTClientPersistence.h"
#include "osal_debug.h"
#include "MQTTClient.h"
#include "los_memory.h"
#include "los_task.h"

#define CLIENTID_SUB    "ExampleClientSub"
#define QOS         1
#define TIMEOUT     10000L
#define KEEPALIVEINTERVAL 20
#define CLEANSESSION      1

volatile MQTTClient_deliveryToken deliveredtoken;
volatile char g_subEnd = 0;
extern int MQTTClient_init(void);
void delivered(void *context, MQTTClient_deliveryToken dt)
{
    (void)context;
    osal_printk("Message with token value %d delivery confirmed\r\n", dt);
    deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    int i;
    char *payloadptr = NULL;
    (void)context;
    (void)topicLen;
    osal_printk("Message arrived\r\n");
    osal_printk("     topic: %s\r\n", topicName);
    osal_printk("   message: ");

    payloadptr = message->payload;
    for (i = 0; i < message->payloadlen; i++) {
        osal_printk("%c", payloadptr[i]);
    }
    osal_printk("\r\n");

    if(memcmp(message->payload, "byebye", message->payloadlen) == 0) {
        g_subEnd = 1;
        osal_printk("g_subEnd = %d\r\n",g_subEnd);
    }
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void connlost(void *context, char *cause)
{
    (void)context;
    osal_printk("\nConnection lost\r\n");
    osal_printk("     cause: %s\r\n", cause);
}

int mqtt_002(char *addr, char *topic, char *user_name, char *password)
{
    osal_printk("start mqtt sync subscribe...\r\n");
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc = 0;

    MQTTClient_init();
    MQTTClient_create(&client, addr, CLIENTID_SUB, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = KEEPALIVEINTERVAL;
    conn_opts.cleansession = CLEANSESSION;
    if (user_name != NULL) {
        conn_opts.username = user_name;
        conn_opts.password = password;
    }

    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        osal_printk("Failed to connect, return code %d\r\n", rc);
        return rc;
    }
    g_subEnd = 0;
    osal_printk("Subscribing to topic %s\nfor client %s using QoS%d\r\n\n"
           "wait for msg \" byebye\"\r\n\n", topic, CLIENTID_SUB, QOS);
    MQTTClient_subscribe(client, topic, QOS);
    do {
        LOS_TaskDelay(10);
    } while((g_subEnd == 0));
    osal_printk("Subscribing End\r\n", rc);
    MQTTClient_unsubscribe(client, topic);
    MQTTClient_disconnect(client, TIMEOUT);
    MQTTClient_destroy(&client);
    return rc;
}
```

## 分发示例代码<a name="ZH-CN_TOPIC_0000001856240653"></a>

```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MQTTClient.h"
#include "MQTTClientPersistence.h"
#include "osal_debug.h"
#include "MQTTClient.h"
#include "los_memory.h"
#include "los_task.h"

#define CLIENTID_PUB    "ExampleClientPub"
#define QOS         1
#define TIMEOUT     10000L
#define KEEPALIVEINTERVAL 20
#define CLEANSESSION      1
extern int MQTTClient_init(void);
int mqtt_001(char *addr, char *topic, char *msg, char *user_name, char *password)
{
    osal_printk("start mqtt publish...\r\n");

    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc = 0;

    MQTTClient_init();
    MQTTClient_create(&client, addr, CLIENTID_PUB, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = KEEPALIVEINTERVAL;
    conn_opts.cleansession = CLEANSESSION;
    if (user_name != NULL) {
        conn_opts.username = user_name;
        conn_opts.password = password;
    }

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        osal_printk("Failed to connect, return code %d\r\n", rc);
        return -1;
    }

    pubmsg.payload = msg;
    pubmsg.payloadlen = (int)strlen(msg);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    osal_printk("Waiting for up to %d seconds for publication of %s\r\n"
            "on topic %s for client with ClientID: %s\r\n",
            (int)(TIMEOUT/1000), msg, topic, CLIENTID_PUB);
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    osal_printk("Message with delivery token %d delivered\r\n", token);
    MQTTClient_disconnect(client, TIMEOUT);
    MQTTClient_destroy(&client);
    return rc;
}
```

# 注意事项<a name="ZH-CN_TOPIC_0000001809521860"></a>


## 支持加密通路<a name="ZH-CN_TOPIC_0000001856160657"></a>

-   如果需要实现MQTT加密传输，MQTT配置项中需要设置SSL参数。只做单端认证（客户端对服务端进行认证）时，需要提供认证服务端的根CA证书；做双端认证（客户端与服务端相互认证）时，除根CA证书外，还需要提供客户端证书与私钥。加密分发可参考如下代码示例：

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >建议使用TLS版本为1.2，证书位数至少为2048位。

    ```
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    
    #include "MQTTClient.h"
    #include "MQTTClientPersistence.h"
    #include "osal_debug.h"
    #include "MQTTClient.h"
    #include "los_memory.h"
    #include "los_task.h"
    
    #define CLIENTID_PUB    "ExampleClientPub"
    #define QOS         1
    #define TIMEOUT     10000L
    #define KEEPALIVEINTERVAL 20
    #define CLEANSESSION      1
    
    /* 客户端证书，请自行填充 */
    unsigned char client_crt[] = "\
    -----BEGIN CERTIFICATE-----\r\n\
    ****************************************************************\r\n\
    ****************************************************************\r\n\
    -----END CERTIFICATE-----\r\n\
    ";
    /* 客户端私钥，请自行填充 */
    unsigned char client_key[] = "\
    -----BEGIN RSA PRIVATE KEY-----\r\n\
    ****************************************************************\r\n\
    ****************************************************************\r\n\
    -----END RSA PRIVATE KEY-----\r\n\
    ";
    /* 根CA证书，请自行填充 */
    unsigned char ca_crt[] = "\
    -----BEGIN CERTIFICATE-----\r\n\
    ****************************************************************\r\n\
    ****************************************************************\r\n\
    -----END CERTIFICATE-----\r\n\
    ";
    extern int MQTTClient_init(void);
    int mqtt_005(char *addr, char *topic, char *msg, char *user_name, char *password)
    {
        osal_printk("start mqtt ssl publish...\r\n");
        MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;
        MQTTClient client;
        MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
        MQTTClient_message pubmsg = MQTTClient_message_initializer;
        MQTTClient_deliveryToken token;
        int rc = 0;
    
        MQTTClient_init();
        cert_string keyStore = {client_crt, sizeof(client_crt)};
        cert_string trustStore = {ca_crt, sizeof(ca_crt)};
        key_string privateKey = {client_key, sizeof(client_key)};
        ssl_opts.los_keyStore = &keyStore;
        ssl_opts.los_trustStore = &trustStore;
        ssl_opts.los_privateKey = &privateKey;
        ssl_opts.sslVersion = MQTT_SSL_VERSION_TLS_1_2;
    
        MQTTClient_create(&client, addr, CLIENTID_PUB, MQTTCLIENT_PERSISTENCE_NONE, NULL);
        conn_opts.keepAliveInterval = KEEPALIVEINTERVAL;
        conn_opts.cleansession = CLEANSESSION;
        conn_opts.ssl = &ssl_opts;
        if (user_name != NULL) {
            conn_opts.username = user_name;
            conn_opts.password = password;
        }
    
        if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
            osal_printk("Failed to connect, return code %d\r\n", rc);
            return -1;
        }
        pubmsg.payload = msg;
        pubmsg.payloadlen = (int)strlen(msg);
        pubmsg.qos = QOS;
        pubmsg.retained = 0;
        MQTTClient_publishMessage(client, topic, &pubmsg, &token);
        osal_printk("Waiting for up to %d seconds for publication of %s\r\n"
                "on topic %s for client with ClientID: %s\r\n",
                (int)(TIMEOUT/1000), msg, topic, CLIENTID_PUB);
        rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
        osal_printk("Message with delivery token %d delivered\r\n", token);
        MQTTClient_disconnect(client, TIMEOUT);
        MQTTClient_destroy(&client);
        return rc;
    }
    ```


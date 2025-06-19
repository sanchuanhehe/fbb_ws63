# 前言<a name="ZH-CN_TOPIC_0000001856160653"></a>

**概述<a name="section4537382116410"></a>**

本文档主要介绍基于MQTT功能开发实现示例。

MQTT基于开源组件paho.mqtt.c-1.3.12实现，详细说明请参考官方说明：[https://www.eclipse.org/paho/files/mqttdoc/MQTTClient/html/index.html](https://www.eclipse.org/paho/files/mqttdoc/MQTTClient/html/index.html)

**产品版本<a name="section12266191774710"></a>**

与本文档相对应的产品版本如下。

<a name="table2270181717471"></a>
<table><thead align="left"><tr id="row15364171712479"><th class="cellrowborder" valign="top" width="31.759999999999998%" id="mcps1.1.3.1.1"><p id="p123646174478"><a name="p123646174478"></a><a name="p123646174478"></a><strong id="b192148202"><a name="b192148202"></a><a name="b192148202"></a>产品名称</strong></p>
</th>
<th class="cellrowborder" valign="top" width="68.24%" id="mcps1.1.3.1.2"><p id="p1936401717470"><a name="p1936401717470"></a><a name="p1936401717470"></a><strong id="b187248502"><a name="b187248502"></a><a name="b187248502"></a>产品版本</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row19364317104716"><td class="cellrowborder" valign="top" width="31.759999999999998%" headers="mcps1.1.3.1.1 "><p id="p7195145011178"><a name="p7195145011178"></a><a name="p7195145011178"></a>WS63</p>
</td>
<td class="cellrowborder" valign="top" width="68.24%" headers="mcps1.1.3.1.2 "><p id="p34453054"><a name="p34453054"></a><a name="p34453054"></a>V100</p>
</td>
</tr>
</tbody>
</table>

**读者对象<a name="section4378592816410"></a>**

本文档主要适用于以下工程师：

-   技术支持工程师
-   软件开发工程师

**符号约定<a name="section133020216410"></a>**

在本文中可能出现下列标志，它们所代表的含义如下。

<a name="table2622507016410"></a>
<table><thead align="left"><tr id="row1530720816410"><th class="cellrowborder" valign="top" width="20.580000000000002%" id="mcps1.1.3.1.1"><p id="p6450074116410"><a name="p6450074116410"></a><a name="p6450074116410"></a><strong id="b2136615816410"><a name="b2136615816410"></a><a name="b2136615816410"></a>符号</strong></p>
</th>
<th class="cellrowborder" valign="top" width="79.42%" id="mcps1.1.3.1.2"><p id="p5435366816410"><a name="p5435366816410"></a><a name="p5435366816410"></a><strong id="b5941558116410"><a name="b5941558116410"></a><a name="b5941558116410"></a>说明</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row1372280416410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p3734547016410"><a name="p3734547016410"></a><a name="p3734547016410"></a><a name="image2670064316410"></a><a name="image2670064316410"></a><span><img class="" id="image2670064316410" height="25.270000000000003" width="55.9265" src="figures/zh-cn_image_0000001809362008.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p1757432116410"><a name="p1757432116410"></a><a name="p1757432116410"></a>表示如不避免则将会导致死亡或严重伤害的具有高等级风险的危害。</p>
</td>
</tr>
<tr id="row466863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p1432579516410"><a name="p1432579516410"></a><a name="p1432579516410"></a><a name="image4895582316410"></a><a name="image4895582316410"></a><span><img class="" id="image4895582316410" height="25.270000000000003" width="55.9265" src="figures/zh-cn_image_0000001856160661.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p959197916410"><a name="p959197916410"></a><a name="p959197916410"></a>表示如不避免则可能导致死亡或严重伤害的具有中等级风险的危害。</p>
</td>
</tr>
<tr id="row123863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p1232579516410"><a name="p1232579516410"></a><a name="p1232579516410"></a><a name="image1235582316410"></a><a name="image1235582316410"></a><span><img class="" id="image1235582316410" height="25.270000000000003" width="55.9265" src="figures/zh-cn_image_0000001809362012.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p123197916410"><a name="p123197916410"></a><a name="p123197916410"></a>表示如不避免则可能导致轻微或中度伤害的具有低等级风险的危害。</p>
</td>
</tr>
<tr id="row5786682116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p2204984716410"><a name="p2204984716410"></a><a name="p2204984716410"></a><a name="image4504446716410"></a><a name="image4504446716410"></a><span><img class="" id="image4504446716410" height="25.270000000000003" width="55.9265" src="figures/zh-cn_image_0000001809521868.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p4388861916410"><a name="p4388861916410"></a><a name="p4388861916410"></a>用于传递设备或环境安全警示信息。如不避免则可能会导致设备损坏、数据丢失、设备性能降低或其它不可预知的结果。</p>
<p id="p1238861916410"><a name="p1238861916410"></a><a name="p1238861916410"></a>“须知”不涉及人身伤害。</p>
</td>
</tr>
<tr id="row2856923116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p5555360116410"><a name="p5555360116410"></a><a name="p5555360116410"></a><a name="image799324016410"></a><a name="image799324016410"></a><span><img class="" id="image799324016410" height="15.96" width="47.88" src="figures/zh-cn_image_0000001856240665.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p4612588116410"><a name="p4612588116410"></a><a name="p4612588116410"></a>对正文中重点信息的补充说明。</p>
<p id="p1232588116410"><a name="p1232588116410"></a><a name="p1232588116410"></a>“说明”不是安全警示信息，不涉及人身、设备及环境伤害信息。</p>
</td>
</tr>
</tbody>
</table>

**修改记录<a name="section2467512116410"></a>**

<a name="table1557726816410"></a>
<table><thead align="left"><tr id="row2942532716410"><th class="cellrowborder" valign="top" width="20.8%" id="mcps1.1.4.1.1"><p id="p3778275416410"><a name="p3778275416410"></a><a name="p3778275416410"></a><strong id="b5687322716410"><a name="b5687322716410"></a><a name="b5687322716410"></a>文档版本</strong></p>
</th>
<th class="cellrowborder" valign="top" width="26.040000000000003%" id="mcps1.1.4.1.2"><p id="p5627845516410"><a name="p5627845516410"></a><a name="p5627845516410"></a><strong id="b5800814916410"><a name="b5800814916410"></a><a name="b5800814916410"></a>发布日期</strong></p>
</th>
<th class="cellrowborder" valign="top" width="53.16%" id="mcps1.1.4.1.3"><p id="p2382284816410"><a name="p2382284816410"></a><a name="p2382284816410"></a><strong id="b3316380216410"><a name="b3316380216410"></a><a name="b3316380216410"></a>修改说明</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row18423161573717"><td class="cellrowborder" valign="top" width="20.8%" headers="mcps1.1.4.1.1 "><p id="p0424121523719"><a name="p0424121523719"></a><a name="p0424121523719"></a>04</p>
</td>
<td class="cellrowborder" valign="top" width="26.040000000000003%" headers="mcps1.1.4.1.2 "><p id="p11424171553719"><a name="p11424171553719"></a><a name="p11424171553719"></a>2025-02-28</p>
</td>
<td class="cellrowborder" valign="top" width="53.16%" headers="mcps1.1.4.1.3 "><p id="p1257615581576"><a name="p1257615581576"></a><a name="p1257615581576"></a>更新“<a href="源码下载说明.md">源码下载说明</a>”小节内容。</p>
</td>
</tr>
<tr id="row560713441118"><td class="cellrowborder" valign="top" width="20.8%" headers="mcps1.1.4.1.1 "><p id="p136071534151116"><a name="p136071534151116"></a><a name="p136071534151116"></a>03</p>
</td>
<td class="cellrowborder" valign="top" width="26.040000000000003%" headers="mcps1.1.4.1.2 "><p id="p116072034121119"><a name="p116072034121119"></a><a name="p116072034121119"></a>2024-10-12</p>
</td>
<td class="cellrowborder" valign="top" width="53.16%" headers="mcps1.1.4.1.3 "><a name="ul858283411219"></a><a name="ul858283411219"></a><ul id="ul858283411219"><li>更新“<a href="订阅示例代码.md">订阅示例代码</a>”小节内容。</li></ul>
</td>
</tr>
<tr id="row13603174220218"><td class="cellrowborder" valign="top" width="20.8%" headers="mcps1.1.4.1.1 "><p id="p3603124232114"><a name="p3603124232114"></a><a name="p3603124232114"></a>02</p>
</td>
<td class="cellrowborder" valign="top" width="26.040000000000003%" headers="mcps1.1.4.1.2 "><p id="p360311421214"><a name="p360311421214"></a><a name="p360311421214"></a>2024-06-27</p>
</td>
<td class="cellrowborder" valign="top" width="53.16%" headers="mcps1.1.4.1.3 "><a name="ul596614255229"></a><a name="ul596614255229"></a><ul id="ul596614255229"><li>更新“<a href="订阅示例代码.md">订阅示例代码</a>”小节内容。</li><li>更新“<a href="分发示例代码.md">分发示例代码</a>”小节内容。</li><li>更新“<a href="支持加密通路.md">支持加密通路</a>”小节内容。</li></ul>
</td>
</tr>
<tr id="row543416518117"><td class="cellrowborder" valign="top" width="20.8%" headers="mcps1.1.4.1.1 "><p id="p13477159577"><a name="p13477159577"></a><a name="p13477159577"></a>01</p>
</td>
<td class="cellrowborder" valign="top" width="26.040000000000003%" headers="mcps1.1.4.1.2 "><p id="p7347191514575"><a name="p7347191514575"></a><a name="p7347191514575"></a>2024-04-10</p>
</td>
<td class="cellrowborder" valign="top" width="53.16%" headers="mcps1.1.4.1.3 "><p id="p663512312573"><a name="p663512312573"></a><a name="p663512312573"></a>第一次正式版本发布。</p>
</td>
</tr>
<tr id="row5947359616410"><td class="cellrowborder" valign="top" width="20.8%" headers="mcps1.1.4.1.1 "><p id="p2149706016410"><a name="p2149706016410"></a><a name="p2149706016410"></a>00B01</p>
</td>
<td class="cellrowborder" valign="top" width="26.040000000000003%" headers="mcps1.1.4.1.2 "><p id="p648803616410"><a name="p648803616410"></a><a name="p648803616410"></a>2024-03-15</p>
</td>
<td class="cellrowborder" valign="top" width="53.16%" headers="mcps1.1.4.1.3 "><p id="p1946537916410"><a name="p1946537916410"></a><a name="p1946537916410"></a>第一次临时版本发布。</p>
</td>
</tr>
</tbody>
</table>

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


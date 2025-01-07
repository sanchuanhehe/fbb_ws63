# mqtt

## 1.1 介绍

**功能介绍：** 实现核心板连接路由器热点或者手机热点，案例中手机或者路由器热点需要设置为SSID：H， 密码：12345678。

**硬件概述：** 核心板。硬件搭建要求如图所示：

<img src="../../../../docs/pic/mqtt_demo/image-20241226114514873.png" alt="image-20241226114514873" style="zoom:50%;" />

## 1.2 约束与限制

### 1.2.1 支持应用运行的芯片和开发板

本示例支持开发板：HiHope_NearLink_DK3863E_V03

### 1.2.2 支持API版本、SDK版本

本示例支持版本号：1.10.102

### 1.2.3 支持IDE版本、支持配套工具版本

本示例支持IDE版本号：1.0.0.8；

## 1.3 效果预览

<img src="../../../../docs/pic/mqtt_demo/image-20241226114526118.png" alt="image-20241226114526118" style="zoom:50%;" />

![image-20241226114535230](../../../../docs/pic/mqtt_demo/image-20241226114535230.png)

![image-20241226115038567](../../../../docs/pic/mqtt_demo/image-20241226115038567.png)



## 1.4 接口介绍

### 1.4.1 wifi_sta_enable()


| **定义：**   | errcode_t wifi_sta_enable(void);                |
| ------------ | ----------------------------------------------- |
| **功能：**   | 开启SoftAP                                      |
| **参数：**   | config：SoftAp的配置                            |
| **返回值：** | ERROCODE_SUCC：成功    Other：失败              |
| **依赖：**   | include\middleware\services\wifi\wifi_hotspot.h |

### 1.4.2 netifapi_netif_set_addr()


| **定义：**   | err_t netifapi_netif_set_addr(struct netif *netif, const ip4_addr_t *ipaddr, const ip4_addr_t *netmask, const ip4_addr_t *gw); |
| ------------ | ------------------------------------------------------------------------------------------------------------------------------ |
| **功能：**   | 设置网络接口的IP地址                                                                                                           |
| **返回值：** | ERROCODE_SUCC：成功    Other：失败                                                                                             |
| **依赖：**   | open_source\lwip\lwip_v2.1.3\src\include\lwip\netifapi.h                                                                       |

### 1.4.3 netifapi_dhcps_start()


| **定义：**   | err_t netifapi_dhcps_start(struct netif *netif, char *start_ip, u16_t ip_num); |
| ------------ | ------------------------------------------------------------------------------ |
| **功能：**   | 启动dhcp服务                                                                   |
| **返回值：** | ERROCODE_SUCC：成功    Other：失败                                             |
| **依赖：**   | open_source\lwip\lwip_v2.1.3\src\include\lwip\netifapi.h                       |

### 1.4.4 wifi_sta_scan()


| **定义：**   | errcode_t wifi_sta_scan(void);                 |
| ------------ | ---------------------------------------------- |
| **功能：**   | 进行全信道基础扫描                             |
| **参数：**   | void类型                                       |
| **返回值：** | ERRCODE_SUCC：成功   Other：失败               |
| **依赖：**   | include\middleware\services\wifi\wifi_device.h |

### 1.4.5 wifi_sta_connect()


| **定义：**   | errcode_t wifi_sta_connect(const wifi_sta_config_stru *config); |
| ------------ | --------------------------------------------------------------- |
| **功能：**   | 进行连接网络                                                    |
| **参数：**   | config：连接的网络参数                                          |
| **返回值：** | ERRCODE_SUCC：成功   Other：失败                                |
| **依赖：**   | include\middleware\services\wifi\wifi_device.h                  |

### 1.4.6 wifi_sta_get_ap_info()


| **定义：**   | errcode_t wifi_sta_get_ap_info(wifi_linked_info_stru *result); |
| ------------ | -------------------------------------------------------------- |
| **功能：**   | 获取station连接的网络状态                                      |
| **参数：**   | result：连接状态                                               |
| **返回值：** | ERRCODE_SUCC：成功   Other：失败                               |
| **依赖：**   | include\middleware\services\radar\radar_service.h              |

## 1.5 具体实现

①调用wifi_sta_enable 、wifi_sta_scan、wifi_sta_connect等函数连接Wi-Fi。

②调用MQTTClient_init、 MQTTClient_create等函数创建一个客户端对象。

③调用MQTTClient_setCallbacks，设置回调函数，客户端进入多线程模式。任何必要的消息确认和状态通信在后台处理，无需任何干预。

④调用MQTTClient_connect连接MQTT服务器

⑤调用mqtt_subscribe，订阅客户需要接收的任何主题。

⑥调用mqtt_publish，发布客户端需要的所有信息，并处理任何信息，重复直到完成。

⑦调用MQTTClient_disconnect断开客户端。

⑧调用MQTTClient_destroy释放客户端正在使用的所有内存。

## 1.6 实验流程

* 步骤1：注册登录华为云账号:https://activity.huaweicloud.com/discount_area_v5/index.html?utm_source=baidu&utm_medium=brand&utm_campaign=10056&utm_content=&utm_term=&utm_adplace=AdPlace024711;

* 步骤2：在搜索栏搜索 ”IoTDA“;

  ![](../../../../docs/pic/mqtt_demo/image-20220920151723528-17351990031972.png)

* 步骤3：选择 ”免费试用“;

  <img src="../../../../docs/pic/mqtt_demo/image-20220920151902522-17351990065824.png" style="zoom: 67%;" />

* 步骤4：如果是第一次使用，请先实名注册（账户信息实名），然后在选择标准版“开通免费单元”(如果以前有开通基础版也可以继续使用)，在控制台选择 “北京四”，然后点击 “产品”；

  ![](../../../../docs/pic/mqtt_demo/image-20220920152227956-17351990116175.png)

* 步骤5：选择 “创建产品”，选择自定义类型，然后弹出对话框，根据提示完善信息，点击 “确定”，出现创建产品成功代表产品创建完成。

  ![](../../../../docs/pic/mqtt_demo/image-20220920152423918-17351990217796.png)

  ![](../../../../docs/pic/mqtt_demo/image-20220920152615871-17351990231857.png)

  ![](../../../../docs/pic/mqtt_demo/image-20220920152721949-17351990247748.png)

* 步骤6：创建成功后，可以在页面处看到产品信息，点击 “查看”,查看产品详细信息，点击 “自定义模型”创建用户自己的模型

  ![](G:/hi3861_hdu_iot_application-master/doc/pic/image-20220920153027665.png)

  ![](../../../../docs/pic/mqtt_demo/image-20220920153059880-17351990263799.png)

* 步骤7：创建自定义模型“添加服务”，根据提示完善信息，点击 “确定”，服务添加完成后可以看到如下界面。

  ![](../../../../docs/pic/mqtt_demo/image-20220920160111901.png)

  <img src="../../../../docs/pic/mqtt_demo/image-20220920160341719.png" style="zoom:50%;" />

* 步骤8：添加成功后，选择 “添加属性”，如：新增属性为属性名称：“environment”，数据类型：“jsonObject(JSON结构体)”，访问权限：“可读，可写”，长度：“255”，点击确定；然后选择 “添加命令”，如：命令名称：“temperature”，点击新增输入参数，参数名称：“temperature”，数据类型：“小数”，长度：“255”。

  <img src="../../../../docs/pic/mqtt_demo/image-20241226113735490.png" alt="image-20241226113735490" style="zoom:67%;" /><img src="../../../../docs/pic/mqtt_demo/image-20241226113813835.png" alt="image-20241226113813835" style="zoom:67%;" />

  

* 步骤9：模型定义完成后，选择左边栏框 “设备”，选择 “所有设备”，然后在点击右上角 “注册设备”，根据弹窗提示完善信息，点击 “确定”，完成设备注册。

  ![](../../../../docs/pic/mqtt_demo/image-20220920163023914.png)

  ![](../../../../docs/pic/mqtt_demo/image-20220920163336853.png)

* 步骤10：设备注册成功后，可以看到设备未激活（这是因为设备已经在云端注册，但是实物还没有连接云端），点击 “查看”，查看设备信息。

  ![](../../../../docs/pic/mqtt_demo/image-20220920163512778.png)

* 步骤11：https://iot-tool.obs-website.cn-north-4.myhuaweicloud.com/打开链接，将下图地方设备ID复制到DeviceId，密钥复制到DeviceSecret，点击Generate生Clientld，Username，Password。

  ![](../../../../docs/pic/mqtt_demo/image-20220924160451960.png)


- 步骤12：在xxx\src\application\samples\peripheral文件夹新建一个sample文件夹，在peripheral上右键选择“新建文件夹”，创建Sample文件夹，例如名称“mqtt“。

  ![image-70551992](../../../../docs/pic/mqtt_demo/image-20240801170551992.png)

- 步骤13：将xxx\vendor\HiHope_NearLink_DK_WS63E_V03\mqtt文件里面内容拷贝到**步骤一创建的Sample文件夹中”mqtt“**。

  ![image-20241226114907563](../../../../docs/pic/mqtt_demo/image-20241226114907563.png)

* 步骤14：在xxx\src\application\samples\peripheral\CMakeLists.txt文件中新增编译案例，具体如下图所示（如果不知道在哪个地方加的，可以在“set(SOURCES "${SOURCES}" PARENT_SCOPE)”上面一行添加）。

  ![image-20241226114822725](../../../../docs/pic/mqtt_demo/image-20241226114822725.png)

  ![image-20250107160907323](../../../../docs/pic/mqtt_demo/image-20250107160907323.png)

* 步骤15：在xxx\src\application\samples\peripheral\Kconfig文件中新增编译案例，具体如下图所示（如果不知道在哪个地方加，可以在最后一行添加）。

  ![image-20241226114849158](../../../../docs/pic/mqtt_demo/image-20241226114849158.png)


- 步骤16：点击如下图标，选择”**系统配置**“，具体选择路径“Application/Enable the Sample of peripheral”，在弹出框中选择“support MQTT Sample”，点击Save，关闭弹窗。

  <img src="../../../../docs/pic/mqtt_demo/image-20240801171406113.png" alt="image-20240801171406113" style="zoom: 67%;" /><img src="../../../../docs/pic/beep/image-20240205105234692-17119401758316.png" alt="image-20240205105234692" style="zoom: 50%;" /><img src="../../../../docs/pic/mqtt_demo/image-20241226114928801.png" alt="image-20241226114928801" style="zoom: 67%;" />

- 步骤17：点击“build”或者“rebuild”编译

  ![image-20240801112427220](../../../../docs/pic/mqtt_demo/image-20240801112427220.png)

- 步骤十七：编译完成如下图所示。

  ![image-20240801165456569](../../../../docs/pic/mqtt_demo/image-20240801165456569.png)

- 步骤十八：在HiSpark Studio工具中点击“工程配置”按钮，选择“程序加载”，传输方式选择“serial”，端口选择“comxxx”，com口在设备管理器中查看（如果找不到com口，请参考windows环境搭建）。

  ![image-20240801173929658](../../../../docs/pic/mqtt_demo/image-20240801173929658.png)

- 步骤十九：配置完成后，点击工具“程序加载”按钮烧录。

  ![image-20240801174117545](../../../../docs/pic/mqtt_demo/image-20240801174117545.png)

- 步骤二十：出现“Connecting, please reset device...”字样时，复位开发板，等待烧录结束。

  ![image-20240801174230202](../../../../docs/pic/mqtt_demo/image-20240801174230202.png)

* 步骤二十一：烧录完成后，屏幕上显示温湿度，云端显示数据。

  


<img src="../../../../docs/pic/mqtt_demo/image-20241226114526118.png" alt="image-20241226114526118" style="zoom:50%;" />

![image-20241226114535230](../../../../docs/pic/mqtt_demo/image-20241226114535230.png)

![image-20241226115038567](../../../../docs/pic/mqtt_demo/image-20241226115038567.png)

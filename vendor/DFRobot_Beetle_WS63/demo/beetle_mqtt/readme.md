# beetle_mqtt

## 一、介绍

本例程演示了如何在WS63开发板上实现MQTT客户端功能，通过本地回环测试验证MQTT消息的发布和订阅能力。开发板连接到MQTT服务器后，能够：

- 自动发布消息：周期性地向指定主题发布预设消息
- 实时订阅消息：监听同一主题并接收处理消息

## 二、实验流程

安装MQTT服务器EMQX和客户端MQTTX，注意开发板和PC需处于同一局域网内。

- 执行Clean命令清理资源
- 安装emqx服务器并开启服务：

![img](.\images\mqtt_img1.png)

- 打开KConfig，选择MQTT示例，并配置好服务器地址、WIFI账号密码、订阅主题，发布主题和发布消息：

![img](.\images\mqtt_img2.png)

- 若连接的MQTT服务器需要配置认证信息(如SIOT服务器)，请勾选"Enable MQTT Authentication"并配置账户密码：

![img](.\images\mqtt_img3.png)

- 编译、烧录后，打开MQTTX客户端，订阅配置好的pubTopc，观察发布的消息：

![img](.\images\mqtt_img4.png)

- MQTTX客户端这边发布主题为subTopic消息，观察开发板打印：

![img](.\images\mqtt_img5.png)

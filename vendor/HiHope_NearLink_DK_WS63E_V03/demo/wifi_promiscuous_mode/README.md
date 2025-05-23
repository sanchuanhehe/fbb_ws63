# Wi-Fi混杂模式

步骤一：如果需要使用wifi混杂模式，需要在build\config\target_config\ws63\config.py文件中新增组件“wifi_promisc”

![image-20250522155451732](../../doc/media/wifi_promiscuous_mode/image-20250522155451732.png)

步骤二：将案例放到对应目录，修改wifi里面的ssid和密码

![image-20250522155737196](../../doc/media/wifi_promiscuous_mode/image-20250522155737196.png)

步骤三：wifi混杂模式需要sta关联，所以需要连接路由器、手机热点

步骤四：效果展示：

![image-20250523142752453](../../doc/media/wifi_promiscuous_mode/image-20250523142752453.png)

